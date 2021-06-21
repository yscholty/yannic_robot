#!/usr/bin/env python3

# will print out the range data to the terminal in which the file is run. Reads range_data topic and saves the value to rangedata. if a distance below a threshold is sensed, the trajectory execution is cancelled. 
# movement will be resumed once the distance sensor is again out of range

# code for array of six sensors


import moveit_commander
import rospy
#import the messages that publish to the range_data topic
from std_msgs.msg import Int16MultiArray
from actionlib_msgs.msg import GoalID
#from moveit_msgs.msg import MoveGroupActionGoal
from geometry_msgs.msg import Pose
from std_msgs.msg import Int16
import time
import numpy as np




threshold = 70
    
rangedata = [1.0,1.0,1.0,1.0,1.0,1.0]

def callback(data):
    global rangedata
    rangedata = data.data

    #rospy.loginfo(rangedata)

def callback_pose(data):
    global position
    position = data.position
    

if __name__ == '__main__':
    group_name = "robot_arm"
    group = moveit_commander.MoveGroupCommander(group_name)

    rospy.init_node('range_data_cancel_movement', anonymous=False)

    rospy.Subscriber("range_data", Int16MultiArray, callback)
    #rospy.Subscriber("/move_group/cancel", GoalID)
    rospy.Subscriber("/goal_position", Pose,callback_pose)

    flag_publisher = rospy.Publisher("/execution_flag",Int16,queue_size=5,latch=True)

    #needed to initialise everything
    time.sleep(1)
    flag = 0

    # in loop to always check with the latest values from rangedata
    while not rospy.is_shutdown():
        #flag = 1 -> motion stop
        #flag = 0 -> motion resume
        #rospy.loginfo(rangedata[0])
        #rospy.loginfo(rangedata[1])
        #rospy.loginfo(rangedata[2])
        
        if ((np.min(rangedata) < threshold) ) : 
            #publish empty message to GoalID -> stop motion  
            group.stop()    
            flag = 1
            #rospy.loginfo(flag)
            flag_publisher.publish(flag)
        elif (np.min(rangedata)>=threshold):
            #rospy.loginfo("resume movement")
            flag=0
            #rospy.loginfo(rangedata[0])
            flag_publisher.publish(flag)



