#ifndef DEFINES_H
#define DEFINES_H
/**********defines******************/
#define BUZZER_IO_0 (1<<18)	  //Pin 18 in PINSEL1 is used to set P0.25 as GPIO 
#define BUZZER_IO_1 (1<<19)	  //Pin 19 in PINSEL1 is used to set P0.25 as GPIO 
#define BUZZER  (1<<25)				  //Buzzer connected to P0.25

#define BAT_VOL_IO_0 1<<26			// battery Voltage
#define BAT_VOL_IO_1 1<<27 		 	// pin 0.13 
#define BAT_VOL 1<<13

#define WH_LINE_SEN_1_IO_0 1<<24 	// White line sensor 1
#define WH_LINE_SEN_1_IO_1 1<<25 	// pin 0.12
#define WH_LINE_SEN_1 1<<12
 
#define WH_LINE_SEN_2_IO_0 1<<24 	// White line sensor 2
#define WH_LINE_SEN_2_IO_1 1<<25 	// pin 0.28
#define WH_LINE_SEN_2 1<<28

#define WH_LINE_SEN_3_IO_0 1<<26 	// White line sensor 3
#define WH_LINE_SEN_3_IO_1 1<<27 	// pin 0.29
#define WH_LINE_SEN_3 1<<29

#define SHR_IR_SEN_2_IO_0 1<<8  	// Sharp IR range sensor 2
#define SHR_IR_SEN_2_IO_1 1<<9		//	pin 0.4
#define SHR_IR_SEN_2 1<<4

#define SHR_IR_SEN_3_IO_0 1<<12 	// Sharp IR range sensor 3
#define SHR_IR_SEN_3_IO_1 1<<13		//	pin 0.6
#define SHR_IR_SEN_3 1<<6

#define SHR_IR_SEN_4_IO_0 1<<10 	// Sharp IR range sensor 4
#define SHR_IR_SEN_4_IO_1 1<<11		//	pin 0.5
#define SHR_IR_SEN_4 1<<5

#define REST 1700.0

/***************Macros*********************/ 
#define Fosc            12000000                    //10MHz~25MHz
#define Fcclk           (Fosc * 5)                  //Fosc(1~32)<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO Fcclk 2񅦰�16�156MHz~320MHz
#define Fpclk           (Fcclk / 1) * 1             //VPB(Fcclk / 1) 1񄿔
#define UART_BPS	38400 		//Change Baud Rate Setting here

#define SIGNAL (1<<28) 
/***********************************/
#endif  
