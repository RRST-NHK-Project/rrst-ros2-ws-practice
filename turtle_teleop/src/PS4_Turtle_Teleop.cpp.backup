/*
PS4コントローラーでturtlesimを操作するノード（講習用の雛形）
teleop_tools/src/Serial_Bridge_Host.cpp と同じ構成にしてある
docs/turtle_teleop講習.pptx の問題に合わせて書き換えていくこと
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
        // bool CIRCLE = msg->buttons[1];
        // bool TRIANGLE = msg->buttons[2];
        // bool SQUARE = msg->buttons[3];

        // bool LEFT = msg->axes[6] == 1.0;
        // bool RIGHT = msg->axes[6] == -1.0;
        // bool UP = msg->axes[7] == 1.0;
        // bool DOWN = msg->axes[7] == -1.0;

        // bool L1 = msg->buttons[4];
        // bool R1 = msg->buttons[5];

        // bool SHARE = msg->buttons[8];
        // bool OPTION = msg->buttons[9];
        // bool PS = msg->buttons[10];

        // 以降、twist_を操作する


        twist_.linear.x = LS_Y * MAX_LINEAR;
        twist_.angular.z = RS_X * MAX_ANGULAR;

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
