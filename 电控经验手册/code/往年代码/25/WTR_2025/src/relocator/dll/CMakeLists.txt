cmake_minimum_required(VERSION 3.8)

project(dll)

if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
set (CMAKE_BUILD_TYPE Debug)
# find dependencies

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(octomap REQUIRED)
# find_package(octomath REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(Glog REQUIRED)
find_package(tf2_ros REQUIRED)

# Ceres solver
find_package(Ceres REQUIRED)

# OpenMP library
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
  message(STATUS "OPENMP FOUND")
  set(OpenMP_FLAGS ${OpenMP_CXX_FLAGS})  # or if you use C: ${OpenMP_C_FLAGS}
  set(OpenMP_LIBS gomp)
endif()
add_compile_options(-O3 -march=native -mtune=native -mavx -mavx2 -mfma -fopenmp-simd ${OpenMP_FLAGS})

add_executable(dll_node src/dll_node.cpp)
ament_target_dependencies(dll_node ament_cmake rclcpp tf2_ros tf2 tf2_geometry_msgs geometry_msgs pcl_conversions pcl_ros sensor_msgs octomap nav_msgs OpenMP Ceres glog)
target_link_libraries(dll_node  ${OpenMP_LIBS} ${OCTOMAP_LIBRARIES} octomap ceres )# Necessary to build 
# target_link_libraries(dll_node octomath)

add_executable(grid3d_node src/grid3d_node.cpp)
ament_target_dependencies(grid3d_node ament_cmake rclcpp tf2 geometry_msgs pcl_conversions pcl_ros sensor_msgs octomap nav_msgs OpenMP tf2_geometry_msgs)
target_link_libraries(grid3d_node  ${OpenMP_LIBS} ${OCTOMAP_LIBRARIES} octomap)# Necessary to build 
# target_link_libraries(grid3d_node octomath)

install(TARGETS
  dll_node
  DESTINATION lib/${PROJECT_NAME}
)
install(TARGETS
  grid3d_node
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY
  launch
  maps
  bags
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

include_directories(
  ${rclcpp_INCLUDE_DIRS}
  ${CERES_INCLUDE_DIRS}
  ${OCTOMAP_INCLUDE_DIRS}
  ${tf2_ros_INCLUDE_DIRS}
  ${tf2_INCLUDE_DIRS}
  ${tf2_geometry_msgs_INCLUDE_DIRS}
)

ament_package()

