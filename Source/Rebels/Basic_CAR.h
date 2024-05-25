/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                 Basic_CAR.h                                                                        */
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

#ifndef BASIC_CAR
#define BASIC_CAR

class Basic_CAR {

	int CAR_Score;											     // Number of games won								
	int CAR_Size_x;												 // x pixel an y pixel length of car (eg 32x32)
	int CAR_Size_y;			
	int CAR_Explosion_Size_x;			     	     // x pixel length of explosion (eg 32)
	int CAR_Explosion_Size_y;			     	     // y pixel length of explosion (eg 32)
	int CAR_Explosion_Frames;					   // number of frames in explosion
	int CAR_Explosion_Speed;					    // Speed of explosion animation
	int *CAR_Anim_Explosion;					      // frame sequence for animation
	int CAR_Frames;											    // no. of frames for the car

public:

	BOB car;															//	Main Car BOB
	BOB car_picture1;										   // Secondary picture (for flag and velocity bars)	
	BOB explosion;											     //  Explosion Animation

	int CAR_Position_x;									      // Starting position of car
	int CAR_Position_y;

	int SMapx;															// Starting Map Position
	int SMapy;

	bool explosion_happening;                        // explosion booleans
	bool Wait_Frame_Explosion;

	bool CAR_Is_Car_Alive;						         // Has car crashed?

	Basic_CAR(int crszx, int crszy,  int expszx, int expszy, int expframs, int *animseq,
						  int animspd, int crfrms);
	~Basic_CAR();

	void CAR_Load_BOB(char *bitmap_name_car, char *bitmap_name_exp);
	void CAR_Draw_Car_Picture1(int frame, int x, int y);
	void CAR_Inc_Score();
	int CAR_Get_Score();
	bool CAR_Check_Status();
	void CAR_Dead();
	void CAR_Draw_Explosion();
	void CAR_Draw_Car();
	void CAR_Car_Crash(); 
};

#endif