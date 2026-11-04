# turtle_teleop 講習フィードバック（`practice/masato`）

対象：コミット `26e8d8f`（2026年11月4日時点）

## 全体

`PS4_Turtle_Teleop_task4.cpp` の動作は問題4の仕様どおりです。`PS4_Turtle_Teleop.cpp` は作業途中の内容が残っているので、ここを正しい最新版にそろえましょう。

## ファイル別の改善案

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`（L87-105）：定数の取り違えと貼りっぱなし

- **良い点：** 三項演算子を自分のコードに取り入れられています
- **改善：** DEADZONE_L では最大 0.10 m/s です。倍率は計算したら使い、貼ったヒントの残りと L105 の混入は消します

現在のコード（先頭の数字は行番号）：

```cpp
87 twist_.linear.x = LS_Y * DEADZONE_L;
89 if (std::fabs(RS_X) < DEADZONE_R){
90     RS_X = 0.0;
91 }
93 twist_.angular.z = RS_X * MAX_ANGULAR;
96 double scale = R1 ? 2.0 : 1.0;  // 未使用
97-102 // ヒントのコメントが残っている
105 // RCLCPP_INFO(getMAX_ANGULAR_logger(), ...
```

改善案：

```cpp
if (std::fabs(LS_Y) < DEADZONE_L) {
    LS_Y = 0.0;
}
if (std::fabs(RS_X) < DEADZONE_R) {
    RS_X = 0.0;
}

double scale = R1 ? 2.0 : 1.0;
twist_.linear.x = LS_Y * MAX_LINEAR * scale;
twist_.angular.z = RS_X * MAX_ANGULAR * scale;
```

### `turtle_teleop/src/PS4_Turtle_Teleop_task4.cpp`（L91-103）：動作は正解。見た目と名前を整える

- **良い点：** 唯一、問題4の動作が仕様どおりです（0.3 倍を前進・旋回の両方に適用）
- **改善：** 閉じ括弧の位置と { の書き方をそろえます。減速なので名前は scale に。○の切り替えログも足すと完成です

現在のコード（先頭の数字は行番号）：

```cpp
91 if (CIRCLE && !last_circle) {
92     slow_mode = !slow_mode;
93     }
94 last_circle = CIRCLE;
96 double boost = 1.0;
98 if (slow_mode)
99 {
100     boost = 0.3;
101 }
102 twist_.linear.x = LS_Y *MAX_LINEAR * boost;
103 twist_.angular.z = RS_X * MAX_ANGULAR * boost;
```

改善案：

```cpp
if (CIRCLE && !last_circle) {
    slow_mode = !slow_mode;
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

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`：6件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 1 | L89 |
| `//` の直後にスペースがない | 3 | L98, L100, L102 |
| 行末の空白 | 2 | L92, L94 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task2.cpp`：2件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| 演算子の前後のスペース | 1 | L88 |
| 空行の連続 | 1 | L87 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task3.cpp`：9件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| 演算子の前後のスペース | 1 | L91 |
| 行末の空白 | 3 | L88, L94, L95 |
| 空行の連続 | 5 | L87, L88, L94, L95, L96 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task4.cpp`：6件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `{` が単独の行にある | 1 | L99 |
| 字下げが4スペース単位でない | 1 | L93 |
| 演算子の前後のスペース | 1 | L102 |
| 行末の空白 | 2 | L87, L97 |
| 空行の連続 | 1 | L87 |

手で直すより、次の設定を `.vscode/settings.json` に入れて、右クリック →「ドキュメントのフォーマット」を使うのが確実です。既定のままだと括弧が改行される形式になり、雛形と合いません。

```json
{
  "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 0 }",
  "files.trimTrailingWhitespace": true
}
```

## コード以外

- `PS4_Turtle_Teleop.cpp.`（末尾に `.`）と `PS4_Turtle_Teleop_task4 .cpp.backup`（スペース入り）は消しましょう。問題ごとのファイルも、1つにまとめれば不要です。

## 次にやること

1. 問題1〜4を `PS4_Turtle_Teleop.cpp` にまとめる（デッドゾーンと R1 を残す）
2. `CMakeLists.txt` の `add_executable` を雛形の指定に戻す
3. 余裕があれば問題5・6へ

## 確認方法について

コードは抜粋で、`//` の注釈は講評用に追記したものがあります。改善案はこのファイルに当てはめ、`rclcpp` を検証用スタブに差し替えてコンパイル（`-Wall -Wextra -Wpedantic`）と入力テストをしました。turtlesim では未確認なので、自分の環境でも動かして確かめてください。
