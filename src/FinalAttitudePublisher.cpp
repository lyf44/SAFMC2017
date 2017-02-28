#include "ros/ros.h"
#include "std_msgs/String.h"
#include "camera/IntArray.h"
#include <mavros_msgs/AttitudeTarget.h>
#include <sstream>
#include <dynamic_reconfigure/server.h>
#include <camera/PIDFinalConfig.h>
#include "math.h"
#include "geometry_msgs/Quaternion.h"
#include "sensor_msgs/Imu.h"
#include "tf/transform_datatypes.h"
#include "geometry_msgs/Vector3.h"
#include "string"
#include "mavros_msgs/State.h"

int cord_x;
int cord_y;
float current_yaw;
/*******PID**************/
int setpoint_x;
int setpoint_y;
float Kp_p,Kd_p,K1_p,K2_p,K3_p,Kw_p,Kw_p_wall;
float Kp_r,Kd_r,K1_r,K2_r,K3_r,Kw_r,Kw_r_wall;
int error_p;
int prev_error_p;
int prev_prev_error_p;
int prev_error_p_wall;
int error_r;
int prev_error_r;
int prev_error_r_wall;
int prev_prev_error_r;
float outRoll,outPitch;
float prev_outRoll,prev_outPitch;
float outRoll_rad, outPitch_rad;
float delta_t = 0.1;
float maxAngle, maxAngle_neg;

/*************ROS********************/
mavros_msgs::AttitudeTarget attTarget;
geometry_msgs::Quaternion msg_quat;


/********flags************/
bool flag_AUTO;
bool flag_firstPulse;
bool flag_zeroAtt;
bool flag_sendError;
bool flag_ctrDetected;

/************counter**********/
int pulse_counter;
int pulse_counter_wall;
int lost_counter;

/***********functions**********/
void clearPID();
void clearWallPID();

/*****************************ROS CALLBACK**********************************/
void reconfigure_callback(camera::PIDFinalConfig &config, uint32_t level) {
    Kp_p = (float) config.Kp_p/100;
    Kd_p = (float) config.Kd_p/10000;
    Kw_p = (float) config.Kw_p;
    Kw_p_wall = (float) config.Kw_p_wall;
    K1_p = Kp_p + Kd_p/delta_t;
    K2_p = (-1)*Kp_p - 2*Kd_p/delta_t;
    K3_p = Kd_p/delta_t;
    Kp_r = (float) config.Kp_r/100;
    Kd_r = (float) config.Kd_r/10000;
    Kw_r = (float) config.Kw_r;
    Kw_r_wall = (float) config.Kw_r_wall;
    K1_r = Kp_r + Kd_r/delta_t;
    K2_r = (-1)*Kp_r - 2*Kd_r/delta_t;
    K3_r = Kd_r/delta_t;
    maxAngle = (float) config.maxAngle;
    maxAngle_neg = maxAngle*(-1);
    clearPID();
    clearWallPID();
    ROS_INFO("parameter changed");
}

void Callback(const camera::IntArray::ConstPtr& msg)
{
    ROS_INFO("ctr message received");
    /*get data*/
    cord_x = msg->data[0];
    cord_y = msg->data[1];

    if ((cord_x!=0)&&(cord_y!=0)){
        flag_ctrDetected = true;
	lost_counter = 0;
    }else{
        flag_ctrDetected = false;
    }
}

void stateCallback(const mavros_msgs::State::ConstPtr& msg){
    std::string mode = msg->mode;
    if (mode.compare("GUIDED_NOGPS")==0){
        flag_AUTO = true;
    }else{
        flag_AUTO = false;
    }
}

/**********************user defined functions*********************/
void clearPID(){
    prev_outPitch = 0.0f;
    prev_outRoll = 0.0f;
    prev_error_p = 0;
    prev_prev_error_p = 0;
    prev_error_r = 0;
    prev_prev_error_r = 0;
    pulse_counter = 0;
}

void clearWallPID(){
    pulse_counter_wall = 0;
    prev_error_r_wall = 0;
    prev_error_p_wall = 0;
}

