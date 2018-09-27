import sys
import cv2
import numpy as np
from PIL import Image
import os

print("number of arguments", len(sys.argv))
print("number of arguments", sys.argv[1])
videoName = sys.argv[1]

# Playing video from file:
cap = cv2.VideoCapture(videoName)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 128)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 128)


try:
    if not os.path.exists('data'):
        os.makedirs('data')
except OSError:
    print ('Error: Creating directory of data')

currentFrame = 0
while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    if ret==False:
        print("Error read frame")
        break
    resized_image = cv2.resize(frame, (128, 128))
    rgbImage = cv2.cvtColor(resized_image, cv2.COLOR_BGR2RGB)

    # Saves image of the current frame in jpg file
    name = './data/frame' + str(currentFrame) + '.jpg'
    print ('Creating...' + name)
    cv2.imwrite(name, rgbImage) 
    currentFrame += 1

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
