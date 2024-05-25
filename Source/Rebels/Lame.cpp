/* ------------------------------------------------------------------------------------------------------------------------- */
/*					        	(c)2k by ChIlLeRz Dragon (UDIC)... aka Riley T. Perry                             */
/* ===================================================================== */
/*                                                                    Lame.cpp                                                                           */
/* ===================================================================== */
/*										          	The main shelll for Lame Carz									    			    */	
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

/* local defs */

#define IDIRECTPLAY2_OR_GREATER			// Use Dxplay2+
#define WIN32_LEAN_AND_MEAN					  // Faster build
#define INITGUID													   // Use GUID functions

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

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
#include <dplobby.h>
#include <dplay.h>

/* My includes */

#include "lame.h"
#include "Sound.h"
#include "map.h"
#include "Gpdumb1.h"
#include "CAR.h"
#include "CAR2.h"
#include "ErrorVille.h"
#include "play.h"
#include "main.h"

/* -------------------------------------------------------------------------------------------------------------- */
/* ===============================================================*/															  
/*                                                       My GUID                                                                           */
/* ===============================================================*/
/* -------------------------------------------------------------------------------------------------------------- */	

GUID g_AppGUID = { 0xc6334fc2, 0x3b80, 0x4fed, { 0x89, 0xf1, 0xa4, 0xde, 0xfe, 0xb6, 0xdb, 0x20 } };

/* -------------------------------------------------------------------------------------------------------------- */
/* ===============================================================*/															  
/*                                                    Library Header                                                                  */
/* ===============================================================*/
/* -------------------------------------------------------------------------------------------------------------- */	

/* defines. macros, globals and prototypes */

// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINXCLASS"  // class name
#define WINDOW_WIDTH  0              // size of window
#define WINDOW_HEIGHT 0

// MACROS /////////////////////////////////////////////////

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// PROTOTYPES /////////////////////////////////////////////

int Init_Game(void *parms=NULL);
int Shutdown(void *parms=NULL);
int Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance
char buffer[80];                // used to print text
HWND	 hwnd;		// generic window handle

/* -------------------------------------------------------------------------------------------------------------- */
/* ===============================================================*/															  
/*																Globals                                                                      */
/* ===============================================================*/
/* -------------------------------------------------------------------------------------------------------------- */	

/* initial setup for cars */

int anim[]={EXPLOSION_ANIM}	;		 // The animation sequence

/*  BOB and Bitmap globals */

BITMAP_IMAGE intro1;						 //   Introduciton bitmaps
BITMAP_IMAGE intro2;
BITMAP_IMAGE title;								//   Pre game bitmaps
BITMAP_IMAGE join;
BITMAP_IMAGE host;
BITMAP_IMAGE lada;
BITMAP_IMAGE vw;
BITMAP_IMAGE sigma;
BITMAP_IMAGE datto;

BOB boundry;											//    Boundry and map BOBs
BOB background;
BOB display;											  //    Panel display
BOB flag;													   //   Flag
BOB lights;                                                   //   Lights

/* MAP elements */

MAP_Coords MAP_Car1 = {MAP_PICTURE1}; 
MAP_Element starting_line = {MAP_STARTING_LINE};
MAP_Element checkpoint = {MAP_CHECKPOINT};
MAP_Element  map[] = {MAP_COLLISION_DATA};

/* Various Globals */

bool	Game_Start=false;							 // Has game started?
bool	Lights_Finished=false;					   // Are the lights finished?	
bool	Victory_Dance_Finished=true;	  // Has the victory dance finished?
int			Winner=0;										  	  // Winner is player number n
bool	Start_First_Timer=true;						// Timer for first lap of race
bool	newgame=false;								   // A race has been won

/* Victory Dance Globals */

int			Size_Addition = 0;					   // counter for increasing size of BOB				
bool	Size_Incrementing=true;			//  is the BOBs size incrementing?
int			Number_Incs=-1;						  // Number of bounces 

/* Sounds */

int	Explosion;											 // Varoius sound ids
int	Engine1;
int	Engine2;
int	Boing;
int	Lap;
int	Red;
int Go;
int Ignition;
int bump;
int bump2;
int screech;

/* Sound bools */

bool Boing_Sound;								// Has Boing sound played?
bool playStartMusic=true;					// Play streaming WAV?	
bool bLooped=true;								// Play streaming WAV looped?
bool Screech_Played=false;			   // Has a screech been played?
bool PlayingGameMusic=false;       // Has in game music been selected?

/* DirectMusic globals */

 CDMPlayer* m_pDMPlayer;			 // MIDI player object

/* Externs */

extern HANDLE g_hNotificationEvents[2];

/* Other car */

int otherScore=0;										// Other car values
int othercarvelocity=0;
int othercartype=0;

int Recx;														// Received information
int Recy;
int RecFrame;
int bumping;
int crashing;
int beeping;
bool isbumping;
bool otherwin=false;

bool iwin=false;										// Has this player won?

int volume_coeff=100;						     // Set up initial volume coefficient

/* States */

int carstate=DATTO;								 // Default car state

enum gamestate {ENTROPY,TITLE,BACKDROP1,JOINHOST,SETUPCON,RECEIVEFIRST,
								    CHOOSECAR,GETOTHERCAR,CLEANUPPRE,SYNCSTARTLIGHTS,GAME};
int gamestate=ENTROPY;

/* Car objects */

CAR mycar(VK_LEFT, VK_RIGHT, 
					VK_CONTROL, VK_SPACE, ACCEL_RATE, MAX_SPEED, CAR_SIZE_X,CAR_SIZE_Y, EXPLOSION_X,
					EXPLOSION_Y,EXPLOSION_FRAMES, anim, ANIM_SPEED, ACCEL_BAR_LEN,
					ACCEL_BAR_HEIGHT, ACCEL_BAR_COL, LAME_VALUE, CAR_FRAMES,
					ACCEL_BAR_END_COL, ACCEL_BAR_END_WIDTH);

CAR2 yourcar(CAR_SIZE_X,CAR_SIZE_Y, EXPLOSION_X,
					EXPLOSION_Y,EXPLOSION_FRAMES, anim, ANIM_SPEED, CAR_FRAMES);

/* Direct play stuff */

struct GAMEMSG_GENERIC
{
    DWORD dwType;
	DWORD myType;
	/* Co-ords */
	DWORD Mapx;
	DWORD Mapy;
	/* Sounds */
	DWORD bump;
	DWORD car;
	/* game info */
	DWORD Score;
	DWORD win;
	DWORD Frame;
	DWORD velocity;
	DWORD explode;
};

// GUIDs, message buffers, DPIDs, etc. 
VOID*              g_pvDPMsgBuffer                           = NULL;
DWORD         g_dwDPMsgBufferSize                       = 0;

LPGUID lpServiceProviderGuid;
LPDIRECTPLAY4   pDP = NULL;
GUID tempGUID;
DPID dpid;

// Console text positions
int textypos=CONNTEXTY;
int textxpos=CONNTEXTX;

// Exploding
bool exploding=false;
bool other_exploding=false;
int Explosion_Immunity_Counter=0;								// no. of frames cars are immune for

// Various bools
bool dplaycon=false;
bool ConnOK=false;
bool msg_rec=true;
bool CarSent=false;
bool CarFound=false;
bool joining = true;

/* Timer */