void sendLongPulseRP(int pulse_counter){
    if (pulse_counter==1){
        outPitch = K1_p*error_p;
        prev_prev_error_p = prev_error_p;
        prev_error_p = error_p;
        prev_outPitch = outPitch;
	if (abs(error_p)>50){
	    outPitch = outPitch +  Kw_p * (error_p - prev_error_p_wall);
       	}
        outRoll = K1_r*error_r;
        prev_prev_error_r = prev_error_r;
        prev_error_r = error_r;
        prev_outRoll = outRoll;
	if (abs(error_r)>50){
	    outRoll = outRoll+ Kw_r * (error_r - prev_error_r_wall);
	}
    }
    else if (pulse_counter<4){
	//outPitch = prev_outPitch;
	//outRoll = prev_outRoll;
    }
    else if (pulse_counter>=4){
        flag_zeroAtt = true;
    }
    return;
}

void sendNormalPulseRP(int pulse_counter){
    if (pulse_counter==1){
	if (abs(error_p)>50){
        outPitch = prev_outPitch + K1_p*error_p + K2_p*prev_error_p + K3_p * prev_prev_error_p;
        prev_prev_error_p = prev_error_p;
        prev_error_p = error_p;
        prev_outPitch = outPitch;
	}else{
	    outPitch = 0.0f;
	    prev_outPitch = 0.0f;
	    prev_error_p = 0;
	    prev_prev_error_p = 0;
	}
	if (abs(error_r)>50){
        outRoll = prev_outRoll + K1_r*error_r + K2_r*prev_error_r + K3_r * prev_prev_error_r;
        prev_prev_error_r = prev_error_r;
        prev_error_r = error_r;
        prev_outRoll = outRoll;}
	else{
	    outRoll = 0.0f;
	    prev_outRoll = 0.0f;
	    prev_error_r = 0;
	    prev_prev_error_r = 0;
	}
    }
    else if (pulse_counter<3){
	outPitch = prev_outPitch;
	outRoll = prev_outRoll;
    }
    else if (pulse_counter>=3){
        flag_zeroAtt = true;
    }
    return;
}

