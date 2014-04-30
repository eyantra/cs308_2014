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
#include <math.h>
#include "LCD.h"		//This header files defines LCD related prototype functions
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
void  __irq IRQ_UART1(void);
//void DelaymSec(unsigned int Delay);
void Init_UART1(void);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
unsigned int AD0_Conversion(unsigned char channel);
unsigned int AD1_Conversion(unsigned char channel);
unsigned int Sharp_GP2D12_Estimation(unsigned int Val);
unsigned int Batt_Voltage_Conversion(unsigned int Val);
void moveAccConfig(float,float,float,float,float);
int chooseHole(float, float);
void Stopper_Stop(void);


/*************************************/


/**********Global variables***********/

volatile unsigned int Left_Shaft_Count=0;
volatile unsigned int Right_Shaft_Count=0;
extern unsigned char String1[16];	//This variable is defined in LCD.c
extern unsigned char String2[16];	//This variable is defined in LCD.c
unsigned int ADC_Data[8];
unsigned int Coordinates[4];
unsigned int valuesReceived;
const float HoleX[6] = {0, TabLen/2, TabLen, 0, TabLen/2, TabLen};
const float HoleY[6] = {0, 0, 0, TabWid, TabWid, TabWid};
float randCoord[100];
void  __irq IRQ_Eint0(void);
void  __irq IRQ_Eint2(void);

/*************************************/


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
			 
/****** Stopper **********/

void Init_Stopper(void) {
	//PINSEL1 |= (F_STOP_IO_0 | F_STOP_IO_1 | B_STOP_IO_0 | B_STOP_IO_1);
	//PINSEL1 &= ~((1<<3) | (1<<2));
	PINSEL0 &= ~(F_STOP_IO_0 | F_STOP_IO_1 | B_STOP_IO_0 | B_STOP_IO_1);
	IO0DIR |= (B_STOP | F_STOP);
	Stopper_Stop();
} 

/******************************/                          

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

/************** Interrupt motion *******/

void Angle_Rotate(unsigned int Degrees)
{
	float ReqdShaftCount = 0;
	unsigned int ReqdShaftCountInt = 0;
	
	ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	Left_Shaft_Count = 0; 
	Right_Shaft_Count = 0; 
	while (1)
	{
	if((Right_Shaft_Count >= ReqdShaftCountInt) | (Left_Shaft_Count >= ReqdShaftCountInt))
	break;
	}
	Stop(); //Stop robot
}

//Function used for moving robot forward by specified distance
void Linear_Distance_mm(unsigned int DistanceInMM)
{
	float ReqdShaftCount = 0;
	unsigned int ReqdShaftCountInt = 0;
	
	ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	
	Right_Shaft_Count = 0;
	while(1)
	{
	if(Right_Shaft_Count > ReqdShaftCountInt)
	{
	break;
	}
	} 
	Stop(); //Stop robot
}


void Forward_mm(unsigned int DistanceInMM)
{
	LCD_Home();
	LCD_String("marching");
	LCD_Print(1,10, (int)(DistanceInMM),4);
	///DelaymSec(4000);

	Forward();
	Linear_Distance_mm(DistanceInMM);
}

void Back_mm(unsigned int DistanceInMM)
{
 Back();
 Linear_Distance_mm(DistanceInMM);
}

void Left_Degrees(unsigned int Degrees) 
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 Left(); //Turn left
 Angle_Rotate(Degrees);
}



void Right_Degrees(unsigned int Degrees)
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 Right(); //Turn right
 Angle_Rotate(Degrees);
}


void Soft_Left_Degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 Soft_Left(); //Turn soft left
 Degrees=Degrees*2;
 Angle_Rotate(Degrees);
}

void Soft_Right_Degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 Soft_Right();  //Turn soft right
 Degrees=Degrees*2;
 Angle_Rotate(Degrees);
}

void Soft_Left_2_Degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 Soft_Left2(); //Turn reverse soft left
 Degrees=Degrees*2;
 Angle_Rotate(Degrees);
}

void Soft_Right_2_Degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 Soft_Right2();  //Turn reverse soft right
 Degrees=Degrees*2;
 Angle_Rotate(Degrees);
}

