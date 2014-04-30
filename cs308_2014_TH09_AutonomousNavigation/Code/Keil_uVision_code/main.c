/********************************************************************************
 
 LCD Connections:
 			  LCD	  Microcontroller Pins
 			  RS  --> P1.19
			  RW  --> P1.18
			  EN  --> P1.17
			  DB7 --> P1.25
			  DB6 --> P1.24
			  DB5 --> P1.23
			  DB4 --> P1.22

 ADC Connection:
 			  	Sensor									  ADC Channel No.

			  	Battery Voltage									AD1.4(P0.13)
			  	White line sensor 1								AD1.3(P0.12)
			  	White line sensor 2								AD0.1(P0.28)
			  	White line sensor 3								AD0.2(P0.29)
			  	IR Proximity analog sensor 2*****				AD0.6(P0.4)
			  	IR Proximity analog sensor 4*****				AD0.7(P0.5)
			  	Sharp IR range sensor 2							AD0.6(P0.4)
			  	Sharp IR range sensor 3							AD1.0(P0.6)
			  	Sharp IR range sensor 4							AD0.7(P0.5)
			  	

 ***** For using Analog IR proximity (2 and 4) sensors ensure that OE resistors are soldered and remove the respective sharp sensors. 
 	    
 
 LCD Display interpretation:
 ****************************************************************************
 *BATTERY VOLTAGE	IR PROX.SENSOR 2	FRONT SHARP 2 DIS 	IR.PROX.SENSOR 4*
 *LEFT WL SENSOR			  CENTER WL SENSOR				RIGHT WL SENSOR	*
 ****************************************************************************
*/ 
 
#include <lpc214x.h>
#include "LCD.h"		//This header files defines LCD related prototype functions
#include <math.h>
#include "defines.h"
#include "type.h"
#include "spi.h"



/***************Macros*********************/ 
#define Fosc            12000000                    //10MHz~25MHz
#define Fcclk           (Fosc * 5)                  //Fosc(1~32)<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO Fcclk 2񅦰�16�156MHz~320MHz
#define Fpclk           (Fcclk / 1) * 1             //VPB(Fcclk / 1) 1񄿔
#define UART_BPS	9600 		//Change Baud Rate Setting here

// SPI communication   //for reading values from ATMega8 slave1
#define	SPI1_SLAVE_SELECT	0x00100000
/*********Function Prototypes********/

void Init(void);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
//External interrupts for left and right wheel encoder
void  __irq IRQ_Eint0(void);
void  __irq IRQ_Eint2(void);
////////////////////////////////////////////////////////
unsigned int AD0_Conversion(unsigned char channel);
unsigned int AD1_Conversion(unsigned char channel);
unsigned int Sharp_GP2D12_Estimation(unsigned int Val);
unsigned int Batt_Voltage_Conversion(unsigned int Val);


void Init_Peripherals(void);
void Init_Ports(void);
void  __irq IRQ_UART1(void); //interrupt for bluetooth communication

void Init_UART1(void);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);


/*************************************/


/**********Global variables***********/

extern unsigned char String1[16];	//This variable is defined in LCD.c
extern unsigned char String2[16];	//This variable is defined in LCD.c
unsigned int ADC_Data[8];
volatile unsigned int Left_Shaft_Count=0;
volatile unsigned int Right_Shaft_Count=0;
/*************************************/
//Motion Functions


/*****Function Prototypes*********************/

