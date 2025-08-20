#include "communicator_node.h"
#include "std_msgs/msg/byte_multi_array.hpp"
#include <chrono>
#include <mutex>

using namespace std::chrono_literals;

namespace communicator
{

Communicator::Communicator()
: Node("communicator")
{
    std::string port = "/dev/ttyUSB0"; 
    unsigned long baud = 115200;      

    try
    {
        serial_port_.setPort(port);
        serial_port_.setBaudrate(baud);
        serial::Timeout timeout = serial::Timeout::simpleTimeout(1000);
        serial_port_.setTimeout(timeout);
        serial_port_.open();
    }
    catch (serial::IOException &e)
    {
        RCLCPP_ERROR(this->get_logger(), "无法打开串口 %s", port.c_str());
        rclcpp::shutdown();
    }

    if(serial_port_.isOpen())
    {
        RCLCPP_INFO(this->get_logger(), "串口 %s 打开成功", port.c_str());
    }
    else
    {
        RCLCPP_ERROR(this->get_logger(), "无法打开串口 %s", port.c_str());
        rclcpp::shutdown();
    }

    publisher_ = this->create_publisher<std_msgs::msg::ByteMultiArray>("received_data", 10);
    subscription_ = this->create_subscription<std_msgs::msg::ByteMultiArray>(
        "send_data", 10,
        std::bind(&Communicator::topicCallback, this, std::placeholders::_1));

    timer_ = this->create_wall_timer(
        10ms, std::bind(&Communicator::timerCallback, this));
}

void Communicator::timerCallback()
{
    readSerialData();
}

void Communicator::readSerialData()
{
    size_t available = serial_port_.available();
    if (available)
    {
        std::vector<uint8_t> buffer(available);
        size_t bytes_read = serial_port_.read(buffer, available);
        if (bytes_read > 0)
        {
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            read_buffer_.insert(read_buffer_.end(), buffer.begin(), buffer.end());

            std::vector<std::vector<uint8_t>> packets;
            if (unpackData(read_buffer_, packets))
            {
                for (auto &packet : packets)
                {
                    std_msgs::msg::ByteMultiArray msg;
                    msg.data = packet;
                    publisher_->publish(msg);
                }
            }
        }
    }
}

void Communicator::topicCallback(const std_msgs::msg::ByteMultiArray::SharedPtr msg)
{
    std::vector<uint8_t> packed_data = packData(msg->data);
    writeSerialData(packed_data);
}

void Communicator::writeSerialData(const std::vector<uint8_t>& data)
{
    if(serial_port_.isOpen())
    {
        size_t bytes_written = serial_port_.write(data);
        if (bytes_written != data.size())
        {
            RCLCPP_WARN(this->get_logger(), "未能全部写入串口");
        }
    }
    else
    {
        RCLCPP_WARN(this->get_logger(), "串口未打开，无法写入数据");
    }
}

std::vector<uint8_t> Communicator::packData(const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> packet;
    packet.push_back(HEADER_BYTE);
    // 添加数据长度（高字节在前）
    uint16_t length = data.size();
    packet.push_back((length >> 8) & 0xFF);
    packet.push_back(length & 0xFF);

    packet.insert(packet.end(), data.begin(), data.end());

    uint16_t crc = calculateCRC(std::vector<uint8_t>(packet.begin()+1, packet.end()));

    packet.push_back((crc >> 8) & 0xFF);
    packet.push_back(crc & 0xFF);
    return packet;
}

bool Communicator::unpackData(const std::vector<uint8_t>& buffer, std::vector<std::vector<uint8_t>>& packets)
{
    size_t i = 0;
    while (i + 4 <= buffer.size())
    {
        if (buffer[i] != HEADER_BYTE)
        {
            ++i;
            continue;
        }

        if (i + 3 >= buffer.size()) 
        {
            break;
        }

        uint16_t length = (buffer[i+1] << 8) | buffer[i+2];
        size_t packet_size = 1 + 2 + length + 2; 

        if (i + packet_size > buffer.size())
        {
            break; 
        }

        std::vector<uint8_t> data(buffer.begin()+i+3, buffer.begin()+i+3+length);
        uint16_t received_crc = (buffer[i+3+length] << 8) | buffer[i+3+length+1];

        uint16_t calculated_crc = calculateCRC(std::vector<uint8_t>(buffer.begin()+i+1, buffer.begin()+i+3+length));

        if(received_crc == calculated_crc)
        {
            packets.push_back(data);
            i += packet_size;
        }
        else
        {
            RCLCPP_WARN(this->get_logger(), "CRC 校验失败");
            i += 1; 
        }
    }

    read_buffer_.erase(read_buffer_.begin(), read_buffer_.begin() + i);
    return !packets.empty();
}

uint16_t Communicator::calculateCRC(const std::vector<uint8_t>& data)
{
    uint16_t crc = 0xFFFF;
    for(auto byte : data)
    {
        crc ^= byte << 8;
        for(int i=0; i<8; ++i)
        {
            if(crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<=1;
        }
    }
    return crc;
}

} // namespace communicator


int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<communicator::Communicator>());
    rclcpp::shutdown();
    return 0;
}
