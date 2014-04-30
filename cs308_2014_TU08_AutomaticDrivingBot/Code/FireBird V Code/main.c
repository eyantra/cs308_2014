#include <lpc214x.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "lcd.h"
#include "queue.h"
#include "stack.h"
#define HIGH 499
#define LOW 400
#define HIGH2 400
#define LOW2 350
#define STRAIGHT 240
#define RIGHT 95
#define DELAY 0
/*************Function Prototypes**********/
void Init_Buzzer_Pin(void);
void Init_Peripherals(void);
void Init_Ports(void);
void  __irq IRQ_UART1(void);
void DelaymSec(unsigned int Delay);
void Init_UART1(void);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);
int move_to(int, int);
/********* Sensor Function Prototypes********/

void Init(void);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
unsigned int AD0_Conversion(unsigned char channel);
unsigned int AD1_Conversion(unsigned char channel);
unsigned int Sharp_GP2D12_Estimation(unsigned int Val);
unsigned int Batt_Voltage_Conversion(unsigned int Val);
void UpdateLeftPWM(unsigned int vel);
void UpdateRightPWM(unsigned int vel);
int to_x=0, to_y=0, got=0;
/*************Global Variables*************/
unsigned char Temp=0;
volatile unsigned int Left_Shaft_Count=0;
volatile unsigned int Right_Shaft_Count=0;

int shortest(int x, int y);
int next_point(void);
int previous_point(void);
/******************************************/
void  __irq IRQ_Eint0(void);
void  __irq IRQ_Eint2(void);

/**************************************/
void DelaymSec(unsigned int j)
{
 unsigned int  i;
 for(;j>0;j--)
 {
  for(i=0; i<10000; i++);
 }
}

/***************Line Following Functions*************/


/****************XBEE and Buzzer functions*******/
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

 VICVectAddr = 0x00;
 UART1_SendByte(Temp);	// Echo Back received character
 if(got==0){    // Got x coord
   to_x = Temp - 48;
   got=1;
 }
 else if(got==1){  // Got y coord
   to_y = Temp - 48;
   got=2;
 }
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
   VICVectCntl2 = 0x20|7;			// UART1
   VICVectAddr2 = (int)IRQ_UART1; 	//UART1 Vector Address
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
void send_value (unsigned int n) {
 char c;
 if(n==0) {
   UART1_SendByte('0');
   return;
 }
 c = (n%10) + '0';
 if(n>=10)
   send_value(n/10);
 UART1_SendByte(c);
}


/*****************Motion Functions**************/
 //Function	to stop robot at current location
 void Stop(void)
{
 IO1CLR = L_F;		//Set P1.21 to logic '0'
 IO0CLR = L_B;	//Set P0.22 to logic '0'
 IO0CLR = R_F;		//Set P0.10 to logic '0'
 IO0CLR = R_B;		//Set P0.11 to logic '0'
 }
void Init_Motion_Pin(void)
{
 PINSEL0&=~(R_F_IO_0 | R_F_IO_1 | R_B_IO_0 | R_B_IO_1 | L_E_IO_0 | L_E_IO_1);//Set Port pins P0.7, P0.10, P0.11 as GPIO
 PINSEL1&=~(R_E_IO_0 | R_E_IO_1 | L_B_IO_0 | L_B_IO_1);//Set Port pins P0.21 and 0.22 as GPIO
 PINSEL2&=~(L_F_IO_0 | L_F_IO_1);//set pins 16-31 of port 1 as GPIO
 IO0DIR|= L_B | R_F | R_B | L_E | R_E; 	//Set Port pins P0.10, P0.11, P0.21, P0.22, P0.7 as Output pins
 IO1DIR|= L_F;		// Set P1.21 as output pin
 Stop();				// Stop both the motors on start up
 IO0SET = L_E | R_E;	// Set PWM pins P0.7/PWM2 and P0.21/PWM5 to logic 1	   This pins are set as PWM pins in Init_PWM fuction
}



//Function to move robot in forward direction
void Forward(void)
{
 Stop();
 IO1SET = L_F;		//Set P1.21 to logic '1'
 IO0SET = R_F;		//Set P0.10 to logic '1'
 IO0CLR = L_B | R_B;  	//clr P0.22 and P0.11 to logic '0'
}

