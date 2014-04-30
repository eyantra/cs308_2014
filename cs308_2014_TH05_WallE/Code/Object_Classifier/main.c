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
 
 
 Connection Details:  	L-1---->P1.21;		L-2---->P0.22;
   						R-1---->P0.10;		R-2---->P0.11;
   						P0.7 (PWM2) ----> Logic 1; 	P0.21 (PWM5) ----> Logic 1;  


  Serial Communication:	P0.1 --> RXD1 UART1 receive for RS232 serial communication
						P0.0 --> TXD1 UART1 transmit for RS232 serial communication


Serial communication baud rate: 9600bps
To control robot use number pad of the keyboard which is located on the right hand side of the keyboard.
Make sure that NUM lock is on.

Commands:
			Keyboard Key	HEX value	Action
				8				0x38	Forward
				2				0x32	Backward
				4				0x34	Left
				6				0x36	Right
				5				0x35	Stop
				7				0x37	Buzzer on
				9				0x39	Buzzer off

 Note: 
 
 1. Note: Make sure that in the Target options following settings are 
 	done for proper operation of the code

 	Microcontroller: LPC2148
 	Frequency: 12 Mhz
 	Create Hex File: Checked (For more information read section 4.3.1 "Setting up Project in Keil uVision" in the hardware manual)

 2. Ensure that following settings are done in Startup.s configuration wizard:

 	Clock Settings:
	
	PLL Steup	 >>	 MSEL=5, PSEL=2
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
#include "LCD.h"
#include <math.h>
#include "defines.h"

/***************Macros*********************/ 
#define Fosc            12000000                    //10MHz~25MHz
#define Fcclk           (Fosc * 5)                  //Fosc(1~32)<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO Fcclk 2񅦰�16�156MHz~320MHz
#define Fpclk           (Fcclk / 1) * 1             //VPB(Fcclk / 1) 1񄿔
#define UART_BPS	9600 		//Change Baud Rate Setting here


#define BUZZER_OFF() IO0CLR=(1<<25)		   //Macro to turn OFF buzzer
#define BUZZER_ON() IO0SET=(1<<25)		   //Macro to turn ON buzzer
/******************************************/

/*****Function Prototypes*********************/

void Delay(unsigned char j);
void Sensor_Values(void);
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
void Init_LCD_Pin(void);
void Init_Motion_Pin(void);
void Init_Buzzer_Pin(void);
void Init_Peripherals(void);
void Init_Ports(void);
void Init(void);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
unsigned int AD0_Conversion(unsigned char channel);
unsigned int AD1_Conversion(unsigned char channel);
unsigned int Sharp_GP2D12_Estimation(unsigned int Val);
unsigned int Batt_Voltage_Conversion(unsigned int Val);
void  __irq IRQ_UART1(void);
void DelaymSec(unsigned int Delay);
void Init_UART1(void);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);

/*Our functions*/
void Start_Conveyor(void);
void Stop_Conveyor(void);
void Push_Arm(void);
void Print_Python_Input(unsigned int n);


/**********************************************/

/******************Global Variables****************/

unsigned char Temp=0;
unsigned int ADC_Data[8];

/**************************************************/

/************************************************************

	Function 		: Init_ADC_Pin
	Return type		: None
	Parameters		: None
	Description 	: Initialises the required GPIOs as ADC pins
************************************************************/

void Init_ADC_Pin(void)
{
 PINSEL0|= (BAT_VOL_IO_0 | BAT_VOL_IO_1 | WH_LINE_SEN_1_IO_0 | WH_LINE_SEN_1_IO_1 | SHR_IR_SEN_2_IO_0 | SHR_IR_SEN_2_IO_1 | SHR_IR_SEN_3_IO_0 | SHR_IR_SEN_3_IO_1 |SHR_IR_SEN_4_IO_0 | SHR_IR_SEN_4_IO_1) ;	//Set pins P0.4, P0.5, P0.6, P0.12, P0.13 as ADC pins
 PINSEL1&= ~(WH_LINE_SEN_2_IO_1 | WH_LINE_SEN_3_IO_1) ;		
 PINSEL1|= (WH_LINE_SEN_2_IO_0 | WH_LINE_SEN_3_IO_0);	    //Set pins P0.28, P0.29 as ADC pins
}


