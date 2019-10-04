#include<ros.h>

#include <std_msgs/Float64.h>

#define pi 3.1415926

int powerR ;

int powerL ;

int powerL2 ;

int powerR2 ;

boolean run = false;// 一開始先設定成「停止」

const int motorright1 = 11;

const int motorright2 = 10;

const int motorleft1 = 9;

const int motorleft2 = 6;

ros::NodeHandle nh;

void Rwheelbit_cb(const std_msgs::Float64& msg){

  if(msg.data>=0){

    powerR =(int) msg.data;

    powerR2 =0;

  }

  else

  {

    powerR2=(int)-msg.data;

    powerR =0;

  }

}

ros::Subscriber<std_msgs::Float64> R_sub("motor_Rwheelbit", &Rwheelbit_cb);

void Lwheelbit_cb(const std_msgs::Float64& msg){

  if(msg.data>=0){

    powerL =(int)msg.data;

    powerL2 =0;

  }

  else

  {

    powerL2=(int)-msg.data;

    powerL =0;

  }

}

ros::Subscriber<std_msgs::Float64> L_sub("motor_Lwheelbit", &Lwheelbit_cb);



void setup() {

  nh.initNode();

  nh.subscribe(R_sub);

  nh.subscribe(L_sub);

  //Serial.begin(9600);

  pinMode(motorright1,OUTPUT);

  pinMode(motorright2,OUTPUT);

  pinMode(motorleft1,OUTPUT);

  pinMode(motorleft2,OUTPUT);

}

void loop() {

  //Serial.print("Right pin(11,10)=");

  //Serial.println(powerR,powerR2);

  //Serial.print("Light pin(9,6)=");

  //Serial.println(powerL,powerL2);

  analogWrite(motorright1,powerR);

  analogWrite(motorright2,powerR2);

  analogWrite(motorleft1,powerL);

  analogWrite(motorleft2,powerL2);

  delay(1000);

  nh.spinOnce();

}
