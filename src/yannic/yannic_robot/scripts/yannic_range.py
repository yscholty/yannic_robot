#!/usr/bin/env python3

# will print out the range data to the terminal in which the file is run. Reads range_data topic and saves the value to rangedata

import rospy
#import the messages that publish to the range_data topic
from sensor_msgs.msg import Range


def callback(data):
    rangedata = data.range
    rospy.loginfo(rangedata)
    
def listener():
    rospy.init_node('range_data_subscriber', anonymous=False)

    rospy.Subscriber("range_data", Range, callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()

