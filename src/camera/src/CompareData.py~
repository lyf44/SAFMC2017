#!/usr/bin/env python
import rospy
from vicon_xb.msg import viconPoseMsg
from lyf_playground.msg import FloatArray

vicon_x = 0
vicon_y = 0

def callback_cameraPose(data):
    cameraPose = data.data
    camera_x = cameraPose[0]
    camera_y = cameraPose[1]
    
    # not sure if this syntax is correct
    
    if vicon_x == 0 or vicon_y == 0:
        print("something is wrong, vicon data not collected")
    delta_x = camera_x - vicon_x;
    delta_y = camera_y - vicon_y;    
    rospy.loginfo("camData: %f %f, Vicon: %f %f, Error: %f %f", camera_x,camera_y,vicon_x,vicon_y,delta_x,delta_y)

def callback_vicon(data):
    global vicon_x
    global vicon_y
    vicon_x = data.x
    vicon_y = data.y
   # print(vicon_x,vicon_y)

def CompareData():
    rospy.init_node('CompareData', anonymous=True)
    rospy.Subscriber('cameraPose', FloatArray, callback_cameraPose)
    rospy.Subscriber('vicon_xb_node0/viconPoseTopic',viconPoseMsg,callback_vicon)
    rospy.spin()

if __name__ == '__main__':
    CompareData()
