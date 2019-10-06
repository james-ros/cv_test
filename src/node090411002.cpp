//馬達控制p-controller,qr-code node：motor_calculator
#include <ros/ros.h>
#include <geometry_msgs/Pose2D.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h>
#include <cv_test/agv_msg.h>
#include <sstream>
#define pi 3.1415926
using namespace std;
double kp,error,Turn;
int offset,tp,i=0,msg_count=0;
float x,y,theta,powerR,powerL,right,left;
string listi[3] = {"Ai","Bi","Ci"};
string listf[3] = {"Af","Bf","Cf"};
std_msgs::String direction;
std_msgs::String information;
cv_test::agv_msg position;
cv_test::agv_msg location;
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
	if ((direction.data == "right")||(direction.data == listf[0])||(direction.data == listf[1])||(direction.data == listf[2])){ //turnright
		powerR = 0;
		powerL = 200;
		i+=2;
		if(direction.data == listf[0]){
			location.positionA = true;
		}
		else if(direction.data == listf[1]){
			location.positionB = true;
		}
		else if(direction.data == listf[2]){
			location.positionC = true;
		}
	}
	else if(direction.data == "stop"){   //turnleft
		powerR = 120;
		powerL = 120;
		i+=4;
	}
	else if(direction.data == listi[0]){
		if (position.positionA == false){
			powerR = 120;
			powerL = 120;
			i+=4;
		}
		else{
			powerR = 200;
			powerL = 0;
			i+=2;
		}
	}
	else if(direction.data == listi[1]){
		if (position.positionB == false){
			powerR = 120;
			powerL = 120;
			i+=4;
		}
		else{
			powerR = 200;
			powerL = 0;
			i+=2;
		}
	}
	else if(direction.data == listi[2]){
		if (position.positionC == false){
			powerR = 120;
			powerL = 120;
			i+=4;
		}
		else{
			powerR = 200;
			powerL = 0;
			i+=2;
		}
	}
	else {
			powerR = 1;
			powerL = 1;
		}
	if (i==20){
		if (direction.data == listi[0]){
			location.positionA = false;
		}
		else if (direction.data == listi[1]){
			location.positionB = false;
		}
		else if (direction.data == listi[2]){
			location.positionC = false;
		}
        std::stringstream ss;
        ss << "zero" << msg_count;
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
void location_cb(const cv_test::agv_msg::ConstPtr &msg){
	position = *msg;
    ROS_INFO_STREAM("Subscriber position msg: "<<"\n"<<position);
}


int main(int argc, char **argv){
	ros::init(argc, argv, "motor_calculator");
	location.positionA = true ;
	location.positionB = true ;
	location.positionC = true ;
	ros::NodeHandle nh;
	ros::Subscriber pv_sub = nh.subscribe("pivideo",1, &motor_cb);
	ros::Subscriber QR_sub = nh.subscribe("barcode",2, &QRcode_cb);
	ros::Subscriber check_sub = nh.subscribe("check",1, &check_cb);
	ros::Subscriber location_sub = nh.subscribe("position",1, &location_cb);
	ros::Publisher right_pub = nh.advertise<std_msgs::Float64>("motor_Rwheelbit", 1);
	ros::Publisher left_pub = nh.advertise<std_msgs::Float64>("motor_Lwheelbit", 1);
	ros::Publisher location_pub = nh.advertise<cv_test::agv_msg>("position",1);
	ros::Rate rate(5);
	while(ros::ok()){
		if((x>0)&&(x<=640)){
			std_msgs::Float64 right,left;
			if(information.data == "wait"){
             QRcodecontroller();
             }
             else
             { 
             pcontroller();
             }
			right.data= powerR;
			left.data= powerL;
			right_pub.publish(right);
			left_pub.publish(left);
			location_pub.publish(location);
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
