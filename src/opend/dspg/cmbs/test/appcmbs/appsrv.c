/*!
*	\file		appservice.c
*	\brief		handles the service relevant interfaces
*	\Author		kelbch
*
*	@(#)	%filespec: appsrv.c~---GIT-- %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
* date        name       action                                          \n
*  ---------------------------------------------------------------------------
* 15-Apr-2015 tcmc_asa  Defect 14073: added app_SrvPropDectSettingsSet
* 15-Apr-2015 tcmc_asa  Defect 14073: added app_SrvSetNewHandsetNameAndLinePref()
* 25-Mar-2015 tcmc_asa  correct call of cmbs_dsr_cord_OpenRegistration
* 23-Mar-2015 tcmc_asa  added app_SrvSubscriptionOpenExt
* 19-Mar-2015 tcmc_asa  added parameters to cmbs_dsr_cord_OpenRegistration
* 21-Nov-2014 tcmc_asa  added parameters to app_SrvHandsetStopPaging
* 17-Oct-2014 tcmc_asa  ---GIT-- added app_SrvHandsetDeleteINL()
* 13-Jan-2014 tcmc_asa  -- GIT--  take checksum changes from 3.46.x to 3_x main (3.5x)
* 20-Dec-2013 tcmc_asa  --GIT--      added app_OnCheckSumFailure
* 30-Nov-2012 tcmc_asa 31.1.16.1.3   removed app_SrvSendCurrentDateAndTime 
*                                      in app_OnHandsetRegistered
* 26-Oct-2011 tcmc_asa 31.1.8        merged 2 versions 31.1.7
* 26-Oct-2011 tcmc_asa NBGD53#31.1.7 added app_SrvFixedCarrierSet
*  14-Dec-09        sergiym     ?        Add start/stop log commands 
*                   Kelbch      1        Initialize 
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
#include <pthread.h>
#endif

#include "cmbs_util.h"
#include "cmbs_api.h"
#include "cfr_ie.h"
#include "cfr_mssg.h"
#include "cmbs_dbg.h"
#include "cmbs_int.h"
#include "appcmbs.h"
#include "appsrv.h"
#include "appsuotafile.h"
#include "appcall.h"
#include "tcx_eep.h"
#include "tcx_log.h"
#include "tcx_util.h"
#include "ListsApp.h"
#include "ListChangeNotif.h"
#include "appmsgparser.h"
#include "cmbs_event.h"
#include "applistacc.h"
#ifdef CMBS_COMA
#include "cfr_coma.h"
#endif
#include "cmbs_han_ie.h"
#include "appfacility.h"
#include "appsuotafile.h"

#include "tcx_hostlog.h"
#define APP_SRV_INFO_PRINT(format, ...) 						tcx_WriteLog(SRV_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define APP_SRV_TRACE_PRINT(format, ...) 						tcx_WriteLog(SRV_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define APP_SRV_WARNING_PRINT(format, ...) 						tcx_WriteLog(SRV_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define APP_SRV_ERROR_PRINT(format, ...) 						tcx_WriteLog(SRV_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define APP_SRV_RT_PRINT(format, ...) 							tcx_WriteLog(SRV_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define APP_SRV_PRINT_DATA(pu8_Buffer, u16_Length) 				tcx_WriteLogData(SRV_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)

/* Forward Declaration */
extern void app_OnHsLocProgress(void *pvAppRefHandle, void *pv_List);
extern void app_OnHsLocAnswer(void *pvAppRefHandle, void *pv_List);
extern void app_OnHsLocRelease(void *pvAppRefHandle, void *pv_List);
extern E_CMBS_RC cmbs_api_ie_HsNumberGet(void *pv_RefIE, u8 *pu8_HsNumber);
void        app_onDisconnectReq(void);
void        app_onTargetUp(void);
void        app_OnEepromSizeResp(void *pv_List);
void 		app_OnEepromExtSizeResp(void *pv_List);
extern ST_CMBS_DEV       g_st_DevMedia;
extern ST_CMBS_DEV       g_st_DevCtl;
void  appOnTerminalCapabilitiesInd(void *pv_List);
void  appOnPropritaryDataRcvInd(void *pv_List);
#ifdef PNCAP
void appProperitaryDataReceived(u16 u16_HsNo, u8 *pu8_Data, u16 u16_DataLen);
#endif
void appOnSetLineTestRes(void *pv_List);
void appOnSrvNltCapTestRes(void *pv_List);
void appOnFxsRingTestStartRes(void *pv_List);
void appOnFxsRingTestStopRes(void *pv_List);

static E_CMBS_RC app_SrvSendCurrentDateAndTime(u8 u8_HsNum);
char           _app_HexString2Byte(char *psz_HexString);

int p_HanServer_OnOpenRegistrationRes( int succeess );
int p_HanServer_OnCloseRegistrationRes( int succeess );
void app_HanStartAndConfig( void );
void app_HANRestoreSequenceNumbers (void);

void app_HanRegularStart( u8 RegisterForHanMessages );

void app_OnSrvFxsStatus(void *pv_List);

void app_SrvEEPROMHanBackupCreate(CMBS_EEPROM_DATA* backup);
void app_SrvEEPROMRepeaterBackupCreate(u32 u32_Repeater_start, u32 u32_Repeater_size);
E_CMBS_RC app_SrvEEPROMHanBackupRestore(CMBS_EEPROM_DATA* st_backup);
E_CMBS_RC app_SrvEEPROMRepeaterBackupRestore(u32 u32_Repeater_start, u32 u32_Repeater_end);



bool b_EepromOnHost = FALSE;
bool b_RestoreEepOnly = FALSE;
bool b_RestoreGroupEep = FALSE;

//  ** for EEPROM on host **
typedef enum
{
    EEP_ON_HOST_INIT,
    EEP_ON_HOST_READING_EEP,
    EEP_ON_HOST_READ_COMPLETED
} E_EEP_ON_HOST_STATE;

E_EEP_ON_HOST_STATE e_EepOnHostState = EEP_ON_HOST_INIT;

extern E_CMBS_RC GetSMSIdByLineID(IN u32 u32_LineId,  OUT u32 *pu32_SMSId); 
extern u32  g_CMBSMaxTransferSize;



#ifdef CMBS_COMA
static void app_CSSCOMAReset(void);
#endif // CMBS_COMA
//		========== app_ASC2HEX  ===========
/*!
        \brief				 convert a two digits asc ii string to a hex value

        \param[in,out]		 pu8_Digits		 pointer to two digits string

        \return				u8              return value of convertion

*/

u8             app_ASC2HEX(char *psz_Digits)
{
    u8 u8_Value = 0;
    int i = 0, j = 1;
    if ( strlen(psz_Digits) < 2 )
        j = 0;

    for (i = 0; (i < 2) && (j >= 0); i++)
    {
        if ( psz_Digits[i] >= '0' && psz_Digits[i] <= '9' )
        {
            u8_Value |= (psz_Digits[i] - '0') << 4 * j;
        }
        else if ( psz_Digits[i] >= 'a' && psz_Digits[i] <= 'f' )
        {
            u8_Value |= (0x0a + (psz_Digits[i] - 'a')) << 4 * j;
        }
        else if ( psz_Digits[i] >= 'A' && psz_Digits[i] <= 'F' )
        {
            u8_Value |= (0x0a + (psz_Digits[i] - 'A')) << 4 * j;
        }
        j--;
    }

    return u8_Value;
}

//		========== app_OnPageStarted ===========
/*!
        \brief			Paging started
        \param[in,out]		 *pv_List		 IE list
        \return			<none>
*/
void app_OnPageStarted(void *pv_List)
{
    UNUSED_PARAMETER(pv_List);

    cmbsevent_OnPageStarted();
}

//		========== app_OnPageStoped ===========
/*!
        \brief			Paging stopped
        \param[in,out]		 *pv_List		 IE list
        \return			<none>
*/
void app_OnPageStoped(void *pv_List)
{
    UNUSED_PARAMETER(pv_List);

    cmbsevent_OnPageStoped();
}

//		========== app_OnHandsetRegistered ===========
/*!
        \brief			a new handset is registered OR a handset was deleted.
        \param[in,out]		 *pv_List		 IE list
        \return			<none>
*/
void app_OnHandsetRegistered(void *pv_List)
{
    ST_IE_HANDSETINFO st_HsInfo;
    ST_IE_RESPONSE st_Res;
    void *pv_IE = NULL;
    u16      u16_IE;

    memset(&st_HsInfo, 0, sizeof(ST_IE_HANDSETINFO));

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    cmbs_api_ie_HandsetInfoGet(pv_IE, &st_HsInfo);
    cmbs_api_ie_ResponseGet(pv_IE, &st_Res);

    if ( st_Res.e_Response == CMBS_RESPONSE_OK )
    {
        if ( st_HsInfo.u8_State == CMBS_HS_REG_STATE_REGISTERED )
        {
            APP_SRV_INFO_PRINT("APPSRV-INFO: Handset:%d IPUI:%02X%02X%02X%02X%02X Type:%s registered\n",
                          st_HsInfo.u8_Hs,
                          st_HsInfo.u8_IPEI[0], st_HsInfo.u8_IPEI[1], st_HsInfo.u8_IPEI[2],
                          st_HsInfo.u8_IPEI[3], st_HsInfo.u8_IPEI[4],
                          cmbs_dbg_GetHsTypeName(st_HsInfo.e_Type));

            List_AddHsToFirstLine(st_HsInfo.u8_Hs);

            /* Send Date & Time Update */
            // ASA Nov 12: Don't send FACILITY to the handset before it finished Access rights and Location registration,
            // Otherwise there is an issue with the instance use (default PMIT in MBCRouteTbl causes use
            // on new instance, although handset has already an acive link. PR ILD53#3681
            // app_SrvSendCurrentDateAndTime( st_HsInfo.u8_Hs );
        }
        else if ( st_HsInfo.u8_State == CMBS_HS_REG_STATE_UNREGISTERED )
        {
            APP_SRV_INFO_PRINT("APPSRV-INFO: Handset:%d Unregistered\n", st_HsInfo.u8_Hs);

            List_RemoveHsFromAllLines(st_HsInfo.u8_Hs);
        }

        // if token is send signal to upper application
        if ( g_cmbsappl.n_Token )
        {
            appcmbs_ObjectSignal((void *)&st_HsInfo, sizeof(st_HsInfo), CMBS_IE_HANDSETINFO, CMBS_EV_DSR_HS_REGISTERED);
        }
    }
    else
    {
        APP_SRV_ERROR_PRINT("\n[%s] ERROR!\n", __FUNCTION__);
    }

    cmbsevent_OnHandsetRegistered(st_HsInfo.u8_Hs);
}


//		========== app_OnHandsetInRange ===========
/*!
        \brief			 handset in range
        \param[in,out]		 *pv_List		 IE list
        \return			<none>
*/
void           app_OnHandsetInRange(void *pv_List)
{
    void *pv_IE = NULL;
    u16      u16_IE, u16_HS=0;
    u8       u8_Hs;
    u32      u32_temp;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: HS num
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        cmbs_api_ie_IntValueGet(pv_IE, &u32_temp);
        u8_Hs = (u8)u32_temp;

        APP_SRV_INFO_PRINT("APPSRV-INFO: Handset:%d in range\n", u8_Hs);

        cmbsevent_OnHandsetInRange(u8_Hs);

        /* Send Date & Time Update */
        app_SrvSendCurrentDateAndTime(u8_Hs);

        /* Send missed call notification */
        {
            u32 pu32_Lines[APPCALL_LINEOBJ_MAX], u32_NumLines = APPCALL_LINEOBJ_MAX, u32_i;
            LIST_RC list_rc;

            list_rc = List_GetLinesOfHS(u8_Hs, pu32_Lines, &u32_NumLines);
            if ( list_rc != LIST_RC_OK )
            {
                APP_SRV_ERROR_PRINT("app_OnHandsetInRange - Failed getting list of lines... cannot send missed call notification! \n");
                return;
            }

            for (u32_i = 0; u32_i < u32_NumLines; ++u32_i)
            {
                u32 u32_SMSId = 0;
                // Send facility message only to this Hs
                ListChangeNotif_MissedCallListChanged(pu32_Lines[u32_i], FALSE, u8_Hs);
#ifdef CATIQ_2_1
				u16_HS |= (1 <<(u8_Hs - 1));
                app_FacilityHSUseIndication(0, (u8)pu32_Lines[u32_i], u16_HS);
                app_FacilityDiagnosticIndication(0, (u8)pu32_Lines[u32_i], CMBS_LINE_RELATED_CHANGE, u16_HS);
                app_FacilityLineUseIndication(0, (u8)pu32_Lines[u32_i], LineAndDiagnosticListGetLineUseStatus((u8)pu32_Lines[u32_i]), u16_HS);

                if ( GetSMSIdByLineID(pu32_Lines[u32_i], &u32_SMSId) == CMBS_RC_OK )
                {
                ListChangeNotif_IncomingSMSListChanged(u32_SMSId, CMBS_SMS_OTHER_LIST_CHANGE, u16_HS);
                }
#endif
			}
        }
    }
}


//		========== app_OnDateTimeIndication ===========
/*!
        \brief			 handset set Date/Time
        \param[in,out]		 *pv_List		 IE list
        \return			<none>
*/
void           app_OnDateTimeIndication(void *pv_List)
{
    void *pv_IE = NULL;
    u16           u16_IE;
    u16           u16_temp;
    ST_DATE_TIME  st_DateTime;
    u8            u8_Hs;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: DateTime and HS num
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        cmbs_api_ie_DateTimeGet(pv_IE, &st_DateTime);
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        cmbs_api_ie_HandsetsGet(pv_IE, &u16_temp);
        u8_Hs = (u8)u16_temp;

        APP_SRV_INFO_PRINT("APPSRV-INFO: Handset: %d set DateTime: %02d.%02d.%02d %02d:%02d:%02d\n", u8_Hs,
                      st_DateTime.u8_Day, st_DateTime.u8_Month, st_DateTime.u8_Year,
                      st_DateTime.u8_Hours, st_DateTime.u8_Mins, st_DateTime.u8_Secs);

		app_SrvSetValidDateTime(1);
		// TODO: Set received Date Time as system time. Received date & time is sent to all handset by target already.
    }
}


//		========== app_OnNoValidDateTimeAvailable ===========
/*!
        \brief			 handset set Date/Time
        \param[in,out]		 *pv_List		 IE list
        \return			<none>
*/
void           app_OnNoValidDateTimeAvailable(void *pv_List)
{
    void *pv_IE = NULL;
    u16           u16_IE;
    u16           u16_temp;
    u8            u8_Hs;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: DateTime and HS num
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        cmbs_api_ie_HandsetsGet(pv_IE, &u16_temp);
        u8_Hs = (u8)u16_temp;

        APP_SRV_INFO_PRINT("APPSRV-INFO: Handset: %d No Valid Date Time Available\n", u8_Hs);
    }
}



