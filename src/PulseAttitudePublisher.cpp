#include "ros/ros.h"
#include "std_msgs/String.h"
#include "camera/IntArray.h"
#include <mavros_msgs/AttitudeTarget.h>
#include <sstream>
#include <dynamic_reconfigure/server.h>
#include <camera/PIDPulseConfig.h>
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
float Kp_p,Ki_p,Kd_p,K1_p,K2_p,K3_p,Kw_p;//K1_p_manual,K2_p_manual;//pitch
float Kp_r,Ki_r,Kd_r,K1_r,K2_r,K3_r,Kw_r;//K1_r_manual,K2_r_manual;//roll
int error_p = 0;
int prev_error_p = 0;
int prev_prev_error_p = 0;
int prev_error_p_wall = 0;
int error_r = 0;
int prev_error_r = 0;
int prev_error_r_wall = 0;
int prev_prev_error_r = 0;
float outRoll,outPitch;
float outRoll_rad, outPitch_rad;
float delta_t = 0.06667;
mavros_msgs::AttitudeTarget attTarget;
float maxAngle, maxAngle_neg;
bool IntegralWindup_pitch;
bool IntegralWindup_roll;
bool flag_Auto;
bool flag_pulse;
bool flag_longPulse;
bool flag_firstPulse_p,flag_firstPulse_r;
bool flag_clearPulse;
float current_yaw;
double roll,pitch,yaw;
bool CtrDetected;
int lost_counter;
int auto_counter;
int pulse_counter;
int pulseLength;
geometry_msgs::Quaternion msg_quat;

