#include "rclcpp/rclcpp.hpp"

#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include <deque>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/passthrough.h>
#include <pcl/io/pcd_io.h>
#include <pcl/conversions.h>
#include <pcl/registration/icp.h>
#include <pcl/io/pcd_io.h>
#include <pcl/surface/mls.h>
#include <pcl/search/kdtree.h>
#include <pcl/filters/approximate_voxel_grid.h>
#include <pcl/filters/statistical_outlier_removal.h> 
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_eigen/tf2_eigen.hpp>

using PCLPoint = pcl::PointXYZ;
using PCLPointCloud = pcl::PointCloud<PCLPoint>;

class BasketRegister : public rclcpp::Node
{
public:
    BasketRegister(const std::string name) : Node(name)
    {
        // 初始化变量
        cloud_current_ = PCLPointCloud::Ptr(new PCLPointCloud());
        // 加载篮球架点云
        basket_stand_cloud_ = PCLPointCloud::Ptr(new PCLPointCloud());
        m_boardilterPlaneThreshold = 0.15;
        m_boardFilterPlaneDistance_low = 2.0;
        m_boardFilterPlaneDistance_high = 4.0;
        m_boardPoints_min = 10;
        m_boardPoints_max = 1000;
        m_searchRadius = 1;
        m_upSamplingRadius = 0.2;
        m_upSamplingStep = 0.04;
        m_lidarFrameId = "body";//laser_link

        // if (pcl::io::loadPCDFile<PCLPoint>("PCD/basketballStand_zero.pcd", *basket_stand_cloud_) == -1) {
        //     RCLCPP_ERROR(this->get_logger(), "Couldn't read file PCD/basketballStand_zero.pcd");
        //     return;
        // }
        // tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        // tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        // 订阅雷达点云数据
        rclcpp::QoS qos(10); // 设置队列大小为10
        qos.reliability(RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT); // 确保使用相同的可靠性设置
        lidar_sub_ = create_subscription<sensor_msgs::msg::PointCloud2>("unilidar/cloud", qos, std::bind(&BasketRegister::lidar_callback, this, std::placeholders::_1));///unilidar/cloud///livox/lidar_PointCloud2

        // 发布
        // transform_pub_ = create_publisher<geometry_msgs::msg::TransformStamped>("transform_matrix", qos);
        marker_pub_ = create_publisher<visualization_msgs::msg::Marker>("registration_marker", qos);
        segment_cloud_pub_1 = create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_segment_close", qos);
        segment_cloud_pub_2 = create_publisher<sensor_msgs::msg::PointCloud2>("/cloud_segment_far", qos);
        // 处理数据定时器
        register_timer_ = create_wall_timer(std::chrono::milliseconds(static_cast<int64_t>(1)), std::bind(&BasketRegister::basket_register_callback, this));

    }

private:
    // 订阅者
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr lidar_sub_;


    // 发布者
    rclcpp::Publisher<geometry_msgs::msg::TransformStamped>::SharedPtr transform_pub_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr marker_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr segment_cloud_pub_1;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr segment_cloud_pub_2;

    // 服务端
   

    // 定时器
    rclcpp::TimerBase::SharedPtr register_timer_;

    // 互斥锁
    std::mutex m_mutex;
    // 条件变量
    std::condition_variable m_data_cond;

    // 变量
    PCLPointCloud::Ptr cloud_current_;                  // 当前点云帧
    PCLPointCloud::Ptr basket_stand_cloud_; // 篮球架点云

    std::deque<PCLPointCloud> lidar_cloud_buffer_; 
    std::deque<rclcpp::Time> lidar_cloud_time_buffer_;

    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    double m_boardilterPlaneThreshold;
    double m_boardFilterPlaneDistance_low;
    double m_boardFilterPlaneDistance_high;
    double m_boardPoints_min;
    double m_boardPoints_max;
    double MAX_BASKETBOARD_POINTS;
    double m_searchRadius;
    double m_upSamplingRadius;
    double m_upSamplingStep;
    std::string m_lidarFrameId;