/************************************************************

	Function 		: ADC0_Init
	Return type		: None
	Parameters		: None
	Description 	: This fuction initialises ADC 0
					  module of LPC2148 microcontroller. It also 
					  configures the required I/o pins to be used as 
					  ADC pins. 
************************************************************/
void Init_ADC0(void)
{
 AD0CR=0x00200E00;	// SEL = 1 	ADC0 channel 1	Channel 1
					// CLKDIV = Fpclk / 1000000 - 1 ;1MHz
					// BURST = 0 
					// CLKS = 0 
 					// PDN = 1 
 					// START = 1
  					// EDGE = 0 (CAP/MAT)
} 


/************************************************************

	Function 		: ADC1_Init
	Return type		: None
	Parameters		: None
	Description 	: This fuction initialises ADC 1
					  module of LPC2148 microcontroller. It also 
					  configures the required I/o pins to be used as 
					  ADC pins. 
************************************************************/
void Init_ADC1(void)
{
 AD1CR=0x00200E00;	// SEL = 1 	ADC0 channel 1	Channel 1
					// CLKDIV = Fpclk / 1000000 - 1 ;1MHz
					// BURST = 0 
					// CLKS = 0 
 					// PDN = 1 
 					// START = 1
  					// EDGE = 0 (CAP/MAT)
} 
                           

//This function converts ADC0 channels. Channel number is passed to this function as integer.
unsigned int AD0_Conversion(unsigned char channel)
{
 unsigned int Temp;
 AD0CR &= ~ (1<<24 | 1<<25 | 1<<26); // stop adc 
 AD0CR = (AD0CR & 0xFFFFFF00) | (1<<channel);
 AD0CR|=(1 << 24);	 //start the conversion
 while((AD0GDR&0x80000000)==0);
 Temp = AD0GDR;						
 Temp = (Temp>>8) & 0x00FF;	  // 8 bit result
 //Temp = (Temp>>6) & 0x03FF;	  // 10 bit result
 return Temp;
}

//This function converts ADC1 channels. Channel number is passed to this function as integer.
unsigned int AD1_Conversion(unsigned char channel)
{
 unsigned int Temp;
 AD1CR &= ~ (1<<24 | 1<<25 | 1<<26); // stop adc 
 AD1CR = (AD1CR & 0xFFFFFF00) | (1<<channel);
 AD1CR|=(1 << 24);	   //start the conversion
 while((AD1GDR&0x80000000)==0);
 Temp = AD1GDR;						
 Temp = (Temp>>8) & 0xFF;  // 8 bit result
 //Temp = (Temp>>6) & 0x03FF;	  // 10 bit result
 return Temp;
}

//This Function estimates the raw digital data of Sharp sensor in mm
unsigned int Sharp_GP2D12_Estimation(unsigned int Val)
{
 float Distance;
 unsigned int DistanceInt;
 Distance = (int)(10.00*(2799.6*(1.00/(pow(Val,1.1546)))));
 DistanceInt = (int)Distance;
 if(DistanceInt>800)
 {
  DistanceInt=800;
 }
 return DistanceInt;
}

//This function convetrs the raw digital data of battery to represent actual battery voltage
unsigned int Batt_Voltage_Conversion(unsigned int Val)
{
 float Batt_Voltage;
 unsigned int Batt_Voltage_Int=0;
 Batt_Voltage = ((float)Val* 0.05226 * 100.0) + 0.7;	//0.05226= (3.3/255)*(1/(3.3/13.3))
														//0.7 = Drop across diode
														//100.0 = Shifting decimal point by 2
 Batt_Voltage_Int = (unsigned int)Batt_Voltage;
 return Batt_Voltage_Int;
}




void Init(void)
{
 Init_LCD_Pin();
 Init_ADC_Pin();
 Init_ADC0();
 Init_ADC1();
}



void DelaymSec(unsigned int Delay)	//App. 1mSec Delay
{  
   unsigned int i;
   for(; Delay>0; Delay--) 
   for(i=0; i<10000; i++);
}


/************************************************************

	Function 		: Init_Buzzer_Pin
	Return type		: None
	Parameters		: None
	Description 	: Initialises Buzzer pin
************************************************************/

void Init_Buzzer_Pin(void)
{
 PINSEL1&=0xFFF3FFFF;		
 PINSEL1|=0x00000000; 		//Set P0.25 as GPIO
 IO0DIR&=0xFDFFFFFF;
 IO0DIR|= (1<<25);  		//Set P0.25 as Output
 BUZZER_OFF();				//Initially turn OFF buzzer
}


