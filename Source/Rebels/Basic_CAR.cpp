/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                 Basic_CAR.cpp                                                                  */
/* ===================================================================== */
/* This class encapsulates basic information on a car (excluding sound info)                  */
/* ------------------------------------------------------------------------------------------------------------------------- */	
/* 000000000000000000000000000000000000000000000000000000000000000000000 */
/* 000000000000000000011000000000011000000000000000000000000000000000000 */
/* 000000011111100000101000110000111000011000000000001111110000000000000 */
/* 000000100001100001010000110001110000110000000000001000111100000000000 */
/* 000011000001100010100001110011010000110000000000001100001100000000000 */
/* 000011000011000111000000100010100001110000001110001100111000010000000 */
/* 000110000011001110100000100011000000100000110100001111000001111000000 */
/* 000110000000001101100111100110000101100000110001111100000010011011000 */
/* 000011000000111110111001111001111001100011111110011111001100010100000 */
/* 000011111111001100000001100001100001111110001000010001110000111000000 */
/* 000000011000000000000000000000000001111000000000010000011100111000000 */
/* 000000000000000000000000000000000000000000000000000000000001011000000 */
/* 000000000000000000000000000000000000000000000000000000000010110000000 */
/* 000000000000000000000000000000000000000000000000000000000000000000000 */
/* ------------------------------------------------------------------------------------------------------------------------- */
/*                            << ENTROPY Software >> email: chillerz@one.net.au                               */       
/* ------------------------------------------------------------------------------------------------------------------------- */


#define WIN32_LEAN_AND_MEAN				// Force options in windows.h

/* -------------------------------------------------------------------------------------------------------------- */
/* ===============================================================*/															  
/*                                                            Includes                                                                        */
/* ===============================================================*/
/* -------------------------------------------------------------------------------------------------------------- */	

/* Windows and c includes */

#include <windows.h>					
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream.h>			
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>

/* Direct x includes */

#include <ddraw.h> 

/* My includes */

#include "ErrorVille.h"
#include "Gpdumb1.h"
#include "Basic_CAR.h"

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                            Functions                                                                     */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

Basic_CAR::Basic_CAR (int crszx, int crszy,  int expszx, int expszy, int expframs, int *animseq,
											  int animspd, int crfrms) {
	/* Constructor */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* crsz = x and y pixel length of car (eg 32x32)				     */
	/* expszx = x  pixel length of  explosion (eg 32x32)			 */
	/* expszy = y pixel length of  explosion (eg 32x32)             */
	/* expframs = number of frames in explosion						  */
	/* animseq = animation sequence for explosion				  */
	/* animspd = speed of explosion												*/
	/* crfrms = number of car frames											   */
	/* ------------------------------------------------------------------------------ */

	/* Set variables to parameter values */

	CAR_Size_x=crszx;														   // Set car size
	CAR_Size_y=crszy;			
	CAR_Explosion_Size_x=expszx;							    // Set explosion size
	CAR_Explosion_Size_y=expszy;	
	CAR_Explosion_Frames=expframs;						   // Number of frames in explosion
	CAR_Explosion_Speed=animspd;							// Explosion speed	
	CAR_Anim_Explosion=animseq;								   // Animation sequence
	CAR_Frames=crfrms; 														// no. of frames for the car

	/* Initialise other variables */

	CAR_Score=0;															       // Initialise Score
	CAR_Is_Car_Alive=true;												   // Car starts off alive
	Wait_Frame_Explosion=false;									    // Wait a frame for explosion?
}

Basic_CAR::~Basic_CAR() {
	/* CAR destructor */

	/* Kill all BOBs */

	// Set up error counter
	int err = 0;																	   	  
	// Destroy all BOBs
	err+=Destroy_BOB(&car);										
	err+=Destroy_BOB(&car_picture1);
	err+=Destroy_BOB(&explosion);

	if (err<3) {
		ErrorVille("Could not kill all CAR BOBs");
	}
}

