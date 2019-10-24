/*!
*	\file		appcall.c
*	\brief	    handle the call control and linked it to media
*	\Author		kelbch
*
*  appcall Automat
*  Incoming call: 1. start as normal incoming call
*                 2. start automatically media, if target sent CMBS answer message
*
*  Outgoing call: 1. Receive establish
*                 2. Reply with set-up ack
*                 3. Start dial tone
*                 4. After receiving '#' send ringing
*                 5. User interact for active call, with Media on
*
*	@(#)	%filespec: appcall.c~NBGD53#77 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================
* date        name     version	 action                               
*	----------------------------------------------------------------------------
* 05-May-2015 tcmc_asa  PR 14071: add RelReason to CALL_PROGRESS DISCONNECTING
* 01-Dec-2014 tcmc_asa     added u16_ToneVolume parameter in appmedia_CallObjTonePlay
* 07-Mar-2014 tcmc_asa  ---GIT--   added NODE audio connection type
* 12-Jun-2013 tcmc_asa      take change of V77 and 77.1.1 form 2.99.9 to 3.x branch
* 28-Jan-2013 tcmc_asa 77      added appcall_CallInbandInfoCNIP() PR 3615
* 24-Jan-2013 tcmc_asa 76      add possibility to switch codec on call answer automatically
* 24-May-2012 tcmc_asa 74      added CMBS_TONE_CALL_WAITING_OUTBAND, PR 3205
* 22-May-2012 tcmc_asa 73      added CLIP type CMBS_ADDR_PROPPLAN_NAT_STD
* 16-Feb-2012 tcmc_asa 66       added use of CLIP AdressProperties from Host (PR 10027)
* 02-Feb-2012 tcmc-asa 65        add CNIP screening indicator
* 08-Dec-2011 tcmc-asa 61        CLIP&CNIP presentation
* 19-sep-09		Kelbch		pj1029-478	add demonstration line handling 
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if ! defined ( WIN32 )
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h> //we need <sys/select.h>; should be included in <sys/types.h> ???
#include <signal.h>
#else
#include "windows.h"
#endif

#include "cmbs_api.h"
#include "cmbs_util.h"
#include "cmbs_dbg.h"
#include "cfr_ie.h"
#include "cmbs_ie.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "apphan.h"
#include "cmbs_han.h"
#include "appcall.h"
#include "ListsApp.h"
#include "ListChangeNotif.h"
#include "LASessionMgr.h"
#include "apphanvoicecallroutingtable.h"
#include "appCallRouter.h"
#ifdef MEDIA5_SIP_SUPPORT
#include "cmbs_voipline_media5.h"
#else
#include "cmbs_voipline.h"
#endif
#include "appmsgparser.h"
#include "cmbs_event.h"
#include "appsrv.h"
#include "cmbs_han_ie.h"
#include "appfacility.h"
#include "cfr_debug.h"     // CMBS debug definition
#ifdef PNCAP
#include "appcall.h"
#include "etp.h"
#include "pie.h"
#include "utils.h"
#include "pncap_codes.h"
#include "pncap_def.h"
//#include "yuv2rgb.h"
#endif

#ifdef AVS_AUDIO_APP
#include "appaudio.h"
#endif // AVS_AUDIO_APP

#ifdef PNCAP
u8  u8_HsTxRx_pie_buf[CMBS_HS_SUBSCRIBED_MAX_NUM][CMBS_PROP_EVENT_LENGTH];
#endif

ST_IE_CALL_STATE st_HsCallState[CMBS_HS_SUBSCRIBED_MAX_NUM][2];


#include "tcx_hostlog.h"
#define APP_CALL_INFO_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define APP_CALL_TRACE_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define APP_CALL_WARNING_PRINT(format, ...) 					tcx_WriteLog(CALL_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define APP_CALL_ERROR_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define APP_CALL_RT_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define APP_CALL_PRINT_DATA(pu8_Buffer, u16_Length) 			tcx_WriteLogData(CALL_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)


// #define DSPG_DECT_COMA_MEM_TEST

#ifdef DSPG_DECT_COMA_MEM_TEST
#define SHARED_MEMORY_BASE 0x033FE000 //0x3C00000
#define MAX_SHMEM_CHANNELS 2
ST_IE_AUDIO_SHMEM dect_shmem_config[MAX_SHMEM_CHANNELS];

#define SHM_BUFFER_SIZE_VOICE	(4*640)
#define SHM_BUFFER_SIZE 		(SHM_BUFFER_SIZE_VOICE + 4)
#define SHM_BUF_SIZE			SHM_BUFFER_SIZE

#define CSS_NB_SAMPLE_SIZE		8	//FIXME: need new css with NB support
#define CSS_NB_PERIOD_SIZE		160

#define CSS_WB_SAMPLE_SIZE		16 
#define CSS_WB_PERIOD_SIZE		640 //640bytes for 20ms packet size in WB (4*160)

#define NUM_TS_WB 0xF
#define NUM_TS_NB 0x1 

#endif

extern u16 app_HandsetMap(char *psz_Handsets);
extern void keyb_ReleaseNotify(u16 u16_CallId);
extern u16 _keyb_LineIdInput(void);

extern u8 g_HoldResumeCfm;
extern u8 g_HoldCfm;
extern u8 g_TransferAutoCfm;
extern u8 g_ConfAutoCfm;
extern u8 g_EarlyMediaAutoCfm;
extern u8 u8_callId;

static hsSuccessfullyConnected successfullyConnected;
static hsSuccessfullyReleased successfullyReleased;

void initMsgParserCall(hsSuccessfullyConnected connectedClb, hsSuccessfullyReleased releasedClb)
{
  if(!connectedClb || !releasedClb) return;
  successfullyConnected = connectedClb;
  successfullyReleased = releasedClb;
}

#define CALL_INSTANCE_UNUSED 0 // in principle 0 is a legal call instance, but it is only theoretical. it will be used only after 4Giga calls... we will live with this.
#define MAX_RELEASE_CALL_INST 2
ST_CALL_RELEASE g_CallRelease[MAX_RELEASE_CALL_INST];

static ST_CALL_OBJ *appcall_GetCallPendingforDeviceObj(ST_IE_CALLERPARTY st_CallerParty);
static bool app_RemoveDeviceFromListOfParticipants(PST_CALL_OBJ pst_Call, u16 u16_DeviceId);
static bool app_AddDeviceToListOfParticipants(PST_CALL_OBJ pst_Call, u16 u16_DeviceId);
static bool app_AddHSToListOfParticipants(PST_CALL_OBJ pst_Call, u8 u8_HsNum);
static bool app_RemoveAllHSFromListOfParticipants(PST_CALL_OBJ pst_Call);

#ifdef AVS_AUDIO_APP

// config
int avs_call_release = 1;
int avs_call_release_timeout;

static time_t rel_after_sec;

u16 g_u16_Merged = 0xFF;

#endif // AVS_AUDIO_APP

u8 g_u8_ToneType = 0;
typedef struct
{   
    u32 u32_CallInstanse;
    u16 u16_HSMask; 
}st_ActiveCalls;

st_ActiveCalls g_ActiveCalls[CMBS_MAX_NUM_OF_LINES][CMBS_MULTICALL_LINE_NUM_OF_CALLS];

E_APPCALL_AUTOMAT_MODE g_call_automat = E_APPCALL_AUTOMAT_MODE_ON;

/*! \brief global line object */
ST_LINE_OBJ g_line_obj[APPCALL_LINEOBJ_MAX];
/*! \brief global call object */
ST_CALL_OBJ g_call_obj[APPCALL_CALLOBJ_MAX];

#ifdef YOGA_DEMO
#define APPCALL_CALL_PPARTICIPANT_DEVICE_LEN 	5
#else
#define APPCALL_CALL_PPARTICIPANT_DEVICE_LEN 	8
#endif

#define APPCALL_CALL_CALLER_DEVICE_LEN 	5

#define APPCALL_CALL_PARTICIPANT_DEVICE_OFFSET 	1
#define APPCALL_CALL_PARTICIPANT_UNIT_OFFSET	6
#define APPCALL_DEVICE_IDENTIFIER			'D'
#define APPCALL_UNIT_IDENTIFIER				'U'

#define APPCALL_CALL_PPARTICIPANT_HS_LEN 	2
#define APPCALL_HS_IDENTIFIER				'H'

/* Slot allocation */

#ifdef FXS_SLOT_TEST 
//#define DSPG_FXS_SLOTS 0x00FF
#ifdef FXS_DUSLIC
#define DSPG_FXS_SLOTS 0xC3C3  //Lantiq
#else
#define DSPG_FXS_SLOTS 0x0000FFFF  //Silab for NB only
#endif
/*Note Sure in between slots can be used after first 2 slots */
#define SILAB_FXS_SLOTS 0x000F000F
/* Slot allocation */
u32 g_u32_UsedSlots = DSPG_FXS_SLOTS;//Reserve first 64 bits for FXS
#else
u32 g_u32_UsedSlots = 0;
#endif
u16 g_u16_Merge_CallId1 = 0xFF, g_u16_Merge_CallId2 = 0xFF;

E_APPCALL_PREFERRED_CODEC g_HostCodec = E_APPCALL_PREFERRED_CODEC_RECEIVED_CODEC;

ST_CMBS_APP_CONF_CALL_ENTRY ConferenceCallTable[CMBS_MAX_NUMBER_OF_CONFERENCE_CALLS];

extern u16 g_u16_DemoCallId;


#ifdef DSPG_DECT_COMA_MEM_TEST
int setup_voshm_channel(void *dect_ie_list, ST_IE_AUDIO_SHMEM *pst_AudioShmem, unsigned int slotmask, unsigned long channid)
{
	E_CMBS_RC rc;
	unsigned long rx_buf, tx_buf;

	memset(pst_AudioShmem, 0, sizeof(ST_IE_AUDIO_SHMEM));

	if(slotmask == NUM_TS_NB)
	{
		pst_AudioShmem->u32_SampleSize = CSS_NB_SAMPLE_SIZE; /* size of a sample in bits */
		pst_AudioShmem->u32_PeriodSize = CSS_NB_PERIOD_SIZE; /* number of bytes in a period  before getting interrupt from css */
	}
	else
	{
		pst_AudioShmem->u32_SampleSize = CSS_WB_SAMPLE_SIZE; 
		pst_AudioShmem->u32_PeriodSize = CSS_WB_PERIOD_SIZE;
	}

	APP_CALL_INFO_PRINT("setup_voshm_channel: u32_SampleSize %d, u32_PeriodSize %d buffer size %d channid %lu\n", pst_AudioShmem->u32_SampleSize, pst_AudioShmem->u32_PeriodSize, SHM_BUF_SIZE, channid);

	rx_buf = SHARED_MEMORY_BASE;
	APP_CALL_INFO_PRINT("setup_voshm_channel: shm rx buf 0x%p\n", (void*)rx_buf);

	pst_AudioShmem->u32_RxBufAddr = rx_buf;    /* physical address of rx sample buffer */ 
	pst_AudioShmem->u32_RxBufSize = SHM_BUF_SIZE;     /*
       * size of rx buffer in bytes (should be a multiple of
       * channels * sample_size (sample_size in bytes))
       * including the 4 bytes for the progress index
       */ 

	tx_buf = rx_buf + SHM_BUF_SIZE;
	APP_CALL_INFO_PRINT("setup_voshm_channel: shm tx buf 0x%p\n", (void*)tx_buf);

	pst_AudioShmem->u32_TxBufAddr = tx_buf;    /* physical address of tx sample buffer */ 
	pst_AudioShmem->u32_TxBufSize = SHM_BUF_SIZE;/*
	* size of tx buffer in bytes (should be a multiple of
	* channels * sample_size (sample_size in bytes))
	* including the 4 bytes for the progress index
	 */

	rc = cmbs_api_ie_AudioShmemAdd( dect_ie_list, pst_AudioShmem);
	APP_CALL_INFO_PRINT("setup_voshm_channel: STI-DECT: cmbs_api_ie_AudioShmemAdd() return %d\n", rc);
}
#endif

//		========== __appcall_FindFreeReleasePendingRequest ===========
/*!
        \brief      Manage pending release request - finds empty entry to handle the release
        \return		first free index or MAX_RELEASE_CALL_INST
*/

static u8 __appcall_FindFreeReleasePendingRequest(void)
{
    u8 i;

    for (i = 0; i < MAX_RELEASE_CALL_INST; ++i)
    {
        if ( g_CallRelease[i].u32_CallInstance == CALL_INSTANCE_UNUSED )
        {
            break;

        }
    }

    return i;
}

//		========== __appcall_FindFirstReleasePendingRequest ===========
/*!
        \brief			        Manage pending release request
        \return				 Istance of g_CallRelease
*/
static u8 __appcall_FindFirstReleasePendingRequest(void)
{
    u8 i;

    for (i = 0; i < MAX_RELEASE_CALL_INST; ++i)
    {
        if ( g_CallRelease[i].u32_CallInstance != CALL_INSTANCE_UNUSED )
        {
            break;
        }
    }

    return i;
}

//		========== _appcall_AddCallEntry ===========
/*!
        \brief			        Add a call entry to call list (new missed call, new outgoing call,  new incoming call)
        \param[in]	            tListType - List to insert (missed, incoming, outgoing)
        \param[in]	            sNumber -   Number of remote party (CLI)
        \param[in]	            u8_LineID - Line ID of call
        \return				    void
*/
static void _appcall_AddCallEntry(IN LIST_TYPE tListType, IN const char *sNumber, IN u8 u8_LineID)
{
    stCallsListEntry st_Entry;
    u32 pu32_Fields[5], u32_NumOfFields, u32_EntryId;

    if ( sNumber[0] == 0 )
    {
        /* we cannot add an entry without number */
        APP_CALL_ERROR_PRINT("Add Call Entry - FAIL - no number!\n");
        return;
    }

    pu32_Fields[0] = FIELD_ID_NUM_OF_CALLS;
    pu32_Fields[1] = FIELD_ID_DATE_AND_TIME;
    pu32_Fields[2] = FIELD_ID_NUMBER;
    pu32_Fields[3] = FIELD_ID_LINE_ID;

    if ( tListType == LIST_TYPE_MISSED_CALLS )
    {
        u32_NumOfFields = 5;
        pu32_Fields[4] = FIELD_ID_READ_STATUS;
        st_Entry.bRead = FALSE;
    }
    else
    {
        u32_NumOfFields = 4;
    }

	// Remove p r n c t s from first byte.
	if ((sNumber[0] == 'p') || (sNumber[0] == 'r') || (sNumber[0] == 'n') ||
		(sNumber[0] == 'c') || (sNumber[0] == 't') || (sNumber[0] == 's'))
	{
		sNumber++;
	}

    st_Entry.u32_NumOfCalls = 1;
    time(&st_Entry.t_DateAndTime);
    strncpy(st_Entry.sNumber, sNumber, LIST_NUMBER_MAX_LEN); st_Entry.sNumber[LIST_NUMBER_MAX_LEN - 1] = 0;
    st_Entry.u32_LineId = u8_LineID;

    List_InsertEntry(tListType, &st_Entry, pu32_Fields, u32_NumOfFields, &u32_EntryId);

	{
		ST_APP_FACILITY_LCN_SUB_DETAILS st_LcnSubDetails;
		u32 u32_PosIndex = 0;
		
		st_LcnSubDetails.u16_Change = APP_FACILITY_CHANGE_ADD;
		st_LcnSubDetails.u16_EntryId = u32_EntryId;
		/* Entry Id is the first member of every entry */
		List_GetEntryIdex(tListType, FIELD_ID_DATE_AND_TIME, FIELD_ID_INVALID, u32_EntryId, &u32_PosIndex);
		st_LcnSubDetails.u16_PosIndex = u32_PosIndex;
		
		List_SetListChangeOriginatingHandset(tListType, 0);
		List_AddListChangeDetails(tListType, &st_LcnSubDetails);
	}

    cmbsevent_onCallListUpdated();
}

//		========== _appcall_LineObjGet ===========
/*!
        \brief			      return a line object identfied by Line ID
        \param[in]	           u16_LineId
        \return				<PST_LINE_OBJ>    line object or NULL
*/
PST_LINE_OBJ _appcall_LineObjGet(u16 u16_LineId)
{
    if ( u16_LineId < APPCALL_LINEOBJ_MAX )
    {
        return g_line_obj + u16_LineId;
    }
    return NULL;
}

//		========== _appcall_CallObjGet ===========
/*!
        \brief			      return a call object identfied by Call ID
        \param[in]	           u16_CallId
        \return				<ST_CALL_OBJ>    call object or NULL
*/
PST_CALL_OBJ _appcall_CallObjGetById(u16 u16_CallId)
{
    if ( u16_CallId < APPCALL_CALLOBJ_MAX )
    {
        return g_call_obj + u16_CallId;
    }
    return NULL;
}

//		========== _appcall_LineObjIdGet ===========
/*!
        \brief				return a Line ID of line object
        \param[in]		     pst_Line		pointer to line object
        \return				<u16>           Line ID
*/
u16 _appcall_LineObjIdGet(PST_LINE_OBJ pst_Line)
{
    return(u16)(pst_Line - g_line_obj);
}






//		========== _appcall_CallObjStateString ===========
/*!
        \brief				return the string of enumeration
        \param[in,out]		e_Call		 enumeration value
        \return				<char *>     string value
*/
char* _appcall_CallObjStateString(E_APPCMBS_CALL e_Call)
{
    switch (e_Call)
    {
        caseretstr(E_APPCMBS_CALL_CLOSE);

        caseretstr(E_APPCMBS_CALL_INC_PEND);
        caseretstr(E_APPCMBS_CALL_INC_RING);

        caseretstr(E_APPCMBS_CALL_OUT_PEND);
        caseretstr(E_APPCMBS_CALL_OUT_PEND_DIAL);
        caseretstr(E_APPCMBS_CALL_OUT_INBAND);
        caseretstr(E_APPCMBS_CALL_OUT_PROC);
        caseretstr(E_APPCMBS_CALL_OUT_RING);

        caseretstr(E_APPCMBS_CALL_ACTIVE);
        caseretstr(E_APPCMBS_CALL_RELEASE);

        caseretstr(E_APPCMBS_CALL_ON_HOLD);
        caseretstr(E_APPCMBS_CALL_CONFERENCE);
        caseretstr(E_APPCMBS_CALL_SCREENING);

    default:
        return(char *)"Call State undefined";
    }

    return NULL;
}

//		========== _appcall_CallObjMediaCodecString ===========
/*!
        \brief				return the string of enumeration
        \param[in,out]		E_CMBS_AUDIO_CODEC
                                     enumeration value
        \return				<char *>     string value
*/
char* _appcall_CallObjMediaCodecString(E_CMBS_AUDIO_CODEC e_Codec)
{
    switch (e_Codec)
    {
        caseretstr(CMBS_AUDIO_CODEC_UNDEF);
        caseretstr(CMBS_AUDIO_CODEC_PCMU);
        caseretstr(CMBS_AUDIO_CODEC_PCMA);
        caseretstr(CMBS_AUDIO_CODEC_PCMU_WB);
        caseretstr(CMBS_AUDIO_CODEC_PCMA_WB);
        caseretstr(CMBS_AUDIO_CODEC_PCM_LINEAR_WB);
        caseretstr(CMBS_AUDIO_CODEC_PCM_LINEAR_NB);
        caseretstr(CMBS_AUDIO_CODEC_PCM8);

    default:
        return(char *)"Codec undefined";
    }

    return NULL;
}

//		========== _appcall_CallObjMediaString ===========
/*!
        \brief				return the string of enumeration
        \param[in,out]		E_APPCMBS_MEDIA
                                     enumeration value
        \return				<char *>     string value
*/
char* _appcall_CallObjMediaString(E_APPCMBS_MEDIA e_Media)
{
    switch (e_Media)
    {
        caseretstr(E_APPCMBS_MEDIA_CLOSE);
        caseretstr(E_APPCMBS_MEDIA_PEND);
        caseretstr(E_APPCMBS_MEDIA_ACTIVE);

    default:
        return(char *)"Media State undefined";
    }

    return NULL;
}

//		========== 	appcall_InfoCall ===========
/*!
        \brief	 		 print the current call information 

        \param[in]		 n_Call		Identifier of call

        \return	 		 <none>
*/
void appcall_InfoCall(u16 u16_CallId)
{
	u8 u8_Index = 0, u8_ActiveRecords = 0;
	
    if ( g_call_obj[u16_CallId].u32_CallInstance )
    {
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO CALL_ID %u\n", u16_CallId);
		printf("APP_Dongle-CALL: INFO CALL_ID %u\n", u16_CallId);

        if ( g_call_obj[u16_CallId].ch_CallerID[0] )
        {
            APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO CALLER ID %s\n", g_call_obj[u16_CallId].ch_CallerID);
			printf("APP_Dongle-CALL: INFO CALLER ID %s\n", g_call_obj[u16_CallId].ch_CallerID);
        }
        if ( g_call_obj[u16_CallId].ch_CalledID[0] )
        {
            APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO CALLED ID %s\n", g_call_obj[u16_CallId].ch_CalledID);
			printf("APP_Dongle-CALL: INFO CALLED ID %s\n", g_call_obj[u16_CallId].ch_CalledID);
        }
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO Call State  %s\n", _appcall_CallObjStateString(g_call_obj[u16_CallId].e_Call));
		printf("APP_Dongle-CALL: INFO Call State  %s\n", _appcall_CallObjStateString(g_call_obj[u16_CallId].e_Call));
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO Media State %s\n", _appcall_CallObjMediaString(g_call_obj[u16_CallId].e_Media));
		printf("APP_Dongle-CALL: INFO Media State %s\n", _appcall_CallObjMediaString(g_call_obj[u16_CallId].e_Media));
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO Codec %s\n", _appcall_CallObjMediaCodecString(g_call_obj[u16_CallId].e_Codec));
		printf("APP_Dongle-CALL: INFO Codec %s\n", _appcall_CallObjMediaCodecString(g_call_obj[u16_CallId].e_Codec));
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO CallInstance %u = 0x%08X\n", g_call_obj[u16_CallId].u32_CallInstance, g_call_obj[u16_CallId].u32_CallInstance);
        printf("APP_Dongle-CALL: INFO CallInstance %u = 0x%08X\n", g_call_obj[u16_CallId].u32_CallInstance, g_call_obj[u16_CallId].u32_CallInstance);
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO LineID %u\n", g_call_obj[u16_CallId].u8_LineId);
		printf("APP_Dongle-CALL: INFO LineID %u\n", g_call_obj[u16_CallId].u8_LineId);
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO Media Channel ID %u\n", g_call_obj[u16_CallId].u32_ChannelID);
		printf("APP_Dongle-CALL: INFO Media Channel ID %u\n", g_call_obj[u16_CallId].u32_ChannelID);
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO Media Slots 0x%X\n", g_call_obj[u16_CallId].u32_ChannelParameter);
		printf("APP_Dongle-CALL: INFO Media Slots 0x%X\n", g_call_obj[u16_CallId].u32_ChannelParameter);

		while( u8_ActiveRecords < g_call_obj[u16_CallId].u8_NumOfParticipants && u8_Index < MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE)
		{
			if (strlen((char *)g_call_obj[u16_CallId].pu8_ListOfParticipants[u8_Index]))
			{
				APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO Participant %d %s\n", u8_ActiveRecords + 1, g_call_obj[u16_CallId].pu8_ListOfParticipants[u8_Index]);
				printf("APP_Dongle-CALL: INFO Participant %d %s\n", u8_ActiveRecords + 1, g_call_obj[u16_CallId].pu8_ListOfParticipants[u8_Index]);

				u8_ActiveRecords++;	
			}

			u8_Index++;
		}
    
    }
    else
    {
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO CALL_ID %d is free\n", u16_CallId);
		printf("APP_Dongle-CALL: INFO CALL_ID %d is free\n", u16_CallId);
    }
}

//		========== appcall_InfoPrint ===========
/*
        \brief				print the current call objects information
        \param[in,out]		<none>
        \return				<none>

*/
void appcall_InfoPrint(void)
{
    u16 i;

    if ( g_call_automat )
    {
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO - - - AUTOMAT ON - - -\n");
    }
    else
    {
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO - - - AUTOMAT OFF - - -\n");
    }
    APP_CALL_INFO_PRINT("       Auto Transfer = %d, Auto Conf = %d,\n       Auto Hold = %d, Auto Resume = %d, Auto EarlyMedia = %d\n",
           g_TransferAutoCfm, g_ConfAutoCfm, g_HoldCfm, g_HoldResumeCfm, g_EarlyMediaAutoCfm);
	printf("       Auto Transfer = %d, Auto Conf = %d,\n       Auto Hold = %d, Auto Resume = %d, Auto EarlyMedia = %d\n",
           g_TransferAutoCfm, g_ConfAutoCfm, g_HoldCfm, g_HoldResumeCfm, g_EarlyMediaAutoCfm);
    APP_CALL_INFO_PRINT("       Host Codec = %s\n", (g_HostCodec == E_APPCALL_PREFERRED_CODEC_RECEIVED_CODEC) ? "use received codec" : (g_HostCodec == E_APPCALL_PREFERRED_CODEC_WB) ? "WB" : "NB");
   	printf("       Host Codec = %s\n", (g_HostCodec == E_APPCALL_PREFERRED_CODEC_RECEIVED_CODEC) ? "use received codec" : (g_HostCodec == E_APPCALL_PREFERRED_CODEC_WB) ? "WB" : "NB");
   	APP_CALL_INFO_PRINT("-================================================-\n\n");
  	printf("-================================================-\n\n");

    for (i = 0; i < APPCALL_CALLOBJ_MAX; i++)
    {
        APP_CALL_INFO_PRINT("APP_Dongle-CALL: INFO ----------------------\n");
		printf("APP_Dongle-CALL: INFO ----------------------\n");
        appcall_InfoCall(i);
    }
}

//		========== appcall_Initialize  ===========
/*!
        \brief				Initialize the call/line management
        \param[in,out]		<none>
        \return				<none>
*/
void appcall_Initialize(void)
{
    u16 i, j;

    memset(&g_line_obj, 0, sizeof(g_line_obj));
    //   for (i=0; i< APPCALL_LINEOBJ_MAX; i++ )
    //   {
    //      g_line_obj[i]. = ;   //TODO  additional initialization for line
    //   }

    memset(&g_call_obj, 0, sizeof(g_call_obj));
    for (i = 0; i < APPCALL_CALLOBJ_MAX; i++)
    {
        g_call_obj[i].st_CallerParty.pu8_Address = (u8 *)g_call_obj[i].ch_CallerID;
        g_call_obj[i].st_CalledParty.pu8_Address = (u8 *)g_call_obj[i].ch_CalledID;
        g_call_obj[i].u8_LineId = APPCALL_NO_LINE;
        g_call_obj[i].u32_ChannelID = APPCALL_NO_MEDIA_CHANNEL;
    }

    for (i = 0; i < CMBS_MULTICALL_LINE_NUM_OF_CALLS; i++)
    {
        for (j = 0; j < CMBS_MAX_NUM_OF_LINES; j++)
          {
            g_ActiveCalls[i][j].u32_CallInstanse = 0xFFFFFFFF;
            g_ActiveCalls[i][j].u16_HSMask = 0;
          }
    }
	
	g_u32_UsedSlots = 0;
	g_u16_DemoCallId = APPCALL_NO_CALL;
	
    appcall_ConferenceCallTableInit();
	appcall_HsStatusInit();
}


//		========== _appcall_CallObjIdGet ===========
/*!
        \brief				return a Call ID of call object
        \param[in]		     pst_Call		pointer to call object
        \return				<u16>           Call ID
*/
u16 _appcall_CallObjIdGet(PST_CALL_OBJ pst_Call)
{
    return(u16)(pst_Call - g_call_obj);
}

//		========== _appcall_CallObjGet ===========
/*!
        \brief			      return a call object identfied by call instance or caller party
        \param[in]	           u32_CallInstance
        \param[in]		     psz_CLI
        \return				<PST_CALL_OBJ>    if no free call object available return NULL
*/
PST_CALL_OBJ _appcall_CallObjGet(u32 u32_CallInstance, char *psz_CLI)
{
    u16 i;

    for (i = 0; i < APPCALL_CALLOBJ_MAX; i++)
    {
        if ( u32_CallInstance && (g_call_obj[i].u32_CallInstance == u32_CallInstance) )
            return g_call_obj + i;

        if ( psz_CLI && !strcmp(g_call_obj[i].ch_CallerID, psz_CLI) )
            return g_call_obj + i;
    }

    return NULL;
}

//		========== _appcall_CallObjNew ===========
/*!
        \brief				get a free call object
        \param[in,out]		<none>
        \return				<PST_CALL_OBJ>    if no free call object available return NULL
*/
PST_CALL_OBJ _appcall_CallObjNew(void)
{
    u16 i;

    for (i = 0; i < APPCALL_CALLOBJ_MAX; i++)
    {
        if ( !g_call_obj[i].u32_CallInstance )
            return g_call_obj + i;
    }

    return NULL;
}

//		========== _appcall_CallObjDelete ===========
/*!
        \brief				delete a call object identfied by call instance or caller party
        \param[in,out]		u32_CallInstance
        \param[in,out]		psz_CLI
        \return				<none>
*/
void _appcall_CallObjDelete(u32 u32_CallInstance, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;

    pst_Call = _appcall_CallObjGet(u32_CallInstance, psz_CLI);

    if ( pst_Call )
    {
        memset(pst_Call, 0, sizeof(ST_CALL_OBJ));
        pst_Call->st_CalledParty.pu8_Address = (u8 *)pst_Call->ch_CalledID;
        pst_Call->st_CallerParty.pu8_Address = (u8 *)pst_Call->ch_CallerID;
        pst_Call->u8_LineId = APPCALL_NO_LINE;
        pst_Call->u32_ChannelID = APPCALL_NO_MEDIA_CHANNEL;
    }
}

//		========== _appcall_CallObjLineObjGet ===========
/*!
        \brief			return the line object of call object

        \param[in]		pst_Call		pointer to call object

        \return			<PST_LINE_OBJ>          pointer to line object  or NULL

*/
PST_LINE_OBJ _appcall_CallObjLineObjGet(PST_CALL_OBJ pst_Call)
{
    if ( pst_Call->u8_LineId != APPCALL_NO_LINE )
    {
        return g_line_obj + pst_Call->u8_LineId;
    }
    return NULL;
}

//		========== _appcall_CallObjLineObjSet ===========
/*!
        \brief			set line object for call object

        \param[in]		pst_Call		pointer to call object
        \param[in]		pst_Line		pointer to line object

        \return			<none>

*/
void _appcall_CallObjLineObjSet(PST_CALL_OBJ pst_Call, PST_LINE_OBJ pst_Line)
{
    if ( pst_Line )
    {
        pst_Call->u8_LineId = (u8)(pst_Line - g_line_obj);
    }
}

