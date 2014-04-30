Team: TH04 (Alphajoes)

Project: Remote fish feeding and monitoring

Team members: 
			  Pawan Nagwani (100050049)
			  Sampath Sagar (1000050051)
			  Ashish Palli  (1000050053)
			  Uday Nath     (100050055)

Contents:

	Feeding part:

	Aquaponis: 
		Contains the source code of the android app.
		Import the project into eclipse. Clean and build the project and install the app in your
		android device by using the apk file generated after the build.
	
	gcm_server_php_new:
		This folder contains the code of the server and the web interface to schedule the feed amount and time.
		Place all the php files in this folder in the htdocs folder of your system and run index.php
	
	gcm_users.sql:
		Contains the database of the users registered for the service. 
		Follow this column structure while storing the information of newly registered users or import
		the sql file into your database.

		
	Monitoring part:	
		
	Tracking:
		newtrack.py: You will need to have python compiler installed to run this file. Connect a USB camera 
		to your system before running this python script. Place the usb camera in front of the aquarium. We have 
		assumed the fish to be red in colour. Run the python script using the Python IDLE interface. 
		
		sms.py: Contains the script to send an sms to the user. A twilio account is required to run this script.
		Register your mobile number and in your twilio account dashboard to receive an sms if the fish is 
		stationary for a long time. Make sure you do not have the DND service activated.
	
	
	Video links:
	Tracking - https://www.youtube.com/watch?v=b0ZwX6mt_SU
	Server - https://www.youtube.com/watch?v=9pJcSSQ2Gao
	Android app - https://www.youtube.com/watch?v=Q-ti1feJMHo 