//Function to move robot in backward direction
void Back(void)
{
 Stop();
 IO0SET = L_B;		//Set P0.22 to logic '1'
 IO0SET = R_B;		//Set P0.11 to logic '1'
 IO1CLR = L_F;		//clr P1.21 to logic '0'
 IO0CLR = R_F;  		//clr P0.10 to logic '0'

}

//Function to turn robot in Left direction
void Left(void)
{
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
 IO1SET = L_F;		//Set P1.21 to logic '1'
 IO0SET = R_B;		//Set P0.11 to logic '1'
 IO0CLR = L_B | R_F;  	//clr P0.22 and P0.10 to logic '0'
}

//Function to turn robot in Left direction by moving right wheel forward
void Soft_Left(void)
{
 Stop();
 IO0SET = R_F;		//Set P0.10 to logic '1'
 IO1CLR = L_F;		//clr P0.21 to logic '0'
 IO0CLR = L_B | R_B;	//clr P0.22 and P0.11 to logic '0'
}

//Function to turn robot in right direction by moving left wheel forward
void Soft_Right(void)
{
 Stop();
 IO1SET = L_F;		//Set P1.21 to logic '1'
 IO0CLR = L_B | R_F | R_B;	//clr P0.10, P0.11 and P0.22 to logic '0'
}

//Function to turn robot in left direction by moving left wheel backward
void Soft_Left2(void)
{
 Stop();
 IO0SET = L_B;		//Set P0.22 to logic '1'
 IO1CLR = L_F;		//clr P1.21 to logic '0'
 IO0CLR = R_F | R_B; 	//clr P0.10 and P0.11 to logic '0'
}

//Function to turn robot in right direction by moving right wheel backward
void Soft_Right2(void)
{
 Stop();
 IO0SET = R_B;		//Set P0.11 to logic '1'
 IO1CLR = L_F;	    //clr P0.21 to logic '0'
 IO0CLR = L_B | R_F;  	//clr P0.22 and P0.10 to logic '0'
}




//Function used for turning robot by specified degrees
void Angle_Rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned int ReqdShaftCountInt = 0;
 unsigned int a;	  //whiteline Left
 unsigned int b;
 unsigned int c;
 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 Left_Shaft_Count = 0;
 Right_Shaft_Count = 0;
 while (1)
 {
	a = AD1_Conversion(3);	  //whiteline Left
	b = AD0_Conversion(1);	  //whiteline Center
	c = AD0_Conversion(2);	  //whiteline Right

 if(((Right_Shaft_Count >= (ReqdShaftCountInt*3)/4) || (Left_Shaft_Count >= (ReqdShaftCountInt*3)/4)) && (a>100 || b>100 || c>100))
  break;
 }
 Stop(); //Stop robot
}

//Function used for moving robot forward by specified distance
int Linear_Distance_mm(unsigned int DistanceInMM)
{
 float ReqdShaftCount = 0;
 unsigned int ReqdShaftCountInt = 0;
 unsigned int a;	  //whiteline Left
 unsigned int b;
 unsigned int c;
 unsigned int d;
 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;

 Right_Shaft_Count = 0;
 Left_Shaft_Count = 0;
 while(1)
 {
 	a = AD1_Conversion(3);	  //whiteline Left
	b = AD0_Conversion(1);	  //whiteline Center
	c = AD0_Conversion(2);	  //whiteline Right
	/*d = Sharp_GP2D12_Estimation(AD1_Conversion(0));
	if(d<50){
		Stop();
		return 0;
	}*/
 	if(a>100 && b<100 && c<100)
	{
		UpdateLeftPWM(LOW);
 		UpdateRightPWM(HIGH);
	}
	else if(a<100 && b>100 && c<100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(HIGH);
	}
	else if(a<100 && b<100 && c>100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(LOW);
	} /*
   else if(a<100 && b<100 && c<100){
      UpdateLeftPWM(LOW);
 		UpdateRightPWM(LOW);
   }   */

	if((Right_Shaft_Count) > ReqdShaftCountInt)
	{
		break;
	}
 }
 Stop(); //Stop robot
 return 1;
}

