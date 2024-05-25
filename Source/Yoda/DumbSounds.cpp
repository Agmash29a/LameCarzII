#define WIN32_LEAN_AND_MEAN  

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

#include <ddraw.h>  // directX includes
#include <dsound.h>
#include "Gpdumb1.h"

#include "Sound.h"

// directsound stuff
LPDIRECTSOUND		lpds;           // directsound interface pointer
DSBUFFERDESC		dsbd;           // directsound description
DSCAPS				dscaps;         // directsound caps
HRESULT				dsresult;       // general directsound result
DSBCAPS				dsbcaps;        // directsound buffer caps
LPDIRECTSOUNDBUFFER	lpdsbprimary;   // the primary mixing buffer
pcm_sound			sound_fx[MAX_SOUNDS];    // the array of secondary sound buffers

WAVEFORMATEX		pcmwf;          // generic waveformat structure

int Load_VOC(char *filename)
{
// this function loads a .voc file, sets up the directsound buffer and loads the data
// into memory, the function returns the id number of the sound

int	sound_id = -1,       // id of sound to be loaded
	index,               // looping variable
	data_offset,         // offset to data part of file
	playback_rate,       // playback rate as encoded in file
	data_length;         // length of data

ULONG  bytesread = 0,    // actual number of bytes read during file read
       filelength;       // length of file

int    file_handle;      // general file handle

UCHAR *snd_buffer,       // temporary sound buffer to hold voc data
      *audio_ptr_1=NULL, // data ptr to first write buffer 
	  *audio_ptr_2=NULL; // data ptr to second write buffer

DWORD audio_length_1=0,  // length of first write buffer
	  audio_length_2=0;  // length of second write buffer
			
// step one: are there any open id's ?
for (index=0; index < MAX_SOUNDS; index++)
	{	
    // make sure this sound is unused
	if (sound_fx[index].state==SOUND_NULL)
	   {
	   sound_id = index;
	   break;
	   } // end if

	} // end for index

// did we get a free id?
if (sound_id==-1)
	return(-1);

// step two: load the voc file off disk
if ((file_handle = _open(filename,_O_BINARY | _O_RDONLY))==-1)
	return(0);

// get size of file so we can allocate temporary read buffer
filelength = _filelength(file_handle);

// allocate a large enough temporary buffer
snd_buffer = (UCHAR *)malloc(filelength);

// now read in the data
bytesread = _read(file_handle, snd_buffer, filelength);

// access all values and decode VOC encoding for data fields
data_offset	  = snd_buffer[20];
playback_rate = (-1000000/(snd_buffer[data_offset+4]-256));
data_length   = ((*(int *)(snd_buffer+data_offset)) >> 8);

// set rate and size in data structure
sound_fx[sound_id].rate  = playback_rate;
sound_fx[sound_id].size  = data_length;
sound_fx[sound_id].state = SOUND_LOADED;

// close the file
_close(file_handle);

// step three: create the sound buffer and copy voc data into buffer

// set up the format data structure
memset(&pcmwf, 0, sizeof(WAVEFORMATEX));

pcmwf.wFormatTag	  = WAVE_FORMAT_PCM;  // pulse code modulation
pcmwf.nChannels		  = 1;                // mono 
pcmwf.nSamplesPerSec  = 11025;            // always this rate
pcmwf.nBlockAlign	  = 1;                
pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
pcmwf.wBitsPerSample  = 8;
pcmwf.cbSize		  = 0;

// prepare to create sounds buffer
dsbd.dwSize			= sizeof(DSBUFFERDESC);
dsbd.dwFlags		= DSBCAPS_CTRLPAN| DSBCAPS_CTRLPAN| DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
dsbd.dwBufferBytes	= data_length-NVB_SIZE;
dsbd.lpwfxFormat	= &pcmwf;

// create the sound buffer
if (lpds->CreateSoundBuffer(&dsbd,&sound_fx[sound_id].dsbuffer,NULL)!=DS_OK)
   {
   // release memory
   free(snd_buffer);

   // return error
   return(-1);
   } // end if

// copy data into sound buffer
if (sound_fx[sound_id].dsbuffer->Lock(0,					 
								      data_length-NVB_SIZE,			
								      (void **) &audio_ptr_1, 
								      &audio_length_1,
								      (void **)&audio_ptr_2, 
								      &audio_length_2,
								      DSBLOCK_FROMWRITECURSOR)!=DS_OK)
								 return(0);

// copy first section of circular buffer
memcpy(audio_ptr_1, snd_buffer+data_offset+NVB_SIZE, audio_length_1);

// copy last section of circular buffer
memcpy(audio_ptr_2, (snd_buffer+data_offset+NVB_SIZE+audio_length_1),audio_length_2);

// unlock the buffer
if (sound_fx[sound_id].dsbuffer->Unlock(audio_ptr_1, 
									    audio_length_1, 
									    audio_ptr_2, 
									    audio_length_2)!=DS_OK)
 							     return(0);

// release the temp buffer
free(snd_buffer);

// return id
return(sound_id);

} // end Load_Voc

