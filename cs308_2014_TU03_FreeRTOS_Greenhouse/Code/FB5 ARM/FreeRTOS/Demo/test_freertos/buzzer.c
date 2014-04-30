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
#include <string.h>
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo app include files. */
#include "LCD.h"
#include "defines.h"

/****Macros***********************/
#define LONG_TIME 0xffff
#define IDLE_TASK_PRIORITY 1

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
unsigned int AD1_Conversion(unsigned char channel);
void vDisplayLight(void* pvParameters);
void vDisplayTask(void* pvParameters);
void IRQ_UART1(void);
void Init_UART1(void);
void UART1_SendByte(unsigned char data);
extern void vUART_ISREntry( void );
void Actuator_On(unsigned int);
void Actuator_Off(unsigned int);
int checkThreshold(unsigned int,unsigned int);
/***********************************/
/******************Global Variables****************/

unsigned int NUM_SETS = 2;
unsigned int NUM_SENSORS = 2;
unsigned int NUM_ACTUATORS = 3;
unsigned char Temp=0;
unsigned char UART_Str[25];
unsigned int *default_thresholds;
unsigned int *thresholds;
unsigned int *taskTimings;
unsigned int *sensor_ADC_channels;
unsigned int *sensor_ADC_ports;
SemaphoreHandle_t *sensorSemaphore = NULL;   
SemaphoreHandle_t * timeTaskSemaphore = NULL;   
TaskHandle_t *sensor_handler;
TaskHandle_t *time_task_handler;
unsigned int *set_state;
unsigned int *actuator_pins;
unsigned int *actuator_ports;
unsigned int *lcd_row;
unsigned int *lcd_col;
unsigned int *lcd_row_debug;
unsigned int *lcd_col_debug;
unsigned int *period;
/**************************************************/

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
/*
void vDisplayLight(void* pvParameters)
{
	unsigned int light;
	float val;
	for ( ;; )
	{
		val = ((float)AD0_Conversion(6)/255.0);				// 8 bit result of the ADC converted to a float value between 0 and 1
		light = (int)((val/(5.0 - val))*REST);				// light is the inverse of the value of resistance of LDR
		
		if(thresholds[0]>0)light_threshold=thresholds[0];
		portENTER_CRITICAL();
		{
			
			if(light<light_threshold) IO1SET |= SIGNAL;
			else IO1CLR |= SIGNAL;
			LCD_Print(1,10,light,4);							// Print value of light on the LCD screen
		}
		portEXIT_CRITICAL();
		vTaskDelay(100);
	}
}	

void vDisplayTemp(void* pvParameters){
	unsigned int temp;
	for ( ;; )
	{
		temp = (((float)AD0_Conversion(7)/255.0)*3.3*100);
		portENTER_CRITICAL();
		{
			LCD_Print(1,3,temp,4);
		}
		portEXIT_CRITICAL();
		vTaskDelay(100);
	}
}
*/

void Actuator_On(unsigned int actuator_num)
{
	if(actuator_ports[actuator_num]==0)
	{
		IO0SET|=1<<actuator_pins[actuator_num];
		IO0SET|=1<<actuator_pins[actuator_num];
		IO0SET|=1<<actuator_pins[actuator_num];
	}
	else
	{
		IO1SET|=1<<actuator_pins[actuator_num];	
		IO1SET|=1<<actuator_pins[actuator_num];			
	   	IO1SET|=1<<actu	ator_pins[actuator_num];
	}
}
/***********************************/
void Actuator_Off(unsigned int actuator_num)
{
	if(actuator_ports[actuator_num]==0)
	{
		IO0CLR|=1<<actuator_pins[actuator_num];
		IO0CLR|=1<<actuator_pins[actuator_num];
		IO0CLR|=1<<actuator_pins[actuator_num];
	}
	else
	{
		IO1CLR|=1<<actuator_pins[actuator_num];
		IO1CLR|=1<<actuator_pins[actuator_num];
		IO1CLR|=1<<actuator_pins[actuator_num];
	}
}
int Get_Sensor_Value(unsigned int sensor_num){
	float val;
	if(sensor_ADC_ports[sensor_num]==0)
	{
		val = ((float)AD0_Conversion(sensor_ADC_channels[sensor_num])/255.0);
	}
	else
	{
		val = ((float)AD1_Conversion(sensor_ADC_channels[sensor_num])/255.0);
	}
	switch(sensor_num%NUM_SENSORS)
	{
		  case 0:
		  {
		  	return (int)((val/(5.0 - val))*REST);				// light is the inverse of the value of resistance of LDR

		  }
		  case 1:
		  {
		  	return 	(int) (val*3.3*100);

		  }
	}
	return -1;
}

