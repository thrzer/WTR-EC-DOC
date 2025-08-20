#pragma once

#include "rclcpp/rclcpp.hpp"
#include <serial/serial.h>
#include <memory>
#include <vector>
#include <mutex>
#include "std_msgs/msg/byte_multi_array.hpp"

namespace communicator
{
class Communicator : public rclcpp::Node
{
public:
    Communicator();
    ~Communicator() = default;

private:
    void timerCallback();

    void readSerialData();

    void topicCallback(const std_msgs::msg::ByteMultiArray::SharedPtr msg);

    void writeSerialData(const std::vector<uint8_t>& data);

    std::vector<uint8_t> packData(const std::vector<uint8_t>& data);

    bool unpackData(const std::vector<uint8_t>& buffer, std::vector<std::vector<uint8_t>>& packets);

    uint16_t calculateCRC(const std::vector<uint8_t>& data);

    serial::Serial serial_port_;

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr publisher_;

    rclcpp::Subscription<std_msgs::msg::ByteMultiArray>::SharedPtr subscription_;

    rclcpp::TimerBase::SharedPtr timer_;

    std::vector<uint8_t> read_buffer_;

    std::mutex buffer_mutex_;

    const uint8_t HEADER_BYTE = 0xAA;
};
} // namespace communicator
