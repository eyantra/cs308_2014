Dancing-Robot
=============
2014-cs308
---------------------------------------------------------------
GroupNumber : TU-11
---------------------------------------------------------------
Members :

100050077  Pruthvi Kaveti
100050078  Naga Raghu Teja Parisa
100050079  Sambasiva Rao P
---------------------------------------------------------------
Title of Project : Dancing Robot
---------------------------------------------------------------
Brief Description :

Our aim is to make the longbow robot dance in rhythm with the given music.
---------------------------------------------------------------
Softwares and Prerequisites:

1.Atmel Studio 						- project code editing and building hex files
2.AVR Dude     						- to load code into the bot (hex file burner)
3.Python 2.7						- serial communication between bot and pc
4.Qt Creator(For writing the code Not for executing)	- gui
5.Pyside 						- python library for Qt
6.Numpy							- scientific computing with Python
7.wave							- python library for accessing wav data
8.matplotlib						- python library for plot graphs
9.Pymedia 						- python library for playing music

1 can be downloaded form 
www.atmel.com/tools/atmelstudio.aspx‎

2 can be installed just by running their .exe files and 
proceeding on prompts(folder given in lab)
	http://download.savannah.gnu.org/releases/avrdude/

3 can be downloaded form 
https://www.python.org/downloads/

4 can be downloaded form 
http://qt-project.org/downloads

5 can be downloaded form 
http://qt-project.org/wiki/PySide_Binaries_Windows

6 can be downloaded form 
http://www.scipy.org/scipylib/download.html

7 can be downloaded form 
http://www.lfd.uci.edu/~gohlke/pythonlibs/#scikits.audiolab

8 can be downloaded form 
http://www.lfd.uci.edu/~gohlke/pythonlibs/#matplotlib

9 can be downloaded form 
http://www.lfd.uci.edu/~gohlke/pythonlibs/#pymedia

-----------------------------------------------------------------
Instructions to Setup:

1.Open Project in Atmel studio, Build
2.Using AVR Dude, burn the dance moves.hex file on 
  to the bot 
  Command : avrdude -c stk500v2 -p m1284p -P NEX-USB-ISP -U flash:w:<Path of Hexfile>:i
3.Run the python script on the server
4.Connect zigbee module to server
5.Run Python script(main.py) on the server
6.follow instructions from the GUI opened after executing..
-----------------------------------------------------------------
Youtube Link:
-----------------------------------------------------------------
Execution : https://www.youtube.com/watch?v=bpI0xT3QZCs
Install : https://www.youtube.com/watch?v=F75QLqxfQno

-----------------------------------------------------------------
Project Link
-----------------------------------------------------------------
https://github.com/raghu7593/Dancing-Robot