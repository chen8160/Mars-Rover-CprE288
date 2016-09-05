
/**
 * 	@file music.c
 *	@brief this file contains the functions that flash the LEDs 
 *	and play music from the robot
 *
 *	@author	Cheng Song
 *
 *	@date 4/12/2015
 */

#include "open_interface.h"

/**
 * 	this function flashes the LEDs on the robot 
 * 	@author Cheng Song
 * 	@date 4/12/2015
 */

void flashLED(){

	for(int i=0;i<5;i++){
		for (int j = 0; j<256; j++)
		{
			oi_set_leds(0,0,256-j,j);
			wait_ms(5);
		}
		for (int j = 255; j>=0; j--)
		{
			oi_set_leds(0,0,256-j,j);
			wait_ms(5);
		}
	}
}

/**
 * 	This function plays the music from the robot
 * 	@author Cheng Song 
 * 	@date 4/12/2015
 */


void play_song(){
	// Notes: oi_load_song takes four arguments
	// Integer from 0 to 16 identifying this song
	// Integer that indicates the number of notes in the song (if greater than 16, it will consume the next song index's storage space)
	// For a note sheet, see page 12 of the iRobot Creat Open Interface datasheet
	
	unsigned char mario1NumNotes = 0;
	unsigned char musicalnote[]    = {48, 60, 45, 57, 46, 58,  0, 48, 60, 45, 57, 46, 58,  0, 41, 53, 38, 50, 39, 51,  0, 41, 53, 38, 50, 39, 51,  0, 51, 50, 49, 48, 51, 50, 44, 43, 49, 48, 54, 53, 52, 58, 57, 56, 51, 47, 46, 45, 44 };
	unsigned char period[] = {12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 48,  8,  8,  8, 24, 24, 24, 24, 24, 24,  8,  8,  8,  8,  8,  8, 16, 16, 16, 16, 16, 16 };

	oi_load_song(0, 49, musicalnote, period);
	
	oi_play_song(0);
	flashLED();
}

