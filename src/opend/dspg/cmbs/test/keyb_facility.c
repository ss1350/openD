/*!
*	\file		keyb_catiq.c
*	\brief		firmware update test
*	\Author		stein 
*
*	@(#)	%filespec: keyb_facility.c~9.1.1 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                \n
*	----------------------------------------------------------------------------\n
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#include "appfacility.h"
#include "tcx_keyb.h"
#include <tcx_util.h>
#ifdef PNCAP
#include "appcall.h"
#include "etp.h"
#include "pie.h"
#include "utils.h"
#include "pncap_codes.h"
#include "pncap_def.h"
//#include "yuv2rgb.h"
#endif

#ifdef PNCAP
extern u16 _keyb_CallIdInput(void);
#endif

#define CMBS_PROP_EVENT_MAX_PAYLOAD_SIZE	CMBS_PROP_EVENT_LENGTH

void keyb_CatIqMWI( void )
{
   u16               u16_RequestId, u16_Messages, u16_HSMask=0;
   u8                u8_LineId, u8_Type;
   E_CMBS_MWI_TYPE   eType;
   char              InputBuffer[20];
   
   printf( "Enter Request ID         (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "Enter Line ID            (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineId = atoi( InputBuffer );

   printf( "Enter number of messages (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_Messages = atoi( InputBuffer );

   printf( "Enter message type: 0-Voice, 1-SMS, 2-Email: " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_Type = atoi( InputBuffer );
   switch( u8_Type )
   {
      case 1:
      eType = CMBS_MWI_SMS;
      break;

      case 2:
      eType = CMBS_MWI_EMAIL;
      break;

      default:
      eType = CMBS_MWI_VOICE;
      break;
   }

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);

   app_FacilityMWI( u16_RequestId, u8_LineId, u16_Messages, u16_HSMask, eType );
}


void keyb_CatIqMissedCalls( void )
{
   u16            u16_RequestId, u16_Messages, u16_TotalNumMessages, u16_HSMask=0;
   u8             u8_LineId;
   bool           bNewMissedCall;
   char           InputBuffer[20];

   printf( "Enter Request ID         (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "Enter Line ID            (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineId = atoi( InputBuffer );

   printf( "Enter number of unread calls    (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_Messages = atoi( InputBuffer );

   printf( "Enter number of total (read + unread) calls    (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_TotalNumMessages = atoi( InputBuffer );

   printf( "Enter 1 - New missed call, 0 - No new missed calls: " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   bNewMissedCall = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);

   app_FacilityMissedCalls( u16_RequestId, u8_LineId, u16_Messages, u16_HSMask, bNewMissedCall, u16_TotalNumMessages );
}


void keyb_CatIqListChanged( void )
{
   u16            u16_RequestId, u16_HSMask=0;
   u8             u8_ListId, u8_ListEntries, u8_LineId, u8_LineSubType;
   char           InputBuffer[20];

   printf( "Enter Request ID         (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "Enter List ID            (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_ListId = atoi( InputBuffer );

   printf( "Enter number of list entries  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_ListEntries = atoi( InputBuffer );

   printf( "Enter Line ID (255 for all lines)  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineId = atoi( InputBuffer );

   printf( "Enter Line Subtype (0 - Line identifier for external call, 3 - Relating-to line identifier, 4 - All lines  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineSubType = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);
   app_FacilityListChanged( u16_RequestId, u8_ListId, u8_ListEntries, u16_HSMask, u8_LineId, u8_LineSubType );
}


void              keyb_CatIqWebContent( void )
{
   u16            u16_RequestId, u16_HSMask=0;
   u8             u8_WebContents;
   char           InputBuffer[20];
    
   printf( "Enter Request ID         (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "Enter number of web contents  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_WebContents = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);
   app_FacilityWebContent( u16_RequestId, u8_WebContents, u16_HSMask );
}


void              keyb_CatIqPropEvent( void )
{
   u16            u16_RequestId, u16_HSMask=0;
   u8             u8_DataLen, u8_Data[CMBS_PROP_EVENT_LENGTH];
   char           InputBuffer[CMBS_PROP_EVENT_LENGTH*2+2];
   int            i, j;

   printf( "Enter Request ID          (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "Enter Data Length (dec. max 40): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_DataLen = (u8)atoi( InputBuffer );
   u8_DataLen = MIN(CMBS_PROP_EVENT_MAX_PAYLOAD_SIZE, u8_DataLen);
   printf( "Enter Data ( example: to send 5, 3A, 60 - type 053A60 (hex string): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   memset( u8_Data, 0, sizeof(u8_Data) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );

   for( i=0, j=0; i < u8_DataLen; i++ )
   {
      u8_Data[i] = app_ASC2HEX( InputBuffer+j );
      j += 2;
   }

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);	

   app_FacilityPropEvent( u16_RequestId, 0, u8_Data, u8_DataLen, u16_HSMask );
}

void              keyb_CatIqPropEventNotification(void)
{
	u16            u16_RequestId, u16_PropEvent = 0;
	u8             u8_DataLen, u8_Data[CMBS_PROP_EVENT_LENGTH];
	char           InputBuffer[CMBS_PROP_EVENT_LENGTH*2+2];
	int            i, j;

	printf("Enter Request ID          (dec): ");
	memset(InputBuffer, 0, sizeof(InputBuffer));
	tcx_gets(InputBuffer, sizeof(InputBuffer));
	u16_RequestId = atoi(InputBuffer);

	printf("Enter Prop event notification ID (dec. max 63): ");
	memset(InputBuffer, 0, sizeof(InputBuffer));
	tcx_gets(InputBuffer, sizeof(InputBuffer));
	u16_PropEvent = (u16)atoi(InputBuffer);

	printf("Enter Data Length (dec. max 40): ");
	memset(InputBuffer, 0, sizeof(InputBuffer));
	tcx_gets(InputBuffer, sizeof(InputBuffer));
	u8_DataLen = (u8)atoi(InputBuffer);
	u8_DataLen = MIN(CMBS_PROP_EVENT_MAX_PAYLOAD_SIZE, u8_DataLen);
	printf("Enter Data ( example: to send 5, 3A, 60 - type 053A60 (hex string): ");
	memset(InputBuffer, 0, sizeof(InputBuffer));
	memset(u8_Data, 0, sizeof(u8_Data));
	tcx_gets(InputBuffer, sizeof(InputBuffer));

	for (i = 0, j = 0; i < u8_DataLen; i++)
	{
		u8_Data[i] = app_ASC2HEX(InputBuffer + j);
		j += 2;
	}

	printf("Enter handset mask ( e.g. 1,2,3,4 or none or all):\n");
	memset(InputBuffer, 0, sizeof(InputBuffer));
	tcx_gets(InputBuffer, sizeof(InputBuffer));

	
	app_FacilityPropEventNotification(u16_RequestId, u16_PropEvent, u8_Data, u8_DataLen, InputBuffer);
}

void              keyb_CatIqTimeUpdate( void )
{
   u16            u16_RequestId, u16_HSMask=0;
   char           InputBuffer[32];
   ST_DATE_TIME   st_DateAndTime;

   time_t t;
   struct tm *t_m;
   t=time(NULL);
   t_m=localtime(&t);

   st_DateAndTime.e_Coding = CMBS_DATE_TIME;
   st_DateAndTime.e_Interpretation = CMBS_CURRENT_TIME;

   st_DateAndTime.u8_Year  = t_m->tm_year - 100;
   st_DateAndTime.u8_Month = t_m->tm_mon + 1;
   st_DateAndTime.u8_Day   = t_m->tm_mday;
   
   st_DateAndTime.u8_Hours = t_m->tm_hour;
   st_DateAndTime.u8_Mins  = t_m->tm_min;
   st_DateAndTime.u8_Secs  = t_m->tm_sec;

   st_DateAndTime.u8_Zone  = 8;   

   printf( "Sending Date:20%02d-%02d-%02d Time:%02d:%02d:%02d Zone:0x%02d\n",
            st_DateAndTime.u8_Year, st_DateAndTime.u8_Month, st_DateAndTime.u8_Day,
            st_DateAndTime.u8_Hours, st_DateAndTime.u8_Mins, st_DateAndTime.u8_Secs,
            st_DateAndTime.u8_Zone );

   printf( "Enter Request ID          (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);
   app_FacilityDateTime(u16_RequestId, &st_DateAndTime, u16_HSMask);
}

void              keyb_CatIqTimeRecovery( void )
{
   u16            u16_RequestId, u16_HSMask=0;
   char           InputBuffer[32];
   ST_DATE_TIME   st_DateAndTime;

   /* not used in this procedure, solve nighly build error
   time_t t;
   struct tm *t_m;
   t=time(NULL);
   t_m=localtime(&t);
   */

   memset( &st_DateAndTime, 0, sizeof(ST_DATE_TIME) );

   st_DateAndTime.e_Coding = CMBS_DATE_TIME;
   st_DateAndTime.e_Interpretation = CMBS_REQUEST_CURR_TIME;

   printf( "Sending current time/date recovery\n");

   printf( "Enter Request ID          (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);
   app_FacilityDateTime(u16_RequestId, &st_DateAndTime, u16_HSMask);
}

