/**
 *	@file util.c
 *	@brief utility functions for the Atmel platform
 * 
 * 	For an overview of how timer based interrupts work, see
 * 	page 111 and 133-137 of the Atmel Mega128 User Guide
 *
 *	 @author Zhao Zhang & Chad Nelson
 *	 @date 06/26/2012
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd.h"
#include <math.h>

/// period of the fast PWM for sonar sensor
#define  pulse_period  43000

// Global used for interrupt driven delay functions
volatile unsigned int timer2_tick;
void timer2_start(char unit);
void timer2_stop();

/**
 * 	Blocks for a specified number of milliseconds
 * 	@author Zhao Zhang & Chad Nelson
 * 	@date 06/26/2012
 * 	@param time_val	number of milliseconds to wait
 */

void wait_ms(unsigned int time_val) 
{
	//Seting OC value for time requested
	OCR2=250; 				//Clock is 16 MHz. At a prescaler of 64, 250 timer ticks = 1ms.
	timer2_tick=0;
	timer2_start(0);

	//Waiting for time
	while(timer2_tick < time_val);

	timer2_stop();
}

/**
 * 	Start timer2
 * 	@author Zhao Zhang & Chad Nelson
 * 	@date 06/26/2012
 * 	@param unit	determines fast or slow 
 */

void timer2_start(char unit) 
{
	timer2_tick=0;
	if ( unit == 0 ) { 		//unit = 0 is for slow 
        TCCR2=0b00001011;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 64
        TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	if (unit == 1) { 		//unit = 1 is for fast
        TCCR2=0b00001001;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 1
        TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	sei();
}

/**
 * 	Stop timer2
 * 	@author Zhao Zhang & Chad Nelson
 * 	@date 06/26/2012
 */

void timer2_stop() 
{
	TIMSK&=~0b10000000;		//Disabling O.C. Interrupt for Timer2
	TCCR2&=0b01111111;		//Clearing O.C. settings
}


// Interrupt handler (runs every 1 ms)
ISR (TIMER2_COMP_vect) 
{
	timer2_tick++;
}

/**
 * 	This function moves the servo to a fixed degree. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@param degree 	fixed degree that servo moves to 
 */

void move_servo(double degree)
{
	degree = 180 - degree;
	unsigned int pulse_width = round(-(degree / 180.0) * 3250 + 4300);
	OCR3B = pulse_width - 1;
	wait_ms(20);
}

/**
 * 	This function initializes fast PWM registers to control the servo. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */

void servo_init(void)
{
	TCCR3A = 0x23;
	TCCR3B = 0x1A;
	OCR3A = pulse_period - 1;
	OCR3B = 2700;
	DDRE = 0x10;
}

/**
 * 	This function sends the sonar pulse  
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */

void send_pulse()
{
	//Disable IC Interrupt
	TIMSK &= 0;
	//Config PD4 to output
	DDRD |= 0x10;
	//Set signal to high
	PORTD |= 0x10;
	wait_ms(1);
	//Set signal to low
	PORTD &= 0XEF;
	//Config PD4 to input
	DDRD &= 0XEF;
	//Clear IC Flag
	TIFR &= 0xDF;
	//Enable IC Interrupt
	TIMSK |= 0x24;
}

/**
 * 	This function initializes the input capture registers for sonar. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */

void sonar_init(void)
{
		TCCR1A = 0;
		TCCR1B = 0xC2;
		TCCR1C = 0;
		TIMSK = 0x24;
}

/**
 * 	This function initializes the ADC registers for reading the IR sensor. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */
 
void IR_init()
{
	ADMUX |= (0b01000010);
}

/**
 * 	This function reads the average of 5 ADC values. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@return returns the average ADC value over 5 reads 
 */
double IR_read()
{
	int sum = 0;
	int avg = 0;
	ADCSRA |= 0xC7;
	for (int i = 0; i < 5; i++)
	{
		while((ADCSRA & 0b01000000) == 0b01000000)
		;
		sum += ADC;
	}
	avg = sum/5;
	double dist = 34272.0 * pow(avg, -1.376);
	return dist;
}

/**
 * 	This function initializes the USART registers for transmitting 
 * 	and receiving information through serial communication.
 *
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@param ubrr	The calculated baud rate 
 */

void USART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	UCSR0B = (1<<RXEN0) | (1 << TXEN0);
	UCSR0C = (1<<USBS0)|(3 << UCSZ00);
	UCSR0A = (1 << U2X0);
}

/**
 * 	This function receives one byte of data.  
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@return the received byte of data
 */

unsigned char USART_Receive(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

/**
 * 	This function transmitts one byte of data 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@param data	The byte of data being transmitted
 */

void USART_Transmit(unsigned char data)
{
	while(!(UCSR0A & (1 << UDRE0)))
	;
	UDR0 = data;
}

