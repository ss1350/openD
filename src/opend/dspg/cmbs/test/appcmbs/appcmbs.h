/*!
*	\file		appmain.h
*	\brief	object and utility declaration
*	\Author		kelbch
*
*	@(#)	%filespec: appcmbs.h~13.1.1.1.1.1.10 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
*******************************************************************************/

#if	!defined( APPCMBS_H )
#define	APPCMBS_H

#include "cfr_mssg.h"

#ifndef caseretstr
#define caseretstr(x) case x: return #x
#endif

#if defined ( TCXLOG_FILE )
#include "tcx_hostlog.h"
#define     APPCMBS_ERROR(x)        GENERAL_LOG_ERROR_PRINT x
#define     APPCMBS_WARN(x)         GENERAL_LOG_WARNING_PRINT x
#define     APPCMBS_INFO(x)         GENERAL_LOG_INFO_PRINT x

#define     APPCMBS_IE(x)      GENERAL_LOG_INFO_PRINT x
#else
#define     APPCMBS_ERROR(x)        printf x
#define     APPCMBS_WARN(x)         printf x
#define     APPCMBS_INFO(x)         printf x
#define     APPCMBS_IE(x)      printf x
#endif

#define		APPCMBS_RECONNECT_TIMEOUT		    (60000)	//60 sec (should be enough for eeprom resetting)
#define HAN_DECT_SUBS_LENGTH (101*1000+5)
#define HAN_ULE_SUBS_LENGTH (15*1000)
#define HAN_FUN_SUBS_LENGTH (131*1000)
#define HAN_FUN_GROUP_LIST_LENGTH (50)
#define HAN_FUN_GROUP_TABLE_LENGTH (500)
#define HAN_ULE_BROADCAST_CONVERSION_TABLE_LENGTH (600)

#define PARAM_REPEATER_SUBS_DATA_LENGTH  50*28  //50 bytes per Repeater and handset over repeater, 28 in maximum case



typedef void* (*start_routine)(void *);

typedef struct
{
    E_CMBS_IE_TYPE e_IE;
    union
    {
        u32 u32_CallInstance;
        u8 u8_LineId;
        ST_IE_CALLEDPARTY st_CalledParty;
        ST_IE_CALLERPARTY st_CallerParty;
        ST_IE_CALLERNAME st_CallerName;
        ST_IE_CALLPROGRESS st_CallProgress;
        ST_IE_CALLINFO st_CallInfo;
        ST_IE_DISPLAY_STRING st_DisplayString;
        ST_IE_RELEASE_REASON st_Reason;
        ST_IE_MEDIA_DESCRIPTOR st_MediaDesc;
        ST_IE_MEDIA_CHANNEL st_MediaChannel;
        ST_IE_TONE st_Tone;
        ST_IE_TIMEOFDAY st_Time;
        ST_IE_HANDSETINFO st_HandsetInfo;
        ST_IE_PARAMETER st_Param;
        ST_IE_FW_VERSION st_FwVersion;
        ST_IE_HW_VERSION st_HwVersion;
        ST_IE_SYS_LOG st_SysLog;
        ST_IE_SUBSCRIBED_HS_LIST st_SubscribedHsList;
        ST_IE_LINE_SETTINGS_LIST st_LineSettingsList;
        ST_IE_DECT_SETTINGS_LIST st_DectSettings;
        ST_IE_PROP_FIELDS_LIST st_PropSettings;
        ST_IE_USER_DEFINED st_UserDefined;
        ST_IE_RESPONSE st_Resp;
        //      ST_IE_STATUS            st_Status;
        u32 u32_IntegerValue;
        u8 u8_Melody;
        ST_IE_EEPROM_VERSION st_EEPROMVersion;
		ST_IE_CALL_LIST st_CallList;
    }Info;
} ST_APPCMBS_IEINFO, *PST_APPCMBS_IEINFO;

typedef struct
{
    void *pv_ApplRef;
    void *pv_CMBSRef;
    int n_MssgAppID;
    int n_Token;
    int RegistrationWindowStatus; // 0 - Closed, 1 - Opened
#ifdef WIN32
    PST_ICOM_ENTRY pst_AppSyncEntry;
#endif
}ST_CMBS_APPL, *PST_CMBS_APPL;

typedef struct
{
    int n_InfoLen;
    int n_Info;
    int n_Event;
    char ch_Info[4*1024];
} ST_APPCMBS_CONTAINER, *PST_APPCMBS_CONTAINER;

