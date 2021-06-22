//for six  distance sensors connected
//disabled the joint_feedback. -> needed if we use ros_control with the hardware interface

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include <WProgram.h>
#endif

#include <Servo.h> 
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/UInt16.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16MultiArray.h>
#include "Adafruit_VL53L0X.h"
#include <rospy_tutorials/Floats.h>

/////////////////////////////////////////////
//////////range sensors update //////////////
////////////////////////////////////////////

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32
#define LOX4_ADDRESS 0x33
#define LOX5_ADDRESS 0x34
#define LOX6_ADDRESS 0x35
/*
    Reset all sensors by setting all of their XSHUT pins low for delay(10), then set all XSHUT high to bring out of reset
    Keep sensor #1 awake by keeping XSHUT pin high
    Put all other sensors into shutdown by pulling XSHUT pins low
    Initialize sensor #1 with lox.begin(new_i2c_address) Pick any number but 0x29 and it must be under 0x7F. Going with 0x30 to 0x3F is probably OK.
    Keep sensor #1 awake, and now bring sensor #2 out of reset by setting its XSHUT pin high.
    Initialize sensor #2 with lox.begin(new_i2c_address) Pick any number but 0x29 and whatever you set the first sensor to
 */


// set the pins to shutdown

#define SHT_LOX1 6
#define SHT_LOX2 5
#define SHT_LOX3 4
#define SHT_LOX4 3
#define SHT_LOX5 2
#define SHT_LOX6 7


// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox4 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox5 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox6 = Adafruit_VL53L0X();


// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
VL53L0X_RangingMeasurementData_t measure3;
VL53L0X_RangingMeasurementData_t measure4;
VL53L0X_RangingMeasurementData_t measure5;
VL53L0X_RangingMeasurementData_t measure6;

///////////////////////////////////////////////
///////////////////////////////////////////////


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

unsigned long t;

// function call -> processing joint_states and controlling servos

void servo_cb(const sensor_msgs::JointState& cmd_msg){
  //get the values from joint_states topic
  //calibrate position of servos so they match the initial simulation start +/- changes direction.
  
  joint1_angle =  cmd_msg.position[0] ;
  joint2_angle = cmd_msg.position[1];
  joint3_angle =  cmd_msg.position[2] ;
  rotate_base_angle = cmd_msg.position[3];
  gripper_joint_angle = cmd_msg.position[4] ;
  
  //write the values from joint_states to the connected Servos
  rotate_base.write(rotate_base_angle);
  joint1.write(joint1_angle);
  joint2.write(joint2_angle);
  joint3.write(joint3_angle);
  gripper_joint.write(gripper_joint_angle);
  nh.loginfo("arduino writttttten");
}

//-----------------ROS setup-------------------//

//set up the ROS node -> servo_cb is subscriber for joint_states
//ROS setup for the control of the Servos controlled by joint_states
ros::Subscriber<sensor_msgs::JointState> sub("joints_to_arduino", servo_cb);

//set up the publisher of the joints
//ROS setup for the analogRead feedback of the servos to the topic joint_feedback
sensor_msgs::JointState robot_state;
ros::Publisher pub("joint_feedback", &robot_state);
char const robot_id[3] = "arm";
char const *joint_name[5] = {"joint1", "joint2", "joint3", "rotate_base", "gripper_joint"};
float pos[5];
float vel[5];
float eff[5];

float readservo1=0,readservo2=0,readservo3=0,readservo_rotate=0,readservo_gripper=0;

///////////////////Range sensor //////////////////
/////////////////////////////////////////////////
//ROS setup for the range sensor
std_msgs::Int16MultiArray range_msg;
ros::Publisher pub_range( "range_data", &range_msg);

