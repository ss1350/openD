/*!
    \brief Initialize the CMBS application

*/


#include <stdio.h>
#if ! defined ( WIN32 )
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h> //we need <sys/select.h>; should be included in <sys/types.h> ???
#include <signal.h>
#include <sys/msg.h>
#include <errno.h>
#endif
#include "cmbs_platf.h"
#include "cmbs_api.h"
#include "cmbs_han.h"
#include "cmbs_util.h"
#include "cmbs_dbg.h"
#include "cfr_ie.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "appsrv.h"
#include "appcall.h"
#include "applog.h"
#include "apphan.h"
#include "appfacility.h"
#include "appmsgparser.h"
#include "appHanMw.h"
#include "ListsApp.h"
#ifdef CMBS_PLUGIN
#include "cmbs_plugin.h"
#endif

extern u8 u8_APP_LA_Initialized;
extern void app_LaInitialize(void);
extern u8 tcx_DetectComPort(bool interactiveMode, E_CMBS_DEVTYPE *pu8_type);
extern void tcx_USBConfig(u8 u8_Port, u32 u32_BaudRate);
u32 g_u32_BaudRate = 115200;
extern ST_CMBS_DEV g_st_DevMedia;
extern ST_CMBS_DEV g_st_DevCtl;
ST_CB_LOG_BUFFER pfn_log_buffer_Cb;
#define DEFAULT_MAX_TRANSFER_SIZE (240)
u32 g_CMBSMaxTransferSize = DEFAULT_MAX_TRANSFER_SIZE;
ST_CMBS_APP_INST g_CMBSAppInstance;
#ifdef __linux
extern void cmbs_api_MsgQDestroy(int nMsgQId);
extern int cmbs_api_MsgQCreate(void);
#elif WIN32
extern void cmbs_api_MsgQDestroy(PST_ICOM_ENTRY pMsgQId);
extern PST_ICOM_ENTRY cmbs_api_MsgQCreate(u32 u32_ThreadID, u32 u32_Timeout);

#endif

//Target alive indication callback
static t_fptrHANTargetAliveIndication 	g_fptrHANTargetAliveIndication = NULL;

/*!***************************************************************************
*
*	\brief CMBS application feedback entity
*
*****************************************************************************/

// different CMBS parts use different entities
// which will be called by callback function
// if the entity consumes the event, it shall return TRUE
typedef int (*PFN_APP_ENTITY_CB)(void *pv_AppRef,
                                 E_CMBS_EVENT_ID e_EventID,
                                 void *pv_EventIEListRef);


// a flag for external Client
t_e_ExternalClient g_e_externalClient = NO_EXTERNAL_CLIENT;

t_e_ExternalClient appcmbs_GetExternalClientInformation()
{
	return g_e_externalClient;
}

void appcmbs_RegisterTargetAliveIndicationCB(t_fptrHANTargetAliveIndication fpt_registerCallBackForTargetAliveIndication)
{	
	g_fptrHANTargetAliveIndication = fpt_registerCallBackForTargetAliveIndication;
}


void appcmbs_SetExternalClientInformation(t_e_ExternalClient client)

{
	g_e_externalClient = client;
}


