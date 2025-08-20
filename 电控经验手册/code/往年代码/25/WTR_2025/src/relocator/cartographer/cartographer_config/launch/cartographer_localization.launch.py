from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os
from launch.substitutions import LaunchConfiguration
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    pkg_share = FindPackageShare(package='cartographer_config').find('cartographer_config')
    cartographer_config_dir = os.path.join(pkg_share, 'config')
    configuration_basename = 'config_locator.lua'
    pbstream_file = os.path.join(pkg_share, 'pbstream')+"/basketball_trajectory.pbstream"

    return LaunchDescription([
        Node(
            package='cartographer_ros',
            executable='cartographer_node',
            output='screen',
            parameters=[{'use_sim_time': False,
                         "/localization":True,#when build change it to False
                         "/set_inital_pose_x":0.64,
                         "/set_inital_pose_y":-2.07,
                         "/set_inital_pose_z":0.0,
                         "/set_inital_pose_ox":0.0,
                         "/set_inital_pose_oy":0.0,
                         "/set_inital_pose_oz":-0.028,
                         "/set_inital_pose_ow":1.0,}],
            arguments=[
                '-configuration_directory', cartographer_config_dir,
                '-configuration_basename', configuration_basename,
                '-load_state_filename', pbstream_file,
            ],
            remappings=[
                ('scan', 'scan'),
            ],
        ),
    ])