//void DelaymSec(unsigned int j);
void Init_Buzzer_Pin(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Delay(void);

void Forward(void);
void Back(void);
void Left(void);
void Right(void);
void Stop(void);
void Soft_Left(void);
void Soft_Right(void);
void Soft_Left2(void);
void Soft_Right2(void);
void Init_Motion_Pin(void);

/**********************************************/


//variables to store information from QR codes
unsigned char Temp=0;
unsigned char start  =0; 
unsigned char direction = 'Z';
unsigned char distance  ='0'; 
//This function is UART0 Receive ISR. This functions is called whenever UART0 receives any data
//Whenever a character is received via bluetooth this routine is called.
// When we receive particular characters we update global variables to store that information in the main control loop
void  __irq IRQ_UART1(void)
{  
 Temp = U1RBR;
 		

 if(Temp==0x46 || Temp==0x4C || Temp==0x52 	|| Temp==0x55)//F or L or R or U  direction to be taken at the next intersection 
 {
 	direction=Temp;
	LCD_Print(2,1,direction,1);
 }
 if(Temp == 0x53)
 {
 	start = 1;
 }
 if(Temp>=0x30 && Temp<=0x39)
 {
 	distance=Temp;
	LCD_Print(2,11,distance,1);
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

//initializing interrupts for bluettoth communication, left wheel encoder and right wheel encoder.
void Init_UART1(void)
{  
   unsigned int Baud16;
   PINSEL0&=0xFFF0FFFF;
   PINSEL0|=0x00050000;
   PINSEL0|= EINT2_E_1; //Enabling P0.15 as EINT2
 PINSEL1&= ~(EINT0_E_1);
 PINSEL1|= EINT0_E_0; //Enabling P0.16 as EINT0


   U1LCR = 0x83;		            // DLAB = 1
   Baud16 = (Fpclk / 16) / UART_BPS;  
   U1DLM = Baud16 / 256;							
   U1DLL = Baud16 % 256;						
   U1LCR = 0x03;
   U1IER = 0x00000001;		//Enable Rx interrupts

   EXTMODE = 0x05;	// EINT2 and EINT0 is edge sensitive
 EXTPOLAR = 0x00;					// EINT2 and EINT0 in triggered on falling edge

   VICIntSelect = 0x00000000;		// IRQ
   VICVectCntl0 = 0x20|7;			// UART1
   VICVectAddr0 = (int)IRQ_UART1; 	//UART1 Vector Address

   VICVectCntl1 = 0x20|16;		// Assigning Highest Priority Slot to EINT2 and enabling this slot
 VICVectAddr1 = (int)IRQ_Eint2; // Storing vector address of EINT2

 VICVectCntl2 = 0x20|14;		// Assigning second Highest Priority Slot to EINT0 and enabling this slot
 VICVectAddr2 = (int)IRQ_Eint0; // Storing vector address of EINT0
 EXTINT = 0x05;	//Clearing EINT2 & EINT0 interrupt flag
 	
 VICIntEnable = (1<<16) | (1<<14) |(1<<7) ;	// Enable EINT2	& EINT0 flags

   //VICIntEnable = (1<<7);	// Enable UART1 Rx interrupt

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


void Init_Motion_Pin(void)
{
 PINSEL0&=~(R_F_IO_0 | R_F_IO_1 | R_B_IO_0 | R_B_IO_1 | L_E_IO_0 | L_E_IO_1);//Set Port pins P0.7, P0.10, P0.11 as GPIO
 PINSEL1&=~(R_E_IO_0 | R_E_IO_1 | L_B_IO_0 | L_B_IO_1);//Set Port pins P0.21 and 0.22 as GPIO
 PINSEL2&=~(L_F_IO_0 | L_F_IO_1);//set pins 16-31 of port 1 as GPIO
 IO0DIR|= L_B | R_F | R_B | L_E | R_E; 	//Set Port pins P0.10, P0.11, P0.21, P0.22, P0.7 as Output pins
 IO1DIR|= L_F;		// Set P1.21 as output pin
 Stop();				// Stop both the motors on start up
 IO0SET = L_E | R_E;	// Set PWM pins P0.7/PWM2 and P0.21/PWM5 to logic 1
}



void Init_Buzzer_Pin(void)
{
 PINSEL1&=~(BUZZER_IO_0); 		
 PINSEL1&=~(BUZZER_IO_1); 		
 IO0DIR|= BUZZER;  			//Set P0.25 as Output
 Buzzer_Off();				//Initially turn OFF buzzer
}
/***********************************/																					
void Buzzer_On(void)
{
IO0SET=BUZZER;	
}
/***********************************/
void Buzzer_Off(void)
{
IO0CLR=BUZZER;	
}
/***********************************/
void Buzzer_Delay(void)
{  
 unsigned int  i,j;
 for(j=0;j<20;j++)
 {
  for(i=0; i<60000; i++);
 } 
}

//Function to move robot in forward direction

void Forward(void)
{
 Stop();
 IO1SET = L_F;					//Set P1.21 to logic '1'
 IO0SET = R_F;					//Set P0.10 to logic '1'
 IO0CLR = L_B | R_B;  	//clr P0.22 and P0.11 to logic '0'
}

//Function to move robot in backward direction
void Back(void)
{
 Stop();
 IO0SET = L_B;		//Set P0.22 to logic '1'
 IO0SET = R_B;		//Set P0.11 to logic '1'
 IO1CLR = L_F;		//clr P1.21 to logic '0'
 IO0CLR = R_F;  	//clr P0.10 to logic '0'

}

//Function to turn robot in Left direction
void Left(void)
{ 
 LCD_Print(2,7,122,3);
 Stop();
 IO0SET = L_B;		//Set P0.22 to logic '1'
 IO0SET = R_F;		//Set P0.10 to logic '1'
 IO1CLR = L_F;		//clr P0.21 to logic '0'
 IO0CLR = R_B;		//P0.11 to logic '0'
}

//Function to turn robot in right direction
void Right(void)
{ 
 Stop();
 IO1SET = L_F;						//Set P1.21 to logic '1'
 IO0SET = R_B;						//Set P0.11 to logic '1'
 IO0CLR = L_B | R_F;   	//clr P0.22 and P0.10 to logic '0'
}

//Function to turn robot in Left direction by moving right wheel forward
void Soft_Left(void)
{
 Stop();
 IO0SET = R_F;				//Set P0.10 to logic '1'
 IO1CLR = L_F;				//clr P0.21 to logic '0'
 IO0CLR = L_B | R_B;	//clr P0.22 and P0.11 to logic '0'
}

//Function to turn robot in right direction by moving left wheel forward
void Soft_Right(void)
{
 Stop();
 IO1SET = L_F;							//Set P1.21 to logic '1'
 IO0CLR = L_B | R_F | R_B;	//clr P0.10, P0.11 and P0.22 to logic '0'
}

//Function to turn robot in left direction by moving left wheel backward
void Soft_Left2(void)
{
 Stop();
 IO0SET = L_B;					//Set P0.22 to logic '1'
 IO1CLR = L_F;					//clr P1.21 to logic '0'
 IO0CLR = R_F | R_B; 	//clr P0.10 and P0.11 to logic '0'
}

//Function to turn robot in right direction by moving right wheel backward 
void Soft_Right2(void)
{
 Stop();
 IO0SET = R_B;					//Set P0.11 to logic '1'
 IO1CLR = L_F;	  		  //clr P0.21 to logic '0'
 IO0CLR = L_B | R_F;  	//clr P0.22 and P0.10 to logic '0'
}

//Function to stop the robot at its current location
void Stop(void)
{
 IO1CLR = L_F;		//Set P1.21 to logic '0'
 IO0CLR = L_B;		//Set P0.22 to logic '0'
 IO0CLR = R_F;		//Set P0.10 to logic '0'
 IO0CLR = R_B;		//Set P0.11 to logic '0'
}
/*******************************************************/



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
					// ADC7CR=0x00200E00;
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

//Function used for turning robot by specified degrees
void Angle_Rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned int ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 Left_Shaft_Count = 0; 
 Right_Shaft_Count = 0;
 //LCD_Print(2,13,126,3); 
 while (1)
 {
  if((Right_Shaft_Count >= ReqdShaftCountInt) | (Left_Shaft_Count >= ReqdShaftCountInt))
  break;
  //LCD_Print(2,1,Left_Shaft_Count,3); 
  //LCD_Print(2,13,Right_Shaft_Count,3);
  
 }
 //LCD_Print(2,13,127,3);
 Stop(); //Stop robot
}

void Left_Degrees(unsigned int Degrees) 
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
//LCD_Print(2,7,123,3);
 Left(); //Turn left

 //LCD_Print(2,13,125,3);
 //DelaymSec(1000);
 Angle_Rotate(Degrees);
 //LCD_Print(2,13,128,3);
 //DelaymSec(10000);
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

void Turn_Left_90(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 Left(); //Turn reverse soft left
 DelaymSec((400/90)*Degrees);
 Stop();
}

void Turn_Right_90(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 Right(); //Turn reverse soft left
 DelaymSec((600/90)*Degrees);
 Stop();
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
 Forward();
 Linear_Distance_mm(DistanceInMM);
}

void Back_mm(unsigned int DistanceInMM)
{
 Back();
 Linear_Distance_mm(DistanceInMM);
}



int main(void)
{
 
  
 unsigned int Temp=0,mindist,fmindist,maxdist,ldist,rdist,fdist,setVar,tempmindist;//local variables for storing arena data
 unsigned int flag=0; 
 PINSEL0 = 0x00000005;		    // Reset all pins as GPIO
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;
//Initlialize peripherals Motion and Buzzer modules
 Init_Peripherals();
 DelaymSec(40);

 Init();

 LCD_4Bit_Mode();
 LCD_Init();
 LCD_Command(0x01);
 DelaymSec(20);


 tempmindist=48;
 mindist=60; //minimum distance a sharp sensor can sense in left or right direction. This is used to stop the bot from colliding with walls
 fmindist=80;  //minimum distance in front direction. It is used to determine if there is a wall in front direction
 maxdist=350; //maximum distance in left/right direction. If sharp sensor reading s greater than this we infer that there is no obstacle on that side 
 

 //This loop is to prevent the program from entering the main control section until it receives start signal via bluetooth.
 while(1)
 {
 	if(start == 1)
	break;
 }
 Forward();

 //main contro loop starts
 while(1)	
 {
 
  //reading and displaying sharp sensor data

  ADC_Data[1] = AD1_Conversion(0);	  //Front Sharp
  fdist = Sharp_GP2D12_Estimation(ADC_Data[1]);
  LCD_Print(1,7,fdist,3);

  ADC_Data[2] = AD0_Conversion(6);	  //Left Sharp
  ldist = Sharp_GP2D12_Estimation(ADC_Data[2]);
  LCD_Print(1,1,ldist,3);

  ADC_Data[3] = AD0_Conversion(7);	  //RIGHT Sharp
  rdist = Sharp_GP2D12_Estimation(ADC_Data[3]);
  LCD_Print(1,13,rdist,3);
   

	//Comparing sharp sensor vlaues to analyse if bot is in the middle of road or on an intersection.
	
  if(fdist>fmindist && (ldist>mindist && ldist<maxdist) && (rdist>mindist && rdist<maxdist)) //middle of road
  {
  	Buzzer_Off();
	Forward();
  } 	  
  //For smooth movement along walls
  else if(fdist>fmindist && ldist<mindist && (rdist>mindist && rdist<maxdist))	 //close to left wall
  {
  	Buzzer_Off();
	Right_Degrees(8);
	Forward();
	DelaymSec(100);
	Stop();
  }
  //For smooth movement along walls
  else if(fdist>fmindist && (ldist>mindist && ldist<maxdist) && rdist<mindist)	//close to right wall
  {
  	Buzzer_Off();
	Left_Degrees(8);
	Forward();
	DelaymSec(100);
	Stop();
  }
  //this is a junction
  else if (ldist>maxdist && rdist>maxdist)
  {
  	   
	Buzzer_Off();
  	Stop();
	Forward_mm(100);  
	
	//moving in the appropriate direction which was received via bluetooth
	if(direction == 'F'){
	   Forward_mm(300);
	   //DelaymSec(500);

	   //move forward if distance variable is also set. This happens only at the last intersection
	   if(distance == 0x44)
	   {
	   LCD_Print(2,13,999,3);
	   	  //unsigned int d = (int)distance;
		  Forward_mm(5*100);
		  Stop();
		  flag=1;
		  Buzzer_On();
	   }
	   
	}
	else if(direction == 'L'){
		Forward_mm(150);
		Left_Degrees(100);
		Forward_mm(150);
	   //move forward if distance variable is also set. This happens only at the last intersection
		if(distance != '0')
	   {
	   	  LCD_Print(2,1,999,3);
	   	  //unsigned int d = (int)distance;
		  Forward_mm(5*100);
		  Stop();
		  flag=1;
		  Buzzer_On();
	   }
		//DelaymSec(500);
	}
	else if(direction == 'R'){
		Forward_mm(150);	
		Right_Degrees(85);
		Forward_mm(150);
		//move forward if distance variable is also set. This happens only at the last intersection
		if(distance == 0x44)
	   {
	   	  Forward_mm(5*100);
		  Stop();
		  flag=1;
		  Buzzer_On();
	   }
		//DelaymSec(500);
	}
	else if(direction == 'U'){
		Forward_mm(150);	
		Right_Degrees(180);
		Forward_mm(150);
		//move forward if distance variable is also set. This happens only at the last intersection
		if(distance == 0x44)
	   {
	   	  //unsigned int d = (int)distance;
		  Forward_mm(5*100);
		  Stop();
		  flag=1;
		  Buzzer_On();
	   }
		//DelaymSec(500);
	}
	
	//Reseting the direction and distance variables in anticipation of next intersection.
	direction = 'Z';
	distance = '0';
  
  }
  //in case obstacle arrives
  else if(fdist<fmindist && (ldist>tempmindist && ldist<maxdist) && (rdist>tempmindist && rdist<maxdist))
  {
  	Stop();	
	Buzzer_On();
			
  }
  //only possible right direction
  else if(fdist<fmindist && (ldist>mindist && ldist<maxdist) && rdist>maxdist)
  {
  	Buzzer_Off();
	Right_Degrees(85);
	Forward_mm(100);
	Stop();
	
				
	 }
  //only possible left direction
  else if( fdist<fmindist && ldist>maxdist && rdist<maxdist)
  {
  
  Buzzer_Off();
  Left_Degrees(85);
  Forward_mm(100);
	Stop();
			   
  }
  else
  {Buzzer_Off();Forward();}

  
  //if destination is reached break from the main loop and stop
  if(flag == 1)
  	break;

   

  DelaymSec(10);
 }
 
}