// global callback function table of received CMBS Events
PFN_APP_ENTITY_CB g_pfn_app_Entity[] =
{
    (PFN_APP_ENTITY_CB)app_ServiceEntity,
    (PFN_APP_ENTITY_CB)app_CallEntity,
    (PFN_APP_ENTITY_CB)app_SwupEntity,
    NULL, //(PFN_APP_ENTITY_CB)app_MediaEntity - currently inside call entity
    (PFN_APP_ENTITY_CB)app_FacilityEntity,
#ifdef CMBS_PLUGIN
    (PFN_APP_ENTITY_CB)app_PluginEntity,
#endif
    (PFN_APP_ENTITY_CB)app_DataEntity,
    (PFN_APP_ENTITY_CB)app_LaEntity,
	// for windows we have only the suota for lcoal file, for linux we will include both and choose in run time
#ifdef WIN32
	// suotafile will be first so if it is enabled and consume the event, the suota app will not interfere
	(PFN_APP_ENTITY_CB)app_SuotaFileEntity,
#ifdef SUOTA_APP
	(PFN_APP_ENTITY_CB)app_SuotaEntity,
#endif
#else
	// suotafile will be first so if it is enabled and consume the event, the suota app will not interfere
	(PFN_APP_ENTITY_CB)app_SuotaFileEntity,
	(PFN_APP_ENTITY_CB)app_SuotaEntity,
#endif

	
    (PFN_APP_ENTITY_CB)app_HANEntity,
    (PFN_APP_ENTITY_CB)app_RTPEntity,
    (PFN_APP_ENTITY_CB)app_LogEntity,
    (PFN_APP_ENTITY_CB)app_CmdEntity
#if defined (CRASH_DUMP)
    ,
    (PFN_APP_ENTITY_CB)app_CrashDumpEntity
#endif
};
// global object of application
ST_CMBS_APPL g_cmbsappl;

u8 g_HoldCfm = 1;       /* 0 - decline hold, 1 - accept hold */
u8 g_HoldResumeCfm = 1; /* 0 - decline hold resume, 1 - accept hold resume */
u8 g_TransferAutoCfm = 1;
u8 g_ConfAutoCfm = 1;
u8 g_EarlyMediaAutoCfm = 1;
u16 g_u16_DemoCallId = APPCALL_NO_CALL;

PST_CMBS_APP_INST appcmbs_get_CMBSAppInstance(void)
{
    return &g_CMBSAppInstance;
}

static opendCallback_t _opendCallback = NULL;

void appcmbs_opend_setCallbackFct( opendCallback_t opendCallback ) {
	_opendCallback = opendCallback;
	return;
}

//		========== appcmbs_CallBack ===========
/*!
        \brief		  registered callback function to CMBS API for received events
        \param[in]	  pv_AppRef		 application reference pointer
        \param[in]	  e_EventID		 received event ID
        \param[in]	  pv_EventData	 IE list pointer
        \return		  <none>
*/
void appcmbs_CallBack(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData)
{
    unsigned int i;

    if ( pv_AppRef )
    {} // Just one callback function registered, so we don't have to evaluate pv_AppRef

	//Update that target is alive every time a message sends from target to host
	if(g_fptrHANTargetAliveIndication)
	{
		g_fptrHANTargetAliveIndication();
	}

    if( _opendCallback ) {
        _opendCallback(pv_AppRef, e_EventID, pv_EventData);
    }

    for (i = 0; i < (sizeof(g_pfn_app_Entity) / sizeof(PFN_APP_ENTITY_CB)); i++)
    {
        if ( g_pfn_app_Entity[i] )
        {
            if ( g_pfn_app_Entity[i](pv_AppRef, e_EventID, pv_EventData) )
            {
                // no handler is needed anymore, event is consumed
                return;
            }
        }
    }
    APPCMBS_WARN(("Warning: Event: %d %s is not handled, yet\n", e_EventID, cmbs_dbg_GetEventName(e_EventID)));
}

int appcmbs_CordlessStart(ST_IE_SYPO_SPECIFICATION *SYPOParameters)
{

    // Start DECT module
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_sys_Start(g_cmbsappl.pv_CMBSRef, SYPOParameters);

    return appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}


