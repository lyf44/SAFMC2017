#include "ros/ros.h"
#include "std_msgs/String.h"
#include "lyf_playground/IntArray.h"
#include "geometry_msgs/PoseStamped.h"
#include <mavros_msgs/CommandBool.h>
#include <sstream>

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

void Callback(const lyf_playground::IntArray::ConstPtr& msg)
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

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher pub = n.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_attitude/attitude",10);
  ros::Subscriber sub = n.subscribe("CentreCordInt", 1, Callback);

  n.getParam("camera_resolution_x",setpoint_x);
  n.getParam("camera_resolution_y",setpoint_y);
  setpoint_x = setpoint_x/2;
  setpoint_y = setpoint_y/2;
  n.getParam("Camera_Kp",Kp);
  n.getParam("Camera_Kd",Kd);
  n.getParam("Camera_Ki",Ki);

  //TEST
  //Kp = 1;
 // Kd = 0;
  //Ki = 0;

  K1 = Kp + Ki*delta_t + Kd/delta_t;
  K2 = -Kp - 2*Kd/delta_t;
  K3 = Kd/delta_t;


  ros::Rate loop_rate(10);

  /* test whether subscriber can get data at the specific rate
   * test result: 
   *  by setting the subscriber queue size = 1, the callback will execute at the fixed frequency
   */
  while (ros::ok())
  {
    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    pub.publish(pose);
    ros::spinOnce();

    loop_rate.sleep();
  }


  return 0;
}
