#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <math.h> //included to support power function
#include "lcd.c"

void port_init_lf();
void timer5_init();
void velocity(unsigned char, unsigned char);
void motors_delay();

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
//unsigned char flag = 0;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;
unsigned char row_found = 0, col_found = 0;
unsigned char row_found_arr[10];
unsigned char col_found_arr[10];
unsigned char tom_found = 0;
unsigned char end_reached = 0;
unsigned int flag = 0;
unsigned char MAX_ROW = 8;
unsigned char MAX_COLUMN = 8;

//Function to configure LCD port
void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//ADC pin configuration
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}

//Function to configure ports to enable robot's motion
void motion_pin_config_lf (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to Initialize PORTS
void port_init_lf()
{
	lcd_port_config();
	adc_pin_config();
	motion_pin_config_lf();	
}

// Timer 5 initialized in PWM mode for velocity control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
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
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//Function For ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

//Function To Print Sesor Values At Desired Row And Coloumn Location on LCD
void print_sensor(char row, char coloumn,unsigned char channel)
{
	
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}

//Function for velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 		// removing upper nibbel for the protection
 PortARestore = PORTA; 		// reading the PORTA original status
 PortARestore &= 0xF0; 		// making lower direction nibbel to 0
 PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
 PORTA = PortARestore; 		// executing the command
}

void forward (void) // Both wheels forward
{
  motion_set (0x06);
}


void backward(void)  //Both wheels Backward
{	
	motion_set(0x09);
}
void stop (void)     //Stops both the wheels
{
  motion_set (0x00);
}

