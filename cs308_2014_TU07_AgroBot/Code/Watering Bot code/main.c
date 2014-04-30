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
void Init_Motor_Pin(void);
void Init_PWM(void);
void Init_Buzzer_Pin(void);
void Init_Peripherals(void);
void Init_Ports(void);
void  __irq IRQ_UART1(void);
void DelaymSec(unsigned int Delay);
void DisplayNum(int num);
void Init_UART1(void);
void UpdateLeftPWM(unsigned int vel);
void UpdateRightPWM(unsigned int vel);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);

/**********************************************/

void Motor_On(void);
void Motor_Off(void);
void Motor_Delay(void);
void Init_Water_Pin(void);
void FillWaterInTrough(void);

/******************Global Variables****************/

unsigned char Temp=0;
 char c[10]; 
 int tempTrough;   // helper variable
 int tempLevel;	   // helper variable
 int Trough_No; ;  // the trough which needs to be served 
 int Water_level;  // the amount of water level in thr current trough - needed to control the time of watering
 int i;			   // helper variable in loops
 int bitarray[8];  // contains the bits of a byte
 int busy=0;	   // status of the bot, if it is serving a trough, status is busy
 int tTemp;        // helper variable
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

/************************************************************

	Function 		: Init_Motor_Pin
	Return type		: None
	Parameters		: None
	Description 	: Initialises Motor pin
************************************************************/

 void Init_Motor_Pin(void)
{
 PINSEL1&=~(MOTOR_IO_0); 		
 PINSEL1&=~(MOTOR_IO_1); 		
 IO0DIR|= MOTOR;  			//Set P0.17 as Output
 Motor_Off();				//Initially turn OFF the motor
}

//This function is UART0 Receive ISR. This functions is called whenever UART0 receives any data

