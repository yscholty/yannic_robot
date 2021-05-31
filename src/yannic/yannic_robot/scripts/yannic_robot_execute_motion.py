#!/usr/bin/env python3

import moveit_commander
import rospy
from geometry_msgs.msg import Pose
from std_msgs.msg import Int16
from sensor_msgs.msg import Range
import time


def callback_flag(data):
    global flag
    flag = data.data
    #print(flag)

def callback_pose(data):
    global position
    position = data.position
    #print(position)

def callback_range(data):
    global rangedata
    rangedata = data.range
    #rospy.loginfo(rangedata)

if __name__ == '__main__':
    group_name = "robot_arm"
    group = moveit_commander.MoveGroupCommander(group_name)
   
    rospy.init_node('execute_motion', anonymous=False)
    #position = Pose().position

    rospy.Subscriber("/execution_flag", Int16,callback_flag)
    rospy.Subscriber("/goal_position", Pose,callback_pose)
    rospy.Subscriber("range_data", Range, callback_range)

    try:
        position
    except NameError:
        print("set initial position")
        position = Pose().position
        position.x = -0.17
        position.y = 0.13
        position.z = 0.02


    flag = 0
    executed = 0

    time.sleep(1)

    while not rospy.is_shutdown():
        if ((flag == 0)):
            print("resume execution")
            group.set_position_target([position.x,position.y,position.z])
            group.set_max_velocity_scaling_factor(0.5)
            plan = group.go(wait=True)
            group.stop()
            group.clear_pose_targets()
            executed = 1
        elif ((flag == 1) ):
            rospy.loginfo("stop movement")
            executed = 0
            group.stop() 

    


