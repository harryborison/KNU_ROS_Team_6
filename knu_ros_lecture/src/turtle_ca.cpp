#include <signal.h>
#include <unistd.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <boost/thread/mutex.hpp>
#include <tf/tf.h>
#include <math.h>
#include <sensor_msgs/LaserScan.h>
#include <stdlib.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define toRadian(degree)	((degree) * (M_PI / 180.))
#define toDegree(radian)	((radian) * (180. / M_PI))



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable
nav_msgs::Odometry g_odom;
sensor_msgs::LaserScan g_scan;
boost::mutex mutex[2];
float pre_dAngleTurned;

double position[10][2];
int flag_coll = 1;
/*
// Decleation of subscriber
ros::Subscriber sub;
// Create a publisher object
ros::Publisher pub;
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A template method to check 'nan'
	template<typename T>
inline bool isnan(T value)
{
	return value != value;
}

void handler(int sig)
{
	signal(SIGTERM, handler);
	
	printf("자동주행을 종료합니다\n");

	exit(0);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// callback functio
	void
odomMsgCallback(const nav_msgs::Odometry &msg)
{
	// receive a '/odom' message with the mutex
	mutex[0].lock(); {
		g_odom = msg;
	} mutex[0].unlock();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// callback function
	void
scanMsgCallback(const sensor_msgs::LaserScan& msg)
{
	// receive a '/odom' message with the mutex
	mutex[1].lock(); {
		g_scan = msg;
	} mutex[1].unlock();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
	void
convertScan2XYZs(sensor_msgs::LaserScan& lrfScan)
{
	int nRangeSize = (int)lrfScan.ranges.size();
	int count = 0;
	float sum = 0;
	float average = 9999;

	
	for(int i=0; i<nRangeSize; i++) { //360도 까지 돌면서 각 각도마다 거리값들.
		double dRange = lrfScan.ranges[i];

		if( !isnan(dRange) && dRange <= 0.4) { //0.3m 이내의 값들을 더함...
			printf("dRange is %f\n", dRange);
			count++;
			sum += dRange;
		} 
		if(i==30) i+=300;// 30 ~ -30의 각도 범위
	}

	if(count == 0) {
		printf("30 ~ -30 평균거리 : %s\n", "물체 없음.");
		return;
	} else {
		//printf("sum : %f count: %d\n", sum, count);
		average = sum/count;
	}
	printf("-30 ~ 30도 사이의 장애물 평균거리 : %f\n", average);
	//충돌가능성 탐지
	if(average <= 0.20 && sum != 0) {
		printf("장애물이 너무 가깝습니다!!\n");
		flag_coll = 0;
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// odom\C0\B8\B7κ\CE\C5\CD \C7\F6\C0\E7\C0\C7 \BA\AFȯ\C7\E0\B7\C4 \C1\A4\BA\B8\B8\A6 \B8\AE\C5\CF!
	tf::Transform
getCurrentTransformation(void)
{
	// transformation \B9\F6\C6\DB
	tf::Transform transformation;

	// odom \B9\F6\C6\DB
	nav_msgs::Odometry odom;

	// copy a global '/odom' message with the mutex
	mutex[0].lock(); {
		odom = g_odom;
	} mutex[0].unlock();

	// \C0\A7ġ \C0\FA\C0\E5
	transformation.setOrigin(tf::Vector3(odom.pose.pose.position.x, odom.pose.pose.position.y, odom.pose.pose.position.z));
	//	printf("set Origin : %f, %f, %f\n", odom.pose.pose.position.x, odom.pose.pose.position.y, odom.pose.pose.position.z);
	// ȸ\C0\FC \C0\FA\C0\E5
	transformation.setRotation(tf::Quaternion(odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z, odom.pose.pose.orientation.w));
	//	printf("set Rotation : %f, %f, %f\n", odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z, odom.pose.pose.orientation.w);
	// \B8\AE\C5\CF
	return transformation;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// \B7κ\BF\C0\CC \B8\D8\C3\E7\C0ִ\C2 \BB\F3\C5\C2(ó\C0\BD \BB\F3\C5\C2)\C0\C7 \C0\A7ġ\B8\A6 \C0\FA\C0\E5!
	tf::Transform
getInitialTransformation(void)
{
	// tf \BA\AFȯ\C7\E0\B7\C4
	tf::Transform transformation;

	// ó\C0\BD\C0\A7ġ\BF\A1 \B4\EB\C7\D1 odometry \B8޽\C3\C1\F6 \B9ޱ\E2
	ros::Rate loopRate(500.0);

	while(ros::ok()) {
		// \C0ϴ\DC callback \B8޽\C3\C1\F6\B8\A6 \B9ް\ED!
		ros::spinOnce();

		// get current transformationreturn;
		transformation = getCurrentTransformation();

		// \B8޽\C3\C1\F6\B8\A6 \B9޾\D2\C0\B8\B8\E9 break!
		if(transformation.getOrigin().getX() != 0. || transformation.getOrigin().getY() != 0. && transformation.getOrigin().getZ() != 0.) {
			break;
		} else {
			loopRate.sleep();
		}
	}

	// \B8\AE\C5\CF
	return transformation;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ȸ\C0\FC\BD\C7\C7\E0
	bool
doRotation(ros::Publisher &pubTeleop, tf::Transform &initialTransformation, double dRotation, double dRotationSpeed)
{
	//the command will be to turn at 'rotationSpeed' rad/s
	geometry_msgs::Twist baseCmd;
	baseCmd.linear.x = 0.0000;
	baseCmd.linear.y = 0.0000;

	int flag = 1;
	int flag2 = 1;
	if(dRotation < 0) {
		baseCmd.angular.z = -dRotationSpeed;
		flag2 = -1;
	} else {
		baseCmd.angular.z = dRotationSpeed;
		flag2 = 1;
	}
	// \C0̵\BF\C7ϸ鼭 \C7\F6\C0\E7\C0\A7ġ\BF\A1 \B4\EB\C7\D1 odometry \B8޽\C3\C1\F6 \B9ޱ\E2
	bool bDone = false;
	ros::Rate loopRate(500.0);


	while(ros::ok() && !bDone) {
		// \C0ϴ\DC callback \B8޽\C3\C1\F6\B8\A6 \B9ް\ED!
		if( flag == 1 && fabs(pre_dAngleTurned - dRotation) <= 0.344533) {
			baseCmd.angular.z = (double) baseCmd.angular.z / (double) 8;
			ros::Rate loopRate(200.0);		
			flag = 0;
		}
		ros::spinOnce();
		// get current transformation
		tf::Transform currentTransformation = getCurrentTransformation();
		//see how far we've traveled
		tf::Transform relativeTransformation = initialTransformation.inverse() * currentTransformation ;
		tf::Quaternion rotationQuat = relativeTransformation.getRotation();

		double dAngleTurned = atan2((2 * rotationQuat[2] * rotationQuat[3]) , (1-(2 * (rotationQuat[2] * rotationQuat[2]) ) ));

		// \C1\BE\B7\E1\C1\B6\B0\C7 üũ
		//	printf("fabs:dAngleTurneed:%f, dRotation:%f\n", fabs(dAngleTurned), fabs(dRotation));
		if( (double) fabs((double)dAngleTurned) > (double)fabs((double)dRotation) || /*(abs(pre_dAngleTurned - dRotation)*flag2 <  abs(dAngleTurned - dRotation))*flag2 */(flag==0&&fabs(dAngleTurned+baseCmd.angular.z)>3.14159)|| (dRotation == 0) ) 
		{
			bDone = true;
			break;
		} else {
			pre_dAngleTurned = dAngleTurned;
			//send the drive command
			pubTeleop.publish(baseCmd);

			// sleep!
			loopRate.sleep();
		}
	}
	// ÃÊ±âÈ­
	baseCmd.linear.x = 0.0000;
	baseCmd.angular.z = 0.0000;

	pubTeleop.publish(baseCmd);
	flag_coll = 1;
	return bDone;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// \C0̵\BF
	bool