void    keyb_CatIqLineUseIndication(void)
{
   u32 u32_Temp, u32_LineUseStatus= 0;
   u16               u16_RequestId, u16_HSMask;
   u8                u8_LineId;
   char              InputBuffer[20];

   printf( "\nEnter Request ID         (dec): \n" );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "\nEnter Line ID            (dec): \n" );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineId = atoi( InputBuffer );

   printf("\nEnter Line Use Status (0 - 'idle', 1 - 'in-use', 2 - 'busy')...\n");
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u32_LineUseStatus = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%x", &u32_Temp);
   u16_HSMask = (u16)u32_Temp;

   app_FacilityLineUseIndication(u16_RequestId, u8_LineId, u32_LineUseStatus, u16_HSMask);
}

void    keyb_CatIqHSUseIndication()
{
   u16               u16_RequestId, u16_HSMask;
   u8                u8_LineId;
   char              InputBuffer[20];
   u32 u32_Temp;
   
   printf( "\nEnter Request ID         (dec): \n" );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "\nEnter Line ID            (dec): \n" );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineId = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%x", &u32_Temp);
   u16_HSMask = (u16)u32_Temp;

   app_FacilityHSUseIndication(u16_RequestId, u8_LineId, u16_HSMask);
}

void    keyb_CatIqDiagnosticIndication()
{
   u8 u8_DiagnosticType = 0;
   u16               u16_RequestId, u16_HSMask;
   u8                u8_LineId;
   char              InputBuffer[20];
   u32 u32_Temp;

   printf( "\nEnter Request ID         (dec): \n" );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "\nEnter Line ID            (dec): \n" );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineId = atoi( InputBuffer );

   printf("\nEnter Diagnostic type: (1 - 'Line related', 2 - 'Non-Line related')...\n");
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_DiagnosticType = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%x", &u32_Temp);
   u16_HSMask = (u16)u32_Temp;

   app_FacilityDiagnosticIndication( u16_RequestId, u8_LineId, u8_DiagnosticType, u16_HSMask);
}