int Linear_Distance_mm_white(unsigned int DistanceInMM)
{
 float ReqdShaftCount = 0;
 unsigned int ReqdShaftCountInt = 0;
 unsigned int a;	  //whiteline Left
 unsigned int b;
 unsigned int c;
 unsigned int d;
 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;

 Right_Shaft_Count = 0;
 Left_Shaft_Count = 0;
 while(1)
 {
 	a = AD1_Conversion(3);	  //whiteline Left
	b = AD0_Conversion(1);	  //whiteline Center
	c = AD0_Conversion(2);	  //whiteline Right
	d = Sharp_GP2D12_Estimation(AD1_Conversion(0));
	if(d<50){
		Stop();
		return 0;
	}

 	if(a>100 && b<100 && c<100)
	{
		UpdateLeftPWM(LOW);
 		UpdateRightPWM(HIGH);
	}
	else if(a<100 && b>100 && c<100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(HIGH);
	}
	else if(a<100 && b<100 && c>100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(LOW);
	}
    else if(a<100 && b<100 && c<100){
		UpdateLeftPWM(LOW2);
		UpdateRightPWM(LOW2);
    }

	if((Right_Shaft_Count) > ReqdShaftCountInt)
	{
		break;
	}
 }
 Stop(); //Stop robot
 UpdateLeftPWM(HIGH);
 UpdateRightPWM(HIGH);
 //DelaymSec(1000);
 return 1;
}

//Function used for moving robot forward by specified distance
int Linear_Distance_mm_int()
{
 unsigned int a;
 unsigned int b;
 unsigned int c;
 unsigned int d;

 while(1)
 {
 	a = AD1_Conversion(3);	  //whiteline Left
	b = AD0_Conversion(1);	  //whiteline Center
	c = AD0_Conversion(2);	  //whiteline Right
	d = Sharp_GP2D12_Estimation(AD1_Conversion(0));
	if(d<50){
		Stop();
		return 0;
	}

   if(a>100 && b<100 && c<100)
	{
		UpdateLeftPWM(LOW);
 		UpdateRightPWM(HIGH);
	}
	else if(a<100 && b>100 && c<100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(HIGH);
	}
	else if(a<100 && b<100 && c>100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(LOW);
	} /*
   else if(a<100 && b<100 && c<100){
      UpdateLeftPWM(LOW);
 		UpdateRightPWM(LOW);
   } */
   else if(a>100 && b>100)
      break;
   else if(b>100 && c>100)
      break;
 }
 Stop(); //Stop robot
 //DelaymSec(1000);
 return 1;
}
//Function used for moving robot forward by specified distance
int Linear_Distance_mm_int2()
{
 unsigned int a;
 unsigned int b;
 unsigned int c;
 //unsigned int d;

 while(1)
 {
 	a = AD1_Conversion(3);	  //whiteline Left
	b = AD0_Conversion(1);	  //whiteline Center
	c = AD0_Conversion(2);	  //whiteline Right
	//d = Sharp_GP2D12_Estimation(AD1_Conversion(0));

   if(a>100 && b<100 && c<100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(LOW);
	}
	else if(a<100 && b>100 && c<100)
	{
		UpdateLeftPWM(HIGH);
 		UpdateRightPWM(HIGH);
	}
	else if(a<100 && b<100 && c>100)
	{
		UpdateLeftPWM(LOW);
 		UpdateRightPWM(HIGH);
	} /*
   else if(a<100 && b<100 && c<100){
      UpdateLeftPWM(LOW);
 		UpdateRightPWM(LOW);
   } */
   else if(a>100 && b>100)
      break;
   else if(b>100 && c>100)
      break;
 }
 Stop(); //Stop robot
 //DelaymSec(1000);
 return 1;
}
int Forward_mm(unsigned int DistanceInMM)
{
 Forward();
 if(!Linear_Distance_mm(DistanceInMM)) return 0;
 return 1;
}

int Forward_mm_white(unsigned int DistanceInMM)
{
 Forward();
 if(!Linear_Distance_mm_white(DistanceInMM)) return 0;
 return 1;
}

int Back_mm(unsigned int DistanceInMM)
{
 Back();
 if(!Linear_Distance_mm(DistanceInMM))return 0;
 return 1;
}

int Forward_mm_int()
{
 Forward();
 if(!Linear_Distance_mm_int()) return 0;
 return 1;
}

