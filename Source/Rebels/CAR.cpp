/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                      CAR.cpp                                                                           */
/* ===================================================================== */
/* This class encapsulates player information on a car (excluding sound info)                */
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
#include <dsound.h>

/* My includes */

#include "ErrorVille.h"
#include "Gpdumb1.h"
#include "CAR.h"

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                            Functions                                                                     */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

CAR::CAR (int left, int right, int brake, int accel, int accel_rate,
			   int mxspd, int crszx, int crszy,  int expszx, int expszy, int expframs, int *animseq,
			   int animspd, int accbarlen, int accbarhght, int barcol, int lame, int crfrms, int accbarendcol,
			   int accbarendlen) : Basic_CAR(crszx, crszy,  expszx, expszy, expframs, animseq,
			   animspd, crfrms)  {
	/* Constructor */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* left = key for left																		    */
	/* right = key for right																	 */
	/* brake = key for brake															  */
	/* accel  = key for accel															    */
	/* accel_rate = rate at which car accelerates (in frames) */
	/* mxspd = maximum speed of car											 */
	/* crsz = x and y pixel length of car (eg 32x32)				      */
	/* expszx = x  pixel length of  explosion (eg 32x32)			 */
	/* expszy = y pixel length of  explosion (eg 32x32)              */
	/* expframs = number of frames in explosion						  */
	/* animseq = animation sequence for explosion				  */
	/* animspd = speed of explosion												*/
	/* accbarlen = length of acceleration bar							    */
	/* accbarhght = height of acceleration bar							  */
	/* barcol = colour of acceleration bar										*/
	/* lame = Max timer value															 */
	/* crfrms = number of car frames											   */
	/* accbarendcol = color of end of accel bar							*/
	/* accbarwidth= width of end of accel bar							   */
	/* ------------------------------------------------------------------------------ */

	/* Set variables to parameter values */

	CAR_Lame_Value=lame;									        // Set timer value
	CAR_Max_Max_Speed=mxspd;                               // overall max of all cars
	CAR_Best_Lap=CAR_Lame_Value;					   // Initialise to "Lame!" value 

	CAR_Acceleration_Rate=accel_rate;					  // Rate at which car accelerates
	CAR_Acceleration_Bar_Length=accbarlen;		   // Length of acceleration bar
	CAR_Acceleration_Bar_Height=accbarhght;		  // Height of acceleration bar
	CAR_Acceleration_Bar_Colour=barcol;   			 				// Colour of acceleration bar
	CAR_Acceleration_Bar_End_Colour=accbarendcol;	// Colour of  end of accel bar
	CAR_Acceleration_Bar_End_Width=accbarendlen;	 //	Width of end of accel bar	

	KEY_RIGHT=right;
	KEY_LEFT=left;
	KEY_BRAKE=brake;
	KEY_ACCEL=accel;

	/* Initialise other variables */
	CAR_Key = NONE;															// no key pressed
	CAR_Frame = 0;																// facing up
	CAR_Velocity_Divider=1;											   // reset velocity divider
	CAR_Acceleration_Counter=0;									 // initialise Acceleration counter
	CAR_Velocity=0;		 													    // No velocity
	CAR_Direction=N;														  // Car starts by facing North
	CAR_Passed_CheckPoint=false;					     	 // Set checkpoint bool					
	CAR_Laps_Completed=0;										    // Initialise lap counter
	CAR_Bounce=false;														// Car not bouncing
	CAR_Clock_Stopped=false;										// clock has not been stopped
}


int CAR::CAR_ID(int div) {
	/* Return new Incremented velocity */	
	/* --> Can be changed to make car slower or faster */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* div = multiply car velocity by this amount						  */
	/* ----------------------------------------------------------------------------- */

	return (CAR_Velocity*div);
}

void CAR::CAR_Mov(int x,int y) {
	/* Move map but not car */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* 	x = x coord																					 */
	/*  y = y coord																				    */
	/* ----------------------------------------------------------------------------- */

	Mapx+=x;
	Mapy+=y;
}

