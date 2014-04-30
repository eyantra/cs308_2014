from SimpleCV import *
from SimpleCV.MachineLearning import *
import numpy as np
import time
import serial

#Training data
image_dirs = ['images/triangles',
              'images/rectangles',
              'images/none']
class_names = ['triangles','rectangles','none']
feature_extractors = []

hue_extractor = HueHistogramFeatureExtractor()
edge_extractor = EdgeHistogramFeatureExtractor()
feature_extractors.append(edge_extractor)
feature_extractors.append(hue_extractor)

classifier = KNNClassifier(feature_extractors,3)	

#Training the classifier
classifier.train(image_dirs,class_names)

#Initialising the serial with registered COM port of XBEE. Change '2' it to whatever COM port being used in your computer. 
ser = serial.Serial(2)

#Using secondary camera as the image source
cam = Camera(1)

#Threshold for the difference between consecutive images.
threshold = 4 # if mean exceeds this amount do something

i=0
previous = cam.getImage()

prev_rectangle = 0;

while True:
        time.sleep(0.2) #wait for half 0.2 seconds
        current = cam.getImage() #grab another frame
        diff = current - previous
        matrix = diff.getNumpy()
        mean = matrix.mean()        
        if mean >= threshold:   #if there is a large difference between current and previous images exceeding the threshold
                current.show()
                i=i+1
                print "Object Detected."                
                cur_class = classifier.classify(current)
                print cur_class
                if cur_class=='rectangles':
                        if prev_rectangle == 1:
                                prev_rectangle = 0
                        else:
                                prev_rectangle=1
                                ser.write('2')  #Sending signal to Firebird to stop conveyor belt
                                time.sleep(0.5)
                                ser.write('1')  #Start conveyor belt
                                time.sleep(1.9)
                                ser.write('2')  #Stop conveyor belt
                                time.sleep(0.5)
                                ser.write('3')  #Push mechanical arm
                                time.sleep(6)
                                ser.write('1')  #Start conveyor belt
        previous=current
        