//Initialise the range sensors for the I2C communication
void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  digitalWrite(SHT_LOX4, LOW);    
  digitalWrite(SHT_LOX5, LOW);
  digitalWrite(SHT_LOX6, LOW);

  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  digitalWrite(SHT_LOX3, HIGH);
  digitalWrite(SHT_LOX4, HIGH);
  digitalWrite(SHT_LOX5, HIGH);
  digitalWrite(SHT_LOX6, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  digitalWrite(SHT_LOX4, LOW);
  digitalWrite(SHT_LOX5, LOW);
  digitalWrite(SHT_LOX6, LOW);

  // initiating LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    nh.loginfo("Failed to boot first VL53L0X");
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initiating LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    nh.loginfo("Failed to boot second VL53L0X");
    while(1);
  }
  delay(10);

  // activating LOX3
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  //initiating LOX3
  if(!lox3.begin(LOX3_ADDRESS)) {
    nh.loginfo("Failed to boot third VL53L0X");
    while(1);
  }
  delay(10);

  // activating LOX4
  digitalWrite(SHT_LOX4, HIGH);
  delay(10);

  //initiating LOX4
  if(!lox4.begin(LOX4_ADDRESS)) {
    nh.loginfo("Failed to boot 4th VL53L0X");
    while(1);
  }
  delay(10);

  // activating LOX5
  digitalWrite(SHT_LOX5, HIGH);
  delay(10);

  //initiating LOX5
  if(!lox5.begin(LOX5_ADDRESS)) {
    nh.loginfo("Failed to boot 5th VL53L0X");
    while(1);
  }
  delay(10);

  // activating LOX6
  digitalWrite(SHT_LOX6, HIGH);
  delay(10);

  //initiating LOX6
  if(!lox6.begin(LOX6_ADDRESS)) {
    nh.loginfo("Failed to boot 6th VL53L0X");
    while(1);
  }
}

void read_dual_sensors() {

  //nh.loginfo("dualsensor");
  
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!
  lox3.rangingTest(&measure3, false); // pass in 'true' to get debug data printout!
  lox4.rangingTest(&measure4, false); // pass in 'true' to get debug data printout!
  lox5.rangingTest(&measure5, false); // pass in 'true' to get debug data printout!
  lox6.rangingTest(&measure6, false); // pass in 'true' to get debug data printout!


  // print sensor one reading
  //nh.loginfo("1: ");
  if(measure1.RangeStatus != 4) {     // if not out of range
  //  char result1[8];
  //dtostrf(measure1.RangeMilliMeter, 6, 2, result1);
  //nh.loginfo(result1);
    range_msg.data[0] = measure1.RangeMilliMeter;
  } else {
    range_msg.data[0] = 4000;
    //nh.loginfo("Out of range");
  }
  
  //nh.loginfo(" ");

  // print sensor two reading
  //nh.loginfo("2: ");
  if(measure2.RangeStatus != 4) {
  //char result2[8];
  //dtostrf(measure2.RangeMilliMeter, 6, 2, result2);
  //nh.loginfo(result2);
  range_msg.data[1] = measure2.RangeMilliMeter;

  } else {
    range_msg.data[1] = 4000;
    //nh.loginfo("Out of range");
  }

  //nh.loginfo(" ");

  // print sensor three reading
  //nh.loginfo("3: ");
  if(measure3.RangeStatus != 4) {
  //char result3[8];
  //dtostrf(measure3.RangeMilliMeter, 6, 2, result3);
  //nh.loginfo(result3);
  range_msg.data[2] = measure3.RangeMilliMeter;

  } else {
    range_msg.data[2] = 4000;
    //nh.loginfo("Out of range");
  }

  //nh.loginfo(" ");

  // print sensor four reading
  //nh.loginfo("4: ");
  if(measure4.RangeStatus != 4) {     // if not out of range
    //char result4[8];
  //dtostrf(measure4.RangeMilliMeter, 6, 2, result4);
  //nh.loginfo(result4);
    range_msg.data[3] = measure4.RangeMilliMeter;
  } else {
    range_msg.data[3] = 4000;
    //nh.loginfo("Out of range");
  }
  
  //nh.loginfo(" ");

  // print sensor five reading
  //nh.loginfo("5: ");
  if(measure5.RangeStatus != 4) {
  //char result5[8];
  //dtostrf(measure5.RangeMilliMeter, 6, 2, result5);
  //nh.loginfo(result5);
  range_msg.data[4] = measure5.RangeMilliMeter;

  } else {
    range_msg.data[4] = 4000;
    //nh.loginfo("Out of range");
  }

  //nh.loginfo(" ");

  // print sensor six reading
  //nh.loginfo("6: ");
  if(measure6.RangeStatus != 4) {
  //char result6[8];
  //dtostrf(measure6.RangeMilliMeter, 6, 2, result6);
  //nh.loginfo(result6);
  range_msg.data[5] = measure6.RangeMilliMeter;

  } else {
    range_msg.data[5] = 4000;
    //nh.loginfo("Out of range");
  }

  for (int i = 0; i <= 5; i++) {
    if (abs(range_msg.data[i]) > 200){
      range_msg.data[i] = 200;
    }
  }

}