void Basic_CAR::CAR_Load_BOB(char *bitmap_name_car, char * bitmap_name_exp) {
	/* Load Car and explosion BOBs, initialise animations */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* bitmap_name_car = file name for car bitmap				  */
	/* bitmap_name_exp = file name for explosion bitmap	  */
	/* ----------------------------------------------------------------------------- */

	/* --------------------------------------------------------------------------------------------------------------*/
	/*															Load BOBS							                                       */
	/* --------------------------------------------------------------------------------------------------------------*/	

	//Load car bitmap from disk 
	if (!Load_Bitmap_File(&bitmap8bit, bitmap_name_car)) {
		ErrorVille("Could not load car BOB");
	}
	// Create the car bitmap objects, eg. 32*32 BOBs with 4 frames  
	if (!Create_BOB(&car, CAR_Position_x, 	CAR_Position_y, CAR_Size_x, CAR_Size_y, 
		CAR_Frames, BOB_ATTR_MULTI_FRAME|BOB_ATTR_VISIBLE|BOB_ATTR_BOUNCE,0)) {
		ErrorVille("Could not create car BOB");
	}
	if (!Create_BOB(&car_picture1, CAR_Position_x, 	CAR_Position_y, CAR_Size_x, CAR_Size_y, 
		CAR_Frames, BOB_ATTR_MULTI_FRAME|BOB_ATTR_VISIBLE ,0)) {
		ErrorVille("Could not create car_picture1 BOB");
	}
	// Load the images into the bitmap objects using cell extraction mode 
	for (int i=0; i<CAR_Frames; i++) {
		if (!Load_Frame_BOB(&car, &bitmap8bit, i, i, 0,BITMAP_EXTRACT_MODE_CELL)) {
			ErrorVille("Could not load car BOB");
		}
		if (!Load_Frame_BOB(&car_picture1, &bitmap8bit, i, i, 0,BITMAP_EXTRACT_MODE_CELL)) {
			ErrorVille("Could not load car_picture1 BOB");
		}
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);
	// Load the explosion bitmap file 
	if (!Load_Bitmap_File(&bitmap8bit, bitmap_name_exp)) {
		ErrorVille("Could not load explosion BOB");
	}
	// Create the explosion BOB, an animation bob 
	if (!Create_BOB(&explosion, 0,0, CAR_Explosion_Size_x, CAR_Explosion_Size_y, 
		CAR_Explosion_Frames, 
		BOB_ATTR_MULTI_ANIM|BOB_ATTR_VISIBLE|BOB_ATTR_ANIM_ONE_SHOT,0)) {
			ErrorVille("Could not create explosion BOB");
	}
	// Load individual frames into BOB 
	for (i=0; i<CAR_Explosion_Frames; i++) {
		if (!Load_Frame_BOB(&explosion, &bitmap8bit, i, i, 0,BITMAP_EXTRACT_MODE_CELL)) {
			ErrorVille("Could not load frames for explosion BOB");
		}
	}
	// Free the memory from the bitmap file 	
	Unload_Bitmap_File(&bitmap8bit);

	/* --------------------------------------------------------------------------------------------------------------*/
	/*												Set up Explosion Anim				                                          */
	/* --------------------------------------------------------------------------------------------------------------*/	

	/* Load the animation information, anim number = 0,  no. frames = CAR_Explosion_Frames,
		Frame sequence = CAR_Anim_Explosion (eg {0,4,5,3,1,2,1,2,1}) */

	Load_Animation_BOB(&explosion, 0,CAR_Explosion_Frames,  CAR_Anim_Explosion);
	// Set speed and current anim 
	Set_Anim_Speed_BOB(&explosion, CAR_Explosion_Speed);		// set speed (eg.2 frames) 
	Set_Animation_BOB(&explosion,0);		
	// current animation = 0	
	car.curr_frame=0;
}

void Basic_CAR::CAR_Draw_Car_Picture1(int frame, int x, int y){
	/* Draw car_picture1 at on backbuffer */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* 	frame = which frame to draw										     	   */
	/* 	x = x coord																					 */
	/*  y = y coord																				    */
	/* ----------------------------------------------------------------------------- */

	Set_Pos_BOB(&car_picture1, x, y);							// set position to x and y
	car_picture1.curr_frame = frame;							    // select frame 
	Draw_BOB(&car_picture1, lpddsback);					  // draw the BOB on the backbuffer
}

void Basic_CAR::CAR_Car_Crash() {
	/* Record race loss and set up for a new race */

	/* Set variables for a new race */
	explosion.anim_state=false;															 // reset anim_state													
	CAR_Is_Car_Alive=true;																    // car is not dead
	Set_Animation_BOB(&explosion,0);											// reset animation	
	car.curr_frame=0;																				 // back to facing up
	Wait_Frame_Explosion=true;														   // wait a frame for the explosion
}



void Basic_CAR::CAR_Draw_Explosion() {
	/* Draw an explosion */

	Draw_BOB(&explosion, lpddsback);
}

void Basic_CAR::CAR_Draw_Car() {
	/*Draw car with correct frame */

			Draw_BOB(&car, lpddsback);										  // Draw car BOB on back buffer   
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                  Get and Set Functions                                                      */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

bool Basic_CAR::CAR_Check_Status() {
	/* Return status of car */
	return  CAR_Is_Car_Alive;
}
void Basic_CAR::CAR_Dead() {
	/* kill the car */
	 CAR_Is_Car_Alive=false;
}

void Basic_CAR::CAR_Inc_Score() {
	/* increment current score */

	CAR_Score++;
}
int Basic_CAR::CAR_Get_Score() {
	/* return the current score */

	return CAR_Score;
}
