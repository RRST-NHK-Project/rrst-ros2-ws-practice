#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/int16_multi_array.hpp>

#define TX_ID 1

#define SLOT_NUM 24

#define PUBLISH_RATE_HZ 20

#define SLOT_servo1 9
#define SLOT_changing_interval 1

//#define SLOT_arg1 90 //0to90 in sg90

//static const int SLOT_arg1_list[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90,100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240};
//static const size_t SLOT_arg1_list_size = sizeof(SLOT_arg1_list) / sizeof(SLOT_arg1_list[0]);
int SLOT_arg1 = 0; //サーボ角度
int cnt = 0;       //インクリメント用変数
void Slot_update(){
  while (rclcpp::ok()){
    //int angle=SLOT_arg1_list[rand() % SLOT_arg1_list_size];
    std::this_thread::sleep_for(std::chrono::seconds(SLOT_changing_interval));
    SLOT_arg1 = (cnt % 19) * 10;
    cnt++;
    
  }
}

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("servo_practice_node");
    auto pub = node->create_publisher<std_msgs::msg::Int16MultiArray>("serial_tx_"+std::to_string(TX_ID), 10);
    // TX SLOT

    std_msgs::msg::Int16MultiArray msg;
    msg.data.resize(SLOT_NUM, 0);
    std::thread t (Slot_update);


    RCLCPP_INFO(node->get_logger(), "Publishing to serial_tx_%d", TX_ID);

    const rclcpp::Time start_time = node->now();

    rclcpp::WallRate rate(PUBLISH_RATE_HZ);

    while (rclcpp::ok()){
      
      msg.data[SLOT_servo1] = SLOT_arg1;
      pub->publish(msg);
      rate.sleep();
    
    }

    t.join();
    rclcpp::shutdown();
    return 0;
}

  



