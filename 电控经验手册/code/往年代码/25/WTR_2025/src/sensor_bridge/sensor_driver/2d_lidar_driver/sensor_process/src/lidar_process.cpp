#include "rclcpp/rclcpp.hpp"

#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include "std_msgs/msg/string.hpp"



#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/passthrough.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/statistical_outlier_removal.h> 
#include <pcl/common/time.h>

#include "pcl_ros/transforms.hpp"
#include <pcl_conversions/pcl_conversions.h>

#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_ros/transform_listener.h"
#include "tf2_eigen/tf2_eigen.hpp"
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2/transform_datatypes.h>

using PCLPoint = pcl::PointXYZ;
using PCLPointCloud = pcl::PointCloud<PCLPoint>;

class LidarProcess : public rclcpp::Node
{
public:
    LidarProcess(const std::string name) : Node(name)
    {
        // 初始化变量
        m_worldFrameId = "map";
        m_lidarFrameId = "base_link";

        m_filter_height_low = 0.0;
        m_filter_height_high = 0.5;

        cloud_current_ = PCLPointCloud::Ptr(new PCLPointCloud());

        // 订阅雷达点云数据
        rclcpp::QoS qos(1); 
        qos.reliability(RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT); 
        lidar_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>("/livox/lidar/pointcloud", qos, std::bind(&LidarProcess::lidar_callback, this, std::placeholders::_1));

        // 发布
        project_cloud_pub_ = create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_in", 10);

        // 处理数据定时器
        project_timer_ = create_wall_timer(std::chrono::milliseconds(static_cast<int64_t>(1/m_scan_rate*1000)), std::bind(&LidarProcess::pointcloud_to_projection, this));

    }

private:
    // 订阅者
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr lidar_sub_;

    // 发布者
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr project_cloud_pub_;

    // 定时器
    rclcpp::TimerBase::SharedPtr project_timer_;

    // 变量
    std::string m_worldFrameId; // 地图坐标系
    std::string m_lidarFrameId; // 雷达坐标系

    double m_filter_height_low;     
    double m_filter_height_high;

    double m_scan_rate;         // 激光扫描频率
   
    PCLPointCloud::Ptr cloud_current_;
    PCLPointCloud cloud_current_project_scan_;                

    rclcpp::Time project_scan_time_;

    bool project_scan_finished = false;
    bool project_scan_new = false;

// TODO: 订阅点云数据
void lidar_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
{
    RCLCPP_INFO(this->get_logger(), "Received a point cloud message");

    cloud_current_->clear();
    pcl::fromROSMsg(*msg, *cloud_current_);
    
    cloud_current_project_scan_ = *cloud_current_;

    project_scan_new = true;
    if(project_scan_finished)
        project_scan_time_ = msg->header.stamp;
  

    m_lidarFrameId = msg->header.frame_id;
    
}

// TODO: 发布投影点云数据
void publish_projected_cloud(sensor_msgs::msg::PointCloud2 msg)
{
    msg.header.frame_id = m_lidarFrameId;
    msg.header.stamp = project_scan_time_;
    project_cloud_pub_->publish(msg);
    RCLCPP_INFO(this->get_logger(), "Publish projected cloud.");
}


// TODO: 投影点云
void pointcloud_to_projection()
{
    if(cloud_current_project_scan_.empty()){
        RCLCPP_ERROR(this->get_logger(),"No PointCloud to project.");
        return;
    }
    if(!project_scan_new)
        return;

    project_scan_finished = false;
    auto start = std::chrono::steady_clock::now();
    PCLPointCloud::Ptr cloud_filtered(new PCLPointCloud());
    PCLPointCloud::Ptr cloud_in(new PCLPointCloud());
    *cloud_in = cloud_current_project_scan_;

    pcl::ConditionAnd<pcl::PointXYZ>::Ptr range_cond(new pcl::ConditionAnd<pcl::PointXYZ>);
    range_cond->addComparison(pcl::FieldComparison<pcl::PointXYZ>::ConstPtr(new pcl::FieldComparison<pcl::PointXYZ>("z", pcl::ComparisonOps::GT, m_filter_height_low)));
    range_cond->addComparison(pcl::FieldComparison<pcl::PointXYZ>::ConstPtr(new pcl::FieldComparison<pcl::PointXYZ>("z", pcl::ComparisonOps::LT, m_filter_height_high)));

   
    pcl::ConditionalRemoval<pcl::PointXYZ> condrem;
    condrem.setCondition(range_cond);
    condrem.setInputCloud(cloud_in);
    condrem.setKeepOrganized(false);
    condrem.filter(*cloud_filtered);



    sensor_msgs::msg::PointCloud2 cloud_filtered_msg;
    pcl::toROSMsg(*cloud_filtered, cloud_filtered_msg);
    publish_projected_cloud(cloud_filtered_msg);

    for (size_t i = 0; i < cloud_filtered->points.size(); i++)
    {
        cloud_filtered->points[i].z = 0;
    }

    project_scan_finished = true;
    project_scan_new = false;
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    RCLCPP_INFO(this->get_logger(), "Time lapse %f", elapsed_seconds.count());  
}


};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LidarProcess>("lidar_process");

    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}