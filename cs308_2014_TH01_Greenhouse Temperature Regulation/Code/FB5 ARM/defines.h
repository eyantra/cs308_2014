#ifndef DEFINES_H
#define DEFINES_H

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

#define L_F				(1<<21)//LEFT_FORWARD
#define L_F_IO_0		(1<<2)//pin 21	LEFT_FORWARD_IO_0 GPIO(16-36) for pinsel2
#define L_F_IO_1		(1<<3)//pin 21	LEFT_FORWARD_IO_1 GPIO(16-36) for pinsel2
#define L_B				(1<<22)//LEFT_BACKWARD
#define L_B_IO_0		(1<<12)//pin 22	LEFT_BACKWARD_IO_0 GPIO for pinsel1
#define L_B_IO_1		(1<<13)//pin 22	LEFT_BACKWARD_IO_1 GPIO for pinsel1
#define R_F  			(1<<10)//RIGHT_FORWARD 
#define R_F_IO_0 		(1<<20)//pin 10	RIGHT_FORWARD_IO_0 GPIO for pinsel0
#define R_F_IO_1 		(1<<21)//pin 10	RIGHT_FORWARD_IO_1 GPIO for pinsel0
#define R_B  			(1<<11)//RIGHT_BACKWARD
#define R_B_IO_0  		(1<<22)//pin 11	RIGHT_BACKWARD_IO_0 GPIO for pinsel0
#define R_B_IO_1  		(1<<23)//pin 11	RIGHT_BACKWARD_IO_1 GPIO for pinsel0
#define L_E     		(1<<7) //LEFT_ENABLE  
#define L_E_IO_0     	(1<<14)//pin 7	LEFT_ENABLE_IO_0 GPIO for pinsel0
#define L_E_IO_1    	(1<<15)//pin 7	LEFT_ENABLE_IO_1 GPIO for pinsel0
#define R_E   			(1<<21)//RIGHT_ENABLE 
#define R_E_IO_0    	(1<<10)//pin 21	RIGHT_ENABLE_IO_0 GPIO for pinsel1
#define R_E_IO_1    	(1<<11)//pin 21	RIGHT_ENABLE_IO_1 GPIO for pinsel1

#define FAN_E			(1<<28)
#define FAN_IO			(1<<2)

#endif