void  __irq IRQ_UART1(void)
{  
 Temp = U1RBR;
 // Process the received byte only if  the bot is not busy
 if(!busy){
 	busy = 1; // set busy status to 1
	tTemp = (int)Temp; 	 // get the current byte in a integer variable
	tTemp-=33;			 // subtract 33 that was added to make the byte printable
	// fill the bits in the bitarray
	 for(i=7;i>=0;i--){
		bitarray[i]=(tTemp&1);
		tTemp = tTemp>>1; 	
	 }		

	 // get the trough id and water level into helper variable
	 // first 4 bits are for trough id and last 4 bits are for water level
	 tempLevel = 8*bitarray[4]+4*bitarray[5]+2*bitarray[6]+1*bitarray[7];	
	 tempTrough = 8*bitarray[0]+4*bitarray[1]+2*bitarray[2]+1*bitarray[3];
	 
	 // set the Trough_No and Water_level for the FillWaterInTrough function 
	 Trough_No  = tempTrough;
	 Water_level = tempLevel;
	 
	 // Function call to fill water in the trough
	 FillWaterInTrough();
	 
	 VICVectAddr = 0x00;
	 busy = 0;	 //  set busy status to 0
	 UART1_SendByte('K');	// Send the ack
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
 PINSEL0&=~(R_F_IO_0 | R_F_IO_1 | R_B_IO_0 | R_B_IO_1 | L_E_IO_0 | L_E_IO_1);//Set Port pins P0.7, P0.10, P0.11 as GPIO
 PINSEL1&=~(R_E_IO_0 | R_E_IO_1 | L_B_IO_0 | L_B_IO_1);//Set Port pins P0.21 and 0.22 as GPIO
 PINSEL2&=~(L_F_IO_0 | L_F_IO_1);//set pins 16-31 of port 1 as GPIO
 IO0DIR|= L_B | R_F | R_B | L_E | R_E; 	//Set Port pins P0.10, P0.11, P0.21, P0.22, P0.7 as Output pins
 IO1DIR|= L_F;		// Set P1.21 as output pin
 Stop();				// Stop both the motors on start up
 IO0SET = L_E | R_E;	// Set PWM pins P0.7/PWM2 and P0.21/PWM5 to logic 1	   This pins are set as PWM pins in Init_PWM fuction
}

/************************************************************

	Function 		: Init_PWM
	Return type		: None
	Parameters		: None
	Description 	: Initialises PWM module for motion control
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
// Functions to change the PWM of left motor
void UpdateLeftPWM(unsigned int vel)
{
 PWMMR2 = vel;
 PWMLER = 0x04;
}

// Functions to change the PWM of right motor
void UpdateRightPWM(unsigned int vel)
{
 PWMMR5 = vel;
 PWMLER = 0x20;
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

/************************************************************

	Function 		: Init_Peripherals
	Return type		: None
	Parameters		: None
	Description 	: Calls all the initialisation functions for various modules
************************************************************/

void Init_Peripherals(void)
{
 Init_Motion_Pin();
 Init_PWM();
 Init_Buzzer_Pin();
 Init_Motor_Pin();
 Init_UART1();
}

// The following functions are for ADC intialisation and reading

void Init(void);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
unsigned int AD0_Conversion(unsigned char channel);
unsigned int AD1_Conversion(unsigned char channel);

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

void Init(void)
{
 Init_LCD_Pin();
 Init_ADC_Pin();
 Init_ADC0();
 Init_ADC1();
}

/***********************************/
// Function to start the motor																					
void Motor_On(void)
{
IO0SET=MOTOR;	
}
/***********************************/
// Function to turn the motor off
void Motor_Off(void)
{
IO0CLR=MOTOR;	
}
/***********************************/
// Function to insert delay into watering action
// 1 Motor_Delay is equivalent to filling 1/3 of a small water trough  (should be changed if time is to be used)
void Motor_Delay(void)
{  
 unsigned int  i,j;
 for(j=0;j<75;j++)
 {
  for(i=0; i<500000; i++);
 } 
}

/***********************************/

// Function to control the release of water from the tank depending on water level of the trough
// If level = 3, already full no need to water the trough
// If level = 2, release water only once
// If level = 1, release water twice
// If level = 0, release water thrice

void ReleaseWater(int level){
	if(level==3){
		;
	}
	else if(level==2){
		Motor_On();					//Turn ON Motor
		Motor_Delay();				//Wait
	    Motor_Off();					//Turn OFF Motor
	    Motor_Delay();				//Wait
		Motor_Delay();				//Wait
	}
	else if(level==1){
		Motor_On();					//Turn ON Motor
		Motor_Delay();				//Wait
		Motor_Delay();				//Wait
	    Motor_Off();					//Turn OFF Motor
	    Motor_Delay();				//Wait
	 	Motor_Delay();				//Wait
	}
	else if(level==0){
		Motor_On();					//Turn ON Motor
		Motor_Delay();				//Wait
		Motor_Delay();				//Wait
		Motor_Delay();				//Wait
	    Motor_Off();					//Turn OFF Motor
	    Motor_Delay();				//Wait
	 	Motor_Delay();				//Wait
	}
}

int left, center, right,diff,hop;
// Function to fill water in the given through using line following
// Given the trough id, from the base position, jump that many black marks to reach the trough and water it
// Move forward until the trough is reached
// Release water in that trough
// Move back to the base position

void FillWaterInTrough(){
	UpdateLeftPWM(420);
	UpdateRightPWM(450);
  Forward();
  hop = 0;

  // move straight until the given trough is reached
  while(hop<=Trough_No){

  	  ADC_Data[4] = AD1_Conversion(3);	  //whiteline Left
	  LCD_Print(2,1,ADC_Data[4],3);
	  
	  ADC_Data[5] = AD0_Conversion(1);	  //whiteline Center
	  LCD_Print(2,7,ADC_Data[5],3);
	
	  ADC_Data[6] = AD0_Conversion(2);	  //whiteline Right
	  LCD_Print(2,13,ADC_Data[6],3);
	   
	  left = (ADC_Data[4]>70)?1:0;
	  center = (ADC_Data[5]>70)?1:0;
	  right = (ADC_Data[6]>70)?1:0;
	
	  // keeping the bot on the white line
	  diff = ADC_Data[6]-ADC_Data[4];
	  diff = diff*50/500;
	  UpdateLeftPWM(420-diff);
	  UpdateRightPWM(450+diff);

      if(left && center && right){	
		hop++; // black marks crossed

		// loop so that entire blackspot isnt counted again and again 
		while(left && center && right){
		  	  ADC_Data[4] = AD1_Conversion(3);	  //whiteline Left
			  LCD_Print(2,1,ADC_Data[4],3);
			  
			  ADC_Data[5] = AD0_Conversion(1);	  //whiteline Center
			  LCD_Print(2,7,ADC_Data[5],3);
			
			  ADC_Data[6] = AD0_Conversion(2);	  //whiteline Right
			  LCD_Print(2,13,ADC_Data[6],3);
			   
			  left = (ADC_Data[4]>70)?1:0;
			  center = (ADC_Data[5]>70)?1:0;
			  right = (ADC_Data[6]>70)?1:0;
		}		
	  }	  
	  DelaymSec(10);
  }

  // Stop and water the given trough
  Stop();
  ReleaseWater(Water_level);

  // Move back to the base position
  UpdateLeftPWM(420);
  UpdateRightPWM(450);
  Back();
  while(hop>0){
  	  ADC_Data[4] = AD1_Conversion(3);	  //whiteline Left
	  LCD_Print(2,1,ADC_Data[4],3);
	  
	  ADC_Data[5] = AD0_Conversion(1);	  //whiteline Center
	  LCD_Print(2,7,ADC_Data[5],3);
	
	  ADC_Data[6] = AD0_Conversion(2);	  //whiteline Right
	  LCD_Print(2,13,ADC_Data[6],3);
	   
	  left = (ADC_Data[4]>70)?1:0;
	  center = (ADC_Data[5]>70)?1:0;
	  right = (ADC_Data[6]>70)?1:0;
	
	// keeping the bot on the white line
	  diff = ADC_Data[6]-ADC_Data[4];
	  diff = diff*50/500;
	  UpdateLeftPWM(420+diff);
	  UpdateRightPWM(450-diff);	
      if(left && center && right){	
		hop--; // black marks crossed
		while(left && center && right){
		  	  ADC_Data[4] = AD1_Conversion(3);	  //whiteline Left
			  LCD_Print(2,1,ADC_Data[4],3);
			  
			  ADC_Data[5] = AD0_Conversion(1);	  //whiteline Center
			  LCD_Print(2,7,ADC_Data[5],3);
			
			  ADC_Data[6] = AD0_Conversion(2);	  //whiteline Right
			  LCD_Print(2,13,ADC_Data[6],3);
			   
			  left = (ADC_Data[4]>70)?1:0;
			  center = (ADC_Data[5]>70)?1:0;
			  right = (ADC_Data[6]>70)?1:0;
		}		
	  }	
	  DelaymSec(10);
  }

  // Stop at the base
  Stop();
  DelaymSec(100);
}



int  main(void)
{  
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
 
 // after all the initialisation wait for the commands from the server
 while(1)	
 {   
  ADC_Data[4] = AD1_Conversion(3);	  //whiteline Left
  LCD_Print(2,1,ADC_Data[4],3);
  
  ADC_Data[5] = AD0_Conversion(1);	  //whiteline Center
  LCD_Print(2,7,ADC_Data[5],3);

  ADC_Data[6] = AD0_Conversion(2);	  //whiteline Right
  LCD_Print(2,13,ADC_Data[6],3);

 }
}
