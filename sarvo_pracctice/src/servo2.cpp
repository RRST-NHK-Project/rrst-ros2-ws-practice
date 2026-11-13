#include "/home/kobatake-ubuntu/ros2_ws/src/common/include/common/PacketController.hpp"
PacketController ctrlPkt;
 
// 送信：serial_tx_[ID] にPublishするだけ
pub_ = create_publisher<Int16MultiArray>("serial_tx_3", 10);
 
void timer_callback() {
  //ctrlPkt.setMD(MD1, 120);        // -255〜255 にクランプ
  ctrlPkt.setServo(SERVO2, 90);   // 0〜270
  //ctrlPkt.setTR(TR5, true);
  Int16MultiArray msg;  msg.data = ctrlPkt.toVector();
  pub_->publish(msg);
}
 
// 受信：serial_rx_[ID] をSubscribe
//void sensor_callback(const Int16MultiArray::SharedPtr m) {
//  int16_t enc1 = m->data[1];      // エンコーダ1
//  bool    sw1  = m->data[9];      // スイッチ1
//}