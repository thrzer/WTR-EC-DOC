#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
#include "tf2_eigen/tf2_eigen.hpp"
#include <pcl/common/transforms.h>
#include "pcl_conversions/pcl_conversions.h"
#include <deque>

namespace lslidar
{
class CombineLslidar : public rclcpp::Node
{
public:
    CombineLslidar() : Node("combine_lslidar")
    {
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        
        left_laser_tf_ = Eigen::Matrix4f::Identity();
        right_laser_tf_ = Eigen::Matrix4f::Identity();

        // 获取坐标变换并存储
        bool get_tf = false;
        while (rclcpp::ok()) {
            try {
                // auto left_laser_geom_ = tf_buffer_->lookupTransform("laser_link_left", "base_link", tf2::TimePointZero);
                // auto left_laser_eigen = tf2::transformToEigen(left_laser_geom_.transform);
                // left_laser_tf_ = left_laser_eigen.matrix().cast<float>();

                auto right_laser_geom_ = tf_buffer_->lookupTransform("laser_link_right", "laser_link_left", tf2::TimePointZero);
                auto right_laser_eigen = tf2::transformToEigen(right_laser_geom_.transform);
                right_laser_tf_ = right_laser_eigen.matrix().cast<float>();

                get_tf = true;
            } catch (tf2::TransformException &ex) {
                RCLCPP_ERROR(this->get_logger(), "%s", ex.what());
                rclcpp::sleep_for(std::chrono::milliseconds(100));
            }

            if (get_tf) {
                break;
            }
        }

        left_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "pc_left", 10, 
            std::bind(&CombineLslidar::left_callback, this, std::placeholders::_1));

        right_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            "pc_right", 10, 
            std::bind(&CombineLslidar::right_callback, this, std::placeholders::_1));

        scan_info_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "scan_left", 10, 
            std::bind(&CombineLslidar::scan_info_callback, this, std::placeholders::_1));

        combined_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("points2", 10);
        
        combined_scan_ = std::make_shared<sensor_msgs::msg::PointCloud2>();
        combined_scan_->header.frame_id = "base_link";
        combined_scan_->width = 0;
        combined_scan_->height = 1;
        combined_scan_->is_dense = false;
        combined_scan_->is_bigendian = false;

        // combined_scan_->angle_min = -M_PI;
        // combined_scan_->angle_max = M_PI;
        // combined_scan_->angle_increment = 2 * M_PI / 480;
        // combined_scan_->time_increment = 0.0;
        // combined_scan_->scan_time = 0.05;
        // combined_scan_->range_min = 0.37;
        // combined_scan_->range_max = 100.0;
        // combined_scan_->intensities.resize(480);
        // combined_scan_->ranges.resize(480);

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&CombineLslidar::combine_scan, this));   
    }

private:
    // 构造tf2变量，用于监听左右两个雷达与base_link的坐标变换
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    // 两个雷达对于base_link的坐标变换
    Eigen::Matrix4f left_laser_tf_;
    Eigen::Matrix4f right_laser_tf_;

    // 两个雷达的订阅者
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr left_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr right_sub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_info_sub_;

    // 合并后的雷达数据的发布者
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr combined_pub_;

    // 合并的 scan 消息
    sensor_msgs::msg::PointCloud2::SharedPtr combined_scan_;

    // 消息存储deque
    std::deque<pcl::PointCloud<pcl::PointXYZ>::Ptr> left_pc_deque_;
    std::deque<pcl::PointCloud<pcl::PointXYZ>::Ptr> right_pc_deque_;
    std::deque<sensor_msgs::msg::LaserScan::SharedPtr> scan_info_deque_;

    // 处理定时器
    rclcpp::TimerBase::SharedPtr timer_;

    void left_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        // RCLCPP_INFO(this->get_logger(), "left_callback");
        // 获取llaser_link_left与base_link之间的坐标关系，对点云进行tf变换处理
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::fromROSMsg(*msg, *cloud_in);
        pcl::transformPointCloud(*cloud_in, *cloud_out, left_laser_tf_);
        // 加入队列
        left_pc_deque_.push_back(cloud_out);
    }

    void right_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        // RCLCPP_INFO(this->get_logger(), "right_callback");
        // 获取llaser_link_right与base_link之间的坐标关系，对点云进行tf变换处理
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZ>);
        pcl::fromROSMsg(*msg, *cloud_in);
        pcl::transformPointCloud(*cloud_in, *cloud_out, right_laser_tf_);
        // 加入队列
        right_pc_deque_.push_back(cloud_out);
    }

    void scan_info_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
    {
        // scan_info_deque_.push_back(msg);
    }

    void combine_scan()
    {
        // RCLCPP_INFO(this->get_logger(), "combine scan");
        if (!left_pc_deque_.empty() && !right_pc_deque_.empty())
        {
            // RCLCPP_INFO(this->get_logger(), "enable combine scan");
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_left = left_pc_deque_.front();
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_right = right_pc_deque_.front();

            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_combined(new pcl::PointCloud<pcl::PointXYZ>);
            *cloud_combined = *cloud_left + *cloud_right;
            pcl::toROSMsg(*cloud_combined, *combined_scan_);


            left_pc_deque_.pop_front();
            right_pc_deque_.pop_front();

            

            // auto scan_info = scan_info_deque_.front();
            // combined_scan_->header = scan_info->header;
            // combined_scan_->angle_increment = scan_info->angle_increment;
            // combined_scan_->time_increment = scan_info->time_increment;
            // scan_info_deque_.pop_front();

            // combined_scan_->ranges.clear();

            // for (const auto& point : cloud_combined->points)
            // {
            //     if (point.x == 0 && point.y == 0)
            //         continue;

            //     float range = std::sqrt(point.x * point.x + point.y * point.y);
            //     float angle = std::atan2(point.y, point.x);

                

            //     if(range > combined_scan_->range_max || range < combined_scan_->range_min)
            //         continue;

            //     int index = static_cast<int>((angle - combined_scan_->angle_min) / combined_scan_->angle_increment);

                
            //     if (index >= 0 && index <= 480)
            //     {
            //         RCLCPP_INFO(this->get_logger(), "index: %d, range: %f, angle: %f", index, range, angle);
            //         combined_scan_->ranges[index] = range;
            //         combined_scan_->intensities[index] = 1;
            //     }
            // }
            combined_scan_->header.stamp = this->now();
            combined_pub_->publish(*combined_scan_);
        }
    }




};
}  // namespace lslidar

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<lslidar::CombineLslidar>());
    rclcpp::shutdown();
    return 0;
}
