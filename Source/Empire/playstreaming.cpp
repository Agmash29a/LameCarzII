/* --------------------------------------------------------------------------------------------------------------*/
/*						(*)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                      */
/* ===============================================================*/
/*                                                               Music.cpp                                                                 */
/* ===============================================================*/
/*  Code for streaming wav loader and midi player                                                      */	
/* --------------------------------------------------------------------------------------------------------------*/	

#include <objbase.h>
#include <commdlg.h>
#include <mmreg.h>
#include <dsound.h>
#include <stdio.h>

#include <direct.h>
#include <dmusicc.h>
#include <dmusici.h>
#include "main.h"

#include "WavRead.h"
#include "play.h"

HRESULT CreateStreamingBuffer();
HRESULT UpdateProgress();
HRESULT FillBuffer( BOOL bLooped );
HRESULT WriteToBuffer( BOOL bLooped, VOID* pbBuffer, DWORD dwBufferLength );
HRESULT RestoreBuffers( BOOL bLooped );

static BOOL GetSearchPath(WCHAR wszPath[MAX_PATH]);

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define NUM_PLAY_NOTIFICATIONS  16

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

LPDIRECTSOUND       g_pDS            = NULL;
LPDIRECTSOUNDBUFFER g_pDSBuffer      = NULL;
LPDIRECTSOUNDNOTIFY g_pDSNotify      = NULL;
CWaveSoundRead*     g_pWaveSoundRead = NULL;

DSBPOSITIONNOTIFY   g_aPosNotify[ NUM_PLAY_NOTIFICATIONS + 1 ];  

HANDLE              g_hNotificationEvents[2];


DWORD               g_dwBufferSize;
DWORD               g_dwNotifySize;
DWORD               g_dwNextWriteOffset;
DWORD               g_dwProgress;
DWORD               g_dwLastPos;
BOOL                g_bFoundEnd;

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                             Streaming WAV loader                                                         */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

//-----------------------------------------------------------------------------
// Name: InitDirectSound()
// Desc: Initilizes DirectSound
//-----------------------------------------------------------------------------

HRESULT InitDirectSound( HWND hDlg )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    ZeroMemory( &g_aPosNotify, sizeof(DSBPOSITIONNOTIFY) * 
                               (NUM_PLAY_NOTIFICATIONS + 1) );
    g_dwBufferSize      = 0;
    g_dwNotifySize      = 0;
    g_dwNextWriteOffset = 0;
    g_dwProgress        = 0;
    g_dwLastPos         = 0;
    g_bFoundEnd         = FALSE;

    // Initialize COM
    if( hr = CoInitialize(NULL) )
        return hr;

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate( NULL, &g_pDS, NULL ) ) )
        return hr;

    // Set coop level to DSSCL_PRIORITY
    if( FAILED( hr = g_pDS->SetCooperativeLevel( hDlg, DSSCL_PRIORITY ) ) )
        return hr;

    // Get the primary buffer 
    DSBUFFERDESC        dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return hr;

    // Set primary buffer format to 22kHz and 16-bit output.
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = 2; 
    wfx.nSamplesPerSec  = 22050; 
    wfx.wBitsPerSample  = 16; 
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return hr;

    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FreeDirectSound()
