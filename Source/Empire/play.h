/* --------------------------------------------------------------------------------------------------------------*/
/*						(*)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                     */
/* ===============================================================*/
/*                                                                 Play.h                                                                      */
/* ===============================================================*/
/*   MIDI and wav loader header file																				  */	
/* --------------------------------------------------------------------------------------------------------------*/	

#ifndef SOUNDWAV
#define SOUNDWAV

#define MIDIPATH "MIDI"
#define LAMEPATH "C:\\LC2\\MIDI\\"

void     LoadWaveFile( HWND hDlg, TCHAR* strFileName );
HRESULT  PlayBuffer( BOOL bLooped );
HRESULT  StopBuffer();
HRESULT InitDirectSound( HWND hDlg );
HRESULT HandleNotification( BOOL bLooped ) ;
HRESULT FreeDirectSound();

#endif