void    keyb_CatIqSMSMessageNotification()
{
   u8 u8_SMSType = 0;
   u16  u16_HSMask = 0, u16_NumOfUnreadMsgs = 0, u16_TotalNumOfMsgs = 0;
   char  InputBuffer[20];
   u32 u32_Temp;

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%x", &u32_Temp);
   u16_HSMask = (u16)u32_Temp;

   printf("\nEnter SMS type: (1 - 'New SMS', 2 - 'No new SMS')...\n");
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_SMSType = atoi( InputBuffer );

   printf("\nEnter total number of messages ...\n");
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_TotalNumOfMsgs = atoi( InputBuffer );

   printf("\nEnter number of unread messages ...\n");
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_NumOfUnreadMsgs = atoi( InputBuffer );

   app_FacilitySMSMessageNotification(u8_SMSType, u16_TotalNumOfMsgs, u16_NumOfUnreadMsgs, u16_HSMask);
}
#ifdef PNCAP
void keyb_PnCapPhoneBookAndDataCall( void )
{
	int            n_Keep = TRUE;
	char ch_cli[20];
	char ch_cni[20];
	u16 u16_CallId;
	u16 u16_HsNo;
   
   while( n_Keep )
   {
//      tcx_appClearScreen();
      printf ( "-----------------------------\n" );
      printf ( "----    PNCAP PHONEBOOK AND DATA CALL  ----\n" );
      printf ( "-----------------------------\n" );
      printf ( "A/a => Data Call Start/pncap_pie_phonebook_start_transfer \n");
      printf ( "B/b => Data Call Cfm/pncap_pie_find_phonebook_cfm \n");
      printf ( "C/c => Insert Phonebook entry/pncap_pie_set_phonebook_insert_entry \n");
      printf ( "D/d => Close Data Call/pncap_pie_set_phonebook_stop_transfer\n");
      printf ( "P/p => Send Data Call Proceeding\n");
      printf ( "I/i =>   Local Phone Book Display/pncap_pie_set_phonebook_activate\n");
      printf ( "- - - - - - - - - - - - - - - \n");      
      printf ( "q => Return to PncapMenu\n" );
      switch ( tcx_getch() )
      {           
         case 'A':
	  case 'a':
	  	{
			char  InputBuffer[5];
			s8 s8_HsNum;
			printf( "Enter HS Number [1 to 5] to which Data Call To be Established " );
			memset( InputBuffer, 0, sizeof(InputBuffer) );
			tcx_gets( InputBuffer, sizeof(InputBuffer) );
			s8_HsNum = atoi( InputBuffer );
			appcall_PnCapDataCallStart(s8_HsNum);
	  	}
            break;

         case 'B':
	  case 'b':
	  	{
			u32 u32_CallId;
			u32 u32_HsMask;
			printf(" \nEnter CallId (0xabcd OR 0xffff)To which Data/Phone book Transfer to be stopped:\n");
			tcx_scanf("%x", &u32_CallId);
			printf(" \nEnter the HS Number[1 to 5]For Whom Cfm to be sent:\n");
                     tcx_scanf("%x", &u32_HsMask);
			appcall_PnCapDataCallCfm((u16)u32_CallId,(u16)u32_HsMask);
	  	}
               break;;
            break;

         case 'C':
	  case 'c':
	  	printf(" \nEnter CallId To which Name & Number To be sent:\n");
		 u16_CallId = _keyb_CallIdInput();
		printf(" \nEnter Name For Phone book Entry:\n");
		tcx_gets(ch_cni, sizeof(ch_cni));
		printf(" \nEnter Number For Phone book Entry:\n");
		tcx_gets(ch_cli, sizeof(ch_cli));
		appcall_PnCapDataPacketSend((u8 *)ch_cni, (u8 *)ch_cli, u16_CallId);
            break;

         case 'D':
	  case 'd':
	  	printf(" \nEnter CallId To which Data/Phone book Transfer to be stopped:\n");
		 u16_CallId = _keyb_CallIdInput();
		 appcall_PnCapDataTransferStop(u16_CallId);
            break;

         case 'P':
	  case 'p':
		{
			u32 u32_CallId;
			u32 u32_HsMask;
			printf(" \nEnter CallId (0xabcd OR 0xffff)To which PNCAP Data Proc to be sent:\n");
			tcx_scanf("%x", &u32_CallId);
			printf(" \nEnter the HS Number[1 to 5]to whom PNCAP Data Proc to be sent:\n");
			tcx_scanf("%x", &u32_HsMask);
			appcall_PnCapDataCallProc((u16)u32_CallId,(u16)u32_HsMask);
		}
            break;
         case 'I':
	  case 'i':
	  	{
		   char  InputBuffer[5];
	         printf(" \nEnter the HS [1 to 5]whos Phone book to be activated:\n");
		  tcx_gets( InputBuffer, sizeof(InputBuffer) );
		 u16_HsNo = atoi( InputBuffer );
		  appcall_PnCapPhoneBookActivate(u16_HsNo );
	  	}
            break;
         case 'q':
            n_Keep = FALSE;
            break;          
      }
   }
}

