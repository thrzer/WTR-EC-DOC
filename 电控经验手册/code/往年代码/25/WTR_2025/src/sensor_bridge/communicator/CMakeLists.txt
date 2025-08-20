cmake_minimum_required(VERSION 3.22)
project(communicator)

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


# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(serial REQUIRED)
find_package(std_msgs REQUIRED)

set(DEPENDS 
  "rclcpp"
  "std_msgs"
  "serial"
)


add_executable(communicator_node  src/communicator_node.cpp)
target_include_directories(communicator_node PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/communicator ${CMAKE_CURRENT_SOURCE_DIR}/../serial/include)
ament_target_dependencies(communicator_node ${DEPENDS})


install(TARGETS 
  communicator_node 
  DESTINATION lib/${PROJECT_NAME}) 

ament_package()
