//顏色辨識到停止
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
int contArea = 0;
//static const std::string OPENCV_WINDOW = "Hough window";
class RGB_HSV
{
	
private:
    ros::NodeHandle nh_;
    ros::Publisher pub_msg;
    ros::Subscriber sub_msg;
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub_;
    //image_transport::Publisher image_pub_;
    std_msgs::String barcode;
    std_msgs::String msg;
    
public:
    RGB_HSV()
    :it_(nh_)
    {
        sub_msg = nh_.subscribe("barcode",1,&RGB_HSV::barcodeCallback,this);
        image_sub_ = it_.subscribe("image", 1, &RGB_HSV::convert_callback, this);
        //image_pub_ = it_.advertise("/image_converter/output_video", 1);
        pub_msg = nh_.advertise<std_msgs::String>("check",1);
     
        //cv::namedWindow(OPENCV_WINDOW);
    }
    //~RGB_HSV()
    //{
    //    cv::destroyWindow(OPENCV_WINDOW);
    //}
    void convert_callback(const sensor_msgs::ImageConstPtr& msg)
    {
        cv_bridge::CvImagePtr cv_ptr;
        try
        {
            cv_ptr =  cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        }
        catch(cv_bridge::Exception& e)
        {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }
        image_process(cv_ptr->image);
    }
    void barcodeCallback(const std_msgs::String::ConstPtr& msg)
    {
		//ROS_INFO(msg->data.c_str());
		barcode.data = msg->data.c_str();
	}
    void image_process(cv::Mat img)
    {
		int largest_area = 0;
		int largest_contour_index = 0;
		
        Mat dst;
        Mat hsv;
        Mat cy;
        Mat cdst;
        cvtColor(img, dst, CV_BGR2HSV);
        cv::Scalar lower_white = cv::Scalar(8,0,80);
        cv::Scalar upper_white = cv::Scalar(52,160,120);
        inRange(dst,lower_white,upper_white,hsv);
        GaussianBlur(hsv,cy, Size(5,5), 0, 0);
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(cy,contours,hierarchy,RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

        vector<Moments> mu(contours.size());
        for (unsigned int i = 0; i < contours.size(); i++){
			Rect rect = boundingRect(contours[i]);
			if(rect.width < img.cols/3)
			continue;
			if(rect.width > (img.cols - 30))
			continue;
           cout << "qr "<< endl;
           Moments m = moments(contours[i]);
           cout << "area = " << m.m00 <<endl;
           double contArea = contourArea(contours[i]);
           cout << "contour area = " << contArea << endl;
//		  if ((barcode.data == "right") || (barcode.data == "stop")){
//					int count = 0 ;
//					std::string ss = "go";
//					msg.data = ss;
//                    pub_msg.publish(msg);
//					std::stringstream aa;
//        			aa << "zero" << count;
//					barcode.data = aa.str();
                   
	//	}
		if (contArea >40000) {
					std::string ss = "wait";
					msg.data = ss;
                    pub_msg.publish(msg);	

				}
			}		
}
};
int main(int argc, char** argv)
{
    ros::init(argc, argv, "check_node");
    RGB_HSV obj;
    ros::spin();
}

