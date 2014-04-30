/********************************************************************************
Written by Krishna Pillutla and Amit Panghal, IIT-Bomabay
 based on previous code Written by: Vinod Desai, NEX Robotics Pvt. Ltd. and
 Edited by: Sachitanand Malewar, NEX Robotics Pvt. Ltd.
 AVR Studio Version 4.17, Build 666

 Date: March 2014
 
 Part 1: Servo motor control using PWM
 Fire Bird V ATMEGA2560 microcontroller board has connection for 3 servo motors (S1, S2, S3).
 Servo motors move between 0 to 180 degrees proportional to the pulse train 
 with the on time of 0.6 to 2 ms with the frequency between 40 to 60 Hz. 50Hz is most recommended.

 We are using Timer 1 at 10 bit fast PWM mode to generate servo control waveform.
 In this mode servo motors can be controlled with the angular resolution of 1.86 degrees.
 Although angular resolution is less this is very simple method.
 
  
 Connection Details:	PORTB 5 OC1A --> Servo 1: Camera pod pan servo
 						PORTB 6 OC1B --> Servo 2: Camera pod tile servo
						PORTB 7 OC1C --> Servo 3: Reserved 
 					  	
 Note: 
 
 1. Make sure that in the configuration options following settings are 
 	done for proper operation of the code

 	Microcontroller: atmega2560
 	Frequency: 14745600
 	Optimization: -O0 (For more information read section: Selecting proper optimization 
 					options below figure 2.22 in the Software Manual)

 2. 5V supply to these motors is provided by separate low drop voltage regulator "5V Servo" which can
 	supply maximum of 800mA current. It is a good practice to move one servo at a time to reduce power surge 
	in the robot's supply lines. Also preferably take ADC readings while servo motor is not moving or stopped
	moving after giving desired position.




 Part 2: Robot control over serial port via XBee wireless communication module 
 					  (located on the ATMEGA260 microcontroller adaptor board)

 Serial Port used: UART0

  
 Connection Details:  	

  Serial Communication:	PORTD 2 --> RXD1 UART1 receive for RS232 serial communication
						PORTD 3 --> TXD1 UART1 transmit for RS232 serial communication

						PORTH 0 --> RXD2 UART 2 receive for USB - RS232 communication
						PORTH 1 --> TXD2 UART 2 transmit for USB - RS232 communication

						PORTE 0 --> RXD0 UART0 receive for ZigBee wireless communication
						PORTE 1 --> TXD0 UART0 transmit for ZigBee wireless communication

						PORTJ 0 --> RXD3 UART3 receive available on microcontroller expansion socket
						PORTJ 1 --> TXD3 UART3 transmit available on microcontroller expansion socket

Serial communication baud rate: 9600bps
To control robot use number pad of the keyboard which is located on the right hand side of the keyboard.
Make sure that NUM lock is on.


On the 4-axis robotic arm, ensure that the elbow joint of the arm is connected to servo 1 
and the gripper is connected to servo 2. Servo 3 has been unused and is a possible improvement.

Commands:
			Keyboard Key   ASCII value	Action
				8				0x38	Forward
				2				0x32	Backward
				4				0x34	Left
				6				0x36	Right
				5				0x35	Stop
				- 				0x00	Free servo motors
				-				0x07	Move servo 1 up by 1 degree
				-				0x09 	Move servo 1 down by 1 degree
				-				0x01	Open Gripper connected on servo 2
				-  				0x03 	Close Gripper connected on servo 2

		For finer control of the arm: if data is the ascii code received over Xbee:
		if(data >= 0x0A && data <= 0x5A)
			finer control of elbow. Move to angle (80 + data)
			(since the range of the arm is 90 deg to 160 deg)
		else if(data > 0x5A && data <= 0xA0)
			move gripper to angle specified by data: Range is 90 deg to 160 deg
		

				

Caution: 
1.  Auxiliary power can supply current up to 1 Ampere while Battery can supply current up to 
 	2 Ampere. When both motors of the robot changes direction suddenly without stopping, 
	it produces large current surge. When robot is powered by Auxiliary power which can supply
	only 1 Ampere of current, sudden direction change in both the motors will cause current 
	surge which can reset the microcontroller because of sudden fall in voltage. 
	It is a good practice to stop the motors for at least 0.5seconds before changing 
	the direction. This will also increase the useable time of the fully charged battery.
	the life of the motor. 
2.	If the servo motor is behaving in unexpected manners or the gripper is unable to grip objects
	that it usually grips, it is an indication that the battery may be low.

3.  LCD control can been used for debugging.

********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// Part 0: LCD Code
#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC

#define sbit(reg,bit)	reg |= (1<<bit)			// Macro defined for Setting a bit of any register.
#define cbit(reg,bit)	reg &= ~(1<<bit)		// Macro defined for Clearing a bit of any register.

void init_ports();
void lcd_reset();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_line1();
void lcd_line2();
void lcd_string(char*);

unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;

//Function to configure LCD port
void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//Function to Initialize PORTS
void port_init_lcd()
{
	lcd_port_config();
}

//Function to Reset LCD
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

	 
//Function to Write Command on LCD
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

//Function to Write Data on LCD
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


//Function to bring cursor at home position
void lcd_home()
{
	lcd_wr_command(0x80);
}


//Function to Print String on LCD
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

//Position the LCD cursor at "row", "column".

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

//Function To Print Any input value upto the desired digit on LCD
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

void init_devices_l (void)
{
 cli(); //Clears the global interrupts
 port_init_lcd();
 sei();   //Enables the global interrupts
}

//PART 1: Servo Motor Control

//Configure PORTB 5 pin for servo motor 1 operation
void servo1_pin_config (void)
{
 DDRB  = DDRB | 0x20;  //making PORTB 5 pin output
 PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
}

//Configure PORTB 6 pin for servo motor 2 operation
void servo2_pin_config (void)
{
 DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
 PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
}

//Configure PORTB 7 pin for servo motor 3 operation
void servo3_pin_config (void)
{
 DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
 PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
}

//Initialize the ports
void port_init(void)
{
 servo1_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
 servo2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation 
 servo3_pin_config(); //Configure PORTB 7 pin for servo motor 3 operation  
}

//TIMER1 initialization in 10 bit fast PWM mode  
//prescale:256
// WGM: 7) PWM 10bit fast, TOP=0x03FF
// actual value: 52.25Hz 
void timer1_init(void)
{
	 TCCR1B = 0x00; //stop
	 TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
	 TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
	 OCR1AH = 0x03;	//Output compare Register high value for servo 1
	 OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
	 OCR1BH = 0x03;	//Output compare Register high value for servo 2
	 OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
	 OCR1CH = 0x03;	//Output compare Register high value for servo 3
	 OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
	 ICR1H  = 0x03;	
	 ICR1L  = 0xFF;
	 TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
	 					For Overriding normal port functionality to OCRnA outputs.
					  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
	 TCCR1C = 0x00;
	 TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

//Function to rotate Servo 1 by a specified angle in the multiples of 1.86 degrees
void servo_1(unsigned char degrees)  
{
 float PositionPanServo = 0;
 PositionPanServo = ((float)degrees / 1.86) + 35.0;
 OCR1AH = 0x00;
 OCR1AL = (unsigned char) PositionPanServo;
}


//Function to rotate Servo 2 by a specified angle in the multiples of 1.86 degrees
void servo_2(unsigned char degrees)
{
 float PositionTiltServo = 0;
 PositionTiltServo = ((float)degrees / 1.86) + 35.0;
 OCR1BH = 0x00;
 OCR1BL = (unsigned char) PositionTiltServo;
}

//Function to rotate Servo 3 by a specified angle in the multiples of 1.86 degrees
void servo_3(unsigned char degrees)
{
 float PositionServo = 0;
 PositionServo = ((float)degrees / 1.86) + 35.0;
 OCR1CH = 0x00;
 OCR1CL = (unsigned char) PositionServo;
}

//servo_free functions unlocks the servo motors from the any angle 
//and make them free by giving 100% duty cycle at the PWM. This function can be used to 
//reduce the power consumption of the motor if it is holding load against the gravity.

void servo_1_free (void) //makes servo 1 free rotating
{
 OCR1AH = 0x03; 
 OCR1AL = 0xFF; //Servo 1 off
}

void servo_2_free (void) //makes servo 2 free rotating
{
 OCR1BH = 0x03;
 OCR1BL = 0xFF; //Servo 2 off
}

void servo_3_free (void) //makes servo 3 free rotating
{
 OCR1CH = 0x03;
 OCR1CL = 0xFF; //Servo 3 off
} 


unsigned char data; //to store received data from UDR1

void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

void motion_pin_config (void)
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to initialize ports
void port_init_motion()
{
	motion_pin_config();
	buzzer_pin_config();
}

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

//Function To Initialize UART0
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x5F; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}

//function prototypes for robotic arm control
//functions to move by 1 degree:
void move_servo_1_up(void);
void move_servo_1_down(void);
void move_servo_2_open(void);
void move_servo_2_close(void);
//functions to move to specified angle
void move_servo_1_up_1(int);
void move_servo_1_down_1(int);
void move_servo_2_open_1(int);
void move_servo_2_close_1(int);

//variables to maintain global position
unsigned char servo_1_pos, servo_2_pos;

SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	data = UDR0; 				//making copy of data from UDR0 in 'data' variable 

	UDR0 = data;	//echo data back to PC
	lcd_cursor(1,3);
	lcd_wr_char(data); //for debugging
	switch(data)
	{
		case 0x08: //ASCII value of 8
		
			PORTA=0x06;  //forward
			_delay_ms(150); 
			PORTA=0x00;	//stop
			break;
		

		case 0x02 : //ASCII value of 2

			PORTA=0x09; //back
			_delay_ms(150);
			PORTA=0x00;
			break;

		case 0x04: //ASCII value of 4
		
			PORTA=0x05;  //left
			_delay_ms(150);
			PORTA=0x00;
			break;

		case 0x06: //ASCII value of 6
		
			PORTA=0x0A; //right
			_delay_ms(150);
			PORTA=0x00;
		 	break;

		case 0x05: //ASCII value of 5
		
			PORTA=0x00; //stop
			break;
		
		//servo control
		case 0x00 : 
		
			servo_1_free();
			servo_2_free();
			servo_1_pos = 90;
			servo_2_pos = 100;
			break;

		case 0x07: 
		
			move_servo_1_up();
			break;	
		

		case 0x09: 
		
			move_servo_1_down();
			break;
		case 0x01: 
		
			move_servo_2_open();
			break;

		case 0x03: 
		
			move_servo_2_close();
			break;
		//finer control of robotic arm
		//see protocol described in comments above
		default : 
			if(data >= 0x0A && data <= 0x5A)
			{
				servo_1_pos = 80 + (int) data;
				servo_1(servo_1_pos);
			}
			else if(data > 0x5A && data <= 0xA0)
			{	
				servo_2_pos = (int) data;
				servo_2(servo_2_pos);
			}
	}


}

void move_servo_1_up(void)
{
	if(servo_1_pos <= 160 && servo_1_pos >=90)
	{	
		servo_1_pos+= 1;
		servo_1(servo_1_pos);
		_delay_ms(15);
	}	
}

void move_servo_1_down(void)
{
	if(servo_1_pos <= 160 && servo_1_pos >= 90)
	{	
		servo_1_pos-= 1;
		servo_1(servo_1_pos);
		_delay_ms(15);
	}	
}

void move_servo_2_open(void)
{
	if(servo_2_pos <= 150 && servo_2_pos >= 95)
	{
		if(servo_2_pos-1 <= 150 && servo_2_pos-1 >= 95)servo_2_pos-= 1;
		servo_2(servo_2_pos);
		_delay_ms(15);
	}	
}

void move_servo_2_close(void)
{
	if(servo_2_pos <= 150 && servo_2_pos >= 95)
	{
		if(servo_2_pos+1 <= 150 && servo_2_pos+1 >= 95)servo_2_pos+= 1;
		servo_2(servo_2_pos);
		_delay_ms(15);
	}	
}

void move_servo_1_down_1(int degrees)
{
	int i=0;
	while(i<degrees)
	{
		move_servo_1_down();
		_delay_ms(30);
		i--;
	}
}
void move_servo_1_up_1(int degrees)
{
	int i=0;
	while(i<degrees)
	{
		move_servo_1_up();
		_delay_ms(30);
		i--;
	}
}

void move_servo_2_open_1(int degrees)
{
	int i=0;
	while(i<degrees)
	{
		move_servo_2_open();
		_delay_ms(30);
		i--;
	}
}
void move_servo_2_close_1(int degrees)
{
	int i=0;
	while(i<degrees)
	{
		move_servo_2_close();
		_delay_ms(30);
		i--;
	}
}
//Function To Initialize all The Devices
void init_devices()
{
 cli(); //Clears the global interrupts
 port_init_motion();  //Initializes all the ports
 port_init_lcd();
 uart0_init(); //Initailize UART1 for serial communiaction
 port_init();
 timer1_init();
 sei();   //Enables the global interrupts
}




//Main function
void main(void)
{
	 unsigned char i = 0;
	 init_devices();
	 //init lcd
	 lcd_set_4bit();
	 lcd_init();
	 //bring robotic arm to default position
	 servo_1_pos = 90;
	 servo_1(servo_1_pos);
	 _delay_ms(100);
	 servo_2_pos = 150;
	 servo_2(servo_2_pos);

	 // Welcome gesture of gripper strarts here:
	 servo_2(100);
	 _delay_ms(100);
	 servo_2(150);
	 _delay_ms(100);
	 servo_2(100);
	 _delay_ms(100);
	 servo_2(150);
	 _delay_ms(100);
	 // Welcome gesture of gripper end

	 lcd_cursor(2, 1);
	 lcd_string("Hi");
 while(1);
}