//		========== _appcall_CallObjStateSet  ===========
/*!
        \brief			set call object call state
        \param[in]		pst_Call		pointer to call object
        \param[in]		e_State		call state
        \return			<none>
*/
void _appcall_CallObjStateSet(PST_CALL_OBJ pst_Call, E_APPCMBS_CALL e_State)
{
	if( pst_Call )
	{
	    pst_Call->e_Call = e_State;
	}
}

//		========== _appcall_CallObjStateGet  ===========
/*!
        \brief			set call object call state
        \param[in]		pst_Call		 pointer to call object
        \return			<E_APPCMBS_CALL> call state of this call object
*/
E_APPCMBS_CALL _appcall_CallObjStateGet(PST_CALL_OBJ pst_Call)
{
    return pst_Call->e_Call;
}

//		========== _appcall_CallObjMediaSet  ===========
/*!
        \brief			set call object media state
        \param[in]		pst_Call		pointer to call object
        \param[in]		e_State		media state
        \return			<none>
*/
void _appcall_CallObjMediaSet(PST_CALL_OBJ pst_Call, E_APPCMBS_MEDIA e_State)
{
    pst_Call->e_Media = e_State;
}

//		========== _appcall_CallObjMediaGet  ===========
/*!
        \brief			return call object media state
        \param[in]		pst_Call		 pointer to call object
        \return			<E_APPCMBS_MEDIA>  media state of this call object
*/
E_APPCMBS_MEDIA _appcall_CallObjMediaGet(PST_CALL_OBJ pst_Call)
{
    return pst_Call->e_Media;
}



int _appcall_CallObjDigitCollectorEndSymbolCheck(PST_CALL_OBJ pst_Call)
{
    int i;

    for (i = 0; i < pst_Call->st_CalledParty.u8_AddressLen; i++)
    {
        if ( pst_Call->st_CalledParty.pu8_Address[i] == '#' )
        {
            return TRUE;
        }
    }

    return FALSE;

}


//		========== _appcall_PropertiesIDXGet ===========
/*!
        \brief			 find index of IE in exchange object
        \param[in]		 pst_Properties	 pointer to exchange object
        \param[in]		 n_Properties		 number of contained IEs
        \param[in]		 e_IE		          to be find IE

        \return	 		 <int>              index of the IE

*/

int _appcall_PropertiesIDXGet(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, E_CMBS_IE_TYPE e_IE)
{
    int i;

    for (i = 0; i < n_Properties; i++)
    {
        if ( pst_Properties[i].e_IE == e_IE )
            break;
    }

    return i;
}
//		========== _appmedia_CallObjMediaPropertySet  ===========
/*!
        \brief			short description
        \param[in]		pst_Call		 pointer to call object
        \param[in]		u16_IE		 current IE
        \param[in,out]	pst_IEInfo	 pointer to IE info object
        \return			<int>        return TRUE, if IEs were consumed
*/

int _appmedia_CallObjMediaPropertySet(PST_CALL_OBJ pst_Call, u16 u16_IE, PST_APPCMBS_IEINFO pst_IEInfo)
{
    if ( !pst_Call )
        return FALSE;

    switch (u16_IE)
    {
    case CMBS_IE_MEDIADESCRIPTOR:
        // APP_CALL_INFO_PRINT(" !!!! MEDIADESCRIPTOR: ");
        if ( (pst_Call->b_Incoming) && (g_HostCodec == E_APPCALL_SWITCH_RECEIVED_CODEC) )
        {
            if ( pst_IEInfo->Info.st_MediaDesc.e_Codec == CMBS_AUDIO_CODEC_PCM_LINEAR_WB )
            {
                pst_Call->e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_NB;
                //    APP_CALL_INFO_PRINT(" NB \n");
            }
            else
            {
                pst_Call->e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_WB;
                //      APP_CALL_INFO_PRINT(" WB \n");
            }
            pst_Call->pu8_CodecsList[0] = pst_Call->e_Codec;
            pst_Call->u8_CodecsLength = 1;
        }
        else if ( (pst_Call->b_Incoming) || (g_HostCodec == E_APPCALL_PREFERRED_CODEC_RECEIVED_CODEC) )
        {
            pst_Call->e_Codec = pst_IEInfo->Info.st_MediaDesc.e_Codec;
            memcpy(&pst_Call->pu8_CodecsList, &pst_IEInfo->Info.st_MediaDesc.pu8_CodecsList, pst_IEInfo->Info.st_MediaDesc.u8_CodecsLength);
            pst_Call->u8_CodecsLength = pst_IEInfo->Info.st_MediaDesc.u8_CodecsLength;
        }
        else
        {
            if ( g_HostCodec == E_APPCALL_PREFERRED_CODEC_NB )
            {
                pst_Call->e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_NB;
            }
            else
            {
                pst_Call->e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_WB;
            }
            pst_Call->pu8_CodecsList[0] = pst_Call->e_Codec;
            pst_Call->u8_CodecsLength = 1;
        }
        return TRUE;

    case CMBS_IE_MEDIACHANNEL:
        pst_Call->u32_ChannelID = pst_IEInfo->Info.st_MediaChannel.u32_ChannelID;
        _appcall_CallObjMediaSet(pst_Call, E_APPCMBS_MEDIA_PEND);
        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_CallObjLineSubTypeGet ===========
/*!
        \brief			get line sub type  for call object

        \param[in]		pst_Call			       pointer to call object

        \return			<pst_Call->u8_LineSubType>

*/
u8 _appcall_CallObjLineSubTypeGet(PST_CALL_OBJ pst_Call)
{
	return pst_Call->u8_LineSubType;
}

//		========== _appcall_CallObjLineSubTypeSet ===========
/*!
        \brief			set line sub type for call object

        \param[in]		pst_Call			pointer to call object
        \param[in]		u8_LineType		line sub type

        \return			<none>

*/
void _appcall_CallObjLineSubTypeSet(PST_CALL_OBJ pst_Call, u8 u8_LineSubType)
{
	if ( pst_Call )
	{
		pst_Call->u8_LineSubType = u8_LineSubType;
	}
}



//		========== _appcall_CheckBackwardCompatibilityLineSelection ===========
/*!
        \brief			        Checks if the HS is using CAT-iq 2.0 backward compatibility line selection (using keypad '#')
        \param[in]	            pst_CallObjObj - pointer to Call object structure
        \return				    TRUE if line selection  identified, FALSE o/w
*/
static bool _appcall_CheckBackwardCompatibilityLineSelection(IN PST_CALL_OBJ pst_CallObj)
{
    if ( pst_CallObj->e_Call < E_APPCMBS_CALL_ACTIVE )
    {
        if ( (pst_CallObj->st_CalledParty.pu8_Address[0] == '#') && (pst_CallObj->st_CalledParty.u8_AddressLen >= 2) )
        {
            if ( pst_CallObj->u8_LineId < APPCALL_LINEOBJ_MAX || pst_CallObj->u8_LineId == APPCALL_NO_LINE )
            {
                pst_CallObj->u8_LineId = pst_CallObj->st_CalledParty.pu8_Address[1] - '0';

                _appcall_CallObjLineObjSet(pst_CallObj, _appcall_LineObjGet(pst_CallObj->u8_LineId));

                return TRUE;
            }
            else
            {
                APP_CALL_WARNING_PRINT("\n _appcall_CheckBackwardCompatibilityLineSelection - unknown line id - treating as digits \n");
            }
        }
    }

    return FALSE;
}

//		========== appmedia_CallObjMediaStop ===========
/*!
        \brief				stop the media channel identified by Call Instance, Call ID or Caller ID
        \param[in,out]		u32_CallInstance	 if not used zero
        \param[in,out]		u16_CallId		      Call ID used, if psz_CLI is NULL
        \param[in,out]		psz_CLI		      pointer to Caller ID, if not needed NULL
        \return				<none>
*/
void appmedia_CallObjMediaStop(u32 u32_CallInstance, u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;
    void *pv_RefIEList = NULL;

    if ( u32_CallInstance )
    {
        pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
    }
    else
    {
        if ( !psz_CLI )
        {
            // Call ID is available
            pst_Call = g_call_obj + u16_CallId;
        }
        else
        {
            pst_Call = _appcall_CallObjGet(0, psz_CLI);
        }
    }

    if ( pst_Call && (E_APPCMBS_MEDIA_ACTIVE == _appcall_CallObjMediaGet(pst_Call)) )
    {
        pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            ST_IE_MEDIA_CHANNEL st_MediaChannel;

            memset(&st_MediaChannel, 0, sizeof(ST_IE_MEDIA_CHANNEL));
            st_MediaChannel.e_Type = CMBS_MEDIA_TYPE_AUDIO_IOM;
            st_MediaChannel.u32_ChannelID = pst_Call->u32_ChannelID;
			
            cmbs_api_ie_MediaChannelAdd(pv_RefIEList, &st_MediaChannel);
            cmbs_dem_ChannelStop(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
            _appcall_CallObjMediaSet(pst_Call, E_APPCMBS_MEDIA_PEND);
        }

        g_u32_UsedSlots &= ~(pst_Call->u32_ChannelParameter);
        pst_Call->u32_ChannelParameter = 0;
    }
}

//		========== appmedia_CallObjMediaStart ===========
/*!
        \brief				start the media channel identified by Call Instance, Call ID or Caller ID
        \param[in,out]		u32_CallInstance    if not used zero
        \param[in,out]		u16_CallId		    Call ID used, if psz_CLI is NULL
        \param[in,out]		u16_StartSlot       Start Slot, of 0xFF for auto
        \param[in,out]		psz_CLI		        pointer to Caller ID, if not needed NULL
        \return				<none>
*/
void appmedia_CallObjMediaStart(u32 u32_CallInstance, u16 u16_CallId, u16 u16_StartSlot, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;
    void *pv_RefIEList = NULL;
#ifdef DSPG_DECT_COMA_MEM_TEST
    ST_IE_AUDIO_SHMEM *pst_AudioShmem = NULL;
#endif

    if ( u32_CallInstance )
    {
        pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
    }
    else
    {
        if ( !psz_CLI )
        {
            // Call ID is available
            pst_Call = g_call_obj + u16_CallId;
        }
        else
        {
            pst_Call = _appcall_CallObjGet(0, psz_CLI);
        }
    }

    if ( pst_Call && (E_APPCMBS_MEDIA_PEND == _appcall_CallObjMediaGet(pst_Call)) )
    {
        pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            ST_IE_MEDIA_CHANNEL st_MediaChannel;
            u32 u32_Slots = 0;

            memset(&st_MediaChannel, 0, sizeof(ST_IE_MEDIA_CHANNEL));
            st_MediaChannel.e_Type = CMBS_MEDIA_TYPE_AUDIO_IOM;

            // ASA  internal call testing via node !!! */
            if ( g_u8_ToneType )
            {
                st_MediaChannel.e_Type = CMBS_MEDIA_TYPE_AUDIO_NODE;
            }

            st_MediaChannel.u32_ChannelID = pst_Call->u32_ChannelID;
            switch (pst_Call->e_Codec)
            {
            case CMBS_AUDIO_CODEC_PCMU:
            case CMBS_AUDIO_CODEC_PCMA:
            case CMBS_AUDIO_CODEC_PCM8:
                u32_Slots = 0x01;
                break;

            case CMBS_AUDIO_CODEC_PCMU_WB:
            case CMBS_AUDIO_CODEC_PCMA_WB:
            case CMBS_AUDIO_CODEC_PCM_LINEAR_NB:
                u32_Slots = 0x03;
                break;

            case CMBS_AUDIO_CODEC_PCM_LINEAR_WB:	
                u32_Slots = 0x0F;
                break;

            default:
                APP_CALL_ERROR_PRINT("appmedia_CallObjMediaStart - Unsupported codec!\n");
                return;
            }

            if ( 0xFF == u16_StartSlot )
            {
                /* auto slot delection (out of 12 available slots) */
                int i, SlotMaxOffset;
                switch (u32_Slots)
                {
                case 0x01:
                    SlotMaxOffset = 31;
                    break;

                case 0x03:
                    SlotMaxOffset = 30;
                    break;

                default:
                    SlotMaxOffset = 28;
                    break;
                }
                for (i = 0; i <= SlotMaxOffset; ++i)
                {
                    if ( ((u32_Slots << i) & g_u32_UsedSlots) == 0 )
                    {
                        break;
                    }
                }

                if ( i <= SlotMaxOffset )
                {
                    /* available slots exist */
                    st_MediaChannel.u32_ChannelParameter = u32_Slots << i;
                    g_u32_UsedSlots |= u32_Slots << i;
                }
                else
                {
                    /* no available slots */
                    APP_CALL_WARNING_PRINT("\n\nappmedia_CallObjMediaStart - No free slots - aborting media start\n\n");
                    return;
                }
            }
            else
            {
                /* manual slot selection */
                st_MediaChannel.u32_ChannelParameter = u32_Slots << (u16_StartSlot);
            }

            pst_Call->u32_ChannelParameter = st_MediaChannel.u32_ChannelParameter;


#ifdef DSPG_DECT_COMA_MEM_TEST	

            st_MediaChannel.e_Type = CMBS_MEDIA_TYPE_AUDIO_SHMEM;

            if(st_MediaChannel.e_Type == CMBS_MEDIA_TYPE_AUDIO_SHMEM)
            {
            	pst_AudioShmem = &dect_shmem_config[0];//[c->channid];
            	setup_voshm_channel(pv_RefIEList, pst_AudioShmem, NUM_TS_WB,0);
            }
#endif

            cmbs_api_ie_MediaChannelAdd(pv_RefIEList, &st_MediaChannel);
            APP_CALL_INFO_PRINT("\nCall cmbs_dem_ChannelStart, Channel=%d, start Slot=%d, ChannelInfo=%X, type=%d\n",
                   pst_Call->u32_ChannelID,
                   u16_StartSlot,
                   st_MediaChannel.u32_ChannelParameter,
                   st_MediaChannel.e_Type
                  );
            cmbs_dem_ChannelStart(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
            _appcall_CallObjMediaSet(pst_Call, E_APPCMBS_MEDIA_ACTIVE);

#ifdef AVS_AUDIO_APP

            if (pst_Call->st_CalledParty.u8_AddressLen > 0)
            {
                if (pst_Call->st_CalledParty.pu8_Address[0] == '*')
                {
                    rel_after_sec = 0;
                    p_AudioApp_AudioStart((u8)pst_Call->u32_ChannelID, (u8 *)pst_Call->ch_CallerID);
                }
            }

#endif // AVS_AUDIO_APP
        }

    }
}


void appmedia_CallObjMediaInternalConnect(int channel, int context, int connect)
{
    ST_IE_MEDIA_INTERNAL_CONNECT st_MediaIC;
    void *pv_RefIEList = NULL;

    pv_RefIEList = cmbs_api_ie_GetList();

    if ( pv_RefIEList )
    {
        st_MediaIC.e_Type = connect;
        st_MediaIC.u32_ChannelID = channel;
        st_MediaIC.u32_NodeId = context;

        // Add call Instance IE
        cmbs_api_ie_MediaICAdd(pv_RefIEList, &st_MediaIC);

        cmbs_dem_ChannelInternalConnect(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
    }
}

void appmedia_CallObjMediaOffer(u16 u16_CallId, char ch_Audio)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_IE_MEDIA_DESCRIPTOR st_MediaDesc;
    ST_IE_NB_CODEC_OTA st_OtaCodec;
    void *pv_RefIEList = NULL;

    pst_Call = g_call_obj + u16_CallId;

    if ( pst_Call )
    {
        appmedia_CallObjMediaStop(pst_Call->u32_CallInstance, 0, 0);

        pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            // add media descriptor IE
            memset(&st_MediaDesc, 0, sizeof(ST_IE_MEDIA_DESCRIPTOR));

            switch (ch_Audio)
            {
            case 'w':
                st_MediaDesc.e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_WB;
                break;

            case 'n':
            case 'g':
                st_MediaDesc.e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_NB;
                break;

            case 'a':
                st_MediaDesc.e_Codec = CMBS_AUDIO_CODEC_PCMA;
                break;

            case 'u':
                st_MediaDesc.e_Codec = CMBS_AUDIO_CODEC_PCMU;
                break;

            case '8':
                st_MediaDesc.e_Codec = CMBS_AUDIO_CODEC_PCM8;
                break;

            default:
                APP_CALL_ERROR_PRINT("Error - Unknown codec %c %s %d\n", ch_Audio, __FILE__, __LINE__);
                return;
            }

            cmbs_api_ie_MediaDescAdd(pv_RefIEList, &st_MediaDesc);

            if ( ch_Audio == 'g' )
            {
                // This is for testing G.711 OTA
                st_OtaCodec.e_Codec = CMBS_NB_CODEC_OTA_G711A;
                cmbs_api_ie_NBOTACodecAdd(pv_RefIEList, &st_OtaCodec);
            }

            APP_CALL_INFO_PRINT("\nCall appmedia_CallObjMediaOffer, Channel=%d, Codec %d\n", pst_Call->u32_ChannelID, st_MediaDesc.e_Codec);

            cmbs_dee_CallMediaOffer(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
        }
    }
}

//		========== appmedia_CallObjTonePlay ===========
/*!
        \brief				play tone on the media channel identified by Call ID or Caller ID
      \param[in]        psz_Value         pointer to CMBS tone enumeration string
      \param[in]        bo_On             TRUE to play, FALSE to stop
        \param[in,out]		u16_CallId  	      Call ID used, if psz_CLI is NULL
        \param[in,out]		psz_CLI		      pointer to caller ID,if not needed NULL
        \param[in]		u16_ToneDur		      This parameter is only valid for DTMF request. 
        									For all other tones value should be 0 (will be ignored).
        \return				<none>
*/
void appmedia_CallObjTonePlay(char *psz_Value, u16 u16_ToneVolume, int bo_On, u16 u16_CallId, char *psz_CLI, u16 u16_ToneDur, u16 u16_DTMFToneToHSMask)
{
    PST_CALL_OBJ pst_Call;

    if ( !psz_CLI )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_CLI);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = NULL;

        if ( _appcall_CallObjMediaGet(pst_Call) == E_APPCMBS_MEDIA_PEND ||
             _appcall_CallObjMediaGet(pst_Call) == E_APPCMBS_MEDIA_ACTIVE )
        {

            pv_RefIEList = cmbs_api_ie_GetList();

            if ( pv_RefIEList )
            {
                ST_IE_MEDIA_CHANNEL st_MediaChannel;

                memset(&st_MediaChannel, 0, sizeof(ST_IE_MEDIA_CHANNEL));
                if ( bo_On & 0x80 )
                {
                    st_MediaChannel.e_Type = CMBS_MEDIA_TYPE_AUDIO_NODE;
                }
                else
                {
                    st_MediaChannel.e_Type = CMBS_MEDIA_TYPE_AUDIO_IOM;
                }

                // reset bo_On
                bo_On &= ~0x0080;

                st_MediaChannel.u32_ChannelID = pst_Call->u32_ChannelID;
                st_MediaChannel.u32_ChannelParameter = pst_Call->u32_ChannelParameter;

                cmbs_api_ie_MediaChannelAdd(pv_RefIEList, &st_MediaChannel);

                if ( !bo_On )
                {
                    cmbs_dem_ToneStop(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
                }
                else
                {
                    ST_IE_TONE st_Tone;

                    memset(&st_Tone, 0, sizeof(ST_IE_TONE));
                    st_Tone.e_Tone = cmbs_dbg_String2E_CMBS_TONE(psz_Value);
                    if ( u16_ToneVolume )
                    {
                        cmbs_api_ie_ToneVolumeAdd(pv_RefIEList, u16_ToneVolume);
                    }
					if(u16_ToneDur)
                    {
                        cmbs_api_ie_DTMFToneDurAdd(pv_RefIEList, u16_ToneDur);
                    }
					if(u16_DTMFToneToHSMask)
                    {
                        cmbs_api_ie_DTMFToneToHSAdd(pv_RefIEList, u16_DTMFToneToHSMask);
                    }						
                    cmbs_api_ie_ToneAdd(pv_RefIEList, &st_Tone);

                    cmbs_dem_ToneStart(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
                }
            }
        }
        else if ( cmbs_dbg_String2E_CMBS_TONE(psz_Value) == CMBS_TONE_CALL_WAITING_OUTBAND )
        {
            // no media channgel connected fo the call waiting,
            // but tone sent outband (<<SIGNAl>> in {CC-INFO})
            pv_RefIEList = cmbs_api_ie_GetList();

            if ( pv_RefIEList )
            {
                ST_IE_TONE st_Tone;
                ST_IE_MEDIA_CHANNEL st_MediaChannel;

                memset(&st_MediaChannel, 0, sizeof(ST_IE_MEDIA_CHANNEL));
                st_MediaChannel.e_Type = CMBS_MEDIA_TYPE_AUDIO_IOM;
                st_MediaChannel.u32_ChannelID = u16_CallId;

                cmbs_api_ie_MediaChannelAdd(pv_RefIEList, &st_MediaChannel);
                memset(&st_Tone, 0, sizeof(ST_IE_TONE));
                st_Tone.e_Tone = CMBS_TONE_CALL_WAITING_OUTBAND;
                if ( u16_ToneVolume )
                {
                    cmbs_api_ie_ToneVolumeAdd(pv_RefIEList, u16_ToneVolume);
                }
                cmbs_api_ie_ToneAdd(pv_RefIEList, &st_Tone);

                cmbs_dem_ToneStart(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
            }
        }
    }
}

void _appDTMFPlay(PST_CALL_OBJ pst_Call, u8 *pu8_Tone, u8 u8_Len)
{
    int i, e;
    char ch_Tone[30] = { 0 };
    // Tone info send
    for (i = 0; i < u8_Len; i++)
    {
        if ( pu8_Tone[i] == '*' )
        {
            e = CMBS_TONE_DTMF_STAR;
        }
        else if ( pu8_Tone[i] == '#' )
        {
            e = CMBS_TONE_DTMF_HASH;

        }
        else if ( pu8_Tone[i] >= 0x30 && pu8_Tone[i] <= 0x39 )
        {
            e = (pu8_Tone[i] - 0x30) + CMBS_TONE_DTMF_0;
        }
        else
        {
            e = (pu8_Tone[i] - 'a') + CMBS_TONE_DTMF_A;
        }
        strncpy(ch_Tone, cmbs_dbg_GetToneName(e), (sizeof(ch_Tone) - 1));

        // added g_u8_ToneType for internal call testing via node !!! */
        appmedia_CallObjTonePlay(ch_Tone, 0, TRUE | g_u8_ToneType, _appcall_CallObjIdGet(pst_Call), NULL, 0, 0);

        SleepMs(200);        // wait 0.2 seconds
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

static bool appcall_ConvertDeviceIdFromStr(u8 *pu8_Buffer, u16 *pu16_DeviceId)
{

	u16 u16_DeviceId = 0;
	u8 u8_DeviceIdNibbleStr, u8_Index,  NumOfNibbles = sizeof(u16_DeviceId)*2;

	for(u8_Index=0; u8_Index<NumOfNibbles; u8_Index++)
		{
			u8_DeviceIdNibbleStr = pu8_Buffer[u8_Index];

			if (u8_DeviceIdNibbleStr >= '0' && u8_DeviceIdNibbleStr <= '9')
				{
					u16_DeviceId |= (u8_DeviceIdNibbleStr - '0') << (12 - u8_Index * 4);
				}
			else if (u8_DeviceIdNibbleStr >= 'A' && u8_DeviceIdNibbleStr <= 'F')
					{
						u16_DeviceId |= (u8_DeviceIdNibbleStr - 'A' + 0xA) << (12 - u8_Index * 4);
					}
		}
	
	*pu16_DeviceId = u16_DeviceId;
	
	return TRUE;	
}

static u8 appcall_ConvertDeviceIdToStr(u16 u16_DeviceId, u8 *pu8_Buffer)
{

	u8 u8_Index, u8_DeviceIdNibble = 0, NumOfNibbles = sizeof(u16_DeviceId)*2;

	for(u8_Index=1; u8_Index<=NumOfNibbles; u8_Index++)
		{

			u8_DeviceIdNibble = u16_DeviceId & 0xF;

			if (u8_DeviceIdNibble <= 9)
				{
					pu8_Buffer[NumOfNibbles-u8_Index] = u8_DeviceIdNibble + '0';
				}
			else if (u8_DeviceIdNibble >= 0xA && u8_DeviceIdNibble <= 0xF)
				{
					pu8_Buffer[NumOfNibbles-u8_Index] = u8_DeviceIdNibble + 'A' - 0xA;
				}

			u16_DeviceId >>= 4;
		}
	
	return NumOfNibbles;	
}

static bool appcall_ConvertUnitIdFromStr(u8 *pu8_Buffer, u8 *pu8_UnitId)
{

	u8 u8_UnitId = 0, u8_DeviceIdNibbleStr, u8_Index,  NumOfNibbles = sizeof(u8_UnitId)*2;

	for(u8_Index=0; u8_Index<NumOfNibbles; u8_Index++)
		{
			u8_DeviceIdNibbleStr = pu8_Buffer[u8_Index];

			if (u8_DeviceIdNibbleStr >= '0' && u8_DeviceIdNibbleStr <= '9')
				{
					u8_UnitId |= (u8_DeviceIdNibbleStr - '0') << (4 - u8_Index * 4);
				}
			else if (u8_DeviceIdNibbleStr >= 'A' && u8_DeviceIdNibbleStr <= 'F')
				{
					u8_UnitId |= (u8_DeviceIdNibbleStr - 'A' + 0xA) << (4 - u8_Index * 4);
				}
		}
	
	*pu8_UnitId = u8_UnitId;
	
	return TRUE;	
}



static u8 appcall_ConvertUnitIdToStr(u8 u8_UnitId, u8 *pu8_Buffer)
{

	u8 u8_Index, u8_UnitIdNibble = 0, NumOfNibbles = sizeof(u8_UnitId)*2;

	for(u8_Index=1; u8_Index<=NumOfNibbles; u8_Index++)
		{

			u8_UnitIdNibble = u8_UnitId & 0xF;

			if (u8_UnitIdNibble <= 9)
				{
					pu8_Buffer[NumOfNibbles - u8_Index] = u8_UnitIdNibble + '0';
				}
			else if (u8_UnitIdNibble >= 0xA && u8_UnitIdNibble <= 0xF)
				{
					pu8_Buffer[NumOfNibbles - u8_Index] = u8_UnitIdNibble + 'A' - 0xA;
				}

			u8_UnitId >>= 4;
		}
	
	return NumOfNibbles;	
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

//		========== app_CallAnswer ===========
/*!
\brief	 		Answer the call
\param[in]		pst_Call   call object
\return	 		new call id
*/
#ifdef AVS_AUDIO_APP
static u16 app_CallAnswer(PST_CALL_OBJ pst_Call)
{
    u16 u16_CallId;
    ST_APPCALL_PROPERTIES st_Properties;

    u16_CallId = _appcall_CallObjIdGet(pst_Call);

    // answer current call
    appcall_AnswerCall(&st_Properties, 0, u16_CallId, NULL);

    return u16_CallId;
}
#endif // AVS_AUDIO_APP

//		========== app_Call2HS ===========
/*!
\brief	 		CMBS Target wants to make an outgoing call
\param[in]		pvAppRefHandle   application reference pointer
\param[in]		pv_List		      IE list pointer
\return	 		<none>
*/

static void app_Call2HS(PST_CALL_OBJ pst_Call, u16 u16_DstId)
{
    ST_APPCALL_PROPERTIES st_Properties;
    ST_APPCALL_PROPERTIES st_PropertiesForHs[5];
    int n_Prop = 0;
    char ch_cld[3];
    char ch_clineid[] = "0";
    char s_codecs[] = "5,6"; //WB, NB
    char ch_melody[] = "3";

    // store HS id as Called Party Number
    pst_Call->st_CalledParty.u8_AddressLen = 1;
    ch_cld[0] = 'h';
    pst_Call->st_CalledParty.pu8_Address[0] = ch_cld[1] = (u8)u16_DstId + '0';
    ch_cld[2] = '\0';

    st_PropertiesForHs[n_Prop].e_IE = CMBS_IE_CALLERPARTY;
    st_PropertiesForHs[n_Prop++].psz_Value = pst_Call->ch_CallerID;
    st_PropertiesForHs[n_Prop].e_IE = CMBS_IE_CALLEDPARTY;
    st_PropertiesForHs[n_Prop++].psz_Value = ch_cld;
    st_PropertiesForHs[n_Prop].e_IE = CMBS_IE_MEDIADESCRIPTOR;
    st_PropertiesForHs[n_Prop++].psz_Value = s_codecs;
    st_PropertiesForHs[n_Prop].e_IE = CMBS_IE_LINE_ID;
    st_PropertiesForHs[n_Prop++].psz_Value = ch_clineid;
    st_PropertiesForHs[n_Prop].e_IE = CMBS_IE_MELODY;
    st_PropertiesForHs[n_Prop++].psz_Value = ch_melody;

    g_u16_Merge_CallId1 = _appcall_CallObjIdGet(pst_Call);

    // answer current call
    appcall_AnswerCall(&st_Properties, 0, g_u16_Merge_CallId1, NULL);

    // establish call to HS
    g_u16_Merge_CallId2 = appcall_EstablishCall(st_PropertiesForHs, n_Prop);

    // merge calls will be invoked on call answer
}

/***************************************************************************
*
*     Old function for tracking
*
****************************************************************************/

//		========== app_OnCallEstablish ===========
/*!
        \brief	 		CMBS Target wants to make an outgoing call
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/

void app_OnCallEstablish(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    PST_CALL_OBJ pst_CallPending = NULL;
    PST_LINE_OBJ pst_Line = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    ST_APPCALL_PROPERTIES st_Properties;
    void *pv_IE = NULL;
    u16 u16_IE;
    u16 u16_CallId;
    ST_IE_RESPONSE st_Response;
	#ifdef MEDIA5_SIP_SUPPORT
	int i;
	#endif
	
    st_Response.e_Response = CMBS_RESPONSE_OK;

    UNUSED_PARAMETER(pvAppRefHandle);
    APP_CALL_INFO_PRINT("%s\n", __FUNCTION__);

    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALLINSTANCE, CMBS_IE_CALLERPARTY, CMBS_IE_CALLEDPARTY (optional) , CMBS_IE_MEDIADESCRIPTOR, CMBS_IE_LINE_ID (optional)

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                pst_Call = _appcall_CallObjNew();
                if ( pst_Call )
                {
                    pst_Call->u32_CallInstance = st_IEInfo.Info.u32_CallInstance;
                }
                break;

            case CMBS_IE_LINE_ID:
                if ( pst_Call )
                {
                    //APP_CALL_INFO_PRINT("app_OnCallEstablish: HS chose LineID %u\n", st_IEInfo.Info.u8_LineId);

                    pst_Line = _appcall_LineObjGet(st_IEInfo.Info.u8_LineId);
                    _appcall_CallObjLineObjSet(pst_Call, pst_Line);
                }
                break;

            case CMBS_IE_CALLERPARTY:
                //APP_CALL_INFO_PRINT("app_OnCallEstablish: CALLER_ID address properties:0x%02x\n", st_IEInfo.Info.st_CallerParty.u8_AddressProperties);
                if ( pst_Call )
                {
					u8 u8_Index = 0;
                    /*int i;
                    APP_CALL_INFO_PRINT ( "CallerID length:%d:", st_IEInfo.Info.st_CallerParty.u8_AddressLen );
                    for ( i=0; i < st_IEInfo.Info.st_CallerParty.u8_AddressLen; i ++ )
                        APP_CALL_INFO_PRINT (" 0x%02x", st_IEInfo.Info.st_CallerParty.pu8_Address[i] );
                    APP_CALL_INFO_PRINT( "\n" );*/

                    memcpy(pst_Call->ch_CallerID, st_IEInfo.Info.st_CallerParty.pu8_Address, st_IEInfo.Info.st_CallerParty.u8_AddressLen);
                    memcpy(&pst_Call->st_CallerParty, &st_IEInfo.Info.st_CallerParty, sizeof(pst_Call->st_CallerParty));
                    pst_Call->st_CallerParty.pu8_Address = (u8 *)pst_Call->ch_CallerID;
                    pst_Call->ch_CallerID[pst_Call->st_CallerParty.u8_AddressLen] = '\0';

					// look for empty record
					for(u8_Index = 0; u8_Index < MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE; u8_Index++)
					{
						if (!strlen((char *)pst_Call->pu8_ListOfParticipants[u8_Index]))
						{	
							u8 u8_Len = 0;
							
							if( pst_Call->ch_CallerID[0] == APPCALL_DEVICE_IDENTIFIER)
							{
								u8_Len =  MIN(st_IEInfo.Info.st_CallerParty.u8_AddressLen + 1, APPCALL_CALL_CALLER_DEVICE_LEN);
							}
							else
							{
								u8_Len = st_IEInfo.Info.st_CallerParty.u8_AddressLen + 1;
							}
							
							memcpy(&pst_Call->pu8_ListOfParticipants[u8_Index][0], pst_Call->ch_CallerID, u8_Len); 

							pst_Call->u8_NumOfParticipants++;
							break;
						}
					}
                }
                break;

            case CMBS_IE_CALLEDPARTY:
                //APP_CALL_INFO_PRINT("app_OnCallEstablish: CALLED_ID address properties:0x%02x\n", st_IEInfo.Info.st_CalledParty.u8_AddressProperties);
                if ( pst_Call )
                {
                    /*int i;
                    APP_CALL_INFO_PRINT ( "CalledID length:%d:", st_IEInfo.Info.st_CalledParty.u8_AddressLen );
                    for ( i=0; i < st_IEInfo.Info.st_CalledParty.u8_AddressLen; i ++ )
                        APP_CALL_INFO_PRINT (" 0x%02x", st_IEInfo.Info.st_CalledParty.pu8_Address[i] );
                    APP_CALL_INFO_PRINT( "\n" );*/

                    memcpy(pst_Call->ch_CalledID, st_IEInfo.Info.st_CalledParty.pu8_Address, st_IEInfo.Info.st_CalledParty.u8_AddressLen);
                    memcpy(&pst_Call->st_CalledParty, &st_IEInfo.Info.st_CalledParty, sizeof(pst_Call->st_CalledParty));
                    pst_Call->st_CalledParty.pu8_Address = (u8 *)pst_Call->ch_CalledID;
                    pst_Call->ch_CalledID[pst_Call->st_CalledParty.u8_AddressLen] = '\0';
                }
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;

            }

            _appmedia_CallObjMediaPropertySet(pst_Call, u16_IE, &st_IEInfo);

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

	
        if ( st_Response.e_Response != CMBS_RESPONSE_OK )
        {
            if ( pst_Call )
            {
                APP_CALL_ERROR_PRINT("app_OnCallEstablish:  Response Error. Call Instance %d\n", pst_Call->u32_CallInstance);
            }
            else
            {
                APP_CALL_ERROR_PRINT("app_OnCallEstablish:  Response Error\n");

            }
            return;
        }

        if ( !pst_Call )
        {
            APP_CALL_INFO_PRINT("app_OnCallEstablish: No Call Instance\n");
            return;
        }
		#ifdef MEDIA5_SIP_SUPPORT
		/* More than one call not allowed, reject the outgoing call*/
		for(i=0;i<APPCALL_CALLOBJ_MAX;i++)
			{
				if(g_call_obj[i].e_Call != E_APPCMBS_CALL_CLOSE)
				{
	                ST_APPCALL_PROPERTIES st_Properties;
	                static char s_reason[5] = { 0 };
	                sprintf(s_reason, "%d", CMBS_REL_REASON_USER_BUSY);
	                // disconnecting call
	                st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
	                st_Properties.psz_Value = s_reason;
					_appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_RELEASE);
	                appcall_ReleaseCall(&st_Properties, 1, _appcall_CallObjIdGet(pst_Call), NULL);
					
	                return;
            	}
			}
		#endif
        /* Mark call as outgoing */
        pst_Call->b_Incoming = FALSE;

        /* reset codecs flag */
        pst_Call->b_CodecsOfferedToTarget = FALSE;

        _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_OUT_PEND);

        if ( g_cmbsappl.n_Token )
        {
            appcmbs_ObjectSignal(NULL, 0, _appcall_CallObjIdGet(pst_Call), CMBS_EV_DEE_CALL_ESTABLISH);
        }

		u16_CallId = _appcall_CallObjIdGet(pst_Call);
	
        // reply early media connect and start dial tone.

        // APP_CALL_INFO_PRINT("\n !!! CallProgress ");

        if ( !pst_Call->st_CalledParty.u8_AddressLen ||
             ((_appcall_CheckBackwardCompatibilityLineSelection(pst_Call)) && (pst_Call->st_CalledParty.u8_AddressLen == 2)) )

        {
            st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
            st_Properties.psz_Value = "CMBS_CALL_PROGR_SETUP_ACK\0";

            // APP_CALL_INFO_PRINT("\n if ...  ");
            appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);
        }
        else
        {
            _appcall_CheckBackwardCompatibilityLineSelection(pst_Call);

            if ( pst_Call->u8_LineId == APPCALL_NO_LINE )
            {
                /* use default line for HS */
                u32 u32_FPManagedLineID;
				bool b_CallerIsDevice;
				u8 u8_HsNum;

			#ifdef YOGA_DEMO
				b_CallerIsDevice = pst_Call->st_CallerParty.u8_AddressLen == APPCALL_CALL_PPARTICIPANT_DEVICE_LEN &&
					pst_Call->st_CallerParty.pu8_Address[APPCALL_CALL_PARTICIPANT_DEVICE_OFFSET-1] == APPCALL_DEVICE_IDENTIFIER;
				
			#else
				b_CallerIsDevice = pst_Call->st_CallerParty.u8_AddressLen == APPCALL_CALL_PPARTICIPANT_DEVICE_LEN &&
					pst_Call->st_CallerParty.pu8_Address[APPCALL_CALL_PARTICIPANT_DEVICE_OFFSET-1] == APPCALL_DEVICE_IDENTIFIER &&
					pst_Call->st_CallerParty.pu8_Address[APPCALL_CALL_PARTICIPANT_UNIT_OFFSET-1] == APPCALL_UNIT_IDENTIFIER;

			#endif
							
				// check whether caller party is a device
				if (b_CallerIsDevice)
				{
					u8_HsNum = APPCALL_INVALID_HS_NUM;
				}
				else
				{
					u8_HsNum = pst_Call->st_CallerParty.pu8_Address[0] - '0';
				}
				
                List_GetDefaultLine(u8_HsNum, &u32_FPManagedLineID);

			#ifdef YOGA_DEMO
				if(u32_FPManagedLineID == APPCALL_NO_LINE)
				{
					u32_FPManagedLineID = 0; //line 0 as desfault for Yoga demo
				}
			#endif
			
                pst_Call->u8_LineId = (u8)u32_FPManagedLineID;
            
                // APP_CALL_INFO_PRINT("\n else if NO_LINE - Line ID = %x ",  pst_Call->u8_LineId);
            }

            if ( pst_Call->u8_LineId == APPCALL_NO_LINE )
            {
                ST_APPCALL_PROPERTIES st_Properties;
                static char s_reason[5] = { 0 };
		sprintf(s_reason, "%d", CMBS_REL_REASON_USER_BUSY);
                // disconnecting call
                st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
                st_Properties.psz_Value = s_reason;
                appcall_ReleaseCall(&st_Properties, 1, _appcall_CallObjIdGet(pst_Call), NULL);
                return;
            }
            else
            {
                st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
                st_Properties.psz_Value = "CMBS_CALL_PROGR_PROCEEDING\0";
                appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);

                // Simulate network delay
                SleepMs(300);

                st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
                st_Properties.psz_Value = "CMBS_CALL_PROGR_RINGING\0";
                
                // APP_CALL_INFO_PRINT("\n  Line ID = %x ",  pst_Call->u8_LineId);
                
                appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);
            }
        }

