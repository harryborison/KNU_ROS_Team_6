#include <ros/ros.h>
#include <iostream>
#include <ctime>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <boost/thread/mutex.hpp>
#include <tf/tf.h>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <sensor_msgs/LaserScan.h>

using namespace cv;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define toRadian(degree)	((degree) * (M_PI / 180.))
#define toDegree(radian)	((radian) * (180. / M_PI))



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable
int stop;
boost::mutex mutex[3];
nav_msgs::Odometry g_odom;
float pre_dAngleTurned;
sensor_msgs::LaserScan g_scan;

template<typename T>
inline bool isnan(T value)
{
    return value != value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// callback function
void
odomMsgCallback(const nav_msgs::Odometry &msg)
{
    // receive a '/odom' message with the mutex
    mutex[0].lock(); {
        g_odom = msg;
    } mutex[0].unlock();
}
////
void
scanMsgCallback(const sensor_msgs::LaserScan& msg)
{
    // receive a '/odom' message with the mutex
    mutex[1].lock(); {
        g_scan = msg;
    } mutex[1].unlock();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// odom으로부터 현재의 변환행렬 정보를 리턴!
tf::Transform
getCurrentTransformation(void)
{
    // transformation 버퍼
    tf::Transform transformation;

    // odom 버퍼
    nav_msgs::Odometry odom;

    // copy a global '/odom' message with the mutex
    mutex[0].lock(); {
        odom = g_odom;
    } mutex[0].unlock();

    // 위치 저장
    transformation.setOrigin(tf::Vector3(odom.pose.pose.position.x, odom.pose.pose.position.y, odom.pose.pose.position.z));

    // 회전 저장
    transformation.setRotation(tf::Quaternion(odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z, odom.pose.pose.orientation.w));

    // 리턴
    return transformation;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 로봇이 멈춰있는 상태(처음 상태)의 위치를 저장!
tf::Transform
getInitialTransformation(void)
{
    // tf 변환행렬
    tf::Transform transformation;

    // 처음위치에 대한 odometry 메시지 받기
    ros::Rate loopRate(1000.0);

    while(ros::ok()) {
        // 일단 callback 메시지를 받고!
        ros::spinOnce();

        // get current transformationreturn;
        transformation = getCurrentTransformation();

        // 메시지를 받았으면 break!
        if(transformation.getOrigin().getX() != 0. || transformation.getOrigin().getY() != 0. && transformation.getOrigin().getZ() != 0.) {
            break;
        } else {
            loopRate.sleep();
        }
    }

    // 리턴
    return transformation;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 회전실행
bool
doRotation(ros::Publisher &pubTeleop, tf::Transform &initialTransformation, double dRotation, double dRotationSpeed)
{
    //the command will be to turn at 'rotationSpeed' rad/s
    geometry_msgs::Twist baseCmd;
    baseCmd.linear.x = 0.0;
    baseCmd.linear.y = 0.0;

    if(dRotation < 0.) {
        baseCmd.angular.z = -dRotationSpeed;
    } else {
        baseCmd.angular.z = dRotationSpeed;
    }

    // 이동하면서 현재위치에 대한 odometry 메시지 받기
    bool bDone = false;
    ros::Rate loopRate(1000.0);



    while(ros::ok() && !bDone) {
        // 일단 callback 메시지를 받고!

        ros::spinOnce();

        // get current transformation
        tf::Transform currentTransformation = getCurrentTransformation();

        //see how far we've traveled
        tf::Transform relativeTransformation = initialTransformation.inverse() * currentTransformation ;
        tf::Quaternion rotationQuat = relativeTransformation.getRotation();


        
         double dAngleTurned = atan2((2 * rotationQuat[2] * rotationQuat[3]) , (1-(2 * (rotationQuat[2] * rotationQuat[2]) ) ));

  // 종료조건 체크

    if( fabs(dAngleTurned) > fabs(dRotation) || (abs(pre_dAngleTurned - dRotation) <  abs(dAngleTurned - dRotation)) || (dRotation == 0)) 
	{
            bDone = true;
		 //send the drive command
         // pubTeleop.publish(baseCmd);
	 //   printf("error!\n");
         //   loopRate.sleep();
	break;
        } else {
	    pre_dAngleTurned = dAngleTurned;
            //send the drive command
            pubTeleop.publish(baseCmd);
 	  //  printf("deded\n");
            // sleep!
            loopRate.sleep();
       	
	}
    }

    // 횄횎짹창횊짯
    baseCmd.linear.x = 0.0;
    baseCmd.angular.z = 0.0;
    pubTeleop.publish(baseCmd);

    pre_dAngleTurned = 0;

    return bDone;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이동
bool
doTranslation(ros::Publisher &pubTeleop, tf::Transform &initialTransformation, double dTranslation, double dTranslationSpeed)
{
    //the command will be to go forward at 'translationSpeed' m/s
    geometry_msgs::Twist baseCmd;

    if(dTranslation < 0) {
        baseCmd.linear.x = -dTranslationSpeed;
    } else {
        baseCmd.linear.x = dTranslationSpeed;
    }

    baseCmd.linear.y = 0;
    baseCmd.angular.z = 0;

    // 이동하면서 현재위치에 대한 odometry 메시지 받기
    bool bDone = false;
    ros::Rate loopRate(1000.0);

    while(ros::ok() && !bDone) {
        // 일단 callback 메시지를 받고!
        ros::spinOnce();

        // get current transformation
        tf::Transform currentTransformation = getCurrentTransformation();

        //see how far we've traveled
        tf::Transform relativeTransformation = initialTransformation.inverse() * currentTransformation ;
        double dDistMoved = relativeTransformation.getOrigin().length();

        // 종료조건 체크

        if(fabs(dDistMoved) >= fabs(dTranslation)) {
            bDone = true;
            break;
        } else {
            //send the drive command
            pubTeleop.publish(baseCmd);
	    
            // sleep!
            loopRate.sleep();
        }
    }

    //  초기화

    baseCmd.linear.x = 0.0;
    baseCmd.angular.z = 0.0;
    pubTeleop.publish(baseCmd);

    return bDone;
}
//###############################################################//
int 
converScan2XYZs(sensor_msgs::LaserScan& lrfScan, vector<Vec3d> &XYZs)
{
    int nRangeSize = (int)lrfScan.ranges.size();
    XYZs.clear();
    XYZs.resize(nRangeSize);
    
    double min;
    double max;
    
  for(int i=0; i<nRangeSize; i++) {
        double dRange = lrfScan.ranges[i];
        
	min = lrfScan.angle_min;
	max = lrfScan.angle_max;
//	printf("angle min %lf, max %lf\n", min, max);
	tf::Transform cur = getInitialTransformation();
        tf::Quaternion currQuate = cur.getRotation();
        double currHear = toDegree(atan2((2*currQuate[2]*currQuate[3]), (1-(2*(currQuate[2]*currQuate[2])))));
	double mcur = currHear +120;
 	double ncur = currHear -120;
        if(dRange < 0.55 && ( currHear < mcur && currHear > ncur ) ) {
            XYZs[i] = Vec3d(0., 0., 0.);
       	    printf("##############################\n");
	    printf("#       Detect obstacles !   #\n");
	    printf("##############################\n");
	    printf("Average of distance : %lf\n",dRange);
	    stop = 1;
	    return 1;
           // doRotation(pub, initialTransformation, toRadian(dRotation), 0.75);

        } else {
	    printf("Average of distance: %lf\n",dRange);
            double dAngle = lrfScan.angle_min + i*lrfScan.angle_increment;
            XYZs[i] = Vec3d(dRange*cos(dAngle), dRange*sin(dAngle), 0.);
	    return 0;
        }
    }
}
//###############################################################//
//###############################################################//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, char **argv)
{
    // ROS 초기화
    ros::init(argc, argv, "turtle_position_move");

    // Ros initialization
    ros::NodeHandle nhp, nhs, nh;

    // Decleation of subscriber
    ros::Subscriber sub = nhs.subscribe("/odom", 100, &odomMsgCallback);
    ros::Subscriber subScan = nh.subscribe("/scan", 10, &scanMsgCallback);

    // Create a publisher object
    ros::Publisher pub = nhp.advertise<geometry_msgs::Twist>("/cmd_vel", 100);

    // exception
   // if(argc != 3) {
   //     printf(">> rosrun knu_ros_lecture turtle_position_move [rot_degree] [trans_meter]\n");
   //     return 1;
   // }
   
   srand((unsigned int)time(0));
   double nRand; 

   sensor_msgs::LaserScan scan;
   nav_msgs::Odometry odom;
    // 옜 � 옜 옜
    Vec3d xyz, rpy;

    // 옜옜
    vector<Vec3d> trajectory;

    // LRF scan 옜
    vector<Vec3d> laserScanXY;
    vector< vector<Vec3d> > laserScanXYMulti;

    // Mat distance for grid
    const double dGridMaxDist = 4.0;

   double dRotation = 2;
   double dTranslation = 0.1;
   int a = 0;
   while(1){
//	nRand = rand() % 20 -10;
//	dRotation = nRand;
//	printf("rnadom : %lf\n", dRotation);
	ros::spinOnce();
	
	mutex[0].lock(); {
	  odom = g_odom;
	} mutex[0].unlock();

	 mutex[1].lock(); {
          scan = g_scan;
        } mutex[1].unlock();

	printf("odom : %f,%f \n", g_odom.pose.pose.position.x,
			g_odom.pose.pose.position.y);

    tf::Transform curr = getInitialTransformation();
    tf::Quaternion currQuater = curr.getRotation();
                double currHead = toDegree(atan2((2*currQuater[2]*currQuater[3]), (1-(2*(currQuater[2]*currQuater[2])))));
      

    float _dRatation = (float)((int)dRotation % 360);
    if(abs(_dRatation) > 180){
          if(dRotation > 0) dRotation = -(360-_dRatation);
          else dRotation = (360+_dRatation); }
    else
         dRotation = _dRatation;

    tf::Transform initialTransformation = getInitialTransformation();

    if(converScan2XYZs(scan, laserScanXY)==1) {

		doRotation(pub, initialTransformation, toRadian(dRotation), 0.8);	

		if(converScan2XYZs(scan, laserScanXY)==0) {

		
	}
    }else {
	 nRand = rand() % 20 -10;
         dRotation = nRand;
		if(nRand == 0) { nRand = 2;  }
	   doTranslation(pub, initialTransformation, dTranslation, 0.2);
	
    }   
	
}  
 
    return 0;
}