int Back_mm_int()
{
 Back();
 if(!Linear_Distance_mm_int2())return 0;
 return 1;
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

 VICIntEnable |= (1<<16) | (1<<14);	// Enable EINT2	& EINT0 flags
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

/*************************************/


/**********Global variables***********/

//extern unsigned char String1[16];	//This variable is defined in LCD.c
//extern unsigned char String2[16];	//This variable is defined in LCD.c
unsigned int ADC_Data[8];

/*************************************/


/************************************************************

	Function 		: Init_ADC_Pin
	Return type		: None
	Parameters		: None
	Description 	: Initialises the required GPIOs as ADC pins
************************************************************/
void Init_PWM(void)
{
 PINSEL0&=~(L_E_IO_0);
 PINSEL0|=L_E_IO_1;	//Enabling P0.7 as PWM2
 PINSEL1|=R_E_IO_0;
 PINSEL1&=~(R_E_IO_1);	//Enabling P0.22 as PWM5

 PWMPR	= 30;	//PWM Prescaler PCLK/30 = 500KHz
 PWMPC	= 0;	//PWMPC increments on every PCLK
 PWMTC	= 0;	//PWMTC increments on every PWMPC=PWMPR
 PWMMR0 = 500;	//PWM base frequency 500KHz/500=1KHz
 PWMMR2 = 0;
 PWMMR5 = 0;
 PWMMCR = 0x00000002;
 PWMPCR	= 0x2400;
 PWMLER	= 0x7F;
 PWMTCR = 0x01;// counter enable
}


void UpdateLeftPWM(unsigned int vel)
{
 PWMMR2 = vel;
 PWMLER = 0x04;
}

void UpdateRightPWM(unsigned int vel)
{
 PWMMR5 = vel;
 PWMLER = 0x20;
}
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
 //Init_LCD_Pin();
 Init_ADC_Pin();
 Init_ADC0();
 Init_ADC1();
}


/*************Map movement functions******/
int current_x, current_y, current_dir;  // Current indices in the map, current_dir = 0(if left), 1(if up), 2(if right) and 3(if down)
int cell_width, cell_height; // Width and Height of each cell in the grid in mm

int move_right_mm(int dist)
{
   if(dist>0){   // go right
      if(current_dir==0)
         Right_Degrees(STRAIGHT);
      else if(current_dir==1) {
         Right_Degrees(RIGHT);
      }
      else if(current_dir==3)
         Left_Degrees(RIGHT);
      current_dir=2;
      //Forward_mm(dist);
   }
   else if(dist<0) { // go left
      if(current_dir==1)
         Left_Degrees(RIGHT);
      else if(current_dir==2)
         Left_Degrees(STRAIGHT);    // turn 180 degree (> 2*90)
      else if(current_dir==3)
         Right_Degrees(RIGHT);
      current_dir=0;
      //Forward_mm(-1*dist);
   }
   else return 1;
   if(!next_point()) return 0;
   current_y=current_y+(dist/cell_height);
   return 1;
}

int move_down_mm(int dist)
{
   if(dist<0){   // go up
      if(current_dir==0)
         Right_Degrees(RIGHT);
      else if(current_dir==2)
         Left_Degrees(RIGHT);
      else if(current_dir==3)
         Left_Degrees(STRAIGHT);
      current_dir=1;
      //Forward_mm(-1*dist);
   }
   else if(dist>0){ // go down
      if(current_dir==0)
         Left_Degrees(RIGHT);
      else if(current_dir==1)
         Left_Degrees(STRAIGHT);
      else if(current_dir==2)
         Right_Degrees(RIGHT);
      current_dir=3;
      //Forward_mm(dist);
   }
   else return 1;
   if(!next_point()) return 0;
   current_x=current_x+(dist/cell_width);
   return 1;
}


int move_to (int x, int y)
{  char buffer[20];
   int dist_x = (x-current_x)*cell_height;
   int dist_y = (y-current_y)*cell_width;
   if(!move_right_mm(dist_y))return 0;
   if(!move_down_mm(dist_x))return 0;

   // Update Display
   LCD_Cursor(1,1);
   snprintf (buffer, sizeof(buffer), "%d",x);
   strcat(buffer,": X   ");
   LCD_String(buffer);
   LCD_Cursor(2,1);
   snprintf (buffer, sizeof(buffer), "%d",y);
   strcat(buffer,": Y   ");
   LCD_String(buffer);

   return 1;
}

