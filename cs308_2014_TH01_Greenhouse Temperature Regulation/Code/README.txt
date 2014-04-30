CS 308 - 2014 Project

Greenhouse Temperature Regulation

Team : TH 1 - Proxymorons
Members:
Aditya Bhandari (100050008)
Akshay Gaikwad (100050010)
Kanishk Parihar (100050018)
Sameer Kumar Agrawal (100050021)

*******************************************************

* Description: 

Our project aims to regulate the temperature of the model greenhouse by 
1) introducing a shade over the plant and 
2) using PC fan for air circulation
We measure the current temperature using temperature sensor and depending on the readings,
initiates appropriate action to regulate it.

We have also provided a simple user interface, so that it can be used by someone with minimal knowledge of computers.

*******************************************************
* HARDWARE CONNECTIONS :

* FOR THE TEMPERATURE SENSOR CIRCUIT :

(1) IR Sensor 4 on the Firebird : 
Right slot: +5V			.... (1.1)
Middle slot: GND		.... (1.2)
Left slot: Vout			.... (1.3)

(2) LM 35 (with LM35 written on it facing us) :
Left pin: +5V			.... (2.1)
Right pin: GND			.... (2.2)
Middle pin: Vout		.... (2.3)

- Connect (2.1) to (1.1), (2.2) to (1.2) and (2.3) to (1.3)
- Connect 4.7K resistor between Vout of LM 35 and GND in the circuit.


* FOR THE PC FAN :

(3) Servo Pod Connector on the Firebird :
With head of the FB5 bot facing away, pins are numbered from right to left,
i.e., pin 1 is the rightmost pin and pin 8 is the leftmost pin

(4) Motor driver circuit used is L293D circuit

- Connect pin 3 (Control Pin) to M2-IN of Motor driver circuit
- Connect pin 8 (+9V) and pin 6 (GND) to the power supply of Motor driver circuit
- Connect the Fan to M2-OUT of Motor driver circuit


*******************************************************

* SOFTWARE REQUIREMENTS FOR WINDOWS 7 or 8:

(1) Keil uVision 4
(2) Python - 2.7.6
(3) Pyserial-2.7.win32 package for python
(4) X-CTU

For set up and installation instructions of these softwares and our project, refer to our Screencast Video and the "screencast_transcript" file containing the transcript of the video.

* SCREENCAST YouTube Link : https://www.youtube.com/watch?v=3k2g4KCClRE

*******************************************************

* CODE :

It has two folders. Each folder is for different platform.
1) FB5 ARM
2) Setting the web server on the PC

The code has been commented appropriately with the required details.
All the latest code has also been commited on github.

* GITHUB Link	: https://github.com/sameerkumaragrawal/Greenhouse-Temperature-Regulation
