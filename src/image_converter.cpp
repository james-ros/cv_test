//霍夫曲線
#include<ros/ros.h>
#include<iostream>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.h>
#include<image_transport/image_transport.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<geometry_msgs/Pose2D.h>

#define pi 3.14159265
int cx = 0;
int cy = 0;

using namespace std;
using namespace cv;

static const std::string OPENCV_WINDOW = "Hough window";
class RGB_HSV
{
	
private:
    ros::NodeHandle nh_;
    ros::Publisher pub_msg;
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub_;
    image_transport::Publisher image_pub_;
    
    geometry_msgs::Pose2D msg;
public:
    RGB_HSV()
    :it_(nh_)
    {
        image_sub_ = it_.subscribe("image", 1, &RGB_HSV::convert_callback, this);
        image_pub_ = it_.advertise("/image_converter/output_video", 1);
        pub_msg = nh_.advertise<geometry_msgs::Pose2D>("pivideo",1);
       
        cv::namedWindow(OPENCV_WINDOW);
    }
    ~RGB_HSV()
    {
        cv::destroyWindow(OPENCV_WINDOW);
    }
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
    void image_process(cv::Mat img)
    {
		int largest_area = 0;
		int largest_contour_index = 0;
		
        Mat dst;
        Mat hsv;
        Mat blur_G;
        Mat blur_E;
        Mat blur_D;
        Mat cdst;
        cvtColor(img, dst, CV_BGR2HSV);
        cv::Scalar lower_blue = cv::Scalar(100,100,100);
        cv::Scalar upper_blue = cv::Scalar(130,255,255);
        inRange(dst,lower_blue,upper_blue,hsv);
        GaussianBlur(hsv,blur_G,Size(5,5),0,0);
        erode(blur_G,blur_E,0,Point(-1,-1),2,1,1);
        dilate(blur_E,blur_D,0,Point(-1,-1),2,1,1);
        Canny(blur_D, cdst, 50, 200, 3);
        
        vector<Vec2f> lines;
        HoughLines(cdst, lines, 1, CV_PI/180, 100, 0, 0 );
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( dst, pt1, pt2, Scalar(0,0,255), 2, CV_AA);
            
            //vector<vector<Point> > contours;
            //vector<Moments> mu(contours.size());
            //findContours(cdst, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
           Moments moments = cv::moments(cdst, false);

		if(moments.m00 > 0) {
			cx = moments.m10/moments.m00;
			cy = moments.m01/moments.m00;
		}
			        
            cout<<"x="<<cx<<endl;
            cout<<"y="<<cy<<endl;
            cout<<"theta"<<theta*180/pi<<endl;
            geometry_msgs::Pose2D msg;
			msg.x = cx;
			msg.y = cy;
			msg.theta = theta*180/pi;
			pub_msg.publish(msg);
			
        }

        
        imshow(OPENCV_WINDOW, cdst);
        waitKey(5);
    }
};
int main(int argc, char** argv)
{
    ros::init(argc, argv, "RGB");
    RGB_HSV obj;
    ros::Rate rate(7);
    ros::spin();
    rate.sleep();   
}
