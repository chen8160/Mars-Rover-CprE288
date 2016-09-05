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



/// Blocks for a specified number of milliseconds
void wait_ms(unsigned int time_val);

/**
 * 	This function moves the servo to a fixed degree. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@param degree 	fixed degree that servo moves to 
 */

void move_servo(double degree);

/**
 * 	This function initializes fast PWM registers to control the servo. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */

void servo_init(void);

/**
 * 	This function sends the sonar pulse  
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */

void send_pulse(void);


/**
 * 	This function initializes the ADC registers for reading the IR sensor. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */

void IR_init();

/**
 * 	This function reads the average of 5 ADC values. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@return returns the average ADC value over 5 reads 
 */

double IR_read();

/**
 * 	This function initializes the input capture registers for sonar. 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 */

void sonar_init(void);

/**
 * 	This function receives one byte of data.  
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@return the received byte of data
 */

unsigned char USART_Receive(void);

/**
 * 	This function initializes the USART registers for transmitting 
 * 	and receiving information through serial communication.
 *
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@param ubrr	The calculated baud rate 
 */

void USART_Init(unsigned int ubrr);

/**
 * 	This function transmitts one byte of data 
 * 	@author Yuixiang Chen 
 * 	@date 4/12/2015
 * 	@param data	The byte of data being transmitted
 */

void USART_Transmit(unsigned char data);
