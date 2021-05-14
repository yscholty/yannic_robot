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
Servo base;
Servo joint1;
Servo joint2;
Servo joint3;
Servo gripper;

double base_angle;
double joint1_angle;
double joint2_angle;
double joint3_angle;

// function call -> processing joint_states and controlling servos
void servo_cb(const sensor_msgs::JointState& cmd_msg){
  //position in Radians
base_angle = cmd_msg.position[0];
joint1_angle = cmd_msg.position[1];
joint2_angle = cmd_msg.position[2];
joint3_angle = cmd_msg.position[3];

// routine to prevent critical positions for the servos -> to be done
  
}


//set up the ROS node -> servo_cb is subscriber for joint_states
ros::Subscriber<sensor_msgs::JointState> sub("joint_states", servo_cb);


void setup() {
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.subscribe(sub);

//Define the servos for the joints
  base.attach(8);
  joint1.attach(9); 
  joint2.attach(10);
  joint3.attach(11);
  gripper.attach(12); 

  delay(1);
  base.write(90);
  joint1.write(90);
  joint2.write(90);
  joint3.write(90);
  gripper.write(0);

}

void loop() {
  nh.spinOnce();
}