void reconfigure_callback(camera::PIDPulseConfig &config, uint32_t level) {
    Kp_p = (float) config.Kp_p/100;
    Kd_p = (float) config.Kd_p/10000;
    Ki_p = (float) config.Ki_p/10;
    K1_p = Kp_p + Ki_p*delta_t + Kd_p/delta_t;
    K2_p = (-1)*Kp_p - 2*Kd_p/delta_t;
    K3_p = Kd_p/delta_t;
    Kw_p = (float) config.Kw_p;
    Kp_r = (float) config.Kp_r/100;
    Kd_r = (float) config.Kd_r/10000;
    Ki_r = (float) config.Ki_r/10;
    K1_r = Kp_r + Ki_r*delta_t + Kd_r/delta_t;
    K2_r = (-1)*Kp_r - 2*Kd_r/delta_t;
    K3_r = Kd_r/delta_t;
    Kw_r = (float) config.Kw_r;

    maxAngle = (float) config.maxAngle;
    maxAngle_neg = maxAngle*(-1);
    pulseLength = (int) config.pulseLength;
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

/*
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
}*/

void Callback(const camera::IntArray::ConstPtr& msg)
{
    /* clear flag*/
    //IntegralWindup_pitch = false;
    //IntegralWindup_roll = false;

    /*get data*/
    cord_x = msg->data[0];
    cord_y = msg->data[1];
    
    if(flag_Auto){
        /***************************AUTO*************************/
        
        /*if center is detected*/
        if ((cord_x!=0)&&(cord_y!=0)){

           // ROS_INFO("Center detected");

            /*discrete PID start*/
            //when error x is positive, copter should move to its right 
            //roll angle is positive
            error_r = setpoint_x-cord_x;

            //when error y is positive,cotper should move backward,
            //pitch angel positive -> give pitch angle negative
            error_p = setpoint_y-cord_y;
                
    	    /*for testing*/
            //ROS_INFO("outPitchRaw: %f", outPitch);
            //ROS_INFO("outRollRaw: %f", outRoll);
            ROS_INFO("error_p: %d",error_p);
            ROS_INFO("error_r: %d",error_r);
 
           /*set flag*/
           CtrDetected = true;
           lost_counter = 0;
	    
           if(abs(error_r)<50 && abs(error_p)<50){
                attTarget.orientation.x = 0;
                attTarget.orientation.y = 0;
                attTarget.orientation.z = 0;
                attTarget.orientation.w = 1;
		ROS_INFO("outPitch: 0");
                ROS_INFO("outRoll: 0");
                /*clear outRoll and outPitch and errors*/
                outRoll = 0.0f;
                outPitch = 0.0f;
		prev_error_p_wall = error_p;
		prev_error_r_wall = error_r;
                error_p = 0.0f;
                prev_error_p = 0.0f;
                prev_prev_error_p = 0.0f;
                error_r = 0.0f;
                prev_error_r = 0.0f;
                prev_prev_error_r = 0.0f;

		flag_pulse = true;
		flag_firstPulse_r = true;
		flag_firstPulse_p = true;
		pulse_counter = 1;
 	   }else{
		/*output a pulse*/
	   	/*calculate long pulse flag*/
                if (pulse_counter == pulseLength){
                    flag_longPulse = false;
           	}

	       /*calculate pulse flag*/
               if (pulse_counter==1){
                   flag_pulse = true;
		   flag_clearPulse = true;
               }else{
		   flag_pulse = false;
	       }
		if (pulse_counter == 3){
		   flag_clearPulse = true;
		}
               pulse_counter++;
		if (pulse_counter>=8){
		    pulse_counter=1;
		}

	       if (flag_pulse){	
		//compensation for camera tilt angle,approximated
	    	//error_r = error_r - 5 * outRoll;
		if (abs(error_r)>50){
		    outRoll = outRoll + K1_r*error_r + K2_r*prev_error_r + K3_r * prev_prev_error_r;
		    if (flag_firstPulse_r){
            	        outRoll = outRoll + Kw_r * (error_r - prev_error_r_wall);
			flag_longPulse = true;
			flag_firstPulse_r = false;
		    }
		    prev_prev_error_r = prev_error_r;
	    	    prev_error_r = error_r;
		}else{
                    outRoll = 0.0f;
		    error_r = 0.0f;
                    prev_error_r = 0.0f;
                    prev_prev_error_r = 0.0f;
		}
	    	//error_p = error_p - 5 * outPitch;
            	if (abs(error_p)>50){
		    outPitch = outPitch + K1_p*error_p + K2_p*prev_error_p + K3_p * prev_prev_error_p;
	    	    if(flag_firstPulse_p){
			outPitch = outPitch + Kw_p * (error_p - prev_error_p_wall);
			flag_longPulse = true;
			flag_firstPulse_p = false;
		    }
		    prev_prev_error_p = prev_error_p;
	    	    prev_error_p = error_p;
		}else{
                    outPitch = 0.0f;
                    error_p = 0.0f;
                    prev_error_p = 0.0f;
                    prev_prev_error_p = 0.0f;
		}
            	/*limit output*/
            	if (outPitch > maxAngle){
                    outPitch = maxAngle;
                    //IntegralWindup_pitch = true;
            	}
            	if (outPitch < maxAngle_neg){
                    outPitch = maxAngle_neg;
                   // IntegralWindup_pitch = true;
                }
                if ((fabs(outPitch)<1) && (fabs(outPitch)>0.0001)){
                    if(outPitch>0){
 			outPitch = 1;
		    }else{
		        outPitch = -1;
		    }
                }
                if (outRoll > maxAngle){
                    outRoll = maxAngle;
                   // IntegralWindup_roll = true;
                }
                if (outRoll < maxAngle_neg){
                    outRoll = maxAngle_neg;
                   // IntegralWindup_roll = true;
                }
                if ((fabs(outRoll)<1) && (fabs(outRoll)>0.0001)){
                    if(outRoll>0){
		        outRoll = 1;
		    }else{
			outRoll = -1;
		    }
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
		
		/*if ((abs(error_r)>40) || (abs(error_p)>40)){
		    flag_longPulse = true;
		}else{
		    flag_longPulse = false;
		}*/
	       }
	       if(flag_clearPulse){
		 if (flag_longPulse){
		    ROS_INFO("outPitch: %f", outPitch);
		    ROS_INFO("outRoll: %f", outRoll);
		 }else{ 
		    /*should not output pulse*/
		    attTarget.orientation.x = 0;
            	    attTarget.orientation.y = 0;
            	    attTarget.orientation.z = 0;
            	    attTarget.orientation.w = 1;
		    ROS_INFO("outPitch: 0");
		    ROS_INFO("outRoll: 0");
		}
	       }
            }
            ROS_INFO("realPitch: %f",pitch);
            ROS_INFO("realRoll: %f",roll);

        }else{
            /*******************Center not detected*************************/
            /*if center was detected, try to go back to find center again, this should only happen in AUTO mode as CtrDetected flag is set in auto mode*/ 
            if (CtrDetected && lost_counter<30){
                //dont clear outRoll and outPitch,

            	ROS_INFO("Center lost. trying to recover. %d", lost_counter);
            	lost_counter++;

            	if (flag_pulse){
                    //trying to go back at a large angle = 1.5*original angle.
                    outPitch_rad = 1.5*outPitch*3.1415926/180*(-1);
                    outRoll_rad = 1.5*outRoll*3.1415926/180;
                    ROS_INFO("outPitch: %f",(outPitch*1.5));
                    ROS_INFO("outRoll: %f",(outRoll*1.5));

                   /*Convert Roll pitch into quaternion*/
                   /*this is same as tf getquaternionfromeuler method*/
/*                   double t0 = std::cos(current_yaw * 0.5f);
                   double t1 = std::sin(current_yaw * 0.5f);
                   double t2 = std::cos(outRoll_rad * 0.5f);
                   double t3 = std::sin(outRoll_rad * 0.5f);
                   double t4 = std::cos(outPitch_rad * 0.5f);
                   double t5 = std::sin(outPitch_rad * 0.5f);

                   attTarget.orientation.x = t0 * t3 * t4 - t1 * t2 * t5;
                   attTarget.orientation.y = t0 * t2 * t5 + t1 * t3 * t4;
                   attTarget.orientation.z = t1 * t2 * t4 - t0 * t3 * t5;
                   attTarget.orientation.w = t0 * t2 * t4 + t1 * t3 * t5;
*/
               }else{
                  /*should not output pulse*/
                  attTarget.orientation.x = 0;
                  attTarget.orientation.y = 0;
                  attTarget.orientation.z = 0;
                  attTarget.orientation.w = 1;
                  ROS_INFO("outPitch: 0");
                  ROS_INFO("outRoll: 0");
               }
	       ROS_INFO("realPitch: %f",pitch);
               ROS_INFO("realRoll: %f",roll);

               return;
           }else{
	      /********************Center totally lost*********************/
              /*else center is not detected or totally lost. No pitch or roll!*/
              ROS_INFO("Center totally lost");
              /*clear flag*/
              CtrDetected = false;
              /*publish zero command*/
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
    }else{
        /***************************MANUAL*************************/
        /*if center is detected*/
        if ((cord_x!=0)&&(cord_y!=0)){

            // ROS_INFO("Center detected");

            /*discrete PID start*/
            //when error x is positive, copter should move to its right 
            //roll angle is positive
            error_r = setpoint_x-cord_x;

            //when error y is positive,cotper should move backward,
            //pitch angel positive -> give pitch angle negative
            error_p = setpoint_y-cord_y;

            ROS_INFO("error_p: %d",error_p);
            ROS_INFO("error_r: %d",error_r);

        }else{
        
            /*else center is not detected or totally lost. No pitch or roll!*/
            ROS_INFO("Center totally lost");
        }
        /*clear flag*/
        CtrDetected = false;
	/*publish zero command*/
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
   // ros::Subscriber IMU_sub = n.subscribe("mavros/imu/data",1,IMUCallback); 

    /*dynamic reconfigure initialization*/
    dynamic_reconfigure::Server<camera::PIDPulseConfig> server;
    dynamic_reconfigure::Server<camera::PIDPulseConfig>::CallbackType f;
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
    pitch = 0.0f;
    roll = 0.0f;
    
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
    ros::Rate loop_rate(10);
    
    pulse_counter = 0;
    pulseLength = 2;
    flag_pulse = true;
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
