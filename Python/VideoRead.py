import sys
import cv2
import numpy as np
from PIL import Image
import os

print("number of arguments", len(sys.argv))
print("number of arguments", sys.argv[1])
videoName   = sys.argv[1]
frameNumber = int(sys.argv[2])
print("Path to video ", videoName)
print("frame number  ", frameNumber)

rgb656A=[]

# Playing video from file:
cap = cv2.VideoCapture(videoName)

frameCount = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
   
print("frameCount =  ", frameCount)

#set and read sellected frame
cap.set(cv2.CAP_PROP_POS_FRAMES,frameNumber)
ret, frame = cap.read()

if ret==False:
    print("Error read frame")
    sys.exit()
#release video file
cap.release()
# resize image to 128*128 pixels
resized_image = cv2.resize(frame, (128, 128))

#save frame in jpg file
name = './frame' + '.jpg'
print ('Creating...' + name)
cv2.imwrite(name, resized_image) 

#get R G B components
b,g,r = cv2.split(resized_image)

#convert RGB to 565 format
for i in range(len(r)):
    for k in range(len(r[i])):
        rgb656A.append((((r[i][k]>>3)<<3) | (g[i][k]>>5)) & 255)  
        rgb656A.append(((((g[i][k]<<3)>>5)<<5) | (b[i][k]>>3)) & 255)
print("cnt = ", len(rgb656A))
#print("rez = ", rgb656A)

# save RAW rgb 565 in file
f = open('./rawFrame565.bin', 'w+b')
binary_format = bytearray(rgb656A)
f.write(binary_format)
f.close()

sys.exit()
