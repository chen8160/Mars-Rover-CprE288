
/**
 *	@file movement.c
 *	@brief this file contains all functions related to the movement of the robot
 *
 *	@author Yuixiang Chen
 *
 *	@date 4/12/2015
 */



#include "open_interface.h"
#include "util.h"


///location variables

int movedangle =0;	/// direction robot is pointing relative to its initial direction 
int x=0;		/// X coordinate position
int y=0;		/// Y coordinate position
double r;		/// Radial distance from initial starting point 

char status[250];	///array used to transmitt the status data of the sensors 

/**
 *	This function turns the robot clockwise a by a defined number of degrees
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data 
 *	@param degrees	number of degrees the robot will turn 
 */

void turn_clockwise(oi_t *sensor, int degrees) { 
	movedangle -= degrees%360;
	if (degrees != 180)
	{
		degrees = (int) round((double) degrees / 10 * 6);
	}
    int sum = degrees;
    oi_set_wheels(-100, 100); // turn left
    while (sum > 0) {
        oi_update(sensor);
        sum += sensor->angle;
    }
    oi_set_wheels(0, 0); // stop
	sprintf(status, "\n\rLocation: X: %d    Y: %d    R: %.2f    Angle: %d\n\r", x, y, r, movedangle);

	/// transmit the position data
	for (int i = 0; i < strlen(status); i++)
	{
		USART_Transmit(status[i]);
	}
	
}

/**
 *	This function turns the robot counterclockwise a by a defined number of degrees
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data 
 *	@param degrees	number of degrees the robot will turn 
 */

void turn_counterclockwise(oi_t *sensor, int degrees) { 
	movedangle += degrees;
	if (degrees != 180)
	{
		degrees = (int) round((double) degrees / 10 * 6);
	}
    int sum = 0;
    oi_set_wheels(100, -100); 		// turn right
    while (sum < degrees) {
        oi_update(sensor);
        sum += sensor->angle;
    }
    oi_set_wheels(0, 0); 		// stop	
	
	sprintf(status, "\n\rLocation: X: %d    Y: %d    R: %.2f    Angle: %d\n\r", x, y, r, movedangle);
	///transmit position data
	for (int i = 0; i < strlen(status); i++)
	{
		USART_Transmit(status[i]);
	}
	
}

/**
 *	This function moves the robot forward a defined distance. 
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data. 
 *	@param dist	distance the robot will travel in centimeters.
 *	@return 1 if detected a cliff, tape, or bumper; 0 if detected nothing 
 */

int move_forward(oi_t *sensor, int dist) { 
	dist = dist / 50 * 45 / 2;
    int sum = 0;
	int condition = 0;
    oi_set_wheels(100, 100); 				// move forward; full speed
    while (sum < dist) {
        oi_update(sensor);
        sum += sensor->distance;
		x+=2 * (int)sensor->distance*cos(movedangle);
		y+=2 * (int)sensor->distance*sin(movedangle);
		r=sqrt(pow(x,2)+pow(y,2));
		condition = checkCondition(sensor);	//check for cliff,tape,bumper	
		if (condition)	
		{
			break;
		}
    }
    oi_set_wheels(0, 0); // stop
	
	
	

	
	sprintf(status, "\n\rLocation: X: %d    Y: %d    R: %.2f    Angle: %d\n\r", x, y, r, movedangle);
	///transmit location data
	for (int i = 0; i < strlen(status); i++)
	{
		USART_Transmit(status[i]);			
	}
	
	return condition;
	
}

/**
 *	This function moves the robot backward a defined distance. 
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data. 
 *	@param dist	distance the robot will travel in centimeters.
 *	@return 1 if detected a cliff, tape, or bumper; 0 if detected nothing 
 */

void move_backward(oi_t *sensor, int dist) {
	dist = dist / 50 * 45 / 2;
	int sum = dist;
	oi_set_wheels(-100, -100); // move forward; full speed
	while (sum > 0) {
		oi_update(sensor);
		sum += sensor->distance;
		x+=(int)sensor->distance*cos(movedangle);
		y+=(int)sensor->distance*sin(movedangle);
		r=sqrt(pow(x,2)+pow(y,2));
	}
	oi_set_wheels(0, 0); // stop
	
	sprintf(status, "\n\rLocation: X: %d    Y: %d    R: %.2f    Angle: %d\n\r", x, y, r, movedangle);
	///transmit location data
	for (int i = 0; i < strlen(status); i++)
	{
		USART_Transmit(status[i]);
	}	
}

