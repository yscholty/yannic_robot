#!/usr/bin/env python3

import moveit_commander
import rospy
import numpy as np
#import the messages that publish to the range_data topic
from sensor_msgs.msg import Range
from actionlib_msgs.msg import GoalID
from moveit_msgs.msg import MoveGroupActionGoal
from geometry_msgs.msg import Pose
from std_msgs.msg import Float32
flag = 1
def callback_flag(data):
    global flag
    flag = data.data
    #print(flag)

rospy.init_node('execute_motion', anonymous=False)

group_name = "robot_arm"
group = moveit_commander.MoveGroupCommander(group_name)

rospy.Subscriber("/execution_flag", Float32,callback_flag)
while(1):
    print(flag)
    if (flag == 0):
        group.set_position_target([position.x,position.y,position.z])
        #group.set_max_velocity_scaling_factor(1)
        plan = group.go(wait=True)
        group.stop()
        group.clear_pose_targets()
        flag = 1
        


