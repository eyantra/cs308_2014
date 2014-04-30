README for PC code
===================

Files:
---
1. localization.py - Python script containing localization code
2. sample_data - File containing the sample data used to train the localization system.

Format:
---
1. localization.py  
    * The code has been well commented.  
2. sample_data  
	* A reading is a line of values.  
	* 3 readings have been taken for each label (or co-ordinate).  
	* The 3 readings are consecutive.  
    * Thus the readings to label map is as follows. The 1st three readings are for label 0, the next 3 readings are for label 1, etc.  
	* Each reading has 3 columns, one for each router. Thus the 1st column is for router ERTS_1, the next is for router ERTS_2, and the last one is for router ERTS_3.  

How to run this code on the PC:
---
1. Install the needed softwares as given in the screencast video.
2. Run localization.py as follows.  
	* On Windows - Open the file in IDLE and press F5.  
	* On Linux - Open the terminal. Go to the folder containing this file and run `python localization.py`.  

License:
---
Refer to [LICENSE](../../LICENSE)