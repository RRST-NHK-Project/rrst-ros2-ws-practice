import time

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import Int16MultiArray


class PS4ServoTest(Node):

    def __init__(self):
        super().__init__('ps4_servo_test')

        # Arduinoへの指令送信
        self.publisher = self.create_publisher(
            Int16MultiArray,
            '/serial_tx_1',
            10
        )

        # PS4コントローラーの入力取得
        self.subscription = self.create_subscription(
            Joy,
            '/joy',
            self.joy_callback,
            10
        )

        self.angle = 90
        self.last_joy = None

        # 20ms周期（50Hz）
        self.timer = self.create_timer(
            0.02,
            self.send_command
        )

        self.get_logger().info(
            'PS4 Servo Test Start'
        )

    def joy_callback(self, msg):

        if len(msg.axes) < 1 or len(msg.buttons) < 4:
            return

        self.last_joy = time.monotonic()

        # 左スティックの左右
        stick = -msg.axes[0]

        # デッドゾーン
        if abs(stick) < 0.1:
            stick = 0.0

        # -1.0～1.0 を60～120度に変換
        self.angle = int(90 + stick * 30)

        # ×ボタン
        if msg.buttons[0]:
            self.angle = 60

        # ○ボタン
        elif msg.buttons[1]:
            self.angle = 120

        # △ボタン
        elif msg.buttons[3]:
            self.angle = 90

        # 角度制限
        self.angle = max(60, min(120, self.angle))

    def send_command(self):

        # コントローラー通信の途絶を検知
        if (
            self.last_joy is None
            or time.monotonic() - self.last_joy > 0.5
        ):
            self.angle = 90

        # Arduinoへの送信データ
        data = [0] * 24

        # 全サーボの初期角度
        for i in range(9, 15):
            data[i] = 90

        # SERVO1の角度
        data[9] = self.angle

        msg = Int16MultiArray()
        msg.data = data

        self.publisher.publish(msg)


def main():
    rclpy.init()
    node = PS4ServoTest()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()