import moveit_commander
import moveit_msgs.msg
from geometry_msgs.msg import Pose
import rospy
import tf

rospy.init_node("Controller_Init")
moveit_commander.roscpp_initialize(sys.argv)
robot = moveit_commander.RobotCommander()
scene = moveit_commander.PlanningSceneInterface()
group_name = "robot_arm"
group = moveit_commander.MoveGroupCommander(group_name)
group.set_goal_tolerance(0.01)

group.set_position_target([0.2,-0.2,0])
plan = group.go(wait=True)
group.stop()
group.clear_pose_targets()