void CAR::CAR_Check_Direction() {
	/* check if user is moving, accelerating or braking and show explosion if car
		is dead */

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                             Check keys and velocity                                                      */
	/* --------------------------------------------------------------------------------------------------------------*/	

	if (!CAR_Bounce) {
		// if car not bouncing 
		if (KEY_DOWN(KEY_RIGHT)) {
			// move player to right 
			CAR_Key=RIGHT;
		}  else {
			if (KEY_DOWN(KEY_LEFT)) {
				// move player to left 
				CAR_Key=LEFT;		
			}  else {
				CAR_Key = NONE;
			}
		}

		/* test if player is accelerating */
	
		if (KEY_DOWN(KEY_ACCEL)) {
			if (CAR_Velocity<=CAR_Max_Speed) {
				// if the car is not already at its maximum speed 
				if(CAR_Acceleration_Counter<CAR_Acceleration_Rate) {
					// if Acceleration rate is still counting 
					CAR_Acceleration_Counter++;
				} else {
					// otherwise increase velocity and reset CAR_Acceleration counter 
					CAR_Velocity++;
					CAR_Acceleration_Counter=0;
				}
			}
		}
	}

	/* test if the player is braking */

	if (KEY_DOWN(KEY_BRAKE)) {
		if (CAR_Velocity>=1) {
			// if the car is not already stopped 
			if(CAR_Acceleration_Counter<CAR_Acceleration_Rate) {
				// if Acceleration rate is still counting
				CAR_Acceleration_Counter++;
			} else {
				// otherwise decrease velocity and reset CAR_Acceleration counter 
				CAR_Velocity=CAR_Velocity-CAR_Brake_Rate;
				if(CAR_Velocity<0) { 
						CAR_Velocity=0;
				}
				CAR_Acceleration_Counter=0;
			}
		}
	} 

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                 Move and draw cars, and car explosion                                     */
	/*								   ----------------------------------------------------------									  */
	/* The following section of code is hard coded for 16 directions (22.5 degree  */
	/* direction changing). To increase or decrease, change the following and      */
	/* the direction enumerated type.																						*/
	/* --------------------------------------------------------------------------------------------------------------*/	

	if(CAR_Is_Car_Alive) {
		// if car is not dead
		if(CAR_Bounce) {
		// if the car is bouncing
			if(CAR_Velocity==0) {
				// test if touching wall and move
				if (CAR_Bounce_NORTH) {
					CAR_Velocity=1;
					if (CAR_Bounce_WEST) {
							CAR_Mov((CAR_ID(2)), (CAR_ID(2)));
							Move_BOB(&car);					
					} else {
						if (CAR_Bounce_EAST) {
							CAR_Mov(-(CAR_ID(2)), (CAR_ID(2)));
							Move_BOB(&car);					
						} else {
							CAR_Mov(0, (CAR_ID(3)));	
							Move_BOB(&car);					
						}
					}					
				}  else if (CAR_Bounce_SOUTH) {
						CAR_Velocity=1;
						if (CAR_Bounce_WEST) {
							CAR_Mov((CAR_ID(2)), -(CAR_ID(2)));	
							Move_BOB(&car);					
						} else {
						if (CAR_Bounce_EAST) {
							CAR_Mov(-(CAR_ID(2)), -(CAR_ID(2)));	
							Move_BOB(&car);					
						} else {
							CAR_Mov(0, -(CAR_ID(3)));	
							Move_BOB(&car);					
						}
					}
				}  else if (CAR_Bounce_WEST) {
						CAR_Velocity=1;
							CAR_Mov((CAR_ID(3)),0);	
							Move_BOB(&car);					
				
				}  else if (CAR_Bounce_EAST) {
						CAR_Velocity=1;
							CAR_Mov(-(CAR_ID(3)),0);	
							Move_BOB(&car);					
				}
			}
			// reduce bounce timer (no control during bounce)
			CAR_Bounce_Count--;
			if((floor(CAR_Velocity/2))||(CAR_Velocity>=1)) {
				// reduce velocity
				CAR_Velocity--;
				if ((!CAR_Bounce_Count)||(CAR_Velocity<=1)) {
				// is bounce over, or has car stopped?
					CAR_Bounce=false;
			}
		} else {
			// Not the first bounce 	
			CAR_Bounce_First=false;
		}
	} else {
		// Not bouncing
		CAR_Bounce_Count=((CAR_Velocity*3));
		CAR_Bounce_X=1;
		CAR_Bounce_Y=1;

		/* if the car is not dead, check CAR_Direction and move car to x,y */

		// if the velocity divider is go, change frames if nec.
		if (CAR_Velocity_Divider==CAR_Turn_Rate) {
			if(CAR_Key==LEFT) {
				if (car.curr_frame==0) { 
					car.curr_frame=15;
					CAR_Direction=15;
				} else {
					car.curr_frame=((car.curr_frame-1)%16);
					CAR_Direction=((CAR_Direction-1)%16);
				}
			}
			if(CAR_Key==RIGHT) {
				car.curr_frame=((car.curr_frame+1)%16);
				CAR_Direction=((CAR_Direction+1)%16);
			}
		}
	}

	/* Move car in direction -->
		eg. N x=0,y=4*div*carbouncemodifier
			   NE x=3*above,y=1*above */

	switch(CAR_Direction) { 
		case N :			CAR_Mov(0, -(CAR_ID(4))*CAR_Bounce_Y);		
			break;
		case NNE :			CAR_Mov(CAR_ID(1)*CAR_Bounce_X,-(CAR_ID(3))*CAR_Bounce_Y);	
			break;
		case NE:		CAR_Mov(CAR_ID(2)*CAR_Bounce_X,-(CAR_ID(2))*CAR_Bounce_Y);		
			break;
		case NEE:			CAR_Mov(CAR_ID(3)*CAR_Bounce_X,-(CAR_ID(1))*CAR_Bounce_Y);		
			break;

		case E :			CAR_Mov(CAR_ID(4)*CAR_Bounce_X , 0);		
			break;
		case SEE :			CAR_Mov(CAR_ID(3)*CAR_Bounce_X ,CAR_ID(1)*CAR_Bounce_Y);		
			break;
		case SE:		CAR_Mov(CAR_ID(2)*CAR_Bounce_X ,CAR_ID(2)*CAR_Bounce_Y);		
			break;
		case SSE:			CAR_Mov(CAR_ID(1) *CAR_Bounce_X, CAR_ID(3)*CAR_Bounce_Y);		
			break;

		case S :			CAR_Mov(0, CAR_ID(4)*CAR_Bounce_Y);		
			break;
		case SSW :			CAR_Mov(-(CAR_ID(1))*CAR_Bounce_X , CAR_ID(3)*CAR_Bounce_Y);			
			break;
		case SW:		CAR_Mov(-(CAR_ID(2))*CAR_Bounce_X, CAR_ID(2)*CAR_Bounce_Y);		
			break;
		case SWW:			CAR_Mov(-(CAR_ID(3))*CAR_Bounce_X , CAR_ID(1)*CAR_Bounce_Y);		
			break;

		case W :			CAR_Mov(-(CAR_ID(4)) *CAR_Bounce_X, 0);			
			break;
		case NWW :			CAR_Mov(-(CAR_ID(3))*CAR_Bounce_X , -(CAR_ID(1))*CAR_Bounce_Y);			
			break;
		case NW:		CAR_Mov(-(CAR_ID(2))*CAR_Bounce_X, -(CAR_ID(2))*CAR_Bounce_Y);		
			break;
		case NNW:			CAR_Mov(-(CAR_ID(1))*CAR_Bounce_X , -(CAR_ID(3))*CAR_Bounce_Y);		
			break;
		}

		// calculate velocity divider
		CAR_Velocity_Divider++;
		if (CAR_Velocity_Divider>CAR_Turn_Rate) {
			CAR_Velocity_Divider=1;
		}
	} else {

		/* car is dead, draw explosion animation, or start new game if anim finished */ 

		CAR_Check_Explosion();
		CAR_Bounce=false;
	}
}