//		========== 	appcmbs_MaxTransferSizeGet ===========
/*!
        \brief				 short description
        \param[in,out]		 None
        \return				<int>       return Max size of transfer supported by the Target
*/
void appcmbs_MaxTransferSizeGet(void)
{
    ST_APPCMBS_CONTAINER st_Container;

    //Get CMBS MAX Buffer size supporetd by target
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_MAX_TRANSFER_SIZE, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    if ( st_Container.n_InfoLen )
    {
        memcpy(&g_CMBSMaxTransferSize, st_Container.ch_Info, CMBS_PARAM_MAX_TRANSFER_SIZE_LENGTH);
    }

    if ( g_CMBSMaxTransferSize == 0 )
    {
        // can happen if the target is old and does not support the CMBS_PARAM_MAX_TRANSFER_SIZE parameter
        APPCMBS_INFO(("g_CMBSMaxTransferSize setting to default %u", DEFAULT_MAX_TRANSFER_SIZE));
        g_CMBSMaxTransferSize = DEFAULT_MAX_TRANSFER_SIZE;
    }
}

//		========== 	appcmbs_Initialize ===========
/*!
        \brief				 short description
        \param[in,out]		 pv_AppReference		application pointer reference
        \return				<int>       return zero if successful
*/
E_CMBS_RC appcmbs_Initialize(void *pv_AppReference, PST_CMBS_DEV pst_DevCtl, PST_CMBS_DEV pst_DevMedia, PST_CB_LOG_BUFFER pfn_log_buffer_Cb)
{
    E_CMBS_RC e_RC = CMBS_RC_OK;
    // reset application
    memset(&g_cmbsappl, 0, sizeof(g_cmbsappl));
    g_cmbsappl.pv_ApplRef = pv_AppReference;

    // initialize application
    appcall_Initialize();
    app_FacilityInit();
    // intercommunication between engine and application
#ifdef __linux__
    g_cmbsappl.n_MssgAppID = cmbs_api_MsgQCreate();
#elif WIN32
    // initialize without any timeout
    g_cmbsappl.pst_AppSyncEntry = cmbs_api_MsgQCreate((u32)GetCurrentThreadId(), 0);
#endif
    // initialize CMBS API and register callback function
    // for test application

    cmbs_api_RegisterLogBufferCb(cmbs_api_get_LogBufferCb(), pfn_log_buffer_Cb);
	cmbs_dbg_SetParseIEFunc(app_PrintIe2Log);

    if ( (e_RC = cmbs_api_Init(CMBS_MODE_MLB, pst_DevCtl, pst_DevMedia)) == CMBS_RC_OK )
    {
        APPCMBS_INFO(("APP: INFO CMBS-API started with Module version %04x\n", cmbs_api_ModuleVersionGet()));

        if ( (g_cmbsappl.pv_CMBSRef = cmbs_api_RegisterCb(&g_cmbsappl, (PFN_CMBS_API_CB)appcmbs_CallBack, 0x0100)) == NULL )
        {
            APPCMBS_WARN(("APP: WARN Can't register API callback function\n"));
        }
    }
    else
    {
        APPCMBS_ERROR(("APP: ERROR !!! Initialization failed\n"));

        return e_RC;
    }

    if ( !u8_APP_LA_Initialized )
    {
        app_LaInitialize();
        u8_APP_LA_Initialized = 1;
    }

    app_HanMWInitialize();
    return e_RC;
}
//		========== appcmbs_Cleanup ===========
/*!
        \brief		 clean up CMBS API layer and CMBS API

        \param[in]	 <none>
        \return		 <none>

*/

void appcmbs_Cleanup(void)
{
    APPCMBS_INFO(("APP: INFO Cleanup CMBS connectivity\n"));
    cmbs_api_UnInit();
#ifdef __linux
    cmbs_api_MsgQDestroy(g_cmbsappl.n_MssgAppID);
#elif WIN32
    cmbs_api_MsgQDestroy(g_cmbsappl.pst_AppSyncEntry);
#endif
	List_Close();

}
//		========== app_ResponseCheck ===========
/*!
        \brief		check the IE response item and return response code
        \param[in]  pv_List	IE list pointer
        \return		<s8>  return response code TRUE/FALSE or -1 in case not finding IE
*/
s8 app_ResponseCheck(void *pv_List)
{
    void *pv_IE;
    u16 u16_IE;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_RESPONSE )
        {
            ST_IE_RESPONSE st_Response;

            // check response code:
            cmbs_api_ie_ResponseGet(pv_IE, &st_Response);

            return st_Response.e_Response;
        }
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

    return -1;
}

