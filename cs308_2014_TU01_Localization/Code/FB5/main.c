/********************************************************************************
 Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.
 
 IDE: Keil uVision4
 Tool Chain: Realview MDK_ARM Ver 4.10
 C Compiler: ARMCC.exe

 Date: 1st OCT 2010
 
 Application example: Robot control over Xbee interface

 Concepts covered:  serial communication

 Serial Port used: UART1

 There are two components to the motion control:
 1. Direction control using pins P0.22, P1.21, P0.10, P0.11
 2. Velocity control by PWM on pins P0.7/PWM2 and P0.21/PWM5.

 In this experiment for the simplicity P0.7 and P0.21 are kept at logic 1.
 
 
 Connection Details:    L-1---->P1.21;    L-2---->P0.22;
              R-1---->P0.10;    R-2---->P0.11;
              P0.7 (PWM2) ----> Logic 1;  P0.21 (PWM5) ----> Logic 1;  


  Serial Communication: P0.1 --> RXD1 UART1 receive for RS232 serial communication
            P0.0 --> TXD1 UART1 transmit for RS232 serial communication


Serial communication baud rate: 9600bps
To control robot use number pad of the keyboard which is located on the right hand side of the keyboard.
Make sure that NUM lock is on.

Commands:
      Keyboard Key  HEX value Action
        8       0x38  Forward
        2       0x32  Backward
        4       0x34  Left
        6       0x36  Right
        5       0x35  Stop
        7       0x37  Buzzer on
        9       0x39  Buzzer off

 Note: 
 
 1. Note: Make sure that in the Target options following settings are 
  done for proper operation of the code

  Microcontroller: LPC2148
  Frequency: 12 Mhz
  Create Hex File: Checked (For more information read section 4.3.1 "Setting up Project in Keil uVision" in the hardware manual)

 2. Ensure that following settings are done in Startup.s configuration wizard:

  Clock Settings:
  
  PLL Steup  >>  MSEL=5, PSEL=2
  VPBDIV Setup >>  VPBCLK = CPU Clock/4
  
  For more details refer section 4.8 in the hardware manual.

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

#include  <lpc214x.h>
#include "lcd.h"
#include "defines.h"
#include <stdio.h>

/***************Macros*********************/ 
#define Fosc            12000000                    //10MHz~25MHz
#define Fcclk           (Fosc * 5)                  //Fosc(1~32)<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO Fcclk 2¡4¡8¡16±156MHz~320MHz
#define Fpclk           (Fcclk / 1) * 1             //VPB(Fcclk / 1) 1¡2¡4
#define UART_BPS  9600    							//Change Baud Rate Setting here


#define BUZZER_OFF() IO0CLR=(1<<25)      			//Macro to turn OFF buzzer
#define BUZZER_ON() IO0SET=(1<<25)       			//Macro to turn ON buzzer
/******************************************/

/*****Function Prototypes*********************/
void Delay(unsigned char j);
void Forward(void);
void Back(void);
void Left(void);
void Right(void);
void Stop(void);
void Soft_Left(void);
void Soft_Right(void);
void Soft_Left2(void);
void Soft_Right2(void);
void L_Forward(void);
void L_Back(void);
void R_Forward(void);
void R_Back(void);
void L_Stop(void);
void R_Stop(void);
void Init_Motion_Pin(void);
void Init_Buzzer_Pin(void);
void Init_Peripherals(void);
void Init_Ports(void);
void  __irq IRQ_UART0(void);
void  __irq IRQ_UART1(void);
void DelaymSec11(unsigned int Delay);
void Init_UART0(void);
void UART0_SendByte(unsigned char data);
void UART0_SendStr(const unsigned char *str);
void UART1_SendStr1(const unsigned char *str);
void Init_UART1(void);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);
/**********************************************/

/******************Global Variables****************/

extern unsigned char String1[16]; 
extern unsigned char String2[16];
unsigned char Temp=0;
unsigned int ADC_Data[3];
char numstr[20];
char tempstr[1000];
int index = -0;
/**************************************************/


void DelaymSec11(unsigned int Delay){  
	unsigned int i;
	for(; Delay>0; Delay--) 
	for(i=0; i<10000; i++);
}


