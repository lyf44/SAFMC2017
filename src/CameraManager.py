import cv2
import time
import picamera
from picamera.array import PiRGBArray
import rospy

class CameraManager():
    def __init__(self, cameraIndex):
        if cameraIndex == 0:
            self.cap = cv2.VideoCapture(cameraIndex)
            self.type = cameraIndex
        if cameraIndex == 1:
            self.camera = picamera.PiCamera()
            self.camera.resolution = (320,240)
            self.camera.framerate = 40
            self.stream = PiRGBArray(self.camera,size=(320,240))
            self.type = cameraIndex
            #rospy.set_param('camera_resolution_x', 320)
            #rospy.set_param('camera_resolution_y', 240) 
    def getFrame(self):
        if self.type == 0:
            for i in xrange(10):
                success, frame = self.cap.read()
                if success:
                    # end_time = time.time()
                    return frame
                else:
                   continue
            print("Cannot get frame")
        if self.type == 1:
            self.camera.capture(self.stream,'bgr',use_video_port=True) 
        if self.stream is not None:
            # end_time = time.time()
            frame = self.stream.array
            self.stream.truncate(0)
            return frame
        else:
            print("cannot get frame") 
        
        return None

if __name__ == "__main__":
    cap = CameraManager(1)
    cv2.namedWindow("cap")
    counter = 0
    #print(cap.getFrame())
    while cv2.waitKey(1) == -1:
        a = cap.getFrame()
        cv2.imshow('cap',a)
        if counter>100:
            break
        counter+=1