bool timesample=false;

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                     Error Functions                                                                 */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void ErrorVille(char *errorMessage) {
	/* My error handler */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* 	errorMessage = string to dislpay      						     	   */
	/* ----------------------------------------------------------------------------- */
	
	// Unlock prim buffer		
	 DD_Unlock_Primary_Surface();
	// copy text into buffer
	sprintf (buffer, errorMessage);		
	// print
	Draw_Text_GDI(buffer,0,0 ,
	RGB(255,255,255),lpddsprimary);
	// give the user time to digest
	Sleep(5000);
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                    Networking Functions                                                      */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void Print_Con_Info(char* text) {
	/* Prints text on the network console screen */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/*  text = text to print on the console											*/
	/* ----------------------------------------------------------------------------- */

	char buffer[TEXTSIZE];

	// unlock secondary surface 
	DD_Unlock_Back_Surface();
	// copy text into buffer
	sprintf (buffer, ">%s",text);										
	// Draw onto back buffer
    Draw_Text_GDI(buffer,textxpos ,textypos ,
         RGB(0,255,0),lpddsback);
	// Wait until the Vertical Blank	
	DD_Wait_For_Vsync();
	// flip the surface 
	DD_Flip();

	/* Do it again to maintain old text for next flip */

	// unlock secondary surface 
	DD_Unlock_Back_Surface();
	// copy text into buffer
	sprintf (buffer, ">%s",text);	
	// Draw onto back buffer	
    Draw_Text_GDI(buffer,textxpos ,textypos ,
         RGB(0,255,0),lpddsback);
	// move 'cursor' down
	textypos+=CONN_Y_INC;
}

BOOL FAR PASCAL DPConnect_EnumConnectionsCallback( LPCGUID   pguidSP, 
                                                   VOID*     pConnection, 
                                                   DWORD     dwConnectionSize, 
                                                   LPCDPNAME pName, 
                                                   DWORD     dwFlags, 
                                                   VOID*     pvContext ) {
	/* DirectPlay Connections callback function */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* (yawn) check SAMS dx7														*/
	/* ----------------------------------------------------------------------------- */

	char buffer[TEXTSIZE];
	
	// Copy service text into buffer
	sprintf(buffer,(const char*)pName->lpszShortNameA );
	// Print text
	Print_Con_Info(buffer); 

	// If IPX has been found
    if (strstr((const char*)pName->lpszShortNameA , "IPX") != NULL) { 
		// Tell network conn
		Print_Con_Info("Using IPX service"); 
		// Make space for the service provider's GUID. 
		lpServiceProviderGuid = new GUID; 
	
		//If memory could not be allocated 
		if (lpServiceProviderGuid == NULL) {
			ErrorVille("Could not allocate memory for service provider's GUID"); 
		} else {
			// Store the GUID of the service provider 
			*lpServiceProviderGuid = *pguidSP; 
			// Init connection
			pDP->InitializeConnection( pConnection, 0 );
		}
	return(FALSE); //Returning false stops further enumeration 
	//Since we found our service provider, we return false 
	} 
    return TRUE; // Keep enumerating
}

BOOL FAR PASCAL DPConnect_EnumSessionsCallback( LPCDPSESSIONDESC2 pdpsd, 
                                                DWORD* pdwTimeout, 
                                                DWORD dwFlags, 
                                                VOID* pvContext ) {
	/* Enumerated sessions callback */

 	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* (yawn) check SAMS dx7														*/
	/* ----------------------------------------------------------------------------- */

	char buffer[TEXTSIZE];

    if( dwFlags & DPESC_TIMEDOUT ) {
        return FALSE; // The enumeration has timed out, so stop the enumeration.
    }

	// Found a good session, save it
	tempGUID=pdpsd->guidInstance;
	//  Print Session name to network conn 
	sprintf(buffer,(const char*)pdpsd->lpszSessionNameA);
	Print_Con_Info(buffer); 
    return TRUE;
}

int Handle_System_Message(LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize,
                          DPID idFrom, DPID idTo) {   
	/* Handle a system message */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* (yawn) check SAMS dx7														*/
	/* ----------------------------------------------------------------------------- */
	
	// Switch on type of system message 
	switch(lpMsg->dwType)    {
    case DPSYS_SESSIONLOST:        {            // inform user
            // PostQuitMessage(0)        
								   } break;    
	case DPSYS_HOST:        {
            // inform user        
							} break;
    case DPSYS_CREATEPLAYERORGROUP:  // a new player        
		{
            // cast to get message
            LPDPMSG_CREATEPLAYERORGROUP lp = (LPDPMSG_CREATEPLAYERORGROUP)lpMsg;
            // inform user a new player has arrived
            // name of this new player is lp->dpnName.lpszShortNameA
			// Print connect info - sleep for a while */
			Print_Con_Info("* Game commencing * - Player 2 connected");
			Sleep(1000);
			// OK to move onto next stage in game state 
			ConnOK=true;
        } break;    
	case DPSYS_DESTROYPLAYERORGROUP:  // a lost player        
		{
            // cast to get message
            LPDPMSG_DESTROYPLAYERORGROUP lp = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
            // inform user a player has left
            // name of this new player is lp->dpnName.lpszShortNameA
        } break;    
	default:    {;}  // an uncaptured message. Error here
   }  // end switch
return(1);  // success
} 

int Handle_Appl_Message(GAMEMSG_GENERIC* lpMsg, DWORD dwMsgSize,
                        DPID idFrom, DPID idTo) {   
	/* Handle game message */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* (yawn) check SAMS dx7														*/
	/* ----------------------------------------------------------------------------- */

	switch(	lpMsg->myType) 
	{
	case DATA :
		{  
			// get various game data 
			Recx=(int)lpMsg->Mapx;
			Recy=(int)lpMsg->Mapy;
			RecFrame=(int)lpMsg->Frame;
			isbumping=(bool)lpMsg->bump;
			othercarvelocity=(int)lpMsg->velocity;
			otherScore=(int)lpMsg->Score;
			other_exploding=(bool)lpMsg->explode;
			// check to see if other player has won
			if((bool)lpMsg->win) {
				otherwin=true;
				// Play screech if nec.
				if (!Screech_Played) {
					Screech_Played=true;
						Play_Sound(screech);
				}
			} else {
				Screech_Played=false;
			//	otherwin=false;
			}
			//Calculate volume coefficient 
			volume_coeff =  100 - (max(abs(mycar.Mapx-Recx),abs(mycar.Mapy-Recy))/10);
			if (volume_coeff<0) {
				volume_coeff=0;
			}
			// message received=OK
		}
	case SYNC :
		{
			// Pre game data
			
			// adjust other car type
			if (!CarFound) {	
				othercartype=(int)lpMsg->car;
				CarFound=true;
			}
		}
	}
    return(1);  // success
} 

HRESULT Receive_Mesg() {
	/* Receive messages in queue */

    DPID    idFrom;
    DPID    idTo;
    LPVOID  pvMsgBuffer;
    DWORD   dwMsgBufferSize;
    HRESULT hr;

    // Read all messages in queue
    dwMsgBufferSize = g_dwDPMsgBufferSize;
    pvMsgBuffer     = g_pvDPMsgBuffer;
    
    while( TRUE )
    {
        // See what's out there
        idFrom = 0;
        idTo   = 0;

        hr = pDP->Receive( &idFrom, &idTo, DPRECEIVE_ALL, 
                             pvMsgBuffer, &dwMsgBufferSize );

        if( hr == DPERR_BUFFERTOOSMALL )
        {
            // The current buffer was too small, 
            // so reallocate it and try again

		    SAFE_DELETE_ARRAY( pvMsgBuffer );

            pvMsgBuffer = new BYTE[ dwMsgBufferSize ];
            if( pvMsgBuffer == NULL )
                return E_OUTOFMEMORY;

            // Save new buffer in globals
            g_pvDPMsgBuffer     = pvMsgBuffer;
            g_dwDPMsgBufferSize = dwMsgBufferSize;

            continue; // Now that the buffer is bigger, try again
        }

        if( DPERR_NOMESSAGES == hr )
            return S_OK;

        if( FAILED(hr) )
            return hr;

        // Handle the messages. If its from DPID_SYSMSG, its a system message, 
        // otherwise its an application message. 
        if( idFrom == DPID_SYSMSG )
        {
            hr = Handle_System_Message((DPMSG_GENERIC*)pvMsgBuffer, 
                                       dwMsgBufferSize, idFrom, idTo );
            if( FAILED(hr) )
                return hr;
        }
        else
        {
            hr = Handle_Appl_Message((GAMEMSG_GENERIC*)pvMsgBuffer, 
                                    dwMsgBufferSize, idFrom, idTo );
            if( FAILED(hr) )
                return hr;
        }
    }
    return S_OK;
}

int Setup_Con() {
	/* initialise DirectPlay */

	HRESULT       hr;
	DPSESSIONDESC2   session_desc;    // session desc
	DPNAME name;

	// COM init
	CoInitialize( NULL ) ;
    
	// Print conn welcome 
	Print_Con_Info("'ChIlLeRz Networking'  Beta version 1.0 - IPX only");
	Print_Con_Info("******************************************************");
    
	// Create a IDirectPlay object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, 
                                       IID_IDirectPlay4A, (VOID**)&pDP ) ) )
        return FALSE; 
	
	// Enumerate connections
    pDP->EnumConnections( &g_AppGUID, DPConnect_EnumConnectionsCallback, 
                                           hwnd, 0 ) ;
	
	// Wipe then fill session desc
	ZeroMemory(&session_desc, sizeof(DPSESSIONDESC2));  // clear the desc
	session_desc.dwSize = sizeof(DPSESSIONDESC2);
	session_desc.guidApplication = g_AppGUID;  

	// Check if hosting or joining
	if(joining) {
		// Find Lame Carz 2 session
		pDP->EnumSessions(&session_desc, 0, DPConnect_EnumSessionsCallback, hwnd, 0);
	
		// Wipe then fill session desc
		ZeroMemory(&session_desc, sizeof(DPSESSIONDESC2));
		session_desc.dwSize = sizeof(DPSESSIONDESC2);
		session_desc.guidInstance = tempGUID;

		// Open connection
		if( FAILED(pDP->Open(&session_desc, DPOPEN_JOIN))) {
			return FALSE;
			Print_Con_Info("Open connection failed"); 
		} else {
			Print_Con_Info("Open connection OK"); 
		}
	} else {
		// Hosting
		session_desc.dwMaxPlayers=2;
		session_desc.lpszSessionNameA="Lame Carz 2";

		// Open connection
		pDP->Open(&session_desc, DPOPEN_CREATE | DPOPEN_RETURNSTATUS);
	}

	// Fill name section (for future use really)
	name.lpszShortNameA = "Player";  // the name the from the user
	name.lpszLongNameA = NULL;

	// DP create
	if( FAILED(	pDP->CreatePlayer(&dpid, &name, NULL, NULL, 0, 0))) {
		return FALSE;
		Print_Con_Info("Create player failed"); 
	} else {
		Print_Con_Info("Create player OK"); 
		if (joining) {
			Print_Con_Info("Connecting to HOST");
			ConnOK=true;
			Sleep(1000);
		} else {
			Print_Con_Info("Waiting for Player 2 to JOIN");
		}
	}