/******************Init Functions****************/
//Initializes Buzzer pin
void Init_Buzzer_Pin(void){
	PINSEL1&=0xFFF3FFFF;   
	PINSEL1|=0x00000000;     	//Set P0.25 as GPIO
	IO0DIR&=0xFDFFFFFF;
	IO0DIR|= (1<<25);      		//Set P0.25 as Output
	BUZZER_OFF();        		//Initially turn OFF buzzer
}

//Initializes UART0 module. 
void Init_UART0(void){  
	unsigned int Baud16;
	PINSEL0&=0xFFFFFFF0;
	PINSEL0|=0x00000005;

	U0LCR = 0x83;                	// DLAB = 1
	Baud16 = (Fpclk / 16) / UART_BPS;  
	U0DLM = Baud16 / 256;              
	U0DLL = Baud16 % 256;            
	U0LCR = 0x03;
	U0IER = 0x00000001;    			// Enable Rx interrupts

	VICIntSelect = 0x00000000;   	// IRQ
	VICVectCntl1 = 0x20|6;     		// UART0
	VICVectAddr1 = (int)IRQ_UART0;  // UART0 Vector Address
	VICIntEnable = (1<<6); 			// Enable UART0 Rx interrupt
}

//Initializes UART1 module. 
void Init_UART1(void){  
	unsigned int Baud16;
	PINSEL0&=0xFFF0FFFF;
	PINSEL0|=0x00050000;

	U1LCR = 0x83;                	// DLAB = 1
	Baud16 = (Fpclk / 16) / UART_BPS;  
	U1DLM = Baud16 / 256;              
	U1DLL = Baud16 % 256;            
	U1LCR = 0x03;
	U1IER = 0x00000001;    			// Enable Rx interrupts

	VICIntSelect = 0x00000000;   	// IRQ
	VICVectCntl0 = 0x20|7;     		// UART1
	VICVectAddr0 = (int)IRQ_UART1;  // UART1 Vector Address
	VICIntEnable = (1<<7); 			// Enable UART1 Rx interrupt
}

//Initializes Motion control pins
void Init_Motion_Pin(void){
	PINSEL0&=0xFF0F3FFF;   
	PINSEL0|=0x00000000;   		//Set Port pins P0.7, P0.10, P0.11 as GPIO
	PINSEL1&=0xFFFFF0FF;
	PINSEL1|=0x00000000;   		//Set Port pins P0.21 and 0.22 as GPIO
	IO0DIR&=0xFF9FF37F;
	IO0DIR|= (1<<10) | (1<<11) | (1<<21) | (1<<22) | (1<<7);   //Set Port pins P0.10, P0.11, P0.21, P0.22, P0.7 as Output pins
	IO1DIR&=0xFFDFFFFF;
	IO1DIR|= (1<<21);    		// Set P1.21 as output pin
	Stop();        				// Stop both the motors on start up
	IO0SET = 0x00200080; 		// Set PWM pins P0.7/PWM2 and P0.21/PWM5 to logic 1
}

// Initializes all the ports.
void Init_Ports(void){
	Init_Motion_Pin();
	Init_Buzzer_Pin();
}

// Initializes all the peripherals
void Init_Peripherals(void){
	Init_Ports();
	Init_UART0();
	Init_UART1();
}

// Initializes ADC pins.
void Init_ADC_Pin(void){
	PINSEL0|= (BAT_VOL_IO_0 | BAT_VOL_IO_1 | WH_LINE_SEN_1_IO_0 | WH_LINE_SEN_1_IO_1 | SHR_IR_SEN_2_IO_0 | SHR_IR_SEN_2_IO_1 | SHR_IR_SEN_3_IO_0 | SHR_IR_SEN_3_IO_1 |SHR_IR_SEN_4_IO_0 | SHR_IR_SEN_4_IO_1) ; //Set pins P0.4, P0.5, P0.6, P0.12, P0.13 as ADC pins
	PINSEL1&= ~(WH_LINE_SEN_2_IO_1 | WH_LINE_SEN_3_IO_1) ;   
	PINSEL1|= (WH_LINE_SEN_2_IO_0 | WH_LINE_SEN_3_IO_0);     //Set pins P0.28, P0.29 as ADC pins
}

// This function initializes ADC module of LPC2148 microcontroller. It also configures the required I/o pins to be used as ADC pins. 
void Init_ADC0(void){
	AD0CR=0x00200E00;  	// SEL = 1  ADC0 channel 1  Channel 1
          				// CLKDIV = Fpclk / 1000000 - 1 ;1MHz
          // BURST = 0 
          // CLKS = 0 
          // PDN = 1 
          // START = 1
            // EDGE = 0 (CAP/MAT)
} 