//		========== appcmbs_PrepareRecvAdd  ===========
/*!
        \brief		set token to pass received infromation to upper layer
        \param[in] 	u32_Token		 TRUE for passing
        \return	  	<none>

*/

void appcmbs_PrepareRecvAdd(u32 u32_Token)
{
    g_cmbsappl.n_Token = (int)u32_Token;
}
//		========== appcmbs_WaitForContainer ===========
/*!
        \brief			Synchronization function enables the application to wait until
                        requested CMBS event was received.
        \param[in]		n_Event          wait for this CMBS event
        \param[in,out]	 pst_Container		pointer to CMBS information contatiner
        \return			<int>            return TRUE, if received, otherwise FALSE on error
      \todo           timeout handling has to be integrated
*/

int appcmbs_WaitForContainer(int n_Event, PST_APPCMBS_CONTAINER pst_Container)
{
#ifdef __linux__
    int nRetVal;
    int bo_Run = TRUE;
    ST_APPCMBS_LINUX_CONTAINER LinuxContainer;
	
 	g_cmbsappl.n_Token = 1;
    
	while (bo_Run)
    {
        nRetVal = msgrcv(g_cmbsappl.n_MssgAppID, &LinuxContainer, sizeof(ST_APPCMBS_CONTAINER), 0, 0);
        if ( nRetVal == -1 )
        {
            g_cmbsappl.n_Token = 0;
            bo_Run = FALSE;
        }
        else if ( n_Event == LinuxContainer.Content.n_Event )
        {
            if ( pst_Container )
                memcpy(pst_Container, &LinuxContainer.Content, nRetVal);
            g_cmbsappl.n_Token = 0;
            return TRUE;
        }

        // timeout handler
    }




#elif WIN32
    u32 u32_MssgID;
    u16 u16_ParamSize;
    void *pv_Param;
    int bo_Run = TRUE;
	
	g_cmbsappl.n_Token = 1;

    while (bo_Run)
    {
        if ( E_CFR_INTERPROCESS_MSSG == cfr_MQueueGet(g_cmbsappl.pst_AppSyncEntry, &u32_MssgID, &pv_Param, &u16_ParamSize) )
        {
            if ( u32_MssgID == CFR_CMBS_MSSG_APPSYNC )
            {
                PST_APPCMBS_CONTAINER pstContainer = (PST_APPCMBS_CONTAINER)pv_Param;

                if ( n_Event == pstContainer->n_Event )
                {
                    if ( pst_Container )
                        memcpy(pst_Container, pstContainer, u16_ParamSize);
                    g_cmbsappl.n_Token = 0;

                    cfr_MQueueMssgFree(g_cmbsappl.pst_AppSyncEntry, pv_Param);
                    return TRUE;
                }
                else
                {
                    cfr_MQueueMssgFree(g_cmbsappl.pst_AppSyncEntry, pv_Param);
                }
            }
            else
            {
                // later check also for destroy message
                cfr_MQueueMssgFree(g_cmbsappl.pst_AppSyncEntry, pv_Param);
                return TRUE;
            }
        }
        else
        {
            bo_Run = FALSE;
        }
    }
#endif // __linux__
    return FALSE;
}

int appcmbs_WaitForEvent(int n_Event)
{
    return appcmbs_WaitForContainer(n_Event, NULL);
}

int appcmbs_ObjectSignalTrace(void)
{
#ifdef __linux__
    struct msqid_ds ds;
    int len = -1;

    memset(&ds, 0, sizeof(ds));

    if (msgctl(g_cmbsappl.n_MssgAppID, IPC_STAT, &ds) == 0)
	len = ds.msg_qnum;

    return len;
#else
    return -1;
#endif
}