//		========== app_OnRegistrationOpenRSP ===========
/*!
        \brief			response of open registration event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnRegistrationOpenRSP(void *pv_List)
{
    int success = FALSE;

    if ( !app_ResponseCheck(pv_List) )
    {
        success = TRUE;

        APP_SRV_INFO_PRINT("APPSRV-INFO: Registration Open successful\n");

        cmbsevent_OnRegistrationOpened();

        g_cmbsappl.RegistrationWindowStatus = 1;
    }
    else
    {
        APP_SRV_ERROR_PRINT("APPSRV-ERROR: !!! Registration Open failed\n");
    }


	if ( g_cmbsappl.n_Token )
    {
        appcmbs_ObjectSignal(NULL, 0, success, CMBS_EV_DSR_CORD_OPENREG_RES);
    }
}

//		========== app_OnRegistrationClose ===========
/*!
        \brief			Notification from target on registration close and the reason for it
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnRegistrationClose(void *pv_List)
{

    void *pv_IE = NULL;
    u16           u16_IE;
    ST_IE_REG_CLOSE_REASON  st_RegCloseReason;

    if ( pv_List )
    {
        // collect information elements. we expect: CMBS_IE_REG_CLOSE_REASON
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {

            if ( u16_IE == CMBS_IE_REG_CLOSE_REASON )
            {
                // check registration close reason:
                cmbs_api_ie_RegCloseReasonGet(pv_IE, &st_RegCloseReason);
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
    else
    {
        APP_SRV_ERROR_PRINT("APPSRV-ERROR: !!! Registration Close failed\n");
    }

    if ( g_cmbsappl.n_Token )
    {
        appcmbs_ObjectSignal(NULL, 0, 0, CMBS_EV_DSR_CORD_CLOSEREG);
    }

    g_cmbsappl.RegistrationWindowStatus = 0;
}


//		========== app_OnRegistrationCloseRSP ===========
/*!
        \brief			response of close registration event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/

void           app_OnRegistrationCloseRSP(void *pv_List)
{
    int success = FALSE;

    if ( !app_ResponseCheck(pv_List) )
    {
        success = TRUE;

        APP_SRV_INFO_PRINT("APPSRV-INFO: Registration Closed successfully by Host\n");

        cmbsevent_OnRegistrationClosed();
    }
    else
    {
        APP_SRV_ERROR_PRINT("APPSRV-ERROR: !!! Registration Close failed\n");
    }


	if ( g_cmbsappl.n_Token )
    {
        appcmbs_ObjectSignal(NULL, 0, success, CMBS_EV_DSR_CORD_CLOSEREG_RES);
    }

    g_cmbsappl.RegistrationWindowStatus = 0;
}


//		========== app_OnParamGetRSP ===========
/*!
        \brief			response of param get event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnParamGetRSP(void *pv_List)
{
    void *pv_IE = NULL;
    u16      u16_IE;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_PARAMETER + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            if ( CMBS_IE_PARAMETER == u16_IE )
            {
                ST_IE_PARAMETER st_Param;

                cmbs_api_ie_ParameterGet(pv_IE, &st_Param);
                // signal parameter setting to upper application
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)st_Param.pu8_Data, st_Param.u16_DataLen, st_Param.e_Param, CMBS_EV_DSR_PARAM_GET_RES);
                }
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

//		========== app_OnParamSetRSP ===========
/*!
        \brief			response of param set event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnParamSetRSP(void *pv_List)
{
    void *pv_IE;
    u16         u16_IE;
    int         i = 0;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_PARAMETER + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            if ( u16_IE == CMBS_IE_PARAMETER )
            {
                APP_SRV_INFO_PRINT("APPSRV-INFO: Param \"");
                app_IEToString(pv_IE, u16_IE);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
        // signal to upper application only IE repsonse info
        if ( app_ResponseCheck(pv_List) == CMBS_RESPONSE_OK )
        {
            i = TRUE;

            APP_SRV_INFO_PRINT("\" set successful\n");
        }
        else
        {
            APP_SRV_ERROR_PRINT("\" set failed !!!\n");
        }

        if ( g_cmbsappl.n_Token )
        {
            appcmbs_ObjectSignal(NULL, 0, i, CMBS_EV_DSR_PARAM_SET_RES);
        }

    }

}

//		========== app_OnParamAreaGetRSP ===========
/*!
        \brief			response of param area get event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnParamAreaGetRSP(void *pv_List)
{
    void *pv_IE = NULL;
    u16      u16_IE;
    ST_IE_PARAMETER_AREA st_ParamArea;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_PARAMETER_AREA + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            if ( CMBS_IE_PARAMETER_AREA == u16_IE )
            {
                cmbs_api_ie_ParameterAreaGet(pv_IE, &st_ParamArea);
                // signal parameter setting to upper application
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)st_ParamArea.pu8_Data, st_ParamArea.u16_DataLen,
                                         st_ParamArea.e_AreaType, CMBS_EV_DSR_PARAM_AREA_GET_RES);
                }
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

//		========== app_OnParamAreaSetRSP ===========
/*!
        \brief			response of param area set event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnParamAreaSetRSP(void *pv_List)
{
    void *pv_IE;
    u16         u16_IE;
    int         i = 0;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            if ( u16_IE == CMBS_IE_PARAMETER_AREA )
            {
                APP_SRV_INFO_PRINT("APPSRV-INFO: Param \"");
                app_IEToString(pv_IE, u16_IE);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
        // signal to upper application only IE repsonse info
        if ( app_ResponseCheck(pv_List) == CMBS_RESPONSE_OK )
        {
            i = TRUE;

            APP_SRV_INFO_PRINT("\" set successful\n");
        }
        else
        {
            APP_SRV_ERROR_PRINT("\" set failed !!!\n");
        }

        if ( g_cmbsappl.n_Token )
        {
            appcmbs_ObjectSignal(NULL, 0, i, CMBS_EV_DSR_PARAM_AREA_SET_RES);
        }

    }
}


//		========== app_OnParamAreaGet ===========
/*!
        \brief			target requests data from the host EEPROM file 
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnParamAreaGet(void *pv_List)
{
    ST_IE_PARAMETER_AREA
       st_ParamArea;
    void *pv_IE;
    u16            u16_IE;
    u8             u8_Buffer[CMBS_PARAM_AREA_MAX_SIZE];
    u32            u32_AreaSize = 0;
    u8             bo_OK = TRUE;

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            if ( u16_IE == CMBS_IE_PARAMETER_AREA )
            {
                ST_IE_RESPONSE   st_Resp;

                cmbs_api_ie_ParameterAreaGet(pv_IE, &st_ParamArea);

                APP_SRV_INFO_PRINT("APPSRV-INFO: OnParamAreaGet: ");
                APPCMBS_IE(("Param_Area=%d, Offset=%d, Length=%d\n", st_ParamArea.e_AreaType, st_ParamArea.u32_Offset, st_ParamArea.u16_DataLen));

                if ( st_ParamArea.e_AreaType == CMBS_PARAM_AREA_TYPE_EEPROM )
                {
                    u32_AreaSize = tcx_EepSize();
                }
                else
                {
                    APP_SRV_WARNING_PRINT("APPSRV-WARN: Get Param area type:%d not supported\n", st_ParamArea.e_AreaType);
                    bo_OK = FALSE;
                }

                if ( st_ParamArea.u32_Offset + st_ParamArea.u16_DataLen > u32_AreaSize )
                {
                    APP_SRV_ERROR_PRINT("APPSRV-ERROR: Get Param area requested size bigger than memory area size\n");
                    bo_OK = FALSE;
                }

                if ( bo_OK == FALSE )
                {
                    st_ParamArea.pu8_Data    = NULL;
                    st_ParamArea.u16_DataLen = 0;
                    st_Resp.e_Response       = CMBS_RESPONSE_ERROR;
                }
                else
                {
                    if ( st_ParamArea.e_AreaType == CMBS_PARAM_AREA_TYPE_EEPROM )
                    {
                        tcx_EepRead(u8_Buffer, st_ParamArea.u32_Offset, st_ParamArea.u16_DataLen);
                        st_ParamArea.pu8_Data = u8_Buffer;
                        st_Resp.e_Response = CMBS_RESPONSE_OK;

                        e_EepOnHostState = EEP_ON_HOST_READING_EEP;
                    }
                }

                cmbs_dsr_ParamAreaGetRes(&st_ParamArea, &st_Resp);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}



//		========== app_OnParamAreaSet ===========
/*!
        \brief			target saves data to the host EEPROM file 
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnParamAreaSet(void *pv_List)
{
    ST_IE_PARAMETER_AREA
       st_ParamArea;
    void *pv_IE;
    u16            u16_IE;
    u32            u32_AreaSize = 0;
    u8             bo_OK = TRUE;

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            if ( u16_IE == CMBS_IE_PARAMETER_AREA )
            {
                ST_IE_RESPONSE   st_Resp;

                APP_SRV_INFO_PRINT("APPSRV-INFO: OnParamAreaSet: ");
                app_IEToString(pv_IE, u16_IE);

                cmbs_api_ie_ParameterAreaGet(pv_IE, &st_ParamArea);

                if ( st_ParamArea.e_AreaType == CMBS_PARAM_AREA_TYPE_EEPROM )
                {
                    u32_AreaSize = tcx_EepSize();
                }
                else
                {
                    APP_SRV_WARNING_PRINT("APPSRV-WARN: Set Param area type:%d not supported\n", st_ParamArea.e_AreaType);
                    bo_OK = FALSE;
                }

                if ( st_ParamArea.u32_Offset + st_ParamArea.u16_DataLen > u32_AreaSize )
                {
                    APP_SRV_ERROR_PRINT("APPSRV-ERROR: Set Param area requested size bigger than memory area size\n");
                    bo_OK = FALSE;
                }

                if ( bo_OK == FALSE )
                {
                    st_Resp.e_Response = CMBS_RESPONSE_ERROR;
                }
                else
                {
                    if ( st_ParamArea.e_AreaType == CMBS_PARAM_AREA_TYPE_EEPROM )
                    {
                        tcx_EepWrite(st_ParamArea.pu8_Data, st_ParamArea.u32_Offset, st_ParamArea.u16_DataLen);
                        st_Resp.e_Response = CMBS_RESPONSE_OK;
                    }
                }

                cmbs_dsr_ParamAreaSetRes(&st_Resp);
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}


//		========== app_OnFwVersionGetRSP ===========
/*!
        \brief			response of FW version get event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnFwVersionGetRSP(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_FW_VERSION + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        memset(&st_IEInfo.Info.st_FwVersion, 0, sizeof(st_IEInfo.Info.st_FwVersion));
        
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_FW_VERSION )
            {
                APP_SRV_INFO_PRINT("CMBS_TARGET Version %d %04x\n", st_IEInfo.Info.st_FwVersion.e_SwModule, st_IEInfo.Info.st_FwVersion.u16_FwVersion);
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_FwVersion,
                                         sizeof(st_IEInfo.Info.st_FwVersion),
                                         CMBS_IE_FW_VERSION,
                                         CMBS_EV_DSR_FW_VERSION_GET_RES);
                }

            }
}

//		========== app_OnHwVersionGetRSP ===========
/*!
        \brief			response of HW version get event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnHwVersionGetRSP(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HW_VERSION + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_HW_VERSION )
            {
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_HwVersion,
                                         sizeof(st_IEInfo.Info.st_HwVersion),
                                         CMBS_IE_HW_VERSION,
                                         CMBS_EV_DSR_HW_VERSION_GET_RES);
                }

            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

//		========== app_OnEEPROMVersionGetRSP ===========
/*!
        \brief			response of EEPROM version get event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/

void    app_OnEEPROMVersionGetRSP(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_EEPROM_VERSION + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_EEPROM_VERSION )
            {
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_EEPROMVersion,
                                         sizeof(st_IEInfo.Info.st_EEPROMVersion),
                                         CMBS_IE_EEPROM_VERSION,
                                         CMBS_EV_DSR_EEPROM_VERSION_GET_RES);
                }

            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

}

//		========== app_OnDCSessionStart ===========
/*!
        \brief			target wants to start data call
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_OnDCSessionStart(void *pv_List)
{

    void *pv_IE = NULL;
    u16      u16_IE;
    u16   u16_Handsets = 0;
    u32   u32_CallInstance = 0;


    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HANDSETS + CMBS_IE_CALLINSTANCE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        switch (u16_IE)
        {
        case CMBS_IE_HANDSETS:
            cmbs_api_ie_ShortValueGet(pv_IE, &u16_Handsets, CMBS_IE_HANDSETS);
            break;
        case CMBS_IE_CALLINSTANCE:
            cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
            break;
        default:
            break;
        }
    }
    cmbs_dsr_dc_SessionStartRes(g_cmbsappl.pv_CMBSRef, u16_Handsets, u32_CallInstance, TRUE);
}

//		========== app_OnDCSessionStartRSP ===========
/*!
        \brief			response to data call session start
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_OnDCSessionStartRSP(void *pv_List)
{
    void *pv_IE = NULL;
    u16           u16_IE;
    ST_IE_RESPONSE          st_Response;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HANDSETS + CMBS_IE_CALLINSTANCE + CMBS_IE_RESPONSE + CMBS_IE_DC_REJECT_REASON
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            switch (u16_IE)
            {
            case CMBS_IE_HANDSETS:
                break;
            case CMBS_IE_CALLINSTANCE:
                break;
            case CMBS_IE_DC_REJECT_REASON:
                break;
            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            default:
                break;
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}


//		========== app_OnDCSessionStop ===========
/*!
        \brief			target wants to stop data call
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_OnDCSessionStop(void *pv_List)
{
    void *pv_IE = NULL;
    u16      u16_IE;
    u16   u16_Handsets = 0;
    u32   u32_CallInstance = 0;


    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HANDSETS + CMBS_IE_CALLINSTANCE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        switch (u16_IE)
        {
        case CMBS_IE_HANDSETS:
            cmbs_api_ie_ShortValueGet(pv_IE, &u16_Handsets, CMBS_IE_HANDSETS);
            break;
        case CMBS_IE_CALLINSTANCE:
            cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
            break;
        default:
            break;
        }
    }
    cmbs_dsr_dc_SessionStopRes(g_cmbsappl.pv_CMBSRef, u16_Handsets, u32_CallInstance, TRUE);
}

//		========== app_OnDCSessionStopRSP ===========
/*!
        \brief			response to data call session stop
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_OnDCSessionStopRSP(void *pv_List)
{
    void *pv_IE = NULL;
    u16           u16_IE;
    ST_IE_RESPONSE          st_Response;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HANDSETS + CMBS_IE_CALLINSTANCE + CMBS_IE_RESPONSE + CMBS_IE_DC_REJECT_REASON
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            switch (u16_IE)
            {
            case CMBS_IE_HANDSETS:
                break;
            case CMBS_IE_CALLINSTANCE:
                break;
            case CMBS_IE_DC_REJECT_REASON:
                break;
            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            default:
                break;
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

//		========== app_OnDCSessionDataSend ===========
/*!
        \brief			Send data via data call
        \param[in]     *pv_List		 IE list
        \return			<none>
*/

void app_OnDCSessionDataSend(void *pv_List)
{
    void *pv_IE = NULL;
    u16      u16_IE;
    ST_IE_DATA pst_data;
    u16   u16_Handsets = 0;
    u32   u32_CallInstance = 0;


    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HANDSETS + CMBS_IE_CALLINSTANCE + CMBS_IE_DATA
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        switch (u16_IE)
        {
        case CMBS_IE_HANDSETS:
            cmbs_api_ie_ShortValueGet(pv_IE, &u16_Handsets, CMBS_IE_HANDSETS);
            break;
        case CMBS_IE_CALLINSTANCE:
            cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
            break;
        case CMBS_IE_DATA:
            cmbs_api_ie_DataGet(pv_IE, &pst_data);
            break;
        default:
            break;
        }
    }
    cmbs_dsr_dc_DataSendRes(g_cmbsappl.pv_CMBSRef, u16_Handsets, u32_CallInstance, TRUE);
}

//		========== app_OnDCSessionDataSendRSP ===========
/*!
        \brief			Response to send data via data call
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_OnDCSessionDataSendRSP(void *pv_List)
{
    void *pv_IE = NULL;
    u16           u16_IE;
    ST_IE_RESPONSE          st_Response;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HANDSETS + CMBS_IE_CALLINSTANCE + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

        while (pv_IE != NULL)
        {
            switch (u16_IE)
            {
            case CMBS_IE_HANDSETS:
                break;
            case CMBS_IE_CALLINSTANCE:
                break;
            case CMBS_IE_RESPONSE:
                // check response code:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            default:
                break;
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}


//		========== app_OnDectSettingsGetRSP ===========
/*!
        \brief			response of DECT settings get event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnDectSettingsGetRSP(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;
    u8 i;
    PST_IE_PROP_FIELDS_LIST	st_PropSettings;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HW_VERSION + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_DECT_SETTINGS_LIST )
            {
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_DectSettings,
                                         sizeof(st_IEInfo.Info.st_DectSettings),
                                         CMBS_IE_DECT_SETTINGS_LIST,
                                         CMBS_EV_DSR_DECT_SETTINGS_LIST_GET_RES);
                }

            }
            else if ( u16_IE == CMBS_IE_PROP_FIELDS_LIST )
            {
                // handle Proprietary field data, those data cannot be handled in parallel to DECT settings 
                // in keypb_DectSettingsGet() via appcmbs_ObjectSignal as the same Info buffer is used (overlay with union).
                
                /* Proprietary fields */
                APP_SRV_INFO_PRINT("\n--- PROPRIETARY FIELD DECT SETTINGS: ---\n");

                // u8_Offset = 0;

                st_PropSettings = (PST_IE_PROP_FIELDS_LIST)(&st_IEInfo.Info.st_PropSettings);    

                for (i = 0; i < CMBS_MAX_NUM_PROP_FIELDS; i++)
                {
                    if (st_PropSettings->u16_PropFieldsMask & (0x0001 << i))
                    {
                        u8 k;
                        APP_SRV_INFO_PRINT("Prop field %d: ",i);

                        for (k = 0; k < st_PropSettings->st_PropField[i].u8_Len; k++)
                        {
                            APP_SRV_INFO_PRINT("%2x ",st_PropSettings->st_PropField[i].u8_Data[k] );
                        }           
                        // u8_Offset += st_PropSettings->st_PropField[i].u8_Len;
                        APP_SRV_INFO_PRINT("\n");
                    }
                }                
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

void app_OnGetBaseNameRSP(void *pv_List)
{
    ST_IE_BASE_NAME st_BaseName;
    void *pv_IE;
    u16      u16_IE;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_BASE_NAME )
        {
            cmbs_api_ie_BaseNameGet(pv_IE, &st_BaseName);
        }

        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

    if ( g_cmbsappl.n_Token )
    {
        appcmbs_ObjectSignal((void *)&st_BaseName.u8_BaseName,
                             sizeof(st_BaseName.u8_BaseName),
                             CMBS_IE_BASE_NAME,
                             CMBS_EV_DSR_GET_BASE_NAME_RES);
    }
}

//		========== app_OnDectSettingsSetRSP ===========
/*!
        \brief			response of DECT settings set event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnDectSettingsSetRSP(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HW_VERSION + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_RESPONSE )
            {
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_DectSettings,
                                         sizeof(st_IEInfo.Info.st_DectSettings),
                                         CMBS_IE_RESPONSE,
                                         CMBS_EV_DSR_DECT_SETTINGS_LIST_SET_RES);
                }

            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

//		========== app_OnSysLog ===========
/*!
        \brief			response of sys log
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnSysLog(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_SYS_LOG + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_SYS_LOG )
            {               
            	tcx_LogWriteLogBuffer(st_IEInfo.Info.st_SysLog.u8_Data, st_IEInfo.Info.st_SysLog.u8_DataLen);
               
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_SysLog,
                                         sizeof(st_IEInfo.Info.st_SysLog),
                                         CMBS_IE_SYS_LOG,
                                         CMBS_EV_DSR_SYS_LOG);
                }
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

//      ========== app_OnSubscribedHsListGet ===========
/*!
        \brief          response of subscribed handsets get
        \param[in]     *pv_List      IE list
        \return         <none>
*/

// Backup Hs Name and its length for during FWUP use case 	
u8	g_u8_HsNameLen[CMBS_HS_SUBSCRIBED_MAX_NUM];	
u8  g_u8_HsName[CMBS_HS_SUBSCRIBED_MAX_NUM][CMBS_HS_NAME_MAX_LENGTH*3]; // Ascii String

void app_OnSubscribedHsListGet(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16     u16_IE;
	u8		u8_HsId;

    cmbsevent_clearHsList();

    if ( pv_List )
    {
        APP_SRV_INFO_PRINT("\n");
        APP_SRV_INFO_PRINT("List of registered handsets:\n");
        APP_SRV_INFO_PRINT("----------------------------\n");

        // collect information elements.
        // we expect: CMBS_IE_SUBSCRIBED_HS_LIST + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_SUBSCRIBED_HS_LIST )
            {
                u8* pu8_HsName = st_IEInfo.Info.st_SubscribedHsList.u8_HsName;
                cmbsevent_addHs2List(st_IEInfo.Info.st_SubscribedHsList.u16_HsID, (char*)pu8_HsName); 

				// Backup HS name and its length during FWUP
				u8_HsId = (u8) st_IEInfo.Info.st_SubscribedHsList.u16_HsID;
				g_u8_HsNameLen[u8_HsId-1] = (u8)strlen((char*)pu8_HsName);
				strcpy((char*)g_u8_HsName[u8_HsId-1], (char*)pu8_HsName);
		
                APP_SRV_INFO_PRINT("Number:[%d]\tSubscribed,\tName: \"%s\"", u8_HsId, pu8_HsName); 
				
                if ( st_IEInfo.Info.st_SubscribedHsList.u8_FXS_ExNumLen != 0 )
                {
                    //the entry is FXS

                    APP_SRV_INFO_PRINT(", Extension Number:[%s]\n", st_IEInfo.Info.st_SubscribedHsList.u8_FXS_ExNum);
                }
                else
                {
                    APP_SRV_INFO_PRINT("\n");
                }

            }
            else if ( u16_IE == CMBS_IE_RESPONSE )
            {
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_Resp,
                                         sizeof(st_IEInfo.Info.st_Resp),
                                         CMBS_IE_SUBSCRIBED_HS_LIST,
                                         CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET_RES);
                }
            }

            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }

    cmbsevent_OnHSListUpdated();
}


//      ========== app_OnLineSettingsListGet ===========
/*!
        \brief          response of line settings get
        \param[in]     *pv_List      IE list
        \return         <none>
*/
void app_OnLineSettingsListGet(void *pv_List)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;

    if ( pv_List )
    {
        APP_SRV_INFO_PRINT("\n");
        APP_SRV_INFO_PRINT("List of Line settings:\n");
        APP_SRV_INFO_PRINT("----------------------------\n");
        // collect information elements.
        // we expect: CMBS_IE_LINE_SETTINGS_LIST + CMBS_IE_RESPONSE
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( u16_IE == CMBS_IE_LINE_SETTINGS_LIST )
            {
                APP_SRV_INFO_PRINT("\nLineId = %d\nAttached HS mask = 0x%X\nCall Intrusion = %d\nMultiple Calls = %d\nLine Type = %d\n",
                       st_IEInfo.Info.st_LineSettingsList.u8_Line_Id, st_IEInfo.Info.st_LineSettingsList.u16_Attached_HS,
                       st_IEInfo.Info.st_LineSettingsList.u8_Call_Intrusion,
                       st_IEInfo.Info.st_LineSettingsList.u8_Multiple_Calls,
                       st_IEInfo.Info.st_LineSettingsList.e_LineType);
            }
            else if ( u16_IE == CMBS_IE_RESPONSE )
            {
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_Resp,
                                         sizeof(st_IEInfo.Info.st_Resp),
                                         CMBS_IE_RESPONSE,
                                         CMBS_EV_DSR_LINE_SETTINGS_LIST_GET_RES);
                }

            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}