// This function initializes ADC 1 module of LPC2148 microcontroller. It also configures the required I/o pins to be used as ADC pins.
void Init_ADC1(void){
	AD1CR=0x00200E00;  // SEL = 1  ADC0 channel 1  Channel 1
          // CLKDIV = Fpclk / 1000000 - 1 ;1MHz
          // BURST = 0 
          // CLKS = 0 
          // PDN = 1 
          // START = 1
            // EDGE = 0 (CAP/MAT)
}
/**************************************************/

void UpdateLeftPWM(unsigned int vel){
	PWMMR2 = vel;
	PWMLER = 0x04;
}

void UpdateRightPWM(unsigned int vel){
	PWMMR5 = vel;
	PWMLER = 0x20;
}

//This function is UART0 Receive ISR. This function is called whenever UART0 receives any data
void  __irq IRQ_UART0(void){  
	int k;
	Temp = U0RBR; 

	// The next piece of code is to handle the signal passed. The values compared are ASCII values of the signal passed.    
	if(Temp == 0x68){ 					//ASCII value of h. Buzzer is switched on.
		BUZZER_ON();
	}
	if(Temp == 0x69){ 					//ASCII value of i. Buzzer is switched off.
		BUZZER_OFF();
	} 

	if(Temp == 0x63){ 					//ASCII value of c. Sends the bot into command mode.
		UART1_SendStr("$$$");
		DelaymSec11(500);
	}

	if(Temp == 0x62){ 					//ASCII value of b. Asks for the RSSI values from the wifly shield.
		UART1_SendStr("show rssi");
		UART1_SendByte(0xD);
	}

	if(Temp == 0x65){ 					//ASCII value of e. Connects to ERTS_1.
		UART1_SendStr("set wlan0 passphrase local@erts123");
		UART1_SendByte(0xD);
		DelaymSec11(500);
		UART1_SendStr("join ERTS_1");
		UART1_SendByte(0xD);
	}

	if(Temp == 0x66){ 					//ASCII value of f. Connects to ERTS_2.
		UART1_SendStr("set wlan0 passphrase local@erts123");
		UART1_SendByte(0xD);
		DelaymSec11(500);
		UART1_SendStr("join ERTS_2");
		UART1_SendByte(0xD);
	}

	if(Temp == 0x67){ 					//ASCII value of g. Connects to ERTS_3.
		UART1_SendStr("set wlan0 passphrase local@erts123");
		UART1_SendByte(0xD);
		DelaymSec11(500);
		UART1_SendStr("join ERTS_3");
		UART1_SendByte(0xD);
	}

	if(Temp == 0x77){					//ASCII value of w. Moves the bot in the forward direction.
		Forward();
	}

	if(Temp == 0x73){					//ASCII value of s. Moves the bot in the backward direction.
		Back();
	}

	if(Temp == 0x61){					//ASCII value of a. Moves the bot in the left direction.
		Left();
	}

	if(Temp == 0x64){					//ASCII value of d. Moves the bot in the right direction.
		Right();
	}

	if(Temp == 0x71){					//ASCII value of q. Stops the bot.
		Stop();
	}

	if(Temp == 0x48){					//ASCII value of H. Pings server for testing purposes.
		UART1_SendStr("ping mars.cse.iitb.ac.in");
		UART1_SendByte(0xD);
		DelaymSec11(500);
		UART1_SendStr("show rssi");
		UART1_SendByte(0xD);
	}

	VICVectAddr = 0x00;
}

//This function is UART1 Receive ISR. This functions is called whenever UART1 receives any data
void  __irq IRQ_UART1(void){  
	Temp = U1RBR;   
	UART0_SendByte(Temp);
	VICVectAddr = 0x00;
}   

//This function sends a single character on the serial port UART0
void UART0_SendByte(unsigned char data){
	U0THR = data;            
	while( (U0LSR&0x40)==0 );      
}

//This function sends a string of characters on the serial port UART0
void UART0_SendStr(const unsigned char *str){  
	while(1){  
		if( *str == '\0' ) break;
		UART0_SendByte(*str++);     
	}
}

//This function sends a string of characters on the serial port UART1
void UART1_SendStr1(const unsigned char *str){  
	while(1){  
		if( *str == '\0' ) break;
		UART1_SendByte(*str++);
		DelaymSec11(10);    
	}
}

