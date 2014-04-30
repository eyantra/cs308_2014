import cv2, math
import numpy as np
import time


class ColourTracker:
  def __init__(self):
    cv2.namedWindow("ColourTrackerWindow", cv2.CV_WINDOW_AUTOSIZE)
    self.capture = cv2.VideoCapture(0)
    #delay to get feed from camera
    time.sleep(5)
    #scale down image for better tracking of the fish
    self.scale_down = 4
  def run(self):
    #counting variables to keep track of time for which the fish is stationary
    timer = 0
    counter = 0
    while True:
      time.sleep(.1)
      #send an sms to the admin if timer reaches 10 seconds
      if timer == 100:
        import sms
        timer = 0
    
      print "timer %d" %timer
      f, orig_img = self.capture.read()
      orig_img = cv2.flip(orig_img, 1)
      img = cv2.GaussianBlur(orig_img, (5,5), 0) #Apply gaussian blurring
      img = cv2.cvtColor(orig_img, cv2.COLOR_BGR2HSV) # convert image from RGB to HSV
      img = cv2.resize(img, (len(orig_img[0]) / self.scale_down, len(orig_img) / self.scale_down))
      red_lower = np.array([0, 150, 0],np.uint8)  # try to find a red object in the image
      red_upper = np.array([5, 255, 255],np.uint8)  
      red_binary = cv2.inRange(img, red_lower, red_upper)
      dilation = np.ones((15, 15), "uint8")
      red_binary = cv2.dilate(red_binary, dilation)
      contours, hierarchy = cv2.findContours(red_binary, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
      max_area = 0
      largest_contour = None
      for idx, contour in enumerate(contours): # find the largest red object
        area = cv2.contourArea(contour)
        if area > max_area:
          max_area = area
          largest_contour = contour
      if not largest_contour == None:
        moment = cv2.moments(largest_contour)
        if moment["m00"] > 1000 / self.scale_down:
          rect = cv2.minAreaRect(largest_contour) # get the coordinates of the rectangle enclosing the object
          x_mid = rect[0][0]
          y_mid = rect[0][1]
          theta = rect[2]
          if counter==0: 
            counter+=1
            x_old = x_mid
            y_old= y_mid
            
          #if the xcoordinate and yccordinate of the red fish both dont change by 20 pixels, increase the timer 
          elif abs(x_old - x_mid) < 20 and abs(y_old - y_mid) < 20:
                timer += 1                

          else:
            timer = 0
            x_old = x_mid
            y_old = y_mid

          #draw the rectangle obtained  
          rect = ((rect[0][0] * self.scale_down, rect[0][1] * self.scale_down), (rect[1][0] * self.scale_down, rect[1][1] * self.scale_down), rect[2])
          box = cv2.cv.BoxPoints(rect)
          box = np.int0(box)
          cv2.drawContours(orig_img,[box], 0, (0, 0, 255), 2)
          cv2.imshow("ColourTrackerWindow", orig_img)
          #quit if escape key is pressed
          if cv2.waitKey(20) == 27:
            cv2.destroyWindow("ColourTrackerWindow")
            self.capture.release()
            break
        else:
          timer+=1
      else:
        timer+=1
if __name__ == "__main__":
  colour_tracker = ColourTracker()
  colour_tracker.run()