// return success
return(1);
}

void Send_SYNC() {
	/* Send a SYNC message */ 
	
	GAMEMSG_GENERIC mesg;

    // fill in type and which car the player has chosen
	mesg.myType=SYNC;
	mesg.car=(int)carstate;
	// Send to other player
     pDP->Send( dpid, DPID_ALLPLAYERS, 
                                      DPSEND_GUARANTEED, &mesg, sizeof(GAMEMSG_GENERIC));
}

void Send_DATA() {
	/* Send a DATA message */ 

     GAMEMSG_GENERIC mesg;

	 // fill in type and various data
    mesg.myType = DATA;
	mesg.Mapx=mycar.Mapx;
	mesg.Mapy=mycar.Mapy;
	mesg.Frame=mycar.car.curr_frame;
	mesg.bump=bumping;
	mesg.Score=mycar.CAR_Get_Score();
	mesg.velocity=mycar.CAR_Get_Velocity();
	mesg.explode=exploding;
	// tell other player you have won
	if (iwin) {
		mesg.win=true;
		iwin=false;
	} else {
		mesg.win=false;
	}

	// Send it to all of the players
    pDP->Send( dpid, DPID_ALLPLAYERS, 
                                      DPSEND_GUARANTEED, &mesg, sizeof(GAMEMSG_GENERIC));

	// we are not exploding
	exploding=false;
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                              Various graphics functions                                                  */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void Paint_Black() {
	/* Paint a black screen on back buffer (for transitions between palettes.) */

	DD_Unlock_Back_Surface();
	// Draw the rectangle 
	Draw_Rectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT, 0, lpddsback);
	// Wait until the Vertical Blank 
	DD_Wait_For_Vsync();
	// flip the surface 
	DD_Flip();
}

int Color_Scan(int x1, int y1, int x2, int y2, 
               UCHAR scan_start, UCHAR scan_end,
               UCHAR *scan_buffer, int scan_lpitch) {
// this function implements a crude collision technique
// based on scanning for a range of colors within a rectangle

// clip rectangle

// x coords first    
if (x1 >= screen_width)
   x1=screen_width-1;
else
if (x1 < 0)
   x1=0;

if (x2 >= screen_width)
   x2=screen_width-1;
else
if (x2 < 0)
   x2=0;

// now y-coords
if (y1 >= screen_height)
   y1=screen_height-1;
else
if (y1 < 0)
   y1=0;

if (y2 >= screen_height)
   y2=screen_height-1;
else
if (y2 < 0)
   y2=0;

// scan the region
scan_buffer +=y1*scan_lpitch;

for (int scan_y=y1; scan_y<=y2; scan_y++)
    {
    for (int scan_x=x1; scan_x<=x2; scan_x++)
        {
        if (scan_buffer[scan_x] >= scan_start && scan_buffer[scan_x] <= scan_end )
            return(1);
        } // end for x

    // move down a line
    scan_buffer+=scan_lpitch;

    } // end for y

// return failure
return(0);
} // end Color_Scan


UCHAR Get_Pixel(int x, int y,UCHAR *video_buffer, int lpitch) {
// this function gets the pixel value at x,y
return(video_buffer[x + y*lpitch]);
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                              Various sound functions                                                       */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void Boing_Play() {
/* play boing if Boing_Sound=true */
	
	if (Boing_Sound) {
		// if boing sound should be played then play it
		if(Winner!=1) {
			Set_Sound_Volume(Boing, volume_coeff-(100-BOING_VOL));
		} else {
			Set_Sound_Volume(Boing,BOING_VOL);		
		}
		Play_Sound(Boing,0);
		Boing_Sound=false;									// reset boing boolean 
	}
}

void Play_Explosion() {
	/* Play an explosion sound */

	if(!(Status_Sound(Explosion)&DSBSTATUS_PLAYING)) {
		// if an explosion sound is not already playing
		Stop_Sound(Engine1);		// Stop engine sounds
		Stop_Sound(Engine2);	
		Play_Sound(Explosion,0);  // Play the explosion souns	
	}
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                              Various print functions                                                           */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void Print_Flags(int x, int y, int amount) {
	/* print flags to backbuffer */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* 	amount = no. flags to draw											     	  */
	/* 	x = x coord																					 */
	/*  y = y coord																				    */
	/* ----------------------------------------------------------------------------- */	
	
	if(amount>0) {
		// if amount of flags is not 0
		for(int i=0;i<amount;i++) {	
			// for each flag
			Set_Pos_BOB(&flag,(x+((i-1)*FLAG_SIZE))+FLAG_SIZE, y);    // Set flag position 	
			Draw_BOB(&flag, lpddsback);																// Draw flag
		}
	}	
}

void Print_Score(int x, int y, int player, int score) {
	/* print a players score */

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* 	score = score to print													     	   */
	/* palyer = player number to print											  */
	/* 	x = x coord																					 */
	/*  y = y coord																				    */
	/* ----------------------------------------------------------------------------- */

	// copy text into buffer
	sprintf (buffer, "%d", score);										
    Draw_Text_GDI(buffer,x ,y ,
         RGB(SCORE_COLOUR_R,SCORE_COLOUR_G,SCORE_COLOUR_B ),lpddsback);
}

void Print_Time(int x, int y, int time)   {
	/* Print out a lap time */ 

	/* ----------------------------------------------------------------------------- */
	/*									     Parameters	      						             */
	/* ----------------------------------------------------------------------------- */
	/* 	time = lap time						    										     	 */
	/* 	x = x coord																					 */
	/*  y = y coord																				    */
	/* ----------------------------------------------------------------------------- */

	if(time<LAME_VALUE) { 
		// if the time is less than a lame value the convert to form ss.msmsms -> to buffer
		sprintf (buffer, "%d.%d",time/(LAME_VALUE/100),time%(LAME_VALUE/100));
	} else {
		// just print lame
		sprintf (buffer, "Lame!");
	}
    Draw_Text_GDI(buffer,x ,y ,
                   RGB(TIME_COLOUR_R,TIME_COLOUR_G,TIME_COLOUR_B),lpddsback);
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                        GET and SET comms. functions                                             */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void Reset_Laps() {
	/* Reset laps completed to 0 on both cars */

	mycar.CAR_Reset_Laps();
}

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                     Pre-game functions                                                         */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

void Introduction() {
	/* Introduction sequence */

	/* initialise the game (load bmps and sounds) */	

	ShowCursor(FALSE);			// Hide the mouse

 	/*-------------------------------------------------------------------------------------------*/		
	/* Setup fullscreen video mode at width,height and bits per pixel*/
	/* palette accessed via      =     lpddpal                                                  */
	/* primary surface				   =	 lpddsprimary									    */
	/* secondary surface           =     lpddsback											  */
	/*-------------------------------------------------------------------------------------------*/	

	if (!DD_Init(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP)) {
	}

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load Clipper                                                               */
	/* --------------------------------------------------------------------------------------------------------------*/	

	RECT screen_rect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	lpddclipper = DD_Attach_Clipper(lpddsback,1,&screen_rect);

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load Intro1                                                                   */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, INTRO1_BMP)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&intro1, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&intro1, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image intro1");
	}
	// set the palette 
	Set_Palette(bitmap8bit.palette);
	// Free the memory from the bitmap file
	Unload_Bitmap_File(&bitmap8bit);	

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Draw Background                                                         */
	/* --------------------------------------------------------------------------------------------------------------*/	

	intro1.x = 0;	// set position
	intro1.y = 0;

	// lock the secondary surface 
	if (!DD_Lock_Back_Surface()) {
		ErrorVille("Could not lock back surface");
	}
	// Draw The ENTROPY screen
	Draw_Bitmap(&intro1, back_buffer, back_lpitch, 0);
	// unlock secondary surface 
	DD_Unlock_Back_Surface();
	// Wait until the Vertical Blank		
	DD_Wait_For_Vsync();

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Play the Tunez  (wav)                                                  */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Play the intro tunez
	PlayBuffer( bLooped );

	/* flip the surface */

	DD_Flip();
}

void IntroScreen() {

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load title                                                                       */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, TITLE_SCREEN)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&title, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&title, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image title");
	}

	Set_Palette(bitmap8bit.palette);
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);	

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Draw Background                                                         */
	/* --------------------------------------------------------------------------------------------------------------*/	

	title.x = 0;	// set position
	title.y = 0;

	// lock the secondary surface 
	if (!DD_Lock_Back_Surface()) {
		ErrorVille("Could not lock back surface");
	}
	// Draw Title 
	Draw_Bitmap(&title, back_buffer, back_lpitch, 0);
	// unlock secondary surface 
	DD_Unlock_Back_Surface();
	// Wait until the Vertical Blank 	
	DD_Wait_For_Vsync();

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Play MIDI tunez                                                              */
	/* --------------------------------------------------------------------------------------------------------------*/	

	m_pDMPlayer = new CDMPlayer(hwnd, TITLE_MUSIC);

	/* flip the surface */

	DD_Flip();
}