//		========== appcmbs_ObjectSignal ===========
/*!
        \brief          signal to application of received event
        \param[in]		 psz_Info      pointer to information buffer
        \param[in]		 n_InfoLen		length of information
        \param[in]		 n_Info		   additional information, e.g. IE element
        \param[in]		 n_Event		   received CMBS event
        \return			<none>
*/
void appcmbs_ObjectSignal(char *psz_Info, int n_InfoLen, int n_Info, int n_Event)
{
#ifdef __linux__
    int nRetVal;

    if ( g_cmbsappl.n_MssgAppID >= 0 )
    {
        ST_APPCMBS_LINUX_CONTAINER LinuxContainer;

        LinuxContainer.mType = 1;
        memcpy(LinuxContainer.Content.ch_Info, psz_Info, n_InfoLen);
        LinuxContainer.Content.n_InfoLen = n_InfoLen;
        LinuxContainer.Content.n_Event = n_Event;
        LinuxContainer.Content.n_Info = n_Info;

        nRetVal = msgsnd(g_cmbsappl.n_MssgAppID, &LinuxContainer, (3 * (sizeof(int)) + n_InfoLen), 0);

        if ( nRetVal == -1 )
        {
            APPCMBS_ERROR(("App:ERROR !!! Container Object was not sent! %d\n",errno));
        }
    }
#elif WIN32
    if ( g_cmbsappl.pst_AppSyncEntry )
    {
        PST_APPCMBS_CONTAINER pst_Container;

        pst_Container = (PST_APPCMBS_CONTAINER)malloc(sizeof(ST_APPCMBS_CONTAINER));

        if ( pst_Container )
        {
            memset(pst_Container, 0, sizeof(ST_APPCMBS_CONTAINER));

            if ( psz_Info )
            {
                memcpy(pst_Container->ch_Info, psz_Info, n_InfoLen);
            }

            pst_Container->n_InfoLen = n_InfoLen;
            pst_Container->n_Event = n_Event;
            pst_Container->n_Info = n_Info;

            cfr_MQueueSend(g_cmbsappl.pst_AppSyncEntry, CFR_CMBS_MSSG_APPSYNC, (void *)pst_Container, 3 * sizeof(int) + n_InfoLen);
        }
        free(pst_Container);
    }

#endif // __linux__
}
//		========== appcmbs_IEInfoGet  ===========
/*!
        \brief				handles general to IE function
        \param[in]		   pv_IE		 IE  buffer
        \param[in]		   u16_IE	 enumeration of IE
        \param[in,out]		p_Info	 pointer to IE object
        \return				<none>
*/
void appcmbs_IEInfoGet(void *pv_IE, u16 u16_IE, PST_APPCMBS_IEINFO p_Info)
{
    switch (u16_IE)
    {
    case CMBS_IE_CALLERNAME:
        cmbs_api_ie_CallerNameGet(pv_IE, &p_Info->Info.st_CallerName);
        break;

    case CMBS_IE_CALLERPARTY:
        cmbs_api_ie_CallerPartyGet(pv_IE, &p_Info->Info.st_CallerParty);
        break;
    case CMBS_IE_CALLEDPARTY:
        cmbs_api_ie_CalledPartyGet(pv_IE, &p_Info->Info.st_CalledParty);
        break;

    case CMBS_IE_CALLINSTANCE:
        cmbs_api_ie_CallInstanceGet(pv_IE, &p_Info->Info.u32_CallInstance);
        break;

    case CMBS_IE_CALLPROGRESS:
        cmbs_api_ie_CallProgressGet(pv_IE, &p_Info->Info.st_CallProgress);
        break;

    case CMBS_IE_CALLINFO:
        cmbs_api_ie_CallInfoGet(pv_IE, &p_Info->Info.st_CallInfo);
        break;

    case CMBS_IE_MEDIACHANNEL:
        cmbs_api_ie_MediaChannelGet(pv_IE, &p_Info->Info.st_MediaChannel);
        break;

    case CMBS_IE_MEDIADESCRIPTOR:
        cmbs_api_ie_MediaDescGet(pv_IE, &p_Info->Info.st_MediaDesc);
        break;

    case CMBS_IE_CALLRELEASE_REASON:
        cmbs_api_ie_CallReleaseReasonGet(pv_IE, &p_Info->Info.st_Reason);
        break;

    case CMBS_IE_PARAMETER:
        cmbs_api_ie_ParameterGet(pv_IE, &p_Info->Info.st_Param);
        break;

    case CMBS_IE_FW_VERSION:
        cmbs_api_ie_FwVersionGet(pv_IE, &p_Info->Info.st_FwVersion);
        break;

    case CMBS_IE_HW_VERSION:
        cmbs_api_ie_HwVersionGet(pv_IE, &p_Info->Info.st_HwVersion);
        break;
    case CMBS_IE_EEPROM_VERSION:
        cmbs_api_ie_EEPROMVersionGet(pv_IE, &p_Info->Info.st_EEPROMVersion);
        break;
    case CMBS_IE_SYS_LOG:
        cmbs_api_ie_SysLogGet(pv_IE, &p_Info->Info.st_SysLog);
        break;

    case CMBS_IE_SUBSCRIBED_HS_LIST:
        cmbs_api_ie_SubscribedHSListGet(pv_IE, &p_Info->Info.st_SubscribedHsList);
        break;

    case CMBS_IE_LINE_SETTINGS_LIST:
        cmbs_api_ie_LineSettingsListGet(pv_IE, &p_Info->Info.st_LineSettingsList);
        break;

    case CMBS_IE_RESPONSE:
        cmbs_api_ie_ResponseGet(pv_IE, &p_Info->Info.st_Resp);
        break;

    case CMBS_IE_LINE_ID:
        cmbs_api_ie_LineIdGet(pv_IE, &p_Info->Info.u8_LineId);
        break;

    case CMBS_IE_MELODY:
        cmbs_api_ie_MelodyGet(pv_IE, &p_Info->Info.u8_Melody);
        break;

    case CMBS_IE_DECT_SETTINGS_LIST:
        cmbs_api_ie_DectSettingsListGet(pv_IE, &p_Info->Info.st_DectSettings);
        break;

    case CMBS_IE_PROP_FIELDS_LIST:
        cmbs_api_ie_PropDectSettingsListGet(pv_IE, &p_Info->Info.st_PropSettings);
        break;

    case CMBS_IE_CALL_LIST:
        cmbs_api_ie_CallListGet(pv_IE, &p_Info->Info.st_CallList);
        break;		

    default:
        if ( CMBS_IE_USER_DEFINED_START <= u16_IE && u16_IE <= CMBS_IE_USER_DEFINED_END )
        {
            break;
        }
        APPCMBS_WARN(("APP: WARN IE_ToString: IE:%d not implemented\n", u16_IE));
    }

}

