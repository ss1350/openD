/*!
*	\file		keyb_suota.c
*	\brief		cat-iq 2.0 data services tests
*	\Author		stein 
*
*	@(#)	%filespec: keyb_suota.c~7 %
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if ! defined ( WIN32 )
    #include <unistd.h>
    #include <termios.h>
    #include <sys/time.h>
    #include <signal.h>
    #include <sys/msg.h>
#else
    #include <conio.h>
    #include <io.h>
#endif

#include <fcntl.h>
#include <errno.h>


#include "cmbs_platf.h"
#include "cmbs_api.h"
#include "cfr_ie.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "appsuota.h"
#include "appsuotafile.h"
#include "tcx_keyb.h"
#include <tcx_util.h>

#ifndef WIN32

/** CMBS Changes  *   */
#include "suota/suota.h"

extern u16 app_HandsetMap( char * psz_Handsets );
extern t_st_SUOTAFlags g_SUOTAFlags;

void keyb_SuotaSendVersionAvailable( void )
{
    char InputBuffer[100];

    ST_SUOTA_UPGRADE_DETAILS  st_HSVerAvail;
    u16 u16_Handset = 0xFF, u16_RequestId;
    ST_VERSION_BUFFER st_SwVersion;

    printf("\n SUOTA Version Available command");

    // read HS number
    printf("\nEnter HS number: ");
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_Handset = app_HandsetMap( InputBuffer);
    memset(InputBuffer,0,sizeof(InputBuffer));

    // read delay in minutes
    printf("Enter delay in minutes: ");
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    st_HSVerAvail.u16_delayInMin = atoi(InputBuffer);
    memset(InputBuffer,0,sizeof(InputBuffer));

    // no settings for user interaction and URL nums
    st_HSVerAvail.u8_UserInteraction    = 0;
    st_HSVerAvail.u8_URLStoFollow       = 1;

    // read version strings
    printf("Enter SW version string(max 20 chars) : ");
    tcx_gets( InputBuffer, CMBS_MAX_VERSION_LENGTH );

    st_SwVersion.u8_VerLen = (u8)strlen(InputBuffer);

    st_SwVersion.type = CMBS_SUOTA_SW_VERSION;

    if ( st_SwVersion.u8_VerLen <= CMBS_MAX_VERSION_LENGTH )
    {
        memcpy(st_SwVersion.pu8_VerBuffer,InputBuffer,st_SwVersion.u8_VerLen);
    }
    else
    {
        st_SwVersion.u8_VerLen = 0;
    }

    printf( "Enter Request ID         (dec): " );
    memset( InputBuffer, 0, sizeof(InputBuffer) );
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_RequestId = atoi( InputBuffer );

    app_SoutaSendHSVersionAvail(st_HSVerAvail,u16_Handset,&st_SwVersion, u16_RequestId);
}
//////////////////////////////////////////////////////////////////////////
void keyb_SuotaSendNewVersionInd( void )
{
    char    InputBuffer[20];
    u16     u16_Handset, u16_RequestId;

    printf("\nSUOTA New Version Ind command");

    // read HS number
    printf("\nEnter HS number: ");
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_Handset = app_HandsetMap( InputBuffer);

    printf( "Enter Request ID         (dec): " );
    memset( InputBuffer, 0, sizeof(InputBuffer) );
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_RequestId = atoi( InputBuffer );

    app_SoutaSendNewVersionInd(u16_Handset, SUOTA_GE00_SU_FW_UPGRAGE, u16_RequestId);
}
//////////////////////////////////////////////////////////////////////////
void keyb_SuotaSendURL( void )
{
    ST_URL_BUFFER st_Url;
    char    InputBuffer[100];
    u16     u16_Handset, u16_RequestId ;
    u8      u8_URLToFollow;

    printf("\nSUOTA Send URL command");

    // read HS number
    printf("\nEnter HS number: ");
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_Handset = app_HandsetMap( InputBuffer);
    memset(InputBuffer,0,sizeof(InputBuffer));

    // read num of URLs
    printf("\nEnter num of URLs to follow: ");
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u8_URLToFollow = atoi( InputBuffer);
    memset(InputBuffer,0,sizeof(InputBuffer));

    // read URL string 
    printf("\nEnter URL addres (max %d chars):\n",CMBS_MAX_URL_SIZE);
    tcx_gets( InputBuffer, CMBS_MAX_URL_SIZE);

    st_Url.u8_UrlLen    = (u8)strlen(InputBuffer);
    if ( st_Url.u8_UrlLen <= CMBS_MAX_URL_SIZE )
    {
        memcpy(st_Url.pu8_UrlBuffer,InputBuffer,st_Url.u8_UrlLen);
    }
    else
    {
        st_Url.u8_UrlLen = 0;
    }

    printf( "Enter Request ID         (dec): " );
    memset( InputBuffer, 0, sizeof(InputBuffer) );
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_RequestId = atoi( InputBuffer );

    app_SoutaSendURL( u16_Handset, u8_URLToFollow, &st_Url, u16_RequestId );
}

