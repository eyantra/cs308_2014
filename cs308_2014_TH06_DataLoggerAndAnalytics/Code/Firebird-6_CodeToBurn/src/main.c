/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "type.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

#include <stdlib.h>
uint32_t encCount=0;

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

// TODO: insert other include files here
#include "HardwareDefinition.h"
#include "FB6Lib.h"

// TODO: insert other definitions and declarations here
/******************Global Variables****************************/

/**************************************************************/

/******************Function Prototypes****************************/
void InitPeripherals(void);
void Delay(void);
void Delay1s(void);
void Delay100ms(void);
/*****************************************************************/


/*===============================================================================
 Name        	 : InitPeripherals()
 Parameters		 : None
 Description 	 : This function initializes the peripherals of LPC1769 microcontroller
 	 	 	 	   and modules of Fire Bird VI robot as per the definitions in
 	 	 	 	   Hardwareprofile.h
 Preconditions	 : None
===============================================================================*/
void InitPeripherals(void)
{
	// delay to allow motor controller to initialize
	Delay100ms();

	UARTInit(2, 115200);
	InitMotorController();
	Stop();

	ResetI2C0();
	I2C0Init();
	AD7998_WriteReg(AD7998_CONFIG,0x0FF8);		//Convert Channel 1, Filter On
	ResetI2C1();
	I2C1Init();

	// delay to allow LCD to initialize
	Delay1s();
	InitLCD();
}


/*===============================================================================
 Name        	 : Delay();
 Parameters		 : None
 Description 	 : Generates delay of very small amount
 Preconditions	 : None
===============================================================================*/
void Delay(void)
{
	uint32_t i=0;
	//for(i=0;i<10000;i++);
	for(i=0;i<100;i++);
}

/*===============================================================================
 Name        	 : Delay1s();
 Parameters		 : None
 Description 	 : Generates delay of approximately 1 Second
 Preconditions	 : None
===============================================================================*/
void Delay1s(void)
{
	volatile uint32_t i=0;
	volatile uint32_t k=0;
	volatile uint32_t j=0;
	for(k=0;k<110;k++)
	{
		for(i=0;i<60000;i++)
		{
			j++;
		}
	}
}


/*===============================================================================
 Name        	 : Delay100ms();
 Parameters		 : None
 Description 	 : Generates delay of approximately 100 milliseconds
 Preconditions	 : None
===============================================================================*/
void Delay100ms(void)
{
	volatile uint32_t i=0;
	volatile uint32_t k=0;
	volatile uint32_t j=0;
	for(k=0;k<11;k++)
	{
		for(i=0;i<60000;i++)
		{
			j++;
		}
	}
}

void ReadingVal(uint32_t Number){

			uint8_t Digits = 4;
			uint32_t Status = FALSE;
			uint32_t Temp=0;
			uint8_t String[10];
			uint8_t Flag=0;
			uint8_t count=0;

				if(Digits>5)
					return(Status);
				if(Digits==5 || Flag==1)
				{
					String[count++] = Number/10000 + 48;
					Flag=1;
				}
				if(Digits==4 || Flag==1)
				{
					Temp = Number/1000;
					String[count++] = (Temp % 10) + 48;
					Flag=1;
				}
				if(Digits==3 || Flag==1)
				{
					Temp = Number/100;
					String[count++] = (Temp % 10) + 48;
					Flag=1;
				}
				if(Digits==2 || Flag==1)
				{
					Temp = Number/10;
					String[count++] = (Temp % 10) + 48;
					Flag=1;
				}
				if(Digits==1 || Flag==1)
				{
					String[count++] = (Number%10) + 48;
					Flag = 0;
				}

			UARTSend(1,String,4);
		}