int checkThreshold(unsigned int sensor_num, unsigned int sensor_value){
	switch(sensor_num%NUM_SENSORS)
	{
		  case 0:
		  {
		  		return sensor_value<thresholds[sensor_num];	

		  }
		  case 1:
		  {
				return sensor_value>thresholds[sensor_num];			  	

		  }
	}
	return 0;	
}
void vSensorTask(void* pvParameters)
{
	unsigned int sensor_num, *sensor_num_ptr,sensor_value,actuator_num,set_num;
	sensor_num_ptr = (unsigned int*) pvParameters;
	sensor_num = *sensor_num_ptr;
	
	set_num = sensor_num/NUM_SENSORS;
	actuator_num = (sensor_num/NUM_SENSORS)*NUM_ACTUATORS + (sensor_num%NUM_SENSORS) ;
	
	sensorSemaphore[sensor_num] = xSemaphoreCreateBinary();
	if( xSemaphoreTake( sensorSemaphore[sensor_num], LONG_TIME ) == pdTRUE )
	{
		for ( ;; )
		{
			
			if(set_state[set_num] == 1){
				
				sensor_value = Get_Sensor_Value(sensor_num);
				
				if(checkThreshold(sensor_num,sensor_value)) Actuator_On(actuator_num);
				else Actuator_Off(actuator_num);
				
				portENTER_CRITICAL();
				{	
					LCD_Print(lcd_row[sensor_num],lcd_col[sensor_num],sensor_value,4);							// Print value of light on the LCD screen
				}
				portEXIT_CRITICAL();   
		
		
			/*	temp = (((float)AD0_Conversion(temp_sensor_ADC[sensor_num])/255.0)*3.3*100);
				if(light<thresholds[NUM_SENSORS*sensor_num+1]) Actuator_On(NUM_ACTUATORS*sensor_num+1);
				else Actuator_Off(NUM_ACTUATORS*sensor_num+1);
				portENTER_CRITICAL();
				{
					LCD_Print(1,3,temp,4);
				}
				portEXIT_CRITICAL(); */
				vTaskDelay(100);
			}
			else 
			{
				Actuator_Off(actuator_num);
				if(xSemaphoreTake( sensorSemaphore[sensor_num], LONG_TIME ) == pdTRUE)
				{
					continue;
				}
				else
					taskYIELD()	 ;
			}//vTaskSuspend(NULL);
			
				
				
		}
	}
	else
	{
		taskYIELD();
	}
}
void vTimeTask( void * pvParameters )
{
    
   unsigned int actuator_num, *actuator_num_ptr,sensor_num;
	actuator_num_ptr = (unsigned int*) pvParameters;
	actuator_num = *actuator_num_ptr;
	sensor_num = (actuator_num/NUM_ACTUATORS)*NUM_SENSORS + (actuator_num%NUM_ACTUATORS);
    
	timeTaskSemaphore[actuator_num] = xSemaphoreCreateBinary();

    for( ;; )
    {
        if( xSemaphoreTake( timeTaskSemaphore[actuator_num], period[actuator_num] ) == pdTRUE )
        {
			//Buzzer_On(); 
				portENTER_CRITICAL();
				{	
					LCD_Print(lcd_row_debug[actuator_num],lcd_col_debug[actuator_num],sensor_num,2);							// Print value of light on the LCD screen
				}
				portEXIT_CRITICAL();
			
			if((actuator_num%NUM_ACTUATORS) + 1<=NUM_SENSORS)	  //when NUM_ACTUATORS > NUM_SENSORS
			{
				
				vTaskSuspend(sensor_handler[sensor_num]);
				Actuator_On(actuator_num);
				
				vTaskDelay(taskTimings[actuator_num]/portTICK_PERIOD_MS);
				Actuator_Off(actuator_num);
				vTaskResume(sensor_handler[sensor_num]);   
			}
			else
			{
				Actuator_On(actuator_num);
				vTaskDelay(taskTimings[actuator_num]/portTICK_PERIOD_MS);
				Actuator_Off(actuator_num);
			}
			//taskTimings[actuator_num]=0;

        }
		else
		{
			/*Actuator_On(actuator_num);
			vTaskDelay(500/portTICK_PERIOD_MS);
			Actuator_Off(actuator_num);*/
			taskYIELD();
		}
			
    }
}
void UART1_SendByte(unsigned char data)
{  
   U1THR = data;				    
   while( (U1LSR&0x40)==0 );	    
}