void Ext_INT_Setup(void)
{
 PINSEL0&=~(EINT2_E_0);
 PINSEL0|= EINT2_E_1; //Enabling P0.15 as EINT2
 PINSEL1&= ~(EINT0_E_1);
 PINSEL1|= EINT0_E_0; //Enabling P0.16 as EINT0


 EXTMODE = 0x05;	// EINT2 and EINT0 is edge sensitive
 EXTPOLAR = 0x00;					// EINT2 and EINT0 in triggered on falling edge

 VICIntSelect = 0x00000000;		// Setting EINT2 and EINt0 as IRQ(Vectored)
 VICVectCntl0 = 0x20|16;		// Assigning Highest Priority Slot to EINT2 and enabling this slot
 VICVectAddr0 = (int)IRQ_Eint2; // Storing vector address of EINT2

 VICVectCntl1 = 0x20|14;		// Assigning second Highest Priority Slot to EINT0 and enabling this slot
 VICVectAddr1 = (int)IRQ_Eint0; // Storing vector address of EINT0
 EXTINT = 0x05;	//Clearing EINT2 & EINT0 interrupt flag
 	
 VICIntEnable = (1<<16) | (1<<14);	// Enable EINT2	& EINT0 flags
}


//ISR for EINT0
void  __irq IRQ_Eint0(void)
{  
   Right_Shaft_Count++;
   EXTINT = 0x01;				// Clear EINT0 flag
   VICVectAddr = 0x00;   		//Acknowledge Interrupt
}	


//ISR for EINT2
void  __irq IRQ_Eint2(void)
{  
   Left_Shaft_Count++;
   EXTINT = 0x04;				// Clear EINT2 flag
   VICVectAddr = 0x00;   	//Acknowledge Interrupt
}	


 //Takes as input, the coordinates of the ball and outputs the optimal hole to be hit into.
int chooseHole(float x, float y){  
	int ans;
	LCD_String("tcH");
	if(y > TabWid/2){				//upper row of the holes.
	 	if(x<TabLen/4) ans=4;
	 	if(x<3*TabLen/4) ans=5;
	 	else ans=6;
	}
	else{									  //lower row
	 	if(x<TabLen/4) return ans=1;
	 	if(x<3*TabLen/4) return ans=2;
	 	else ans=3;
	}
	
	LCD_Print(2,1, (int)(10*x),3);
	LCD_Print(2,5, (int)(10*y),3);
	LCD_Print(2,10, ans,2);
	return ans;									//Nearest hole
}


//Based on the observations of manual experiments, 
//this function gives the negative offset required in the Angle to be rotated 
//so as to remove the error in the angular motion arising from the wheels' inertia.
float removeAngOffset(float d){
 	if(d<10) return d-5;
	if(d<30) return d-7;
	if(d<60) return d-10;
	if(d<100) return d-15;
	else return d-20;
}

//Takes the initial and final angles as inputs and rotates the bot in the quickest way possible.
void rotateQuick(float fromAng, float toAng){
 	float diff = toAng-fromAng;
	if(diff<0) diff+=360;								  //making it a positive quantity
	LCD_Home();
	LCD_String("turning");
	LCD_Print(1,10, (int)(diff),3);
	if(diff<180) Right_Degrees(removeAngOffset((int)diff));				  //Takes a counter/clock-wise turn; whichever is quicker
	else Left_Degrees(removeAngOffset((int)(360-diff)));
	//DelaymSec(4000);

}

//Based on the observations of manual experiments, 
//this function gives the negative offset required in the distance to be travelled 
//so as to remove the error in the linear motion arising from the wheels' inertia.
float removeLinOffset(float d){
 	if(d<20) return d-10;
	if(d<50) return d-20;		
	if(d<150) return d-30;
	if(d<300) return d-45;
	if(d<500) return d-60;
	if(d<700) return d-75;
	if(d<900) return d-90;
	else return d-100;
}