    // TODO: 订阅点云数据
    void lidar_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "Received a point cloud message");
        // m_mutex.lock();
        cloud_current_->clear();
        pcl::fromROSMsg(*msg, *cloud_current_);

        lidar_cloud_buffer_.push_back(*cloud_current_);

        lidar_cloud_time_buffer_.push_back(msg->header.stamp);
        
        // m_mutex.unlock();
        // m_data_cond.notify_all();
    }
    
    void publish_segment_cloud(sensor_msgs::msg::PointCloud2 msg1, sensor_msgs::msg::PointCloud2 msg2)
    {
        msg1.header.frame_id = m_lidarFrameId;
        msg1.header.stamp = lidar_cloud_time_buffer_.front();
        msg2.header.frame_id = m_lidarFrameId;
        msg2.header.stamp = lidar_cloud_time_buffer_.front();
        lidar_cloud_time_buffer_.pop_front();
        segment_cloud_pub_1->publish(msg1);
        segment_cloud_pub_2->publish(msg2);
        RCLCPP_INFO(this->get_logger(), "Publish segment cloud.");
    }

    bool areBoards(const std::vector<float>& v1, const std::vector<float>& v2) {
        bool areBoards = false;
        if(std::abs((v1[0] * v2[1] - v2[0] * v1[1])) < 1e-3){
            double distance = std::abs(v1[3] - v2[3]) / std::sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
            if(std::abs(distance - 13.8) < 1){
                areBoards = true;
            }else{
                RCLCPP_ERROR(this->get_logger(), "Distance doesn't match!");
            }
        }else{
            RCLCPP_ERROR(this->get_logger(), "no parallel!");
        }
        return areBoards;
    }




    void basket_register_callback()
    {
        if (lidar_cloud_buffer_.empty())
            return;

        PCLPointCloud::Ptr cloud_input(new PCLPointCloud(lidar_cloud_buffer_.front()));
        lidar_cloud_buffer_.pop_front();

        // // 使用ICP进行点云配准
        // pcl::IterativeClosestPoint<PCLPoint, PCLPoint> icp;
        // icp.setInputSource(cloud);
        // icp.setInputTarget(basket_stand_cloud_);
        // PCLPointCloud Final;
        // icp.align(Final);

        // if (icp.hasConverged()) {
        //     Eigen::Matrix4f transformation = icp.getFinalTransformation();
        //     publish_transformation(transformation);
        //     publish_marker(transformation); // 发布标记
        // } else {
        //     RCLCPP_WARN(this->get_logger(), "ICP did not converge.");
        // }

        pcl::PointCloud<PCLPoint>::Ptr pc(new pcl::PointCloud<PCLPoint>);
        
        pcl::PassThrough<PCLPoint> pass;
        pass.setInputCloud(cloud_input);
        pass.setFilterFieldName("z"); 
        pass.setFilterLimits(m_boardFilterPlaneDistance_low, m_boardFilterPlaneDistance_high);
        pass.filter(*pc);

        // pcl::MovingLeastSquares<pcl::PointXYZ, pcl::PointNormal> up;
        // up.setInputCloud(pc);
        // pcl::search::KdTree<pcl::PointXYZ>::Ptr tree;
        // up.setSearchMethod(tree);
        // up.setSearchRadius(m_searchRadius);

        // up.setComputeNormals(true);
        // up.setUpsamplingMethod(pcl::MovingLeastSquares<pcl::PointXYZ, pcl::PointNormal>::SAMPLE_LOCAL_PLANE);
        // up.setUpsamplingRadius(m_upSamplingRadius);
        // up.setUpsamplingStepSize(m_upSamplingStep);
        // up.setPolynomialOrder(2); 
        // up.setProjectionMethod(pcl::MLSResult::ProjectionMethod::SIMPLE);
        // up.setNumberOfThreads(4);
        // up.setPointDensity(1);
        // pcl::PointCloud<pcl::PointNormal>::Ptr cloud_up(new pcl::PointCloud<pcl::PointNormal>); 
        // cloud_up->clear(); 
        // up.process(*cloud_up);

        // pcl::PointCloud<pcl::PointNormal>::Ptr filtered_cloud1(new pcl::PointCloud<pcl::PointNormal>);
        // pcl::ApproximateVoxelGrid<pcl::PointNormal> avf1;
        // avf1.setInputCloud(cloud_up);     
        // avf1.setLeafSize(0.04, 0.04, 0.04);
        // avf1.filter(*filtered_cloud1);     

        // pcl::copyPointCloud(*cloud_up, *pc);

        PCLPointCloud::Ptr pc_board = PCLPointCloud::Ptr(new PCLPointCloud);    

        pc_board->header = pc->header;

        pcl::ModelCoefficients coefficients;

        pcl::PointIndices::Ptr inliers(new pcl::PointIndices);

        pcl::SACSegmentation<PCLPoint> seg;

        seg.setOptimizeCoefficients(true);
        // TODO: 也许基于表面法线的过滤可能更稳健/更准确？
        seg.setModelType(pcl::SACMODEL_PLANE);
        seg.setMethodType(pcl::SAC_RANSAC);
        seg.setMaxIterations(200);
        seg.setDistanceThreshold(m_boardilterPlaneThreshold);

        PCLPointCloud::Ptr cloud_filtered(pc);
        pcl::ExtractIndices<PCLPoint> extract;
        int board_cnt = 0;

        std::vector<std::pair<std::vector<float>, PCLPointCloud>> clouds;
        
        if (pc->size() < m_boardPoints_min)
        {
            RCLCPP_WARN(this->get_logger(), "Pointcloud too small, skipping board plane extraction");
        }
        else
        {
            while (cloud_filtered->size() > 100 || board_cnt < 2)
            {   
                seg.setInputCloud(cloud_filtered);
                seg.segment(*inliers, coefficients);
                extract.setInputCloud(cloud_filtered);
                extract.setIndices(inliers);
                if (inliers->indices.size() == 0)
                {
                    RCLCPP_INFO(this->get_logger(), "did not find any plane.");
                    break;
                }else if(inliers->indices.size() < m_boardPoints_min || inliers->indices.size() > m_boardPoints_max)
                {
                    RCLCPP_INFO(this->get_logger(), "Found a plane, but it doesn't match the size of basketball board.");
                    if (inliers->indices.size() != cloud_filtered->size())
                    {    
                        pcl::PointCloud<PCLPoint> cloud_out;
                        extract.setNegative(true);
                        extract.filter(cloud_out);
                        *cloud_filtered = cloud_out;
                    }
                    else
                    {    
                        cloud_filtered->points.clear();
                    }
                    continue;
                }else{
                    if (std::abs(coefficients.values.at(3)) > m_boardFilterPlaneDistance_low && std::abs(coefficients.values.at(2)) - 0.1 < 1e-6 )
                    {
                        RCLCPP_INFO(this->get_logger(), "Board plane found: %zu/%zu inliers. Coeff: %f %f %f %f",
                            inliers->indices.size(), cloud_filtered->size(),
                            coefficients.values.at(0), coefficients.values.at(1),
                            coefficients.values.at(2), coefficients.values.at(3));

                        extract.setNegative(false);
                        pcl::PointCloud<PCLPoint> cloud_out;
                        extract.filter(cloud_out);
                        *pc_board += cloud_out;

                        clouds.emplace_back(coefficients.values, cloud_out);
                        board_cnt++;

                        if (inliers->indices.size() != cloud_filtered->size())
                        {
                            extract.setNegative(true);
                            cloud_out.clear();
                            extract.filter(cloud_out);
                            *cloud_filtered = cloud_out;
                        }
                        else
                        {    
                            cloud_filtered->points.clear();
                        }
                    }
                    else
                    {
                        RCLCPP_INFO(this->get_logger(), "Horizontal plane (not board) found: %zu/%zu inliers. Coeff: %f %f %f %f",
                            inliers->indices.size(), cloud_filtered->size(),
                            coefficients.values.at(0), coefficients.values.at(1),
                            coefficients.values.at(2), coefficients.values.at(3));

                        if (inliers->indices.size() != cloud_filtered->size())
                        {    
                            pcl::PointCloud<PCLPoint> cloud_out;
                            extract.setNegative(true);
                            extract.filter(cloud_out);
                            *cloud_filtered = cloud_out;
                        }
                        else
                        {    
                            cloud_filtered->points.clear();
                        }
                        
                    }
                    RCLCPP_INFO(this->get_logger(), "cloud_filtered size: %zu", cloud_filtered->size());
                }
            }
        }
        

        // pcl::PointCloud<pcl::PointXYZ>::Ptr filtered_cloud(new pcl::PointCloud<pcl::PointXYZ>);
        // pcl::ApproximateVoxelGrid<pcl::PointXYZ> avf;
        // avf.setInputCloud(pc_board);         // 输入点云
        // avf.setLeafSize(0.08, 0.08, 0.08);// 最小体素的边长
        // avf.filter(*filtered_cloud);      // 进行滤波

        // pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_output(new pcl::PointCloud<pcl::PointXYZ>);
        // // 创建滤波器，对每个点分析的临近点的个数设置为50 ，并将标准差的倍数设置为1  这意味着如果一
        // // 个点的距离超出了平均距离一个标准差以上，则该点被标记为离群点，并将它移除，存储起来
        // pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
        // sor.setInputCloud(pc_board);  // 设置待滤波的点云
        // sor.setMeanK(50);          // 设置在进行统计时考虑查询点邻近点数
        // sor.setStddevMulThresh(1); // 设置判断是否为离群点的阈值，里边的数字表示标准差的倍数，1个标准差以上就是离群点。
        // // 即：当判断点的k近邻平均距离(mean distance)大于全局的1倍标准差+平均距离(global distances mean and standard)，则为离群点。
        // sor.filter(*cloud_output); // 存储内点
        bool foundBoards = false;
        pcl::PointCloud<pcl::PointXYZRGB> coloredCloud_i,coloredCloud_j;
        coloredCloud_i.clear();
        coloredCloud_j.clear();
        RCLCPP_ERROR(this->get_logger(), "COME TO SAMPLE!");
        if(clouds.empty()){
            RCLCPP_ERROR(this->get_logger(), "No board!");
            lidar_cloud_time_buffer_.pop_front();
            return;
        }
        else if(clouds.size() == 1){
            PCLPointCloud repeat = clouds[0].second;
            coloredCloud_j.width = repeat.width;
            coloredCloud_j.height = repeat.height;
            coloredCloud_j.points.resize(coloredCloud_j.width * coloredCloud_j.height);
            for (size_t i = 0; i < repeat.points.size(); ++i) {
                coloredCloud_j.points[i].x = repeat.points[i].x;
                coloredCloud_j.points[i].y = repeat.points[i].y;
                coloredCloud_j.points[i].z = repeat.points[i].z;
                coloredCloud_j.points[i].r = 100; 
                coloredCloud_j.points[i].g = 100;  
                coloredCloud_j.points[i].b = 0;   
            }
            sensor_msgs::msg::PointCloud2 cloud_filtered_msg;
            pcl::toROSMsg(coloredCloud_j, cloud_filtered_msg);
            publish_segment_cloud(cloud_filtered_msg, cloud_filtered_msg);
            RCLCPP_WARN(this->get_logger(), "Only one board!");
        }else{
            for (size_t i = 0; i < clouds.size(); ++i) {
                for (size_t j = i + 1; j < clouds.size(); ++j) {
                    if (areBoards(clouds[i].first, clouds[j].first)) {
                        double distance_i = std::abs(clouds[i].first[3]) / std::sqrt(clouds[i].first[0]*clouds[i].first[0] + clouds[i].first[1]*clouds[i].first[1] + clouds[i].first[2]*clouds[i].first[2]);
                        double distance_j = std::abs(clouds[j].first[3]) / std::sqrt(clouds[j].first[0]*clouds[j].first[0] + clouds[j].first[1]*clouds[j].first[1] + clouds[j].first[2]*clouds[j].first[2]);
                        PCLPointCloud cloud_i, cloud_j;
                        
                        if(distance_i < distance_j){
                            PCLPointCloud cloud_i = clouds[i].second;
                            coloredCloud_i.width = cloud_i.width;
                            coloredCloud_i.height = cloud_i.height;
                            coloredCloud_i.points.resize(coloredCloud_i.width * coloredCloud_i.height);
                            for (size_t i = 0; i < cloud_i.points.size(); ++i) {
                                coloredCloud_i.points[i].x = cloud_i.points[i].x;
                                coloredCloud_i.points[i].y = cloud_i.points[i].y;
                                coloredCloud_i.points[i].z = cloud_i.points[i].z;
                                coloredCloud_i.points[i].r = 255; 
                                coloredCloud_i.points[i].g = 0;  
                                coloredCloud_i.points[i].b = 0;   
                            }

                            PCLPointCloud cloud_j = clouds[j].second;
                            coloredCloud_j.width = cloud_j.width;
                            coloredCloud_j.height = cloud_j.height;
                            coloredCloud_j.points.resize(coloredCloud_j.width * coloredCloud_j.height);
                            for (size_t i = 0; i < cloud_j.points.size(); ++i) {
                                coloredCloud_j.points[i].x = cloud_j.points[i].x;
                                coloredCloud_j.points[i].y = cloud_j.points[i].y;
                                coloredCloud_j.points[i].z = cloud_j.points[i].z;
                                coloredCloud_j.points[i].r = 0; 
                                coloredCloud_j.points[i].g = 0;  
                                coloredCloud_j.points[i].b = 255;   
                            }
                        }else{
                            cloud_i = clouds[i].second;
                            coloredCloud_i.width = cloud_i.width;
                            coloredCloud_i.height = cloud_i.height;
                            coloredCloud_i.points.resize(coloredCloud_i.width * coloredCloud_i.height);
                            for (size_t i = 0; i < cloud_i.points.size(); ++i) {
                                coloredCloud_i.points[i].x = cloud_i.points[i].x;
                                coloredCloud_i.points[i].y = cloud_i.points[i].y;
                                coloredCloud_i.points[i].z = cloud_i.points[i].z;
                                coloredCloud_i.points[i].r = 0; 
                                coloredCloud_i.points[i].g = 0;  
                                coloredCloud_i.points[i].b = 255;   
                            }

                            cloud_j = clouds[j].second;
                            
                            coloredCloud_j.width = cloud_j.width;
                            coloredCloud_j.height = cloud_j.height;
                            coloredCloud_j.points.resize(coloredCloud_j.width * coloredCloud_j.height);
                            for (size_t i = 0; i < cloud_j.points.size(); ++i) {
                                coloredCloud_j.points[i].x = cloud_j.points[i].x;
                                coloredCloud_j.points[i].y = cloud_j.points[i].y;
                                coloredCloud_j.points[i].z = cloud_j.points[i].z;
                                coloredCloud_j.points[i].r = 255; 
                                coloredCloud_j.points[i].g = 0;  
                                coloredCloud_j.points[i].b = 0;   
                            }
                        }
                    
                        foundBoards = true;
                        RCLCPP_INFO(this->get_logger(), "Find boards!");
                        sensor_msgs::msg::PointCloud2 cloud_filtered_msg_i;
                        pcl::toROSMsg(coloredCloud_i, cloud_filtered_msg_i);
                        sensor_msgs::msg::PointCloud2 cloud_filtered_msg_j;
                        pcl::toROSMsg(coloredCloud_j, cloud_filtered_msg_j);
                        publish_segment_cloud(cloud_filtered_msg_i, cloud_filtered_msg_j);
                        break;
                    }
                }
                if(foundBoards) break;
                else RCLCPP_ERROR(this->get_logger(), "No board!");
            }
           
        }
        RCLCPP_ERROR(this->get_logger(), "OVER!");
        
    }



    void publish_marker(const Eigen::Matrix4f& transform)
    {
    
        visualization_msgs::msg::Marker marker;
        marker.header.frame_id = "camera_init"; // 以 odom 为参考系
        marker.header.stamp = this->get_clock()->now();
        marker.ns = "registration";
        marker.id = 0;
        marker.type = visualization_msgs::msg::Marker::SPHERE;
        marker.action = visualization_msgs::msg::Marker::ADD;

        // 设置位置
        marker.pose.position.x = 0;
        marker.pose.position.y = 0;
        marker.pose.position.z = 0;

        // 设置标记的大小和颜色
        marker.scale.x = 0.1;
        marker.scale.y = 0.1;
        marker.scale.z = 0.1;
        marker.color.r = 1.0f;
        marker.color.g = 0.0f;
        marker.color.b = 0.0f;
        marker.color.a = 1.0;

        marker.lifetime = rclcpp::Duration::from_seconds(0);

        marker_pub_->publish(marker);
    }



};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BasketRegister>("lidar_process");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}