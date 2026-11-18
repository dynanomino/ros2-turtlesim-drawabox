cmake_minimum_required(VERSION 3.8)
project(turtlesim_catchemall)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(my_robot_interfaces REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)

add_executable(turtle1_controller src/turtle1_controller.cpp)
ament_target_dependencies(turtle1_controller rclcpp geometry_msgs turtlesim my_robot_interfaces)

install(TARGETS
  turtle1_controller
  DESTINATION lib/${PROJECT_NAME}  
)

ament_package()
