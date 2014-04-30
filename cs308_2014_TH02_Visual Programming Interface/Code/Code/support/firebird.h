/********************************************************************************
Written by: AKSHAR/SHWETA ERTS LAB. IIT BOmbay
 AVR Studio Version 4.17, Build 666

 Date: 13th Sept. 2010
 
 This JAVA file contains code to set-up the IDE and convert the visual program to C program.


********************/
/**************************

   Copyright (c) 2010, ERTS LAB CSE IIT-Bombay            -*- c -*-
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <inttypes.h>


void init_devices_xbee();
/*//////////////////////////////////////////////////////////////////////// */
//buzzer on and off functions
void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}

// motion functions


void motion_pin_config (void)
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to initialize ports
void init_devices()
{
 cli();
 motion_pin_config();
 port_init();
 init_adc();
 timer5_init();
 lcd_port_config();
 lcd_set_4bit();
 lcd_init();
 //init_lcd();
 init_buzzer();
 init_ir();      
 uart0_init();
 sei();
}



void uart0_init(void) 
{ 
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x5F; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
} 


//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 			// removing upper nibbel as it is not needed
 PortARestore = PORTA; 			// reading the PORTA's original status
 PortARestore &= 0xF0; 			// setting lower direction nibbel to 0
 PortARestore |= Direction; 	// adding lower nibbel for direction command and restoring the PORTA status
 PORTA = PortARestore; 			// setting the command to the port

}


void MOVE_FWD(void) //both wheels forward
{
  motion_set(0x06);
}

void MOVE_REV(void) //both wheels backward
{
  motion_set(0x09);
}

void MOVE_LEFT(void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

void MOVE_RIGHT(void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

void MOVE_SOFT_LEFT (void) //Left wheel stationary, Right wheel forward
{
 motion_set(0x04);
}

void MOVE_SOFT_RIGHT (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

void MOVE_INPLACE_LEFT(void) //Left wheel backward, right wheel stationary
{
 motion_set(0x01);
}

void MOVE_INPLACE_RIGHT (void) //Left wheel stationary, Right wheel backward
{
 motion_set(0x08);
}

void STOP (void)
{
  motion_set(0x00);
}

// Timer 5 initialised in PWM mode for velocity control
// Prescale:64
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:674.988Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionalit to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

// Function for robot velocity control
// Make sure that you pass only unsigned char value.


void velocity (unsigned char left_motor, unsigned char right_motor)
{
	PORTA=0x06;
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}


void MOTOR_LEFT_SPEED(int val)
{
    if ( val > 254 )   //limiting the max velocity
	   val = 255;	
    OCR5AL = (unsigned char) val;
}

void MOTOR_RIGHT_SPEED(int val)
{
  if ( val > 254 )     //limiting the max velocity
	  val = 255;
  OCR5BL = (unsigned char) val;
}



/*   ////////////////////////////////////////////////          */

//  whiteline sensing



#define LIGHT_LEFT ADC_Conversion(3)
#define LIGHT_MIDDLE ADC_Conversion(2)
#define LIGHT_RIGHT ADC_Conversion(1)

#define FRONT_IR front_dist_mm()




#define FCPU 11059200ul //defined here to make sure that program works properly

void port_init();
void timer5_init();
void velocity(unsigned char, unsigned char);
void motors_delay();

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char flag = 0;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;



//ADC pin configuration
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}


void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}



//Function to Initialize PORTS
void port_init()
{
	ultrasonic_trigger_config();
	adc_pin_config();
}

void init_adc()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//Function For ADC Conversion
unsigned int ADC_Conversion(unsigned char Ch)
{
 //unsigned char a;
 unsigned int a = 0,b = 0;
 
 if(Ch>7)
  {
	ADCSRB = 0x08;
  }

 Ch = Ch & 0x07;  			
 ADMUX= 0x20| Ch;	   		
 ADCSRA = ADCSRA | 0x40;	    //Set start conversion bit
 while((ADCSRA&0x10)==0);	    //Wait for ADC conversion to complete
 b = (unsigned int)(ADCL>>6);   //read & adjust ADCL result to read as a right adjusted result
 a = (unsigned int)(ADCH<<2);   //read & adjust ADCH result to read as a right adjusted result
 a = a | b;                      
 ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
 ADCSRB = 0x00;
 return a;
}


/***********************************************************************************
 This Function calculates the actual distance in millimeters(mm) from the input
 analog value of Sharp Sensor. 
 ***********************************************************************************/

/***** Function to Initialize PORTS *****/
void init_ir()
{
	DDRF = 0x00; 
	PORTF = 0x00;
	DDRA = 0x0F;
	PORTA = 0x00;
	DDRL = 0x18;
	PORTL = 0x18;
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRJ = 0x00;
	PORTJ = 0xFF;
}


unsigned int Sharp_estimation(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
}


int front_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(11);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}