#ifdef AVS_AUDIO_APP

        // Answer the call 
        app_CallAnswer(pst_Call);

        if (pst_Call->st_CalledParty.u8_AddressLen > 0)
        {
            // Postpone the Adio Start till Media Channel is known (CMBS_EV_DEM_CHANNEL_START) 

            // make a call to HS number 1 (temp)
            if (pst_Call->st_CalledParty.pu8_Address[0] == '#')
                app_Call2HS(pst_Call, 1);
        }

        return;

#endif // AVS_AUDIO_APP 

		// make sure Caller Party Number is not empty 
		if(pst_Call->st_CallerParty.u8_AddressLen >= 1)
		{
			// check whether a call is waiting for this HS/ device to be merged
			pst_CallPending  = appcall_GetCallPendingforDeviceObj(pst_Call->st_CallerParty);

					
			if (pst_CallPending)
			{
				ST_APPCALL_PROPERTIES st_Properties;
						
				pst_CallPending->b_WaitingForMerge = FALSE;
				memset ( &st_Properties,0, sizeof(st_Properties));

				g_u16_Merge_CallId1 = u16_CallId;
				g_u16_Merge_CallId2 = _appcall_CallObjIdGet(pst_CallPending);

				    //Answer both calls
				    APP_CALL_INFO_PRINT("Answer and merge calls: %d, %d\n", g_u16_Merge_CallId1, g_u16_Merge_CallId2);
					 
				appcall_AnswerCall ( &st_Properties, 0, g_u16_Merge_CallId2, NULL);  // current call
				appcall_AnswerCall ( &st_Properties, 0, g_u16_Merge_CallId1, NULL);  //pending call

				// stop media channel
				appmedia_CallObjMediaStop(pst_Call->u32_CallInstance, 0, NULL);
				appmedia_CallObjMediaStop(pst_CallPending->u32_CallInstance, 0, NULL);
					
				// merge calls
				appcall_MergeCalls(g_u16_Merge_CallId2, g_u16_Merge_CallId1, FALSE);
			}
			else
			{
				// check wether the calling device has specified the called party	
				if (pst_Call->st_CalledParty.u8_AddressLen == APPCALL_CALL_PPARTICIPANT_DEVICE_LEN && pst_Call->st_CalledParty.pu8_Address[0] == APPCALL_DEVICE_IDENTIFIER)
				{	
					u16 u16_DeviceId = 0;
					u8 u8_UnitId = 1;
						
					ST_HAN_VOICE_CALL_REQ_PARAMS st_CallReqParams;

					appcall_ConvertDeviceIdFromStr(&pst_Call->st_CalledParty.pu8_Address[APPCALL_CALL_PARTICIPANT_DEVICE_OFFSET], &u16_DeviceId);

					if(pst_Call->st_CalledParty.pu8_Address[5] == APPCALL_UNIT_IDENTIFIER)
					{
						appcall_ConvertUnitIdFromStr(&pst_Call->st_CalledParty.pu8_Address[APPCALL_CALL_PARTICIPANT_UNIT_OFFSET], &u8_UnitId);
					}
						
					st_CallReqParams.u32_FieldMask = 0;
					APP_CALL_INFO_PRINT("Calling device has specified the called party - device#%d, unit %d\n", u16_DeviceId, u8_UnitId);

					// Mark call as waiting to be merged
					pst_Call->b_WaitingForMerge = TRUE;

					// send FUN message to device requesting it to set up a call
					app_DsrHanVoiceCallReq(u16_DeviceId, u8_UnitId, &st_CallReqParams);
				}

				else if (pst_Call->st_CalledParty.u8_AddressLen == APPCALL_CALL_PPARTICIPANT_HS_LEN && pst_Call->st_CalledParty.pu8_Address[0] == APPCALL_HS_IDENTIFIER)
					{
						u8 u8_HsNum = pst_Call->st_CalledParty.pu8_Address[1] - '0';

						APP_CALL_INFO_PRINT("Calling device has specified the called party - HS %d\n", u8_HsNum);
		
						app_Call2HS(pst_Call, u8_HsNum);
					}

				// valid called party was not specified by the calling party, later: handle HS number/ digits
				else
				{
					u16 u16_DeviceId = 0;
					u8 u8_UnitId = 1, u8_HsNum = 0;
					bool b_MatchFound;
						
					st_apphan_VoiceCallRoutingTableRecord st_VoiceCallRoutingTableRecord;

					if (pst_Call->st_CallerParty.pu8_Address[0] == APPCALL_DEVICE_IDENTIFIER)
					{
						appcall_ConvertDeviceIdFromStr(&pst_Call->st_CallerParty.pu8_Address[1], &u16_DeviceId);
						appcall_ConvertUnitIdFromStr(&pst_Call->st_CallerParty.pu8_Address[6], &u8_UnitId);
							
					    APP_CALL_INFO_PRINT("Calling device #%d, unit%d\n", u16_DeviceId, u8_UnitId);

						// look for a match for device in Voice Call Routing Table
						b_MatchFound = p_VoiceCallRoutingTable_FindMatchForDevice(u16_DeviceId, u8_UnitId, NULL, &st_VoiceCallRoutingTableRecord);
					}
					else
					{
						// Convert HS number from char to decimal
						u8_HsNum = pst_Call->st_CallerParty.pu8_Address[0] - '0';
							
						APP_CALL_INFO_PRINT("Calling HS #%d\n", u8_HsNum);

						// look for a match for HS in Voice Call Routing Table
						b_MatchFound = p_VoiceCallRoutingTable_FindMatchForHS(u8_HsNum, NULL, &st_VoiceCallRoutingTableRecord);
					}
						
					// look for a match for this device/ HS in audio routing table
					if (b_MatchFound)
					{
					    ST_HAN_VOICE_CALL_REQ_PARAMS st_CallReqParams;

						st_CallReqParams.u32_FieldMask = 0;
						APP_CALL_INFO_PRINT("Found a match for Calling party number in Voice Call Routing Table\n");

						// Mark call as waiting to be merged
						pst_Call->b_WaitingForMerge = TRUE;

						if(st_VoiceCallRoutingTableRecord.u8_DstType == DEVICE_ENTITY)
						{
							u8 u8_Len = 0;
							pst_Call->st_CalledParty.u8_AddressLen = 0;

							// send FUN "Make a Call" req to device
							app_DsrHanVoiceCallReq(st_VoiceCallRoutingTableRecord.u16_DstId, st_VoiceCallRoutingTableRecord.u8_DstUnitId, &st_CallReqParams);

							// store device and unit id as Called Party Number 
							pst_Call->st_CalledParty.pu8_Address[0] = APPCALL_DEVICE_IDENTIFIER;
							pst_Call->st_CalledParty.u8_AddressLen++;
									
							u8_Len = appcall_ConvertDeviceIdToStr(st_VoiceCallRoutingTableRecord.u16_DstId, &pst_Call->st_CalledParty.pu8_Address[1]);
							pst_Call->st_CalledParty.u8_AddressLen += u8_Len;

							pst_Call->st_CalledParty.pu8_Address[5] = APPCALL_UNIT_IDENTIFIER;
							pst_Call->st_CalledParty.u8_AddressLen++;
																
							u8_Len  = appcall_ConvertUnitIdToStr(st_VoiceCallRoutingTableRecord.u8_DstUnitId, &pst_Call->st_CalledParty.pu8_Address[6]);
							pst_Call->st_CalledParty.u8_AddressLen += u8_Len ;
									
						}
						// Destination is HS
						else
						{
                            app_Call2HS(pst_Call, st_VoiceCallRoutingTableRecord.u16_DstId);
						}
					}
					else
					{
						APP_CALL_INFO_PRINT("Failed to find match for device in AudioRoutingTable for device#%d\n", u16_DeviceId != 0 ? u16_DeviceId : u8_HsNum);
					}
				}
			}
	    }
	

        // AG: External VoIP library integration
        // As we do not have state machines & timeouts implemented -
        // wait for '#' as a signal that digits are completed and start will a call
            
		AppCallRouter_AppSelection(_appcall_CallObjIdGet(pst_Call), pst_Call->ch_CalledID, (pst_Call->ch_CallerID), pst_Call->u8_LineId);
    }

}

//		========== app_OnCallProgress ===========
/*!
        \brief	 		CMBS Target signal call progres
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnCallProgress(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    void *pv_IE;
    u16 u16_IE;
    //u8             u8_i;
    ST_IE_RESPONSE st_Response;
    ST_IE_CALLPROGRESS st_CallProgress;
    ST_IE_CALLEDPARTY st_CalledParty;

    st_Response.e_Response = CMBS_RESPONSE_OK;

    memset(&st_CalledParty, 0, sizeof(st_CalledParty));

    UNUSED_PARAMETER(pvAppRefHandle);
    APP_CALL_INFO_PRINT("%s\n", __FUNCTION__);

    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALLINSTANCE, CMBS_IE_CALLEDPARTY and CMBS_IE_CALLPROGRESS

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                // Todo: get call resources for this CallInstance...
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
                break;

            case CMBS_IE_CALLPROGRESS:
                st_CallProgress = st_IEInfo.Info.st_CallProgress;

                if ( pst_Call && (!pst_Call->b_IsSplashRing) )  // Ignore SplashRing call events
                {
                    switch (st_CallProgress.e_Progress)
                    {
                    case CMBS_CALL_PROGR_PROCEEDING:
                        //                       _appcall_CallObjStateSet (pst_Call, E_APPCMBS_CALL_OUT_PROC );
                        extvoip_ProceedingCall(_appcall_CallObjIdGet(pst_Call));
                        break;
                    case CMBS_CALL_PROGR_RINGING:
                        _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_INC_RING);
                        extvoip_AlertingCall(_appcall_CallObjIdGet(pst_Call));
                        break;
                    case CMBS_CALL_PROGR_BUSY:
                        break;
                    case CMBS_CALL_PROGR_INBAND:
                        break;
                    default:
                        break;
                    }
                }
                break;

            case CMBS_IE_CALLEDPARTY:
                st_CalledParty = st_IEInfo.Info.st_CalledParty;
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;

            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( st_Response.e_Response != CMBS_RESPONSE_OK )
        {
            if ( pst_Call )
            {
                APP_CALL_ERROR_PRINT("app_OnCallProgress:  Response Error. Call Instance %d\n", pst_Call->u32_CallInstance);
            }
            else
            {
                APP_CALL_ERROR_PRINT("app_OnCallProgress:  Response Error\n");

            }
            return;
        }

        if ( !pst_Call )
        {
            APP_CALL_ERROR_PRINT("app_OnCallProgress: ERROR: invalid Call Instance\n");
            return;
        }

        if ( pst_Call->b_IsSplashRing )
        {
            APP_CALL_WARNING_PRINT("app_OnCallProgress: ignore due to b_IsSplashRing\n");
            // Ignore Events Except Release
            return;
        }

        //TODO  save st_CalledParty to pst_Call  if need
        /*APP_CALL_INFO_PRINT("\napp_OnCallProgress: CallInstance=0x%08X, CallProgress=%s, CalledParty[ prop=%d, pres=%d, len=%d, addr=",
               u32_CallInstance, cmbs_dbg_GetCallProgressName(st_CallProgress.e_Progress), st_CalledParty.u8_AddressProperties,
               st_CalledParty.u8_AddressPresentation, st_CalledParty.u8_AddressLen);
        for ( u8_i = 0; u8_i < st_CalledParty.u8_AddressLen; ++u8_i )
        {
            APP_CALL_INFO_PRINT("%c", st_CalledParty.pu8_Address[u8_i]);
        }
        APP_CALL_INFO_PRINT(" ]\n");*/


        if ( g_cmbsappl.n_Token )
        {
            appcmbs_ObjectSignal((void *)&st_CallProgress,
                                 sizeof(st_CallProgress),
                                 _appcall_CallObjIdGet(pst_Call),
                                 CMBS_EV_DEE_CALL_PROGRESS);
        }

    }
    else
    {
		APP_CALL_ERROR_PRINT("app_OnCallProgress: ERROR: invalid IE list\n");
    }
}

//		========== app_OnCallInbandInfo ===========
/*!
        \brief	 		CMBS Target signal inband info, e.g. digits
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnCallInbandInfo(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    ST_IE_RESPONSE st_Response;
    void *pv_IE;
    u16 u16_IE;

    st_Response.e_Response = CMBS_RESPONSE_OK;

    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                // Todo: get call resources for this CallInstance...
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
                break;

            case CMBS_IE_LINE_ID:
                if ( pst_Call )
                    pst_Call->u8_LineId = st_IEInfo.Info.u8_LineId;
                break;

            case CMBS_IE_CALLINFO :
                if ( pst_Call )
                {
				#ifdef MEDIA5_SIP_SUPPORT
                	if ( _appcall_CallObjStateGet(pst_Call) == E_APPCMBS_CALL_RELEASE )
                		{
                			return;
							/* Call is already released in OnCallEstablish*/
                		}
						#endif
                    // enter to called party entry digits;

                    if ( _appcall_CallObjStateGet(pst_Call) == E_APPCMBS_CALL_OUT_PEND )
                    {
                        if ( _appcall_CallObjMediaGet(pst_Call) == E_APPCMBS_MEDIA_PEND )
                        {
                            appmedia_CallObjTonePlay(NULL, 0, FALSE, _appcall_CallObjIdGet(pst_Call), NULL, 0, 0);
                        }

                        _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_OUT_PEND_DIAL);
                    }

                    if ( _appcall_CallObjStateGet(pst_Call) < E_APPCMBS_CALL_ACTIVE )
                    {
                        if ( (pst_Call->st_CalledParty.u8_AddressLen + st_IEInfo.Info.st_CallInfo.u8_DataLen) < sizeof(pst_Call->ch_CalledID) )
                        {
                            memcpy(pst_Call->st_CalledParty.pu8_Address + pst_Call->st_CalledParty.u8_AddressLen,
                                   st_IEInfo.Info.st_CallInfo.pu8_Info,
                                   st_IEInfo.Info.st_CallInfo.u8_DataLen);

                            *(pst_Call->st_CalledParty.pu8_Address
                              + pst_Call->st_CalledParty.u8_AddressLen
                              + st_IEInfo.Info.st_CallInfo.u8_DataLen) = '\0';

                            pst_Call->st_CalledParty.u8_AddressLen = (u8)strlen((char *)pst_Call->st_CalledParty.pu8_Address);

                            // analize handset's line selection, e.g. "*07*..."
                            if ( (pst_Call->st_CalledParty.u8_AddressLen == 4)
                                 && (pst_Call->st_CalledParty.pu8_Address[0] == '*')
                                 && (pst_Call->st_CalledParty.pu8_Address[3] == '*') )
                            {
                                u8 u8_NewLineId = APPCALL_NO_LINE;
                                char buf[3];
                                buf[0] = pst_Call->st_CalledParty.pu8_Address[1];
                                buf[1] = pst_Call->st_CalledParty.pu8_Address[2];
                                buf[2] = '\0';

                                u8_NewLineId = atoi(buf);

                                if ( u8_NewLineId < APPCALL_LINEOBJ_MAX )
                                    _appcall_CallObjLineObjSet(pst_Call, _appcall_LineObjGet(u8_NewLineId));
                            }

                            /* analyze handset's line selection - CAT-iq 2.0 way */
                            _appcall_CheckBackwardCompatibilityLineSelection(pst_Call);

                            if ( pst_Call->u8_LineId == APPCALL_NO_LINE )
                            {
                                /* use default line for HS */
                                u32 u32_FPManagedLineID;
								u8 u8_HsNum;
								bool b_CallerIsDevice;

							#ifdef YOGA_DEMO
								b_CallerIsDevice = pst_Call->st_CallerParty.u8_AddressLen == APPCALL_CALL_PPARTICIPANT_DEVICE_LEN &&
									pst_Call->st_CallerParty.pu8_Address[APPCALL_CALL_PARTICIPANT_DEVICE_OFFSET-1] == APPCALL_DEVICE_IDENTIFIER;
								
							#else
								b_CallerIsDevice = pst_Call->st_CallerParty.u8_AddressLen == APPCALL_CALL_PPARTICIPANT_DEVICE_LEN &&
									pst_Call->st_CallerParty.pu8_Address[APPCALL_CALL_PARTICIPANT_DEVICE_OFFSET-1] == APPCALL_DEVICE_IDENTIFIER &&
									pst_Call->st_CallerParty.pu8_Address[APPCALL_CALL_PARTICIPANT_UNIT_OFFSET-1] == APPCALL_UNIT_IDENTIFIER;

							#endif
								
								// check whether caller party is a device
								if (b_CallerIsDevice)
								{
									u8_HsNum = APPCALL_INVALID_HS_NUM;
								}
								else
								{
									u8_HsNum = pst_Call->st_CallerParty.pu8_Address[0] - '0';
								}
								
                                List_GetDefaultLine(u8_HsNum, &u32_FPManagedLineID);

								#ifdef YOGA_DEMO
								if(u32_FPManagedLineID == APPCALL_NO_LINE)
								{
									u32_FPManagedLineID = 0; //line 0 as desfault for Yoga demo
								}
								#endif
								
                                pst_Call->u8_LineId = (u8)u32_FPManagedLineID;
                            }

                            //release call, if there is no available line
                            if ( pst_Call->u8_LineId == APPCALL_NO_LINE )
                            {
                                ST_APPCALL_PROPERTIES st_Properties;
                                static char s_reason[5] = { 0 };
				sprintf(s_reason, "%d", CMBS_REL_REASON_USER_BUSY);
                                // disconnecting call
                                st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
                                st_Properties.psz_Value = s_reason;
								APP_CALL_INFO_PRINT("app_OnCallInbandInfo:  No line available, releasing call.\n");
                                appcall_ReleaseCall(&st_Properties, 1, _appcall_CallObjIdGet(pst_Call), NULL);
                                return;
                            }


                            if ( g_EarlyMediaAutoCfm &&
                                 ((pst_Call->st_CalledParty.u8_AddressLen > 2) ||
                                  (pst_Call->st_CalledParty.u8_AddressLen > 0 && pst_Call->st_CalledParty.pu8_Address[0] != '#')) )
                            {
                                ST_APPCALL_PROPERTIES st_Properties;
                                u16 u16_CallId = _appcall_CallObjIdGet(pst_Call);

                                if ( pst_Call->e_Call < E_APPCMBS_CALL_OUT_RING )
                                {
                                    if ( pst_Call->u8_LineId == APPCALL_NO_LINE )
                                    {
                                        // if there is still no Line ID assigned
                                        /* use default line for HS */
                                        u32 u32_FPManagedLineID;
                                        List_GetDefaultLine(pst_Call->st_CallerParty.pu8_Address[0] - '0', &u32_FPManagedLineID);
                                        pst_Call->u8_LineId = (u8)u32_FPManagedLineID;

                                        APP_CALL_ERROR_PRINT("\n !!! Still NO_LINE - assign it now - Line ID = %x ",  pst_Call->u8_LineId);
                                    }
                                    
                                    st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
                                    st_Properties.psz_Value = "CMBS_CALL_PROGR_PROCEEDING\0";
                                    appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);

                                    // Simulate network delay
                                    SleepMs(300);

                                    st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
                                    st_Properties.psz_Value = "CMBS_CALL_PROGR_RINGING\0";
                                    appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);
                                }
                            }
                            else
                                appmedia_CallObjTonePlay("CMBS_TONE_RING_BACK\0", 0, TRUE, _appcall_CallObjIdGet(pst_Call), NULL, 0, 0);

                            if ( (pst_Call->st_CalledParty.u8_AddressLen > 3)
                                 && (pst_Call->st_CalledParty.pu8_Address[0] == '*')
                                 && (pst_Call->st_CalledParty.pu8_Address[1] == '*')
                                 && (pst_Call->st_CalledParty.pu8_Address[pst_Call->st_CalledParty.u8_AddressLen - 1] == '*')
                                 && ((pst_Call->st_CalledParty.pu8_Address[2] == '6') || (pst_Call->st_CalledParty.pu8_Address[2] == '7')) )
                            {
                                ST_APPCALL_PROPERTIES st_Properties[6];
                                int n_Prop = 0;
                                char ch_cni[] = "Device --";
                                char ch_cld[4];
                                char ch_clineid[] = "0";
                                char s_codecs[] = "5,6"; //WB and NB
                                char ch_melody[] = "3";

                                ch_cni[7] = pst_Call->ch_CallerID[0];
                                if ( pst_Call->ch_CallerID[1] != '\0' )
                                {
                                    ch_cni[8] = pst_Call->ch_CallerID[1];
                                    ch_cni[9] = '\0';
                                }
                                else
                                {
                                    ch_cni[8] = '\0';
                                }

                                ch_cld[0] = (pst_Call->st_CalledParty.pu8_Address[2] == '6') ? 'h' : 'd';
                                ch_cld[1] = pst_Call->st_CalledParty.pu8_Address[3];
                                if ( (pst_Call->st_CalledParty.pu8_Address[4] != '\0') && (pst_Call->st_CalledParty.pu8_Address[4] != '*') )
                                {
                                    ch_cld[2] = pst_Call->st_CalledParty.pu8_Address[4];
                                    ch_cld[3] = '\0';
                                }
                                else
                                {
                                    ch_cld[2] = '\0';
                                }

                                st_Properties[n_Prop].e_IE = CMBS_IE_CALLERPARTY;
                                st_Properties[n_Prop++].psz_Value = pst_Call->ch_CallerID;
                                st_Properties[n_Prop].e_IE = CMBS_IE_CALLEDPARTY;
                                st_Properties[n_Prop++].psz_Value = ch_cld;
                                st_Properties[n_Prop].e_IE = CMBS_IE_MEDIADESCRIPTOR;
                                st_Properties[n_Prop++].psz_Value = s_codecs;
                                st_Properties[n_Prop].e_IE = CMBS_IE_LINE_ID;
                                st_Properties[n_Prop++].psz_Value = ch_clineid;
                                st_Properties[n_Prop].e_IE = CMBS_IE_CALLERNAME;
                                st_Properties[n_Prop++].psz_Value = ch_cni;
                                st_Properties[n_Prop].e_IE = CMBS_IE_MELODY;
                                st_Properties[n_Prop++].psz_Value = ch_melody;

                                g_u16_Merge_CallId2 = appcall_EstablishCall(st_Properties, n_Prop);
                                g_u16_Merge_CallId1 = _appcall_CallObjIdGet(pst_Call);
                            }
                        }
                        else
                        {
                            APP_CALL_ERROR_PRINT("Digits full\n");
                        }
                    }

                    else if ( _appcall_CallObjStateGet(pst_Call) == E_APPCMBS_CALL_ACTIVE )
                    {
#ifdef AVS_AUDIO_APP
                        if (st_IEInfo.Info.st_CallInfo.u8_DataLen > 0)
                        {
                            if (st_IEInfo.Info.st_CallInfo.pu8_Info[0] == '#')
                            {
                                p_AudioApp_AudioStop((u8 *)pst_Call->ch_CallerID);
                            }
                            else if (st_IEInfo.Info.st_CallInfo.pu8_Info[0] == '*')
                            {
                                rel_after_sec = 0;
                                p_AudioApp_AudioStart((u8)pst_Call->u32_ChannelID, (u8 *)pst_Call->ch_CallerID);
                            }
                        }

#endif // AVS_AUDIO_APP
                        // following lines are commented to pass CAT-iq 2.0 tests - no FP DTMF echo to CAT-iq 2.0 HS
                        // For GAP HS it is legal, though, but not implemented here
                        //                      _appDTMFPlay ( pst_Call,
                        //                                     st_IEInfo.Info.st_CallInfo.pu8_Info,
                        //                                     st_IEInfo.Info.st_CallInfo.u8_DataLen );
                    }

					/* 
					 * Send digits to ext voip App as and when received.
					 */
					if (st_IEInfo.Info.st_CallInfo.u8_DataLen > 0)
					{
						extvoip_SendDigits(_appcall_CallObjIdGet(pst_Call), ( const char* )st_IEInfo.Info.st_CallInfo.pu8_Info);
					}
                }
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            }

            if ( st_Response.e_Response != CMBS_RESPONSE_OK )
            {
                if ( pst_Call )
                {
                    APP_CALL_ERROR_PRINT("app_OnCallInbandInfo:  Response Error. Call Instance %d\n", pst_Call->u32_CallInstance);
                }
                else
                {
                    APP_CALL_ERROR_PRINT("app_OnCallInbandInfo:  Response Error\n");

                }
                return;
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

//		========== app_OnCallAnswer ===========
/*!
        \brief	 		CMBS Target answer call
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnCallAnswer(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    ST_IE_RESPONSE st_Response;
    void *pv_IE;
    u16 u16_IE;

    st_Response.e_Response = CMBS_RESPONSE_OK;

    UNUSED_PARAMETER(pvAppRefHandle);
    APP_CALL_INFO_PRINT("%s\n", __FUNCTION__);

    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALLINSTANCE, CMBS_IE_MEDIACHANNEL and CMBS_IE_MEDIADESCRIPTOR
        // Todo: Search for call instance first

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                // Todo: get call resources for this CallInstance...
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
                break;

            case CMBS_IE_CALLEDPARTY:
                //APP_CALL_INFO_PRINT("app_OnCallAnswer: CALLED_ID address properties:0x%02x\n", st_IEInfo.Info.st_CalledParty.u8_AddressProperties);
                if ( pst_Call )
                {
                    /*int i;
                    APP_CALL_INFO_PRINT ( "CalledID length: %d:", st_IEInfo.Info.st_CalledParty.u8_AddressLen );
                    for ( i=0; i < st_IEInfo.Info.st_CalledParty.u8_AddressLen; i ++ )
                        APP_CALL_INFO_PRINT (" 0x%02x", st_IEInfo.Info.st_CalledParty.pu8_Address[i] );
                    APP_CALL_INFO_PRINT( "\n" );*/

                    memcpy(pst_Call->ch_CalledID, st_IEInfo.Info.st_CalledParty.pu8_Address, st_IEInfo.Info.st_CalledParty.u8_AddressLen);
                    memcpy(&pst_Call->st_CalledParty, &st_IEInfo.Info.st_CalledParty, sizeof(pst_Call->st_CalledParty));
                    pst_Call->st_CalledParty.pu8_Address = (u8 *)pst_Call->ch_CalledID;
                    pst_Call->ch_CalledID[pst_Call->st_CalledParty.u8_AddressLen] = '\0';
                }
                break;
            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            }

            _appmedia_CallObjMediaPropertySet(pst_Call, u16_IE, &st_IEInfo);

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( st_Response.e_Response != CMBS_RESPONSE_OK )
        {
            if ( pst_Call )
            {
                APP_CALL_ERROR_PRINT("app_OnCallAnswer:  Response Error. Call Instance %d\n", pst_Call->u32_CallInstance);
            }
            else
            {
                APP_CALL_ERROR_PRINT("app_OnCallAnswer:  Response Error\n");

            }
            return;
        }

        if ( !pst_Call )
        {
            APP_CALL_ERROR_PRINT("app_OnCallAnswer: ERROR: No Call Instance\n");
            return;
        }

        _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_ACTIVE);

        if ( g_call_automat )
        {
            appmedia_CallObjMediaStart(pst_Call->u32_CallInstance, 0, 0xFF, NULL);
        }

        // AG: External VoIP library integration
        // Answer incoming VoIP call
        extvoip_AnswerCall(_appcall_CallObjIdGet(pst_Call));

        cmbsevent_onCallAnswered(_appcall_CallObjIdGet(pst_Call));

        if ( (g_u16_Merge_CallId2 != 0xFF) && (g_u16_Merge_CallId2 == _appcall_CallObjIdGet(pst_Call)) && (g_u16_Merge_CallId1 != 0xFF) )
        {
			bool b_ConvertToIntFlag = FALSE;
			
			// clear call pending 
			g_call_obj[g_u16_Merge_CallId1].b_WaitingForMerge = FALSE;
			g_call_obj[g_u16_Merge_CallId2].b_WaitingForMerge = FALSE;
			
			// stop media channel
			appmedia_CallObjMediaStop(g_call_obj[g_u16_Merge_CallId1].u32_CallInstance, 0, NULL);
			appmedia_CallObjMediaStop(g_call_obj[g_u16_Merge_CallId2].u32_CallInstance, 0, NULL);

		#ifdef YOGA_DEMO
			b_ConvertToIntFlag = TRUE;
		#endif
		
            appcall_MergeCalls(g_u16_Merge_CallId1, g_u16_Merge_CallId2, b_ConvertToIntFlag);
        }
    }
}


