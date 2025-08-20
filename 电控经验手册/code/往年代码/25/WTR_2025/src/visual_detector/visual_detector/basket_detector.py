#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from std_msgs.msg import Float32MultiArray

import cv2
import numpy as np
from cv_bridge import CvBridge, CvBridgeError

class VisionRecognitionNode(Node):
    def __init__(self):
        super().__init__('vision_recognition_node')

        self.bridge = CvBridge()
        
        self.image_subscriber = self.create_subscription(
            Image,
            '/camera/image_raw',
            self.image_callback,
            10
        )

        self.detections_publisher = self.create_publisher(Float32MultiArray, 'result_msg', 10)

        self.get_logger().info('Vision Recognition Node has been started.')

    def image_callback(self, msg):
        try:
            cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        except CvBridgeError as e:
            self.get_logger().error(f'CvBridge Error: {e}')
            return

       

def main(args=None):
    rclpy.init(args=args)
    node = VisionRecognitionNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    node.destroy_node()
    rclpy.shutdown()
    

if __name__ == '__main__':
    main()
