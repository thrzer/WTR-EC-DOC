import os.path

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch.conditions import IfCondition

from launch_ros.actions import Node
import numpy as np


def generate_launch_description():
    package_path = get_package_share_directory('serial_communicate_')

    serial_node = Node(
        package='serial_communicate_',
        executable='serial_to_stm32',
        output='screen'
    )
    # r = -0.238
    # theta = 24.26/180.0*np.pi
    tf_node = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='static_tf',
        output='screen',
        arguments=[
            '-0.21','-0.23', '-0.40',                           
            '0.0', '0.0', '0.0',                   
            'body', 'base_link'                 
        ]
    )

    

    ld = LaunchDescription()
    ld.add_action(tf_node)
    ld.add_action(serial_node)

    return ld
