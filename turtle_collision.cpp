#include <sstream>

#include <stdio.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <boost/thread/mutex.hpp>
#include <tf/tf.h>
#include <boost/thread/mutex.hpp>
#include <math.h>
#include <sensor_msgs/LaserScan.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include "std_msgs/String.h"


#define toRadian(degree)	((degree) * (M_PI / 180.))
#define toDegree(radian)	((radian) * (180. / M_PI))







boost::mutex mutex[2];
nav_msgs::Odometry g_odom;
float pre_dAngleTurned;
int ff=0;
int collision=1;






void
odomMsgCallback(const nav_msgs::Odometry &msg)
{

	mutex[0].lock(); {
		g_odom = msg;
	} mutex[0].unlock();
}





	tf::Transform
getCurrentTransformation(void)
{

	tf::Transform transformation;


	nav_msgs::Odometry odom;


	mutex[0].lock(); {
		odom = g_odom;
	} mutex[0].unlock();


	transformation.setOrigin(tf::Vector3(odom.pose.pose.position.x, odom.pose.pose.position.y, odom.pose.pose.position.z));


	transformation.setRotation(tf::Quaternion(odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z, odom.pose.pose.orientation.w));


	return transformation;
}





	tf::Transform
getInitialTransformation(void)
{

	tf::Transform transformation;


	ros::Rate loopRate(1000.0);

	while(ros::ok()) {

		ros::spinOnce();

		
		transformation = getCurrentTransformation();

		
		if(transformation.getOrigin().getX() != 0. || transformation.getOrigin().getY() != 0. && transformation.getOrigin().getZ() != 0.) {
			break;
		} else {
			loopRate.sleep();
		}
	}

	
	return transformation;
}


	bool
doRotation(ros::Publisher &pubTeleop, tf::Transform &initialTransformation, double dRotation, double dRotationSpeed)
{

	geometry_msgs::Twist baseCmd;
	baseCmd.linear.x = 0.0;
	baseCmd.linear.y = 0.0;

	if(dRotation < 0.) {

		baseCmd.angular.z = -dRotationSpeed;
	} else {
		baseCmd.angular.z = dRotationSpeed;
	}


	bool bDone = false;
	ros::Rate loopRate(500.0);



	while(ros::ok() && !bDone) {


		ros::spinOnce();


		tf::Transform currentTransformation = getCurrentTransformation();


		tf::Transform relativeTransformation = initialTransformation.inverse() * currentTransformation ;
		tf::Quaternion rotationQuat = relativeTransformation.getRotation();


		double dAngleTurned = atan2((2 * rotationQuat[2] * rotationQuat[3]) , (1-(2 * (rotationQuat[2] * rotationQuat[2]) ) ));




		if( fabs(dAngleTurned) > fabs(dRotation) || (abs(pre_dAngleTurned - dRotation) <  abs(dAngleTurned - dRotation)) || (dRotation == 0)) 
		{
			

			bDone = true;
			break;
		} else {
			pre_dAngleTurned = dAngleTurned;
			
			pubTeleop.publish(baseCmd);

			
			loopRate.sleep();
		}
	}

	
	baseCmd.linear.x = 0.0;
	baseCmd.angular.z = 0.0;
	pubTeleop.publish(baseCmd);
	pre_dAngleTurned = 0;
	
	return bDone;
}



	bool
doTranslation(ros::Publisher &pubTeleop, tf::Transform &initialTransformation, double dTranslation, double dTranslationSpeed)
{

	geometry_msgs::Twist baseCmd;

	if(dTranslation < 0) {
		baseCmd.linear.x = -dTranslationSpeed;
	} else {
		baseCmd.linear.x = dTranslationSpeed;
	}

	baseCmd.linear.y = 0;
	baseCmd.angular.z = 0;


	bool bDone = false;
	ros::Rate loopRate(1000.0);
	collision = 1;	

	while(ros::ok() && !bDone) {
		
		ros::spinOnce();

		tf::Transform currentTransformation = getCurrentTransformation();


		tf::Transform relativeTransformation = initialTransformation.inverse() * currentTransformation ;
		double dDistMoved = relativeTransformation.getOrigin().length();
		
		if(fabs(dDistMoved) >= fabs(dTranslation)) {
			bDone = true;
			break;
		} else {

			pubTeleop.publish(baseCmd);


			loopRate.sleep();
		}
	}




	baseCmd.linear.x = 0.0;
	baseCmd.angular.z = 0.0;
	pubTeleop.publish(baseCmd);


	return bDone;
}




