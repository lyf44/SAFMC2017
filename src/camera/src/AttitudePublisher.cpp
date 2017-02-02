#include "ros/ros.h"
#include "std_msgs/String.h"
#include "camera/IntArray.h"
#include "geometry_msgs/PoseStamped.h"
#include <mavros_msgs/CommandBool.h>
#include <sstream>
#include <camera/server.h>
#include <camera/PIDConfig.h>

int cord_x;
int cord_y;
int setpoint_x;
int setpoint_y;
float Kp,Ki,Kd,K1,K2,K3;
int error_x = 0;
int prev_error_x = 0;
int prev_prev_error_x = 0;
int error_y = 0;
int prev_error_y = 0;
int prev_prev_error_y = 0;
float outRoll,outPitch;
float delta_t = 0.1;
geometry_msgs::PoseStamped pose;

void reconfigure_callback(camera::PIDConfig &config, uint32_t level) {
    K1 = config.Kp + config.Ki*delta_t + config.Kd/delta_t;
    K2 = (-1)*config.Kp - 2*config.Kd/delta_t;
    K3 = config.Kd/delta_t;
}

void Callback(const camera::IntArray::ConstPtr& msg)
{
    cord_x = msg->data[0];
    cord_y = msg->data[1];

    /*PID start*/
    error_x = setpoint_x-cord_x;
    outPitch = outPitch + K1*error_x + K2*prev_error_x + K3*prev_prev_error_x;
    prev_prev_error_x = prev_error_x;
    prev_error_x = error_x;

    error_y = setpoint_y-cord_y;
    outRoll = outRoll + K1*error_y + K2*prev_error_y + K3*prev_prev_error_y;
    prev_prev_error_y = prev_error_y;
    prev_error_y = error_y;

    /*Convert Roll pitch into quaternion*/
    //TODO I am not sure this is correct
    double t0 = 1;//std::cos(yaw * 0.5f);
    double t1 = 0;//std::sin(yaw * 0.5f);
    double t2 = std::cos(outRoll * 0.5f);
    double t3 = std::sin(outRoll * 0.5f);
    double t4 = std::cos(outPitch * 0.5f);
    double t5 = std::sin(outPitch * 0.5f);

    pose.pose.orientation.x = t0 * t3 * t4 - t1 * t2 * t5;
    pose.pose.orientation.y = t0 * t2 * t5 + t1 * t3 * t4;
    pose.pose.orientation.z = t1 * t2 * t4 - t0 * t3 * t5;
    pose.pose.orientation.w = t0 * t2 * t4 + t1 * t3 * t5;

}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "attitudePublisher");

    ros::NodeHandle n;

    ros::Publisher pub = n.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_attitude/attitude",10);
    ros::Subscriber sub = n.subscribe("CentreCordInt", 1, Callback);//queue size =1 to force subscriber to execute according to the loop rate

    /*dynamic reconfigure initialization*/
    dynamic_reconfigure::Server<camera::PIDConfig> server;
    dynamic_reconfigure::Server<camera::PIDConfig>::CallbackType f;
    f = boost::bind(&reconfigure_callback, _1, _2);
    server.setCallback(f);

    /*get setpoint*/
    n.getParam("camera_resolution_x",setpoint_x);
    n.getParam("camera_resolution_y",setpoint_y);
    setpoint_x = setpoint_x/2;
    setpoint_y = setpoint_y/2;


    /*initialzation*/
    Kp = 1;
    Kd = 0;
    Ki = 0;
    K1 = Kp + Ki*delta_t + Kd/delta_t;
    K2 = -Kp - 2*Kd/delta_t;
    K3 = Kd/delta_t;

    /*specify loop rate*/
    ros::Rate loop_rate(10);

    /* test whether subscriber can get data at the specific rate
     * test result:
     *  by setting the subscriber queue size = 1, the callback will execute at the fixed frequency
     */
    while (ros::ok())
    {
        pub.publish(pose);
        ros::spinOnce();

        loop_rate.sleep();
    }
    return 0;
}