void keyb_SuotaSendNack( void )
{
    char    InputBuffer[20];
    u16     u16_Handset, u16_RequestId;
    E_SUOTA_RejectReason enReason;

    printf("\nSUOTA Send NACK command");

    // read HS number
    printf("\nEnter HS number:");
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_Handset = app_HandsetMap( InputBuffer);
    memset(InputBuffer,0,sizeof(InputBuffer));

    // read reason
    printf("\nEnter reason:");
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    enReason = atoi( InputBuffer);

    printf( "Enter Request ID         (dec): " );
    memset( InputBuffer, 0, sizeof(InputBuffer) );
    tcx_gets( InputBuffer, sizeof(InputBuffer) );
    u16_RequestId = atoi( InputBuffer );

    app_SoutaSendNack(u16_Handset, enReason, u16_RequestId);
}


void keyb_SuotaQuit( void )
{
   // suota_quitThread();   
}


void keyb_SuotaThread( int pushMode )
{
    char InputBuffer[60] = {0};
    char InputHsBuffer[20] = {0};
    u16 portNumber=0;
    u16 u16_Handset=0;
    
     
    static bool threadCreate=0;
    if(g_SUOTAFlags.b_FileDownload)    
    {
        printf("\nEnter Port Number xxxx: ");
        tcx_gets( InputBuffer, sizeof(InputBuffer) );
        portNumber = atoi(InputBuffer);

        // Read Server IP Address
        printf("\nEnter IP Address xxx.xxx.xxx.xxx: ");
        tcx_gets( InputBuffer, sizeof(InputBuffer) );
    }
    if( pushMode)
    {
        printf("\nEnter HS number: ");
        tcx_gets( InputHsBuffer, sizeof(InputHsBuffer) );
        u16_Handset = app_HandsetMap( InputHsBuffer);
    }
    if( !threadCreate )
    {
        suota_createThread(InputBuffer,portNumber,pushMode,u16_Handset);   
        threadCreate = 1;
    }
}


void keyb_SuotaUnRegAppCB(void)
{
    app_SoutaUnRegAppCB();
}


void keyb_SuotaModeSelect(void)
{

    int Key;

    tcx_appClearScreen();
#ifdef SUOTA_CURL
    printf("\n Download file fron HTTP server? [y/n] :\n ");
    Key = tcx_getch();
    if(Key == 'y')
         g_SUOTAFlags.b_FileDownload = TRUE;
    else
#endif        
         g_SUOTAFlags.b_FileDownload = FALSE;

    printf("\n Header created by Host? [y/n] : \n");
    Key = tcx_getch();
    if(Key == 'y')
       g_SUOTAFlags.b_HeaderCreatedbyHost = TRUE;
    else
       g_SUOTAFlags.b_HeaderCreatedbyHost = FALSE;

    printf("\n Early chunk prepare? [y/n] : \n");
    Key = tcx_getch();
    if(Key == 'y')
       g_SUOTAFlags.b_EarlyChunkPrepare = TRUE;
    else
       g_SUOTAFlags.b_EarlyChunkPrepare = FALSE;        

    printf("Chosen SUOTA configuration is:\n");
    printf("Download file: %s\n", g_SUOTAFlags.b_FileDownload == TRUE ? "YES" : "NO");
    printf("Header created by %s\n", g_SUOTAFlags.b_HeaderCreatedbyHost == TRUE ? "HOST" : "TARGET");
    printf("Early chunk prepare is %s used\n", g_SUOTAFlags.b_EarlyChunkPrepare == TRUE ? "" : "NOT");

}

