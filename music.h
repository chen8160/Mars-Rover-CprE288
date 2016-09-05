/**
 * 	@file music.h
 *	@brief this is the header that file contains the functions 
 *	that flash the LEDs and play music from the robot
 *
 *	@author	Cheng Song
 *
 *	@date 4/12/2015
 */


#define RICK_ROLL		0
#define IMERPIAL_MARCH 		1
#define MARIO_UNDERWORLD	3
#define MARIO_UNDERWATER	7



#warning "Make sure the open interface has been initialized before calling load_songs()."
/// you may delete this warning
/// Loads some songs over the open interface

/**
 * 	This function plays the music from the robot
 * 	@author Cheng Song 
 * 	@date 4/12/2015
 */

void play_song();
