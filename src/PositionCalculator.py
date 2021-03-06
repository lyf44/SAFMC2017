#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from camera.msg import IntArray
from camera.msg import FloatArray
import math

focalLength = 0.0036 # 3.6 mm provided by the documentation
pub = rospy.Publisher('cameraPose', FloatArray, queue_size=10)
res_half_X = rospy.get_param('camera_resolution_x')/2
res_half_Y = rospy.get_param('camera_resolution_y')/2
coef_X = math.tan(53.5/180*math.pi/2)*focalLength/res_half_X; # horizontal field of view = 53.5 degree
coef_Y = math.tan(41.4/180*math.pi/2)*focalLength/res_half_Y; # vertical field of view = 41.4 degree
markCord_X = rospy.get_param('/CtrCordPublisher/markCord_X')
markCord_Y = rospy.get_param('/CtrCordPublisher/markCord_Y')

def pixDataCallback(data):
    roll = data.roll
    pitch = data.pitch
    height = data.height

def callback(data):
    Cord = data.data
    if Cord[0] == 0 or Cord[1]==0:
        return
    
    #TODO camera compensation


    realDistance_X = (Cord[0] - res_half_X)*coef_X*height/focalLength
    realDistance_Y = (Cord[1] - res_half_Y)*coef_Y*height/focalLength

    realCord = FloatArray()
    realCord.data = [realDistance_X+markCord_X,realDistance_Y+markCord_Y]
    print(realCord.data)
    global pub
    pub.publish(realCord)
    
 
def PositionCalculator():
    print("coefficient are %f %f, resolution is %d %d" % (coef_X,coef_Y,res_half_X, res_half_Y))
    
    # In ROS, nodes are uniquely named. If two nodes with the same
    # node are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.
    rospy.init_node('PositionCalculator', anonymous=True)
    rospy.Subscriber('CentreCordInt', IntArray, callback)
    rospy.Subscriber('PixData', PixData,pixDataCallback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    PositionCalculator()