//		========== app_OnUserDefinedInd ===========
/*!
        \brief			reception of user defined indication
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void           app_OnUserDefinedInd(void *pv_List, E_CMBS_EVENT_ID e_EventID)
{
    ST_APPCMBS_IEINFO
       st_IEInfo;
    void *pv_IE;
    u16      u16_IE;

    if ( pv_List )
    {
        // collect information elements.
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            appcmbs_IEInfoGet(pv_IE, u16_IE, &st_IEInfo);
            if ( CMBS_IE_USER_DEFINED_START <= u16_IE && u16_IE <= CMBS_IE_USER_DEFINED_END )
            {
                if ( g_cmbsappl.n_Token )
                {
                    appcmbs_ObjectSignal((void *)&st_IEInfo.Info.st_UserDefined,
                                         sizeof(st_IEInfo.Info.st_UserDefined),
                                         u16_IE,
                                         e_EventID);
                }
            }
            cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
        }
    }
}

void app_OnDCDataSendRes(void *pv_List)
{
    // signal parameter setting to upper application
    UNUSED_PARAMETER(pv_List);
    if ( g_cmbsappl.n_Token )
    {
        appcmbs_ObjectSignal(NULL, 0, 0, CMBS_EV_DSR_DC_DATA_SEND_RES);
    }
}

void app_OnDCSessionStartRes(void *pv_List)
{
    // signal parameter setting to upper application
    UNUSED_PARAMETER(pv_List);
    if ( g_cmbsappl.n_Token )
    {
        appcmbs_ObjectSignal(NULL, 0, 0, CMBS_EV_DSR_DC_SESSION_START_RES);
    }
}
void appOnAFEChannelAllocateRes(void *pv_List)
{
	if ( g_cmbsappl.n_Token )
	{
    	appcmbs_ObjectSignal(pv_List, ((PST_CFR_IE_LIST)pv_List)->u16_CurSize, 0, CMBS_EV_DSR_AFE_CHANNEL_ALLOCATE_RES);
	}
}


void app_OnTurnOnNEMORes(void *pv_List)
{
    if ( app_ResponseCheck(pv_List) == CMBS_RESPONSE_OK )
    {
        APP_SRV_INFO_PRINT("NEMO set successful\n");
    }
    else
    {
        APP_SRV_ERROR_PRINT("NEMO set failed !!!\n");
    }
}

#if CHECKSUM_SUPPORT
//		========== app_OnCheckSumFailure ===========
/*!
        \brief			reception of checksum failure

        \param[in]     *pv_List		 IE list

        \return			<none>

*/
void           app_OnCheckSumFailure(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    ST_IE_CHECKSUM_ERROR st_CheckSumError;

    if ( pv_List )
    {
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
        cmbs_api_ie_ChecksumErrorGet(pv_IE, &st_CheckSumError);

        APP_SRV_INFO_PRINT(" Checksum failure received\n Error type: 0x%02X, Event: 0x%04X \n",
                      st_CheckSumError.e_CheckSumError, st_CheckSumError.u16_ReceivedEvent);

        /* NOTE: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * Here the application can resend the IE and clear the WAIT buffer (exit the loop)
         */

    }
}
#endif

//		========== app_ServiceEntity ===========
/*!
        \brief		 CMBS entity to handle response information from target side
        \param[in]	 pv_AppRef		 application reference
        \param[in]	 e_EventID		 received CMBS event
        \param[in]	 pv_EventData	 pointer to IE list
        \return	 	 <int>

*/

int app_ServiceEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData)
{
    UNUSED_PARAMETER(pv_AppRef);

    switch (e_EventID)
    {
    case  CMBS_EV_DSR_HS_REGISTERED:
        app_OnHandsetRegistered(pv_EventData);
        return FALSE; // allow HAN to consume this event as well.

    case  CMBS_EV_DSR_HS_DELETE_RES:
        app_OnHandsetRegistered(pv_EventData);
        break;

    case  CMBS_EV_DSR_HS_IN_RANGE:
        app_OnHandsetInRange(pv_EventData);
        break;

    case  CMBS_EV_DSR_TIME_INDICATION:
        app_OnDateTimeIndication(pv_EventData);
        break;

    case CMBS_EV_DSR_HS_PAGE_PROGRESS:
        app_OnHsLocProgress(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DSR_HS_PAGE_ANSWER:
        app_OnHsLocAnswer(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DSR_HS_PAGE_STOP:
        app_OnHsLocRelease(pv_AppRef, pv_EventData);
        return TRUE;

    case CMBS_EV_DSR_CORD_OPENREG_RES:
        app_OnRegistrationOpenRSP(pv_EventData);
	 return FALSE; // allow HAN to consume this event as well.	

    case CMBS_EV_DSR_CORD_CLOSEREG_RES:
        app_OnRegistrationCloseRSP(pv_EventData);
	 return FALSE; // allow HAN to consume this event as well.	

    case CMBS_EV_DSR_CORD_CLOSEREG:
        app_OnRegistrationClose(pv_EventData);
        return FALSE; // allow HAN to consume this event as well.	

    case CMBS_EV_DSR_PARAM_GET_RES:
        app_OnParamGetRSP(pv_EventData);
		if(appcmbs_GetExternalClientInformation() != NO_EXTERNAL_CLIENT)
		{
			return FALSE;
		}
        break;

    case CMBS_EV_DSR_PARAM_SET_RES:
        app_OnParamSetRSP(pv_EventData);
		if(appcmbs_GetExternalClientInformation() != NO_EXTERNAL_CLIENT)
		{
			return FALSE;
		}
        break;

    case CMBS_EV_DSR_PARAM_AREA_GET_RES:
        app_OnParamAreaGetRSP(pv_EventData);
		if(appcmbs_GetExternalClientInformation() != NO_EXTERNAL_CLIENT)
		{
			return FALSE;
		};
        break;

    case CMBS_EV_DSR_PARAM_AREA_SET_RES:

		app_OnParamAreaSetRSP(pv_EventData);
		if(appcmbs_GetExternalClientInformation() != NO_EXTERNAL_CLIENT)
		{
			return FALSE;
		}
        break;

    case CMBS_EV_DSR_PARAM_AREA_GET:
        app_OnParamAreaGet(pv_EventData);
        break;

    case CMBS_EV_DSR_PARAM_AREA_SET:
        app_OnParamAreaSet(pv_EventData);
        break;

    case CMBS_EV_DSR_FW_VERSION_GET_RES:
        app_OnFwVersionGetRSP(pv_EventData);
        break;

    case CMBS_EV_DSR_HW_VERSION_GET_RES:
        app_OnHwVersionGetRSP(pv_EventData);
        break;

    case CMBS_EV_DSR_SYS_LOG:
        app_OnSysLog(pv_EventData);
        break;

    case CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET_RES:
        app_OnSubscribedHsListGet(pv_EventData);
        break;

    case CMBS_EV_DSR_LINE_SETTINGS_LIST_GET_RES:
        app_OnLineSettingsListGet(pv_EventData);
        break;

    case CMBS_EV_DSR_HS_PAGE_START_RES:
        app_OnPageStarted(pv_EventData);
        break;

    case CMBS_EV_DSR_HS_PAGE_STOP_RES:
        app_OnPageStoped(pv_EventData);
        break;

    case CMBS_EV_DSR_DECT_SETTINGS_LIST_GET_RES:
        app_OnDectSettingsGetRSP(pv_EventData);
        break;

    case CMBS_EV_DSR_DECT_SETTINGS_LIST_SET_RES:
        app_OnDectSettingsSetRSP(pv_EventData);
        break;

    case CMBS_EV_DSR_HS_SUBSCRIBED_LIST_SET_RES:
    case CMBS_EV_DSR_LINE_SETTINGS_LIST_SET_RES:
    case CMBS_EV_DSR_GPIO_CONNECT_RES:
    case CMBS_EV_DSR_GPIO_DISCONNECT_RES:
    case CMBS_EV_DSR_ATE_TEST_START_RES:
    case CMBS_EV_DSR_ATE_TEST_LEAVE_RES:
    case CMBS_EV_DSR_FIXED_CARRIER_RES:
    case CMBS_EV_DSR_AFE_ENDPOINT_CONNECT_RES:
    case CMBS_EV_DSR_AFE_ENDPOINT_ENABLE_RES:
    case CMBS_EV_DSR_AFE_ENDPOINT_DISABLE_RES:
    case CMBS_EV_DSR_AFE_CHANNEL_DEALLOCATE_RES:
    case CMBS_EV_DSR_AFE_ENDPOINT_GAIN_RES:
    case CMBS_EV_DSR_DHSG_SEND_BYTE_RES	:
    case CMBS_EV_DSR_GPIO_ENABLE_RES:
    case CMBS_EV_DSR_GPIO_DISABLE_RES:
    case CMBS_EV_DSR_GPIO_CONFIG_SET_RES:
    case CMBS_EV_DSR_EXT_INT_CONFIG_RES:
    case CMBS_EV_DSR_EXT_INT_ENABLE_RES:
    case CMBS_EV_DSR_EXT_INT_DISABLE_RES:
    case CMBS_EV_DSR_CALL_STATE_SET_FILTER_RES:
    case CMBS_EV_DSR_SLIC_LINE_TEST_STOP_RES:
       {
            u8 u8_Resp = app_ResponseCheck(pv_EventData);
            if ( u8_Resp == CMBS_RESPONSE_OK )
            {
                APP_SRV_INFO_PRINT("APPCMBS-SRV: INFO Response: OK\n");
            }
            else
            {
                APP_SRV_ERROR_PRINT("APPCMBS-SRV: ERROR !!! Response ERROR\n");
            }

            if ( g_cmbsappl.n_Token )
            {
                appcmbs_ObjectSignal(NULL, 0, 0, e_EventID);
            }

            break;
        }
    case CMBS_EV_DSR_GPIO_CONFIG_GET_RES:
        {
            u8 u8_Resp = app_ResponseCheck(pv_EventData);
            if ( u8_Resp == CMBS_RESPONSE_OK )
            {
                APP_SRV_INFO_PRINT("APPCMBS-SRV: INFO Response: OK\n");
            }
            if ( g_cmbsappl.n_Token )
            {
                appcmbs_ObjectSignal(NULL, 0, 0, e_EventID);
            }
        }
        break;

    case CMBS_EV_DSR_TARGET_UP:
        app_onTargetUp();
        break;
    case CMBS_EV_DSR_SYS_START_RES:
		if ( g_cmbsappl.n_Token )
        {
			appcmbs_ObjectSignal(NULL, 0, 0, e_EventID);
		}
		
		// Init HAN after system restart, for EepromOnHost delay it till end of file reading by target, 
		if ( appcmbs_GetExternalClientInformation() == EXTERNAL_CLIENT_EXIST && (!b_EepromOnHost || (e_EepOnHostState == EEP_ON_HOST_READ_COMPLETED)))
		{
			app_HanStartAndConfig();

			// try doing suota init as well
			cmbs_dsr_suota_Init(g_cmbsappl.pv_CMBSRef);

			// enable the auotafile enntity since han server needs to support suota
			app_SuotaFileEnable(TRUE);

			app_HANRestoreSequenceNumbers();

			return FALSE; // allow HAN to consume this event as well.	
		}
        break;
    case CMBS_EV_DEE_HANDSET_LINK_RELEASE:
        app_OnHandsetLinkRelease(pv_EventData);
        break;

    case CMBS_EV_DSR_EEPROM_SIZE_GET_RES:
        app_OnEepromSizeResp(pv_EventData);
        break;

    case CMBS_EV_DSR_EEPROM_EXTENDED_SIZE_GET_RES:
        app_OnEepromExtSizeResp(pv_EventData);
		if(appcmbs_GetExternalClientInformation() != NO_EXTERNAL_CLIENT)
		{
			return FALSE;
		}
        break;		

    case CMBS_EV_DSR_RECONNECT_REQ:
        app_onDisconnectReq();
        break;
    case CMBS_EV_DSR_GET_BASE_NAME_RES:
        app_OnGetBaseNameRSP(pv_EventData);
        break;
    case CMBS_EV_DSR_EEPROM_VERSION_GET_RES:
        app_OnEEPROMVersionGetRSP(pv_EventData);
        break;
    case CMBS_EV_DSR_PING_RES:
		// no handler
        break;
    case CMBS_EV_DSR_DC_DATA_SEND_RES:
        app_OnDCDataSendRes(pv_EventData);
        break;
    case CMBS_EV_DSR_DC_SESSION_START_RES:
        app_OnDCSessionStartRes(pv_EventData);
        break;
    case CMBS_EV_DSR_AFE_CHANNEL_ALLOCATE_RES:
        appOnAFEChannelAllocateRes(pv_EventData);
        break;
        //    case CMBS_EV_DSR_AFE_TDM_CONNECT_RES:
        //		appOnAFETDMConnectRes(pv_EventData);
        //		break;
        //   case CMBS_EV_DSR_HAN_DEVICE_READ_TABLE_RES:
        //app_OnReadHanDeviceTableRes(pv_EventData);
        //	 	break;
    case CMBS_EV_DSR_EXT_INT_INDICATION:
    case CMBS_EV_DSR_AFE_AUX_MEASUREMENT_RES:
        if ( g_cmbsappl.n_Token )
        {
            appcmbs_ObjectSignal(NULL, 0, 0, e_EventID);
        }
        break;

    case CMBS_EV_DSR_TERMINAL_CAPABILITIES_IND:
        appOnTerminalCapabilitiesInd(pv_EventData);
        break;

    case CMBS_EV_DSR_HS_PROP_DATA_RCV_IND:
        appOnPropritaryDataRcvInd(pv_EventData);
        break;

    case CMBS_EV_DSR_DTAM_START_SESSION:
        appOnDTAMStartSession(pv_EventData);
        break;
		
    case CMBS_EV_DSR_DTAM_SELECT_NEIGHBOUR_MESSAGE:
        appOnDTAMSelectNeighbourMsg(pv_EventData);
        break;
		
    case CMBS_EV_DSR_DTAM_DELETE_MESSAGE:
        appOnDTAMDeleteMsg(pv_EventData);
        break;
		
    case CMBS_EV_DSR_DTAM_PLAY_MESSAGE:
        appOnDTAMPlayMsg(pv_EventData);
        break;
			
    case CMBS_EV_DSR_DTAM_PAUSE_RESUME_MESSAGE:
        appOnDTAMPauseResume(pv_EventData);
        break;
		
    case CMBS_EV_DSR_DTAM_STOP_MESSAGE_PLAY:
        appOnDTAMStopMsgPlay(pv_EventData);
        break;
		
    case CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE:
        appOnDTAMRecordWelcomeMsg(pv_EventData);
        break;
		
    case CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE_STOP:
        appOnDTAMRecordWelcomeMsgStop(pv_EventData);
        break;
		
#if CHECKSUM_SUPPORT
    case CMBS_EV_CHECKSUM_FAILURE:
        app_OnCheckSumFailure(pv_EventData);
        break;
#endif

    case CMBS_EV_DSR_SLIC_LINE_TEST_RES:
        appOnSetLineTestRes(pv_EventData);
        break;

    case CMBS_EV_DSR_SLIC_NLT_CAP_TEST_START_RES:
        appOnSrvNltCapTestRes(pv_EventData);
        break;

	case CMBS_EV_DSR_FXS_STATUS_RES:
		app_OnSrvFxsStatus(pv_EventData);
		break;

	case CMBS_EV_DSR_FXS_STATUS_IND:
		app_OnSrvFxsStatus(pv_EventData);
		break;

	case CMBS_EV_DSR_FXS_RING_TEST_START_RES:
		appOnFxsRingTestStartRes(pv_EventData);
		break;

	
	case CMBS_EV_DSR_FXS_RING_TEST_STOP_RES:
		appOnFxsRingTestStopRes(pv_EventData);
		break;
	
	case  CMBS_EV_DSR_NO_VALID_DATA_TIME_AVAIL:
		app_OnNoValidDateTimeAvailable(pv_EventData);
		break;



    default:
        if ( CMBS_EV_DSR_USER_DEFINED_START <= e_EventID && e_EventID <= CMBS_EV_DSR_USER_DEFINED_END )
        {
            app_OnUserDefinedInd(pv_EventData, e_EventID);
            break;
        }
        return FALSE;
    }

    return TRUE;
}


//		==========  app_HandsetMap ===========
/*!
        \brief			   convert handset string to used handset bit mask
                        does also support "all" and "none" as input
        \param[in,out]		psz_Handsets	pointer to handset string
        \return				<u16>    return handset bit mask
*/

u16            app_HandsetMap(char *psz_Handsets)
{
    u16   u16_Handset = 0;

    if ( !strcmp(psz_Handsets, "all\0") )    //including FXS
        return 0xFFFF;

    if ( !strcmp(psz_Handsets, "none\0") )
    {
        return 0;
    }

    sscanf((char *)psz_Handsets, "%hd", &u16_Handset);

    if ( (u16_Handset != 0) && (u16_Handset <= CMBS_HS_SUBSCRIBED_MAX_NUM + CMBS_MAX_FXS_EXTENSIONS) )
    {
        u16_Handset = (1 << (u16_Handset - 1));
    }

    APP_SRV_INFO_PRINT("APPCMBS-SRV: INFO u16_Handset %04x\n", u16_Handset);

    return u16_Handset;
}

//		========== app_SrvHandsetDelete  ===========
/*!
        \brief				 call CMBS function of handset delete
        \param[in,out]		 psz_Handsets     pointer to parameter string,e.g."1234" or "all"
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvHandsetDelete(char *psz_Handsets)
{
    return cmbs_dsr_hs_Delete(g_cmbsappl.pv_CMBSRef, app_HandsetMap(psz_Handsets));
}


//		========== app_SrvHandsetDeleteINL  ===========
/*!
        \brief				 call CMBS function of handset delete from internal name list (INL)
        \param[in,out]		 psz_Handsets     pointer to parameter string,e.g."1234" or "all"
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvHandsetDeleteINL(u16 u16_HandsetMask)
{
  // u8 u8_BufferIn[2];
  // u8 u8_BufferOut[2];
   
  // Endianess issue and byte order on CMBS API.
  // u16_HandsetMask read via tcx_scanf() provides the wrong order, thus it needs to be changed:
  
  // u8_BufferIn[1] = u16_HandsetMask >> 8;
  // u8_BufferIn[0] = u16_HandsetMask & 0xFF;
  
  // and now ensure the correct endieness 
  // cfr_ie_ser_u16(u8_BufferOut, u8_BufferIn);
  
  // return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_INL_DEL, u8_BufferOut, CMBS_PARAM_INL_DEL_LENGTH);
  return cmbs_dsr_DeleteINLEntries(g_cmbsappl.pv_CMBSRef, u16_HandsetMask);
}

//		========== app_SrvHandsetAddINL  ===========
/*!
        \brief				 call CMBS function of handset add to internal name list (INL)
        \param[in,out]		 psz_Handsets     pointer to parameter string,e.g."1234" or "all"
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC app_SrvHandsetAddINL(char *psz_Handsets)
{
    u16 u16_HandsetMask = app_HandsetMap(psz_Handsets);

    return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_INL_ADD, (u8 *)&u16_HandsetMask, CMBS_PARAM_INL_DEL_LENGTH);
}


//		========== app_SrvHandsetPage  ===========
/*!
        \brief				 call CMBS function of handset page
        \param[in,out]		 psz_Handsets     pointer to parameter string,e.g."1234" or "all" or "none"
        \return				 <E_CMBS_RC>
      \note              stopp pagine, re-call this function with "none"
*/
E_CMBS_RC      app_SrvHandsetPage(char *psz_Handsets)
{
    return cmbs_dsr_hs_Page(g_cmbsappl.pv_CMBSRef, app_HandsetMap(psz_Handsets));
}

//      ========== app_SrvHandsetStopPaging  ===========
/*!
        \brief               call CMBS function of stop handsets paging
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
      \note              stopp pagine, re-call this function with "none"
*/
E_CMBS_RC      app_SrvHandsetStopPaging(u16 u16_HsMask, u32 u32_CallInstance)
{
    return cmbs_dsr_hs_StopPaging(g_cmbsappl.pv_CMBSRef, u16_HsMask, u32_CallInstance);
}


//		========== app_SrvCWDisable  ===========
/*!
        \brief				 call CMBS function of Disabling Call waiting for specified handsets
        \param[in,out]		 psz_Handsets     pointer to parameter string,e.g."1234" or "all"
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvCWDisable(u16 u16_HandsetMask)
{  
  return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_HS_CW_DISABLED, (u8*) &u16_HandsetMask , CMBS_PARAM_HS_CW_DISABLED_LENGTH);
}


//		========== app_SrvSubscriptionOpen  ===========
/*!
        \brief				 call CMBS function of open registration
        \param[in]	     	 u32_timeout
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC app_SrvSubscriptionOpen(u32 u32_timeout)
{
    return cmbs_dsr_cord_OpenRegistration(g_cmbsappl.pv_CMBSRef, u32_timeout);
}

//		========== app_SrvSubscriptionOpenExt  ===========
/*!
        \brief				 call CMBS function of open registration
        \param[in]		     u32_timeout
        \param[in]           e_RegEnable
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC app_SrvSubscriptionOpenExt(u32 u32_timeout,E_CMBS_HS_REGISTER_ENABLE e_RegEnable)
{
    return cmbs_dsr_cord_OpenRegistrationExt(g_cmbsappl.pv_CMBSRef, u32_timeout, e_RegEnable);
}

//		========== app_SrvSubscriptionClose  ===========
/*!
        \brief				 call CMBS function of open registration
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvSubscriptionClose(void)
{
    return cmbs_dsr_cord_CloseRegistration(g_cmbsappl.pv_CMBSRef);
}

//		========== app_SrvEncryptionDisable  ===========
/*!
        \brief				 call CMBS function of Disabling encryption
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvEncryptionDisable(void)
{
    return cmbs_dsr_cord_DisableEncryption(g_cmbsappl.pv_CMBSRef);
}

//		========== app_SrvEncryptionEnable  ===========
/*!
        \brief				 call CMBS function of Enabling encryption
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvEncryptionEnable(void)
{
    return cmbs_dsr_cord_EnableEncryption(g_cmbsappl.pv_CMBSRef);
}

//		========== app_SrvNemoControlSet  ===========
/*!
        \brief				 call CMBS function of setting Nemo Control
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvNemoControlSet(u8 u8_Value)
{
    return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_NEMO_CONTROL, &u8_Value, CMBS_PARAM_NEMO_CONTROL_LENGTH);
}

//		========== app_SrvRepeaterTypeSet  ===========
/*!
        \brief				 call CMBS function of setting repeater type
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvRepeaterTypeSet(u8 u8_Value)
{
    return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_REPEATER_TYPE, &u8_Value, CMBS_PARAM_REPEATER_TYPE_LENGTH);
}

//		========== app_SrvFixedCarrierSet  ===========
/*!
        \brief				 call CMBS function of setting a fixed carrier
        \param[in,out]		 <u8_Value> new carrier setting
        \return				 <E_CMBS_RC>
*/

E_CMBS_RC      app_SrvFixedCarrierSet(u8 u8_Value)
{
    return cmbs_dsr_FixedCarrierSet(g_cmbsappl.pv_CMBSRef, u8_Value);
}

//		========== app_SrvPINCodeSet  ===========
/*!
        \brief				 set authentication PIN code, registration code
        \param[in,out]		 psz_PIN    pointer to PIN string, e.g. "ffff1590" for PIN 1590
        \return				 <E_CMBS_RC>

*/
E_CMBS_RC      app_SrvPINCodeSet(char *psz_PIN)
{
    unsigned int      i;

    u8       szPinCode[CMBS_PARAM_PIN_CODE_LENGTH] = { 0, 0, 0, 0 };

    if ( strlen(psz_PIN) == (CMBS_PARAM_PIN_CODE_LENGTH * 2) )
    {
        APP_SRV_INFO_PRINT("PIN %s\n", psz_PIN);
        for (i = 0; i < (CMBS_PARAM_PIN_CODE_LENGTH * 2); i++)
        {
            APP_SRV_INFO_PRINT("%c => %d BCD %02x\n", psz_PIN[i], (i / 2), szPinCode[i / 2]);

            if ( psz_PIN[i] == 'f' )
            {
                if ( (i & 0x01) )
                {
                    szPinCode[i / 2] |= 0x0F;
                }
                else
                {
                    szPinCode[i / 2] |= 0xF0;
                }

            }
            else if ( psz_PIN[i] >= '0' && psz_PIN[i] <= '9' )
            {
                if ( (i & 0x01) )
                {
                    szPinCode[i / 2] |= (psz_PIN[i] - '0');
                }
                else
                {
                    szPinCode[i / 2] |= (psz_PIN[i] - '0') << 4;
                }
            }
            else
            {
                if ( (i & 0x01) )
                {
                    szPinCode[i / 2] |= 1;
                }
                else
                {
                    szPinCode[i / 2] |= 1 << 4;
                }

            }
        }
    }
    APP_SRV_INFO_PRINT("%02x %02x %02x\n", szPinCode[0], szPinCode[1], szPinCode[2]);
    return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_AUTH_PIN, szPinCode, CMBS_PARAM_PIN_CODE_LENGTH);
}

//		========== app_SrvTestModeGet  ===========
/*!
        \brief				 get test mode state
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvTestModeGet(u32 u32_Token)
{

    return cmbs_dsr_param_Get(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_TEST_MODE);
}

//		========== app_SrvTestModeGet  ===========
/*!
        \brief				 set TBR 6 on
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvTestModeSet(void)
{
    u8 u8_TestMode = CMBS_TEST_MODE_TBR6;

    return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, CMBS_PARAM_TEST_MODE, &u8_TestMode, CMBS_PARAM_TEST_MODE_LENGTH);
}


//		========== _app_HexString2Byte  ===========
/*!
      \brief            convert a 2 bytes hex character string to binary
      \param[in]        psz_HexString     2 bytes hex character string
      \return           1 byte
*/
char           _app_HexString2Byte(char *psz_HexString)
{
    char        c;

    // first character
    c = (psz_HexString[0] >= 'A' ? ((psz_HexString[0] & 0xdf) - 'A') + 10 : (psz_HexString[0] - '0'));

    c <<= 4;
    // second character
    c += (psz_HexString[1] >= 'A' ? ((psz_HexString[1] & 0xdf) - 'A') + 10 : (psz_HexString[1] - '0'));

    return c;
}


//		========== app_SrvParamGet  ===========
/*!
        \brief				 get parameter information
        \param[in,out]		 e_Param       pre-defined parameter settings
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvParamGet(E_CMBS_PARAM e_Param, u32 u32_Token)
{

    return cmbs_dsr_param_Get(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, e_Param);
}

//		========== app_SrvParamSet  ===========
/*!
      \brief            set parameter value
      \param[in]        e_Param           pre-defined parameter settings
      \param[in]        pu8_Data          user input data
        \param[in]        u16_Length        length of data
        \param[in]        u32_Token         TRUE tells the upper application to wait for an answer
      \return           <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvParamSet(E_CMBS_PARAM e_Param, u8 *pu8_Data, u16 u16_Length, u32 u32_Token)
{
    if ( e_Param != CMBS_PARAM_UNKNOWN )
    {

        return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_EEPROM, e_Param, pu8_Data, u16_Length);
    }
    else
    {
        APP_SRV_ERROR_PRINT("ERROR: app_SrvParamSet offset is not implemented, yet \n");
    }

    return CMBS_RC_ERROR_GENERAL;

}

//      ========== app_ProductionParamGet  ===========
/*!
        \brief               get parameter information
        \param[in,out]       e_Param       pre-defined parameter settings
        \param[in,out]       u32_Token     TRUE, if upper application waits for answer
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_ProductionParamGet(E_CMBS_PARAM e_Param, u32 u32_Token)
{

    return cmbs_dsr_param_Get(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_PRODUCTION, e_Param);
}

//      ========== app_ProductionParamSet  ===========
/*!
      \brief            set parameter value
      \param[in]        e_Param           pre-defined parameter settings
      \param[in]        pu8_Data          user input data
      \param[in]        u16_Length        length of data
      \param[in]        u32_Token         TRUE tells the upper application to wait for an answer
      \return           <E_CMBS_RC>
*/
E_CMBS_RC      app_ProductionParamSet(E_CMBS_PARAM e_Param, u8 *pu8_Data, u16 u16_Length, u32 u32_Token)
{
    if ( e_Param != CMBS_PARAM_UNKNOWN )
    {

        return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_PRODUCTION, e_Param, pu8_Data, u16_Length);
    }
    else
    {
        APP_SRV_ERROR_PRINT("ERROR: app_ProductionParamSet offset is not implemented, yet \n");
    }

    return CMBS_RC_ERROR_GENERAL;

}