///////////////////////////////////////////////////////////

int Load_WAV(char *filename, int control_flags)
{
// this function loads a .wav file, sets up the directsound 
// buffer and loads the data into memory, the function returns 
// the id number of the sound


HMMIO 			hwav;    // handle to wave file
MMCKINFO		parent,  // parent chunk
                child;   // child chunk
WAVEFORMATEX    wfmtx;   // wave format structure

int	sound_id = -1,       // id of sound to be loaded
	index;               // looping variable

UCHAR *snd_buffer,       // temporary sound buffer to hold voc data
      *audio_ptr_1=NULL, // data ptr to first write buffer 
	  *audio_ptr_2=NULL; // data ptr to second write buffer

DWORD audio_length_1=0,  // length of first write buffer
	  audio_length_2=0;  // length of second write buffer
			
// step one: are there any open id's ?
for (index=0; index < MAX_SOUNDS; index++)
	{	
    // make sure this sound is unused
	if (sound_fx[index].state==SOUND_NULL)
	   {
	   sound_id = index;
	   break;
	   } // end if

	} // end for index

// did we get a free id?
if (sound_id==-1)
	return(-1);

// set up chunk info structure
parent.ckid 	    = (FOURCC)0;
parent.cksize 	    = 0;
parent.fccType	    = (FOURCC)0;
parent.dwDataOffset = 0;
parent.dwFlags		= 0;

// copy data
child = parent;

// open the WAV file
if ((hwav = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF))==NULL)
    return(-1);

// descend into the RIFF 
parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

if (mmioDescend(hwav, &parent, NULL, MMIO_FINDRIFF))
    {
    // close the file
    mmioClose(hwav, 0);

    // return error, no wave section
    return(-1); 	
    } // end if

// descend to the WAVEfmt 
child.ckid = mmioFOURCC('f', 'm', 't', ' ');

if (mmioDescend(hwav, &child, &parent, 0))
    {
    // close the file
    mmioClose(hwav, 0);

    // return error, no format section
    return(-1); 	
    } // end if

// now read the wave format information from file
if (mmioRead(hwav, (char *)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx))
    {
    // close file
    mmioClose(hwav, 0);

    // return error, no wave format data
    return(-1);
    } // end if

// make sure that the data format is PCM
if (wfmtx.wFormatTag != WAVE_FORMAT_PCM)
    {
    // close the file
    mmioClose(hwav, 0);

    // return error, not the right data format
    return(-1); 
    } // end if

// now ascend up one level, so we can access data chunk
if (mmioAscend(hwav, &child, 0))
   {
   // close file
   mmioClose(hwav, 0);

   // return error, couldn't ascend
   return(-1); 	
   } // end if

// descend to the data chunk 
child.ckid = mmioFOURCC('d', 'a', 't', 'a');

if (mmioDescend(hwav, &child, &parent, MMIO_FINDCHUNK))
    {
    // close file
    mmioClose(hwav, 0);

    // return error, no data
    return(-1); 	
    } // end if

// finally!!!! now all we have to do is read the data in and
// set up the directsound buffer

// allocate the memory to load sound data
snd_buffer = (UCHAR *)malloc(child.cksize);

// read the wave data 
mmioRead(hwav, (char *)snd_buffer, child.cksize);

// close the file
mmioClose(hwav, 0);

// set rate and size in data structure
sound_fx[sound_id].rate  = wfmtx.nSamplesPerSec;
sound_fx[sound_id].size  = child.cksize;
sound_fx[sound_id].state = SOUND_LOADED;

// set up the format data structure
memset(&pcmwf, 0, sizeof(WAVEFORMATEX));

