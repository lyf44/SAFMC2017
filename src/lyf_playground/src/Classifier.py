import cv2
import numpy as np
import time

class Classifier():
    def __init__(self):
        type = 0
    def classify(self,frame):
        # start = time.time()
        # remove high frequence noise
        frame = cv2.GaussianBlur(frame,(15,15),0)
        # convert image from RGB to HSV
        hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        # set red range, pure red in HSV is (0,255,255)
        lower_red_0 = np.array([0, 100, 100])
        upper_red_0 = np.array([10, 255, 255])
        lower_red_180 = np.array([160,100,100])
        upper_red_180 = np.array([180,255,255])
        # set mask individually
        mask1 = cv2.inRange(hsv_frame, lower_red_0, upper_red_0)
        mask2 = cv2.inRange(hsv_frame, lower_red_180, upper_red_180)
        # combine two masks
        mask = cv2.bitwise_or(mask1,mask2)
        # debug propose
        # morphology operation to remove noise, can be set to larger value
        kernel = np.ones((15, 15), np.uint8)
        opening = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
        res = cv2.bitwise_and(frame, frame, mask=opening)
        cv2.imshow('mask', res)
        # print("Time took to classify: ",time.time()-start)
        return (hsv_frame,opening)

    def findLocation(self,frame):
        _,mask = self.classify(frame)
        # cv2.imshow("mask",mask)
        contours, hier = cv2.findContours(mask,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
        if(contours==None):
            return None
        c = max(contours, key=cv2.contourArea)
        (x, y), radius = cv2.minEnclosingCircle(c)
        center = (int(x),int(y))
        radius = int(radius)
        print(int(x),int(y),radius)
        return (center,radius)




if __name__ == "__main__":
    import CameraManager
    camera = CameraManager.CameraManager(0)
    classifier = Classifier()
    cv2.namedWindow("frame")
    for i in xrange(2000):
        frame = camera.getFrame()
        circle = classifier.findLocation(frame)
        if(circle == None):
            continue
        center, radius = circle
        cv2.circle(frame, center, radius,(0,255,0),2)
        exception
    cv2.destroyAllWindows()




