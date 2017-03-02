#include "ros/ros.h"
#include "std_msgs/String.h"
#include "camera/IntArray.h"
#include <mavros_msgs/AttitudeTarget.h>
#include <sstream>
#include <dynamic_reconfigure/server.h>
#include <camera/PIDConfig.h>
#include "math.h"
#include "geometry_msgs/Quaternion.h"
#include "sensor_msgs/Imu.h"
#include "tf/transform_datatypes.h"
#include "geometry_msgs/Vector3.h"
#include "string"
#include "mavros_msgs/State.h"

/* naming convention explanation:
 *   _p means pitch and _r means roll.
 *   _x means camera x axis and _y means camera y axis.
*/

int cord_x;
int cord_y;
int setpoint_x;
int setpoint_y;
float Kp_p,Ki_p,Kd_p,K1_p,K2_p,K3_p;//K1_p_manual,K2_p_manual;//pitch
float Kp_r,Ki_r,Kd_r,K1_r,K2_r,K3_r;//K1_r_manual,K2_r_manual;//roll
int error_p = 0;
int prev_error_p = 0;
int prev_prev_error_p = 0;
int error_r = 0;
int prev_error_r = 0;
int prev_prev_error_r = 0;
float outRoll,outPitch;
float outRoll_rad, outPitch_rad;
float delta_t = 0.06667;
mavros_msgs::AttitudeTarget attTarget;
float maxAngle, maxAngle_neg;
bool IntegralWindup_pitch;
bool IntegralWindup_roll;
bool flag_Auto;
float current_yaw;
double roll,pitch,yaw;
bool CtrDetected;
int lost_counter;
int auto_counter;
geometry_msgs::Quaternion msg_quat;

void reconfigure_callback(camera::PIDConfig &config, uint32_t level) {
    Kp_p = (float) config.Kp_p/100;
    Kd_p = (float) config.Kd_p/10000;
    Ki_p = (float) config.Ki_p/10;
    K1_p = Kp_p + Ki_p*delta_t + Kd_p/delta_t;
    K2_p = (-1)*Kp_p - 2*Kd_p/delta_t;
    K3_p = Kd_p/delta_t;
    Kp_r = (float) config.Kp_r/100;
    Kd_r = (float) config.Kd_r/10000;
    Ki_r = (float) config.Ki_r/10;
    K1_r = Kp_r + Ki_r*delta_t + Kd_r/delta_t;
    K2_r = (-1)*Kp_r - 2*Kd_r/delta_t;
    K3_r = Kd_r/delta_t;

    maxAngle = (float) config.maxAngle;
    maxAngle_neg = maxAngle*(-1);
    outRoll = 0.0f;
    outPitch = 0.0f;
    error_r = 0;
    error_p = 0;
    prev_error_r = 0;
    prev_error_p = 0;
    prev_prev_error_r = 0;
    prev_prev_error_p = 0;
    ROS_INFO("parameter changed:");
    ROS_INFO("Kp_p: %f Kd_p: %f Ki_p: %f maxAngle: %f",Kp_p,Kd_p,Ki_p,maxAngle);
    ROS_INFO("Kp_r: %f Kd_r: %f Ki_r: %f maxAngle: %f",Kp_r,Kd_r,Ki_r,maxAngle);
}

void stateCallback(const mavros_msgs::State::ConstPtr& msg){
    std::string mode = msg->mode;
    if (mode.compare("GUIDED_NOGPS")==0){
	ROS_INFO("AUTO");
	flag_Auto = true;
    }else{
	ROS_INFO("MANUAL");
        flag_Auto = false;
    }
}


void IMUCallback(const sensor_msgs::Imu::ConstPtr& msg){
    msg_quat = msg->orientation;
    // the incoming geometry_msgs::Quaternion is transformed to a tf::Quaterion
    tf::Quaternion quat;
    tf::quaternionMsgToTF(msg_quat, quat);
    // the tf::Quaternion has a method to acess roll pitch and yaw
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    //current_yaw = (float) yaw;
    roll = roll/3.1415926*180;
    pitch = pitch/3.1415926*180;
}