void IRQ_UART1(void)
{  
	//static signed BaseType_t xHigherPriorityTaskWoken;
	unsigned char str[2];
	char* pch;
	

	int i=0, set_num;
	Temp = U1RBR;
	if(Temp == '~')
	{
		pch = strtok ((char*)UART_Str,",");
		while (pch != NULL && i<NUM_SETS*NUM_SENSORS)
		{
			if (atoi((const char*) pch) >= 0) thresholds[i++] = atoi((const char*)pch);
			pch = strtok (NULL, ",");
		
		}
				
		UART1_SendByte('~');
		memset (UART_Str,0,25);
	
	} else	if(Temp == '|') {
		//xHigherPriorityTaskWoken = pdFALSE;
		i=0;
		pch = strtok ((char*)UART_Str,",");
		while (pch != NULL && i<NUM_SETS*NUM_ACTUATORS)
		{
			if (atoi((const char*) pch) >= 0) taskTimings[i++] = atoi((const char*)pch);
			else taskTimings[i++] = 0; 
			pch = strtok (NULL, ",");
		
		}
				
		UART1_SendByte('|');
		memset (UART_Str,0,25);

		i=0;
		while(i<NUM_SETS*NUM_ACTUATORS)
		{
			if(taskTimings[i]>0)
				xSemaphoreGiveFromISR( timeTaskSemaphore[i], NULL );
			i++;
		}
		//portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
	else if (Temp == ':')
	{						
		set_num = atoi((const char*)UART_Str);
		set_state[set_num] = 1;
		i=0;
		while(i<NUM_SENSORS)
		{
			xSemaphoreGiveFromISR( sensorSemaphore[set_num*NUM_SENSORS+i], NULL );
			i++;
		}
		//BaseType_t xYieldRequired;
		//xTaskResumeFromISR(sensor_handler[sensor_num]);
		//xTaskResumeFromISR(water_handler[sensor_num]);

		//vTaskPrioritySetFromISR(sensor_handler[sensor_num], IDLE_TASK_PRIORITY + 1);
		//vTaskPrioritySetFromISR(water_handler[sensor_num], IDLE_TASK_PRIORITY + 1);
	   
		//vTaskResume(sensor_handler[sensor_num]);
		//vTaskResume(water_handler[sensor_num]);

		Buzzer_On();
		DelaymSec(1000);
		Buzzer_Off();

		UART1_SendByte(':');

		memset (UART_Str,0,25);

		//if (xYieldRequired == pdTRUE) portYIELD_FROM_ISR();
	}
	else if (Temp == '!')
	{
		set_num = atoi((const char*)UART_Str);
		set_state[set_num] = 0;

		//vTaskPrioritySetFromISR(sensor_handler[sensor_num], IDLE_TASK_PRIORITY - 1);
		//vTaskPrioritySetFromISR(water_handler[sensor_num], IDLE_TASK_PRIORITY - 1);

		//vTaskSuspend(sensor_handler[sensor_num]);
		//vTaskSuspend(water_handler[sensor_num]);

		UART1_SendByte('!');

		memset (UART_Str,0,25);
	}	
	else {
		str[0] = Temp;
		str[1] = 0;
		strncat((char *)UART_Str, (const char * )str, 1 );
		UART1_SendByte(Temp);
	}			
	
	VICVectAddr = 0x00;
}

void Init_UART1(void)
{  
   portENTER_CRITICAL()
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
	
//	   VICIntSelect = 0x00000000;		// IRQ
//	   VICVectCntl0 = 0x20|7;			// UART1
//	   VICVectAddr0 = ( unsigned long )vUART_ISREntry; 	//UART1 Vector Address
//	   VICIntEnable = (1<<7);	// Enable UART1 Rx interrupt

	   VICIntSelect &= ~(0x00000080);		// IRQ
	   VICVectCntl3 = 0x00000020|7;			// UART1
	   VICVectAddr3 = ( unsigned long )vUART_ISREntry; 	//UART1 Vector Address
	   //VICVectAddr0 = ( unsigned long )UART1_SendByte; 	//UART1 Vector Address
	   VICIntEnable = 0x00000080;	// Enable UART1 Rx interrupt
   }
   portEXIT_CRITICAL()
}