//The primary function of the program. It takes the configuration, 
//comprising of the bot's position and orientation and the ball's position as its input 
//and makes the bot move to the required position and orientation so as to hit the ball in the optimal hole.
void moveAccConfig(float BotX, float BotY, float inAng, float BallX, float BallY){
	int h = chooseHole(BallX, BallY);
	float destX,destY,depX,depY;
	float horAng, verAng;
	float finAng = atan2((HoleY[h-1]-BallY),(HoleX[h-1]-BallX));   	  //final desired orientation
	//float slope = (HoleY[h-1]-BallY)/(HoleX[h-1]-BallX);

	destY = BallY - (CueLen+PivotDist)*sin(finAng);
	destX = BallX - (CueLen+PivotDist)*cos(finAng);	  
	//destX,destY gives the desired position of the bot to hit the shot. 	
	horAng = atan2(0,destX-BotX);					  //Angle for the horizontal motion. 0 or 180.
	verAng = atan2(destY-BotY,0);					   //Angle for the horizontal motion. 90 or 270.
	depX = BotX;
	depY = BotY;

	//depX,depY is the position from which it departs. For now, it's simply the pivot's position. If the centroid's position is desired, there will be changes.
	horAng *= 180/PI;
	verAng *= 180/PI; 
	inAng *= 180/PI;
	finAng *= 180/PI;
	//converting the angles in degrees.
	rotateQuick(inAng,horAng);		   //aligning the bot with the horizontal.
	DelaymSec(1000);
	
	Forward_mm(fabs(removeLinOffset((destX-depX)*1000)));					  //Horizontal motion first.

	DelaymSec(1000);
	
	rotateQuick(horAng,verAng);				 //aligning the bot with the vertical
	DelaymSec(2000);
	
 	Forward_mm(fabs(removeLinOffset((destY-depY)*1000)));						 //vertical motion
	DelaymSec(1000);
	
	rotateQuick(verAng,finAng);																	 //orienting the bot towards the chosen hole.
}

/*****************************************/

void Init(void)
{
 valuesReceived = 0;
 Init_LCD_Pin();
 Init_ADC_Pin();
 Init_ADC0();
 Init_ADC1();
 Init_Stopper();
}


/**********************************************/

/******************Global Variables****************/

unsigned char Temp=0;
/**************************************************/


/*void DelaymSec(unsigned int Delay)	//App. 1mSec Delay
{  
   unsigned int i;
   for(; Delay>0; Delay--) 
   for(i=0; i<10000; i++);
} */


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

volatile int numChars=0;
int numCoordinates=0;
char coordinates[1000];
char number[5];
int busy=0;