//
int main(int argc, char **argv)
{
	int i, n;
	float x, y, bx, by;

	FILE *fin;
	fin = fopen("/home/yha1483/catkin_ws/src/knu_ros_lecture/src/path.txt","r");
	if(fin == NULL)
		printf("null\n");

	fscanf(fin, "%d", &n);

	ros::init(argc, argv, "turtle_position_move4");

	ros::NodeHandle nhp, nhs,nh3;
	ros::Publisher pubpub = nh3.advertise<std_msgs::String>("chatter",1000);
	ros::Subscriber sub = nhs.subscribe("/odom", 100, &odomMsgCallback);

		


	ros::Publisher pub = nhp.advertise<geometry_msgs::Twist>("/cmd_vel", 100);
	
	float curheading = 0;

	bx = 0;
	by = 0;
	
	
	
	
	for(i=0;i<n;i++)
	{
		fscanf(fin, "%f %f", &x, &y);
		printf("\n**%d Destination : [x : %f] [ y : %f] !!**\n",i, x, y);

		
		tf::Transform currentTransformation = getInitialTransformation();
		tf::Quaternion rotationQuat = currentTransformation.getRotation();
		tf::Quaternion rr = currentTransformation.getRotation();
		float rr2 = atan2((2*rr[2]*rr[3]),(1-(2*(rr[2]*rr[2]))))*180/3.14; //degree


		curheading = rotationQuat.getAngle();

		bx = currentTransformation.getOrigin().getX();
		by = currentTransformation.getOrigin().getY();
		
		printf("** Current Position [x : %f] [y : %f]**\n", bx, by);


		
		if(isnan(curheading) != 0)
			curheading = 0;
		
		curheading = toDegree(curheading);



		double dRotation = toDegree(atan2((y-by),(x-bx)));
		dRotation = (float)((int)dRotation % 360);
		curheading = (float)((int)curheading % 360);


		if(dRotation < 0)
			dRotation = dRotation + 360;
		
		if(rr2<0)
			rr2=rr2+360;

		printf("** between tow spot Rotation %f **\n", dRotation);
		printf("** Current head angle :  %f **\n", rr2);


		
		

		dRotation = dRotation - rr2;
		printf("** Rotation angle :  %f ** \n", dRotation);


		tf::Transform initialTransformation = getInitialTransformation();
		

		if(dRotation>180)
		{
		dRotation = dRotation -360;
		
		}
		else if(dRotation<-180)
		{
		dRotation = dRotation + 360;
		}
		doRotation(pub, initialTransformation, toRadian(dRotation), 0.1);
		
		if(ff=0)
		{
		std_msgs::String msg2;
		std::stringstream ss2;
		ss2<<"aa";
		msg2.data=ss2.str();
		int asdf=0;
		pubpub.publish(msg2);
		ff=1;
		}
		
		bx = currentTransformation.getOrigin().getX();
		by = currentTransformation.getOrigin().getY();
		
		double dTranslation = sqrt((x-bx)*(x-bx) + (y-by)*(y-by));
		printf("** Translation distance :  %f **\n", dTranslation);
			


		
		doTranslation(pub, initialTransformation, dTranslation, 0.3);

		std_msgs::String msg;
		std::stringstream ss;
		ss<<"aa";
		msg.data=ss.str();
		int asdf=0;
		if(asdf==0)
		{
		pubpub.publish(msg);	
		asdf=1;		
		}
		sleep(1);
		asdf=0;

		
		printf("\n\n");

		
	}

	fclose(fin);

	return 0;
}