void left (void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

void right (void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

void init_devices_lf (void)
{
 	cli(); //Clears the global interrupts
	port_init_lf();
	adc_init();
	timer5_init();
	sei();   //Enables the global interrupts
}


void move_dir(unsigned int steps, int dir)  // moves the bot given no of steps in the given direction
{
	if (steps==0)
		return;

	else
	{
		if (dir==1)          //dir value 1 for forward and 0 for backward
			forward();
		else
			backward();
		while(1)
		{

			Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
			Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
			Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

			//flag=0;

			print_sensor(1,1,3);	//Prints value of White Line Sensor1
			print_sensor(1,5,2);	//Prints Value of White Line Sensor2
			print_sensor(1,9,1);	//Prints Value of White Line Sensor3
		
		

			if(Left_white_line<0x19 && Center_white_line<0x19 && Right_white_line<0x19)  //www
			{
				if (dir==1)
					velocity(150,250);    //adjusting the bot to move left for forward direction
				else
					velocity(250,150);    //Doing the reverse for backward direction
			}

			else if(Left_white_line<0x19 && Center_white_line<0x19 && Right_white_line>0x19)  //wwb
			{
				if (dir==1)
					velocity(250,200);	  //Bot takes a slight right turn
				else
					velocity(200,250);
			}

		    else if(Left_white_line<0x19 && Center_white_line>0x19 && Right_white_line<0x19)  //wbw
			{
				velocity(250,250);       //Bot simply moves forward or backward based on motors spinning direction
			}
		
			else if(Left_white_line<0x19 && Center_white_line>0x19 && Right_white_line>0x19)  //wbb
			{
				if (dir==1)
					velocity(220,200);   //Bot moves slightly to right
				else
					velocity(200,220);
			}

			else if(Left_white_line>0x19 && Center_white_line<0x19 && Right_white_line<0x19)  //bww -- right
			{
				if (dir==1)
					velocity(200,250);	 //Bot moves towards left
				else
					velocity(250,200);
			}


			else if(Left_white_line>0x19 && Center_white_line<0x19 && Right_white_line>0x19)	 //bwb -- straight
			{
				velocity(250,250);   // Bot moves forward
			}

			else if(Left_white_line>0x19 && Center_white_line>0x19 && Right_white_line<0x19)  //bbw -- more right
			{
				if (dir==1)
					velocity(200,220);    //Bot moves slightly to left
				else
					velocity(220,200);
			}

			else if(Left_white_line>0x19 && Center_white_line>0x19 && Right_white_line>0x19)  //bbb
			{
				stop();               //completed one step
				velocity(0,0);
				_delay_ms(500);
				move_dir(steps-1, dir);  // decremented the no of steps in that direction by 1
				_delay_ms(500);
				break;
			}
		}
	}
}


void move_bot(unsigned int row, unsigned int col)   // Moves the bot to a particular row and column from starting position (0 0)
{
	
	if (row==0 && col==0)
		return;	

	if (col==0)
		move_dir(row,1);   //simply cover the rows
	else
	{
		move_dir(row,1);   // first cover the rows
		velocity(0,250);   // Takes left turn 
		_delay_ms(3000);
		move_dir(col,1);   // now covers the columns
			
	}
}


// Moves the bot from (row1 col1) to (row2 col2)

// While using these functions adjust the delay properly while taking turns
void move_bot_p2p(unsigned int row1, unsigned int col1, unsigned int row2, unsigned int col2)
{
	if (col1==col2)
	{
		if (row1<row2)
			move_dir(row2-row1,1);  // Simply cover the rows in the correct direction
		else if (row1>row2)
			move_dir(row1-row2,0);
	}
	
	else if (col1<col2)
	{
		if(row1<=row2)
		{
			if (row1==row2)
				move_dir(1,0);     //moves one step backward
			else if(row1 < row2-1) 
				move_dir(row2-row1-1,1); // moves diff minus 1 steps because some rows are dedicated to plants so u cant walk along that row so go to previous row and cover the columns
			_delay_ms(450);
			velocity(250,250);
			right();			// takes right turn 
			_delay_ms(550);
			move_dir(col2-col1,1);  // covers the columns
			_delay_ms(450);
			velocity(250,250);
			left();				// takes left turn 
			_delay_ms(550);
			move_dir(1,1);		// now takes the last step for covering rows
			
		}
		else
		{
			if(row1-1>row2)
				move_dir(row1-row2-1,0);  // moves diff minus 1 steps 
			_delay_ms(450);
			velocity(250,250);
			right();				// takes right turn
			_delay_ms(550);
			move_dir(col2-col1,1);    // covers columns
			_delay_ms(450);
			velocity(250,250);
			right();				// takes right turn
			_delay_ms(550);
			move_dir(1,1);           // takes the remaining step
		}
	}
	else
	{
		if(row1<=row2)
		{
			if (row1==row2)
				move_dir(1,0);     // moves one step backward
			else if(row1 < row2-1) 
				move_dir(row2-row1-1,1);  // covers diff minus 1 rows

			_delay_ms(450);
			velocity(250,250);
			left();                 // takes left turn
			_delay_ms(550);
			move_dir(col2-col1,1);   // Now covers the columns
			_delay_ms(450);
			velocity(250,250);
			right();				// takes right turn
			_delay_ms(550);
			move_dir(1,1);			// takes the last step for rows
		}
		else
		{
			if(row1-1>row2)
				move_dir(row1-row2-1,0);  // moves rows minus 1 steps 
			_delay_ms(450);
			velocity(250,250);
			left();                 // takes left turn
			_delay_ms(550);
			move_dir(col2-col1,1);    // covers columns
			_delay_ms(450);
			velocity(250,250);
			left();					// takes left turn
			_delay_ms(550);
			move_dir(1,1);			// takes the last step
		}
	}	
}



//Main Function
void line_follower()
{
	
	forward();
	while(1)
	{

		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

		//flag=0;

		print_sensor(1,1,3);	//Prints value of White Line Sensor1
		print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		print_sensor(1,9,1);	//Prints Value of White Line Sensor3
		
		

		if(Left_white_line<0x19 && Center_white_line<0x19 && Right_white_line<0x19)  //www
		{
			velocity(150,250);       // Bot moves more left
		}

		else if(Left_white_line<0x19 && Center_white_line<0x19 && Right_white_line>0x19)  //wwb -- left
		{
			velocity(250,200);      // Bot moves right
		}

	    else if(Left_white_line<0x19 && Center_white_line>0x19 && Right_white_line<0x19)  //wbw -- invalid
		{
			velocity(250,250);     // Bot moves in the same direction
		}
		
		else if(Left_white_line<0x19 && Center_white_line>0x19 && Right_white_line>0x19)  //wbb -- more left
		{
			velocity(220,200);     // Bot moves slightly towards right
		}

		else if(Left_white_line>0x19 && Center_white_line<0x19 && Right_white_line<0x19)  //bww -- right
		{
			velocity(200,250);    // Bot moves left
		}


		else if(Left_white_line>0x19 && Center_white_line<0x19 && Right_white_line>0x19)	 //bwb -- straight
		{
			velocity(250,250);    // Bot moves in the same direction
		}

		else if(Left_white_line>0x19 && Center_white_line>0x19 && Right_white_line<0x19)  //bbw -- more right
		{
			velocity(200,220);    // Bot moves slightly towards left
		}

		else if(Left_white_line>0x19 && Center_white_line>0x19 && Right_white_line>0x19)  //bbb
		{
			//_delay_ms(450);
			//left();
			//_delay_ms(550);			

			velocity(0,0);     // Reached the checkpoint 
			stop();
			_delay_ms(500);
			row_found++;		// Incrementing the row number

			if (row_found==MAX_ROW)
			{

				//velocity(250,250);
				//forward();
				//_delay_ms(250);
				//right();
				//_delay_ms(1350);
				//velocity(0,0);
				//move_dir(MAX_ROW-1,1);
				end_reached = 1;
				//_delay_ms(450);
				//velocity(250,250);
				//forward();
				//_delay_ms(450);
				//right();
				//_delay_ms(1350);
				velocity(0,0);
				//right();
				//_delay_ms(550);
				row_found=0;
				col_found++;

			}
/*
			if (row_found==MAX_ROW)
			{
				if (flag==0 || flag==1)
				{
					velocity(250,250);
					forward();
					_delay_ms(250);
					right();
					_delay_ms(550);
					velocity(0,0);
					_delay_ms(2000);
					forward();
					velocity(250,250);
					flag++;
					row_found--;
					continue;
				}

				else if (flag == 2)
				{
					velocity(250,250);
					move_dir(MAX_ROW-1,1);
					velocity(250,250);
					forward();
					_delay_ms(250);
					flag = 0;
					right();
					_delay_ms(1100);
					velocity(0,0);
					
				}
			}

*/
			UDR0 = 0x31;
			_delay_ms(500);
			break;
		}

		//_delay_ms(1);
	}
}