//This function is UART0 Receive ISR. This functions is called whenever UART0 receives any data
void  __irq IRQ_UART1(void)
{  
 Temp = U1RBR;	
 
 if(numCoordinates<8) {
	 number[numChars%4] = Temp;
	 numChars++;
	 
	 if(numChars%4 == 0) {
	 	 randCoord[numCoordinates] = (float) atoi(number);
		 numCoordinates++;
	 }
 }

 VICVectAddr = 0x00;
 UART1_SendByte(Temp);	// Echo Back received character
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
   VICVectCntl3 = 0x20|7;			// UART1
   VICVectAddr3 = (int)IRQ_UART1; 	//UART1 Vector Address
   VICIntEnable = (1<<7);	// Enable UART1 Rx interrupt

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


void Stopper_Stop(void)
{
 IO0CLR |= B_STOP;
 IO0CLR |= F_STOP;
}

void Stopper_Release(void)
{
 //IO0SET |= 0x00010000;
 Stopper_Stop();
 IO0SET |= F_STOP;
}

void Stopper_Pull(void)
{
 //IO0SET |= 0x00080000;
 //IO0SET |= 0x00010000;
 Stopper_Stop();
 IO0SET |= B_STOP;
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
void UART1_SendInt(int b){

	int a = b;
	int i=0;
	char d[3];
	while(i<3){
		//UART1_SendByte(a%10+'0');
		d[2-i] = a%10+'0';
		a=a/10;
		i++;
	}
	UART1_SendByte(d[0]); 
	UART1_SendByte(d[1]);
	UART1_SendByte(d[2]);

}

//Uses the globally stored coordinates which are in random order, 
//and rearranges the coordinates to match the respective components.
void GetConfig(){		  				 
	int i, j,mini=-1,minj=-1,min2i=-1;						  //some iterators
	float minSqrDist = 400000, currDist;	  
	//float randCoord[8] = {150,250,300,300,1520,700,250,150};
	float BotX,BotY,inAng,BallX,BallY;							  //variables for the actual config.
 	for(i=0;i<4;i++){
	 	for(j=i+1; j<4; j++){
			currDist=(randCoord[2*i]-randCoord[2*j])*(randCoord[2*i]-randCoord[2*j])+(randCoord[2*i+1]-randCoord[2*j+1])*(randCoord[2*i+1]-randCoord[2*j+1]);
			if(minSqrDist>currDist){
				minSqrDist=currDist;
				mini=i;
				minj=j;
			}
		}
	}

	//mini,minj give the indices of the spots which are the closest to each other.
	//the midpoint of these is the midpoint of the wheels:-
	BotX = (randCoord[2*mini] + randCoord[2*minj])/2;
	BotY = (randCoord[2*mini+1] + randCoord[2*minj+1])/2;
	//DelaymSec(5000);
	minSqrDist = 1000000;
	for(i=0;i<4;i++){
		if(i==mini || i==minj) continue;
		currDist=(randCoord[2*i]-randCoord[2*mini])*(randCoord[2*i]-randCoord[2*mini])+(randCoord[2*i+1]-randCoord[2*mini+1])*(randCoord[2*i+1]-randCoord[2*mini+1]);
		if(minSqrDist>currDist){
			minSqrDist=currDist;
			min2i=i;
		}
	}  
	//min2i gives the index of the 3rd spot on the bot. The line joining this to the pivot just calculated, gives the initial orientation.

	inAng = atan2((randCoord[min2i*2+1]-BotY),(randCoord[min2i*2]-BotX));													   //initial angle
	if(inAng<0) inAng+=2*PI;																								  //making it positive
	
	for(i=0;i<4;i++){
		if(i==mini || i==minj || i==min2i) continue;
		else break;
	}
	//the remaining spot gives the index of the ball.
	BallX=randCoord[2*i];
	BallY=randCoord[2*i+1];

	randCoord[0] = BotX;
	randCoord[1] = BotY;
	randCoord[2] = inAng;
	randCoord[3] = BallX;
	randCoord[4] = BallY;
	//the config is stored back in the global array randCoord.
}

//To take the shot, once the bot's in place.
void triggerImpactMechanism() {
	Stopper_Pull();
	DelaymSec(11000);
	Stopper_Stop();
	DelaymSec(1000);
	Stopper_Release();
	DelaymSec(11000);
	Stopper_Stop();
}

int  main(void)
{  	
	unsigned int Temp=0; 
	PINSEL0 = 0x00000005;		    // Reset all pins as GPIO
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	Init_Peripherals();

	DelaymSec(40);
	Init();
	LCD_4Bit_Mode();
	LCD_Init();
	LCD_Command(0x01);
	Ext_INT_Setup();
	Init_Motion_Pin();
	//LCD,motion pins etc are all initialized.
	DelaymSec(20);
	while(numCoordinates < 8) {
		LCD_Print(1,1,numChars,3);
		LCD_Print(1,5,numCoordinates,3);
	}
	//asserted that all the coordinates are recd. numCoordinates is global and is updated through the Xigbee Interrupt.
	DelaymSec(2000);
	
	GetConfig();
	//The coordiantes recd are now properly sorted.
	DelaymSec(3000);
	LCD_Print(1,1, (int)randCoord[0],4);
	LCD_Print(1,6, (int)randCoord[1],4);
	LCD_Print(1,11, (int)(randCoord[2]*100),4);
	LCD_Print(2,1, (int)randCoord[3],4);
	LCD_Print(2,6, (int)randCoord[4],4);
	//DelaymSec(20000); 
	//Call the function to move the bot according to config.
	moveAccConfig(randCoord[0]/1000,randCoord[1]/1000,randCoord[2],randCoord[3]/1000,randCoord[4]/1000);

	DelaymSec(1000);
	triggerImpactMechanism();

	BUZZER_ON();
	DelaymSec(1000);
	BUZZER_OFF();
					
}
