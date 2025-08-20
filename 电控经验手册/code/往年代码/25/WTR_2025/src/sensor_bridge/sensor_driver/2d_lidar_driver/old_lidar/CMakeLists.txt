cmake_minimum_required(VERSION 3.22)
project(old_lidar_driver)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_EXPORT_COMPLIE_COMMANDS ON)
set(SENSOR_BRIDGE_PACKAGE_NAME ${PROJECT_NAME})


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs)
find_package(tf2)
find_package(tf2_ros)

set(DEPENDS 
  rclcpp
  sensor_msgs
)

include_directories(
  sdk/include/
)

add_library(2d_lidar_sdk SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/C3iroboticsLidar.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CArcTime.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CCountDown.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CDeviceConnection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CLidarPacket.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CLidarPacketReceiver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CLidarUnpacket.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CSerialConnection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CSimulateSerial.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sdk/src/CTime.cpp
)

add_executable(old_lidar_driver 
  ${CMAKE_CURRENT_SOURCE_DIR}/2d_lidar_driver.cpp
)

set(PROJECT_TARGETS 
  old_lidar_driver 
)

foreach(target ${PROJECT_TARGETS})
  ament_target_dependencies(${target}
    ${DEPENDS}
  )
endforeach()

target_link_libraries(old_lidar_driver 2d_lidar_sdk)

install(TARGETS 
  2d_lidar_sdk
  LIBRARY DESTINATION lib)

install(TARGETS 
  old_lidar_driver
  DESTINATION lib/${SENSOR_BRIDGE_PACKAGE_NAME})

ament_package()