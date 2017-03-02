#!/usr/bin/env python
from time import sleep
from picamera import PiCamera

camera = PiCamera()
camera.resolution = (240,240)
#camera.start_preview()
# Camera warm-up time
sleep(1)
camera.capture('foo.jpg')
#camera.capture('foo.jpg', resize=(240, 240))
#camera.stop_preview()
