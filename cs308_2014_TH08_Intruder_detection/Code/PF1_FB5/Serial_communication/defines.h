
#ifndef DEFINES_H
#define DEFINES_H
/**********Macros******************/
#define BUZZER_OFF()    IO0CLR=(1<<25)		   //Macro to turn OFF buzzer
#define BUZZER_ON()     IO0SET=(1<<25)		   //Macro to turn ON buzzer
/***********************************/


#define L_F				 		(1<<21)//LEFT_FORWARD
#define L_F_IO_0			(1<<2)//pin 21	LEFT_FORWARD_IO_0 GPIO(16-36) for pinsel2
#define L_F_IO_1			(1<<3)//pin 21	LEFT_FORWARD_IO_1 GPIO(16-36) for pinsel2
#define L_B						(1<<22)//LEFT_BACKWARD
#define L_B_IO_0			(1<<12)//pin 22	LEFT_BACKWARD_IO_0 GPIO for pinsel1
#define L_B_IO_1			(1<<13)//pin 22	LEFT_BACKWARD_IO_1 GPIO for pinsel1
#define R_F  					(1<<10)//RIGHT_FORWARD 
#define R_F_IO_0 			(1<<20)//pin 10	RIGHT_FORWARD_IO_0 GPIO for pinsel0
#define R_F_IO_1 			(1<<21)//pin 10	RIGHT_FORWARD_IO_1 GPIO for pinsel0
#define R_B  					(1<<11)//RIGHT_BACKWARD
#define R_B_IO_0  		(1<<22)//pin 11	RIGHT_BACKWARD_IO_0 GPIO for pinsel0
#define R_B_IO_1  		(1<<23)//pin 11	RIGHT_BACKWARD_IO_1 GPIO for pinsel0
#define L_E     			(1<<7) //LEFT_ENABLE  
#define L_E_IO_0     	(1<<14)//pin 7	LEFT_ENABLE_IO_0 GPIO for pinsel0
#define L_E_IO_1    	(1<<15)//pin 7	LEFT_ENABLE_IO_1 GPIO for pinsel0
#define R_E   				(1<<21)//RIGHT_ENABLE 
#define R_E_IO_0    	(1<<10)//pin 21	RIGHT_ENABLE_IO_0 GPIO for pinsel1
#define R_E_IO_1    	(1<<11)//pin 21	RIGHT_ENABLE_IO_1 GPIO for pinsel1
#define EINT2_E_0  		(1<<30)//EINT2 enable for PINSEL0
#define EINT2_E_1  		(1<<31)//EINT2 enable for PINSEL0
#define EINT0_E_0 		(1<<0)//EINT0 enable for PINSEL1
#define EINT0_E_1 		(1<<1)//EINT0 enable for PINSEL1

#endif