pcmwf.wFormatTag	  = WAVE_FORMAT_PCM;  // pulse code modulation
pcmwf.nChannels		  = 1;                // mono 
pcmwf.nSamplesPerSec  = 11025;            // always this rate
pcmwf.nBlockAlign	  = 1;                
pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
pcmwf.wBitsPerSample  = 8;
pcmwf.cbSize		  = 0;

// prepare to create sounds buffer
dsbd.dwSize			= sizeof(DSBUFFERDESC);
dsbd.dwFlags		= control_flags | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
dsbd.dwBufferBytes	= child.cksize;
dsbd.lpwfxFormat	= &pcmwf;

// create the sound buffer
if (lpds->CreateSoundBuffer(&dsbd,&sound_fx[sound_id].dsbuffer,NULL)!=DS_OK)
   {
   // release memory
   free(snd_buffer);

   // return error
   return(-1);
   } // end if

// copy data into sound buffer
if (sound_fx[sound_id].dsbuffer->Lock(0,					 
								      child.cksize,			
								      (void **) &audio_ptr_1, 
								      &audio_length_1,
								      (void **)&audio_ptr_2, 
								      &audio_length_2,
								      DSBLOCK_FROMWRITECURSOR)!=DS_OK)
								 return(0);

// copy first section of circular buffer
memcpy(audio_ptr_1, snd_buffer, audio_length_1);

// copy last section of circular buffer
memcpy(audio_ptr_2, (snd_buffer+audio_length_1),audio_length_2);

// unlock the buffer
if (sound_fx[sound_id].dsbuffer->Unlock(audio_ptr_1, 
									    audio_length_1, 
									    audio_ptr_2, 
									    audio_length_2)!=DS_OK)
 							     return(0);

// release the temp buffer
free(snd_buffer);

// return id
return(sound_id);

} // end Load_WAV

///////////////////////////////////////////////////////////

int Replicate_Sound(int source_id)
{
// this function replicates the sent sound and sends back the
// id of the replicated sound, you would use this function
// to make multiple copies of a gunshot or something that
// you want to play multiple times simulataneously, but you
// only want to load once

if (source_id!=-1)
    {
    // duplicate the sound buffer
    // first hunt for an open id

    for (int id=0; id < MAX_SOUNDS; id++)
        {
        // is this sound open?
        if (sound_fx[id].state==SOUND_NULL)
            {
            // first make an identical copy
            sound_fx[id] = sound_fx[source_id];

            // now actually replicate the directsound buffer
            if (lpds->DuplicateSoundBuffer(sound_fx[source_id].dsbuffer,
                                           &sound_fx[id].dsbuffer)!=DS_OK)
                {
                // reset sound to NULL
                sound_fx[id].dsbuffer = NULL;
                sound_fx[id].state    = SOUND_NULL;

                // return error
                return(-1);
                } // end if

            // now fix up id
            sound_fx[id].id = id;
            
            // return replicated sound
            return(id);

            } // end if found
  
        } // end for id

    } // end if
else
   return(-1);
    
// else failure
return(-1);

} // end 







///////////////////////////////////////////////////////////

int DSound_Init(void)
{
// this function initializes the sound system
static int first_time = 1; // used to track the first time the function
                           // is entered

// test for very first time
if (first_time)
	{		
	// clear everything out
	memset(sound_fx,0,sizeof(pcm_sound)*MAX_SOUNDS);
	
	// reset first time
	first_time = 0;

	// create a directsound object
	if (DirectSoundCreate(NULL, &lpds, NULL)!=DS_OK )
		return(0);

	// set cooperation level
	if (lpds->SetCooperativeLevel((HWND)main_window_handle,DSSCL_NORMAL)!=DS_OK)
		return(0);

	} // end if

// initialize the sound fx array
for (int index=0; index<MAX_SOUNDS; index++)
	{
	// test if this sound has been loaded
	if (sound_fx[index].dsbuffer)
		{
		// stop the sound
		sound_fx[index].dsbuffer->Stop();

		// release the buffer
		sound_fx[index].dsbuffer->Release();
	
		} // end if

	// clear the record out
	memset(&sound_fx[index],0,sizeof(pcm_sound));

	// now set up the fields
	sound_fx[index].state = SOUND_NULL;
	sound_fx[index].id    = index;

	} // end for index

// return sucess
return(1);

} // end DSound_Init

///////////////////////////////////////////////////////////

