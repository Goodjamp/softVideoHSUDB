import sys
import cv2
import numpy as np
import os
#for delete not empty directory
import shutil

#print("number of arguments", len(sys.argv))
#print("number of arguments", sys.argv[1])
videoName   = sys.argv[1]
#framesPath  = sys.argv[2]
#print("Path to video ", videoName)
#print("frame number  ", frameNumber)

# Playing video from file:
cap = cv2.VideoCapture(videoName)

try:
    if not os.path.exists('data'):
        os.makedirs('data')
    else:
        print("delet dir")
        shutil.rmtree("data")    
        os.makedirs('data')
except OSError:
    print ('Error: Creating directory of data')

#frameCount = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
   
#print("frameCount =  ", frameCount)

#set and read sellected frame
#cap.set(cv2.CAP_PROP_POS_FRAMES,frameNumber)
cntFrame = 0;
while True:
    rgb565A=[]
    ret, frame = cap.read()
    if ret==False:
        print("Last frame was read or error read frame")
        sys.exit()
    #release video file
    
    # resize image to 128*128 pixels
    resized_image = cv2.resize(frame, (128, 128))

    #save frame in jpg file
   # name = './data/' + str(cntFrame) + '.jpg'
    #print ('Creating...' + name)
    #cv2.imwrite(name, resized_image) 

    #get R G B components
    b,g,r = cv2.split(resized_image)

    #convert RGB to 565 format
    for i in range(len(r)):
        for k in range(len(r[i])):
            rgb565A.append(((((g[i][k]<<3)>>5)<<5) | (b[i][k]>>3)) & 255)  
            rgb565A.append((((r[i][k]>>3)<<3) | (g[i][k]>>5)) & 255)
    print("cnt = ", cntFrame)
    #print("rez = ", rgb565A)

    # save RAW rgb 565 in file
    f = open('./data/rawFrame565_' + str(cntFrame) + '.bin', 'w+b')
    binary_format = bytearray(rgb565A)
    f.write(binary_format)
    f.close()
    cntFrame = cntFrame + 1
    
cap.release()
sys.exit()
