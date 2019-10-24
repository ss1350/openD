/*!
*	\file			appcall.h
*	\brief			
*	\Author		DSPG 
*
*	@(#)	%filespec: appcall.h~NBGD53#18 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================
*	date       name	   version  action                                        
*	----------------------------------------------------------------------------
*
* 12-Jun-2013 tcmc_asa         brought verison 18 (2.99.9) to 3.x branch
* 28-Jan-2013 tcmc_asa 18      Added appcall_CallInbandInfoCNIP(), PR 3615
* 24-Jan-2013 tcmc_asa 17      Added E_APPCALL_SWITCH_RECEIVED_CODEC
*
*******************************************************************************/

#if	!defined( APPCALL_H )
#define	APPCALL_H


#ifdef PNCAP
#include "pncap_def.h"
#endif

typedef enum
{
   E_APPCALL_AUTOMAT_MODE_OFF,      /*!< call object automat mode off */
   E_APPCALL_AUTOMAT_MODE_ON        /*!< call object automat mode on, incoming and outgoing calls shall be handled in simple statemachine */
}E_APPCALL_AUTOMAT_MODE;

typedef enum
{
   E_APPCALL_PREFERRED_CODEC_RECEIVED_CODEC,
   E_APPCALL_PREFERRED_CODEC_WB,	/*! G722 */
   E_APPCALL_PREFERRED_CODEC_NB,	/*! G726 */
   E_APPCALL_SWITCH_RECEIVED_CODEC  /* WB->NB or NB->WB */
}E_APPCALL_PREFERRED_CODEC;

/*! \brief exchange structure between CMBS API layer and upper application */
typedef  struct
{
   E_CMBS_IE_TYPE e_IE;          /*! IE type */
   char *         psz_Value;     /*! string value in case of upper -> CMBS API layer */
}ST_APPCALL_PROPERTIES, * PST_APPCALL_PROPERTIES;

/*! \brief CMBS API layer call states */
typedef enum
{
   E_APPCMBS_CALL_CLOSE,         /*!<  line is closed */
   
   E_APPCMBS_CALL_INC_PEND,      /*!<  CMBS target is informed of an incoming call*/
   E_APPCMBS_CALL_INC_RING,      /*!<  CMBS target let the handset ringing */
   
   E_APPCMBS_CALL_OUT_PEND,      /*!<  CMBS-API layer received a outgoing call establishment event */
   E_APPCMBS_CALL_OUT_PEND_DIAL, /*!<  Digits will be collected in CLD array, if Dialtone was switched on.
                                       it is automatically switched off, if call enters this state */
   E_APPCMBS_CALL_OUT_INBAND,    /*!<  The outgoing call is set-up to carry inband signalling, e.g. network tones */
   E_APPCMBS_CALL_OUT_PROC,      /*!<  The outgoing call is proceeding state */
   E_APPCMBS_CALL_OUT_RING,      /*!<  The outgoing call is in ringing state, if not the inband tone is available */
   
   E_APPCMBS_CALL_ACTIVE,        /*!<  The call is in active mode, media shall be transmitted after channel start */
   E_APPCMBS_CALL_RELEASE,       /*!<  The call is in release mode */
   E_APPCMBS_CALL_ON_HOLD,       /*!<  The call is on hold */
   E_APPCMBS_CALL_CONFERENCE,    /*!<  The call is in conference mode */
   E_APPCMBS_CALL_SCREENING,	 /*!<  The call is in screening mode */
}E_APPCMBS_CALL;

/*! \brief CBS API layer media state */
typedef enum
{
   E_APPCMBS_MEDIA_CLOSE,        /*!< Media entity is closed */
   E_APPCMBS_MEDIA_PEND,         /*!< Media entity is prepared, codec negotiated and channel ID from Target available */
   E_APPCMBS_MEDIA_ACTIVE        /*!< Media entity is started to stream */
} E_APPCMBS_MEDIA;

/*! \brief  Line node, it contains every important information of the line */
typedef struct
{
   u32   u32_foo;             /*!< foo */  //TODO
} ST_LINE_OBJ, * PST_LINE_OBJ;


#define APPCALL_INVALID_HS_NUM					0xFF

#define MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE 	10
#define CALL_PARTICIPANT_ID_MAX_LEN 			30

