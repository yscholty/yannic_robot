#!/usr/bin/env python3

# will print out the range data to the terminal in which the file is run. Reads range_data topic and saves the value to rangedata. if a distance below a threshold is sensed, the trajectory execution is cancelled. 
# movement will be resumed once the distance sensor is again out of range

import moveit_commander
import rospy
import numpy as np
#import the messages that publish to the range_data topic
from sensor_msgs.msg import Range
from actionlib_msgs.msg import GoalID
from moveit_msgs.msg import MoveGroupActionGoal
from geometry_msgs.msg import Pose
from std_msgs.msg import Float32




threshold = 0.06
    
rangedata = 1.0

def callback(data):
    global rangedata
    rangedata = data.range
    rospy.loginfo(rangedata)

def callback_pose(data):
    global position
    position = data.position
    

if __name__ == '__main__':
    group_name = "robot_arm"
    group = moveit_commander.MoveGroupCommander(group_name)

    rospy.init_node('range_data_cancel_movement', anonymous=False)

    rospy.Subscriber("range_data", Range, callback)
    rospy.Subscriber("/move_group/cancel", GoalID)
    rospy.Subscriber("/goal_position", Pose,callback_pose)

    flag_publisher = rospy.Publisher("/execution_flag",Float32,queue_size=20)

    goalid_prev = Pose()

    
    flag = 0
    # in loop to always check with the latest values from rangedata
    while not rospy.is_shutdown():
        if (rangedata < threshold) : 
            #publish empty message to GoalID -> stop motion
            group.stop()
            rospy.loginfo("stop movement")
            flag = 1
            flag_publisher.publish(flag)
        elif ((flag ==1) & (rangedata>=threshold)):
            rospy.loginfo("resume movement")
           
            
            group.set_position_target([position.x,position.y,position.z])
            #group.set_max_velocity_scaling_factor(1)
            #group.set_pose_target(feedback.pose) #give for 6 or more dof arms
            plan = group.go(wait=True)
            group.stop()
            group.clear_pose_targets()
            flag = 0
    #rospy.spin()

