/********************************************************************************
 Written by: Vinod Desai, NEX Robotics Pvt. Ltd.
 Edited by: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
 AVR Studio Version 4.17, Build 666

 Date: 26th December 2010
 
 This experiment demonstrates Servo motor control using 10 bit fast PWM mode.

 Concepts covered:  Use of timer to generate PWM for servo motor control

 Fire Bird V ATMEGA2560 microcontroller board has connection for 3 servo motors (S1, S2, S3).
 Servo motors move between 0 to 180 degrees proportional to the pulse train 
 with the on time of 0.6 to 2 ms with the frequency between 40 to 60 Hz. 50Hz is most recommended.

 We are using Timer 1 at 10 bit fast PWM mode to generate servo control waveform.
 In this mode servo motors can be controlled with the angular resolution of 1.86 degrees.
 Although angular resolution is less this is very simple method.
 
 There are better ways to produce very high resolution PWM but it involves interrupts at the frequency of the PWM.
 High resolution PWM is used for servo control in the Hexapod robot.
  
 Connection Details:	PORTB 5 OC1A --> Servo 1: Camera pod pan servo
 						PORTB 6 OC1B --> Servo 2: Camera pod tile servo
						PORTB 7 OC1C --> Servo 3: Reserved 
 					  	
 Note: 
 
 1. Make sure that in the configuration options following settings are 
 	done for proper operation of the code

 	Microcontroller: atmega2560
 	Frequency: 14745600
 	Optimization: -O0 (For more information read section: Selecting proper optimization 
 					options below figure 2.22 in the Software Manual)

 2. 5V supply to these motors is provided by separate low drop voltage regulator "5V Servo" which can
 	supply maximum of 800mA current. It is a good practice to move one servo at a time to reduce power surge 
	in the robot's supply lines. Also preferably take ADC readings while servo motor is not moving or stopped
	moving after giving desired position.
*********************************************************************************/