//      ========== app_FxsParamSet  ===========
/*!
      \brief            set parameter value
      \param[in]        e_Param           pre-defined parameter settings
      \param[in]        pu8_Data          user input data
      \param[in]        u16_Length        length of data
      \param[in]        u32_Token         TRUE tells the upper application to wait for an answer
      \return           <E_CMBS_RC>
*/
E_CMBS_RC      app_FxsParamSet(E_CMBS_PARAM e_Param, u8 *pu8_Data, u16 u16_Length, u32 u32_Token)
{
    if ( e_Param != CMBS_PARAM_UNKNOWN )
    {

        return cmbs_dsr_param_Set(g_cmbsappl.pv_CMBSRef, CMBS_PARAM_TYPE_FXS, e_Param, pu8_Data, u16_Length);
    }
    else
    {
        APP_SRV_ERROR_PRINT("ERROR: app_FxsParamSet offset is not implemented, yet \n");
    }

    return CMBS_RC_ERROR_GENERAL;

}


//		========== app_SrvParamAreaGet  ===========
/*!
        \brief				 get parameter area data
        \param[in,out]		 e_AreaType    memory area type.
        \param[in,out]		 u16_Pos       offset in memory area
        \param[in,out]		 u16_Length    length of to read area
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvParamAreaGet(E_CMBS_PARAM_AREA_TYPE e_AreaType, u32 u32_Pos, u16 u16_Length, u32 u32_Token)
{

    return cmbs_dsr_param_area_Get(g_cmbsappl.pv_CMBSRef, e_AreaType, u32_Pos, u16_Length);
}

//		========== app_SrvParamSet  ===========
/*!
      \brief            set parameter value
      \param[in]        e_AreaType     memory area type
        \param[in]        u16_Pos        offset in memory area
        \param[in]        u16_Length        length of data
      \param[in]        pu8_Data          user input data
        \param[in]        u32_Token         TRUE tells the upper application to wait for an answer
      \return           <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvParamAreaSet(E_CMBS_PARAM_AREA_TYPE e_AreaType, u32 u32_Pos, u16 u16_Length, u8 *pu8_Data, u32 u32_Token)
{

    return cmbs_dsr_param_area_Set(g_cmbsappl.pv_CMBSRef, e_AreaType, u32_Pos, pu8_Data, u16_Length);
}

//		========== app_SrvFWVersionGet  ===========
/*!
        \brief				 get firmware version
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>

*/
E_CMBS_RC      app_SrvFWVersionGet(E_CMBS_MODULE e_Module, u32 u32_Token)
{

    return cmbs_dsr_fw_VersionGet(g_cmbsappl.pv_CMBSRef, e_Module);
}

E_CMBS_RC   app_SrvEEPROMVersionGet(u32 u32_Token)
{

    return cmbs_dsr_EEPROM_VersionGet(g_cmbsappl.pv_CMBSRef);
}

//		========== app_SrvHWVersionGet  ===========
/*!
        \brief				 get hardware version
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvHWVersionGet(u32 u32_Token)
{
    ST_APPCMBS_CONTAINER st_Container;
    PST_IE_HW_VERSION  		pst_hwVersion;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_hw_VersionGet(g_cmbsappl.pv_CMBSRef);

    appcmbs_WaitForContainer(CMBS_EV_DSR_HW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    pst_hwVersion = (PST_IE_HW_VERSION)st_Container.ch_Info;
	
    appcmbs_get_CMBSAppInstance()->u8_HwChip = pst_hwVersion->u8_HwChip;
	appcmbs_get_CMBSAppInstance()->u8_HwChipVersion = pst_hwVersion->u8_HwChipVersion;	  
    appcmbs_get_CMBSAppInstance()->u8_HwBoard = pst_hwVersion->u8_HwBoard;
    appcmbs_get_CMBSAppInstance()->u8_HwComType = pst_hwVersion->u8_HwComType; 

    return CMBS_RC_OK;
}

//		========== app_SrvLogBufferStart  ===========
/*!
        \brief				 start system log
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvLogBufferStart(void)
{
    return cmbs_dsr_LogBufferStart(g_cmbsappl.pv_CMBSRef);
}

//		========== app_SrvLogBufferStop  ===========
/*!
        \brief				 stop system log
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvLogBufferStop(void)
{
    return cmbs_dsr_LogBufferStop(g_cmbsappl.pv_CMBSRef);
}


//		========== app_SrvLogBufferRead  ===========
/*!
        \brief				 read log buffer
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvLogBufferRead(u32 u32_Token)
{

    return cmbs_dsr_LogBufferRead(g_cmbsappl.pv_CMBSRef);
}

E_CMBS_RC app_SrvSendHSRSSIReq(u8 u8_HSNum)
{
    return cmbs_dsr_HSRSSIReq(g_cmbsappl.pv_CMBSRef, u8_HSNum);
}

//		========== app_SrvSystemReboot  ===========
/*!
        \brief				 reboot CMBS Target
        \param[in,out]		 <none>
        \return				 <E_CMBS_RC>
      \note              also re-register CMBS-API to system!

*/
E_CMBS_RC      app_SrvSystemReboot(void)
{
    u8 u8_RetCode = cmbs_dsr_sys_Reset(g_cmbsappl.pv_CMBSRef);
    
#ifdef CMBS_COMA
    app_CSSCOMAReset();
#else

    if ( appcmbs_get_CMBSAppInstance()->u8_HwComType == CMBS_HW_COM_TYPE_USB ) 
    {
        // for USB we must close the CDC device to allow the USB dongle to perform a new USB enumeration
        APP_SRV_INFO_PRINT("\nReconnecting to target, please wait %d seconds \n", APPCMBS_RECONNECT_TIMEOUT / 1000); 
        if ( (u8_RetCode = appcmbs_ReconnectApplication(APPCMBS_RECONNECT_TIMEOUT)) != CMBS_RC_OK ) 
        {
            APP_SRV_ERROR_PRINT("\n *****************************************************");
            APP_SRV_ERROR_PRINT("\n Can't reconnect to target!!!!!");
            APP_SRV_ERROR_PRINT("\n *****************************************************");
        }
    }
#endif
   
    return u8_RetCode;
}

/* == ALTDV == */