/*     ////////////////////////////////////////////////////////////////////////////////////////////////////////////   */

// lcd functions


#define FCPU 11059200ul
#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC

#define sbit(reg,bit)	reg |= (1<<bit)
#define cbit(reg,bit)	reg &= ~(1<<bit)


int ultraSonicRangeSensor();
void ultraSonicRangeSensorLCD(char row, char col);
void ultrasonic_trigger_config();
void adc_pin_config ();
void lcd_port_config ();
void ultrasonic_trigger();
void init_ports();
void lcd_reset_4bit();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_home();
void lcd_cursor(char, char);
void lcd_print(char, char, unsigned int, int);
void lcd_string(char*);
void print_sensor(char row, char coloumn,unsigned char channel);

unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;

int i;

//Function to configure LCD port
void lcd_port_config()
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

void init_lcd()
{
	DDRF = 0x00; 
	PORTF = 0x00;
	DDRA = 0x0F;
	PORTA = 0x00;
	DDRL = 0x18;
	PORTL = 0x18;
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRJ = 0x00;
	PORTJ = 0xFF;
}

/*****Function to Reset LCD*****/
void lcd_set_4bit()
{
	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(1);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	
}

/*****Function to Initialize LCD*****/
//Function to Initialize LCD
void lcd_init()
{
	_delay_ms(1);

	lcd_wr_command(0x28);			//LCD 4-bit mode and 2 lines.
	lcd_wr_command(0x01);
	lcd_wr_command(0x06);
	lcd_wr_command(0x0E);
	lcd_wr_command(0x80);
		
}
	 
/*****Function to Write Command on LCD*****/
void lcd_wr_command(unsigned char cmd)
{
	unsigned char temp;
	temp = cmd;
	temp = temp & 0xF0;
	lcd_port &= 0x0F;
	lcd_port |= temp;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	lcd_port &= 0x0F;
	lcd_port |= cmd;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}

/*****Function to Write Data on LCD*****/
void lcd_wr_char(char letter)
{
	char temp;
	temp = letter;
	temp = (temp & 0xF0);
	lcd_port &= 0x0F;
	lcd_port |= temp;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	lcd_port &= 0x0F;
	lcd_port |= letter;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}


void lcd_home()
{
	lcd_wr_command(0x80);
}


/*****Function to Print String on LCD*****/
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

/*** Position the LCD cursor at "row", "column". ***/

void lcd_cursor (char row, char column)
{
	switch (row) {
		case 1: lcd_wr_command (0x80 + column - 1); break;
		case 2: lcd_wr_command (0xc0 + column - 1); break;
		case 3: lcd_wr_command (0x94 + column - 1); break;
		case 4: lcd_wr_command (0xd4 + column - 1); break;
		default: break;
	}
}