/********************************************************************************

   Copyright (c) 2010, NEX Robotics Pvt. Ltd.                       -*- c -*-
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commence cc by-nc-sa licence.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

********************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

int MAX_X = 640;
int MAX_Y = 480;
unsigned char initial_angle;
//Configure PORTB 5 pin for servo motor 1 operation
void servo1_pin_config (void)
{
 DDRB  = DDRB | 0x20;  //making PORTB 5 pin output
 PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
}

//Configure PORTB 6 pin for servo motor 2 operation
void servo2_pin_config (void)
{
 DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
 PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
}

//Configure PORTB 7 pin for servo motor 3 operation
void servo3_pin_config (void)
{
 DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
 PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
}

//Initialize the ports
void port_init_motion(void)
{
 servo1_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
 servo2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation 
 servo3_pin_config(); //Configure PORTB 7 pin for servo motor 3 operation  
}

//TIMER1 initialization in 10 bit fast PWM mode  
//prescale:256
// WGM: 7) PWM 10bit fast, TOP=0x03FF
// actual value: 52.25Hz 
void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
 TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
 OCR1AH = 0x03;	//Output compare Register high value for servo 1
 OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
 OCR1BH = 0x03;	//Output compare Register high value for servo 2
 OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
 OCR1CH = 0x03;	//Output compare Register high value for servo 3
 OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
 ICR1H  = 0x03;	
 ICR1L  = 0xFF;
 TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 					For Overriding normal port functionality to OCRnA outputs.
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
 TCCR1C = 0x00;
 TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}


//Function to initialize all the peripherals
void init_devices_motion(void)
{
 cli(); //disable all interrupts
 port_init_motion();
 timer1_init();
 sei(); //re-enable interrupts 
}


//Function to rotate Servo 1 by a specified angle in the multiples of 1.86 degrees
void servo_1(unsigned char degrees)  
{
 float PositionPanServo = 0;
  PositionPanServo = ((float)degrees / 1.86) + 35.0;
 OCR1AH = 0x00;
 OCR1AL = (unsigned char) PositionPanServo;
}


//Function to rotate Servo 2 by a specified angle in the multiples of 1.86 degrees
void servo_2(unsigned char degrees)
{
 float PositionTiltServo = 0;
 PositionTiltServo = ((float)degrees / 1.86) + 35.0;
 OCR1BH = 0x00;
 OCR1BL = (unsigned char) PositionTiltServo;
}

//Function to rotate Servo 3 by a specified angle in the multiples of 1.86 degrees
void servo_3(unsigned char degrees)
{
 float PositionServo = 0;
 PositionServo = ((float)degrees / 1.86) + 35.0;
 OCR1CH = 0x00;
 OCR1CL = (unsigned char) PositionServo;
}

//servo_free functions unlocks the servo motors from the any angle 
//and make them free by giving 100% duty cycle at the PWM. This function can be used to 
//reduce the power consumption of the motor if it is holding load against the gravity.

void servo_1_free (void) //makes servo 1 free rotating
{
 OCR1AH = 0x03; 
 OCR1AL = 0xFF; //Servo 1 off
}

void servo_2_free (void) //makes servo 2 free rotating
{
 OCR1BH = 0x03;
 OCR1BL = 0xFF; //Servo 2 off
}

void servo_3_free (void) //makes servo 3 free rotating
{
 OCR1CH = 0x03;
 OCR1CL = 0xFF; //Servo 3 off
} 


void move_servo2_forward(void)	//move servo2(upper arm) forward
{
    unsigned char i = 0;
	
	servo_2(30);
	_delay_ms(30);	//intially move to 30 degrees

	for (i=30;i<=145;i+=5)	//moves from 30 to 145 degrees with 5 degree increment 
	{
		servo_2(i);
		_delay_ms(30);		//give 30ms delay
	}
}

void move_servo2_backward(void)	//move servo2(upper arm) backward
{
    unsigned char i = 0;

	servo_2(145);	//intially move to 145 degrees
	_delay_ms(30);

	for (i=145;i>=30;i-=5)	//move from 145 to 30 degrees with 5 degree decrement
	{
		servo_2(i);
		_delay_ms(30);	//give dealy of 30ms
	}
}

void move_servo1_forward(void)	//move servo1(lower arm) forward
{
    unsigned char i = 0;
	
	servo_1(0);	
	_delay_ms(30);	

	for (i=30;i<=120;i+=15)		//moves from 30 to 120 degrees with 15 degree increment
	{
		servo_1(i);
		_delay_ms(30);		//give dealy of 30ms
	}
}

void move_servo1_backward(void)
{
    unsigned char i = 0;

	servo_1(120);	//intially move to 120 degrees
	_delay_ms(30);

	for (i=120;i>30;i-=10)		//moves from 120 to 30 degrees with 10 degree decrement
	{
		servo_1(i);
		_delay_ms(30);		//give dealy of 30ms
	}
}


void servo1_init(void)	//initialize servo1(lower arm)
{
    unsigned char i = 0;

	servo_1(60);	//to 60(depends on intial pos of motor) degrees,nearly vertical 
	_delay_ms(30);

}

/*move bot such that tomato detected aligns in the center*/
/*our camera rotated 90 degrees, so x in image corresponds to y in real*/
void align_center(unsigned int x, unsigned int y)
{
   unsigned int diff;
   if (x<MAX_X/2)	//above the center, therefore move upwards
   {
	  //diff = abs(x-MAX_X/2);
	  //servo_2(initial_angle+diff/15);
	  initial_angle+=1;		//move the hand upwards
	  servo_2(initial_angle);
	  _delay_ms(100);
   }

   else	//below the center, therefore move downwards 
   {
	  //diff = abs(x-MAX_X/2);
	  //servo_2(initial_angle-diff/15);
	  initial_angle-=1;	//move the hand downwards
	  servo_2(initial_angle);
	  _delay_ms(100);    
   }


   if (y<MAX_Y/2)	//left to center
   {
      PORTA=0x09;	//move backward
	  velocity(250,250);
	  //diff = abs(y-MAX_Y/2);
	  //_delay_ms(diff/100);
	  _delay_ms(20);	
	  PORTA=0x00;	  //stop
   }

   else	//right to center
   {
	  PORTA=0x06;	//move forward
	  velocity(250,250);
	  //diff = abs(y-MAX_Y/2);
	  //_delay_ms(diff/100);
	  _delay_ms(20);
	  PORTA=0x00;
   }
	
}

void pluck_fruit()	//cutting the fruit, servo 3 initially at 180 degree, open pos
{
	servo_3(120);	//move to 120 degrees close position
	_delay_ms(300);
	servo_3(120);
}

