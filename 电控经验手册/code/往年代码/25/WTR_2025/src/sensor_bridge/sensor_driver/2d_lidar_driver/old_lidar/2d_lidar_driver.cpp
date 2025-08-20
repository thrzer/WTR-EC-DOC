/*
*  3iRoboticsLIDAR System II
*  Driver Interface
*
*  Copyright 2017 3iRobotics
*  All rights reserved.
*
*	Author: 3iRobotics, Data:2017-09-15
*
*/

/********** ******************************
*
*   基于原ros1工程，迁移至ros2 humble
*   Author: Kurihara_mio
*
*****************************************/

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

#include "C3iroboticsLidar.h"
#include "CSerialConnection.h"

#define DEG2RAD(x) ((x)*M_PI/180.)

using namespace std::chrono_literals;


typedef struct _rslidar_data
{
    _rslidar_data()
    {
        signal = 0;
        angle = 0.0;
        distance = 0.0;
    }
    uint8_t signal;
    float   angle;
    float   distance;
}RslidarDataComplete;


using namespace std;
using namespace everest::hwdrivers;

void publish_scan(const rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr pub,
                  _rslidar_data *nodes,
                  size_t node_count, rclcpp::Time start,
                  double scan_time,
                  float angle_min, float angle_max,
                  std::string frame_id)
{
    sensor_msgs::msg::LaserScan scan_msg;

    scan_msg.header.stamp = start;
    scan_msg.header.frame_id = frame_id;
    scan_msg.angle_min = angle_min;
    scan_msg.angle_max = angle_max;
    scan_msg.angle_increment = (scan_msg.angle_max - scan_msg.angle_min) / (360.0f - 1.0f);

    scan_msg.scan_time = scan_time;
    scan_msg.time_increment = scan_time / (double)(node_count-1);
    scan_msg.range_min = 0.15;
    scan_msg.range_max = 5.0;

    scan_msg.ranges.resize(360, std::numeric_limits<float>::infinity());
    scan_msg.intensities.resize(360, 0.0);

    //Unpack data
    for (size_t i = 0; i < node_count; i++)
    {
        size_t current_angle = floor(nodes[i].angle);
        if(current_angle > 360.0)
        {
            printf("Lidar angle is out of range %d\n", (int)current_angle);
            continue;
        }
        float read_value = (float) nodes[i].distance;
        if (read_value < scan_msg.range_min || read_value > scan_msg.range_max)
            scan_msg.ranges[360- 1- current_angle] = std::numeric_limits<float>::infinity();
        else
            scan_msg.ranges[360 -1- current_angle] = read_value;

		float intensities = (float) nodes[i].signal;
		scan_msg.intensities[360 -1- current_angle] = intensities;

	}

    pub->publish(scan_msg);
}

class Delta2BLidarNode : public rclcpp::Node
{
public:
    Delta2BLidarNode() : Node("delta_2b_lidar_node")
    {
        // 读取参数
        this->declare_parameter<std::string>("serial_port", "/dev/ttyUSB0");
        this->declare_parameter<std::string>("frame_id", "laser_link");
        this->get_parameter("serial_port", opt_com_path);
        this->get_parameter("frame_id", frame_id);

        // 创建发布者
        scan_pub = this->create_publisher<sensor_msgs::msg::LaserScan>("scan", 1000);

        // 初始化串口连接和激光雷达
        serial_connect.setBaud(opt_com_baudrate);
        serial_connect.setPort(opt_com_path.c_str());
        if (!serial_connect.openSimple())
        {
            RCLCPP_ERROR(this->get_logger(), "Open serial port %s failed!", opt_com_path.c_str());
            exit(-1);
        }
        RCLCPP_INFO(this->get_logger(), "3iRoboticsLidar connected");

        robotics_lidar.initilize(&serial_connect);
        start_scan_time = this->now();

        // 启动定时器
        timer_ = this->create_wall_timer(
            10us, std::bind(&Delta2BLidarNode::timer_callback, this));
    }

private:
    void timer_callback()
    {
        if(!rclcpp::ok()){
            rclcpp::shutdown();
        }
        TLidarGrabResult result = robotics_lidar.getScanData();
        switch (result)
        {
            case LIDAR_GRAB_SUCESS:
            {
                TLidarScan lidar_scan = robotics_lidar.getLidarScan();
                size_t lidar_scan_size = lidar_scan.getSize();
                std::vector<RslidarDataComplete> send_lidar_scan_data;
                send_lidar_scan_data.resize(lidar_scan_size);
                RslidarDataComplete one_lidar_data;
                for (size_t i = 0; i < lidar_scan_size; i++)
                {
                    one_lidar_data.signal = lidar_scan.signal[i];
                    one_lidar_data.angle = lidar_scan.angle[i];
                    one_lidar_data.distance = lidar_scan.distance[i];
                    send_lidar_scan_data[i] = one_lidar_data;
                }

                float angle_min = DEG2RAD(0.0f);
                float angle_max = DEG2RAD(359.0f);

                auto end_scan_time = this->now();
                auto scan_duration = (end_scan_time - start_scan_time).seconds();
                RCLCPP_INFO(this->get_logger(), "Receive Lidar count %u!", lidar_scan_size);

                // 发布激光雷达扫描数据
                int start_node = 0, end_node = 359;
                publish_scan(scan_pub, send_lidar_scan_data.data(), lidar_scan_size,
                             start_scan_time, scan_duration,
                             angle_min, angle_max,
                             frame_id);

                start_scan_time = end_scan_time;
                break;
            }
            case LIDAR_GRAB_ERRO:
                RCLCPP_ERROR(this->get_logger(), "Lidar grab error!");
                break;
            case LIDAR_GRAB_ELSE:
                RCLCPP_WARN(this->get_logger(), "LIDAR_GRAB_ELSE!");
                break;
            default:
                break;
        }
    }

    CSerialConnection serial_connect;
    C3iroboticsLidar robotics_lidar;

    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_pub;
    rclcpp::TimerBase::SharedPtr timer_;

    int opt_com_baudrate = 115200;
    std::string opt_com_path;
    std::string frame_id;

    rclcpp::Time start_scan_time;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Delta2BLidarNode>());
    rclcpp::shutdown();
    return 0;
}
