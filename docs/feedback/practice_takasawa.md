# turtle_teleop 講習フィードバック（`practice/takasawa`）

対象：コミット `faed5be`（2026年11月4日時点）

## 全体

問題2は仕様をすべて満たしています。定数に名前を付ける工夫や、符号の対応を自分でまとめたメモ（`docs/コントローラーと亀の方位`）も良い取り組みです。次は仕様に合わせつつ、コードを短く保つ書き方に挑戦しましょう。

## ファイル別の改善案

### `turtle_teleop/src/PS4_Turtle_Teleop_task2.cpp`（L86-97）：デッドゾーンを先に処理すると短い

- **良い点：** 両スティックのデッドゾーンが正しく動きます。問題2の仕様を唯一すべて満たしています
- **改善：** 今の形だと倍率を足すたびに if の中を直します。先に入力を 0 にすれば、代入は2行で済みます

現在のコード（先頭の数字は行番号）：

```cpp
86 if (std::fabs(LS_Y) > DEADZONE_L) {
87     twist_.linear.x = LS_Y * MAX_LINEAR;
88 }
89 else {
90     twist_.linear.x = 0.0;
91 }
92 if (std::fabs(RS_X) > DEADZONE_R) {
93     twist_.angular.z = RS_X * MAX_ANGULAR;
94 }
95 else {
96     twist_.angular.z = 0.0;
97 }
```

改善案：

```cpp
if (std::fabs(LS_Y) < DEADZONE_L) {
    LS_Y = 0.0;
}
if (std::fabs(RS_X) < DEADZONE_R) {
    RS_X = 0.0;
}

twist_.linear.x = LS_Y * MAX_LINEAR;
twist_.angular.z = RS_X * MAX_ANGULAR;
```

### `turtle_teleop/src/PS4_Turtle_Teleop_task3and4.cpp`（L91-120）：組み合わせは掛け算にまとめる

- **良い点：** 全パターンを書き出して確かめ、定数に名前も付いています。丁寧な進め方です
- **改善：** 問題文は R1・0.3 倍です。if は倍率ごとに1つにすれば、モードが増えても増えません。空の else は不要です

現在のコード（先頭の数字は行番号）：

```cpp
91 bool mode_fast = 0;
96 if (CIRCLE && !last_circle){
97     mode_slow = !mode_slow;
98 }else{ //none }
102 if (L1==true){ mode_fast = 1; }else{ //none }
109 if (mode_slow==1 && mode_fast==0  ){
110     speed = default_speed*slow_rate;
112 else if (mode_slow==0 && mode_fast==1  ){
113     speed = default_speed*fast_rate;
115 else if (mode_slow==0 && mode_fast==0  ){
116     speed = default_speed;
118 else if (mode_slow==1 && mode_fast==1  ){
119     speed = default_speed*slow_rate*fast_rate;
```

改善案：

```cpp
const double fast_rate = 2.0;
const double slow_rate = 0.3;
if (CIRCLE && !last_circle) {
    slow_mode = !slow_mode;
}
last_circle = CIRCLE;
double scale = 1.0;
if (R1) {
    scale *= fast_rate;
}
if (slow_mode) {
    scale *= slow_rate;
}
```

### `turtle_teleop/src/PS4_Turtle_Teleop_task3and4.cpp`（L124-135, L164）：旋回の倍率と、別物の mode_slow

- **良い点：** static mode_slow をコールバック内に置いたので、切り替えは正しく動いています
- **改善：** 旋回にも倍率を掛けます。main() の mode_slow は名前が同じだけの別の変数で、未使用の警告が出ます

現在のコード（先頭の数字は行番号）：

```cpp
124 if (std::fabs(LS_Y) > DEADZONE_L) {
125     twist_.linear.x = LS_Y * MAX_LINEAR*speed;
126 }
   ...
130 if (std::fabs(RS_X) > DEADZONE_R) {
131     twist_.angular.z = RS_X * MAX_ANGULAR;
132 }
   ...
160 int main(int argc, char *argv[]) {
164      bool mode_slow = 0;  // 別の変数
```

改善案：

