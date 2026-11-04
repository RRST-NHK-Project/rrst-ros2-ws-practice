# turtle_teleop 講習フィードバック（`practice/masaki`）

対象：コミット `ac90cee`（2026年11月4日時点）

## 全体

代入を `if` の外に書く形と、`last_circle` の更新がしっかりできています。ロジックの組み立ては良いので、仕様の値と名前をそろえれば完成です。

## ファイル別の改善案

### `turtle_teleop/src/PS4_Turtle_Teleop_task2.cpp`（L64, L86-89）：形は良い。スペースと前進を整える

- **良い点：** 代入を if の外に書けています。止まらなくなるミスが起きない良い形です
- **改善：** 演算子の前後にスペースをそろえます。問題1の前進を消さず、左スティックにもデッドゾーンを入れます

現在のコード（先頭の数字は行番号）：

```cpp
64 //float LS_Y = msg->axes[1];
65 float RS_X = msg->axes[3];

86 if (std::fabs(RS_X)< DEADZONE_R){
87     RS_X = 0.0;
88 }
89 twist_.angular.z= RS_X *MAX_ANGULAR;
```

改善案：

```cpp
float LS_Y = msg->axes[1];
float RS_X = msg->axes[3];

if (std::fabs(LS_Y) < DEADZONE_L) {
    LS_Y = 0.0;
}
if (std::fabs(RS_X) < DEADZONE_R) {
    RS_X = 0.0;
}
twist_.linear.x = LS_Y * MAX_LINEAR;
twist_.angular.z = RS_X * MAX_ANGULAR;
```

### `turtle_teleop/src/PS4_Turtle_Teleop_task4.cpp`（L90-102）：名前と値を仕様に合わせる

- **良い点：** last_circle の更新が if の外に正しく書けていて、立ち上がり検出は完成しています
- **改善：** boost = 0 だと止まるので 0.3 に。減速に boost は逆の意味なので scale に。ログは RCLCPP_INFO を使います

現在のコード（先頭の数字は行番号）：

```cpp
90 double boost = 1.0;
91 if (CIRCLE && !last_circle){
92     slow_mode = !slow_mode;
93     //std::cout >> slow_mode>> std::endl;
94 }
95 if (slow_mode){
96         boost = 0;
97 }
99 last_circle = CIRCLE;
101 twist_.linear.x = LS_Y * MAX_LINEAR * boost;
102 twist_.angular.z = RS_X * MAX_ANGULAR * boost;
```

改善案：

```cpp
if (CIRCLE && !last_circle) {
    slow_mode = !slow_mode;
    RCLCPP_INFO(get_logger(), "slow_mode: %s",
                slow_mode ? "ON" : "OFF");
}
last_circle = CIRCLE;

double scale = 1.0;
if (slow_mode) {
    scale = 0.3;
}
twist_.linear.x = LS_Y * MAX_LINEAR * scale;
twist_.angular.z = RS_X * MAX_ANGULAR * scale;
```

## 書式（雛形との比較）

雛形から変わった行だけを機械的に調べた結果です。動作の良し悪しとは別の話で、見た目をそろえるための情報です。

### `turtle_teleop/src/PS4_Turtle_Teleop_task2.cpp`：4件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 1 | L86 |
| 演算子の前後のスペース | 1 | L89 |
| `//` の直後にスペースがない | 1 | L64 |
| 空行の連続 | 1 | L91 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task3.cpp`：2件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 1 | L87 |
| 空行の連続 | 1 | L94 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task4.cpp`：8件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 2 | L91, L95 |
| 字下げが4スペース単位でない | 1 | L96 |
| `//` の直後にスペースがない | 1 | L93 |
| 行末の空白 | 2 | L90, L98 |
| 空行の連続 | 2 | L28, L104 |

手で直すより、次の設定を `.vscode/settings.json` に入れて、右クリック →「ドキュメントのフォーマット」を使うのが確実です。既定のままだと括弧が改行される形式になり、雛形と合いません。

```json
{
  "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 0 }",
  "files.trimTrailingWhitespace": true
}
```

## コード以外

- 問題ごとのファイルと `.backup` は不要です。1つのファイルに積み上げ、問題ごとにコミットすれば `git log` から戻せます。

## 次にやること

1. 問題1〜4を `PS4_Turtle_Teleop.cpp` にまとめる（デッドゾーンと R1 を残す）
2. `CMakeLists.txt` の `add_executable` を雛形の指定に戻す
3. 余裕があれば問題5・6へ

## 確認方法について

コードは抜粋で、`//` の注釈は講評用に追記したものがあります。改善案はこのファイルに当てはめ、`rclcpp` を検証用スタブに差し替えてコンパイル（`-Wall -Wextra -Wpedantic`）と入力テストをしました。turtlesim では未確認なので、自分の環境でも動かして確かめてください。
