cmake_minimum_required(VERSION 3.22)
project(sensor_process)

if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(Eigen3)
find_package(pcl_ros REQUIRED)
find_package(Pcap REQUIRED)

add_executable(lidar_process_node src/lidar_process.cpp)
ament_target_dependencies(lidar_process_node rclcpp sensor_msgs  geometry_msgs  PCL pcl_conversions pcl_ros tf2_ros tf2_eigen tf2_geometry_msgs Eigen3 Pcap)

install(TARGETS
  lidar_process_node
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY src include
  DESTINATION share/${PROJECT_NAME}
)



ament_package()