void appcmbs_VersionGet(char *pc_Version)
{
	char ch_RcBuild[20];
    u16 u16_Version = cmbs_api_ModuleVersionGet();
    u16 u16_Build = cmbs_api_ModuleVersionBuildGet();

	if (u16_Build>>8)
	{
		sprintf (ch_RcBuild,"- RC %u", (u16_Build>>8));
	}
	else
	{
		ch_RcBuild[0] = '\0';
	}

    if ( u16_Version > 0xFFF )
        sprintf(pc_Version, "Version %02x.%03x - Build %hu %s", (u16_Version >> 12), (u16_Version & 0xFFF), (u16_Build & 0xFF), ch_RcBuild);
    else
        sprintf(pc_Version, "Version %02x.%02x - Build %hu %s", (u16_Version >> 8), (u16_Version & 0xFF), (u16_Build & 0xFF), ch_RcBuild);
}

unsigned long appcmbs_GetTickCount(void)
{
#ifdef WIN32
    return GetTickCount();
#else //__linux__
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return ((unsigned long)tv.tv_sec*1000);
#endif
}

int appcmbs_ReconnectApplication(unsigned long ulTimeoutMs)
{
    u32 u32_Sec, i;
    int resp;
    u8 u8_HwChip_temp, u8_HwComType_temp;

    //effi - need this to re-regsiter the log callbacks
    ST_CB_LOG_BUFFER * pfnLogCallbacks=cmbs_api_get_LogBufferCb();

    cmbs_api_UnInit();

    if ( ulTimeoutMs )
    {
        u32_Sec = ulTimeoutMs / 1000;

        for (i = 0; i <= u32_Sec; ++i)
        {
            printf("\r%d sec. remaining...", u32_Sec - i);
            SleepMs(1000);
        }
    }

    APPCMBS_INFO(("\n"));

    // restore USB port (windows only), it could be changed during target init (different RFPI or reinit EEPROM)
#if defined ( WIN32 )
    if ( g_st_DevCtl.e_DevType == CMBS_DEVTYPE_USB )
    {
        u8 u8_Port;
        E_CMBS_DEVTYPE e_type;

        u8_Port = tcx_DetectComPort(FALSE, &e_type); /* non interactive mode */
        if ( u8_Port )
        {
            tcx_USBConfig(u8_Port, g_u32_BaudRate);
        }
    }
#endif

    // Back and restore u8_HwChip and u8_HwComType since cmbs_api_Init cleans them
    u8_HwChip_temp = appcmbs_get_CMBSAppInstance()->u8_HwChip;
    u8_HwComType_temp = appcmbs_get_CMBSAppInstance()->u8_HwComType;

    resp = cmbs_api_Init(CMBS_MODE_MLB, &g_st_DevCtl, &g_st_DevMedia);
    cmbs_api_RegisterCb(&g_cmbsappl, (PFN_CMBS_API_CB)appcmbs_CallBack, 0x0100);

    // Effi - need to reregister the log functions
    // Register callback functions for log buffer handling
    cmbs_api_RegisterLogBufferCb(&g_cmbsappl, pfnLogCallbacks);
    
    cmbs_dbg_SetParseIEFunc(app_PrintIe2Log);

    appcmbs_get_CMBSAppInstance()->u8_HwChip = u8_HwChip_temp;
    appcmbs_get_CMBSAppInstance()->u8_HwComType = u8_HwComType_temp;

    if ( resp == CMBS_RC_OK )
    {
        cmbs_dsr_sys_Start(g_cmbsappl.pv_CMBSRef, NULL);
    }
    else
    {
        APPCMBS_ERROR(("appcmbs_Initialize failure %d!!!", resp));
    }
    return resp;
}

int app_CmdEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData)
{
    UNUSED_PARAMETER(pv_AppRef);
    UNUSED_PARAMETER(pv_EventData);
    // return TRUE for all commands
    return (e_EventID & CMBS_CMD_MASK) == CMBS_CMD_MASK;
}

#ifdef CMBS_PLUGIN
int app_PluginEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData)
{
    return CMBS_HandleEventPlugins(pv_AppRef, e_EventID, pv_EventData);
#if 0
    if( e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_OPEN ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_OPEN_RES ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_CLOSE ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_CLOSE_RES ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SEND ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SEND_RES )
    {
        return TRUE;
    }
    else
    return FALSE;
#endif
}
#endif
#ifdef WIN32
void app_CreateThread(LPTHREAD_START_ROUTINE pfn)
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pfn, 0, 0, NULL);
}
#elif __linux__
void app_CreateThread(start_routine pfn)
{
    pthread_t uTempThread;
    pthread_create( &uTempThread, NULL, pfn, (void*) NULL);
}
#endif

//*/