//		========== app_OnCallRelease ===========
/*!
        \brief	 		CMBS Target release call
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnCallRelease(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    void *pv_IE;
    u16 u16_IE;
    ST_IE_RELEASE_REASON st_Reason;
    u8 u8_idx;

    UNUSED_PARAMETER(pvAppRefHandle);

    memset(&st_Reason, 0, sizeof(ST_IE_RELEASE_REASON));
    
    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALLINSTANCE and CMBS_IE_CALLRELEASE_REASON

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            if ( u16_IE == CMBS_IE_CALLINSTANCE )
            {
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
            }

            if ( u16_IE == CMBS_IE_CALLRELEASE_REASON )
            {
                // we have to keep the structure for later messaging
                cmbs_api_ie_CallReleaseReasonGet(pv_IE, &st_Reason);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( !pst_Call )
        {
            APP_CALL_ERROR_PRINT("app_OnCallRelease: ERROR: No Call Instance\n");
            return;
        }

        // handle reason CMBS_REL_SPLASH_RING_TIMEOUT only if call is still in b_IsSplashRing. If not -- Ignore it
        if ( st_Reason.e_Reason == CMBS_REL_SPLASH_RING_TIMEOUT )
        {
            if ( pst_Call->b_IsSplashRing )
            {
                //This "Release" is "Release me" event, a trigger from target to host to send Release call FROM HOST!
                appcall_ReleaseSplashRingCall(pst_Call);
            }
            // Always return
            return;
        }

        APP_CALL_INFO_PRINT("Call release request for instance 0x%04X\n", st_IEInfo.Info.u32_CallInstance);

        /* Add Entry to call list */
        if ( pst_Call->b_Incoming )
        {
            /* Incoming accepted call */
            _appcall_AddCallEntry(LIST_TYPE_INCOMING_ACCEPTED_CALLS, pst_Call->ch_CallerID, pst_Call->u8_LineId);
			//ListChangeNotif_SendListChangeDetails(LIST_TYPE_INCOMING_ACCEPTED_CALLS);
        }
        else
        {
            /* Outgoing call */
            _appcall_AddCallEntry(LIST_TYPE_OUTGOING_CALLS, pst_Call->ch_CalledID, pst_Call->u8_LineId);
			//ListChangeNotif_SendListChangeDetails(LIST_TYPE_OUTGOING_CALLS);
        }
		
		appcall_HSUseChange(pst_Call->u8_LineId, 0, pst_Call->u32_CallInstance);
		appcall_RemoveCallFromTable(pst_Call->u8_LineId, pst_Call->u32_CallInstance);			 
		appcall_HandleLineUseStatus(pst_Call->u8_LineId);

        if ( _appcall_CallObjMediaGet(pst_Call) == E_APPCMBS_MEDIA_ACTIVE )
        {
            // release Media stream
            /*!\todo release media stream */
            appmedia_CallObjMediaStop(pst_Call->u32_CallInstance, 0, NULL);

            u8_idx = __appcall_FindFreeReleasePendingRequest();

            if ( u8_idx < MAX_RELEASE_CALL_INST )
            {
                g_CallRelease[u8_idx].u8_CallReleaseInProgress = TRUE;
                g_CallRelease[u8_idx].u32_CallInstance = pst_Call->u32_CallInstance;
                memcpy(&g_CallRelease[u8_idx].st_Reason, &st_Reason, sizeof(ST_IE_RELEASE_REASON));
            }
            else
            {
                APP_CALL_ERROR_PRINT("Error! Too many parallel call releasing in progress \n");
                // send Release_Complete
                cmbs_dee_CallReleaseComplete(pvAppRefHandle, pst_Call->u32_CallInstance);
            }

            if ( pst_Call->e_Call == E_APPCMBS_CALL_CONFERENCE )
            {
                // When one party of the conference is released, need to update the other call state
                PST_CALL_OBJ pst_ConfPartnerCall;
                pst_ConfPartnerCall = _appcall_CallObjGet(appcall_ConferencePartnerGet(pst_Call->u32_CallInstance), NULL);
                if ( pst_ConfPartnerCall == NULL )
                    APP_CALL_ERROR_PRINT("Error! Conference call partner was not found\n");
                else
                {
                    _appcall_CallObjStateSet(pst_ConfPartnerCall, E_APPCMBS_CALL_ACTIVE);
                    appcall_ConferenceCallTableClearEntry(pst_Call->u32_CallInstance);
                }
            }
#ifdef AVS_AUDIO_APP
            {
                p_AudioApp_AudioEnd((u8 *)pst_Call->ch_CallerID);
            }
#endif // AVS_AUDIO_APP
        }
        else
        {
        // AG: External VoIP library integration
        // Release VoIP call
        extvoip_DisconnectCall(_appcall_CallObjIdGet(pst_Call));

        cmbsevent_onCallReleased(_appcall_CallObjIdGet(pst_Call));

        // send Release_Complete
        cmbs_dee_CallReleaseComplete(pvAppRefHandle, pst_Call->u32_CallInstance);

        _appcall_CallObjDelete(pst_Call->u32_CallInstance, NULL);
        }
    }

#ifdef AVS_AUDIO_APP
    if (_appcall_CallObjIdGet(pst_Call) == g_u16_Merged)
        g_u16_Merged = 0xFF;
#endif // AVS_AUDIO_APP
}


//		========== app_OnCallReleaseComplete ===========
/*!
        \brief	 		CMBS Target reports on call relase completion
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		     IE list pointer
        \return	 		<none>
*/
void app_OnCallReleaseComplete(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    void *pv_IE;
    u16 u16_IE;

    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        /* collect information elements. we expect: CMBS_IE_CALLINSTANCE */

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            if ( u16_IE == CMBS_IE_CALLINSTANCE )
            {
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( !pst_Call )
        {
            APP_CALL_ERROR_PRINT("app_OnCallReleaseComplete: ERROR: No Call Instance\n");
            return;
        }

        /* Add Entry to call list (if not Splash Ring)*/
        if ( !pst_Call->b_IsSplashRing )
        {
            if ( pst_Call->b_Incoming )
            {
                if ( pst_Call->e_Call < E_APPCMBS_CALL_ACTIVE )
                {
                    /* Missed call */
                    _appcall_AddCallEntry(LIST_TYPE_MISSED_CALLS, pst_Call->ch_CallerID, pst_Call->u8_LineId);
                    ListChangeNotif_MissedCallListChanged(pst_Call->u8_LineId, TRUE, CMBS_ALL_RELEVANT_HS_ID);
					//ListChangeNotif_SendListChangeDetails(LIST_TYPE_MISSED_CALLS);
                }
                else
                {
                    /* Incoming accepted call */
                    _appcall_AddCallEntry(LIST_TYPE_INCOMING_ACCEPTED_CALLS, pst_Call->ch_CallerID, pst_Call->u8_LineId);
					//ListChangeNotif_SendListChangeDetails(LIST_TYPE_INCOMING_ACCEPTED_CALLS);
                }
            }
            else
            {
                /* Outgoing call */
                _appcall_AddCallEntry(LIST_TYPE_OUTGOING_CALLS, pst_Call->ch_CalledID, pst_Call->u8_LineId);
				//ListChangeNotif_SendListChangeDetails(LIST_TYPE_OUTGOING_CALLS);
            }
        }

		appcall_HSUseChange(pst_Call->u8_LineId, 0, pst_Call->u32_CallInstance);
		appcall_RemoveCallFromTable(pst_Call->u8_LineId, pst_Call->u32_CallInstance);			 
		appcall_HandleLineUseStatus(pst_Call->u8_LineId);

        if ( pst_Call )
            extvoip_DisconnectDoneCall(_appcall_CallObjIdGet(pst_Call));

        _appcall_CallObjDelete(pst_Call->u32_CallInstance, NULL);
    }
    else
    {
        APP_CALL_ERROR_PRINT("app_OnCallReleaseComplete: ERROR: No IE List!\n");
    }
}

void app_OnCallHold(void *pvAppRefHandle, void *pv_List)
{
    u32 u32_CallInstance = 0;
    void *pv_IE;
    u16 u16_IE;
    u16 u16_Handset = 0;
    PST_CALL_OBJ pst_Call = NULL;


    if ( pvAppRefHandle )
    {};

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            if ( CMBS_IE_CALLINSTANCE == u16_IE )
            {
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
                pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
            }
            else if ( CMBS_IE_HANDSETS == u16_IE )
            {
                cmbs_api_ie_HandsetsGet(pv_IE, &u16_Handset);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

    APP_CALL_INFO_PRINT("app_OnCallHold: Call Instance = %x, HS-Mask = 0x%X\n", u32_CallInstance, u16_Handset);
    APP_CALL_INFO_PRINT("\nHold ");
    if ( g_HoldCfm == 1 )
    {
        APP_CALL_INFO_PRINT("accepted\n");
        cmbs_dee_CallHoldRes(pvAppRefHandle, u32_CallInstance, u16_Handset, CMBS_RESPONSE_OK);
        if ( pst_Call )
        {
            _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_ON_HOLD);
            extvoip_HoldCall(_appcall_CallObjIdGet(pst_Call));
        }
    }
    else
    {
        APP_CALL_WARNING_PRINT("declined\n");
        cmbs_dee_CallHoldRes(pvAppRefHandle, u32_CallInstance, u16_Handset, CMBS_RESPONSE_ERROR);
    }
}


