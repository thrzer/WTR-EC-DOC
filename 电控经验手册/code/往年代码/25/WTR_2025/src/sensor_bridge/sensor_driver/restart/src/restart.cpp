#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
#include "tf2_eigen/tf2_eigen.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"
using std::placeholders::_1;

class Restart : public rclcpp::Node
{
public:
    Restart():Node("restart_node")
    {
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
        timer_= this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&Restart::timer_callback, this));
    }
private:
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    rclcpp::TimerBase::SharedPtr timer_; 

void timer_callback()
{
    float x(0);
    try {
        auto right_laser_geom_ = tf_buffer_->lookupTransform("camera_init", "body", tf2::TimePointZero);//the first is odom
        x = right_laser_geom_.transform.translation.x;

    } catch (tf2::TransformException &ex) {
        RCLCPP_ERROR(this->get_logger(), "%s", ex.what());
        return;
    }

    int result=3;
    if(x>500)
    {
        result=system("echo 'm' | sudo -S systemctl restart fast_lio2.service");
    }
    if(result==0)
    {
        RCLCPP_INFO(this->get_logger(),"restart successful");
    }
    else
    {
        RCLCPP_INFO(this->get_logger(),"damn");
    }
}   
};

int main(int argc,char **argv)
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<Restart>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