//This function is UART0 Receive ISR. This functions is called whenever UART0 receives any data
void  __irq IRQ_UART1(void)
{  
 Temp = U1RBR;			

 
 if(Temp == 0x31) //ASCII value of 1
 {
  Start_Conveyor();		  //Start Coneyor belt
  Print_Python_Input(1); //Print 1 on LCD panel
 } 
 
 if(Temp == 0x32) //ASCII value of 2
 {
  Stop_Conveyor();	//Stop conveyor belt 
  Print_Python_Input(2);	//Print 2 on LCD panel
 }

 if(Temp == 0x33) //ASCII value of 3
 {
  Push_Arm();	//Push the mechanical arm.
  Print_Python_Input(3);  //Print 3 on LCD panel
 }

 if(Temp == 0x34) //ASCII value of 4
 {
  Left();  //left
 }
  
 if(Temp == 0x36) //ASCII value of 6
 {
  Right(); //right
 }

 if(Temp == 0x35) //ASCII value of 5
 {
  Stop(); //stop
 }

 if(Temp == 0x37) //ASCII value of 7
 {
  BUZZER_ON();
  Temp = 'B';
 }

 if(Temp == 0x38) //ASCII value of 8
 {
  Forward();  //forward
 }

 if(Temp == 0x39) //ASCII value of 9
 {
  BUZZER_OFF();
  Temp = 'C';
 }
 
 
 
 VICVectAddr = 0x00;
 //UART1_SendByte(Temp);	// Echo Back received character
}		


/************************************************************

	Function 		: Init_UART1
	Return type		: None
	Parameters		: None
	Description 	: Initialises UART1 module. 
************************************************************/
void Init_UART1(void)
{  
   unsigned int Baud16;
   PINSEL0&=0xFFF0FFFF;
   PINSEL0|=0x00050000;

   U1LCR = 0x83;		            // DLAB = 1
   Baud16 = (Fpclk / 16) / UART_BPS;  
   U1DLM = Baud16 / 256;							
   U1DLL = Baud16 % 256;						
   U1LCR = 0x03;
   U1IER = 0x00000001;		//Enable Rx interrupts

   VICIntSelect = 0x00000000;		// IRQ
   VICVectCntl0 = 0x20|7;			// UART1
   VICVectAddr0 = (int)IRQ_UART1; 	//UART1 Vector Address
   VICIntEnable = (1<<7);	// Enable UART1 Rx interrupt

}
				

//This function sends a single character on the serial port
void UART1_SendByte(unsigned char data)
{  
   U1THR = data;				    
   while( (U1LSR&0x40)==0 );	    
}

//This function sends a string of characters on the serial port
void UART1_SendStr(const unsigned char *str)
{  
   while(1)
   {  
      if( *str == '\0' ) break;
      UART1_SendByte(*str++);	    
   }
}


/************************************************************

	Function 		: Init_Motion_Pin
	Return type		: None
	Parameters		: None
	Description 	: Initialises Motion control pins
************************************************************/

void Init_Motion_Pin(void)
{
 PINSEL0&=0xFF0F3FFF;		
 PINSEL0|=0x00000000;		//Set Port pins P0.7, P0.10, P0.11 as GPIO
 PINSEL1&=0xFFFFF0FF;
 PINSEL1|=0x00000000;		//Set Port pins P0.21 and 0.22 as GPIO
 IO0DIR&=0xFF9FF37F;
 IO0DIR|= (1<<10) | (1<<11) | (1<<21) | (1<<22) | (1<<7); 	//Set Port pins P0.10, P0.11, P0.21, P0.22, P0.7 as Output pins
 IO1DIR&=0xFFDFFFFF;
 IO1DIR|= (1<<21);		// Set P1.21 as output pin
 Stop();				// Stop both the motors on start up
 IO0SET = 0x00200080;	// Set PWM pins P0.7/PWM2 and P0.21/PWM5 to logic 1
}

//Function to move Left motor forward
void L_Forward(void)
{
 IO1SET = 0x00200000;		//Set P1.21 to logic '1'
}

//Function to move Left motor backward
void L_Back(void)
{
 IO0SET = 0x00400000;		//Set P0.22 to logic '1'
}

//Function to move Right motor forward
void R_Forward(void)
{
 IO0SET = 0x00000400;		//Set P0.10 to logic '1'
}

//Function to move Right motor backward
void R_Back(void)
{
 IO0SET = 0x00000800;		//Set P0.11 to logic '1'
}

//Function to stop left motor
void L_Stop(void)
{
 IO1CLR = 0x00200000;		//Set P1.21 to logic '0'
 IO0CLR = 0x00400000;		//Set P0.22 to logic '0'
}