void app_OnInternalCallTransfer(void *pvAppRefHandle, void *pv_List)
{
    ST_IE_INTERNAL_TRANSFER st_IntTransReq;
    void *pv_IE;
    u16 u16_IE;

    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            switch (u16_IE)
            {
            case CMBS_IE_INTERNAL_TRANSFER:
                cmbs_api_ie_InternalCallTransferReqGet(pv_IE, &st_IntTransReq);
                break;

            default:
                break;
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

void app_OnCallTransfer(void *pvAppRefHandle, void *pv_List)
{
    ST_IE_CALLTRANSFERREQ st_TrfReq;
    ST_IE_RESPONSE st_Resp;
    void *pv_IE;
    u16 u16_IE;

    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        
        while (pv_IE != NULL)
        {
            switch (u16_IE)
            {
            case CMBS_IE_CALLTRANSFERREQ:
                cmbs_api_ie_CallTransferReqGet(pv_IE, &st_TrfReq);
                break;
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

    if ( g_TransferAutoCfm == 1 )
    {
        st_Resp.e_Response = CMBS_RESPONSE_OK;
    }
    else
    {
        st_Resp.e_Response = CMBS_RESPONSE_ERROR;
    }

    cmbs_dcm_CallTransferRes(&st_TrfReq, &st_Resp);
}

void app_OnCallConference(void *pvAppRefHandle, void *pv_List)
{
    ST_IE_CALLTRANSFERREQ st_TrfReq;
    ST_IE_RESPONSE st_Resp;
    void *pv_IE;
    u16 u16_IE;
	u8 u8_CallId = 0;

    ST_APPCMBS_IEINFO st_IEInfo;

    UNUSED_PARAMETER(pvAppRefHandle);
    memset(&st_TrfReq, 0, sizeof(ST_IE_CALLTRANSFERREQ));

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLTRANSFERREQ:
                cmbs_api_ie_CallTransferReqGet(pv_IE, &st_TrfReq);
                break;
			case CMBS_IE_CALLINFO:
				cmbs_api_ie_ByteValueGet(pv_IE, &u8_CallId, CMBS_IE_CALLINFO);
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

    //cmbs_api_ie_CallTransferReqGet(u16_IE,&st_TrfReq);
    /*when even received use CallID receive as conf CallID & release other one simply maintain one callid which is received*/
	APP_CALL_TRACE_PRINT("\n TWO EXT LINE CONF st_TrfReq->u8_TermInstance (HS Number) = %x ", st_TrfReq.u8_TermInstance);
	APP_CALL_TRACE_PRINT("\n TWO EXT LINE CONF CallId (Receive CallID during conf) = %x ",u8_CallId);
    if (st_TrfReq.u8_TermInstance <= CMBS_HS_SUBSCRIBED_MAX_NUM)
    {
		st_HsCallState[st_TrfReq.u8_TermInstance-1][0].u8_ActCallID = u8_CallId;
		st_HsCallState[st_TrfReq.u8_TermInstance-1][0].e_CallType = CMBS_CALL_STATE_TYPE_CONFERENCE;
		st_HsCallState[st_TrfReq.u8_TermInstance-1][0].u16_HandsetsMask = 1 << (st_TrfReq.u8_TermInstance - 1);
		//Clear Other Call Id
	    memset(&st_HsCallState[st_TrfReq.u8_TermInstance-1][1], 0, sizeof(ST_IE_CALL_STATE));
		st_HsCallState[st_TrfReq.u8_TermInstance-1][1].u8_ActCallID = 0xFF;
    }

    if ( g_ConfAutoCfm == 1 )
    {
        st_Resp.e_Response = CMBS_RESPONSE_OK;
        _appcall_CallObjStateSet(_appcall_CallObjGet(st_TrfReq.u32_CallInstanceFrom, NULL), E_APPCMBS_CALL_CONFERENCE);
        _appcall_CallObjStateSet(_appcall_CallObjGet(st_TrfReq.u32_CallInstanceTo, NULL), E_APPCMBS_CALL_CONFERENCE);

        if ( appcall_ConferenceCallTableSet(st_TrfReq.u32_CallInstanceFrom, st_TrfReq.u32_CallInstanceTo) == APPCALL_NO_INDEX )
            APP_CALL_ERROR_PRINT("Error setting call table!\n");
        extvoip_ConferenceCall(_appcall_CallObjIdGet(_appcall_CallObjGet(st_TrfReq.u32_CallInstanceFrom, NULL)), _appcall_CallObjIdGet(_appcall_CallObjGet(st_TrfReq.u32_CallInstanceTo, NULL)));

    }
    else
    {
        st_Resp.e_Response = CMBS_RESPONSE_ERROR;
    }

    cmbs_dcm_CallConferenceRes(&st_TrfReq, &st_Resp);

}

void app_OnMediaOfferRes(void *pvAppRefHandle, void *pv_List)
{
    ST_IE_MEDIA_DESCRIPTOR st_MediaDesc;
    u32 u32_CallInstance;
    void *pv_IE;
    u16 u16_IE;
    PST_CALL_OBJ pst_Call = NULL;

    UNUSED_PARAMETER(pvAppRefHandle);
    memset(&st_MediaDesc, 0, sizeof(ST_IE_MEDIA_DESCRIPTOR));

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            if ( CMBS_IE_CALLINSTANCE == u16_IE )
            {
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
                pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
            }
            else if ( CMBS_IE_MEDIADESCRIPTOR == u16_IE )
            {
                cmbs_api_ie_MediaDescGet(pv_IE, &st_MediaDesc);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
        if ( pst_Call )
        {
            if ( pst_Call->e_Codec != st_MediaDesc.e_Codec )
            {
                pst_Call->e_Codec = st_MediaDesc.e_Codec;
                extvoip_MediaAckChange(_appcall_CallObjIdGet(pst_Call), (EXTVOIP_CODEC)st_MediaDesc.e_Codec);
            }

            appmedia_CallObjMediaStart(u32_CallInstance, 0, 0xFF, 0);
        }
    }
}

void app_OnHsCodecCfmFailed(void *pvAppRefHandle, void *pv_List)
{
    u32 u32_CallInstance = 0;
    void *pv_IE;
    u16 u16_IE;
    ST_IE_RESPONSE st_Response;

    st_Response.e_Response = CMBS_RESPONSE_OK;

    if ( pvAppRefHandle )
    {};

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            if ( CMBS_IE_CALLINSTANCE == u16_IE )
            {
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
            }
            else if ( CMBS_IE_RESPONSE == u16_IE )
            {
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

    if ( st_Response.e_Response != CMBS_RESPONSE_OK )
    {
        APP_CALL_ERROR_PRINT("app_OnHsCodecCfmFailed Recieved response not OK!. Call Instance %d\n", u32_CallInstance);
        return;
    }

    APP_CALL_INFO_PRINT("\n\napp_OnHsCodecCfmFailed:  CallInstance = 0x%08X\n", u32_CallInstance);

}

void app_OnCallResume(void *pvAppRefHandle, void *pv_List)
{
    u32 u32_CallInstance = 0;
    void *pv_IE;
    u16 u16_IE;
    u16 u16_Handset = 0;
    PST_CALL_OBJ pst_Call = NULL;


    if ( pvAppRefHandle )
    {};

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            if ( CMBS_IE_CALLINSTANCE == u16_IE )
            {
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
                pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
            }
            else if ( CMBS_IE_HANDSETS == u16_IE )
            {
                cmbs_api_ie_HandsetsGet(pv_IE, &u16_Handset);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

    APP_CALL_INFO_PRINT("app_OnCallResume: Call Instance = %x, HS-Mask = 0x%X\n", u32_CallInstance, u16_Handset);
    APP_CALL_INFO_PRINT("\nHold resume ");
    if ( g_HoldResumeCfm == 1 )
    {
        APP_CALL_INFO_PRINT("accepted\n");
        cmbs_dee_CallResumeRes(pvAppRefHandle, u32_CallInstance, u16_Handset, CMBS_RESPONSE_OK);

        if ( pst_Call )
        {
            _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_ACTIVE);
            extvoip_ResumeCall(_appcall_CallObjIdGet(pst_Call));
        }
    }
    else
    {
        APP_CALL_ERROR_PRINT("declined\n");
        cmbs_dee_CallResumeRes(pvAppRefHandle, u32_CallInstance, u16_Handset, CMBS_RESPONSE_ERROR);
    }
}


void app_OnCallState(void *pvAppRefHandle, void *pv_List)
{
    u32 u32_CallInstance = 0;
    void *pv_IE;
    u16 u16_IE;
    ST_IE_CALL_STATE st_CallState;
    ST_IE_RESPONSE st_Response;
    ST_IE_HS_RELEASE_REASON st_HsState= {0,};
    PST_CALL_OBJ pst_Call = NULL;
	ST_IE_UPDATED_DEVICE_STATE st_DeviceState = { 0 };

	u16 u16_HsMask;
	u8 u8_HsNum;

    memset(&st_CallState, 0, sizeof(ST_IE_CALL_STATE));
    st_Response.e_Response = CMBS_RESPONSE_OK;

    if ( pvAppRefHandle )
    {};

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE)
        {
            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
                break;

            case CMBS_IE_CALLSTATE:
                cmbs_api_ie_CallStateGet(pv_IE, &st_CallState);
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            
            case CMBS_IE_HSRELEASE_REASON:
                cmbs_api_ie_HSReleaseReasonGet(pv_IE, &st_HsState);
                break;
				
			case CMBS_IE_UPDATED_DEVICE_STATE:
				cmbs_api_ie_UpdatedDeviceStateGet(pv_IE, &st_DeviceState);
				break;
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

    pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
	//Basu Update Call Status & store
	appcall_SetHsStatus(u32_CallInstance, &st_CallState);
    if ( pst_Call != NULL )
    {
        if ( pst_Call->b_IsSplashRing )
        {
            // Ignore Events Except Release
            return;
        }
        switch (st_CallState.e_CallStatus)
        {
        case CMBS_CALL_STATE_STATUS_CONF_CONNECTED:


           if((st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_INCOMING) || (st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_OUTGOING))
           {
             if( st_CallState.u16_HandsetsMask != 0)
              {
          	     appcall_HSUseChange(pst_Call->u8_LineId, st_CallState.u16_HandsetsMask, u32_CallInstance);
                 // Keep current HS mask for next change
                 appcall_SetHSMask(pst_Call->u8_LineId, u32_CallInstance, st_CallState.u16_HandsetsMask);
               }
           }

			// remove all HSs from list of participants
			app_RemoveAllHSFromListOfParticipants(pst_Call);

			// update active HS in list of participants
			u16_HsMask = st_CallState.u16_HandsetsMask;
			u8_HsNum = 1;
			
			while (u16_HsMask)
			{
				// check whether HS mask is set - HS is active
				if(u16_HsMask & 1)
				{
					app_AddHSToListOfParticipants(pst_Call, u8_HsNum);
				}

				u16_HsMask >>= 1;
				u8_HsNum++;
			}

            
            break;
        case CMBS_CALL_STATE_STATUS_CALL_CONNECTED:
			
            if((st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_INCOMING) || (st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_OUTGOING))
            {
                appcall_HSUseChange(pst_Call->u8_LineId, st_CallState.u16_HandsetsMask, u32_CallInstance);
                if(appcall_IsCallInTable(pst_Call->u8_LineId, pst_Call->u32_CallInstance) == FALSE)
                {
                    appcall_SetActiveCallTable(pst_Call->u8_LineId, pst_Call->u32_CallInstance);
                 }               
                 appcall_HandleLineUseStatus(pst_Call->u8_LineId);

                 // Keep current HS mask for next change
                 appcall_SetHSMask(pst_Call->u8_LineId, u32_CallInstance, st_CallState.u16_HandsetsMask);
             }
            _appcall_CallObjStateSet( pst_Call, E_APPCMBS_CALL_ACTIVE);

			// remove all HSs from list of participants
			app_RemoveAllHSFromListOfParticipants(pst_Call);

			// update active HS in list of participants
			u16_HsMask = st_CallState.u16_HandsetsMask;
			u8_HsNum = 1;
			
			while (u16_HsMask)
			{
				// check whether HS mask is set - HS is active
				if(u16_HsMask & 1)
				{
					app_AddHSToListOfParticipants(pst_Call, u8_HsNum);
				}

				u16_HsMask >>= 1;
				u8_HsNum++;
			}

#ifdef AVS_AUDIO_APP
             if (st_CallState.u16_HandsetsMask == 0 && g_u16_Merged != 0xFF)
             {
                 g_u16_Merged = 0xFF;

                 ST_APPCALL_PROPERTIES st_Properties;
                 static char s_reason[5] = { 0 };
                 sprintf(s_reason, "%d", CMBS_REL_REASON_NORMAL);
                 // disconnecting call
                 st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
                 st_Properties.psz_Value = s_reason;
                 appcall_ReleaseCall(&st_Properties, 1, _appcall_CallObjIdGet(pst_Call), NULL);
             }
#endif // AVS_AUDIO_APP
			 
            break;
        case CMBS_CALL_STATE_STATUS_CALL_HOLD:
            _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_ON_HOLD);
            break;
          case CMBS_CALL_STATE_STATUS_IDLE:
		  	// if the handset mask is 0, then update and send HS use status and line use status on release or release complete.
            if(( st_CallState.u16_HandsetsMask != 0) && 
				((st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_INCOMING) ||
				(st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_OUTGOING)))
            {
                appcall_HSUseChange(pst_Call->u8_LineId, st_CallState.u16_HandsetsMask, u32_CallInstance);
                appcall_RemoveCallFromTable(pst_Call->u8_LineId, pst_Call->u32_CallInstance);            
                appcall_HandleLineUseStatus(pst_Call->u8_LineId);
            }
            break;            
        case CMBS_CALL_STATE_STATUS_CALL_UNDER_TRANSFER:
            if((st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_INCOMING) || (st_CallState.e_CallType == CMBS_CALL_STATE_TYPE_EXT_OUTGOING))
            {
              if(st_CallState.u16_HandsetsMask != 0)
              {
                appcall_HSUseChange(pst_Call->u8_LineId, st_CallState.u16_HandsetsMask, u32_CallInstance);
                // Keep current HS mask for next change
                appcall_SetHSMask(pst_Call->u8_LineId, u32_CallInstance, st_CallState.u16_HandsetsMask);
               }
            }
            break;
            
        default:
            break;
        }

		if(st_DeviceState.u16_DeviceID != 0)
		{
			switch(st_DeviceState.e_CallStatus)
			{
				case CMBS_CALL_STATE_STATUS_IDLE:
					app_RemoveDeviceFromListOfParticipants(pst_Call, st_DeviceState.u16_DeviceID);
					break;

				case CMBS_CALL_STATE_STATUS_CALL_CONNECTED:
				case CMBS_CALL_STATE_STATUS_CONF_CONNECTED:
					app_AddDeviceToListOfParticipants(pst_Call, st_DeviceState.u16_DeviceID);
					break;
					
				default:
					break;
			}
		}
    }
    if ( st_Response.e_Response != CMBS_RESPONSE_OK )
    {
        APP_CALL_WARNING_PRINT("app_OnCallState Recieved response not OK!. Call Instance %d\n", u32_CallInstance);
        return;
    }

    //TODO  change call object state ?
}


//		========== app_OnCallMediaUpdate ===========
/*!
        \brief	 		CMBS Target announce media connectivity information
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnCallMediaUpdate(void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;

    void *pv_IE;
    u16 u16_IE;

    APP_CALL_INFO_PRINT("Media Update\n");

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            if ( u16_IE == CMBS_IE_CALLINSTANCE )
            {
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
            }

            _appmedia_CallObjMediaPropertySet(pst_Call, u16_IE, &st_IEInfo);

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( !pst_Call )
        {
            APP_CALL_ERROR_PRINT("app_OnCallMediaUpdate: ERROR: No Call Instance\n");
            return;
        }

        appmedia_CallObjMediaStart(pst_Call->u32_CallInstance, 0, 0xFF, NULL);
        _appcall_CallObjMediaSet(pst_Call, E_APPCMBS_MEDIA_ACTIVE);

        extvoip_MediaChange(_appcall_CallObjIdGet(pst_Call), pst_Call->e_Codec);
    }
}

//		========== app_OnChannelStartRsp ===========
/*!
        \brief	 		CMBS Target ackknoledge channel start
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnChannelStartRsp(void *pv_List)
{
    u32 u32_CallInstance;
    void *pv_IE;
    u16 u16_IE;

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            // for your convenience, Call Instance IE will allways be the first
            if ( u16_IE == CMBS_IE_CALLINSTANCE )
            {
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
                //            app_CallAnswer( &CmbsApp.u32_AppRef, u32_CallInstance );
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}


//		========== app_OnHsLocProgress ===========
/*!
        \brief	 		CMBS Target signal handset locator progress
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnHsLocProgress(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;

    void *pv_IE;
    u16 u16_IE;
    u8 u8_i;

    u32 u32_CallInstance = 0;
    ST_IE_CALLPROGRESS st_CallProgress;
    ST_IE_CALLEDPARTY st_CalledParty;
    ST_IE_RESPONSE st_Response;

    st_Response.e_Response = CMBS_RESPONSE_OK;
    memset(&st_CallProgress, 0, sizeof(st_CallProgress));
    memset(&st_CalledParty, 0, sizeof(st_CalledParty));

    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALLINSTANCE, CMBS_IE_CALLEDPARTY and CMBS_IE_CALLPROGRESS

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                u32_CallInstance = st_IEInfo.Info.u32_CallInstance;

                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
                break;

            case CMBS_IE_CALLPROGRESS:
                st_CallProgress = st_IEInfo.Info.st_CallProgress;

                if ( pst_Call )
                {
                    switch (st_CallProgress.e_Progress)
                    {
                    case CMBS_CALL_PROGR_PROCEEDING:
                        //                       _appcall_CallObjStateSet (pst_Call, E_APPCMBS_CALL_OUT_PROC );
                        break;
                    case CMBS_CALL_PROGR_RINGING:
                        _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_INC_RING);
                        break;
                    case CMBS_CALL_PROGR_BUSY:
                        break;
                    case CMBS_CALL_PROGR_INBAND:
                        break;
                    default:
                        break;
                    }
                }
                break;

            case CMBS_IE_CALLEDPARTY:
                st_CalledParty = st_IEInfo.Info.st_CalledParty;
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;

            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( st_Response.e_Response != CMBS_RESPONSE_OK )
        {
            APP_CALL_ERROR_PRINT("app_OnHsLocProgress Recieved response not OK!. Call Instance %d\n", u32_CallInstance);
            return;
        }

        //TODO  save st_CalledParty to pst_Call  if need
        APP_CALL_INFO_PRINT("\napp_OnHsLocProgress: CallInstance=0x%08X, CallProgress=%s, CalledParty[ prop=%d, pres=%d, len=%d, addr=",
               u32_CallInstance, cmbs_dbg_GetCallProgressName(st_CallProgress.e_Progress), st_CalledParty.u8_AddressProperties,
               st_CalledParty.u8_AddressPresentation, st_CalledParty.u8_AddressLen);
        for (u8_i = 0; u8_i < st_CalledParty.u8_AddressLen; ++u8_i)
        {
            APP_CALL_INFO_PRINT("%c", st_CalledParty.pu8_Address[u8_i]);
        }
        APP_CALL_INFO_PRINT(" ]\n");
    }
    else
    {
        APP_CALL_ERROR_PRINT("app_OnCallProgress: ERROR: invalid IE list\n");
    }
}



//		========== app_OnHsLocAnswer ===========
/*!
        \brief	 		CMBS Target signal handset locator answer
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnHsLocAnswer(void *pvAppRefHandle, void *pv_List)
{
    ST_APPCMBS_IEINFO st_IEInfo;

    void *pv_IE;
    u16 u16_IE;
    u8 u8_i;

    u32 u32_CallInstance = 0;
    ST_IE_CALLEDPARTY st_CalledParty;
    ST_IE_RESPONSE st_Response;

    st_Response.e_Response = CMBS_RESPONSE_OK;

    memset(&st_CalledParty, 0, sizeof(st_CalledParty));

    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALLINSTANCE and CMBS_IE_CALLEDPARTY

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                u32_CallInstance = st_IEInfo.Info.u32_CallInstance;
                break;

            case CMBS_IE_CALLEDPARTY:
                st_CalledParty = st_IEInfo.Info.st_CalledParty;
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( st_Response.e_Response != CMBS_RESPONSE_OK )
        {
            APP_CALL_ERROR_PRINT("app_OnHsLocAnswer Recieved response not OK!. Call Instance %d\n", u32_CallInstance);
            return;
        }

        //TODO  save st_CalledParty to pst_Call  if need
		APP_CALL_INFO_PRINT("\napp_OnHsLocAnswer: CallInstance=0x%08X, CalledParty[ prop=%d, pres=%d, len=%d, addr=",
        u32_CallInstance, st_CalledParty.u8_AddressProperties,
        st_CalledParty.u8_AddressPresentation, st_CalledParty.u8_AddressLen);
        for (u8_i = 0; u8_i < st_CalledParty.u8_AddressLen; ++u8_i)
        {
            APP_CALL_INFO_PRINT("%c", st_CalledParty.pu8_Address[u8_i]);
        }
        APP_CALL_INFO_PRINT(" ]\n");
    }
    else
    {
        APP_CALL_ERROR_PRINT("app_OnHsLocAnswer: ERROR: invalid IE list\n");
    }
}


//		========== app_OnHsLocRelease ===========
/*!
        \brief	 		CMBS Target signal handset locator release
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnHsLocRelease(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;

    void *pv_IE;
    u16 u16_IE;
    u32 u32_CallInstance = 0;
    ST_IE_RESPONSE st_Response;

    st_Response.e_Response = CMBS_RESPONSE_OK;

    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALLINSTANCE

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                u32_CallInstance = st_IEInfo.Info.u32_CallInstance;
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( st_Response.e_Response != CMBS_RESPONSE_OK )
        {
            APP_CALL_ERROR_PRINT("app_OnHsLocProgress Received response not OK!. Call Instance %d\n", u32_CallInstance);
            return;
        }

        APP_CALL_ERROR_PRINT("\napp_OnHsLocRelease: CallInstance=0x%08X\n", u32_CallInstance);
    }
    else
    {
        APP_CALL_ERROR_PRINT("app_OnHsLocRelease: ERROR: invalid IE list\n");
    }

    UNUSED_PARAMETER(pst_Call);
}

//		========== app_OnMergeCallsResponse ===========
/*!
        \brief	 		CMBS Target signal merge calls response
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnMergeCallsResponse(void *pvAppRefHandle, void *pv_List)
{
    void *pv_IE;
    u16 u16_IE;
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    ST_IE_RESPONSE st_Response;
	st_Response.e_Response = CMBS_RESPONSE_ERROR;
    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            switch (u16_IE)
            {
            case CMBS_IE_CALLINSTANCE:
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
				break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

	if ( (pst_Call == NULL) || (st_Response.e_Response != CMBS_RESPONSE_OK) )
    {
        APP_CALL_ERROR_PRINT("app_OnMergeCallsResponse Received response not OK!\n");
        return;
    }
    else
    {
        APP_CALL_INFO_PRINT("app_OnMergeCallsResponse Received response : Merged Call id : %d\n", _appcall_CallObjIdGet(pst_Call));

        g_u16_Merge_CallId1 = 0xFF;
        g_u16_Merge_CallId2 = 0xFF;
    }

#ifdef AVS_AUDIO_APP

    printf("app_OnMergeCallsResponse Received response : Merged Call id : %d\n", _appcall_CallObjIdGet(pst_Call));
    g_u16_Merged = _appcall_CallObjIdGet(pst_Call);
#endif //AVS_AUDIO_APP

}

//		========== app_OnChannelStopRes ===========
/*!
        \brief	 		CMBS Target signal channel stop response
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/

void app_OnChannelStopRes(void *pvAppRefHandle, void *pv_List)
{

    ST_IE_RESPONSE st_Response;
    PST_CALL_OBJ pst_Call = NULL;
    u8 u8_Idx;

    st_Response.e_Response = app_ResponseCheck(pv_List);

    u8_Idx = __appcall_FindFirstReleasePendingRequest();

    if ( u8_Idx < MAX_RELEASE_CALL_INST )
    {
        pst_Call = _appcall_CallObjGet(g_CallRelease[u8_Idx].u32_CallInstance, NULL);

        if ( g_CallRelease[u8_Idx].u8_CallReleaseInProgress == TRUE )
        {

            keyb_ReleaseNotify(_appcall_CallObjIdGet(pst_Call));

            if ( g_cmbsappl.n_Token )
            {
                appcmbs_ObjectSignal((void *)&g_CallRelease[u8_Idx].st_Reason, sizeof(ST_IE_RELEASE_REASON), _appcall_CallObjIdGet(pst_Call), CMBS_EV_DEE_CALL_RELEASE);
            }
			
			if ( g_CallRelease[u8_Idx].b_SendReleaseRequest == TRUE )
			{
			    void *pv_RefIEList = cmbs_api_ie_GetList();
				cmbs_api_ie_CallInstanceAdd(pv_RefIEList, g_CallRelease[u8_Idx].u32_CallInstance);
				cmbs_api_ie_CallReleaseReasonAdd(pv_RefIEList, &g_CallRelease[u8_Idx].st_Reason);
				cmbs_dee_CallRelease(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
				g_CallRelease[u8_Idx].b_SendReleaseRequest = FALSE;
				memset(&g_CallRelease[u8_Idx], 0, sizeof(g_CallRelease[u8_Idx]));
				return;
			}
			
            // AG: External VoIP library integration
            // Release VoIP call
            extvoip_DisconnectCall(_appcall_CallObjIdGet(pst_Call));

            cmbsevent_onCallReleased(_appcall_CallObjIdGet(pst_Call));

            // send Release_Complete
            cmbs_dee_CallReleaseComplete(pvAppRefHandle, pst_Call->u32_CallInstance);

            _appcall_CallObjDelete(pst_Call->u32_CallInstance, NULL);

            memset(&g_CallRelease[u8_Idx], 0, sizeof(g_CallRelease[u8_Idx]));
        }
    }
    else
    {
        APP_CALL_INFO_PRINT("No call release in progress \n");

        // nothing to do
    }

    // anyhow notify upper layer
    if ( g_cmbsappl.n_Token )
    {
        appcmbs_ObjectSignal((void *)&st_Response, sizeof(ST_IE_RESPONSE), CMBS_IE_RESPONSE, CMBS_EV_DEM_CHANNEL_STOP_RES);
    }
}

//		========== app_OnEmergencyCallReleaseResponse ===========
/*!
        \brief	 		CMBS Target signal merge calls response
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void        app_OnEmergencyCallReleaseResponse( void * pvAppRefHandle,void * pv_List )
{   
    void *         pv_IE;
    u16            u16_IE;
    PST_CALL_OBJ   pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    ST_IE_RESPONSE st_Response;

    st_Response.e_Response = CMBS_RESPONSE_ERROR;


    UNUSED_PARAMETER(pvAppRefHandle);

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst( pv_List, &pv_IE, &u16_IE );
        while ( pv_IE != NULL )
        {
            appcmbs_IEInfoGet ( pv_IE, u16_IE, &st_IEInfo );
            switch ( u16_IE )
            {
            case CMBS_IE_CALLINSTANCE:   
                pst_Call = _appcall_CallObjGet(st_IEInfo.Info.u32_CallInstance, NULL);
                break;

            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet( pv_IE, &st_Response );
                break;
            }

            cmbs_api_ie_GetNext( pv_List, &pv_IE, &u16_IE );
        }
    }

    if ( (pst_Call == NULL) || ( st_Response.e_Response != CMBS_RESPONSE_OK ) )
    {
        APP_CALL_ERROR_PRINT("app_OnEmergencyCallReleaseResponse Received response not OK!\n");
        return;
    }
    else
    {
    	APP_CALL_INFO_PRINT ("app_OnEmergencyCallReleaseResponse Received response OK! \nReleased Call id : %d\n", _appcall_CallObjIdGet(pst_Call));
    }

}

//		========== app_OnCallInfoRestore ===========
/*!
        \brief	 		CMBS reports all active calls to Host
        \param[in]		pvAppRefHandle   application reference pointer
        \param[in]		pv_List		      IE list pointer
        \return	 		<none>
*/
void app_OnCallInfoRestore(void *pvAppRefHandle, void *pv_List)
{
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
    void *pv_IE;
    u16 u16_IE;
	PST_IE_CALL_LIST pst_callList = NULL;
	
    //u8             u8_i;
    ST_IE_RESPONSE st_Response;
	
    st_Response.e_Response = CMBS_RESPONSE_OK;

    memset(&st_IEInfo, 0, sizeof(st_IEInfo));

    UNUSED_PARAMETER(pvAppRefHandle);
    APP_CALL_INFO_PRINT("%s\n", __FUNCTION__);

    if ( pv_List )
    {
        // collect information elements. we expect:
        // CMBS_IE_CALL_LIST, CMBS_IE_RESPONSE

        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            switch (u16_IE)
            {
	            case CMBS_IE_CALL_LIST:
					pst_callList = &st_IEInfo.Info.st_CallList;
					pst_Call = _appcall_CallObjGet(pst_callList->u32_CallInstanse, NULL);

					if(pst_callList->u32_CallInstanse != 0 
						&& (pst_callList->u8_CallType == CMBS_CALL_STATE_TYPE_EXT_INCOMING || pst_callList->u8_CallType == CMBS_CALL_STATE_TYPE_EXT_OUTGOING)
						&& pst_Call == NULL)
					{
					    pst_Call = _appcall_CallObjNew();
					    if ( pst_Call )
					    {
					        pst_Call->u32_CallInstance = pst_callList->u32_CallInstanse;
					        pst_Call->u8_LineId = pst_callList->u8_LineId;
					        pst_Call->st_CallerParty.u8_AddressLen = pst_callList->u8_CallerNumberLen;		
							memcpy(pst_Call->st_CallerParty.pu8_Address, pst_callList->u8_CallerParty, pst_Call->st_CallerParty.u8_AddressLen);
					        pst_Call->st_CalledParty.u8_AddressLen = pst_callList->u8_CalledNumberLen;		
							memcpy(pst_Call->st_CalledParty.pu8_Address, pst_callList->u8_CalledParty, pst_Call->st_CalledParty.u8_AddressLen);							
							//pst_Call->u16_HSMask = pst_callList->u16_HSMask;
							pst_Call->u32_ChannelID = pst_callList->u32_ChannelID;
							pst_Call->u32_ChannelParameter = pst_callList->u32_ChannelParameter;
							pst_Call->e_Codec = pst_callList->u8_Codec;
							pst_Call->u8_CodecsLength = pst_callList->u8_CodecsLength;
							memcpy(pst_Call->pu8_CodecsList, pst_callList->pu8_CodecsList, sizeof(pst_Call->pu8_CodecsList));
							switch( pst_callList->u8_Callstatus){
								case CMBS_CALL_STATE_STATUS_IDLE:
									pst_Call->e_Call = E_APPCMBS_CALL_CLOSE;
									break;

								case CMBS_CALL_STATE_STATUS_CALL_PROCEEDING:
									pst_Call->e_Call = E_APPCMBS_CALL_OUT_PEND;
									break;

								case CMBS_CALL_STATE_STATUS_CALL_ALERTING:
									pst_Call->e_Call = E_APPCMBS_CALL_OUT_RING;
									break;

								case CMBS_CALL_STATE_STATUS_CALL_CONNECTED:
									pst_Call->e_Call = E_APPCMBS_CALL_ACTIVE;
									break;

								case CMBS_CALL_STATE_STATUS_CALL_HOLD:
									pst_Call->e_Call = E_APPCMBS_CALL_ON_HOLD;
									break;

								case CMBS_CALL_STATE_STATUS_CONF_CONNECTED:
									pst_Call->e_Call = E_APPCMBS_CALL_CONFERENCE;
									break;					

								default:
									pst_Call->e_Call = E_APPCMBS_CALL_CLOSE;
									break;
							}

							pst_Call->e_Media = pst_callList->u8_Media;
							if(pst_callList->u8_CallType == CMBS_CALL_STATE_TYPE_EXT_INCOMING){
								pst_Call->b_Incoming = 1;
							}

							if(pst_Call->e_Call == E_APPCMBS_CALL_ACTIVE)
							{
							 	appcall_SetActiveCallTable(pst_Call->u8_LineId, pst_Call->u32_CallInstance);
							}

							g_u16_DemoCallId = _appcall_CallObjIdGet(pst_Call);
						
							g_u32_UsedSlots |= pst_Call->u32_ChannelParameter;

						    //appcall_ConferenceCallTableInit();
					    }										
					}
	                break;

	            case CMBS_IE_RESPONSE:
	                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
	                break;
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }

        if ( st_Response.e_Response != CMBS_RESPONSE_OK )
        {
            APP_CALL_ERROR_PRINT("app_OnCallInfoRestore:  Response Error\n");
            return;
        }

        if ( g_cmbsappl.n_Token && pst_callList)
        {
            appcmbs_ObjectSignal((void *)pst_callList,
                                 sizeof(ST_IE_CALL_LIST),
                                 _appcall_CallObjIdGet(pst_Call),
                                 CMBS_EV_DSR_GET_LIST_OF_ACTIVE_CALLS_RES);
        }

    }
    else
    {
		APP_CALL_ERROR_PRINT("app_OnCallInfoRestore: ERROR: invalid IE list\n");
    }
}


void app_OnCallScreening(void *pvAppRefHandle, void *pv_List)
{
    u32 u32_CallInstance = 0;
    void *pv_IE;
    u16 u16_IE;
    u16 u16_Handset = 0;
    PST_CALL_OBJ pst_Call = NULL;
	ST_IE_CALL_SCREENING st_CallScreening;
	ST_APPCALL_PROPERTIES st_Properties;
	u16 u16_CallId;
    ST_APPCMBS_IEINFO st_IEInfo;

    memset(&st_CallScreening, 0x0, sizeof(st_CallScreening)); /* Coverity warning fix*/

    if ( pvAppRefHandle )
    {};

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            if ( CMBS_IE_CALLINSTANCE == u16_IE )
            {
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
                pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
            }
            else if ( CMBS_IE_HANDSETS == u16_IE )
            {
                cmbs_api_ie_HandsetsGet(pv_IE, &u16_Handset);
            }
            else if ( CMBS_IE_CALL_SCREENING == u16_IE )
            {
                cmbs_api_ie_CallScreeningGet(pv_IE, &st_CallScreening);
            }
            else if ( CMBS_IE_CALLEDPARTY == u16_IE )
            {
                if ( pst_Call )
                {
                    memcpy(pst_Call->ch_CalledID, st_IEInfo.Info.st_CalledParty.pu8_Address, st_IEInfo.Info.st_CalledParty.u8_AddressLen);
                    memcpy(&pst_Call->st_CalledParty, &st_IEInfo.Info.st_CalledParty, sizeof(pst_Call->st_CalledParty));
                    pst_Call->st_CalledParty.pu8_Address = (u8 *)pst_Call->ch_CalledID;
                    pst_Call->ch_CalledID[pst_Call->st_CalledParty.u8_AddressLen] = '\0';
                }
            }

            _appmedia_CallObjMediaPropertySet(pst_Call, u16_IE, &st_IEInfo);


            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

	if ( pst_Call )
	{
		u16_CallId = _appcall_CallObjIdGet(pst_Call);

	    APP_CALL_INFO_PRINT("app_OnCallScreening: Call Instance = %x, HS-Mask = 0x%X\n", u32_CallInstance, u16_Handset);
	    APP_CALL_INFO_PRINT("\nScreening : ");
	    if (st_CallScreening.eScreen == E_CMBS_CALL_SCREENING_ACCEPT)
	    {
	        APP_CALL_INFO_PRINT("Accept\n");

			_appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_SCREENING);
			
			if ( g_call_automat )
			{
				appmedia_CallObjMediaStart(pst_Call->u32_CallInstance, 0, 0xFF, NULL);
			}

			st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
			st_Properties.psz_Value = "CMBS_CALL_PROGR_SCREEN_CONNECT\0";
			appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);
	    }
	    else
	    {
	        APP_CALL_WARNING_PRINT("Intercept\n");

			st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
			st_Properties.psz_Value = "CMBS_CALL_PROGR_CONNECT\0";
			appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);

			_appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_ACTIVE);
	    }
	}
}


void app_OnCallDeflection(void *pvAppRefHandle, void *pv_List)
{
    u32 u32_CallInstance = 0;
    void *pv_IE;
    u16 u16_IE;
    u16 u16_Handset = 0;
    u32 u32_External = 0;
    PST_CALL_OBJ pst_Call = NULL;
    ST_APPCMBS_IEINFO st_IEInfo;
	char  ch_DeflectionID[CALL_PARTICIPANT_ID_MAX_LEN];

	memset (ch_DeflectionID, 0, CALL_PARTICIPANT_ID_MAX_LEN);

    if ( pvAppRefHandle )
    {};

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);

            if ( CMBS_IE_CALLINSTANCE == u16_IE )
            {
                cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
                pst_Call = _appcall_CallObjGet(u32_CallInstance, NULL);
            }
            else if ( CMBS_IE_INTEGER_VALUE == u16_IE )
            {
                cmbs_api_ie_IntValueGet(pv_IE, &u32_External);
            }
            else if ( CMBS_IE_HANDSETS == u16_IE )
            {
                cmbs_api_ie_HandsetsGet(pv_IE, &u16_Handset);
            }
            else if ( CMBS_IE_CALLINFO == u16_IE )
            {
                if ( pst_Call )
                {
					memcpy(ch_DeflectionID,
						   st_IEInfo.Info.st_CallInfo.pu8_Info,
						   st_IEInfo.Info.st_CallInfo.u8_DataLen);
                	APP_CALL_TRACE_PRINT ("\n\n \t ***** Call Deflection from Handset %d to %s %s ***** \n\n", u16_Handset, u32_External? "Externl Number" : "Internal HS", ch_DeflectionID);
                }
            }

            _appmedia_CallObjMediaPropertySet(pst_Call, u16_IE, &st_IEInfo);


            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

	if (pst_Call)
	{
		ST_APPCALL_PROPERTIES st_Properties;
		static char s_reason[5] = { 0 };

		if (!u32_External)
		{
			ST_APPCALL_PROPERTIES st_Properties[6];
			int n_Prop = 0;
			char ch_cld[4];
			char ch_clineid[] = "0";
			char s_codecs[] = "5,6"; //WB and NB
			char ch_melody[] = "3";


			ch_cld[0] = 'h';
			ch_cld[1] = ch_DeflectionID[0];
			ch_cld[2] = '\0';

			ch_clineid[0] = pst_Call->u8_LineId + '0';

			st_Properties[n_Prop].e_IE = CMBS_IE_CALLERPARTY;
			st_Properties[n_Prop++].psz_Value = pst_Call->ch_CallerID;
			st_Properties[n_Prop].e_IE = CMBS_IE_CALLEDPARTY;
			st_Properties[n_Prop++].psz_Value = ch_cld;
			st_Properties[n_Prop].e_IE = CMBS_IE_MEDIADESCRIPTOR;
			st_Properties[n_Prop++].psz_Value = s_codecs;
			st_Properties[n_Prop].e_IE = CMBS_IE_LINE_ID;
			st_Properties[n_Prop++].psz_Value = ch_clineid;
			st_Properties[n_Prop].e_IE = CMBS_IE_CALLERNAME;
			st_Properties[n_Prop++].psz_Value = "Deflection";
			st_Properties[n_Prop].e_IE = CMBS_IE_MELODY;
			st_Properties[n_Prop++].psz_Value = ch_melody;

			appcall_EstablishCall(st_Properties, n_Prop);
		}

        cmbs_dee_CallDeflectionRes(pvAppRefHandle, u32_CallInstance, u16_Handset, CMBS_RESPONSE_OK);

		sprintf(s_reason, "%d", CMBS_REL_REASON_NORMAL);
		// disconnecting call
		st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
		st_Properties.psz_Value = s_reason;
		APP_CALL_INFO_PRINT("app_OnCallDeflection:  releasing call.\n");
		appcall_ReleaseCall(&st_Properties, 1, _appcall_CallObjIdGet(pst_Call), NULL);
	}
	else
	{
        cmbs_dee_CallDeflectionRes(pvAppRefHandle, u32_CallInstance, u16_Handset, CMBS_RESPONSE_ERROR);
	}

}


//		========== _appcall_CallerIDSet ===========
/*!
        \brief				 convert caller ID from string to IE structure

        \param[in,out]		 pst_This		   pointer to call object
        \param[in]        		 pst_Properties   pointer to exchange object
        \param[in]		           n_properties     number of containing IEs in exchange object
        \param[out]		      pst_CallerParty	pointer to IE structure
        \return				      <int>                return TRUE, if string was converted

*/
int _appcall_CallerIDSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_CALLERPARTY pst_CallerParty)
{
    int n_Pos;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_CALLERPARTY);
    memset(pst_Call->ch_CallerID, 0, sizeof(pst_Call->ch_CallerID));
    if ( n_Pos < n_Properties )
    {
        strncpy(pst_Call->ch_CallerID, pst_Properties[n_Pos].psz_Value, (sizeof(pst_Call->ch_CallerID) - 1));
        pst_CallerParty->u8_AddressProperties = CMBS_ADDR_PROPTYPE_INTERNATIONAL | CMBS_ADDR_PROPPLAN_E164;

        if ( pst_Call->ch_CallerID[0] == 'p' || pst_Call->ch_CallerID[0] == 'r' )
        {
            /* long format of CLIP: <p/r><CLI> */
            pst_CallerParty->u8_AddressPresentation = pst_Call->ch_CallerID[0] == 'p' ?
               CMBS_ADDR_PRESENT_ALLOW : CMBS_ADDR_PRESENT_DENIED;
            pst_CallerParty->u8_AddressLen = (u8)strlen(pst_Call->ch_CallerID) - 1;
            pst_CallerParty->pu8_Address = (u8 *)(pst_Call->ch_CallerID + 1);
        }
        else if ( pst_Call->ch_CallerID[0] == 'n' )
        {
            /* format of CLIP: <n> */
            pst_CallerParty->u8_AddressPresentation = CMBS_ADDR_PRESENT_NOTAVAIL;
            pst_CallerParty->u8_AddressLen = 0;
        }
        else if ( pst_Call->ch_CallerID[0] == 'c' )
        {
            /* format of CLIP: <c> */
            /* Calling Party Name from Contact list, Number from Network */
            /* Screening indicator for CNIP: User defined, verified and passed, 
             * CLIP allowed as well (must be available in that case). */
            pst_CallerParty->u8_AddressPresentation =
               CMBS_ADDR_PRESENT_ALLOW | CMBS_ADDR_PRESENT_CNIP_USER;
            pst_CallerParty->u8_AddressLen = (u8)strlen(pst_Call->ch_CallerID) - 1;
            pst_CallerParty->pu8_Address = (u8 *)(pst_Call->ch_CallerID + 1);
        }
        else if ( pst_Call->ch_CallerID[0] == 't' )
        {
            pst_CallerParty->u8_AddressProperties = CMBS_ADDR_PROPTYPE_NATIONAL | CMBS_ADDR_PROPPLAN_PRIVATE;
            /* long format of CLIP: <p/r><CLI> */
            pst_CallerParty->u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
            pst_CallerParty->u8_AddressLen = (u8)strlen(pst_Call->ch_CallerID) - 1;
            pst_CallerParty->pu8_Address = (u8 *)(pst_Call->ch_CallerID + 1);
        }
        else if ( pst_Call->ch_CallerID[0] == 's' )
        {
            pst_CallerParty->u8_AddressProperties = CMBS_ADDR_PROPTYPE_NATIONAL | CMBS_ADDR_PROPPLAN_NAT_STD;
            /* long format of CLIP: <p/r><CLI> */
            pst_CallerParty->u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
            pst_CallerParty->u8_AddressLen = (u8)strlen(pst_Call->ch_CallerID) - 1;
            pst_CallerParty->pu8_Address = (u8 *)(pst_Call->ch_CallerID + 1);
        }
        else
        {
            /* short format of CLIP: <CLI> */
            pst_CallerParty->u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
            pst_CallerParty->u8_AddressLen = (u8)strlen(pst_Call->ch_CallerID);
            pst_CallerParty->pu8_Address = (u8 *)(pst_Call->ch_CallerID);
        }


        memcpy(&pst_Call->st_CallerParty, pst_CallerParty, sizeof(pst_Call->st_CallerParty));
        pst_Call->st_CallerParty.pu8_Address = (u8 *)pst_Call->ch_CallerID;
        pst_Call->st_CallerParty.u8_AddressLen = (u8)strlen(pst_Call->ch_CallerID);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_LineIDSet ===========
/*!
        \brief				 convert Line ID from string to IE structure

        \param[in,out]		 pst_This		   pointer to call object
        \param[in]		           pst_Properties   pointer to exchange object
        \param[in]		           n_properties     number of containing IEs in exchange object
        \param[out]		      pu8_LineId     	pointer to Line Id
        \return				      <int>                return TRUE, if string was converted

*/
int _appcall_LineIDSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u8 *pu8_LineId)
{
    int n_Pos;
    char *psz_Value;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_LINE_ID);

    if ( n_Pos < n_Properties )
    {
        psz_Value = pst_Properties[n_Pos].psz_Value;

        *pu8_LineId = atoi(psz_Value);
        if ( *pu8_LineId >= APPCALL_LINEOBJ_MAX )
        {
            APP_CALL_WARNING_PRINT("APP_WARNING: Line ID is out of range, and reset to O\n");
            *pu8_LineId = 0;
        }

        _appcall_CallObjLineObjSet(pst_Call, _appcall_LineObjGet(*pu8_LineId));

        return TRUE;
    }

    return FALSE;
}



