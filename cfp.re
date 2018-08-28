#@# https://github.com/kmuto/review/blob/master/doc/format.ja.md

= セキュリティキャンプ 2018 全国大会 募集要項

== データベースシステムとは

データベースは，データの集まりで，使う人が必要に応じて検索したり，データを追加，編集，削除したり，まとめて加工集計をしたりするためのものです．

独りで使うデータベースであれば，用途にもよりますが，Excel などの表計算ソフトでも足りることが多いと思います．複数人で，組織で，Web サービスで，ひとつのデータベースを管理したい場合は，もっと良い方法があります．それがデータベースシステムを使うことです．

データベースシステムといったとき，皆さんは具体的にどんなソフトウェアを思い浮べるでしょうか？オープンソースのデータベースシステムだけとっても，MySQL，PostgreSQL など，商用だと Oracle，DB2，SQL Server，Google Spannar など，たくさんあります．これらは SQL という操作インターフェースを供えており，トランザクション処理が可能です．また，大量のデータを加工，集計する処理も，規模や状況によりますが可能です．

もう少し広い範囲でも見てみることにします．

Hadoop や Spark などは，細かい単位でのデータ操作，とりわけトランザクション処理には対応していない代わりに，SQL でいうところの SELECT 文の実行，つまり，データを大量に加工，集計する用途に特化しています．また，ひとつのホストでは保持できない程の大量のデータを扱えるように，複数のホストをまたがって動作する分散システムです．
etcd などは，設定情報を分散システムで安全に共有する目的で使われることが多いデータベースシステムです．Key-value store という分類をされ，ソフトウェアによって違いはありますが，主に key を指定して，value を検索，操作する，
SQL と比べてより単純なインターフェースを持っています．
同様の key-value store として，memcached や Redis などは，データの永続化をある程度犠牲にしてでも，高速に処理したいデータを扱う目的で使われることが多いようです．

以上のように，目的，用途に応じて様々なデータベースシステムがあります．


== トランザクション処理

本ゼミは，上記で紹介した様々なデータベースシステムが持っている機能の中で，トランザクション処理を対象とします．トランザクション処理は，世の中になくてはならない処理のひとつです．皆さん(のご両親)が銀行 ATM でお金を入出金したり，送金したりする度に，裏でトランザクションが実行されます．ATM での操作が「きちんと」実行されないと困りますよね．また，皆さん(のご両親)がインターネット上の販売サイトで何かを購入するときに，決済(支払い)の画面で，決済ボタンを押す度に，裏でトランザクションが実行されます．これらの操作も「きちんと」処理してくれないと困りますよね．お金のやりとりにはほぼ必須ですが，それに限らず様々なやりとりでトランザクション処理は必要とされています．世の中には「きちんと」処理してくれないと困る処理が多いからです．「きちんと」がどういう意味を持つかについて理解するには，ACID の理解が必要となります．応募時の設問にもなってますので，分からない方，興味を持った方は自分で調べてみてください．

トランザクション処理の重要性についてつらつらと書きましたが，本ゼミに取り組む一番の理由は、講師である私が，トランザクション処理をおもしろい，と思っているからです．


== トランザクション処理を学ぶべき人

アルゴリズムとデータ構造を学ぶのがおもしろいと思う方は，トランザクション処理もおもしろいと感じる適性が間違いなくあると思います．何故ならトランザクション処理は，メモリ上，ディスク上で，様々なデータ構造を必要とし，その操作に伴うアルゴリズムを必要とするからです．
並列プログラミングが大好きな人も，トランザクション処理の性能を高めるために，それを駆使する必要があるので，楽しいと思います．


== データベースゼミの目的

本ゼミを通じて、データベースシステムの仕組みを理解し、作る側の人間になれる人を増やしたいと思っています。

セキュリティという観点では、SQL インジェクションはトピックのひとつかと思いますが、本ゼミでは SQL を扱いません。SQL はデータベースシステムを操作するインターフェースのひとつ(デファクトスタンダードではあります)でしかありません．

SQL についての情報はたくさんあります．本もたくさん出ています．それはデータベースシステムを使う人のための知識で，データベースシステムを使う人はたくさんいるからです．
また，アプリケーションを設計実装する人達は，一般に，単に良い性質を持つ一連のデータベース操作としてトランザクションを設計するだけで済むことが多いと思います．
それほどトランザクションという概念のもたらす抽象化は良く出来ていると思います．トランザクションは万能である一方，その皺寄せはトランザクション処理を実行するデータベースシステムの方に来ているのです．
データベースシステムを作れるであろう人は，私が思うに少ないです．もちろん，使う人ほど多くなくていいですが，作る人がもっといて欲しいなと思います．さらに，データベースの作り方を教える人も少ないと思います．というわけで，微力ではありますが多少の心得がある私が本ゼミを担当することになり，応募を検討されている皆さんに向けてこの文章を書いているというわけです．


