= データの atomic な永続化方法

ファイルに対する `write()` システムコールで書いたデータの永続化は `fsync()` か `fdatasync()` で，
mmap されたファイルの変更データの永続化は `msync()` を使えば良いのでした．

基本はこれだけですが，もう少し高度な操作として，
atomic に書き込みを永続化するにはどうすれば良いかについて考えていきましょう．

== (1) atomic に追記したい場合

atomic に書きたいデータのファイル内のオフセットは分かっているものとします．
(それまでも同様に atomic に追記していれば，データのおしりのオフセットは分かるはずなので，
それが今回のデータの先頭オフセットとなります．)
データの checksum を計算します．アルゴリズムは crc32 など好きなものを選んでください．
もちろん特性は良く理解して選んでください．
checksum(通常固定サイズ)，データサイズ(固定サイズ)，データの中身(データサイズ)を書き，(必要なら)永続化します．
Crash recovery 時に，checksum とデータサイズとデータの中身相当のものをメモリに読み込みます．
checksum を再計算して，記録されているものと一致すれば，それは完全に書かれたものと信じて良く，
その書き込みはあったことにします．
checksum が不一致ならば，不完全なので，なかったことにします．
データサイズが固定長の場合は，データ毎にサイズを記録しなくても良いでしょう．
この方法は追記ファイルである WAL ファイルの atomic 書き込みに使えます．



== (2) atomic に上書きしたい場合

MySQL などでは double write という手段が使われています．
Double write buffer という専用のファイルを用意し，そこにまず書いて永続化してから，
本体ファイルを上書きし，最後に double write buffer から消します．
Double write buffer に書くときは atomic に追記するテクニックを使います．
Crash recovery 時に double write buffer に残っているデータは中途半端に書かれている可能性があるので，
上書き操作を redo することで atomic 性を担保します．

実質的には double write buffer は WAL における redo log と似た働きをすると思って良いでしょう．
WAL を使っているシステムの場合，WAL ファイルにログとして上書きしたいイメージを追記することで，
Crash recovery 時に上書き操作を redo することで atomic 性を担保できます．
毎回 WAL などに書くとログファイルが膨れあがってしまうので，
checkpoint 後に初めて上書きする場合などに留めるなどの最適化が行われます．
この方式は PostgreSQL で採用されています．

通常，ブロックストレージでは block の書き込みは atomic に書かれます．
ファイルシステムが何か特別なことをしていない限り，昨今の Linux は 4KiB sector HDD の存在により，
実体としての atomic write 単位が 512B だったり 4KiB だったりしますが，小さい方に合わせて，
512B alignment された領域に 512B の `write()` システムコールを 1 回のみ用いた書き込みであれば，
まず問題なく atomic に書かれると思います．
少なくとも以前の Linux の ファイルシステムのスーパーブロックの書き込みはこの性質を仮定していると思います．
ただ，これはシステム依存の挙動であることに十分注意してください．
あなたがシステム全てをコントロールできる立場にあり，それが atomic に書かれることが確信できるなら
ご自分の責任でそうしてください．


== (3) atomic に上書きしたい場合 (ファイルまるごと)

設定ファイルなどを更新するときに，ファイルまるごと atomic に変更したいときがあります．
これは write-once の挙動なので，プログラムが単純になり，うれしいです．
大きなファイルでも使えますが，変更点が少ない場合は無駄が多いです．
まず temporary な名前をつけて対象と同一ディレクトリ内にファイルを作成し，新しい中身を書き込み，永続化します．
(同じファイルシステムに属する場所でないと次の rename が失敗するので注意してください．)
次に，`rename()` システムコールを使って名前を対象のものに変えます(上書きします)．
`rename()` は atomic 動作が保証されており，`rename()` の前後で対象ファイルを open した人は，
以前のファイル内容か，新しいファイル内容のどちらかを必ず見ることになります．
古いファイルは，既に open している人がいなくなったら実際に削除されます．
以前は `rename()` によるファイルメタデータの変更を永続化するために
ファイルの存在するディレクトリエントリを永続化する必要がある，という話がありましたが，
最近のファイルシステムでは気にしなくて良いかも知れません．
参考(1): http://d.hatena.ne.jp/kazuhooku/20100202/1265106190
参考(2): https://thunk.org/tytso/blog/2009/03/15/dont-fear-the-fsync/


== (4) Copy-on-Write (CoW)

上書きするときにコピーする，という名前通りの手法です．
CoW というときは，メモリ断片とそれを指すポインタ(ポインタは atomic に書き換えられることが前提)の話と，
ディスク上で Tree 構造を扱うときの話があるように思います．今回は後者の話です．
Tree ノードの一部を上書きしたいとき(通常は leaf ノード)，新しいノードを確保し，
ノードの中身をまるごとコピーして，必要な変更を新しいノードに加えます．
新しい変更は atomic に実行する必要はありません．まだ root ノードから辿れない状態なので．
ノードの位置情報を参照しているノード上で書き換える必要がありますが，
その書き換えが atomic に出来るならそうして終わりです．
atomic に書き換えできないなら，やっぱり位置情報を書き換えたノードのコピーを作って，，，という操作を
再帰的に繰り返します．
いずれ一番上の root ノードに到達します．
root ノードが atomic に書けるならそれを atomic に書き換えて終わりです．
そうでないなら，root ノードの変更されたコピーを用意して，
新しい root ノードの位置情報を，
何らかの方法でその tree 全体を管理するデータ(root の位置情報が記録されている場所) を atomic に書き換えます．
CoW のメリットは，WAL が不要な点と，(root ノードまで CoW する場合に)自然に過去の snapshot が作れる点です．
(root ノードまで CoW する場合の)デメリットは，tree の深さと同じ数のノードの CoW を実行する必要がある点です．


== (5) その他

他にもあるかも知れません．興味があれば探求してみてください．