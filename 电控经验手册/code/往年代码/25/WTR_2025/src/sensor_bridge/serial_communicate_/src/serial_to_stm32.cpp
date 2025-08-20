#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include "serial/serial.h"
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/point.hpp" // 引入正确的头文件
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
#include "tf2_eigen/tf2_eigen.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
#include <stdlib.h>
#include "window_filter.h"
using std::placeholders::_1;

serial::Serial ros_serial;

float test_send_buffer[2] = {66, 77};
static bool flag_x=true;
static bool flag_y=true;
// int count=0;

class SerialToSTM32 : public rclcpp::Node
{
public:
  SerialToSTM32()
      : Node("serial_to_stm32")
  {
    tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    // 假设位置数据（x, y）通过 "location" 话题传输，使用 Point 类型消息
    /*sub_ = this->create_subscription<geometry_msgs::msg::Point>(
        "location", 10, std::bind(&SerialToSTM32::topic_callback, this, _1));*/ // 创建订阅location话题的订阅者
    camera_sub_ = this->create_subscription<std_msgs::msg::Float32MultiArray>(
        "camera_xyz", 10, std::bind(&SerialToSTM32::camera_callback, this, _1)); 
    // 创建定时器，每0.5秒更新test_send_buffer并发送
    timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&SerialToSTM32::timer_callback, this));
  }

private:
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

  rclcpp::Subscription<geometry_msgs::msg::Point>::SharedPtr sub_;
  rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr camera_sub_;

  rclcpp::TimerBase::SharedPtr timer_; // 定时器
  float camera_x = 0;
  float camera_y = 0;
  float camera_z = 0;

  int result=3;

  uint16_t compute_crc16(const uint8_t* data, size_t length) 
  {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; ++j)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
    }
    return crc;
  }

  void camera_callback(const std_msgs::msg::Float32MultiArray::SharedPtr msg) {
    camera_x = msg->data[0];
    camera_y = msg->data[1];
    camera_z = msg->data[2];
  }
  /*void topic_callback(const geometry_msgs::msg::Point::SharedPtr msg) const
{
    float x = msg->x;
    float y = msg->y;
    
    // 将两个 float 拆解成字节数组
    uint8_t buffer[10] = {0xAA, 0x55, 0};  // 头帧
    memcpy(buffer + 2, &x, 4);
    memcpy(buffer + 6, &y, 4);

    // 打印调试信息
    std::cout << "Sending float data: x = " << x << ", y = " << y << std::endl;
    for (int i = 0; i < 8; i++) {
        std::cout << std::hex << (buffer[i] & 0xff) << " ";
    }
    std::cout << std::endl;

    if (ros_serial.isOpen()) {
        ros_serial.write(buffer, sizeof(buffer));
    } else {
        RCLCPP_ERROR(this->get_logger(), "Serial port not open. Skipping write.");
    }
}*/