//      ========== app_SrvSystemPowerOff  ===========
/*!
        \brief               power off CMBS Target
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvSystemPowerOff(void)
{
    return cmbs_dsr_sys_PowerOff(g_cmbsappl.pv_CMBSRef);
}

//      ========== app_SrvRFSuspend  ===========
/*!
        \brief               RF Suspend on CMBS Target
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvRFSuspend(void)
{
    return cmbs_dsr_sys_RFSuspend(g_cmbsappl.pv_CMBSRef);
}

//      ========== app_SrvRFResume  ===========
/*!
        \brief               RF Resume on CMBS Target
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvRFResume(void)
{
    return cmbs_dsr_sys_RFResume(g_cmbsappl.pv_CMBSRef);
}

//      ========== app_SrvTurnOnNEMo  ===========
/*!
        \brief               Turn On NEMo mode for the CMBS base
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvTurnOnNEMo(void)
{
    return cmbs_dsr_sys_TurnOnNEMo(g_cmbsappl.pv_CMBSRef);
}

//      ========== app_SrvTurnOffNEMo  ===========
/*!
        \brief               Turn Off NEMo mode for the CMBS base
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvTurnOffNEMo(void)
{
    return cmbs_dsr_sys_TurnOffNEMo(g_cmbsappl.pv_CMBSRef);
}

//      ========== app_SrvHandsetSubscribed  ===========
/*!
        \brief               Get information about subscribed handsets
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvRegisteredHandsets(u16 u16_HsMask, u32 u32_Token)
{
    return cmbs_dsr_GET_InternalnameList(g_cmbsappl.pv_CMBSRef, u16_HsMask);
}

//      ========== app_SrvSetNewHandsetName  ===========
/*!
        \brief               Set new name for subscribed handset
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC app_SrvSetNewHandsetName(u16 u16_HsId, u8 *pu8_HsName, u16 u16_HsNameSize, u32 u32_Token)
{
    ST_IE_SUBSCRIBED_HS_LIST    st_SubscribedHsList;

    memset(&st_SubscribedHsList, 0, sizeof(st_SubscribedHsList));

    st_SubscribedHsList.u16_HsID = u16_HsId;
    memcpy(st_SubscribedHsList.u8_HsName, pu8_HsName, MIN(u16_HsNameSize, CMBS_HS_NAME_MAX_LENGTH));
    st_SubscribedHsList.u16_NameLength = u16_HsNameSize;
    return cmbs_dsr_SET_InternalnameList(g_cmbsappl.pv_CMBSRef, &st_SubscribedHsList);
}
E_CMBS_RC app_SrvSetNewHandsetNameWithCatIqProp(u16 u16_HsId, u8 *pu8_HsName, u16 u16_HsNameSize, u32 u32_Token, u16 u16_HsCapabilities)
{
    ST_IE_SUBSCRIBED_HS_LIST    st_SubscribedHsList;

    memset(&st_SubscribedHsList, 0, sizeof(st_SubscribedHsList));

    st_SubscribedHsList.u16_HsID = u16_HsId;
    memcpy(st_SubscribedHsList.u8_HsName, pu8_HsName, MIN(u16_HsNameSize, CMBS_HS_NAME_MAX_LENGTH));
    st_SubscribedHsList.u16_NameLength = u16_HsNameSize;
	st_SubscribedHsList.u16_HsCapabilities = u16_HsCapabilities;
    return cmbs_dsr_SET_InternalnameList(g_cmbsappl.pv_CMBSRef, &st_SubscribedHsList);
}

E_CMBS_RC app_SrvSetBackupedHandsetNames(void )
{
	u8 i;
	E_CMBS_RC ret = CMBS_RC_OK;

	APP_SRV_INFO_PRINT("\napp_SrvSetBackupedHandsetNames -->\n");
	
	for (i=0; i< CMBS_HS_SUBSCRIBED_MAX_NUM; i++)
	{
		if (g_u8_HsNameLen[i])
		{
			// HS index should start from 1, g_u8_HsName and g_u8_HsNameLen are 0 based
			ret = app_SrvSetNewHandsetName(i+1, (u8 *)(g_u8_HsName[i]), g_u8_HsNameLen[i], i);
			SleepMs(20);
		}
	}
		APP_SRV_INFO_PRINT("\napp_SrvSetBackupedHandsetNames <--\n");

	return ret;
}

//      ========== app_SrvSetNewHandsetNameAndLinePref  ===========
/*!
        \brief               Set new name for subscribed handset and it's line preference
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC app_SrvSetNewHandsetNameAndLinePref(u16 u16_HsId, u8 *pu8_HsName, u16 u16_HsNameSize, u32 u32_Token, u8 u8_LinePref, ST_IE_PROP_FIELDS_LIST *pst_PropSettingsList)
{
    ST_IE_SUBSCRIBED_HS_LIST    st_SubscribedHsList;

    memset(&st_SubscribedHsList, 0, sizeof(st_SubscribedHsList));

    st_SubscribedHsList.u16_HsID = u16_HsId;
    st_SubscribedHsList.u8_Pref_OutgoingCall_LineId = u8_LinePref;
    memcpy(st_SubscribedHsList.u8_HsName, pu8_HsName, MIN(u16_HsNameSize, CMBS_HS_NAME_MAX_LENGTH));
    st_SubscribedHsList.u16_NameLength = MIN(u16_HsNameSize, CMBS_HS_NAME_MAX_LENGTH);
    return cmbs_dsr_SET_InternalnamePropList(g_cmbsappl.pv_CMBSRef, &st_SubscribedHsList, pst_PropSettingsList);
}

//		========== app_SrvAddNewExtension  ===========
/*!
        \brief				 Add new extension to internal names list
        \param[in,out]		 extension number, extension name
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC app_SrvAddNewExtension(u8 *pu8_Name, u16 u16_NameSize, u8 *pu8_Number, u8 u8_NumberSize, u32 u32_Token)
{
    ST_IE_SUBSCRIBED_HS_LIST    st_Extension;

    memset(&st_Extension, 0, sizeof(st_Extension));


    memcpy(st_Extension.u8_HsName, pu8_Name, u16_NameSize); 
    st_Extension.u16_NameLength = u16_NameSize;
    st_Extension.u8_HsName[u16_NameSize] = 0;

    memcpy(st_Extension.u8_FXS_ExNum, pu8_Number, u8_NumberSize);
    st_Extension.u8_FXS_ExNumLen = u8_NumberSize;

    return cmbs_dsr_AddNewExtension(g_cmbsappl.pv_CMBSRef, &st_Extension);
}

//		========== app_SrvSetBaseName  ===========
/*!
        \brief				 Set new Base name
        \param[in,out]		 base name, base name length
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvSetBaseName(u8 *pu8_BaseName, u8 u8_BaseNameSize, u32 u32_Token)
{
    ST_IE_BASE_NAME st_BaseName;

    memset(&st_BaseName, 0, sizeof(st_BaseName));



    memcpy(st_BaseName.u8_BaseName, pu8_BaseName, u8_BaseNameSize);
    st_BaseName.u8_BaseNameLen = u8_BaseNameSize;

    return cmbs_dsr_SetBaseName(g_cmbsappl.pv_CMBSRef, &st_BaseName);

}

E_CMBS_RC   app_SrvGetBaseName(u32 u32_Token)
{
    return cmbs_dsr_GetBaseName(g_cmbsappl.pv_CMBSRef);
}

//      ========== app_SrvLineSettingsGet  ===========
/*!
        \brief               Get information about lines settings
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvLineSettingsGet(u16 u16_LinesMask, u32 u32_Token)
{
    return cmbs_dsr_GET_Line_setting_list(g_cmbsappl.pv_CMBSRef, u16_LinesMask);
}

//      ========== app_SrvLineSettingsSet  ===========
/*!
        \brief               Set information about lines settings
        \param[in,out]       <none>
        \return              <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvLineSettingsSet(ST_IE_LINE_SETTINGS_LIST *pst_LineSettingsList, u32 u32_Token)
{
    return cmbs_dsr_SET_Line_setting_list(g_cmbsappl.pv_CMBSRef, pst_LineSettingsList);
}


E_CMBS_RC      app_OnHandsetLinkRelease(void *pv_List)
{
    u8 u8_HsNumber;
    void *pv_IE;
    u16      u16_IE;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    cmbs_api_ie_HsNumberGet(pv_IE, &u8_HsNumber);

    APP_SRV_INFO_PRINT("app_OnHandsetLinkRelease HandsetNumber=%d\n", u8_HsNumber);

    cmbsevent_OnHandsetLinkReleased(u8_HsNumber);

    return CMBS_RC_OK;
}


//		========== app_SrvDectSettingsGet  ===========
/*!
        \brief				 get DECT settings
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvDectSettingsGet(u32 u32_Token)
{

    return cmbs_dsr_DectSettingsList_Get(g_cmbsappl.pv_CMBSRef);
}

//		========== app_SrvDectSettingsSet  ===========
/*!
        \brief				 set DECT settings
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvDectSettingsSet(ST_IE_DECT_SETTINGS_LIST *pst_DectSettingsList, u32 u32_Token)
{

    return cmbs_dsr_DectSettingsList_Set(g_cmbsappl.pv_CMBSRef, pst_DectSettingsList);
}

//		========== app_SrvPropDectSettingsSet  ===========
/*!
        \brief				 set DECT settings including Proprietary fields
        \param[in,out]		 u32_Token     TRUE, if upper application waits for answer
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC      app_SrvPropDectSettingsSet(ST_IE_DECT_SETTINGS_LIST *pst_DectSettingsList, ST_IE_PROP_FIELDS_LIST *pst_PropSettingsList,u32 u32_Token)
{

    return cmbs_dsr_PropDectSettingsList_Set(g_cmbsappl.pv_CMBSRef, pst_DectSettingsList, pst_PropSettingsList);
}

static u8 g_u8_HasValidDateTime = 1;
void app_SrvSetValidDateTime(u8 u8_Valid)
{
	g_u8_HasValidDateTime = u8_Valid;
}

u8 app_SrvGetValidDateTime(void)
{
	return g_u8_HasValidDateTime;
}

static E_CMBS_RC app_SrvSendCurrentDateAndTime(u8 u8_HsNum)
{
    ST_DATE_TIME   st_DateAndTime;

    time_t t;
    struct tm *t_m;

	memset (&st_DateAndTime, 0, sizeof (ST_DATE_TIME));
	if (g_u8_HasValidDateTime == 0)
	{
		st_DateAndTime.e_Coding = CMBS_DATE_TIME;
		st_DateAndTime.e_Interpretation = CMBS_REQUEST_CURR_TIME;
	}
	else
	{
	    t = time(NULL);
	    t_m = localtime(&t);

	    st_DateAndTime.e_Coding = CMBS_DATE_TIME;
	    st_DateAndTime.e_Interpretation = CMBS_CURRENT_TIME;

	    st_DateAndTime.u8_Year  = t_m->tm_year - 100;
	    st_DateAndTime.u8_Month = t_m->tm_mon + 1;
	    st_DateAndTime.u8_Day   = t_m->tm_mday;

	    st_DateAndTime.u8_Hours = t_m->tm_hour;
	    st_DateAndTime.u8_Mins  = t_m->tm_min;
	    st_DateAndTime.u8_Secs  = t_m->tm_sec;

	    st_DateAndTime.u8_Zone  = 8;
	}
	
    return app_FacilityDateTime( 1, &st_DateAndTime, 1 << (u8_HsNum - 1) );
}
//		========== app_OnEepromSizeResp ===========
/*!
        \brief			Processing of EEprom size responce event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_OnEepromSizeResp(void *pv_List)
{
    u16 u16_EepromSize = 0;
    void *pv_IE;
    u16      u16_IE;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
    cmbs_api_ie_ShortValueGet(pv_IE, &u16_EepromSize, CMBS_IE_SHORT_VALUE);

	if ( g_cmbsappl.n_Token )
	{
    	appcmbs_ObjectSignal((char *)&u16_EepromSize, sizeof(u16_EepromSize), 0, CMBS_EV_DSR_EEPROM_SIZE_GET_RES);
	}
}

//		========== app_OnEepromExtSizeResp ===========
/*!
        \brief			Processing of EEprom size responce event
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_OnEepromExtSizeResp(void *pv_List)
{
    u32 u32_EepromSize = 0;
    void *pv_IE;
    u16      u16_IE;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
    cmbs_api_ie_u32ValueGet(pv_IE, &u32_EepromSize, CMBS_IE_INTEGER_VALUE);

	if ( g_cmbsappl.n_Token )
	{
   		appcmbs_ObjectSignal((char *)&u32_EepromSize, sizeof(u32_EepromSize), 0, CMBS_EV_DSR_EEPROM_EXTENDED_SIZE_GET_RES);
	}
}

//		========== app_SrvGetEepromSize  ===========
/*!
        \brief				 get EEprom size
        \param[in,out]		 void
        \return				 <E_CMBS_RC>

        Obsolete
*/
E_CMBS_RC   app_SrvGetEepromSize(void)
{
    return cmbs_dsr_GetEepromSize(g_cmbsappl.pv_CMBSRef);
}

//		========== app_SrvGetEepromExtSize  ===========
/*!
        \brief				 get EEprom size
        \param[in,out]		 void
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC   app_SrvGetEepromExtSize(void)
{
    return cmbs_dsr_GetEepromExtSize(g_cmbsappl.pv_CMBSRef);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC   app_SrvReconnectResp(void)
{
    return cmbs_dsr_ReconnectResp(g_cmbsappl.pv_CMBSRef);
}
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
DWORD app_ReconnectThread(LPVOID lpThreadParameter)
{
    UNUSED_PARAMETER(lpThreadParameter);

    SleepMs(100);
    appcmbs_ReconnectApplication(0);
    cfr_ie_DeregisterThread((u32)GetCurrentThreadId()); 
    return 0;
}
#elif __linux__
void* app_ReconnectThread(void *ptr)
{
    UNUSED_PARAMETER(ptr);

    SleepMs(100);
#if SU_XHOST
#if SU_XHOST_USB
    appcmbs_ReconnectApplication(2000); // Wait 2 seconds before initializing so Linux can redetect the device
#endif
#else
    appcmbs_ReconnectApplication(0);
#endif
    cfr_ie_DeregisterThread((u32)pthread_self());
    return 0;
}
#endif
//		========== app_onDisconnectReq ===========
/*!
        \brief			Perform host reconnect on target request
        \param[in]     *pv_List		 IE list
        \return			<none>
*/
void app_onDisconnectReq(void)
{
    app_SrvReconnectResp();
    app_onTargetUp();
}


void app_onTargetUp(void)
{
	/*Target exception, release all calls and la */
	appcall_Initialize();
	app_LaInitialize();
	
#ifdef WIN32
	app_CreateThread((LPTHREAD_START_ROUTINE)app_ReconnectThread);
#elif __linux__
	app_CreateThread((start_routine)app_ReconnectThread);
#endif

    // for EEPROM on host
    if ( e_EepOnHostState == EEP_ON_HOST_READING_EEP )
    {
        // last state was "reading", now we received "target up". conclusion: target completed reading the EEPROM
        e_EepOnHostState = EEP_ON_HOST_READ_COMPLETED;
    }
}


#ifdef CMBS_COMA
static void app_CSSCOMAReset(void)
{
    cfr_comaStop();
    SleepMs(3000);
    system("/css/cssload.sh");
    APP_SRV_INFO_PRINT("Resetting COMA system.\nPlease, start App again...\n");
    exit(0);
}
#endif //CMBS_COMA

st_AFEConfiguration AFEconfig;