/***********************************/	                            
int main(void)
{
	unsigned int iter, inIter, *ptr;	
 	PINSEL2 = 0x00000000; 			//As it is not zero on reset
	//portENABLE_INTERRUPTS();
	Init_Buzzer_Pin();
	Buzzer_On();
	DelaymSec(200);
	Buzzer_Off();
	DelaymSec(40);					// Initializing the LCD
	Init();

	LCD_4Bit_Mode();
	LCD_Init();
	LCD_Command(0x01);
	DelaymSec(20);
	
	sensor_handler = (TaskHandle_t) pvPortMalloc(sizeof(TaskHandle_t)*NUM_SETS*NUM_SENSORS);
	time_task_handler = (TaskHandle_t) pvPortMalloc(sizeof(TaskHandle_t)*NUM_SETS*NUM_ACTUATORS);

	thresholds = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_SENSORS);
	taskTimings = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_ACTUATORS);

	default_thresholds = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SENSORS);
	default_thresholds[0] = 400;
	default_thresholds[1] = 60;

	sensorSemaphore = (SemaphoreHandle_t) pvPortMalloc(sizeof(SemaphoreHandle_t)*NUM_SETS*NUM_SENSORS);
	timeTaskSemaphore = (SemaphoreHandle_t) pvPortMalloc(sizeof(SemaphoreHandle_t)*NUM_SETS*NUM_ACTUATORS);
	
	sensor_ADC_channels = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_SENSORS);
	sensor_ADC_ports = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_SENSORS);

	set_state = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS);

	actuator_pins = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_ACTUATORS);
	actuator_ports = (unsigned int*) pvPortMalloc(sizeof(unsigned char)*NUM_SETS*NUM_ACTUATORS);
	
	lcd_row = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_SENSORS);
	lcd_col = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_SENSORS);

	lcd_row_debug = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_ACTUATORS);
	lcd_col_debug = (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_ACTUATORS);

	period =  (unsigned int*) pvPortMalloc(sizeof(unsigned int)*NUM_SETS*NUM_ACTUATORS);;

	actuator_pins[1]=17;   //left expansion 7
	actuator_ports[1]=0;
	PINSEL1 &= ~(1<<3 | 1<<2);
	IO0DIR |= 1<<actuator_pins[1];
	Actuator_Off(1);

	
	actuator_pins[0]=28;   //servo
	actuator_ports[0]=1;
	//PINSEL1 &= ~(1<<4 | 1<<5);
	IO1DIR |= 1<<actuator_pins[0];
	Actuator_Off(0);

	actuator_pins[2]=19;	//left expansion 10
	actuator_ports[2]=0;
	PINSEL1 &= ~(1<<6 | 1<<7);
	IO0DIR |= 1<<actuator_pins[2];
	Actuator_Off(2);

	sensor_ADC_channels[0] = 2;
	sensor_ADC_ports[0] = 0;
	sensor_ADC_channels[1] = 1;
	sensor_ADC_ports[1] = 0;

	lcd_row[0] = 1;
	lcd_col[0] = 3;
	lcd_row[1] = 1;
	lcd_col[1] = 8;

	/*lcd_row_debug[0] = 2;
	lcd_col_debug[0] = 1;
	lcd_row_debug[1] = 2;
	lcd_col_debug[1] = 4;
	lcd_row_debug[2] = 2;
	lcd_col_debug[2] = 8;*/

	/*actuator_pins[3]=8;   //left expansion 13
	actuator_ports[3]=0;
	PINSEL0 &= ~(1<<16 | 1<<17);
	IO0DIR |= 1<<actuator_pins[3];
	Actuator_Off(3);

	actuator_pins[4]=9;   //left expansion 11
	actuator_ports[4]=0;
	PINSEL0 &= ~(1<<18 | 1<<19);
	IO0DIR |= 1<<actuator_pins[4];
	Actuator_Off(4);  */

	actuator_pins[3]=2;	//left expansion 15
	actuator_ports[3]=0;
	PINSEL0 &= ~(1<<4 | 1<<5);
	IO0DIR |= 1<<actuator_pins[3];
	Actuator_Off(3);

	actuator_pins[4]=3;	//left expansion 16
	actuator_ports[4]=0;
	PINSEL0 &= ~(1<<6 | 1<<7);
	IO0DIR |= 1<<actuator_pins[4];
	Actuator_Off(4);

	actuator_pins[5]=28;	//servo
	actuator_ports[5]=1;
	//PINSEL1 &= ~(1<<6 | 1<<7);
	IO1DIR |= 1<<actuator_pins[5];
	Actuator_Off(5);

	sensor_ADC_channels[2] = 7;
	sensor_ADC_ports[2] = 0;
	sensor_ADC_channels[3] = 6;
	sensor_ADC_ports[3] = 0;

	lcd_row[2] = 2;
	lcd_col[2] = 3;
	lcd_row[3] = 2;
	lcd_col[3] = 8;

	for (iter = 0 ; iter < NUM_SETS ; iter++)
	{
		for(inIter =0; inIter<NUM_SENSORS;inIter++)
		{
			thresholds[NUM_SENSORS*iter+inIter] = default_thresholds[inIter];
		}
		for(inIter =0; inIter<NUM_ACTUATORS;inIter++)
		{
			taskTimings[NUM_ACTUATORS*iter+inIter] = 0;

		}
	
		set_state[iter] = 0;
	}
	
	period[2]=3000;	
	period[1]=LONG_TIME;
	period[0]=LONG_TIME;
	period[5]=3000;	
	period[4]=LONG_TIME;
	period[3]=LONG_TIME;

	//IO1DIR |= SIGNAL;
	//IO1CLR |= SIGNAL;

	for (iter = 0 ; iter < NUM_SETS ; iter++)
	{
		for(inIter = 0; inIter<NUM_SENSORS;inIter++)
		{
			ptr = (unsigned int*) pvPortMalloc(sizeof(unsigned int));
			*ptr = NUM_SENSORS*iter+inIter;
			xTaskCreate( vSensorTask, "Display Task", 100, (void*)ptr, tskIDLE_PRIORITY + 1, &sensor_handler[*ptr]);
		}
		//xTaskCreate( vATask, "Water Task", 100, (void*)ptr, tskIDLE_PRIORITY + 1, &water_handler[iter]);
	}
	for (iter = 0 ; iter < NUM_SETS ; iter++)
	{
		for(inIter = 0; inIter<NUM_ACTUATORS;inIter++)
		{
			ptr = (unsigned int*) pvPortMalloc(sizeof(unsigned int));
			*ptr = NUM_ACTUATORS*iter+inIter;
			xTaskCreate( vTimeTask, "Task", 100, (void*)ptr, tskIDLE_PRIORITY + 1, &time_task_handler[*ptr]);
		}
	}

	Init_UART1();
	UART1_SendByte('H');
	UART1_SendByte('i');
	UART1_SendByte('!');
	
	//portENABLE_INTERRUPTS();
    // Start the scheduler so the tasks start executing.  
    vTaskStartScheduler();     
	
    for( ;; ); 
	
}