// Desc: Releases DirectSound 
//-----------------------------------------------------------------------------
HRESULT FreeDirectSound()
{
    if( NULL != g_pWaveSoundRead )
    {
        g_pWaveSoundRead->Close();
        SAFE_DELETE( g_pWaveSoundRead );
    }

    // Release DirectSound interfaces
    SAFE_RELEASE( g_pDSNotify );
    SAFE_RELEASE( g_pDSBuffer );
    SAFE_RELEASE( g_pDS ); 

    // Release COM
    CoUninitialize();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadWaveFile()
// Desc: Loads a wave file into a secondary streaming DirectSound buffer
//-----------------------------------------------------------------------------
VOID LoadWaveFile( HWND hDlg, TCHAR* strFileName )
{
    // Close and delete any existing open wave file
    if( NULL != g_pWaveSoundRead )
    {
        g_pWaveSoundRead->Close();
        SAFE_DELETE( g_pWaveSoundRead );
    }

    g_pWaveSoundRead = new CWaveSoundRead();

    // Load the wave file
    if( FAILED( g_pWaveSoundRead->Open( strFileName ) ) )
    {
       
    }
    else // The load call succeeded
    {
        // Start with a new sound buffer object
        SAFE_RELEASE( g_pDSNotify );
        SAFE_RELEASE( g_pDSBuffer );
        ZeroMemory( &g_aPosNotify, sizeof(DSBPOSITIONNOTIFY) );

        // Create the sound buffer object from the wave file data
        if( FAILED( CreateStreamingBuffer() ) )
        {
            //SetFileUI( hDlg, TEXT("Couldn't create sound buffer.") );
        }
        else // The sound buffer was successfully created
        {
            // Update the UI controls to show the sound as the file is loaded

         //   SetFileUI( hDlg, strFileName );
        }
    }
}

//-----------------------------------------------------------------------------
// Name: CreateStreamingBuffer()
// Desc: Creates a streaming buffer, and the notification events to handle
//       filling it as sound is played
//-----------------------------------------------------------------------------
HRESULT CreateStreamingBuffer()
{
    HRESULT hr; 

    // This samples works by dividing a 3 second streaming buffer into 
    // NUM_PLAY_NOTIFICATIONS (or 16) pieces.  it creates a notification for each
    // piece and when a notification arrives then it fills the circular streaming 
    // buffer with new wav data over the sound data which was just played

    // The size of wave data is in pWaveFileSound->m_ckIn
    DWORD nBlockAlign = (DWORD)g_pWaveSoundRead->m_pwfx->nBlockAlign;
    INT nSamplesPerSec = g_pWaveSoundRead->m_pwfx->nSamplesPerSec;

    // The g_dwNotifySize should be an integer multiple of nBlockAlign
    g_dwNotifySize = nSamplesPerSec * 3 * nBlockAlign / NUM_PLAY_NOTIFICATIONS;
    g_dwNotifySize -= g_dwNotifySize % nBlockAlign;   

    // The buffersize should approximately 3 seconds of wav data
    g_dwBufferSize  = g_dwNotifySize * NUM_PLAY_NOTIFICATIONS;
    g_bFoundEnd     = FALSE;
    g_dwProgress    = 0;
    g_dwLastPos     = 0;

    // Set up the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = g_dwBufferSize;
    dsbd.lpwfxFormat   = g_pWaveSoundRead->m_pwfx;

    // Create a DirectSound buffer
    if( FAILED( hr = g_pDS->CreateSoundBuffer( &dsbd, &g_pDSBuffer, NULL ) ) )
        return hr;

    // Create a notification event, for when the sound stops playing
    if( FAILED( hr = g_pDSBuffer->QueryInterface( IID_IDirectSoundNotify, 
                                                  (VOID**)&g_pDSNotify ) ) )
        return hr;

    for( INT i = 0; i < NUM_PLAY_NOTIFICATIONS; i++ )
    {
        g_aPosNotify[i].dwOffset = (g_dwNotifySize * i) + g_dwNotifySize - 1;
        g_aPosNotify[i].hEventNotify = g_hNotificationEvents[0];             
    }
    
    g_aPosNotify[i].dwOffset     = DSBPN_OFFSETSTOP;
    g_aPosNotify[i].hEventNotify = g_hNotificationEvents[1];

    // Tell DirectSound when to notify us. the notification will come in the from 
    // of signaled events that are handled in WinMain()
    if( FAILED( hr = g_pDSNotify->SetNotificationPositions( NUM_PLAY_NOTIFICATIONS + 1, 
                                                            g_aPosNotify ) ) )
        return hr;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: Play the DirectSound buffer
//-----------------------------------------------------------------------------
HRESULT PlayBuffer( BOOL bLooped )
{
    HRESULT hr;
    VOID*   pbBuffer = NULL;

    if( NULL == g_pDSBuffer )
        return E_FAIL;

    // Restore the buffers if they are lost
    if( FAILED( hr = RestoreBuffers( bLooped ) ) )
        return hr;

    // Fill the entire buffer with wave data
    if( FAILED( hr = FillBuffer( bLooped ) ) )
        return hr;

    // Always play with the LOOPING flag since the streaming buffer
    // wraps around before the entire WAV is played
    if( FAILED( hr = g_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING ) ) )
        return hr;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FillBuffer()
// Desc: Fills the DirectSound buffer with wave data
//-----------------------------------------------------------------------------
HRESULT FillBuffer( BOOL bLooped )
{
    HRESULT hr;
    VOID*   pbBuffer = NULL;
    DWORD   dwBufferLength;

    if( NULL == g_pDSBuffer )
        return E_FAIL;

    g_bFoundEnd = FALSE;
    g_dwNextWriteOffset = 0; 
    g_dwProgress = 0;
    g_dwLastPos  = 0;

    // Reset the wav file to the beginning
    g_pWaveSoundRead->Reset();
    g_pDSBuffer->SetCurrentPosition( 0 );

    // Lock the buffer down, 
    if( FAILED( hr = g_pDSBuffer->Lock( 0, g_dwBufferSize, 
                                        &pbBuffer, &dwBufferLength, 
                                        NULL, NULL, 0L ) ) )
        return hr;

    // Fill the buffer with wav data 
    if( FAILED( hr = WriteToBuffer( bLooped, pbBuffer, dwBufferLength ) ) )
        return hr;

    // Now unlock the buffer
    g_pDSBuffer->Unlock( pbBuffer, dwBufferLength, NULL, 0 );

    g_dwNextWriteOffset = dwBufferLength; 
    g_dwNextWriteOffset %= g_dwBufferSize; // Circular buffer

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: WriteToBuffer()
// Desc: Writes wave data to the streaming DirectSound buffer 
//-----------------------------------------------------------------------------
HRESULT WriteToBuffer( BOOL bLooped, VOID* pbBuffer, DWORD dwBufferLength )
{
    HRESULT hr;
    UINT nActualBytesWritten;

    if( !g_bFoundEnd )
    {
        // Fill the DirectSound buffer with WAV data
        if( FAILED( hr = g_pWaveSoundRead->Read( dwBufferLength, 
                                                 (BYTE*) pbBuffer, 
                                                 &nActualBytesWritten ) ) )           
            return hr;
    }
    else
    {
        // Fill the DirectSound buffer with silence
        FillMemory( pbBuffer, dwBufferLength, 
                    (BYTE)( g_pWaveSoundRead->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        nActualBytesWritten = dwBufferLength;
    }

    // If the number of bytes written is less than the 
    // amount we requested, we have a short file.
    if( nActualBytesWritten < dwBufferLength )
    {
        if( !bLooped ) 
        {
            g_bFoundEnd = TRUE;

            // Fill in silence for the rest of the buffer.
            FillMemory( (BYTE*) pbBuffer + nActualBytesWritten, 
                        dwBufferLength - nActualBytesWritten, 
                        (BYTE)(g_pWaveSoundRead->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
        }
        else
        {
            // We are looping.
            UINT nWritten = nActualBytesWritten;    // From previous call above.
            while( nWritten < dwBufferLength )
            {  
                // This will keep reading in until the buffer is full. For very short files.
                if( FAILED( hr = g_pWaveSoundRead->Reset() ) )
                    return hr;

                if( FAILED( hr = g_pWaveSoundRead->Read( (UINT)dwBufferLength - nWritten,
                                                          (BYTE*)pbBuffer + nWritten,
                                                          &nActualBytesWritten ) ) )
                    return hr;

                nWritten += nActualBytesWritten;
            } 
        } 
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: StopBuffer()
// Desc: Stop the DirectSound buffer
//-----------------------------------------------------------------------------
HRESULT StopBuffer() 
{
    if( NULL != g_pDSBuffer )
    {
        g_pDSBuffer->Stop();    
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: HandleNotification()
// Desc: Handle the notification that tell us to put more wav data in the 
//       circular buffer
//-----------------------------------------------------------------------------
HRESULT HandleNotification( BOOL bLooped ) 
{
    HRESULT hr;
    VOID* pbBuffer  = NULL;
    DWORD dwBufferLength;

    UpdateProgress();

    // Lock the buffer down
    if( FAILED( hr = g_pDSBuffer->Lock( g_dwNextWriteOffset, g_dwNotifySize, 
                                        &pbBuffer, &dwBufferLength, NULL, NULL, 0L ) ) )
        return hr;

    // Fill the buffer with wav data 
    if( FAILED( hr = WriteToBuffer( bLooped, pbBuffer, dwBufferLength ) ) )
        return hr;

    // Now unlock the buffer
    g_pDSBuffer->Unlock( pbBuffer, dwBufferLength, NULL, 0 );
    pbBuffer = NULL;

    // If the end was found, detrimine if there's more data to play 
    // and if not, stop playing
    if( g_bFoundEnd )
    {
        // We don't want to cut off the sound before it's done playing.
        // if doneplaying is set, the next notification event will post a stop message.
        if( g_pWaveSoundRead->m_ckInRiff.cksize > g_dwNotifySize )
        {
            if( g_dwProgress >= g_pWaveSoundRead->m_ckInRiff.cksize - g_dwNotifySize )
            {
                g_pDSBuffer->Stop();
            }
        }
        else // For short files.
        {
            if( g_dwProgress >= g_pWaveSoundRead->m_ckInRiff.cksize )
            {
                g_pDSBuffer->Stop();
            }
        }
    }

    g_dwNextWriteOffset += dwBufferLength; 
    g_dwNextWriteOffset %= g_dwBufferSize; // Circular buffer

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateProgress()
// Desc: Update the progress variable to know when the entire buffer was played
//-----------------------------------------------------------------------------
HRESULT UpdateProgress()
{
    HRESULT hr;
    DWORD   dwPlayPos;
    DWORD   dwWritePos;
    DWORD   dwPlayed;

    if( FAILED( hr = g_pDSBuffer->GetCurrentPosition( &dwPlayPos, &dwWritePos ) ) )
        return hr;

    if( dwPlayPos < g_dwLastPos )
        dwPlayed = g_dwBufferSize - g_dwLastPos + dwPlayPos;
    else
        dwPlayed = dwPlayPos - g_dwLastPos;

    g_dwProgress += dwPlayed;
    g_dwLastPos = dwPlayPos;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreBuffers()
// Desc: Restore lost buffers and fill them up with sound if possible
//-----------------------------------------------------------------------------
HRESULT RestoreBuffers( BOOL bLooped )
{
    HRESULT hr;

    if( NULL == g_pDSBuffer )
        return S_OK;

    DWORD dwStatus;
    if( FAILED( hr = g_pDSBuffer->GetStatus( &dwStatus ) ) )
        return hr;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = g_pDSBuffer->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( hr = g_pDSBuffer->Restore() );

        if( FAILED( hr = FillBuffer( bLooped ) ) )
            return hr;
    }

    return S_OK;
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                             DMusic MIDI player                                                                */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

CDMPlayer::CDMPlayer(HWND hWnd, int schemenum)
{
	m_hWnd = hWnd;
	m_bIsPlaying = FALSE;
	m_bIsInitialized = FALSE;
	m_pComposer = NULL;
	m_pLoader = NULL;
	m_pDirectMusic = NULL;
	m_pPerformance = NULL;
	m_pPort = NULL;

	NullMediaPointers();
	
	//Initialize COM
	if ( FAILED( CoInitialize(NULL) ))
	{
		OutputDebugString( "Could not initialize COM\n" );
		return;
	}

	// Create the loader
	if ( FAILED(CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
		IID_IDirectMusicLoader, (void**)&m_pLoader)))
	{
		OutputDebugString( "Could not create DMusic Loader\n" );
		return;
	}

	//Set the search directory
	WCHAR wzSearchDir[MAX_PATH];
    HRESULT hr = E_FAIL;

    if ( GetSearchPath(wzSearchDir) )
    {
	    hr = m_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wzSearchDir, FALSE );
    }

    if ( FAILED(hr) )
    {
	    hr = m_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, L".", FALSE );
    }

    if ( FAILED(hr) ) 
	{
		OutputDebugString( "Could not set search directory\n" );
        return;
	}

	// Create the performance object
	if (FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL,
		CLSCTX_INPROC, IID_IDirectMusicPerformance, (void**)&m_pPerformance)))
	{
		OutputDebugString( "Could not create DMusic performance\n" );
		return;
	}

	// Initialize the composer
	if (FAILED(::CoCreateInstance(CLSID_DirectMusicComposer, NULL, CLSCTX_INPROC, 
		IID_IDirectMusicComposer, (void**)&m_pComposer)))
	{
		OutputDebugString( "Could not create DMusic composer\n" );
		return;
	}

	// Initialize the software synthesizer
	if( InitializeSynth() == FALSE )
	{
		OutputDebugString( "Initializing software synthesizer failed\n" );
		return;
	}

	m_bIsInitialized = TRUE;
	
	// Load the necessary scheme media
	if( !LoadScheme(schemenum))
	{
        MessageBox(m_hWnd, "Unable to load the media for the default scheme.",
			"DirectMusic Shell: Initialization Error", MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	// Start Playing
	m_pDirectMusic->Activate(TRUE);
	PlaySegment(m_hWnd);
	m_bIsPlaying = TRUE;
}

CDMPlayer::~CDMPlayer()
{
	ReleaseMediaPointers();

	if (m_pPerformance)
		m_pPerformance->Stop( NULL, NULL, 0, 0 );

	if (m_pLoader)
		m_pLoader->ClearCache( GUID_DirectMusicAllTypes );

	if (m_pLoader)
		m_pLoader->Release();

	if (m_pPort)
	{
		if (m_pPerformance)
			m_pPerformance->RemovePort(m_pPort);
		m_pPort->Release();
	}
	if (m_pDirectMusic)
		m_pDirectMusic->Release();
	
	if (m_pComposer)
		m_pComposer->Release();

	if (m_pPerformance)
		m_pPerformance->Release();

	CoUninitialize();
}

// PlayEvent is the point of input for all of the shell events.
// Most events just play motifs. The application open/close/switch events
// trigger new primary segments.
void CDMPlayer::PlayEvent(WPARAM wEventType, LPARAM lEventData)
{
	if (!m_bIsPlaying)
		return;

	if (!m_bIsInitialized)
		return;

	// Deal with the events that switch between primary segments
	switch (wEventType)
	{
		case DME_APPOPEN:
			PlaySegment( HWND(lEventData) );
			PlayMotif(DME_APPOPEN);
			return;

		case DME_APPSWITCH:
			// Note that this happens upon any application switch, including clicking
			// the start bar. Therefore, clicking the start bar to switch music schemes
			// will cause PlaySegment to be called three times - once upon the DME_APPSWITCH
			// to the task bar, once upon the call to switch music schemes, and again
			// upon the DME_APPSWITCH away from the start bar.
			PlaySegment( HWND(lEventData) );
			return;

		case DME_APPCLOSE:
			m_TemplateMapper.DeleteWin( HWND(lEventData) );
			PlayMotif(DME_WINDOWCLOSE);
			return;
	}

	// If it got this far, play the motif corresponding to the event.
	PlayMotif(wEventType);
}


// Initialize the software synthesizer into the performance.
// This function also calls IDirectMusicPerformance::Init to
// initialize the performance and create the DirectMusic object.
BOOL CDMPlayer::InitializeSynth()
{
	// Initialize the performance. Have the performance create the
	// DirectMusic object by setting pDM to NULL. It is needed to
	// create the port.
	if( FAILED( m_pPerformance->Init( &m_pDirectMusic, NULL, m_hWnd )))
		return FALSE;

	DMUS_PORTPARAMS dmos;
	ZeroMemory(&dmos, sizeof(DMUS_PORTPARAMS));
	// create 1 channel group on the port
	dmos.dwChannelGroups = 1;
	// set the dwChannelGroups field as valid
	dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
	dmos.dwSize = sizeof(DMUS_PORTPARAMS);

	m_pDirectMusic->Activate(FALSE);

	if ( FAILED(m_pDirectMusic->CreatePort(CLSID_DirectMusicSynth, &dmos, &m_pPort, NULL)))
	{
		OutputDebugString( "Could not create port\n" );
		return FALSE;
	}
    if ( FAILED( m_pPort->Activate(TRUE) ) ) 
    {
		OutputDebugString( "Could not activate port\n" );
        return FALSE;
    }
	if( FAILED( m_pPerformance->AddPort( m_pPort)))
	{
		OutputDebugString( "Could not add port to the performance\n" );
        return FALSE;
    }
    if ( FAILED(m_pPerformance->AssignPChannelBlock( 0, m_pPort, 1 )) )
    {
        OutputDebugString( "Could not assign PChannel block for port\n");
        return FALSE;
	}
	return TRUE;
}

BOOL CDMPlayer::SelectOutputPort(WORD wPortIndex)
{
	if (!m_bIsInitialized)
		return FALSE;

	// Stop the performance and release the current port
	BOOL bStatus = m_bIsPlaying;
	m_bIsPlaying = FALSE;
	if (m_pPort && m_pPerformance)
	{
		m_pPerformance->Invalidate(0, 0);
		Stop();
		m_pPerformance->RemovePort(m_pPort);
	}
    if (m_pBand)
    {
        m_pBand->Unload(m_pPerformance);
    }
	if (m_pPort)
	{
		m_pPort->Release();
		m_pPort = NULL;
	}

	// Enumerate the new port to get its GUID
	HRESULT hr;
	DMUS_PORTCAPS dmpc;
	GUID guidSynthGUID;
	dmpc.dwSize = sizeof(DMUS_PORTCAPS);
	
	hr = m_pDirectMusic->EnumPort(wPortIndex, &dmpc);

	// Open the new port
	if(SUCCEEDED(hr) && hr != S_FALSE)
	{
		CopyMemory(&guidSynthGUID, &dmpc.guidPort, sizeof(GUID));
		
		DMUS_PORTPARAMS dmos;
		ZeroMemory(&dmos, sizeof(dmos));
		// create 1 channel group on the port
		dmos.dwChannelGroups = 1;
		// set the dwChannelGroups field as valid
		dmos.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
		dmos.dwSize = sizeof(DMUS_PORTPARAMS);

		hr = m_pDirectMusic->CreatePort(guidSynthGUID,
			&dmos, &m_pPort, NULL);

		if (FAILED (hr) )
			return FALSE;

        hr = m_pPort->Activate(TRUE);
        if (FAILED(hr))
            return FALSE;

		// Add it to the performance
		if( SUCCEEDED( m_pPerformance->AddPort( m_pPort)))
		{
			m_pPerformance->AssignPChannelBlock( 0, m_pPort, 1 );
			if (m_pBand)
			{
				m_pBand->Download(m_pPerformance);
			}
			ResetMidiStream();
			if (bStatus)
				Start();

			return TRUE;
		}
	}
	OutputDebugString( "Could not select output port\n" );
	return FALSE;
}

// Returns VALID_PORT for valid output ports
// Returns INVALID_PORT for input or invalid ports
// Returns SELECTED_PORT for the port that is currently selected
WORD CDMPlayer::EnumOutputPort(WORD wPortIndex, PWSTR pwzPortName, WORD wBufferLength)
{
	if (!m_bIsInitialized)
		return INVALID_PORT;

	HRESULT hr;
	DMUS_PORTCAPS dmpc;
	GUID guidEnumPort;
	dmpc.dwSize = sizeof(DMUS_PORTCAPS);
	
	hr = m_pDirectMusic->EnumPort(wPortIndex, &dmpc);

	if(SUCCEEDED(hr) && hr != S_FALSE)
	{
		if ((dmpc.dwClass == DMUS_PC_OUTPUTCLASS) && (dmpc.dwFlags & DMUS_PC_DLS))
		{
			//Get string length and account for the ending NULL character
			WORD wStrLength = wcslen(dmpc.wszDescription) + 1;
			if (wStrLength > wBufferLength)
			{
				wStrLength = wBufferLength - 1;			    //leave room for null character
				*(pwzPortName + (wBufferLength - 1) ) = 0;	//change the last character to NULL
															//realizing that there are wSize charaters
															//counting from 0, not 1
			}
			wcsncpy(pwzPortName, dmpc.wszDescription, wStrLength);
			guidEnumPort = dmpc.guidPort;
			if( m_pPort )
			{
				hr = m_pPort->GetCaps(&dmpc);
				if(SUCCEEDED(hr) && hr != S_FALSE)
				{
					if (dmpc.guidPort == guidEnumPort)
						return SELECTED_PORT;
				}
			}
			return VALID_PORT;
		}
		else
		{
			return INVALID_PORT;
		}
	}
	else
	{
		OutputDebugString( "Finished enumertating ports\n" );
		return NO_PORT;
	}
}

// This routine loads the style and band
// there is a hard coded assumption that the band name is "Default"
BOOL CDMPlayer::LoadStyle(WCHAR* pwzFileName)
{
	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicStyle;
	wcscpy(ObjectDescript.wszFileName, pwzFileName );
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME ;

    if (SUCCEEDED(m_pLoader->GetObject( &ObjectDescript, IID_IDirectMusicStyle,
		(void**)&m_pStyle)))
	{
		BSTR bstr = SysAllocString(L"Default");
		m_pStyle->GetBand(bstr, &m_pBand) ;
		SysFreeString(bstr);
		if (m_pBand)
		{
			m_pBand->Download(m_pPerformance);
			return TRUE;
		}
	}
	OutputDebugString( "Could not load style\n" );
	return FALSE;
}

BOOL CDMPlayer::LoadChordMap(WCHAR* pwzFileName)
{
	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicChordMap;
	wcscpy(ObjectDescript.wszFileName, pwzFileName);
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
    if (SUCCEEDED(m_pLoader->GetObject( &ObjectDescript, IID_IDirectMusicChordMap,
		(void**)&m_pChordMap)))
	{
		return TRUE;
	}
	OutputDebugString( "Could not load chord map\n" );
	return FALSE;
}

BOOL CDMPlayer::LoadTemplate(DWORD dwIndex, WCHAR* pwzName)
{ 	
	if (m_pTemplates[dwIndex])
		m_pTemplates[dwIndex]->Release();

	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicSegment;
	wcscpy(ObjectDescript.wszFileName, pwzName);
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
    if (SUCCEEDED(m_pLoader->GetObject( &ObjectDescript, IID_IDirectMusicSegment,
		(void**)&m_pTemplates[dwIndex])))
	{
		return TRUE;
	}
	OutputDebugString( "Could not load template\n" );
	return FALSE;
}

BOOL CDMPlayer::ComposeSegment(DWORD dwIndex)
{ 	
	m_pComposer->ComposeSegmentFromTemplate(
		m_pStyle,
		m_pTemplates[dwIndex],
		1,
		m_pChordMap,
		&m_pPrimarySegments[dwIndex]);
	if (m_pPrimarySegments[dwIndex])
	{
		m_pPrimarySegments[dwIndex]->SetRepeats(999);
		return TRUE;
	}
	OutputDebugString( "Could not compose segment\n" );
	return FALSE;
}

void CDMPlayer::PlaySegment(HWND hWnd)
{
	WORD wTemplateNum = m_TemplateMapper.GetTemplate( hWnd );

	if (wTemplateNum > NUM_TEMPLATES)
		return;

	if (m_pPrimarySegments[wTemplateNum] )
	{
		m_pPerformance->PlaySegment( 
			m_pPrimarySegments[wTemplateNum], DMUS_SEGF_BEAT , 0, NULL);
	}
}

//	Load the dls set that the band needs, but just leave it in the cache.
//	Then, it will be referenced and loaded by the bands in the styles and segments.
BOOL CDMPlayer::LoadDLS(WCHAR* sFileName)
{
	// No need to load GM.DLS. The filename will be blank in this case
	if ( wcscmp(sFileName, L"") == 0 )
		return TRUE;
	
	DMUS_OBJECTDESC ObjectDescript;
	ObjectDescript.dwSize = sizeof(DMUS_OBJECTDESC);
	ObjectDescript.guidClass = CLSID_DirectMusicCollection;
	wcscpy(ObjectDescript.wszFileName, sFileName);
	ObjectDescript.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME ;
	IDirectMusicObject* pObject = NULL;
	if (SUCCEEDED(m_pLoader->GetObject(&ObjectDescript, IID_IDirectMusicObject,
        (void**)&pObject)))
	{
		pObject->Release();
		return TRUE;
	}
	OutputDebugString( "Could not load dls\n" );
	return FALSE;
}

void CDMPlayer::PlayMotif(WORD wMotif)
{
	if (wMotif >= DME_MAXEVENTNUM)
		return;

	// Throw away redundant events that occur in the same beat.
	// The purpose is to prevent more than one motif of any given
	// type from triggering in the same beat.
	MUSIC_TIME mtNow, mtNowBeat;
	m_pPerformance->GetTime(NULL, &mtNow);
	mtNowBeat = mtNow/DMUS_PPQ;
	if (mtNowBeat == m_mtPreviousEventBeatNumber[wMotif])
		return;
	m_mtPreviousEventBeatNumber[wMotif] = mtNowBeat;

	//Play the motif
	if (m_pMotifs[wMotif] && m_pPerformance)
	{
		m_pPerformance->PlaySegment(m_pMotifs[wMotif],
			(DMUS_SEGF_SECONDARY | DMUS_SEGF_BEAT), 0, NULL);
	}
}

BOOL CDMPlayer::LoadScheme(WORD wSchIndx)
{
	if (!LoadDLS(Schemes[wSchIndx].DLSName) )
		return FALSE;
	if (!LoadStyle(Schemes[wSchIndx].StyleName) )
		return FALSE;
	if (!LoadChordMap(Schemes[wSchIndx].ChordMapName) )
		return FALSE;

	WORD x;
	
	for ( x = 0; x < NUM_TEMPLATES ; x++)
	{
		if (wcscmp(Schemes[wSchIndx].TemplateNames[x], L"") != 0)
			if ( LoadTemplate( x, Schemes[wSchIndx].TemplateNames[x] ) )
				ComposeSegment( x );
	}

	for ( x = 0; x < DME_MAXEVENTNUM ; x++)
	{
		if ( Schemes[wSchIndx].MotifNames[x] )
			if (wcscmp(Schemes[wSchIndx].MotifNames[x], L"") != 0)
				m_pStyle->GetMotif(Schemes[wSchIndx].MotifNames[x], &m_pMotifs[x]);
	}

	return TRUE;
}

BOOL CDMPlayer::SelectNewScheme(WORD wSchemeNum)
{
	if (!m_bIsInitialized)
		return FALSE;

	ReleaseMediaPointers();
	NullMediaPointers();
	BOOL bReturn = LoadScheme(wSchemeNum);

	ResetMidiStream();
	
	return bReturn;
}

BOOL CDMPlayer::GetSchemeName(WORD wSchemeNum, PWSTR pwzSchemeName, WORD wBufferLength)
{
	if (wSchemeNum >= NUM_SCHEMES)
		return FALSE;
	WORD wStrLength = wcslen(Schemes[wSchemeNum].FriendlySchemeName) + 1; //grab null char also
	if (wStrLength > wBufferLength)
	{
		wStrLength = wBufferLength - 1;					//leave room for null character
		*(pwzSchemeName + (wBufferLength - 1) ) = 0;	//change the last character to NULL
														//realizing that there are wSize charaters
														//counting from 0, not 1
	}
	wcsncpy(pwzSchemeName, Schemes[wSchemeNum].FriendlySchemeName, wStrLength);
	return TRUE;
}

void CDMPlayer::Start()
{
	if (!m_bIsInitialized)
		return;

	PlaySegment(m_hWnd);
	m_bIsPlaying = TRUE;
}

void CDMPlayer::Stop()
{
	if (m_pPerformance)
		m_pPerformance->Stop( NULL, NULL, 0, 0 );

	ResetMidiStream();
	m_bIsPlaying = FALSE;
}

BOOL CDMPlayer::IsPlaying()
{
	return m_bIsPlaying;
}

void CDMPlayer::NullMediaPointers()
{
	for (int x = 0; x < NUM_TEMPLATES; x++)
		m_pPrimarySegments[x] = NULL;

	m_pChordMap = NULL;

	m_pStyle = NULL;

	for (x = 0; x < DME_MAXEVENTNUM; x++)
		m_pMotifs[x] = NULL;

	for (x = 0; x < NUM_TEMPLATES; x++)
		m_pTemplates[x] = NULL;

	m_pBand = NULL;

	for (x = 0; x < DME_MAXEVENTRANGE; x++)
		m_mtPreviousEventBeatNumber[x] = 0;

}

void CDMPlayer::ReleaseMediaPointers()
{
	for (int x = 0; x < DME_MAXEVENTNUM; x++)
	{
		if (m_pMotifs[x])
			m_pMotifs[x]->Release();
	}
	
	if (m_pChordMap)
		m_pChordMap->Release();
	
	for (x = 0; x < NUM_TEMPLATES; x++)
	{
		if (m_pPrimarySegments[x])
			m_pPrimarySegments[x]->Release();
	}
	
	if (m_pStyle)
		m_pStyle->Release();
	
	for (x = 0; x < NUM_TEMPLATES; x++)
	{
		if (m_pTemplates[x])
			m_pTemplates[x]->Release();
	}

	if (m_pBand)
	{
		m_pBand->Unload(m_pPerformance);
		m_pBand->Release();
	}
}

void CDMPlayer::ResetMidiStream()
{
	if (!m_pPerformance)
		return;

	DMUS_MIDI_PMSG* pMidi;
	DMUS_PMSG* pEvent;
	for (int x = 0; x < 16; x ++ )
	{
		if( SUCCEEDED( m_pPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
			(DMUS_PMSG**)&pMidi )))
		{
			ZeroMemory( pMidi, sizeof(DMUS_MIDI_PMSG) );
			pMidi->bStatus = 0xb0 | x;
			pMidi->bByte1 = 0x79;			//Reset all controllers
			pMidi->bByte2 = 0x00;
			pMidi->dwType = DMUS_PMSGT_MIDI;
			pEvent = (DMUS_PMSG*)pMidi;
			pEvent->dwFlags = DMUS_PMSGF_REFTIME;
			m_pPerformance->SendPMsg( pEvent );
		}
		if( SUCCEEDED( m_pPerformance->AllocPMsg( sizeof(DMUS_MIDI_PMSG),
			(DMUS_PMSG**)&pMidi )))
		{
			ZeroMemory( pMidi, sizeof(DMUS_MIDI_PMSG) );
			pMidi->bStatus = 0xb0 | x;
			pMidi->bByte1 = 0x7b;			//All notes off
			pMidi->bByte2 = 0x00;
			pMidi->dwType = DMUS_PMSGT_MIDI;
			pEvent = (DMUS_PMSG*)pMidi;
			pEvent->dwFlags = DMUS_PMSGF_REFTIME;
			m_pPerformance->SendPMsg( pEvent );
		}
	}
}

// Get registry search path
//
static char szDirectMusicMedia[] = MIDIPATH;

static BOOL GetSearchPath(WCHAR wszPath[MAX_PATH])
{
	HKEY	hkDirectX;
	BOOL	bRet = FALSE;
	char	szPath[MAX_PATH];
	DWORD	cbPath;


	// Get DirectX SDK search path from the registry
	//
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					 "Software\\Microsoft\\DirectX",
					 0,							// Reserved
					 KEY_READ,
					 &hkDirectX))
    {
		OutputDebugString( "Could not open registry\n" );
		return FALSE;
	}

	cbPath = sizeof(szPath);
	if (RegQueryValueEx(hkDirectX,
                                                "DXSDK Samples Path",
						NULL,					// Reserved
						NULL,					// Type: don't care
						(LPBYTE)szPath,
						&cbPath) == ERROR_SUCCESS)
    {
		if (cbPath + sizeof(szDirectMusicMedia) > MAX_PATH)
		{
			return FALSE;
		}

		strcat(szPath, szDirectMusicMedia);

		/* --> my little hack to make it work */
		sprintf(szPath, LAMEPATH);
		/* ---------------------------------------------- */

		// DirectMusic requires the search path as a wide string
		//

		mbstowcs(wszPath, 
				 szPath,
				 MAX_PATH);
		bRet = TRUE;
	}

	RegCloseKey(hkDirectX);
	return bRet;
}



