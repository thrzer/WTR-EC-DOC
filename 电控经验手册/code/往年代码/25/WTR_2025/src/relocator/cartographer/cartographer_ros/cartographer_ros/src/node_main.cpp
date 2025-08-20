/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 #include "absl/memory/memory.h"
 #include "cartographer/mapping/map_builder.h"
 #include "cartographer_ros/node.h"
 #include "cartographer_ros/node_options.h"
 #include "cartographer_ros/ros_log_sink.h"
 #include "gflags/gflags.h"
 #include "tf2_ros/transform_listener.h"
 #include <rclcpp/rclcpp.hpp>
 #include <string>
 #include <cartographer_ros/msg_conversion.h> 
 #include <cartographer/transform/rigid_transform.h> 
 
 template<typename T>
 T getParam_Func(rclcpp::Node::SharedPtr node,
                 const std::string& param_name, const T& default_val)
 {
     T param_val;
     if (node->has_parameter(param_name)) {
         node->get_parameter(param_name, param_val);
     } else {
         param_val = default_val;
 }
     RCLCPP_INFO(node->get_logger(),"we had successfully gotten into  cartographer_ros package !!!!!!!!!!");
     return param_val;
     RCLCPP_INFO(node->get_logger(),"we had successfully gotten into  cartographer_ros package !!!!!!!!!!");
 }
 
 DEFINE_bool(collect_metrics, false,
             "Activates the collection of runtime metrics. If activated, the "
             "metrics can be accessed via a ROS service.");
 DEFINE_string(configuration_directory, "",
               "First directory in which configuration files are searched, "
               "second is always the Cartographer installation to allow "
               "including files from there.");
 DEFINE_string(configuration_basename, "",
               "Basename, i.e. not containing any directory prefix, of the "
               "configuration file.");
 DEFINE_string(load_state_filename, "",
               "If non-empty, filename of a .pbstream file to load, containing "
               "a saved SLAM state.");
 DEFINE_bool(load_frozen_state, true,
             "Load the saved state as frozen (non-optimized) trajectories.");
 DEFINE_bool(
     start_trajectory_with_default_topics, true,
     "Enable to immediately start the first trajectory with default topics.");
 DEFINE_string(
     save_state_filename, "",
     "If non-empty, serialize state and write it to disk before shutting down.");
 
 namespace cartographer_ros {
 namespace {
 
 void Run() {
   rclcpp::Node::SharedPtr cartographer_node = rclcpp::Node::make_shared("cartographer_node");
   constexpr double kTfBufferCacheTimeInSeconds = 10.;
 
   std::shared_ptr<tf2_ros::Buffer> tf_buffer =
       std::make_shared<tf2_ros::Buffer>(
         cartographer_node->get_clock(),
         tf2::durationFromSec(kTfBufferCacheTimeInSeconds),
         cartographer_node);
 
   std::shared_ptr<tf2_ros::TransformListener> tf_listener =
       std::make_shared<tf2_ros::TransformListener>(*tf_buffer);
 
   NodeOptions node_options;
   TrajectoryOptions trajectory_options;
   std::tie(node_options, trajectory_options) =
       LoadOptions(FLAGS_configuration_directory, FLAGS_configuration_basename);
 
   auto map_builder =
     cartographer::mapping::CreateMapBuilder(node_options.map_builder_options);
   auto node = std::make_shared<cartographer_ros::Node>(
     node_options, std::move(map_builder), tf_buffer, cartographer_node,
     FLAGS_collect_metrics);
 
 // 获取轨迹配置项指针
 auto trajectory_options_handle = &(trajectory_options);  //原博文这里少了个auto
 // ros::NodeHandle pnh("~");
 // 获取配置参数
 //声明参数
 cartographer_node->declare_parameter<bool>("/localization", false);
 cartographer_node->declare_parameter<double>("/set_inital_pose_x", 0);
 cartographer_node->declare_parameter<double>("/set_inital_pose_y", 0);
 cartographer_node->declare_parameter<double>("/set_inital_pose_z", 0);
 cartographer_node->declare_parameter<double>("/set_inital_pose_ox", 0);
 cartographer_node->declare_parameter<double>("/set_inital_pose_oy", 0);
 cartographer_node->declare_parameter<double>("/set_inital_pose_oz", 0);
 cartographer_node->declare_parameter<double>("/set_inital_pose_ow", 1.0);
 
 bool localization = getParam_Func<bool>(cartographer_node, "/localization", false);      //是否为纯定位模式
 double pos_x = getParam_Func<double>(cartographer_node, "/set_inital_pose_x", 0.0); 
 double pos_y = getParam_Func<double>(cartographer_node, "/set_inital_pose_y", 0.0); 
 double pos_z = getParam_Func<double>(cartographer_node, "/set_inital_pose_z", 0.0); 
 double pos_ox = getParam_Func<double>(cartographer_node, "/set_inital_pose_ox", 0.0); 
 double pos_oy = getParam_Func<double>(cartographer_node, "/set_inital_pose_oy", 0.0); 
 double pos_oz = getParam_Func<double>(cartographer_node, "/set_inital_pose_oz", 0.0); 
 double pos_ow = getParam_Func<double>(cartographer_node, "/set_inital_pose_ow", 1.0); 
 geometry_msgs::msg::Pose init_pose;
 init_pose.position.x = pos_x;
 init_pose.position.y = pos_y;
 init_pose.position.z = pos_z;
 init_pose.orientation.x = pos_ox;
 init_pose.orientation.y = pos_oy;
 init_pose.orientation.z = pos_oz;
 init_pose.orientation.w = pos_ow;
 
 if(localization == true)//localization == true
 {
 //更改轨迹配置项中的初始位姿值
 *trajectory_options_handle->trajectory_builder_options.mutable_initial_trajectory_pose()->mutable_relative_pose()
     = cartographer::transform::ToProto(cartographer_ros::ToRigid3d(init_pose));
 }
 
     
   if (!FLAGS_load_state_filename.empty()) {
     node->LoadState(FLAGS_load_state_filename, FLAGS_load_frozen_state);
   }
 
   if (FLAGS_start_trajectory_with_default_topics) {
     node->StartTrajectoryWithDefaultTopics(trajectory_options);
   }
 
   rclcpp::spin(cartographer_node);
 
   node->FinishAllTrajectories();
   node->RunFinalOptimization();
 
   if (!FLAGS_save_state_filename.empty()) {
     node->SerializeState(FLAGS_save_state_filename,
                         true /* include_unfinished_submaps */);
   }
 }
 
 }  // namespace
 }  // namespace cartographer_ros
 
 int main(int argc, char** argv) {
   // Init rclcpp first because gflags reorders command line flags in argv
   rclcpp::init(argc, argv);
 
   google::AllowCommandLineReparsing();
   google::InitGoogleLogging(argv[0]);
   google::ParseCommandLineFlags(&argc, &argv, false);
 
   CHECK(!FLAGS_configuration_directory.empty())
       << "-configuration_directory is missing.";
   CHECK(!FLAGS_configuration_basename.empty())
       << "-configuration_basename is missing.";
 
   cartographer_ros::ScopedRosLogSink ros_log_sink;
   cartographer_ros::Run();
   ::rclcpp::shutdown();
 }
 