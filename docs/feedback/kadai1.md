# turtle_teleop 講習フィードバック（`kadai1`）

対象：コミット `bd79846`（2026年11月4日時点）

## 全体

問題1は正しく動いていて、旋回も先に書けています。余計な処理がなく読みやすいコードです。続きは問題2のデッドゾーンからです。

## ファイル別の改善案

### `turtle_teleop/src/PS4_Turtle_Teleop.cpp`（L63, L87-88）：使わない入力は戻し、次の問題へ

- **良い点：** 係数も符号も正しく、余計な処理のない読みやすいコードです
- **改善：** L63 の LS_X はコメントに戻します。次はデッドゾーン（問題2）を足しましょう

現在のコード（先頭の数字は行番号）：

```cpp
63 float LS_X = msg->axes[0];  // 未使用
64 float LS_Y = msg->axes[1];
65 float RS_X = msg->axes[3];

87 twist_.linear.x = LS_Y * MAX_LINEAR;
88 twist_.angular.z = RS_X * MAX_ANGULAR;
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

## 書式（雛形との比較）

雛形の書式から離れている箇所はありませんでした。この調子で書いていきましょう。

## コード以外

- ブランチ名は講習資料の決まりに合わせて `practice/自分の名前` にしましょう。
- コミットメッセージ `a` は、`問題1 前進・後退` のように内容が分かる形にしましょう。

## 次にやること

1. L63 の `LS_X` をコメントに戻す
2. 問題2のデッドゾーンを足す
3. 問題3（R1）、問題4（○）へ進む

## 確認方法について

コードは抜粋で、`//` の注釈は講評用に追記したものがあります。改善案はこのファイルに当てはめ、`rclcpp` を検証用スタブに差し替えてコンパイル（`-Wall -Wextra -Wpedantic`）と入力テストをしました。turtlesim では未確認なので、自分の環境でも動かして確かめてください。
