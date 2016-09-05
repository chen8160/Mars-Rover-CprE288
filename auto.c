/**
*	@file Auto.c
*	@brief this is the main c file that controls the robot. 
*	@author Yuxiang Chen
*	@date 4/12/2015
*/ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "open_interface.h"
#include "util.h"
#include "movement.h"

#include "music.h"

///structure for recording the characteristics of each object seen
struct objects{
	int degrees;		//the degree the object was seen at 
	int dwidth;		//the width of the object in degrees
	double sonar;		//distance from robot measured useing sonar pulse
	int index;		//how many objects were seen before it 
	double width;		//width in centimeters
	double ir;		//distance from robot measured with IR sensor
};
	
double IR_dist;			//distance measured using IR sensor
volatile int rise;		//rising edge of received sonar pulse
volatile int fall;		//falling edge of received sonar pulse 
volatile int delta;		//difference in between rising and falling edge of received sonar pulse
volatile double distance;	//stores measured sonar distances in centimeters
volatile double time;		//stores time between rising and falling edge of received sonar pulse 
volatile int overflow;		//stores the overflow
volatile char finish = 0;	//is set to 1 when sonar is done measuring 
struct objects myObject[10];	//array that contaings the data for each object 
int index = 0;			//variable to keep track of the current index 
	




///interrupt occurs when the rising edge of a pulse on the IC pin is detected
ISR (TIMER1_CAPT_vect){
	if ((TCCR1B & 0x40))
	{
		rise = ICR1;
		TCCR1B = 0x82;
		} else{
		fall = ICR1;
		TCCR1B = 0xC2;
		delta = fall - rise;			//calculate clock ticks
		time = 0.0005 * delta;			//calculate time
		distance = 34 * time / 2-30;		//calculate distance 
		finish = 1;				
	}
}


///This interrupt occurs when an overflow is detected 
ISR(TIMER1_OVF_vect){
	overflow++;
}


/**
 *	This function uses the servo to sweep from 0 to 180
 *	degrees and measure the distance seen by the IR and 
 *	Sonar sensors for each degree. 
 *	@author Yuixiang Chen
 *	@date 4/12/2015  
 */

void sweep()
{

	//initializations 
	USART_Init(34);
	servo_init();
	sonar_init();
	IR_init();
	move_servo(0);
	wait_ms(1000);
	

	double lastDistance = 0;		//distance measured at the previous degree
	double currentDistance = 0;		//distance measured at the current degree
	int startDegree = 0;			//degree when object is first detected
	int endDegree = 0;			//the last degree that the object was detected
	double distances[90];			//array that holds distance data for each degree 
	index = 0;
	
	char s[] = "Degrees\t\tIR Distance (cm)\t\tSonar Distance (cm)\n\r";
	for(int i = 0; i < strlen(s) ; i++){
		USART_Transmit(s[i]);
	}
	
	//loop through each degree
	for (int i = 0; i <= 180; i++)
	{
		move_servo(i);
		IR_dist = IR_read();		
		unsigned char output[30];
		send_pulse();
		while(!finish);
		sprintf(output, "%d\t\t%.2f\t\t\t\t%.2f\n\r", i, IR_dist, distance);
		//lprintf("%d\n%.2f\n%.3f\n", i, IR_dist,distance);
		for (int j = 0; j < strlen(output); j++)
		{
			USART_Transmit(output[j]);
		}
		
		lastDistance = currentDistance;
		currentDistance = IR_dist;
		distances[i/2] = currentDistance;
		
		//first detect the object
		if ((lastDistance < 5 || lastDistance > 50) && currentDistance < 50 && currentDistance > 5)
		{
			startDegree = i;
		}

		//detect end of object
		if (lastDistance > 5 && lastDistance < 50 && (currentDistance > 50 || currentDistance < 5))
		{
			//fill myObject with the objects data
			endDegree = i;
			myObject[index].degrees = (endDegree + startDegree)/2;
			myObject[index].sonar = distance;
			myObject[index].ir = distances[myObject[index].degrees/2];
			myObject[index].index = index;
			myObject[index].dwidth = endDegree - startDegree;
			myObject[index].width = (double) (2 * myObject[index].sonar * tan((double)myObject[index].dwidth / 360.0 * 3.1415926));
			index++;
		}
	}
	
	//transmit data obtained from the sweep
	for (int i = 0; i < index; i++)
	{
		char output[100];
		sprintf(output, "Index: %d\n\rDegree: %d\n\rWidth: %.2f\n\rSonar Distance: %.2f\n\rIR distance: %.2f\n\r\n\r",myObject[i].index,myObject[i].degrees,myObject[i].width, myObject[i].sonar, myObject[i].ir);
		for (int j = 0; j < strlen(output); j++)
		{
			USART_Transmit(output[j]);
		}
	}
}

/**
 *	This function updates and transmits the current state of all robot sensors.
 *	@author Yuixiang Chen
 *	@date 4/12/2015
 *	@param sensor_data 	memory space for the sensor data to be held 
 */

