import sys
#If you want to store TIS.py in another folder, write as follows 
#sys.path.append("../python-common")

import cv2
import numpy as np
import TIS
import time

# This sample describes a series of processing methods for acquiring an image from a camera and processing an image with OpenCV.
# Required packages:
# pyhton-opencv
# pyhton-gst-1.0
# tiscamera


Tis = TIS.TIS()
#　DMK 33UX264 Serial: 16710581, display at resolution 640x480 @ 30 fps 
#　Commented out to open the device using selectDevice
#　Tis.openDevice("16710581", 640, 480, "30/1", TIS.SinkFormats.BGRA,True)

#You can specify the device selection, format, and frame rate on the command line by using selectDevice. 
if not Tis.selectDevice():
        quit(0)



#Video transmission with Gstreamer 
Tis.Start_pipeline() 

#Display a list of camera properties (check the setting range) 
Tis.List_Properties()


print('Press Esc to stop')
lastkey = 0

cv2.namedWindow('Window')  

#Arguments used in the OpenCV erode function 
kernel = np.ones((5, 5), np.uint8)  

#Counter initialization
imagecounter=0

while lastkey != 27:
        #If the image can be acquired on the PC within the Timeout set time (1 second), proceed to the next process.
        if Tis.Snap_image(1) is True:  # Snap an image with one second timeout
                # Image acquisition
                image = Tis.Get_image()  
                # Image processing with OpenCV 
                #image = cv2.erode(image, kernel, iterations=1)  # Example OpenCV image processing

         
                cv2.imshow('Window', image) 
               
                imagecounter += 1
                filename = "./image{:04}.jpg".format(imagecounter)
                #Count the number of frames to give an index number to the name of the Jpeg file
                cv2.imwrite(filename, image)

        lastkey = cv2.waitKey(1)

# Stop Gstreamer pipeline
Tis.Stop_pipeline()
cv2.destroyAllWindows()
print('Program ends')


