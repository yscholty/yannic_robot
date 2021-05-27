# yannic_robot
4DOF Robot Arm with ROS noetic

control 5 joints through Moveit and ROS.

IKFast solution is still only Translation3D - might need to write own IK Plugin

Startup: ->
source ~/yannic_master/devel/setup.bash
roslaunch yannic_robot_moveit_config demo.launch

"go to scripts in yannic_robot" ->
./yannic_test.py
"new terminal" ->
./test.py

--------->> robot stops when distance threshold is reached and motion will be resumed after clearing the space again


ToDO:
still no feedback from servos but fake_joint_states
edit velocity of the robot motions using the distance sensors
