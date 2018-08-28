= トランザクション

ここでは，トランザクションをより具体的にイメージしてもらうことを目標に説明します．

RDBMS における具体的なトランザクションは，データを読むか，レコードを書く操作を複数回行います．
データを読む場合，クエリ (一般には SQL) を実行して，その出力を得ます．
SQL などをサポートしないより基本的な DBMS では，
ある table において key を指定して絞り込んだ record data を読むという操作を行います．
(Join や group by などの高機能な演算はサポートされておらず，トランザクションの
ロジック内でそれらの機能を実装する必要があるかも知れないということです．)
Key で絞り込んだ record 集合に対して更新(update)，削除(delete) 操作も行えます．
挿入(insert) は，record data を生成し，table に対して操作します．
SQL の世界では，select, update, delete, insert ですが，
ここでは，read, update, delete, insert ということにしておきましょう．

Read, update, delete, insert の 4 つの操作ができるのが一般的ですが，
もっと低機能な DBMS は何を供えているべきでしょうか．
Read はさすがにできないと DBMS の意味がないでしょうね．
あとは，insert があればとりあえず，空のデータベースから始めてデータを読み書きはできそうです．
データが増える一方というのも困るでしょうから，次に delete が欲しくなるでしょうか．
update は delete と insert で代用できますので，単純に機能だけで見ると，
一番優先順位が低いといえます．
別の視点で見ると，初期データは別の手段で用意するとして，データは増えもせず減りもしないという
データベースを考えることができます．その場合は，read と update だけあれば事は足ります．

トランザクションの実行は，begin で始まり，先に説明した読み書き操作を複数回行った後，
commit または abort の操作で終わります．
トランザクションの開始状態が明らかであるときは begin が省略できるインターフェースもあります．
commit とは，トランザクションの正常終了を試みる操作です．成功した場合，
その結果がデータベースに反映されます．
abort は，トランザクションを意図的に失敗させて，なかったことにする操作です．
commit 操作は失敗した場合，abort と同じ結果となります．
commit 操作を実行しても，その返事が返ってくるまでは，commit 成功したかどうかは分かりません．
また，突然の電源断など(以後 crash と呼びます)が起きたとき，commit 操作の返事の有無によらず，
commit できたかどうかは，再起動時のデータベース recovery 操作が終わるまで分かりません．
(分散 DBMS においては故障の概念が違うのでその限りではありませんが，ここでは議論しません．)

理想的には，実行されたトランザクションが ACID の性質を満たすように DBMS は実装される必要があります．
実装によっては isolation level について設定で緩くできるものがありますが，
DBMS 側で性能面の制約が減る代わりに，
isolation が一部のケースで保証されない場合のケアをする責任がアプリケーション側に負わされます．
isolation については並行実行制御の節でも述べます．

どんな条件でレコードを特定し，どんな計算をし，どう読み書きするかについては
トランザクションの内容次第です．その内容はどう表現されているでしょうか？
一般には，アプリケーション内に実装されたトランザクションを実行するプログラムコード断片や，
DBMS 内に記録されたストアドプロシージャなどにその内容が記録されています．
処理の具体的な内容をトランザクションロジックと呼びます．

トランザクションは，one-shot トランザクションとそれ以外に分けることが出来ます．
one-shot トランザクションとは，トランザクションの開始前に外部から入力データを
与えて，トランザクションの完了後に外部に出力データを(少なくとも commit 成功か失敗かは) 返す
トランザクションのことで，トランザクションの実行中に，外部とのデータのやりとりを行わないものです．
one-shot トランザクションしか受けつけないシステムは，そうでないものに比べて，より単純です．

BerkeleyDB などの組み込み DBMS は，トランザクションロジックコードから
DBMS の機能をライブラリ経由で呼び出す仕組みになっています．
SQL をサポートして，ネットワーク経由のデータのやりとりをサポートして，，，とする仕組みよりは
単純なものです．これらは必ずしも one-shot トランザクションではないです．

大雑把に言うと，DBMS のアーキテクチャは大きく二種類あって，以下のどちらかです．

 1. ライブラリ形式で実装され，関数呼び出し経由で操作およびデータのやりとりをする
 2. サーバープロセス形式で実装されネットワーク経由で操作およびデータのやりとりをする

1. よりも 2. の方が面倒であることは想像がつきますでしょうか？
テストやデバッグ，ベンチマークのみを差し当っての目的にするのであれば，
ライブラリとして作るよりも，
DBMS とトランザクションロジックを一緒のソースコードとして開発してしまい，
一緒にコンパイル/ビルド/リンク等してしまうのがもっと単純かも知れません．
その場合，入出力の機能すら不要であれば省略してしまうことができます．
ハードコーディングするとか，乱数を用いて自動生成するとか．
