#include  <lpc214x.h>
#include "lcd.h"

void Init_LCD_Pin(void)
{								
 IO1DIR|= LCD_IO; 	// Set P1.17, P1.18, P1.19 P1.22, P1.23, P1.24, P1.25   as Output		
}

//This function writes the data on the data PORT using IOSET and IOCLR
void LCD_Write(unsigned int data4)
{
  IO1PIN&= ~DATA_BUS;
  IO1PIN|=(data4) << 22;
}
//This function sends commands to LCD
void LCD_Command(unsigned int data8)
{
 EN_LOW();
 COMMAND_PORT();
 WRITE_DATA();
 
 LCD_Write((data8&0xF0)>>4);
 
 EN_HI();
 DelaymSec(1);
 EN_LOW();
 
 LCD_Write((data8&0x0F));
 
 EN_HI();
 DelaymSec(1);
 EN_LOW();
 
} 

//This Functions switches LCD panel in 4-bit interfacing mode
void LCD_4Bit_Mode(void)
{
 EN_LOW();
 COMMAND_PORT();
 WRITE_DATA();

 LCD_Write(0x03);

 EN_HI();
 DelaymSec(5);
 EN_LOW();
 DelaymSec(5);

 LCD_Write(0x03);
 
 EN_HI();
 DelaymSec(5);
 EN_LOW();
 DelaymSec(5);

 LCD_Write(0x03);
 
 EN_HI();
 DelaymSec(5);
 EN_LOW();
 DelaymSec(5);

 LCD_Write(0x02);
 
 EN_HI();
 DelaymSec(5);
 EN_LOW();
 DelaymSec(5);

} 


//This function writes data on the LCD display
void LCD_Data(unsigned int data8)
{
 EN_LOW();
 DATA_PORT();
 WRITE_DATA();
 
 LCD_Write((data8&0xF0)>>4);

 EN_HI();
 DelaymSec(1);
 EN_LOW();
 
 LCD_Write((data8&0x0F));

 EN_HI();
 DelaymSec(1);
 EN_LOW();
}


//This function enables LCD's internal funcions
void LCD_Init(void)
{
 Init_LCD_Pin();
 LCD_4Bit_Mode();
 LCD_Command(0x28);
 LCD_Command(0x0C);
 LCD_Command(0x06);
 LCD_Command(0x01);
 DelaymSec(20);
}

//This function writes string on LCD display
void LCD_String(unsigned char *data)
{
 while(*data)
 {
  LCD_Data(*data);
  data++;
 } 
}  


//This Function puts LCD cursor to Home position
void LCD_Home(void)
{
 LCD_Command(0x80);
}

//This function sets cursor position
void LCD_Cursor(unsigned char Row,unsigned char Col)
{
 switch(Row)
 {
  case 1: LCD_Command(0x80+ Col -1);break;
  case 2: LCD_Command(0xC0+ Col -1);break;
  default: break;
 }
}


//This function displays any data upto 5 digits. It also requires row and column address
void LCD_Print(unsigned char Row, char Col,unsigned int Val, unsigned int Digits)
{
 unsigned char Flag=0;
 unsigned int Temp,Mi,Th,Hu,Te,Un=0;

 if(Row==0 || Col==0)
 {
  LCD_Home();
 }
 else
 {
  LCD_Cursor(Row,Col);
 }
 if(Digits==5 || Flag==1)
 {
  Mi=Val/10000+48;
  LCD_Data(Mi);
  Flag=1;
 }
 if(Digits==4 || Flag==1)
 {
  Temp = Val/1000;
  Th = (Temp % 10) + 48;
  LCD_Data(Th);
  Flag=1;
 }
 if(Digits==3 || Flag==1)
 {
  Temp = Val/100;
  Hu = (Temp%10) + 48;
  LCD_Data(Hu);
  Flag=1;
 }
 if(Digits==2 || Flag==1)
 {
  Temp = Val/10;
  Te = (Temp%10) + 48;
  LCD_Data(Te);
  Flag=1;
 }
 if(Digits==1 || Flag==1)
 {
  Un = (Val%10) + 48;
  LCD_Data(Un);
 }
 if(Digits>5)
 {
  LCD_Data('E');
 }
	
}

