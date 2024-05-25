/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                      CAR2.cpp                                                                         */
/* ===================================================================== */
/* This class encapsulates player(2) information on a car (excluding sound info)           */
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
#include "CAR2.h"

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                            Functions                                                                     */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

CAR2::CAR2(int crszx, int crszy,  int expszx, int expszy, int expframs, int *animseq,
		   int animspd, int crfrms) : Basic_CAR(crszx, crszy,  expszx, expszy, expframs, animseq,
		   animspd, crfrms)  {
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

	CAR_Voulme_coeff=100;															// volume of other car
	Frame=0;																						  // starting frame of other car
}

void CAR2::CAR_Draw_Car() {
	/*Draw car with correct frame */

			// set x and y coords
			car.x=Recx;
			car.y=Recy;
			// set correct frame
			car.curr_frame=Frame;
			// draw car!
			Basic_CAR::CAR_Draw_Car();										  // Draw car BOB on back buffer  
}

void CAR2::CAR_Check_Direction() {
	/* check explosions */

	if(!CAR_Is_Car_Alive) {
		/* car is dead, draw explosion animation, or start new game if anim finished */ 
		CAR_Check_Explosion();
	}
}

void CAR2::CAR_Car_Crash() {
	/* Record race loss and set up for a new race */

	Basic_CAR::CAR_Car_Crash();

	/* Set variables for a new race */
	Recx=SMapx;																						 // Starting positions
	Recy=SMapy;																			
}

void CAR2::CAR_Check_Explosion() {
	/* car is dead, draw explosion animation, or start new game if anim finished */ 
	if(!explosion.anim_state) {
		explosion_happening=true;
		/* animation still going or has not started */
		Set_Pos_BOB(&explosion,car.x,car.y);					   // Explosion position -> on top of car
		Animate_BOB(&explosion);											 // Animate the explosion
	} else {
		/* animation has finished */
		explosion_happening=false;
		CAR_Car_Crash();															     // call cleanup routine
	}
}