//		========== _appcall_CalledIDSet ===========
/*!
        \brief				 convert caller ID from string to IE structure

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pst_CalledParty	pointer to IE structure
        \return			      <int>               return TRUE, if string was converted

*/
int _appcall_CalledIDSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_CALLEDPARTY pst_CalledParty)
{
    int n_Pos;
    char *psz_Value;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_CALLEDPARTY);
    memset(pst_Call->ch_CalledID, 0, sizeof(pst_Call->ch_CalledID));
    if ( n_Pos < n_Properties )
    {
        psz_Value = pst_Properties[n_Pos].psz_Value;
        strncpy(pst_Call->ch_CalledID, psz_Value, (sizeof(pst_Call->ch_CalledID) - 1));

        if ( (psz_Value[0] == 'h') || (psz_Value[0] == 'd') )
        {
            pst_CalledParty->u8_AddressProperties = CMBS_ADDR_PROPTYPE_UNKNOWN | CMBS_ADDR_PROPPLAN_INTHS;
            pst_CalledParty->u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
            // 'h' of handsets should be eliminated, if exists
            if ( psz_Value[0] == 'h' )
            {
                pst_CalledParty->u8_AddressLen = (u8)strlen(psz_Value) - 1;
                pst_CalledParty->pu8_Address = (u8 *)(psz_Value + 1);
            }
            else
            {
                pst_CalledParty->u8_AddressLen = (u8)strlen(psz_Value);
                pst_CalledParty->pu8_Address = (u8 *)(psz_Value);
            }
        }
        else if ( psz_Value[0] == 'l' )
        {
            pst_CalledParty->u8_AddressProperties = CMBS_ADDR_PROPTYPE_UNKNOWN | CMBS_ADDR_PROPPLAN_INTLINE;
            pst_CalledParty->u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
            pst_CalledParty->u8_AddressLen = 1;
            pst_CalledParty->pu8_Address = (u8 *)(psz_Value + 1);
        }

        memcpy(&pst_Call->st_CalledParty, pst_CalledParty, sizeof(pst_Call->st_CalledParty));
        pst_Call->st_CalledParty.pu8_Address = (u8 *)pst_Call->ch_CalledID;
        pst_Call->st_CalledParty.u8_AddressLen = (u8)strlen(pst_Call->ch_CalledID);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_MediaDescriptorSet ===========
/*!
        \brief				 convert media descriptor from string to IE structure

        \param[in,out]	 pst_This		     pointer to call object
        \param[in]		      pst_Properties     pointer to exchange object
        \param[in]		      n_properties       number of containing IEs in exchange object
        \param[out]		 pst_MediaDescr	pointer to IE structure
        \return			      <int>                 return TRUE, if string was converted

*/
int _appcall_MediaDescriptorSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_MEDIA_DESCRIPTOR pst_MediaDescr)
{
    int n_Pos;
    u32 i;
    int n_CodecPos = 0;
    char s_Codec[4] = { 0 };
    E_CMBS_AUDIO_CODEC e_Codec;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_MEDIADESCRIPTOR);

    pst_MediaDescr->u8_CodecsLength = 0;

    if ( n_Pos >= n_Properties )
        return FALSE;

    // parse codecs string with codecs numbers (format: "1,2,3")
    for (i = 0; i <= strlen(pst_Properties[n_Pos].psz_Value); i++)
    {
        if ( pst_Properties[n_Pos].psz_Value[i] == ',' || i == strlen(pst_Properties[n_Pos].psz_Value) )
        {
            s_Codec[n_CodecPos] = '\0';
            e_Codec = atoi(s_Codec);
            s_Codec[0] = '\0';
            n_CodecPos = 0;

            if ( e_Codec > CMBS_AUDIO_CODEC_UNDEF && e_Codec < CMBS_AUDIO_CODEC_MAX )
                pst_MediaDescr->pu8_CodecsList[pst_MediaDescr->u8_CodecsLength++] = e_Codec;
        }
        else
            s_Codec[n_CodecPos++] = pst_Properties[n_Pos].psz_Value[i];
    }

    // check valid first codec
    if ( !pst_MediaDescr->pu8_CodecsList[0] )
        pst_MediaDescr->pu8_CodecsList[0] = CMBS_AUDIO_CODEC_PCM_LINEAR_NB;

    // set first codec as default for the call
    pst_Call->e_Codec = pst_MediaDescr->pu8_CodecsList[0];
    pst_MediaDescr->e_Codec = pst_Call->e_Codec;

    return TRUE;

}

//		========== _appcall_CallerNameSet ===========
/*!
        \brief				 convert caller name from string to IE structure

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pst_CallerName	pointer to IE structure
        \return			      <int>                 return TRUE, if string was converted

*/
int _appcall_CallerNameSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_CALLERNAME pst_CallerName)
{
    int n_Pos;

    if ( pst_Call )
    {};

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_CALLERNAME);

    if ( n_Pos < n_Properties )
    {
        pst_CallerName->pu8_Name = (u8 *)pst_Properties[n_Pos].psz_Value;
        pst_CallerName->u8_DataLen = (u8)strlen(pst_Properties[n_Pos].psz_Value);

        return TRUE;
    }

    return FALSE;
}


//		========== _appcall_NBCodecOTASet ===========
/*!
        \brief				 convert NB Codec OTA from string to IE structure

        \param[in]		        pst_Properties   pointer to exchange object
        \param[in]		        n_properties     number of containing IEs in exchange object
        \param[out]		        pst_CallerName	pointer to IE structure
        \return			        <int>                 return TRUE, if string was converted

*/
int _appcall_NBCodecOTASet(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_NB_CODEC_OTA pst_Codec)
{
    int n_Pos;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_NB_CODEC_OTA);

    if ( n_Pos < n_Properties )
    {
        pst_Codec->e_Codec = (E_CMBS_NB_CODEC_OTA_TYPE)atoi(pst_Properties[n_Pos].psz_Value);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_MelodySet ===========
/*!
        \brief				 convert NB Codec OTA from string to IE structure

        \param[in]		        pst_Properties   pointer to exchange object
        \param[in]		        n_properties     number of containing IEs in exchange object
        \param[out]		        pst_CallerName	pointer to IE structure
        \return			        <int>                 return TRUE, if string was converted

*/
int _appcall_MelodySet(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u8 *u8_Melody)
{
    int n_Pos;
    char *psz_Value;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_MELODY);

    if ( n_Pos < n_Properties )
    {
        psz_Value = pst_Properties[n_Pos].psz_Value;

        *u8_Melody = atoi(psz_Value);

        if ( (*u8_Melody > 8) || (*u8_Melody < 1) )
        {
            APP_CALL_WARNING_PRINT("APP_WARNING: Melody is out of range, and reset to 1\n");
            *u8_Melody = 1;
        }

        return TRUE;
    }

    return FALSE;
}


int _appcall_CallDirectionSet(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_CALL_DIRECTION pst_CallDirection)
{
    int n_Pos;
    char *psz_Value;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_CALL_DIRECTION);

    if ( n_Pos < n_Properties )
    {
        psz_Value = pst_Properties[n_Pos].psz_Value;

		pst_CallDirection->e_CallDirection = atoi(psz_Value);

		if (pst_CallDirection->e_CallDirection > 1)
        {
            APP_CALL_WARNING_PRINT("APP_WARNING: Call Direction is out of range, and reset to 1\n");
			pst_CallDirection->e_CallDirection = 1;
        }

        return TRUE;
    }

    return FALSE;
}




//		========== _appcall_ReleaseReasonSet ===========
/*!
        \brief				 convert realease reason from string to IE structure

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pst_Reason	      pointer to IE structure
        \return			      <int>                 return TRUE, if string was converted

*/
int _appcall_ReleaseReasonSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_RELEASE_REASON pst_Reason)
{
    int n_Pos;

    if ( pst_Call )
    {};

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_CALLRELEASE_REASON);

    if ( n_Pos < n_Properties )
    {
        pst_Reason->e_Reason = atoi(pst_Properties[n_Pos].psz_Value);
        pst_Reason->u32_ExtReason = 0;

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_CallProgressSet ===========
/*!
        \brief				 convert call progress from string to IE structure

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pst_CallProgress pointer to IE structure
        \return			      <int>                 return TRUE, if string was converted

*/
int _appcall_CallProgressSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_CALLPROGRESS pst_CallProgress)
{
    int n_Pos;

    UNUSED_PARAMETER(pst_Call);

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_CALLPROGRESS);

    if ( n_Pos < n_Properties )
    {
        pst_CallProgress->e_Progress = cmbs_dbg_String2E_CMBS_CALL_PROGR(pst_Properties[n_Pos].psz_Value);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_CallDisplaySet ===========
/*!
        \brief				 convert caller ID from string to IE structure

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pst_CallInfo     pointer to IE structure
        \return			      <int>               return TRUE, if string was converted

*/
int _appcall_CallDisplaySet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_CALLINFO pst_CallInfo)
{
    int n_Pos;

    UNUSED_PARAMETER(pst_Call);

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_CALLINFO);

    if ( n_Pos < n_Properties )
    {
        pst_CallInfo->e_Type = CMBS_CALL_INFO_TYPE_DISPLAY;
        pst_CallInfo->pu8_Info = (u8 *)pst_Properties[n_Pos].psz_Value;
        // take care on max string length according DECT specification
        pst_CallInfo->u8_DataLen = (u8)strlen(pst_Properties[n_Pos].psz_Value);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_CallDisplaySet ===========
/*!
        \brief				 convert caller ID from string to IE structure

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pst_CallInfo     pointer to IE structure
        \return			      <int>               return TRUE, if string was converted

*/
int _appcall_DisplayStringSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, PST_IE_DISPLAY_STRING pst_DisplayString)
{
    int n_Pos;

    UNUSED_PARAMETER(pst_Call);

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_DISPLAY_STRING);

    if ( n_Pos < n_Properties )
    {
        pst_DisplayString->pu8_Info = (u8 *)pst_Properties[n_Pos].psz_Value;
        // take care on max string length according DECT specification
        pst_DisplayString->u8_DataLen = (u8)strlen(pst_Properties[n_Pos].psz_Value);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_HandsetsSet ===========
/*!
        \brief				 convert Hs Number from string to u16

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pu16_HsNr     pointer to IE structure
        \return			      <int>               return TRUE, if string was converted

*/
int _appcall_HandsetsSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 *pu16_HsNr)
{
    int n_Pos;

    UNUSED_PARAMETER(pst_Call);

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_HANDSETS);

    if ( n_Pos < n_Properties )
    {
        *pu16_HsNr = atoi(pst_Properties[n_Pos].psz_Value);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_HanDeviceSet ===========
/*!
        \brief				 convert Device Id from string to u16

        \param[in,out]	 pst_This		   pointer to call object
        \param[in]		      pst_Properties   pointer to exchange object
        \param[in]		      n_properties     number of containing IEs in exchange object
        \param[out]		 pu16_HsNr     pointer to IE structure
        \return			      <int>               return TRUE, if string was converted

*/
int _appcall_HanDeviceSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 *pu16_DeviceIdr)
{
    int n_Pos;

    UNUSED_PARAMETER(pst_Call);

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_HAN_DEVICE);

    if ( n_Pos < n_Properties )
    {
        *pu16_DeviceIdr = atoi(pst_Properties[n_Pos].psz_Value);

        return TRUE;
    }

    return FALSE;
}

//		========== _appcall_LineSubTypeSet ===========
/*!
        \brief				 convert Line Sub Type from string to IE structure

        \param[in,out]		 pst_This		      pointer to call object
        \param[in]		        pst_Properties     pointer to exchange object
        \param[in]		        n_properties        number of containing IEs in exchange object
        \param[out]		        pu8_LineType      pointer to Line Sub Type
        \return				 <int>                  return TRUE, if string was converted

*/
int _appcall_LineSubTypeSet(PST_CALL_OBJ pst_Call, PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u8 *pu8_LineSubType)
{
    int n_Pos;
    char *psz_Value;

    n_Pos = _appcall_PropertiesIDXGet(pst_Properties, n_Properties, CMBS_IE_LINE_SUBTYPE);

    if ( n_Pos < n_Properties )
    {
        psz_Value = pst_Properties[n_Pos].psz_Value;

        *pu8_LineSubType = atoi(psz_Value);
        if ( *pu8_LineSubType >= CMBS_LINE_SUBTYPE_INVALID)
        {
            APP_CALL_WARNING_PRINT("APP_WARNING: Line Sub Type is out of range\n");
            return FALSE;
        }

        _appcall_CallObjLineSubTypeSet(pst_Call, *pu8_LineSubType);

        return TRUE;
    }

    return FALSE;
}

#ifdef AVS_AUDIO_APP

static void app_ReleaseCall(u16 u16_CallID, E_CMBS_REL_REASON reason)
{
    ST_APPCALL_PROPERTIES st_Properties;

    memset(&st_Properties, 0, sizeof(st_Properties));

    char s_reason[5] = { 0 };
    sprintf(s_reason, "%d", reason);
    st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
    st_Properties.psz_Value = s_reason;
    APP_CALL_INFO_PRINT("OnAudioStop: Release Call ID 0\n");
    appcall_ReleaseCall(&st_Properties, 1, u16_CallID, NULL);
}

void app_AudioTimeoutHandler(void)
{
    // cehck if timeout is initialized

    if (rel_after_sec > 0)
    {
        APP_CALL_INFO_PRINT("Call release timeout: start %lu now %lu\n", rel_after_sec, time(NULL));
        if (time(NULL) - rel_after_sec >= avs_call_release_timeout)
        {
            // release call now
            app_ReleaseCall(0, CMBS_REL_REASON_NORMAL);
            rel_after_sec = 0;
        }
    }
}

static void OnAudioStart(const u8 *pu8_DevUnitID)
{
    ST_APPCALL_PROPERTIES st_Properties;

    memset(&st_Properties, 0, sizeof(st_Properties));
    st_Properties.e_IE = CMBS_IE_DISPLAY_STRING;
    st_Properties.psz_Value = "*";

    // disable timer
    rel_after_sec = 0;

    appcall_DisplayString(&st_Properties, 1, 0, NULL);
}

static void OnAudioStop(const u8 *pu8_DevUnitID)
{
    ST_APPCALL_PROPERTIES st_Properties;

    memset(&st_Properties, 0, sizeof(st_Properties));

    if (avs_call_release)
    {
        if (avs_call_release_timeout > 0)

            // disconnect after timeout
            rel_after_sec = time(NULL);
        else
        // release call now
            app_ReleaseCall(0, CMBS_REL_REASON_NORMAL);
    }
    else
    {
        // send Stop Event
        st_Properties.e_IE = CMBS_IE_DISPLAY_STRING;
        st_Properties.psz_Value = "#";

        appcall_DisplayString(&st_Properties, 1, 0, NULL);

    }
}

static void OnAudioProgress(const u8 *pu8_DevUnitID)
{
    printf("OnAudioProgress\n");
    // disable timer only
    rel_after_sec = 0;
}

static void OnAudioQueryEnd(const u8 *pu8_DevUnitID)
{
    ST_APPCALL_PROPERTIES st_Properties;

    memset(&st_Properties, 0, sizeof(st_Properties));

    // send Stop Event
    st_Properties.e_IE = CMBS_IE_DISPLAY_STRING;
    st_Properties.psz_Value = "q";

    appcall_DisplayString(&st_Properties, 1, 0, NULL);

    if (avs_call_release)
    {
        if (avs_call_release_timeout > 0)

            // disconnect after timeout
            rel_after_sec = time(NULL);
    }
}

static const stAudioMgmtCbks st_AudioMgmtCbks = 
{
    OnAudioStart,
    OnAudioStop,
    OnAudioQueryEnd,
    OnAudioProgress
};

static void AudioApp_MakeCall(const u8 *pu8_SrcDevUnitID, const u8 *pu8_DectDevUnitID)
{
    APP_CALL_INFO_PRINT("Make Call from \'%s\' to \'%s\'\n", pu8_SrcDevUnitID, pu8_DectDevUnitID);
}

static const stCallMgmtCbks st_CallMgmtCbks = 
{
    AudioApp_MakeCall
};


static int audio_init;


#endif // AVS_AUDIO_APP

