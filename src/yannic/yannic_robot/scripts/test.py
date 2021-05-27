#!/usr/bin/env python3

import moveit_commander
import rospy
from geometry_msgs.msg import Pose
from std_msgs.msg import Int16
import time


def callback_flag(data):
    global flag
    flag = data.data
    #print(flag)

def callback_pose(data):
    global position
    position = data.position
    #print(position)

if __name__ == '__main__':
    group_name = "robot_arm"
    group = moveit_commander.MoveGroupCommander(group_name)
   
    rospy.init_node('execute_motion', anonymous=False)
    #position = Pose().position

    rospy.Subscriber("/execution_flag", Int16,callback_flag)
    rospy.Subscriber("/goal_position", Pose,callback_pose)

    #flag = 0
    executed = 0

    time.sleep(1)

    while not rospy.is_shutdown():
        if ((flag == 0)&(executed==0)):
            print("resume execution")
            group.set_position_target([position.x,position.y,position.z])
            #group.set_max_velocity_scaling_factor(1)
            plan = group.go(wait=True)
            group.stop()
            group.clear_pose_targets()
            executed = 1
        elif ((flag == 1) &(executed==1)):
            group.stop()
            rospy.loginfo("stop movement")
            executed = 0

    


