#!/usr/bin/env python
import rospy
from vicon_xb.msg import viconPoseMsg
from camera.msg import IntArray
from camera.msg import PixData
vicon_x = 0
vicon_y = 0
roll = 0
pitch = 0
height = 0
focalLength = 0.006
delta_x = 0
delta_y = 0
camera_x = 0
camera_y = 0

def callback_pixData(data):
    global roll,pitch,height
    roll = data.roll
    pitch = data.pitch
    height = data.height

def callback_camera(data):
    Cord = data.data
    if Cord[0] == 0 or Cord[1]==0:
        return
    
    #camera compensation
    error_x = 120 - Cord[0]
    error_y = 120 - Cord[1]
    #error_x = error_x + roll*5.54 # when roll angle is postive, error become more negative
    #error_y = error_y - pitch*5.54 # when pitch angle is positive, error become more positive

    # calculate real cordinate 
    realDistance_x = error_x * (-1) * (3.75e-3)
    realDistance_y = error_y * (3.75e-3)

    global camera_x, camera_y
    camera_x = realDistance_x + 0.0022
    camera_y = realDistance_y + 0.064
    #camera_x = Cord[0]
    #camera_y = Cord[1]

def callback_vicon(data):
    global vicon_x
    global vicon_y
    vicon_x = data.pose.position.x
    vicon_y = data.pose.position.y
   # print(vicon_x,vicon_y)

def CompareData():
    rospy.init_node('CompareData', anonymous=True)
    rospy.Subscriber('vicon_xb/viconPoseTopic',viconPoseMsg,callback_vicon)
    rospy.Subscriber('CentreCordInt', IntArray, callback_camera)
    rospy.Subscriber('PixData', PixData, callback_pixData)
    rate = rospy.Rate(15)

    while not rospy.is_shutdown():
        delta_x = camera_x - vicon_x
        delta_y = camera_y - vicon_y
        rospy.loginfo("camData: %f %f, Vicon: %f %f, Error: %f %f", camera_x,camera_y,vicon_x,vicon_y,delta_x,delta_y)
        rate.sleep()

if __name__ == '__main__':
    CompareData()