/*! \brief  Call node, it contains every important information of the connection */
typedef struct
{
   u32   u32_CallInstance;                                      /*!< Call Instance to identify the call on CMBS */
   u8    u8_LineId;                                             /*!< Line ID*/
   u8    u8_LineSubType;                                             /*!< Line Sub Type*/
   ST_IE_CALLERPARTY  st_CallerParty;                           /*!< Caller Party, incoming call CLI, outgoing call Handset number */
   ST_IE_CALLEDPARTY  st_CalledParty;                           /*!< Called Party, incoming call ringing mask, outgoing call to be dialled number */
   ST_IE_CALLERPARTY  st_TmpParty;                              /*!< further feature, temp party, e.g. call waiting active and the CMBS API layer has to restore connection*/
   char  ch_TmpParty[CALL_PARTICIPANT_ID_MAX_LEN];              /*!< buffer of temp party number */
   char  ch_CallerID[CALL_PARTICIPANT_ID_MAX_LEN];              /*!< buffer of caller party number */
   char  ch_CalledID[CALL_PARTICIPANT_ID_MAX_LEN];              /*!< buffer of called party number */
   u32   u32_ChannelID;                                         /*!< Channel ID to identify the media connection on CMBS */
   u32	 u32_ChannelParameter;		                            /*!< Channel Parameter provides information about the parameter settings, e.g. IOM - used slots */
   E_CMBS_AUDIO_CODEC   e_Codec;                                /*!< used codec */
   u8					pu8_CodecsList[CMBS_AUDIO_CODEC_MAX];   /*!< Codecs list */
   u8					u8_CodecsLength;                        /*!< Codecs list length */
   E_APPCMBS_CALL       e_Call;                                 /*!< call state */
   E_APPCMBS_MEDIA      e_Media;                                /*!< media entity state */
   bool                 b_Incoming;                             /*!< TRUE for incoming calls, FALSE o/w */
   bool                 b_CodecsOfferedToTarget;                /*!< TRUE when codecs have been sent to target (relevant for outgoing calls only) */
   bool                 b_IsSplashRing;			                /*!< TRUE when this call is for Splash Ring only */
   bool					b_WaitingForMerge;						/*!< TRUE if call is waiting to be merged, FALSE o/w */
   u8  					pu8_ListOfParticipants[MAX_NUM_OF_PARTICIPANTS_IN_CONFERENCE][CALL_PARTICIPANT_ID_MAX_LEN];		/*!< stores participant ids for all participants of the call */
   u8					u8_NumOfParticipants;					/*!< stores number of participants in the call */
} ST_CALL_OBJ, * PST_CALL_OBJ;

typedef struct
{
    u8  u8_CallReleaseInProgress;
    u32 u32_CallInstance;
    ST_IE_RELEASE_REASON st_Reason;
	bool b_SendReleaseRequest;
}ST_CALL_RELEASE;

typedef struct
{
    u32 u32_CallInstance_1;
    u32 u32_CallInstance_2;
}ST_CMBS_APP_CONF_CALL_ENTRY, *PST_CMBS_APP_CONF_CALL_ENTRY;

#ifdef PNCAP
typedef enum {
	DateTimeSync,
	MWISend,
	MissedCallNotification,
	NameNumber
}ePnCapAttribute;

typedef struct {
	u8 IntName[26];
	int u8_IntNum;
	int u8_Coding;	 
}pncap_name_and_number;

typedef union {
	/** all values should be in BCD format */
	struct pncap_date_and_time PnCapDateTime;
	pncap_name_and_number PnCapNameNum;
	u8 u8_MessagesWaiting;
}UnionPnCapFacility;

typedef enum{
	PNCAP_RESERVE_FACILITY=1
}ePnCapReserved;
#endif

#define CMBS_MAX_NUMBER_OF_CONFERENCE_CALLS 5

                                          /*! \brief max line */
#define     APPCALL_LINEOBJ_MAX        40
                                          /*! \brief max connection for CMBS API layer */
#if (NBS==1)
#define     APPCALL_CALLOBJ_MAX        10  
#else
#define     APPCALL_CALLOBJ_MAX        8
#endif

#define     APPCALL_NO_LINE	         0xFF
#define     APPCALL_NO_CALL	         0xFFFF
#define 	APPCALL_NO_INDEX		 0xFF
#define     APPCALL_NO_MEDIA_CHANNEL     0xFF

#define ALL_HS_STRING "h123456789A\0"

#define CMBS_MULTICALL_LINE_NUM_OF_CALLS    2
#define CMBS_NO_LINE_DEFINED        		0x7F
#define CMBS_NO_ENTRY_FOUND         		0xFF
#define CMBS_MAX_NUM_OF_LINES   			5