char dim0_label[] = "range_data";

//////////////////setup///////////////////
void setup() {
  nh.getHardware()->setBaud(250000);
  nh.initNode();
  nh.advertise(pub);
  nh.subscribe(sub);
  nh.advertise(pub_range);

  while (!nh.connected() ){
    nh.spinOnce();
}
/*
  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  pinMode(SHT_LOX3, OUTPUT);
  pinMode(SHT_LOX4, OUTPUT);
  pinMode(SHT_LOX5, OUTPUT);
  pinMode(SHT_LOX6, OUTPUT);
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  digitalWrite(SHT_LOX4, LOW);
  digitalWrite(SHT_LOX5, LOW);
  digitalWrite(SHT_LOX6, LOW);

  setID();
  delay(100);

  
  nh.loginfo("VL53L0X API serial node started");
  nh.loginfo("Read from analogread started -> joint_feedback");
  nh.loginfo("joint_state subscriber set up for servo control <- joint_states");
  
  // fill static range message fields
  range_msg.data_length = 6;
  range_msg.layout.dim = (std_msgs::MultiArrayDimension *)
  malloc(sizeof(std_msgs::MultiArrayDimension) * 6);
  range_msg.layout.dim[0].label = dim0_label;
  range_msg.layout.dim[0].size = 6;
  range_msg.layout.dim[0].stride = 1*6;
  range_msg.layout.data_offset = 0;
  range_msg.data = (int *)malloc(sizeof(int)*6);
  // Fulfill the sensor_msg/JointState msg
  //define the header of the joint_state message
  */
  
  robot_state.header.frame_id = robot_id;
  robot_state.name = joint_name;
  robot_state.name_length = 5;
  robot_state.velocity_length = 5;
  robot_state.position_length = 5;
  robot_state.effort_length = 5;

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

  //read_dual_sensors();
  if (millis()-t > 100) {

  //reading the RANGE sensor and publishing to the corresponding topic
    
    //pub_range.publish(&range_msg);
   

   
  //reading the analog values and saving them to the later on published topic
  readservo1 = analogRead(A0);
  readservo2 = analogRead(A1);
  readservo3 = analogRead(A2);
  readservo_rotate = analogRead(A3);
  readservo_gripper = analogRead(A4);
/*
 joint1 :
 -30  :  140
 -120 :  286

 joint2 :
 -40  : 203
 -120 : 311
 
  joint3 :
 -0   : 102
 -180 : 378

   rotate_base :
 -0   : 98
 -180 : 382

   gripper :
 -0   : 102
 -180 : 378

 angle = (adc_x - adc_0) * 180/(adc_180 - adc_0)
 or https://www.mathepower.com/lineare_funktionen.php
 */

  pos[0]=-((0.616 * readservo1) - (56.3)) ;
  pos[1]=-((0.74 * readservo2) - (110.3)) ;
  pos[2]=-((0.652 * readservo3) - (66.5)) ;  
  pos[3]=-((0.643 * readservo_rotate) - (63)) ;
  pos[4]=(readservo_gripper-95) * (180.0/-152) ; 

  char re[8];
  dtostrf(readservo2, 6, 2, re);
  nh.loginfo(re);

  robot_state.header.stamp = nh.now();
  robot_state.position = pos;
  pub.publish( &robot_state); 
  

    t = millis();
  }
nh.spinOnce();
  
  //delay(100);
}

// convert the radians to degree angles for the servo
double radiansToDegrees(float position_radians)
{

  position_radians = position_radians + 1.57;  //shift by 1.57 at least to only have positive values

  return position_radians * 57.2958;

}
