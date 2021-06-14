roslaunch yannic_robot_moveit_config demo_stop_motion.launch:

marker can be set and motion will be stopped when ONE sensors reads a value smaller than threshold.

roslaunch yannic_robot_moveit_config demo_stop_motion_array.launch:

does the same but for an array of sensors of three sensors. Be careful different ros message.


TODO:
Make available for any size of arrays withouth modifying the code.
