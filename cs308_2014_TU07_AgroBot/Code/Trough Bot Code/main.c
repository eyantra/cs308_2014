/********************************************************************************
 IDE: Keil uVision4
 Tool Chain: Realview MDK_ARM Ver 4.10
 C Compiler: ARMCC.exe

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

*********************************************************************************/

#include  <lpc214x.h>
#include "defines.h"
#include "LCD.h"
#include <stdio.h>
#include <math.h>

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
void DelaymSec(unsigned int Delay);
void Init_UART1(void);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);
unsigned int get_digital(unsigned int val, unsigned int th);
/**********************************************/

/******************Global Variables****************/

unsigned char Temp=0;
/**************************************************/


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



//This function is UART0 Receive ISR. 
// This functions is called whenever UART0 receives any data
void  __irq IRQ_UART1(void)
{  
 Temp = U1RBR;			

 if(Temp == 0x38) //ASCII value of 8
 {
  Forward();  //forward
 }
 
 if(Temp == 0x32) //ASCII value of 2
 {
  Back(); //back
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

 if(Temp == 0x39) //ASCII value of 9
 {
  BUZZER_OFF();
  Temp = 'C';
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


void Init(void);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
unsigned int AD0_Conversion(unsigned char channel);
unsigned int AD1_Conversion(unsigned char channel);
unsigned int Sharp_GP2D12_Estimation(unsigned int Val);
unsigned int Batt_Voltage_Conversion(unsigned int Val);

extern unsigned char String1[16];	//This variable is defined in LCD.c
extern unsigned char String2[16];	//This variable is defined in LCD.c
unsigned int ADC_Data[8];

void Init_ADC_Pin(void)
{
 PINSEL0|= (BAT_VOL_IO_0 | BAT_VOL_IO_1 | WH_LINE_SEN_1_IO_0 | WH_LINE_SEN_1_IO_1 | SHR_IR_SEN_2_IO_0 | SHR_IR_SEN_2_IO_1 | SHR_IR_SEN_3_IO_0 | SHR_IR_SEN_3_IO_1 |SHR_IR_SEN_4_IO_0 | SHR_IR_SEN_4_IO_1) ;	//Set pins P0.4, P0.5, P0.6, P0.12, P0.13 as ADC pins
 PINSEL1&= ~(WH_LINE_SEN_2_IO_1 | WH_LINE_SEN_3_IO_1) ;		
 PINSEL1|= (WH_LINE_SEN_2_IO_0 | WH_LINE_SEN_3_IO_0);	    //Set pins P0.28, P0.29 as ADC pins
}


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

// This function converts ADC0 channels. 
// Channel number is passed to this function as integer.
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

// This function converts ADC1 channels. 
// Channel number is passed to this function as integer.
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


/************************************************************
	Function 		: Init
	Return type		: None
	Parameters		: None
	Description 	: Initialises LCD, ADC, ADC0, ADC1
************************************************************/
void Init(void)
{
 // Initializes the pins to make LCD work
 Init_LCD_Pin();
 // Initializes the pins to make ADC work
 Init_ADC_Pin();
 // Initializes the pins to make ADC0 work
 Init_ADC0();
 // Initializes the pins to make ADC1 work
 Init_ADC1();
}

/************************************************************
	Function 		: get_digital
	Return type		: int(0 or 1) 
	Parameters		: unsigned int val - analog sensor value,  unsigned int th - threshold
	Description 	: returns 1 if val > th, 0 otherwise
************************************************************/
unsigned int get_digital(unsigned int val, unsigned int th)
{
	if(val > th)
		return 1;
	else
		return 0;
}

int  main(void)
{  
 // Variable to hold the sensor values
 // a,b,c corresponds to values from sensor 1
 // d,e,f corresponds to values from sensor 2
 unsigned int a,b,c,d,e,f;
 unsigned char bt;
 // Reset all pins as GPIO
 PINSEL0 = 0x00000005;		    
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;																									
 DelaymSec(40);
 // Initializing all the peripherals
 Init();
 LCD_4Bit_Mode();
 LCD_Init();
 LCD_Command(0x01);
 DelaymSec(20);
 Init_Peripherals();

 // Always reading the sensor values
 while(1)	
 { 
  // Getting the analog sensor values from each of the 6 pins
  // We use ADi_Conversion to get the analog values
  ADC_Data[0] = AD0_Conversion(6);	  //Sensor 1 Pin 1
  ADC_Data[1] = AD1_Conversion(0);	  //Sensor 1 Pin 2
  ADC_Data[2] = AD0_Conversion(7);	  //Sensor 1 Pin 3
  ADC_Data[4] = AD1_Conversion(3);	  //Sensor 2 Pin 1
  ADC_Data[5] = AD0_Conversion(1);	  //Sensor 2 Pin 2
  ADC_Data[6] = AD0_Conversion(2);	  //Sensor 2 Pin 3
   
  // Getting the digital values corresponding to the analog values
  // 1 if analog values is greater than 40
  // We use threshold as 40 because
  // normally the analog values is 20 when 2 wires are not in water
  // and 60 when both the probes are in water											   
  a = get_digital(ADC_Data[0],40);
  b = get_digital(ADC_Data[1],40);
  c = get_digital(ADC_Data[2],40);
  d = get_digital(ADC_Data[4],40);
  e = get_digital(ADC_Data[5],40);
  f = get_digital(ADC_Data[6],40);
  
  // Printing the details of Trough 1
  // Printing 0 or 1 corresponding to the sensor 1 values
  LCD_Print(1,1,a,1);
  LCD_Print(1,5,b,1);
  LCD_Print(1,9,c,1);

  // Printing the level of water in Trough 1
  // F - Full
  // M - Medium
  // L - Low
  // E - Empty
  if(a&b&c)
  {	
  	LCD_Cursor(1,13);
 	LCD_String("F");
  }
  else if(a&b)
  {
  	LCD_Cursor(1,13);
 	LCD_String("M");
  }
  else if(a)
  {
  	LCD_Cursor(1,13);
 	LCD_String("L");
  }		
  else
  {
  	LCD_Cursor(1,13);
 	LCD_String("E");
  }				
  
  // Printing the details of Trough 2
  // Printing 0 or 1 corresponding to the sensor 2 values
  LCD_Print(2,1,d,1);
  LCD_Print(2,5,e,1);
  LCD_Print(2,9,f,1);
  
  // Printing the level of water in Trough 2
  // F - Full
  // M - Medium
  // L - Low
  // E - Empty
  if(d&e&f)
  {	
  	LCD_Cursor(2,13);
 	LCD_String("F");
  }
  else if(d&e)
  {
  	LCD_Cursor(2,13);
 	LCD_String("M");
  }
  else if(d)
  {
  	LCD_Cursor(2,13);
 	LCD_String("L");
  }		
  else
  {
  	LCD_Cursor(2,13);
 	LCD_String("E");
  }				
  // Calculating the binary equivalent of the 6 bits
  // We add 33 so that bt corresponds to a printable character					 
  bt = a + 2*b + 4*c + 8*d + 16*e + 32*f + 33;

  // Sending the byte bt via Xbee
  UART1_SendByte(bt);	
 }
}