/***** Function To Print Any input value upto the desired digit on LCD *****/
void lcd_print (char row, char coloumn, unsigned int value, int digits)
{
	unsigned char flag=0;
	if(row==0||coloumn==0)
	{
		lcd_home();
	}
	else
	{
		lcd_cursor(row,coloumn);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		lcd_wr_char(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcd_wr_char(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcd_wr_char(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcd_wr_char(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcd_wr_char(unit);
	}
	if(digits>5)
	{
		lcd_wr_char('E');
	}
	
}


// This Function prints the Analog Value Of Corresponding Channel No. at required Row
// and Coloumn Location. 
void print_sensor(char row, char coloumn,unsigned char channel)
{
	unsigned char ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}

void LCD_DISPLAY_INT_1(int val)
{


 lcd_print(1,1,val,4);

}

/* reverse: reverse string s in place */
void reverse(char s[])
{
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
void itoa1(int n, char s[])
{
    int i, sign;
    if ((sign = n) < 0) /* record sign */
         n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
         s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}


void LCD_DISPLAY_INT_2(int val)
{	

	lcd_print(2,1,val,4);

}


/*  ////////////////////////////////////////////    */

// BUZZER Functions

/*****Function to Initialize PORTS*****/
void init_buzzer()
{
	DDRC = DDRC | 0x08;				//Setting PORTC pin3 as outpt
	PORTC = 0x00;				//Initialize Values of PORTC to 0.
}

/***** Buzzer Delay Function*****/
void buzzer_delay()
{
	unsigned int i;
	for(i=0; i<10;i++)
	{
		_delay_us(100);
	}
}

void BUZZER_ON()
{

int i=0; 
   unsigned char port_C_copy;
   port_C_copy = PORTC;
   port_C_copy = port_C_copy | 0x08;
   PORTC = port_C_copy;	
   //for(i=0;i<10;i++)
	//tick_timer();
	buzzer_delay();
   port_C_copy = PORTC;
   port_C_copy = port_C_copy & 0xF7;
   PORTC = port_C_copy;	

}


void BUZZER_OFF()
{

PORTC = 0x00;			//Clear pin3 to turn the "buzzer OFF"
buzzer_delay();			//delay

}





/***************************FIREBIRD.H***************************/

void move_forward(int val)
{    PORTA=0x06;
	 MOTOR_RIGHT_SPEED(val);
	 MOTOR_LEFT_SPEED(val);
}

void move_forward_right(int val)
{	PORTA=0x06;
	MOTOR_RIGHT_SPEED(0);
	MOTOR_LEFT_SPEED(val);
}

void move_forward_left(int val)
{	PORTA=0x06;	
	MOTOR_RIGHT_SPEED(val);
	MOTOR_LEFT_SPEED(0);
}

void move_backward(int val)
{	PORTA=0x09;
	MOTOR_RIGHT_SPEED(val);
	MOTOR_LEFT_SPEED(val);
	
}

void buzzer(int val)
{
	BUZZER_ON();
	_delay_ms(val);
	BUZZER_OFF();
	
}

int front_IR()
{
	return front_dist_mm();
}

int white_line_left()
{
	return LIGHT_LEFT ;
}

int white_line_right()
{
	return LIGHT_RIGHT;
}

int white_line_middle()
{
	return LIGHT_MIDDLE ;
}


/*void init_devices_xbee()
{
 cli(); //Clears the global interrupts
 //port_init();  //Initializes all the ports
 motion_pin_config();
 buzzer_pin_config();
 //adc_pin_config();
 uart0_init(); //Initailize UART1 for serial communiaction
 sei();   //Enables the global interrupts
}*/

////////////////////UltraSonic Range Sensor

void ultrasonic_trigger()                 
{
 PORTB = PORTB | 0x10;  // make high the Trigger input for Ultrasonic sensor
 _delay_us(50);         // Wait for >20usec
 PORTB = PORTB & 0xEF;  // make low the Trigger input for Ultrasonic sensor
}

// ultarasonic trigger configuration
void ultrasonic_trigger_config()
{
 DDRB = DDRB | 0x10;   // PB4 direction set as output
 PORTB = PORTB & 0x00; // PB4 set to logic 0 
}

int ultraSonicRangeSensor(){
	 ultrasonic_trigger();            // call ultrasonic triggering after enery 150msec  
     _delay_ms(150);
	 float distance_in_cm =0;
	 unsigned int ADC_Value =0;
	 unsigned in distance_in_cm_int = 0;
	 ADC_Value = ADC_Conversion(15);
 
     distance_in_cm = ADC_Value * 1.268;  // where, 5V/1024 = 0.00488/step & 9.85mV/2.54cm = 0.00385mV/cm
                                        // for distance in cm, we get 0.00488/0.00385 = 1.267 as const multiplier
	 distance_in_cm_int = floor(distance_in_cm);
	 return distance_in_cm_int;
}

void ultraSonicRangeSensorLCD(char row, char col){
	 lcd_set_4bit();                  // initialise LCD
	 lcd_init(); 
	 ultrasonic_trigger();            // call ultrasonic triggering after enery 150msec  
     _delay_ms(150);
	 float distance_in_cm =0;
	 unsigned int ADC_Value =0;
	 unsigned in distance_in_cm_int = 0;
	 ADC_Value = ADC_Conversion(15);
 
     distance_in_cm = ADC_Value * 1.268;  // where, 5V/1024 = 0.00488/step & 9.85mV/2.54cm = 0.00385mV/cm
                                        // for distance in cm, we get 0.00488/0.00385 = 1.267 as const multiplier
	 distance_in_cm_int = floor(distance_in_cm);
	 lcd_print(row,col, distance_in_cm_int, 3);
}