void              keyb_PnCapLoop( void )
{
	int            n_Keep = TRUE;
	UnionPnCapFacility UnionPnCap;
	unsigned int u16_HSMsk;
	char Name[]="DSPG";
	char              InputBuffer[20];

   while( n_Keep )
   {
//      tcx_appClearScreen();
      printf ( "-----------------------------\n" );
      printf ( "----    PNCAP TEST MENU  ----\n" );
      printf ( "-----------------------------\n" );
      printf ( "D/d => Date And Time Synchrozitation \n");
      printf ( "M/m => MWI Sending \n");
      printf ( "N/n => Name and Number Setting \n");
      printf ( "P/p => PhoneBook And Data Call Menu\n");
      printf ( "- - - - - - - - - - - - - - - \n");      
      printf ( "q => Return to Interface Menu\n" );
	  
      switch ( tcx_getch() )
      {
           
         case 'D'://Hard coded values
         	{
		UnionPnCap.PnCapDateTime.date_mode = 0; // Ger. format
		UnionPnCap.PnCapDateTime.day = 02;// day 23
		UnionPnCap.PnCapDateTime.month = 02;// month 2
		UnionPnCap.PnCapDateTime.year = 15;// year 17
		UnionPnCap.PnCapDateTime.time_mode = 1; // 24h format
		UnionPnCap.PnCapDateTime.minutes = 02;//5
		UnionPnCap.PnCapDateTime.hours = 02;//17
		printf("Enter HS Mask to which Date & Time is to be sent; 0xff is max,0th bit for HS1");
		tcx_scanf("%x", &u16_HSMsk);
		appcall_PnCapAttribute(DateTimeSync, UnionPnCap, u16_HSMsk);
         	}
		 break;
	   case 'd': //User input
	   	{
		printf( "\nEnter date mode: [0-day/month/year, 1-month/day/year] " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapDateTime.date_mode = atoi(InputBuffer);

		printf( "\nEnter day[dd]: " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapDateTime.day =  atoi(InputBuffer);

		printf( "\nEnter month[mm]: " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapDateTime.month =  atoi(InputBuffer);

		printf( "\nEnter year[yy]: " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapDateTime.year =  atoi(InputBuffer);

		printf( "\nEnter time mode:[0-12h, 1-24h] " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapDateTime.time_mode =  atoi(InputBuffer);

		printf( "\nEnter minutes: " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapDateTime.minutes =  atoi(InputBuffer);

		printf( "\nEnter hours: " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapDateTime.hours =  atoi(InputBuffer);

		printf("\nEnter HS Mask to which Date & Time is to be sent; 0xff is max,0th bit for HS1");
		tcx_scanf("%x", &u16_HSMsk);
		
		appcall_PnCapAttribute(DateTimeSync, UnionPnCap, u16_HSMsk);	
	   	}
            break;

         case 'M': //Hard coded values
		{
		UnionPnCap.u8_MessagesWaiting = 3;
		printf("\nEnter HS Mask to which Message Waiting Indication is to be sent; \n0x0x001F is max,0th bit for HS1\n");
		tcx_scanf("%x", &u16_HSMsk);
		appcall_PnCapAttribute(MWISend, UnionPnCap, u16_HSMsk);
		}
		 break;

	   case 'm': //User Input
	   	{
		printf( "\nEnter number of messages (dec): " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.u8_MessagesWaiting= atoi( InputBuffer );
		
		printf("\nEnter HS Mask to which Message Waiting Indication is to be sent; \n0x0x001F is max,0th bit for HS1\n");
		tcx_scanf("%x", &u16_HSMsk);
		appcall_PnCapAttribute(MWISend, UnionPnCap, u16_HSMsk);
	   	}
		break;
	
         case 'N': //Hard coded values
		{
		memcpy(UnionPnCap.PnCapNameNum.IntName,Name,sizeof(Name));
             UnionPnCap.PnCapNameNum.u8_IntNum = 1;
		UnionPnCap.PnCapNameNum.u8_Coding = 0;
		//printf("Enter HS Mask to which Name & Numberto be sent; 0xff is max,0th bit for HS1");
		printf("\nEnter HS Mask to which Name & Number  to be sent; \n0x0x001F is max,0th bit for HS1\n");
		tcx_scanf("%x", &u16_HSMsk);
		appcall_PnCapAttribute(NameNumber, UnionPnCap, u16_HSMsk);
		}
            break;

         case 'n': //User Input
		{
		printf( "\nEnter handset internal name: " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		memcpy(UnionPnCap.PnCapNameNum.IntName,InputBuffer,sizeof(InputBuffer));
             	printf( "\nEnter internal handset number (dec): " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapNameNum.u8_IntNum= atoi( InputBuffer );

		printf( "\nEnter coding info (dec): " );
		memset( InputBuffer, 0, sizeof(InputBuffer) );
		tcx_gets( InputBuffer, sizeof(InputBuffer) );
		UnionPnCap.PnCapNameNum.u8_Coding= atoi( InputBuffer );
		printf("\nEnter HS Mask to which Name & Number  to be sent; \n0x0x001F is max,0th bit for HS1\n");
              tcx_scanf("%x", &u16_HSMsk);
		appcall_PnCapAttribute(NameNumber, UnionPnCap, u16_HSMsk);
		}
	  	break;
         case 'P':
	  case 'p':
               keyb_PnCapPhoneBookAndDataCall();
            break;
         case 'q':
            n_Keep = FALSE;
            break;          
      }
   }
}
#endif

void keyb_CatIqListChangedEx( void )
{
   u16            u16_RequestId, u16_HSMask=0;
   u8             u8_ListId, u8_ListEntries, u8_LineId, u8_LineSubType, i;
   char           InputBuffer[20];
   ST_APP_FACILITY_LCN_DETAILS st_Details;

   printf( "Enter Request ID         (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u16_RequestId = atoi( InputBuffer );

   printf( "Enter List ID            (dec): " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_ListId = atoi( InputBuffer );

   printf( "Enter number of list entries  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_ListEntries = atoi( InputBuffer );

   printf( "Enter Line ID (255 for all lines)  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineId = atoi( InputBuffer );

   printf( "Enter Line Subtype (0 - Line identifier for external call, 3 - Relating-to line identifier, 4 - All lines  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   u8_LineSubType = atoi( InputBuffer );

   printf("\nEnter HS mask (hex)...\n");
   tcx_scanf("%hX", &u16_HSMask);

   printf("\nEnter Exteneded Details count [Max 5]\n");
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   st_Details.u8_Count = atoi( InputBuffer );

   printf( "Origination HS (more than 1 Handsets - 0, otherwise Handset No  : " );
   memset( InputBuffer, 0, sizeof(InputBuffer) );
   tcx_gets( InputBuffer, sizeof(InputBuffer) );
   st_Details.u16_OrgHs = atoi( InputBuffer );

   for (i = 0; i < st_Details.u8_Count; i++)
   {
	   printf( "Change Type (0 - Edit, 1 - Add, 2 - Delete  : " );
	   memset( InputBuffer, 0, sizeof(InputBuffer) );
	   tcx_gets( InputBuffer, sizeof(InputBuffer) );
	   st_Details.st_SubDetails[i].u16_Change = atoi( InputBuffer );

	   printf( "Entry Id  : " );
	   memset( InputBuffer, 0, sizeof(InputBuffer) );
	   tcx_gets( InputBuffer, sizeof(InputBuffer) );
	   st_Details.st_SubDetails[i].u16_EntryId = atoi( InputBuffer );

	   printf( "Position Index  : " );
	   memset( InputBuffer, 0, sizeof(InputBuffer) );
	   tcx_gets( InputBuffer, sizeof(InputBuffer) );
	   st_Details.st_SubDetails[i].u16_PosIndex = atoi( InputBuffer );
   	}

   app_FacilityListChangedEx( u16_RequestId, u8_ListId, u8_ListEntries, u16_HSMask, u8_LineId, u8_LineSubType, &st_Details );
}


//		========== keyb_CatIqLoop ===========
/*!
		\brief         keyboard loop for CAT-iq menu
      \param[in,ou]  <none>
		\return		

*/
void              keyb_CatIqLoop( void )
{
   int            n_Keep = TRUE;
   
   while( n_Keep )
   {
//      tcx_appClearScreen();
      printf ( "-----------------------------\n" );
      printf ( "----    CAT IQ 2.0  ----\n" );
      printf ( "-----------------------------\n" );
      printf ( "1 => Send Message Waiting Indication\n");
      printf ( "2 => Send Missed Calls Notification\n");
      printf ( "3 => Send List Changed Notification\n");
      printf ( "4 => Send Web Content Notification\n");
      printf ( "- - - - - - - - - - - - - - - \n");
      printf ( "5 => Send Proprietary Event\n");
      printf ( "6 => Send Date_Time Update Event\n");
      printf ( "- - - - - - - - - - - - - - - \n");
      printf ( "----    CAT IQ 2.1  ----\n" );
      printf ( "- - - - - - - - - - - - - - - \n");      
      printf ( "7 => Send Line in use indication\n");
      printf ( "8 => Send HS in use indication\n");
      printf ( "9 => Send Diagnostic indication\n");
      printf ( "a => Send SMS indication\n");
      printf ( "b => Send Proprietary Event Notification\n");
      printf ( "c => Send Date_Time Recovery Event\n");
      printf ( "d => Send Extended List Changed Notification\n");
            
      printf ( "- - - - - - - - - - - - - - - \n");      
      printf ( "q => Return to Interface Menu\n" );
      
      switch ( tcx_getch() )
      {
         case ' ':
            tcx_appClearScreen();
         break;
            
         case '1':
               keyb_CatIqMWI();
            break;

         case '2':
               keyb_CatIqMissedCalls();
            break;

         case '3':
               keyb_CatIqListChanged();
            break;

         case '4':
               keyb_CatIqWebContent();
            break;

         case '5':
               keyb_CatIqPropEvent();
            break;

         case '6':
               keyb_CatIqTimeUpdate();
            break;
            
         case '7':
               keyb_CatIqLineUseIndication();
            break;

         case '8':
               keyb_CatIqHSUseIndication();
            break;

         case '9':
               keyb_CatIqDiagnosticIndication();
            break;
            
         case 'A':
         case 'a':           
                keyb_CatIqSMSMessageNotification();
            break;
         case 'b':
                keyb_CatIqPropEventNotification();
            break;           
         case 'C':
         case 'c':
                keyb_CatIqTimeRecovery();
            break;
		case 'D':
		case 'd':
			   keyb_CatIqListChangedEx();
		   break;
         case 'q':
            n_Keep = FALSE;
            break;          
      }
   }
}

//*/
