
import os
from launch import LaunchDescription
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    pkg_share = FindPackageShare(package='cartographer_config').find('cartographer_config')
    
    use_sim_time = LaunchConfiguration('use_sim_time', default='false')
    # 地图的分辨率
    resolution = LaunchConfiguration('resolution', default='0.05')
    # 地图的发布周期
    publish_period_sec = LaunchConfiguration('publish_period_sec', default='1.0')
    # 配置文件夹路径
    configuration_directory = LaunchConfiguration('configuration_directory',default= os.path.join(pkg_share, 'config') )
    # 配置文件
    configuration_basename = LaunchConfiguration('configuration_basename', default='config_map.lua')
    rviz_config_dir = os.path.join(pkg_share, 'config')+"/cartographer.rviz"

    cartographer_node = Node(
        package='cartographer_ros',
        executable='cartographer_node',
        name='cartographer_node',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}],
        arguments=['-configuration_directory', configuration_directory,
                   '-configuration_basename', configuration_basename])

    cartographer_occupancy_grid_node = Node(
        package='cartographer_ros',
        executable='cartographer_occupancy_grid_node',
        name='cartographer_occupancy_grid_node',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}],
        arguments=['-resolution', resolution, '-publish_period_sec', publish_period_sec])
    
    rviz_node =  Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config_dir],
            parameters=[{'use_sim_time': use_sim_time}],
            output='screen')
    
    # pointcloud2scan = Node(
    #     package='pointcloud_to_laserscan',
    #     executable='pointcloud_to_laserscan_node',
    #     name='pointcloud_to_laserscan',
    #     remappings=[('cloud_in', ['/points2']),
    #                 ('scan', ['/scan'])],
    #     parameters=[{'target_frame': 'base_link', 'transform_tolerance': 0.01}]
    # )
    params = {
        'target_frame': 'base_link',  # 禁用以输出scan在pointcloud的frame中
        'transform_tolerance': 0.01,
        'use_sim_time': False,
        'min_height': 0.0,
        'max_height': 20.0,
        # 'angle_min': -3.1415926,  # -M_PI
        # 'angle_max': 3.1415926,   # M_PI
        # 'angle_increment': 0.8, # 0.17度
        # 'scan_time': 0.1,
        # 'range_min': 0.2,
        # 'range_max': 100,
        # 'use_inf': True,
        # 'concurrency_level': 1
    }

    # 创建节点
    pointcloud_to_laserscan_node = Node(
        package='pointcloud_to_laserscan',
        executable='pointcloud_to_laserscan_node',
        name='pointcloud_to_laserscan',
        remappings=[('cloud_in', '/points2'),('scan', ['/scan'])],
        parameters=[params]
    )

    ld = LaunchDescription()
    ld.add_action(pointcloud_to_laserscan_node)
    ld.add_action(cartographer_node)
    ld.add_action(cartographer_occupancy_grid_node)
    ld.add_action(rviz_node)

    return ld