// yannic_robot version 1.0
// 4DOF robot arm with servo5 being the gripper actuator
// tested under ROS noetic with Ubuntu 20.04

#include <ros.h>
#include <rospy_tutorials/Floats.h>
#include <yannic_robot/Floats_array.h>
#include <Servo.h> 

ros::NodeHandle  nh;


int cur_pos[5]={0,0,0,0,0};

Servo servo1,servo2,servo3,servo4,servo5;

float readservo1=0,readservo2=0,readservo3=0,readservo4=0,readservo5=0;

void rotate_servo(int servo,int new_pos,int cur_pos,int dir)
{
  int pos = 0;
  
  if (servo==0)
  {
    if (new_pos<5)
      new_pos=5;
    if (dir == 1)
    {
      for(pos=cur_pos;pos<= new_pos;pos += 1)
      {
        servo1.write(pos);
        delay(10);
      }      
    }
    else if(dir == -1)
    {
      for(pos=cur_pos;pos>= new_pos;pos -= 1)
      {
        servo1.write(pos);
        delay(10);
      }      
    }
  }
  if (servo==1)
  {
    if (new_pos<12)
      new_pos=12;
    else if(new_pos > 161)
    {
      //nh.loginfo("Limiting Servo2 to 155");
      new_pos=155;
    }
    
    if (dir == 1)
    {
      for(pos=cur_pos;pos<= new_pos;pos += 1)
      {
        servo2.write(pos);
        delay(10);
      }      
    }
    else if(dir == -1)
    {
      for(pos=cur_pos;pos>= new_pos;pos -= 1)
      {
        servo2.write(pos);
        delay(10);
      }      
    }
  }
  if (servo==2)
  {
    if (new_pos<6)
      new_pos=6;
    if (dir == 1)
    {
      for(pos=cur_pos;pos<= new_pos;pos += 1)
      {
        servo3.write(pos);
        delay(10);
      }      
    }
    else if(dir == -1)
    {
      for(pos=cur_pos;pos>= new_pos;pos -= 1)
      {
        servo3.write(pos);
        delay(10);
      }      
    }
  }

  if (servo==3)
  {
    if (new_pos<20)
      new_pos=20;
    if (dir == 1)
    {
      for(pos=cur_pos;pos<= new_pos;pos += 1)
      {
        servo4.write(pos);
        delay(10);
      }      
    }
    else if(dir == -1)
    {
      for(pos=cur_pos;pos>= new_pos;pos -= 1)
      {
        servo4.write(pos);
        delay(10);
      }      
    }
  }

  if (servo==4)
  {
    if (new_pos<20)
      new_pos=20;
    if (dir == 1)
    {
      for(pos=cur_pos;pos<= new_pos;pos += 1)
      {
        servo5.write(pos);
        delay(10);
      }      
    }
    else if(dir == -1)
    {
      for(pos=cur_pos;pos>= new_pos;pos -= 1)
      {
        servo5.write(pos);
        delay(10);
      }      
    }
  }
  
}


void servo_cb( const rospy_tutorials::Floats& cmd_msg){
  //nh.loginfo("Command Received");
  
  int new_pos[5]={cmd_msg.data[0],cmd_msg.data[1],cmd_msg.data[2],cmd_msg.data[3],cmd_msg.data[4]};
  int i=0;
  
  for(i=0;i<5;i++)
  {
    if (new_pos[i]>cur_pos[i])
    {
      rotate_servo(i,new_pos[i],cur_pos[i],1);
      cur_pos[i]=new_pos[i];
        
    }
    else if(new_pos[i]<cur_pos[i])
    {
      rotate_servo(i,new_pos[i],cur_pos[i],-1);
      cur_pos[i]=new_pos[i];
    }
    
  }
  
}

void callback(const yannic_robot::Floats_array::Request & req, yannic_robot::Floats_array::Response & res)
{
  // Simulate function running for a non-deterministic amount of time
  

  res.res_length=5;
  readservo1=analogRead(A0);
  res.res[0]=(readservo1-90) * (180.0/270.0);
  readservo2=analogRead(A1);
  res.res[1]=(readservo2-90) * (180.0/270.0);
  readservo3=analogRead(A2);
  res.res[2]=(readservo3-90) * (180.0/270.0);  
  readservo4=analogRead(A3);
  res.res[3]=(readservo4-90) * (180.0/270.0);  //to be determined
  readservo5=analogRead(A4);
  res.res[4]=(readservo5-90) * (180.0/270.0); 
  return;
  
}



ros::Subscriber<rospy_tutorials::Floats> sub("/joints_to_arduino", servo_cb);
ros::ServiceServer<yannic_robot::Floats_array::Request, yannic_robot::Floats_array::Response> server("/read_joint_state",&callback);


void setup(){

  nh.initNode();
  nh.subscribe(sub);
  nh.advertiseService(server);
  
  servo1.attach(6); //servo1
  servo2.attach(9); //servo2
  servo3.attach(11); //servo3
  servo4.attach(10); //servo4
  servo5.attach(5); //servo5 = gripper
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  servo5.write(0);
  
  
}

void loop(){

  nh.spinOnce();
}
