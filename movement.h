
/**
 *	@file movement.h
 *	@brief this is the header file that contains the functions 
 *	related to the movement of the robot
 *
 *	@author Yuixiang Chen
 *
 *	@date 4/12/2015
 */

#include "open_interface.h"

/**
 *	This function turns the robot clockwise a by a defined number of degrees
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data 
 *	@param degrees	number of degrees the robot will turn 
 */

void turn_clockwise(oi_t *sensor, int degrees);

/**
 *	This function turns the robot counterclockwise a by a defined number of degrees
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data 
 *	@param degrees	number of degrees the robot will turn 
 */

void turn_counterclockwise(oi_t *sensor, int degrees);

/**
 *	This function moves the robot forward a defined distance. 
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data. 
 *	@param dist	distance the robot will travel in centimeters.
 *	@return 1 if detected a cliff, tape, or bumper; 0 if detected nothing 
 */

int move_forward(oi_t *sensor, int dist);

/**
 *	This function moves the robot backward a defined distance. 
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	sensor is a struct that contains all sensor data. 
 *	@param dist	distance the robot will travel in centimeters.
 *	@return 1 if detected a cliff, tape, or bumper; 0 if detected nothing 
 */

void move_backward(oi_t *sensor, int dist);

/**
 *	This function checks all the sensors to detect a cliff, tape, or a bumper 
 *	@author Yuixiang Chen 
 *	@date 4/12/2015
 *	@param sensor	structure that contains all the sensor data
 *	@return  returns 1 if any of the check condition are met or 0 if none. 
 */

int checkCondition(oi_t *sensor);