/************************************MAIN***********************************/
int main(int argc, char **argv)
{
    ros::init(argc, argv, "AttitudePublisher");

    ros::NodeHandle n;

    ros::Publisher pub = n.advertise<mavros_msgs::AttitudeTarget>("mavros/setpoint_raw/attitude",1);
    ros::Subscriber sub = n.subscribe("CentreCordInt",1, Callback);//queue size =1 to force subscriber to execute according to the loop rate
    ros::Subscriber state_sub = n.subscribe("mavros/state",1,stateCallback);

    /*dynamic reconfigure initialization*/
    dynamic_reconfigure::Server<camera::PIDFinalConfig> server;
    dynamic_reconfigure::Server<camera::PIDFinalConfig>::CallbackType f;
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


    /*initialize variable*/
    pulse_counter = 1;
    pulse_counter_wall = 1;
    clearPID();
    clearWallPID();

    /*specify loop rate*/
    ros::Rate loop_rate(10);

    /* test whether subscriber can get data at the specific rate
     * test result:
     *  by setting the subscriber queue size = 1, the callback will execute at the fixed frequency
     */
    while (ros::ok()){
        /*clear flag*/
        flag_zeroAtt = false;//should always give command unless this flag is set
        flag_sendError = true;//should always send error unless this flag is set
        //outPitch = 0.0f;//previous command is stored in prev_outPitch;
        //outRoll = 0.0f;//previous command is stored in prev_outRoll;
	int temp, temp1;

        if (flag_AUTO){
            /******************AUTO***********************/
            if (flag_ctrDetected){
		error_r = setpoint_x - cord_x;
		error_p = setpoint_y - cord_y;
                /*****************Center found**************/
                if ((abs(error_r)<=50) && (abs(error_p)<=50)){
                    /******************within area****************/
                    /* clear pulse counter and flag */
                    //pulse_counter = 1;
                    flag_firstPulse = true;
                    clearPID();

		    ROS_INFO("pulse_counter_wall: %d", pulse_counter_wall);
                    /* feedforward attitude command*/
                    pulse_counter_wall++;
                    if (pulse_counter_wall>=5){
                        pulse_counter_wall=1;
                    }
                    if(pulse_counter_wall==1){
                        outPitch = Kw_p_wall * (error_p - prev_error_p_wall);
                        outRoll = Kw_r_wall * (error_r - prev_error_r_wall);
                    }else if(pulse_counter_wall>=2){
                        flag_zeroAtt = true;
                    }
                    prev_error_r_wall = temp;
                    prev_error_p_wall = temp1;
		    temp = error_r;
		    temp1 = error_p;

                }else{
                    /*****************outside area***************/

                    /*calculate pulse flag*/
                    pulse_counter++;
                    if (pulse_counter>=7){
                        pulse_counter=1;
                        /*clear first pulse flag*/
                        if(flag_firstPulse){
                            flag_firstPulse = false;
                        }
                    }

		    ROS_INFO("pulse_counter: %d", pulse_counter);
                    if (flag_firstPulse){
                        sendLongPulseRP(pulse_counter);//set flag_zeroAtt if pulse_counter>=4
                    }else{
                        sendNormalPulseRP(pulse_counter);//set flag_zeroAtt if pulse_counter>=3
                    }
		    
		    /**********clear wall PID*****************/
		    clearWallPID(); //only after prev_error_r_wall and prev_error_p_wall are used.
                }
            }else{
                /****************Center not found***********/
                if (lost_counter<15){
                    //dont clear outRoll and outPitch,
            	    ROS_INFO("Center lost. trying to recover. %d", lost_counter);
            	    lost_counter++;
		    flag_zeroAtt = true;
                }else{
    		    flag_zeroAtt= true;
                    flag_sendError = false;
		}
            }
        }else{
            /****************MANUAL*****************/
	    flag_firstPulse = false;
	    if (flag_ctrDetected){
                error_r = setpoint_x - cord_x;
                error_p = setpoint_y - cord_y;
	    }
            clearWallPID();
            clearPID();
            flag_zeroAtt = true;
            if(!flag_ctrDetected){
                flag_sendError = false;
            }
        }

        /*************calculate AttTarget and ROSINFO*************/
        if (flag_zeroAtt){
            attTarget.orientation.x = 0;
            attTarget.orientation.y = 0;
            attTarget.orientation.z = 0;
            attTarget.orientation.w = 1;
        }else{
	    /***********limit output**********/
            	if (outPitch > maxAngle){
                    outPitch = maxAngle;
                    //IntegralWindup_pitch = true;
            	}
            	if (outPitch < maxAngle_neg){
                    outPitch = maxAngle_neg;
                   // IntegralWindup_pitch = true;
                }
                if (outRoll > maxAngle){
                    outRoll = maxAngle;
                   // IntegralWindup_roll = true;
                }
                if (outRoll < maxAngle_neg){
                    outRoll = maxAngle_neg;
                   // IntegralWindup_roll = true;
                }
            /*convert to radian*/
            outPitch_rad = outPitch*3.1415926/180;//pitch is reverse in pixhawk;
            outRoll_rad = outRoll*3.1415926/180*(-1);

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
        }

        /******************ROS_INFO for log*************************/
        if (flag_AUTO){
            ROS_INFO("AUTO");
        }else{
            ROS_INFO("MANUAL");
        }
        if (flag_ctrDetected){
            if (flag_sendError){
                ROS_INFO("error_p: %d",error_p);
                ROS_INFO("error_r: %d",error_r);
            }
            if (flag_zeroAtt){
                ROS_INFO("outPitch: 0");
                ROS_INFO("outRoll: 0");
		ROS_INFO("realPitch: 0");
		ROS_INFO("realRoll: 0");
            }else{
                ROS_INFO("outPitch: %f",outPitch);
                ROS_INFO("outRoll: %f",outRoll);
		ROS_INFO("realPitch: 0");
		ROS_INFO("realRoll: 0");
            }
	    
        }else{
            ROS_INFO("Center totally lost");
        }


        /******************publish mavros attitude setpoint*********************/
        pub.publish(attTarget);

        ros::spinOnce();

        loop_rate.sleep();

    }
    return 0;
}
