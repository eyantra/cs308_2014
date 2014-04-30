Team Name :-
Team TU-3
Team [A]lliance

Project Name :-
FreeRTOS-based Greenhouse Controller

Members :-
Ankush Das (100050042) - Team Leader
Vivek Paharia (10050044)
Nilesh Jagnik (100050040)
Pulkit Piyush (100050037)

Description :-
We have developed a FreeRTOS-based Greenhouse Controller, which includes a Java-based desktop application and a Firebird bot, on which the FreeRTOS code has been burnt. The bot and the desktop application communicate using XBee radios, and the sensor and actuator circuit, consisting of temperature and light sensors, and their corresponding actuators, is attached to the Firebird bot, which is, in practice, controlled by the desktop application.

Software Prerequisites :-
1. FreeRTOS source code
2. Flash Magic
3. Keil uVision 4

Hardware Prerequisites :-
1. USB-to-Serial Converter
2. Serial Cable
3. Firebird bot

Platforms Used :-
1. FreeRTOS
2. Java (Netbeans)
3. Keil uVision 4

Installation Instructions :-
1. After extracting the .zip file, you will find the test_freertos folder in Code\FB5 ARM\FreeRTOS\Demo folder. This folder contains all the source code files used in this project. Open this folder, and open the test_freertos.uvproj file using Keil uVision 4. Now, using Keil, build the code using the arm-elf-gcc cross compiler provided in Keil.

2. After building the code, test_freertos.hex file is generated. Now, you need to burn the test_freertos.hex file to the Firebird bot. Use Flash Magic to do this. 
Open Flash Magic, select the microcontroller as LPC2148. When you connect the serial port of the Firebird bot using the USB-Serial converter, open Device Manager. You will see "Ports (COM & LPT)" tab in the Device Manager. Click on this, and you will see the COM port number where the USB-Serial Converter is connected. Now, enter this COM port number in the COM port option in Flash Magic. Now, browse to the file "test_freertos.hex", and click on Start.

3. Your code will start burning to the Firebird bot. Meanwhile, connect the XBee radio to your computer and go to the Code\PC\dist folder and double click on GreenhouseManager.jar file. This will open a desktop application which will be used to control the Firebird bot. Again, use the same technique to figure out the COM port used by XBee radio, and click on Test to see if the COM port is configured correctly.

4. Once, the COM port is configured, you are ready! Press the reset button on the Firebird bot, and start using the application to control the greenhouse environment.

5. If you still have problems, refer to the Youtube video below, and if you are still unclear, feel free to reach us at ankushdas@iitb.ac.in.

Link to Youtube Video :-
http://youtu.be/5XaSsgBbr0M