int previous_point(){
   if(!Back_mm_int()) return 0;//Move backwards until an intersection detected
   if(!Forward_mm_white(cell_width/2)) return 0;
   return 1;
}

int next_point(){
   if(!Forward_mm_int()) return 0;//Move forward until an intersection detected
   //DelaymSec(1000);
   if(!Forward_mm_white(cell_width/2)) return 0;
   //DelaymSec(2000);
   return 1;
}


void handle_collision(int to_x, int to_y){
	unsigned int x2,y2;
	DelaymSec((rand()%5000) + 1);
	previous_point();
	x2=current_x;
	y2=current_y;
	if(current_dir==0){
		edges[x2][y2].left=0;
		if(!shortest(to_x,to_y))handle_collision(to_x,to_y);
		edges[x2][y2].left=1;
	}
	else if(current_dir==1){
		edges[x2][y2].top=0;
		if(!shortest(to_x,to_y))handle_collision(to_x,to_y);
		edges[x2][y2].top=1;
	}
	else if(current_dir==2){
		edges[x2][y2].right=0;
		if(!shortest(to_x,to_y))handle_collision(to_x,to_y);
		edges[x2][y2].right=1;
	}
	else {
		edges[x2][y2].bottom=0;
		if(!shortest(to_x,to_y))handle_collision(to_x,to_y);
		edges[x2][y2].bottom=1;
	}
   /*int dir=(current_dir+1)%4;
   while(dir!=current_dir){
      if(dir==0 && edges[current_x][current_y].left==1){
         move_to(current_x,current_y-1);
         shortest(to_x,to_y);
      }
      else if(dir==1 && edges[current_x][current_y].top==1){
         move_to(current_x-1,current_y);
         shortest(to_x,to_y);
      }
      else if(dir==2 && edges[current_x][current_y].right==1){
         move_to(current_x,current_y+1);
         shortest(to_x,to_y);
      }
      else if(dir==3 && edges[current_x][current_y].bottom==1){
         move_to(current_x+1,current_y);
         shortest(to_x,to_y);
      }
      dir=(dir+1)%4;
   }*/
   /*if(dir==current_dir){
      DelaymSec((rand()%5000) + 1);
   }*/
}

