VisualFirebird
==============

Visual Programming Enviranment for Firebird

Team: embed'em

TH­2

 

Aashay Harlalka ­ 100050035

Nishanth Dikkala ­ 100050052

Nivvedan S ­ 100050084

Vipul Singh ­ 100050057

(Team Leader: Nishanth Dikkala)

 
Video Tutorial at: http://youtu.be/ETjtv49P6rc?hd=1
 

Description of the project:

Embedded systems and robotics generate a lot of enthusiasm among a

huge section of the population, notably school­going kids.

Programming on these systems usually involves C, knowledge of

which cannot be expected from such people. To enable them to

convert their zeal into useful projects, we wish to remove this

programming road­block and provide them a user­friendly visual

interface with a high level of abstraction. This is similar to the

MIT­Scratch API. Using our interface, one can drag and drop

blocks, enter few values and thus, create programs based purely on

intuition and run them on the bots.

Our project aims at developing a rich visual programming

environment for Firebird V. This project would be building upon

the work by Akshar and Shweta for a course project CS 684, on

similar lines and is highly inspired from MIT’s scratch.

Pre­requisites to install:

1. Netbeans (install the latest version along with JDK 1.6 or

higher if you don't already have JDK)

Installation Instructions:

1. Netbeans can be downloaded from the internet.

2. Obtain AVR Studio 4 from ERTS lab personnel.

3. Open the project VisualFirebird in Netbeans and set the file

controller.WorkspaceController.java as the main class.

4. Run the project to open the programming interface, build

programs using it and generate the C code of the program built.

5. Open a new project in AVR studio 4 and in the include options

add the folder where the file firebird.h is present. Also add the

firebird.h file to the list of header files.

6. Then copy paste the code into a C file in the AVR project and

hit build. This will generate a set of files in a folder names

default at the same location as the C file.

7. Go to the deault folder mentioned above and burn the hex file

generated onto Firebird V.

* For saving and opening programs in the interface:

1. Save the program as a .xml file in a folder which has both the

following files present, lang_def.xml and lang_def.dtd.
