******************************************************************************
*************		    CS-308-2014 Final Report 				 *************
*************			Project Title : Intruder Detection 		 *************
*************			Team Name : 	Mavericks 				 *************
*************			Team Code : 	TH-08  					 *************
******************************************************************************	
						Team Member
******************************************************************************
Raghav Sagar: 100050001
Anil Kumar: 100050013
Vinod Talapa: 100050014
Ashish Lalawat: 100050017

******************************************************************************
						Breif dicription of project
******************************************************************************
We have built an "Intruder Detection System". The motivation for the project is detection of unknown objects using a fixed surveillance camera in our arena.
This is followed by providing live video feed of this unknown object, by our patrol bot.
This is useful in detection of animal intruders in restricted areas (for example Greenhouse).


******************************************************************************
						Prerequisites to install
******************************************************************************
Webcam driver:
	- We are using iball face2face c8.0 usb webcam as surveillance cameras, thus we need the driver for the same. Download it from http://www.iball.co.in/Product/Face2Face-C8-0--Rev-3-0-/90

Keil uVison:
	- Using this IDE for embedded c programming, the code is used for moving the bot acoording to the instructions sent to it via serial port communication.
	- Download Keil from https://www.keil.com/demo/eval/c51.htm

IP webcam:
	- Using this android application for getting the live video feed from the mobile camera installed on the bot via wifi.
	- Install app on the phone using https://play.google.com/store/search?q=ip%20webcam

******************************************************************************
						Project Set up
******************************************************************************
FireBird Configuration 
***********************
	- Compile and Build the Keil Project for serial communication
	- Install the bin file on the FireBird
	- Connect Xbee to the PC
	
Camera and Arena set up
***********************
	- Fix arena at some location
	- Position the inclined camera to cover the arena
	- Capture the base image by running points.m
	
Camera Calibration
*******************
	- Mark some (atleast 4) points in the image with known world coordinates
	- Update the above world and pixel coordinates in worldRef.m

Setup Intruder Detection Program
**********************************
	- Introduce FireBird into the Arena
	- Run the "main.m" program
	- Considering the difference in the pixels and angle between the bot's direction of motion and line joining bot and intruder, bot will reach near intruder 

Setup Web Application 
*********************
	- First we have to install 'wampserver' (refer to video tutorial for this step)
	- Next, this current folder 'serial_port' needs to be copied to the location 'C:\wamp\www\'
	- Then, we have to configure and start an ad-hoc Wifi network ("http://www.addictivetips.com/windows-tips/how-to-create-wireless-ad-hoc-internet-connection-in-windows-8/")
	- Then we need to install an android application in our mobile phone (which is acting as the live feed camera)
	- The android application is IP Webcam ("https://play.google.com/store/apps/details?id=com.pas.webcam")
	- Then, we need to connect our mobile phone to this ad hoc network.
	- After this we need to open IP Webcam and 'Begin Server' (We may need to change 'line 19' in script 'callpy.php' as per the given IP address shown in IP Webcam app)
	- Now we can attach the mobile on top of the FireBird to act as live feed camera
	- Next we need to connect Xbee module to our PC, and start our Firebird V
	- We need to ensure the Xbee port 'COM4' is open for communication to FireBird V (We can test using X-CTU)
	- Now we run our web application (refer to video tutorial for this step)
	
	
*********************************************************************************************	
Screen-cast video youtube Link:  https://www.youtube.com/watch?v=tBX3uE1KZXU&feature=youtu.be