int shortest(int x, int y){
   // Find shortest path from current point using BFS
   struct node temp, temp1,temp2,temp3,temp4;
   int i,j,path_found=0;
   for(i=0;i<MAX;i=i+1){
      for(j=0;j<MAX;j=j+1){
         visited[i][j]=0;
         parents[i][j][0]=parents[i][j][1]=-1;
      }
   }
   temp.x=current_x;
   temp.y=current_y;
   queue_init();
   insert(temp);
   visited[current_x][current_y] = 1;
	while(!empty()){
		temp = queue_arr[front];
		del();
      if(temp.x==x && temp.y==y){
         int xs[MAX];
         int ys[MAX];
         int cur=0, xold, yold;
         // Path found
      	 path_found=1;
         while(x!=current_x || y!=current_y){
            xs[cur]=x;
            ys[cur]=y;
            cur++;
            xold=x;
            yold=y;
            x=parents[xold][yold][0];
            y=parents[xold][yold][1];
         }
         for(xold=cur-1;xold>=0;xold=xold-1){
            //UART1_SendByte(10);UART1_SendByte('B');send_value(xs[xold]);send_value(ys[xold]);UART1_SendByte('B');UART1_SendByte(10);
            if(!move_to(xs[xold],ys[xold]))return 0;
         }
         break;
      }
      if(temp.y>=1 && edges[temp.x][temp.y].left==1 && visited[temp.x][temp.y-1]==0){
			  temp1.x = temp.x;
           temp1.y = temp.y-1;
           parents[temp1.x][temp1.y][0]=temp.x;
           parents[temp1.x][temp1.y][1]=temp.y;
           insert(temp1);
           //UART1_SendByte('T');send_value(temp.x);send_value(temp.y);send_value(temp1.x);send_value(temp1.y);UART1_SendByte('P');
           visited[temp1.x][temp1.y]=1;
      }
		if(temp.x>=1 && edges[temp.x][temp.y].top==1 && visited[temp.x-1][temp.y]==0){
			  temp2.y = temp.y;
           temp2.x = temp.x-1;
           parents[temp2.x][temp2.y][0]=temp.x;
           parents[temp2.x][temp2.y][1]=temp.y;
           insert(temp2);
           //UART1_SendByte('T');send_value(temp.x);send_value(temp.y);send_value(temp2.x);send_value(temp2.y);UART1_SendByte('P');
           visited[temp2.x][temp2.y]=1;
      }
      if(temp.y<(MAX-1) && edges[temp.x][temp.y].right==1 && visited[temp.x][temp.y+1]==0){
			  temp3.x = temp.x;
           temp3.y = temp.y+1;
           parents[temp3.x][temp3.y][0]=temp.x;
           parents[temp3.x][temp3.y][1]=temp.y;
           insert(temp3);
           //UART1_SendByte('T');send_value(temp.x);send_value(temp.y);send_value(temp3.x);send_value(temp3.y);UART1_SendByte('P');
           visited[temp3.x][temp3.y]=1;
      }
      if(temp.x<(MAX-1) && edges[temp.x][temp.y].bottom==1 && visited[temp.x+1][temp.y]==0){
			  temp4.y = temp.y;
           temp4.x = temp.x+1;
           parents[temp4.x][temp4.y][0]=temp.x;
           parents[temp4.x][temp4.y][1]=temp.y;
           insert(temp4);
           //UART1_SendByte('T');send_value(temp.x);send_value(temp.y);send_value(temp4.x);send_value(temp4.y);UART1_SendByte('P');
           visited[temp4.x][temp4.y]=1;
      }
	}
	if(path_found==0) {
		// No path available
		LCD_Cursor(1,1);
 	 	LCD_String("NOT POSSIBLE");
	}
	else {
		// Moved successfully
	}
	return 1;
}

void updateEdges(int dir, int x, int y) {

    float ReqdShaftCount = 0;
    unsigned int ReqdShaftCountInt = 0;
    unsigned int isEdge[4];
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int Degrees = 45;
    int i = 1;

    isEdge[0] = isEdge[1] = isEdge[2] = isEdge[3] = 0;

    a = AD1_Conversion(3);
    b = AD0_Conversion(1);
    c = AD0_Conversion(2);

    LCD_Print(2,1,a,3);
    LCD_Print(2,5,b,3);
    LCD_Print(2,9,c,3);

    if(a>100 || b>100 || c>100) {
        isEdge[dir] = 1;
    }
    //UART1_SendByte('T');send_value(isEdge[dir]);UART1_SendByte(' ');

    Right();
    //Rotate initial 45 degrees
    ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
    ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
    Left_Shaft_Count = 0;
    Right_Shaft_Count = 0;
    //UART1_SendByte('T');
    while (1) {
        //UART1_SendByte('T');UART1_SendByte(' ');send_value(Right_Shaft_Count);UART1_SendByte(' ');send_value(Left_Shaft_Count);UART1_SendByte(' ');
        //a = AD1_Conversion(3);
        //b = AD0_Conversion(1);
        //c = AD0_Conversion(2);
        if((Right_Shaft_Count >= ReqdShaftCountInt) || (Left_Shaft_Count >= ReqdShaftCountInt))
            break;
    }
    //UART1_SendByte('Z');
    Stop();
    DelaymSec(DELAY);
    Degrees = 85;
    while(i < 6) {
        Right();
        //Find First Left
        ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
        ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
        Left_Shaft_Count = 0;
        Right_Shaft_Count = 0;
        while (1) {
            a = AD1_Conversion(3);
            b = AD0_Conversion(1);
            c = AD0_Conversion(2);
            if(a>100 || b>100 || c>100) {
                if(i == 5)
                    break;
                else
                    isEdge[(i+dir)%4] = 1;
            }
            if(i<5) {
                if((Right_Shaft_Count >= ReqdShaftCountInt) || (Left_Shaft_Count >= ReqdShaftCountInt)) {
                    break;
                }
            }
        }
        i++;
        Stop();
        DelaymSec(DELAY);
    }
    //UART1_SendByte('L');send_value(isEdge[dir]);UART1_SendByte(' ');
    current_dir=(current_dir+1)%4;
    for(i=1; isEdge[(dir+i)%4]==0 && i<=4; i++)
        current_dir = (current_dir+1)%4;
    UART1_SendByte('A');send_value(x);send_value(y);send_value(isEdge[0]);send_value(isEdge[1]);send_value(isEdge[2]);send_value(isEdge[3]);
    edges[x][y].left = isEdge[0];
    edges[x][y].top = isEdge[1];
    edges[x][y].right = isEdge[2];
    edges[x][y].bottom = isEdge[3];
}