void     appsrv_AFEOpenAudioChannel(void)
{

    ST_IE_MEDIA_CHANNEL st_ChannelConfig;
    ST_IE_MEDIA_DESCRIPTOR st_CodecConfig;
    u8 u8_CodecsLength = 0;
    PST_CFR_IE_LIST pv_RefIEList = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    // construct information element for MediaDesc
    st_CodecConfig.e_Codec = AFEconfig.e_Codec;
    st_CodecConfig.pu8_CodecsList[u8_CodecsLength++] = AFEconfig.e_Codec;
    st_CodecConfig.pu8_CodecsList[u8_CodecsLength++] = AFEconfig.e_Codec == CMBS_AUDIO_CODEC_PCM_LINEAR_NB ? CMBS_AUDIO_CODEC_PCM_LINEAR_WB : CMBS_AUDIO_CODEC_PCM_LINEAR_NB;
    st_CodecConfig.u8_CodecsLength = u8_CodecsLength;
    cmbs_api_ie_MediaDescAdd(pv_RefIEList, &st_CodecConfig);

    // construct information element for media channel.
    // u32_ChannelParameter is the slot mask. u32_ChannelID is recieved from CMBS_EV_DSR_AFE_CHANNEL_ALLOCATE_RES
    st_ChannelConfig.u32_ChannelID = AFEconfig.u32_ChannelID;
    st_ChannelConfig.u32_ChannelParameter = AFEconfig.u32_SlotMask;
    st_ChannelConfig.e_Type = CMBS_MEDIA_TYPE_AUDIO_IOM;

    cmbs_api_ie_MediaChannelAdd(pv_RefIEList, &st_ChannelConfig);

    cmbs_dem_ChannelStart(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

}

void  appsrv_AFECloseAudioChannel(u32 u32_ChannelID)
{
    ST_APPCMBS_CONTAINER        st_Container;
    ST_IE_MEDIA_CHANNEL st_ChannelConfig;
    ST_IE_RESPONSE st_Response;
    PST_CFR_IE_LIST pv_RefIEList = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    // we only need the u32_ChannelID
    memset(&st_ChannelConfig, 0, sizeof(ST_IE_MEDIA_CHANNEL));

    st_ChannelConfig.u32_ChannelID = u32_ChannelID;
    cmbs_api_ie_MediaChannelAdd(pv_RefIEList, &st_ChannelConfig);
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dem_ChannelStop(g_cmbsappl.pv_CMBSRef, pv_RefIEList);

    appcmbs_WaitForContainer(CMBS_EV_DEM_CHANNEL_STOP_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    st_Response.e_Response = st_Container.ch_Info[0];

    if ( st_Response.e_Response != CMBS_RESPONSE_OK )
    {
        APP_SRV_ERROR_PRINT("CHANNEL STOP returned error!!!\n");
    }
    else
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        cmbs_dsr_AFE_ChannelDeallocate(g_cmbsappl.pv_CMBSRef, &st_ChannelConfig);

        appcmbs_WaitForEvent(CMBS_EV_DSR_AFE_CHANNEL_DEALLOCATE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }

}
void appsrv_AFEAllocateChannel(void)
{
    ST_APPCMBS_CONTAINER        st_Container;
    ST_IE_MEDIA_CHANNEL st_ChannelConfig;
    ST_IE_RESPONSE st_Response;
    void *pv_IE;
    u16      u16_IE;
	
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_AFE_ChannelAllocate(g_cmbsappl.pv_CMBSRef,  AFEconfig.u8_Resource);

    appcmbs_WaitForContainer(CMBS_EV_DSR_AFE_CHANNEL_ALLOCATE_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    st_Response.e_Response = CMBS_RESPONSE_ERROR;

    if ( st_Container.ch_Info[0] )
    {
        cmbs_api_ie_GetFirst((PST_CFR_IE_LIST)&st_Container.ch_Info, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            switch (u16_IE)
            {
            case CMBS_IE_AFE_INSTANCE_NUM:
                cmbs_api_ie_AFE_InstanceNumGet(pv_IE, &AFEconfig.u8_InstanceNum);
                APP_SRV_INFO_PRINT("u8_InstanceNumber is %2X \n", AFEconfig.u8_InstanceNum);
                break;
            case CMBS_IE_MEDIACHANNEL:
                cmbs_api_ie_MediaChannelGet(pv_IE, &st_ChannelConfig);
                AFEconfig.u32_ChannelID = st_ChannelConfig.u32_ChannelID;
                APP_SRV_INFO_PRINT("st_ChannelConfig is %2X, %2X, %2X \n", st_ChannelConfig.e_Type, st_ChannelConfig.u32_ChannelID, st_ChannelConfig.u32_ChannelParameter);
                break;
            case CMBS_IE_RESPONSE:
                cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                break;
            }
            cmbs_api_ie_GetNext((PST_CFR_IE_LIST)&st_Container.ch_Info, &pv_IE, &u16_IE);
        }
    }
    if ( st_Response.e_Response == CMBS_RESPONSE_ERROR )
    {
        APP_SRV_ERROR_PRINT("CMBS_EV_DSR_AFE_CHANNEL_ALLOCATE_RES returned ERROR response, not sending CHANNEL START\n");
    }

}


void app_AFEConnectEndpoints(void)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_AFE_EndpointConnect(g_cmbsappl.pv_CMBSRef, &AFEconfig.st_AFEEndpoints); // add CMBS_AFE_ADC_IN_START to each chosen number
    appcmbs_WaitForEvent(CMBS_EV_DSR_AFE_ENDPOINT_CONNECT_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void app_AFEEnableDisableEndpoint(ST_IE_AFE_ENDPOINT *pst_Endpoint, u16 u16_Enable)
{

    if ( u16_Enable )
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        cmbs_dsr_AFE_EndpointEnable(g_cmbsappl.pv_CMBSRef, pst_Endpoint);
        appcmbs_WaitForEvent(CMBS_EV_DSR_AFE_ENDPOINT_ENABLE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    }
    else
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        cmbs_dsr_AFE_EndpointDisable(g_cmbsappl.pv_CMBSRef, pst_Endpoint);
        appcmbs_WaitForEvent(CMBS_EV_DSR_AFE_ENDPOINT_DISABLE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }
}
void appsrv_AFESetEndpointGain(ST_IE_AFE_ENDPOINT_GAIN *pst_EndpointGain, u16 u16_Input)
{
    UNUSED_PARAMETER(u16_Input);
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_AFE_SetEndpointGain(g_cmbsappl.pv_CMBSRef, pst_EndpointGain);
    appcmbs_WaitForEvent(CMBS_EV_DSR_AFE_ENDPOINT_GAIN_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void appsrv_AFEDHSGSendByte(u8 u8_Value)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_DHSGValueSend(g_cmbsappl.pv_CMBSRef, u8_Value);
    appcmbs_WaitForEvent(CMBS_EV_DSR_DHSG_SEND_BYTE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void app_SrvGPIOEnable(PST_IE_GPIO_ID st_GPIOId)
{  
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	cmbs_dsr_GPIOEnable(g_cmbsappl.pv_CMBSRef, st_GPIOId);
    appcmbs_WaitForEvent(CMBS_EV_DSR_GPIO_ENABLE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void app_SrvGPIODisable(PST_IE_GPIO_ID st_GPIOId)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_GPIODisable(g_cmbsappl.pv_CMBSRef, st_GPIOId);
    appcmbs_WaitForEvent(CMBS_EV_DSR_GPIO_DISABLE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void app_SrvGPIOSet(PST_IE_GPIO_ID st_GPIOId, PST_GPIO_Properties pst_GPIOProp)
{
    void *pv_RefIEList = NULL;

    pv_RefIEList = cmbs_api_ie_GetList();

    if ( pv_RefIEList )
    {
        cmbs_api_ie_GPIOIDAdd(pv_RefIEList, st_GPIOId);

        if ( pst_GPIOProp[0].u8_Value != 0xF )
            cmbs_api_ie_GPIOModeAdd(pv_RefIEList, pst_GPIOProp[0].u8_Value);
        if ( pst_GPIOProp[1].u8_Value != 0xF )
            cmbs_api_ie_GPIOPullTypeAdd(pv_RefIEList, pst_GPIOProp[1].u8_Value);
        if ( pst_GPIOProp[2].u8_Value != 0xF )
            cmbs_api_ie_GPIOPullEnaAdd(pv_RefIEList, pst_GPIOProp[2].u8_Value);
        if ( pst_GPIOProp[3].u8_Value != 0xF )
            cmbs_api_ie_GPIOValueAdd(pv_RefIEList, pst_GPIOProp[3].u8_Value);

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    	cmbs_dsr_GPIOConfigSet(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
    	appcmbs_WaitForEvent(CMBS_EV_DSR_GPIO_CONFIG_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
	}
}

void app_SrvGPIOGet(PST_IE_GPIO_ID st_GPIOId, PST_GPIO_Properties pst_GPIOProp)
{
    void *pv_RefIEList = NULL;

    pv_RefIEList = cmbs_api_ie_GetList();

    if ( pv_RefIEList )
    {
        cmbs_api_ie_GPIOIDAdd(pv_RefIEList, st_GPIOId);

        if ( pst_GPIOProp[0].e_IE != 0xF )
            cmbs_api_ie_GPIOModeAdd(pv_RefIEList, pst_GPIOProp[0].u8_Value);
        if ( pst_GPIOProp[1].e_IE != 0xF )
            cmbs_api_ie_GPIOValueAdd(pv_RefIEList, pst_GPIOProp[1].u8_Value);
        if ( pst_GPIOProp[2].e_IE != 0xF )
            cmbs_api_ie_GPIOPullTypeAdd(pv_RefIEList, pst_GPIOProp[2].u8_Value);
        if ( pst_GPIOProp[3].e_IE != 0xF )
            cmbs_api_ie_GPIOPullEnaAdd(pv_RefIEList, pst_GPIOProp[3].u8_Value);

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    	cmbs_dsr_GPIOConfigGet(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
    	appcmbs_WaitForEvent(CMBS_EV_DSR_GPIO_CONFIG_GET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }

}

void app_SrvExtIntConfigure(PST_IE_GPIO_ID st_GpioId, PST_IE_INT_CONFIGURATION st_Config, u8 u8_IntNumber)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_ExtIntConfigure(g_cmbsappl.pv_CMBSRef, st_GpioId, st_Config, u8_IntNumber);
    appcmbs_WaitForEvent(CMBS_EV_DSR_EXT_INT_CONFIG_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void app_SrvExtIntEnable(u8 u8_IntNumber)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_ExtIntEnable(g_cmbsappl.pv_CMBSRef, u8_IntNumber);
    appcmbs_WaitForEvent(CMBS_EV_DSR_EXT_INT_ENABLE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void app_SrvExtIntDisable(u8 u8_IntNumber)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_ExtIntDisable(g_cmbsappl.pv_CMBSRef, u8_IntNumber);
    appcmbs_WaitForEvent(CMBS_EV_DSR_EXT_INT_DISABLE_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

E_CMBS_RC app_SrvLocateSuggest(u16 u16_Handsets)
{
    return cmbs_dsr_LocateSuggestReq(g_cmbsappl.pv_CMBSRef, u16_Handsets);
}

E_CMBS_RC app_SrvInfoSuggest(u8 u8_HsNumber, u8 * pu8_Data, u16 u16_DataLen)
{
    return cmbs_dsr_InfoSuggest(g_cmbsappl.pv_CMBSRef, u8_HsNumber, pu8_Data, u16_DataLen);
}

void appOnTerminalCapabilitiesInd(void *pv_List)
{
    UNUSED_PARAMETER(pv_List);
    //TBD: Use u8_EchoParameter parameter to enable/disable echo canceller for calls with this
    //specific HS
}
#ifdef PNCAP
void appProperitaryDataReceived(u16 u16_HsNo, u8 *pu8_Data, u16 u16_DataLen)
{
	UNUSED_PARAMETER(u16_HsNo);
	UNUSED_PARAMETER(pu8_Data);
	UNUSED_PARAMETER(u16_DataLen);
}
#endif
void appOnPropritaryDataRcvInd(void *pv_List)
{
    // TBD: Customer application should handle the proprietary message recieved from the handset
    void *pv_IE = NULL;
    u16      u16_IE;
    ST_IE_DATA pst_data;
    u16   u16_Handsets = 0;
    u32   u32_CallInstance = 0;
	UNUSED_PARAMETER(pv_List);

    if ( pv_List )
    {
        // collect information elements.
        // we expect: CMBS_IE_HANDSETS + CMBS_IE_CALLINSTANCE + CMBS_IE_DATA
        cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
    while (pv_IE != NULL)
    	{
        	switch (u16_IE)
	        {
	        case CMBS_IE_HANDSETS:
	            cmbs_api_ie_ShortValueGet(pv_IE, &u16_Handsets, CMBS_IE_HANDSETS);
	            break;
	        case CMBS_IE_CALLINSTANCE:
	            cmbs_api_ie_CallInstanceGet(pv_IE, &u32_CallInstance);
	            break;
	        case CMBS_IE_DATA:
	            cmbs_api_ie_DataGet(pv_IE, &pst_data);
	            break;
	        default:
	            break;
	        }
               cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);

    	}
#ifdef PNCAP
	appProperitaryDataReceived(u16_Handsets,pst_data.pu8_Data, pst_data.u16_DataLen);
#endif
    }
}

void app_SrvEEPROMHanBackupCreate(CMBS_EEPROM_DATA* backup)
{
	ST_APPCMBS_CONTAINER st_Container;
	E_CMBS_RC e_status;
	u32 u32_ULE_dect_start , u32_ULE_dect_end, u32_ULE_dect_size;
	u32 u32_ULE_start, u32_ULE_end, u32_ULE_size;
    u32 u32_FUN_start, u32_FUN_end, u32_FUN_size;
	u32 u32_GROUP_start, u32_GROUP_end, u32_GROUP_size;
	u32 u32_offsetGroupFile = 0;
	
	u8 pu8_filename_DECT[] = "DECT.bin"; //DECT file name
    u8 pu8_filename_ULE[] = "ULE.bin"; //ULE file name
    u8 pu8_filename_FUN[] = "FUN.bin"; //FUN file name
    u8 pu8_filename_GROUP[] = "GROUP.bin"; // GROUP file name

	static UnionHanEEPROMBackupEBlocksPools union_HanEEPROMBackupBlocks;

	FILE *f_binFile = 0;

	//Get next ULE TPUI
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	app_SrvParamGet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, 1);
	appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
	memcpy(&backup->m_ULENextTPUI, st_Container.ch_Info,  CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DECT Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	//Check binary files can be created
	e_status = tcx_fileOpen(&f_binFile, pu8_filename_DECT, (u8 *)"w+b");
	if ( e_status != CMBS_RC_OK )
	{
		APP_SRV_ERROR_PRINT("\nError: Could not create file: %s", pu8_filename_DECT);
		return;
	}

		
	// Get DECT parameters
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

   	// Get ULE DECT parameters
   	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

	//Calculate the needed size
    u32_ULE_dect_size = u32_ULE_dect_end - u32_ULE_dect_start;
		
    memset(&union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], 0, HAN_DECT_SUBS_LENGTH);

    //Get data from Target
    if ( u32_ULE_dect_size <= HAN_DECT_SUBS_LENGTH )
    {
        app_ParamAreaGetBySegments(u32_ULE_dect_start, u32_ULE_dect_size, &union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], g_CMBSMaxTransferSize);
   	}
   	 else
   	{
       	u32_ULE_dect_size = 0;
    }

    tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], 0, u32_ULE_dect_size);
    fflush(f_binFile);
    tcx_fileClose(f_binFile);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ULE Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Check binary files can be created
    e_status = tcx_fileOpen(&f_binFile, pu8_filename_ULE, (u8 *)"w+b");
    if ( e_status != CMBS_RC_OK )
    {
       	APP_SRV_ERROR_PRINT("\nError: Could not create file: %s", pu8_filename_ULE);
       	return;
    }

    // Get ULE  parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);


    // Get ULE parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

    //Calculate the needed size
    u32_ULE_size = u32_ULE_end - u32_ULE_start;

    memset(&union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA[0], 0, HAN_ULE_SUBS_LENGTH);

    //Get data from Target
    if ( u32_ULE_size <= HAN_ULE_SUBS_LENGTH )
    {
       	app_ParamAreaGetBySegments(u32_ULE_start, u32_ULE_size, &union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
       	APP_SRV_ERROR_PRINT("Error in ULE  DB size - received 0x%X\n", u32_ULE_size);
       	u32_ULE_size = 0;
   	}

    tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA[0], 0, u32_ULE_size);
    fflush(f_binFile);
    tcx_fileClose(f_binFile);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// FUN Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
    // Check binary files can be created
    e_status = tcx_fileOpen(&f_binFile, pu8_filename_FUN, (u8 *)"w+b");
    if ( e_status != CMBS_RC_OK )
    {
       	APP_SRV_ERROR_PRINT("\nError: Could not create file: %s", pu8_filename_FUN);
       	return;
    }

    // Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);


    // Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

    //Calculate the needed size
    u32_FUN_size = u32_FUN_end - u32_FUN_start;

    memset(&union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA[0], 0, HAN_FUN_SUBS_LENGTH);

    //Get data from Target
    if ( u32_FUN_size <= HAN_FUN_SUBS_LENGTH )
    {
       	app_ParamAreaGetBySegments(u32_FUN_start, u32_FUN_size, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
       	u32_FUN_size = 0;
    }

    tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA[0], 0, u32_FUN_size);
    fflush(f_binFile);
	tcx_fileClose(f_binFile);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GROUP Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////
	//Get GROUP LIST:
	/////////////////


	// Check binary files can be created
    e_status = tcx_fileOpen(&f_binFile, pu8_filename_GROUP, (u8 *)"w+b");
    if ( e_status != CMBS_RC_OK )
    {
       	APP_SRV_ERROR_PRINT("\nError: Could not create file: %s", pu8_filename_GROUP);
       	return;
    }

	
    // Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_GROUP_LIST_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

	if (st_Container.n_InfoLen != 0)
	{
		b_RestoreGroupEep = TRUE;
	}
	else
	{
		APP_SRV_ERROR_PRINT("\nTarget version don't support Group");
		tcx_fileClose(f_binFile);
		return;
	}
    memcpy(&u32_GROUP_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

    // Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_GROUP_LIST_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_GROUP_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);



	
	//Calculate the needed size
    u32_GROUP_size = u32_GROUP_end - u32_GROUP_start;

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_LIST_DATA[0], 0, HAN_FUN_GROUP_LIST_LENGTH);

    //Get data from Target
    if ( u32_GROUP_size <= HAN_FUN_GROUP_LIST_LENGTH )
    {
       	app_ParamAreaGetBySegments(u32_GROUP_start, u32_GROUP_size, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_LIST_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
       	u32_GROUP_size = 0;
    }	

	tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_LIST_DATA[0], u32_offsetGroupFile, u32_GROUP_size);
	fflush(f_binFile);

	u32_offsetGroupFile += u32_GROUP_size;

	
	//////////////////
	//Get GROUP TABLE:
	//////////////////

	// Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_GROUP_TABLE_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_GROUP_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

    // Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_GROUP_TABLE_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_GROUP_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

	//Calculate the needed size
    u32_GROUP_size = u32_GROUP_end - u32_GROUP_start;

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_TABLE_DATA[0], 0, HAN_FUN_GROUP_TABLE_LENGTH);

    //Get data from Target
    if ( u32_GROUP_size <= HAN_FUN_GROUP_TABLE_LENGTH )
    {
       	app_ParamAreaGetBySegments(u32_GROUP_start, u32_GROUP_size, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_TABLE_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
       	u32_GROUP_size = 0;
    }	


	tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_TABLE_DATA[0], u32_offsetGroupFile, u32_GROUP_size);
	fflush(f_binFile);

	u32_offsetGroupFile += u32_GROUP_size;

	/////////////////////////////////
	//Get BROADCAST CONVERSION TABLE:
	/////////////////////////////////

	// Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_GROUP_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

    // Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_GROUP_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

	//Calculate the needed size
    u32_GROUP_size = u32_GROUP_end - u32_GROUP_start;

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_ULE_BROADCAST_CONVERSION_TABLE_DATA[0], 0, HAN_ULE_BROADCAST_CONVERSION_TABLE_LENGTH);

    //Get data from Target
    if ( u32_GROUP_size <= HAN_ULE_BROADCAST_CONVERSION_TABLE_LENGTH )
    {
       	app_ParamAreaGetBySegments(u32_GROUP_start, u32_GROUP_size, &union_HanEEPROMBackupBlocks.pu8_HAN_ULE_BROADCAST_CONVERSION_TABLE_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
       	u32_GROUP_size = 0;
    }	

	tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_ULE_BROADCAST_CONVERSION_TABLE_DATA[0], u32_offsetGroupFile, u32_GROUP_size);
	fflush(f_binFile);

	u32_offsetGroupFile += u32_GROUP_size;

	// Get ULE MULTICAST ENC PARAMS
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	app_SrvParamGet(CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS, 1);
	appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
	memcpy(&union_HanEEPROMBackupBlocks.pu8_HAN_ULE_MULTICAST_ENC_PARAMS_DATA[0], st_Container.ch_Info,  CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH);
	
	tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_ULE_MULTICAST_ENC_PARAMS_DATA[0], u32_offsetGroupFile, CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH);
	fflush(f_binFile);
    tcx_fileClose(f_binFile);
}

void app_SrvEEPROMRepeaterBackupCreate(u32 u32_Repeater_start, u32 u32_Repeater_size)
{
	E_CMBS_RC e_status;
	
	u8 pu8_filename_REPEATER_SUB[] = "Repeater.bin"; //Repeater data file name
 
	static UnionHanEEPROMBackupEBlocksPools union_HanEEPROMBackupBlocks;

	FILE *f_binFile = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Repeater subscription Data
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Check binary files can be created
    e_status = tcx_fileOpen(&f_binFile, pu8_filename_REPEATER_SUB, (u8 *)"w+b");
    if ( e_status != CMBS_RC_OK )
    {
       	APP_SRV_ERROR_PRINT("\nError: Could not create file: %s", pu8_filename_REPEATER_SUB);
       	return;
    }

    memset(&union_HanEEPROMBackupBlocks.pu8_REPEATER_SUBS_DATA[0], 0, PARAM_REPEATER_SUBS_DATA_LENGTH);

    //Get data from Target
    app_ParamAreaGetBySegments(u32_Repeater_start, u32_Repeater_size, &union_HanEEPROMBackupBlocks.pu8_REPEATER_SUBS_DATA[0], g_CMBSMaxTransferSize);

    tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_REPEATER_SUBS_DATA[0], 0, u32_Repeater_size);
    fflush(f_binFile);
    tcx_fileClose(f_binFile);
}    
    
    
E_CMBS_RC app_SrvEEPROMBackupCreate(void)
{
    char *psz_EepFileName = "EEPROMBackup.bin";
    CMBS_EEPROM_DATA backup;
    ST_APPCMBS_CONTAINER st_Container;
	
	u32 u32_ULE_dect_start , u32_ULE_dect_end, u32_ULE_dect_size;
    u32 u32_Repeater_end, u32_Repeater_start, u32_Repeater_size = 0;
	
    memset(&backup, 0, sizeof(CMBS_EEPROM_DATA));

    if ( tcx_EepNewFile(psz_EepFileName, sizeof(CMBS_EEPROM_DATA)) == 0 )
    {
        APP_SRV_ERROR_PRINT("Can't create new EEPROM file %s size %zu\n", psz_EepFileName, sizeof(CMBS_EEPROM_DATA));
        return CMBS_RC_ERROR_GENERAL;
    }
    else
    {
        // CMBS_PARAM_DECT_TYPE
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamGet(CMBS_PARAM_DECT_TYPE, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_DectType, st_Container.ch_Info, CMBS_PARAM_DECT_TYPE_LENGTH);

        // CMBS_PARAM_RF_FULL_POWER
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF_FULL_POWER, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_RF_FULL_POWER, st_Container.ch_Info,  CMBS_PARAM_RF_FULL_POWER_LENGTH);

        // CMBS_PARAM_RFPI
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RFPI, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_RFPI, st_Container.ch_Info, CMBS_PARAM_RFPI_LENGTH);

        // CMBS_PARAM_RXTUN
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RXTUN, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_RXTUN, st_Container.ch_Info, CMBS_PARAM_RXTUN_LENGTH);

        //CMBS_PARAM_PREAM_NORM
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_PREAM_NORM, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_PreamNormal, st_Container.ch_Info, CMBS_PARAM_PREAM_NORM_LENGTH);

        //CMBS_PARAM_RF19APU_SUPPORT_FCC
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_SUPPORT_FCC, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_RF19APUSupportFCC, st_Container.ch_Info, CMBS_PARAM_RF19APU_SUPPORT_FCC_LENGTH);

        //CMBS_PARAM_RF19APU_DEVIATION
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_DEVIATION, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_RF19APUDeviation, st_Container.ch_Info, CMBS_PARAM_RF19APU_DEVIATION_LENGTH);

        //CMBS_PARAM_RF19APU_PA2_COMP
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_PA2_COMP, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_RF19APUPA2Comp, st_Container.ch_Info, CMBS_PARAM_RF19APU_PA2_COMP_LENGTH);

        //CMBS_PARAM_MAX_USABLE_RSSI
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_MAX_USABLE_RSSI, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_MAXUsableRSSI, st_Container.ch_Info, CMBS_PARAM_MAX_USABLE_RSSI_LENGTH);

        //CMBS_PARAM_LOWER_RSSI_LIMIT
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_LOWER_RSSI_LIMIT, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_LowerRSSILimit, st_Container.ch_Info, CMBS_PARAM_LOWER_RSSI_LIMIT_LENGTH);

        //CMBS_PARAM_PHS_SCAN_PARAM
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_PHS_SCAN_PARAM, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_PHSScanParam, st_Container.ch_Info, CMBS_PARAM_PHS_SCAN_PARAM_LENGTH);

        //CMBS_PARAM_JDECT_LEVEL1_M82
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_JDECT_LEVEL1_M82, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_JDECTLevelM82, st_Container.ch_Info, CMBS_PARAM_JDECT_LEVEL1_M82_LENGTH);

        //CMBS_PARAM_JDECT_LEVEL2_M62
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_JDECT_LEVEL2_M62, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_JDECTLevelM62, st_Container.ch_Info, CMBS_PARAM_JDECT_LEVEL2_M62_LENGTH);

        //CMBS_PARAM_SUBS_DATA
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_SUBS_DATA, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_SUBSDATA, st_Container.ch_Info, CMBS_PARAM_SUBS_DATA_LENGTH);

        //CMBS_PARAM_SUBS_DATA_EX
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_SUBS_DATA_EX, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_SUBSDATA_EX, st_Container.ch_Info, CMBS_PARAM_SUBS_DATA_EX_LENGTH);

        //CMBS_PARAM_REPEATER_SUBS_START
	    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_START, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        if ( st_Container.ch_Info[0] == CMBS_RESPONSE_ERROR )
        {
            // ASA: checking for ERROR might be a problem if a valid size has the same byte value.
            printf("\n !! Repeater support not enabled in target SW !!!\n");
        }
        else
        {
            memcpy(&u32_Repeater_start, st_Container.ch_Info,  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH);
	        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.

            app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_END, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

            memcpy(&u32_Repeater_end, st_Container.ch_Info,  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH);

            //Calculate the needed size
            u32_Repeater_size = u32_Repeater_end - u32_Repeater_start;

            //Get data from Target
            if (u32_Repeater_size <= (CMBS_CRFP_SUBSCRIBED_MAX_NUM * sizeof(ST_CMBS_SUBS_DATA)))
            {
  	            // app_ParamAreaGetBySegments(u32_Repeater_start, u32_Repeater_size, (u8 *) &u8_RepeaterSubsData, g_CMBSMaxTransferSize);
  	            app_SrvEEPROMRepeaterBackupCreate(u32_Repeater_start, u32_Repeater_size);
            }
        }
                
        
        //CMBS_PARAM_AUTH_PIN
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_AUTH_PIN, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&backup.m_AUTH_PIN, st_Container.ch_Info, CMBS_PARAM_PIN_CODE_LENGTH);

		//Check if Han Backup is needed:

		// Get DECT parameters
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    	app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
    	appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    	memcpy(&u32_ULE_dect_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

   		// Get ULE DECT parameters
   		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    	app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
    	appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    	memcpy(&u32_ULE_dect_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);



		//Calculate the needed size
    	u32_ULE_dect_size = u32_ULE_dect_end - u32_ULE_dect_start;

		if(u32_ULE_dect_size != 0 )
		{
			app_SrvEEPROMHanBackupCreate(&backup);
		}
		
		
        if ( appcmbs_get_CMBSAppInstance()->u8_HwChip == CMBS_HW_CHIP_VEGAONE )
        {
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_RVREF, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(&backup.m_RVREF, st_Container.ch_Info, CMBS_PARAM_RVREF_LENGTH);

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_GFSK, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(&backup.m_GFSK, st_Container.ch_Info, CMBS_PARAM_GFSK_LENGTH);
        }
        backup.m_Initialized = 1;

        // write EEPROM parameters to a file
        tcx_EepOpen(psz_EepFileName);
        tcx_EepWrite((u8 *)&backup, 0, sizeof(CMBS_EEPROM_DATA));
        tcx_EepClose();
    }

    return CMBS_RC_OK;
}

