#include <lpc214x.h>
#include "LCD.h"
#include <math.h>
#include "defines.h"


/*********Function Prototypes********/

/* Init functions */
void Init(void);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
unsigned int AD0_Conversion(unsigned char channel);

void Init_Motion_Pin(void);
void Init_Peripherals(void);
void Init_Ports(void);
void  __irq IRQ_UART1(void);
void Init_UART1(void);
void UART1_SendByte(unsigned char data);
void UART1_SendStr(const unsigned char *str);

/* User functions */
void Forward(void);
void Back(void);
void Stop(void);

void openShade(void);
void closeShade(void);
void switchOnFan(void);
void switchOffFan(void);
void temperatureResponse(int T);


/************Constants*****************/
#define Fosc            12000000                    //10MHz~25MHz
#define Fcclk           (Fosc * 5)                  //Fosc(1~32)<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO Fcclk 2񅦰�16�156MHz~320MHz
#define Fpclk           (Fcclk / 1) * 1             //VPB(Fcclk / 1) 1񄿔
#define UART_BPS		9600 						//Change Baud Rate Setting here


/**********Global variables***********/

extern unsigned char String1[16];	//This variable is defined in LCD.c
extern unsigned char String2[16];	//This variable is defined in LCD.c
unsigned int ADC_Data[8];

int shadeOpen = 0;			// whether the shade is open or closed (closed initially)
int fanOn = 0;				// whether the fan is on or off (off initially)

int inputDigit = 0;			// digit read from the web interface
int thresholdT1 = 35;		// the lower temperature threshold
int thresholdT2 = 40;		// the higher temperature threshold
int tempConstant = 2;		// the temperature buffer
int interruptCount = -1;	// the count of the interrupts from the web interface


/**********Function definitions*********/

/* This function is UART0 Receive ISR. This functions is called whenever UART0 receives any data */
/* The temperature values received are stored in the corresponding global variables based on the interrupt count */
void  __irq IRQ_UART1(void)
{  
	interruptCount++;
	inputDigit = U1RBR-48;			
	if (interruptCount == 0){
		thresholdT1 = inputDigit;
	}
	else if(interruptCount == 1){
		thresholdT1 = thresholdT1 * 10 + inputDigit;
	}
	else if(interruptCount == 2){
		thresholdT2 = inputDigit;
	}
	else if(interruptCount == 3){
		thresholdT2 = thresholdT2 * 10 + inputDigit;
	}
	else if(interruptCount == 4){
		tempConstant = inputDigit;
		interruptCount = -1;
	}
	VICVectAddr = 0x00;
}		

/* This function initializes the UART interface */
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
				
/* This function sends a single character on the serial port */
void UART1_SendByte(unsigned char data)
{  
   U1THR = data;				    
   while( (U1LSR&0x40)==0 );	    
}

/* This function sends a string of characters on the serial port */
void UART1_SendStr(const unsigned char *str)
{  
   while(1)
   {  
      if( *str == '\0' ) break;
      UART1_SendByte(*str++);	    
   }
}

/* This function initializes the required GPIOs as ADC pins */
void Init_ADC_Pin(void)
{
 PINSEL0|= (BAT_VOL_IO_0 | BAT_VOL_IO_1 | WH_LINE_SEN_1_IO_0 | WH_LINE_SEN_1_IO_1 | SHR_IR_SEN_2_IO_0 | SHR_IR_SEN_2_IO_1 | SHR_IR_SEN_3_IO_0 | SHR_IR_SEN_3_IO_1 |SHR_IR_SEN_4_IO_0 | SHR_IR_SEN_4_IO_1) ;	//Set pins P0.4, P0.5, P0.6, P0.12, P0.13 as ADC pins
 PINSEL1&= ~(WH_LINE_SEN_2_IO_1 | WH_LINE_SEN_3_IO_1) ;		
 PINSEL1|= (WH_LINE_SEN_2_IO_0 | WH_LINE_SEN_3_IO_0);	    //Set pins P0.28, P0.29 as ADC pins
}

/* This fuction initialises ADC 0 module of LPC2148 microcontroller. */
/* It also configures the required I/o pins to be used as ADC pins. */
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