#if defined( __cplusplus )
extern "C"
{
#endif

void        appcall_Initialize (void);
void        appcall_AutomatMode ( E_APPCALL_AUTOMAT_MODE e_Mode );
u16         appcall_EstablishCall ( PST_APPCALL_PROPERTIES pst_Properties, int n_Properties );
int         appcall_ReleaseCall( PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char * psz_CLI );
int         appcall_AnswerCall ( PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char * psz_CLI );
int         appcall_ProgressCall ( PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char * psz_CLI );
int         appcall_DisplayCall ( PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char * psz_CLI );
int         appcall_DisplayString ( PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u16 u16_CallId, char * psz_Display );
int         appcall_ResumeCall( u16 u16_CallId, char * psz_CLI );
int         appcall_HoldCall( u16 u16_CallId, char * psz_CLI );
int         appcall_CallInbandInfo( u16 u16_CallId, char * psz_CLI );
int         appcall_CallInbandInfoCNIP( u16 u16_CallId, char * pu8_Name, char * pu8_FirstName, char * pch_cli );
int			appcall_MergeCalls( u16 u16_CallId1, u16 u16_CallId2, u8 u8_CovertToInt );
int			appcall_EmergencyCallRelease( u16 u16_CallId );
int         appcall_EmergencyCallReleaseByCallInstance( u16 u16_CallInstance );
u16			appcall_EstablishSplashRing( PST_APPCALL_PROPERTIES pst_Properties, int n_Properties, u8 u8_SplashTimer );
void 		appcall_ReleaseSplashRingCall(PST_CALL_OBJ pst_Call);

/* appmedia_CallObjTonePlay: u16_ToneDur parameter & u16_DTMFToneToHSMask are only valid for DTMF request. 
 * For all other tones values should be 0 (will be ignored)
 */
void 		appmedia_CallObjTonePlay(char *psz_Value, u16 u16_ToneVolume, int bo_On, u16 u16_CallId, char *psz_CLI, u16 u16_ToneDur, u16 u16_DTMFToneToHSMask);
void        appmedia_CallObjMediaStart( u32 u32_CallInstance, u16 u16_CallId, u16 u16_StartSlot, char * psz_CLI );
void        appmedia_CallObjMediaStop( u32 u32_CallInstance, u16 u16_CallId, char * psz_CLI );
void        appmedia_CallObjMediaOffer( u16 u16_CallId, char ch_Audio );

void        appcall_InfoPrint			( void							);
bool		appcall_IsHsInCallWithLine	( u8 u8_HsNumber, u8 u8_LineId	);
bool		appcall_IsLineInUse			( u8 u8_LineId					);
void 		appcall_HandleLineUseStatus	(u8 u8_LineId);
void 		appcall_HandleHSUseStatus	(u8 u8_LineId, u16 u16_HsNr, bool b_InUse);
u8 			appcall_GetMaximumNumberOfAllowedCalls(u8 u8_LineId);

void 		appcall_SetActiveCallTable(u8 u8_Line, u32 u32_CallInstanse);
void 		appcall_RemoveCallFromTable(u8 u8_Line, u32 u32_CallInstanse);
bool 		appcall_IsCallInTable(u8 u8_Line, u32 u32_CallInstanse);
u8 	 		appcall_GetNumOfActiveCallsForLine(u8 u8_Line);
bool 		appcall_CheckHSInOtherCall(u8 u8_Line, u16 u16_HSNum, u32 u32_CallInstance);
u16  		appcall_GetHSMask(u8 u8_Line, u32 u32_CallInstanse);
void 		appcall_SetHSMask(u8 u8_Line, u32 u32_CallInstanse, u16 u16_HSMask);
void 		appcall_HSUseChange(u8 u8_LineId, u16 u16_HSMask, u32 u32_CallInstance);
void        appcall_SetHsStatus( u32 u32_CallInstanse, PST_IE_CALL_STATE st_CallState);
void        appcall_GetHsStatus( u8 u8_HsNo, ST_IE_CALL_STATE *st_CallState);
void 		appcall_ConferenceCallTableClearEntry(u32 u32_CallInstance);
u32 		appcall_ConferencePartnerGet(u32 u32_CallInstance);
u8 		appcall_ConferenceCallTableSet(u32 u32_CallInstance_to, u32 u32_CallInstance_frm);
void 		appcall_ConferenceCallTableInit();
void         appcall_RefreshCallInfo();
void appcall_HsStatusInit(void);

#ifdef PNCAP
u16  appcall_PnCapDataCallStart( u16 u16_HsNo );
u16  appcall_PnCapDataPacketSend( u8 *u8_Name, u8 *u8_Num, u16 u16_CallId );
u16  appcall_PnCapDataTransferStop(  u16 u16_CallId );
u16 appcall_PnCapDataCallProc(u16 u16_CallId , u16 u16_HsNo);
u16 appcall_PnCapDataCallCfm(u16 u16_CallId , u16 u16_HsNo);
u16 appcall_PnCapPhoneBookActivate(u16 u16_HsNo );
void appcall_PnCapAttribute(ePnCapAttribute PncapAttribute, UnionPnCapFacility AttributeStructure, u16 u16_HSMask );
#endif

//void        app_PrintCallInfo( E_CMBS_CALL_INFO e_Info );

PST_CALL_OBJ   _appcall_CallObjGetById( u16 u16_CallId );
#if defined( __cplusplus )
}
#endif

#endif	//	APPCALL_H
//*/
