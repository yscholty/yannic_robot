#!/usr/bin/env python3

import moveit_commander
import rospy
import numpy as np
#import the messages that publish to the range_data topic
from sensor_msgs.msg import Range
from actionlib_msgs.msg import GoalID
from moveit_msgs.msg import MoveGroupActionGoal

group_name = "robot_arm"
group = moveit_commander.MoveGroupCommander(group_name)

group.set_position_target([-0.2,0.1,0.016])
#group.set_pose_target(feedback.pose) #give for 6 or more dof arms
plan = group.go(wait=True)
group.stop()
group.clear_pose_targets()