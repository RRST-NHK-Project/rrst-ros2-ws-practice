cmake_minimum_required(VERSION 3.8)
project(turtle_teleop)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_srvs REQUIRED)   # 問題5
find_package(turtlesim REQUIRED)  # 問題6

add_executable(ps4_turtle src/PS4_Turtle_Teleop.cpp)
ament_target_dependencies(ps4_turtle
  rclcpp sensor_msgs geometry_msgs
  std_srvs turtlesim)

# インストール設定
install(TARGETS
  ps4_turtle
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
