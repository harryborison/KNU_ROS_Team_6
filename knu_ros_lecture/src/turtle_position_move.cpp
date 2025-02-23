#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <boost/thread/mutex.hpp>
#include <tf/tf.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define toRadian(degree)	((degree) * (M_PI / 180.))
#define toDegree(radian)	((radian) * (180. / M_PI))



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable
boost::mutex mutex;
nav_msgs::Odometry g_odom;
float pre_dAngleTurned;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// callback function
void
odomMsgCallback(const nav_msgs::Odometry &msg)
{
    // receive a '/odom' message with the mutex
    mutex.lock(); {
        g_odom = msg;
    } mutex.unlock();
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
    mutex.lock(); {
        odom = g_odom;
    } mutex.unlock();

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
    ros::Rate loopRate(300.0);



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
			printf("Rotation Finish\n");

            break;
        } else {
	    pre_dAngleTurned = dAngleTurned;
            //send the drive command
            pubTeleop.publish(baseCmd);

            // sleep!
            loopRate.sleep();
        }
    }

    // 횄횎짹창횊짯
    baseCmd.linear.x = 0.0;
    baseCmd.angular.z = 0.0;
    pubTeleop.publish(baseCmd);

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
    ros::Rate loopRate(300.0);

    while(ros::ok() && !bDone) {
        // 일단 callback 메시지를 받고!
        ros::spinOnce();

        // get current transformation
        tf::Transform currentTransformation = getCurrentTransformation();

        //see how far we've traveled
        tf::Transform relativeTransformation = initialTransformation.inverse() * currentTransformation ;
        double dDistMoved = relativeTransformation.getOrigin().length();

        // 종료조건 체
	//	printf("origin %f dDist %f dTrans %f origin2 %f\n", dDistMoved, fabs(dDistMoved), fabs(dTranslation), dTranslation);
		
        if(fabs(dDistMoved) >= fabs(dTranslation)) {
            bDone = true;
			printf("Translate Finsih\n");
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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
int main(int argc, char **argv)
{
    // ROS 초기화
    ros::init(argc, argv, "turtle_position_move");

    // Ros initialization
    ros::NodeHandle nhp, nhs;

    // Decleation of subscriber
    ros::Subscriber sub = nhs.subscribe("/odom", 100, &odomMsgCallback);

    // Create a publisher object
    ros::Publisher pub = nhp.advertise<geometry_msgs::Twist>("/cmd_vel", 100);

    // exception
    if(argc != 3) {
        printf(">> rosrun knu_ros_lecture turtle_position_move [rot_degree] [trans_meter]\n");
        return 1;
    }

    // 파라미터 받아오기
    double dRotation = atof(argv[1]);

    float _dRatation = (float)((int)dRotation % 360);
    double dTranslation = atof(argv[2]);


    if(abs(_dRatation) > 180){
          if(dRotation > 0) dRotation = -(360-_dRatation);
          else dRotation = (360+_dRatation); }
    else
         dRotation = _dRatation;


    // 로봇이 멈춰있는 상태(처음 상태)의 변환행렬 가져오기
    tf::Transform initialTransformation = getInitialTransformation();

    // 회전

    doRotation(pub, initialTransformation, toRadian(dRotation), 0.75);
//doRotation(pub, initialTransformation, 3.14*2, 0.75);

    // 이동
    doTranslation(pub, initialTransformation, dTranslation, 0.25);

    return 0;
}