void CAR::CAR_Car_Crash() {
	/* Record race loss and set up for a new race */

	Basic_CAR::CAR_Car_Crash();

	/* decrement laps if not 0 */

	if (CAR_Laps_Completed>0) {
		CAR_Laps_Completed--;
	}
	
	/* Set variables for a new race */
	Mapx=SMapx;																						 // Starting positions
	Mapy=SMapy;																			

	CAR_Velocity=0;																				   // stop car
	CAR_Passed_CheckPoint=false;												// car has not passed checkpoint				
	CAR_Acceleration_Counter=0;														// reset acceleration counter 
	Set_Vel_BOB(&car,0,0);																	// tell BOB to stop moving 
	CAR_Direction=N;																			  // Facing North
	CAR_Velocity_Divider=0;																   // Reset Velocity divider
	CAR_Reset_Clock();																		   // Reset clock
}

void CAR::CAR_Check_Explosion() {
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

void CAR::CAR_Draw_Acceleration_Bar(int x, int y) {
	/* Draw acceleration rectangle on the backbuffer*/

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* 	x = x coord																					 */
	/*  y = y coord																				    */
	/* ----------------------------------------------------------------------------- */

	/* Draw the rectangle at x,y (top left corner), to x,y at bottom right by, 
		(x) will increment dynamically depending on CAR_Max_Speed.. the length
		of the acceleration bar is determined by CAR_Acceleration_Bar_Length.
		(y) the height of the acceleration bar (CAR_Acceleration_Bar_Height) */ 

	
	Draw_Rectangle(x,y,x+(CAR_Velocity*(CAR_Acceleration_Bar_Length/CAR_Max_Max_Speed))-1
		,y+CAR_Acceleration_Bar_Height, CAR_Acceleration_Bar_Colour, lpddsback);
	// Draw color on end of accel bar
	if(CAR_Velocity>0) {
	Draw_Rectangle(x+(CAR_Velocity*(CAR_Acceleration_Bar_Length/CAR_Max_Max_Speed))-1,y,
		x+((CAR_Velocity*(CAR_Acceleration_Bar_Length/CAR_Max_Max_Speed))-1)+CAR_Acceleration_Bar_End_Width,
		y+CAR_Acceleration_Bar_Height,CAR_Acceleration_Bar_End_Colour, lpddsback);
	}
}

void CAR::CAR_Check_Best_Lap() {
	/* check to see if the current lap is the best lap, if so, change it to the new lap time*/

	if((CAR_Get_Clock()<CAR_Best_Lap)&&(CAR_Check_CheckPoint())) {
		CAR_Best_Lap=CAR_Get_Clock();
	}
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                  Get and Set Functions                                                      */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void CAR::CAR_Passed_Start() {
	/* Car just passed the start, reset checkpoint value */
	CAR_Passed_CheckPoint=false;
};

bool CAR::CAR_Check_CheckPoint() {
	/* return the checkpoint status */
	return CAR_Passed_CheckPoint;
};

void CAR::CAR_Toggle_CheckPoint() {
	/* Toggle the checkpoint variable */
	if (CAR_Passed_CheckPoint==false) {
		CAR_Passed_CheckPoint=true;
	} else {
		CAR_Passed_CheckPoint=false;
	}
}

DWORD CAR::CAR_Get_Clock() {
	/* return current adjusted clock value */ 
	if (!CAR_Clock_Stopped) {
	CAR_Clock_Stopped_Value=Get_Clock()-CAR_Start_Clock;
	} 
	return (CAR_Clock_Stopped_Value);
}

void CAR::CAR_Reset_Clock() {
	/* reset the clock to 0 */
	CAR_Start_Clock = Start_Clock(); 
	CAR_Clock_Stopped=false;
}

int CAR::CAR_Check_Laps_Completed() {
	/* return number of laps completed */
	return CAR_Laps_Completed;
}

void CAR::CAR_Increment_Laps_Completed() {
	/* increment number of laps */
	CAR_Laps_Completed++;
}

DWORD CAR::CAR_Return_Best_Lap() {
	/* return the best lap time */

	return CAR_Best_Lap;
}

void CAR::CAR_Reset_Laps() {
	/* reset laps completed */

	CAR_Laps_Completed=0;
}

void CAR::CAR_Reset_Velocity() {
	/* reset acceleration */

	CAR_Velocity=0;
}

int CAR::CAR_Get_Velocity() {
	/* return acceleration */

	return CAR_Velocity;
}

void CAR::CAR_Reset_Direction() {
	/* Reset acceleration */

	CAR_Direction=N;
	car.curr_frame=0;
}

void CAR::CAR_Setup_Rates(int mxspd, int accel, int turn, int brake) {
	/* setup car rates */
	
	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* mxspd = max speed of car													   */
	/* accel = acceleration rate of car											  */
	/* turn = turning rate of car														    */
	/* brake = braking rate of car													  */
	/* ----------------------------------------------------------------------------- */

	// Set rates 
	CAR_Max_Speed=mxspd;
	CAR_Acceleration_Rate=accel;
	CAR_Brake_Rate=brake;
	CAR_Turn_Rate=turn;
};


void CAR::CAR_Stop_Clock() {
	/* reset the clock to 0 */
	CAR_Clock_Stopped=true;
}
