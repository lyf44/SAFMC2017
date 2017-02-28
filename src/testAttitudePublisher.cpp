#include "ros/ros.h"
#include "std_msgs/String.h"
#include "camera/IntArray.h"
#include <mavros_msgs/AttitudeTarget.h>
#include <sstream>
#include <dynamic_reconfigure/server.h>
#include <camera/AttConfig.h>
#include "math.h"
#include "geometry_msgs/Quaternion.h"
#include "tf/transform_datatypes.h"
#include "geometry_msgs/Vector3.h"

/* naming convention explanation:
 *   _p means pitch and _r means roll.
 *   _x means camera x axis and _y means camera y axis.
*/

float outRoll,outPitch;
float outRoll_rad, outPitch_rad;
mavros_msgs::AttitudeTarget attTarget;
float current_yaw;;
geometry_msgs::Quaternion msg_quat;

void reconfigure_callback(camera::AttConfig &config, uint32_t level) {
    outRoll = (float) config.roll;
    outPitch = (float) config.pitch;
    ROS_INFO("parameter changed:");
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "testAttitudePublisher");

    ros::NodeHandle n;

    ros::Publisher pub = n.advertise<mavros_msgs::AttitudeTarget>("mavros/setpoint_raw/attitude",10);

    /*dynamic reconfigure initialization*/
    dynamic_reconfigure::Server<camera::AttConfig> server;
    dynamic_reconfigure::Server<camera::AttConfig>::CallbackType f;
    f = boost::bind(&reconfigure_callback, _1, _2);
    server.setCallback(f);

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

    /*specify loop rate*/
    ros::Rate loop_rate(10);

    double t0,t1,t2,t3,t4,t5;
    /* test whether subscriber can get data at the specific rate
     * test result:
     *  by setting the subscriber queue size = 1, the callback will execute at the fixed frequency
     */
    while (ros::ok())
    {
        /*for testing*/
        ROS_INFO("outPitch: %f",outPitch);
        ROS_INFO("outRoll: %f",outRoll);

        /*convert to radian*/
        outPitch_rad = outPitch*3.1415926/180;
        outRoll_rad = outRoll*3.1415926/180;

        t0 = std::cos(current_yaw * 0.5f);
        t1 = std::sin(current_yaw * 0.5f);
        t2 = std::cos(outRoll_rad * 0.5f);
        t3 = std::sin(outRoll_rad * 0.5f);
        t4 = std::cos(outPitch_rad * 0.5f);
        t5 = std::sin(outPitch_rad * 0.5f);

        attTarget.orientation.x = t0 * t3 * t4 - t1 * t2 * t5;
        attTarget.orientation.y = t0 * t2 * t5 + t1 * t3 * t4;
        attTarget.orientation.z = t1 * t2 * t4 - t0 * t3 * t5;
        attTarget.orientation.w = t0 * t2 * t4 + t1 * t3 * t5;

       /*publish mavros attitude setpoint*/
       pub.publish(attTarget);
 
       ros::spinOnce();//CtrDetected flag should be set in this step

       loop_rate.sleep();
    }
    return 0;
}