void Part1() {

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load join                                                                       */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, NETWORKING)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&join, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&join, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image join");
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);	

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load host                                                                     */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, NETWORKING2)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&host, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&host, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image host");
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);	

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load lada                                                                    */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, INTRO_LADA_BMP)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&lada, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&lada, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image lada");
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);	

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load vw                                                                        */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, INTRO_VW_BMP)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&vw, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&vw, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image vw");
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);	

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load sigma                                                                 */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, INTRO_SIGMA_BMP)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&sigma, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&sigma, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image sigma");
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);	

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Load datto                                                                   */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the background image 

	if (!Load_Bitmap_File(&bitmap8bit, INTRO_DATTO_BMP)) {
		ErrorVille("Could not load background");
	}
	// Create tha bitmap object 
	if (!Create_Bitmap(&datto, 0, 0, SCREEN_WIDTH,SCREEN_HEIGHT)) {
		ErrorVille("Could not create background");
	}
	// Load the image into the bitmap object 
	if (!Load_Image_Bitmap(&datto, &bitmap8bit, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
			ErrorVille("Could not load -> image datto");
	}
	// set the palette 
	Set_Palette(bitmap8bit.palette);
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);	

}

void joinhost () {
	/* choose  whether to host or join a game */

	// lock the secondary surface
	if (!DD_Lock_Back_Surface()) {
		ErrorVille("Could not lock back surface");
	}

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                    Draw screens                                                                    */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// depending on joining bool - draw networking screen
	if (joining) {
			Draw_Bitmap(&join, back_buffer, back_lpitch, 0);
	} else {
			Draw_Bitmap(&host, back_buffer, back_lpitch, 0);
	}

	// unlock secondary surface 
	DD_Unlock_Back_Surface();
	// Wait until the Vertical Blank 	
	DD_Wait_For_Vsync();
	// flip the surface 
	DD_Flip();

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                         Check keys                                                                   */
	/* --------------------------------------------------------------------------------------------------------------*/	
	
	if (KEY_DOWN(VK_LEFT)) {
		joining=true;
	}  
	if (KEY_DOWN(VK_RIGHT)) {
		joining=false;
	}  
	if (KEY_DOWN(VK_SPACE)) {
		// next state
		gamestate=SETUPCON;		
		Sleep(500);
	}  
}

void whichcar() {
	/* Draw appropriate car bitmap */ 

	// lock the secondary surface 
	if (!DD_Lock_Back_Surface()) {
		ErrorVille("Could not lock back surface");
	}

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Draw cars                                                                    */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Draw correct car bmp
	switch (carstate) {
	case DATTO :
		{	
		Draw_Bitmap(&datto, back_buffer, back_lpitch, 0);
		break;
		 }
	case VW :
		{
			Draw_Bitmap(&vw, back_buffer, back_lpitch, 0);
		break;
		 }
	case LADA :
		{
			Draw_Bitmap(&lada, back_buffer, back_lpitch, 0);
		break;
		 }	
	case SIGMA :
		{
			Draw_Bitmap(&sigma, back_buffer, back_lpitch, 0);
		break;
		 }
	}

	// unlock secondary surface 
	DD_Unlock_Back_Surface();
	// Wait until the Vertical Blank 	
	DD_Wait_For_Vsync();
	// flip the surface 
	DD_Flip();
	
	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                        Check keys and change states                                               */
	/* --------------------------------------------------------------------------------------------------------------*/		
	
	// depending on key press, increment or decrement car state
	if (KEY_DOWN(VK_LEFT)) {
		if (carstate!=DATTO) {
			carstate--;
		} else {
			carstate=SIGMA;
		}
		Sleep(150);
	}  
	if (KEY_DOWN(VK_RIGHT)) {
		if (carstate!=SIGMA) {
			carstate++;
		} else {
			carstate=DATTO;
		}
		Sleep(150);
	}  
	// next game state
	if (KEY_DOWN(VK_SPACE)) {
		gamestate=GETOTHERCAR;		
		Sleep(500);
	}  
}

void CleanUpPre() {
	/* clean up pre-game objects */

	delete m_pDMPlayer;
	// Wav player free COM
	FreeDirectSound();

	/* destroy bitmaps */

	Destroy_Bitmap(&datto);
	Destroy_Bitmap(&vw);
	Destroy_Bitmap(&lada);
	Destroy_Bitmap(&sigma);
	Destroy_Bitmap(&join);
	Destroy_Bitmap(&host);
	Destroy_Bitmap(&title);
	Destroy_Bitmap(&intro1);

	/* new game state */

	gamestate=SYNCSTARTLIGHTS;
}

/* -------------------------------------------------------------------------------------------------------------- */
/* ===============================================================*/															  
/*                     Code for Windows event checking and initialisation                           */
/* ===============================================================*/
/* -------------------------------------------------------------------------------------------------------------- */	

LRESULT CALLBACK 
WindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
	/* My mini win callback function */

    switch (uMsg)
    {

	case WM_DESTROY: {
	
            PostQuitMessage(0);
            break;
	}
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
	return 0L;
}

// WINMAIN

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{

WNDCLASS winclass;	// this will hold the class we create

	MSG		 msg;		// generic message
	LONGLONG cur_time;       // current time
	BOOL notDone=TRUE;       // flag for thread completion
	DWORD  time_count=16;    // ms per frame, default if no performance counter
	LONGLONG perf_cnt;       // performance timer frequency
	BOOL perf_flag=FALSE;    // flag determining which timer to use
	LONGLONG next_time=0;    // time to render next frame
	LONGLONG last_time=0;	 // time of previous frame
	double time_elapsed;	 // time since previous frame
	double time_scale;		 // scaling factor for time

// first fill in the window class stucture
winclass.style			= CS_DBLCLKS | CS_OWNDC | 
                          CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc	= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
winclass.hbrBackground	= GetStockObject(BLACK_BRUSH);
winclass.lpszMenuName	= NULL; 
winclass.lpszClassName	= WINDOW_CLASS_NAME;

// register the window class
if (!RegisterClass(&winclass))
	return(0);

// create the window
if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, // class
						  "Lame Carz 2",	 // title
						  WS_POPUP| WS_VISIBLE,
					 	  0,0,	   // x,y
						  WINDOW_WIDTH,  // width
                          WINDOW_HEIGHT, // height
						  NULL,	   // handle to parent 
						  NULL,	   // handle to menu
						  hinstance,// instance
						  NULL)))	// creation parms
return(0);

// save the window handle and instance in a global
main_window_handle = hwnd;
main_instance      = hinstance;

  g_hNotificationEvents[0] = CreateEvent( NULL, FALSE, FALSE, NULL );
  g_hNotificationEvents[1] = CreateEvent( NULL, FALSE, FALSE, NULL );
		
	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                           Entropy screen + music                                                           */
	/* --------------------------------------------------------------------------------------------------------------*/	

	/* Set up directSound and start streaming wav loader */

	InitDirectSound( hwnd);
	LoadWaveFile( hwnd, MUSIC_WAV);

	/* Draw intro picture and init DDRAW + clipper */

	Introduction();

