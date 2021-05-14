#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include <WProgram.h>
#endif

#include <Servo.h> 
#include <ros.h>
#include <std_msgs/UInt16.h>
#include <sensor_msgs/JointState.h>

ros::NodeHandle  nh;
Servo rotate_base;
Servo joint1;
Servo joint2;
Servo joint3;
Servo gripper_joint;

double rotate_base_angle = 0;
double joint1_angle = 0;
double joint2_angle = 0;
double joint3_angle = 0;
double gripper_joint_angle = 0;

// function call -> processing joint_states and controlling servos
void servo_cb(const sensor_msgs::JointState& cmd_msg){
//get the values from joint_states topic
rotate_base_angle = radiansToDegrees(cmd_msg.position[0]);
joint1_angle = radiansToDegrees(cmd_msg.position[1]);
joint2_angle = radiansToDegrees(cmd_msg.position[2]);
joint3_angle = radiansToDegrees(cmd_msg.position[3]);
gripper_joint_angle = radiansToDegrees(50*cmd_msg.position[4]);


//write the values from joint_states to the connected Servos
rotate_base.write(rotate_base_angle);
joint1.write(joint1_angle);
joint2.write(joint2_angle);
joint3.write(joint3_angle);
gripper_joint.write(gripper_joint_angle);
}


//set up the ROS node -> servo_cb is subscriber for joint_states
ros::Subscriber<sensor_msgs::JointState> sub("joint_states", servo_cb);


void setup() {
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.subscribe(sub);

//Define the servos for the joints
  rotate_base.attach(8);
  joint1.attach(9); 
  joint2.attach(10);
  joint3.attach(11);
  gripper_joint.attach(12); 

  delay(1);
  rotate_base.write(90);
  joint1.write(90);
  joint2.write(90);
  joint3.write(90);
  gripper_joint.write(0);

}

void loop() {
  nh.spinOnce();
}

// convert the radians to degree angles for the servo
double radiansToDegrees(float position_radians)
{

  position_radians = position_radians + 1.57;  //shift by 1.57 at least to only have positive values

  return position_radians * 57.2958;

}
