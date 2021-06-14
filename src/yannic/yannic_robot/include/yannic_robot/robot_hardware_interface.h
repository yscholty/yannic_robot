#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/robot_hw.h>
#include <joint_limits_interface/joint_limits.h>
#include <joint_limits_interface/joint_limits_interface.h>
#include <joint_limits_interface/joint_limits_rosparam.h>
#include <joint_limits_interface/joint_limits_urdf.h>
#include <controller_manager/controller_manager.h>
#include <boost/scoped_ptr.hpp>
#include <ros/ros.h>
#include <rospy_tutorials/Floats.h>
#include <yannic_robot/Floats_array.h>
#include <angles/angles.h>
#include "sensor_msgs/JointState.h"
#include <iostream>


class ROBOTHardwareInterface : public hardware_interface::RobotHW 
{
	public:
        ROBOTHardwareInterface();
        ~ROBOTHardwareInterface();
        bool init(ros::NodeHandle& root_nh, ros::NodeHandle& robot_hw_nh);
        void update(const ros::TimerEvent& e);
        void read(const ros::Time& time, const ros::Duration& period);
        void write(const ros::Time& time, const ros::Duration& period);

        ros::Publisher pub;
        //ros::Subscriber sub;
        
        std::vector<double> joints_pub;
        yannic_robot::Floats_array joint_read;
        sensor_msgs::JointState joint_feedback;
        
    protected:
        ros::NodeHandle nh_;

        //interfaces
    	hardware_interface::JointStateInterface joint_state_interface;
    	hardware_interface::PositionJointInterface position_joint_interface;

        joint_limits_interface::PositionJointSaturationInterface     position_joint_saturation_interface;
        joint_limits_interface::PositionJointSoftLimitsInterface positionJointSoftLimitsInterface;
        
        int num_joints;
        std::vector<std::string> joint_name;
        
        //actual states
    std::vector<double> joint_position_state;
    std::vector<double> joint_velocity_state;
    std::vector<double> joint_effort_state;

    //given setpoints
    std::vector<double> joint_position_command;

    //MyRobot1CPP* robot;
        

               
};
