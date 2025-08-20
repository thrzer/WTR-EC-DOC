#from pymavlink import mavutil
import serial
import serial.tools.list_ports
from time import sleep
from .msg_define import *
from rclpy.node import Node
# from geometry_msgs.msg import Pose2D
from std_msgs.msg import UInt64
from sensor_msgs.msg import Joy
from threading import Thread
import rclpy
import os
from typing import Callable


class topic2Mavlink():
    __slots__ = ['sub','topictype','topicname','transfromFunc']
class Mavlink2topic():
    __slots__ = ['mavtypename','pub','transfromFunc']


# def mav_pose_to_pose2d(msgin:MAVLink_pose_message)->Pose2D:
#     msgout = Pose2D()
#     msgout.x = msgin.x
#     msgout.y = msgin.y
#     msgout.theta = msgin.w
#     return msgout

# def pose2d_to_mav_speed(msgin:Pose2D)->MAVLink_speed_message:
#     return MAVLink_speed_message(msgin.x,msgin.y,msgin.theta)

# def upcmd_to_mav_up_control(msgin:UInt32)->MAVLink_up_control_message:
#     return MAVLink_up_control_message(4)
def mav_heartbeat2u64(msgin:MAVLink_heartbeat_message)->UInt64:
    msgout =UInt64()
    msgout.data=msgin.time
    return msgout
def joy_to_mav_speed(msgin:Joy)->MAVLink_speed_message:
    Kx=1.0
    Ky=1.0
    Kz=1.0
    x_val = Kx*msgin.axes[1]
    y_val = Ky*msgin.axes[0]
    z_val = Kz*msgin.axes[3]
    # print(f'x:{x_val},y:{y_val},z:{z_val}')
    return MAVLink_speed_message(x_val,y_val,z_val)

class Mav_ros_bridge(Node):
    def __init__(self,name:str):
        super().__init__(name)
        self.serial_port = self.try_open_serial()
        self.Mav = MAVLink(self.serial_port)
        #使mavlink接收不完整信息时返回None而不是raise MAVError("invalid MAVLink prefix '%s'" % magic)
        self.Mav.have_prefix_error =True
        self.mav_to_topic_list = []
        self.topic_to_mav_list = []
        mav_revice_thread = Thread(target=self.mav_revice_thread)
        mav_revice_thread.start()

    def try_open_serial(self)->serial.Serial:
        try_open_cnt = 0
        while try_open_cnt<5:
            try:
                # change this to your serial port and baud rate
                ports = list(serial.tools.list_ports.comports())
                if not ports:
                    self.get_logger().error('No serial port foundn\n\n\n')
                    sleep(1)
                    continue
                port2open = [port[0] for port in ports if ('ACM' in port[0] or 'USB' in port[0])]
                self.get_logger().info(f'try to start {port2open} ')
                serial_port = serial.Serial(port2open[0], 115200,timeout=0.5)
                self.get_logger().info('serial start sussessed!')
                return serial_port
            except IndexError:
                self.get_logger().fatal('No serial port foundn\n\n\n')
                sleep(1)
                try_open_cnt+=1
            except OSError as e:
                self.get_logger().fatal(f'port busy\n {e}')
                sleep(1)
                try_open_cnt+=1
        exit()
    
    def add_mav2topic(self,mavtype:str,topictype,topicname:str,
                      transfromFunc:Callable[[MAVLink_message],Any]):
        self.get_logger().info(f'start boardcast mavlink:{mavtype} -> topic:{topicname}')
        obj = Mavlink2topic()
        obj.mavtypename = mavtype
        obj.pub = self.create_publisher(topictype,topicname,3)
        obj.transfromFunc=transfromFunc
        #obj.pub.publish()
        self.mav_to_topic_list.append(obj)
        

    
    def add_topic2mav(self,topictype:Any,topicname:str,
                      transfromFunc:Callable[[Any],MAVLink_message]):
        self.get_logger().info(f'start boardcast topic:{topicname}(type:{topictype}) -> mavlink msg')
        obj = topic2Mavlink()
        obj.topictype = topictype
        obj.topicname = topicname
        #obj.Mavtype = Mavtype
        obj.transfromFunc=transfromFunc
        obj.sub = self.create_subscription(topictype,topicname,self.topic_sub_callback,3)
        self.topic_to_mav_list.append(obj)
        
    
    def topic_sub_callback(self,msgin)->None:
        for t2m in self.topic_to_mav_list:
            if  isinstance(msgin,t2m.topictype):
                mav_msgout = t2m.transfromFunc(msgin)
                #self.Mavconnetion.mav.send(mav_msgout)
                self.get_logger().info(f'mav send {mav_msgout}')
                self.Mav.send(mav_msgout)
                
                return
    
    def mav_revice_thread(self):
        while rclpy.ok():
            # try:
            #msgin = self.Mavconnetion.recv_match(blocking=True)
            msgin = self.Mav.parse_char(self.serial_port.read())
            if isinstance(msgin, MAVLink_message):
                #self.get_logger().info(f'mav recv {msgin._type}')
                for m2t in self.mav_to_topic_list:
                    if msgin._type == m2t.mavtypename:
                        m2t.pub.publish(m2t.transfromFunc(msgin))
            # except MAVError:
            #     self.get_logger().info(f'mavlink error\n\n\n\n\n\n\n\n\n\n\n')


def main():
    
    rclpy.init() # 初始化rclpy
    bridge_node = Mav_ros_bridge('mav_ros_bridge')
    # bridge_node.add_topic2mav(Pose2D,
    #                           'chassis_mv_cmd',
    #                           pose2d_to_mav_speed)
    # bridge_node.add_topic2mav(UInt32,
    #                           'up_cmd',
    #                           upcmd_to_mav_up_control)
    # bridge_node.add_mav2topic('POSE',
    #                           Pose2D,
    #                           'chassis_actual_pose',
    #                           mav_pose_to_pose2d)
    bridge_node.add_mav2topic('HEARTBEAT',
                              UInt64,
                              'chassis_time',
                              mav_heartbeat2u64
                              )
    bridge_node.add_topic2mav(Joy,
                              'joy',
                              joy_to_mav_speed
                              )
    rclpy.spin(bridge_node)
    rclpy.shutdown()
        #connetion = mavutil.mavlink_connection('/dev/ttyUSB0',baud=115200,dialect='')