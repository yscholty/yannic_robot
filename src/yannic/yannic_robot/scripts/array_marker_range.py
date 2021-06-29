#!/usr/bin/env python3

# code for array of six sensors

import rospy
import tf
import copy
import time

import moveit_commander
import moveit_msgs.msg
from geometry_msgs.msg import Pose
from math import pi
from std_msgs.msg import String
from moveit_commander.conversions import pose_to_list

from interactive_markers.interactive_marker_server import *
from interactive_markers.menu_handler import *
from visualization_msgs.msg import *
from geometry_msgs.msg import Pose
from std_msgs.msg import Float32


#import the messages that publish to the range_data topic
#from sts_msgs.msg import Int16MultiArray

#rangedata = []

#def callback(data):
#	global rangedata
#	rangedata = data.range
#	rospy.loginfo(rangedata)
    


viapoints=[]

viapoints_marker=MarkerArray()
count =0

# menu_entry_id is the context menu for right click
def processFeedback(feedback):
	#indents are off -> use 4x space for indent
    print('+++++++++++',feedback.menu_entry_id,'+++++++++++++++++++++')
    print(feedback.pose.position)
    if feedback.event_type == InteractiveMarkerFeedback.MENU_SELECT:
        if feedback.menu_entry_id==1:
	        goal_position.publish(feedback.pose)
        	

        	group.set_position_target([feedback.pose.position.x,feedback.pose.position.y,feedback.pose.position.z])
        	plan = group.go(wait=True)
        	group.stop()
        	group.clear_pose_targets()
            
  
        elif feedback.menu_entry_id==2:
            listener.waitForTransform('/endeff','/base_link',rospy.Time(), rospy.Duration(1.0))
            (trans,rot)=listener.lookupTransform('base_link','endeff',rospy.Time())
            print(trans,rot)
			
            feedback.pose.position.x=trans[0]
            feedback.pose.position.y=trans[1]
            feedback.pose.position.z=trans[2]
            feedback.pose.orientation.x=rot[0]
            feedback.pose.orientation.y=rot[1]
            feedback.pose.orientation.z=rot[2]
            feedback.pose.orientation.w=rot[3]
            server.setPose( feedback.marker_name, feedback.pose )
            
        elif feedback.menu_entry_id==4:
            viapoints.append(feedback.pose)
            print('Count: ',len(viapoints))
            marker= Marker()
            marker.header.frame_id = "base_link"
            marker.type=Marker.SPHERE
            marker.action=marker.ADD
            marker.scale.x=0.005
            marker.scale.y=0.005
            marker.scale.z=0.005
            marker.color.r=1
            marker.color.g=1
            marker.color.b=0
            marker.color.a=1.0

            marker.pose=feedback.pose
            
            viapoints_marker.markers.append(marker)
            id=0
            
            for m in viapoints_marker.markers:
            	m.id=id
            	id+=1
            
            
            marker_pub.publish(viapoints_marker)
        
        elif feedback.menu_entry_id==5:
            if len(viapoints)>0:
               (plan, fraction)=group.compute_cartesian_path(viapoints, 0.01, 0.0)
               display_traj=moveit_msgs.msg.DisplayTrajectory()
               display_traj.trajectory_start = robot.get_current_state()
               display_traj.trajectory.append(plan)
               display_traj_pub.publish(display_traj)
               group.execute(plan,wait=True)			
            
        elif feedback.menu_entry_id==6:
            if len(viapoints_marker.markers)>0:
            	viapoints[:]=[]
            	for m in viapoints_marker.markers:
            		m.action=Marker().DELETE
            		marker_pub.publish(viapoints_marker)
    
  

if __name__=="__main__":
	time.sleep(2)
	rospy.init_node("simple_marker")
	
	#rospy.Subscriber("range_data", Range, callback)
	#print(rangedata)
	# goal position for the marker
	goal_position = rospy.Publisher('/goal_position',Pose,queue_size=20,latch=True)

	int_marker = InteractiveMarker()
	menu_handler = MenuHandler()
	listener=tf.TransformListener()
	display_traj_pub=rospy.Publisher('/move_group/display_planned_path',moveit_msgs.msg.DisplayTrajectory,queue_size=20)
	marker_pub= rospy.Publisher('/via_points',MarkerArray,queue_size=20)

	# moveit start

	moveit_commander.roscpp_initialize(sys.argv)
	robot = moveit_commander.RobotCommander()
	scene = moveit_commander.PlanningSceneInterface()
	group_name = "robot_arm"
	group = moveit_commander.MoveGroupCommander(group_name)
	group.set_goal_tolerance(0.01)

	group.set_position_target([0.2,-0.2,0])
	plan = group.go(wait=True)
	group.stop()
	group.clear_pose_targets()

	
	# create an interactive marker server on the topic namespace simple_marker
	server = InteractiveMarkerServer("simple_marker")

	# create an interactive marker for our server

	int_marker.header.frame_id = "base_link"
	int_marker.name = "my_marker"
	int_marker.description = "Simple 1-DOF Control"
	int_marker.scale=0.05


	listener.waitForTransform('/endeff','/base_link',rospy.Time(), rospy.Duration(1.0))
	print(listener.frameExists('base_link'))
	print(listener.frameExists('endeff'))
	(trans,rot)=listener.lookupTransform('base_link','endeff',rospy.Time())
	print(trans,rot)

	int_marker.pose.position.x=trans[0]
	int_marker.pose.position.y=trans[1]
	int_marker.pose.position.z=trans[2]
	int_marker.pose.orientation.x=rot[0]
	int_marker.pose.orientation.y=rot[1]
	int_marker.pose.orientation.z=rot[2]
	int_marker.pose.orientation.w=rot[3]
	

	
	marker= Marker()
	marker.type=Marker.SPHERE
	marker.scale.x=0.01
	marker.scale.y=0.01
	marker.scale.z=0.01
	marker.color.r=1
	marker.color.g=0
	marker.color.b=0
	marker.color.a=1.0
	
	rotate_control = InteractiveMarkerControl()
	rotate_control.name = "moving"
	rotate_control.interaction_mode = InteractiveMarkerControl.MOVE_3D
	rotate_control.always_visible = True
	rotate_control.markers.append(marker)
	
	int_marker.controls.append(rotate_control);

	# add the interactive marker to our collection &
	# tell the server to call processFeedback() when feedback arrives for it
	server.insert(int_marker, processFeedback)
	menu_handler.insert( "Execute Motion",callback=processFeedback )
	menu_handler.insert( "Move to endeff", callback=processFeedback )
	ViaPoints=menu_handler.insert( "Via Points")
	menu_handler.insert( "Add via-point", parent=ViaPoints ,callback=processFeedback )
	menu_handler.insert( "Execute Trajectory", parent=ViaPoints ,callback=processFeedback )
	menu_handler.insert( "Reset via-points", parent=ViaPoints, callback=processFeedback )

	menu_handler.apply( server, int_marker.name )

	# 'commit' changes and send to all clients
	server.applyChanges()

	rospy.spin()
