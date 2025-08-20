#!/usr/bin/python3
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import LifecycleNode
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
import numpy as np
import lifecycle_msgs.msg
import os

def generate_launch_description():

    driver_dir_left = os.path.join(get_package_share_directory('lslidar_driver'), 'params','lidar_uart_ros2', 'lsn10p_left.yaml')
    driver_dir_right = os.path.join(get_package_share_directory('lslidar_driver'), 'params','lidar_uart_ros2', 'lsn10p_right.yaml')

    static_tf_left = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='static_tf_left',
        output='screen',
        arguments=[
            str(-0.36256 * np.sin(14.73 / 180.0 * np.pi)), str(0.36256 * np.cos(14.73 / 180.0 * np.pi)), '0.0',                           
            str(-3.1415926 / 2), str(3.1415926), '0.0',                   
            'base_link', 'laser_link_left'                    
        ]
    )
    
    static_tf_right = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='static_tf_right',
        output='screen',
        arguments=[
            str(-0.36256 * np.sin(14.73 / 180.0 * 3.1415926)), str(-0.36256 * np.cos(14.73 / 180.0 * 3.1415926)), '0.0',                           
            str(+3.1415926 / 2), str(3.1415926), '0.0',       
            'base_link', 'laser_link_right'                    
        ]
    )
                     
    driver_node_left = LifecycleNode(package='lslidar_driver',
                                executable='lslidar_driver_node',
                                name='lslidar_driver_node',		
                                output='screen',
                                emulate_tty=True,
                                namespace='',
                                parameters=[driver_dir_left],
                                )
    
    driver_node_right = LifecycleNode(package='lslidar_driver',
                                executable='lslidar_driver_node',
                                name='lslidar_driver_node',		
                                output='screen',
                                emulate_tty=True,
                                namespace='',
                                parameters=[driver_dir_right],
                                )
    
    combine_scan_node = Node(
                package='lslidar_driver',
                executable='lslidar_combine_node',
                name='lslidar_combine_node',
                output='screen',
            )
    


    return LaunchDescription([
        static_tf_left,
        static_tf_right,
        driver_node_left,
        driver_node_right,
        combine_scan_node,
    ])

