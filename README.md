# 朱の帳<br>

## 概要<br>
ジャンル：パズルアクション<br>
プラットフォーム:Windows<br>
テーマ：御朱印帳<br>
コンセプト:畳んで・開いて<br>
<br>
主人公の朱野とばり が神社を目指すゲームです。<br>
朱印の欠片を集めなくともゴールすることは可能ですが、朱印の欠片を集めることで<br>
１つの御朱印が完成します。全ての御朱印を集めましょう！<br>

## アピールポイント<br>
### ゲーム<br>
このゲームは、畳む・開くという概念のあるパズルアクションゲームです。<br>
そのままでは突破困難な道も、畳む・開くを用いることで切り拓くことができます。<br>
何度も畳むことを繰り返し、朱印を全て獲得しゴールした時の気持ちよさがたまらないゲームです。<br>
先々のステージを予測して畳むも良し、適当に畳みまくるも良し、プレイスタイルはあなた次第！<br>

### 個人
DirectX11で作った2番目の作品です。<br>
この時点では3次元的な回転(行列)を知らない、また折る畳む処理が思いつかず実現不可能と思われていました。<br>
しかし妥協はしたくなかったので、エディタ―を用意するなどして出来るだけ折る畳む処理に時間を割き、<br>
たとえプログラムが不格好になってもゲームとして出来るぐらいには仕上げました。<br>
また、企画・仕様担当と連携しフェードの画面を動かすなどプログラマーとして追加できそうな演出を追加したこと。<br>
上記二つを含む出来るだけゲームとして良いものになるようにするところです。<br>

## 制作における問題点・改善点<br>
### 作業割り振りが偏りすぎている<br>
ゲームの規模に対して仕様担当が圧倒的に不足していた。(さらに、ステージ作成を兼任している)<br>
そのため、全体的に仕様決定が後倒しとなり、大体のプログラマーが動けずプログラマーの作業期間が想定よりも半分ほど短くなっている。<br>
結果、大半のプログラム作業がメインプログラマーに集中する事態と化してしまった。<br>

### 作業割り振りがそもそも適していない<br>
作業割り振りは作業割り振り担当が決めていたが、<br>
作業担当が各個人の能力を把握していないため無茶な作業工程が多く含まれていた。<br>
また、一つの分野(仕様・UI・シーン処理など)を一人が全て担当するため、<br>
作業工程が偏り、またそれぞれの作業量で締め切りが決定するため衝突が起きていた。<br>
(例：あらゆる親クラス担当とギミック担当ではギミック担当の方が締め切りが早いが、親クラスがないため仕様が分からないなど)

## 問題点の改善<br>
### 作業割り振りが偏りすぎている<br>
プログラマー陣営全員が仕様担当と連携することで人手不足と仕様担当とプログラマー陣営間の情報伝達の手間を省いた。<br>
また、手の空いたプログラマー陣営は、エディター作成やステージ作成をすることで提出日までにステージ制作が間に合うようにしている。<br>

### 作業割り振りがそもそも適していない<br>
それぞれの陣営のメイン○○が期日に完了しない場合の回避策を用意していた。<br>
メインプログラマーとして考えた回避策は、事前に判明しているならやりたい・出来る作業を優先して割り振りなおすこと。<br>
また、どうしてもプログラムに自信がない場合はプログラマー視点での仕様・ステージ作成にまわっていただいた(最終手段)。<br>

## 制作<br>
形態：チーム制作<br>
チームメンバー数：10人
(プランナー数：4人, 
プログラマー数：4人, 
デザイナー数：2人)<br>
担当役職：サブリーダー兼メインプログラマー兼勉強会幹事<br>

### 理想の担当箇所<br>
自分：サブリーダー、メインプログラマー、勉強会幹事、根幹部分のプログラム、折る畳む処理、ステージエディター、結合・バグ取り、企画<br>
プログラマー1：XAudioをはじめとしたサウンド処理、サウンド素材収集、企画<br>
プログラマー2：シーン遷移、ゲーム以外のシーン、UI(エディタ―含む)、企画<br>
プログラマー3：ギミック全般、企画<br>
プランナー1：リーダー、プログラマー4、アニメーター(リギング)、企画<br>
プランナー2：メインプランナー、仕様、ステージ作成、企画<br>
プランナー3：画像素材規格担当、企画<br>
プランナー4：日程管理、企画<br>
デザイナー1：メインデザイナー、企画<br>
デザイナー2：UIデザイナー、企画<br>

### 最終的な担当箇所<br>
自分：サブリーダー、メインプログラマー、勉強会幹事、プログラマー1~3の担当箇所以外全てのプログラム、ステージエディター、結合・バグ取り、画像素材規格担当補佐、仕様、企画<br>
プログラマー1：XAudioをはじめとしたサウンド処理、サウンド素材収集、ステージ作成、仕様、企画<br>
プログラマー2：各種素材編集、竜巻ギミック、シーン遷移雛型、ステージ作成、素材収集、テストプレイヤー、仕様、企画<br>
プログラマー3：ステージ作成、テストプレイヤー、仕様、企画<br>
プランナー1：リーダー、アニメーター(リギング)、画像編集、企画<br>
プランナー2：メインプランナー、テストプレイヤー、仕様、ステージ作成、書記、12月頃から画像素材規格担当、企画<br>
プランナー3：画像素材規格担当、テストプレイヤー、企画<br>
プランナー4：日程管理、企画<br>
デザイナー1：メインデザイナー、テストプレイヤー、企画<br>
デザイナー2：UIデザイナー、テストプレイヤー、企画<br>

