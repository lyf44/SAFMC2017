#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import String
from lyf_playground.msg import IntArray
import cv2
import CameraManager
import Classifier

def CtrCordPublisher():
    

    pub = rospy.Publisher('CentreCordString', String, queue_size=10)
    pub1 = rospy.Publisher('CentreCordInt', IntArray, queue_size = 10)
    rospy.init_node('CtrCordPublisher', anonymous=True)
    rate = rospy.Rate(10) # 10hz


    
    # choose which camera to use
    # 0 for USB webcam
    # 1 for Picamera 
    camera = CameraManager.CameraManager(1)
    classifier = Classifier.Classifier()

    while not rospy.is_shutdown():
        frame = camera.getFrame()
        height, width, channels = frame.shape
        center, radius = classifier.findLocation(frame)
                
        #flight_controller.moveByCord(goalCord,width,height)
        
        if center is None:
            goalCord_str = "object not found"
            goalCord_int = [0,0]
        else:
            # (goalCord_X, goalCord_Y) = center    
            # goalCord_str = "goalCord is %d %d"  % (goalCord_X, goalCord_Y)
            goalCord_str = "Centre Cord is %d %d"  % (center[0], center[1])
            goalCord_int = IntArray()
            goalCord_int.data = [center[0],center[1]]
        rospy.loginfo(goalCord_str)
        pub.publish(goalCord_str)
        pub1.publish(goalCord_int)
        rate.sleep()

if __name__ == '__main__':
    try:
        CtrCordPublisher()
    except rospy.ROSInterruptException:
        pass