//		========== app_CallEntity ===========
/*!
        \brief			 dispatcher fo call CMBS events

        \param[in]		 pv_AppRef		 application reference pointer
        \param[in]		 e_EventID		 received CMBS event
        \param[in]		 pv_EventData	 pointer to IE list
        \return			 <int>   TRUE, if consumed

*/
int app_CallEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData)
{
#ifdef AVS_AUDIO_APP

    if (!audio_init)
    {
        audio_init = 1;
        p_AudioApp_RegAudioMgmtIface(&st_AudioMgmtCbks);
        p_AudioApp_RegMakeCallCallback(&st_CallMgmtCbks);
    }

#endif // AVS_AUDIO_APP

    switch (e_EventID)
    {
    case CMBS_EV_DEE_CALL_ESTABLISH:
        app_OnCallEstablish(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_PROGRESS:
        app_OnCallProgress(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_INBANDINFO:
        app_OnCallInbandInfo(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_ANSWER:
        app_OnCallAnswer(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_RELEASE:
        if(successfullyReleased != 0) successfullyReleased();
        app_OnCallRelease(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_RELEASECOMPLETE:
        if(successfullyReleased != 0) successfullyReleased();
        app_OnCallReleaseComplete(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_MEDIA_UPDATE:
        app_OnCallMediaUpdate(pv_EventData);
        return TRUE;

    case CMBS_EV_DEM_CHANNEL_START_RES:
        if(successfullyConnected != 0) successfullyConnected(u8_callId);
        app_OnChannelStartRsp(pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_HOLD:
        app_OnCallHold(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_RESUME:
        app_OnCallResume(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DCM_CALL_STATE:
        app_OnCallState(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DCM_CALL_TRANSFER:
        app_OnCallTransfer(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DCM_INTERNAL_TRANSFER:
        app_OnInternalCallTransfer(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DCM_CALL_CONFERENCE:
        app_OnCallConference(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_MEDIA_OFFER_RES:
        app_OnMediaOfferRes(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_HS_CODEC_CFM_FAILED:
        app_OnHsCodecCfmFailed(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEM_CHANNEL_STOP_RES:
        app_OnChannelStopRes(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_MERGE_CALLS_RES:
        app_OnMergeCallsResponse(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DEE_CALL_EMERGENCY_RELEASE_RES:
        app_OnEmergencyCallReleaseResponse ( pv_AppRef, pv_EventData );
        return TRUE;

	case CMBS_EV_DSR_GET_LIST_OF_ACTIVE_CALLS_RES:
		app_OnCallInfoRestore(pv_AppRef, pv_EventData);
		return TRUE;

    case CMBS_EV_DEE_CALL_SCREENING:
        app_OnCallScreening(pv_AppRef, pv_EventData);
		return TRUE;

    case CMBS_EV_DEE_CALL_DEFLECTION:
        app_OnCallDeflection(pv_AppRef, pv_EventData);
		return TRUE;

    default:
        return FALSE;
    }

    return FALSE;
}

//		========== appcall_ReleaseCall ===========
/*!
        \brief			release call identified by Call ID or caller party
        \param[in]		pst_Properties	pointer to exchange object
        \param[in]		n_Properties		number of containing IEs
        \param[in]		u16_CallId		      relevant Call ID
        \param[in]		psz_CLI		      Caller Party string
        \return			<int>             TRUE, if successful
*/
int appcall_ReleaseCall(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;

    if ( !psz_CLI )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_CLI);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = NULL;

        ST_IE_RELEASE_REASON st_Reason;

        u16 u16_HsNr;

        _appcall_ReleaseReasonSet(pst_Call, pst_Properties, n_Properties, &st_Reason);

        if ( pst_Call && (E_APPCMBS_MEDIA_ACTIVE == _appcall_CallObjMediaGet(pst_Call)) )
        {
            appmedia_CallObjMediaStop(pst_Call->u32_CallInstance, 0, NULL);

            if ( pst_Call->u8_LineId == APPCALL_NO_LINE ) 
            {
                u8 u8_idx = __appcall_FindFreeReleasePendingRequest();
                g_CallRelease[u8_idx].u8_CallReleaseInProgress = TRUE;
                g_CallRelease[u8_idx].u32_CallInstance = pst_Call->u32_CallInstance;
                g_CallRelease[u8_idx].b_SendReleaseRequest = TRUE;
                memcpy(&g_CallRelease[u8_idx].st_Reason, &st_Reason, sizeof(ST_IE_RELEASE_REASON));
                return TRUE;
            }
        }
		
        cmbsevent_onCallReleased(_appcall_CallObjIdGet(pst_Call));
        pv_RefIEList = cmbs_api_ie_GetList();
		
        if ( pv_RefIEList )
        {
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            // Add release reason IE
            cmbs_api_ie_CallReleaseReasonAdd(pv_RefIEList, &st_Reason);

            // Add handsets /device  IE
            if ( _appcall_HandsetsSet(pst_Call, pst_Properties, n_Properties, &u16_HsNr) )
            {
                cmbs_api_ie_HandsetsAdd(pv_RefIEList, u16_HsNr);
            }
            else if ( _appcall_HanDeviceSet(pst_Call, pst_Properties, n_Properties, &u16_HsNr) )
            {
                cmbs_api_ie_HANDeviceAdd(pv_RefIEList, u16_HsNr);
            }

            cmbs_dee_CallRelease(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            if ( pst_Call->e_Call == E_APPCMBS_CALL_CONFERENCE )
            {
                // When one party of the conference is released, need to update the other call state
                PST_CALL_OBJ pst_ConfPartnerCall;
                pst_ConfPartnerCall = _appcall_CallObjGet(appcall_ConferencePartnerGet(pst_Call->u32_CallInstance), NULL);
                if ( pst_ConfPartnerCall == NULL )
                    APP_CALL_ERROR_PRINT("Error! Conference call partner was not found");
                else
                {
                    _appcall_CallObjStateSet(pst_ConfPartnerCall, E_APPCMBS_CALL_ACTIVE);
                    appcall_ConferenceCallTableClearEntry(pst_Call->u32_CallInstance);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

//		========== app_ReleaseSplashRingCall ===========
/*!
        \brief			release call identified by pst_Call
        \ No Media to release in this call
*/

void appcall_ReleaseSplashRingCall(PST_CALL_OBJ pst_Call)
{
    void *pv_RefIEList = NULL;

    ST_IE_RELEASE_REASON st_Reason;

    APP_CALL_INFO_PRINT("Release Splash Ring Call \n");

    st_Reason.e_Reason = CMBS_REL_REASON_NORMAL;
    st_Reason.u32_ExtReason = 0;

    cmbsevent_onCallReleased(_appcall_CallObjIdGet(pst_Call));

    pv_RefIEList = cmbs_api_ie_GetList();
    if ( pv_RefIEList )
    {
        // Add call Instance IE
        cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

        // Add release reason IE
        cmbs_api_ie_CallReleaseReasonAdd(pv_RefIEList, &st_Reason);

        cmbs_dee_CallRelease(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
    }
}

//		========== appcall_EstablishCall ===========
/*!
        \brief			establish incoming call
        \param[in]		pst_Properties	pointer to exchange object
        \param[in]		n_Properties		number of containing IEs
        \return			<u16>                Call ID, or ~0  if failed!
*/
u16 appcall_EstablishCall(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties)
{
    PST_CALL_OBJ pst_Call;
    void *pv_RefIEList = NULL;
    ST_IE_CALLEDPARTY st_CalledParty;
    ST_IE_CALLERPARTY st_CallerParty;
    ST_IE_CALLERNAME st_CallerName;
    ST_IE_MEDIA_DESCRIPTOR st_MediaDescr;
    ST_IE_NB_CODEC_OTA st_NBCodecOTA;
    u8 u8_Melody;
    ST_IE_CALL_DIRECTION st_CallDirection;

    ST_DATE_TIME st_DateTime;
	
    u8 u8_LineId = 0xFF;
    u8 u8_LineSubType=CMBS_LINE_SUBTYPE_EXTERNAL;

	// Fix Date & time for testing
	st_DateTime.e_Coding = CMBS_DATE_TIME;
    st_DateTime.e_Interpretation =CMBS_CURRENT_TIME;
    st_DateTime.u8_Year = 16;
    st_DateTime.u8_Month = 4;
    st_DateTime.u8_Day = 3;
   	st_DateTime.u8_Hours = 9;
	st_DateTime.u8_Mins = 55;
    st_DateTime.u8_Secs = 20;
	st_DateTime.u8_Zone = 8;               
	
    pst_Call = _appcall_CallObjNew();
    if ( !pst_Call )
    {
        return ~0;
    }

    /* Mark call as incoming */
    pst_Call->b_Incoming = TRUE;

    // Create a new Call ID
    pst_Call->u32_CallInstance = cmbs_dee_CallInstanceNew(g_cmbsappl.pv_CMBSRef);

    // Initialize IE List
    pv_RefIEList = cmbs_api_ie_GetList();
    if ( pv_RefIEList )
    {
        // Add call Instance IE
        cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

        // Add Line Id
        _appcall_LineIDSet(pst_Call, pst_Properties, n_Properties, &u8_LineId);
        cmbs_api_ie_LineIdAdd(pv_RefIEList, u8_LineId);

        // Add called ID IE
        _appcall_CalledIDSet(pst_Call, pst_Properties, n_Properties, &st_CalledParty);
        cmbs_api_ie_CalledPartyAdd(pv_RefIEList, &st_CalledParty);

        // Add caller ID IE
        _appcall_CallerIDSet(pst_Call, pst_Properties, n_Properties, &st_CallerParty);
        cmbs_api_ie_CallerPartyAdd(pv_RefIEList, &st_CallerParty);

        // Add caller Name IE
        if ( _appcall_CallerNameSet(pst_Call, pst_Properties, n_Properties, &st_CallerName) )
        {
            cmbs_api_ie_CallerNameAdd(pv_RefIEList, &st_CallerName);
        }

        // Add media descriptor IE
        _appcall_MediaDescriptorSet(pst_Call, pst_Properties, n_Properties, &st_MediaDescr);
        cmbs_api_ie_MediaDescAdd(pv_RefIEList, &st_MediaDescr);

        // NB Codec OTA
        if ( _appcall_NBCodecOTASet(pst_Properties, n_Properties, &st_NBCodecOTA) )
        {
            cmbs_api_ie_NBOTACodecAdd(pv_RefIEList, &st_NBCodecOTA);
        }

        // Alerting pattern OTA
        if ( _appcall_MelodySet(pst_Properties, n_Properties, &u8_Melody) )
        {
            cmbs_api_ie_MelodyAdd(pv_RefIEList, u8_Melody);
        }
		
        // Add Call Direction
        if ( _appcall_CallDirectionSet(pst_Properties, n_Properties, &st_CallDirection))
        {
        	cmbs_api_ie_CallDirectionAdd(pv_RefIEList, &st_CallDirection);
        }

        // Add Date Time
        {
        	cmbs_api_ie_DateTimeAdd(pv_RefIEList, &st_DateTime);
        }

        // Add Line Sub type
        if(_appcall_LineSubTypeSet(pst_Call, pst_Properties, n_Properties, &u8_LineSubType))
        {
        	cmbs_api_ie_LineSubtypeAdd(pv_RefIEList, u8_LineSubType);
        }


        // Establish the call now...
        cmbs_dee_CallEstablish(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

        return _appcall_CallObjIdGet(pst_Call);
    }

    return ~0;
}

u16 appcall_EstablishSplashRing(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u8 u8_SplashTimer)
{
    PST_CALL_OBJ pst_Call;
    void *pv_RefIEList = NULL;
    ST_IE_CALLEDPARTY st_CalledParty;
    ST_IE_CALLERPARTY st_CallerParty;
    ST_IE_CALLERNAME st_CallerName;
    ST_IE_MEDIA_DESCRIPTOR st_MediaDescr;
    ST_IE_NB_CODEC_OTA st_NBCodecOTA;
    u8 u8_Melody;
    u8 u8_LineId = 0xFF;

    pst_Call = _appcall_CallObjNew();
    if ( !pst_Call )
    {
        return ~0;
    }

    /* Mark call as incoming */
    pst_Call->b_Incoming = TRUE;

    // Create a new Call ID
    pst_Call->u32_CallInstance = cmbs_dee_CallInstanceNew(g_cmbsappl.pv_CMBSRef);

    /* Mark call as Splash Ring */
    pst_Call->b_IsSplashRing = TRUE;

    // Initialize IE List
    pv_RefIEList = cmbs_api_ie_GetList();
    if ( pv_RefIEList )
    {
        // Add call Instance IE
        cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

        // Add Line Id
        _appcall_LineIDSet(pst_Call, pst_Properties, n_Properties, &u8_LineId);
        cmbs_api_ie_LineIdAdd(pv_RefIEList, u8_LineId);

        // Add called ID IE
        _appcall_CalledIDSet(pst_Call, pst_Properties, n_Properties, &st_CalledParty);
        cmbs_api_ie_CalledPartyAdd(pv_RefIEList, &st_CalledParty);

        // Add caller ID IE
        _appcall_CallerIDSet(pst_Call, pst_Properties, n_Properties, &st_CallerParty);
        cmbs_api_ie_CallerPartyAdd(pv_RefIEList, &st_CallerParty);

        // Add caller name IE
        if ( _appcall_CallerNameSet(pst_Call, pst_Properties, n_Properties, &st_CallerName) )
        {
            cmbs_api_ie_CallerNameAdd(pv_RefIEList, &st_CallerName);
        }

        // Add media descriptor IE
        _appcall_MediaDescriptorSet(pst_Call, pst_Properties, n_Properties, &st_MediaDescr);
        cmbs_api_ie_MediaDescAdd(pv_RefIEList, &st_MediaDescr);

        // NB Codec OTA
        if ( _appcall_NBCodecOTASet(pst_Properties, n_Properties, &st_NBCodecOTA) )
        {
            cmbs_api_ie_NBOTACodecAdd(pv_RefIEList, &st_NBCodecOTA);
        }

        // Alerting pattern OTA
        if ( _appcall_MelodySet(pst_Properties, n_Properties, &u8_Melody) )
        {
            cmbs_api_ie_MelodyAdd(pv_RefIEList, u8_Melody);
        }

        // Add u8_SplashTimer IE
        cmbs_api_ie_SplashTimerAdd(pv_RefIEList, u8_SplashTimer);

        // Establish the call now...
        cmbs_dee_CallEstablish(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

        return _appcall_CallObjIdGet(pst_Call);
    }

    return ~0;
}
//		========== appcall_ProgressCall ===========
/*!
        \brief			providing progress inforation of call identified by Call ID or caller party
        \param[in]		pst_Properties	pointer to exchange object
        \param[in]		n_Properties		number of containing IEs
        \param[in]		u16_CallId		      relevant Call ID
        \param[in]		psz_CLI		      Caller Party string
        \return			<int>             TRUE, if successful
*/
int appcall_ProgressCall(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;
    ST_IE_CALLPROGRESS st_CallProgress;
    ST_IE_RELEASE_REASON st_Reason;

    if ( !psz_CLI )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_CLI);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            ST_IE_MEDIA_DESCRIPTOR st_MediaDesc;

            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);
            // answer the call: send CMBS_IE_CALLINSTANCE, CMBS_IE_MEDIACHANNEL and CMBS_IE_MEDIADESCRIPTOR

            if ( n_Properties > 1 )
            {
                // Add release reason IE, it's in the second property field
                _appcall_ReleaseReasonSet(pst_Call, pst_Properties, n_Properties, &st_Reason);

                cmbs_api_ie_CallReleaseReasonAdd(pv_RefIEList, &st_Reason);
            }

            _appcall_CallProgressSet(pst_Call, pst_Properties, n_Properties, &st_CallProgress);

            if ( pst_Call->b_CodecsOfferedToTarget == FALSE )
            {
                // add media descriptor IE
                memset(&st_MediaDesc, 0, sizeof(ST_IE_MEDIA_DESCRIPTOR));
                st_MediaDesc.e_Codec = pst_Call->e_Codec;
                cmbs_api_ie_MediaDescAdd(pv_RefIEList, &st_MediaDesc);

                pst_Call->b_CodecsOfferedToTarget = TRUE;
            }

            cmbs_api_ie_CallProgressAdd(pv_RefIEList, &st_CallProgress);

            /* Add Line-Id */
            cmbs_api_ie_LineIdAdd(pv_RefIEList, pst_Call->u8_LineId);

            // APP_CALL_INFO_PRINT(" ... ProgressCall Line ID = %x\n", pst_Call->u8_LineId);


            cmbs_dee_CallProgress(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            switch (st_CallProgress.e_Progress)
            {
            case CMBS_CALL_PROGR_SETUP_ACK:
                _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_OUT_PEND);
                break;
            case CMBS_CALL_PROGR_PROCEEDING:
            case CMBS_CALL_PROGR_PROCEED_EMERGENCY:
			case CMBS_CALL_PROGR_FAX:
                _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_OUT_PROC);
                break;
            case CMBS_CALL_PROGR_RINGING :
                _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_OUT_RING);
                break;
            case CMBS_CALL_PROGR_INBAND:
                _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_OUT_INBAND);
                break;
            default:
                break;
            }
        }
    }

    return TRUE;
}

//		========== appcall_AnswerCall ===========
/*!
        \brief			answer call identified by Call ID or caller party
        \param[in]		pst_Properties	pointer to exchange object
        \param[in]		n_Properties		number of containing IEs
        \param[in]		u16_CallId		      relevant Call ID
        \param[in]		psz_CLI		      Caller Party string
        \return			<int>             TRUE, if successful
*/
int appcall_AnswerCall(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;

    if ( pst_Properties && n_Properties )
    {};

    if ( !psz_CLI )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_CLI);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        cmbsevent_onCallAnswered(_appcall_CallObjIdGet(pst_Call));

        if ( pv_RefIEList )
        {
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);
            // answer the call: send CMBS_IE_CALLINSTANCE, CMBS_IE_MEDIACHANNEL and CMBS_IE_MEDIADESCRIPTOR

            cmbs_dee_CallAnswer(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            _appcall_CallObjStateSet(pst_Call, E_APPCMBS_CALL_ACTIVE);

            if ( g_call_automat )
            {
                appmedia_CallObjMediaStart(pst_Call->u32_CallInstance, 0, 0xFF, NULL);
            }

            return TRUE;
        }
    }

    return FALSE;
}

//		========== appcall_DisplayCall ===========
/*!
        \brief			provide display information of a call identified by Call ID or caller party
        \param[in]		pst_Properties	pointer to exchange object
        \param[in]		n_Properties		number of containing IEs
        \param[in]		u16_CallId		      relevant Call ID
        \param[in]		psz_CLI		      Caller Party string
        \return			<int>             TRUE, if successful
*/
int appcall_DisplayCall(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;

    if ( !psz_CLI )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_CLI);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        APP_CALL_INFO_PRINT("CW Display\n");

        if ( pv_RefIEList )
        {
            ST_IE_CALLINFO st_CallInfo;
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            _appcall_CallDisplaySet(pst_Call, pst_Properties, n_Properties, &st_CallInfo);
            cmbs_api_ie_CallInfoAdd(pv_RefIEList, &st_CallInfo);

            cmbs_dee_CallInbandInfo(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            return TRUE;
        }
    }

    return FALSE;
}

//		========== appcall_DisplayString ===========
/*!
        \brief			provide display information 
        \param[in]		pst_Properties	pointer to exchange object
        \param[in]		n_Properties		number of containing IEs
        \param[in]		u16_CallId		      relevant Call ID
        \param[in]		psz_Display		      Display string
        \return			<int>             TRUE, if successful
*/
int appcall_DisplayString(PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char *psz_Display)
{
    PST_CALL_OBJ pst_Call;

    if ( !psz_Display )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_Display);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        APP_CALL_INFO_PRINT(" Display String\n");

        if ( pv_RefIEList )
        {
            ST_IE_DISPLAY_STRING st_DisplayString;
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            _appcall_DisplayStringSet(pst_Call, pst_Properties, n_Properties, &st_DisplayString);
            cmbs_api_ie_DisplayStringAdd(pv_RefIEList, &st_DisplayString);

            cmbs_dee_CallInbandInfo(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            return TRUE;
        }
    }

    return FALSE;
}

//		========== appcall_CallInbandInfo ===========
/*!
        \brief			Host changes Caller ID (for transfer) of an active call
        \param[in]		u16_CallId		  relevant Call ID
        \param[in]		psz_CLI		      new Caller Party string or CNIP string
        \return			<int>             TRUE, if successful
*/
int appcall_CallInbandInfo(u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call = g_call_obj + u16_CallId;

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            ST_IE_CALLINFO st_CallInfo;
            st_CallInfo.e_Type = CMBS_CALL_INFO_TYPE_DISPLAY;
            st_CallInfo.pu8_Info = (u8 *)psz_CLI;
            st_CallInfo.u8_DataLen = 2 + strlen(psz_CLI + 2);

            cmbs_api_ie_CallInfoAdd(pv_RefIEList, &st_CallInfo);
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            cmbs_dee_CallInbandInfo(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            /* Update Host CLIP */
            if ( g_call_obj[u16_CallId].b_Incoming )
            {
                strcpy(g_call_obj[u16_CallId].ch_CallerID, psz_CLI + 2);
            }
            else
            {
                strcpy(g_call_obj[u16_CallId].ch_CalledID, psz_CLI + 2);
            }

            return TRUE;
        }
    }

    return FALSE;
}


//		========== appcall_CallInbandInfoCNIP ===========
/*!
        \brief			CNIP (matching Contact number) during external outgoing call establishment
        \param[in]		u16_CallId		  relevant Call ID
        \param[in]		psz_CLI		      new Called Party name and first name string
        \return			<int>             TRUE, if successful
*/
int appcall_CallInbandInfoCNIP(u16 u16_CallId, char *pu8_Name, char *pu8_FirstName, char *pch_cli)
{
    PST_CALL_OBJ pst_Call = g_call_obj + u16_CallId;

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            ST_IE_CALLEDNAME st_CalledName;
            ST_IE_CALLERPARTY st_CalledParty;

            if ( pch_cli )
            {
                /* Called Number is available */
                // Add called ID IE
                st_CalledParty.u8_AddressProperties = CMBS_ADDR_PROPTYPE_UNKNOWN | CMBS_ADDR_PROPPLAN_UNKNOWN;
                st_CalledParty.u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
                st_CalledParty.u8_AddressLen = (u8)strlen(pch_cli);
                st_CalledParty.pu8_Address = (u8 *)(pch_cli);

                cmbs_api_ie_CalledPartyAdd(pv_RefIEList, &st_CalledParty);
            }


            st_CalledName.u8_AlphabetAndScreening = *pu8_Name;
            st_CalledName.pu8_Name = (u8 *)++pu8_Name;
            st_CalledName.u8_DataLen = strlen(pu8_Name);
            st_CalledName.u8_DataLenFirst = strlen(pu8_FirstName);
            st_CalledName.pu8_FirstName = (u8 *)pu8_FirstName;

            cmbs_api_ie_CalledNameAdd(pv_RefIEList, &st_CalledName);

            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            cmbs_dee_CallInbandInfo(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            return TRUE;
        }
    }

    return FALSE;
}

//		========== appcall_HoldCall ===========
/*!
        \brief			signal CMBS target call hold, mute media stream
        \param[in]		u16_CallId		      relevant Call ID
        \param[in]		psz_CLI		      Caller Party string
        \return			<int>             TRUE, if successful
*/
int appcall_HoldCall(u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;

    if ( !psz_CLI )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_CLI);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            cmbs_dee_CallHold(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            return TRUE;
        }
    }

    return FALSE;
}

//		========== appcall_ResumeCall ===========
/*!
        \brief			signal CMBS target call resume, unmute media stream
        \param[in]		u16_CallId		      relevant Call ID
        \param[in]		psz_CLI		      Caller Party string
        \return			<int>             TRUE, if successful
*/
int appcall_ResumeCall(u16 u16_CallId, char *psz_CLI)
{
    PST_CALL_OBJ pst_Call;

    if ( !psz_CLI )
    {
        // Call ID is available
        pst_Call = g_call_obj + u16_CallId;
    }
    else
    {
        pst_Call = _appcall_CallObjGet(0, psz_CLI);
    }

    if ( pst_Call )
    {
        void *pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);

            cmbs_dee_CallResume(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            return TRUE;
        }
    }

    return FALSE;
}

//		========== appcall_MergeCalls ===========
/*!
        \brief			signal CMBS target merge two calls
        \param[in]		u16_CallId1		      relevant Call ID1
        \param[in]		u16_CallId2		      relevant Call ID2
        \param[in]		u8_LineId		      relevant Line ID
        \return			<int>             TRUE, if successful
*/
int appcall_MergeCalls(u16 u16_CallId1, u16 u16_CallId2, u8 u8_CovertToInt)
{
	void *pv_RefIEList;
    PST_CALL_OBJ pst_Call1, pst_Call2;

	if (u16_CallId1 == u16_CallId2)
	{
		APP_CALL_WARNING_PRINT("Invalid input: CallId1 is equal CallId2 (%d)\n", u16_CallId1);
		return FALSE;
	}

    pst_Call1 = g_call_obj + u16_CallId1;
    pst_Call2 = g_call_obj + u16_CallId2;

    if ( pst_Call1 && pst_Call2 )
    {	
        pv_RefIEList = cmbs_api_ie_GetList();

        if ( pv_RefIEList )
        {
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call1->u32_CallInstance);
            // Add call Instance IE
            cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call2->u32_CallInstance);
            // Add line Id IE
            cmbs_api_ie_IntValueAdd(pv_RefIEList, u8_CovertToInt);

            cmbs_dee_MergeCalls(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

            return TRUE;
        }
    }

    return FALSE;
}


//		========== appcall_EmergencyCallRelease ===========
/*!
        \brief			signal CMBS target merge two calls
        \param[in]		u8_CallInstance    relevant Call Instance
        \return			<int>             TRUE, if successful
*/
int     appcall_EmergencyCallRelease( u16 u16_CallId )
{
    PST_CALL_OBJ   pst_Call;
    void*  pv_RefIEList = cmbs_api_ie_GetList();
    // get call object
    pst_Call = _appcall_CallObjGetById(u16_CallId);



    if ( pv_RefIEList && pst_Call != NULL)
    {
        // Add line Id IE
        cmbs_api_ie_CallInstanceAdd( pv_RefIEList, pst_Call->u32_CallInstance );

        cmbs_dee_EmergencyCallRelease( g_cmbsappl.pv_CMBSRef, pv_RefIEList );

        return  TRUE;
    }

    return  FALSE;
}

//		========== appcall_EmergencyCallReleaseByCallInstance ===========
/*!
        \brief			signal CMBS target merge two calls
        \param[in]		u8_CallInstance    relevant Call Instance
        \return			<int>             TRUE, if successful
*/
int     appcall_EmergencyCallReleaseByCallInstance( u16 u16_CallInstance )
{
    void*  pv_RefIEList = cmbs_api_ie_GetList();

    if ( pv_RefIEList )
    {
        // Add line Id IE
        cmbs_api_ie_CallInstanceAdd( pv_RefIEList, u16_CallInstance );

        cmbs_dee_EmergencyCallRelease( g_cmbsappl.pv_CMBSRef, pv_RefIEList );

        return  TRUE;
    }

    return  FALSE;
}

void appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE e_Mode)
{
    if ( e_Mode == E_APPCALL_AUTOMAT_MODE_ON )
    {
        APP_CALL_INFO_PRINT("Switch application to automat mode\n");
    }
    else
    {
        APP_CALL_INFO_PRINT("Switch application to step mode\n");
    }

    g_call_automat = e_Mode;
}

// accept presentations templates "1", "3", "h1", "h3"
bool appcall_DestinationIsHs(u8 u8_HsNumber, ST_IE_CALLEDPARTY *pst_Caller)
{
    if ( ((pst_Caller->u8_AddressProperties & CMBS_ADDR_PROPPLAN_MASK) == CMBS_ADDR_PROPPLAN_INTHS) &&
         (pst_Caller->pu8_Address != NULL) )
    {
        if ( pst_Caller->u8_AddressLen == 1 )
        {
            if ( (pst_Caller->pu8_Address[0] - '0') == u8_HsNumber )
            {
                return TRUE;
            }
        }
        else if ( (pst_Caller->u8_AddressLen == 2) && (pst_Caller->pu8_Address[0] == 'h') )
        {
            if ( (pst_Caller->pu8_Address[1] - '0') == u8_HsNumber )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}


u16 appcall_FindHsNumByCallId(u16 u16_CallId)
{
  PST_CALL_OBJ pst_Call;
  pst_Call = _appcall_CallObjGetById(u16_CallId);
 if ( !pst_Call )
    {
        printf("NO SUCH CALL WITH ID %d\n", u16_CallId);
        return (0xFF);
    }
    if(pst_Call->b_Incoming == 0)
    	{//OG call
    	if( pst_Call->st_CallerParty.u8_AddressLen != 0)
	{
		if( pst_Call->st_CallerParty.u8_AddressLen == 1)
			{
				return(pst_Call->st_CallerParty.pu8_Address[0]- '0');
			}else 
			if(( pst_Call->st_CallerParty.u8_AddressLen == 2) && (pst_Call->st_CallerParty.pu8_Address[0] == 'h'))
			{
				return(pst_Call->st_CallerParty.pu8_Address[1]- '0');
			}else
			{
				return(0xff);
			}
			
	}
    	}
	else{//IC Call
	if( pst_Call->st_CalledParty.u8_AddressLen != 0)
	{
		if( pst_Call->st_CalledParty.u8_AddressLen == 1)
			{
				return(pst_Call->st_CalledParty.pu8_Address[0]- '0');
			}else 
			if(( pst_Call->st_CalledParty.u8_AddressLen == 2) && (pst_Call->st_CalledParty.pu8_Address[0] == 'h'))
			{
				return(pst_Call->st_CalledParty.pu8_Address[1]- '0');
			}else
			{
				return(0xff);
			}
			
	}
}
	return(0xff);
}


bool appcall_IsHsInCallWithLine(u8 u8_HsNumber, u8 u8_LineId)
{
    u16 i;

    for (i = 0; i < APPCALL_CALLOBJ_MAX; i++)
    {
        if ( g_call_obj[i].u32_CallInstance )
        {
            // does u8_HsNumber involved in call?
            if ( appcall_DestinationIsHs(u8_HsNumber, &(g_call_obj[i].st_CallerParty)) ||
                 appcall_DestinationIsHs(u8_HsNumber, &(g_call_obj[i].st_CalledParty)) )
            {
                // is this our line?
                if ( g_call_obj[i].u8_LineId == u8_LineId )
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

bool appcall_IsLineInUse(u8 u8_LineId)
{
    int i;
    for (i = 0; i < APPCALL_CALLOBJ_MAX; i++)
    {
        if ( (g_call_obj[i].u32_CallInstance) && (g_call_obj[i].u8_LineId == u8_LineId) )
        {
            return TRUE;
        }
    }
    return FALSE;
}

void appcall_HandleLineUseStatus(u8 u8_LineId)
{
    E_CMBS_LINE_DIAG_STATUS e_LineUse;
    u32 u32_HandsetMask = 0;
    u8 u8_MaxNumOfCalls, u8_NumberOfActiveCalls;
        
    if(List_GetAttachedHs(u8_LineId, &u32_HandsetMask) == LIST_RC_OK)
	{	
	    u8_MaxNumOfCalls = appcall_GetMaximumNumberOfAllowedCalls(u8_LineId);
	    u8_NumberOfActiveCalls = appcall_GetNumOfActiveCallsForLine(u8_LineId);
	    APP_CALL_INFO_PRINT("u8_MaxNumOfCalls = [%d], u8_NumberOfActiveCalls = [%d] \n", u8_MaxNumOfCalls, u8_NumberOfActiveCalls);    

	    if (u8_NumberOfActiveCalls == 0)
	        e_LineUse = CMBS_LINE_DIAG_LINE_IDLE;
	    else if (u8_NumberOfActiveCalls >= u8_MaxNumOfCalls)
	        e_LineUse = CMBS_LINE_DIAG_LINE_SYSTEM_BUSY;
	    else if (u8_NumberOfActiveCalls < u8_MaxNumOfCalls)
	        e_LineUse = CMBS_LINE_DIAG_LINE_IN_USE;
    
	    if(LineAndDiagnosticListUpdateLineUse(u8_LineId, e_LineUse))
	        app_FacilityLineUseIndication(0, u8_LineId, e_LineUse, u32_HandsetMask);
	}
	else
		APP_CALL_ERROR_PRINT("List_GetAttachedHs Failed!\n");
}

void appcall_HandleHSUseStatus(u8 u8_LineId, u16 u16_HsNr, bool b_InUse)
{
    u32 u32_HandsetMask = 0;
    
	List_GetAttachedHs(u8_LineId, &u32_HandsetMask);

    LineAndDiagnosticListUpdateHSUse(u8_LineId, u16_HsNr, b_InUse);
    app_FacilityHSUseIndication(0, u8_LineId, (u16)u32_HandsetMask);
}

u8 appcall_GetMaximumNumberOfAllowedCalls(u8 u8_LineId)
{
    u32 u32_FieldId = 0;
    stLineSettingsListEntry st_Entry;
    u32 u32_EntryId = 0;
    u8 u8_Multiple_Calls = 0;

    memset(&st_Entry, 0, sizeof(stLineSettingsListEntry));        
    
    u32_FieldId = FIELD_ID_MULTIPLE_CALLS_MODE;
    u32_EntryId = LineSettingsGetEntryIdByLineId(u8_LineId);

    List_ReadEntryById(LIST_TYPE_LINE_SETTINGS_LIST, u32_EntryId, &u32_FieldId, 1, &st_Entry);

    u8_Multiple_Calls    = st_Entry.bMultiCalls ? CMBS_MULTICALL_LINE_NUM_OF_CALLS : 1;  

    return u8_Multiple_Calls;
}

// Put call in call table
void appcall_SetActiveCallTable(u8 u8_Line, u32 u32_CallInstanse)
{
    u8 u8_Index;
    //Search for free entry
    for (u8_Index =0; u8_Index < CMBS_MULTICALL_LINE_NUM_OF_CALLS; u8_Index++)
    {
        if (g_ActiveCalls[u8_Line][u8_Index].u32_CallInstanse == 0xFFFFFFFF)
            break;
    }
    g_ActiveCalls[u8_Line][u8_Index].u32_CallInstanse = u32_CallInstanse;
}

// Remove call from the table
void appcall_RemoveCallFromTable(u8 u8_Line, u32 u32_CallInstanse)
{
    u8 u8_index;
    //Search for entry
    for (u8_index =0; u8_index < CMBS_MULTICALL_LINE_NUM_OF_CALLS; u8_index++)
        {
            if(g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse == u32_CallInstanse)
                break;
        }
   g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse = 0xFFFFFFFF;
   g_ActiveCalls[u8_Line][u8_index].u16_HSMask = 0;
}

// Check if call is in the table
bool appcall_IsCallInTable(u8 u8_Line, u32 u32_CallInstanse)
{
    u8 u8_index;
    
    for (u8_index = 0; u8_index < CMBS_MULTICALL_LINE_NUM_OF_CALLS; u8_index++)
    {
        if(g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse == u32_CallInstanse)
            return TRUE;
    }
    return FALSE;
}

u8 appcall_GetNumOfActiveCallsForLine(u8 u8_Line)
{
    u8 u8_index, u8_NumOfActiveCalls = 0;
    
    for (u8_index = 0; u8_index < CMBS_MULTICALL_LINE_NUM_OF_CALLS; u8_index++)
    {
        if (g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse != 0xFFFFFFFF)
            u8_NumOfActiveCalls++;
    }
    return u8_NumOfActiveCalls;
}

bool appcall_CheckHSInOtherCall(u8 u8_Line, u16 u16_HSNum, u32 u32_CallInstance)
{
    u8 u8_index;
    
    for (u8_index = 0; u8_index < CMBS_MULTICALL_LINE_NUM_OF_CALLS; u8_index++)
    {
        if ((g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse != 0xFFFFFFFF) && (g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse != u32_CallInstance))
        {
            //For other active calls check if this HS in a mask
            if (g_ActiveCalls[u8_Line][u8_index].u16_HSMask & (1 << (u16_HSNum -1)))
                return TRUE;
        }
    }
    return FALSE;
}

void appcall_SetHSMask(u8 u8_Line, u32 u32_CallInstanse, u16 u16_HSMask)
{
    u8 u8_index;
    
    for (u8_index = 0; u8_index < CMBS_MULTICALL_LINE_NUM_OF_CALLS; u8_index++)
    {
        if(g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse == u32_CallInstanse)
         {
                g_ActiveCalls[u8_Line][u8_index].u16_HSMask = u16_HSMask;
         }
    }
}

u16 appcall_GetHSMask(u8 u8_Line, u32 u32_CallInstanse)
{
    u8 u8_index;
    
    for (u8_index = 0; u8_index < CMBS_MULTICALL_LINE_NUM_OF_CALLS; u8_index++)
    {
        if(g_ActiveCalls[u8_Line][u8_index].u32_CallInstanse == u32_CallInstanse)
         {
            return g_ActiveCalls[u8_Line][u8_index].u16_HSMask;
         }
    }
	return 0;
}

void appcall_HSUseChange(u8 u8_LineId, u16 u16_HSMask, u32 u32_CallInstance)
{
    u8 u8_i, u8_CangedHSNum;
    bool InUse;
    u16 u16_ChangedHSMask = u16_HSMask ^ appcall_GetHSMask(u8_LineId, u32_CallInstance);     
                                
    for (u8_i = 0; u8_i < 16; ++u8_i)
    {
      if ( u16_ChangedHSMask & (1 << u8_i) )
      {
         //Now we know this HS number status changed. Check if it joined or left
         u8_CangedHSNum = (u8_i + 1);
         if(u16_HSMask & (1 << u8_i) )
           InUse = TRUE; //HS joined
         else
           InUse = FALSE;  //HS left                           
         //Now check if the HS is involved in other call
         if(!appcall_CheckHSInOtherCall(u8_LineId, u8_CangedHSNum, u32_CallInstance))
          {
              //Update data base
           	  appcall_HandleHSUseStatus(u8_LineId, u8_CangedHSNum, InUse);
           }                            
        }
     }
}

void appcall_HsStatusInit(void)
{
	u8 u8_HsNum;

	for (u8_HsNum=0;u8_HsNum<CMBS_HS_SUBSCRIBED_MAX_NUM;u8_HsNum++)
	{
		memset(&st_HsCallState[u8_HsNum][0], 0, sizeof(ST_IE_CALL_STATE));
		st_HsCallState[u8_HsNum][0].u8_ActCallID = 0xFF;
		memset(&st_HsCallState[u8_HsNum][1], 0, sizeof(ST_IE_CALL_STATE));
		st_HsCallState[u8_HsNum][1].u8_ActCallID = 0xFF;

	}
}


void appcall_SetHsStatus( u32 u32_CallInstanse, PST_IE_CALL_STATE st_CallState)
{
u8 u8_HsNum;
if (st_CallState->e_CallStatus == CMBS_CALL_STATE_STATUS_IDLE)
{//HS mask is 0 So remove everything based on CallID
	for (u8_HsNum=0;u8_HsNum<CMBS_HS_SUBSCRIBED_MAX_NUM;u8_HsNum++)
    {
    	//if ((st_CallState->u16_HandsetsMask)&(1<< u8_HsNum))
    	{//HS Matches
    		if(st_HsCallState[u8_HsNum][0].u8_ActCallID == st_CallState->u8_ActCallID)
    		{//Call Id Matches with 0th location
    		 APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][0] for IDLE for HsNo = %x>>>\n",u8_HsNum);
    		 memset(&st_HsCallState[u8_HsNum][0], 0, sizeof(ST_IE_CALL_STATE));
			 st_HsCallState[u8_HsNum][0].u8_ActCallID = 0xFF;
    		}else if (st_HsCallState[u8_HsNum][1].u8_ActCallID == st_CallState->u8_ActCallID)
    		{//Call Id Matches with 1th location
    		  APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][1] for IDLE for HsNo = %x >>>\n",u8_HsNum);
    		 memset(&st_HsCallState[u8_HsNum][1], 0, sizeof(ST_IE_CALL_STATE));
			 st_HsCallState[u8_HsNum][1].u8_ActCallID = 0xFF;
    		}
    	}
    }
}else
{
	//Update the Data
    for (u8_HsNum = 0; u8_HsNum < CMBS_HS_SUBSCRIBED_MAX_NUM; u8_HsNum++)
    {
    	if ((st_CallState->u16_HandsetsMask)&(1<< u8_HsNum))
    	{//HS Matches	
//Special Condtion when CallId does not come for first time

			if(st_HsCallState[u8_HsNum][0].u8_ActCallID  == st_CallState->u8_ActCallID)
			{//CallID alreday present
				 APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][0] for STATUS for HsNo = %x >>>\n",u8_HsNum);
    			st_HsCallState[u8_HsNum][0] = *(st_CallState);
			}
			else if (st_HsCallState[u8_HsNum][1].u8_ActCallID  == st_CallState->u8_ActCallID)
			{//Call Id Already Present
				 APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][1] for STATUS for HsNo = %x >>>\n",u8_HsNum);
    			st_HsCallState[u8_HsNum][1] = *(st_CallState);
			}
			else	

//Special Condition End

		
    		if((st_HsCallState[u8_HsNum][0].u8_ActCallID == st_CallState->u8_ActCallID) || 
				(st_HsCallState[u8_HsNum][0].e_CallStatus == CMBS_CALL_STATE_STATUS_IDLE))
    		{//Call Id Matches with 0th location(New Call ID)
    			 APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][0] for STATUS for HsNo = %x >>>\n",u8_HsNum);
    			st_HsCallState[u8_HsNum][0] = *(st_CallState);
    		}else if ((st_HsCallState[u8_HsNum][1].u8_ActCallID == st_CallState->u8_ActCallID) ||
				(st_HsCallState[u8_HsNum][1].e_CallStatus == CMBS_CALL_STATE_STATUS_IDLE))
    		{//Call Id Matches with 1th location (New Call ID)
    			 APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][1] for STATUS for HsNo = %x >>>\n",u8_HsNum);
    			st_HsCallState[u8_HsNum][1] = *(st_CallState);
    		}
    	}
    }
    /*When Some HSs goes out of conferenc taret dont CS_IDLE with those  HS mask. So when update comes if
      current status is CONF conect & Mask is not present remove status*/
   for (u8_HsNum = 0; u8_HsNum < CMBS_HS_SUBSCRIBED_MAX_NUM; u8_HsNum++)
    {
        //Status received is not Conf connected
    	if ((st_CallState->e_CallStatus != CMBS_CALL_STATE_STATUS_CONF_CONNECTED) && 
			!((st_CallState->u16_HandsetsMask)&(1<< u8_HsNum)))
    	{//HS Matches	
#if 0
    	//	if((st_HsCallState[u8_HsNum][0].u8_ActCallID == st_CallState->u8_ActCallID) &&
			//	(st_HsCallState[u8_HsNum][0].e_CallStatus == CMBS_CALL_STATE_STATUS_CONF_CONNECTED))
#endif
			if (st_HsCallState[u8_HsNum][0].u8_ActCallID == st_CallState->u8_ActCallID)
    		{//Call Id Matches & HS in conf connecte state, means HS gone out of conf
    			 APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][0] for GOING out of conf >>>\n");
    			memset(&st_HsCallState[u8_HsNum][0], 0, sizeof(ST_IE_CALL_STATE));
				 st_HsCallState[u8_HsNum][0].u8_ActCallID = 0xFF;
    		}else
#if 0
				if ((st_HsCallState[u8_HsNum][1].u8_ActCallID == st_CallState->u8_ActCallID) && 
				(st_HsCallState[u8_HsNum][1].e_CallStatus == CMBS_CALL_STATE_STATUS_CONF_CONNECTED))
#endif
			if(st_HsCallState[u8_HsNum][1].u8_ActCallID == st_CallState->u8_ActCallID)
    		{//Call Id Matches with 1th location
    			 APP_CALL_TRACE_PRINT("\n\n<<< UPDATING st_HsCallState[u8_HsNum][1] for GOING out of conf>>>\n");
    			memset(&st_HsCallState[u8_HsNum][1], 0, sizeof(ST_IE_CALL_STATE));
				 st_HsCallState[u8_HsNum][1].u8_ActCallID = 0xFF;
    		}
    	}
    }
	
}
//Print
	APP_CALL_TRACE_PRINT("\n\n<<<<<<<<<$$$ UPDATED STATUS DATA$$$$>>>\n");
	for (u8_HsNum = 0; u8_HsNum < CMBS_HS_SUBSCRIBED_MAX_NUM; u8_HsNum++)
    {
    	if ((st_CallState->u16_HandsetsMask)&(1<< u8_HsNum))
    	{//HS Matches
    		APP_CALL_TRACE_PRINT("\n\n<<<<<<<<<$$$ First  CALLID DATA HsNo = %x $$$$>>>\n",u8_HsNum);
    		APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].e_CallStatus = %x ", st_HsCallState[u8_HsNum][0].e_CallStatus);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].e_CallType = %x ",st_HsCallState[u8_HsNum][0].e_CallType);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].u16_HandsetsMask = %x ",st_HsCallState[u8_HsNum][0].u16_HandsetsMask);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].u8_ActCallID = %x ",st_HsCallState[u8_HsNum][0].u8_ActCallID);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].u8_LinesMask = %x ",st_HsCallState[u8_HsNum][0].u8_LinesMask);
			APP_CALL_TRACE_PRINT("\n\n<<<<<<<<<$$$ SECOND CALLID DATA HsNo = %x $$$$>>>\n",u8_HsNum);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].e_CallStatus = %x ", st_HsCallState[u8_HsNum][1].e_CallStatus);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].e_CallType = %x ", st_HsCallState[u8_HsNum][1].e_CallType);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].u16_HandsetsMask = %x ", st_HsCallState[u8_HsNum][1].u16_HandsetsMask);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].u8_ActCallID = %x ", st_HsCallState[u8_HsNum][1].u8_ActCallID);
			APP_CALL_TRACE_PRINT("\nst_HsCallState[u8_HsNum].u8_LinesMask = %x ", st_HsCallState[u8_HsNum][1].u8_LinesMask);
    	}
    }
}


void appcall_GetHsStatus( u8 u8_HsNo, ST_IE_CALL_STATE *st_CallState)
{
	if (u8_HsNo <= CMBS_HS_SUBSCRIBED_MAX_NUM)
	{
		st_CallState[0] = st_HsCallState[u8_HsNo - 1][0];
		st_CallState[1] = st_HsCallState[u8_HsNo - 1][1];
	}else
	{
		APP_CALL_TRACE_PRINT("\n\n<<< ERROR: Enter Proper HsNo = %x>>>\n",u8_HsNo);
	}
}



