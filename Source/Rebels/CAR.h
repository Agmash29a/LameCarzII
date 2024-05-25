/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                      CAR.h                                                                                */
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

#include "Basic_CAR.h"

class CAR : public Basic_CAR {

	int CAR_Direction;												// Direction that car is travelling (see enumerated type) 
	int CAR_Velocity;		  										 // Velocity of car		
	int CAR_Velocity_Divider;								// Counter for dividing velocity by VELOCITY_DIVISION
	int CAR_Engine_Multiplier;					            // Engine speed of car (For SFX)
	int CAR_Frame;											

	int CAR_Acceleration_Bar_Height;			   // Height of the acceleration bar
	int CAR_Acceleration_Bar_Length;			   // Length of the acceleration bar
	int CAR_Acceleration_Bar_End_Colour;	 // Colour of  end of accel bar
	int CAR_Acceleration_Bar_End_Width;		  // Width of end of accel bar		
	int CAR_Acceleration_Bar_Colour;				// Colour of acceleration bar

	int CAR_Lame_Value;										   // Max timer value

	DWORD CAR_Current_Clock;		   	      // Current clock variable (for lap time and best lap) 
	DWORD CAR_Start_Clock;                       // Initial clock value (time = current - start clock);
	DWORD CAR_Best_Lap;							  // Best Lap time.

	enum CAR_Key  {LEFT,RIGHT, NONE};
	int CAR_Key;													// Key pressed (enum)
	int KEY_RIGHT;												// Key codes
	int KEY_LEFT;
	int KEY_BRAKE;
	int KEY_ACCEL;

	int CAR_Clock_Stopped_Value;			   // Clock values
	bool CAR_Clock_Stopped;

	enum CAR_Direction { N, NNE,NE,NEE,E,SEE,SE,SSE,S,SSW,SW,SWW,W,NWW,NW,NNW };

	void CAR_Car_Crash();
	int CAR_ID(int div);
	void CAR_Mov(int x,int y);

public:

	int Mapx;															// Map coords
	int Mapy;

	int CAR_Bounce_Count;                             // Car bounces for this long
	int CAR_Bounce_X ;                                     // neg or pos multipliers for direction
	int CAR_Bounce_Y ;  
	bool CAR_Bounce;									  // Has car bounced off wall?
	bool CAR_Bounce_First;                            // First frame of bounce?
	bool CAR_Bounce_NORTH;					  // which side(s) did it bounce off 
	bool CAR_Bounce_WEST;
    bool CAR_Bounce_SOUTH;
	bool CAR_Bounce_EAST;

	int CAR_Acceleration_Counter;			  // Car accelerates at CAR_Accel_Rate * Frame, for 1 increment  

	int CAR_Max_Speed;								  // Maximum speed of car
	int CAR_Max_Max_Speed;					   // Max speed of any car

	int CAR_Acceleration_Rate;					 // see above
	int CAR_Brake_Rate;
	int CAR_Turn_Rate;

	bool CAR_Passed_CheckPoint;			  // Has passed checkpoint? 
	int CAR_Laps_Completed;					     // Laps completed

	CAR(int left, int right, int brake, int accel, int accel_rate,
			   int mxspd, int crszx, int crszy,  int expszx, int expszy, int expframs, int *animseq,
			   int animspd, int accbarlen, int accbarhght, int barcol, int lame, int crfrms, int accbarendcol,
			   int accbarendlen);

	void CAR_Check_Direction();
	void CAR_Passed_Start();
	bool CAR_Check_CheckPoint();
	DWORD CAR_Get_Clock(); 
	void CAR_Reset_Clock();
	void CAR_Toggle_CheckPoint();
	int CAR_Check_Laps_Completed();
	void CAR_Increment_Laps_Completed();
	void CAR_Draw_Acceleration_Bar(int x, int y);
	void CAR_Check_Best_Lap();
	DWORD CAR_Return_Best_Lap();
	void CAR_Reset_Laps();
	void CAR_Reset_Velocity();
	int CAR_Get_Velocity();
	void CAR_Reset_Direction();
	void CAR_Setup_Rates(int mxspd, int accel, int turn, int brake);
	void CAR_Stop_Clock();
	void CAR_Check_Explosion();
};
