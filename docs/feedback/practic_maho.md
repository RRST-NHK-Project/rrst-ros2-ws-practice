# turtle_teleop 講習フィードバック（`practic_maho`）

対象：コミット `154e50d`（2026年11月4日時点）

## 全体

`static` を使った状態の保持や `std::fabs` の使い方など、大事な部品は書けています。優先して直したいのは「代入の場所」と「前回の値の更新」です。どちらも実機では動作に直結します。

## ファイル別の改善案

### `turtle_teleop/src/PS4_Turtle_Teleop_task2.cpp.backup`（L88-94）：else の中だけで代入している

- **良い点：** デッドゾーンの判定と std::fabs の使い方は正しくできています
- **改善：** else の中だけの代入だと、離しても angular.z = 2.00 のまま回り続けます。代入は if の外で毎回行います

現在のコード（先頭の数字は行番号）：

```cpp
88 if (std::fabs(RS_X) < DEADZONE_R){
89     RS_X = 0.0;  // twist_ は変わらない
90 }
91 else {
92     twist_.angular.z = RS_X*MAX_ANGULAR;
93 
94 }
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

### `turtle_teleop/src/PS4_Turtle_Teleop_task3.cpp`（L87-95）：倍率を片方にしか掛けていない

- **良い点：** 倍率を scale という変数にしたので、後から変更しやすい形です
- **改善：** 問題文は「前進・旋回の両方を2倍」です。代入は1か所に並べると掛け忘れに気づけます

現在のコード（先頭の数字は行番号）：

```cpp
87 twist_.angular.z = RS_X * MAX_ANGULAR;
89 double scale = 1.0;
91 if (R1){
92     scale = 5.0;
93 
94 }
95  twist_.linear.x = LS_Y * MAX_LINEAR*scale;
```

改善案：

```cpp
double scale = 1.0;
if (R1) {
    scale = 2.0;
}

// 倍率を決めてから、まとめて代入する
twist_.linear.x = LS_Y * MAX_LINEAR * scale;
twist_.angular.z = RS_X * MAX_ANGULAR * scale;
```

### `turtle_teleop/src/PS4_Turtle_Teleop_task4.cpp`（L25, L92-107）：前回の値の更新と重複した代入

- **良い点：** static で状態を持ち、立ち上がりを判定する形は正しくできています
- **改善：** 更新がないと、押している間 3.00 と 10.00 を行き来します。代入を1か所にすれば旋回の掛け忘れも防げます

現在のコード（先頭の数字は行番号）：

```cpp
25 #define MAX_LINEAR 10// [m/s]
92 if(CIRCLE && !last_circle){
93     slow_mode = !slow_mode;
95 }
   // last_circle の更新がない
98 twist_.angular.z = RS_X * MAX_ANGULAR;
101 if(slow_mode){
102     twist_.linear.x = LS_Y * MAX_LINEAR * 0.3;
104 }
105 else{
106      twist_.linear.x = LS_Y * MAX_LINEAR;
107 }
```

改善案：

```cpp
#define MAX_LINEAR 2.0  // [m/s]

if (CIRCLE && !last_circle) {
    slow_mode = !slow_mode;
}
last_circle = CIRCLE;  // if の外で毎回

double scale = 1.0;
if (slow_mode) {
    scale = 0.3;
}
twist_.linear.x = LS_Y * MAX_LINEAR * scale;
twist_.angular.z = RS_X * MAX_ANGULAR * scale;
```

## 書式（雛形との比較）

雛形から変わった行だけを機械的に調べた結果です。動作の良し悪しとは別の話で、見た目をそろえるための情報です。

### `turtle_teleop/src/PS4_Turtle_Teleop_task2.cpp.backup`：5件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 1 | L88 |
| `else` が `}` と別の行にある | 1 | L91 |
| 演算子の前後のスペース | 1 | L92 |
| 行末の空白 | 1 | L93 |
| 空行の連続 | 1 | L87 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task3.cpp`：8件

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 1 | L91 |
| 字下げが4スペース単位でない | 1 | L95 |
| 演算子の前後のスペース | 1 | L95 |
| `//` の直後にスペースがない | 1 | L78 |
| 行末の空白 | 3 | L86, L93, L96 |
| 空行の連続 | 1 | L97 |

### `turtle_teleop/src/PS4_Turtle_Teleop_task4.cpp`：25件（雛形から大きく離れているので優先して整えましょう）

| 項目 | 件数 | 行 |
| --- | --- | --- |
| `if` や `{` の前後のスペース | 3 | L92, L101, L105 |
| `else` が `}` と別の行にある | 1 | L105 |
| 字下げが4スペース単位でない | 1 | L106 |
| `//` の直後にスペースがない | 3 | L78, L79, L81 |
| 行末の空白 | 8 | L86, L87, L97, L99, L100, L108, L109, L112 |
| 空行の連続 | 9 | L87, L88, L97, L100, L109, L110, L111, L112, L113 |

手で直すより、次の設定を `.vscode/settings.json` に入れて、右クリック →「ドキュメントのフォーマット」を使うのが確実です。既定のままだと括弧が改行される形式になり、雛形と合いません。

```json
{
  "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 0 }",
  "files.trimTrailingWhitespace": true
}
```

## コード以外

- ブランチ名は `practic_maho` ではなく `practice/maho` の形が決まりです。
- `src/testfile` は不要なので消しましょう。`git add` の前に `git status` で確認すると混入を防げます。

## 次にやること

1. 問題1〜4を `PS4_Turtle_Teleop.cpp` にまとめる（`.backup` を戻す）
2. `MAX_LINEAR` を 2.0 に戻す
3. `CMakeLists.txt` の `add_executable` を雛形の指定に戻す

## 確認方法について

コードは抜粋で、`//` の注釈は講評用に追記したものがあります。改善案はこのファイルに当てはめ、`rclcpp` を検証用スタブに差し替えてコンパイル（`-Wall -Wextra -Wpedantic`）と入力テストをしました。turtlesim では未確認なので、自分の環境でも動かして確かめてください。
