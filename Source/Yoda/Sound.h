/* --------------------------------------------------------------------------------------------------------------*/
/*						(*)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                     */
/* ===============================================================*/
/*                                                                Sound.h                                                                    */
/* ===============================================================*/
/*   Just the sounds from GPDUMB2																				  */	
/* --------------------------------------------------------------------------------------------------------------*/

// GPDUMB2.H - Header file for GPDUMB2.CPP game engine library

// watch for multiple inclusions
#ifndef SOUND
#define SOUND
// DEFINES ////////////////////////////////////////////////

#define MAX_SOUNDS     64 // max number of sounds in system at once 

#define SOUND_NULL     0
#define SOUND_LOADED   1
#define SOUND_PLAYING  2
#define SOUND_STOPPED  3

// voc file defines
#define NVB_SIZE       6  // size of new voice block in bytes

// MACROS /////////////////////////////////////////////////

#define DSVOLUME_TO_DB(volume) ((DWORD)(-30*(100 - volume)))

// TYPES //////////////////////////////////////////////////

// this holds a single sound
typedef struct pcm_sound_typ
	{
	LPDIRECTSOUNDBUFFER dsbuffer;   // the ds buffer containing the sound
	int state;                      // state of the sound
	int rate;                       // playback rate
	int size;                       // size of sound
	int id;                         // id number of the sound
	} pcm_sound, *pcm_sound_ptr;

// PROTOTYPES /////////////////////////////////////////////

int Collision_Test(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
// sound
int Load_VOC(char *filename);
//int Load_WAV(char *filename, int control_flags = DSBCAPS_CTRLDEFAULT);
int Load_WAV(char *filename, int control_flags = DSBCAPS_CTRLPAN| DSBCAPS_CTRLFREQUENCY| DSBCAPS_CTRLVOLUME);
int Replicate_Sound(int source_id);
int Play_Sound(int id, int flags=0,int volume=0, int rate=0, int pan=0);
int Stop_Sound(int id);
int Stop_All_Sounds(void);
int DSound_Init(void);
int DSound_Shutdown(void);
int Delete_Sound(int id);
int Delete_All_Sounds(void);
int Status_Sound(int id);
int Set_Sound_Volume(int id,int vol);
int Set_Sound_Freq(int id,int freq);
int Set_Sound_Pan(int id,int pan);


// EXTERNALS //////////////////////////////////////////////

extern HWND main_window_handle; // save the window handle
extern HINSTANCE main_instance; // save the instance

extern LPDIRECTSOUND		lpds;           // directsound interface pointer
extern DSBUFFERDESC			dsbd;           // directsound description
extern DSCAPS				dscaps;         // directsound caps
extern HRESULT				dsresult;       // general directsound result
extern DSBCAPS				dsbcaps;        // directsound buffer caps

extern LPDIRECTSOUNDBUFFER	lpdsbprimary;   // the primary mixing buffer
extern pcm_sound			sound_fx[MAX_SOUNDS];    // the array of secondary sound buffers

extern WAVEFORMATEX			pcmwf;          // generic waveformat structure

#endif