while(1)
	{

		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                             Streaming WAV loader                                                         */
		/* --------------------------------------------------------------------------------------------------------------*/	

		// check for new buffer fill		
		if (playStartMusic) {

			HRESULT dwResult;
			dwResult = MsgWaitForMultipleObjects( 2, g_hNotificationEvents, 
		                                          FALSE, INFINITE, QS_ALLEVENTS );
		    switch( dwResult )
		    {
		        case WAIT_OBJECT_0 + 0:
				{
		            // g_hNotificationEvents[0] is signaled

		            // This means that DirectSound just finished playing 
		            // a piece of the buffer, so we need to fill the circular 
			        // buffer with new sound from the wav file
			        bLooped = (true);
		            HandleNotification( bLooped );
					break;
				}
			}
		}

		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                             Windows message handling                                              */
		/* --------------------------------------------------------------------------------------------------------------*/	

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{ 
			// test if this is a quit
	        if (msg.message == WM_QUIT)
	           break;
	
			// translate any accelerator keys
			TranslateMessage(&msg);
	
			// send the message to the window proc
			DispatchMessage(&msg);
		} 
  
		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                             Gamestate processing                                                         */
		/* --------------------------------------------------------------------------------------------------------------*/	

		// which game state?
		switch (gamestate)
		{
		case ENTROPY :
			{
				if (KEY_DOWN(VK_SPACE)) {
					// Stop wav
					playStartMusic=false;
					// different color pallete, blacken
					Paint_Black();
					// Stop reading circ buffer (STWAVLD)
					StopBuffer();
					// Title screen
					IntroScreen();
					gamestate=TITLE;
				}  
				break;
			}
		case TITLE : 
			{
				if (KEY_DOWN(VK_SPACE)) {
					gamestate=JOINHOST;
					Paint_Black();
					Part1();
					}  
				break;
			}
		case JOINHOST :
			{
				joinhost();
				break;
			}
		case SETUPCON :
			{
				// check if conn setup
				if(Setup_Con()) {
					// yes - next game state 
					dplaycon=true;
					gamestate=RECEIVEFIRST;
				} else {
					// no, conn failed -  try again
					gamestate=JOINHOST;
					Print_Con_Info("Connection failed");
					Sleep(2000);	
					textypos=CONNTEXTY;
				}
			}
		case RECEIVEFIRST :
			{
				if(dplaycon) {
					// wait for mesg 
					Receive_Mesg();
				}

				// ------------------> Add in following values to test for 1 player 
				// ConnOK=true;
				// ------------------> (1 of 4)

				if (ConnOK) {
					// joined OK, or hosted OK
					Sleep(500);
					gamestate=CHOOSECAR;
				}
				break;
			}
		case CHOOSECAR:
			{
				whichcar();
				break;
			}
		case GETOTHERCAR :
			{

				// ------------------> Add in following values to test for 1 player 
				// gamestate=CLEANUPPRE;
				// ------------------> (2 of 4)

				// get other players car type
				Receive_Mesg();
				if(!CarSent) {				
					// SYNC message send
					Send_SYNC();
					CarSent=true;
				}					
				if(CarFound) {
					// SYNC message received - start game
					gamestate=CLEANUPPRE;
					CarSent=false;
					CarFound=false;
				}			
				break;
			}
		case CLEANUPPRE:
			{
				// delete pre-game sounds and bmp's
				CleanUpPre();	
				// Load game data
				Init_Game();
				break;
			}
		case SYNCSTARTLIGHTS :
			{
				// ------------------> Add in following values to test for 1 player 
				//	gamestate=GAME;
				// ------------------> (3 of 4)

				// get other players car type
				Receive_Mesg();
				if(!CarSent) {				
					// SYNC message send
					Send_SYNC();
					CarSent=true;
				}					
				if(CarFound) {
					// SYNC message received - start game
					gamestate=GAME;
					CarSent=false;
					CarFound=false;
				}			
				break;
			}
		case GAME:
			{
				// Send and receive msg's (non blocking)
				if (msg_rec) {
					Send_DATA();
					msg_rec=false;
				}
				if (!msg_rec) {
					Receive_Mesg();		
				} 

				/*--------------------------------------------------------------------------------------------------*/
				/*													Timer Code														   */
				/*--------------------------------------------------------------------------------------------------*/

				if (!timesample) {
					if (QueryPerformanceFrequency((LARGE_INTEGER *) &perf_cnt)) { 
						// yes, set time_count and timer choice flag 
						perf_flag=TRUE;
						time_count=perf_cnt/30;
						QueryPerformanceCounter((LARGE_INTEGER *) &next_time); 
						time_scale=1.0/perf_cnt;
					} else { 
						// no performance counter, read in using timeGetTime 
						next_time=timeGetTime(); 
						time_scale=0.001;
						time_count=30;
					} 
				// save time of last frame	
				last_time=next_time;
				timesample=true;
			}
			if (perf_flag) 
				QueryPerformanceCounter((LARGE_INTEGER *) &cur_time); 
			else 
				cur_time=timeGetTime(); 
			// is it time to render the frame? 
			if (cur_time>next_time) { 
				// calculate elapsed time
				time_elapsed=(cur_time-last_time)*time_scale;
				// save frame time
				last_time=cur_time;

				/*----------------------------------------------------------------------------------*/

				// ------------------> Add in following values to test for 1 player 
				msg_rec=true;
				// ------------------> (4 of 4)

				//if(msg_rec) {  <--------- Blocking version
					// AOK - execute this frame of the game
					Main();
				//} <---------- Blocking version
				// increment timer
				next_time=cur_time+time_count;
			}
				break;
			}
		}
		// check if user is trying to end the game
		//--------------> The following is for testing purposes 
		if (KEY_DOWN(VK_ESCAPE) )
			PostMessage(main_window_handle, WM_DESTROY,0,0);
		// --------------> Remove when not testing
	} // end while

// shutdown game and release all resources

// clean up depending on gamestate
if (gamestate<CLEANUPPRE) {
	CleanUpPre();	
} else {
	Shutdown();
}
// return to Windows
return(msg.wParam);
} // end WinMain

/* --------------------------------------------------------------------------------------------------------------*/
/* ===============================================================*/
/*                                                   Game Functions                                                                 */
/* ===============================================================*/
/* --------------------------------------------------------------------------------------------------------------*/	