void Callback(const camera::IntArray::ConstPtr& msg)
{
    /* clear flag*/
    //IntegralWindup_pitch = false;
    //IntegralWindup_roll = false;

    /*get data*/
    cord_x = msg->data[0];
    cord_y = msg->data[1];
    
    /*if center is detected*/
    if ((cord_x!=0)&&(cord_y!=0)){

       // ROS_INFO("Center detected");

        /*set flag*/
        CtrDetected = true;
        lost_counter = 0;

        /*if the system has not entered guided mode, remove Ki so that error would not accumulate*/

        /*discrete PID start*/
        //when error x is positive, copter should move to its right 
        //roll angle is positive
        error_r = setpoint_x-cord_x;
        
        //when error y is positive,cotper should move backward,
        //pitch angel positive -> give pitch angle negative
        error_p = setpoint_y-cord_y;

	if(flag_Auto){
	    //compensation for camera tilt angle,approximated
	    error_r = error_r - 5 * outRoll;
        outRoll = outRoll + K1_r*error_r + K2_r*prev_error_r + K3_r*prev_prev_error_r;
	    prev_prev_error_r = prev_error_r;
	    prev_error_r = error_r;

	    //compensation for camera tilt angle, approximated
	    error_p = error_p - 5 * outPitch;
            outPitch = outPitch + K1_p*error_p + K2_p*prev_error_p + K3_p*prev_prev_error_p;
	    prev_prev_error_p = prev_error_p;
	    prev_error_p = error_p;

            /*for testing*/
            //ROS_INFO("outPitchRaw: %f", outPitch);
            //ROS_INFO("outRollRaw: %f", outRoll);
            ROS_INFO("error_p: %d",error_p);
    	    ROS_INFO("error_r: %d",error_r);

            /*limit output*/
            if (outPitch > maxAngle){
                outPitch = maxAngle;
               // IntegralWindup_pitch = true;
            }
            if (outPitch < maxAngle_neg){
                outPitch = maxAngle_neg;
               // IntegralWindup_pitch = true;
            }
            if (fabs(outPitch)<0.01){
                outPitch = 0.0f;
            }
            if (outRoll > maxAngle){
                outRoll = maxAngle;
               // IntegralWindup_roll = true;
            }
            if (outRoll < maxAngle_neg){
                outRoll = maxAngle_neg;
               // IntegralWindup_roll = true;
            }
            if (fabs(outRoll)<0.01){
                outRoll = 0.0f;
            }
    
            /*Integralwindup*/
            /* only copy error when integral windup does not happen*/
            /*
            if (!IntegralWindup_pitch){
               prev_prev_error_x = prev_error_x;
               prev_error_x = error_x;
            }
            if (!IntegralWindup_roll){
               prev_prev_error_y = prev_error_y;
               prev_error_y = error_y;
            }*/

            /*for testing*/
            ROS_INFO("outPitch: %f",outPitch);
            ROS_INFO("outRoll: %f",outRoll);
	    ROS_INFO("realPitch: %f",pitch);
	    ROS_INFO("realRoll: %f",roll);

            /*test*/
            // outRoll = 0;
            // outPitch = 2;
            /*convert to radian*/
            outPitch_rad = outPitch*3.1415926/180*(-1);//pitch is reverse in pixhawk
            outRoll_rad = outRoll*3.1415926/180;

            /*Convert Roll pitch into quaternion*/
            /*this is same as tf getquaternionfromeuler method*/
            double t0 = std::cos(current_yaw * 0.5f);
            double t1 = std::sin(current_yaw * 0.5f);
            double t2 = std::cos(outRoll_rad * 0.5f);
            double t3 = std::sin(outRoll_rad * 0.5f);
            double t4 = std::cos(outPitch_rad * 0.5f);
            double t5 = std::sin(outPitch_rad * 0.5f);

            attTarget.orientation.x = t0 * t3 * t4 - t1 * t2 * t5;
            attTarget.orientation.y = t0 * t2 * t5 + t1 * t3 * t4;
            attTarget.orientation.z = t1 * t2 * t4 - t0 * t3 * t5;
            attTarget.orientation.w = t0 * t2 * t4 + t1 * t3 * t5;
	}else{
	    /*****************manual*********************/
	    attTarget.orientation.x = 0;
	    attTarget.orientation.y = 0;
	    attTarget.orientation.z = 0;
	    attTarget.orientation.w = 1;
	    ROS_INFO("error_p: %d",error_p);
            ROS_INFO("error_r: %d",error_r);
            /*clear outRoll and outPitch and errors*/
            outRoll = 0.0f;
            outPitch = 0.0f;
            error_p = 0.0f;
            prev_error_p = 0.0f;
            prev_prev_error_p = 0.0f;
            error_r = 0.0f;
            prev_error_r = 0.0f;
            prev_prev_error_r = 0.0f;
	}
    }else{
        /*if center was detected, try to go back to find center again*/ 
        if (CtrDetected && lost_counter<30){
            //dont clear outRoll and outPitch,

            ROS_INFO("Center lost. trying to recover. %d", lost_counter);
            lost_counter++;

            //trying to go back at a large angle = 1.5*original angle.
	    outPitch_rad = 1.5*outPitch*3.1415926/180*(-1);
	    outRoll_rad = 1.5*outRoll*3.1415926/180;
            ROS_INFO("outPitch: %f",(outPitch*1.5));
            ROS_INFO("outRoll: %f",(outRoll*1.5));
            ROS_INFO("realPitch: %f",pitch);
            ROS_INFO("realRoll: %f",roll);

            /*Convert Roll pitch into quaternion*/
            /*this is same as tf getquaternionfromeuler method*/
            double t0 = std::cos(current_yaw * 0.5f);
            double t1 = std::sin(current_yaw * 0.5f);
            double t2 = std::cos(outRoll_rad * 0.5f);
            double t3 = std::sin(outRoll_rad * 0.5f);
            double t4 = std::cos(outPitch_rad * 0.5f);
            double t5 = std::sin(outPitch_rad * 0.5f);

            attTarget.orientation.x = t0 * t3 * t4 - t1 * t2 * t5;
            attTarget.orientation.y = t0 * t2 * t5 + t1 * t3 * t4;
            attTarget.orientation.z = t1 * t2 * t4 - t0 * t3 * t5;
            attTarget.orientation.w = t0 * t2 * t4 + t1 * t3 * t5;

            return;
        } 
        
        /*else center is not detected or totally lost. No pitch or roll!*/
        ROS_INFO("Center totally lost");
        
        CtrDetected = false;
        
        attTarget.orientation.x = 0;
        attTarget.orientation.y = 0;
        attTarget.orientation.z = 0;
        attTarget.orientation.w = 1;
        
        /*clear outRoll and outPitch and errors*/
        outRoll = 0.0f;
        outPitch = 0.0f;
        error_p = 0.0f;
        prev_error_p = 0.0f;
        prev_prev_error_p = 0.0f;
        error_r = 0.0f;
        prev_error_r = 0.0f;
        prev_prev_error_r = 0.0f;
   } 
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "AttitudePublisher");

    ros::NodeHandle n;

    ros::Publisher pub = n.advertise<mavros_msgs::AttitudeTarget>("mavros/setpoint_raw/attitude",1);
    ros::Subscriber sub = n.subscribe("CentreCordInt",1, Callback);//queue size =1 to force subscriber to execute according to the loop rate
    ros::Subscriber state_sub = n.subscribe("mavros/state",1,stateCallback); 
    ros::Subscriber IMU_sub = n.subscribe("mavros/imu/data",1,IMUCallback); 

    /*dynamic reconfigure initialization*/
    dynamic_reconfigure::Server<camera::PIDConfig> server;
    dynamic_reconfigure::Server<camera::PIDConfig>::CallbackType f;
    f = boost::bind(&reconfigure_callback, _1, _2);
    server.setCallback(f);

    /*get setpoint*/
    n.getParam("camera_resolution_x",setpoint_x);
    n.getParam("camera_resolution_y",setpoint_y);
    setpoint_x = 120;
    setpoint_y = 120;

    /*initialzation*/
    //Kp = 1;
    //Kd = 0;
    //Ki = 0;
    //K1 = Kp + Ki*delta_t + Kd/delta_t;
    //K2 = -Kp - 2*Kd/delta_t;
    //K3 = Kd/delta_t;
    outRoll = 0.0f;
    outPitch = 0.0f;
    current_yaw = 0.0f;
    
    /*initialzie message*/
    geometry_msgs::Vector3 bodyRate;
    bodyRate.x = 0.0f;
    bodyRate.y = 0.0f;
    bodyRate.z = 0.0f;
    
    attTarget.orientation.x = 0;
    attTarget.orientation.y = 0;
    attTarget.orientation.z = 0;
    attTarget.orientation.w = 1;
    attTarget.type_mask = 3;//ignore roll,pitch rate,use yaw rate
    attTarget.body_rate = bodyRate;
    attTarget.thrust = 0.5f;

    CtrDetected = false;
    lost_counter = 0;
    /*specify loop rate*/
    ros::Rate loop_rate(15);

    /* test whether subscriber can get data at the specific rate
     * test result:
     *  by setting the subscriber queue size = 1, the callback will execute at the fixed frequency
     */
    while (ros::ok())
    {
       /*publish mavros attitude setpoint*/
       pub.publish(attTarget);
       
       ros::spinOnce();//CtrDetected flag should be set in this step

       loop_rate.sleep();
    }
    return 0;
}