int DSound_Shutdown(void)
{
// this function releases all the memory allocated and the directsound object
// itself

// first turn all sounds off
Stop_All_Sounds();

// now release all sound buffers
for (int index=0; index<MAX_SOUNDS; index++)
	if (sound_fx[index].dsbuffer)
		sound_fx[index].dsbuffer->Release();

// now release the directsound interface itself
lpds->Release();

// return success
return(1);

} // end DSound_Shutdown

///////////////////////////////////////////////////////////

int Play_Sound(int id, int flags, int volume, int rate, int pan)
{
// this function plays a sound, the only parameter that 
// works is the flags which can be 0 to play once or
// DSBPLAY_LOOPING

if (sound_fx[id].dsbuffer)
	{
	// reset position to start
	if (sound_fx[id].dsbuffer->SetCurrentPosition(0)!=DS_OK)
		return(0);
	
	// play sound
	if (sound_fx[id].dsbuffer->Play(0,0,flags)!=DS_OK)
		return(0);
	} // end if

// return success
return(1);

} // end Play_Sound

///////////////////////////////////////////////////////////

int Set_Sound_Volume(int id,int vol)
{
// this function sets the volume on a sound 0-100

if (sound_fx[id].dsbuffer->SetVolume(DSVOLUME_TO_DB(vol))!=DS_OK)
	return(0);

// return success
return(1);

} // end Set_Sound_Volume

///////////////////////////////////////////////////////////

int Set_Sound_Freq(int id,int freq)
{
// this function sets the playback rate

if (sound_fx[id].dsbuffer->SetFrequency(freq)!=DS_OK)
	return(0);

// return success
return(1);

} // end Set_Sound_Freq

///////////////////////////////////////////////////////////

int Set_Sound_Pan(int id,int pan)
{
// this function sets the pan, -10,000 to 10,0000

if (sound_fx[id].dsbuffer->SetPan(pan)!=DS_OK)
	return(0);

// return success
return(1);

} // end Set_Sound_Pan

////////////////////////////////////////////////////////////

int Stop_Sound(int id)
{
// this function stops a sound from playing
if (sound_fx[id].dsbuffer)
   {
   sound_fx[id].dsbuffer->Stop();
   sound_fx[id].dsbuffer->SetCurrentPosition(0);
   } // end if

// return success
return(1);

} // end Stop_Sound

///////////////////////////////////////////////////////////

int Delete_All_Sounds(void)
{
// this function deletes all the sounds

for (int index=0; index < MAX_SOUNDS; index++)
    Delete_Sound(index);

// return success always
return(1);

} // end Delete_All_Sounds

///////////////////////////////////////////////////////////

int Delete_Sound(int id)
{
// this function deletes a single sound and puts it back onto the available list

// first stop it
if (!Stop_Sound(id))
   return(0);

// now delete it
if (sound_fx[id].dsbuffer)
   {
   // release the com object
   sound_fx[id].dsbuffer->Release();
   sound_fx[id].dsbuffer = NULL;
   
   // return success
   return(1);
   } // end if

// return success
return(1);

} // end Delete_Sound

///////////////////////////////////////////////////////////

int Stop_All_Sounds(void)
{
// this function stops all sounds

for (int index=0; index<MAX_SOUNDS; index++)
	Stop_Sound(index);	

// return success
return(1);

} // end Stop_All_Sounds

///////////////////////////////////////////////////////////

int Status_Sound(int id)
{
// this function returns the status of a sound
if (sound_fx[id].dsbuffer)
	{
	ULONG status; 

	// get the status
	sound_fx[id].dsbuffer->GetStatus(&status);

	// return the status
	return(status);

	} // end if
else // total failure
	return(-1);

} // end Status_Sound

///////////////////////////////////////////////////////////


int Collision_Test(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
// this function tests if the two rects overlap

// get the radi of each rect
int width1  = (w1>>1) - (w1>>3);
int height1 = (h1>>1) - (h1>>3);

int width2  = (w2>>1) - (w2>>3);
int height2 = (h2>>1) - (h2>>3);

// compute center of each rect
int cx1 = x1 + width1;
int cy1 = y1 + height1;

int cx2 = x2 + width2;
int cy2 = y2 + height2;

// compute deltas
int dx = abs(cx2 - cx1);
int dy = abs(cy2 - cy1);

// test if rects overlap
if (dx < (width1+width2) && dy < (height1+height2))
   return(1);
else
// else no collision
return(0);

} // end Collision_Test

