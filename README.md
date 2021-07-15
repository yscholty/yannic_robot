# yannic_robot
4DOF Robot Arm with ROS noetic

control 5 joints through Moveit and ROS.

IKFast solution is Translation3D 

Startup: ->
source ~/yannic_master/devel/setup.bash
roslaunch yannic_robot_moveit_config demo_stop_motion_array.launch


--------->> robot stops when distance threshold is reached and motion will be resumed after clearing the space again

if just the urdf shall be displayed, run:
roslaunch yannic_robot check_urdf.launch