E_CMBS_RC app_SrvEEPROMHanBackupRestore(CMBS_EEPROM_DATA* st_backup)
{
	ST_APPCMBS_CONTAINER st_Container;
	u32 u32_ULE_dect_start,u32_ULE_dect_end, u32_ULE_dect_size;
    u32 u32_ULE_start, u32_ULE_size = 0;
    u32 u32_FUN_start, u32_FUN_size = 0;
	u32 u32_Group_List_start, u32_Group_List_size, u32_Total_Group_info_size;
	u32 u32_Group_Table_start, u32_Group_Table_size;
	u32 u32_Broadcast_Conversion_Table_start, u32_Broadcast_Conversion_Table_size;
	u32 u32_Filesize = 0;
	u32 u32_offsetGroupFile = 0;

	u8 pu8_filename_DECT[] = "DECT.bin"; //DECT file name
    u8 pu8_filename_ULE[] = "ULE.bin"; //ULE file name
    u8 pu8_filename_FUN[] = "FUN.bin"; //FUN file name
    u8 pu8_filename_GROUP[] = "GROUP.bin"; // GROUP file name

	FILE *f_binFile = 0;

	
	s8 s8_i;
    E_CMBS_RC e_status;
	static UnionHanEEPROMBackupEBlocksPools union_HanEEPROMBackupBlocks;

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], 0, HAN_DECT_SUBS_LENGTH);


	e_status = tcx_fileOpen(&f_binFile, pu8_filename_DECT, (u8 *)"r+b");
	
	u32_ULE_dect_size = tcx_fileSize(f_binFile);

	tcx_fileClose(f_binFile);

	u32_Group_List_size = HAN_FUN_GROUP_LIST_LENGTH;
	u32_Group_Table_size = HAN_FUN_GROUP_TABLE_LENGTH;
	u32_Broadcast_Conversion_Table_size = HAN_ULE_BROADCAST_CONVERSION_TABLE_LENGTH;

	// restore ULE next TPUI
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	app_SrvParamSet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, st_backup->m_ULENextTPUI, CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH, 1);
	appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DECT Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	// Get DECT parameters
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);

	// Get DECT parameters
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_end, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);

	if(u32_ULE_dect_end - u32_ULE_dect_start < u32_ULE_dect_size)
	{
		APP_SRV_ERROR_PRINT("Abort app_SrvEEPROMHanBackupRestore! New version can't support the same number of devices");
		return CMBS_RC_ERROR_GENERAL;
	}

    // Get data from file
    e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA, pu8_filename_DECT, &u32_Filesize);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
       u32_ULE_dect_size = 0;
       return CMBS_RC_ERROR_GENERAL;
     }

	if(u32_ULE_dect_size == 0)
	{
		/* use the size of the file */
		u32_ULE_dect_size = u32_Filesize;
	}

    // Check the DECT DB size
    if ( u32_ULE_dect_size )
    {
		APP_SRV_INFO_PRINT("DECT Size = %d\n", u32_ULE_dect_size);
        app_ParamAreaSetBySegments(u32_ULE_dect_start, u32_ULE_dect_size, union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        APP_SRV_ERROR_PRINT("Fail !");
		return CMBS_RC_ERROR_GENERAL;
    }


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ULE Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Get ULE  parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);

    // Get data from file
    e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA, pu8_filename_ULE, &u32_Filesize);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
        u32_ULE_size = 0;
        return CMBS_RC_ERROR_GENERAL;
     }

	if(u32_ULE_size == 0)
	{
		/* use the size of the file */
		u32_ULE_size = u32_Filesize;
	}


    if ( u32_ULE_size )
    {
		APP_SRV_INFO_PRINT("ULE Size = %d\n", u32_ULE_size);
        app_ParamAreaSetBySegments(u32_ULE_start, u32_ULE_size, union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        APP_SRV_ERROR_PRINT("Fail !");
		return CMBS_RC_ERROR_GENERAL;
    }


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// FUN Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Get FUN parameters
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);

    // Get data from file
    e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA, pu8_filename_FUN, &u32_Filesize);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
          u32_FUN_size = 0;
          return CMBS_RC_ERROR_GENERAL;
    }

	if(u32_FUN_size == 0) 
	{	/* use the size of the file */
		u32_FUN_size = u32_Filesize;
	}	

    if ( u32_FUN_size )
    {
		APP_SRV_INFO_PRINT("FUNN Size = %d\n", u32_FUN_size);
        app_ParamAreaSetBySegments(u32_FUN_start, u32_FUN_size, union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        APP_SRV_ERROR_PRINT("Fail !");
		return CMBS_RC_ERROR_GENERAL;
    }


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GROUP Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	e_status = tcx_fileOpen(&f_binFile, pu8_filename_GROUP, (u8 *)"r+b");

	if(e_status == CMBS_RC_OK)
	{
		u32_Total_Group_info_size = tcx_fileSize(f_binFile);

		if (u32_Total_Group_info_size == HAN_FUN_GROUP_LIST_LENGTH + HAN_FUN_GROUP_TABLE_LENGTH + 
			HAN_ULE_BROADCAST_CONVERSION_TABLE_LENGTH + CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH)
		{
			// try reading group list offset	
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
			app_SrvParamGet(CMBS_PARAM_HAN_FUN_GROUP_LIST_START, 1);
			appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
			
			/////////////////
			//Get GROUP LIST:
			/////////////////
			if (st_Container.n_InfoLen != 0)
			{
				memcpy(&u32_Group_List_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);

			    // Get data from file
				e_status = tcx_fileRead(f_binFile, union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_LIST_DATA, u32_offsetGroupFile, u32_Group_List_size);
			    if ( e_status == CMBS_RC_ERROR_GENERAL )
			    {
			          u32_Group_List_size = 0;
			          return CMBS_RC_ERROR_GENERAL;
			    }

				u32_offsetGroupFile += u32_Group_List_size;

			    if ( u32_Group_List_size )
			    {
					APP_SRV_INFO_PRINT("Group List Size = %d\n", u32_Group_List_size);
			        app_ParamAreaSetBySegments(u32_Group_List_start, u32_Group_List_size, union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_LIST_DATA, g_CMBSMaxTransferSize);
			    }
			    else
			    {
			        APP_SRV_ERROR_PRINT("Fail !");
					return CMBS_RC_ERROR_GENERAL;
			    }


				//////////////////
				//Get GROUP TABLE:
				//////////////////



				// Get FUN parameters
			    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
			    app_SrvParamGet(CMBS_PARAM_HAN_FUN_GROUP_TABLE_START, 1);
			    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
			    memcpy(&u32_Group_Table_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);

			    // Get data from file
				e_status = tcx_fileRead(f_binFile, union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_TABLE_DATA, u32_offsetGroupFile, u32_Group_Table_size);
			    if ( e_status == CMBS_RC_ERROR_GENERAL )
			    {
			          u32_Group_Table_size = 0;
			          return CMBS_RC_ERROR_GENERAL;
			    }

				u32_offsetGroupFile += u32_Group_Table_size;

			    if ( u32_Group_Table_size )
			    {
					APP_SRV_INFO_PRINT("Group Table Size = %d\n", u32_Group_Table_size);
			        app_ParamAreaSetBySegments(u32_Group_Table_start, u32_Group_Table_size, union_HanEEPROMBackupBlocks.pu8_HAN_FUN_GROUP_TABLE_DATA, g_CMBSMaxTransferSize);
			    }
			    else
			    {
			        APP_SRV_ERROR_PRINT("Fail !\n");
					return CMBS_RC_ERROR_GENERAL;
			    }

				

				/////////////////////////////////
				//Get BROADCAST CONVERSION TABLE:
				/////////////////////////////////
				

				// Get FUN parameters
			    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
			    app_SrvParamGet(CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_START, 1);
			    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
			    memcpy(&u32_Broadcast_Conversion_Table_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);

			    // Get data from file
				e_status = tcx_fileRead(f_binFile, union_HanEEPROMBackupBlocks.pu8_HAN_ULE_BROADCAST_CONVERSION_TABLE_DATA, u32_offsetGroupFile, u32_Broadcast_Conversion_Table_size);
			    if ( e_status == CMBS_RC_ERROR_GENERAL )
			    {
			          u32_Broadcast_Conversion_Table_size = 0;
			          return CMBS_RC_ERROR_GENERAL;
			    }

				u32_offsetGroupFile += u32_Broadcast_Conversion_Table_size;

			    if ( u32_Broadcast_Conversion_Table_size )
			    {
					APP_SRV_INFO_PRINT("Broadcast Conversion Table Size = %d\n", u32_Broadcast_Conversion_Table_size);
			        app_ParamAreaSetBySegments(u32_Broadcast_Conversion_Table_start, u32_Broadcast_Conversion_Table_size, union_HanEEPROMBackupBlocks.pu8_HAN_ULE_BROADCAST_CONVERSION_TABLE_DATA, g_CMBSMaxTransferSize);
			    }
			    else
			    {
			        APP_SRV_ERROR_PRINT("Fail !\n");
					return CMBS_RC_ERROR_GENERAL;
			    }

				///////////////////////////////////////////////////////

			    // Get data from file
				e_status = tcx_fileRead(f_binFile, union_HanEEPROMBackupBlocks.pu8_HAN_ULE_MULTICAST_ENC_PARAMS_DATA, u32_offsetGroupFile, CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH);
			    if ( e_status == CMBS_RC_ERROR_GENERAL )
			    {
			          u32_Broadcast_Conversion_Table_size = 0;
			          return CMBS_RC_ERROR_GENERAL;
			    }

				// CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS
				appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
			    app_SrvParamSet(CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS, union_HanEEPROMBackupBlocks.pu8_HAN_ULE_MULTICAST_ENC_PARAMS_DATA, CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH, 1);
			    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

				tcx_fileClose(f_binFile);
			}
			else
			{
				APP_SRV_ERROR_PRINT("\nNew Target version doesn't support Group\n");
			}
		}
		else
		{
			APP_SRV_ERROR_PRINT("\nOld Target version doesn't support Group\n");
		}
	}
	else
	{
		APP_SRV_ERROR_PRINT("\nError: Could not Open file: %s\n", pu8_filename_GROUP);
	}
		
	///////////////////////////////////////////////////////
	

    if(b_RestoreEepOnly)
    {	/* 
    	 * Directly reboot CMBS and will quit cmbs_tcx *
    	 * Don't delete the files *
    	*/
		cmbs_dsr_sys_Reset(g_cmbsappl.pv_CMBSRef);
    }
	else
	{
	    for (s8_i = 20; s8_i >= 0; --s8_i)
	    {
	        APP_SRV_INFO_PRINT("\rWaiting for target to finish writing to EEPROM... %02d", s8_i);
	        SleepMs(1000);
	    }

	    // system restart
	    APP_SRV_INFO_PRINT("Restarting system... \n");
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	    app_SrvSystemReboot();

		appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
		
	    tcx_fileDelete((char *)pu8_filename_DECT);
	    tcx_fileDelete((char *)pu8_filename_ULE);
	    tcx_fileDelete((char *)pu8_filename_FUN);
		tcx_fileDelete((char *)pu8_filename_GROUP);
    }

	return CMBS_RC_OK;
}


E_CMBS_RC app_SrvEEPROMRepeaterBackupRestore(u32 u32_Repeater_start, u32 u32_Repeater_end)
{
    u32 u32_Repeater_size = 0;
	u8 pu8_filename_REPEATER_SUB[] = "Repeater.bin"; //Repeater data file name

	FILE *f_binFile = 0;
	u32 u32_Filesize = 0;
	
	s8 s8_i;
    E_CMBS_RC e_status;
	static UnionHanEEPROMBackupEBlocksPools union_HanEEPROMBackupBlocks;

    memset(&union_HanEEPROMBackupBlocks.pu8_REPEATER_SUBS_DATA[0], 0, PARAM_REPEATER_SUBS_DATA_LENGTH);

	e_status = tcx_fileOpen(&f_binFile, pu8_filename_REPEATER_SUB, (u8 *)"r+b");
	
	u32_Repeater_size = tcx_fileSize(f_binFile);

	tcx_fileClose(f_binFile);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// repeater subscription data Block
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	if(u32_Repeater_end - u32_Repeater_start < u32_Repeater_size)
	{
		APP_SRV_ERROR_PRINT("Abort app_SrvEEPROMRepeaterBackupRestore! New version can't support the same number of repeater subscriptions");
		return CMBS_RC_ERROR_GENERAL;
	}

    // Get data from file
    e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_REPEATER_SUBS_DATA, pu8_filename_REPEATER_SUB, &u32_Filesize);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
       u32_Repeater_size = 0;
       return CMBS_RC_ERROR_GENERAL;
     }

	if(u32_Repeater_size == 0)
	{
		/* use the size of the file */
		u32_Repeater_size = u32_Filesize;
	}

    // Check the DECT DB size
    if ( u32_Repeater_size )
    {
		APP_SRV_INFO_PRINT("Repeater data Size = %d\n", u32_Repeater_size);
        app_ParamAreaSetBySegments(u32_Repeater_start, u32_Repeater_size, union_HanEEPROMBackupBlocks.pu8_REPEATER_SUBS_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        APP_SRV_ERROR_PRINT("Fail !");
		return CMBS_RC_ERROR_GENERAL;
    }
    
	for (s8_i = 5; s8_i >= 0; --s8_i)
	{
	    APP_SRV_INFO_PRINT("\rWaiting for target to finish writing to EEPROM... %02d", s8_i);
	    SleepMs(1000);
	}

    /* ASA: System restart is done for ULE and RFPI, I don't see a need to perform it for repeater:
	// system restart
	APP_SRV_INFO_PRINT("Restarting system... \n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	app_SrvSystemReboot();

	appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    END od ASA comment 
            */

	tcx_fileDelete((char *)pu8_filename_REPEATER_SUB);

	return CMBS_RC_OK;
	
}    
E_CMBS_RC app_SrvEEPROMBackupRestore(void)
{
	ST_APPCMBS_CONTAINER st_Container;
    char *psz_EepFileName = "EEPROMBackup.bin";
    CMBS_EEPROM_DATA st_backup;

	u32 u32_ULE_dect_start , u32_ULE_dect_end, u32_ULE_dect_size;
    u32 u32_Repeater_end, u32_Repeater_start;
	
    APP_SRV_INFO_PRINT("Restoring EEPROM from file...\n");
    if(tcx_EepOpen(psz_EepFileName) == 0)
    {
		return CMBS_RC_ERROR_GENERAL;
    }

    //Read EEPROM backup values from a file
    if(tcx_EepRead((u8 *)&st_backup, 0, sizeof(CMBS_EEPROM_DATA)) == 0)
    {
		return CMBS_RC_ERROR_GENERAL;
    }

	tcx_EepClose();

	//do not delete file, in case of failure it is possible to run restore again
	//tcx_fileDelete(psz_EepFileName);
	
    if ( !st_backup.m_Initialized )
    {
        APP_SRV_ERROR_PRINT("EEPROM backup file is not initialized!\n");
		return CMBS_RC_ERROR_GENERAL;
    }

    // CMBS_PARAM_DECT_TYPE
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_ProductionParamSet(CMBS_PARAM_DECT_TYPE, &st_backup.m_DectType, CMBS_PARAM_DECT_TYPE_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // CMBS_PARAM_RF_FULL_POWER
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RF_FULL_POWER, &st_backup.m_RF_FULL_POWER, CMBS_PARAM_RF_FULL_POWER_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // CMBS_PARAM_RFPI
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RFPI, st_backup.m_RFPI, CMBS_PARAM_RFPI_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

	if(!b_RestoreEepOnly)
	{	/* If we recover EEPROM to another new sample, we don't want to overwrite the new sample's RXTUN */
	    // CMBS_PARAM_RXTUN
	    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	    app_SrvParamSet(CMBS_PARAM_RXTUN, st_backup.m_RXTUN, CMBS_PARAM_RXTUN_LENGTH, 1);
	    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
	}	

    //CMBS_PARAM_PREAM_NORM
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_PREAM_NORM, &st_backup.m_PreamNormal, CMBS_PARAM_PREAM_NORM_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_RF19APU_SUPPORT_FCC
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RF19APU_SUPPORT_FCC, &st_backup.m_RF19APUSupportFCC, CMBS_PARAM_RF19APU_SUPPORT_FCC_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_RF19APU_DEVIATION
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RF19APU_DEVIATION, &st_backup.m_RF19APUDeviation, CMBS_PARAM_RF19APU_DEVIATION_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_RF19APU_PA2_COMP
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RF19APU_PA2_COMP, &st_backup.m_RF19APUPA2Comp, CMBS_PARAM_RF19APU_PA2_COMP_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_MAX_USABLE_RSSI
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_MAX_USABLE_RSSI, &st_backup.m_MAXUsableRSSI, CMBS_PARAM_MAX_USABLE_RSSI_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_LOWER_RSSI_LIMIT
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_LOWER_RSSI_LIMIT, &st_backup.m_LowerRSSILimit, CMBS_PARAM_LOWER_RSSI_LIMIT_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_PHS_SCAN_PARAM
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_PHS_SCAN_PARAM, &st_backup.m_PHSScanParam, CMBS_PARAM_PHS_SCAN_PARAM_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_JDECT_LEVEL1_M82
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_JDECT_LEVEL1_M82, &st_backup.m_JDECTLevelM82, CMBS_PARAM_JDECT_LEVEL1_M82_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_JDECT_LEVEL2_M62
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_JDECT_LEVEL2_M62, &st_backup.m_JDECTLevelM62, CMBS_PARAM_JDECT_LEVEL2_M62_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // CMBS_PARAM_SUBS_DATA
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_SUBS_DATA, st_backup.m_SUBSDATA, CMBS_PARAM_SUBS_DATA_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    
    // CMBS_PARAM_SUBS_DATA_EX
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_SUBS_DATA_EX, st_backup.m_SUBSDATA_EX, CMBS_PARAM_SUBS_DATA_EX_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //CMBS_PARAM_REPEATER_SUBS_START
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    if ( st_Container.ch_Info[0] == CMBS_RESPONSE_ERROR )
    {
        // ASA: checking for ERROR might be a problem if a valid size has the same byte value.
        printf("\n !! Repeater support not enabled in target SW !!!\n");
    }
    else
    {
        memcpy(&u32_Repeater_start, st_Container.ch_Info,  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH);
	    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.

        app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_END, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

        memcpy(&u32_Repeater_end, st_Container.ch_Info,  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH);

        //Calculate the needed size
        // u32_Repeater_size = u32_Repeater_end - u32_Repeater_start;

        //Set data in Target
        // if (u32_Repeater_size <= (CMBS_CRFP_SUBSCRIBED_MAX_NUM * sizeof(ST_CMBS_SUBS_DATA)))
        {
  	        app_SrvEEPROMRepeaterBackupRestore(u32_Repeater_start, u32_Repeater_end);
        }
    }
       
    	    
    // CMBS_PARAM_AUTH_PIN
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_AUTH_PIN, st_backup.m_AUTH_PIN, CMBS_PARAM_PIN_CODE_LENGTH, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    if ( appcmbs_get_CMBSAppInstance()->u8_HwChip == CMBS_HW_CHIP_VEGAONE )
    {
        // CMBS_PARAM_RVREF
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamSet(CMBS_PARAM_RVREF, st_backup.m_RVREF, CMBS_PARAM_RVREF_LENGTH, 1);
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

        // CMBS_PARAM_GFSK
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamSet(CMBS_PARAM_GFSK, st_backup.m_GFSK, CMBS_PARAM_GFSK_LENGTH, 1);
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }

	//Check if Han Restore is needed:

	// Get DECT parameters
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

   	// Get ULE DECT parameters
   	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);

	//Calculate the needed size
    u32_ULE_dect_size = u32_ULE_dect_end - u32_ULE_dect_start;

	if(u32_ULE_dect_size != 0 )
	{
		return app_SrvEEPROMHanBackupRestore(&st_backup);
	}
    else
    {
        // ASA: writing RFPI back requires system reset, otherwise handsets are not coming back into range.
        app_SrvSystemReboot();
    }

	return CMBS_RC_OK;


}

