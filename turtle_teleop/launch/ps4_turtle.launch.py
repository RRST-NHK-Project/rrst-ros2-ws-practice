from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(package='joy', executable='joy_node', name='joy_node'),
        Node(package='turtlesim', executable='turtlesim_node', name='turtlesim'),
        Node(package='turtle_teleop', executable='ps4_turtle', name='ps4_turtle_teleop',
             output='screen'),
    ])
