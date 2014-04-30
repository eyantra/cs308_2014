#ifndef LCD_H
#define LCD_H
#include  <lpc214x.h>

#define RS  1<<19
#define RW  1<<18
#define	EN  1<<17
#define DB7 1<<25
#define DB6 1<<24
#define DB5 1<<23
#define DB4 1<<22

#define DATA_BUS ((DB7)|(DB6)|(DB5)|(DB4))
#define LCD_IO ((RS)|(RW)|(EN)| DATA_BUS)	
	

#define DATA_PORT() IO1SET=RS	
#define READ_DATA() IO1SET=RW
#define EN_HI() IO1SET=EN

#define COMMAND_PORT() IO1CLR=RS	
#define WRITE_DATA() IO1CLR=RW
#define EN_LOW() IO1CLR=EN
/***********Prototypes****************/

void Init_LCD_Pin(void);
void DelaymSec(unsigned int j);
void LCD_Write(unsigned int data4);
void LCD_Command(unsigned int data);
void LCD_4Bit_Mode(void);
void LCD_Init(void);
void LCD_String(unsigned char *data);
void LCD_Home(void);
void LCD_Cursor(unsigned char Row,unsigned char Col);
void LCD_Print(unsigned char Row, char Col,unsigned int Val, unsigned int Digits);
/**************************************/
#endif 