//Function to stop Right motor
void R_Stop(void)
{
 IO0CLR = 0x00000400;		//Set P0.10 to logic '0'
 IO0CLR = 0x00000800;		//Set P0.11 to logic '0'
}

//Function to move robot in forward direction
void Forward(void)
{
 Stop();
 L_Forward();
 R_Forward();
}

//Function to move robot in backward direction
void Back(void)
{
 Stop();
 L_Back();
 R_Back();
}

//Function to turn robot in Left direction
void Left(void)
{ 
 Stop();
 L_Back();
 R_Forward();
}

//Function to turn robot in right direction
void Right(void)
{ 
 Stop();
 L_Forward();
 R_Back();
}

//Function to turn robot in Left direction by moving right wheel forward
void Soft_Left(void)
{
 Stop();
 R_Forward();
}

//Function to turn robot in right direction by moving left wheel forward
void Soft_Right(void)
{
 Stop();
 L_Forward();
}

//Function to turn robot in left direction by moving left wheel backward
void Soft_Left2(void)
{
 Stop();
 L_Back();
}

//Function to turn robot in right direction by moving right wheel backward 
void Soft_Right2(void)
{
 Stop();
 R_Back();
}

//Function to stop the robot at its current location
void Stop(void)
{
 L_Stop();
 R_Stop();
}

void Init_Ports(void)
{
 Init_Motion_Pin();
 Init_Buzzer_Pin();
}

void Init_Peripherals(void)
{
 Init_Ports();
 Init_UART1();
}

void Sensor_Values(void)
{ 
  const unsigned char* str = "\nLeft IR: ";
  ADC_Data[0] = AD0_Conversion(6);	  //Left IR
  LCD_Print(1,1,ADC_Data[0],3);	
  //str = "\nLeft IR: ";
  UART1_SendStr(str);

  ADC_Data[1] = AD1_Conversion(0);	  //Front Sharp
  Temp = Sharp_GP2D12_Estimation(ADC_Data[1]);
  LCD_Print(1,5,Temp,3);
  str = "\nLeft IR: ";
  UART1_SendStr(str);

  ADC_Data[2] = AD0_Conversion(7);	  //Right IR
  LCD_Print(1,9,ADC_Data[2],3);
  str = "\nLeft IR: ";
  UART1_SendStr(str);
   
  ADC_Data[3] = AD1_Conversion(4);	  //Batt
  Temp = Batt_Voltage_Conversion(ADC_Data[3]);
  LCD_Print(1,13,Temp,4);
  str = "\nLeft IR: ";
  UART1_SendStr(str);
  
  ADC_Data[4] = AD1_Conversion(3);	  //whiteline Left
  LCD_Print(2,1,ADC_Data[4],3);
  str = "\nLeft IR: ";
  UART1_SendStr(str);
  
  ADC_Data[5] = AD0_Conversion(1);	  //whiteline Center
  LCD_Print(2,7,ADC_Data[5],3);
  str = "\nLeft IR: ";
  UART1_SendStr(str);

  ADC_Data[6] = AD0_Conversion(2);	  //whiteline Right
  LCD_Print(2,13,ADC_Data[6],3);
  str = "\nLeft IR: ";
  UART1_SendStr(str);
	 
}

//Function to print numbers on LCD panels
void Print_Python_Input(unsigned int n)
{
 //const unsigned char* str = "\nLeft IR: ";
 //ADC_Data[0] = AD0_Conversion(6);	  //Left IR
 LCD_Print(1,1,n,1);	
 //str = "\nLeft IR: ";
 //UART1_SendStr(str);
}


//Function to start conveyor belt
void Start_Conveyor(void)
{
 Stop();
 R_Back();
}


//Function to stop conveyor belt
void Stop_Conveyor(void)
{
 Stop();
}


//Function to move the arm and bring it back
void Push_Arm(void)
{
 Stop();
 L_Forward();
 DelaymSec(1500);
 Stop();
 DelaymSec(500);

 L_Back();
 DelaymSec(1500);
 Stop();
 DelaymSec(500);

}

int  main(void)
{
 //unsigned int Temp=0;   
 PINSEL0 = 0x00000005;		    // Reset all pins as GPIO
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;

 DelaymSec(40);
 Init();
 LCD_4Bit_Mode();
 LCD_Init();
 LCD_Command(0x01);
 DelaymSec(20);
 Init_Peripherals();
 LCD_Print(1,1,4,1);	
 //UART1_SendByte('H');
 //UART1_SendByte('i');
 //UART1_SendByte('!');

 Start_Conveyor();
 while(1)			   
 {  
     
 }
}	 

