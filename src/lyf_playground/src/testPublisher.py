#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import String
import cv2
import CameraManager
import Classifier

def testPublisher():
    pub = rospy.Publisher('testResult', String, queue_size=10)
    rospy.init_node('test', anonymous=True)
    rate = rospy.Rate(10) # 10hz

    camera = CameraManager.CameraManager(1)
    classifier = Classifier.Classifier()

    while not rospy.is_shutdown():
        frame = camera.getFrame()
        height, width, channels = frame.shape
        goalCord_X,goalCord_Y, _ = classifier.findLocation(frame)
        
        #flight_controller.moveByCord(goalCord,width,height)

        goalCord_str = "goalCord is %d %d"  % (goalCord_X, goalCord_Y)
        rospy.loginfo(goalCord_str)
        pub.publish(goalCord_str)
        rate.sleep()

if __name__ == '__main__':
    try:
        testPublisher()
    except rospy.ROSInterruptException:
        pass