//*/
void appcall_ConferenceCallTableInit(void)
{
    u8 u8_Index;
    for (u8_Index = 0; u8_Index < CMBS_MAX_NUMBER_OF_CONFERENCE_CALLS; u8_Index++)
    {
        ConferenceCallTable[u8_Index].u32_CallInstance_1 = 0xFFFFFFFF;
        ConferenceCallTable[u8_Index].u32_CallInstance_2 = 0xFFFFFFFF;
    }
}

u8 appcall_ConferenceCallTableSet(u32 u32_CallInstance_to, u32 u32_CallInstance_frm)
{
    u8 u8_Index;
    for (u8_Index = 0; u8_Index < CMBS_MAX_NUMBER_OF_CONFERENCE_CALLS; u8_Index++)
    {
        if ( (ConferenceCallTable[u8_Index].u32_CallInstance_1 == 0xFFFFFFFF) && (ConferenceCallTable[u8_Index].u32_CallInstance_2 == 0xFFFFFFFF) )
            break;
    }
    if ( u8_Index < CMBS_MAX_NUMBER_OF_CONFERENCE_CALLS )
    {
        ConferenceCallTable[u8_Index].u32_CallInstance_1 = u32_CallInstance_to;
        ConferenceCallTable[u8_Index].u32_CallInstance_2 = u32_CallInstance_frm;
        return u8_Index;
    }
    else
        return APPCALL_NO_INDEX;
}

u32 appcall_ConferencePartnerGet(u32 u32_CallInstance)
{
    u8 u8_Index;
    for (u8_Index = 0; u8_Index < CMBS_MAX_NUMBER_OF_CONFERENCE_CALLS; u8_Index++)
    {
        if ( ConferenceCallTable[u8_Index].u32_CallInstance_1 == u32_CallInstance )
            return ConferenceCallTable[u8_Index].u32_CallInstance_2;
        else if ( ConferenceCallTable[u8_Index].u32_CallInstance_2 == u32_CallInstance )
            return ConferenceCallTable[u8_Index].u32_CallInstance_1;
    }
    return 0;
}

void appcall_ConferenceCallTableClearEntry(u32 u32_CallInstance)
{
    u8 u8_Index;
    for (u8_Index = 0; u8_Index < CMBS_MAX_NUMBER_OF_CONFERENCE_CALLS; u8_Index++)
    {
        if ( (ConferenceCallTable[u8_Index].u32_CallInstance_1 == u32_CallInstance) || (ConferenceCallTable[u8_Index].u32_CallInstance_2 == u32_CallInstance) )
        {
            ConferenceCallTable[u8_Index].u32_CallInstance_1 = 0xFFFFFFFF;
            ConferenceCallTable[u8_Index].u32_CallInstance_2 = 0xFFFFFFFF;
            break;
        }
    }
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

static ST_CALL_OBJ *appcall_GetCallPendingforDeviceObj(ST_IE_CALLERPARTY st_CallerParty)
{
    u8 u8_CallIndex, u8_AddressIndex;
    bool b_MatchFound = TRUE;
	
    for (u8_CallIndex = 0; u8_CallIndex < APPCALL_CALLOBJ_MAX; u8_CallIndex++)
    {
    	// find pendinf call
        if ( g_call_obj[u8_CallIndex].u32_CallInstance && 
		g_call_obj[u8_CallIndex].b_WaitingForMerge &&
		g_call_obj[u8_CallIndex].st_CalledParty.u8_AddressLen == st_CallerParty.u8_AddressLen)
			
        {
        	// compare address
            for (u8_AddressIndex = 0; u8_AddressIndex < g_call_obj[u8_CallIndex].st_CalledParty.u8_AddressLen; u8_AddressIndex++)
            	{
			if (st_CallerParty.pu8_Address[u8_AddressIndex] != g_call_obj[u8_CallIndex].st_CalledParty.pu8_Address[u8_AddressIndex])
				{
					b_MatchFound = FALSE;
					break;
				}
            	}

		if(b_MatchFound)
			{
				printf("\n\n<<< appcall_GetCallPendingforDeviceObj, return g_call_obj[%d]>>>\n", u8_CallIndex);
				return &g_call_obj[u8_CallIndex];
			}
        }
    }
    return NULL;
}


static bool app_RemoveDeviceFromListOfParticipants(PST_CALL_OBJ pst_Call, u16 u16_DeviceId)
{
	u8 pu8_Buffer[6];
	u8 u8_Index;

	pu8_Buffer[0] = APPCALL_DEVICE_IDENTIFIER;
	
	appcall_ConvertDeviceIdToStr(u16_DeviceId, &pu8_Buffer[1]);
	pu8_Buffer[5] = 0;

	for (u8_Index = 0; u8_Index < pst_Call->u8_NumOfParticipants; u8_Index++)
	{
		if (strstr((char *)&pst_Call->pu8_ListOfParticipants[u8_Index][0], (char *)&pu8_Buffer[0]))
		{
			memset(&pst_Call->pu8_ListOfParticipants[u8_Index], 0, sizeof(pst_Call->pu8_ListOfParticipants[u8_Index]));
			pst_Call->u8_NumOfParticipants--;

			return TRUE;
		}

	}

	return FALSE;
}


static bool app_AddDeviceToListOfParticipants(PST_CALL_OBJ pst_Call, u16 u16_DeviceId)
{
	u8 pu8_Buffer[6] = {0};

	pu8_Buffer[0] = APPCALL_DEVICE_IDENTIFIER;
	
	appcall_ConvertDeviceIdToStr(u16_DeviceId, &pu8_Buffer[1]);
	
	if(pst_Call->u8_NumOfParticipants < MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE)
	{
		u8 u8_Index;

		// look for empty record
		for(u8_Index = 0; u8_Index < MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE; u8_Index++)
		{
			if (!strlen((char *)pst_Call->pu8_ListOfParticipants[u8_Index]))
			{
				memcpy((void *)&pst_Call->pu8_ListOfParticipants[u8_Index][0], (void *)pu8_Buffer, 
					    MIN(sizeof(pst_Call->pu8_ListOfParticipants[u8_Index]), sizeof(pu8_Buffer)));
				
				pst_Call->u8_NumOfParticipants++;

				return TRUE;
			}
		}
	}
	
	return FALSE;
}

static bool app_AddHSToListOfParticipants(PST_CALL_OBJ pst_Call, u8 u8_HsNum)
{
	
	if(pst_Call->u8_NumOfParticipants < MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE)
	{
		u8 u8_Index;

		// look for empty record
		for(u8_Index = 0; u8_Index < MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE; u8_Index++)
		{
			if (!strlen((char *)pst_Call->pu8_ListOfParticipants[u8_Index]))
			{
				pst_Call->pu8_ListOfParticipants[u8_Index][0] = u8_HsNum + '0';
				
				pst_Call->u8_NumOfParticipants++;

				return TRUE;
			}
		}
	}
	
	return FALSE;
}

static bool app_RemoveAllHSFromListOfParticipants(PST_CALL_OBJ pst_Call)
{
	u8 u8_Index;
	
	// update list of participants
	 for(u8_Index = 0; u8_Index < MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE; u8_Index++)
	 {
		 if (pst_Call->pu8_ListOfParticipants[u8_Index][0] != 0 &&
			 pst_Call->pu8_ListOfParticipants[u8_Index][0] != APPCALL_DEVICE_IDENTIFIER)
		{

			memset(&pst_Call->pu8_ListOfParticipants[u8_Index], 0, sizeof(pst_Call->pu8_ListOfParticipants[u8_Index]));
			pst_Call->u8_NumOfParticipants--;

		}
	}
	
	 return TRUE;
}


//		========== appcall_RefreshCallInfo ===========
/*!
        \brief			signal to CMBS Target, that read call info table from target .
*/
void appcall_RefreshCallInfo()
{
    cmbs_dsr_GetActiveCalls(NULL);
}


#ifdef PNCAP
/*!
	\brief         Called to send MWI,Date/Time and setting Name & Number
	\u16_HsNo  : Valid HS Mask to which ETP to be sent Note:bit-0-HS1,bit-1=HS2,etc..
	\PncapAttribute : Can be DateTimeSync, MWISend, MissedCallNotification,NameNumber
	\AttributeStructure: Based on what is sent in "PncapAttribute", respective structure should be filled inside AttributeStructure
	\u16_HSMask: HSs to which ETP through facility to be sent, bit-0-HS1,bit-1=HS2,etc..
	\return	: void, once ETP is sent trhough Facility , there whill be response OK or Error
		Note: There will be response for each individual HS, HOST can sent next ETP after getting response for first one*/
void appcall_PnCapAttribute(ePnCapAttribute PncapAttribute, UnionPnCapFacility AttributeStructure, u16 u16_HSMask )
{
    switch (PncapAttribute)
    	{
	case DateTimeSync: //Date and Time Sync 
	{
	u8  u8_DataLen, u8_Data[CMBS_PROP_EVENT_LENGTH];
	u8 pie_buff[56];
      u8 pie_len;
	u8 u8_Index;
	u16 u16_RequestId = 1;
       struct pncap_date_and_time  Pncapdate;
       struct pncap_etp_msg etp;
	etp.buff = (uint8_t *)(&u8_Data[0]);
	etp.buff_size = CMBS_PROP_EVENT_LENGTH;
	Pncapdate.date_mode = ucBin2Bcd(AttributeStructure.PnCapDateTime.date_mode); //0/1 format
	Pncapdate.day = ucBin2Bcd(AttributeStructure.PnCapDateTime.day);// day 
	Pncapdate.month = ucBin2Bcd(AttributeStructure.PnCapDateTime.month);// month 
	Pncapdate.year = ucBin2Bcd(AttributeStructure.PnCapDateTime.year);// year 
	Pncapdate.time_mode = ucBin2Bcd(AttributeStructure.PnCapDateTime.time_mode); // 12/24 format
	Pncapdate.minutes = ucBin2Bcd(AttributeStructure.PnCapDateTime.minutes);//  minutes
	Pncapdate.hours = ucBin2Bcd(AttributeStructure.PnCapDateTime.hours);// hours
	// Assemble the PIE
	pie_len = pncap_pie_set_date_time(pie_buff, &Pncapdate);
	// Insert the PIE into DECT_IE_ETP
	pncap_etp_open(&etp); // Write the Header of the ETP message
	pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
	u8_DataLen = pncap_etp_close(&etp);
	//Removing 7B & Length as it will added in target
	for (u8_Index=0;u8_Index<u8_DataLen;u8_Index++)
	{
	u8_Data[u8_Index] = u8_Data[u8_Index+2];
	}
	u8_DataLen = u8_DataLen-2;
	printf("\n@@@@@@@@@@@@ Sending PNCAP Data for GS HSs\n");

     app_FacilityPropEvent( u16_RequestId, PNCAP_RESERVE_FACILITY, u8_Data, u8_DataLen, u16_HSMask );
	
	}
	break;
	case MWISend: //Message Waiting Indication
		{
			u8  u8_DataLen, u8_Data[CMBS_PROP_EVENT_LENGTH];
			u8 pie_buff[56];
			u8 pie_len;
			u8 u8_Index;
			u16 u16_RequestId = 1;
			u8 u8_MsgsWaiting;
		       struct pncap_etp_msg etp;
			etp.buff = (uint8_t *)(&u8_Data[0]);
			etp.buff_size = CMBS_PROP_EVENT_LENGTH;
			u8_MsgsWaiting = AttributeStructure.u8_MessagesWaiting;
			
			// Assemble the PIE
			pie_len = pncap_pie_set_mwi_am(pie_buff, u8_MsgsWaiting);

			// Insert the PIE into DECT_IE_ETP
			pncap_etp_open(&etp);				      // Write the Header of the ETP message
			pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
			u8_DataLen = pncap_etp_close(&etp);
			
			//Removing 7B & Length as it will added in target
			for (u8_Index=0;u8_Index<u8_DataLen;u8_Index++)
			{
			u8_Data[u8_Index] = u8_Data[u8_Index+2];
			}
			u8_DataLen = u8_DataLen-2;
			printf("\n@@@@@@@@@@@@ Sending PNCAP Data for GS HSs\n");

		     app_FacilityPropEvent( u16_RequestId, PNCAP_RESERVE_FACILITY, u8_Data, u8_DataLen, u16_HSMask );


		}
	break;
	case 2: //Missed Call Notification
	break;
	case NameNumber: //Handset Internal Name and Number
		{
			u8  u8_DataLen, u8_Data[CMBS_PROP_EVENT_LENGTH];
			u8 pie_buff[56];
		      u8 pie_len;
			u8 u8_Index;
			u16 u16_RequestId = 1;
		       pncap_name_and_number  PncapNameNumber;
		       struct pncap_etp_msg etp;
			etp.buff = (uint8_t *)(&u8_Data[0]);
			etp.buff_size = CMBS_PROP_EVENT_LENGTH;
			strcpy((char *)PncapNameNumber.IntName, (const char *)AttributeStructure.PnCapNameNum.IntName);
			PncapNameNumber.u8_IntNum= AttributeStructure.PnCapNameNum.u8_IntNum;
			PncapNameNumber.u8_Coding = AttributeStructure.PnCapNameNum.u8_Coding;
			
			// Assemble the PIE
			pie_len = pncap_pie_set_int_name_number(pie_buff, PncapNameNumber.IntName, PncapNameNumber.u8_Coding, PncapNameNumber.u8_IntNum);

			// Insert the PIE into DECT_IE_ETP
			pncap_etp_open(&etp); // Write the Header of the ETP message
			pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
			u8_DataLen = pncap_etp_close(&etp);

			//Removing 7B & Length as it will added in target
			for (u8_Index=0;u8_Index<u8_DataLen;u8_Index++)
			{
			u8_Data[u8_Index] = u8_Data[u8_Index+2];
			}
			u8_DataLen = u8_DataLen-2;
			printf("\n@@@@@@@@@@@@ Sending PNCAP Data for GS HSs\n");

		     app_FacilityPropEvent( u16_RequestId, PNCAP_RESERVE_FACILITY, u8_Data, u8_DataLen, u16_HSMask );
		}
	break;
	default:
	break;
    	}
}

/*!
	\brief         Called to send PNCAP_PIE_LOCAL_FEATURE_ACTIVATION to perticular HS
	\u16_HsNo  : Valid HS number to which PNCAP_PIE_LOCAL_FEATURE_ACTIVATION to be sent
	\return			<u16>                (1), or ~0  if failed!*/

u16 appcall_PnCapPhoneBookActivate(u16 u16_HsNo )
{
	struct pncap_etp_msg etp;
	u8 pie_len,u8_EtpLen;
	u8 pie_buff[50];
	ST_IE_DATA		st_Data;
	PST_CFR_IE_LIST p_List;
	ALLOCATE_IE_LIST(p_List);
	
	if ((u16_HsNo < 0) || (u16_HsNo > 5))
	{
		printf("Wrong HsNo=%d\n", u16_HsNo);
	return ~0;
	}
	memset(&u8_HsTxRx_pie_buf[u16_HsNo - 1][0], 0x00, sizeof(u8_HsTxRx_pie_buf[u16_HsNo - 1]));
	//Initilising  etp buffers
	etp.buff = (uint8_t *)(&u8_HsTxRx_pie_buf[u16_HsNo-1][0]);
	etp.buff_size = sizeof(u8_HsTxRx_pie_buf[u16_HsNo-1]);
	// Forming pie
	pie_len = pncap_pie_set_phonebook_activate(pie_buff);
	//  Forming etp
	pncap_etp_open(&etp); // Write the Header of the ETP message
	pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
	u8_EtpLen = pncap_etp_close(&etp);
	st_Data.u16_DataLen = u8_EtpLen;   //second byte contains length + 1 for first byte
	st_Data.pu8_Data = &u8_HsTxRx_pie_buf[u16_HsNo - 1][0];
	//Add HS Num
	cmbs_api_ie_ShortValueAdd( p_List, u16_HsNo, CMBS_IE_HANDSETS );
	//Add Packet
	cmbs_api_ie_DataAdd( p_List, &st_Data );	
	cmbs_int_EventSend( CMBS_EV_DSR_PNCAP_DATA_TX, p_List->pu8_Buffer, p_List->u16_CurSize );
	return(1);
}

/*!
	\brief         Called to send Link_cfm to perticular CallId OR to HS
	\u16_CallId : Valid u16_CallId of ext  call
	\u16_HsNo  : Valid HS number to which Link_proc to be sent
	Note: Either u16_CallId OR u16_HsNo should be used, both cant be used
	        i.e  if u16_CallId used then u16_HsNo must be 0xFFFF vice versa
	\return			<u16>                (1), or ~0  if failed!*/


u16 appcall_PnCapDataCallCfm(u16 u16_CallId , u16 u16_HsNo)
{
	PST_CALL_OBJ pst_Call;
	struct pncap_etp_msg etp;
	u16 u16_HsNum;
	u8 pie_len,u8_EtpLen;
	u8 pie_buff[50];
	ST_IE_DATA		st_Data;
	PST_CFR_IE_LIST p_List;
	ALLOCATE_IE_LIST(p_List);

	if (u16_CallId == 0xFFFF && u16_HsNo == 0xFFFF)
	{
		printf("\nBoth Call ID & HsNo Cant be 0xffff\n");
		return ~0;
	}

	
	if (u16_CallId != 0xFFFF && u16_HsNo != 0xFFFF)
	{
		printf("\nBoth Call ID & HsNo Cant be Valid Value One of them must be Valid Value\n");
		return ~0;
	}
	if (u16_HsNo == 0xFFFF)
	{//For External Call only
		pst_Call = _appcall_CallObjGetById(u16_CallId);
		if ( !pst_Call )
		{
		    printf("NO SUCH CALL WITH ID %d\n", u16_CallId);
		    return ~0;
		}
		u16_HsNum =  appcall_FindHsNumByCallId(u16_CallId);
		if (u16_HsNum == 0xFF)
		{
			printf("HS Numb & CallId does not match  HSNum=%d\n", u16_HsNum);
		return ~0;
		}
		   // Add call Instance IE
		     cmbs_api_ie_CallInstanceAdd(p_List, pst_Call->u32_CallInstance);
		//Clear HS Send buffer
	}
	if (u16_CallId == 0xFFFF)
	{//For Intercom Call Only
		u16_HsNum = u16_HsNo;
	}
	memset(&u8_HsTxRx_pie_buf[u16_HsNum-1][0], 0x00,sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]));
	//Initilising  etp buffers
	etp.buff = (uint8_t *)(&u8_HsTxRx_pie_buf[u16_HsNum-1][0]);
	etp.buff_size = sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]);
	// Forming pie
	pie_len = pncap_pie_Send_phonebook_cfm(pie_buff);
	//  Forming etp
	pncap_etp_open(&etp); // Write the Header of the ETP message
	pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
	u8_EtpLen = pncap_etp_close(&etp);
	st_Data.u16_DataLen = u8_EtpLen;   //second byte contains length + 1 for first byte
	st_Data.pu8_Data    = &u8_HsTxRx_pie_buf[u16_HsNum-1][0];
	//Add HS Num
	cmbs_api_ie_ShortValueAdd( p_List, u16_HsNum, CMBS_IE_HANDSETS );
	//Add Packet
	cmbs_api_ie_DataAdd( p_List, &st_Data );	
	cmbs_int_EventSend( CMBS_EV_DSR_PNCAP_DATA_TX, p_List->pu8_Buffer, p_List->u16_CurSize );
	return(1);
}

/*!
	\brief         Called to send Link_proc to perticular CallId OR to HS
	\u16_CallId : Valid u16_CallId of ext  call
	\u16_HsNo  : Valid HS number to which Link_proc to be sent
	Note: Either u16_CallId OR u16_HsNo should be used, both cant be used
	        i.e  if u16_CallId used then u16_HsNo must be 0xFFFF vice versa
	\return			<u16>                (1), or ~0  if failed!*/

u16 appcall_PnCapDataCallProc(u16 u16_CallId , u16 u16_HsNo)
{
	PST_CALL_OBJ pst_Call;
	struct pncap_etp_msg etp;
	u16 u16_HsNum;
	u8 pie_len,u8_EtpLen;
	u8 pie_buff[50];
	ST_IE_DATA		st_Data;
	PST_CFR_IE_LIST p_List;
	ALLOCATE_IE_LIST(p_List);
	
	if (u16_CallId == 0xFFFF && u16_HsNo == 0xFFFF)
	{
		printf("\nBoth Call ID & HsNo Cant be 0xffff\n");
		return ~0;
	}

	
	if (u16_CallId != 0xFFFF && u16_HsNo != 0xFFFF)
	{
		printf("\nBoth Call ID & HsNo Cant be Valid Value One of them must be Valid Value\n");
		return ~0;
	}
	if (u16_HsNo == 0xFFFF)
	{//For External Call only
		pst_Call = _appcall_CallObjGetById(u16_CallId);
		if ( !pst_Call )
		{
		    printf("NO SUCH CALL WITH ID %d\n", u16_CallId);
		    return ~0;
		}
		u16_HsNum =  appcall_FindHsNumByCallId(u16_CallId);
		if (u16_HsNum == 0xFF)
		{
			printf("HS Numb & CallId does not match  HSNum=%d\n", u16_HsNum);
		return ~0;
		}
		   // Add call Instance IE
		     cmbs_api_ie_CallInstanceAdd(p_List, pst_Call->u32_CallInstance);
		//Clear HS Send buffer
	}
	if (u16_CallId == 0xFFFF)
	{//For Intercom Call Only
		u16_HsNum = u16_HsNo;
	}
	memset(&u8_HsTxRx_pie_buf[u16_HsNum-1][0], 0x00,sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]));
	//Initilising  etp buffers
	etp.buff = (uint8_t *)(&u8_HsTxRx_pie_buf[u16_HsNum-1][0]);
	etp.buff_size = sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]);
	// Forming pie
	pie_len = pncap_pie_set_phonebook_Proceeding(pie_buff);
	//  Forming etp
	pncap_etp_open(&etp); // Write the Header of the ETP message
	pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
	u8_EtpLen = pncap_etp_close(&etp);
	st_Data.u16_DataLen = u8_EtpLen;   //second byte contains length + 1 for first byte
	st_Data.pu8_Data    = &u8_HsTxRx_pie_buf[u16_HsNum-1][0];
	//Add HS Num
	cmbs_api_ie_ShortValueAdd( p_List, u16_HsNum, CMBS_IE_HANDSETS );
	//Add Packet
	cmbs_api_ie_DataAdd( p_List, &st_Data );	
	cmbs_int_EventSend( CMBS_EV_DSR_PNCAP_DATA_TX, p_List->pu8_Buffer, p_List->u16_CurSize );
	return(1);
}

/*!
	\brief         Called to send Link_Rel to perticular CallId
	\u16_CallId : Valid u16_CallId of ext IC call
	\return			<u16>                (1), or ~0  if failed!*/

u16  appcall_PnCapDataTransferStop(  u16 u16_CallId )
{
	PST_CALL_OBJ pst_Call;
	struct pncap_etp_msg etp;
	u16 u16_HsNum;
	u8 pie_len,u8_EtpLen;
	u8 pie_buff[50];
	ST_IE_DATA		st_Data;
	PST_CFR_IE_LIST p_List;
	ALLOCATE_IE_LIST(p_List);
	pst_Call = _appcall_CallObjGetById(u16_CallId);
	if ( !pst_Call )
	{
	    printf("NO SUCH CALL WITH ID %d\n", u16_CallId);
	    return ~0;
	}
	u16_HsNum =  appcall_FindHsNumByCallId(u16_CallId);
	if (u16_HsNum == 0xFF)
	{
		printf("HS Numb & CallId does not match  HSNum=%d\n", u16_HsNum);
	return ~0;
	}
	   // Add call Instance IE
	     cmbs_api_ie_CallInstanceAdd(p_List, pst_Call->u32_CallInstance);
	//Clear HS Send buffer
	memset(&u8_HsTxRx_pie_buf[u16_HsNum-1][0], 0x00,sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]));
	//Initilising  etp buffers
	etp.buff = (uint8_t *)(&u8_HsTxRx_pie_buf[u16_HsNum-1][0]);
	etp.buff_size = sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]);
	// Forming pie
	pie_len = pncap_pie_set_phonebook_stop_transfer(pie_buff);
	//  Forming etp
	pncap_etp_open(&etp); // Write the Header of the ETP message
	pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
	u8_EtpLen = pncap_etp_close(&etp);
	st_Data.u16_DataLen = u8_EtpLen;   //second byte contains length + 1 for first byte
	st_Data.pu8_Data    = &u8_HsTxRx_pie_buf[u16_HsNum-1][0];
	//Add HS Num
	cmbs_api_ie_ShortValueAdd( p_List, u16_HsNum, CMBS_IE_HANDSETS );
	//Add Packet
	cmbs_api_ie_DataAdd( p_List, &st_Data );	
	cmbs_int_EventSend( CMBS_EV_DSR_PNCAP_DATA_TX, p_List->pu8_Buffer, p_List->u16_CurSize );
	return(1);
}

/*!
	\brief         Called when Data packet need to be sent to HS(Name & Number)
	\u8_Name : Null terninater String
	\u8_Num : Null Ternimated string
	\u16_HsNum  HS Number
	\return			<u16>                (1), or ~0  if failed!*/
u16  appcall_PnCapDataPacketSend( u8 *u8_Name, u8 *u8_Num, u16 u16_CallId )
{

	PST_CALL_OBJ pst_Call;
	struct pncap_etp_msg etp;
	u16 u16_HsNum;
	u8 pie_len,u8_EtpLen;
	u8 pie_buff[50];
	ST_IE_DATA		st_Data;
	PST_CFR_IE_LIST p_List;
	ALLOCATE_IE_LIST(p_List);
	pst_Call = _appcall_CallObjGetById(u16_CallId);
	if ( !pst_Call )
	{
	    printf("NO SUCH CALL WITH ID %d\n", u16_CallId);
	    return ~0;
	}
	u16_HsNum =  appcall_FindHsNumByCallId(u16_CallId);
	if (u16_HsNum == 0xFF)
	{
		printf("HS Numb & CallId does not match  HSNum=%d\n", u16_HsNum);
	return ~0;
	}
	   // Add call Instance IE
	     cmbs_api_ie_CallInstanceAdd(p_List, pst_Call->u32_CallInstance);
	//Clear HS Send buffer
	memset(&u8_HsTxRx_pie_buf[u16_HsNum-1][0], 0x00,sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]));
	//Initilising  etp buffers
	etp.buff = (uint8_t *)(&u8_HsTxRx_pie_buf[u16_HsNum-1][0]);
	etp.buff_size = sizeof(u8_HsTxRx_pie_buf[u16_HsNum-1]);
	// Forming pie
	pie_len = pncap_pie_set_phonebook_insert_entry(pie_buff, u8_Name, u8_Num);
	//  Forming etp
	pncap_etp_open(&etp); // Write the Header of the ETP message
	pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
	u8_EtpLen = pncap_etp_close(&etp);
	st_Data.u16_DataLen = u8_EtpLen;   //second byte contains length + 1 for first byte
	st_Data.pu8_Data    = &u8_HsTxRx_pie_buf[u16_HsNum-1][0];
	//Add HS Num
	cmbs_api_ie_ShortValueAdd( p_List, u16_HsNum, CMBS_IE_HANDSETS );
	//Add Packet
	cmbs_api_ie_DataAdd( p_List, &st_Data );	
	cmbs_int_EventSend( CMBS_EV_DSR_PNCAP_DATA_TX, p_List->pu8_Buffer, p_List->u16_CurSize );
	return(1);
}


/*!
        \brief			Establish External IC call & send link_Req in SETUP
        \u16_HsNo		HS number to which Data Call to be Setup
        \return			<u16>                Call ID, or ~0  if failed!
*/


u16  appcall_PnCapDataCallStart( u16 u16_HsNo )
{
      	ST_APPCALL_PROPERTIES st_Properties[6];
	ST_IE_CALLEDPARTY st_CalledParty;
       PST_CALL_OBJ pst_Call;
	u8 u8_Hsnum[3]="h1";
	u8 pie_buff[50];
	u8 pie_len,u8_LineId;
	u8 u8_EtpLen;
	struct pncap_etp_msg etp;
	PST_CFR_IE_LIST p_List;
	ST_IE_DATA		st_Data;
	ALLOCATE_IE_LIST(p_List);
	//Preparing Data fro EXt IC call
	pst_Call = _appcall_CallObjNew();
	if ( !pst_Call )
	{
		return ~0;
	}
if (u16_HsNo < 1 || u16_HsNo > 5)
{
	printf("\nEnter the HsNo between 1 to 5\n");
	return(~0);
}
      u8_Hsnum[1] =  u16_HsNo + 0x30;
	
	/* Mark call as incoming */
	pst_Call->b_Incoming = TRUE;
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        //st_Properties[1].psz_Value = "h1\0"; //Tobe changed from hardcoded value
	st_Properties[1].psz_Value = (char *)u8_Hsnum; //Tobe changed from hardcoded value
	// Create a new Call ID
	pst_Call->u32_CallInstance = cmbs_dee_CallInstanceNew(g_cmbsappl.pv_CMBSRef);

 	// Add call Instance IE
        cmbs_api_ie_CallInstanceAdd(p_List, pst_Call->u32_CallInstance);
	//Add LineId
	u8_LineId = 0;//Hard coded to 0 now
	cmbs_api_ie_LineIdAdd(p_List, u8_LineId);
       //Called party is mush to call a HS
        _appcall_CalledIDSet(pst_Call, st_Properties, 5, &st_CalledParty);  
        cmbs_api_ie_CalledPartyAdd(p_List, &st_CalledParty);
	   
	//Forming etp packet
	etp.buff = (uint8_t *)(&u8_HsTxRx_pie_buf[u16_HsNo-1][0]);
	etp.buff_size = sizeof(u8_HsTxRx_pie_buf[u16_HsNo-1]);
	//Forming Full Packet
	pie_len = pncap_pie_set_phonebook_start_transfer(pie_buff, (u8)u16_HsNo);
	pncap_etp_open(&etp); // Write the Header of the ETP message
	pncap_etp_append(&etp, pie_buff, pie_len); // Insert PIE into ETP message
	u8_EtpLen = pncap_etp_close(&etp);
	st_Data.u16_DataLen = u8_EtpLen;   //second byte contains length + 1 for first byte
	st_Data.pu8_Data    = &u8_HsTxRx_pie_buf[u16_HsNo-1][0];
	cmbs_api_ie_ShortValueAdd( p_List, u16_HsNo, CMBS_IE_HANDSETS );
	cmbs_api_ie_DataAdd( p_List, &st_Data );	
	cmbs_int_EventSend( CMBS_EV_DSR_PNCAP_DATA_TX, p_List->pu8_Buffer, p_List->u16_CurSize );
	return _appcall_CallObjIdGet(pst_Call);
}
#endif
