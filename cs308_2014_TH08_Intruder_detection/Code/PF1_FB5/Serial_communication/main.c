#include  <lpc214x.h>
#include "defines.h"



/*************Global Variables*************/

volatile unsigned int Left_Shaft_Count=0;
volatile unsigned int Right_Shaft_Count=0;
volatile unsigned int ReqdShaftCountInt;
unsigned int x_dist = 450, y_dist = 300;

/******************************************/
void  __irq IRQ_Eint0(void);
void  __irq IRQ_Eint2(void);

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
void Left_Degrees(unsigned int Degrees);
void Right(void);
void Right_Degrees(unsigned int Degrees);
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


//This function is UART1 Receive ISR. This functions is called whenever UART1 receives any data
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
	Left_Degrees(40);
 }
  
 if(Temp == 0x36) //ASCII value of 6
 {
  Right_Degrees(40); //right
 }

 if(Temp == 0x35) //ASCII value of 5
 {
  Stop(); //stop
	DelaymSec(500);
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
 IO0CLR = L_B | R_B;
}

//Function to move robot in backward direction
void Back(void)
{
 Stop();
 L_Back();
 R_Back();
 IO1CLR = L_F;		//clr P1.21 to logic '0'
 IO0CLR = R_F;  		//clr P0.10 to logic '0'
}

//Function to turn robot in Left direction
void Left(void)
{ 
 Stop();
 L_Back();
 R_Forward();
 IO1CLR = L_F;		//clr P0.21 to logic '0'
 IO0CLR = R_B;		//P0.11 to logic '0'
}

//Function to turn robot in right direction
void Right(void)
{ 
 Stop();
 L_Forward();
 R_Back();
 IO0CLR = L_B | R_F;  	//clr P0.22 and P0.10 to logic '0'
//	IO0CLR = L_B | R_F;  	//clr P0.22 and P0.10 to logic '0'
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

//Function used for turning robot by specified degrees
void Angle_Rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 Left_Shaft_Count = 0;
 Right_Shaft_Count = 0;
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

void Ext_INT_Setup(void)
{
 PINSEL0&=~(EINT2_E_0);
 PINSEL0|= EINT2_E_1; //Enabling P0.15 as EINT2
 PINSEL1&= ~(EINT0_E_1);
 PINSEL1|= EINT0_E_0; //Enabling P0.16 as EINT0


 EXTMODE = 0x05;	// EINT2 and EINT0 is edge sensitive
 EXTPOLAR = 0x00;					// EINT2 and EINT0 in triggered on falling edge

 VICIntSelect = 0x00000000;		// Setting EINT2 and EINt0 as IRQ(Vectored)
 VICVectCntl1 = 0x20|16;		// Assigning Highest Priority Slot to EINT2 and enabling this slot
 VICVectAddr1 = (int)IRQ_Eint2; // Storing vector address of EINT2

 VICVectCntl0 = 0x20|14;		// Assigning second Highest Priority Slot to EINT0 and enabling this slot
 VICVectAddr0 = (int)IRQ_Eint0; // Storing vector address of EINT0
 EXTINT = 0x05;	//Clearing EINT2 & EINT0 interrupt flag
 	
 VICIntEnable = (1<<16) | (1<<14);	// Enable EINT2	& EINT0 flags
}


//ISR for EINT0
void  __irq IRQ_Eint0(void)
{  
   Right_Shaft_Count++;
	 if(Temp==0x34){
		 UART1_SendByte('L');
		 if(Right_Shaft_Count >= ReqdShaftCountInt) Stop();
	 }
   EXTINT = 0x01;				// Clear EINT0 flag
   VICVectAddr = 0x00;   		//Acknowledge Interrupt
}	


//ISR for EINT2
void  __irq IRQ_Eint2(void)
{  
   Left_Shaft_Count++;
	 if(Temp==0x36){
		 UART1_SendByte('R');
		 if(Left_Shaft_Count >= ReqdShaftCountInt ) Stop();
	 }
	 
   EXTINT = 0x04;				// Clear EINT2 flag
   VICVectAddr = 0x00;   	//Acknowledge Interrupt
}

void Init_Ports(void)
{
 Init_Motion_Pin();
}

void Init_Peripherals(void)
{
 Init_Ports();
 Init_UART1();
}


int  main(void)
{  
 PINSEL0 = 0x00000000;		    // Reset all pins as GPIO
 PINSEL1 = 0x00000000;
 PINSEL2 = 0x00000000;
 Init_Peripherals();
 Ext_INT_Setup();
 
 UART1_SendByte('H');
 UART1_SendByte('i');
 UART1_SendByte('!');
 while(1)			   
 {  
     
 }
}