== データベースゼミで学んでもらうこと

本ゼミでは，トランザクションを処理する仕組みについて開発を通じて学んでもらいます．

トランザクションを実行するのに必要な主な仕組みは，メモリ上およびディスク上のデータ構造に加えて，ログ先行書き込み (Write-ahead logging, WAL) と並行実行制御(Concurrency Control) です．

私がおもしろく，そして難しいと思うところは，トランザクションを並列に実行するための，様々な仕組みです．Concurrency Control がまずそれにあたります．Concurrency Control とひとくちに言っても様々な手法がありますが，S2PL という手法が基本中の基本だと思ってもらって良いと思います．ただ，Concurrency Control をデータベースシステム上で実現するにあたって，本当に複数の CPU コアを使って複数のスレッド/プロセスでトランザクションを並列に実行する場合，複数スレッド/プロセスからひとつのデータ構造にアクセスするので，並列プログラミングと言われている技術が必要になります．具体的には lock や latch などデータの適切な排他を行う仕組みが，場合によっては，lock-free だとか mutex-free と呼ばれているような技術もです．

皆さん次第ではありますが，本ゼミに与えられた時間でそこまで到達するのは簡単ではないと思っています．そこで，並列処理にチャレンジする前に到達して欲しいマイルストーンとして，逐次プログラミングによるデータベースシステムの開発を目指して欲しいと思います．逐次プログラミングに対象を絞ったときは，初めに，ディスク，つまり永続ストレージの特性と操作について，次にデータ構造(主にアクセスメソッドとしてのインデクス)，そして WAL について学んでもらいます．ここまで来ると，クラッシュリカバリができるようになります．
次なるステップとして，逐次プログラミングでもできる Concurrency Control について学んでもらおうと思っています．これにより，ディスク IO の間に，別のトランザクションを実行できるようになります．

ここまでくると，その先に，楽しい並列プログラミングの世界が待っています．


== 前提となる知識と経験

プログラミング言語として主に Python3 を使いますので，Python でプログラムを書いたことがない人には，ハードルが高いと思います．データベースシステムは，C/C++ などのよりメモリや CPU を直接扱いやすい言語で書いてあることが多いですし，最近ですと，golang や Java で書かれているものもあるようです．ただ、本ゼミでは，基本的な知識を学んでもらうことを目的としていますし，データベースシステムを作るために必要な知識を持っていないであろう方を対象としていますので，必ずしも高速なプログラムを作ってもらおうというわけではありません．そこで，私が思うに，おそらく多くの人が使ったことがあって，比較的簡単に扱えるであろう Python を選びました．他の言語が使いたいという方は，場合によっては認めますが，講師が用意した教材を改造するのではなく，イチから作ってもらうことになります．もちろん，それなりのプログラミング能力を持っている場合に限ります．

知識について，基本的なアルゴリズムとデータ構造について理解していれば，足りないということはありません．Tree 構造と hash 構造の特性の違いだとか，search と sort アルゴリズムだとか．分からなければ，そのときに学べば良いです．ただ，時間の制約から，そのあたりの知識が少ない方は，本ゼミの恩恵をあまり受けられないかも知れません．


== 応募を考えている人達へ

皆さんには，本ゼミで学んだことを生かして，いずれ，是非オリジナルの特徴を持ったデータベースシステムの開発にチャレンジしていただきたいですし，実用的なデータベースシステムを作る側の人になって活躍して欲しいです．もちろん，データベースシステムを使う側の人になったとしても，ここで得た知識は多いに役立つでしょう．どのような仕組みになっているかを知らないで使うのと，知っていて使うのでは，大きな差が出ます．データベースシステムの気持ちを知らない人は，データベースをうまく使うことが出来ません．また，研究の分野でもまだまだ課題はたくさんあり，それらを解決に向かわせる新しい手法を探求していって欲しいとも思います．

データベースシステムは，秘密にしなければならないデータを格納することも多いので，データの漏洩や改竄をされないように厳重に守らなければなりません．その第一歩は，バグを出来るだけ少なくするような設計実装であることはいうまでもありませんが，データベースシステムそのものだけでなく，周辺環境，使う人達のことも含めて，セキュリティを十分意識して頂きたいと思います．
