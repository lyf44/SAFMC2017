#!/usr/bin/env python
# license removed for brevity
import rospy
from std_msgs.msg import String
from camera.msg import IntArray
#import CameraManager
import Classifier
import cv2
import time
import picamera
from picamera.array import PiRGBArray

#initialize ros  
pub = rospy.Publisher('CentreCordInt', IntArray, queue_size = 10)
rospy.init_node('CtrCordPublisher', anonymous=True)
goalCord_int = IntArray()

#initialize classfier
classifier = Classifier.Classifier()  
    
#initialize camera
camera = picamera.PiCamera()
camera.resolution = (240,240)
camera.framerate = 30
stream = PiRGBArray(camera,size=(240,240)) 
#rospy.set_param('camera_resolution_x', 240)
#rospy.set_param('camera_resolution_y', 240)
time.sleep(2)

def streams():
    while not rospy.is_shutdown():
        yield stream

        #start = time.time()

        frame = stream.array
        center = classifier.findLocation(frame)
        #center = None
        stream.truncate(0)
        if center is None:
            goalCord_int.data = [0,0]
	    #print("haha")
        else:
            goalCord_int.data = [center[0],center[1]]
            #for test
        print(goalCord_int)

        pub.publish(goalCord_int)

        #end = time.time()
        #print(end - start)
        
def CtrCordPublisher():
    camera.capture_sequence(streams(),'bgr',use_video_port=True)

if __name__ == '__main__':
    try:
        CtrCordPublisher()
    except rospy.ROSInterruptException:
        pass