int Init_Game(void *parms) {
	/* Load and init game data */

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                     initialialise  sound                                                            */
	/* --------------------------------------------------------------------------------------------------------------*/	

	DSound_Init();
	
	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Load Background                                                         */
	/* --------------------------------------------------------------------------------------------------------------*/	

	 // Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, BACKGROUND_BMP)) {
		ErrorVille("Could not load background");
	}
	if (!Create_BOB(&background, 0,0, MAP1_WIDTH, MAP1_HEIGHT,1,
		BOB_ATTR_SINGLE_FRAME|BOB_ATTR_VISIBLE ,0)) {
		ErrorVille("Could not load create bg");
	}
	// Load the images into the bitmap objects
	if (!Load_Frame_BOB(&background, &bitmap8bit, 0, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {
		
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Load Collision map                                                      */
	/* --------------------------------------------------------------------------------------------------------------*/	

		// Load the background image 
	if (!Load_Bitmap_File(&bitmap8bit, COLLISION)) {
		ErrorVille("Could not load background");
	}
	if (!Create_BOB(&boundry, 0,0, MAP1_WIDTH, MAP1_HEIGHT,1,
		BOB_ATTR_SINGLE_FRAME|BOB_ATTR_VISIBLE ,0)) {
		ErrorVille("Could not load create bg");
	}
	// Load the images into the bitmap objects
	if (!Load_Frame_BOB(&boundry, &bitmap8bit, 0, 0, 0,BITMAP_EXTRACT_MODE_ABS)) {	
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Load flag.bmp                                                                */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load flag image
	if (!Load_Bitmap_File(&bitmap8bit, FLAG_BMP)) {
		ErrorVille("Could not load flag bmp");
	}
	// Create the flag bitmap object
	if (!Create_BOB(&flag, 0,0, FLAG_SIZE, FLAG_SIZE,1,
		BOB_ATTR_SINGLE_FRAME|BOB_ATTR_VISIBLE ,0)) {
		ErrorVille("Could not load create flag");
	}
		if (!Load_Frame_BOB(&flag, &bitmap8bit, 0, 0, 0,BITMAP_EXTRACT_MODE_CELL)) {
			ErrorVille("Could not load flag -> BOB");
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                           Initialise Cars                                                              */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load appropriate carz
	switch (carstate)
		{
		case DATTO :
			{
				mycar.CAR_Load_BOB(DATTO_BMP, EXPLOSION_BMP);
				// (int mxspd, int accel, int turn, int brake)
				mycar.CAR_Setup_Rates(DATTO_RATES);
				break;
			}
		case VW :
			{
				mycar.CAR_Load_BOB(VW_BMP, EXPLOSION_BMP);
				mycar.CAR_Setup_Rates(VW_RATES);
				break;
			}
		case SIGMA :
			{
				mycar.CAR_Load_BOB(SIGMA_BMP, EXPLOSION_BMP);
				mycar.CAR_Setup_Rates(SIGMA_RATES);
				break;
			}
		case LADA :
			{
				mycar.CAR_Load_BOB(LADA_BMP, EXPLOSION_BMP);
				mycar.CAR_Setup_Rates(LADA_RATES);
				break;
			}
		}

	switch (othercartype)
		{
		case DATTO :
			{
				yourcar.CAR_Load_BOB(DATTO_BMP, EXPLOSION_BMP);
				break;
			}
		case VW :
			{
				yourcar.CAR_Load_BOB(VW_BMP, EXPLOSION_BMP);
				break;
			}
		case SIGMA :
			{
				yourcar.CAR_Load_BOB(SIGMA_BMP, EXPLOSION_BMP);
				break;
			}
		case LADA :
			{
				yourcar.CAR_Load_BOB(LADA_BMP, EXPLOSION_BMP);
				break;
			}
		}
 
	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Load display.bmp                                                         */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load the display image
	if (!Load_Bitmap_File(&bitmap8bit, DISPLAY_BMP)) {
		ErrorVille("Could not load display");
	}
	// Create tha bitmap object 
	// ????? Some weird quirk means i can only use frame 1 as a single bmp ?????
	if (!Create_BOB(&display, 0,0, DISPLAY_SIZE,2,
		BOB_ATTR_MULTI_FRAME|BOB_ATTR_VISIBLE ,0)) {
		ErrorVille("Could not load create display");
	}
	// Load the images into the bitmap objects 
	if (!Load_Frame_BOB(&display, &bitmap8bit, 0, 0, 0,BITMAP_EXTRACT_MODE_CELL)) {
		ErrorVille("Could not load flag -> BOB");
	}
	if (!Load_Frame_BOB(&display, &bitmap8bit, 1, 0, 0,BITMAP_EXTRACT_MODE_CELL)) {
		ErrorVille("Could not load flag -> BOB");
	}
	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);

	// ????? Fix weird quirks in flag and display ?????
	display.curr_frame = 1;			
	// Set position	
	display.x=DISPLAYX_POS;
	display.y=DISPLAYY_POS;

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Load lights.bmp                                                             */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Load bitmap
	if (!Load_Bitmap_File(&bitmap8bit, LIGHTS_BMP)) {
		ErrorVille("Could not load lights bmp");
	}
	// Create the lights bitmap objects 
	if (!Create_BOB(&lights, 0, 0, LIGHTS_WIDTH, LIGHTS_HEIGHT, LIGHTS_FRAMES,
		BOB_ATTR_MULTI_ANIM|BOB_ATTR_VISIBLE|BOB_ATTR_ANIM_ONE_SHOT ,0)) {
		ErrorVille("Could not create lights anim");
	}

	/* --------------------------------------------------------------------------------------------------------------*/
	/*												Set up Lights Anim				                                                   */
	/* --------------------------------------------------------------------------------------------------------------*/	

	/* Load the images into the bitmap objects */

	int anim_lights[] = {LIGHTS_ANIM};					//	store animation sequence temporarily

	// load individual frames into the BOB
	for (int i=0; i<LIGHTS_FRAMES; i++) {
		if (!Load_Frame_BOB(&lights, &bitmap8bit, i, ((i*LIGHTS_WIDTH)+1)
			 , 0,BITMAP_EXTRACT_MODE_ABS)) {
		ErrorVille("Could not create lights anim frames");
		}
	}

	/* Load the animation information, anim number = 0,  no. frames = LIGHTS_FRAMES,
		Frame sequence = LIGHTS_ANIM (anim_lights) (eg {0,4,5,3,1,2,1,2,1}) */

	Load_Animation_BOB(&lights, 0, LIGHTS_FRAMES ,  anim_lights);
	Set_Anim_Speed_BOB(&lights, LIGHTS_ANIM_SPEED );		  // set speed (eg.2 frames) 	
	Set_Animation_BOB(&lights,0);																// current animation = 0

	// Free the memory from the bitmap file 
	Unload_Bitmap_File(&bitmap8bit);

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                     Set up sound values                                                      */
	/* --------------------------------------------------------------------------------------------------------------*/		
	
	/* load sounds */

	Explosion = Load_WAV(EXPLOSION_WAV);
	Engine1 = Load_WAV(ENGINE_WAV );
	Engine2 = Load_WAV(ENGINE2_WAV);
	Boing = Load_WAV(BOING_WAV );
	Lap = Load_WAV(LAP_WAV );
	Red = Load_WAV(LIGHTS_BEEP_WAV );
	Go = Load_WAV(GO_WAV );
	bump = Load_WAV(BUMP_WAV);
	bump2 =  Load_WAV(BUMP_WAV);
	screech = Load_WAV(SCREECH_WAV);

	/* set sound pans and volumes for cars */

	Set_Sound_Volume(bump, BUMP_VOL);
	Set_Sound_Volume(Engine1,ENGINE1_VOL );
	Set_Sound_Volume(Engine2,ENGINE2_VOL);

	Set_Sound_Pan(Engine1, PAN_LFT);
	Set_Sound_Pan(Engine2,PAN_RGHT);

	/* Set starting positions of cars */

	if (joining) {
		mycar.Mapx=MAP1_RIGHT_STARTX;
		mycar.Mapy=MAP1_RIGHT_STARTY;
	} else {
		mycar.Mapx=MAP1_LEFT_STARTX;
		mycar.Mapy=MAP1_LEFT_STARTY;
	}

return(1);
} 

///////////////////////////////////////////////////////////

int Shutdown(void *parms) {
/* Shutdown game, shutdown fullscreen mode, sound and free memory */

	/* delete wav loader */

	delete m_pDMPlayer;

	/* Close fullscreen mode */
	
	DD_Shutdown();											

	if (!DSound_Shutdown()) {
		ErrorVille("Could not shutdown sound");
	}

	/* free memory from the background, cars, flag and lights*/

	Delete_All_Sounds();

	Destroy_BOB(&boundry);
	Destroy_BOB(&background);
	Destroy_BOB(&display);
	Destroy_BOB(&flag);
	Destroy_BOB(&lights);

	 yourcar.~CAR2();
	 mycar.~CAR();

// return success
return(1);
}

///////////////////////////////////////////////////////////

int Main(void *parms) {
	/* Main game loop */

	// check for in game music
	if (KEY_DOWN(VK_S) ) {
		if(!PlayingGameMusic) {
			PlayingGameMusic=true;
			m_pDMPlayer = new CDMPlayer(hwnd, GAME_MUSIC);
		}
	}

	/* Setup initial send settings */

	bumping=false;
	crashing=false;

	/* Move carz (if game underway) */

	if(Game_Start) {		
		// move mycar 
		mycar.CAR_Check_Direction();
		// move yourcar
		yourcar.Recx=Recx-(mycar.Mapx-mycar.car.x);
		yourcar.Recy=Recy-(mycar.Mapy-mycar.car.y);
		yourcar.Frame=RecFrame;
		// check yourcar for explosions
		yourcar.CAR_Check_Direction();
	}

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Draw Collision bmp                                                      */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Unlock back 
	DD_Unlock_Back_Surface();
	// set map coords
	background.x=MIDDLE_SCREEN_X-mycar.Mapx;
	background.y=MIDDLE_SCREEN_Y-mycar.Mapy;
	boundry.x=MIDDLE_SCREEN_X-mycar.Mapx;
	boundry.y=MIDDLE_SCREEN_Y-mycar.Mapy;
	// Draw colis. map
	Draw_BOB(&boundry, lpddsback);		

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                       Check New Game                                                         */
	/* --------------------------------------------------------------------------------------------------------------*/	

	if(Game_Start) {
		// if the game has started (no lights or victory dance) 

		/* Set sound frequency of the engines */

		Set_Sound_Freq(Engine1, ENGINE_FREQ*(mycar.CAR_Get_Velocity())+BASE_FREQ);
		Set_Sound_Freq(Engine2, ENGINE_FREQ*othercarvelocity+BASE_FREQ2);

		/* play engine sounds (if no explosion is sounding) */

		Set_Sound_Volume(Engine2, volume_coeff-(100-ENGINE2_VOL));
	
		if((!(Status_Sound(Explosion)&DSBSTATUS_PLAYING))&&
			(!(Status_Sound(Engine1)&DSBSTATUS_PLAYING))) {
			Play_Sound(Engine1,DSBPLAY_LOOPING);
			Play_Sound(Engine2,DSBPLAY_LOOPING);
		}

		// lock back surface - for pixel check
		if (!DD_Lock_Back_Surface()) {
			ErrorVille("Could not lock back surface");
		}
		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                               Check for bounce Collisions                                              */
		/* --------------------------------------------------------------------------------------------------------------*/	

		// reset bounce indicators
		mycar.CAR_Bounce_NORTH=false;
		mycar.CAR_Bounce_WEST=false;
		mycar.CAR_Bounce_SOUTH=false;
		mycar.CAR_Bounce_EAST=false;

		/* check four sides of car and adjust xv and yv */

		/* eg 1     (y1)  ...x3,x4..xn-3...
						 (y2)	...x3,x4..xn-3..
						 (y3)	...x3,x4..xn-3...

		/* this version JUST checks the middle pixel in each row (just 1 pixel) */

		/*_____
		  | ****** |
	      |            |         
	      |            |
		   ---------   */

		if (Color_Scan(mycar.car.x+(CAR_SIZE_X/2) , mycar.car.y , mycar.car.x+((CAR_SIZE_X/2) +1) ,
			                         mycar.car.y+1, 240, 240 ,  back_buffer, back_lpitch)) {
			// unlock back surface
			DD_Unlock_Back_Surface();
			//  Car is bumping
			bumping=true;
			// Set bounce indicators
			mycar.CAR_Bounce_First=true;
			mycar.CAR_Bounce=true;
			mycar.CAR_Bounce_Y=-1;
			mycar.CAR_Bounce_NORTH=true;
			// lock back surface
			if (!DD_Lock_Back_Surface()) {
				ErrorVille("Could not lock back surface");
			}
		}

		/*____
		  |        *|
	      |        *|         
	      |        *|
		   -------   */
	
		if (Color_Scan(mycar.car.x+(CAR_SIZE_X-1) , mycar.car.y+(CAR_SIZE_Y/2) 
									, mycar.car.x+(CAR_SIZE_X) , mycar.car.y+((CAR_SIZE_Y/2)+1) 
									, 240, 240 ,   back_buffer, back_lpitch)) {
	
			DD_Unlock_Back_Surface();
			bumping=true;
			mycar.CAR_Bounce_First=true;
			mycar.CAR_Bounce=true;
			mycar.CAR_Bounce_X=-1;
			mycar.CAR_Bounce_EAST=true;
			if (!DD_Lock_Back_Surface()) {
				ErrorVille("Could not lock back surface");
			}		
		}

		/*_____
		  |            |
	      |            |         
	      | ****** |
		   ---------   */

		if (Color_Scan(mycar.car.x+(CAR_SIZE_X/2) , mycar.car.y+(CAR_SIZE_Y-1) 
								, mycar.car.x+((CAR_SIZE_X/2)+1), mycar.car.y+(CAR_SIZE_Y)
								, 240, 240 ,    back_buffer, back_lpitch)) {

			DD_Unlock_Back_Surface();
			bumping=true;
			mycar.CAR_Bounce_First=true;
			mycar.CAR_Bounce=true;
			mycar.CAR_Bounce_Y=-1;
			mycar.CAR_Bounce_SOUTH=true;
			if (!DD_Lock_Back_Surface()) {
				ErrorVille("Could not lock back surface");
			}
		}

		/*____
		  |*        |
	      |*        |         
	      |*        |
		   -------   */
			
		if (Color_Scan(mycar.car.x , mycar.car.y+(CAR_SIZE_Y/2) , mycar.car.x+1,
		                         mycar.car.y+((CAR_SIZE_Y/2)+1), 240, 240 ,    back_buffer, back_lpitch)) {

			DD_Unlock_Back_Surface();				
			bumping=true;
			mycar.CAR_Bounce_First=true;
			mycar.CAR_Bounce_X=-1;
			mycar.CAR_Bounce=true;
			mycar.CAR_Bounce_WEST=true;
			if (!DD_Lock_Back_Surface()) {
				ErrorVille("Could not lock back surface");
			}	
		}

		// Play bump sound if need be
		if(bumping) {
			if(!(Status_Sound(bump)&(DSBSTATUS_PLAYING))) { 
				Play_Sound(bump,0);  
			}
		}
		
		// And the same for the other car
		if (isbumping) {
			// Set bump volume to coeff value
			Set_Sound_Volume(bump2,(volume_coeff/2));
			if(!(Status_Sound(bump2)&(DSBSTATUS_PLAYING))) { 
				Play_Sound(bump2,0);
			}
		}

		// unlock back surface
		DD_Unlock_Back_Surface();

		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                     Check Timers,Winners and Checkpoint                                  */
		/* --------------------------------------------------------------------------------------------------------------*/	
		
		// This player is not winning... yet
		iwin=false;

		/* Set off the start timer? */

		if (Start_First_Timer) {
				mycar.CAR_Reset_Clock();							     // reset the lap timer
				Start_First_Timer=false;
		}
		/* Check if Car just passed checkpoint */
	
		if((Collision_Test(checkpoint.x, checkpoint.y, checkpoint.width, checkpoint.height,
			mycar.Mapx, mycar.Mapy,  CAR_SIZE_X, CAR_SIZE_Y))&& (!mycar.CAR_Check_CheckPoint())) {
				// if car 1 crossed checkpoint 
				mycar.CAR_Passed_CheckPoint=true;	
		}
				
		/* Check Lap increment and Winner */	

		if((Collision_Test(starting_line.x, starting_line.y, starting_line.width, starting_line.height,
			mycar.Mapx, mycar.Mapy, CAR_SIZE_X, CAR_SIZE_Y)) && (mycar.CAR_Check_CheckPoint())) {
				// if car 1 has crossed the finish line (and checkpoint) 
				mycar.CAR_Check_Best_Lap();						     // check the best lap time
				Play_Sound(Lap);												     // play lap sound 
				mycar.CAR_Increment_Laps_Completed();    // increment the number of laps completed
				mycar.CAR_Passed_CheckPoint=false;

				if(mycar.CAR_Check_Laps_Completed()>MAP_MAXLAPS) {
					// if car 1 has finished the race
					Game_Start=false;												// Reset bools
					Lights_Finished=false;
					Victory_Dance_Finished=false;
					newgame=true;
					Boing_Sound=true;												// Boing sound is go
					Winner=1;																  // We have a winner!
					iwin=true;
					mycar.CAR_Inc_Score();									  // increment score
					Reset_Laps();														  // Reset laps to 0
					mycar.CAR_Stop_Clock();
				} else {
					mycar.CAR_Reset_Clock();							   // reset the lap timer
				}
		}
		if(otherwin) {
			// if car 2 has finished the race
			Game_Start=false;											    	// Reset bools
			Lights_Finished=false;
			Victory_Dance_Finished=false;
			mycar.CAR_Passed_CheckPoint=false;
			newgame=true;
			Boing_Sound=true;													// Boing sound is go
			Winner=2;																      // We have a winner!
			yourcar.CAR_Inc_Score();									  // increment score
			Reset_Laps();	
			otherwin=false;
			mycar.CAR_Stop_Clock();
		}

		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                               Check for crash Collisions                                                  */
		/* --------------------------------------------------------------------------------------------------------------*/	
		
		/* check for 2 car collision */
		if(Game_Start) {
			if (!mycar.Wait_Frame_Explosion) {
				if (  (Collision_BOBS(&mycar.car, &yourcar.car)) || (other_exploding)) {
					// cars have collided, play explosion and kill both cars
					Play_Explosion();
					// set new car positions
					if (joining) {
						mycar.SMapx=MAP1_RIGHT_STARTX;
						mycar.SMapy=MAP1_RIGHT_STARTY;
						yourcar.SMapx=MAP1_LEFT_STARTX;
						yourcar.SMapy=MAP1_LEFT_STARTY;
					} else {
						mycar.SMapx=MAP1_LEFT_STARTX;
						mycar.SMapy=MAP1_LEFT_STARTY;
						yourcar.SMapx=MAP1_RIGHT_STARTX;
						yourcar.SMapy=MAP1_RIGHT_STARTY;
					}
					// tell carz they are dead
					mycar.CAR_Dead();
					yourcar.CAR_Dead();
					// Stop the clock
					mycar.CAR_Stop_Clock();
					other_exploding=false;
					exploding=true;
					mycar.CAR_Bounce=false;
				}
			} else {
				// account for frame's difference (immunity)
				Explosion_Immunity_Counter++;
				if (Explosion_Immunity_Counter>NUM_IMMUNITY_FRAMES) {
					mycar.Wait_Frame_Explosion=false;
					Explosion_Immunity_Counter=0;
				}
				other_exploding=false;
			}
		}
		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                                       Draw Background bmp                                               */
		/* --------------------------------------------------------------------------------------------------------------*/	

		// unlock secondary surface 
		DD_Unlock_Back_Surface();
		// Draw
		Draw_BOB(&background, lpddsback);
		
	} else {
		// The game has not started

		// Car is not bouncing anymore
		mycar.CAR_Bounce=false;
		mycar.CAR_Laps_Completed=0;					     // Laps completed

		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                                       Draw Background bmp                                               */
		/* --------------------------------------------------------------------------------------------------------------*/	
		// Unlock
		DD_Unlock_Back_Surface();
		// Draw
		Draw_BOB(&background, lpddsback);
		
		/* Stop engine sounds */

		Stop_Sound(Engine1);
		Stop_Sound(Engine2);

		/* --------------------------------------------------------------------------------------------------------------*/
		/*                                         Play victory dance or lights                                                     */
		/* --------------------------------------------------------------------------------------------------------------*/	

		if (Victory_Dance_Finished) {
			// if victory dance finished then execute lights code

			/* Draw car BOBs at starting position */

			if(joining) {
				mycar.CAR_Draw_Car_Picture1(0, MIDDLE_SCREEN);
				yourcar.CAR_Draw_Car_Picture1(0, SCREEN_POS_JOIN);
				mycar.Mapx=MAP1_RIGHT_STARTX;
				mycar.Mapy=MAP1_RIGHT_STARTY;

			} else {
				mycar.CAR_Draw_Car_Picture1(0, MIDDLE_SCREEN);
				yourcar.CAR_Draw_Car_Picture1(0, SCREEN_POS_HOST);
				mycar.Mapx=MAP1_LEFT_STARTX;
				mycar.Mapy=MAP1_LEFT_STARTY;
			}

			/* Play lights animation */

			Set_Pos_BOB(&lights,MAP_LIGHT_GREEN);		// start of animation
			Animate_BOB(&lights);													 // animate lights 
			Draw_BOB(&lights, lpddsback);									// draw lights

			if ((lights.curr_frame==0)||(lights.curr_frame==((LIGHTS_FRAMES/2)-1))) {
				// if it's the red or yellow light
				Play_Sound(Red);														 // Play light sound
			}
			if (lights.curr_frame==LIGHTS_FRAMES-2) {
				// Lights are green
				// Play Go sound
				Play_Sound(Go);		
			}

			/* set positions of car BOBs to start */

			Set_Pos_BOB(&mycar.car,MIDDLE_SCREEN);
			if (joining) {
				Set_Pos_BOB(&yourcar.car,SCREEN_POS_JOIN);
			} else {
					Set_Pos_BOB(&yourcar.car,SCREEN_POS_HOST);
			}
		
			if (lights.anim_state) {
				// if lights have finished animation
									
				/* Animation finished */
				
				lights.anim_state=false;												 // Reset light animation info
				lights.curr_frame=0;
				Set_Animation_BOB(&lights,0);									

				/* set positions of car BOBs to start */

				Set_Pos_BOB(&mycar.car,MIDDLE_SCREEN);
				if (joining) {
					Set_Pos_BOB(&yourcar.car,SCREEN_POS_JOIN);
					yourcar.Recx=MAP1_LEFT_STARTX;
					yourcar.Recy=MAP1_LEFT_STARTY;
				} else {
					Set_Pos_BOB(&yourcar.car,SCREEN_POS_HOST);
					yourcar.Recx=MAP1_RIGHT_STARTX;
					yourcar.Recy=MAP1_RIGHT_STARTY;
				}

				/* Acceleration = 0 */
				
				mycar.CAR_Reset_Velocity();
				mycar.CAR_Reset_Direction();

				/* Start new game (bool) */

				Game_Start=true;
				Start_First_Timer=true;
			}
		} else {
			// if victory dance not finished
			
			/* Show victory dance */
			
			if (Winner==1) {
				// if car 1 is the winner
				Boing_Play();												 	 // Check boing, if true play sound	
				Draw_BOB(&yourcar.car,lpddsback);	  // Draw car2 in its last position
				if (Size_Incrementing) {
					// if size of BOB increasing, decrease x and y coords (top left corner)
					mycar.car.x-=VICTORY_SIZE_INC;
					mycar.car.y-=VICTORY_SIZE_INC;
				} else {
					// if size of BOB decreasing, increase x and y coords
					mycar.car.x+=VICTORY_SIZE_INC;
					mycar.car.y+=VICTORY_SIZE_INC;
				}

				/* draw the scaled BOB (center remains constant) */

				Draw_Scaled_BOB(&mycar.car, CAR_SIZE_X+Size_Addition, 
														CAR_SIZE_Y+Size_Addition, lpddsback);
			} else {
				Boing_Play();												     // Check boing, if true play sound	
				Draw_BOB(&mycar.car,lpddsback);		   // Draw car1 in its last position
				if (Size_Incrementing) {
					// if size of BOB increasing, decrease x and y coords (top left corner)
					yourcar.car.x-=VICTORY_SIZE_INC;
					yourcar.car.y-=VICTORY_SIZE_INC;
				} else {
					// if size of BOB decreasing, increase x and y coords
					yourcar.car.x+=VICTORY_SIZE_INC;
					yourcar.car.y+=VICTORY_SIZE_INC;
				}

				/* draw the scaled BOB (center remains constant) */

				Draw_Scaled_BOB(&yourcar.car, CAR_SIZE_X+Size_Addition, 
					CAR_SIZE_Y+Size_Addition, lpddsback);
			}
			if (Size_Addition>=MAX_VICTORY_SIZE) {
				// if the size is 32 the start decrementing the size of the BOB
				Size_Incrementing=false;
				Boing_Sound=false;								// dont play boing anytime soon
			} 
			
			if (Size_Addition<1) {
				// if the size is less the 1 start incrementing the size of the BOB
				Size_Incrementing=true;
				Number_Incs++;									    // We only want BOUNCE bounces (counter)
				if (Number_Incs>BOUNCE) {
					// if bounces have finished set up for next victory dance
					Victory_Dance_Finished=true;
					Number_Incs=0;
					Boing_Sound=false;
				} else {
					// play the boing sound = true
					Boing_Sound=true;
				}
			}
			if (Size_Incrementing) {
				// increment or decrement by 2 pixels
				Size_Addition+=VICTORY_SIZE_INC*VICTORY_SIZE_INC;
			}	else {
				Size_Addition-=VICTORY_SIZE_INC*VICTORY_SIZE_INC;
			}
		}
	}

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                               Draw  BOBs, Accel, etc.			                                          */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// Unlock back surface
	DD_Unlock_Back_Surface();

	/* Draw carz and explosions */

	if((Game_Start)||(newgame)) {
		mycar.CAR_Draw_Car();
		yourcar.CAR_Draw_Car();
		newgame=false;
	}

	/* Check Explosions */

	if(mycar.explosion_happening) {
		mycar.CAR_Draw_Explosion();
		yourcar.CAR_Draw_Explosion();
	}
	
	/* Draw display */

	Draw_BOB(&display, lpddsback);

	/* Acceleration Bars */
	if((Game_Start)) {
		mycar.CAR_Draw_Acceleration_Bar(MAP_CAR1_ACCEL_BAR);
	}

	/* Flags */

	Print_Flags(MAP_Car1.x, MAP_Car1.y, mycar.CAR_Check_Laps_Completed());

	/* Score */
	
	if (joining) {
		// depending on which player this is
		Print_Score(MAP_CAR2_SCORE,2,mycar.CAR_Get_Score());
		Print_Score(MAP_CAR1_SCORE,1,otherScore);
	} else {
		Print_Score(MAP_CAR1_SCORE,1,mycar.CAR_Get_Score());
		Print_Score(MAP_CAR2_SCORE,2,otherScore);
	}

	/* --------------------------------------------------------------------------------------------------------------*/
	/*                                                        Draw Timers, etc.                                                         */
	/* --------------------------------------------------------------------------------------------------------------*/	
	
		/* Print Timers */
	
		Print_Time(MAP_CAR1_LAP_TIME,(int)mycar.CAR_Get_Clock());
		Print_Time(MAP_CAR1_BEST_LAP_TIME,(int)mycar.CAR_Return_Best_Lap());

	/* --------------------------------------------------------------------------------------------------------------*/
	/*																	 Flip			                                                            */
	/* --------------------------------------------------------------------------------------------------------------*/	

	// flip the surface 
	DD_Flip();
// return success
return(1);
}

// Chill! //

