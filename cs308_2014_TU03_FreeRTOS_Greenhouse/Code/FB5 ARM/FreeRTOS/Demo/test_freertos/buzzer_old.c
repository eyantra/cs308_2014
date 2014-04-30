 /********************************************************************************
 Written by: Rohit Chauhan, NEX Robotics Pvt. Ltd.  Date: 1st OCT 2010
 Edited by: Vishwanathan, ERTS Lab, IIT Bombay. Date: 17th JAN 2014

 IDE: Keil uVision4
 Tool Chain: Realview MDK_ARM Ver 4.10
 C Compiler: ARMCC.exe


 
 This experiment demonstrates the simple operation of Buzzer ON/OFF with
 delay. Buzzer is connected to P0.25 of LPC2148

 Concepts covered:
 Output operation, generating delay

 Note:
 
 1. Make sure that in the Target options following settings are 
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

   Copyright (c) 2010, NEX Robotics Pvt. Ltd.			-*- c -*-
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
#include <stdlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"


/* Demo app include files. */
#include "LCD.h"
#include "defines.h"
/****Function Prototypes**********/
void Init_Buzzer_Pin(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Delay(void);
void vBuzzer_Task(void *pvParameters);
void Init_ADC_Pin(void);
void Init_ADC0(void);
void Init_ADC1(void);
void Init(void);
unsigned int AD0_Conversion(unsigned char channel);
void vDisplayLight(void* pvParameters);
void vDisplayTask(void* pvParameters);
/***********************************/
void vBuzzer_Task( void *pvParameters ) 
{ 
    /* As per most tasks, this task is implemented in an infinite loop. */ 
    for( ;; ) 
    { 
		Buzzer_On();					//Turn ON buzzer
       	vTaskDelay(25);			//Wait
       	Buzzer_Off();					//Turn OFF Buzzer
       	vTaskDelay(25);				//Wait
	}
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

void Init(void)
{
	Init_LCD_Pin();
	Init_ADC_Pin();
	Init_ADC0();
	Init_ADC1();
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

void vDisplayLight(void* pvParameters)
{
	unsigned int light;
	float val;
	for ( ;; )
	{
		val = ((float)AD0_Conversion(7)/255.0);		
		light = (int)((val/(5.0 - val))*REST);		// light is the inverse
		LCD_Print(1,2,light,4);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}	

void vDisplayTask(void* pvParameters)
{
	unsigned int temp;
	unsigned int light;
	float val;
	for ( ;; )
	{
		temp = (((float)AD0_Conversion(6)/255.0)*3.3*100);
		LCD_Print(1,3,temp,4);

		val = ((float)AD0_Conversion(7)/255.0);				// 8 bit result of the ADC converted to a float value between 0 and 1
		light = (int)((val/(5.0 - val))*REST);				// light is the inverse of the value of resistance of LDR
		LCD_Print(1,10,light,4);							// Print value of light on the LCD screen
		
		vTaskDelay(100);									// Blocks the task for 100 ticks
	}
}
/***********************************/	                            
int main(void)
{  
 	PINSEL2 = 0x00000000; 			//As it is not zero on reset
	DelaymSec(40);					// Initializing the LCD
	Init();
	LCD_4Bit_Mode();
	LCD_Init();
	LCD_Command(0x01);
	DelaymSec(20);
	
 	//xTaskCreate( vBuzzer_Task, "Buzzer Task",100, NULL, 2, NULL );
	
	//xTaskCreate( vDisplayTemp, "T_Disp", 100, NULL, 3, NULL);
	
	xTaskCreate( vDisplayTask, "Disp", 100, NULL, 2, NULL);			// Creating the task to display the sensor readings on the LCD 
 
    /* Start the scheduler so the tasks start executing. */ 
    vTaskStartScheduler();     
     
    /* If all is well then main() will never reach here as the scheduler will  
    now be running the tasks.  If main() does reach here then it is likely that  
    there was insufficient heap memory available for the idle task to be created.  
    CHAPTER 5 provides more information on memory management. */ 
    for( ;; ); 
}