/* This function converts ADC0 channels. Channel number is passed to this function as integer. */
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

/* This functions calls the individual init functions */
void Init(void)
{
 Init_LCD_Pin();
 Init_ADC_Pin();
 Init_ADC0();
 Init_Peripherals();
}

/* This function initializes the input/output ports */
void Init_Ports(void)
{
 Init_Motion_Pin();
}

/* This function calls the individual functions to initialize the ports and xbee communication */
void Init_Peripherals(void)
{
 Init_Ports();
 Init_UART1();
}

/* This function initializes the input/output motor pins */ 
void Init_Motion_Pin(void)
{
 PINSEL0&=~(R_F_IO_0 | R_F_IO_1 | R_B_IO_0 | R_B_IO_1 | L_E_IO_0 | L_E_IO_1); //Set Port pins P0.7, P0.10, P0.11 as GPIO
 PINSEL1&=~(R_E_IO_0 | R_E_IO_1 | L_B_IO_0 | L_B_IO_1); //Set Port pins P0.21 and 0.22 as GPIO
 PINSEL2&=~(L_F_IO_0 | L_F_IO_1); //Set pins 16-31 of port 1 as GPIO
 IO0DIR|= L_B | R_F | R_B | L_E | R_E; 	//Set Port pins P0.10, P0.11, P0.21, P0.22, P0.7 as Output pins
 IO1DIR|= L_F | FAN_E;		//Set P1.21 as output pin
 IO1CLR = FAN_E;
 Stop();				//Stop both the motors at the start
 IO0SET = L_E | R_E;
}

/* This function turns both the motors forward/clockwise */
void Forward(void)
{ 
 //Stop();
 IO1SET = L_F;
 IO0SET = R_F;
 IO0CLR = L_B | R_B;
}

/* This function turns both the motors backward/anti-clockwise */
void Back(void)
{ 
 //Stop();													
 IO0SET = L_B | R_B;
 IO1CLR = L_F;
 IO0CLR = R_F;
}

/* This function stops both the motors */
void Stop(void)
{
 IO1CLR = L_F;
 IO0CLR = L_B | R_F| R_B;
}

/* This function opens the shade by rotating both the motors in clockwise direction for 5 seconds */
void openShade(void) {
	shadeOpen = 1;
	Forward();
	DelaymSec(6000);
	Stop();
}

/* This function opens the shade by rotating both the motors in anti-clockwise direction for 5 seconds */
void closeShade(void) {
	shadeOpen = 0;
	Back();
	DelaymSec(6000);
	Stop();
}

/* This function switches on the fan by setting the appropriate port to high */
void switchOnFan(void) {
	fanOn = 1;
	IO1SET = FAN_E;
}

/* This function switches off the fan by setting the appropriate port low */
void switchOffFan(void) {
	fanOn = 0;
	IO1CLR = FAN_E;
}

/* This is the control function to operate the motors and fan according to the temperature */
void temperatureResponse(int T) {
	//LCD_Print(1,1,T,3);
	if (shadeOpen && T < thresholdT1) {
		closeShade();
	}
	else if (!shadeOpen && T > thresholdT1+tempConstant) {
		//LCD_Print(1,13,T,3);
		openShade();
	}

	if (fanOn && T < thresholdT2) {
		switchOffFan();
	}
	else if (!fanOn && T > thresholdT2+tempConstant) {
		switchOnFan();
	}
}

/* The main function */
int main(void)
{
 unsigned int Temp = 0; 
 PINSEL2 = 0x00000000;
 DelaymSec(40);
 Init();
 LCD_4Bit_Mode();
 LCD_Init();
 LCD_Command(0x01);
 DelaymSec(20);
 Init_Motion_Pin();

 while(1)	
 { 
	
	ADC_Data[0] = AD0_Conversion(7);	  //data from the temperature sensor
	Temp = ((float)ADC_Data[0]/255.0) * 330;
	
	LCD_Print(1,8,Temp,2);
	LCD_Print(2,1,thresholdT1,2);
	LCD_Print(2,8,thresholdT2,2);
	LCD_Print(2,14,tempConstant,2);		//print the values on the LCD screen

	temperatureResponse(Temp);
  	
  DelaymSec(200);
 }
 
}
