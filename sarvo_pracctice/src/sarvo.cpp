#include "rclcpp/rclcpp.hpp"//ros2用のおまじない
#include "std_msgs/msg/int16_multi_array.hpp" //16bit整数配列

#include <iostream>
#include <chrono>
#include <thread>

#define TX_DEVICE_ID 1 //マイコンのIDが1だから1 ID:2だと2
#define SLOT_NUM 24 //スロット数24
#define PUBLISH_RATE_HZ 40 //40Hzで送信 1sに40回

#define SLOT_SERVO1 9 //基盤では9番スロットをservo1に指定

int main(int argc,char **argv){
    rclcpp::init(argc,argv); //ros2の初期化

    auto node= rclcpp::Node::make_shared("serial_bride_example_servo_min");//ノードの作成
    auto pub = node -> create_publisher<std_msgs::msg::Int16MultiArray>(
        "serial_tx_"+std::to_string(TX_DEVICE_ID),10);

        std_msgs::msg::Int16MultiArray msg; //送信するメッセージの型を宣言
        msg.data.resize(SLOT_NUM,0);

        RCLCPP_INFO(node ->get_logger(),"seria_tx_%d started.",TX_DEVICE_ID);
        rclcpp::WallRate rate(PUBLISH_RATE_HZ);
        while(rclcpp::ok()){
            //================================================================
            //ここに制御を書く
            msg.data[SLOT_SERVO1] = 90;  //サーボ1に90を送信 90degへ
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            pub->publish(msg); //送信
            msg.data[SLOT_SERVO1] = 0;   //サーボ1に0を送信 0degへ
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            pub->publish(msg); //送信

            //================================================================

            //pub->publish(msg); //送信
            //rate.sleep();
        }

    rclcpp::shutdown(); //ros2の終了処理
    return 0;

}