/********************************************************************************
 Written by: Vinod Desai, NEX Robotics Pvt. Ltd.
 Edited by: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
 AVR Studio Version 4.17, Build 666

 Date: 26th December 2010

 Application example: Robot control over serial port via XBee wireless communication module 
 					  (located on the ATMEGA260 microcontroller adaptor board)

 Concepts covered:  serial communication
 
 Serial Port used: UART0

 There are two components to the motion control:
 1. Direction control using pins PORTA0 to PORTA3
 2. Velocity control by PWM on pins PL3 and PL4 using OC5A and OC5B.

 In this experiment for the simplicity PL3 and PL4 are kept at logic 1.
 
 Pins for PWM are kept at logic 1.
  
 Connection Details:  	
 						
  Motion control:		L-1---->PA0;		L-2---->PA1;
   						R-1---->PA2;		R-2---->PA3;
   						PL3 (OC5A) ----> Logic 1; 	PL4 (OC5B) ----> Logic 1; 


  Serial Communication:	PORTD 2 --> RXD1 UART1 receive for RS232 serial communication
						PORTD 3 --> TXD1 UART1 transmit for RS232 serial communication

						PORTH 0 --> RXD2 UART 2 receive for USB - RS232 communication
						PORTH 1 --> TXD2 UART 2 transmit for USB - RS232 communication

						PORTE 0 --> RXD0 UART0 receive for ZigBee wireless communication
						PORTE 1 --> TXD0 UART0 transmit for ZigBee wireless communication

						PORTJ 0 --> RXD3 UART3 receive available on microcontroller expansion socket
						PORTJ 1 --> TXD3 UART3 transmit available on microcontroller expansion socket

Serial communication baud rate: 9600bps
To control robot use number pad of the keyboard which is located on the right hand side of the keyboard.
Make sure that NUM lock is on.

Commands:
			Keyboard Key   ASCII value	Action
				8				0x38	Forward
				2				0x32	Backward
				4				0x34	Left
				6				0x36	Right
				5				0x35	Stop
				7				0x37	Buzzer on
				9				0x39	Buzzer off

 Note: 
 
 1. Make sure that in the configuration options following settings are 
 	done for proper operation of the code

 	Microcontroller: atmega2560
 	Frequency: 14745600
 	Optimization: -O0 (For more information read section: Selecting proper optimization 
 						options below figure 2.22 in the Software Manual)

 2. Difference between the codes for RS232 serial, USB and wireless communication is only in the serial port number.
 	Rest of the things are the same. 

 3. For USB communication check the Jumper 1 position on the ATMEGA2560 microcontroller adaptor board

 4. Auxiliary power can supply current up to 1 Ampere while Battery can supply current up to 
 	2 Ampere. When both motors of the robot changes direction suddenly without stopping, 
	it produces large current surge. When robot is powered by Auxiliary power which can supply
	only 1 Ampere of current, sudden direction change in both the motors will cause current 
	surge which can reset the microcontroller because of sudden fall in voltage. 
	It is a good practice to stop the motors for at least 0.5seconds before changing 
	the direction. This will also increase the useable time of the fully charged battery.
	the life of the motor.

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


#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "motion.h"
#include "line_follower.h"
unsigned char data; //to store received data from UDR1
unsigned int num_read_mode = 0; //to read data from Zigbee
unsigned int count = 0;  //to count the number of digits of number read
unsigned int sum_x = 0;  //to read x value(tomato center) send by zigbee
unsigned int sum_y = 0;	 //to read y value(tomato center) send by zigbee 	
unsigned int row_no = 0, col_no = 0; //row, col where tomato is detected
unsigned char bot_mode = 0;	//1 for monitoring mode , 2 for plucking mode
unsigned char row_read_mode = 0; //to read row for plucking mode
unsigned char col_read_mode = 0; //to read row for plucking mode

void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

void motion_pin_config (void)
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to initialize ports
void port_init()
{
	motion_pin_config();
	buzzer_pin_config();
}

void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}

//Function To Initialize UART0
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x5F; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}

	

SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	data = UDR0; 				//making copy of data from UDR0 in 'data' variable 

	//UDR0 = data; 				//echo data back to PC
	

		/*read the row from serial port and move to column read mode*/
		if (row_read_mode == 1)
		{
			row_no = (int)data -48; //read the row value
			col_read_mode = 1;		//move to column read mode
			row_read_mode = 0;
		}

		else if (col_read_mode == 1)
		{
			col_no = (int)data - 48;	//read the column value
			col_read_mode = 0;
			lcd_print(1, 1, row_no, 3);	//print row value on first line of lcd	
			lcd_print(2, 1, col_no, 3);	//print column value on second line of lcd
			move_bot(row_no, col_no);	//move the bot to given row and column
			UDR0 = 0x31;				//send success signal

		}
		
		/*used to read x and y values of tomato center, each 3 digit*/
		else if (num_read_mode == 1)	//to read x val
		{
		 	if (count==0)
			{
				sum_x+= ((int)data-48);	//add units digit of x	
			}
			
			if (count==1)
			{
				sum_x+= ((int)data-48)*10;	//add tens digit of x
			}

			if (count==2)
			{
				sum_x+= ((int)data-48)*100;	//add hundred's digit of x
				lcd_print(1, 1, sum_x, 3);	//print the x value detected
				num_read_mode = 2;			//go to read y_value
			} 

			count = (count+1)%3;
		}	
		

		else if (num_read_mode ==2)		//to read y value
		{
		 	if (count==0)
			{
				sum_y+= ((int)data-48);	//add units digit of y
			}
			
			if (count==1)
			{
				sum_y+= ((int)data-48)*10;	//add tens digit of x
			}

			if (count==2)
			{
				sum_y+= ((int)data-48)*100;	//add tens digit of x
				lcd_print(2, 1, sum_y, 3);	//print detected y value

				//checks if tomato center is in center of window, within certain range
				if (sum_x<=MAX_X/2+75 && sum_x>=MAX_X/2-75 && sum_y<=MAX_Y/2+75 && sum_y>=MAX_Y/2-75)
				{
					UDR0 = 0x32;	//send sucess, stop detecting
					_delay_ms(2000);
					buzzer_on();	
					_delay_ms(2000);
					buzzer_off();
					pluck_fruit();	//call the pluck fruit,which cuts
					while (initial_angle>5)
					{
						initial_angle-=5;	//pulls down
						servo_2(initial_angle);
					}
					servo_3(60);	
				}
				else
				{	 
					align_center(sum_x,sum_y);	//if not in center, call align center
					_delay_ms(2000);
					UDR0 = 0x31;
				}

				sum_x = 0;	//intialize back values for next iteration
				sum_y = 0;
				num_read_mode = 0;
			} 

			count = (count+1)%3;
		}

		else if(data == 0x30) //ASCII value of 0, tomato not found
		{	
			initial_angle-=5;	//decrement angle by 5 degree, moves arm downwards
			if (initial_angle>=5)	//if not reached the bottom
			{
				servo_2(initial_angle);	//move to new position
				UDR0 = 0x31;			//send the signal
			}
			else if (initial_angle<5)	//if reached bottom
			{
				initial_angle = 120;	//move to top, 120 degrees
				servo_2(initial_angle);

				if (end_reached==1)		//if end of grid reached in monitoring mode
				{
					UDR0 = 0x34;		//send corresponding signal
					_delay_ms(1000);
					UDR0 = (unsigned char)((int)tom_found+48);	//send no. of tomatoes found
					
					//send row and column no's where tomaotes are found
					for (int iter=0;iter<tom_found;iter++)
					{
						_delay_ms(1000);
						UDR0 = (unsigned char)((int)row_found_arr[iter]+48);
						_delay_ms(1000);
						UDR0 = (unsigned char)((int)col_found_arr[iter]+48);
					}
				}				
				//if not reached the end, 
				else
				{
					if (bot_mode==1)
				    {
						PORTA=0x06;			//move forward and do line following in monitoring mode
						velocity(250,250);
						_delay_ms(200);
						PORTA=0x00;
						line_follower();
						//UDR0 = 0x31;
					}
					else
						UDR0 = 0x31;		//don't move forward, repeat there itself
				}		
				
			}
			else
			{
				UDR0 = 0x30;
				_delay_ms(2000);
				pluck_fruit();
				initial_angle+=15;
				servo_2(initial_angle);
			}
		}		

		
		else if(data == 0x31) //ASCII value of 1, tomato found
		{
			if (bot_mode==2)	//plucking mode
				num_read_mode = 1;	//read x and y values of center to align
			else if (bot_mode==1) 	//monitoring mode
			{
				num_read_mode = 0;
				row_found_arr[tom_found] = row_found;	//store the row where it is found
				col_found_arr[tom_found] = col_found;	//store the col where it is found
				tom_found++;	//increase the no. of tomatoes found

				initial_angle = 120;	//move the hand to top
				servo_2(initial_angle);

				if (end_reached==1)		//if end of grid reached
				{
					UDR0 = 0x34;	//indicate the signal
					_delay_ms(1000);
					UDR0 = (unsigned char)((int)tom_found+48); //send the number of tomatoes found
				
					//send the row no and col no where tomatoes are found
					for (int iter=0;iter<tom_found;iter++)
					{
						_delay_ms(1000);
						UDR0 = (unsigned char)((int)row_found_arr[iter]+48);
						_delay_ms(1000);
						UDR0 = (unsigned char)((int)col_found_arr[iter]+48);
					}
				}				
				else	//if end of grid not reached
				{
				    PORTA=0x06;	//move forward and do line following
					velocity(250,250);
					_delay_ms(200);
					PORTA=0x00;
					line_follower();
					//UDR0 = 0x31;
				}	
				
			}		
			//PORTA=0x06;  //forward
		}

		else if (data == 0x32)	//ASCII value of 2, input for monitoring mode
		{
			bot_mode = 1;	//put in monitoring mode
			row_found = 0;
			col_found = 0;
			line_follower();	//start monitoring 
		}

		else if (data == 0x33)	//ASCII value of 3, input for plucking mode
		{
			bot_mode = 2;	//put in plucking mode
			row_read_mode = 1;	//start reading row and column no.s to pluck
		}
}


//Function To Initialize all The Devices
void init_devices()
{
 cli(); //Clears the global interrupts
 port_init();  //Initializes all the ports
 uart0_init(); //Initailize UART1 for serial communiaction
 sei();   //Enables the global interrupts
}

//Main Function
int main(void)
{   
	//all initializations
    init_devices_motion();
	init_devices();
	init_devices_lf();
	lcd_set_4bit();
	lcd_init();

	//setting gripper arm 
	initial_angle = 120;
	servo1_init();
	servo_2(initial_angle);
	servo_3(180);
	//UDR0 = 0x31;
	//line_follower();
	//pluck_fruit();

	while(1);
}