void read(oi_t *sensor_data){
	oi_update(sensor_data);
	char status[250];

	sprintf(status, "Bump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor_data->bumper_left, sensor_data->bumper_right, sensor_data->cliff_left, sensor_data->cliff_frontleft, sensor_data->cliff_frontright, sensor_data->cliff_right, sensor_data->cliff_left_signal, sensor_data->cliff_frontleft_signal, sensor_data->cliff_frontright_signal, sensor_data->cliff_right_signal);

	for (int i = 0; i < strlen(status); i++)
	{
		USART_Transmit(status[i]);
	}
}

/**
 *	This is the main functiom. It calls functions and allows us 
 *	to communicate with the robot
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 */

int main(void)
{
	//initializations
	USART_Init(34);

	char status[250];	//array that temporarily hold sensor data before being transmitted
	
    while(1)
    {
		oi_t *sensor_data = oi_alloc();
		oi_init(sensor_data);
		
		sprintf(status, "Bump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor_data->bumper_left, sensor_data->bumper_right, sensor_data->cliff_left, sensor_data->cliff_frontleft, sensor_data->cliff_frontright, sensor_data->cliff_right, sensor_data->cliff_left_signal, sensor_data->cliff_frontleft_signal, sensor_data->cliff_frontright_signal, sensor_data->cliff_right_signal);
		
		//transmitting current state of all robot sensors
		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		
		unsigned char comm = USART_Receive();		//character that represents a remote control command 

		//move forward slowly
		if (comm == 'q')
		{
			move_forward(sensor_data, 100);
			if (sensor_data->cliff_left_signal > 500 && sensor_data->cliff_left_signal < 650)
			{
				turn_counterclockwise(sensor_data, 55);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_right_signal > 800 && sensor_data->cliff_right_signal < 950)
			{
				turn_clockwise(sensor_data, 55);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_frontleft_signal > 1000 && sensor_data->cliff_frontleft_signal < 1420)
			{
				turn_counterclockwise(sensor_data, 20);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_frontright_signal > 300 && sensor_data->cliff_frontleft_signal < 400)
			{
				turn_clockwise(sensor_data, 20);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			}
		} 

		//move forward 
		else if (comm == 'w')
		{
			move_forward(sensor_data, 200);

			//check for tape
			if (sensor_data->cliff_left_signal > 500 && sensor_data->cliff_left_signal < 650)
			{
				turn_counterclockwise(sensor_data, 55);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_right_signal > 800 && sensor_data->cliff_right_signal < 950)
			{
				turn_clockwise(sensor_data, 55);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_frontleft_signal > 1000 && sensor_data->cliff_frontleft_signal < 1420)
			{
				turn_counterclockwise(sensor_data, 20);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_frontright_signal > 300 && sensor_data->cliff_frontleft_signal < 400)
			{
				turn_clockwise(sensor_data, 20);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			}
		}
	       
		//move forward fast
		else if (comm == 'e')
		{
			move_forward(sensor_data, 300);

			//check for tape
			if (sensor_data->cliff_left_signal > 500 && sensor_data->cliff_left_signal < 650)
			{
				turn_counterclockwise(sensor_data, 55);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_right_signal > 800 && sensor_data->cliff_right_signal < 950)
			{
				turn_clockwise(sensor_data, 55);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_frontleft_signal > 1000 && sensor_data->cliff_frontleft_signal < 1420)
			{
				turn_counterclockwise(sensor_data, 20);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			} else if (sensor_data->cliff_frontright_signal > 300 && sensor_data->cliff_frontleft_signal < 400)
			{
				turn_clockwise(sensor_data, 20);
				int sum = 0;
				oi_set_wheels(100, 100); // move forward; full speed
				while (sum < 150) {
					oi_update(sensor_data);
					sum += sensor_data->distance;
				}
				oi_set_wheels(0, 0); // stop
				play_song();
				break;
			}

		}
		//move backward
		else if (comm == 's')
		{
			move_backward(sensor_data, 100);
		}
		//turn counterclockwise
	       	else if (comm == 'a')
		{
			turn_counterclockwise(sensor_data, 10);
		}
		//turn clockwise
		else if (comm == 'd')
		{
			turn_clockwise(sensor_data, 10);
		}
		//turn counterclockwise
	       	else if (comm == 'x')
		{
			turn_counterclockwise(sensor_data, 180);
		}
		//sweep measurement
	       	else if (comm == 'g')
		{
			sweep();
		}
		//transmit current state of all robot sensors
	       	else if (comm == 'r')
		{
			read(sensor_data);
		}
		//menuever around bumper counterclockwise
	       	else if (comm == '1')
		{
			int con = 0;
			turn_counterclockwise(sensor_data, 90);
			con = move_forward(sensor_data, 250);
			if (!con)
			{
				turn_clockwise(sensor_data, 50);
				con = move_forward(sensor_data, 250);
				if (!con)
				{
					turn_clockwise(sensor_data, 40);
					move_forward(sensor_data, 200);
				}
			}
		}
		//menuever around bumper clockwise
		else if (comm == '2')
		{
			int con = 0;
			turn_clockwise(sensor_data, 90);
			con = move_forward(sensor_data, 250);
			if (!con)
			{
				turn_counterclockwise(sensor_data, 50);
				con = move_forward(sensor_data, 250);
				if (!con)
				{
					turn_counterclockwise(sensor_data, 40);
					move_forward(sensor_data, 200);
				}
			}
		}
	       //turn counterclockwise
		else if (comm == 'z')
		{
			turn_counterclockwise(sensor_data, 5);
		} 
		//turn clockwise
		else if (comm == 'c')
		{
			turn_clockwise(sensor_data, 5);
		}
		
		//free the sensor data memory space 
		oi_free(sensor_data);

		wait_ms(100);
    }
}