```cpp
if (std::fabs(LS_Y) < DEADZONE_L) {
    LS_Y = 0.0;
}
if (std::fabs(RS_X) < DEADZONE_R) {
    RS_X = 0.0;
}

twist_.linear.x = LS_Y * MAX_LINEAR * scale;
twist_.angular.z = RS_X * MAX_ANGULAR * scale;

// main() の mode_slow は削除
```

### `turtle_teleop/src/PS4_Turtle_Teleop_task5.cpp`（L86-93）：使うときに宣言する

- **良い点：** 問題5へ進もうとファイルを用意できています。発展に手を付けたのはこのブランチだけです
- **改善：** まだ使わない変数は書かず、必要になった時点で宣言します。}else; は else ごと消します。詳しくは問題5の解答例へ

現在のコード（先頭の数字は行番号）：

```cpp
86 double speed = 0.0;       // 未使用
87 bool fast_mode = 0;       // 未使用
88 bool slow_mode = 0;       // 未使用
89 //MODE CHANGE
91 if (L1==true){
92     fast_mode = 1;//
93 }else;

   // 問題5のサービス呼び出しはこれから
```

改善案：

```cpp
bool PS = msg->buttons[10];
static bool last_ps = false;

if (PS && !last_ps) {
    auto req = std::make_shared<
        std_srvs::srv::Empty::Request>();
    reset_client_->async_send_request(req);
}
last_ps = PS;
```

## 書式（雛形との比較）

雛形から変わった行だけを機械的に調べた結果です。動作の良し悪しとは別の話で、見た目をそろえるための情報です。

### `turtle_teleop/src/PS4_Turtle_Teleop_task2.cpp`：12件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `else` が `}` と別の行にある | 2 | L89, L95 |
| `//` の直後にスペースがない | 2 | L63, L66 |
| 行末の空白 | 4 | L86, L92, L99, L106 |
| 空行の連続 | 4 | L99, L100, L101, L102 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task3and4.cpp`：43件（雛形から大きく離れているので優先して整えましょう）

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 8 | L96, L98, L102, L104, L109, L112, L115, L118 |
| `else` が `}` と別の行にある | 5 | L112, L115, L118, L127, L133 |
| 字下げが4スペース単位でない | 1 | L164 |
| 演算子の前後のスペース | 9 | L102, L109, L110, L112, L113, L115, L118, L119, L125 |
| `//` の直後にスペースがない | 6 | L63, L66, L95, L99, L105, L108 |
| 行末の空白 | 8 | L90, L121, L123, L124, L130, L137, L144, L168 |
| 空行の連続 | 6 | L122, L123, L137, L138, L139, L140 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task5.cpp`：22件（雛形から大きく離れているので優先して整えましょう）

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 2 | L91, L93 |
| `else` が `}` と別の行にある | 2 | L103, L109 |
| 演算子の前後のスペース | 1 | L91 |
| `//` の直後にスペースがない | 4 | L63, L66, L89, L97 |
| 行末の空白 | 6 | L90, L98, L100, L106, L113, L120 |
| 空行の連続 | 7 | L95, L96, L99, L113, L114, L115, L116 |

手で直すより、次の設定を `.vscode/settings.json` に入れて、右クリック →「ドキュメントのフォーマット」を使うのが確実です。既定のままだと括弧が改行される形式になり、雛形と合いません。

```json
{
  "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 0 }",
  "files.trimTrailingWhitespace": true
}
```

## コード以外

- git の作者名が `41` になっています。`git config --global user.name "名前"` で設定しましょう。
- 1コミットに問題2〜4が入っています。1問 = 1コミットにすると戻しやすくなります。

## 次にやること

1. 問題1〜4を `PS4_Turtle_Teleop.cpp` にまとめる（R1・0.3 倍・旋回の倍率）
2. `CMakeLists.txt` の `add_executable` を雛形の指定に戻す
3. 問題5の `/reset` を実装する

## 確認方法について

コードは抜粋で、`//` の注釈は講評用に追記したものがあります。改善案はこのファイルに当てはめ、`rclcpp` を検証用スタブに差し替えてコンパイル（`-Wall -Wextra -Wpedantic`）と入力テストをしました。turtlesim では未確認なので、自分の環境でも動かして確かめてください。
