from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory
from rclpy.qos import QoSProfile, QoSReliabilityPolicy, QoSHistoryPolicy

def generate_launch_description():
    package_name = 'pointcloud_to_laserscan'
    package_share_dir = get_package_share_directory('cp_to_scan')
    params_file = os.path.join(package_share_dir, 'config', 'params.yaml')

    pointcloud_to_laserscan_node = Node(
        package='pointcloud_to_laserscan',
        executable='pointcloud_to_laserscan_node',
        name='pointcloud_to_laserscan',
        output='screen',
        parameters=[
            {
            'target_frame': 'base_link',      # 目标坐标系
            'transform_tolerance': 0.01,    # 坐标变换的容忍时间
            'min_height': -1.0,              # 点云的最小高度
            'max_height': 1.0,              # 点云的最大高度
            'angle_min': -3.1415927,        # 激光扫描的最小角度（弧度）
            'angle_max': 3.1415927,         # 激光扫描的最大角度（弧度）
            'angle_increment': 0.00174532923,  # 激光扫描的角度增量0.0174532923
            'scan_time': 0.5333,            # 扫描时间
            'range_min': 0.12,              # 激光扫描的最小范围
            'range_max': 18.0,              # 激光扫描的最大范围jj
            'use_inf': True,                # 是否使用无穷大值表示最大范围
            'inf_epsilon': 1.0,             # 无穷大值的近似值
            'concurrency_level': 1,          # 并发级别
            'point_cloud_topic': '/cloud_in', # 输入的点云话题
            'scan_topic': '/scan',      
            }
                    ]
    )

    return LaunchDescription([
        pointcloud_to_laserscan_node
    ])