int main(void) {

	// TODO: insert code here
	//uint32_t Tick=0;
	//uint32_t Temp=0;
	SystemInit();			/*Inits PLL and Peripheral Clocks*/	//Core Clock(CCLK) = 120MHz, All Peripheral Clock = 1/4 * CCLK
	SystemClockUpdate();	/* SystemClockUpdate() updates the SystemFrequency variable */
	InitPeripherals();
	ADCInit(ADC_CLK);
	Delay1s();
    UARTInit(0,9600);
    UARTInit(1,19200);
	LCDSetCursorPosition(1,1);
	LCD_WriteStr(20,(uint8_t *)"    I2C ADC DEMO    ");


		// set mode to position control
		SetMode(2);

		ClearEncoderCounts();
		SetAcceleration(10);
		DisableSafety();
		Delay1s();


	uint8_t String1[10] = "@";
	uint8_t String2[10] = ",";
	uint8_t String3[10] = "$";
	//uint8_t String4[10] = "ABCD";
	//UARTSend(0,String1,2);
	//UARTSend(1,String1,2);
	//uint32_t Tick=0;
	int flag_alternate = 1;
		while(1)
		{
			//Get_AD7998_Data(AD7998ADC);
			if (flag_alternate == 0){
				MoveForwardDistanceMM(100,1000);
				encCount = abs(GetRightMotorCount());
				LCD_PrintData(2,1,encCount,4);
				Delay1s();
				Delay1s();
				Delay1s();
				Delay1s();
				Delay1s();
				Delay1s();
				Delay1s();
				flag_alternate =1;
				//Delay1s();
			}
			else {
				//Tick++;
				//if(Tick==100)

						uint32_t Number1 = ADCRead(0);
						uint32_t Number2 = ADCRead(1);
						uint32_t Number3 = ADCRead(2);
						uint32_t Number4 = ADCRead(5);

						UARTSend(1,String1,1);
						ReadingVal(Number1);
			UARTSend(1,String2,1);
			ReadingVal(Number2);
			UARTSend(1,String2,1);
			ReadingVal(Number3);
			UARTSend(1,String2,1);
			ReadingVal(Number4);
			UARTSend(1,String3,1);
			//Tick = 0;
			Delay1s();
			Delay1s();
			Delay1s();
			Delay1s();
				flag_alternate = 0;

		}
			Delay1s();
			Delay1s();
			Delay1s();
			Delay1s();
			}
	//}

	// Enter an infinite loop
//	while(1)
//	{
//		//Get_AD7998_Data(AD7998ADC);
//		Tick++;
//		if(Tick==100)
//		{
//			//uint8_t *ADC_buffer;
//			//ADC_buffer = AD7998ADC;
//
////			Temp = AD7998ADC[0];
////			Temp|= ((uint32_t) (AD7998ADC[1] & 0x0F)) << 8;
////			LCD_PrintData(3,1,Temp,4);
////
////			Temp = AD7998ADC[2];
////			Temp|= ((uint32_t) (AD7998ADC[3] & 0x0F)) << 8;
////			LCD_PrintData(3,6,Temp,4);
////
////			Temp = AD7998ADC[4];
////			Temp|= ((uint32_t) (AD7998ADC[5] & 0x0F)) << 8;
////			LCD_PrintData(3,11,Temp,4);
////
////			Temp = AD7998ADC[6];
////			Temp|= ((uint32_t) (AD7998ADC[7] & 0x0F)) << 8;
////			LCD_PrintData(3,16,Temp,4);
////
////			Temp = AD7998ADC[8];
////			Temp|= ((uint32_t) (AD7998ADC[9] & 0x0F)) << 8;
////			LCD_PrintData(4,1,Temp,4);
////
////			Temp = AD7998ADC[10];
////			Temp|= ((uint32_t) (AD7998ADC[11] & 0x0F)) << 8;
////			LCD_PrintData(4,6,Temp,4);
////
////			Temp = AD7998ADC[12];
////			Temp|= ((uint32_t) (AD7998ADC[13] & 0x0F)) << 8;
////			LCD_PrintData(4,11,Temp,4);
////
////			Temp = AD7998ADC[14];
////			Temp|= ((uint32_t) (AD7998ADC[15] & 0x0F)) << 8;
////			LCD_PrintData(4,16,Temp,4);
////
//			uint32_t Number1 = ADCRead(0);
//			uint32_t Number2 = ADCRead(1);
//			uint32_t Number3 = ADCRead(2);
//			uint32_t Number4 = ADCRead(5);
//			//uint8_t String[10];
//			//uint32_t Number = 1234;
//			uint8_t Digits = 4;
//			uint32_t Status = FALSE;
//			uint32_t Temp=0;
//			uint8_t String[10];
//			uint8_t Flag=0;
//			uint8_t count=0;
//			UARTSend(1,String1,3);
//				if(Digits>5)
//					return(Status);
//				if(Digits==5 || Flag==1)
//				{
//					String[count++] = Number/10000 + 48;
//					Flag=1;
//				}
//				if(Digits==4 || Flag==1)
//				{
//					Temp = Number/1000;
//					String[count++] = (Temp % 10) + 48;
//					Flag=1;
//				}
//				if(Digits==3 || Flag==1)
//				{
//					Temp = Number/100;
//					String[count++] = (Temp % 10) + 48;
//					Flag=1;
//				}
//				if(Digits==2 || Flag==1)
//				{
//					Temp = Number/10;
//					String[count++] = (Temp % 10) + 48;
//					Flag=1;
//				}
//				if(Digits==1 || Flag==1)
//				{
//					String[count++] = (Number%10) + 48;
//					Flag = 0;
//				}
//			Tick = 0;
//			UARTSend(1,String,4);
//		}
//	}
	return 0 ;
}
