/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                         Map.h                                                                              */
/* ===================================================================== */
/* Various map data, change these to change the map (see also Lame.h)			      	    */	
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

/* MAP_Elements */

// Map collosion data, a series of collision rectangles to check 
#define MAP_COLLISION_DATA  0,0,32,416						// Not impl.       
#define MAP_STARTING_LINE 820,605,124,31			    //  Starting line
#define MAP_CHECKPOINT 1564,1462, 32, 124				 //  Checkpoint

/* MAP_Coords */

#define MAP_PICTURE1 430,533										    // Car 1 position (flag bar)
#define MAP_CAR1_ACCEL_BAR 81,532						     // positions of accel bars
#define MAP_CAR1_LAP_TIME 295,542							   // positions of lap and best lap times
#define MAP_CAR1_BEST_LAP_TIME 295,507
#define MAP_LIGHT_GREEN 150,288									  // position of lights
#define MAP1_RIGHT_STARTX 895									   // car starting positions
#define MAP1_RIGHT_STARTY 638	   						    
#define MAP1_LEFT_STARTX 830
#define MAP1_LEFT_STARTY 638
#define MAP_CAR1_SCORE 472,499										// car score positions
#define MAP_CAR2_SCORE 642,499
#define DISPLAYX_POS 64															// Display bar position
#define DISPLAYY_POS 480

#define MIDDLE_SCREEN 383,283											// Car screen positions
#define MIDDLE_SCREEN_X 383
#define	MIDDLE_SCREEN_Y 283	
#define SCREEN_POS_JOIN 324,283
#define SCREEN_POS_HOST 448,283

/* Large Map1 dimensions */

#define MAP1_WIDTH	2560
#define MAP1_HEIGHT 1920

/* Other */

#define MAP_NUMBER_COLLISION_REGIONS 0			  // number of collision rectangles

/* Types */

typedef struct MAP_ELEMENT_TYP {
 	int x;
	int y;
	int width;
	int height;
} MAP_Element;

typedef struct MAP_COORDS_TYP {
	int x;
	int y;
} MAP_Coords;