void timer_callback()
{
  static int cnt = 0;
  if(cnt == 30){
    RCLCPP_INFO(this->get_logger(), "Serial port is working...");
    cnt = 0;
  }
  if (!ros_serial.isOpen()) {
    try {
      ros_serial.open();
      RCLCPP_INFO(this->get_logger(), "Serial port reconnected.");
    } catch (const serial::IOException& e) {
      RCLCPP_WARN_THROTTLE(this->get_logger(), *get_clock(), 5000, "Failed to reconnect serial: %s", e.what());
      return;  // 跳过本次定时器执行
    }
  }

  float x(0), y(0), yaw_(0);
  try {
      auto right_laser_geom_ = tf_buffer_->lookupTransform("camera_init", "body", tf2::TimePointZero);//the first is odom second=base_link
      
      x = right_laser_geom_.transform.translation.x;
      y = right_laser_geom_.transform.translation.y;
      // 从四元数中获取 yaw
      tf2::Quaternion q(
        right_laser_geom_.transform.rotation.x,
        right_laser_geom_.transform.rotation.y,
        right_laser_geom_.transform.rotation.z,
        right_laser_geom_.transform.rotation.w);

      double roll, pitch, yaw;
      tf2::Matrix3x3(q).getRPY(roll, pitch, yaw);
      yaw_ = yaw;

  } catch (tf2::TransformException &ex) {
      RCLCPP_ERROR(this->get_logger(), "%s", ex.what());
      return;
  }
  //filter the data
  float aver_x=windowfilter_testx(x,119,&flag_x);
  float aver_y=windowfilter_testy(y,119,&flag_y);
  // RCLCPP_INFO(this->get_logger(),"aver_x is : %f   aver_y is : %f",aver_x,aver_y);
  // RCLCPP_INFO(this->get_logger(),"x is : %f   y is : %f",x,y);

  // if(x>500)
  // {
  //   result=system("echo 'm' | sudo -S sudo systemctl stop fast_lio2.service");
  // }
  // if(result==0)
  // {
  //   RCLCPP_INFO(this->get_logger(),"restart service successfully!!!");
  // }else
  // {
  //   RCLCPP_INFO(this->get_logger(),"we have not restart service");
  // }

    // 测试用 float 数据（模拟坐标）
    // float x = test_send_buffer[0];
    // float y = test_send_buffer[1];

    //打印浮点值
    // std::cout << "Sending now location: x = " << x << ", y = " << y << ", yaw = "<< yaw_ << std::endl;
    // std::cout << "Sending basket pose: camera_x = " << camera_x << ", camera_y = " << camera_y << ", camera_z = " << camera_z << std::endl;  
    // 将 float 拆成字节发送
    uint8_t buffer[28] = {0xAA, 0x55, 0};  // 头帧
    memcpy(buffer + 2, &aver_x, 4);
    memcpy(buffer + 6, &aver_y, 4);
    memcpy(buffer + 10, &yaw_, 4);
    memcpy(buffer + 14, &camera_x, 4);
    memcpy(buffer + 18, &camera_y, 4);
    memcpy(buffer + 22, &camera_z, 4);
    // 计算 CRC
    uint16_t crc = compute_crc16(buffer, 26);
    memcpy(buffer + 26, &crc, 2);

    // 打印字节内容用于调试
    // std::cout << "Byte buffer: ";
    // for (int i = 0; i < 26; i++)
    // {
    //     std::cout << std::hex << (buffer[i] & 0xff) << " ";
    // }
    // std::cout << std::endl;

    // 发送数据
    // if (ros_serial.isOpen()) {
    //   ros_serial.write(buffer, sizeof(buffer));
    // }
    try {
      if (ros_serial.isOpen()) {
        ros_serial.write(buffer, sizeof(buffer));
        cnt++;
      } else {
        RCLCPP_WARN(this->get_logger(), "Serial port is not open, skipping write.");
      }
    } catch (const serial::SerialException& e) {
      RCLCPP_ERROR(this->get_logger(), "Serial write failed: %s", e.what());
      ros_serial.close();  // 关闭串口，触发后续重连逻辑
    } catch (const std::exception& e) {
      RCLCPP_ERROR(this->get_logger(), "Unexpected error during serial write: %s", e.what());
      ros_serial.close();
    }
    // 测试值递增
    /*test_send_buffer[0] += 0.01;
    test_send_buffer[1] += 0.01;

    if (test_send_buffer[0] > 1000.0f)
        test_send_buffer[0] = 66.0f;
    if (test_send_buffer[1] > 1000.0f)
        test_send_buffer[1] = 77.0f;*/
}

};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);

  // 初始化串口配置
  ros_serial.setPort("/dev/ttyCH341USB0");//ttyCH341USB0   ttyCH341USB1
  ros_serial.setBaudrate(115200);
  serial::Timeout to = serial::Timeout::simpleTimeout(1000);
  ros_serial.setTimeout(to);

  try
  {
    ros_serial.open();
  }
  catch (serial::IOException &e)
  {
    std::cout << "Unable to open stm32 serial port" << std::endl;
    return -1;
  }

  if (ros_serial.isOpen())
  {
    std::cout << "Serial port opened successfully" << std::endl;
  }
  else
  {
    return -1;
  }

  // 创建并启动 ROS 2 节点
  auto node = std::make_shared<SerialToSTM32>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  ros_serial.close();

  return 0;
}