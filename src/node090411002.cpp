#include <ros/ros.h>
#include <geometry_msgs/Pose2D.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h>
#include <sstream>
#define pi 3.1415926
double kp,error,Turn;
int offset,tp,i=0,count=0;
float x,y,theta,powerR,powerL,right,left;
std_msgs::String direction;
std_msgs::String information;
void pcontroller(){
//	kp = 0.3;
	kp = 0.32;
  	offset = 320;
//  tp = 90;
  	tp = 90;
  	error = x - offset;
  	Turn= kp*error;  
  	powerR = tp - Turn;    
  	powerL = tp + Turn;
}
void QRcodecontroller(){
	if (direction.data == "right"){     //turnright
			powerR = 0;
			powerL = 200;
		    i+=2;
		}
	else if(direction.data == "stop"){   //turnleft
			powerR = 120;
			powerL = 120;
			i+=4;
		}
	  else {
			powerR = 1;
			powerL = 1;
		}
//		else if(direction.data == "zero"){
//			std::stringstream ss;
//        	ss << "zero" << count;
//			information.data = ss.str();	
//        }
		if (i==24){
        	std::stringstream ss;
        	ss << "zero" << count;
			direction.data = ss.str();
			information.data = ss.str();
			i=0;	
		}
	}

void motor_cb(const geometry_msgs::Pose2D &msg){
	if((msg.x>0)&&(msg.x<=640)){
		ROS_INFO_STREAM("Subscriber Pose2D:"<<"x="<<msg.x<<"theta="<<msg.theta);
		x=msg.x;
		y=msg.y;
		theta=msg.theta;
 	}
}
void QRcode_cb(const std_msgs::String::ConstPtr &msg){
    ROS_INFO_STREAM("Subscriber QRcode:"<<"direction="<<msg->data.c_str());
    direction.data = msg->data.c_str();
}

void check_cb(const std_msgs::String::ConstPtr &msg){
	information.data = msg->data.c_str();
    ROS_INFO_STREAM("Subscriber checking msg:"<<"msg="<<information.data);
}


int main(int argc, char **argv){
	ros::init(argc, argv, "motor_calculator");
	ros::NodeHandle nh;
	ros::Subscriber pv_sub = nh.subscribe("pivideo",1, &motor_cb);
	ros::Subscriber QR_sub = nh.subscribe("barcode",1, &QRcode_cb);
	ros::Subscriber check_sub = nh.subscribe("check",5, &check_cb);
	ros::Publisher right_pub = nh.advertise<std_msgs::Float64>("motor_Rwheelbit", 1);
	ros::Publisher left_pub = nh.advertise<std_msgs::Float64>("motor_Lwheelbit", 1);
	ros::Rate rate(5);
	while(ros::ok()){
		if((x>0)&&(x<=640)){
			if(information.data == "wait"){
             QRcodecontroller();
             }
             else
             { 
             pcontroller();
             }
			std_msgs::Float64 right,left ;
			right.data= powerR;
			left.data= powerL;
			right_pub.publish(right);
			left_pub.publish(left);
			ROS_INFO_STREAM("Sending arduino motor value:"<<" right="<<right.data<<" left="<<left.data);
		}
		else{
			ROS_INFO("no get pivideo");
		}
		ros::spinOnce();		
		rate.sleep();
	}
	return 0;
}
