# yannic_robot
4DOF Robot Arm with ROS noetic

control 5 joints through Moveit and ROS.

Startup: ->
catkin build
source ~/yannic_master/devel/setup.bash
roslaunch yannic_robot_moveit_config demo_stop_motion_array.launch 

What it does: 
Move red marker with mouse and right click on it. in the context menu select the action as wanted. for example execute motion. Robot will try to reach the desired marker point. If an object is entering the safe space of the distance sensors, the motion will stop. once removing this object again, the motion will be resumed





--------->> robot stops when distance threshold is reached and motion will be resumed after clearing the space again


ToDO:
still no feedback from servos but fake_joint_states
edit velocity of the robot motions using the distance sensors