void appOnDTAMStartSession(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE, u16_SessionID = 0;
    u8 u8_LineId = 0, u8_HSNum=0;
    ST_CMBS_DTAM_START_SESSION_CFM st_Response;

    if ( pv_List == NULL )
    {
		APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_START_SESSION not sent\n");
    	return;
    }

    memset(&st_Response, 0, sizeof(ST_CMBS_DTAM_START_SESSION_CFM));
    
    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
    while (pv_IE != NULL)
    {
	    switch (u16_IE)
	    {
	    case CMBS_IE_LINE_ID:
	    	cmbs_api_ie_LineIdGet(pv_IE, &u8_LineId);
	        break;
	    case CMBS_IE_DTAM_SESSION_ID:
	    	cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionID);
	        break;
	    case CMBS_IE_HS_NUMBER:
	    	cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
	        break;
	            
	    default:
	    	break;
	    }
		cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
	}
        
    st_Response.u8_LineId = u8_LineId;
    st_Response.u8_DiscType = 0; 	//Discriminator type set to '0' means - do NOT use proprietary DTAM commands
    st_Response.u16_Disc = 0; 		//In case Discriminator type set to '1' here shall come the EMC. In case Discriminator type is '0' - this value is 'don't care'
    st_Response.u16_SessionId = u16_SessionID; 	
    st_Response.e_RejectReason = 0xF;
	
    if (cmbs_dsr_DTAMStartSessionCfm(&st_Response, u8_HSNum) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_START_SESSION_CFM not sent\n");
}

void appOnDTAMSelectNeighbourMsg(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    u16 u16_SessionId = 0;
    u8 u8_HSNum = 0;
    
    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DTAM_SESSION_ID)
        {
            cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionId);
        }
        if (u16_IE == CMBS_IE_HS_NUMBER)
        {
            cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
        }
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

	if (cmbs_dsr_DTAMSelectNeighbourMsgCfm(u16_SessionId, u8_HSNum) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_SELECT_NEIGHBOUR_MESSAGE_CFM not sent\n");
}

void appOnDTAMPlayMsg(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    u16 u16_SessionId = 0;
    u8 u8_HSNum = 0;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DTAM_SESSION_ID)
        {
            cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionId);
        }
        if (u16_IE == CMBS_IE_HS_NUMBER)
        {
            cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
        }
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

	if (cmbs_dsr_DTAMPlayMsgCfm(u16_SessionId, u8_HSNum) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_PLAY_MESSAGE_CFM not sent\n");
}

void appOnDTAMDeleteMsg(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    u16 u16_SessionId = 0;
    u8 u8_HSNum = 0;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DTAM_SESSION_ID)
        {
            cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionId);
        }
        if (u16_IE == CMBS_IE_HS_NUMBER)
        {
            cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
        }
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

	if (cmbs_dsr_DTAMDeleteMsgCfm(u16_SessionId,u8_HSNum ) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_DELETE_MESSAGE_CFM not sent\n");
}

void appOnDTAMPauseResume(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    u16 u16_SessionId = 0;
    u8 u8_HSNum = 0;
    
    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DTAM_SESSION_ID)
        {
            cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionId);
        }
        if (u16_IE == CMBS_IE_HS_NUMBER)
        {
            cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
        }
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

	if(cmbs_dsr_DTAMPauseResumeCfm(u16_SessionId, u8_HSNum) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_PAUSE_RESUME_MESSAGE_CFM not sent\n");
}

void appOnDTAMStopMsgPlay(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    u16 u16_SessionId = 0;
    u8 u8_HSNum = 0;

    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DTAM_SESSION_ID)
        {
            cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionId);
        }
        if (u16_IE == CMBS_IE_HS_NUMBER)
        {
            cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
        }
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

	if (cmbs_dsr_DTAMStopMsgPlayCfm(u16_SessionId, u8_HSNum) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_STOP_MESSAGE_PLAY_CFM not sent\n");
}

void appOnDTAMRecordWelcomeMsg(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    u16 u16_SessionId = 0;
    u8 u8_HSNum = 0;
    
    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DTAM_SESSION_ID)
        {
            cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionId);
        }
        if (u16_IE == CMBS_IE_HS_NUMBER)
        {
            cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
        }

        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

	if (cmbs_dsr_DTAMRecordWelcomeMsgCfm(u16_SessionId, u8_HSNum) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE_CFM not sent\n");
}

void appOnDTAMRecordWelcomeMsgStop(void *pv_List)
{
    void *pv_IE;
    u16      u16_IE;
    u16 u16_SessionId = 0;
    u8 u8_HSNum = 0;
    
    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DTAM_SESSION_ID)
        {
            cmbs_api_ie_DTAMSessionIdGet(pv_IE, &u16_SessionId);
        }
        if (u16_IE == CMBS_IE_HS_NUMBER)
        {
            cmbs_api_ie_HsNumberGet(pv_IE, &u8_HSNum);
        }
        cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
    }

	if (cmbs_dsr_DTAMRecordWelcomeMsgStopCfm(u16_SessionId, u8_HSNum) != CMBS_RC_OK)
      APP_SRV_ERROR_PRINT("CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE_STOP_CFM not sent\n");
}
void app_SrvBBDUpdate(void)
{
#ifdef BBD_TEXT_FILE // not fully debugged
    FILE *fp;
    char *line[sizeof(char) * 10];
    int linemax = 2000;
    int i = 0, j = 0;
    char **stringarray;
    char sFilename[20] = "./bbd.dat";
    char *buffer[2000];
    int buffer2[2000];
    int value = 8;
    E_CMBS_RC rc;


    rc = tcx_fileOpen(&fp, sFilename, "r");
    if ( rc != CMBS_RC_OK )
    {
        APP_SRV_ERROR_PRINT("could not open file %s", sFilename);
    }

    fgets(line, linemax, fp);

    APP_SRV_INFO_PRINT("%s\n", line);

    buffer[0] = (strtok(line, ","));
    buffer[1] = (strtok(NULL, ","));
    buffer[2] = (strtok(NULL, ","));
    buffer[3] = (strtok(NULL, ","));
    buffer[4] = (strtok(NULL, ","));
    buffer[5] = (strtok(NULL, ","));

    sscanf(buffer[0], "0x%X", &value);
    sscanf(buffer[1], "0x%X", &value);
    sscanf(buffer[2], "0x%X", &value);

#else // BIN file

    FILE *fp;
    char sFilename[20] = "./bbd.bin";
    char buffer[2000];
    int size = 0;
    E_CMBS_RC rc;

    memset(buffer, 0xAA, MAX_BBD_TRANSACTION_SIZE);

    rc = tcx_fileOpen(&fp, (u8*)sFilename, (const u8 *)"r");
    if ( rc != CMBS_RC_OK )
    {
        APP_SRV_ERROR_PRINT("could not open file %s\n", sFilename);
		return;
    }

    if ( fseek(fp, 0, SEEK_END) != 0 ) // seek to end of file
    {
        APP_SRV_ERROR_PRINT("app_SrvBBDUpdate Error: fseek fail.");
		tcx_fileClose(fp);
        return;
    }
	
    size = ftell(fp); // get current file pointer
    
    if ( fseek(fp, 0, SEEK_SET) != 0 ) // seek back to beginning of file
    {
        APP_SRV_ERROR_PRINT("app_SrvBBDUpdate Error: fseek fail.");
		tcx_fileClose(fp);
        return;
    }

    if ( size > MAX_BBD_TRANSACTION_SIZE )
    {
        APP_SRV_ERROR_PRINT("Size of BBD file is greater than supported. BBD file size = %d, Supported size = %d", size, MAX_BBD_TRANSACTION_SIZE);
		tcx_fileClose(fp);
        return;
    }

	if ( size < 0 )
    {
        APP_SRV_ERROR_PRINT("Could not get file position indicator - ftell error. ");
		tcx_fileClose(fp);
        return;
    }

    rc = tcx_fileRead(fp, (u8*)buffer, 0, (u32)size);

    if ( rc != CMBS_RC_OK )
    {
        APP_SRV_ERROR_PRINT("could not read file %s\n", sFilename);
		tcx_fileClose(fp);
		return;
    }

#endif

    // Now call the SetParam to send the BBD data to Target
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_BBD_UPDATE, (u8 *)buffer, size, 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
	tcx_fileClose(fp);
}

void app_SrvLineTestStart(u8 Channel)
{
//    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.	
    cmbs_dsr_slic_LineTest_Start(g_cmbsappl.pv_CMBSRef, Channel);
//    appcmbs_WaitForEvent(CMBS_EV_DSR_SLIC_LINE_TEST_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void app_SrvLineTestStop(u8 Channel)
{
//    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_slic_LineTest_Stop(g_cmbsappl.pv_CMBSRef, Channel);
//    appcmbs_WaitForEvent(CMBS_EV_DSR_SLIC_LINE_TEST_STOP_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}


void appOnSetLineTestRes(void *pv_List)
{
    UNUSED_PARAMETER(pv_List);
    // TBD: Customer application should handle the proprietary message recieved from the handset

}

void app_SrvNltCapTestStart(u8 Channel)
{
    cmbs_dsr_slic_NltCapTest_Start(g_cmbsappl.pv_CMBSRef, Channel);
}

void app_SrvNltCapTestStop(u8 Channel)
{

    cmbs_dsr_slic_NltCapTest_Stop(g_cmbsappl.pv_CMBSRef, Channel);
}


void appOnSrvNltCapTestRes(void *pv_List)
{
    UNUSED_PARAMETER(pv_List);
    // TBD: Customer application should handle the proprietary message recieved from the handset

}

void app_SrvFxsStatusReq(u8 Channel)
{
	cmbs_dsr_FxsStatus_Request(g_cmbsappl.pv_CMBSRef, Channel);
}

void app_SrvFxsRingTestStart(u8 Channel)
{
	cmbs_dsr_FxsRingTest_Start(g_cmbsappl.pv_CMBSRef, Channel);
}

void app_SrvFxsRingTestStop(u8 Channel)
{

	cmbs_dsr_FxsRingTest_Stop(g_cmbsappl.pv_CMBSRef, Channel);
}

void app_SrvFxsOpenLoopReq(u8 Channel)
{

	cmbs_dsr_FxsOpenLoop_Request(g_cmbsappl.pv_CMBSRef, Channel);
}

void appOnFxsRingTestStartRes(void *pv_List)
{
	void *pv_IE = NULL;
	u16 u16_IE;
	u8 u8_Channel;
	u32 u32_temp;
	
	ST_IE_RESPONSE st_Response;
	
	if ( pv_List )
	{
	  bool b_gotChannel = FALSE;
	  cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
	
	  while (pv_IE != NULL)
	  {
		switch (u16_IE)
		{
			case CMBS_IE_INTEGER_VALUE:
				cmbs_api_ie_IntValueGet(pv_IE, &u32_temp);
				u8_Channel = (u8)u32_temp;		
				b_gotChannel = TRUE;
				break;
			case CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
				break;
		}
	
		cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
	  }
	  
	  if ( b_gotChannel ) 
	  {
		  APP_SRV_INFO_PRINT("APPSRV-INFO: appOnFxsRingTestStartRes: Channel:%d \n", u8_Channel);
	  }
	  else 
	  {
	      APP_SRV_ERROR_PRINT("Could not get channel.\n");
	  }
	
	}

}
void appOnFxsRingTestStopRes(void *pv_List)
{
	void *pv_IE = NULL;
	u16 u16_IE;
	u8 u8_Channel;
	u32 u32_temp;
	
	ST_IE_RESPONSE st_Response;
	
	if ( pv_List )
	{
      bool b_gotChannel = FALSE;
	  cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);

	  while (pv_IE != NULL)
	  {
		switch (u16_IE)
		{
			case CMBS_IE_INTEGER_VALUE:
				cmbs_api_ie_IntValueGet(pv_IE, &u32_temp);
				u8_Channel = (u8)u32_temp;	
				b_gotChannel = FALSE;
				break;
			case CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
				break;
		}

		cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);
 	  }

	  if ( b_gotChannel ) 
	  {
		  APP_SRV_INFO_PRINT("APPSRV-INFO: appOnFxsRingTestStopRes: Channel:%d \n", u8_Channel);
	  }
	  else 
	  {
	      APP_SRV_ERROR_PRINT("Could not get channel.\n");
	  }

	}
}

E_CMBS_RC app_FXSEvent(const ST_IE_DATA* pst_Data)
{
    PST_CFR_IE_LIST p_List;
    ALLOCATE_IE_LIST(p_List);
    cmbs_api_ie_DataAdd(p_List, (ST_IE_DATA *)pst_Data);
    return cmbs_int_EventSend(CMBS_EV_DEE_FXS_EVENT, p_List->pu8_Buffer, p_List->u16_CurSize);
}

void app_OnSrvFxsStatus(void *pv_List)
{
	void *pv_IE = NULL;
	u16 u16_IE;
	u8 u8_Channel;
	u32 u32_temp;
	u8 u8_status;
	
	if ( pv_List )
	{
		// collect information elements.
		// we expect: HS num
		cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
		cmbs_api_ie_IntValueGet(pv_IE, &u32_temp);
		u8_Channel = (u8)u32_temp;

		cmbs_api_ie_GetNext(pv_List, &pv_IE, &u16_IE);

		cmbs_api_ie_ByteValueGet(pv_IE, &u8_status, CMBS_IE_FXS_HOOK_STATUS);

		APP_SRV_INFO_PRINT("APPSRV-INFO: Channel:%d in status %d\n", u8_Channel, u8_status);

	}
}

void appsrv_SetCallStateFilter(u8 u8_FilterType)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_CallStateFilterSet(g_cmbsappl.pv_CMBSRef, u8_FilterType);
    appcmbs_WaitForEvent(CMBS_EV_DSR_CALL_STATE_SET_FILTER_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//This is utility function for copying big buffers to the EEPROM in the target.
//This function is doing the fragmentation for packets of packet_max_size.
//This function is blocking and waiting for the packet acknoledge.

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void app_ParamAreaSetBySegments(u32 u32_Pos, u32 u32_Length, u8 *pu8_Data, u16 packet_max_size)
{

    u32 i, u32_number_of_packet = 0;
    u32 u32_offset, u32_remainder = 0;

    //check validity of packet_max_size
    if (packet_max_size == 0)
    	{
		APP_SRV_ERROR_PRINT("cannot write to EEPROM by segments when packet_max_size = 0");
		return;
    	}
	
    if ( u32_Length < packet_max_size )
    {
        APP_SRV_INFO_PRINT("Setting Data ...");
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamAreaSet(CMBS_PARAM_AREA_TYPE_EEPROM,  u32_Pos,  u32_Length, pu8_Data, 1);
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_AREA_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }
    else
    {
        u32_number_of_packet = (u32_Length / packet_max_size);
        u32_remainder = (u32_Length % packet_max_size);
        u32_offset = 0;
        for (i = 0; i < u32_number_of_packet; i++)
        {
            APP_SRV_INFO_PRINT("Setting Data Packet %d... \n", i + 1);
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamAreaSet(CMBS_PARAM_AREA_TYPE_EEPROM,  u32_Pos + u32_offset,  packet_max_size, pu8_Data + u32_offset, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_AREA_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            u32_offset += packet_max_size;
        }
        if ( u32_remainder )
        {
            APP_SRV_INFO_PRINT("Setting Data Packet %d... \n", i + 1);
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamAreaSet(CMBS_PARAM_AREA_TYPE_EEPROM,  u32_Pos + u32_offset,  u32_remainder, pu8_Data + u32_offset, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_AREA_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            u32_offset += u32_remainder;
        }
    }

    APP_SRV_INFO_PRINT("Done!");


}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//This is utility function for getting big buffers from the EEPROM in the target.
//This function is doing the fragmentation for packets of packet_max_size.
//This function is blocking and waiting for the packet.

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void app_ParamAreaGetBySegments(u32 u32_Pos, u32 u32_Length, u8 *pu8_Data, u32 packet_max_size)
{
    u32 i, u32_number_of_packet = 0;

    ST_APPCMBS_CONTAINER st_Container;
    u32 u32_offset, u32_remainder = 0;

    //check validity of packet_max_size
    if (packet_max_size == 0)
    	{
		APP_SRV_ERROR_PRINT("cannot read EEPROM by segments when packet_max_size = 0");
		return;
    	}
	
    if ( u32_Length < packet_max_size )
    {
        APP_SRV_INFO_PRINT("Getting Data ...");
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM,  u32_Pos,  u32_Length, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_AREA_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(pu8_Data, st_Container.ch_Info,  u32_Length);
    }
    else
    {
        u32_number_of_packet = (u32_Length / packet_max_size);
        u32_remainder = (u32_Length % packet_max_size);
        u32_offset = 0;

        for (i = 0; i < u32_number_of_packet; i++)
        {
			APP_SRV_INFO_PRINT("Getting Data Packet %d... \n", i + 1);
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM,  u32_Pos + u32_offset,  packet_max_size, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_AREA_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(pu8_Data + u32_offset, st_Container.ch_Info,  packet_max_size);
            u32_offset += packet_max_size;
        }
        if ( u32_remainder )
        {
            APP_SRV_INFO_PRINT("Getting Data Packet %d... \n", i + 1);
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM,  u32_Pos + u32_offset,  u32_remainder, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_AREA_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(pu8_Data + u32_offset, st_Container.ch_Info,  u32_remainder);
            u32_offset += u32_remainder;
        }
    }

    APP_SRV_INFO_PRINT("Done!");


}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//    Writing a binary file content into a buffer
//    Parameters: a buffer to write into, a file path
//    Assumption: file size <= buffer size
//    Return value: 1 on success, 0 on faliure

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
E_CMBS_RC app_ParamAreaGetFromFile(u8 *pu8_Data, u8 *pu8_path, u32 *pu32_fileSize)
{

    FILE *fp_binFile = 0;
    u32 u32_size = 0;
    E_CMBS_RC e_status;

    // Open binary file for read
    e_status = tcx_fileOpen(&fp_binFile, pu8_path, (u8 *)"r+b");

    if ( e_status != CMBS_RC_OK )
    {
        APP_SRV_ERROR_PRINT("Failed to open file - %s\n", pu8_path);
        return CMBS_RC_ERROR_GENERAL;
    }

    // get the size of the file in bytes
    u32_size = tcx_fileSize(fp_binFile);

    if ( (s32)u32_size < 0 )
    {
        APP_SRV_ERROR_PRINT("Failed to get file size - %s\n", pu8_path);
        tcx_fileClose(fp_binFile);
        return CMBS_RC_ERROR_GENERAL;
    }

    // Read u32_size bytes from binFile
    e_status = tcx_fileRead(fp_binFile, pu8_Data, 0, u32_size);

    if ( e_status != CMBS_RC_OK )
    {
        APP_SRV_ERROR_PRINT("Failed to read from file into EEprom buffer\n");
        tcx_fileClose(fp_binFile);
        return CMBS_RC_ERROR_GENERAL;
    }

	if(pu32_fileSize != NULL)
	{
		*pu32_fileSize = u32_size;
	}

    // Close file
    tcx_fileClose(fp_binFile);

    return CMBS_RC_OK;
}

void app_SrvJEDECidGet()
{
    cmbs_dsr_JEDECiDGet();
}

void app_SrvPing()
{
    cmbs_dsr_Ping(NULL);
}

E_CMBS_RC app_SrvSendTDMSlotConfiguration(u32 u32_TDMSlots)
{
    return cmbs_dem_ConfigTDMSlots(g_cmbsappl.pv_CMBSRef, u32_TDMSlots);
}

E_CMBS_RC app_SrvSendRAMDumpStoreCmd(void)
{
    return cmbs_int_SendRAMDumpStoreCmd();
}


/* ** [End of file] ** ************************************************************************************************************** */






