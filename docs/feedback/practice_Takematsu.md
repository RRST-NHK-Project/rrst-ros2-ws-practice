# turtle_teleop 講習フィードバック（`practice/Takematsu`）

対象：コミット `78799d7`（2026年11月6日時点）

## 全体

問題5まで完了です。PS ボタンを押した瞬間に1回だけ `/reset` を呼ぶ処理も、`std_srvs` の依存追加（`CMakeLists.txt`・`package.xml`）も正しくできています。問題1〜4も入力テストではすべて仕様どおりの値でした。ヒントを見ながらでも発展問題を最後まで書けたのは立派です。コメントに書いてくれた疑問には、下で答えます。

## コメントの疑問への回答

### L52「これもturtlesimにpublish?」

相手は turtlesim ですが、publish ではなく**サービスの呼び出し**です。

| | トピック（publish） | サービス（request） |
| --- | --- | --- |
| 送り方 | 流し続ける | 必要なときに1回頼む |
| 返事 | なし | あり |
| 今回の例 | `twist_` を 20ms ごとに送る | リセットを1回頼む |

L53 の `create_client` は、頼むための窓口を作るだけです。実際に送るのは PS ボタンを押したときです。

### L141「このrequest? でreset 用のrequestを送る」

その理解で合っています。`Empty::Request` は中身が空のリクエストで、`async_send_request` がそれを送ります。`async` は「結果を待たない」という意味です。

### L151「ここなにしてるかわからない たぶん変数の定義？」

これも合っています。クラスのメンバ変数の定義です。L53 で作ったクライアントをここに入れておくので、別の関数の `reset_turtlesim()` からも使えます。コンストラクタ内のローカル変数にすると、コンストラクタを抜けた時点で消えて使えません。上の `joy_sub_` や `publisher_` も同じ形です。

## ファイル別の改善案

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`（L53, L92-95, L141-145, L151）：問題5は完成。メモのコメントを説明に変える

- **良い点：** 立ち上がり検出を問題4と同じ形で書けています。リセットを `reset_turtlesim()` に分けたので、コールバックも読みやすくなっています
- **改善：** 疑問のメモは解決したら消し、処理の説明に書き換えます。L93 の空の `//` も消します。L142-143 は1行にまとめられます。`Last_PS` は `last_ps` にします

現在のコード（先頭の数字は行番号）：

```cpp
 52 //これもturtlesimにpublish?
 53 reset_client_ = this->create_client<std_srvs::srv::Empty>("reset");

 92 if (PS && !Last_PS) {  // PSボタンが押された瞬間リセット
 93     reset_turtlesim(); //
 94 }
 95 Last_PS = PS; // PSボタンの状態を保存

140 //request
141 void reset_turtlesim() {//ここも追加 このrequest? でreset 用のrequestを送る
142     auto req = std::make_shared
143         <std_srvs::srv::Empty::Request>();
144     reset_client_->async_send_request(req);
145 }

151 rclcpp::Client<std_srvs::srv::Empty>::SharedPtr reset_client_;//ここ追加した。（以下略）
```

改善案：

```cpp
// /reset サービスを呼ぶクライアントを作成
reset_client_ = this->create_client<std_srvs::srv::Empty>("reset");

if (PS && !last_ps) { // PSボタンが押された瞬間リセット
    reset_turtlesim();
}
last_ps = PS; // PSボタンの状態を保存

// /reset を呼ぶ（結果は待たない）
void reset_turtlesim() {
    auto req = std::make_shared<std_srvs::srv::Empty::Request>();
    reset_client_->async_send_request(req);
}

rclcpp::Client<std_srvs::srv::Empty>::SharedPtr reset_client_;
```

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`（L107-124）：倍率は入力を書き換えず `scale` にまとめる

- **良い点：** デッドゾーンを先に処理し、代入を if の外に書けています。`Last_CIRCLE` の更新も if の外にあり、立ち上がり検出は完成しています
- **改善：** LS_Y と RS_X に毎回同じ計算を書くと、片方だけ直すミスが起きやすくなります。倍率は `scale` にまとめ、代入のときに掛けます。問題4の確認で使う RCLCPP_INFO のログも足します

現在のコード（先頭の数字は行番号）：

```cpp
107 if(R1){   // R1ボタンが押されているときは速度をブースト
108     LS_Y *= 2.0;
109     RS_X *= 2.0;
110 }
112 if(CIRCLE && !Last_CIRCLE){  // 〇ボタンで低速モード切り替え
113     low_speed_mode = !low_speed_mode;
114 }
116 if(low_speed_mode){  // 低速モードのときは速度を半分にする
117     LS_Y *= 0.3;
118     RS_X *= 0.3;
119 }
120 Last_CIRCLE = CIRCLE;  // 〇ボタンの状態を保存
122 twist_.linear.x = LS_Y * MAX_LINEAR;    // 前進(up)・後退(down)
124 twist_.angular.z = RS_X * MAX_ANGULAR;  // 右旋回(left)・左旋回(right)
```

改善案：

```cpp
if (CIRCLE && !last_circle) {
    slow_mode = !slow_mode;
    RCLCPP_INFO(get_logger(), "slow_mode: %s", slow_mode ? "ON" : "OFF");
}
last_circle = CIRCLE;