void keyb_SuotaAbort(void)
{
    app_SoutaAbort(g_SUOTAFlags.u32_AbortSessionId);
}

void keyb_SuotaLoop( void )
{
    int n_Keep = TRUE;
   
    while ( n_Keep )
    {
        printf ("------------------------------------\n"); 
        printf ("Modes:\t(change modes using option \"Select SUOTA mode\" below):\n");
        printf ("Download file: %s\n", g_SUOTAFlags.b_FileDownload == TRUE ? "YES" : "NO");
        printf ("Header created by %s\n", g_SUOTAFlags.b_HeaderCreatedbyHost == TRUE ? "HOST" : "TARGET");
        printf ("Early chunk prepare is %s used\n", g_SUOTAFlags.b_EarlyChunkPrepare == TRUE ? "" : "NOT"); 
        printf ( "------------------------------------\n");
        printf ( "1 => Send HS Version Available Ind  \n");
        printf ( "2 => Send New Version Ind	   	      \n");
        printf ( "3 => Send URL indication 		      \n");
        printf ( "4 => Send NACK					  \n");
        printf ( "5 => Start SUOTA Process			  \n");
        printf ( "6 => Start Suota Push Mode		  \n");
        printf ( "7 => Select SUOTA mode     		  \n");
        printf(  "8 => Un-register APP callback		  \n");
		printf ( "a => Abort SUOTA process     		  \n");
        printf ( "- - - - - - - - - - - - - - -	      \n");
        printf ( "q => Return to Interface Menu       \n");

        switch ( tcx_getch() )
        {
        case ' ':
            tcx_appClearScreen();
            break;

        case '1':
            keyb_SuotaSendVersionAvailable();
            break;

        case '2':
            keyb_SuotaSendNewVersionInd();
            break;

        case '3':
            keyb_SuotaSendURL();
            break;

        case '4':
            keyb_SuotaSendNack();
            break;

        case '5':
            keyb_SuotaThread(0);
            break;

        case '6':
            keyb_SuotaThread(1);
            break;
            
        case '7':
            keyb_SuotaModeSelect();
            break;

        case '8':
            keyb_SuotaUnRegAppCB();
            break; 

		case 'a':
            keyb_SuotaAbort();
            break;
            
        case 'q':
            keyb_SuotaQuit();
            n_Keep = FALSE;
            break;          
        }
    }

}

#else //WIN32


void keyb_SuotaInit(void)
{
	app_SoutaParseCfgFile();

	cmbs_dsr_suota_Init(g_cmbsappl.pv_CMBSRef);
}

void keyb_SuotaLoop( void )
{
	int n_Keep = TRUE;

	while ( n_Keep )
	{
		printf ( "\n\n ----------------------------------- \n");
		printf ( " Windows SUOTA Menu \n");
		printf ( " - - - - - - - - - - - - - - - - - - \n");
		printf ( " 1 => SUOTA Init \n");
		printf ( " 2 => SUOTA Local File Enables \n");
		printf ( " 3 => SUOTA Local File Disables \n");
		printf ( " - - - - - - - - - - - - - - - - - - \n");
		printf ( " q => Return to HAN Test Menu\n" );
		printf ( " ----------------------------------- \n");
		switch ( tcx_getch() )
		{
			case 'x':
	        	tcx_appClearScreen();
		    	 break;							
			case '1':
				keyb_SuotaInit();
				break;
			case '2':
				app_SuotaFileEnable(TRUE);
			break;
			case '3':
				app_SuotaFileEnable(FALSE);
				break;
			case 'q':
			case 'Q':
				n_Keep = FALSE;
				break;

			default:
				break;
		}

	}
}


#endif //WIN32

/*---------[End Of File]---------------------------------------------------------------------------------------------------------------------------*/
