/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                    Lame.h                                                                                */
/* ===================================================================== */
/*	Various game data, change these to change the game (see also Map.h) 				  */	
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

#define MAP_MAXLAPS 8								    // Maximum laps
#define EXPLOSION_ANIM 0,1,2,3,4,5,6,7,8,9,10,11,12,13		// explosion anim sequence
#define ACCEL_RATE 10 								     // Acceleration rate (frames)
#define MAX_SPEED 7									       // Maximum speed
#define CAR_SIZE_X 36									   // Car size in pixels
#define CAR_SIZE_Y 36
#define CAR_FRAMES 16							       // Number of frames for a car 
#define EXPLOSION_X 42									// Explosion size (x and y)
#define EXPLOSION_Y 36
#define EXPLOSION_FRAMES 14					 // Number of frames in explosion
#define ANIM_SPEED 2									  // Speed of explosion animation
#define ACCEL_BAR_LEN 110						  // Length of acceleration bar
#define ACCEL_BAR_HEIGHT 29					  // Height of acceleration bar 
#define ACCEL_BAR_COL 130						  // Colour of acceleration bar
#define ACCEL_BAR_END_COL 30                // Colour of end of accel bar
#define ACCEL_BAR_END_WIDTH 3			 // Width of end of accel bar
#define LAME_VALUE 100000						    // Lame value for timers 
#define FLAG_SIZE 32										  // size in pixels of a flag
#define SCORE_COLOUR_R 0                		 // colour of scores
#define SCORE_COLOUR_G 0 
#define SCORE_COLOUR_B 0 
#define	TIME_COLOUR_R 0								 // colour of lap times
#define	TIME_COLOUR_G 0
#define	TIME_COLOUR_B 0	

#define INTRO1_BMP "Gfx\\intro.bmp"					  // Intro bitmaps
#define TITLE_SCREEN "Gfx\\INTROtitle.bmp"
#define NETWORKING "Gfx\\INTROjoin.bmp"
#define NETWORKING2 "Gfx\\INTROhost.bmp"

#define BACKGROUND_BMP "Gfx\\background.bmp"	// name of file containing background bmp
#define COLLISION "Gfx\\boundry.bmp"										 //  name of file containing collision bmp
#define FLAG_BMP "Gfx\\flag.bmp"												 // name of file containing flag bmp 
#define LIGHTS_BMP "Gfx\\lights.bmp"										 // name of file containing lights bmp 
#define INTRO_LADA_BMP "Gfx\\INTROlada.bmp"					// Carz
#define INTRO_VW_BMP "Gfx\\INTROvw.bmp"
#define INTRO_SIGMA_BMP "Gfx\\INTROsigma.bmp"
#define INTRO_DATTO_BMP "Gfx\\INTROdatto.bmp"
#define EXPLOSION_BMP "Gfx\\explosion.bmp"						  // name of file containing explosion bmp
#define DISPLAY_BMP "Gfx\\display.bmp"									 // in-game display

#define LADA_BMP "Gfx\\LADA.bmp"		// Carz
#define VW_BMP "Gfx\\VW.bmp"
#define SIGMA_BMP "Gfx\\SIGMA.bmp"
#define DATTO_BMP "Gfx\\DATTO.bmp"

#define	EXPLOSION_WAV "WAVS\\explosion.wav"		 // name of files containing sounds
#define ENGINE_WAV "WAVS\\engine.wav"
#define BOING_WAV "WAVS\\boing.wav"
#define LAP_WAV "WAVS\\lap_beep.wav"
#define LIGHTS_BEEP_WAV "WAVS\\lights_beep.wav"
#define GO_WAV "WAVS\\go_beep.wav"
#define MUSIC_WAV "WAVS\\WarDance.wav"
#define ENGINE2_WAV "WAVS\\engine2.wav"
#define BUMP_WAV "WAVS\\bump.wav"
#define SCREECH_WAV "WAVS\\skid.wav"

#define PAN_RGHT 10000									// Right Sound pan value
#define PAN_LFT -10000										// Left sound pan value

/* Lights */

#define LIGHTS_WIDTH 34									// width and height of lights
#define LIGHTS_HEIGHT 113
#define LIGHTS_FRAMES 20							  // number of frames in light animation
#define LIGHTS_ANIM	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19		// lights anim sequence
#define LIGHTS_ANIM_SPEED 7						  //  speed of lights animation (once every 7 frames)

#define DISPLAY_SIZE 800,128						   // Size of game display

#define BOUNCE	2													// number of bounces -1

#define ENGINE_FREQ 250									 //  multiplier of engine freq sound
#define	BASE_FREQ 4000								       //  base engine frequency
#define	BASE_FREQ2 8000								      //  base engine frequency2

/* Network console */

#define CONNTEXTY 270										//  x&y starting co-ords for network conn text
#define CONNTEXTX 105
#define TEXTSIZE 256											 //  length of text buffer
#define CONN_Y_INC 14										  //  network conn text y increment

/* Messages */

#define DATA 777													  // data and sync defs
#define SYNC 666

/* Car enum type */

#define DATTO 0														   // car enum type
#define VW 1
#define LADA 2
#define SIGMA 3

/* Volumes */

#define BUMP_VOL 50												// various volumes of sounds
#define ENGINE1_VOL 85
#define ENGINE2_VOL 75
#define BOING_VOL 100

/* Victory Dance */

#define VICTORY_SIZE_INC 2                              // no. of pixels to increase size by
#define MAX_VICTORY_SIZE 32							 // Max size of victory BOB

/* MIDI */

#define TITLE_MUSIC 0											// Schemes
#define GAME_MUSIC 2

/* CAR rates */

#define DATTO_RATES 6,8,3,1
#define VW_RATES 4,5,3,2
#define LADA_RATES 5,13,2,2
#define SIGMA_RATES 7,11,2,1

/* Keys */

#define VK_S 83

/* Immunity from explosion time */

#define NUM_IMMUNITY_FRAMES 60