/**
 *	This function checks all the sensors to detect a cliff, tape, or a bumper 
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	structure that contains all the sensor data
 *	@return  returns 1 if any of the check condition are met or 0 if none. 
 */

int checkCondition(oi_t *sensor){
	char status[500];
	int result = 0;
	
	oi_update(sensor);

	// hit bumper left
	if(sensor->bumper_left)
	{
		
		
	
		
		sprintf(status, "\n\rleft bumper!\n\r");
		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		sensor->bumper_left = 0;
		result = 1;
	}
	
		
		
	// hit bumper right	
	else if(sensor->bumper_right)
	{

		sprintf(status, "\n\rright bumper!\n\r");
		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
		
		
	// left side went into cliff
	else if(sensor->cliff_left)
	{
		sprintf(status, "\n\rleft cliff!\n\r");
		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
	// right side went into cliff	
	else if(sensor->cliff_right)
	{
		sprintf(status, "\n\rright cliff!\n\r");
		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		

	// front left side went into cliff
	else if(sensor->cliff_frontleft)
	{
		sprintf(status, "\n\rfront left cliff!\n\r");
		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
	// front right side went into cliff	
	else if(sensor->cliff_frontright)
	{
		sprintf(status, "\n\rfront right cliff!\n\r");
		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
	// left side run over white tape
	else if(sensor->cliff_left_signal > 280 && sensor->cliff_left_signal < 370)
	{
		sprintf(status, "\n\rleft cliff: white tape!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
	// right side run over white tape	
	else if(sensor->cliff_right_signal > 500 && sensor->cliff_right_signal < 600)
	{
		sprintf(status, "\n\tright cliff: white tape!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
	// front left side run over white tape
	else if(sensor->cliff_frontleft_signal > 650 && sensor->cliff_frontleft_signal < 780)
	{

		sprintf(status, "\n\rfront left cliff: white tape!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
	// front right side run over white tape	
	else if(sensor->cliff_frontright_signal > 200 && sensor->cliff_frontright_signal < 250)
	{
		sprintf(status, "\n\rfront right cliff: white tape!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		move_backward(sensor, 50);
		result = 1;
	}
		
		
		
		
		
		
	// At Destination		
	else if(sensor->cliff_left_signal > 500 && sensor->cliff_left_signal < 650)
	{
		sprintf(status, "\n\rL_Destination!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		oi_set_wheels(0, 0); // stop
		result = 1;
	}
		
		
		
		
		
	// At Destination
	else if(sensor->cliff_right_signal > 800 && sensor->cliff_right_signal < 950)
	{
		sprintf(status, "\n\rR_Destination!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		oi_set_wheels(0, 0); // stop
		result = 1;
	}
		
		
	// At Destination		
	else if(sensor->cliff_frontleft_signal > 1000 && sensor->cliff_frontleft_signal < 1420)
	{
		sprintf(status, "\n\rFL_Destination!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		oi_set_wheels(0, 0); // stop
		result = 1;
	}
		
		
	// At Destination
	else if(sensor->cliff_frontright_signal > 300 && sensor->cliff_frontleft_signal < 400)
	{
		sprintf(status, "\n\rFR_Destination!\n\rBump Sensor( Left: %d   Right: %d)\n\rCliff Sensors(Left: %d   Front left: %d   Front right: %d   Right: %d)\n\rCliff Sensor Signals(Left: %d   Left Front: %d   Right Front: %d   Right: %d\n\r", sensor->bumper_left, sensor->bumper_right, sensor->cliff_left, sensor->cliff_frontleft, sensor->cliff_frontright, sensor->cliff_right, sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);

		for (int i = 0; i < strlen(status); i++)
		{
			USART_Transmit(status[i]);
		}
		oi_set_wheels(0, 0); // stop
		result = 1;
	}


	return result;
}