//This function sends a single character on the serial port UART1
void UART1_SendByte(unsigned char data){  
	U1THR = data;            
	while( (U1LSR&0x40)==0 );      
}

//This function sends a string of characters on the serial port UART1
void UART1_SendStr(const unsigned char *str){  
	while(1){  
		if( *str == '\0' ) break;
		UART1_SendByte(*str++);     
	}
}

//Function to move Left motor forward
void L_Forward(void){
	IO1SET = 0x00200000;   //Set P1.21 to logic '1'
}

//Function to move Left motor backward
void L_Back(void){
	IO0SET = 0x00400000;   //Set P0.22 to logic '1'
}

//Function to move Right motor forward
void R_Forward(void){
	IO0SET = 0x00000400;   //Set P0.10 to logic '1'
}

//Function to move Right motor backward
void R_Back(void){
	IO0SET = 0x00000800;   //Set P0.11 to logic '1'
}

//Function to stop left motor
void L_Stop(void){
	IO1CLR = 0x00200000;   //Set P1.21 to logic '0'
	IO0CLR = 0x00400000;   //Set P0.22 to logic '0'
}

//Function to stop Right motor
void R_Stop(void){
	IO0CLR = 0x00000400;   //Set P0.10 to logic '0'
	IO0CLR = 0x00000800;   //Set P0.11 to logic '0'
}

//Function to move robot in forward direction
void Forward(void){
	Stop();
	L_Forward();
	R_Forward();
}

//Function to move robot in backward direction
void Back(void){
	Stop();
	L_Back();
	R_Back();
}

//Function to turn robot in Left direction
void Left(void){ 
	Stop();
	L_Back();
	R_Forward();
}

//Function to turn robot in right direction
void Right(void){ 
	Stop();
	L_Forward();
	R_Back();
}

//Function to turn robot in Left direction by moving right wheel forward
void Soft_Left(void){
	Stop();
	R_Forward();
}

//Function to turn robot in right direction by moving left wheel forward
void Soft_Right(void){
	Stop();
	L_Forward();
}

//Function to turn robot in left direction by moving left wheel backward
void Soft_Left2(void){
	Stop();
	L_Back();
}

//Function to turn robot in right direction by moving right wheel backward 
void Soft_Right2(void){
	Stop();
	R_Back();
}

//Function to stop the robot at its current location
void Stop(void){
	L_Stop();
	R_Stop();
}

//This function converts ADC0 channels. Channel number is passed to this function as integer.
unsigned int AD0_Conversion(unsigned char channel){
	unsigned int Temp;
	AD0CR &= ~ (1<<24 | 1<<25 | 1<<26); 			// stop adc 
	AD0CR = (AD0CR & 0xFFFFFF00) | (1<<channel);
	AD0CR|=(1 << 24);   							//start the conversion
	while((AD0GDR&0x80000000)==0);
	Temp = AD0GDR;           
	Temp = (Temp>>8) & 0x00FF;   					// 8 bit result
	//Temp = (Temp>>6) & 0x03FF;   					// 10 bit result
	return Temp;
}

// Main initialization function. Initializes all the other Init functions
void Init(void){
	Init_LCD_Pin();
	Init_ADC_Pin();
	Init_ADC0();
	Init_ADC1();
	Init_Peripherals();
}

//This function converts ADC1 channels. Channel number is passed to this function as integer.
unsigned int AD1_Conversion(unsigned char channel){
	unsigned int Temp;
	AD1CR &= ~ (1<<24 | 1<<25 | 1<<26); 			// stop adc 
	AD1CR = (AD1CR & 0xFFFFFF00) | (1<<channel);
	AD1CR|=(1 << 24);     							//start the conversion
	while((AD1GDR&0x80000000)==0);
	Temp = AD1GDR;           
	Temp = (Temp>>8) & 0xFF;  						// 8 bit result
	//Temp = (Temp>>6) & 0x03FF;   					// 10 bit result
	return Temp;
}

// Function Main
int  main(void){  
	PINSEL0 = 0x00000005;        // Reset all pins as GPIO
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	Init();
	BUZZER_ON();
	UART0_SendByte('H');
	DelaymSec11(1000);
	BUZZER_OFF();
	UART0_SendByte('i');
	while(1){

	}
}
