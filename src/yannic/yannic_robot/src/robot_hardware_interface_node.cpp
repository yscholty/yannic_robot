#include "ros/ros.h"
#include "sensor_msgs/JointState.h"
#include <mutex>
#include <iostream>
#include <map>
#include <vector>

#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/robot_hw.h>
#include <joint_limits_interface/joint_limits.h>
#include <joint_limits_interface/joint_limits_interface.h>
#include <joint_limits_interface/joint_limits_rosparam.h>
#include <joint_limits_interface/joint_limits_urdf.h>
#include <controller_manager/controller_manager.h>
#include <angles/angles.h>
#include <rospy_tutorials/Floats.h>


typedef std::map<std::string, double> MapType;

class ROBOTHardwareInterface  : public hardware_interface::RobotHW  {

public:
  
  ROBOTHardwareInterface() {}

  bool init(ros::NodeHandle &root_nh, ros::NodeHandle &robot_hw_nh){
        joint_state_sub_ = root_nh.subscribe("joint_feedback", 1000, &ROBOTHardwareInterface::arrayCallback, this);
        pub = robot_hw_nh.advertise<sensor_msgs::JointState>("/joints_to_arduino",10);
        controller_manager_.reset(new controller_manager::ControllerManager(this, robot_hw_nh));

        joints_pub.position.clear();
	joints_pub.position.push_back((0 ));
	joints_pub.position.push_back((90 ));
joints_pub.position.push_back((0 ));	
joints_pub.position.push_back((90 ));
joints_pub.position.push_back((90 ));
pub.publish(joints_pub);

        
        num_joints_=5;
	joint_names_[0]="joint1";	
	joint_names_[1]="joint2";
	joint_names_[2]="joint3";
	joint_names_[3]="rotate_base";
	joint_names_[4]="gripper_joint";

for (int i = 0; i < num_joints_; ++i) {

         // Create joint state interface
        hardware_interface::JointStateHandle jointStateHandle(joint_names_[i], &joint_position_state_[joint_names_[i]], &joint_velocity_[i], &joint_effort_[i]);
        joint_state_interface_.registerHandle(jointStateHandle);
        
        registerInterface(&joint_state_interface_);

        // Create position joint interface
        hardware_interface::JointHandle jointPositionHandle(jointStateHandle, &joint_position_command_[i]);
        
        position_joint_interface_.registerHandle(jointPositionHandle);
   
   
   registerInterface(&position_joint_interface_);
    }
    
    return true;
  }

  void read(const ros::Time &time, const ros::Duration &period) {
    std::unique_lock<std::mutex> lck(joint_mtx_);
    for (const auto name : joint_names_) {
      ROS_INFO_STREAM(name << " " << angles::to_degrees(joint_position_state_[name]));
    }
  }
  
  void write(const ros::Time &time, const ros::Duration &period) {
    
	joints_pub.position.clear();
	joints_pub.position.push_back((90 - angles::to_degrees(joint_position_command_[0])));
	joints_pub.position.push_back(( 120 - angles::to_degrees(joint_position_command_[1])));
	joints_pub.position.push_back( ( 90 - angles::to_degrees(joint_position_command_[2])));
	joints_pub.position.push_back(90 - (angles::to_degrees(joint_position_command_[3])));
	joints_pub.position.push_back((angles::to_degrees(joint_position_command_[4])));
	//ROS_INFO("Publishing j1: %.2f, j2: %.2f, j3: %.2f, j4: %.2f, j5: %.2f",joints_pub.data[0],joints_pub.data[1],joints_pub.data[2],joints_pub.data[3],joints_pub.data[4]);
	
	ROS_INFO("Publishing j1: %.2f, j2: %.2f, j3: %.2f, j4: %.2f, j5: %.2f",joints_pub.position[0],joints_pub.position[1],joints_pub.position[2],joints_pub.position[3],joints_pub.position[4]);
	pub.publish(joints_pub);	
}



  void arrayCallback(const sensor_msgs::JointState::ConstPtr &msg) {
    std::unique_lock<std::mutex> lck(joint_mtx_);
    //for (unsigned int i = 0; i < msg->name.size(); i++) {
      joint_position_state_[msg->name[0]] = angles::from_degrees(90 + msg->position[0]);
      joint_position_state_[msg->name[1]] = -(angles::from_degrees( msg->position[1])   );
      joint_position_state_[msg->name[2]] = angles::from_degrees(90+ msg->position[2]);
      joint_position_state_[msg->name[3]] = angles::from_degrees(90 + msg->position[3]);
      joint_position_state_[msg->name[4]] = angles::from_degrees(msg->position[4])*1/40;
    
  }

  ~ROBOTHardwareInterface(){

  };

private:

  std::string joint_names_[5];
  std::mutex joint_mtx_;
  int num_joints_;
  ros::Subscriber joint_state_sub_;
  ros::Publisher pub;

  
  double joint_velocity_[5];
  double joint_effort_[5];
  double joint_position_command_[5];
  sensor_msgs::JointState joints_pub;
  boost::shared_ptr<controller_manager::ControllerManager> controller_manager_;

  MapType joint_position_state_;
  
  hardware_interface::JointStateInterface joint_state_interface_;
  hardware_interface::PositionJointInterface position_joint_interface_;
  joint_limits_interface::PositionJointSaturationInterface position_joint_saturation_interface_;
  joint_limits_interface::PositionJointSoftLimitsInterface positionJointSoftLimitsInterface;
        
};

int main(int argc, char **argv) {
  ros::init(argc, argv, "robot_hardware_interface");
  ros::NodeHandle nh;

  

  
  
  
  


  ros::AsyncSpinner spinner(
      2); // 2 threads for controller service and for the Service client used to
          // get the feedback from arduino
  spinner.start();

  ROBOTHardwareInterface ROBOT;
  controller_manager::ControllerManager cm(&ROBOT);
  //controller_manager_.reset(new controller_manager::ControllerManager(ROBOTHardwareInterface, nh));

  bool init_success = ROBOT.init(nh, nh);

  ros::Duration period(1.0 / 10); // 10Hz update rate
  while (ros::ok()) {
    ROS_INFO("Readiiiing");
    ROBOT.read(ros::Time::now(), period);
    
    ROS_INFO("Updatiiiing");

    cm.update(ros::Time::now(), period);
    ROS_INFO("Writiiiiing");
    ROBOT.write(ros::Time::now(), period);
    period.sleep();
  }
  spinner.stop();

  return 0;
}
