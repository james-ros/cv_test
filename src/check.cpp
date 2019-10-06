//顏色辨識到停止 node：check_node
#include <ros/ros.h>
#include <iostream>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <std_msgs/String.h>
#include <unistd.h>
#include <sstream>
using namespace std;
using namespace cv;
double contArea ;
std_msgs::String action;

void image_process(cv::Mat img){
	contArea = 0 ;
    cout << "1 "<< endl;
	int largest_area = 0;
	int largest_contour_index = 0;	
    cv::Mat dst,hsv,cy,cdst;
    cv::cvtColor(img, dst, CV_BGR2HSV);
    cv::Scalar lower_white = cv::Scalar(18,94,78);
    cv::Scalar upper_white = cv::Scalar(42,182,123);
    cv::inRange(dst,lower_white,upper_white,hsv);
    cv::GaussianBlur(hsv,cy, Size(5,5), 0, 0);
    std::vector<std::vector<cv::Point> > contours;
    std::vector<Vec4i> hierarchy;
    cv::findContours(cy,contours,hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE, Point(0,0));
    std::vector<Moments> mu(contours.size());
    for (unsigned int i = 0; i < contours.size(); i++){
		Rect rect = boundingRect(contours[i]);
		if(rect.width < img.cols/3)
		continue;
		if(rect.width > (img.cols - 30))
		continue;
        cout << "qr "<< endl;
        Moments m = cv::moments(contours[i]);
        cout << "area = " << m.m00 <<endl;
        contArea = contourArea(contours[i]);
        cout << "contour area = " << contArea << endl;
	}		
}

void convert_callback(const sensor_msgs::ImageConstPtr& msg){
    cv_bridge::CvImagePtr cv_ptr;
   try
    {
        cv_ptr =  cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        image_process(cv_ptr->image);
    }
   catch(cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
}
int main(int argc, char **argv){
	ros::init(argc, argv, "check_node");
	ros::NodeHandle nh_;
    ros::Publisher pub_msg = nh_.advertise<std_msgs::String>("check",1);
	image_transport::ImageTransport it_(nh_);
	image_transport::Subscriber image_sub_ = it_.subscribe("image", 1, &convert_callback);
	ros::Rate rate(50);
	while(ros::ok()){
		if (contArea > 40000.0) {
			std::string ss = "wait";
			action.data = ss;
			pub_msg.publish(action);
		}else{
			std::string aa = "none";
			action.data = aa;
		}
		ros::spinOnce();		
		rate.sleep();
	}
	return 0;
}
