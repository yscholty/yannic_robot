#include <yannic_robot/robot_hardware_interface.h>
#include "sensor_msgs/JointState.h"

void arrayCallback(const sensor_msgs::JointState::ConstPtr& joint_states);
typedef std::map<std::string, double> MapType;
MapType jointpositions;
MapType jointvelocities;




class listener
{
public:
	void arrayCallback(const sensor_msgs::JointState::ConstPtr& msg);
};



void listener::arrayCallback(const sensor_msgs::JointState::ConstPtr& msg)
{
	MapType::iterator iter = jointpositions.begin();
	for(unsigned int i = 0; i < msg->name.size(); i++)
	{
		
		jointpositions[msg->name[i]] = msg -> position[i];
			}
 
    //double joint_position_[5];
	/*joint_position_[0]=angles::from_degrees(jointpositions[0]);
	joint_position_[1]=angles::from_degrees(jointpositions[1]);
	joint_position_[2]=angles::from_degrees(jointpositions[2]);
	joint_position_[3]=angles::from_degrees(jointpositions[3]);
	joint_position_[4]=angles::from_degrees(jointpositions[4]);*/
	

return;
}



ROBOTHardwareInterface::ROBOTHardwareInterface(){
}

ROBOTHardwareInterface::~ROBOTHardwareInterface() {
}

bool ROBOTHardwareInterface::init(ros::NodeHandle& root_nh, ros::NodeHandle& robot_hw_nh) {
    
    /*
    num_joints_=5;
	joint_names_[0]="joint1";	
	joint_names_[1]="joint2";
	joint_names_[2]="joint3";
	joint_names_[3]="rotate_base";
	joint_names_[4]="gripper_joint";*/
	
	//get joint names and num of joint
    robot_hw_nh.getParam("joints", joint_name);
    num_joints = joint_name.size();

    //resize vectors
    joint_position_state.resize(num_joints);
    joint_velocity_state.resize(num_joints);
    joint_effort_state.resize(num_joints);
    joint_position_command.resize(num_joints);
    
    joints_pub.resize(num_joints);
	

    for (int i = 0; i < num_joints; ++i) {

        // Create joint state interface
        hardware_interface::JointStateHandle jointStateHandle(joint_name[i], &joint_position_state[i], &joint_velocity_state[i], &joint_effort_state[i]);
        joint_state_interface.registerHandle(jointStateHandle);

        // Create position joint interface
        hardware_interface::JointHandle jointPositionHandle(jointStateHandle, &joint_position_command[i]);
        
        position_joint_interface.registerHandle(jointPositionHandle);
   
    }
    registerInterface(&joint_state_interface);
    registerInterface(&position_joint_interface);
    
    return true;
}


void ROBOTHardwareInterface::read(const ros::Time& time, const ros::Duration& period) {

    //get the values from the topic "joint_states"  that were made accessibkle through the class method
    // joint_states_sub is the object for the class listener
    
   //sub = nh_.subscribe("joint_states", 100, arrayCallback); 
   //joint_position_state = jointpositions;
   //ROS_INFO("%.2f",joint_states_sub.jointpositions);
   //ROS_INFO("Receiving  j1: %.2f, j2: %.2f, j3: %.2f, j4: %.2f,j5: %.2f",joint_position_state[0],joint_position_state[1], joint_position_state[2] , joint_position_state[3],joint_position_state[4]);
   
}


void ROBOTHardwareInterface::write(const ros::Time& time, const ros::Duration& period) {
    
	//joints_pub.data.clear();
	//joints_pub[0] = 90+(angles::to_degrees(joint_position_command[0]));
	/*joints_pub.data.push_back(90+(angles::to_degrees(joint_position_command[0])));
	joints_pub.data.push_back(90+(angles::to_degrees(joint_position_command[1])));
	joints_pub.data.push_back(90+(angles::to_degrees(joint_position_command[2])));
	joints_pub.data.push_back(90+(angles::to_degrees(joint_position_command[3])));
	joints_pub.data.push_back(90+(angles::to_degrees(joint_position_command[4])));*/
	//ROS_INFO("Publishing j1: %.2f, j2: %.2f, j3: %.2f,j4: %.2f",joints_pub.data[0],joints_pub.data[1],joints_pub.data[2], joints_pub.data[3]);
	//pub.publish(joints_pub);	
}



int main(int argc, char** argv)
{
    ros::init(argc, argv, "robot_hardware_interface");
    ros::NodeHandle nh;
    
    //the subscriber to the joint_statres topic
    listener joint_states_sub;
    ros::Subscriber sub = nh.subscribe("joint_states",1000, &listener::arrayCallback, &joint_states_sub);
    //ros::Publisher pub;
    
    /* nodes defined in the Arduino code */
	//pub = nh.advertise<rospy_tutorials::Floats>("/joints_to_arduino",10);
    //ros::Subscriber sub;
    ros::AsyncSpinner spinner(2);// 2 threads for controller service and for the Service client used to get the feedback from arduino
    spinner.start();
    
    ROBOTHardwareInterface ROBOT;
    bool init_success = ROBOT.init(nh,nh);

    
    //ros::spin();
    
     controller_manager::ControllerManager cm(&ROBOT,nh);
     
    ros::Duration period(1.0/10); // 10Hz update rate
    while(ros::ok()){
        ROBOT.read(ros::Time::now(), period);
        	ROS_INFO("Readiiiing");
        cm.update(ros::Time::now(), period);
        	ROS_INFO("controlllling");
        ROBOT.write(ros::Time::now(), period);
        	ROS_INFO("writiiing");
        period.sleep();
        }
        spinner.stop();
        
    return 0;
}