## 朱の帳の歴史<br>
|  時季  |  内容  |
| --- | --- |
|  9/16  |  チーム始動  |
|  9/21  |  プログラマースキルチェックアンケート(リーダー作)  |
|  9/27  |  プログラマースキルチェック問題出題開始  |
|  9/30  |  草案その1否決  |
|  10/3  |  Photoshop講座開始  |
|  10/6  |  プログラマー勉強会開始  |
|  10/11  |  デザイナー加入  |
|  10月中旬  |  草案その2消滅  |
|  10/27  |  草案その3決定  |
|  11/10  |  プランナーの企画担当箇所決定  |
|  11/19  |  企画決定  |
|  11/27  |  プログラマー作業振り分け開始  |
|  12/14  |  企画練りなおし&作業振り分けなおし(初めに戻る)  |
|  12/17  |  仕様書作成開始&折り畳みプログラム作成開始  |
|  12/27  |  ギミックプログラム作成開始  |
|  1/28  |  エディタ―最終更新  |
|  1/31  |  最終テストプレイ  |
|  1/31  |  マスター版作成終了&学内コンテスト提出  |
|  3/10  |  学内コンテスト銀賞受賞  |
|  10/28  |  ゲームクリエイター甲子園2022提出  |

## 朱の帳制作時の自分の歴史<br>
|  時季  |  内容  |
| --- | --- |
|  9/16  |  チーム始動  |
|  9/16  |  WindowやDirectXなど根幹部分作成開始  |
|  9/21  |  プログラマースキルチェック問題作成  |
|  10/4  |  ゲームプロトタイプ作成完了(草案その1)  |
|  10/6  |  プログラマー勉強会開始  |
|  10/11  |  デザイナー加入  |
|  10月中旬  |  折る畳む処理作成開始(仕様・草案無し)  |
|  10月中旬  |  各種エディタ―作成開始(仕様・草案無し)  |
|  10月下旬  |  企画担当と連携を開始する  |
|  11月全般  |  他プログラマーをサポートしつつアニメーション再生など仕様に左右されない部分の制作  |
|  11月下旬  |  画像素材規格担当と連携を開始する  |
|  11月下旬  |  画像サイズ確認用実行ファイル作成完了  |
|  12/17  |  ゲームプロト対応兼エディタ―アルファ版作成完了  |
|  12/17  |  折る畳む処理作成開始(仕様無し・企画あり)  |
|  12/17  |  連携中の企画担当さんが仕様担当になる  |
|  12月初旬  |  プレイヤーの移動速度等を操作するエディタ―作成  |
|  12/31  |  他プログラマーのサポート&提出期限切れ申請があった作業の回収  |
|  1/1  |  お正月のため全プログラマーを休日にする。  |
|  1/2  |  提出期限切れ申請のあった作業開始  |
|  1/10  |  プレイヤーアニメーション画像の修正  |
|  1/21  |  エディタ―の複数ギミック設置時のバグ修正  |
|  1/21  |  エディタ―のサンプルシーンでのプレイヤー当たり判定更新  |
|  1/28  |  エディタ―最終更新  |
|  1/28  |  プログラム関係の全ての作業を請け負う  |
|  1/28  |  アルファ版作成終了  |
|  1/30  |  全てのゲーム内素材受け取り&差し替え  |
|  1/30  |  ベータ版作成終了  |
|  1/30  |  チュートリアルイラスト表示  |
|  1/31  |  マスター版作成終了&学内コンテスト提出  |

## 勉強会<br>
週に1回の集会と勉強会用ディスコードグループで構成。<br>
集会では質問のあったプログラム技術について納得できるように話し合います。<br>
質問がない場合は私が決めたことについて話し合います(スキルチェックアンケートをもとに難易度決定)。<br>
ディスコードグループでは個人でのプログラム、チームでのプログラム双方への質問対応を行う。<br>

## 操作方法<br>
|  マウス&キー  |  XInputコントローラー  |  操作  |
| --- | --- | --- |
|  A, D  |  方向キー  |  移動  |
|  E  |  Y  |  裏面確認  |
|  SPACE  |  A  |  ジャンプ  |
|  ESC  |  BACK or START  |  メニュー  |
|  マウス移動  |  L, R  |  選択ページの移動  |
|  左クリック  |  B  |  ページをつかむ&放す  |
|  右クリック  |  X  |  1ページ開く  |
|  SHIFT&DELETE  |  無し  |  データ削除(タイトル)  |

### ゲーム内での確認<br>
ゲーム内メニュー→操作説明で確認可能<br>
XInputコントローラーが接続されている場合コントローラー操作が表示される。<br>
それ以外ではキーボード操作が表示される。<br>

