import serial
import numpy
class FlightController():
    def __init__(self,serial_port="/dev/ttyACM0",dual_rate=9600):
        self.serial_port = serial_port
        self.dual_rate = dual_rate
        self.serial_conn = serial.Serial(self.serial_port,self.dual_rate)

    def sendMessage(self,message):
        self.serial_conn.write(message)
        return self.serial_conn.read_all()

    def getSensor(self,sensor_index):
        sensor_reading = self.sendMessage("H"+sensor_index)
        return sensor_reading

    def pitch(self,degree):
        mesg = self.sendMessage("P"+str(degree))
        return mesg

    def roll(self,degree):
        mesg = self.sendMessage("R"+str(degree))
        return mesg

    def yaw(self,degree):
        mesg = self.sendMessage("Y"+str(degree))
        return mesg

    def forward(self,distance):
        # calculate speed here
        mesg = self.sendMessage("W"+str(distance))
        return mesg

    def backward(self,distance):
        mesg = self.sendMessage("S"+str(distance))
        return mesg

    def left(self,distance):
        mesg = self.sendMessage("A"+str(distance))
        return mesg

    def right(self,distance):
        mesg = self.sendMessage("D"+str(distance))
        return mesg

    def moveByCord(self,goal_cord, resolution):
        goal_cord = numpy.array(goal_cord)
        center = numpy.array(resolution)/2
        offset = goal_cord - center
        if(offset[0]>0):
            self.forward(offset[0])
        else:
            self.backward(offset[0])
        if(offset[1]>0):
            self.left((offset[1]))
        else:
            self.write(offset[1])

if __name__ == "__main__":
    fc = FlightController()
    cord = [100,200]
    fc.moveByCord(cord,[640,480])