typedef union {
    u8 pu8_HAN_DECT_DATA[HAN_DECT_SUBS_LENGTH];
    u8 pu8_HAN_ULE_DATA[HAN_ULE_SUBS_LENGTH];
    u8 pu8_HAN_FUN_DATA[HAN_FUN_SUBS_LENGTH];
	u8 pu8_HAN_FUN_GROUP_LIST_DATA[HAN_FUN_GROUP_LIST_LENGTH];
	u8 pu8_HAN_FUN_GROUP_TABLE_DATA[HAN_FUN_GROUP_TABLE_LENGTH];
	u8 pu8_HAN_ULE_BROADCAST_CONVERSION_TABLE_DATA[HAN_ULE_BROADCAST_CONVERSION_TABLE_LENGTH];
	u8 pu8_HAN_ULE_MULTICAST_ENC_PARAMS_DATA[CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH];
    u8 pu8_REPEATER_SUBS_DATA[PARAM_REPEATER_SUBS_DATA_LENGTH];
} UnionHanEEPROMBackupEBlocksPools;

typedef struct
{
    long mType;
    ST_APPCMBS_CONTAINER Content;
} ST_APPCMBS_LINUX_CONTAINER, *PST_APPCMBS_LINUX_CONTAINER;

typedef struct
{
	E_CMBS_HW_CHIP          u8_HwChip;          /*!< HW chip */
    E_CMBS_HW_CHIP_VERSION  u8_HwChipVersion;   /*!< HW chip version */
    E_CMBS_HW_BOARD         u8_HwBoard;         /*!< HW board type */
    E_CMBS_HW_COM_TYPE      u8_HwComType;       /*!< HW communication */
	
} ST_CMBS_APP_INST, *PST_CMBS_APP_INST;

typedef enum
{
	NO_EXTERNAL_CLIENT,
	EXTERNAL_CLIENT_EXIST
}t_e_ExternalClient;

typedef u32(*t_fptrHANTargetAliveIndication)();


extern ST_CMBS_APPL g_cmbsappl;

#if defined( __cplusplus )
extern "C"
{
#endif

s8 app_ResponseCheck(void *pv_List);
u8 app_ASC2HEX(char *psz_Digits);

void keyb_ParamAreaGetBySegments(u32 u32_Pos, u32 u32_Length, u8 *pu8_Data, u32 packet_max_size);
void keyb_ParamAreaSetBySegments(u32 u32_Pos, u32 u32_Length, u8 *pu8_Data, u16 packet_max_size);
E_CMBS_RC keyb_ParamAreaGetFromFile(u8 *pu8_Data, u8 *pu8_path);


t_e_ExternalClient appcmbs_GetExternalClientInformation();
void appcmbs_SetExternalClientInformation(t_e_ExternalClient client);

//callback for target alive indication
void appcmbs_RegisterTargetAliveIndicationCB(t_fptrHANTargetAliveIndication fpt_registerCallBackForTargetAliveIndication);

typedef int (*opendCallback_t)(void *pv_AppRef, E_CMBS_EVENT_ID e_EventId, void *pv_EventData);
void appcmbs_opend_setCallbackFct( opendCallback_t opendCallback );

// callback for CMBS responses according specification
int app_ServiceEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_CallEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_SwupEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_FacilityEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_DataEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_LaEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_SuotaEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_SuotaFileEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_RTPEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_HANEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_CmdEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
int app_PluginEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
#if defined (CRASH_DUMP)
int app_CrashDumpEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
#endif

// initialize the CMBS application
E_CMBS_RC appcmbs_Initialize(void *pv_AppReference, PST_CMBS_DEV pst_DevCtl, PST_CMBS_DEV pst_DevMedia, PST_CB_LOG_BUFFER pfn_log_buffer_Cb);
void appcmbs_Cleanup(void);
void appcmbs_InitEeprom(u8 *pu8_EEprom, u16 u16_Size);
int appcmbs_CordlessStart(ST_IE_SYPO_SPECIFICATION *SYPOParameters);
// synchronize upper application with async CMBS received Data
int appcmbs_ObjectSignalTrace(void);
void appcmbs_ObjectSignal(char *psz_Info, int n_InfoLen, int n_Info, int n_Event);
void appcmbs_PrepareRecvAdd(u32 u32_Token);
int appcmbs_WaitForContainer(int n_Event, PST_APPCMBS_CONTAINER pst_Container);
int appcmbs_WaitForEvent(int n_Event);

void appcmbs_IEInfoGet(void *pv_IE, u16 u16_IE, PST_APPCMBS_IEINFO p_Info);
void appcmbs_VersionGet(char *pc_Version);
unsigned long appcmbs_GetTickCount(void);
int appcmbs_ReconnectApplication(unsigned long ulTimeoutMs);
#ifdef WIN32
void app_CreateThread(LPTHREAD_START_ROUTINE);
#elif __linux__
void app_CreateThread(start_routine);
#endif
PST_CMBS_APP_INST appcmbs_get_CMBSAppInstance(void);
void appcmbs_MaxTransferSizeGet(void);
#if defined( __cplusplus )
}
#endif

#endif	//	APPCMBS_H
//*/
