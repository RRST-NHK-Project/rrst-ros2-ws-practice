/*
PS4コントローラーでturtlesimを操作するノード（解答例：問題1〜6）
雛形 turtle_teleop/src/PS4_Turtle_Teleop.cpp に docs/turtle_teleop講習.pptx の問題1〜6を実装したもの
Copyright (c) 2025 RRST-NHK-Project
SPDX-License-Identifier: MIT
*/

#include <chrono>
#include <cmath>
#include <iostream>

// ROS
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "std_srvs/srv/empty.hpp"     // 問題5：/reset サービス
#include "turtlesim/srv/set_pen.hpp"  // 問題6：/turtle1/set_pen サービス

#define PUBLISH_RATE_MS 20 // publish周期(ms)

// スティックのデッドゾーン
#define DEADZONE_L 0.1
#define DEADZONE_R 0.1

// 最大速度
#define MAX_LINEAR 2.0  // [m/s]
#define MAX_ANGULAR 2.0 // [rad/s]

class PS4TurtleTeleop : public rclcpp::Node {
public:
    PS4TurtleTeleop()
        : Node("ps4_turtle_teleop") {

        /*
        turtlesimに送信される速度指令"twist_"（geometry_msgs/msg/Twist）
        | メンバ | 詳細 | 単位 |
        | ---- | ---- | ---- |
        | twist_.linear.x | 前進(+)・後退(-) | m/s |
        | twist_.angular.z | 左旋回(+)・右旋回(-) | rad/s |
        ※ それ以外のメンバ(linear.y など)は turtlesim では使わない
        */

        // joyノードのSubscribe
        joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy", 10,
            std::bind(&PS4TurtleTeleop::ps4_listener_callback, this, std::placeholders::_1));

        // turtlesimへpublish
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "turtle1/cmd_vel", 10);

        // 問題5：亀を中央に戻して線を消すサービスのクライアント
        reset_client_ = this->create_client<std_srvs::srv::Empty>("reset");

        // 問題6：ペンの設定を変えるサービスのクライアント
        pen_client_ = this->create_client<turtlesim::srv::SetPen>("turtle1/set_pen");

        // timer_callbackを呼び出すタイマーを作成
        timer_ = create_wall_timer(
            std::chrono::milliseconds(PUBLISH_RATE_MS),
            std::bind(&PS4TurtleTeleop::publisher_timer_callback, this));

        RCLCPP_INFO(get_logger(), "ps4_turtle_teleop started.");
    }

private:
    void ps4_listener_callback(const sensor_msgs::msg::Joy::SharedPtr msg) {

        // コントローラーの入力を取得、使う入力のコメントアウトを外す
        // float LS_X = msg->axes[0];  // 左: +1
        float LS_Y = msg->axes[1];  // 上: +1
        float RS_X = msg->axes[3];  // 左: +1
        // float RS_Y = msg->axes[4];  // 上: +1

        // bool CROSS = msg->buttons[0];
        bool CIRCLE = msg->buttons[1];
        bool TRIANGLE = msg->buttons[2];
        // bool SQUARE = msg->buttons[3];

        // bool LEFT = msg->axes[6] == 1.0;
        // bool RIGHT = msg->axes[6] == -1.0;
        // bool UP = msg->axes[7] == 1.0;
        // bool DOWN = msg->axes[7] == -1.0;

        // bool L1 = msg->buttons[4];
        bool R1 = msg->buttons[5];

        // bool SHARE = msg->buttons[8];
        // bool OPTION = msg->buttons[9];
        bool PS = msg->buttons[10];

        // 前回の入力（関数を抜けても値が残る）
        static bool last_circle = false;
        static bool last_ps = false;
        static bool last_triangle = false;

        // 状態
        static bool slow_mode = false;
        static bool pen_off = false;

        // 以降、twist_を操作する

        // 問題2：デッドゾーン（速度を計算する前に）
        if (std::fabs(LS_Y) < DEADZONE_L) {
            LS_Y = 0.0;
        }
        if (std::fabs(RS_X) < DEADZONE_R) {
            RS_X = 0.0;
        }

        // 問題4：○を押した瞬間だけ低速モードを切り替え
        if (CIRCLE && !last_circle) {
            slow_mode = !slow_mode;
            RCLCPP_INFO(get_logger(), "slow_mode: %s", slow_mode ? "ON" : "OFF");
        }
        last_circle = CIRCLE;

        // 問題5：PSボタンを押した瞬間だけ /reset を呼ぶ
        if (PS && !last_ps) {
            auto req = std::make_shared<std_srvs::srv::Empty::Request>();
            reset_client_->async_send_request(req);
        }
        last_ps = PS;

        // 問題6：△を押した瞬間だけペンのON/OFFを切り替え
        if (TRIANGLE && !last_triangle) {
            pen_off = !pen_off;
            auto req = std::make_shared<turtlesim::srv::SetPen::Request>();
            req->r = 255;
            req->g = 255;
            req->b = 255;
            req->width = 3;
            req->off = pen_off;
            pen_client_->async_send_request(req);
            RCLCPP_INFO(get_logger(), "pen: %s", pen_off ? "OFF" : "ON");
        }
        last_triangle = TRIANGLE;

        // 問題3・4：倍率は最後にまとめて掛ける
        double scale = R1 ? 2.0 : 1.0;
        if (slow_mode) {
            scale *= 0.3;
        }

        // 問題1・2
        twist_.linear.x = LS_Y * MAX_LINEAR * scale;
        twist_.angular.z = RS_X * MAX_ANGULAR * scale;

        // デバッグ用
        // RCLCPP_INFO(get_logger(), "linear.x=%.2f, angular.z=%.2f",
        //             twist_.linear.x, twist_.angular.z);

        // twist_の操作ここまで
    }

    // publish
    void publisher_timer_callback() {
        publisher_->publish(twist_);
    }

    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Client<std_srvs::srv::Empty>::SharedPtr reset_client_;   // 問題5
    rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr pen_client_;   // 問題6
    rclcpp::TimerBase::SharedPtr timer_;

    geometry_msgs::msg::Twist twist_; // 0で初期化される
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);

    // figletでノード名を表示
    std::string figletout = "figlet PS4 Turtle Teleop";
    int result = std::system(figletout.c_str());
    if (result != 0) {
        std::cerr << "Please install 'figlet' with: sudo apt install figlet" << std::endl;
    }

    auto node = std::make_shared<PS4TurtleTeleop>();
    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}