doTranslation(ros::Publisher &pubTeleop, tf::Transform &initialTransformation, double dTranslation, double dTranslationSpeed)
{
	//the command will be to go forward at 'translationSpeed' m/s
	geometry_msgs::Twist baseCmd;
	sensor_msgs::LaserScan scan;

	if(dTranslation < 0) {
		baseCmd.linear.x = -dTranslationSpeed;
	} else {
		baseCmd.linear.x = dTranslationSpeed;
	}

	baseCmd.linear.y = 0.0000;
	baseCmd.angular.z = 0.0000;

	// \C0̵\BF\C7ϸ鼭 \C7\F6\C0\E7\C0\A7ġ\BF\A1 \B4\EB\C7\D1 odometry \B8޽\C3\C1\F6 \B9ޱ\E2
	bool bDone = false;
	ros::Rate loopRate(500.0);

	flag_coll = 1;

	while(ros::ok() && !bDone) {
		ros::spinOnce();
		// \C0ϴ\DC callback \B8޽\C3\C1\F6\B8\A6 \B9ް\ED!
		tf::Transform temp = getCurrentTransformation();
		printf("=======================================\n");
		//printf("터틀봇의 현재 위치 : %.4f, %.4f\n", temp.getOrigin().x(), temp.getOrigin().y() );

		tf::Quaternion tempQuater = temp.getRotation();
		double currentRotate = atan2((2 * tempQuater[2] * tempQuater[3]) , (1-(2 * (tempQuater[2] * tempQuater[2]) ) ))* 180/3.14159265;
		//printf("터틀봇의 현재 방향 : %f\n", currentRotate);

		// receive the global '/scan' message with the mutex
		mutex[1].lock(); {
			scan = g_scan;
		} mutex[1].unlock();

		convertScan2XYZs(scan);
		printf("=======================================\n");
		//충돌가능성 감지
		if(flag_coll == 0) {
			baseCmd.linear.x = 0.0000;
			baseCmd.angular.z = 0.0000;
			pubTeleop.publish(baseCmd);
			return false;
		}

		// get current transformation
		tf::Transform currentTransformation = getCurrentTransformation();

		//see how far we've traveled
		tf::Transform relativeTransformation = initialTransformation.inverse() * currentTransformation ;
		double dDistMoved = relativeTransformation.getOrigin().length();

		// \C1\BE\B7\E1\C1\B6\B0\C7 üũ
		//	printf("fabs:dDistMoved:%f, dTranslation:%f\n", fabs(dDistMoved), fabs(dTranslation));
		if( fabs(dDistMoved) >= fabs(dTranslation) ) {
			bDone = true;
			break;
		} else {
			//send the drive command
			//printf("current moving distance : %f\n", dDistMoved);
			pubTeleop.publish(baseCmd);

			// sleep!
			loopRate.sleep();
		}
	}

	//  \C3ʱ\E2ȭ

	baseCmd.linear.x = 0.0000;
	baseCmd.angular.z = 0.0000;
	pubTeleop.publish(baseCmd);
	flag_coll = 1;

	return bDone;
}
/*
   void moveTurtle(double _rotation, double _translation) {

   float _dRatation = (float)((int)_rotation % 360);

   if(abs(_dRatation) > 180){
   if(_rotation > 0) _rotation = -(360-_dRatation);
   else _rotation = (360+_dRatation); }
   else
   _rotation = _dRatation;

   tf::Transform initialTransformation = getInitialTransformation();

   if( _rotation > 0.0001 ) {
   printf("what?\n");
   doRotation(pub, initialTransformation, toRadian(_rotation), 0.60);
   }
   doTranslation(pub, initialTransformation, _translation, 0.25);

   }*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, char **argv)
{
	int i;
	signal(SIGTERM, handler);
	ros::init(argc, argv, "turtle_ca");
	// Ros initialization
	ros::NodeHandle nhp, nhs;

	// Decleation of subscriber
	ros::Subscriber sub = nhs.subscribe("/odom", 100, &odomMsgCallback);
	ros::Subscriber subScan = nhs.subscribe("/scan", 10, &scanMsgCallback);

	// Create a publisher object
	ros::Publisher pub = nhp.advertise<geometry_msgs::Twist>("/cmd_vel", 100);
	/*    sub = nhs.subscribe("/odom", 100, &odomMsgCallback);
		  pub = nhp.advertise<geometry_msgs::Twist>("/cmd_vel", 100);*/

	double temp_rotation;
	double tempLength;
	double dRotation;

	srand( (unsigned) time(NULL) );
	dRotation = (rand()%90)-45;


	tf::Transform initialTransformation = getInitialTransformation();
	//doTranslation(pub, initialTransformation, 500, 0.2);

	printf("\n 시작 방향을 정합니다.\n");
	if( fabs(dRotation) > 0.0001 ) {
		doRotation(pub, initialTransformation, toRadian(dRotation), 0.60);
	}

	while(1) {
		initialTransformation = getInitialTransformation();
		doTranslation(pub, initialTransformation, 500, 0.1);

		if(flag_coll == 0) {
			while(!flag_coll) {
				printf("장애물발견!! 강아지인지 장애물인지 구별중...\n");
				sleep(5);
				int ret = system("python3 /home/mystery/rosgit/KNU_ROS_Team_6/face.py");
				sleep(5);
				
				tf::Transform temp2 = getInitialTransformation();

				double tempvector[2];
				tempvector[0] = 0 - temp2.getOrigin().x();
				tempvector[1] = 0 - temp2.getOrigin().y();
				tf::Quaternion tempQuater = temp2.getRotation();
				double currentRotate = atan2((2 * tempQuater[2] * tempQuater[3]) , (1-(2 * (tempQuater[2] * tempQuater[2]) ) ))* 180/3.14159265;
				temp_rotation = (atan2( tempvector[1], tempvector[0] ) * 180/3.14159265) - currentRotate;

				srand( (unsigned) time(NULL) );
				int randnum = (rand()%90)+30;

				if( ((int) temp_rotation+360)%360 >= 180 ) {
					doRotation(pub, temp2, toRadian(randnum*(-1)), 0.60);	
				} else {
					doRotation(pub, temp2, toRadian(randnum), 0.60);
				}

			}
		}	
	}


	return 0;
}