void generate_map() {
    //struct node current;
    struct node temp;
    int i,j;

    temp.x = 0;
    temp.y = 0;

    for(i=0;i<MAX;i=i+1){
      for(j=0;j<MAX;j=j+1){
         visited[i][j]=0;
         processed[i][j]=0;
      }
    }

    //temp = current;
    stack_init();
    sinsert(temp);
    visited[temp.x][temp.y] = 1;
    while(!sempty()) {
        temp = stop();
        i = temp.x; j = temp.y;
        if(processed[i][j]==0){
        	updateEdges(current_dir, i, j);
        	processed[i][j]=1;
        }
        if(edges[i][j].left==1 && visited[i][j-1]==0) {
            struct node left;
            left.x = i;
            left.y = j-1;
            sinsert(left);
            visited[i][j-1]=1;
            move_to(left.x, left.y);
        } else if(edges[i][j].top==1 && visited[i-1][j]==0) {
            struct node up;
            up.x = i-1;
            up.y = j;
            sinsert(up);
            visited[i-1][j]=1;
            move_to(up.x, up.y);
        }else if(edges[i][j].right==1 && visited[i][j+1]==0) {
            struct node right;
            right.x = i;
            right.y = j+1;
            sinsert(right);
            visited[i][j+1]=1;
            move_to(right.x, right.y);
        } else if(edges[i][j].bottom==1 && visited[i+1][j]==0) {
            struct node down;
            down.x = i+1;
            down.y = j;
            sinsert(down);
            visited[i+1][j]=1;
            move_to(down.x, down.y);
        } else {
            struct node parent;
            spop();
            if(sempty()) break;
            parent = stop();
            move_to(parent.x, parent.y);
        }
    }
    UART1_SendByte('B');
}

int  main(void)
{
 unsigned int battery_value, temp;//,a, b, c;
 PINSEL0 = 0x00000005;		    // Reset all pins as GPIO
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;
 Init_Peripherals();
 /*UART1_SendByte('H');
 UART1_SendByte('i');
 UART1_SendByte('!');
 UART1_SendByte(' ');*/
 Init_Motion_Pin();
 LCD_Init();
 Init();
 Ext_INT_Setup();
 Init_PWM();
 UpdateLeftPWM(HIGH);		//Set Velocity here. Smaller the value lesser will be the velocity.
 UpdateRightPWM(HIGH);
 current_x = 0; current_y = 0; current_dir = 2; cell_width = 200; cell_height = 200;//move_to(1,1);move_to(0,0);

 LCD_Cursor(1,1);
 LCD_String("AUTOMATIC DRIVING");
 LCD_Cursor(2,1);
 LCD_String("     BOT     ");
 //shortest(1,1);
 //shortest(2,2);
 generate_map();
 while(1)
 {
   /*a = AD1_Conversion(3);	  //whiteline Left
		LCD_Print(2,1,a,3);

		b = AD0_Conversion(1);	  //whiteline Center
		LCD_Print(2,5,b,3);

		c = AD0_Conversion(2);	  //whiteline Right
		LCD_Print(2,9,c,3);	 */
   if(got==2){
     if(!shortest(to_x,to_y)){
     	handle_collision(to_x,to_y);
     }
     got=0;
   }
   temp = AD1_Conversion(4);	  //Battery
   battery_value = Batt_Voltage_Conversion(temp);
   if(battery_value<100){
   	 shortest(MAX-1,MAX-1);
   	 LCD_Cursor(1,1);
 	 LCD_String("LOW BATTERY");
 	 LCD_Cursor(2,1);
     LCD_String("GOING 2 CHARGE");
   }
 }
}