double scale = R1 ? 2.0 : 1.0;
if (slow_mode) {
    scale *= 0.3;
}

twist_.linear.x = LS_Y * MAX_LINEAR * scale;   // 前進(+)・後退(-)
twist_.angular.z = RS_X * MAX_ANGULAR * scale; // 左旋回(+)・右旋回(-)
```

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`（L116, L124）：コメントを動作に合わせる

- **良い点：** L120 のコメントを「〇ボタンの状態を保存」と具体的に直せています
- **改善：** 次の2か所は、コードは正しいのにコメントが動作と違います。後で読む人が誤解するので直します

| 行 | 現在のコメント | 実際の動作 |
| --- | --- | --- |
| L116 | 速度を半分にする | 0.3 倍にする |
| L124 | 右旋回(left)・左旋回(right) | スティックを左に倒すと左旋回、右に倒すと右旋回 |

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`（L73-75, L90）：`static` 変数の場所と名前を整える

- **良い点：** `static` で前回の状態を残す形は、問題4・5のねらいどおりです
- **改善：** 入力の一覧の途中ではなく、一覧の後ろにまとめます。変数名は小文字にそろえ、`Last_CIRCLE` は `last_circle` にします。`low_speed_mode` は意味が伝わる良い名前なので、そのままでも構いません（改善案の `slow_mode` は解答例に合わせた名前です）

現在のコード（先頭の数字は行番号）：

```cpp
73 bool CIRCLE = msg->buttons[1];
74 static bool Last_CIRCLE = false;
75 static bool low_speed_mode = false;
76 // bool TRIANGLE = msg->buttons[2];

89 bool PS = msg->buttons[10];
90 static bool Last_PS = false;
```

改善案：

```cpp
bool CIRCLE = msg->buttons[1];
// bool TRIANGLE = msg->buttons[2];

bool PS = msg->buttons[10];

// （入力の一覧の後ろに）
// 前回の入力と状態（関数を抜けても値が残る）
static bool last_circle = false;
static bool last_ps = false;
static bool slow_mode = false;
```

## 書式（雛形との比較）

雛形から変わった行だけを機械的に調べた結果です。動作の良し悪しとは別の話で、見た目をそろえるための情報です。

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`：22件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 5 | L99, L103, L107, L112, L116 |
| 字下げが4スペース単位でない | 1 | L85 |
| `//` の直後にスペースがない | 6 | L52, L99, L103, L140, L141, L151 |
| 行末の空白 | 8 | L53, L99, L100, L103, L104, L139, L140, L144 |
| 空行の連続 | 2 | L59, L126 |

手で直すより、次の設定を `.vscode/settings.json` に入れて、右クリック →「ドキュメントのフォーマット」を使うのが確実です。既定のままだと括弧が改行される形式になり、雛形と合いません。

```json
{
  "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 0 }",
  "files.trimTrailingWhitespace": true
}
```

## コード以外

- ファイルを増やさず `PS4_Turtle_Teleop.cpp` に積み上げられています。この進め方を続けましょう。
- 問題5は1問1コミットになっていて、メッセージからも内容が分かります。この形を続けましょう。

## 次にやること

1. L52, L93, L140, L141, L151 のメモを説明のコメントに直す
2. 倍率を `scale` にまとめ、`RCLCPP_INFO` で ON/OFF を表示する
3. L116 と L124 のコメントを直す
4. 自動整形をかける
5. 問題6（ペンの ON/OFF）へ進む。問題5と同じくサービスの呼び出しです

## 確認方法について

コードは抜粋です。現在のコードと改善案を `rclcpp` の検証用スタブでコンパイル（`-Wall -Wextra -Wpedantic`）し、入力テストをしました。警告はなく、問題1〜4の動作と、PS ボタンを押し続けても `/reset` の呼び出しが1回だけになることを確かめました。スタブでは実際のリセットは起きず、`CMakeLists.txt` と `package.xml` は内容を読んだだけでビルドは試していません。turtlesim で亀が中央に戻るところは、自分の環境で確かめてください。
