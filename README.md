# yannic_robot
4DOF Robot Arm with ROS noetic

control 5 joints trhough Moveit and ROS.

IKFast solution is still only Translation3D - might need to write own IK Plugin

When running "roslaunch yannic_robot check_motor.launch" and initiating the communication wih the rosserial, only the joint1 can be moved. Output from it would for example be:
name: 
  - gripper_joint
  - joint1
  - joint2
  - joint3
  - rotate_base
  - 
position: [-1.5707963267948966, -0.6981270402658242, -1.5707963267948966, -1.5707963267948966, -1.5707963267948966]
velocity: [0.0, 1e-323, -7e-323, 0.0, 0.0]
effort: [-7e-323, 0.0, 0.0, 0.0, 1e-323]

where only the entry for joint1 is variable when I move the servo connected to A0. the other values stay as they are. Joint2 now sometimes goes to negative infinity

new bug: ros.controller_manager: Could not load controller '/yannic_robot/robot_arm_joint_controller' because controller type 'position_controllers/JointTrajectoryController' does not exist.
ros.controller_manager: Use 'rosservice call controller_manager/list_controller_types' to get the available types
