cmake_minimum_required(VERSION 3.8)
project(eweek)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(serial REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)

include_directories(
  include
)

add_executable(app src/app.cpp include/app.h)
ament_target_dependencies(app rclcpp serial geometry_msgs std_msgs)


install(DIRECTORY 
  launch
  DESTINATION share/${PROJECT_NAME}/launch )

install(TARGETS
  app 
  DESTINATION lib/${PROJECT_NAME})

ament_package()
