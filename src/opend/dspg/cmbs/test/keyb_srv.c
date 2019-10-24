/*!
*	\file		tcx_keyb.c
*	\brief		test command line generator for manual testing
*	\Author		kelbch
*
*	@(#)	%filespec: keyb_srv.c~48.9.1.7 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
* date        name      action                                \n
* ----------------------------------------------------------------------------\
* 29-May-2015 tcmc_asa  preset st_PropSettings in keyb_SetNewHandsetName()
* 23-Apr-2015 tcmc_asa  Defect 14073:added Proprietary DECT settings
* 15-Apr-2015 tcmc_asa  Defect 14073:added parameter in keyb_SetNewHandsetName
* 19-Mar-2015 tcmc_asa  added parameter to app_SrvSubscriptionOpen
* 21-Nov-2014 tcmc_asa  Handset locator feature enhancements
* 17-Oct-14 tcmc_asa  ---GIT-- added keypb_HandsetDeleteINL()
* 13-Jan-2014 tcmc_asa  -- GIT--  take checksum changes from 3.46.x to 3_x main (3.5x)
* 20-Dec-2013 tcmc_asa                 added CHECKSUM_SUPPORT
*
*   16-apr-09		kelbch		1.0		 Initialize \n
*   14-dec-09		sergiym		 ?		 Add start/stop log menu \n
*   21-Oct-2011 tcmc_asa   48.1.11 Added Fixed carrier selection
*   26-Oct-2011 tcmc_asa   48.1.12 merged NBG 48.1.11 with BLR 48.1.12
*   17-Jan-2013 tcmc_asa   48.1.16.1.6.1.3  PR3613/3614 Internal call/name blocking
* 12-Jun-2013 tcmc_asa - GIT - add 2.99.x changes to 3.3x
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cmbs_platf.h"
#include "cmbs_api.h"
#include "cfr_ie.h"
#include "cfr_mssg.h"
#include "cmbs_util.h"
#include "appcmbs.h"
#include "appsrv.h"
#include "appcall.h"
#include "appfacility.h"
#include "tcx_log.h"
#include "tcx_hostlog.h"
#include "tcx_keyb.h"
#include "ListsApp.h"
#include "appmsgparser.h"
#include "applog.h"
#include "tcx_eep.h"
#include "cmbs_int.h"
#include "cmbs_dbg.h"
#include "LASessionMgr.h"
#include "ListChangeNotif.h"
#include <tcx_util.h>
#ifdef WIN32
#define kbhit _kbhit
#else
extern int kbhit(void);
#endif

// size of segment for CMBS_PARAM_AREA_SET or CMBS_PARAM_AREA_GET
#define CMBS_SEGMENT_SIZE			240

#define CMBS_IWU_DATA_MAX_TRANSMIT_LENGTH			 59
#define CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH		 51 //CMBS_IWU_DATA_MAX_TRANSMIT_LENGTH - size of header
#define CMBS_IWU_DATA_MAX_NEXT_PAYLOAD_LENGTH		 53 //header is smaller for next messages because total length is not needed


static void keyb_DiableEPFast(u8 HSID);
void keyb_SYPOSettingsSet(void);
void keyb_SYPOSettingsGet(void);
const char* keyb_dbg_GetListTypeName(u16 u16_ListType);
void  keypb_FxsToneConfig(bool IsSimpleTone);

extern u8 g_u8_LogOutput;

#define MAX_PATH_SIZE 100
#define MAX_OVERNIGHT_TESTS	1000
extern st_AFEConfiguration AFEconfig;
extern u32	g_u32_UsedSlots;
extern ST_CMBS_API_INST	g_CMBSInstance;
extern u8	g_EepromOnHost;
extern u32  g_CMBSMaxTransferSize;
extern void keyb_IncCallNB(void);
extern u32 LineDiagnosticGetEntryIdByLineId(int lineID);
extern void ListChangeNotif_SMSListChanged(u32 u32_SMSId, u8 ListType, u16 u16_HSMask);
void keypb_SetEEPROMFromFile(void);

typedef enum {
    OK,
    NO_INPUT,
    TOO_LONG
} E_CMBS_RetVal;

const stDandAContactListEntry g_pst_DandAContactsList[] = 
{
    /* Entry Id	 |  Last Name        |   First Name	     |  Number1  		      Type              Default   Own  |  Number2  		      Type              Default    Own  |	Number3  		      Type              Default    Own  | Melody |  Line_ID 	 */
    /*  -----	     ------------	     -----------      ----------------------------------------------------------  -----------------------------------------------------	-----------------------------------------------------------	  ------     ------      */
    {   0,              "ALEXANDER",       "Christian",     "00441324778824" ,    NUM_TYPE_FIXED,     TRUE,   0,      "00449876543210",   NUM_TYPE_WORK,     FALSE,     0,        0,                 NUM_TYPE_FIXED,   FALSE,      0,         1,          2     },    /* 1   */
    {   0,              "ALEXANDER",       "Christina",     "00441324778812" ,    NUM_TYPE_FIXED,     TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         2,          2     },    /* 2   */
    {   0,              "ALOUSSI",         "RAMIN",         "0156891247" ,        NUM_TYPE_FIXED,     TRUE,   0,      "0675000209",       NUM_TYPE_MOBILE,   FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         3,          0     },    /* 3   */
    {   0,              "BORDONADO",       "Karlita",       "0581321185" ,        NUM_TYPE_WORK,      TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         4,          0     },    /* 4   */
    {   0,              "DEL PIETRO",      "David",         "00550123456789" ,    NUM_TYPE_WORK,      TRUE,   0,      "0675000209",       NUM_TYPE_MOBILE,   FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         5,          0     },    /* 5   */
    {   0,              "FENJIRO",         "Carlos",        "0490413002" ,        NUM_TYPE_WORK,      TRUE,   0,      "0612345678",       NUM_TYPE_MOBILE,   FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         6,          2     },    /* 6   */
    {   0,              "LAGDEC",          "Jerome",        "02298951214" ,       NUM_TYPE_WORK,      TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         7,          0     },    /* 7   */
    {   0,              "UWE",             "MARCUS",        "008989945270" ,      NUM_TYPE_WORK,      TRUE,   0,      "00491603794505",   NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         1,          0     },    /* 8   */
    {   0,              "WALKER",          "BARCLAY",       "06123123" ,          NUM_TYPE_MOBILE,    TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         2,          2     },    /* 9   */
    {   0,              "WOJCIECHOSKI",    NULL,            "0675000321" ,        NUM_TYPE_MOBILE,    TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         3,          1     },    /* 10   */

    {   0,              "SURANY",          "Horvath",       "08093316433" ,       NUM_TYPE_WORK,      TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         4,          1     },    /* 11   */
    {   0,              "WEATHERBURN",     "Ashton",        "5461106" ,           NUM_TYPE_FIXED,     TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         3,          0     },    /* 12   */
    {   0,              "ALEXEYEVA",       "Vanessa",       "2446544" ,           NUM_TYPE_MOBILE,    TRUE,   0,      "157615665",        NUM_TYPE_WORK,     FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         8,          2     },    /* 13   */
    {   0,              "MAJEWSKI",        "Szczesny",      "5506308" ,           NUM_TYPE_WORK,      TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         6,          0     },    /* 14   */
    {   0,              "VAN ZUIDAM",      "Melchior",      "6786088" ,           NUM_TYPE_MOBILE,    TRUE,   0,      "48716156119",      NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         5,          2     },    /* 15   */
    {   0,              "YAGAWA",          "Yumi",          "6129828" ,           NUM_TYPE_MOBILE,    TRUE,   0,      "14179664",         NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         7,          3     },    /* 16   */
    {   0,              "LUKIC",           "Frano",         "4378400" ,           NUM_TYPE_WORK,      TRUE,   0,      "7960069",          NUM_TYPE_MOBILE,   FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         4,          1     },    /* 17   */
    {   0,              "LAISNE",          "-",             "5282472" ,           NUM_TYPE_FIXED,     TRUE,   0,      "351671601",        NUM_TYPE_WORK,     FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         3,          6     },    /* 18   */
    {   0,              "HAUKAS",          "Ingfryd",       "040916155",          NUM_TYPE_FIXED,     TRUE,   0,      "6667465",          NUM_TYPE_WORK,     FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         1,          2     },    /* 19   */
    {   0,              "GUAJARDO",        "Joscio Verdugo","5119225" ,           NUM_TYPE_WORK,      TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         1,          5     },    /* 20   */
    {   0,              "NNAMUTAEZINWA",   "Fumnanya",      "051108463" ,         NUM_TYPE_FIXED,     TRUE,   0,      "15117161781",      NUM_TYPE_WORK,     FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         8,          7     },    /* 21   */
    {   0,              "BARISIC",         "Dinka",         "3342267" ,           NUM_TYPE_WORK,      TRUE,   0,      "415879",           NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         2,          5     },    /* 22   */
    {   0,              NULL,                "Kunihide",    "6269134" ,           NUM_TYPE_MOBILE,    TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         5,          4     },    /* 23   */
    {   0,              "KALLIOMAKI",      "Jasmin",        "6876433" ,           NUM_TYPE_WORK,      TRUE,   0,      "61871617",         NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         4,          1     },    /* 24   */
    {   0,              "BJORKLUND",       "Alexsandra",    "6859168" ,           NUM_TYPE_MOBILE,    TRUE,   0,      NULL,               NUM_TYPE_FIXED,    FALSE,     0,        NULL,              NUM_TYPE_FIXED,   FALSE,      0,         1,          0     },    /* 25   */
	
};
#define CONTACT_LIST_SIZE	LENGTHOF(g_pst_DandAContactsList)


/*****************************************************************************
*
*        Service Keyboard loop
*
******************************************************************************/
//		========== keyb_EEPROMReset  ===========
/*!
\brief				 Send CMBS_PARAM_RESET_ALL and wait for reconnection

\param[in,out]		 <none>

\return				<none>

*/

extern void p_HanServer_DisablePingToTarget( bool b_value);


extern bool b_EepromOnHost;
void keyb_EEPROMReset(void)
{
    u8 u8_Dummy = 0;
	
    

    if ( b_EepromOnHost )
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
		app_SrvParamSet(CMBS_PARAM_RESET_ALL, &u8_Dummy, sizeof(u8_Dummy), 1);
        // Wait till target finishes writing in eeprom file the flag for factory reset
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }
	else
	{
		app_SrvParamSet(CMBS_PARAM_RESET_ALL, &u8_Dummy, sizeof(u8_Dummy), 1);
	}

	
	//Disable Pings to target 
	p_HanServer_DisablePingToTarget(TRUE);

	printf ("pings disabled\n");

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.

#ifdef WIN32
    if ( appcmbs_get_CMBSAppInstance()->u8_HwComType == CMBS_HW_COM_TYPE_USB )
    {
        // for USB we must close the CDC device to allow the USB dongle to perform a new USB enumeration
        printf("\nReconnecting to target, please wait %d seconds \n", APPCMBS_RECONNECT_TIMEOUT / 1000);
        if ( appcmbs_ReconnectApplication(APPCMBS_RECONNECT_TIMEOUT) != CMBS_RC_OK )
        {
            printf("\n *****************************************************");
            printf("\n Can't reconnect to target!!!!!");
            printf("\n *****************************************************");
        }
    }
#endif

    appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

	//Reenable pings 
	p_HanServer_DisablePingToTarget(FALSE);

	printf ("pings enabled\n");
}

//		========== keyb_SSTFlashResetPrepare  ===========
/*!
\brief				 Send CMBS_PARAM_PREP_QSPI_FOR_HW_RESET and wait for response

\param[in,out]		 <none>

\return				<none>

*/

void keyb_PrepareSSTFlashForReset(void)
{
    u8 u8_Dummy = 0;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_PREP_QSPI_FOR_HW_RESET, &u8_Dummy, sizeof(u8_Dummy), 1);

    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // The host can now reset the target by RSTN PIN,
    
}

void keyb_EEPROMBackupCreate(void)
{
    app_SrvEEPROMBackupCreate();

}

void     keyb_ParamFlexGet(void)
{
    char  buffer[12];
    u32   u32_Location;

    printf("Enter Location        (dec): ");
    tcx_gets(buffer, sizeof(buffer));
    u32_Location = atoi(buffer);

    printf("Enter Length (dec. max %d): ", g_CMBSMaxTransferSize);
    tcx_gets(buffer, sizeof(buffer));

    app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM, u32_Location, (u16)atoi(buffer), 1);

}

void     keyb_RAMParamGet(void)
{
    char  buffer[12];
    u32   u32_Location;
	u8    u8_Base = 16;

	printf("\nPress d to change address base to decimal (default: hex) ");
	if ( tcx_getch() == 'd' ) u8_Base = 10;

    printf("\nEnter Location (base %d): ", u8_Base);
    tcx_gets(buffer, sizeof(buffer));
    u32_Location = strtol(buffer, NULL, u8_Base);

    printf("\nEnter Length (dec. max %d): ", g_CMBSMaxTransferSize);
    tcx_gets(buffer, sizeof(buffer));

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_RAM, u32_Location, (u16)atoi(buffer), 1);

	appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_AREA_GET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    printf("Press Any Key!\n");
    tcx_getch();

}


//		========== keyb_RFPISet ===========
/*!
\brief				 set the RFPI parameter
\param[in,out]		 <none>
\return				<none>
*/
void     keyb_RFPISet(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    char	buffer[12];
    u8		u8_RFPI[CMBS_PARAM_RFPI_LENGTH];
    int		i, j = 0;
    u8		u8_Value[CMBS_PARAM_RFPI_LENGTH] = { 0 };


    // save parser state as we want to disable it temporary
    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    // get current RFPI
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RFPI, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u8_Value[0], st_Container.ch_Info,  CMBS_PARAM_RFPI_LENGTH);
    printf("Current RFPI: \t %.2X%.2X%.2X%.2X%.2X\n", u8_Value[0], u8_Value[1], u8_Value[2], u8_Value[3], u8_Value[4]);

    // ask for a new RFPI
    printf("Enter New RFPI: ");
    tcx_gets(buffer, sizeof(buffer));

    for (i = 0; i < 10; i += 2)
    {
        u8_RFPI[j] = app_ASC2HEX(buffer + i);
        j++;
    }

    // save new RFPI
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RFPI, u8_RFPI, sizeof(u8_RFPI), 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvSystemReboot();

	appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // restore parser state
    app_set_msgparserEnabled(msgparserEnabled);
}

//		========== _EcoMode2Str ===========
/*!
\brief				 converts E_CMBS_ECO_MODE_TYPE value into string
\param[in,out]		 u8  u8_Value
\return				 char*
*/
char* _EcoMode2Str(u8  u8_Value)
{
    switch (u8_Value)
    {
    case CMBS_ECO_MODE_TYPE_LOW_POWER:
        return "ECO mode enabled low power (CMBS_ECO_MODE_TYPE_LOW_POWER)";
    case CMBS_ECO_MODE_TYPE_LOWEST_POWER:
        return "ECO mode enabled lowest power (CMBS_ECO_MODE_TYPE_LOWEST_POWER)";
    case CMBS_ECO_MODE_TYPE_NONE:
        return "ECO mode disabled (CMBS_ECO_MODE_TYPE_NONE)";
    }

    return "UNKNOWN";
}

//		========== keyb_ECOModeSet ===========
/*!
\brief				 set the ECO_MODE parameter
\param[in,out]		 <none>
\return				<none>
*/
void     keyb_ECOModeSet(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    u8                   u8_Value = CMBS_ECO_MODE_TYPE_NONE;
    u32					 u32_Value;

    // get current ECO MODE
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_ProductionParamGet(CMBS_PARAM_ECO_MODE, CMBS_PARAM_ECO_MODE_LENGTH);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    SleepMs(100);
    printf("Current ECO Mode is: \t 0x%X  %s", st_Container.ch_Info[0], _EcoMode2Str(st_Container.ch_Info[0]));

    printf("\nEnter: \n%d-%s \n%d-%s \n%d-%s \n",
           CMBS_ECO_MODE_TYPE_NONE,		_EcoMode2Str(CMBS_ECO_MODE_TYPE_NONE),
           CMBS_ECO_MODE_TYPE_LOW_POWER,	_EcoMode2Str(CMBS_ECO_MODE_TYPE_LOW_POWER),
           CMBS_ECO_MODE_TYPE_LOWEST_POWER, _EcoMode2Str(CMBS_ECO_MODE_TYPE_LOWEST_POWER));

    tcx_scanf("%d", &u32_Value);
    u8_Value = (u8)u32_Value;

    if ( u8_Value < CMBS_ECO_MODE_TYPE_MAX )
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamSet(CMBS_PARAM_ECO_MODE, &u8_Value, sizeof(u8), 1);
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }

    // get current ECO MODE
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_ProductionParamGet(CMBS_PARAM_ECO_MODE, CMBS_PARAM_ECO_MODE_LENGTH);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    SleepMs(100);
    printf("Current ECO Mode is: \t 0x%X  %s", st_Container.ch_Info[0], _EcoMode2Str(st_Container.ch_Info[0]));

    printf("\nPress Any Key!\n");
    tcx_getch();

}

//		========== _DectType2Str ===========
/*!
\brief				 converts E_CMBS_DECT_TYPE value into string
\param[in,out]		 u8  u8_Value
\return				 char*
*/
char* _DectType2Str(u8  u8_Value)
{
    switch (u8_Value)
    {
    case CMBS_DECT_TYPE_EU:
        return "EU DECT (CMBS_DECT_TYPE_EU)";
    case CMBS_DECT_TYPE_US:
        return "US DECT (CMBS_DECT_TYPE_US)";
    case CMBS_DECT_TYPE_US_FCC:
        return "US FCC DECT (CMBS_DECT_TYPE_US_FCC)";
    case CMBS_DECT_TYPE_JAPAN:
        return "JAPAN DECT (CMBS_DECT_TYPE_JAPAN)";
    case CMBS_DECT_TYPE_JAPAN_FCC:
        return "JAPAN FCC DECT (CMBS_DECT_TYPE_JAPAN_FCC)";
    case CMBS_DECT_TYPE_KOREAN:
        return "KOREAN DECT (CMBS_DECT_TYPE_KOREAN)";	
    }

    return "UNKNOWN";
}

//		========== keyb_DectTypeSet ===========
/*!
\brief				 set the DECT TYPE parameter
\param[in,out]		 <none>
\return				<none>
*/
void     keyb_DectTypeSet(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    u8  u8_Value;
    u32 u32_Value;

    // get current DECT Type
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_ProductionParamGet(CMBS_PARAM_DECT_TYPE, CMBS_PARAM_DECT_TYPE_LENGTH);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    SleepMs(100);
    printf("Current DECT type is: 0x%X %s ", st_Container.ch_Info[0], _DectType2Str(st_Container.ch_Info[0]));

    printf("\n\nEnter DECT type: \n%d-%s \n%d-%s \n%d-%s \n%d-%s \n%d-%s \n%d-%s \n ",
           CMBS_DECT_TYPE_EU,			_DectType2Str(CMBS_DECT_TYPE_EU),
           CMBS_DECT_TYPE_US,			_DectType2Str(CMBS_DECT_TYPE_US),
           CMBS_DECT_TYPE_US_FCC,		_DectType2Str(CMBS_DECT_TYPE_US_FCC),
           CMBS_DECT_TYPE_JAPAN,		_DectType2Str(CMBS_DECT_TYPE_JAPAN),
           CMBS_DECT_TYPE_JAPAN_FCC,	_DectType2Str(CMBS_DECT_TYPE_JAPAN_FCC),
           CMBS_DECT_TYPE_KOREAN,		_DectType2Str(CMBS_DECT_TYPE_KOREAN));

    tcx_scanf("%X", &u32_Value);
    u8_Value = (u8)u32_Value;

    if ( u8_Value < CMBS_DECT_TYPE_MAX )
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamSet(CMBS_PARAM_DECT_TYPE, &u8_Value, sizeof(u8_Value), 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_SET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }
    if ( st_Container.n_Info == TRUE )
    {
        // reboot module only in case the set was successful
        printf("Rebooting module...Please wait...\n");
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvSystemReboot();

		appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

        // get current DECT Type
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamGet(CMBS_PARAM_DECT_TYPE, CMBS_PARAM_DECT_TYPE_LENGTH);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        SleepMs(100);
        printf("Current DECT type is: 0x%X %s ", st_Container.ch_Info[0], _DectType2Str(st_Container.ch_Info[0]));
    }
    else
    {
        printf("\n ERROR, could not set DECT type !!!");
        if ( (u8_Value == CMBS_DECT_TYPE_JAPAN) || (u8_Value == CMBS_DECT_TYPE_JAPAN_FCC) )
        {
            printf(" In case you wish to switch to Japan-DECT, first make sure to disable NEMo!\n");
        }
    }
    printf("Press Any Key!\n");
    tcx_getch();
}

//		========== keyb_ChipSettingsSet ===========
/*!
\brief				 set the tuning parameter of the chipset

\param[in,out]		 <none>

\return				<none>

*/
void     keyb_ChipSettingsSet(void)
{
    char  buffer[21];
    u8    u8_Value;
    ST_APPCMBS_CONTAINER st_Container;

    u8 u8_RFPI[CMBS_PARAM_RFPI_LENGTH] = { 0 };
    bool isVegaOne = FALSE;
    u8 u8_RVBG = 0;
    u8 u8_RVREF = 0;
    u8 u8_RXTUN = 0;
    u8 u8_TestMode = 0;
    u32 u32_BG = 0;
    u32 u32_ADC = 0;
    u32 u32_PMU = 0;


    if ( appcmbs_get_CMBSAppInstance()->u8_HwChip ==  CMBS_HW_CHIP_VEGAONE )
        isVegaOne = TRUE;

    if ( !isVegaOne )
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_AUXBGPROG, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&u32_BG, st_Container.ch_Info,  CMBS_PARAM_AUXBGPROG_LENGTH);

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_ADC_MEASUREMENT, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&u32_ADC, st_Container.ch_Info,  CMBS_PARAM_ADC_MEASUREMENT_LENGTH);

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_PMU_MEASUREMENT, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(&u32_PMU, st_Container.ch_Info,  CMBS_PARAM_PMU_MEASUREMENT_LENGTH);
    }
    else
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RVBG, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_RVBG = st_Container.ch_Info[0];

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RVREF, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_RVREF = st_Container.ch_Info[0];
    }

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RXTUN, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_RXTUN = st_Container.ch_Info[0];

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_TEST_MODE, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_TestMode = st_Container.ch_Info[0];

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RFPI, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(u8_RFPI, st_Container.ch_Info,  CMBS_PARAM_RFPI_LENGTH);

    tcx_appClearScreen();
    printf("-- Current chipset values: \n");
    if ( !isVegaOne )
    {
        printf("DCIN2 ADC VALUE: \t%d mV\n", u32_ADC);
        printf("PMU VALUE:       \t%d mV\n", u32_PMU);
        printf("BG_Calibrate:    \t0x%X\n", u32_BG);
    }
    else
    {
        printf("RVBG:     \t%X\n", u8_RVBG);
        printf("RVREF:    \t%X\n", u8_RVREF);
    }
    printf("RXTUN:    \t%X\n", u8_RXTUN);
    printf("RFPI:     \t%.2X%.2X%.2X%.2X%.2X\n", u8_RFPI[0], u8_RFPI[1], u8_RFPI[2], u8_RFPI[3], u8_RFPI[4]);
    printf("TestMode: \t");
    switch (u8_TestMode)
    {
    case CMBS_TEST_MODE_NORMAL:
        printf("None\n"); break;
    case CMBS_TEST_MODE_TBR6:
        printf("TBR6\n"); break;
    case CMBS_TEST_MODE_TBR10:
        printf("TBR10\n"); break;
    }

    printf("------------------------\n");

    if ( isVegaOne )
    {
        printf("Enter RVBG: ");
        tcx_gets(buffer, sizeof(buffer));
        if ( strlen(buffer) )
        {
            u8_Value = app_ASC2HEX(buffer);
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_RVBG, &u8_Value, sizeof(u8_Value), 1);

            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }

        printf("\nEnter RVREF: ");
        tcx_gets(buffer, sizeof(buffer));
        if ( strlen(buffer) )
        {
            u8_Value = app_ASC2HEX(buffer);
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_RVREF, &u8_Value, sizeof(u8_Value), 1);

            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }
    }

    printf("\nEnter RXTUN: ");
    tcx_gets(buffer, sizeof(buffer));
    {
        u8_Value = app_ASC2HEX(buffer);
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamSet(CMBS_PARAM_RXTUN, &u8_Value, sizeof(u8_Value), 1);

        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }

    if ( isVegaOne )
    {
        printf("\nEnter GFSK (10 bytes): ");
        tcx_gets(buffer, sizeof(buffer));
        {
            if ( strlen(buffer) == 20 )
            {
                u8 i;
                for (i = 0; i < 10; ++i)
                {
                    buffer[i] = app_ASC2HEX(&buffer[i * 2]);
                }

				appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
                app_SrvParamSet(CMBS_PARAM_GFSK, (u8 *)buffer, 10, 1);

                appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            }
        }
    }
}
//		========== keyb_ChipSettingsGet ===========
/*!
\brief				 Shows the tuning parameter of the CMBS module

\param[in,out]		 <none>

\return				<none>

*/

void     keyb_ChipSettingsGet(void)
{
    u8 u8_Value[13], i;
    ST_APPCMBS_CONTAINER st_Container;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RVBG, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_Value[0] = st_Container.ch_Info[0];

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RVREF, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_Value[1] = st_Container.ch_Info[0];

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RXTUN, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_Value[2] = st_Container.ch_Info[0];

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_GFSK, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u8_Value[3], st_Container.ch_Info,  10);

    tcx_appClearScreen();


    printf("Chipset settings\n");
    printf("RVBG: %02x\n", u8_Value[0]);
    printf("RVREF: %02x\n", u8_Value[1]);
    printf("RXTUN: %02x\n", u8_Value[2]);
    printf("GFSK: ");
    for (i = 0; i < 10; ++i)
    {
        printf("%X ", u8_Value[3 + i]);
    }

}
//		========== keypb_EEPromParamGet ===========
/*!
\brief         Handle EEProm Settings get
\param[in,ou]  <none>
\return

*/

void keypb_param_test(void)
{
    int i;
    for (i = 0; i < 30; i++)
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RFPI, 1);
        // wait for CMBS target message
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_GET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

        /*        memcpy( &st_Resp, &st_Container.ch_Info, st_Container.n_InfoLen );
        if(st_Resp.e_Response != CMBS_RESPONSE_OK)
        {
        printf("******** ERRROR CMBS_PARAM_RFPI!!!  on %d try ********\n",i);
        }
        */
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_AUTH_PIN, 1);
        // wait for CMBS target message
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_GET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        /*        memcpy( &st_Resp, &st_Container.ch_Info, st_Container.n_InfoLen );
        if(st_Resp.e_Response != CMBS_RESPONSE_OK)
        {
        printf("******** ERRROR CMBS_PARAM_AUTH_PIN!!!  on %d try ********\n",i);
        }
        */
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvFWVersionGet(CMBS_MODULE_CMBS, 1);
        // wait for CMBS target message
        appcmbs_WaitForEvent(CMBS_EV_DSR_FW_VERSION_GET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        /*        memcpy( &st_Resp, &st_Container.ch_Info, st_Container.n_InfoLen );
        if(st_Resp.e_Response != CMBS_RESPONSE_OK)
        {
        printf("******** ERRROR CMBS_EV_DSR_FW_VERSION_GET_RES!!!  on %d try ********\n",i);
        }
        */
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvLineSettingsGet(0xFF, 1);
        // wait for CMBS target message
        appcmbs_WaitForEvent(CMBS_EV_DSR_LINE_SETTINGS_LIST_GET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        /*        memcpy( &st_Resp, &st_Container.ch_Info, st_Container.n_InfoLen );
        if(st_Resp.e_Response != CMBS_RESPONSE_OK)
        {
        printf("******** ERRROR CMBS_EV_DSR_LINE_SETTINGS_LIST_GET_RES!!!  on %d try ********\n",i);
        }
        */
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvRegisteredHandsets(0xFFFF, 1);
        // wait for CMBS target message
        appcmbs_WaitForEvent(CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        /*        memcpy( &st_Resp, &st_Container.ch_Info, st_Container.n_InfoLen );
        if(st_Resp.e_Response != CMBS_RESPONSE_OK)
        {
        printf("******** ERRROR CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET_RES!!!  on %d try ********\n",i);
        }
        */
    }
}

void keyb_GetEntireEEPROM(void)
{
    ST_APPCMBS_CONTAINER	st_Container;
    char					file_name[FILENAME_MAX];
    u32						u32_EepromSize;
    u32						u32_Index = 0;
    FILE					*pFile	= NULL;
    static const u8         u8_PacketSize = 128;

    printf("\nThis menu dumps entire eeprom to given file");
    printf("\nEnter file name: ");
    tcx_gets(file_name, sizeof(file_name));

    // open output file
    pFile = fopen(file_name, "wb");

    if ( pFile == NULL )
    {
        printf("\n Can't open file %s ", file_name);
        return;
    }

    //get eeprom size
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvGetEepromExtSize();
    appcmbs_WaitForContainer(CMBS_EV_DSR_EEPROM_EXTENDED_SIZE_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    //extract eeprom size
    memcpy(&u32_EepromSize, st_Container.ch_Info, sizeof(u32_EepromSize));
    printf("\n Eeprom size = %d ", u32_EepromSize);

    //read eeprom by 128 bytes
    while (u32_Index < u32_EepromSize)
    {
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM, u32_Index, MIN(u8_PacketSize, u32_EepromSize - u32_Index), 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_AREA_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u32_Index += u8_PacketSize;

        printf("\n packet size = %d ", st_Container.n_InfoLen);
        fwrite(st_Container.ch_Info, 1, st_Container.n_InfoLen, pFile);
        fflush(pFile);
    }
	
    fclose(pFile);
}
void keyb_SYPOSettingsSet(void)
{

    u8 u8_SYPO_GPIO = 0;
    u8 u8_SYPO_WaitForSync = 0;
    u8 u8_SYPO_Mode = 0;
    u32					 u32_Value;

    printf("\nPlease enter SYPO mode (0 - no SYPO, 1 - SLAVE mode, 2 - MASTER mode) :\n");
    tcx_scanf("%d", &u32_Value);
    u8_SYPO_Mode = (u8)u32_Value;
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_SYPO_MODE, &u8_SYPO_Mode, sizeof(u8_SYPO_Mode), 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    if ( u8_SYPO_Mode != CMBS_SYPO_MODE_NONE )
    // Ask for other parameters only if SYPO is enabled
    {
        printf("\nPlease enter SYPO GPIO number: \n");
        tcx_scanf("%d", &u32_Value);
        u8_SYPO_GPIO = (u8)u32_Value;
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamSet(CMBS_PARAM_SYPO_GPIO, &u8_SYPO_GPIO, sizeof(u8_SYPO_GPIO), 1);
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

        printf("\nPlease enter SYPO wait for sync (1 - wait for sync, 0 - do NOT wait for sync) :\n");
        tcx_scanf("%d", &u32_Value);
        u8_SYPO_WaitForSync = (u8)u32_Value;
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamSet(CMBS_PARAM_SYPO_WAIT_FOR_SYNC, &u8_SYPO_WaitForSync, sizeof(u8_SYPO_WaitForSync), 1);
        appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }
}

void keyb_SYPOSettingsGet(void)
{

    u8 u8_SYPO_GPIO = 0;
    u8 u8_SYPO_WaitForSync = 0;
    u8 u8_SYPO_Mode = 0;
    ST_APPCMBS_CONTAINER st_Container;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_SYPO_GPIO, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_SYPO_GPIO = st_Container.ch_Info[0];

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_SYPO_WAIT_FOR_SYNC, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_SYPO_WaitForSync = st_Container.ch_Info[0];

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_SYPO_MODE, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_SYPO_Mode = st_Container.ch_Info[0];

    tcx_appClearScreen();

    printf("SYPO settings\n");
    printf("SYPO GPIO number: %02x\n", u8_SYPO_GPIO);
    printf("SYPO wait for sync: %s\n", u8_SYPO_WaitForSync == TRUE ? "YES" : "NO");
    if ( u8_SYPO_Mode == CMBS_SYPO_MODE_NONE )
        printf("SYPO not activated\n");
    else if ( u8_SYPO_Mode == CMBS_SYPO_MODE_SLAVE )
        printf("SYPO in SLAVE mode\n");
    else if ( u8_SYPO_Mode == CMBS_SYPO_MODE_MASTER )
        printf("SYPO in MASTER mode\n");
}

void  keypb_EEPromParamGet(void)
{
    int   nEvent;

    printf("\nSelect EEProm Param:\n");
    printf("1 => RFPI\n");
    printf("2 => PIN\n");
    printf("3 => Chipset settings\n");
    printf("4 => TEST Mode\n");
    printf("5 => Master PIN\n");
    printf("6 => Flex EEprom get\n");
    printf("7 => Subscription data get\n");
    printf("x => Exended Subscription data get\n");
    printf("8 => Generate eeprom file \n");
    printf("9 => PORBGCFG value \n");
    printf("a => AUXBGPROG value\n");
    printf("b => HAN DECT subscription DB start\n");
    printf("c => HAN DECT subscription DB end\n");
    printf("d => HAN ULE subscription DB start\n");
    printf("e => HAN ULE subscription DB end\n");
    printf("f => HAN FUN subscription DB start\n");
    printf("g => HAN FUN subscription DB end\n");
    printf("h => HAN next TPUI\n");
    printf("i => Pream Normal\n");
    printf("j => Full Power\n");
    printf("k => Low Power\n");
    printf("l => Lowest Power\n");
    printf("m => RF19APU MLSE\n");
    printf("n => RF19APU KCALOVR\n");
    printf("o => RF19APU_KCALOVR_LINEAR\n");
    printf("p => RF19APU Support FCC\n");
    printf("Q => RF19APU Deviation\n");
    printf("r => RF19APU PA2 compatibility\n");
    printf("s => RFIC Selection\n");
    printf("t => MAX usable RSSI\n");
    printf("u => Lower RSSI Limit\n");
    printf("v => PHS Scan Parameter\n");
    printf("w => L1 - minus 82 dBm RSSI threshold for Japan regulation\n");
    printf("z => L2 - minus 62 dBm RSSI threshold for Japan regulation\n");

    printf("y => Maximum number of active calls of 1 PT\n");
    printf("B => Side Tone Volume\n");
    printf("S => SYPO parameters\n");
    printf("U => UART delay configuration\n");
    printf("C => Call Waiting disable per handset\n");
    printf("M => Clock Master Editability\n");
	printf("\n-------------------------------\n");
	printf("q => Return\n");

    switch (tcx_getch())
    {
    case '1':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RFPI, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case '2':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_AUTH_PIN, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case '3':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        keyb_ChipSettingsGet();
        printf("\nPress Any Key!\n");
        tcx_getch();
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        return;

    case '4':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_TEST_MODE, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case '5':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_MASTER_PIN, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case '6':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        keyb_ParamFlexGet();
        nEvent = CMBS_EV_DSR_PARAM_AREA_GET_RES;
        break;

    case '7':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_SUBS_DATA, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;
        
    case 'x':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_SUBS_DATA_EX, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case '8':
        keyb_GetEntireEEPROM();
        nEvent = 0;
        break;

    case '9':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_PORBGCFG, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'a':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_AUXBGPROG_DIRECT, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'b':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'c':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'd':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_START, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'e':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_END, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'f':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_START, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'g':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_END, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'h':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'i':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_PREAM_NORM, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'j':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF_FULL_POWER, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'k':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF_LOW_POWER, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'l':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF_LOWEST_POWER, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'm':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_MLSE, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'n':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_KCALOVR, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'o':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_KCALOVR_LINEAR, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'p':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_SUPPORT_FCC, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'Q':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_DEVIATION, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'r':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RF19APU_PA2_COMP, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 's':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RFIC_SELECTION, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 't':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_MAX_USABLE_RSSI, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'u':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_LOWER_RSSI_LIMIT, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'v':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_PHS_SCAN_PARAM, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'w':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_JDECT_LEVEL1_M82, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'z':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_JDECT_LEVEL2_M62, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case '0':
        keypb_param_test();
        printf("Press Any Key!\n");
        tcx_getch();
        return;

    case 'y':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_MAX_NUM_ACT_CALLS_PT, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'B':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_DAC1_VOL, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'S':
        keyb_SYPOSettingsGet();
        printf("\nPress Any Key!\n");
        tcx_getch();
        return;

    case 'U':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_UART_DELAY_TIMER, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

    case 'C':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_HS_CW_DISABLED, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;
        
    case 'M':
        app_SrvParamGet(CMBS_PARAM_CLOCK_MASTER_EDIT, 1);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;

	case 'q':
    default:
        return;
    }
    // wait for CMBS target message
    if ( nEvent != 0 )
    {
        appcmbs_WaitForEvent(nEvent); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }

    printf("Press Any Key!\n");
    tcx_getch();
}

//		========== keypb_ProductionParamGet ===========
/*!
\brief         Handle Production Settings get
\param[in,ou]  <none>
\return

*/

void  keypb_ProductionParamGet(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    int   nEvent;
    u8 u8_parameter;

    memset(&st_Container, 0, sizeof(st_Container));

    printf("\nSelect Production Param:\n");
    printf("1 => ECO mode\n");
    printf("2 => Get DECT Type (EU, US, Japan, etc...)\n");
    printf("3 => Get Nemo Mode\n");
	printf("\n-------------------------------\n");
	printf("q => Return\n");

    u8_parameter = tcx_getch();
    switch (u8_parameter)
    {
    case '1':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamGet(CMBS_PARAM_ECO_MODE, CMBS_PARAM_ECO_MODE_LENGTH);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;
    case '2':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamGet(CMBS_PARAM_DECT_TYPE, CMBS_PARAM_DECT_TYPE_LENGTH);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;
    case '3':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamGet(CMBS_PARAM_NEMO_MODE, CMBS_PARAM_NEMO_MODE_LENGTH);
        nEvent = CMBS_EV_DSR_PARAM_GET_RES;
        break;
	case 'q':
	default:
        return;
    }

    // wait for CMBS target message
    appcmbs_WaitForContainer(nEvent, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // give time to print debug output
    SleepMs(100);

    switch (u8_parameter)
    {
    case '1':   // CMBS_PARAM_ECO_MODE
        printf("Current ECO Mode is: \t 0x%X  %s", st_Container.ch_Info[0], _EcoMode2Str(st_Container.ch_Info[0]));
        break;
    default: //	case '2':   // CMBS_PARAM_DECT_TYPE
        printf("Current DECT type is: 0x%X %s ", st_Container.ch_Info[0], _DectType2Str(st_Container.ch_Info[0]));
        break;
    }

    printf("\nPress Any Key!\n");
    tcx_getch();
}

void  keyb_ParamFlexSet(void)
{
    char buffer[512] = { 0 };
    u8  u8_Value[1024] = { 0 };
    u8  u8_Data[1024] = { 0 };
    u16 u16_Len = 0;
    u32 u32_Location, i, x;
    ST_APPCMBS_CONTAINER st_Container;

    // save parser state as we want to disable it temporary
    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    printf("Enter Location (dec): ");
    tcx_gets(buffer, sizeof(buffer));
    u32_Location = atoi(buffer);

    printf("Enter Length (dec. max %d): ", g_CMBSMaxTransferSize);
    tcx_gets(buffer, sizeof(buffer));
    u16_Len = (u16)atoi(buffer);
    if ( u16_Len > g_CMBSMaxTransferSize )
        u16_Len = g_CMBSMaxTransferSize;

    // read current value before writing a new one
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM, u32_Location, (u16)u16_Len, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_AREA_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u8_Value[0], st_Container.ch_Info,  u16_Len);

    tcx_appClearScreen();

    // display current data
    printf("CURRENT VALUE: \n");
    for (i = 0; i < u16_Len; i++) printf("%02x ", u8_Value[i]);
    printf("\n\n");

    // restore parser state
    app_set_msgparserEnabled(msgparserEnabled);

    // ask for new data
    printf("Enter New Data (hexadecimal): \n");
    tcx_gets(buffer, sizeof(buffer));

    for (i = 0, x = 0; i < u16_Len; i++)
    {
        u8_Data[i] = app_ASC2HEX(buffer + x);
        x += 2;
    }

    // set new data
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamAreaSet(CMBS_PARAM_AREA_TYPE_EEPROM, u32_Location, u16_Len, u8_Data, 1);

    // wait for response
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_AREA_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // save parser state as we want to disable it temporary
    msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    // read current value before writing a new one
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM, u32_Location, (u16)u16_Len, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_AREA_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u8_Value[0], st_Container.ch_Info,  u16_Len);

    tcx_appClearScreen();

    // display current data
    printf("CURRENT VALUE: \n");
    for (i = 0; i < u16_Len; i++) printf("%02x ", u8_Value[i]);
    printf("\n\n");

    // restore parser state
    app_set_msgparserEnabled(msgparserEnabled);

    printf("\nPress any key...");
    tcx_getch();
}


void     keyb_RxtnGpioConnect(void)
{
    u32 u32_GPIO = 0;

    printf("Enter GPIO (0x00 -0x1B): ");
    tcx_scanf("%X", &u32_GPIO);

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_RxtunGpioConnect(g_cmbsappl.pv_CMBSRef, (u16)u32_GPIO);
    appcmbs_WaitForEvent(CMBS_EV_DSR_GPIO_CONNECT_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void     keyb_RxtnGpioDisconnect(void)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_RxtunGpioDisconnect(g_cmbsappl.pv_CMBSRef);
    appcmbs_WaitForEvent(CMBS_EV_DSR_GPIO_DISCONNECT_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void     keyb_StartATETest(void)
{
    ST_IE_ATE_SETTINGS  st_AteSettings;
    u8                  u8_Ans;
    bool                isVegaOne = FALSE;
    char                hexbuffer[2] = { 0 };
    ST_APPCMBS_CONTAINER st_Container;

    // save parser state as we want to disable it temporary
    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);


    if ( appcmbs_get_CMBSAppInstance()->u8_HwChip ==  CMBS_HW_CHIP_VEGAONE )
        isVegaOne = TRUE;

    SleepMs(20);

    st_AteSettings.u8_NormalPreamble = 0;
    st_AteSettings.u8_PowerLevel = 0;
    st_AteSettings.u8_Pattern = 0;
    st_AteSettings.u8_Slot = 0;

    tcx_appClearScreen();
    printf("ATE test:");

    printf("\nEnter device identity (00-09):\n\t00 - 01 22 33 00 00\n\t01 - 01 22 33 11 10\n\t02 - 01 22 33 22 20\n\t03 - 01 22 33 33 30\n\t04 - 01 22 33 22 40\n\t05 - 01 22 33 22 50\n\t06 - 01 22 33 22 60\n\t07 - 01 22 33 22 70\n\t08 - 01 22 33 22 80\n\t09 - 01 22 33 22 90\nOR FF for default:\n");
    hexbuffer[0] = tcx_getch();
    hexbuffer[1] = tcx_getch();
    st_AteSettings.u8_DeviceIdentity = app_ASC2HEX(hexbuffer);
    printf("\nChosen device identity is = %X\n", st_AteSettings.u8_DeviceIdentity);

    printf("\nEnter slot type:\n\t0 - slot\n\t1 - double slot\n\t2 - long slot\n: ");
    u8_Ans = tcx_getch();
    switch (u8_Ans)
    {
    case '0':
        st_AteSettings.e_ATESlotType = CMBS_ATE_SLOT_TYPE;
        break;

    case '1':
        st_AteSettings.e_ATESlotType = CMBS_ATE_SLOT_TYPE_DOUBLE;
        break;

    case '2':
        st_AteSettings.e_ATESlotType = CMBS_ATE_SLOT_TYPE_LONG;
        break;

    default:
        printf("\nError parameter !\n");
        tcx_getch();
        return;
    }
    printf("\nEnter ATE test type:\n\t0 - RX\n\t1 - continuous RX\n\t2 - TX\n");
    if ( !isVegaOne )
    {
        // currently continuous TX is not supported in V1
        printf("\t3 - continuous TX\n: ");
    }

    u8_Ans = tcx_getch();
    switch (u8_Ans)
    {
    case '0':
        st_AteSettings.e_ATEType = CMBS_ATE_TYPE_RX;
        break;

    case '1':
        st_AteSettings.e_ATEType = CMBS_ATE_TYPE_CONTINUOUS_RX;
        break;

    case '2':
        st_AteSettings.e_ATEType = CMBS_ATE_TYPE_TX;
        break;

    case '3':
        st_AteSettings.e_ATEType = CMBS_ATE_TYPE_CONTINUOUS_TX;
        break;

    default:
        printf("\nError parameter !\n");
        tcx_getch();
        return;
    }

    if ( st_AteSettings.e_ATEType == CMBS_ATE_TYPE_CONTINUOUS_RX || st_AteSettings.e_ATEType == CMBS_ATE_TYPE_RX )
    {
        u8 quit = 0;
        // GPIO connect
        do
        {
            printf("\nInsert the GPIO (two hex digits, 03 - 1B) to use for measurement (FF for None): ");

            hexbuffer[0] = tcx_getch();
            hexbuffer[1] = tcx_getch();
            st_AteSettings.u8_Gpio = app_ASC2HEX(hexbuffer);

            if ( st_AteSettings.u8_Gpio >= 0x03 && st_AteSettings.u8_Gpio <= 0x1B )
                quit = 1;
            else if ( st_AteSettings.u8_Gpio == 0xFF )
                quit = 1;
        }while (!quit);
    }

    printf("\nEnter Instance (0..9): ");
    st_AteSettings.u8_Instance = tcx_getch() - '0';

    if ( st_AteSettings.e_ATEType == CMBS_ATE_TYPE_RX )
    {
        printf("\nSync pattern: 0-FP, 1-PP: ");
        st_AteSettings.u8_Pattern = tcx_getch() - '0';
        printf("\nEnter Slot (two digits, 0..11): ");
        hexbuffer[0] = tcx_getch();
        hexbuffer[1] = tcx_getch();
        st_AteSettings.u8_Slot = app_ASC2HEX(hexbuffer);
    }
    printf("\nEnter Carrier 0..9 for ETSI DECT, 0..94 for WDCT, 23..27 for US DECT (decimal): ");
    hexbuffer[0] = tcx_getch();
    hexbuffer[1] = tcx_getch();
    st_AteSettings.u8_Carrier = atoi(hexbuffer);

    printf("\nEnter Ant (0,1): ");
    st_AteSettings.u8_Ant = tcx_getch() - '0';
    if ( st_AteSettings.e_ATEType == CMBS_ATE_TYPE_TX )
    {
        printf("\nEnter Slot (two digits, 0..11): ");
        hexbuffer[0] = tcx_getch();
        hexbuffer[1] = tcx_getch();
        st_AteSettings.u8_Slot = app_ASC2HEX(hexbuffer);

        printf("\nEnter Pattern(0..4: [0,0x22,0xF0,0xDD,Fig31]): ");
        st_AteSettings.u8_Pattern = tcx_getch() - '0';

        printf("\nEnter Power Level (0,1 or 2): ");
        st_AteSettings.u8_PowerLevel = tcx_getch() - '0';

        printf("\nEnter Normal Preamble(y/n): ");
        u8_Ans = tcx_getch();
        switch (u8_Ans)
        {
        case 'y':
        case 'Y':
            st_AteSettings.u8_NormalPreamble = 1;
            break;

        case 'n':
        case 'N':
        default:
            st_AteSettings.u8_NormalPreamble = 0;
            break;
        }
    }
    else if ( st_AteSettings.e_ATEType == CMBS_ATE_TYPE_RX )
    {
        printf("\nEnable BER-FER measurement (y/n): ");
        u8_Ans = tcx_getch();
        switch (u8_Ans)
        {
        case 'y':
        case 'Y':
            {
                char hexbuffer[10];
                printf("\nNumber of frames for ber measurement (hex, >0x64 for manual test):");
                hexbuffer[0] = tcx_getch();
                hexbuffer[1] = tcx_getch();
                st_AteSettings.u8_BERFERFrameCount = app_ASC2HEX(hexbuffer);
            }
            st_AteSettings.u8_BERFEREnabled = 1;
            break;

        case 'n':
        case 'N':
        default:
            st_AteSettings.u8_BERFEREnabled = 0;
            break;
        }
    }
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    if ( cmbs_dsr_ATETestStart(g_cmbsappl.pv_CMBSRef, &st_AteSettings) == CMBS_RC_OK )
    {
        appcmbs_WaitForEvent(CMBS_EV_DSR_ATE_TEST_START_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

        if ( st_AteSettings.e_ATEType == CMBS_ATE_TYPE_RX )
        {
            if ( st_AteSettings.u8_BERFEREnabled )
            {
                u8 u8_Buffer[CMBS_PARAM_BERFER_VALUE_LENGTH];

                u32 u32_Value = 0;

                printf("FER       BER     (per 0x%x frames)\n", st_AteSettings.u8_BERFERFrameCount);

                while (!kbhit())
                {
					appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
                    app_SrvParamGet(CMBS_PARAM_BERFER_VALUE, 1);
                    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
                    memcpy(&u8_Buffer, st_Container.ch_Info,  CMBS_PARAM_BERFER_VALUE_LENGTH);
                    memcpy(&u32_Value, u8_Buffer, 4); // Copy BERFER values

                    printf("BERFER: ");
                    if ( u32_Value & 0xFF000000 )
                    {
                        u16 u16_BERcnt;
                        u8 u8_FERcnt;

                        u16_BERcnt = u32_Value & 0xFFFF;
                        u8_FERcnt = (u32_Value >> 16) & 0xFF;

                        if ( st_AteSettings.u8_BERFERFrameCount > 100 )
                        {
                            u32 u32_Temp = 0;
                            if ( u8_FERcnt != 100 )
                                u32_Temp = (u16_BERcnt * 100000) / (320 * (100 - u8_FERcnt));

                            if ( u32_Temp < 1000 )
                            {
                                printf("FER(*10E-2) = %03d\tBER(*10E-5) = %4d:\n", u8_FERcnt, u32_Temp);
                            }
                            else if ( u32_Temp < 10000 )
                            {
                                printf("FER(*10E-2) = %03d\tBER(*10E-4) = %4d:\n", u8_FERcnt, (u32_Temp / 10));
                            }
                            else if ( u32_Temp < 100000 )
                            {
                                printf("FER(*10E-2) = %03d\tBER(*10E-3) = %4d:\n", u8_FERcnt, (u32_Temp / 100));
                            }
                            else if ( u32_Temp < 1000000 )
                            {
                                printf("FER(*10E-2) = %03d\tBER(*10E-2) = %4d:\n", u8_FERcnt, (u32_Temp / 1000));
                            }
                        }

                        else
                        {
                            printf("%03x\t%4x\n", u8_FERcnt, u16_BERcnt);
                        }

                        printf("RSSI MAX = %X, RSSI MIN = %X", u8_Buffer[4], u8_Buffer[5]);
                        SleepMs(1000);
                    }
                    else
                    {
                        printf("...\n");
                    }

                    SleepMs(200);
                }
            }
            else
            {
                u32 u32_ADC = 0;

                while (!kbhit())
                {
					appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
                    app_SrvParamGet(CMBS_PARAM_RSSI_VALUE, 1);
                    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
                    memcpy(&u32_ADC, st_Container.ch_Info,  CMBS_PARAM_RSSI_VALUE_LENGTH);

                    printf("\n\nRSSI: %d\n\n", u32_ADC);

                    SleepMs(200);
                }
            }
        }
    }
    else
    {
		appcmbs_PrepareRecvAdd ( FALSE);
        printf("Test was not started as parameters are wrong");
    }

    app_set_msgparserEnabled(msgparserEnabled);

    printf("\nPress any key !\n");
    tcx_getch();
}

void     keyb_LeaveATETest(void)
{
    cmbs_dsr_ATETestLeave(g_cmbsappl.pv_CMBSRef);
    SleepMs(20);
    printf("\nPress any key !\n");
    tcx_getch();
}


void keyb_ListAccess_DeleteList(void)
{
    u8 u8_Ans;

    tcx_appClearScreen();

    printf("List Access Delete List\n");
    printf("========================\n\n");
    printf("select list:\n");
    printf("1: Missed calls List\n");
    printf("2: Outgoing calls List\n");
    printf("3: Incoming accepted calls List\n");
    printf("4: Line Settings List\n");
    printf("5: Contact List\n");
    printf("6: Line Status and Diagnostic List\n");
    printf("7: DTAM Settings List\n");
    printf("8: DTAM Welcome messages List\n");
    printf("9: SMS Settings List\n");
    

    printf("\n");

    u8_Ans = tcx_getch();

    switch (u8_Ans)
    {
    case '1':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_MISSED_CALLS);

            List_DeleteAllEntries(LIST_TYPE_MISSED_CALLS);

            printf("Missed calls list cleared!");
        }
        break;

    case '2':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_OUTGOING_CALLS);

            List_DeleteAllEntries(LIST_TYPE_OUTGOING_CALLS);

            printf("Outgoing calls list cleared!");
        }
        break;

    case '3':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_INCOMING_ACCEPTED_CALLS);

            List_DeleteAllEntries(LIST_TYPE_INCOMING_ACCEPTED_CALLS);

            printf("Incoming Accepted calls list cleared!");
        }
        break;

    case '4':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_LINE_SETTINGS_LIST);

            List_DeleteAllEntries(LIST_TYPE_LINE_SETTINGS_LIST);

            printf("Line Settings list cleared!");
        }
        break;

    case '5':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_CONTACT_LIST);
        }
        break;

    case '6':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS);

            List_DeleteAllEntries(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS);

            printf("Line and Diagnostic list cleared!");
        }
        break;

    case '7':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_DTAM_SETTINGS_LIST);

            List_DeleteAllEntries(LIST_TYPE_DTAM_SETTINGS_LIST);

            printf("DTAM Settings List cleared!");
        }
        break;

    case '8':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_DTAM_WELCOME_MESSAGE);

            List_DeleteAllEntries(LIST_TYPE_DTAM_WELCOME_MESSAGE);

            printf("DTAM Welcome Messages List cleared!");
        }
        break;
        
    case '9':
        {
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_SMS_SETTINGS_LIST);

            List_DeleteAllEntries(LIST_TYPE_SMS_SETTINGS_LIST);

            printf("SMS Settings List cleared!");
        }
        break;

    default:
        printf("\nError parameter !\n");
        return;
    }
}

void keyb_ListAccess_DumpEntry(LIST_TYPE eListType, void *pv_Entry)
{
    const char *sTrue = "TRUE", *sFalse = "FALSE";

    switch (eListType)
    {
    case LIST_TYPE_CONTACT_LIST:
        {
            stContactListEntry *pst_Entry = pv_Entry;
            const char *sNum1Type, *sNum2Type, *sNum3Type, *sWork = "WORK", *sMobile = "MOBILE", *sFixed = "FIXED";

            switch (pst_Entry->cNumber1Type)
            {
            case NUM_TYPE_WORK:
                sNum1Type = sWork;    break;
            case NUM_TYPE_MOBILE:
                sNum1Type = sMobile;  break;
            default:
                sNum1Type = sFixed;   break;
            }

            switch (pst_Entry->cNumber2Type)
            {
            case NUM_TYPE_WORK:
                sNum2Type = sWork;    break;
            case NUM_TYPE_MOBILE:
                sNum2Type = sMobile;  break;
            default:
                sNum2Type = sFixed;   break;
            }

            switch (pst_Entry->cNumber3Type)
            {
            case NUM_TYPE_WORK:
                sNum3Type = sWork;    break;
            case NUM_TYPE_MOBILE:
                sNum3Type = sMobile;  break;
            default:
                sNum3Type = sFixed;   break;
            }

            printf("Entry Id            = %d\n", pst_Entry->u32_EntryId);
            printf("Last Name           = %s\n", pst_Entry->sLastName);
            printf("First Name          = %s\n", pst_Entry->sFirstName);
            printf("Number 1            = %s\n", pst_Entry->sNumber1);
            printf("Number 1 Type       = %s\n", sNum1Type);
            printf("Number 1 Default    = %s\n", pst_Entry->bNumber1Default ? sTrue : sFalse);
            printf("Number 1 Own        = %s\n", pst_Entry->bNumber1Own ? sTrue : sFalse);
            printf("Number 2            = %s\n", pst_Entry->sNumber2);
            printf("Number 2 Type       = %s\n", sNum2Type);
            printf("Number 2 Default    = %s\n", pst_Entry->bNumber2Default ? sTrue : sFalse);
            printf("Number 2 Own        = %s\n", pst_Entry->bNumber2Own ? sTrue : sFalse);
            printf("Associated Melody   = %d\n", pst_Entry->u32_AssociatedMelody);
            printf("Number 3            = %s\n", pst_Entry->sNumber3);
            printf("Number 3 Type       = %s\n", sNum3Type);
            printf("Number 3 Default    = %s\n", pst_Entry->bNumber3Default ? sTrue : sFalse);
            printf("Number 3 Own        = %s\n", pst_Entry->bNumber3Own ? sTrue : sFalse);
            printf("Line Id             = %d\n", pst_Entry->u32_LineId);
        }
        break;

    case LIST_TYPE_LINE_SETTINGS_LIST:
        {
            stLineSettingsListEntry *pst_Entry = pv_Entry;
            printf("Entry Id                            = %d\n", pst_Entry->u32_EntryId);
            printf("Line Name                           = %s\n", pst_Entry->sLineName);
            printf("Line Id                             = %d\n", pst_Entry->u32_LineId);
            printf("Attached Hs Mask                    = 0x%X\n", pst_Entry->u32_AttachedHsMask);
            printf("Dial Prefix                         = %s\n", pst_Entry->sDialPrefix);
            printf("FP Melody                           = %d\n", pst_Entry->u32_FPMelody);
            printf("FP Volume                           = %d\n", pst_Entry->u32_FPVolume-0x30);
            printf("Blocked Number                      = %s\n", pst_Entry->sBlockedNumber);
            printf("Multi-Call Enabled                  = %s\n", pst_Entry->bMultiCalls ? sTrue : sFalse);
            printf("Intrusion Enabled                   = %s\n", pst_Entry->bIntrusionCall ? sTrue : sFalse);
            printf("Permanent CLIR set                  = %s\n", pst_Entry->bPermanentCLIR ? sTrue : sFalse);
            printf("Permanent CLIR Activation code      = %s\n", pst_Entry->sPermanentCLIRActCode);
            printf("Permanent CLIR DeActivation code    = %s\n", pst_Entry->sPermanentCLIRDeactCode);
            printf("Call Fwd Unconditional Enabled      = %s\n", pst_Entry->bCallFwdUncond ? sTrue : sFalse);
            printf("Call Fwd Uncond Activation code     = %s\n", pst_Entry->sCallFwdUncondActCode);
            printf("Call Fwd Uncond DeActivation code   = %s\n", pst_Entry->sCallFwdUncondDeactCode);
            printf("Call Fwd Uncond Number              = %s\n", pst_Entry->sCallFwdUncondNum);
            printf("Call Fwd No Answer Enabled          = %s\n", pst_Entry->bCallFwdNoAns ? sTrue : sFalse);
            printf("Call Fwd No Ans Activation code     = %s\n", pst_Entry->sCallFwdNoAnsActCode);
            printf("Call Fwd No Ans DeActivation code   = %s\n", pst_Entry->sCallFwdNoAnsDeactCode);
            printf("Call Fwd No Answer Number           = %s\n", pst_Entry->sCallFwdNoAnsNum);
            printf("Call Fwd No Answer Timeout          = %d\n", pst_Entry->u32_CallFwdNoAnsTimeout);
            printf("Call Fwd Busy Enabled               = %s\n", pst_Entry->bCallFwdBusy ? sTrue : sFalse);
            printf("Call Fwd Busy Activation code       = %s\n", pst_Entry->sCallFwdBusyActCode);
            printf("Call Fwd Busy DeActivation code     = %s\n", pst_Entry->sCallFwdBusyDeactCode);
            printf("Call Fwd Busy Number                = %s\n", pst_Entry->sCallFwdBusyNum);
        }
        break;
        
    case LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS:
        {
            const char *sEnabled = "ENABLED", *sDisabled = "DISABLED";
            const char *sWANErr = "WAN ERROR", *sUnknown = "UNKNOWN";

            stLineAndDiagnosticListEntry*pst_Entry = pv_Entry;
            printf("Entry Id\t= %d\n", pst_Entry->u32_EntryId);
            printf("Line Id\t= %d\n", pst_Entry->u32_LineId);
            printf("OK Status\t= %s\n", app_GetOKStatus(pst_Entry->u32_OKStatus));
            printf("Line Use Status\t= %s\n", app_GetLineUseStatus(pst_Entry->u32_LineUseStatus));
            printf("HS Use Status (HS Mask)\t= 0x%X\n\n", pst_Entry->u32_HSUseStatus);
            printf("Call Forwarding Unconditional Status\t= %s\n", pst_Entry->b_CallFwdUncondStatus ? sEnabled : sDisabled);
            printf("Call Forwarding No Answer Status\t= %s\n", pst_Entry->b_CallFwdNoAnswerStatus ? sEnabled : sDisabled);
            printf("Call Forwarding Busy Status\t= %s\n", pst_Entry->b_CallFwdBusyStatus ? sEnabled : sDisabled);
            printf("Diagnostic Error Status\t= %d\n", pst_Entry->b_DiagnosticErrorStatus);
            printf("Diagnostic Error Type\t= %s\n", app_GetErrorType(pst_Entry->u32_DiagnosticErrorType));
            if(pst_Entry->u32_DiagnosticErrorType <= CMBS_LINE_DIAG_NETWORK_ERROR)
                printf("Diagnostic Network Error Number\t= %s\n", pst_Entry->u32_DiagnosticErrorNumber ? sWANErr : sUnknown);       
            else
                printf("Diagnostic Local Error Number\t= %s\n", app_GetLocalErrorNumber(pst_Entry->u32_DiagnosticErrorNumber));       
            
        }
        break;

    case LIST_TYPE_DTAM_SETTINGS_LIST:
        {
            const char *sLocal = "Local", *sRemote = "Remote";
            const char *sVoice = "Voice", *sVisual = "Visual";

            stDTAMSettingsListEntry*pst_Entry = pv_Entry;
            printf("Entry Id\t= %d\n", pst_Entry->u32_EntryId);
            printf("Line Id\t= %d\n", pst_Entry->u32_LineId);
            printf("DTAM Identifier\t= %d\n", pst_Entry->u32_DTAMIdentifier);
            printf("DTAM Type\t= %s\n", pst_Entry->b_DTAMType ? sVisual :sVoice);
            printf("DTAM Location\t= %s\n", pst_Entry->b_DTAMLocation? sRemote : sLocal);
            printf("DTAM Number\t= %s\n", pst_Entry->sDTAMNumber);
            printf("DTAM Activation\t= %s\n", pst_Entry->b_DTAMActivation ? sTrue : sFalse);
            printf("DTAM Default timeout activated\t= %s\n", pst_Entry->b_DTAMDefaultTimeout ? sTrue : sFalse);
            printf("DTAM Timeout\t= %d\n", pst_Entry->u32_DTAMTimeout);
            printf("DTAM Weblink\t= %s\n", pst_Entry->sDTAMWebLink);
            printf("Welcome Message index\t= %d\n", pst_Entry->u32_WelcomeMsgParams);
        }
        break;
        
    case LIST_TYPE_DTAM_WELCOME_MESSAGE:
        {
            const char *sLocal = "Local", *sRemote = "Remote";
            const char *sVoice = "Voice", *sVisual = "Visual";

            stDTAMWelcomeMessageListEntry*pst_Entry = pv_Entry;
            printf("Entry Id\t= %d\n", pst_Entry->u32_EntryId);
            printf("DTAM Identifier\t= %d\n", pst_Entry->u32_DTAMIdentifier);
            printf("DTAM Type\t= %s\n", pst_Entry->b_DTAMType ? sVisual :sVoice);
            printf("DTAM Location\t= %s\n", pst_Entry->b_DTAMLocation? sRemote : sLocal);
            printf("Position Index\t= %d\n", pst_Entry->u32_PositionIdx);            
            printf("Recorded Message Name\t= %s\n", pst_Entry->sRecordedMsgName);            
            printf("Recorded Message Length Hours\t= %d\n", pst_Entry->u32_TimeDurationHours);
            printf("Recorded Message Length Minutes\t= %d\n", pst_Entry->u32_TimeDurationMinutes);
            printf("Recorded Message Length Seconds\t= %d\n", pst_Entry->u32_TimeDurationSeconds);
            
        }
        break;

    case LIST_TYPE_SMS_SETTINGS_LIST:
        {
            const char *sEnabled = "ENABLED", *sDisabled = "DISABLED";

            stSMSSettingsListEntry*pst_Entry = pv_Entry;
            printf("Entry Id\t= %d\n", pst_Entry->u32_EntryId);
            printf("SMS Id\t= %d\n", pst_Entry->u32_SMSServiseId);
            printf("Line Id\t= %d\n", pst_Entry->u32_LineId);
            printf("SMS Enabled\t= %s\n", pst_Entry->b_EnableSMS ? sEnabled : sDisabled);
            printf("SMS Max size\t= %d\n", pst_Entry->u32_SMSMaxSize);
            printf("SMSC Send server\t= %s\n", pst_Entry->sSMSCSendServer);
            printf("SMSC Receive Server\t= %s\n", pst_Entry->sSMSCReceiveServer);
            printf("SMS Delivery reprot\t= %s\n", pst_Entry->b_SMSDeliveryReport? sEnabled : sDisabled);
            printf("SMS Validity period\t= %d\n", pst_Entry->u32_SMSValidityPeriod);
            printf("SMS char allowed encoding GSM 7 bit\t= %d\n", pst_Entry->u32_SMSCharacterEncodingValue_1);
            printf("SMS allowed encoding default\t= %d\n", pst_Entry->u32_AllowedEncoding_0);
            printf("SMS allowed encoding Turkish\t= %d\n", pst_Entry->u32_AllowedEncoding_1);
            printf("SMS allowed encoding Spanish\t= %d\n", pst_Entry->u32_AllowedEncoding_2);
            printf("SMS allowed encoding Portuguese\t= %d\n", pst_Entry->u32_AllowedEncoding_3);
            printf("SMS allowed encoding Bengali\t= %d\n", pst_Entry->u32_AllowedEncoding_4);
            printf("SMS allowed encoding Gujarati\t= %d\n", pst_Entry->u32_AllowedEncoding_5);
            printf("SMS allowed encoding Hindi\t= %d\n", pst_Entry->u32_AllowedEncoding_6);
            printf("SMS allowed encoding Kannada\t= %d\n", pst_Entry->u32_AllowedEncoding_7);
            printf("SMS allowed encoding Malayalam\t= %d\n", pst_Entry->u32_AllowedEncoding_8);
            printf("SMS allowed encoding Oriya\t= %d\n", pst_Entry->u32_AllowedEncoding_9);
            printf("SMS allowed encoding Punjabi\t= %d\n", pst_Entry->u32_AllowedEncoding_10);
            printf("SMS allowed encoding Tamil\t= %d\n", pst_Entry->u32_AllowedEncoding_11);
            printf("SMS allowed encoding Telugu\t= %d\n", pst_Entry->u32_AllowedEncoding_12);
            printf("SMS allowed encoding default\t= %d\n", pst_Entry->u32_AllowedEncoding_13);
            printf("SMS allowed encoding Reseved\t= %d\n", pst_Entry->u32_AllowedEncoding_14);
            printf("SMS char allowed encoding GSM 8 bit\t= %d\n", pst_Entry->u32_SMSCharacterEncodingValue_2);
            printf("SMS char allowed encoding UCS-2\t= %d\n", pst_Entry->u32_SMSCharacterEncodingValue_3);
            printf("SMS char allowed encoding UTF-8 bit\t= %d\n", pst_Entry->u32_SMSCharacterEncodingValue_4);
        }
        break;

    case LIST_TYPE_OUTGOING_SMS:
    case LIST_TYPE_SENT_SMS:        
         {
            struct tm *pst_Time;
            stSMSListEntry*pst_Entry = pv_Entry;            

            pst_Time = localtime(&(pst_Entry->t_DateAndTime));

            printf("Entry Id\t= %d\n", pst_Entry->u32_EntryId);
            printf("Number\t = %s\n", pst_Entry->sNumber);
            printf("Name\t = %s\n", pst_Entry->sName);
            printf("Time - Year   = %d\n", pst_Time->tm_year + 1900);
            printf("Time - Month  = %d\n", pst_Time->tm_mon + 1);
            printf("Time - Day    = %d\n", pst_Time->tm_mday);
            printf("Time - Hour   = %d\n", pst_Time->tm_hour);
            printf("Time - Min    = %d\n", pst_Time->tm_min);
            printf("Time - Sec    = %d\n", pst_Time->tm_sec);  
            printf("SMS Id\t= %d\n", pst_Entry->u32_SMSServiseId);            
            printf("Network Side Encoding\t= %d\n", pst_Entry->u32_NetworkSideEncoding);
            printf("Encoding Variant #1\t = %d\n", pst_Entry->u32_EncVariant_1);
            printf("Encoding Varinat #1\t = %d\n", pst_Entry->u32_EncVariant_1);            
            printf("SMS Size\t= %d\n", pst_Entry->u32_SMSSize);
            printf("SMS Content\t= %s\n", pst_Entry->sSMSContent);
        }
        break;

    case LIST_TYPE_INCOMING_SMS:
        {
            const char *sRead = "READ", *sUnread = "UNREAD";
            stSMSListEntry*pst_Entry = pv_Entry;            

            struct tm *pst_Time;
            pst_Time = localtime(&(pst_Entry->t_DateAndTime));

            printf("Entry Id\t= %d\n", pst_Entry->u32_EntryId);
            printf("Number\t = %s\n", pst_Entry->sNumber);
            printf("Name\t = %s\n", pst_Entry->sName);
            printf("Time - Year   = %d\n", pst_Time->tm_year + 1900);
            printf("Time - Month  = %d\n", pst_Time->tm_mon + 1);
            printf("Time - Day    = %d\n", pst_Time->tm_mday);
            printf("Time - Hour   = %d\n", pst_Time->tm_hour);
            printf("Time - Min    = %d\n", pst_Time->tm_min);
            printf("Time - Sec    = %d\n", pst_Time->tm_sec);       
            printf("SMS Id\t= %d\n", pst_Entry->u32_SMSServiseId);            
            printf("Read Status %s\n", pst_Entry->b_Read ? sRead : sUnread);                        
            printf("SMS Size\t= %d\n", pst_Entry->u32_SMSSize);
            printf("SMS Content\t= %s\n", pst_Entry->sSMSContent);        
        }
        break;

    case LIST_TYPE_DRAFT_SMS:
        {
            struct tm *pst_Time;
            const char *sSend = "SEND REQ SENT", *sNotSend = "SEND REQ NOT SENT";
            stSMSListEntry*pst_Entry = pv_Entry;            

            pst_Time = localtime(&(pst_Entry->t_DateAndTime));

            printf("Entry Id\t= %d\n", pst_Entry->u32_EntryId);
            printf("Number\t = %s\n", pst_Entry->sNumber);
            printf("Name\t = %s\n", pst_Entry->sName);
            printf("Time - Year   = %d\n", pst_Time->tm_year + 1900);
            printf("Time - Month  = %d\n", pst_Time->tm_mon + 1);
            printf("Time - Day    = %d\n", pst_Time->tm_mday);
            printf("Time - Hour   = %d\n", pst_Time->tm_hour);
            printf("Time - Min    = %d\n", pst_Time->tm_min);
            printf("Time - Sec    = %d\n", pst_Time->tm_sec);  
            printf("SMS Id\t= %d\n", pst_Entry->u32_SMSServiseId);            
            printf("Read Status %s\n", pst_Entry->b_SendRequest ? sSend : sNotSend);                                    
            printf("Network Side Encoding\t= %d\n", pst_Entry->u32_NetworkSideEncoding);
            printf("Encoding Variant #1\t = %d\n", pst_Entry->u32_EncVariant_1);
            printf("Encoding Varinat #1\t = %d\n", pst_Entry->u32_EncVariant_1);            
            printf("SMS Size\t= %d\n", pst_Entry->u32_SMSSize);
            printf("SMS Content\t= %s\n", pst_Entry->sSMSContent);
        }
        break;
        
    default:
        {
            stCallsListEntry *pst_Entry = pv_Entry;
            const char *sCallType;
            struct tm *pst_Time;
            pst_Time = localtime(&(pst_Entry->t_DateAndTime));

            switch (pst_Entry->cCallType)
            {
            case CALL_TYPE_MISSED:
                sCallType = "Missed Call";      break;
            case CALL_TYPE_OUTGOING:
                sCallType = "Outgoing Call";    break;
            default:
                sCallType = "Incoming Call";    break;
            }

            printf("Entry Id      = %d\n", pst_Entry->u32_EntryId);
            printf("Number        = %s\n", pst_Entry->sNumber);
            printf("Time - Year   = %d\n", pst_Time->tm_year + 1900);
            printf("Time - Month  = %d\n", pst_Time->tm_mon + 1);
            printf("Time - Day    = %d\n", pst_Time->tm_mday);
            printf("Time - Hour   = %d\n", pst_Time->tm_hour);
            printf("Time - Min    = %d\n", pst_Time->tm_min);
            printf("Time - Sec    = %d\n", pst_Time->tm_sec);
            printf("Line name     = %s\n", pst_Entry->sLineName);
            printf("Line ID       = %d\n", pst_Entry->u32_LineId);
            printf("Last name     = %s\n", pst_Entry->sLastName);
            printf("First name    = %s\n", pst_Entry->sFirstName);

            if ( eListType == LIST_TYPE_MISSED_CALLS || eListType == LIST_TYPE_ALL_INCOMING_CALLS )
            {
                printf("Read          = %s\n", pst_Entry->bRead ? sTrue : sFalse);
                printf("Num Of Calls  = %d\n", pst_Entry->u32_NumOfCalls);
            }
            else if ( eListType == LIST_TYPE_ALL_CALLS )
            {
                printf("Call Type     = %s\n", sCallType);
            }
        }
        break;
    }
}

void keyb_ListAccess_AddToSuppList(void)
{
    printf("\nAdding Outgoing calls list...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_la_AddSupportedList(NULL, CMBS_LA_LIST_OUTGOING_CALLS);
    appcmbs_WaitForEvent(CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    printf("\ndone.\n");

    printf("\nAdding All calls list...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_la_AddSupportedList(NULL, CMBS_LA_LIST_ALL_CALLS);
    appcmbs_WaitForEvent(CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    printf("\ndone.\n");

    printf("\nAdding All incoming calls list...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_la_AddSupportedList(NULL, CMBS_LA_LIST_ALL_INCOMING_CALLS);
    appcmbs_WaitForEvent(CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    printf("\ndone.\n");
}

void keyb_ListAccess_SpecificFields(void)
{
    u8 u8_Ans;
    LIST_RC rc;
    u32 u32_LineId, u32_AttachedHsMask;
    char ps8_Buffer[10];

    tcx_appClearScreen();

    printf("List Access Specifc Fields\n");
    printf("==========================\n\n");
    printf("1: Get Line Attached Handsets\n");
    printf("2: Set Line Attached Handsets\n");

    printf("\n");

    u8_Ans = tcx_getch();

    switch (u8_Ans)
    {
    case '1':
        {
            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            u32_LineId = atoi(ps8_Buffer);

            rc = List_GetAttachedHs(u32_LineId, &u32_AttachedHsMask);
            if ( rc != LIST_RC_OK )
            {
                printf("error: rc = %d\n", rc);
            }
            else
            {
                printf("Attached Hs Mask  = 0x%X\n", u32_AttachedHsMask);
            }
        }
        break;
    case '2':
        {
            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            u32_LineId = atoi(ps8_Buffer);

            printf("Enter Attahced HS Mask (1 or 2 bytes)...\n");
            tcx_scanf("%x", &u32_AttachedHsMask);
            rc = List_SetAttachedHs(u32_LineId, u32_AttachedHsMask);
            if ( rc != LIST_RC_OK )
            {
                printf("error: rc = %d\n", rc);
            }
            else
            {
                rc = List_GetAttachedHs(u32_LineId, &u32_AttachedHsMask);
                if(rc != LIST_RC_OK)
	                printf("error: rc = %d for LineId = %d\n", rc, u32_LineId);                
                printf("Attached Hs Mask  = 0x%X\n", u32_AttachedHsMask);
                printf("done.\n");
            }
        }
        break;
    default:
        printf("\nError parameter !\n");

        return;

    }
}

void keyb_ListAccess_FillContactList(void)
{
    stContactListEntry st_Entry;
    u32 pu32_FiledIDs[5], u32_EntryId;
    u32 i = 0, j = 0;

    pu32_FiledIDs[0] = FIELD_ID_LAST_NAME;
    pu32_FiledIDs[1] = FIELD_ID_FIRST_NAME;
    pu32_FiledIDs[2] = FIELD_ID_CONTACT_NUM_1;
    pu32_FiledIDs[3] = FIELD_ID_ASSOCIATED_MELODY;
    pu32_FiledIDs[4] = FIELD_ID_LINE_ID;

    st_Entry.u32_AssociatedMelody = 0;
    st_Entry.u32_LineId = 0;
    st_Entry.bNumber1Default = TRUE;
    st_Entry.bNumber1Own = FALSE;
    st_Entry.cNumber1Type = NUM_TYPE_FIXED;

    printf("\nFilling database with entries... \n");

    for (; i < 26; ++i)
    {
        for (j = 0; j < 10; ++j)
        {
            sprintf(st_Entry.sFirstName,  "%c%d", 'a' + i, j);
            sprintf(st_Entry.sLastName,  "%c%d", 'A' + i, j * 10);
            sprintf(st_Entry.sNumber1,  "%d", i * 10 + j * 100 + i);

            List_InsertEntry(LIST_TYPE_CONTACT_LIST, &st_Entry, pu32_FiledIDs, 5, &u32_EntryId);

            printf("\r%d%%", 100 * (10 * i + j + 1) / 260);
        }
    }

    printf("\nDone!\n");
}

void keyb_ListAccess_DumpList(void)
{
    u8 u8_Ans, pu8_Entry[LIST_ENTRY_MAX_SIZE];
    u32 u32_Count, u32_Index, pu32_Fields[FIELD_ID_MAX], u32_FieldsSize, u32_SortField, u32_SortField2 = FIELD_ID_INVALID, u32_NumOfEntries = 1;
    LIST_TYPE eListType;
    const char *sListName;

    tcx_appClearScreen();

    printf("List Access Display List Contents\n");
    printf("=================================\n\n");
    printf("select list:\n");
    printf("1: Contact List\n");
    printf("2: Line Settings List\n");
    printf("3: Missed calls List\n");
    printf("4: Outgoing Calls List\n");
    printf("5: Incoming Accepted Calls List\n");
    printf("6: All Calls List\n");
    printf("7: All Incoming Calls List\n");
    printf("8: Line Status and Diagnostic List\n");
    printf("9: DTAM Settings List\n");
    printf("A: DTAM Welcome Messages List\n");
    printf("B: SMS Settings List\n");

    printf("\n");

    u8_Ans = tcx_getch();

    switch (u8_Ans)
    {
    case '1':
        {
            eListType = LIST_TYPE_CONTACT_LIST;
            sListName = "Contacts";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_LAST_NAME;
            pu32_Fields[2] = FIELD_ID_FIRST_NAME;
            pu32_Fields[3] = FIELD_ID_CONTACT_NUM_1;
            pu32_Fields[4] = FIELD_ID_CONTACT_NUM_2;
            pu32_Fields[5] = FIELD_ID_CONTACT_NUM_3;
            pu32_Fields[6] = FIELD_ID_ASSOCIATED_MELODY;
            pu32_Fields[7] = FIELD_ID_LINE_ID;
            u32_FieldsSize = 8;

            u32_SortField   = FIELD_ID_LAST_NAME;
            u32_SortField2  = FIELD_ID_FIRST_NAME;
        }
        break;

    case '2':
        {
            eListType = LIST_TYPE_LINE_SETTINGS_LIST;
            sListName = "Line Settings";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_LINE_NAME;
            pu32_Fields[2] = FIELD_ID_LINE_ID;
            pu32_Fields[3] = FIELD_ID_ATTACHED_HANDSETS;
            pu32_Fields[4] = FIELD_ID_DIALING_PREFIX;
            pu32_Fields[5] = FIELD_ID_FP_MELODY;
            pu32_Fields[6] = FIELD_ID_FP_VOLUME;
            pu32_Fields[7] = FIELD_ID_BLOCKED_NUMBER;
            pu32_Fields[8] = FIELD_ID_MULTIPLE_CALLS_MODE;
            pu32_Fields[9] = FIELD_ID_INTRUSION_CALL;
            pu32_Fields[10] = FIELD_ID_PERMANENT_CLIR;
            pu32_Fields[11] = FIELD_ID_CALL_FWD_UNCOND;
            pu32_Fields[12] = FIELD_ID_CALL_FWD_NO_ANSWER;
            pu32_Fields[13] = FIELD_ID_CALL_FWD_BUSY;
            u32_FieldsSize = 14;

            u32_SortField = FIELD_ID_LINE_ID;
        }
        break;

    case '3':
        {
            eListType = LIST_TYPE_MISSED_CALLS;
            sListName = "Missed Calls";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_NUMBER;
            pu32_Fields[2] = FIELD_ID_LAST_NAME;
            pu32_Fields[3] = FIELD_ID_FIRST_NAME;
            pu32_Fields[4] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[5] = FIELD_ID_READ_STATUS;
            pu32_Fields[6] = FIELD_ID_LINE_NAME;
            pu32_Fields[7] = FIELD_ID_LINE_ID;
            pu32_Fields[8] = FIELD_ID_NUM_OF_CALLS;
            u32_FieldsSize = 9;

            u32_SortField = FIELD_ID_DATE_AND_TIME;
        }
        break;

    case '4':
        {
            eListType = LIST_TYPE_OUTGOING_CALLS;
            sListName = "Outgoing Calls";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_NUMBER;
            pu32_Fields[2] = FIELD_ID_LAST_NAME;
            pu32_Fields[3] = FIELD_ID_FIRST_NAME;
            pu32_Fields[4] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[5] = FIELD_ID_LINE_NAME;
            pu32_Fields[6] = FIELD_ID_LINE_ID;
            u32_FieldsSize = 7;

            u32_SortField = FIELD_ID_DATE_AND_TIME;
        }
        break;

    case '5':
        {
            eListType = LIST_TYPE_INCOMING_ACCEPTED_CALLS;
            sListName = "Incoming Accepted";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_NUMBER;
            pu32_Fields[2] = FIELD_ID_LAST_NAME;
            pu32_Fields[3] = FIELD_ID_FIRST_NAME;
            pu32_Fields[4] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[5] = FIELD_ID_LINE_NAME;
            pu32_Fields[6] = FIELD_ID_LINE_ID;
            u32_FieldsSize = 7;

            u32_SortField = FIELD_ID_DATE_AND_TIME;
        }
        break;

    case '6':
        {
            eListType = LIST_TYPE_ALL_CALLS;
            sListName = "All Calls";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_CALL_TYPE;
            pu32_Fields[2] = FIELD_ID_NUMBER;
            pu32_Fields[3] = FIELD_ID_LAST_NAME;
            pu32_Fields[4] = FIELD_ID_FIRST_NAME;
            pu32_Fields[5] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[6] = FIELD_ID_LINE_NAME;
            pu32_Fields[7] = FIELD_ID_LINE_ID;
            u32_FieldsSize = 8;

            u32_SortField = FIELD_ID_DATE_AND_TIME;
        }
        break;

    case '7':
        {
            eListType = LIST_TYPE_ALL_INCOMING_CALLS;
            sListName = "All Incoming Calls";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_NUMBER;
            pu32_Fields[2] = FIELD_ID_LAST_NAME;
            pu32_Fields[3] = FIELD_ID_FIRST_NAME;
            pu32_Fields[4] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[5] = FIELD_ID_READ_STATUS;
            pu32_Fields[6] = FIELD_ID_LINE_NAME;
            pu32_Fields[7] = FIELD_ID_LINE_ID;
            pu32_Fields[8] = FIELD_ID_NUM_OF_CALLS;
            u32_FieldsSize = 9;

            u32_SortField = FIELD_ID_DATE_AND_TIME;
        }
        break;
        
    case '8':
        {
            eListType = LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS;
            sListName = "Line and Diagnostic Status";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_LINE_ID;
            pu32_Fields[2] = FIELD_ID_OK_STATUS;
            pu32_Fields[3] = FIELD_ID_LINE_USE_STATUS;
            pu32_Fields[4] = FIELD_ID_HS_USE_STATUS;
            pu32_Fields[5] = FIELD_ID_CALL_FWD_CFU_STATUS;
            pu32_Fields[6] = FIELD_ID_DIAGNOSTIC_ERROR_STATUS;
            u32_FieldsSize = 7;

            u32_SortField = FIELD_ID_LINE_ID;
        }
        break;
        
    case '9':
        {
            eListType = LIST_TYPE_DTAM_SETTINGS_LIST;
            sListName = "DTAM Settings List";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_LINE_ID;
            pu32_Fields[2] = FIELD_ID_DTAM_IDENTIFIER;
            pu32_Fields[3] = FIELD_ID_DTAM_NUMBER;
            pu32_Fields[4] = FIELD_ID_DTAM_ACTIVATION;
            pu32_Fields[5] = FIELD_ID_DTAM_WEB_LINK;
            pu32_Fields[6] = FIELD_ID_DTAM_WELCOME_MSG_PARAMETERS;
            u32_FieldsSize = 7;

            u32_SortField = FIELD_ID_LINE_ID;
        }
        break;
        
    case 'A':
    case 'a':
        {
            eListType = LIST_TYPE_DTAM_WELCOME_MESSAGE;
            sListName = "DTAM Welcome Messages List";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_DTAM_IDENTIFIER;
            pu32_Fields[2] = FIELD_ID_DTAM_POSITION_INDEX;
            pu32_Fields[3] = FIELD_ID_DTAM_RECORDED_MSG_NAME;
            pu32_Fields[4] = FIELD_ID_DTAM_TIME_DURATION_HOURS;
            u32_FieldsSize = 5;

            u32_SortField = FIELD_ID_DTAM_IDENTIFIER;
        }
        break;
        
    case 'B':
    case 'b':
        {
            eListType = LIST_TYPE_SMS_SETTINGS_LIST;
            sListName = "SMS Settings List";

            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_SMS_IDENTIFIER;
            pu32_Fields[2] = FIELD_ID_LINE_ID;            
            pu32_Fields[3] = FIELD_ID_SMS_ENABLE;
            pu32_Fields[4] = FIELD_ID_SMS_MAX_SIZE;
            pu32_Fields[5] = FIELD_ID_SMSC_SEND_SERVER;
            pu32_Fields[6] = FIELD_ID_SMSC_RECEIVE_SERVER;
            pu32_Fields[7] = FIELD_ID_SMS_DELIVERY_REPORT;
            pu32_Fields[8] = FIELD_ID_SMS_VALIDITY_PERIOD;
            pu32_Fields[9] = FIELD_ID_SMS_CHAR_ALLOWED_ENCODING;
            u32_FieldsSize = 10;

            u32_SortField = FIELD_ID_SMS_IDENTIFIER;
        }
        break;

    default:
        printf("\nError parameter !\n");
        return;
    }

    tcx_appClearScreen();

    printf("Contents of %s List:\n", sListName);
    printf("====================\n");

    /* Create if not exist */
    List_CreateList(eListType);

    List_GetCount(eListType, &u32_Count);
    printf("Total Num Of Entries = %d\n", u32_Count);

    for (u32_Index = 0; u32_Index < u32_Count; ++u32_Index)
    {
        u32 u32_StartIdx = u32_Index + 1;
        List_ReadEntries(eListType, &u32_StartIdx, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsSize,
                         u32_SortField, u32_SortField2, pu8_Entry, &u32_NumOfEntries);

        printf("Entry #%d of %d:\n", u32_Index + 1, u32_Count);
        printf("===================\n");
        keyb_ListAccess_DumpEntry(eListType, pu8_Entry);
        printf("**********************************************\n");
    }
}

void keyb_ListAccess_InsertEntry(void)
{
    u8 u8_Ans;
    char ps8_Buffer[10];
    stCallsListEntry         st_CallListEntry;
    stLineSettingsListEntry  st_LineSettingsListEntry;
    stContactListEntry       st_ContactEntry;
    stLineAndDiagnosticListEntry st_LineDiagnosticEntry;
    stDTAMSettingsListEntry st_DTAMSettingsListEntry;
    stDTAMWelcomeMessageListEntry st_DTAMWelcomeMessageListEntry;
    stSMSSettingsListEntry st_SMSSettingsListEntry;
    u8 u8_Selection;
    
    time_t t_Time;

    memset(&st_CallListEntry,           0, sizeof(st_CallListEntry));
    memset(&st_LineSettingsListEntry,   0, sizeof(st_LineSettingsListEntry));
    memset(&st_ContactEntry,           	0, sizeof(st_ContactEntry));
	memset(&st_LineDiagnosticEntry, 0, sizeof(st_LineDiagnosticEntry));
	memset(&st_DTAMSettingsListEntry, 0, sizeof(st_DTAMSettingsListEntry));
	memset(&st_DTAMWelcomeMessageListEntry, 0, sizeof(st_DTAMWelcomeMessageListEntry));
	memset(&st_SMSSettingsListEntry, 0, sizeof(st_SMSSettingsListEntry));


    tcx_appClearScreen();

    printf("List Access Insert Entry\n");
    printf("========================\n\n");
    printf("select list:\n");
    printf("1: Missed calls List\n");
    printf("2: Outgoing calls List\n");
    printf("3: Incoming accepted calls List\n");
    printf("4: Line Settings List\n");
    printf("5: Contact List\n");
    printf("6: Line Status and Diagnostic List\n");
    printf("7: DTAM Settings List\n");
    printf("8: DTAM Welcome Messages List\n");
    printf("9: SMS settings List\n");
    
    printf("\n");

    u8_Ans = tcx_getch();

    switch (u8_Ans)
    {
    case '1':
        {
            u32 pu32_Fields[5], u32_FieldsNum = 5, u32_EntryId;
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_MISSED_CALLS);

            printf("Enter Number...\n");
            tcx_gets(st_CallListEntry.sNumber, sizeof(st_CallListEntry.sNumber));

            time(&t_Time);
            printf("Using current time as Date and Time...\n");
            st_CallListEntry.t_DateAndTime = t_Time;

            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_CallListEntry.u32_LineId = atoi(ps8_Buffer);

            printf("Enter Number of calls...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_CallListEntry.u32_NumOfCalls = atoi(ps8_Buffer);

            printf("Enter Read Status (0 - Unread, 1 - Read)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_CallListEntry.bRead = atoi(ps8_Buffer);

            pu32_Fields[0] = FIELD_ID_NUMBER;
            pu32_Fields[1] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[2] = FIELD_ID_LINE_ID;
            pu32_Fields[3] = FIELD_ID_NUM_OF_CALLS;
            pu32_Fields[4] = FIELD_ID_READ_STATUS;

            List_InsertEntry(LIST_TYPE_MISSED_CALLS, &st_CallListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;

    case '2':
    case '3':
        {
            u32 pu32_Fields[4], u32_FieldsNum = 3, u32_EntryId;
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList((u8_Ans == '2') ? LIST_TYPE_OUTGOING_CALLS : LIST_TYPE_INCOMING_ACCEPTED_CALLS);

            printf("Enter Number...\n");
            tcx_gets(st_CallListEntry.sNumber, sizeof(st_CallListEntry.sNumber));

            time(&t_Time);
            printf("Using current time as Date and Time...\n");
            st_CallListEntry.t_DateAndTime = t_Time;

            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_CallListEntry.u32_LineId = atoi(ps8_Buffer);

            pu32_Fields[0] = FIELD_ID_NUMBER;
            pu32_Fields[1] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[2] = FIELD_ID_LINE_ID;

            List_InsertEntry((u8_Ans == '2') ? LIST_TYPE_OUTGOING_CALLS : LIST_TYPE_INCOMING_ACCEPTED_CALLS,
                             &st_CallListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;

    case '4':
        {
            u32 pu32_Fields[13], u32_FieldsNum = 13, u32_EntryId;
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_LINE_SETTINGS_LIST);

            printf("Enter Line name...\n");
            tcx_gets(st_LineSettingsListEntry.sLineName, sizeof(st_LineSettingsListEntry.sLineName));

            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.u32_LineId = atoi(ps8_Buffer);

            printf("Enter Attahced HS Mask (1 or 2 bytes)... (HEX)\n");
            tcx_scanf("%x", &st_LineSettingsListEntry.u32_AttachedHsMask);

            printf("Enter Dialing prefix...(any number or 'none')\n");
            tcx_gets(st_LineSettingsListEntry.sDialPrefix, sizeof(st_LineSettingsListEntry.sDialPrefix));
            if ( strcmp(st_LineSettingsListEntry.sDialPrefix, "none") == 0 )
            {
                st_LineSettingsListEntry.sDialPrefix[0] = 0;
            }

            printf("Enter FP Melody...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.u32_FPMelody = atoi(ps8_Buffer);

            printf("Enter FP Volume...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.u32_FPVolume = atoi(ps8_Buffer)+0x30;

            printf("Enter Blocked telephone number...\n");
            tcx_gets(st_LineSettingsListEntry.sBlockedNumber, sizeof(st_LineSettingsListEntry.sBlockedNumber));

            printf("Enter Multicall enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.bMultiCalls = atoi(ps8_Buffer);

            printf("Enter Intrusion enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.bIntrusionCall = atoi(ps8_Buffer);

            printf("Enter Permanent CLIR enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.bPermanentCLIR = atoi(ps8_Buffer);

            printf("Enter Permanent CLIR Activation code...\n");
            tcx_gets(st_LineSettingsListEntry.sPermanentCLIRActCode, sizeof(st_LineSettingsListEntry.sPermanentCLIRActCode));

            printf("Enter Permanent CLIR Deactivation code...\n");
            tcx_gets(st_LineSettingsListEntry.sPermanentCLIRDeactCode, sizeof(st_LineSettingsListEntry.sPermanentCLIRDeactCode));

            printf("Enter Call Fwd Uncond enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.bCallFwdUncond = atoi(ps8_Buffer);

            printf("Enter Call Fwd Uncond Activation code...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdUncondActCode, sizeof(st_LineSettingsListEntry.sCallFwdUncondActCode));

            printf("Enter Call Fwd Uncond Deactivation code...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdUncondDeactCode, sizeof(st_LineSettingsListEntry.sCallFwdUncondDeactCode));

            printf("Enter Call Fwd Uncond Number...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdUncondNum, sizeof(st_LineSettingsListEntry.sCallFwdUncondNum));

            printf("Enter Call Fwd No Answer enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.bCallFwdNoAns = atoi(ps8_Buffer);

            printf("Enter Call Fwd No Answer Activation code...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdNoAnsActCode, sizeof(st_LineSettingsListEntry.sCallFwdNoAnsActCode));

            printf("Enter Call Fwd No Answer Deactivation code...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdNoAnsDeactCode, sizeof(st_LineSettingsListEntry.sCallFwdNoAnsDeactCode));

            printf("Enter Call Fwd No Answer Number...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdNoAnsNum, sizeof(st_LineSettingsListEntry.sCallFwdNoAnsNum));

            printf("Enter Call Fwd No Answer Timeout [0..64]...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.u32_CallFwdNoAnsTimeout = atoi(ps8_Buffer);

            printf("Enter Call Fwd Busy enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineSettingsListEntry.bCallFwdBusy = atoi(ps8_Buffer);

            printf("Enter Call Fwd Busy Activation code...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdBusyActCode, sizeof(st_LineSettingsListEntry.sCallFwdBusyActCode));

            printf("Enter Call Fwd Busy Deactivation code...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdBusyDeactCode, sizeof(st_LineSettingsListEntry.sCallFwdBusyDeactCode));

            printf("Enter Call Fwd Busy Number...\n");
            tcx_gets(st_LineSettingsListEntry.sCallFwdBusyNum, sizeof(st_LineSettingsListEntry.sCallFwdBusyNum));

            pu32_Fields[0] = FIELD_ID_LINE_NAME;
            pu32_Fields[1] = FIELD_ID_LINE_ID;
            pu32_Fields[2] = FIELD_ID_ATTACHED_HANDSETS;
            pu32_Fields[3] = FIELD_ID_DIALING_PREFIX;
            pu32_Fields[4] = FIELD_ID_FP_VOLUME;
            pu32_Fields[5] = FIELD_ID_FP_MELODY;
            pu32_Fields[6] = FIELD_ID_BLOCKED_NUMBER;
            pu32_Fields[7] = FIELD_ID_MULTIPLE_CALLS_MODE;
            pu32_Fields[8] = FIELD_ID_INTRUSION_CALL;
            pu32_Fields[9] = FIELD_ID_PERMANENT_CLIR;
            pu32_Fields[10] = FIELD_ID_CALL_FWD_UNCOND;
            pu32_Fields[11] = FIELD_ID_CALL_FWD_NO_ANSWER;
            pu32_Fields[12] = FIELD_ID_CALL_FWD_BUSY;

            List_InsertEntry(LIST_TYPE_LINE_SETTINGS_LIST, &st_LineSettingsListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            /* Need to update target with Line Settings */
            {
                ST_IE_LINE_SETTINGS_LIST st_LineSettingsList;

                st_LineSettingsList.u16_Attached_HS     = (u16)st_LineSettingsListEntry.u32_AttachedHsMask;
                st_LineSettingsList.u8_Call_Intrusion   = st_LineSettingsListEntry.bIntrusionCall;
                st_LineSettingsList.u8_Line_Id          = (u8)st_LineSettingsListEntry.u32_LineId;
                st_LineSettingsList.u8_Multiple_Calls   = st_LineSettingsListEntry.bMultiCalls ? CMBS_MULTICALL_LINE_NUM_OF_CALLS : 0;
                st_LineSettingsList.e_LineType          = st_LineSettingsListEntry.bMultiCalls ? CMBS_LINE_TYPE_VOIP_PARALLEL_CALL : CMBS_LINE_TYPE_VOIP_DOUBLE_CALL;

                app_SrvLineSettingsSet(&st_LineSettingsList, 1);
            }

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;

    case '5':
        {
            u32 pu32_Fields[10], u32_FieldsNum = 0, u32_EntryId;
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_CONTACT_LIST);

            printf("Enter Last name...\n");
            tcx_gets(st_ContactEntry.sLastName, sizeof(st_ContactEntry.sLastName));
            pu32_Fields[u32_FieldsNum++] = FIELD_ID_LAST_NAME;

            printf("Enter First name...\n");
            tcx_gets(st_ContactEntry.sFirstName, sizeof(st_ContactEntry.sFirstName));
            pu32_Fields[u32_FieldsNum++] = FIELD_ID_FIRST_NAME;

            printf("Enter Number...\n");
            tcx_gets(st_ContactEntry.sNumber1, sizeof(st_ContactEntry.sNumber1));

            printf("Enter Number Type: 0=Fixed, 1=Mobile, 2=Work...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_ContactEntry.cNumber1Type = atoi(ps8_Buffer);

            printf("Enter Is Number Default (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_ContactEntry.bNumber1Default = atoi(ps8_Buffer);

            printf("Enter Is Number Own (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_ContactEntry.bNumber1Own = atoi(ps8_Buffer);
			
            pu32_Fields[u32_FieldsNum++] = FIELD_ID_CONTACT_NUM_1;

            printf("To Enter Number 2 details press Y , to skip press N \n");

            u8_Selection = tcx_getch();

            if((u8_Selection == 'Y') || (u8_Selection == 'y'))
            {
	            printf("Enter Number 2...\n");
	            tcx_gets(st_ContactEntry.sNumber2, sizeof(st_ContactEntry.sNumber2));

	            printf("Enter Number 2 Type: 0=Fixed, 1=Mobile, 2=Work...\n");
	            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
	            st_ContactEntry.cNumber2Type = atoi(ps8_Buffer);

	            printf("Enter Is Number 2 Default (0 / 1)...\n");
	            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
	            st_ContactEntry.bNumber2Default = atoi(ps8_Buffer);

	            printf("Enter Is Number 2 Own (0 / 1)...\n");
	            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
	            st_ContactEntry.bNumber2Own = atoi(ps8_Buffer);

	            pu32_Fields[u32_FieldsNum++] = FIELD_ID_CONTACT_NUM_2;
            }

            printf("To Enter Number 3 details press Y , to skip press N \n");

            u8_Selection = tcx_getch();

            if((u8_Selection == 'Y') || (u8_Selection == 'y'))
            {
	            printf("Enter Number 3...\n");
	            tcx_gets(st_ContactEntry.sNumber3, sizeof(st_ContactEntry.sNumber3));

	            printf("Enter Number 3 Type: 0=Fixed, 1=Mobile, 2=Work...\n");
	            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
	            st_ContactEntry.cNumber3Type = atoi(ps8_Buffer);

	            printf("Enter Is Number 3 Default (0 / 1)...\n");
	            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
	            st_ContactEntry.bNumber3Default = atoi(ps8_Buffer);

	            printf("Enter Is Number 3 Own (0 / 1)...\n");
	            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
	            st_ContactEntry.bNumber3Own = atoi(ps8_Buffer);

	            pu32_Fields[u32_FieldsNum++] = FIELD_ID_CONTACT_NUM_3;
            }
		 
            printf("Enter Associated Melody (0-255)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_ContactEntry.u32_AssociatedMelody = atoi(ps8_Buffer);
            pu32_Fields[u32_FieldsNum++] = FIELD_ID_ASSOCIATED_MELODY;

            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_ContactEntry.u32_LineId = atoi(ps8_Buffer);
            pu32_Fields[u32_FieldsNum++] = FIELD_ID_LINE_ID;

            List_InsertEntry(LIST_TYPE_CONTACT_LIST, &st_ContactEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;
        
    case '6':
        {
            u32 pu32_Fields[6], u32_FieldsNum = 6, u32_EntryId;
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS);

            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.u32_LineId = atoi(ps8_Buffer);

            printf("Enter OK Status (0 - 'down', 1 - 'registering', 2 - 'initializing', 3 - 'up')...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.u32_OKStatus = atoi(ps8_Buffer);

            printf("Enter Line Use Status (0 - 'idle', 1 - 'in-use', 2 - 'busy')...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.u32_LineUseStatus = atoi(ps8_Buffer);

            printf("Enter Call Fwd Uncond enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.b_CallFwdUncondStatus = atoi(ps8_Buffer);

            printf("Enter Call Fwd No Answer enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.b_CallFwdNoAnswerStatus = atoi(ps8_Buffer);

            printf("Enter Call Fwd Busy enable (0 / 1)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.b_CallFwdBusyStatus = atoi(ps8_Buffer);

            printf("Enter HS Use Status (HS Mask HEX) ...\n"); 
            tcx_scanf("%x", &st_LineDiagnosticEntry.u32_HSUseStatus);

            printf("Enter Diagnostic Error Status (0 - 'No error or no identified cause', 1 - 'Identified cause for the occuring error') ...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.b_DiagnosticErrorStatus = atoi(ps8_Buffer);

            printf("Enter Diagnostic Error type (0 - 'Unknown error', 1 - 'Network error', 2 - 'Local error')...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_LineDiagnosticEntry.u32_DiagnosticErrorType = atoi(ps8_Buffer);

            if(st_LineDiagnosticEntry.u32_DiagnosticErrorType == CMBS_LINE_DIAG_LOCAL_ERROR)
            {
                printf("Enter Local Diagnostic Error Number: \n");
                printf("'0' - Unknown local error \n");
                printf("'1' - LAN error \n");
                printf("'2' - LAN level 2 error (e.g. Ethernet error) \n");
                printf("'3' - LAN level 3 error (e.g. IP error) \n");
                printf("'4' - Cable error  \n");
                printf("'5' - Modem or Home Gateway error \n");

                tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
                st_LineDiagnosticEntry.u32_DiagnosticErrorNumber = atoi(ps8_Buffer);
            }
            else if(st_LineDiagnosticEntry.u32_DiagnosticErrorType == CMBS_LINE_DIAG_NETWORK_ERROR)
            {
                printf("Enter Network Diagnostic Error Number: \n");
                printf("'0' - Unknown Network error \n");
                printf("'1' - WAN error \n");

                tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
                st_LineDiagnosticEntry.u32_DiagnosticErrorNumber = atoi(ps8_Buffer);
             }
            else
            {
                 st_LineDiagnosticEntry.u32_DiagnosticErrorNumber = 0;
            }
            
            pu32_Fields[0] = FIELD_ID_LINE_ID;
            pu32_Fields[1] = FIELD_ID_OK_STATUS;
            pu32_Fields[2] = FIELD_ID_LINE_USE_STATUS;
            pu32_Fields[3] = FIELD_ID_HS_USE_STATUS;
            pu32_Fields[4] = FIELD_ID_CALL_FWD_CFU_STATUS;
            pu32_Fields[5] = FIELD_ID_DIAGNOSTIC_ERROR_STATUS;

            List_InsertEntry(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS, &st_LineDiagnosticEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;
        
    case '7':
        {
            u32 pu32_Fields[6], u32_FieldsNum = 6, u32_EntryId;
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_DTAM_SETTINGS_LIST);

            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMSettingsListEntry.u32_LineId = atoi(ps8_Buffer);

            printf("Enter DTAM ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMSettingsListEntry.u32_DTAMIdentifier = atoi(ps8_Buffer);
            st_DTAMSettingsListEntry.b_DTAMLocation = 1; //Remote DTAM
            st_DTAMSettingsListEntry.b_DTAMType = 0;    //Voice DTAM

            printf("Enter DTAM Number...\n");
            tcx_gets(st_DTAMSettingsListEntry.sDTAMNumber, sizeof(st_DTAMSettingsListEntry.sDTAMNumber));

            printf("Enter DTAM Activation: 0=Deactivated, 1=Activated\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMSettingsListEntry.b_DTAMActivation = atoi(ps8_Buffer);

            printf("Enter DTAM timeout setting (1=FP default timeout value 0=Configured timeout)...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMSettingsListEntry.b_DTAMDefaultTimeout = atoi(ps8_Buffer);

            printf("Enter DTAM timeout ...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMSettingsListEntry.u32_DTAMTimeout= atoi(ps8_Buffer);

            printf("Enter Web Link...\n");
            tcx_gets(st_DTAMSettingsListEntry.sDTAMWebLink, sizeof(st_DTAMSettingsListEntry.sDTAMWebLink));

            printf("EnterWelcome Message position index...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMSettingsListEntry.u32_WelcomeMsgParams= atoi(ps8_Buffer);

            pu32_Fields[0] = FIELD_ID_LINE_ID;
            pu32_Fields[1] = FIELD_ID_DTAM_IDENTIFIER;
            pu32_Fields[2] = FIELD_ID_DTAM_NUMBER;
            pu32_Fields[3] = FIELD_ID_DTAM_ACTIVATION;
            pu32_Fields[4] = FIELD_ID_DTAM_WEB_LINK;
            pu32_Fields[5] = FIELD_ID_DTAM_WELCOME_MSG_PARAMETERS;

            List_InsertEntry(LIST_TYPE_DTAM_SETTINGS_LIST, &st_DTAMSettingsListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;

    case '8':
        {
            u32 pu32_Fields[4], u32_FieldsNum = 4, u32_EntryId;
            tcx_appClearScreen();

            /* Create if not exist */
            List_CreateList(LIST_TYPE_DTAM_WELCOME_MESSAGE);

            printf("Enter DTAM ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMWelcomeMessageListEntry.u32_DTAMIdentifier = atoi(ps8_Buffer);
            st_DTAMWelcomeMessageListEntry.b_DTAMLocation = 1; //Remote DTAM
            st_DTAMWelcomeMessageListEntry.b_DTAMType = 0;    //Voice DTAM

            printf("Enter Position Index...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMWelcomeMessageListEntry.u32_PositionIdx = atoi(ps8_Buffer);

            printf("Enter Recorded Message Name...\n");
            tcx_gets(st_DTAMWelcomeMessageListEntry.sRecordedMsgName, sizeof(st_DTAMWelcomeMessageListEntry.sRecordedMsgName));

            printf("Enter Duration - Hours...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMWelcomeMessageListEntry.u32_TimeDurationHours = atoi(ps8_Buffer);
            
            printf("Enter Duration - Minutes...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMWelcomeMessageListEntry.u32_TimeDurationMinutes = atoi(ps8_Buffer);
            
            printf("Enter Duration - Seconds...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_DTAMWelcomeMessageListEntry.u32_TimeDurationSeconds = atoi(ps8_Buffer);

            pu32_Fields[0] = FIELD_ID_DTAM_IDENTIFIER;
            pu32_Fields[1] = FIELD_ID_DTAM_POSITION_INDEX;
            pu32_Fields[2] = FIELD_ID_DTAM_RECORDED_MSG_NAME;
            pu32_Fields[3] = FIELD_ID_DTAM_TIME_DURATION_HOURS;

            List_InsertEntry(LIST_TYPE_DTAM_WELCOME_MESSAGE, &st_DTAMWelcomeMessageListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;
        
    case '9':
        {
            u32 pu32_Fields[9], u32_FieldsNum = 9, u32_EntryId;
            stAllowedEncodingVariant st_EncVariant;
            u8 u8_selection = 0;
            
            tcx_appClearScreen();
            memset(&st_EncVariant, 0 , sizeof(st_EncVariant));
            /* Create if not exist */
            List_CreateList(LIST_TYPE_SMS_SETTINGS_LIST);

            printf("Enter SMS ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.u32_SMSServiseId = atoi(ps8_Buffer);
      
            printf("Enter Line ID...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.u32_LineId = atoi(ps8_Buffer);

            printf("SMS Enabled? '0' - No, '1' - Yes...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.b_EnableSMS = atoi(ps8_Buffer);

            printf("Enter SMS Maximum size ...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.u32_SMSMaxSize = atoi(ps8_Buffer);
            
            printf("Enter SMSC Send server...\n");
            tcx_gets(st_SMSSettingsListEntry.sSMSCSendServer, sizeof(st_SMSSettingsListEntry.sSMSCSendServer));

            printf("Enter SMSC Receive server...\n");
            tcx_gets(st_SMSSettingsListEntry.sSMSCReceiveServer, sizeof(st_SMSSettingsListEntry.sSMSCReceiveServer));
            
            printf("SMS Delivery Report requested?  '0' - No, '1' - Yes...\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.b_SMSDeliveryReport = atoi(ps8_Buffer);

            printf("Enter SMS Validity period (values: 0-255)\n");
            printf ("   0-143 => 5 minutes intervals up to 12 hours\n");
            printf ("   144-167 => 30 minutes intervals up to 24 hours\n");
            printf ("   168-196 => day intervals up to 30 days\n");
            printf ("   197-255 => week intervals up to 63 weeks\n");
                        
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.u32_SMSValidityPeriod = atoi(ps8_Buffer);

            printf("Allowed SMS character encodings and variants\n");
            printf("Choose Allowed SMS character encodings and variants:\n");
            printf ("GSM 7 bit support ('0' - No, '1' - Yes)\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.u32_SMSCharacterEncodingValue_1 = atoi(ps8_Buffer);
            if(st_SMSSettingsListEntry.u32_SMSCharacterEncodingValue_1 == TRUE)
                {
                        while (u8_selection != 'q' && u8_selection != 'Q')
                        {
                            printf("1. Default: %d\n", st_EncVariant.b_Default);
                            printf("2. Turkish: %d\n", st_EncVariant.b_Turkish);
                            printf("3. Spanish: %d\n", st_EncVariant.b_Spanish);                           
                            printf("4. Portuguese: %d\n", st_EncVariant.b_Portuguese);
                            printf("5. Bengali: %d\n", st_EncVariant.b_Bengali);
                            printf("6. Gujarati: %d\n", st_EncVariant.b_Gujarati);
                            printf("7. Hindi: %d\n", st_EncVariant.b_Hindi);
                            printf("8. Kannada: %d\n", st_EncVariant.b_Kannada);
                            printf("9. Malayalam: %d\n", st_EncVariant.b_Malayalam);
                            printf("a. Oriya: %d\n", st_EncVariant.b_Oriya);
                            printf("b. Punjabi: %d\n", st_EncVariant.b_Punjabi);
                            printf("c. Tamil: %d\n", st_EncVariant.b_Tamil);
                            printf("d. Telugu: %d\n", st_EncVariant.b_Telugu);
                            printf("e. Urdu: %d\n", st_EncVariant.b_Urdu);
                            printf ("Enter 'q' to quit\n");

                            u8_selection = tcx_getch();

                            tcx_appClearScreen();

                            switch (u8_selection)
                            {
                                case '1':
                                    st_EncVariant.b_Default = !st_EncVariant.b_Default;
                                    if(st_EncVariant.b_Default)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_0 = st_EncVariant.b_Default;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }
                                    break;
                                case '2':
                                    st_EncVariant.b_Turkish = !st_EncVariant.b_Turkish;
                                    if(st_EncVariant.b_Turkish)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_1 = st_EncVariant.b_Turkish;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }
                                    break;
                                case '3':
                                    st_EncVariant.b_Spanish = !st_EncVariant.b_Spanish;
                                    if(st_EncVariant.b_Spanish)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_2 = st_EncVariant.b_Spanish;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                    
                                    break;                                    
                                case '4':
                                    st_EncVariant.b_Portuguese = !st_EncVariant.b_Portuguese;
                                    if(st_EncVariant.b_Portuguese)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_3 = st_EncVariant.b_Portuguese;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                    
                                    break;
                                case '5':
                                    st_EncVariant.b_Bengali = !st_EncVariant.b_Bengali;
                                    if(st_EncVariant.b_Bengali)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_4 = st_EncVariant.b_Bengali;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                        
                                    break;
                                case '6':
                                    st_EncVariant.b_Gujarati = !st_EncVariant.b_Gujarati;
                                    if(st_EncVariant.b_Gujarati)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_5 = st_EncVariant.b_Gujarati;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case '7':
                                    st_EncVariant.b_Hindi = !st_EncVariant.b_Hindi;
                                    if(st_EncVariant.b_Hindi)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_6 = st_EncVariant.b_Hindi;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case '8':
                                    st_EncVariant.b_Kannada = !st_EncVariant.b_Kannada;
                                    if(st_EncVariant.b_Kannada)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_7 = st_EncVariant.b_Kannada;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case '9':
                                    st_EncVariant.b_Malayalam = !st_EncVariant.b_Malayalam;
                                    if(st_EncVariant.b_Malayalam)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_8 = st_EncVariant.b_Malayalam;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case 'a':
                                    st_EncVariant.b_Oriya = !st_EncVariant.b_Oriya;
                                    if(st_EncVariant.b_Oriya)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_9 = st_EncVariant.b_Oriya;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case 'b':
                                    st_EncVariant.b_Punjabi = !st_EncVariant.b_Punjabi;
                                    if(st_EncVariant.b_Punjabi)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_10 = st_EncVariant.b_Punjabi;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case 'c':
                                    st_EncVariant.b_Tamil = !st_EncVariant.b_Tamil;
                                    if(st_EncVariant.b_Tamil)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_11 = st_EncVariant.b_Tamil;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case 'd':
                                    st_EncVariant.b_Telugu = !st_EncVariant.b_Telugu;
                                    if(st_EncVariant.b_Telugu)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_12 = st_EncVariant.b_Telugu;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;
                                case 'e':
                                    st_EncVariant.b_Urdu = !st_EncVariant.b_Urdu;
                                    if(st_EncVariant.b_Urdu)
                                    {
                                       st_SMSSettingsListEntry.u32_AllowedEncoding_13 = st_EncVariant.b_Urdu;                                    
                                       st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_1++;
                                     }                                                                                                            
                                    break;                                    
                           }
                     }
                }
            printf ("GSM 8 bit support ('0' - No, '1' - Yes)\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_2 = atoi(ps8_Buffer);
            if( st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_2)
            {
                st_SMSSettingsListEntry.u32_SMSCharacterEncodingValue_2 = CMBS_SMS_ENC_TYPE_GSM_8_BIT;
            }

            printf ("UCS support ('0' - No, '1' - Yes)\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));            
            st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_3 = atoi(ps8_Buffer);
            if( st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_3)
            {
                st_SMSSettingsListEntry.u32_SMSCharacterEncodingValue_3 = CMBS_SMS_ENC_TYPE_UCS_2;
            }
            printf ("UTF-8 support ('0' - No, '1' - Yes)\n");
            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));            
            st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_4 = atoi(ps8_Buffer);
            if( st_SMSSettingsListEntry.u32_LengthOfAllowedEncoding_4)
            {
                st_SMSSettingsListEntry.u32_SMSCharacterEncodingValue_4 = CMBS_SMS_ENC_TYPE_UTF_8;
            }
            pu32_Fields[0] = FIELD_ID_SMS_IDENTIFIER;
            pu32_Fields[1] = FIELD_ID_LINE_ID;            
            pu32_Fields[2] = FIELD_ID_SMS_ENABLE;
            pu32_Fields[3] = FIELD_ID_SMS_MAX_SIZE;
            pu32_Fields[4] = FIELD_ID_SMSC_SEND_SERVER;
            pu32_Fields[5] = FIELD_ID_SMSC_RECEIVE_SERVER;
            pu32_Fields[6] = FIELD_ID_SMS_DELIVERY_REPORT;
            pu32_Fields[7] = FIELD_ID_SMS_VALIDITY_PERIOD;
            pu32_Fields[8] = FIELD_ID_SMS_CHAR_ALLOWED_ENCODING;

            List_InsertEntry(LIST_TYPE_SMS_SETTINGS_LIST, &st_SMSSettingsListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

            printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
        }
        break;

    default:
        printf("\nError parameter !\n");
        return;
    }
}
void keyb_ListAccess_SimulateOKStatus(void)
{
     stLineAndDiagnosticListEntry st_Entry;
     char ps8_Buffer[10];
     u32 pu32_Fields[1];
     u32 u32_HandsetMask = 0;
    

     LIST_RC rc;

     memset(&st_Entry, 0, sizeof(st_Entry));
     
     tcx_appClearScreen();


     printf("Enter Line Id...\n");
     tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
     st_Entry.u32_LineId = atoi(ps8_Buffer);

     printf("Enter OK Status (0 - 'down', 1 - 'registering', 2 - 'initializing', 3 - 'up')...\n");
     tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
     st_Entry.u32_OKStatus = atoi(ps8_Buffer);

     pu32_Fields[0] = FIELD_ID_OK_STATUS;

     st_Entry.u32_EntryId = LineDiagnosticGetEntryIdByLineId(st_Entry.u32_LineId);
     /* Update in Database */
     rc = List_UpdateEntry(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS, &st_Entry, pu32_Fields, 1, st_Entry.u32_EntryId);
     if(rc > 0)
        printf("List_UpdateEntry error\n");
     // Send Notification to the HS
     List_GetAttachedHs((u8)st_Entry.u32_LineId, &u32_HandsetMask);

     app_FacilityDiagnosticIndication(1, st_Entry.u32_LineId, CMBS_LINE_RELATED_CHANGE, (u16)u32_HandsetMask);
}

void keyb_ListAccess_SimulateDiagnosticError(void)
{
     stLineAndDiagnosticListEntry st_Entry;

     char ps8_Buffer[10];
     u32 pu32_Fields[5];
      u32 u32_HandsetMask = 0, u32_FieldCnt = 0;

     LIST_RC rc;

     memset(&st_Entry, 0, sizeof(st_Entry));

     tcx_appClearScreen();

     /* Create if not exist */
     List_CreateList(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS);

     printf("Enter Line Id... (\"none\" for system related)\n");
     tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
	 if (strcmp(ps8_Buffer, "none") == 0)
		 st_Entry.u32_LineId = CMBS_NO_LINE_DEFINED;
	 else
	 	st_Entry.u32_LineId = atoi(ps8_Buffer);

     st_Entry.u32_EntryId = LineDiagnosticGetEntryIdByLineId(st_Entry.u32_LineId);  

     printf("Enter OK Status (0 - 'down', 1 - 'registering', 2 - 'initializing', 3 - 'up')...\n");
     tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
     st_Entry.u32_OKStatus = atoi(ps8_Buffer);


     printf("Enter Diagnostic Error Status (0 - 'No error or no identified cause', 1 - 'Identified cause for the occuring error')...\n");
     tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
     st_Entry.b_DiagnosticErrorStatus = atoi(ps8_Buffer);

     if (st_Entry.b_DiagnosticErrorStatus == TRUE)
     {
         printf("Enter Diagnostic Error Type (0 - 'Unknown error', 1 - 'Network error', 2 - 'Local error') ...\n");
         tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
         st_Entry.u32_DiagnosticErrorType = atoi(ps8_Buffer);
         
         if(st_Entry.u32_DiagnosticErrorType == CMBS_LINE_DIAG_ERROR_UNKNOWN)
        {
             st_Entry.u32_DiagnosticErrorNumber = 0; //'Unknown' error Type demands 'Unknown' error Number
         }
         if(st_Entry.u32_DiagnosticErrorType == CMBS_LINE_DIAG_LOCAL_ERROR)
         {
            printf("Enter Local Diagnostic Error Number: \n");
            printf("'0' - Unknown local error \n");
            printf("'1' - LAN error \n");
            printf("'2' - LAN level 2 error (e.g. Ethernet error) \n");
            printf("'3' - LAN level 3 error (e.g. IP error) \n");
            printf("'4' - Cable error  \n");
            printf("'5' - Modem or Home Gateway error \n");

            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_Entry.u32_DiagnosticErrorNumber = atoi(ps8_Buffer);
         }
         if(st_Entry.u32_DiagnosticErrorType == CMBS_LINE_DIAG_NETWORK_ERROR)
         {
            printf("Enter Network Diagnostic Error Number: \n");
            printf("'0' - Unknown Network error \n");
            printf("'1' - WAN error \n");

            tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
            st_Entry.u32_DiagnosticErrorNumber = atoi(ps8_Buffer);
         }
         
     }
     else
     {
         // No error or no identified cause   
         st_Entry.u32_DiagnosticErrorType = 0;
         st_Entry.u32_DiagnosticErrorNumber = 0;
     }
     
     pu32_Fields[u32_FieldCnt++] = FIELD_ID_OK_STATUS;
     pu32_Fields[u32_FieldCnt++] = FIELD_ID_DIAGNOSTIC_ERROR_STATUS;
//     pu32_Fields[u32_FieldCnt++] = FIELD_ID_DIAGNOSTIC_ERROR_TYPE;
//     pu32_Fields[u32_FieldCnt++] = FIELD_ID_DIAGNOSTIC_ERROR_NUMBER;
     
      /* Update in Database */
      rc = List_UpdateEntry(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS, &st_Entry, pu32_Fields, u32_FieldCnt, st_Entry.u32_EntryId);
      if(rc > 0)
        printf("List_UpdateEntry error\n");

      // Send Notification to the HS
      if (st_Entry.u32_LineId != CMBS_NO_LINE_DEFINED)
	  	List_GetAttachedHs((u8)st_Entry.u32_LineId, &u32_HandsetMask);
	  else
	  	u32_HandsetMask = CMBS_ALL_HS_MASK;

      app_FacilityDiagnosticIndication(1,  st_Entry.u32_LineId, CMBS_LINE_RELATED_CHANGE, (u16)u32_HandsetMask);
}

void keyb_ListAccess_TriggerLineAndDiagnosticListChange (void)
{

    u8 u8_Ans;

    tcx_appClearScreen();

    printf("Line And Diagnostic List change simulation\n");
    printf("=====================\n\n");
    printf("select option:\n");
    printf("1: OK Status\n");
    printf("2: Diagnostic Error\n");
    
    printf("\n");

    u8_Ans = tcx_getch();

    switch (u8_Ans)
    {
    case '1':
        keyb_ListAccess_SimulateOKStatus();
        break;

    case '2':
        keyb_ListAccess_SimulateDiagnosticError();
        break;
        
    default:
        printf("\nError parameter !\n");
        return;
        
        }

}
void Keyb_ListAccess_WriteOutgoingSMS (void)
{
    char ps8_Buffer[10];
    stSMSListEntry         st_SMSEntry;
    u32 pu32_Fields[8], u32_FieldsNum = 8, u32_EntryId;
    
    time_t t_Time;

    memset(&st_SMSEntry, 0, sizeof(st_SMSEntry));
    
    tcx_appClearScreen();

    /* Create if not exist */
    List_CreateList(LIST_TYPE_OUTGOING_SMS);

    printf("Enter Number...\n");
    tcx_gets(st_SMSEntry.sNumber, sizeof(st_SMSEntry.sNumber));

    printf("Enter Name...\n");
    tcx_gets(st_SMSEntry.sName, sizeof(st_SMSEntry.sName));

    time(&t_Time);
    printf("Using current time as Date and Time...\n");
    st_SMSEntry.t_DateAndTime = t_Time;

    printf("Enter SMS ID...\n");
    tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
    st_SMSEntry.u32_SMSServiseId = atoi(ps8_Buffer);

    printf("Enter Network side SMS character encoding value...\n");
    tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
    st_SMSEntry.u32_NetworkSideEncoding = atoi(ps8_Buffer);

    printf("Enter encoding variant #1...\n");
    tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
    st_SMSEntry.u32_EncVariant_1 = atoi(ps8_Buffer);

    printf("Enter encoding variant #2...\n");
    tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
    st_SMSEntry.u32_EncVariant_2 = atoi(ps8_Buffer);

    printf("Enter SMS content...\n");
    tcx_gets(st_SMSEntry.sSMSContent, sizeof(st_SMSEntry.sSMSContent));

    st_SMSEntry.u32_SMSSize = strlen((char *)st_SMSEntry.sSMSContent);

    st_SMSEntry.b_SendRequest = FALSE;
  
     pu32_Fields[0] = FIELD_ID_NUMBER;
     pu32_Fields[1] = FIELD_ID_NAME;            
     pu32_Fields[2] = FIELD_ID_DATE_AND_TIME;
     pu32_Fields[3] = FIELD_ID_SMS_IDENTIFIER;
     pu32_Fields[4] = FIELD_ID_SMS_NETWORK_ENCODING;
     pu32_Fields[5] = FIELD_ID_SMS_SIZE;
     pu32_Fields[6] = FIELD_ID_SMS_CONTENT;
     pu32_Fields[7] = FIELD_ID_SMS_SEND_REQUEST;
     
     List_InsertEntry(LIST_TYPE_DRAFT_SMS, &st_SMSEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);
     printf("Entry inserted. Entry Id = %d\n", u32_EntryId);     
     ListChangeNotif_SMSListChanged( st_SMSEntry.u32_SMSServiseId, LIST_TYPE_DRAFT_SMS, CMBS_ALL_RELEVANT_HS_ID);
}

void keyb_ListAccess_DisplaySMSList(void)
{
    u32 u32_Index, u32_Count, pu32_Fields[FIELD_ID_MAX], u32_FieldsSize, u32_NumOfEntries = 1;
    u8 pu8_Entry[LIST_ENTRY_MAX_SIZE];
    u16 u16_ListType;
    stSMSListEntry st_Entry;
    struct tm *pst_Time;
	tcx_appClearScreen();
    memset(&st_Entry, 0, sizeof(st_Entry));



    pu32_Fields[0] = FIELD_ID_ENTRY_ID;
    pu32_Fields[1] = FIELD_ID_NUMBER;
    pu32_Fields[2] = FIELD_ID_NAME;
    pu32_Fields[3] = FIELD_ID_DATE_AND_TIME;
    pu32_Fields[4] = FIELD_ID_SMS_IDENTIFIER;
    pu32_Fields[5] = FIELD_ID_SMS_SIZE;
    pu32_Fields[6] = FIELD_ID_SMS_CONTENT;            
    pu32_Fields[7] = FIELD_ID_SMS_TYPE;            
    for (u16_ListType= LIST_TYPE_INCOMING_SMS; u16_ListType <= LIST_TYPE_DRAFT_SMS; u16_ListType++ )
    {
        List_GetCount((LIST_TYPE)u16_ListType, &u32_Count);
        printf("Total Num Of Entries in %s list is %d\n", keyb_dbg_GetListTypeName(u16_ListType), u32_Count);
        if(u32_Count > 0)
        {
            switch (u16_ListType)
            {
                case LIST_TYPE_INCOMING_SMS:
                    pu32_Fields[8] = FIELD_ID_READ_STATUS;            
                    u32_FieldsSize = 9;
                    break;
                case LIST_TYPE_SENT_SMS:
                case LIST_TYPE_OUTGOING_SMS:                    
                    pu32_Fields[8] = FIELD_ID_SMS_NETWORK_ENCODING;                                
                    u32_FieldsSize = 9;                    
                    break;
                case LIST_TYPE_DRAFT_SMS:
                    pu32_Fields[8] = FIELD_ID_SMS_NETWORK_ENCODING;    
                    pu32_Fields[9] = FIELD_ID_SMS_SEND_REQUEST;                        
                    u32_FieldsSize = 10;                                        
        			break;
           }
           for (u32_Index = 0; u32_Index < u32_Count; ++u32_Index)
        	{
                u32 u32_StartIdx = u32_Index + 1;
                List_ReadEntries((LIST_TYPE)u16_ListType, &u32_StartIdx, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsSize,
                                 FIELD_ID_ENTRY_ID, FIELD_ID_INVALID, pu8_Entry, &u32_NumOfEntries);
                pst_Time = localtime(&(((stSMSListEntry *)pu8_Entry)->t_DateAndTime));

                printf("Entry #%d of %d:\n", u32_Index + 1, u32_Count);
                printf("===================\n");
                printf("SMS Type = %s\n", keyb_dbg_GetListTypeName(((stSMSListEntry *)pu8_Entry)->cSMSType));
                printf("Entry Id\t= %d\n", ((stSMSListEntry *)pu8_Entry)->u32_EntryId);
                printf("Number\t = %s\n", ((stSMSListEntry *)pu8_Entry)->sNumber);
                printf("Name\t = %s\n", ((stSMSListEntry *)pu8_Entry)->sName);
                printf("Time - Year   = %d\n", pst_Time->tm_year + 1900);
                printf("Time - Month  = %d\n", pst_Time->tm_mon + 1);
                printf("Time - Day    = %d\n", pst_Time->tm_mday);
                printf("Time - Hour   = %d\n", pst_Time->tm_hour);
                printf("Time - Min    = %d\n", pst_Time->tm_min);
                printf("Time - Sec    = %d\n", pst_Time->tm_sec);                 
                printf("SMS Content\t= %s\n", ((stSMSListEntry *)pu8_Entry)->sSMSContent);
                printf("**********************************************\n");
                printf("\n");
            }
        }
    }
}

void keyb_ListAccess_TriggerSMSSendReq(void)
{
    u32 u32_Index, u32_Count, pu32_Fields[FIELD_ID_MAX], u32_FieldsSize, u32_NumOfEntries = 1, u32_FieldsNum, u32_EntryId;
    u8 pu8_Entry[LIST_ENTRY_MAX_SIZE];
    stSMSListEntry st_Entry;
    LIST_RC rc;

    memset(&st_Entry, 0, sizeof(st_Entry));
    tcx_appClearScreen();
    pu32_Fields[0] = FIELD_ID_ENTRY_ID;
    pu32_Fields[1] = FIELD_ID_NUMBER;
    pu32_Fields[2] = FIELD_ID_NAME;
    pu32_Fields[3] = FIELD_ID_DATE_AND_TIME;
    pu32_Fields[4] = FIELD_ID_SMS_IDENTIFIER;
    pu32_Fields[5] = FIELD_ID_SMS_SEND_REQUEST;
    pu32_Fields[6] = FIELD_ID_SMS_NETWORK_ENCODING;
    pu32_Fields[7] = FIELD_ID_SMS_SIZE;
    pu32_Fields[8] = FIELD_ID_SMS_CONTENT;            
    u32_FieldsSize = 9;

    List_GetCount(LIST_TYPE_DRAFT_SMS, &u32_Count);
    printf("Total Num Of Entries in SMS Draft list = %d\n", u32_Count);
    if (u32_Count>0)
    {
       u32 u32_SMSId;
        for (u32_Index = 0; u32_Index < u32_Count; ++u32_Index)
        {
            u32 u32_StartIdx = u32_Index + 1;
            List_ReadEntries(LIST_TYPE_DRAFT_SMS, &u32_StartIdx, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsSize,
                             FIELD_ID_ENTRY_ID, FIELD_ID_INVALID, pu8_Entry, &u32_NumOfEntries);
            printf("Entry #%d of %d:\n", u32_Index + 1, u32_Count);
            printf("===================\n");
            keyb_ListAccess_DumpEntry(LIST_TYPE_DRAFT_SMS, pu8_Entry);
            printf("**********************************************\n");
        }

        printf("Please provide entry ID of the message need to be sent\n");
        
        tcx_scanf("%d", &u32_EntryId);
        List_ReadEntryById(LIST_TYPE_DRAFT_SMS, u32_EntryId, pu32_Fields, u32_FieldsSize, &st_Entry);
        printf("Deleting entry # %d from Draft SMS list\n",u32_EntryId);

        u32_SMSId= GetSMSIdByEntryId(u32_EntryId, LIST_TYPE_DRAFT_SMS);
        rc = List_DeleteEntry(LIST_TYPE_DRAFT_SMS, u32_EntryId);          
        
        ListChangeNotif_SMSListChanged( u32_SMSId, LIST_TYPE_DRAFT_SMS, CMBS_ALL_RELEVANT_HS_ID);

        if ( rc != LIST_RC_OK )
        {
    	  printf("Entry #%d is not deleted from Draft SMS list\n", u32_EntryId);
        }
        else
        {
            printf("Entry #%d deleted from Draft SMS list\n", u32_EntryId);
            pu32_Fields[0] = FIELD_ID_NUMBER;
            pu32_Fields[1] = FIELD_ID_NAME;
            pu32_Fields[2] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[3] = FIELD_ID_SMS_IDENTIFIER;
            pu32_Fields[4] = FIELD_ID_SMS_NETWORK_ENCODING;
            pu32_Fields[5] = FIELD_ID_SMS_SIZE;
            pu32_Fields[6] = FIELD_ID_SMS_CONTENT;            
            u32_FieldsNum = 7;

            printf("Inserting entry to Outgoing SMS list\n");
            rc = List_InsertEntry(LIST_TYPE_OUTGOING_SMS, &st_Entry, pu32_Fields, u32_FieldsNum, &u32_EntryId);
            if ( rc != LIST_RC_OK )
        	    printf("Entry is not inserted\n");
            else
        	    printf("Entry inserted. Enrty ID %d\n", u32_EntryId);
    	}
    }
}
void keyb_ListAccess_SendSMS(void)
{
    u32 u32_Index, u32_Count, pu32_Fields[FIELD_ID_MAX], u32_FieldsSize, u32_NumOfEntries = 1, u32_EntryId;
    u8 pu8_Entry[LIST_ENTRY_MAX_SIZE], u8_Ans = 0;
    stSMSListEntry st_Entry;
    LIST_RC rc;
    memset(&st_Entry, 0, sizeof(st_Entry));
    pu32_Fields[0] = FIELD_ID_ENTRY_ID;
    pu32_Fields[1] = FIELD_ID_NUMBER;
    pu32_Fields[2] = FIELD_ID_NAME;
    pu32_Fields[3] = FIELD_ID_SMS_CONTENT;            
    u32_FieldsSize = 4;
    List_GetCount(LIST_TYPE_OUTGOING_SMS, &u32_Count);
    if (u32_Count > 0)
    {
        printf("Total Num Of Entries in SMS Outgoing list = %d\n", u32_Count);
        for (u32_Index = 0; u32_Index < u32_Count; ++u32_Index)
        {
            u32 u32_StartIdx = u32_Index + 1;
            List_ReadEntries(LIST_TYPE_OUTGOING_SMS, &u32_StartIdx, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsSize,
                             FIELD_ID_ENTRY_ID, FIELD_ID_INVALID, pu8_Entry, &u32_NumOfEntries);
            printf("Entry #%d of %d:\n", u32_Index + 1, u32_Count);
            printf("===================\n");
            printf("Entry Id\t= %d\n", ((stSMSListEntry *)pu8_Entry)->u32_EntryId);
            printf("Number\t = %s\n", ((stSMSListEntry *)pu8_Entry)->sNumber);
            printf("Name\t = %s\n", ((stSMSListEntry *)pu8_Entry)->sName);
            printf("SMS Content\t= %s\n", ((stSMSListEntry *)pu8_Entry)->sSMSContent);
            printf("**********************************************\n");
            printf("\n");
        }
        printf("Please provide entry ID of the message need to be sent\n");
        tcx_scanf("%d", &u32_EntryId);
        printf("Shall the sending to Network result in Success ('1') or Failure ('0') ?\n");

        u8_Ans = tcx_getch();
        SleepMs(2000);
        
        if (u8_Ans == '1')
        {
            pu32_Fields[0] = FIELD_ID_NUMBER;
            pu32_Fields[1] = FIELD_ID_NAME;
            pu32_Fields[2] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[3] = FIELD_ID_SMS_IDENTIFIER;
            pu32_Fields[4] = FIELD_ID_SMS_NETWORK_ENCODING;
            pu32_Fields[5] = FIELD_ID_SMS_SIZE;
            pu32_Fields[6] = FIELD_ID_SMS_CONTENT;            
            u32_FieldsSize = 7;
            List_ReadEntryById(LIST_TYPE_OUTGOING_SMS, u32_EntryId, pu32_Fields, u32_FieldsSize, &st_Entry);

            // Delete entry from Draft SMS list
            printf("Deleting entry # %d from Outgoing SMS list\n",u32_EntryId);
            
            rc = List_DeleteEntry(LIST_TYPE_OUTGOING_SMS, u32_EntryId);            
            if ( rc != LIST_RC_OK )
            {
        		printf("Entry #%d is not deleted from Outgoing SMS list\n", u32_EntryId);
         }
            
         printf("Inserting entry to Sent SMS list\n");
         rc = List_InsertEntry(LIST_TYPE_SENT_SMS, &st_Entry, pu32_Fields, u32_FieldsSize, &u32_EntryId);
         if ( rc != LIST_RC_OK )
        	printf("Entry is not inserted\n");
			
         else
         {
             ListChangeNotif_SMSListChanged( st_Entry.u32_SMSServiseId, LIST_TYPE_SENT_SMS, CMBS_ALL_RELEVANT_HS_ID);
         }
      }
      else
      {
            u32 u32_SMSServiseId;
			u32_SMSServiseId = GetSMSIdByEntryId(u32_EntryId, LIST_TYPE_OUTGOING_SMS);
            // SMS was not sent, keep it in Outgoing list, send list change notification (addition)
            ListChangeNotif_SMSListChanged( u32_SMSServiseId, LIST_TYPE_OUTGOING_SMS, CMBS_ALL_RELEVANT_HS_ID);            
       }
    }
}

void keyb_ListAccess_WriteIncomingSMS(void)
{
    char ps8_Buffer[10];
    stSMSListEntry         st_SMSEntry;
    u32 pu32_Fields[7], u32_FieldsNum = 7, u32_EntryId;
    time_t t_Time;
    memset(&st_SMSEntry, 0, sizeof(st_SMSEntry));
    tcx_appClearScreen();

    /* Create if not exist */
    List_CreateList(LIST_TYPE_INCOMING_SMS);

    printf("Enter Number...\n");
    tcx_gets(st_SMSEntry.sNumber, sizeof(st_SMSEntry.sNumber));
    printf("Enter Name...\n");
    tcx_gets(st_SMSEntry.sName, sizeof(st_SMSEntry.sName));

    time(&t_Time);
    printf("Using current time as Date and Time...\n");
    st_SMSEntry.t_DateAndTime = t_Time;
    printf("Enter SMS ID...\n");
    tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
    st_SMSEntry.u32_SMSServiseId = atoi(ps8_Buffer);
    printf("Enter SMS content...\n");
    tcx_gets(st_SMSEntry.sSMSContent, sizeof(st_SMSEntry.sSMSContent));
    st_SMSEntry.u32_SMSSize = strlen((char *)st_SMSEntry.sSMSContent);
    st_SMSEntry.b_Read = FALSE;
	pu32_Fields[0] = FIELD_ID_NUMBER;
    pu32_Fields[1] = FIELD_ID_NAME;            
    pu32_Fields[2] = FIELD_ID_DATE_AND_TIME;
    pu32_Fields[3] = FIELD_ID_READ_STATUS;     
    pu32_Fields[4] = FIELD_ID_SMS_IDENTIFIER;
    pu32_Fields[5] = FIELD_ID_SMS_SIZE;
    pu32_Fields[6] = FIELD_ID_SMS_CONTENT;
    List_InsertEntry(LIST_TYPE_INCOMING_SMS, &st_SMSEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);
    printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
    // Send SMS notification    
    ListChangeNotif_IncomingSMSListChanged(st_SMSEntry.u32_SMSServiseId, CMBS_SMS_NEW_ARRIVED, CMBS_ALL_RELEVANT_HS_ID);
}

void keyb_ListAccess_DeleteSMS(void)
{
    u32 u32_Index, u32_Count, pu32_Fields[FIELD_ID_MAX], u32_FieldsSize, u32_NumOfEntries = 1, u32_SMSId;
    u8 pu8_Entry[LIST_ENTRY_MAX_SIZE];
    u16 u16_ListType = 0;
    stSMSListEntry st_Entry;
    LIST_RC rc;
    bool b_Empty = TRUE;
    memset(&st_Entry, 0, sizeof(st_Entry));
    pu32_Fields[0] = FIELD_ID_ENTRY_ID;
    pu32_Fields[1] = FIELD_ID_SMS_CONTENT;       
    pu32_Fields[2] = FIELD_ID_SMS_TYPE;            
    u32_FieldsSize = 3;
    for (u16_ListType = LIST_TYPE_INCOMING_SMS; u16_ListType <= LIST_TYPE_DRAFT_SMS; u16_ListType++ )
    {
        List_GetCount((LIST_TYPE)u16_ListType, &u32_Count);
        printf("Total Num Of Entries in %s list is %d\n", keyb_dbg_GetListTypeName(u16_ListType), u32_Count);

        if(u32_Count > 0)
        {
            b_Empty = FALSE;
    		for (u32_Index = 0; u32_Index < u32_Count; ++u32_Index)
    		{
        		u32 u32_StartIdx = u32_Index + 1;
                List_ReadEntries((LIST_TYPE)u16_ListType, &u32_StartIdx, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsSize,
                                 FIELD_ID_ENTRY_ID, FIELD_ID_INVALID, pu8_Entry, &u32_NumOfEntries);

        		printf("Entry #%d of %d:\n", u32_Index + 1, u32_Count);
        		printf("===================\n");
                printf("SMS Type = %s\n", keyb_dbg_GetListTypeName(((stSMSListEntry *)pu8_Entry)->cSMSType));
                printf("Entry Id\t= %d\n", ((stSMSListEntry *)pu8_Entry)->u32_EntryId);
                printf("SMS Content\t= %s\n", ((stSMSListEntry *)pu8_Entry)->sSMSContent);
        		printf("**********************************************\n");
                printf("\n");
   			  }
        }
	}
    if(! b_Empty)
	{
        printf("Please provide list type ('0' - INCOMING, '1' - OUTGOING, '2' - DRAFT, '3' - SENT\n");

        tcx_scanf("%hu", &u16_ListType);

        switch(u16_ListType)
		{
        case SMS_TYPE_INCOMING:
            st_Entry.cSMSType = LIST_TYPE_INCOMING_SMS;
                break;
        case SMS_TYPE_OUTGOING:
            st_Entry.cSMSType = LIST_TYPE_OUTGOING_SMS;
                break;
        case SMS_TYPE_DRAFT:
            st_Entry.cSMSType = LIST_TYPE_DRAFT_SMS;
                break;
        case SMS_TYPE_SENT:
            st_Entry.cSMSType = LIST_TYPE_SENT_SMS;
                break;    
             default:
                break;
		}
        printf("Please provide entry ID of the message need to be deleted\n");
        tcx_scanf("%d", &st_Entry.u32_EntryId);

        printf("Deleting Entry # %d\n", st_Entry.u32_EntryId);

		u32_SMSId = GetSMSIdByEntryId(st_Entry.u32_EntryId, st_Entry.cSMSType);
        rc = List_DeleteEntry(st_Entry.cSMSType, st_Entry.u32_EntryId);
        if(rc > 0)
          printf("List_DeleteEntry error\n");
        else
        {
            printf("Entry id %d is deleted\n", st_Entry.u32_EntryId);
            if(st_Entry.cSMSType == LIST_TYPE_INCOMING_SMS)
                ListChangeNotif_IncomingSMSListChanged(u32_SMSId, CMBS_SMS_OTHER_LIST_CHANGE, CMBS_ALL_RELEVANT_HS_ID);                
            else
                ListChangeNotif_SMSListChanged( u32_SMSId,  st_Entry.cSMSType, CMBS_ALL_RELEVANT_HS_ID);           
        }
}

}

void keyb_ListAccess_SMSTestMenu(void)
{

    u8 u8_Ans = 0;

    tcx_appClearScreen();
    while (u8_Ans != 'q')
    {
	    printf("SMS Test menu\n");
	    printf("=====================\n\n");
	    printf("select option ('q' to exit menu):\n");
	    printf("1: Write Outgoing SMS Message (Entry will be saved in 'Draft' SMS List)\n");
	    printf("2: Trigger SMS Send request \n");
	    printf("(Entry will be deleted from 'Draft' SMS List and added to 'Outgoing' SMS List)\n");
	    printf("3: Simulate sending SMS to Network\n");
	    printf("4: Write Incoming SMS message\n");
	    printf("5: Delete SMS message\n");
	    printf("6: Display SMS message list\n");
    
     	printf("\n");

	    u8_Ans = tcx_getch();

	    switch (u8_Ans)
	    {
	    case '1':
	        Keyb_ListAccess_WriteOutgoingSMS();
	        break;

	    case '2':
	        keyb_ListAccess_TriggerSMSSendReq();
	        break;

	    case '3':
	        keyb_ListAccess_SendSMS();
	        break;

	    case '4':
	        keyb_ListAccess_WriteIncomingSMS();
	        break;

	    case '5':
	        keyb_ListAccess_DeleteSMS();
	        break;
        
	    case '6':
	        keyb_ListAccess_DisplaySMSList();
	        break;
                
	    }
  	}
}

void keyb_ListAccess(void)
{
    u8 u8_Ans;

    tcx_appClearScreen();

    printf("List Access Operation\n");
    printf("=====================\n\n");
    printf("select option:\n");
    printf("1: Insert Entry to List\n");
    printf("2: Delete List\n");
    printf("3: Display List contents\n");
    printf("4: Fill contact list with entries\n");
    printf("5: Add optional lists to list of supporeted list\n");
    printf("6: List Access Specific fields\n");
    printf("7: Simulate system events for Line And Diagnostic List Change\n");
    printf("8: SMS testing menu\n");
    
    printf("\n");

    u8_Ans = tcx_getch();

    switch (u8_Ans)
    {
    case '1':
        keyb_ListAccess_InsertEntry();
        break;

    case '2':
        keyb_ListAccess_DeleteList();
        break;

    case '3':
        keyb_ListAccess_DumpList();
        break;

    case '4':
        keyb_ListAccess_FillContactList();
        break;

    case '5':
        keyb_ListAccess_AddToSuppList();
        break;

    case '6':
        keyb_ListAccess_SpecificFields();
        break;
        
    case '7':
        keyb_ListAccess_TriggerLineAndDiagnosticListChange();
        break;
        
    case '8':
        keyb_ListAccess_SMSTestMenu();
        break;
        
    default:
        printf("\nError parameter !\n");
        return;
    }

    u8_Ans = tcx_getch();
    tcx_appClearScreen();
}

//		========== keyb_BGCalibrationSettings ===========
/*!
\brief				 Set Bandgap calibration arguments. DCIN & Resistor divider can be set other than SW defaults.
\param[in,out]		 u8_BG_DCIN - The DCIN index used for BG calibration
\return				<none>
*/

void keyb_BGCalibrationSettings(u8 u8_BG_DCIN)
{
    ST_APPCMBS_CONTAINER st_Container;
    u8 u8_BG_ResistorFactor = 0;
    u8 u8_Getch;
    bool ValidSelect = FALSE;


	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_AUXBGP_RESISTOR_FACTOR, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_BG_ResistorFactor = st_Container.ch_Info[0];

    do
    {
        //   	tcx_appClearScreen();
        printf("\n-----------------------------------------\n");
        printf("------ Aux BG Calibration Settings    -----\n");
        printf("-------------------------------------------\n");
        printf("Auxiliary Calibration DCIN  = DCIN%d\n", u8_BG_DCIN);
        printf("Auxiliary Calibration Resistor Factor  = %d\n", u8_BG_ResistorFactor);
        printf("-------------------------------------------\n");
        printf("a) Change Settings \n");
        printf("q) Keep Settings W/O changes \n");
        printf("\n Please Select!\n");

        u8_Getch = tcx_getch();
        switch (u8_Getch)
        {
        case 'a':
            printf("\nSelect DCIN input\n");
            printf("0) DCIN0 \n");
            printf("1) DCIN1 \n");
            printf("2) DCIN2 \n");
            printf("3) DCIN3 \n");
            do
            {
                printf("\nPlease select value between 0-3 \n");
                u8_Getch = tcx_getch();
            }while ((u8_Getch < '0') || (u8_Getch > '3'));
            u8_BG_DCIN = u8_Getch - '0';

            do
            {
                printf("\nPlease select Resistor Factor (1 - 4):\n");
                u8_Getch = tcx_getch();
            }while ((u8_Getch < '1') || (u8_Getch > '4'));
            u8_BG_ResistorFactor = u8_Getch - '0';


            //	u8 msgparserEnabled = app_get_msgparserEnabled();
            //	app_set_msgparserEnabled(0);

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_AUXBGP_DCIN, &u8_BG_DCIN, CMBS_PARAM_AUXBGP_DCIN_LENGTH, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_AUXBGP_RESISTOR_FACTOR, &u8_BG_ResistorFactor, CMBS_PARAM_AUXBGP_RESISTOR_FACTOR_LENGTH, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

            //	app_set_msgparserEnabled(msgparserEnabled);

            ValidSelect = TRUE;
            break;
        case 'q':
            ValidSelect = TRUE;
            break;
        default:
            break;
        }
    }while (!ValidSelect);

    SleepMs(100);
    printf("-------------------------------------------\n");
    printf("Auxiliary Calibration DCIN  = DCIN%d\n", u8_BG_DCIN);
    printf("Auxiliary Calibration Resistor Factor  = %d\n", u8_BG_ResistorFactor);
    printf("-------------------------------------------\n\n");

}

//		========== keyb_ChipSettingsCalibrate_BG ===========
/*!
\brief				 Calibration of BG
\param[in,out]		 <none>
\return				<none>
*/

void keyb_ChipSettingsCalibrate_BG(void)
{
    u8 u8_Value[2] = { 0 };
    char buffer[16] = { 0 };
    u32 ValueVREF = 0;
    ST_APPCMBS_CONTAINER st_Container;

    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    printf("Enter Supply Volt [mV]\n");
    tcx_gets(buffer, sizeof(buffer));
    ValueVREF = atoi(buffer);
    memcpy(u8_Value, &ValueVREF, CMBS_PARAM_AUXBGPROG_LENGTH);

    printf("\nRunning Calibration: Input supply %d.  vref_1v1_FromDCDC (11010)\n", ValueVREF);

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_AUXBGPROG, u8_Value, CMBS_PARAM_AUXBGPROG_LENGTH, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_SET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    if ( st_Container.ch_Info[0] != TRUE )
    {
        printf("BG calibration process failed. Difference between Input voltage and Read voltage is too big to compensate!!!\n\n");
    }


    app_set_msgparserEnabled(msgparserEnabled);
}



//		========== keyb_ChipSettingsCalibrate_RVBG ===========
/*!
\brief				 Calibration of RVBG
\param[in,out]		 <none>
\return				<none>
*/

void keyb_ChipSettingsCalibrate_RVBG(void)
{
    u8 quit = 0;

    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    do
    {
        u8 u8_Value;
        u8 u8_Index = 0;
        u8 u8_Max = 0x7F;
        ST_APPCMBS_CONTAINER st_Container;

        u8 const u8_AllowedValue[] =
        {
            0x14, /* -99 mV */0x17, /* -93 mV */0x16, /* -87 mV */0x1C, /* -84 mV */
            0x11, /* -81 mV */0x1F, /* -78 mV */0x10, /* -75 mV */0x24, /* -74 mV */
            0x1E, /* -72 mV */0x13, /* -69 mV */0x27, /* -68 mV */0x19, /* -66 mV */
            0x12, /* -63 mV */0x26, /* -62 mV */0x18, /* -60 mV */0x2C, /* -59 mV */
            0x15, /* -57 mV */0x21, /* -56 mV */0x1B, /* -54 mV */0x2F, /* -53 mV */
            0x20, /* -50 mV */0x34, /* -49 mV */0x1A, /* -48 mV */0x2E, /* -47 mV */
            0x1D, /* -45 mV */0x23, /* -44 mV */0x37, /* -43 mV */0x29, /* -41 mV */
            0x22, /* -38 mV */0x36, /* -37 mV */0x28, /* -35 mV */0x3C, /* -34 mV */
            0x25, /* -32 mV */0x31, /* -31 mV */0x2B, /* -29 mV */0x3F, /* -28 mV */
            0x30, /* -25 mV */0x4, /* -24 mV */0x2A, /* -23 mV */0x3E, /* -22 mV */
            0x2D, /* -20 mV */0x33, /* -19 mV */0x7, /* -18 mV */0x39, /* -16 mV */
            0x32, /* -13 mV */0x6, /* -12 mV */0x38, /* -10 mV */0xC, /*  -9 mV */
            0x35, /*  -7 mV */0x1, /*  -6 mV */0x3B, /*  -4 mV */0xF, /*  -3 mV */
            0x0, /*   0 mV */0x44, /*   1 mV */0x3A, /*   2 mV */0xE, /*   3 mV */
            0x3D, /*   5 mV */0x3, /*   6 mV */0x47, /*   7 mV */0x9, /*   9 mV */
            0x2, /*  12 mV */0x46, /*  13 mV */0x8, /*  15 mV */0x4C, /*  16 mV */
            0x5, /*  18 mV */0x41, /*  19 mV */0xB, /*  21 mV */0x4F, /*  22 mV */
            0x40, /*  25 mV */0x54, /*  26 mV */0xA, /*  27 mV */0x4E, /*  28 mV */
            0xD, /*  30 mV */0x43, /*  31 mV */0x57, /*  32 mV */0x49, /*  34 mV */
            0x42, /*  37 mV */0x56, /*  38 mV */0x48, /*  40 mV */0x5C, /*  41 mV */
            0x45, /*  43 mV */0x51, /*  44 mV */0x4B, /*  46 mV */0x5F, /*  47 mV */
            0x50, /*  50 mV */0x64, /*  51 mV */0x4A, /*  52 mV */0x5E, /*  53 mV */
            0x4D, /*  55 mV */0x53, /*  56 mV */0x67, /*  57 mV */0x59, /*  59 mV */
            0x52, /*  62 mV */0x66, /*  63 mV */0x58, /*  65 mV */0x6C, /*  66 mV */
            0x55, /*  68 mV */0x61, /*  69 mV */0x5B, /*  71 mV */0x6F, /*  72 mV */
            0x60, /*  75 mV */0x74, /*  76 mV */0x5A, /*  77 mV */0x6E, /*  78 mV */
            0x5D, /*  80 mV */0x63, /*  81 mV */0x77, /*  82 mV */0x69, /*  84 mV */
            0x62, /*  87 mV */0x76, /*  88 mV */0x68, /*  90 mV */0x7C, /*  91 mV */
            0x65, /*  93 mV */0x71, /*  94 mV */0x6B, /*  96 mV */0x7F, /*  97 mV */
            0x70, /* 100 mV */0x6A, /* 102 mV */0x7E, /* 103 mV */0x6D, /* 105 mV */
            0x73, /* 106 mV */0x79, /* 109 mV */0x72, /* 112 mV */0x78, /* 115 mV */
            0x75, /* 118 mV */0x7B, /* 121 mV */0x7A, /* 127 mV */0x7D, /* 130 mV */
        };

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RVBG, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_Value = st_Container.ch_Info[0];

        /*find index*/
        for (u8_Index = 0; u8_Index < (u8_Max + 1); u8_Index++)
        {
            if ( u8_Value == u8_AllowedValue[u8_Index] )
                break;
        }

        if ( u8_Index == (u8_Max + 1) )
        {
            /* Value was not found in the list. i.e. illegal value */
            /* so force to value that is in both lists */
            u8_Index = 0;
        }

        printf("\nUse < or > to adjust the value, 'q' to quit the menu:\n");
        printf("\nRVBG: 0x%X\n", u8_Value);

        switch (tcx_getch())
        {
        case '>':
            if ( u8_Index < u8_Max )
                u8_Index++;
            break;
        case '<':
            if ( u8_Index > 0 )
                u8_Index--;
            break;
        case 'q':
            quit = 1;
            break;
        }

        u8_Value = u8_AllowedValue[u8_Index];

        if ( !quit )
        {
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_RVBG, &u8_Value, sizeof(u8_Value), 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }

    }while (!quit);

    app_set_msgparserEnabled(msgparserEnabled);
}

//		========== keyb_ChipSettingsCalibrate_RVREF ===========
/*!
\brief				 Calibration of RVREF
\param[in,out]		 <none>
\return				<none>
*/
void keyb_ChipSettingsCalibrate_RVREF(void)
{
    u8 quit = 0;

    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    do
    {
        u8 u8_Value;
        u8 u8_Max = 0x3F;
        ST_APPCMBS_CONTAINER st_Container;

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RVREF, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_Value = st_Container.ch_Info[0];

        printf("\nUse < or > to adjust the value, 'q' to quit the menu:\n");
        printf("\nRVREF: 0x%X\n", u8_Value);

        switch (tcx_getch())
        {
        case '>':
            if ( u8_Value < u8_Max )
                u8_Value++;
            break;
        case '<':
            if ( u8_Value > 0 )
                u8_Value--;
            break;
        case 'q':
            quit = 1;
            break;
        }

        if ( !quit )
        {
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_RVREF, &u8_Value, sizeof(u8_Value), 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }

    }while (!quit);

    app_set_msgparserEnabled(msgparserEnabled);
}

//		========== keyb_ChipSettingsCalibrate_RXTUN ===========
/*!
\brief				 Calibration of RXTUN
\param[in,out]		 <none>
\return				<none>
*/
void keyb_ChipSettingsCalibrate_RXTUN(void)
{
    u8 quit = 0;
    u32 u32_GPIO = 0;

    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    // GPIO connect
    do
    {
        printf("Insert the GPIO (0x00 - 0x1B) to use for measurement (0xFF for None): ");
        tcx_scanf("%X", &u32_GPIO);

        if ( u32_GPIO <= 0x1B )
        {
            printf("Connecting GPIO 0x%X...\n", u32_GPIO);
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            cmbs_dsr_RxtunGpioConnect(g_cmbsappl.pv_CMBSRef, (u16)u32_GPIO);
            appcmbs_WaitForEvent(CMBS_EV_DSR_GPIO_CONNECT_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            quit = 1;
        }

        else if ( u32_GPIO == 0xFF )
            quit = 1;
    }while (!quit);

    // Value calibration
    quit = 0;
    do
    {
        u8 u8_Value;
        ST_APPCMBS_CONTAINER st_Container;

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RXTUN, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_Value = st_Container.ch_Info[0];

        printf("\nUse < or > to adjust the value, 'q' to quit the menu:\n");
        printf("\nRXTUN: 0x%X\n", u8_Value);

        switch (tcx_getch())
        {
        case '>':
            u8_Value++;
            break;
        case '<':
            u8_Value--;
            break;
        case 'q':
            quit = 1;
            break;
        }

        if ( !quit )
        {
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_RXTUN, &u8_Value, sizeof(u8_Value), 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }

    }while (!quit);

    printf("Disconnecting GPIO 0x%X...\n", u32_GPIO);
    cmbs_dsr_RxtunGpioDisconnect(g_cmbsappl.pv_CMBSRef);

    app_set_msgparserEnabled(msgparserEnabled);
}

//		========== keyb_ChipSettingsCalibrate ===========
/*!
\brief				 provides calibration menu
\param[in,out]		 <none>
\return				<none>
*/
void     keyb_ChipSettingsCalibrate(void)
{
    u8 quit = 0;

    do
    {
        u8 u8_RFPI[CMBS_PARAM_RFPI_LENGTH] = { 0 };
        bool isVegaOne = FALSE;
        u8 u8_RVBG = 0;
        u8 u8_RVREF = 0;
        u8 u8_RXTUN = 0;
        u8 u8_TestMode = CMBS_TEST_MODE_NORMAL;
        u8 u8_DectType = CMBS_DECT_TYPE_EU;
        u32 u32_BG = 0;
        u32 u32_ADC = 0;
        u32 u32_PMU = 0;
        u8  u8_AntSwitch = CMBS_ANT_SWITCH_NONE;
        u8 u8_BG_DCIN = 2; //DCIN2 is the default DCIN. But this is anyway updated from target side.
        ST_APPCMBS_CONTAINER st_Container;


        if ( appcmbs_get_CMBSAppInstance()->u8_HwChip ==  CMBS_HW_CHIP_VEGAONE )
            isVegaOne = TRUE;

        if ( !isVegaOne )
        {
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_AUXBGPROG, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(&u32_BG, st_Container.ch_Info,  CMBS_PARAM_AUXBGPROG_LENGTH);

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_ADC_MEASUREMENT, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(&u32_ADC, st_Container.ch_Info,  CMBS_PARAM_ADC_MEASUREMENT_LENGTH);

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_PMU_MEASUREMENT, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(&u32_PMU, st_Container.ch_Info,  CMBS_PARAM_PMU_MEASUREMENT_LENGTH);
        }
        else
        {
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_RVBG, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            u8_RVBG = st_Container.ch_Info[0];

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_RVREF, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            u8_RVREF = st_Container.ch_Info[0];
        }

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_AUXBGP_DCIN, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_BG_DCIN = st_Container.ch_Info[0];

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RXTUN, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_RXTUN = st_Container.ch_Info[0];

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_TEST_MODE, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_TestMode = st_Container.ch_Info[0];

        if ( u8_TestMode == CMBS_TEST_MODE_TBR6 )
        {
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_ANT_SWITCH_MASK, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            u8_AntSwitch = st_Container.ch_Info[0];
        }

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamGet(CMBS_PARAM_RFPI, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        memcpy(u8_RFPI, st_Container.ch_Info,  CMBS_PARAM_RFPI_LENGTH);

        // get current DECT Type
        appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_ProductionParamGet(CMBS_PARAM_DECT_TYPE, CMBS_PARAM_DECT_TYPE_LENGTH);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u8_DectType = st_Container.ch_Info[0];


        tcx_appClearScreen();
        printf("\n-------------------------\n");
        printf("-- CALIBRATION MENU    --\n");
        printf("-------------------------\n");
        printf("Select Parameter:\n");
        if ( !isVegaOne )
        {
            printf("DCIN%d ADC VALUE: \t%d mV\n", u8_BG_DCIN, u32_ADC);
            printf("PMU VALUE:       \t%d mV\n", u32_PMU);
            printf("a) BG_Calibrate: \t0x%X\n", u32_BG);
        }
        else
        {
            printf("a) RVBG:     \t%X\n", u8_RVBG);
            printf("b) RVREF:    \t%X\n", u8_RVREF);
        }
        printf("c) RXTUN:    \t%X\n", u8_RXTUN);
        printf("d) RFPI:     \t%.2X%.2X%.2X%.2X%.2X\n", u8_RFPI[0], u8_RFPI[1], u8_RFPI[2], u8_RFPI[3], u8_RFPI[4]);
        printf("e) TestMode: \t");
        switch (u8_TestMode)
        {
        case CMBS_TEST_MODE_NORMAL:
            printf("None\n"); break;
        case CMBS_TEST_MODE_TBR6:
            printf("TBR6\n"); break;
        case CMBS_TEST_MODE_TBR10:
            printf("TBR10\n"); break;
        }

        // Antenna switch for TBR6
        if ( u8_TestMode == CMBS_TEST_MODE_TBR6 )
        {
            printf("n) Antenna:\t%X (%s)\n", u8_AntSwitch, u8_AntSwitch ? (u8_AntSwitch == CMBS_ANT_SWITCH_ANT0 ? "Antenna 1" : "Antenna 2") : "None");
        }

        printf("f) DECT Type: \t0x%X %s\n\n", u8_DectType, _DectType2Str(u8_DectType));

        printf("s) Start ATE tests\n");
        printf("u) Stop ATE tests\n\n");

        printf("x) EEPROM set\n");
        printf("z) EEPROM reset\n\n");

        printf("r) Reboot target\n\n");

        printf("q) Quit\n");

        switch (tcx_getch())
        {
        case 'a':
            if ( !isVegaOne )
            {
                keyb_BGCalibrationSettings(u8_BG_DCIN);
                keyb_ChipSettingsCalibrate_BG();
            }
            else
            {
                keyb_ChipSettingsCalibrate_RVBG();
            }
            break;
        case 'b':
            keyb_ChipSettingsCalibrate_RVREF();
            break;
        case 'c':
            keyb_ChipSettingsCalibrate_RXTUN();
            break;
        case 'd':
            keyb_RFPISet();
            break;
        case 'e':
            {
                u8 u8_Value = CMBS_TEST_MODE_NORMAL;

                if ( u8_TestMode == CMBS_TEST_MODE_NORMAL )
                    u8_Value = CMBS_TEST_MODE_TBR6;
                else if ( u8_TestMode == CMBS_TEST_MODE_TBR6 )
                    u8_Value = CMBS_TEST_MODE_TBR10;

				appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
                app_SrvParamSet(CMBS_PARAM_TEST_MODE, &u8_Value, sizeof(u8_Value), 1);
                appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

                // timeout to apply changes on target side
                SleepMs(3000);

				appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
                app_SrvSystemReboot();

				appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

                break;
            }
        case 'n':
            {
                u8 u8_Value = CMBS_ANT_SWITCH_NONE;
                if ( u8_AntSwitch == CMBS_ANT_SWITCH_NONE )
                    u8_Value = CMBS_ANT_SWITCH_ANT0;
                else if ( u8_AntSwitch == CMBS_ANT_SWITCH_ANT0 )
                    u8_Value = CMBS_ANT_SWITCH_ANT1;

				appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
                app_SrvParamSet(CMBS_PARAM_ANT_SWITCH_MASK, &u8_Value, sizeof(u8_Value), 1);
                appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

                SleepMs(3000);

                break;
            }
            break;
        case 'f':
            keyb_DectTypeSet();
            break;
        case 's':
            keyb_StartATETest();
            break;
        case 'u':
            keyb_LeaveATETest();
            break;
        case 'x':
            keyb_ParamFlexSet();
            break;
        case 'z':
            keyb_EEPROMReset();
            break;
        case 'r':
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvSystemReboot();

			appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            break;
        case 'q':
            quit = 1;
            break;
        }

    }while (!quit);
}


//		========== keypb_EEPromParamSet ===========
/*!
\brief         Handle EEProm Settings set
\param[in,ou]  <none>
\return

*/
void  keypb_EEPromParamSet(void)
{
    char buffer[16];
    int i = 0;
    int j = 0;
    u8 u8_Getch;

    // get current settings
    //keyb_ChipSettingsGet();

    // show menu
    printf("\nSelect Parameter:\n");
    printf("1 => RFPI\n");
    printf("2 => PIN\n");
    printf("3 => Chipset parameters\n");
    printf("4 => RXTUN GPIO connect\n");
    printf("5 => RXTUN GPIO disconnect\n");
    printf("6 => Test mode\n");
    printf("7 => Flex EEprom set\n");
    printf("8 => PORBGCFG\n");
    printf("9 => AUXBGPROG\n");
    printf("0 => Reset EEprom\n");
    printf("* => Overnight EEPROM reset test\n");
    printf("a => HAN next TPUI\n");
    printf("i => Pream Normal\n");
    printf("j => Full Power\n");
    printf("k => Low Power\n");
    printf("l => Lowest Power\n");
    printf("m => RF19APU MLSE\n");
    printf("n => RF19APU KCALOVR\n");
    printf("o => RF19APU_KCALOVR_LINEAR\n");
    printf("p => RF19APU Support FCC\n");
    printf("Q => RF19APU Deviation\n");
    printf("r => RF19APU PA2 compatibility\n");
    printf("s => RFIC Selection\n");
    printf("t => MAX usable RSSI\n");
    printf("u => Lower RSSI Limit\n");
    printf("v => PHS Scan Parameter\n");
    printf("w => L1 - minus 82 dBm RSSI threshold for Japan regulation\n");
    printf("z => L2 - minus 62 dBm RSSI threshold for Japan regulation\n");
    printf("x => Chipset Tunes\n");
    printf("y => Maximum number of call per HS\n");
    printf("B => Side Tone Volume\n");
    printf("S => SYPO settings\n");
    printf("U => UART delay configuration\n");
    printf("C => Call Waiting disable per handset\n");
    printf("M => Clock Master Editability\n");
    printf("R => Prepare SST flash for target reset by host (RSTN)\n");
    printf("O => Load EEPROM from file\n");
	printf("\n-------------------------------\n");
	printf("q => Return\n");

    u8_Getch = tcx_getch();
    switch (u8_Getch)
    {
    case '1':
        keyb_RFPISet();
        break;

    case '2':
        printf("\n");
        printf("New PIN Code (4-digit): ");
        tcx_gets(buffer, sizeof(buffer));

        /* change to FFFFxxxx form (Expected by app_SrvPINCodeSet) */
        for (i = 0; i < 4; ++i)
        {
            buffer[i + 4] = buffer[i];
            buffer[i] = 'f';
        }
        buffer[8] = 0;

        app_SrvPINCodeSet(buffer);
        printf("Press Any Key!\n");
        tcx_getch();
        break;

    case '3':
        keyb_ChipSettingsSet();
        break;

    case '4':
        keyb_RxtnGpioConnect();
        break;

    case '5':
        keyb_RxtnGpioDisconnect();
        break;

    case '6':
        {
            u8 u8_Value;

            printf("New Test mode value (hex, 00=>disabled, 81=>TBR6, 82=>TBR10): ");

            memset(buffer, 0, sizeof(buffer));
            tcx_gets(buffer, sizeof(buffer));

            if ( strlen(buffer) )
            {
                u8_Value = app_ASC2HEX(buffer);
				appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
                app_SrvParamSet(CMBS_PARAM_TEST_MODE, &u8_Value, sizeof(u8_Value), 1);
                appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

                // timeout to apply changes on target side
                SleepMs(3000);
            }
        }
        break;

    case '7':
        keyb_ParamFlexSet();
        break;

    case '8':
        {
            u8 u8_Value = 0;
            u32 u32_Value;

            printf("\n");
            printf("New value for PORBGCFG: ");
            tcx_scanf("%X", &u32_Value);
            u8_Value = (u8)u32_Value;

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_PORBGCFG, &u8_Value, sizeof(u8_Value), 1);

            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }
        break;

    case '9':
        {
            ST_APPCMBS_CONTAINER st_Container;
            char auxbg_buffer[10];
            char auxbg_buffer_hex[CMBS_PARAM_AUXBGPROG_DIRECT_LENGTH];

            printf("\n");
            printf("New value for AUXBGPROG (2 characters [1 byte]. Example 66): ");
            tcx_gets(auxbg_buffer, sizeof(auxbg_buffer));

            for (i = 0, j = 0; i < CMBS_PARAM_AUXBGPROG_DIRECT_LENGTH * 2; i += 2)
            {
                auxbg_buffer_hex[j] = app_ASC2HEX(auxbg_buffer + i);
                j++;
            }
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_AUXBGPROG_DIRECT, (u8 *)auxbg_buffer_hex, sizeof(auxbg_buffer_hex), 1);

            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_SET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }
        break;

    case '*':
        {
            u32 u32_NumOfTests = 0, u32_Current = 0;
            printf("\nEEPROM reset overnight tests");
            printf("\nPlease enter number of tests... (max 1000)");
            tcx_gets(buffer, sizeof(buffer));
            u32_NumOfTests = (u32)atoi(buffer);
            u32_NumOfTests = MIN(MAX_OVERNIGHT_TESTS, u32_NumOfTests);
            while (++u32_Current <= u32_NumOfTests)
            {
                printf("\nRunning test # %d out of requested %d...", u32_Current, u32_NumOfTests);
                keyb_EEPROMReset();
            }

            printf("\nCompleted. # of tests performed = %d. Hit any key to continue...\n", u32_NumOfTests);
            tcx_getch();
        }
        break;

    case '0':
        {
            keyb_EEPROMReset();
        }
        break;

    case 'a':
        {

            ST_APPCMBS_CONTAINER st_Container;
            u8 buff[16] = { 0 };
            u8 buff_hex[CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH] = { 0 };
            u8 u8_Value[CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH] = { 0 };
            u8 i, j = 0;

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(&u8_Value[0], st_Container.ch_Info,  CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH);
            printf("Current TPUI: \t %.2X%.2X%.2X\n", u8_Value[0], u8_Value[1], u8_Value[2]);

            printf("\n");
            printf("HAN Next TPUI (6-digit): ");
            tcx_gets((char *)buff, sizeof(buff));

            for (i = 0; i < CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH * 2; i += 2)
            {
                buff_hex[j] = app_ASC2HEX((char *)buff + i);
                j++;
            }

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, buff_hex, CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

        }
        break;

    case 'B':
        {
            ST_APPCMBS_CONTAINER st_Container;
            u8 buff[16] = { 0 };
            u8 buff_hex[CMBS_PARAM_DAC_VOL_LENGTH] = { 0 };
            u8 u8_Value[CMBS_PARAM_DAC_VOL_LENGTH] = { 0 };
            u8 i, j = 0;

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_DAC1_VOL, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(&u8_Value[0], st_Container.ch_Info,  CMBS_PARAM_DAC_VOL_LENGTH);
            printf("Current DAC1 Volume (in Hex): \t %.2X%.2X\n", u8_Value[0], u8_Value[1]);

            printf("\n");
            printf("New DAC1 Volume(4 nibbles in Hex):");
            tcx_gets((char *)buff, sizeof(buff));

            for (i = 0; i < CMBS_PARAM_DAC_VOL_LENGTH * 2; i += 2)
            {
                buff_hex[j] = app_ASC2HEX((char *)buff + i);
                j++;
            }

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_DAC1_VOL, buff_hex, CMBS_PARAM_DAC_VOL_LENGTH, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_DAC2_VOL, buff_hex, CMBS_PARAM_DAC_VOL_LENGTH, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }
        break;

    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'Q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'z':
        {
            u8 u8_Value = 0;
            u32 u32_Value;
            E_CMBS_PARAM e_cmbsParam = CMBS_PARAM_UNKNOWN;

            switch (u8_Getch)
            {
            case 'i':
                e_cmbsParam = CMBS_PARAM_PREAM_NORM;
                break;
            case 'j':
                e_cmbsParam = CMBS_PARAM_RF_FULL_POWER;
                break;
            case 'k':
                e_cmbsParam = CMBS_PARAM_RF_LOW_POWER;
                break;
            case 'l':
                e_cmbsParam = CMBS_PARAM_RF_LOWEST_POWER;
                break;
            case 'm':
                e_cmbsParam = CMBS_PARAM_RF19APU_MLSE;
                break;
            case 'n':
                e_cmbsParam = CMBS_PARAM_RF19APU_KCALOVR;
                break;

            case 'o':
                e_cmbsParam = CMBS_PARAM_RF19APU_KCALOVR_LINEAR;
                break;
            case 'p':
                e_cmbsParam = CMBS_PARAM_RF19APU_SUPPORT_FCC;
                break;
            case 'Q':
                e_cmbsParam = CMBS_PARAM_RF19APU_DEVIATION;
                break;
            case 'r':
                e_cmbsParam = CMBS_PARAM_RF19APU_PA2_COMP;
                break;
            case 's':
                e_cmbsParam = CMBS_PARAM_RFIC_SELECTION;
                break;
            case 't':
                e_cmbsParam = CMBS_PARAM_MAX_USABLE_RSSI;
                break;
            case 'u':
                e_cmbsParam = CMBS_PARAM_LOWER_RSSI_LIMIT;
                break;
            case 'v':
                e_cmbsParam = CMBS_PARAM_PHS_SCAN_PARAM;
                break;
            case 'w':
                e_cmbsParam = CMBS_PARAM_JDECT_LEVEL1_M82;
                break;
            case 'z':
                e_cmbsParam = CMBS_PARAM_JDECT_LEVEL2_M62;
                break;
            }

            printf("\n");
            printf("New value (in Hex): ");
            tcx_scanf("%X", &u32_Value);
            u8_Value = (u8)u32_Value;

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(e_cmbsParam, &u8_Value, sizeof(u8_Value), 1);

            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }
        break;

    case 'x':
        keyb_ChipSettingsCalibrate();
        break;

    case 'y':
        {
            u8 u8_Value = 0;
            u32 u32_Value;

            printf("\n");
            printf("New value for max number of call per HS is: ");
            tcx_scanf("%X", &u32_Value);
            u8_Value = (u8)u32_Value;

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_MAX_NUM_ACT_CALLS_PT, &u8_Value, sizeof(u8_Value), 1);

            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }
        break;

    case 'S':
        keyb_SYPOSettingsSet();
        break;

    case 'U':
        {
            u8 u8_Value = 0;
            u32 u32_Value;

            printf("\n");
            printf("Please enter new value for UART delay (in resolution of 10msec, i.e if value = 2 => delay = 20msec): \n");
            tcx_scanf("%d", &u32_Value);
            u8_Value = (u8)u32_Value;

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_UART_DELAY_TIMER, &u8_Value, sizeof(u8_Value), 1);

            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        }
        break;

    case 'C':
        {
            u32 u16_HsMask = 0;

            printf("Enter handset mask CW disable (Hex u16): ");
            tcx_scanf("%x", &u16_HsMask);            

            app_SrvCWDisable(u16_HsMask);
        }

        break;
        
    case 'M':
        {
            u8 buff_hex[CMBS_PARAM_CLOCK_MASTER_EDIT_LENGTH] = { 0 };

            printf("\n Clock Master Editability (enable = 1; Disable = 0): ");
            
            switch (tcx_getch())
            {
              case '1':
                buff_hex[0] = 1;
                app_SrvParamSet(CMBS_PARAM_CLOCK_MASTER_EDIT, buff_hex, CMBS_PARAM_CLOCK_MASTER_EDIT_LENGTH, 1);
                break;
                
              case '0':
                buff_hex[0] = 0;
                app_SrvParamSet(CMBS_PARAM_CLOCK_MASTER_EDIT, buff_hex, CMBS_PARAM_CLOCK_MASTER_EDIT_LENGTH, 1);
                break;
                
              default:
                printf("\n wrong input !!! \n");
              break;                        
            }

        }

        break;
        
    case 'O':
        keypb_SetEEPROMFromFile();
        break;
		
    case 'R':
        keyb_PrepareSSTFlashForReset();
        break;

	case 'q':
		return;
    }
}

//		========== keypb_ProductionParamSet ===========
/*!
\brief         Handle Production Settings set
\param[in,ou]  <none>
\return

*/
void  keypb_ProductionParamSet(void)
{
    printf("\nSelect EEProm Param:\n");
    printf("1 => ECO mode\n");
    printf("2 => Set DECT Type(Japan, ...)\n");
	printf("\n-------------------------------\n");
	printf("q => Return\n");

    switch (tcx_getch())
    {
    case '1':
        keyb_ECOModeSet();
        break;
    case '2':
        keyb_DectTypeSet();
        break;
        break;

	case 'q':
    default:
        return;
    }
}


//		========== keypb_FxsParamSet ===========
/*!
\brief         Handle FXS Settings set
\param[in,ou]  <none>
\return

*/
void  keypb_FxsParamSet(void)
{

    E_CMBS_PARAM e_cmbsParam;
    u32 u32_Value;
    u8 u8_Value;
    s32 s32_getch;
	bool Request_already_sent = FALSE;
	bool IsToneConfig = FALSE;

    printf("\nSelect FXS Param:\n");
    printf("1 => FXS_CALLER_REGRET_TIME \n");
    printf("2 => FXS_CALLEE_REGRET_TIME \n");
    printf("3 => FXS_FIRST_DIGIT_TIMER \n");
    printf("4 => FXS_INTER_DIGIT_TIMER \n");
    printf("5 => FXS_STAR_HASH_CON_TIMER \n");
    printf("6 => FXS Simple TONE_CONFIG \n");
    printf("7 => FXS Composed TONE_CONFIG \n");

    s32_getch = tcx_getch();

   IsToneConfig = ((s32_getch == '6') ||(s32_getch == '7')) ;
   	
	
   if (! IsToneConfig)
   {
	printf("Please enter timer value in seconds: (Limited to 255 seconds)");
    	tcx_scanf("%d", &u32_Value);
    	u8_Value = (u8)u32_Value;
    }

    switch (s32_getch)
    {
        case '1':
            printf("This param is not sent to Target. It is responsibility of Host to handle \n");
            return;
        case '2':
            e_cmbsParam = CMBS_PARAM_FXS_CALLEE_REGRET_TIME;
            break;
        case '3':
            e_cmbsParam = CMBS_PARAM_FXS_FIRST_DIGIT_TIMER;
            break;
        case '4':
            e_cmbsParam = CMBS_PARAM_FXS_INTER_DIGIT_TIMER;
            break;
	 case '5':
            e_cmbsParam = CMBS_PARAM_FXS_STAR_HASH_CON_TIMER;
            break;
	 case '6':
            e_cmbsParam = CMBS_PARAM_FXS_TONE_CONFIG;
			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
		keypb_FxsToneConfig(TRUE);
		Request_already_sent = TRUE;
            break;
	 case '7':
	 	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            e_cmbsParam = CMBS_PARAM_FXS_TONE_CONFIG;
		keypb_FxsToneConfig(FALSE);
		Request_already_sent = TRUE;
            break;

        default:
        {
            printf("Param not supported \n");
            return;
        }
    
    }

	if (!Request_already_sent)	// For CMBS_PARAM_FXS_TONE_CONFIG case - Set request already sent 
	{
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
		app_FxsParamSet(e_cmbsParam, &u8_Value, sizeof(u8_Value), 1);
	}
	
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

}

// Is SimpleTone: Is it configuration of simple tone or of composed (complex) tone
void  keypb_FxsToneConfig(bool IsSimpleTone)
{
	ST_IE_FXS_TONE_CFG ToneCfg;

	if (IsSimpleTone)														// Example of Simple tone
	{
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_Id, (u16) CMBS_FXS_TONE_DIAL);		// Main tone is Dial Tone
		cfr_ie_ser_u16((u8 *) &ToneCfg.loop, 0);
		cfr_ie_ser_u16((u8 *) &ToneCfg.Counter, 1);								// Only 1 simple tone
		
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_Id1, CMBS_FXS_TONE_DIAL);			// Our Simple tone is Dial Tone
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_type1, FXS_TONE_NEW_CONFIGURATION);
		
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.level1A, -30);		// different from default 
	    	cfr_ie_ser_u16((u8 *) & ToneCfg.level1B, -30);  
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq1A, 480);  		// different from default 
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq1B, 620); 
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence1On, 1000); 
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence1Off, 0); 
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.loop1, 0);

		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_Id2, 0);								// 0 means no other simple tone here
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_type2, FXS_TONE_EXIST_CONFIGURATION);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.level2A, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.level2B, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq2A, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq2B, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence2On, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence2Off, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.loop2, 0); 	
	}
	else																		// Example of Composed tone
	{
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_Id, (u16) CMBS_FXS_TONE_MSG_WAIT);	// Main tone is MSG Wait (composed tone)
		cfr_ie_ser_u16((u8 *) &ToneCfg.loop, 0);
		cfr_ie_ser_u16((u8 *) &ToneCfg.Counter, 2);								// 2 Simple tones are in our Composed tone
		
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_Id1, CMBS_FXS_TONE_MSG_WAIT_SIMPLE0);	// First part of tone: first simple tone
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_type1, FXS_TONE_NEW_CONFIGURATION);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.level1A, -129); 
	    	cfr_ie_ser_u16((u8 *) & ToneCfg.level1B, -129);  
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq1A, 350);  
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq1B, 440); 
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence1On, 100); 
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence1Off, 100); 
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.loop1, 10);

		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_Id2, CMBS_FXS_TONE_DIAL);				// 2nd part of MW tone is normal dial tone
		cfr_ie_ser_u16((u8 *) &ToneCfg.tone_type2, FXS_TONE_EXIST_CONFIGURATION);	// Do not change Dial Tone configuration now
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.level2A, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.level2B, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq2A, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.freq2B, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence2On, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.cadence2Off, 0);
	    	cfr_ie_ser_u16((u8 *) &ToneCfg.loop2, 0); 		
	}
    	app_FxsParamSet(CMBS_PARAM_FXS_TONE_CONFIG, (u8 *) &ToneCfg, sizeof(ST_IE_FXS_TONE_CFG), 1);

}

//		========== keypb_HandsetPage ===========
/*!
\brief         Page a handset or all handsets
\param[in,ou]  <none>
\return

*/
void  keypb_HandsetPage(void)
{
  u32 u16_HsMask = 0;
  
  printf("\nInput possibilities for HS paging\n");
  printf(" a - all\n");
  printf(" h - handset mask\n");
  
  switch (tcx_getch())
  {
    case 'a':
      u16_HsMask = CMBS_ALL_HS_MASK;
      printf("\n call ALL handset (loator) \n");
      break;
            
    case 'h':      
      printf("\n Enter HS mask  (in hex format): ");
      tcx_scanf("%x", &u16_HsMask);
      break;
      
    default:
      printf("\n Invalid input! \n");
      break;
  }
 
  cmbs_dsr_hs_Page(g_cmbsappl.pv_CMBSRef, u16_HsMask); // use maximal possible mask
}

//      ========== keypb_HandsetStopPaging ===========
/*!
\brief         Stop page handsets
\param[in,ou]  <none>
\return

*/
void  keypb_HandsetStopPaging(void)
{
  u32 u16_HsMask = 0;
  u32 u32_CallInstance = 0xFFFF;
          
  printf("\nInput possibilities for HS paging\n");
  printf(" a - all\n");
  printf(" c - call ID\n");
  printf(" h - handset mask\n");
  
  switch (tcx_getch())
  {
    case 'a':
      // u16_HsMask = 0xFFFF;
      printf("\n release of ALL locator calls\n");
      break;
      
    case 'c':      
      printf("\n Enter Call Instance (in hex format): ");
      tcx_scanf("%x", &u32_CallInstance);
      break;
      
    case 'h':      
      printf("\n Enter HS mask  (in hex format): ");
      tcx_scanf("%x", &u16_HsMask);
      break;
      
    default:
      printf("\n Invalid input! \n");
      break;
  }
  
  app_SrvHandsetStopPaging(u16_HsMask, u32_CallInstance);
}


//		========== keypb_HandsetDelete ===========
/*!
\brief         Delete a handset or all handsets
\param[in,ou]  <none>
\return

*/
void  keypb_HandsetDelete(void)
{
    char buffer[20];

    printf("Enter handset number ( e.g. 1,2,3,4,....,max hs or none or all):\n");
    tcx_gets(buffer, sizeof(buffer));

    app_SrvHandsetDelete(buffer);

}

//		========== keypb_HandsetDelete ===========
/*!
\brief         Delete a handset or all handsets
\param[in,ou]  <none>
\return

*/
void  keypb_HandsetDeleteINL(void)
{
    u16  u16_HandsetMask = 0;

    printf("\nEnter handset mask (FFFF for all): ");
    tcx_scanf("%hX", &u16_HandsetMask);

    app_SrvHandsetDeleteINL(u16_HandsetMask);
}


/*!
\brief         Add HS to INL
\param[in,ou]  <none>
\return

*/
void  keypb_HandsetAddINL(void)
{
    char buffer[20];

    printf("Enter handset mask ( e.g. 1,2,3,4,....,max hs or none or all):\n");
    tcx_gets(buffer, sizeof(buffer));

    app_SrvHandsetAddINL(buffer);
}
//		========== keypb_SYSRegistrationMode ===========
/*!
\brief         Subscription on/off
\param[in,ou]  <none>
\return

*/
void  keypb_SYSRegistrationMode(void)
{
    u32 u32_Timeout_Value;

    printf("\n1 => Registration open DECT HS + ULE\n");
    printf("2 => Registration close\n");
    printf("3 => Registration open DECT handsets and ULE resub\n");
    printf("4 => Registration open ULE Deveices and DECT handsets resub\n");
    printf("5 => Registration open only resub of DECT handsets and ULE devices\n");

    switch (tcx_getch())
    {
    case '1':
        printf("\nSet timeout in seconds, use 255 for no restart of timer:\n");
        tcx_scanf("%d", &u32_Timeout_Value);

        app_SrvSubscriptionOpen(u32_Timeout_Value);
        break;
        
    case '2':
        app_SrvSubscriptionClose();
        break;
        
    case '3':
        printf("\nSet timeout in seconds, use 255 for no restart of timer:\n");
        tcx_scanf("%d", &u32_Timeout_Value);

        // only DECT handsets
        app_SrvSubscriptionOpenExt(u32_Timeout_Value, CMBS_HS_REG_ENABLE_HANDSET);
        break;
        
    case '4':
        printf("\nSet timeout in seconds, use 255 for no restart of timer:\n");
        tcx_scanf("%d", &u32_Timeout_Value);

        // only ULE deveices
        app_SrvSubscriptionOpenExt(u32_Timeout_Value, CMBS_HS_REG_ENABLE_ULE);
        break;
        
    case '5':
        printf("\nSet timeout in seconds, use 255 for no restart of timer:\n");
        tcx_scanf("%d", &u32_Timeout_Value);

        // only DECT handsets
        app_SrvSubscriptionOpenExt(u32_Timeout_Value, CMBS_HS_REG_ENABLE_ALL_RESUB);
        break;
        
    }
}

//		========== keypb_SYSEncryptionMode ===========
/*!
\brief         Encryption on/off
\param[in,ou]  <none>
\return

*/
void  keypb_SYSEncryptionMode(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    // u8 u8_EncDisable;
    
    printf("\n1 => Disable Encryption\n");
    printf("2 => Enable Encryption\n");
    printf("3 => Read Encryption Status\n");

    switch (tcx_getch())
    {
    case '1':
        app_SrvEncryptionDisable();
        break;
    case '2':
        app_SrvEncryptionEnable();
        break;
    case '3':
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
     
        // get Subscription data
        app_SrvParamGet(CMBS_PARAM_ENC_DISABLE, 1);

        // wait for CMBS target message
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        // u8_EncDisable = st_Container.ch_Info[0];
        
        switch (st_Container.ch_Info[0])
        {
          case 0:
            printf(" Encryption enabled\n");
            break;
          case 1:
            printf(" Encryption disabled\n");
            break;
        }
      
        break;
    }
}

//		========== keypb_SYSFPCustomFeatures ===========
/*!
\brief         Internal call and ConfCall enable/disable         
\param[in,ou]  <none>
\return

*/

void  keypb_SYSFPCustomFeatures(void)
{
    u8 u8_Value = 0;
    u8 u8_IntCall;
    u8 u8_ConfCall;
    s8 s8_Char;
    ST_APPCMBS_CONTAINER st_Container;

    /* get actual setting from target */
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_FP_CUSTOM_FEATURES, 1);

    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u8_Value = st_Container.ch_Info[0];

    /* print and change setting until exit */
    do
    {
        u8_IntCall = u8_Value & CMBS_CUSTOM_INT_DIS_MASK;
        u8_ConfCall = u8_Value & CMBS_CUSTOM_CALL_TRANS_DIS_MASK;

        printf("\n actual status: \n");
        if ( u8_IntCall )
        {
            printf("  Internal call disabled\n");
        }
        else
        {
            printf("  Internal call enabled\n");
        }

        if ( u8_ConfCall )
        {
            printf("  2-Line Call transfer disabled\n");
        }
        else
        {
            printf("  2-Line Call transfer enabled\n");
        }
        if ( u8_Value & CMBS_CUSTOM_INTRUSION_DIS_MASK )
        {
            printf("  Call Intrusion disabled\n");
        }
        else
        {
            printf("  Call Intrusion enabled\n");
        }
        if ( u8_Value & CMBS_CUSTOM_CONF_RPL_ENBL_MASK )
        {
            printf("  Call replacement during conference enabled\n");
        }
        else
        {
            printf("  Call replacement during conference disabled\n");
        }
        if ( u8_Value & CMBS_CUSTOM_CATIQ_TESTER_MASK )
        {
            printf("  CAT-iq test flag enabled\n");
        }
        else
        {
            printf("  CAT-iq test flag not enabled\n");
        }

        printf("\n1 => Disable Internal call\n");
        printf("2 => Enable  Internal call\n");
        printf("3 => Disable 2-Line Conference call\n");
        printf("4 => Enable  2-Line Conference call\n");
        printf("5 => Enable  Call replacement during Conference\n");
        printf("6 => Disable Call replacement during Conference\n");
        printf("b => Disable Call intrusion\n");
        printf("c => Enable  Call intrusion\n");
        printf("d =>   Set Tester mode (CAT-iq testing) \n");
        printf("e => Unset Tester mode (CAT-iq testing) \n");
        printf("\ns => Save new setting and exit (send to target)\n");
        printf("q => exit without saving changes \n");
        printf("Select: ...  ");

        s8_Char = tcx_getch(); //  atoi
        printf("\n");

        switch (s8_Char)
        {
        case '1':
            u8_Value |= CMBS_CUSTOM_INT_DIS_MASK;
            break;

        case '2':
            u8_Value &= ~CMBS_CUSTOM_INT_DIS_MASK;
            break;

        case '3':
            u8_Value |= CMBS_CUSTOM_CALL_TRANS_DIS_MASK;
            break;

        case '4':
            u8_Value &= ~CMBS_CUSTOM_CALL_TRANS_DIS_MASK;
            break;
 
        case '5':
            u8_Value |= CMBS_CUSTOM_CONF_RPL_ENBL_MASK;
            break;

        case '6':
            u8_Value &= ~CMBS_CUSTOM_CONF_RPL_ENBL_MASK;
            break;
            
        case 'b':
            u8_Value |= CMBS_CUSTOM_INTRUSION_DIS_MASK;
            break;

        case 'c':
            u8_Value &= ~CMBS_CUSTOM_INTRUSION_DIS_MASK;
            break;
            
        case 'd':
            u8_Value |= CMBS_CUSTOM_CATIQ_TESTER_MASK;
            break;

        case 'e':
            u8_Value &= ~CMBS_CUSTOM_CATIQ_TESTER_MASK;
            break;
        }

    } while ((s8_Char != 's') && (s8_Char != 'q'));

    if ( s8_Char == 's' )
    {
        /* send new setting to target */
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvParamSet(CMBS_PARAM_FP_CUSTOM_FEATURES, &u8_Value, sizeof(u8_Value), 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_SET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }
}

//		========== keypb_SYSFixedCarrierSet ===========
/*!
\brief         Set or reset Fixed carrier for RF testing
\param[in,ou]  <none>
\return

*/
void  keypb_SYSFixedCarrierSet(void)
{
    char buffer[10];
    u8 u8_Value;

    printf("\nSet fixed Carrier for TX measurement (2 digit hex)");
    printf("\n                e.g. for 2 use 02, FF for disable: ");

    memset(buffer, 0, sizeof(buffer));
    tcx_gets(buffer, sizeof(buffer));

    if ( strlen(buffer) )
    {
        u8_Value = app_ASC2HEX(buffer);

        // 0x80 for enable)
        if ( u8_Value == 0xFF )
        {
            // disable
            u8_Value = 0;
        }
        else
        {
            // add MSB (0x80) for enable
            u8_Value |= 0x80;
        }


        // app_SrvParamSet( CMBS_PARAM_FIXED_CARRIER, &u8_Value, sizeof(u8_Value), 1 );
        app_SrvFixedCarrierSet(u8_Value);
    }
}

//		========== keypb_SYSNemoContol ===========
/*!
\brief         Encryption on/off
\param[in,ou]  <none>
\return

*/
void  keypb_SYSNemoContol(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    u8 u8_NemoControl;
    u8 u8_CounterValue;
    char buffer[10];
    
    printf("\n1 => Set Default (0x00) \n");
    printf("2 => Enable  HM00_NEMO_DUMMY_USE_RSSI_FOR_IF_CHECK\n");
    printf("3 => Disable HM00_NEMO_DUMMY_USE_RSSI_FOR_IF_CHECK\n");
    printf("4 => Set value for N210Counter \n");

    app_SrvParamGet(CMBS_PARAM_NEMO_CONTROL, 1);
    
     // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container);
    
    u8_NemoControl = st_Container.ch_Info[0];
        
   switch (tcx_getch())
   {
    case '1':
      // Set Default (0x00) 
        app_SrvNemoControlSet(0);
        break;
    case '2':
     // Enable  HM00_NEMO_DUMMY_USE_RSSI_FOR_IF_CHECK
        app_SrvNemoControlSet(u8_NemoControl | 0x80);
        break;
    case '3':
      // Disable HM00_NEMO_DUMMY_USE_RSSI_FOR_IF_CHECK
        app_SrvNemoControlSet(u8_NemoControl &= ~0x80);
        break;
    case '4':
      // Disable HM00_NEMO_DUMMY_USE_RSSI_FOR_IF_CHECK
        memset(buffer, 0, sizeof(buffer));
        tcx_gets(buffer, sizeof(buffer));
        if ( strlen(buffer) )
        {
            u8_CounterValue = app_ASC2HEX(buffer);

            app_SrvNemoControlSet((u8_NemoControl & 0x80) | u8_CounterValue);
        }
        break;
    }
}

//		========== keypb_SYSRestart ===========
/*!
\brief         Reboot CMBS Target
\param[in,ou]  <none>
\return

*/
void  keypb_SYSRestart(void)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvSystemReboot();

	appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

//		========== keypb_SYSFWVersionGet ===========
/*!
\brief         Get current CMBS Target version
\param[in,ou]  <none>
\return

*/
void  keypb_SYSFWVersionGet(void)
{
    ST_APPCMBS_CONTAINER st_Container;

    u16 u16_FwModule = 0;
    u16 u16_FwDect = 0;
    u16 u16_FwDSP = 0;
    u16 u16_FwEEPROM = 0;
    u16 u16_FwUSB = 0;
    u16 u16_FwBuild = 0;
    u16 u16_FwBooter = 0;

    u8 parserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvFWVersionGet(CMBS_MODULE_CMBS, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_FW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_FwModule = ((PST_IE_FW_VERSION)st_Container.ch_Info)->u16_FwVersion;
    SleepMs(10);    // skip logging messages

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
	app_SrvFWVersionGet(CMBS_MODULE_DECT, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_FW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_FwDect = ((PST_IE_FW_VERSION)st_Container.ch_Info)->u16_FwVersion;
    SleepMs(10);    // skip logging messages

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvFWVersionGet(CMBS_MODULE_DSP, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_FW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_FwDSP = ((PST_IE_FW_VERSION)st_Container.ch_Info)->u16_FwVersion;
    SleepMs(10);    // skip logging messages

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvFWVersionGet(CMBS_MODULE_EEPROM, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_FW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_FwEEPROM = ((PST_IE_FW_VERSION)st_Container.ch_Info)->u16_FwVersion;
    SleepMs(10);    // skip logging messages

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvFWVersionGet(CMBS_MODULE_USB, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_FW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_FwUSB = ((PST_IE_FW_VERSION)st_Container.ch_Info)->u16_FwVersion;
    SleepMs(10);    // skip logging messages

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvFWVersionGet(CMBS_MODULE_BUILD, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_FW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_FwBuild = ((PST_IE_FW_VERSION)st_Container.ch_Info)->u16_FwVersion;
    SleepMs(10);    // skip logging messagesV

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvFWVersionGet(CMBS_MODULE_BOOTER, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_FW_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_FwBooter = ((PST_IE_FW_VERSION)st_Container.ch_Info)->u16_FwVersion;
    SleepMs(10);    // skip logging messages

    printf("\n\n**\n** FIRMWARE Version:\n");
    printf("** CMBS  : %X\n", u16_FwModule);
    printf("** DECT  : %X\n", u16_FwDect);
    printf("** DSP   : %X\n", u16_FwDSP);
    printf("** EEPROM: %X\n", u16_FwEEPROM);
    printf("** USB   : %X\n", u16_FwUSB);
    printf("** Build : %X\n", u16_FwBuild);
    printf("** Booter: %X\n", u16_FwBooter);

    app_set_msgparserEnabled(parserEnabled);
}

//		========== keypb_JEDECIdGet ===========
/*!
\brief         Get JEDEC ID of the flash (for DCX81 / DHX91)
\param[in,ou]  <none>
\return

*/
void keypb_JEDECIdGet()
{
    app_SrvJEDECidGet();
}


//		========== keypb_SYSEEPROMVersionGet ===========
/*!
\brief         Get current CMBS Target EEPROM version
\param[in,ou]  <none>
\return

*/
void keypb_SYSEEPROMVersionGet(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    PST_IE_EEPROM_VERSION  p_IE;

    u8 parserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvEEPROMVersionGet(TRUE);

    appcmbs_WaitForContainer(CMBS_EV_DSR_EEPROM_VERSION_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    p_IE = (PST_IE_EEPROM_VERSION)st_Container.ch_Info;

    SleepMs(10);    // skip logging messages

    printf("\n\n**\n** EEPROM Version:\n** ");
    printf("VER_%04x", p_IE->u32_EEPROM_Version);
    printf("\n**\n\n");

    app_set_msgparserEnabled(parserEnabled);

}


//		========== keypb_SYSHWVersionGet ===========
/*!
\brief         Get current CMBS Target hardware version
\param[in,ou]  <none>
\return

*/
void  keypb_SYSHWVersionGet(void)
{
    app_SrvHWVersionGet(TRUE);
}

//		========== keypb_DectSettingsGet ===========
/*!
\brief         Get DECT settings from target
\param[in,ou]  <none>
\return

*/
void  keypb_DectSettingsGet(void)
{
    ST_APPCMBS_CONTAINER		st_Container;
    PST_IE_DECT_SETTINGS_LIST	st_DectSettings;
    char str[128] = { 0 };
    int i = 0;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvDectSettingsGet(TRUE);

    appcmbs_WaitForContainer(CMBS_EV_DSR_DECT_SETTINGS_LIST_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    st_DectSettings = (PST_IE_DECT_SETTINGS_LIST)st_Container.ch_Info;    

    SleepMs(10);    // skip logging messages
    printf("\n--- DECT SETTINGS: ---\n");
    printf("ClockMaster: %d\n", st_DectSettings->u16_ClockMaster);
    printf("EmissionMode: %d\n", st_DectSettings->u16_EmissionMode);
    printf("IP address type: %s\n", st_DectSettings->u8_IPAddressType ? "Static" : "DHCP");
    printf("IP address: %d.%d.%d.%d\n", st_DectSettings->pu8_IPAddressValue[0], st_DectSettings->pu8_IPAddressValue[1], st_DectSettings->pu8_IPAddressValue[2], st_DectSettings->pu8_IPAddressValue[3]);
    printf("Subnet mask: %d.%d.%d.%d\n", st_DectSettings->pu8_IPAddressSubnetMask[0], st_DectSettings->pu8_IPAddressSubnetMask[1], st_DectSettings->pu8_IPAddressSubnetMask[2], st_DectSettings->pu8_IPAddressSubnetMask[3]);
    printf("Gateway: %d.%d.%d.%d\n", st_DectSettings->pu8_IPAddressGateway[0], st_DectSettings->pu8_IPAddressGateway[1], st_DectSettings->pu8_IPAddressGateway[2], st_DectSettings->pu8_IPAddressGateway[3]);
    printf("DNS Server: %d.%d.%d.%d\n", st_DectSettings->pu8_IPAddressDNSServer[0], st_DectSettings->pu8_IPAddressDNSServer[1], st_DectSettings->pu8_IPAddressDNSServer[2], st_DectSettings->pu8_IPAddressDNSServer[3]);

    strncpy(str, (const char *)st_DectSettings->pu8_FirmwareVersion, st_DectSettings->u8_FirmwareVersionLength);
    printf("FirmwareVersion: %s\n", str);
    memset(&str, 0, sizeof(str));

    strncpy(str, (const char *)st_DectSettings->pu8_EepromVersion, st_DectSettings->u8_EepromVersionLength);
    printf("EepromVersion: %s\n", str);
    memset(&str, 0, sizeof(str));

    strncpy(str, (const char *)st_DectSettings->pu8_HardwareVersion, st_DectSettings->u8_HardwareVersionLength);
    printf("HardwareVersion: %s\n", str);

    printf("Pin Code: ");
    for (i = 0; i < CMBS_MAX_PIN_CODE_LENGTH; i++)
    {
        if ( (st_DectSettings->pu8_PinCode[i] & 0xF0) != 0xF0 )
            printf("%d", st_DectSettings->pu8_PinCode[i] >> 4);
        if ( (st_DectSettings->pu8_PinCode[i] & 0xF) != 0xF )
            printf("%d", st_DectSettings->pu8_PinCode[i] & 0xF);
    }
    printf("\n");

#if (VDSW3_FEATURES & VDSW3_CATIQ_21)
    printf("FP Power level: %d\n", st_DectSettings->u16_FPPowerLevel);            
#endif //(VDSW3_FEATURES & VDSW3_CATIQ_21)
        
    tcx_getch();
}


//		========== keypb_DectSettingsSet ===========
/*!
\brief         Set DECT settings to target
\param[in,ou]  <none>
\return

*/
void  keypb_DectSettingsSet(void)
{
    ST_APPCMBS_CONTAINER		    st_Container;
    ST_IE_DECT_SETTINGS_LIST	    st_DectSettings;
#ifdef PROP_DSSL_FIELDS_BASE_ID
	ST_IE_PROP_FIELDS_LIST	st_PropSettings;
#endif
    PST_IE_RESPONSE				    st_Response;

    memset(&st_DectSettings, 0, sizeof(st_DectSettings));
    st_DectSettings.u16_FieldsMask = 0;

    // fill with dummy data
    /*
    st_DectSettings.u16_BaseReset = 0x1;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_BASE_RESET;
    */

    st_DectSettings.u16_ClockMaster = 0x1;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_CLCK_MASTER;

    st_DectSettings.u16_EmissionMode = 0x0;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_EMISSION_MODE;

    st_DectSettings.u8_IPAddressType = 0x1;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_IP_ADDR_TYPE;

    st_DectSettings.pu8_IPAddressValue[0] = 192;
    st_DectSettings.pu8_IPAddressValue[1] = 168;
    st_DectSettings.pu8_IPAddressValue[2] = 2;
    st_DectSettings.pu8_IPAddressValue[3] = 5;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_IP_ADDR_VAL;

    st_DectSettings.pu8_IPAddressSubnetMask[0] = 255;
    st_DectSettings.pu8_IPAddressSubnetMask[1] = 255;
    st_DectSettings.pu8_IPAddressSubnetMask[2] = 255;
    st_DectSettings.pu8_IPAddressSubnetMask[3] = 0;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_IP_ADDR_SUBNET_MASK;

    st_DectSettings.pu8_IPAddressGateway[0] = 192;
    st_DectSettings.pu8_IPAddressGateway[1] = 168;
    st_DectSettings.pu8_IPAddressGateway[2] = 2;
    st_DectSettings.pu8_IPAddressGateway[3] = 1;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_IP_GATEWAY;

    st_DectSettings.pu8_IPAddressDNSServer[0] = 8;
    st_DectSettings.pu8_IPAddressDNSServer[1] = 8;
    st_DectSettings.pu8_IPAddressDNSServer[2] = 8;
    st_DectSettings.pu8_IPAddressDNSServer[3] = 8;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_IP_DNS_SERVER;

    strcpy((char *)st_DectSettings.pu8_EepromVersion, "5588");
    st_DectSettings.u8_EepromVersionLength = strlen((const char *)st_DectSettings.pu8_EepromVersion);
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_EEPROM_VERSION;

    strcpy((char *)st_DectSettings.pu8_FirmwareVersion, "8855");
    st_DectSettings.u8_FirmwareVersionLength = strlen((const char *)st_DectSettings.pu8_FirmwareVersion);
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_FIRMWARE_VERSION;

    strcpy((char *)st_DectSettings.pu8_HardwareVersion, "AA55");
    st_DectSettings.u8_HardwareVersionLength = strlen((const char *)st_DectSettings.pu8_HardwareVersion);
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_HARDWARE_VERSION;

#ifdef CATIQ_2_1
    st_DectSettings.u16_FPPowerLevel = 0x1;
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_FP_POWER_LEVEL;
#endif //(CATIQ_2_1)

    /* New PIN code: 1212 */
    /*
    st_DectSettings.pu8_PinCode[0] = 0xFF;	// mark as not used
    st_DectSettings.pu8_PinCode[1] = 0xFF;	// mark as not used 
    st_DectSettings.pu8_PinCode[2] = 0x12;	// first two digits
    st_DectSettings.pu8_PinCode[3] = 0x12;  // last two digits
    st_DectSettings.u16_FieldsMask |= 1 << CMBS_DECT_SETTINGS_FIELD_PIN_CODE;
    */

    /* Proprietary fields: */
    // Length is the length inside thise field, but differnt to the length to be used in the 
    // target for the field.
// Base Identifier Hi
#ifdef PROP_DSSL_FIELDS_BASE_ID
            
 
    memset(&st_PropSettings, 0, sizeof(st_PropSettings));
    st_PropSettings.u16_PropFieldsMask = 0;
    
    st_PropSettings.st_PropField[0].u8_Len = 7;
    st_PropSettings.st_PropField[0].u8_Data[0] = CMBS_DECT_SETTINGS_PROP_FIELD_1;
    st_PropSettings.st_PropField[0].u8_Data[1] = 0;  // Properties Editable and PIN protected set to 0
    st_PropSettings.st_PropField[0].u8_Data[2] = 'S'; // Manufacturer Hi
    st_PropSettings.st_PropField[0].u8_Data[3] = 'H'; // Manufacturer Lo

    
    printf("\n Select proprietary field settings");
    printf("\n 1 -> DSSL: Extension 2, Base ID 0");
    printf("\n 2 -> DSSL: Extension 4, Base ID 1\n");
    printf("\n 3 -> DSSL: Extension 5, Base ID 0\n");
    
    switch (tcx_getch())
    {
      case '1':
         st_PropSettings.st_PropField[0].u8_Data[4] = 2;  // Extension version, to be clarified
         st_PropSettings.st_PropField[0].u8_Data[5] = 0; // Base Identifier Hi - I don't know useful values
         break;
       case '2':
         st_PropSettings.st_PropField[0].u8_Data[4] = 4;  // Extension version, to be clarified
         st_PropSettings.st_PropField[0].u8_Data[5] = 1; // Base Identifier Hi - I don't know useful values
         break;
        case '3':
         st_PropSettings.st_PropField[0].u8_Data[4] = 5;  // Extension version, to be clarified
         st_PropSettings.st_PropField[0].u8_Data[5] = 0; // Base Identifier Hi - I don't know useful values
         break;
      default:
         st_PropSettings.st_PropField[0].u8_Data[4] = 0;  // Extension version, to be clarified
         st_PropSettings.st_PropField[0].u8_Data[5] = 0; // Base Identifier Hi - I don't know useful values
          break;
    }
    
    st_PropSettings.st_PropField[0].u8_Data[6] = 0; // Base Identifier Lo - I don't know useful values
    st_PropSettings.u16_PropFieldsMask |= 1 << (CMBS_DECT_SETTINGS_PROP_FIELD_1-1 & 0x7F);
            
    // E.g. Capabilities: 
    st_PropSettings.st_PropField[1].u8_Len = 3;
    st_PropSettings.st_PropField[1].u8_Data[0] = CMBS_DECT_SETTINGS_PROP_FIELD_1 + 1;
    st_PropSettings.st_PropField[1].u8_Data[1] = 0;  // Properties Editable and PIN protected set to 0
    // ASA: Version 4: new fields are supported:
//    st_PropSettings.st_PropField[1].u8_Data[2] = 0x0E;  // Preferred outgoing line ID, CATiq and GAP repeater support
    // ASA: Added Model Name support
    st_PropSettings.st_PropField[1].u8_Data[2] = 0xEC;  // HS Model Name, Read only Call list, Read only Contact list
                                                        // Preferred outgoing line ID, CATiq support
    st_PropSettings.u16_PropFieldsMask |= 1 << ((CMBS_DECT_SETTINGS_PROP_FIELD_1) & 0x7F);
    
     // E.g. Flags: 
    st_PropSettings.st_PropField[2].u8_Len = 3;
    st_PropSettings.st_PropField[2].u8_Data[0] = CMBS_DECT_SETTINGS_PROP_FIELD_1 + 2;
    st_PropSettings.st_PropField[2].u8_Data[1] = 0;  // Properties Editable and PIN protected set to 0
    st_PropSettings.st_PropField[2].u8_Data[2] = 2;  // Disable CAT-iq Repeater support, enable GAP Repeater support
    st_PropSettings.u16_PropFieldsMask |= 1 << ((CMBS_DECT_SETTINGS_PROP_FIELD_1+1) & 0x7F);
         
    // ASA: There was an error with the mask, it needs to start on the first bit (LSB), but CMBS_DECT_SETTINGS_PROP_FIELD_1 is 0x81, thus it starts with a shift by 1:
    st_PropSettings.u16_PropFieldsMask = st_PropSettings.u16_PropFieldsMask >> 1;
    
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.	 
    app_SrvPropDectSettingsSet(&st_DectSettings, &st_PropSettings,  1);
#else
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvDectSettingsSet(&st_DectSettings,  1);
#endif
    
    appcmbs_WaitForContainer(CMBS_EV_DSR_DECT_SETTINGS_LIST_SET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    st_Response = (PST_IE_RESPONSE)st_Container.ch_Info;
    printf("Dect Settings SET Result: %s\n", st_Response->e_Response == CMBS_RESPONSE_OK ? "OK" : "FAIL");

    tcx_getch();
}



//		========== keyb_RepeaterSupport ===========
/*!
\brief         Send repeater type (support) setting to target (enable/disable) by writing to EEOROM parameter
\param[in,ou]  <none>
\return

*/
void  keyb_RepeaterSupport(void)
{
    ST_APPCMBS_CONTAINER		    st_Container;
    s8 s8_Char;
    u8 u8_Value;

    // get current ECO MODE
    // appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_REPEATER_TYPE, CMBS_PARAM_REPEATER_TYPE_LENGTH);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    printf("\nCurrent Repeater type setting is: \t 0x%X \n", st_Container.ch_Info[0] );
        
    /* print and change setting until exit */
    do
    {
        printf(" Options for encrypted repeater support:\n");
        printf("  1 -> enable repeater support (send 0x27)\n");
        printf("  2 -> disable repeater support (send 0)\n");
        printf("\n  q -> exit \n\n Input: ");
        
        s8_Char = tcx_getch(); //  atoi
        printf("\n");

        switch (s8_Char)
        {
          case '1':
            // Enable repeater support
            u8_Value = 0x27;
            app_SrvRepeaterTypeSet(u8_Value);
            break;
            
          case '2':
            // Disable repeater support
            u8_Value = 0;
            /* send new setting to target */
            app_SrvRepeaterTypeSet(u8_Value);
            break;
        }
    } while (s8_Char != 'q');
}

//		========== keypb_SysLogStart ===========
/*!
\brief         Get current content of log buffer
\param[in,ou]  <none>
\return

*/
void  keypb_SysLogStart(void)
{
    app_SrvLogBufferStart();

    printf("SysLog started\n");
}

//		========== keypb_SysLogStop ===========
/*!
\brief         Get current content of log buffer
\param[in,ou]  <none>
\return

*/
void  keypb_SysLogStop(void)
{
    app_SrvLogBufferStop();

    printf("SysLog stopped\n");
}

//		========== keypb_SysLogRead ===========
/*!
\brief         Get current content of log buffer
\param[in,ou]  <none>
\return

*/
void  keypb_SysLogRead(void)
{
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvLogBufferRead(TRUE);

    appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_LOG); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    printf("Press Any Key!\n");
    tcx_getch();
}

/* == ALTDV == */

//      ========== keyb_SYSPowerOff ===========
/*!
\brief         Power Off CMBS Target
\param[in,ou]  <none>
\return

*/
void  keypb_SYSPowerOff(void)
{
    app_SrvSystemPowerOff();
    printf("\n");
    printf("Power Off CMBS Target\n");
}

void keypb_SendHSRSSIReq(void)
{
    u32 u32_HSNum = 0;

    printf("\nplease enter HS number\n");

    tcx_scanf("%d", &u32_HSNum);

    // send request for RSSI
    app_SrvSendHSRSSIReq((u8)u32_HSNum);
}

//      ========== keyb_RF_Control ===========
/*!
\brief         Suspend/Resume RF on CMBS Target
\param[in,ou]  <none>
\return

*/
void keypb_RF_Control(void)
{
    printf("\n");
    printf("1 => RF Suspend\n");
    printf("2 => RF Resume\n");
    printf("3 => Use fixed frequency (carrier) for Test purpose\n");
    printf("4 => Get RSSI of a HS\n");

    switch (tcx_getch())
    {
    case '1':
        app_SrvRFSuspend();
        break;
    case '2':
        app_SrvRFResume();
        break;
    case '3':
        keypb_SYSFixedCarrierSet();
        break;
    case '4':
        keypb_SendHSRSSIReq();
        break;
    }
}


//      ========== keypb_TurnOn_NEMo_mode ===========
/*!
\brief         Turn On/Off NEMo mode on CMBS Target
\param[in,ou]  <none>
\return

*/
void keypb_Turn_On_Off_NEMo_mode(void)
{
    printf("\n");
    printf("1 => Turn On NEMo mode\n");
    printf("2 => Turn Off NEMo mode\n");

    switch (tcx_getch())
    {
    case '1':
        app_SrvTurnOnNEMo();
        break;
    case '2':
        app_SrvTurnOffNEMo();
        break;
    }
}

//      ========== keyb_RegisteredHandsets ===========
/*!
\brief         Get List of subscribed handsets
\param[in,ou]  <none>
\return

*/
void  keypb_RegisteredHandsets(void)
{
    ST_APPCMBS_CONTAINER        st_Container;
    ST_IE_RESPONSE              st_Resp;

    memset(&st_Container, 0, sizeof(st_Container));

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvRegisteredHandsets(0xFFFF, 1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    memcpy(&st_Resp, &st_Container.ch_Info, st_Container.n_InfoLen);

    printf("\nSubscribed list get %s\n", st_Resp.e_Response == CMBS_RESPONSE_OK ? "Success" : "Fail");
    printf("Press Any Key!\n");
    tcx_getch();
}


//      ========== keyb_SetNewHandsetName ===========
/*!
\brief         Set List of subscribed handsets
\param[in,ou]  <none>
\return

*/
typedef enum {
CS00_CF_SUPPORTED,
CS00_CATIQ_1_0_SUPPORTED,
CS00_CATIQ_2_0_SUPPORTED,
CS00_CATIQ_2_1_SUPPORTED,
CS00_CALL_SCREENING_SUPPORTED,
CS00_CATIQ_3_0_SUPPORTED,
CS00_REKEY_EARLYENC_SUPPORTED,
CS00_ASSOC_MELODY_SUPPORTED,
#ifdef CMBS
CS00_ENCRYPT_DISABLED,
#else //CMBS
CS00_ENCRYPT_SUPPORTED,
#endif //CMBS
CS00_IPQ_SUPPORTED,
CS00_ULE_SUPPORTED,
#ifdef DDV
CS00_DDV_SUPPORTED,
#endif
} t_e_cs00_FeatureType;
void  keyb_SetNewHandsetName(void)
{
    u16     u16_HsID = 0;
    u8      u8_HsName[CMBS_HS_NAME_MAX_LENGTH + 1];
    u16     u16_NameSize = 0; 

#ifdef PROP_DSSL_FIELDS_BASE_ID 
    u8      u8_OutgPrefLine = 0x7F;
    u8      u8_SendProp;
	ST_IE_PROP_FIELDS_LIST st_PropSettings;
    char    ps8_Buffer[6];
    memset(&st_PropSettings, 0, sizeof(st_PropSettings));
#endif
    
    memset(u8_HsName, 0, sizeof(u8_HsName));

    printf("\nSet new HS name: ");
    printf("\nEnter HS/Extension number (1-12): ");
    tcx_scanf("%hu", &u16_HsID);
    if ( u16_HsID < 1 || u16_HsID > 12 )
    {
        printf("\n HS number not in range 1-12 ");
        return;
    }
    printf("\nEnter name (max 32 symbols): ");
    tcx_scanf("%s", u8_HsName);

    u16_NameSize = (u16)strlen((char *)u8_HsName);

    if ( u16_NameSize > CMBS_HS_NAME_MAX_LENGTH )
    {
        printf("\n HS name too long ");
        return;
    }

#ifdef PROP_DSSL_FIELDS_BASE_ID  
    printf("\nEnter outgoing line preference (dec, n=none, d=don't change): ");    
    tcx_gets(ps8_Buffer, sizeof(ps8_Buffer));
    if (*ps8_Buffer == 'n')
    {
        // set it to 'none' (=FT line selection)
        u8_OutgPrefLine = 0x7F;
    }
    else if (*ps8_Buffer == 'd')
    {
        // don't change the actual setting
        u8_OutgPrefLine = 0xFF;
    }
    else
    {
        u8_OutgPrefLine = atoi(ps8_Buffer);
    }           
    
    printf("\n Send Preset Handset Indication? y/n: ");
    u8_SendProp = tcx_getch();
    printf("\n");
    
    if (u8_SendProp == 'y')
    {
        // E.g. Handset Identifier (0x81)
        // st_PropSettings.st_PropField[1].u8_Len = 0;
        // st_PropSettings.st_PropField[2].u8_Len = 0;
        st_PropSettings.st_PropField[0].u8_Len = 13;
        st_PropSettings.st_PropField[0].u8_Data[0] = CMBS_DECT_SETTINGS_PROP_FIELD_1;
        st_PropSettings.st_PropField[0].u8_Data[1] = 0;  // Properties Editable and PIN protected set to 0
        st_PropSettings.st_PropField[0].u8_Data[2] = 'S'; // Manufactuer Hi
        st_PropSettings.st_PropField[0].u8_Data[3] = 'V'; // Manufactuer Lo
        st_PropSettings.st_PropField[0].u8_Data[4] = 1;  // Extension version number, to be clarified
        st_PropSettings.st_PropField[0].u8_Data[5] = 0;  // SW version - I don't know useful values
        st_PropSettings.st_PropField[0].u8_Data[6] = 1;  //  SW version - I don't know useful values
        st_PropSettings.st_PropField[0].u8_Data[7] = 0;  //  SW version - I don't know useful values
        st_PropSettings.st_PropField[0].u8_Data[8] = 1;  //  Model Hi - I don't know useful values
        st_PropSettings.st_PropField[0].u8_Data[9] = 0;  //  Model Lo - I don't know useful values
        st_PropSettings.st_PropField[0].u8_Data[10] = 1;  // Data version - I don't know useful values
        st_PropSettings.st_PropField[0].u8_Data[11] = 0;  // Data version - I don't know useful values
        st_PropSettings.st_PropField[0].u8_Data[12] = 1;  // Data version - I don't know useful values

        st_PropSettings.u16_PropFieldsMask = 1;	
        
        // E.g. SC MWI Support (0x83)
        st_PropSettings.st_PropField[1].u8_Len = 3;
        st_PropSettings.st_PropField[1].u8_Data[0] = CMBS_DECT_SETTINGS_PROP_FIELD_1+2;
        st_PropSettings.st_PropField[1].u8_Data[1] = 0x40;  // Properties Editable 1 and PIN protected set to 0
        
        printf("\n\n Set SC MWI Support y/n: ");
        u8_SendProp = tcx_getch();
        if (u8_SendProp == 'y')
        {
            st_PropSettings.st_PropField[1].u8_Data[2] = 1; 
        }
        else
        {
            st_PropSettings.st_PropField[1].u8_Data[2] = 0; 
        }
        
        st_PropSettings.u16_PropFieldsMask |= 2;	

		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        app_SrvSetNewHandsetNameAndLinePref(u16_HsID, u8_HsName, u16_NameSize, 1, u8_OutgPrefLine, &st_PropSettings);
    }
    else
    {
		u16 u16_HsCapabilities =0;
		
        printf("\n\n Catiq 1.0 y/n: ");
        u8_SendProp = tcx_getch();
		if(u8_SendProp == 'y')
			u16_HsCapabilities |= 1<<CS00_CATIQ_1_0_SUPPORTED;
		
        printf("\n\n Catiq 2.0 y/n: ");
        u8_SendProp = tcx_getch();
		if(u8_SendProp == 'y')
			u16_HsCapabilities |= 1<<CS00_CATIQ_2_0_SUPPORTED;

        printf("\n\n Catiq 2.1 y/n: ");
        u8_SendProp = tcx_getch();
		if(u8_SendProp == 'y')
			u16_HsCapabilities |= 1<<CS00_CATIQ_2_1_SUPPORTED;

        printf("\n\n Catiq 3.0 y/n: ");
        u8_SendProp = tcx_getch();
		if(u8_SendProp == 'y')
			u16_HsCapabilities |= 1<<CS00_CATIQ_3_0_SUPPORTED;
		
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.


        app_SrvSetNewHandsetNameWithCatIqProp(u16_HsID, u8_HsName, u16_NameSize, 1, u16_HsCapabilities);
    }
#else
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvSetNewHandsetName(u16_HsID, u8_HsName, u16_NameSize, 1);
#endif

    // app_SrvSetNewHandsetName(u16_HsID, u8_HsName, u16_NameSize, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_HS_SUBSCRIBED_LIST_SET_RES, NULL);

    // Display the list so the user can verify change is OK
    printf("\nList of registered handsets:\n");
    app_SrvRegisteredHandsets(0xFFFF, 1);
}

//		========== keypb_AddNewExtension ===========
/*!
\brief         Add new extension to internal names list (FXS)
\param[in,ou]  <none>
\return

*/
void  keypb_AddNewExtension(void)
{
    u8	u8_NumberSize;
    u8	u8_Number[CMBS_FXS_NUMBER_MAX_LENGTH + 1];

    u16	u16_NameSize;
    u8	u8_Name[CMBS_HS_NAME_MAX_LENGTH + 1];

    memset(u8_Name, 0, sizeof(u8_Name));
    memset(u8_Number, 0, sizeof(u8_Number));


    printf("\nEnter Extension Number(max 4 symbols): \n");
    tcx_scanf("%s", u8_Number);

    u8_NumberSize = strlen((char *)u8_Number);
    if ( u8_NumberSize < 1 || u8_NumberSize > 4 )
    {
        printf("\n Wrong Extension Number length");
        return;
    }

    printf("\nEnter Extension Name(max 31 symbols): \n");
    tcx_scanf("%s", u8_Name);

    u16_NameSize = strlen((char *)u8_Name);

    if ( u16_NameSize >= CMBS_HS_NAME_MAX_LENGTH )
    {
        printf("\n HS name too long ");
        return;
    }

    app_SrvAddNewExtension(u8_Name, u16_NameSize, u8_Number, u8_NumberSize, 1);
}

//      ========== keypb_SetBaseName ===========
/*!
\brief         Set Base name
\param[in,ou]  <none>
\return

*/
void keypb_SetBaseName(void)
{
    u8	u8_BaseNameSize;
    u8   u8_BaseName[CMBS_BS_NAME_MAX_LENGTH + 1];

    printf("\nEnter new base name (max 13 symbols): \n");
    tcx_scanf("%s", u8_BaseName);

    u8_BaseNameSize = strlen((char *)u8_BaseName);

    app_SrvSetBaseName(u8_BaseName, u8_BaseNameSize, 1);

}

void keypb_GetBaseName(void)
{
    ST_APPCMBS_CONTAINER        st_Container;
    ST_IE_BASE_NAME           	st_BaseName;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvGetBaseName(1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_GET_BASE_NAME_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    memcpy(&st_BaseName.u8_BaseName, &st_Container.ch_Info, st_Container.n_InfoLen);

    printf("\nBase Name %s\n", st_BaseName.u8_BaseName);
    printf("Press Any Key!\n");
    tcx_getch();
}

/*=== keyb_CommStress  =======================================================*/
/*                                                                            */
/* FUNCTIONAL DESCRIPTION:                                                    */
/*                                                                            */
/* This function is called when a stress test is required for the communication
 *  channel between host and target.
 *                                                                      
 * PARAMETERS
 *
 *                                                                            */
/* INTERFACE DECLARATION:                                                     */
void keyb_CommStress(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    u8 pu8_SubsDataRx[CMBS_PARAM_SUBS_DATA_LENGTH], pu8_SubsDataTx[CMBS_PARAM_SUBS_DATA_LENGTH], pu8_Buffer[8];
    u8 i;
    u32 u32_Counter, u32_Errors = 0;
    u8 u8_TestSelect;
    u8 szPinCode[CMBS_PARAM_PIN_CODE_LENGTH] = { 0xFF, 0xFF, 0, 0 };
    u8 szPinCodeRx[CMBS_PARAM_PIN_CODE_LENGTH];

    // save parser state as we want to disable it temporary
    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    // Clear screen
    tcx_appClearScreen();

    printf("This test performs stress test for the communication channel with 2 options:\n");
    printf(" s - by setting and getting PP Subscription data (200 bytes).\n");
    printf(" a - by getting PP Subscription data (200 bytes) and setting the AUTH PIN\n");
    printf(" Select test to perform: (s or a): ");
    u8_TestSelect = tcx_getch();

    printf("\n Enter number of iterations to perform (max 1000):\n");
    tcx_gets((char *)pu8_Buffer, sizeof(pu8_Buffer));
    u32_Counter = (u32)atoi((const char *)pu8_Buffer);
    u32_Counter = MIN(MAX_OVERNIGHT_TESTS, u32_Counter);
    // get Subscription data
    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_SUBS_DATA, 1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // copy to internal buffers
    memcpy(pu8_SubsDataRx, st_Container.ch_Info, st_Container.n_InfoLen);
    memcpy(pu8_SubsDataTx, st_Container.ch_Info, st_Container.n_InfoLen);

    if ( u8_TestSelect == 's' )
    {

        for (; u32_Counter > 0; --u32_Counter)
        {
            // Set Subscription data
            appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_SUBS_DATA, pu8_SubsDataTx, CMBS_PARAM_SUBS_DATA_LENGTH, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

            // Get Subscription data
            appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_SUBS_DATA, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(pu8_SubsDataRx, st_Container.ch_Info, st_Container.n_InfoLen);

            // compare
            if ( memcmp(pu8_SubsDataRx, pu8_SubsDataTx, sizeof(pu8_SubsDataRx)) )
            {
                printf("\nCommunication error!\n");
                ++u32_Errors;
            }
        }
    }
    else
    {
        /* use FFFFxxxx form (Expected by app_SrvPINCodeSet) 
        for ( i = 0; i < 4; ++i )
        {
            PINCode[i] = 'f';
            PINCode[i + 4] = '0';
        }
        PINCode[8] = 0;
        PINCodeRx[8] = 0; */

        // AUTH_PIN stresstest combined with read of subcsriptiondata
        for (; u32_Counter > 0; --u32_Counter)
        {
            // Get Subscription data and compare it with initial data (first read)
            appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_SUBS_DATA, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(pu8_SubsDataRx, st_Container.ch_Info, st_Container.n_InfoLen);

            // compare
            if ( memcmp(pu8_SubsDataRx, pu8_SubsDataTx, sizeof(pu8_SubsDataRx)) )
            {
                printf("\nCommunication error Subsdata!\n");
                ++u32_Errors;
            }
            // Set AUTH PIN data
            // app_SrvParamSet( CMBS_PARAM_AUTH_PIN, , 1 );
            // ASA: I don't understand why appcmbs_WaitForContainer doesn't work in that case !!!!
            // app_SrvPINCodeSet( PINCode );

            // do it different:
            appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamSet(CMBS_PARAM_AUTH_PIN, szPinCode, CMBS_PARAM_PIN_CODE_LENGTH, 1);
            appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

			appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
            app_SrvParamGet(CMBS_PARAM_AUTH_PIN, 1);
            appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
            memcpy(szPinCodeRx, st_Container.ch_Info, 4);
            printf(" Rec. PIN:\n");
            for (i = 0; i < 4; i++)
            {
                printf(" %x", szPinCodeRx[i]);
                if ( szPinCodeRx[i] != szPinCode[i] )
                {
                    ++u32_Errors;
                    printf(" AUTH_PIN ERROR !!! \n");
                }

            }
            printf("\n");
        }
    }

    // restore parser state
    app_set_msgparserEnabled(msgparserEnabled);

    printf("\nAll done with %d errors.\n", u32_Errors);

    tcx_getch();
}

//		========== keypb_SubsHSTest ===========
/*!
\brief         Get subscribed HS Data, perform EEPROM reset, restore subscribed HS Data
\param[in,out]  <none>
\return void
*/
void keyb_SubsHSTest(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    u8 pu8_SubsData[CMBS_PARAM_SUBS_DATA_LENGTH];
    u8 pu8_SubsDataEx[CMBS_PARAM_SUBS_DATA_EX_LENGTH];
    ST_CMBS_SUBS_DATA u8_RepeaterSubsData[CMBS_CRFP_SUBSCRIBED_MAX_NUM];
    u8 pu8_RFPI[CMBS_PARAM_RFPI_LENGTH] = { 0 };
    u8 pu8_ULENextTPUI[CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH] = { 0 };
    u32 u32_ULE_dect_start, u32_ULE_dect_end, u32_ULE_dect_size;
    u32 u32_ULE_start, u32_ULE_end, u32_ULE_size;
    u32 u32_FUN_start, u32_FUN_end, u32_FUN_size;
    u32 u32_Repeater_end, u32_Repeater_start, u32_Repeater_size = 0;
    s8 s8_i;
    // u8 u8_RepeaterSupported = 0;
    E_CMBS_RC e_status;
    FILE *f_binFile = 0;

	UnionHanEEPROMBackupEBlocksPools union_HanEEPROMBackupBlocks;

    u8 pu8_filename_DECT[] = "DECT.bin"; //DECT file name
    u8 pu8_filename_ULE[] = "ULE.bin"; //ULE file name
    u8 pu8_filename_FUN[] = "FUN.bin"; //FUN file name
    // save parser state as we want to disable it temporary
    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    // Clear screen
    tcx_appClearScreen();

    // get current RFPI
    printf("Getting Current RFPI...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RFPI, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&pu8_RFPI[0], st_Container.ch_Info,  CMBS_PARAM_RFPI_LENGTH);


    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");

    printf("Current RFPI: \t %.2X%.2X%.2X%.2X%.2X\n", pu8_RFPI[0], pu8_RFPI[1], pu8_RFPI[2], pu8_RFPI[3], pu8_RFPI[4]);

    SleepMs(3000);

    // Get current subscribe data
    printf("\nGetting SUBS Data... \n");

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_SUBS_DATA, 1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // copy to internal buffer
    memcpy(pu8_SubsData, st_Container.ch_Info, st_Container.n_InfoLen);

    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");
    printf("Getting SUBS Data... done.");

    // Handle ULE Subscription restore

    // Check binary files can be created
    e_status = tcx_fileOpen(&f_binFile, pu8_filename_DECT, (u8 *)"w+b");
    if ( e_status != CMBS_RC_OK )
    {
        printf("\nError: Could not create file: %s", pu8_filename_DECT);
        return;
    }

    
    // Get current extended subscribe data
    printf("\nGetting Extended SUBS Data... \n");
	
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
  
    app_SrvParamGet(CMBS_PARAM_SUBS_DATA_EX, 1);
  
    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container);

    // copy to internal buffer
    memcpy(pu8_SubsDataEx, st_Container.ch_Info, st_Container.n_InfoLen);
 
    // Get current repeater subscribe data
    printf(" done\nGetting Repeater SUBS Data... \n");
/*
    app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_DATA, 1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container);

    if ( st_Container.ch_Info[0] != CMBS_RESPONSE_OK )
    {
        printf("\n !! Repeater support not enabled in target SW !!!\n");
    }
    else
    {
        // copy to internal buffer
        memcpy(pu8_RepeaterSubsData, st_Container.ch_Info, st_Container.n_InfoLen);
        u8_RepeaterSupported = 1;
        printf(" done\n");
    }
  */
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
        
        printf("Repeater subscription start address: \t 0x%X\n", u32_Repeater_start);
        
        printf("Getting Repeater subscription end address...\n");
	    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        
        app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_END, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        
        memcpy(&u32_Repeater_end, st_Container.ch_Info,  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH);
        
	    memset(&u8_RepeaterSubsData, 0, CMBS_CRFP_SUBSCRIBED_MAX_NUM * sizeof(ST_CMBS_SUBS_DATA));
        
        //Calculate the needed size
        u32_Repeater_size = u32_Repeater_end - u32_Repeater_start;
        
        //Get data from Target
        if (u32_Repeater_size <= (CMBS_CRFP_SUBSCRIBED_MAX_NUM * sizeof(ST_CMBS_SUBS_DATA)))
        {
  	        app_ParamAreaGetBySegments(u32_Repeater_start, u32_Repeater_size, (u8 *) &u8_RepeaterSubsData, g_CMBSMaxTransferSize);
        }
    }
          
          
    // Get DECT parameters
    printf("Getting DECT DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("DECT DB start address: \t 0x%X\n", u32_ULE_dect_start);

    // Get ULE DECT parameters
    printf("Getting DECT DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("DECT DB end address: \t 0x%X\n", u32_ULE_dect_end);

    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");
    printf("Getting SUBS Data... done.");
    printf("Getting DECT DB address...done\n");

    //Calculate the needed size
    u32_ULE_dect_size = u32_ULE_dect_end - u32_ULE_dect_start;

    printf("DECT DB size: \t 0x%X\n", u32_ULE_dect_size);

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], 0, HAN_DECT_SUBS_LENGTH);

    //Get data from Target
    if ( u32_ULE_dect_size <= HAN_DECT_SUBS_LENGTH )
    {
		app_ParamAreaGetBySegments(u32_ULE_dect_start, u32_ULE_dect_size, &union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Error in DECT DB size - received 0x%X\n", u32_ULE_dect_size);
        u32_ULE_dect_size = 0;
    }

	tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], 0, u32_ULE_dect_size);
    fflush(f_binFile);
    tcx_fileClose(f_binFile);


    // Check binary files can be created
    e_status = tcx_fileOpen(&f_binFile, pu8_filename_ULE, (u8 *)"w+b");
    if ( e_status != CMBS_RC_OK )
    {
        printf("\nError: Could not create file: %s", pu8_filename_ULE);
        return;
    }

    // Get ULE  parameters
    printf("Getting ULE DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("ULE DB start address: \t 0x%X\n", u32_ULE_start);


    // Get ULE parameters
    printf("Getting ULE  DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("ULE DB end address: \t 0x%X\n", u32_ULE_end);

    //Calculate the needed size
    u32_ULE_size = u32_ULE_end - u32_ULE_start;

    printf("ULE DB size: \t 0x%X\n", u32_ULE_size);

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA[0], 0, HAN_ULE_SUBS_LENGTH);

    //Get data from Target
    if ( u32_ULE_size <= HAN_ULE_SUBS_LENGTH )
    {
		app_ParamAreaGetBySegments(u32_ULE_start, u32_ULE_size, &union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Error in ULE  DB size - received 0x%X\n", u32_ULE_size);
        u32_ULE_size = 0;
    }

	tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA[0], 0, u32_ULE_size);
    fflush(f_binFile);
    tcx_fileClose(f_binFile);

    // Check binary files can be created
    e_status = tcx_fileOpen(&f_binFile, pu8_filename_FUN, (u8 *)"w+b");
    if ( e_status != CMBS_RC_OK )
    {
        printf("\nError: Could not create file: %s", pu8_filename_FUN);
        return;
    }

    // Get FUN parameters
    printf("Getting FUN DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("FUN DB start address: \t 0x%X\n", u32_FUN_start);


    // Get FUN parameters
    printf("Getting FUN  DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("FUN DB end address: \t 0x%X\n", u32_FUN_end);

    //Calculate the needed size
    u32_FUN_size = u32_FUN_end - u32_FUN_start;

    printf("HAN FUN DB size: \t 0x%X\n", u32_FUN_size);

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA[0], 0, HAN_FUN_SUBS_LENGTH);

    //Get data from Target
    if ( u32_FUN_size <= HAN_FUN_SUBS_LENGTH )
    {
		app_ParamAreaGetBySegments(u32_FUN_start, u32_FUN_size, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA[0], g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Error in FUN  DB size - received 0x%X\n", u32_FUN_size);
        u32_FUN_size = 0;
    }

	tcx_fileWrite(f_binFile, &union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA[0], 0, u32_FUN_size);
    fflush(f_binFile);
    tcx_fileClose(f_binFile);

    //Get next ULE TPUI
    printf("Getting Next TPUI...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(pu8_ULENextTPUI, st_Container.ch_Info,  CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH);

    // perform EEPROM reset
    printf("\nPerforming EEPROM reset... \n");

    keyb_EEPROMReset();

    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");
    printf("Getting SUBS Data... done.\n");
    printf("Getting ULE HAN DB address...done\n");
    printf("Performing EEPROM reset... done.\n");

    // restore SUBS Data
    printf("Restoring SUBS Data... \n");

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_SUBS_DATA, pu8_SubsData, CMBS_PARAM_SUBS_DATA_LENGTH, 1);

    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");
    printf("Getting SUBS Data... done.\n");
    printf("Getting ULE HAN DB...done\n");
    printf("Performing EEPROM reset... done.\n");
    printf("Restoring SUBS Data... done.\n");

    // restore RFPI
    printf("\nRestoring RFPI... \n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RFPI, pu8_RFPI, sizeof(pu8_RFPI), 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // restore HAN Data
    printf("Restoring DECT Data... \n");
   
    // restore Extended SUBS Data
    printf("Restoring Extended SUBS Data... \n");

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
 
    app_SrvParamSet(CMBS_PARAM_SUBS_DATA_EX, pu8_SubsDataEx, CMBS_PARAM_SUBS_DATA_EX_LENGTH, 1);

    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES);

    if (u32_Repeater_size)
    {
        // restore Extended SUBS Data
        printf("Restoring Repeater SUBS Data... \n");

	    // appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.

        // app_SrvParamSet(CMBS_PARAM_REPEATER_SUBS_DATA, pu8_RepeaterSubsData, CMBS_PARAM_REPEATER_SUBS_DATA_LENGTH, 1);

        // appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES);
		    
        app_ParamAreaSetBySegments(u32_Repeater_start, u32_Repeater_size, (u8 *)  &u8_RepeaterSubsData, g_CMBSMaxTransferSize);
    }
               
    // Get DECT parameters
    printf("Getting DECT DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("DECT DB start address: \t 0x%X\n", u32_ULE_dect_start);

    // Get ULE DECT parameters
    printf("Getting DECT DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_end, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("DECT DB end address: \t 0x%X\n", u32_ULE_dect_end);

    // Get data from file
	e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA, pu8_filename_DECT, NULL);

    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
       printf("Failed to read DECT data from file %s into buffer\n", pu8_filename_DECT);
       u32_ULE_dect_size = 0;
       return;
     }


    // Setting DECT Data
    printf("Setting DECT Data... \n");

    // Check the DECT DB size
    if ( u32_ULE_dect_size )
    {
		app_ParamAreaSetBySegments(u32_ULE_dect_start, u32_ULE_dect_size, union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Fail !");
    }


    // restore ULE Data
    printf("Restoring ULE Data... \n");

    // Get ULE  parameters
    printf("Getting ULE DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("ULE DB start address: \t 0x%X\n", u32_ULE_start);


    // Get ULE parameters
    printf("Getting ULE  DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_end, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("ULE DB end address: \t 0x%X\n", u32_ULE_end);

    // Get data from file
	e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA, pu8_filename_ULE, NULL);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
        printf("Failed to read ULE data from file %s into buffer\nExiting...", pu8_filename_ULE);
        u32_ULE_size = 0;
        return;
     }

    // Setting ULE Data
    printf("Setting ULE Data... \n");
    if ( u32_ULE_size )
    {
		app_ParamAreaSetBySegments(u32_ULE_start, u32_ULE_size, union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Fail !");
    }

    // restore FUN Data
    printf("Restoring FUN Data... \n");

    // Get FUN parameters
    printf("Getting FUN DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_start, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("FUN DB start address: \t 0x%X\n", u32_FUN_start);


    // Get FUN parameters
    printf("Getting FUN  DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_end, st_Container.ch_Info, CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("FUN DB end address: \t 0x%X\n", u32_FUN_end);

    // Get data from file
	e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA, pu8_filename_FUN, NULL);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
          printf("Failed to read FUN data from file %s into buffer\nExiting...", pu8_filename_FUN);
          u32_FUN_size = 0;
          return;
    }

    // Setting FUN Data
    printf("Setting FUN Data... \n");
    if ( u32_FUN_size )
    {
		app_ParamAreaSetBySegments(u32_FUN_start, u32_FUN_size, union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Fail !");
    }

    // restore ULE next TPUI
    printf("\nRestoring ULE Next TPUI... \n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, pu8_ULENextTPUI, sizeof(pu8_ULENextTPUI), 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)


    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");
    printf("Getting SUBS Data... done.\n");
    printf("Getting ULE HAN DB address...done\n");
    printf("Performing EEPROM reset... done.\n");
    printf("Restoring SUBS Data... done.\n");
    printf("Restoring ULE Subscription Data... done.\n");
    printf("Restoring RFPI... done.\n");
    printf("Restoring ULE HAN DB ...done.\n");

    for (s8_i = 20; s8_i >= 0; --s8_i)
    {
        printf("\rWaiting for target to finish writing to EEPROM... %02d", s8_i);
        SleepMs(1000);
    }

    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");
    printf("Getting SUBS Data... done.\n");
    printf("Performing EEPROM reset... done.\n");
    printf("Restoring SUBS Data... done.\n");
    printf("Restoring RFPI... done.\n");
    printf("Restoring ULE Subscription Data... done.\n");

    // system restart
    printf("Restarting system... \n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvSystemReboot();

	appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    tcx_appClearScreen();
    printf("Getting Current RFPI... done.\n");
    printf("Getting SUBS Data... done.\n");
    printf("Performing EEPROM reset... done.\n");
    printf("Restoring SUBS Data... done.\n");
    printf("Restoring RFPI... done.\n");
    printf("Restoring ULE Subscription Data... done.\n");
    printf("Restarting system... done.\n");

    printf("All done! press any key...");

    // restore parser state
    app_set_msgparserEnabled(msgparserEnabled);

    tcx_fileDelete((char *)pu8_filename_DECT);
    tcx_fileDelete((char *)pu8_filename_ULE);
    tcx_fileDelete((char *)pu8_filename_FUN);

    tcx_getch();
}




///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//This is utility function for getting input line from the user

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
static  E_CMBS_RetVal getLine(u8 *pu8_prmpt, u8 *pu8_buff, u32 pu8_sz)
{

    u32 u32_ch, u32_extra;

    // Get line with buffer overrun protection
    if ( pu8_prmpt != NULL )
    {
        printf("%s", pu8_prmpt);
        fflush(stdout);
    }

    if ( fgets((char *)pu8_buff, pu8_sz, stdin) == NULL )
        return NO_INPUT;

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if ( pu8_buff[strlen((char *)pu8_buff) - 1] != '\n' )
    {
        u32_extra = 0;
        while (((u32_ch = getchar()) != '\n') && (u32_ch != (u32)EOF)) u32_extra = 1;
        return (u32_extra == 1) ? TOO_LONG : OK;
    }

    // Otherwise remove newline and give string back to caller.
    pu8_buff[strlen((char *)pu8_buff) - 1] = '\0';
    return OK;
}



u8 keyb_FxsSelect(void)
{
	int n_Keep = TRUE;
	u8 ret = 0xFF;

	while (n_Keep)
	{

		printf("\n enter channel number (0,1):\n");
		printf("0 => Channel 0\n");
		printf("1 => Channel 1\n");
		printf("- - - - - - - - - - - - - - - \n");
		printf("q => Return to Interface Menu\n");
		

		switch (tcx_getch())
		{
//		case ' ':
//			tcx_appClearScreen();
//			break;

		case '0':
			ret = 0;
            n_Keep = FALSE;
			break;
		case '1':
			ret = 1;
            n_Keep = FALSE;
            break;
		case 'q':
			ret = 0xFF;
            n_Keep = FALSE;
            break;
		default:
			printf("Selected wrong channel! Only 0 or 1 are available:\n");
			ret = 0xFF;
			break;

		}
	}
	return ret;
}






void keypb_LineTestStart(void)
{
    u8 u8_channel = 0;

	u8_channel = keyb_FxsSelect();

	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvLineTestStart(u8_channel);
	}
}

void keypb_LineTestStop(void)
{
	u8 u8_channel = 0;
	
	u8_channel = keyb_FxsSelect();
	
 	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvLineTestStop(u8_channel);
	}
}

void keypb_NltCapTestStart(void)
{
    u8 u8_channel = 0;

	u8_channel = keyb_FxsSelect();

	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvNltCapTestStart(u8_channel);
	}
}

void keypb_NltCapTestStop(void)
{
	u8 u8_channel = 0;
	
	u8_channel = keyb_FxsSelect();
	
 	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvNltCapTestStop(u8_channel);
	}
}
void keypb_FxsRingTestStart(void)
{
	u8 u8_channel = 0;

	u8_channel = keyb_FxsSelect();

	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvFxsRingTestStart(u8_channel);
	}
}

void keypb_FxsRingTestStop(void)
{
	u8 u8_channel = 0;

	u8_channel = keyb_FxsSelect();

	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvFxsRingTestStop(u8_channel);
	}
}

void keypb_FxsOpenLoopReq(void)
{
	u8 u8_channel = 0;

	u8_channel = keyb_FxsSelect();

	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvFxsOpenLoopReq(u8_channel);
	}
}

void keyb_BBD_Update(void)
{
    //Ask for path of BBD file
    //temporary use fixed location
    

    //Read from BBD file and send to target
   app_SrvBBDUpdate();
}

void keyb_FxsHookStatus(void)
{
	u8 u8_channel = 0;
	
	u8_channel = keyb_FxsSelect();
	
	if (u8_channel == 0 || u8_channel == 1)
	{
		app_SrvFxsStatusReq(u8_channel);
	}
}

void keyb_FXS_Service_Menu(void)
{
    int n_Keep = TRUE;

    while (n_Keep)
    {
        //      tcx_appClearScreen();
        printf("-----------------------------\n");
        printf("Choose service:\n");
        printf("1 => Line Test Start\n");
        printf("2 => Line Test Stop\n");
        printf("3 => BBD update\n");
		printf("4 => Get FXS Hook Status\n");
		printf("5 => Fxs Ring Test Start\n");
		printf("6 => Fxs Ring Test Stop\n");
		printf("7 => Fxs Set Param\n");
		printf("8 => Fxs Open Loop\n");
        printf("9 => Nlt Capacitance Test Start\n");
        printf("0 => Nlt Capacitance Test Stop\n");

        printf("- - - - - - - - - - - - - - - \n");
        printf("q => Return to Interface Menu\n");

        switch (tcx_getch())
        {
        case ' ':
            tcx_appClearScreen();
            break;

        case '1':
            keypb_LineTestStart();
            break;
        case '2':
            keypb_LineTestStop();
            break;
        case '3':
            keyb_BBD_Update();
            break;		
		case '4':
			keyb_FxsHookStatus();
			break;
		case '5':
			 keypb_FxsRingTestStart();
			 break;
		case '6':
			 keypb_FxsRingTestStop();
			 break;
		case '7':
			 keypb_FxsParamSet();
			 break; 
		case '8':
			 keypb_FxsOpenLoopReq();
			 break;
		case '9':
            		keypb_NltCapTestStart();
            		break;
        	case '0':
            		keypb_NltCapTestStop();
            		break;

        case 'q':
            n_Keep = FALSE;
            break;
        default:
            break;

        }
    }
}

void keyb_SetDateTimeValidFlag(void)
{
    int n_Keep = TRUE;

    while (n_Keep)
    {
        printf("-----------------------------\n");
        printf("Has Valid Date Time %s :\n", (app_SrvGetValidDateTime()) ? "Yes" : "No");
        printf("Choose flag:\n");
        printf("1 => Set date time valid\n");
        printf("2 => Set date time invalid\n");

        printf("- - - - - - - - - - - - - - - \n");
        printf("q => Return to Service Menu\n");

        switch (tcx_getch())
        {
        case ' ':
            tcx_appClearScreen();
            break;

        case '1':
            app_SrvSetValidDateTime(1);
            break;
        case '2':
            app_SrvSetValidDateTime(0);
            break;

        case 'q':
            n_Keep = FALSE;
            break;
        default:
            break;

        }
    }
}

//		========== keyb_ULEDevicesAutoSubscription ===========
/*!
\brief         Get subscribed Data from binary files, updates subscribed Data from files
\param[in,out]  <none>
\return void
*/
void keyb_ULEDevicesAutoSubscription(void)
{
    E_CMBS_RC e_status;
    E_CMBS_RetVal e_statusLine;
    ST_APPCMBS_CONTAINER st_Container;
    u8 pu8_RFPI[CMBS_PARAM_RFPI_LENGTH] = { 0 };
    u8 pu8_ULENextTPUI[CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH] = { 0 };
    u32 u32_ULE_dect_start, u32_ULE_dect_end, u32_ULE_dect_size;
    u32 u32_ULE_start, u32_ULE_end, u32_ULE_size;
    u32 u32_FUN_start, u32_FUN_end, u32_FUN_size;
    u8 pu8_path_RFPI[MAX_PATH_SIZE], pu8_path_TPUI[MAX_PATH_SIZE], pu8_path_DECT[MAX_PATH_SIZE],
       pu8_path_ULE[MAX_PATH_SIZE], pu8_path_FUN[MAX_PATH_SIZE];
    u8 pu8_buff[MAX_PATH_SIZE];
    u32 u32_fileStatus;
    FILE *f_binFile = 0;

	UnionHanEEPROMBackupEBlocksPools union_HanEEPROMBackupBlocks;

    // Binary file names
    const u8 pu8_filename_RFPI[] = "RFPI.bin"; //RFPI file name
    const u8 pu8_filename_DECT[] = "DECT.bin"; //DECT file name
    const u8 pu8_filename_ULE[] = "ULE.bin"; //ULE file name
    const u8 pu8_filename_FUN[] = "FUN.bin"; //FUN file name
    const u8 pu8_filename_TPUI[] = "Next_TPUI.bin"; //Next TPUI file name

    // Clear screen
    tcx_appClearScreen();

    // Get directory from the user
    e_statusLine = getLine((u8 *)"Enter a directory path for the location of the binary files > ", pu8_buff, sizeof(pu8_buff));

    switch (e_statusLine)
    {
    case NO_INPUT:
        printf("\nError: path is empty!\nExiting...");
        tcx_getch();
        return;
    case TOO_LONG:
        printf("\nError: path is too long!\nExiting...");
        tcx_getch();
        return;
    default:
        break;
    }

    // Get RFPI.bin full path
    pu8_path_RFPI[0] = '\0';
    strncat((char *)pu8_path_RFPI, (const char *)pu8_buff, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_RFPI) + 1);
    strncat((char *)pu8_path_RFPI, (const char *)pu8_filename_RFPI, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_RFPI) + 1);

    // Get DECT.bin full path
    pu8_path_DECT[0] = '\0';
    strncat((char *)pu8_path_DECT, (const char *)pu8_buff, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_DECT) + 1);
    strncat((char *)pu8_path_DECT, (const char *)pu8_filename_DECT, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_DECT) + 1);

    // Get ULE.bin full path
    pu8_path_ULE[0] = '\0';
    strncat((char *)pu8_path_ULE, (const char *)pu8_buff, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_ULE) + 1);
    strncat((char *)pu8_path_ULE, (const char *)pu8_filename_ULE, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_ULE) + 1);

    // Get FUN.bin full path
    pu8_path_FUN[0] = '\0';
    strncat((char *)pu8_path_FUN, (const char *)pu8_buff, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_FUN) + 1);
    strncat((char *)pu8_path_FUN, (const char *)pu8_filename_FUN, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_FUN) + 1);

    // Get Next_TPUI.bin full path
    pu8_path_TPUI[0] = '\0';
    strncat((char *)pu8_path_TPUI, (const char *)pu8_buff, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_TPUI) + 1);
    strncat((char *)pu8_path_TPUI, (const char *)pu8_filename_TPUI, strlen((char *)pu8_buff) + strlen((char *)pu8_filename_TPUI) + 1);

    printf("\nUsing Files:\n");
    printf("============\n%s\n%s\n%s\n%s\n%s\n", pu8_path_RFPI, pu8_path_DECT, pu8_path_ULE, pu8_path_FUN, pu8_path_TPUI);

    u32_fileStatus = 1;

    // Check binary files exist
    e_status = tcx_fileOpen(&f_binFile, pu8_path_RFPI, (u8 *)"r+b");
    if ( e_status != CMBS_RC_OK )
    {
        u32_fileStatus = 0;
        printf("\nError: Can't Open File: %s", pu8_path_RFPI);
    }
    else
    {
        tcx_fileClose(f_binFile);
    }

    e_status = tcx_fileOpen(&f_binFile, pu8_path_DECT, (u8 *)"r+b");
    if ( e_status != CMBS_RC_OK )
    {
        u32_fileStatus = 0;
        printf("\nError: Can't Open File: %s", pu8_path_DECT);
    }
    else
    {
        tcx_fileClose(f_binFile);
    }

    e_status = tcx_fileOpen(&f_binFile, pu8_path_ULE, (u8 *)"r+b");
    if ( e_status != CMBS_RC_OK )
    {
        u32_fileStatus = 0;
        printf("\nError: Can't Open File: %s", pu8_path_ULE);
    }
    else
    {
        tcx_fileClose(f_binFile);
    }

    e_status = tcx_fileOpen(&f_binFile, pu8_path_FUN, (u8 *)"r+b");
    if ( e_status != CMBS_RC_OK )
    {
        u32_fileStatus = 0;
        printf("\nError: Can't Open File: %s", pu8_path_FUN);
    }
    else
    {
        tcx_fileClose(f_binFile);
    }

    e_status = tcx_fileOpen(&f_binFile, pu8_path_TPUI, (u8 *)"r+b");
    if ( e_status != CMBS_RC_OK )
    {
        u32_fileStatus = 0;
        printf("\nError: Can't Open File: %s", pu8_path_TPUI);
    }
    else
    {
        tcx_fileClose(f_binFile);
    }

    // Exit in case at least one file could not be opened
    if ( !u32_fileStatus )
    {
        printf("\n\nAborting...");
        printf("\nPress any key to exit...");
        tcx_getch();
        printf("\n\n");
        return;
    }

    // Get current RFPI from file
    printf("\nGetting RFPI from file - %s...\n", pu8_path_RFPI);
    e_status = app_ParamAreaGetFromFile(pu8_RFPI, pu8_path_RFPI, NULL);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
        printf("Failed to read RFPI data from file %s into buffer\nExiting...", pu8_path_RFPI);
        return;
    }

    // Setting RFPI
    printf("Setting RFPI... \n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_RFPI, pu8_RFPI, sizeof(pu8_RFPI), 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // Handle ULE Auto Subscription

    // Get ULE DECT parameters
    printf("\n********************************************\n");
    printf("\nGetting ULE DECT DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("DECT DB start address: \t 0x%X\n", u32_ULE_dect_start);

    // Get ULE DECT parameters
    printf("\nGetting DECT DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_DECT_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_dect_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("DECT DB end address: \t 0x%X\n", u32_ULE_dect_end);

    // Calculate the needed size
    u32_ULE_dect_size = u32_ULE_dect_end - u32_ULE_dect_start;

    printf("DECT DB size: \t 0x%X\n", u32_ULE_dect_size);
    printf("\n********************************************\n");

    printf("\nGetting DECT block from file - %s...\nDone!", pu8_path_DECT);

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA[0], 0, HAN_DECT_SUBS_LENGTH);

    // Get data from file
    if ( u32_ULE_dect_size <= HAN_DECT_SUBS_LENGTH )
    {
		e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA, pu8_path_DECT, NULL);

        if ( e_status == CMBS_RC_ERROR_GENERAL )
        {
            printf("Failed to read DECT data from file %s into buffer\n", pu8_path_DECT);
            u32_ULE_dect_size = 0;
            return;
        }
    }
    else
    {
        printf("Error in DECT DB size - received 0x%X\n", u32_ULE_dect_size);
        u32_ULE_dect_size = 0;
    }


    // Setting DECT Data
    printf("Setting DECT Data... \n");

    // Check the DECT DB size
    if ( u32_ULE_dect_size )
    {
		app_ParamAreaSetBySegments(u32_ULE_dect_start, u32_ULE_dect_size, union_HanEEPROMBackupBlocks.pu8_HAN_DECT_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Fail !");
    }


    // Get ULE  parameters
    printf("\nGetting ULE DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("ULE DB start address: \t 0x%X\n", u32_ULE_start);

    // Get ULE parameters
    printf("Getting ULE  DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_ULE_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_ULE_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("ULE DB end address: \t 0x%X\n", u32_ULE_end);

    //Calculate the needed size
    u32_ULE_size = u32_ULE_end - u32_ULE_start;

    printf("ULE DB size: \t 0x%X\n", u32_ULE_size);
    printf("\n********************************************\n");

    printf("\nGetting ULE block from file - %s...\n", pu8_path_ULE);

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA[0], 0, HAN_ULE_SUBS_LENGTH);

    // Get data from file
    if ( u32_ULE_size <= HAN_ULE_SUBS_LENGTH )
    {
		e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA, pu8_path_ULE, NULL);
        if ( e_status == CMBS_RC_ERROR_GENERAL )
        {
            printf("Failed to read ULE data from file %s into buffer\nExiting...", pu8_path_ULE);
            u32_ULE_size = 0;
            return;
        }
    }
    else
    {
        printf("Error in ULE  DB size - received 0x%X\n", u32_ULE_size);
        u32_ULE_size = 0;
    }

    // Setting ULE Data
    printf("Setting ULE Data... \n");
    if ( u32_ULE_size )
    {
		app_ParamAreaSetBySegments(u32_ULE_start, u32_ULE_size, union_HanEEPROMBackupBlocks.pu8_HAN_ULE_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Fail !");
    }

    // Get FUN parameters
    printf("Getting FUN DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_start, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("FUN DB start address: \t 0x%X\n", u32_FUN_start);

    // Get FUN parameters
    printf("Getting FUN  DB address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_HAN_FUN_SUB_DB_END, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    memcpy(&u32_FUN_end, st_Container.ch_Info,  CMBS_PARAM_HAN_DB_ADDR_LENGTH);
    printf("FUN DB end address: \t 0x%X\n", u32_FUN_end);

    //Calculate the needed size
    u32_FUN_size = u32_FUN_end - u32_FUN_start;

    printf("HAN FUN DB size: \t 0x%X\n", u32_FUN_size);
    printf("\n********************************************\n");

    printf("\nGetting FUN block from file - %s...\n", pu8_path_FUN);

	memset(&union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA[0], 0, HAN_FUN_SUBS_LENGTH);

    // Get data from file
    if ( u32_FUN_size <= HAN_FUN_SUBS_LENGTH )
    {
		e_status = app_ParamAreaGetFromFile(union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA, pu8_path_FUN, NULL);
        if ( e_status == CMBS_RC_ERROR_GENERAL )
        {
            printf("Failed to read FUN data from file %s into buffer\nExiting...", pu8_path_FUN);
            u32_FUN_size = 0;
            return;
        }
    }
    else
    {
        printf("Error in FUN DB size - received 0x%X\n", u32_FUN_size);
        u32_FUN_size = 0;
    }


    // Setting FUN Data
    printf("Setting FUN Data... \n");
    if ( u32_FUN_size )
    {
		app_ParamAreaSetBySegments(u32_FUN_start, u32_FUN_size, union_HanEEPROMBackupBlocks.pu8_HAN_FUN_DATA, g_CMBSMaxTransferSize);
    }
    else
    {
        printf("Fail !");
    }

    // Get next ULE TPUI from file
    printf("Getting Next TPUI from file - %s...\n", pu8_path_TPUI);
    e_status = app_ParamAreaGetFromFile(pu8_ULENextTPUI, pu8_path_TPUI, NULL);
    if ( e_status == CMBS_RC_ERROR_GENERAL )
    {
        printf("Failed to read ULE Next TPUI from file %s into buffer\nExiting...", pu8_path_TPUI);
        return;
    }


    // Setting ULE next TPUI
    printf("Setting ULE Next TPUI... \n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamSet(CMBS_PARAM_HAN_ULE_NEXT_TPUI, pu8_ULENextTPUI, sizeof(pu8_ULENextTPUI), 1);
    appcmbs_WaitForEvent(CMBS_EV_DSR_PARAM_SET_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    printf("\ndone!\n\n");

    // System restart
    printf("\n\nRestarting system... ");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvSystemReboot();

	appcmbs_WaitForEvent(CMBS_EV_DSR_SYS_START_RES);  // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    printf("done.\n\n");

    tcx_appClearScreen();
    printf("All done! press any key to exit...");

    tcx_getch();
}


void keyb_DisplaySubscriptionData(void)
{
    ST_APPCMBS_CONTAINER st_Container;
    ST_CMBS_SUBS_DATA    Subs_Data[CMBS_HS_SUBSCRIBED_MAX_NUM];
    ST_CMBS_SUBS_DATA_EX Subs_DataEx[CMBS_HS_SUBSCRIBED_MAX_NUM];
    ST_CMBS_SUBS_DATA    RepeaterSubs_Data[CMBS_CRFP_SUBSCRIBED_MAX_NUM];
    u8 u8_j;
    u8 u8_i;
    u32 u32_Repeater_end, u32_Repeater_start, u32_Repeater_size = 0;

    // save parser state as we want to disable it temporary
    u8 msgparserEnabled = app_get_msgparserEnabled();
    app_set_msgparserEnabled(0);

    // Get current subscribe data
    printf("\nGetting SUBS Data... \n");

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_SUBS_DATA, 1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // copy to internal buffer
    memcpy(Subs_Data, st_Container.ch_Info, st_Container.n_InfoLen);

    // Get current extended subscribe data
    printf("\nGetting SUBS Data EX... \n");
    
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
  
    // Get current Extended subscribe data
    app_SrvParamGet(CMBS_PARAM_SUBS_DATA_EX, 1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    // copy to internal buffer
    memcpy(Subs_DataEx, st_Container.ch_Info, st_Container.n_InfoLen);
      
    // Get current Repeater subscribe data
    printf("Getting Repeater start address...\n");
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_START, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
                  
    // if ( st_Container.ch_Info[0] != CMBS_RESPONSE_OK )
    if ( st_Container.ch_Info[0] == CMBS_RESPONSE_ERROR )
    {
        printf("\n !! Repeater support not enabled in target SW !!!\n");
    }
    else
    {
        memcpy(&u32_Repeater_start, st_Container.ch_Info,  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH);
        
        printf("Repeater subscription start address: \t 0x%X\n", u32_Repeater_start);
        
        printf("Getting Repeater subscription end address...\n");
	    appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        
        app_SrvParamGet(CMBS_PARAM_REPEATER_SUBS_END, 1);
        appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        
        memcpy(&u32_Repeater_end, st_Container.ch_Info,  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH);
        
	    memset(&RepeaterSubs_Data, 0, CMBS_CRFP_SUBSCRIBED_MAX_NUM * sizeof(ST_CMBS_SUBS_DATA));
        
        //Calculate the needed size
        u32_Repeater_size = u32_Repeater_end - u32_Repeater_start;
        
        //Get data from Target
        if (u32_Repeater_size <= (CMBS_CRFP_SUBSCRIBED_MAX_NUM * sizeof(ST_CMBS_SUBS_DATA)))
        {
  	        app_ParamAreaGetBySegments(u32_Repeater_start, u32_Repeater_size, (u8 *) &RepeaterSubs_Data, g_CMBSMaxTransferSize);
        }
    }
          
    // Clear screen
    tcx_appClearScreen();

    // go over the buffer and parse data
    for (u8_j = 0; u8_j < CMBS_HS_SUBSCRIBED_MAX_NUM; ++u8_j)
    {
        if ( Subs_Data[u8_j].u8_SUB_STATUS == 1 ) // if the hadnset is subscribed
        {
            printf("\nSubscription Data for HS number %d\n", (Subs_Data[u8_j].u8_HANDSET_NR) & 0x0F);
            printf("\nIPUI:	");
            for (u8_i = 0; u8_i < CMBS_IPUI_LEN; ++u8_i)
            {
                printf("%02X ", Subs_Data[u8_j].pu8_IPUI[u8_i]);
            }

            printf("\nTPUI:	");
            for (u8_i = 0; u8_i < CMBS_TPUI_LEN; ++u8_i)
            {
                printf("%02X ", Subs_Data[u8_j].pu8_TPUI[u8_i]);
            }

            printf("\nDCK:	");
            for (u8_i = 0; u8_i < CMBS_DCK_LEN; ++u8_i)
            {
                printf("%02X ", Subs_Data[u8_j].pu8_DCK[u8_i]);
            }

            printf("\nUAK:	");
            for (u8_i = 0; u8_i < CMBS_UAK_LEN; ++u8_i)
            {
                printf("%02X ", Subs_Data[u8_j].pu8_UAK[u8_i]);
            }

            printf("\nAuthentication code	");
            for (u8_i = 0; u8_i < CMBS_AC_LEN; ++u8_i)
            {
                printf("%02X ", Subs_Data[u8_j].pu8_AC[u8_i]);
            }

            printf("\n\n UAK authorization:	%02X", Subs_Data[u8_j].u8_UAK_AUTH);
            printf("\n Status:	%02X", Subs_Data[u8_j].u8_SUB_STATUS);
            printf("\n HS number:	%02X", (Subs_Data[u8_j].u8_HANDSET_NR) & 0x0F);
            printf("\n DCK assigned:	%02X", Subs_Data[u8_j].u8_DCK_ASSIGNED);
            printf("\n Cipher Key Length:	%02X", Subs_Data[u8_j].u8_CK_LEN);
            printf("\n Handset ULE feature support:	%02X\n", Subs_Data[u8_j].u8_FEATURES);
            printf("\n Default CK assigned:	%02X\n", Subs_DataEx[u8_j].u8_DCKAssigned);
            printf("\n---------------------\n");
        }
    }
    
    if (u32_Repeater_size)
    {
        for (u8_j = 0; u8_j < CMBS_CRFP_SUBSCRIBED_MAX_NUM; ++u8_j)
        {
            if ( RepeaterSubs_Data[u8_j].u8_SUB_STATUS == 1 ) // if the hadnset is subscribed
            {
                printf("\nSubscription Data for Repeater ecord %d\n", u8_j);
                printf("\nIPUI:	");
                for (u8_i = 0; u8_i < CMBS_IPUI_LEN; ++u8_i)
                {
                    printf("%02X ", RepeaterSubs_Data[u8_j].pu8_IPUI[u8_i]);
                }

                printf("\nTPUI:	");
                for (u8_i = 0; u8_i < CMBS_TPUI_LEN; ++u8_i)
                {
                    printf("%02X ", RepeaterSubs_Data[u8_j].pu8_TPUI[u8_i]);
                }

                printf("\nDCK:	");
                for (u8_i = 0; u8_i < CMBS_DCK_LEN; ++u8_i)
                {
                    printf("%02X ", RepeaterSubs_Data[u8_j].pu8_DCK[u8_i]);
                }

                printf("\nUAK:	");
                for (u8_i = 0; u8_i < CMBS_UAK_LEN; ++u8_i)
                {
                    printf("%02X ", RepeaterSubs_Data[u8_j].pu8_UAK[u8_i]);
                }

                printf("\nAuthentication code	");
                for (u8_i = 0; u8_i < CMBS_AC_LEN; ++u8_i)
                {
                    printf("%02X ", RepeaterSubs_Data[u8_j].pu8_AC[u8_i]);
                }

                printf("\n\n UAK authorization:	%02X", RepeaterSubs_Data[u8_j].u8_UAK_AUTH);
                printf("\n Status:	%02X", RepeaterSubs_Data[u8_j].u8_SUB_STATUS);
                printf("\n HS number:	%02X", (RepeaterSubs_Data[u8_j].u8_HANDSET_NR) & 0x0F);
                printf("\n DCK assigned:	%02X", RepeaterSubs_Data[u8_j].u8_DCK_ASSIGNED);
                printf("\n Cipher Key Length:	%02X", RepeaterSubs_Data[u8_j].u8_CK_LEN);
                printf("\n Handset ULE feature support:	%02X\n", RepeaterSubs_Data[u8_j].u8_FEATURES);
                printf("\n---------------------\n");
            }
        }
    }


    tcx_getch();
    // restore parser state
    app_set_msgparserEnabled(msgparserEnabled);

}

//      ========== keypb_LineSettingsGet ===========
/*!
\brief         Get List of Lines settings
\param[in,ou]  <none>
\return

*/
void  keypb_LineSettingsGet(void)
{
    ST_APPCMBS_CONTAINER        st_Container;
    ST_IE_RESPONSE              st_Resp;
    u16                         u16_LinesMask = 0;

    memset(&st_Container, 0, sizeof(st_Container));

    printf("\nEnter Mask of lines in hex (FF for all): ");
    tcx_scanf("%hX", &u16_LinesMask);

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvLineSettingsGet(u16_LinesMask, 1);

    // wait for CMBS target message
    appcmbs_WaitForContainer(CMBS_EV_DSR_LINE_SETTINGS_LIST_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

    memcpy(&st_Resp, &st_Container.ch_Info, st_Container.n_InfoLen);

    printf("\nLines settings list get %s\n", st_Resp.e_Response == CMBS_RESPONSE_OK ? "Success" : "Fail");
    printf("Press Any Key!\n");
    tcx_getch();
}


//      ========== keypb_LineSettingsSet ===========
/*!
\brief         Set List of Lines settings
\param[in,ou]  <none>
\return

*/
void  keypb_LineSettingsSet(void)
{
    u16 u16_Line_Id = 0, u16_Temp;
    ST_IE_LINE_SETTINGS_LIST    st_LineSettingsList;
    memset(&st_LineSettingsList, 0, sizeof(st_LineSettingsList));

    /* Line ID */
    printf("\nEnter line ID (in decimal): ");
    tcx_scanf("%hd", &u16_Line_Id);
    st_LineSettingsList.u8_Line_Id = (u8)u16_Line_Id;

    /* Attahced HS Mask */
    printf("\nEnter Hs mask in hex: ");
    tcx_scanf("%hX", &st_LineSettingsList.u16_Attached_HS);

    /* Call intrusion en / dis */
    printf("\nCall Intrusion: Enter 0 - disabled, 1 - enabled: ");
    tcx_scanf("%hd", &u16_Temp);
    st_LineSettingsList.u8_Call_Intrusion = (u8)u16_Temp;

    /* Multicall mode en / dis */
    printf("\nMulticall mode: Enter number of parallel calls (0 = multicalls disabled): ");
    tcx_scanf("%hd", &u16_Temp);
    st_LineSettingsList.u8_Multiple_Calls = (u8)u16_Temp;

    /* PSTN / VoIP Line */
    printf("\nLine Type: Enter\n0 - PSTN_DOUBLE_CALL\n1 - PSTN_PARALLEL_CALL\n2 - VOIP_DOUBLE_CALL\n3 - VOIP_PARALLEL_CALL: ");
    tcx_scanf("%hd", &u16_Temp);
    st_LineSettingsList.e_LineType = (E_CMBS_LINE_TYPE)u16_Temp;

    app_SrvLineSettingsSet(&st_LineSettingsList, 1);
}

void  keyb_AddPhoneABToContactList(void)
{
    stContactListEntry st_Entry;
    u32 pu32_FiledIDs[5], u32_EntryId;
    char s_PhoneBCNIP[] = { 0x50, 0x68, 0x6F, 0x6E, 0x65, 0x20, 0x42, 0x20, 0xC3, 0xA9, 0xC3, 0xA0, 0xC3, 0xBC, 0x00 }; //Phone B + (UTF-8 characters)

    pu32_FiledIDs[0] = FIELD_ID_LAST_NAME;
    pu32_FiledIDs[1] = FIELD_ID_FIRST_NAME;
    pu32_FiledIDs[2] = FIELD_ID_CONTACT_NUM_1;
    pu32_FiledIDs[3] = FIELD_ID_ASSOCIATED_MELODY;
    pu32_FiledIDs[4] = FIELD_ID_LINE_ID;

    strcpy(st_Entry.sLastName, "Phone A");
    strcpy(st_Entry.sFirstName, "x");
    strcpy(st_Entry.sNumber1, "121");
    st_Entry.cNumber1Type = NUM_TYPE_FIXED;
    st_Entry.bNumber1Default = TRUE;
    st_Entry.bNumber1Own = FALSE;
    st_Entry.u32_AssociatedMelody = 1;
    st_Entry.u32_LineId = 0;

    /* Insert Phone A */
    List_InsertEntry(LIST_TYPE_CONTACT_LIST, &st_Entry, pu32_FiledIDs, 5, &u32_EntryId);

    strcpy(st_Entry.sLastName, s_PhoneBCNIP);
    strcpy(st_Entry.sNumber1, "120");

    /* Insert Phone B */
    List_InsertEntry(LIST_TYPE_CONTACT_LIST, &st_Entry, pu32_FiledIDs, 5, &u32_EntryId);
}


void  keyb_AddLine0ToLineSettingsList(void)
{
    stLineSettingsListEntry st_Entry;
    u32 pu32_FiledIDs[12], u32_EntryId;

    pu32_FiledIDs[0] = FIELD_ID_LINE_NAME;
    pu32_FiledIDs[1] = FIELD_ID_LINE_ID;
    pu32_FiledIDs[2] = FIELD_ID_ATTACHED_HANDSETS;
    pu32_FiledIDs[3] = FIELD_ID_FP_MELODY;
    pu32_FiledIDs[4] = FIELD_ID_FP_VOLUME;
    pu32_FiledIDs[5] = FIELD_ID_BLOCKED_NUMBER;
    pu32_FiledIDs[6] = FIELD_ID_MULTIPLE_CALLS_MODE;
    pu32_FiledIDs[7] = FIELD_ID_INTRUSION_CALL;
    pu32_FiledIDs[8] = FIELD_ID_PERMANENT_CLIR;
    pu32_FiledIDs[9] = FIELD_ID_CALL_FWD_UNCOND;
    pu32_FiledIDs[10] = FIELD_ID_CALL_FWD_BUSY;
    pu32_FiledIDs[11] = FIELD_ID_CALL_FWD_NO_ANSWER;

    strcpy(st_Entry.sLineName, "Line-0");
    st_Entry.u32_LineId = 0;
    st_Entry.u32_AttachedHsMask = 0x03;
    st_Entry.u32_FPMelody = 1;
    st_Entry.u32_FPVolume = 1+0x30;
    strcpy(st_Entry.sBlockedNumber, "555");
    st_Entry.bMultiCalls = TRUE;
    st_Entry.bIntrusionCall = TRUE;
    st_Entry.bPermanentCLIR = FALSE;
    strcpy(st_Entry.sPermanentCLIRActCode, "333");
    strcpy(st_Entry.sPermanentCLIRDeactCode, "444");
    st_Entry.bCallFwdUncond = FALSE;
    strcpy(st_Entry.sCallFwdUncondActCode, "555");
    strcpy(st_Entry.sCallFwdUncondDeactCode, "666");
    strcpy(st_Entry.sCallFwdUncondNum, "123456");
    st_Entry.bCallFwdNoAns = FALSE;
    strcpy(st_Entry.sCallFwdNoAnsActCode, "888");
    strcpy(st_Entry.sCallFwdNoAnsDeactCode, "999");
    strcpy(st_Entry.sCallFwdNoAnsNum, "654321");
    st_Entry.u32_CallFwdNoAnsTimeout = 30;
    st_Entry.bCallFwdBusy = FALSE;
    strcpy(st_Entry.sCallFwdBusyActCode, "777");
    strcpy(st_Entry.sCallFwdBusyDeactCode, "222");
    strcpy(st_Entry.sCallFwdBusyNum, "98765432");

    List_InsertEntry(LIST_TYPE_LINE_SETTINGS_LIST, &st_Entry, pu32_FiledIDs, 12, &u32_EntryId);

    /* Need to update target with Line Settings */
    {
        ST_IE_LINE_SETTINGS_LIST st_LineSettingsList;

        st_LineSettingsList.u16_Attached_HS     = (u16)st_Entry.u32_AttachedHsMask;
        st_LineSettingsList.u8_Call_Intrusion   = st_Entry.bIntrusionCall;
        st_LineSettingsList.u8_Line_Id          = (u8)st_Entry.u32_LineId;
        st_LineSettingsList.u8_Multiple_Calls   = st_Entry.bMultiCalls ? CMBS_MULTICALL_LINE_NUM_OF_CALLS : 0;
        st_LineSettingsList.e_LineType          = CMBS_LINE_TYPE_VOIP_PARALLEL_CALL;

        app_SrvLineSettingsSet(&st_LineSettingsList, 1);
    }
}


//		========== keypb_SYSTestModeGet ===========
/*!
\brief         Get current CMBS Target test mode state
\param[in,ou]  <none>
\return

*/
void  keypb_SYSTestModeGet(void)
{
    app_SrvTestModeGet(TRUE);
}

//		========== keypb_SYSTestModeSet ===========
/*!
\brief         enable TBR 6 mode
\param[in,ou]  <none>
\return

*/
void  keypb_SYSTestModeSet(void)
{
    app_SrvTestModeSet();
}
//////////////////////////////////////////////////////////////////////////
void keyb_StartCmbsLogger(void)
{
    char strFileName[100];
    char strHostFileName[105] = {0};
    char * ptr = NULL;
	
    if ( tcx_IsLoggerEnabled() )
    {
        printf("Logger already started\n");
        return;
    }
    printf("Enter log file name:\n");
    tcx_scanf("%s", strFileName);
    if ( tcx_LogOpenLogfile(strFileName) )
    {
        printf("Logger started successfully\n");
    }
    else
    {
        printf("Failed to open log file \n");
    }

    strcpy(strHostFileName, strFileName);
	ptr = strchr(strHostFileName, '.');
    strHostFileName[ptr - strHostFileName] = 0;
    strcat(strHostFileName, "_Host_Log.txt");
	
    if ( tcx_HostLogOpenLogfile(strHostFileName) )
    {
        printf("Host Logger started successfully: %s\n", strHostFileName);
    }
    else
    {
        printf("Failed to open Host log file \n");
    }
}
//////////////////////////////////////////////////////////////////////////
u8 g_u8_temp;

void keyb_StopCmbsLogger(void)
{
    tcx_LogCloseLogfile();
    tcx_HostLogCloseLogfile();
    printf("CMBS logger stopped \n");
}

void keyb_EnableAllLogLevels( void )
{
	tcx_EnableAllLogLevels();
}

void keyb_DisableAllLogLevels( void )
{
	tcx_DisableAllLogLevels();
}

void keyb_ConfigLogLevelForModule( bool b_Enable )
{
	u8 u8_LogLevel = 0, u8_ModuleId = 0, u8_Index = 0;
	
	printf("\n\nEnter Module Id (hex):\n[");
	
	for(u8_Index = 0; u8_Index < MAX_MODULE; u8_Index++)
		{
			if(u8_Index == MAX_MODULE-1)
				{
					printf("%s=0x%X]\n", tcx_GetModuleName((E_MODULE_ID) u8_Index), u8_Index);
				}
			else
				{
					printf("%s=0x%X\n", tcx_GetModuleName((E_MODULE_ID) u8_Index), u8_Index);
				}
		}
	
	tcx_scanf("%hhx",&g_u8_temp);
	u8_ModuleId = g_u8_temp;

	printf("\n\nEnter Log Level (hex):\n");
	printf("[%s = 0x%X\n", tcx_GetLogLevelName(LOG_LEVEL_INFO), LOG_LEVEL_INFO);
	printf("%s = 0x%X\n", tcx_GetLogLevelName(LOG_LEVEL_TRACE), LOG_LEVEL_TRACE);
	printf("%s = 0x%X\n", tcx_GetLogLevelName(LOG_LEVEL_WARNING), LOG_LEVEL_WARNING);
	printf("%s = 0x%X\n", tcx_GetLogLevelName(LOG_LEVEL_ERROR), LOG_LEVEL_ERROR);
	printf("%s = 0x%X]\n", tcx_GetLogLevelName(LOG_LEVEL_REAL_TIME), LOG_LEVEL_REAL_TIME);
	
	tcx_scanf("%hhx",&g_u8_temp);
	u8_LogLevel = g_u8_temp;
	
	tcx_ConfigLogLevelForApp(u8_ModuleId, u8_LogLevel, b_Enable);
}



void keyb_PrintLogLevelForModule( void )
{
	u8 u8_ModuleId = 0, u8_LogLevelMask = 0, u8_Index = 0;

	printf("\n\nEnter Module Id (hex):\n");

	for(u8_Index = 0; u8_Index < MAX_MODULE; u8_Index++)
		{
			if(u8_Index == MAX_MODULE-1)
				{
					printf("%s=0x%X]\n", tcx_GetModuleName((E_MODULE_ID) u8_Index), u8_Index);
				}
			else
				{
					printf("%s=0x%X\n", tcx_GetModuleName((E_MODULE_ID) u8_Index), u8_Index);
				}
		}
	
	tcx_scanf("%hhx",&g_u8_temp);
	u8_ModuleId = g_u8_temp;

	u8_LogLevelMask = tcx_GetLogLevelConfigForApp(u8_ModuleId);
	
	printf("\n--------------------------------------\n");
	printf("Log Level for Module %s:\n", tcx_GetModuleName(u8_ModuleId));
	printf("\n================================\n");

	printf("%-7s is %s\n", tcx_GetLogLevelName(LOG_LEVEL_INFO), LOG_LEVEL_INFO & u8_LogLevelMask ? "Enabled" : "Disabled");
	printf("%-7s is %s\n", tcx_GetLogLevelName(LOG_LEVEL_TRACE), LOG_LEVEL_TRACE & u8_LogLevelMask ? "Enabled" : "Disabled");
	printf("%-7s is %s\n", tcx_GetLogLevelName(LOG_LEVEL_WARNING), LOG_LEVEL_WARNING & u8_LogLevelMask ? "Enabled" : "Disabled");
	printf("%-7s is %s\n", tcx_GetLogLevelName(LOG_LEVEL_ERROR), LOG_LEVEL_ERROR & u8_LogLevelMask ? "Enabled" : "Disabled");
	printf("%-7s is %s\n", tcx_GetLogLevelName(LOG_LEVEL_REAL_TIME), LOG_LEVEL_REAL_TIME & u8_LogLevelMask ? "Enabled" : "Disabled");
	printf("--------------------------------------\n\n");
}
	
//////////////////////////////////////////////////////////////////////////
void keyb_HostLogLevelMenu( void )
{
    u8 u8_Ans = 0;

    tcx_appClearScreen();
    while (u8_Ans != 'q')
    {
        printf("\n-----------------------------------------\n");
        printf("\nChoose Logger option, 'q' to exit menu: \n");
        printf("1 => Enable All Log Levels \n");
        printf("2 => Disable All Log Levels \n");
        printf("3 => Enable Log Level for Module \n");
        printf("4 => Disable Log Level for Module \n");
        printf("5 => Get Log Level for Module \n");		

	printf("- - - - - - - - - - - - - - - \n");
        printf("q => Return to Interface Menu\n");
        
        u8_Ans = tcx_getch();
    
        switch (u8_Ans)
        {

	case 'x':
	case 'X':
	    tcx_appClearScreen();
		break;
		
        case '1':
		keyb_EnableAllLogLevels();
            break;
            
        case '2':
            keyb_DisableAllLogLevels();
            break;
            
        case '3':
		keyb_ConfigLogLevelForModule(TRUE);
            break;
            
        case '4':
		keyb_ConfigLogLevelForModule(FALSE);
            break;
            
        case '5':
		keyb_PrintLogLevelForModule();
            break;
			
        default:
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void keyb_CmbsLoggerOptions(void)
{
    u8 u8_Ans = 0;
    while (u8_Ans != 'q')
    {
        printf("\n-----------------------------------------\n");
        printf("\nChoose Logger option, 'q' to exit menu: \n");
        printf("1 => Logger output only to Terminal \n");
        printf("2 => Logger output only to Log File \n");
        printf("3 => Logger output both to Terminal and Log File \n");
        printf("4 => No Logger output \n");
        printf("5 => Stop Logger output to Log file and close Log File \n");
	printf("6 => Host Log Level Menu\n");

	printf("- - - - - - - - - - - - - - - \n");
        printf("q => Return to Interface Menu\n");
        
        u8_Ans = tcx_getch();
    
        switch (u8_Ans)
        {
        case '1':
            g_u8_LogOutput = CMBS_LOG_OUTPUT_TERMINAL;
			keyb_StopCmbsLogger();
            keyb_DisableAllLogLevels();
            break;
            
        case '2':
            g_u8_LogOutput = CMBS_LOG_OUTPUT_LOGFILE;
            keyb_StartCmbsLogger();
            break;
            
        case '3':
            g_u8_LogOutput = CMBS_LOG_OUTPUT_BOTH;
            keyb_StartCmbsLogger();
            break;
            
        case '4':
            g_u8_LogOutput = CMBS_LOG_OUTPUT_NONE;
 		    keyb_StopCmbsLogger();
            keyb_DisableAllLogLevels();
            break;
            
        case '5':
            g_u8_LogOutput = CMBS_LOG_OUTPUT_TERMINAL; //CMBS_LOG_OUTPUT_TERMINAL is the default
            keyb_StopCmbsLogger();
            keyb_DisableAllLogLevels();
            break;

        case '6':
              keyb_HostLogLevelMenu();
			
        default:
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void              keyb_DECTLoggerStart(void)
{
    app_LogStartDectLogger();
}
//////////////////////////////////////////////////////////////////////////
void              keyb_DECTLoggerStop(void)
{
    app_LogStoptDectLoggerAndRead();
}

void keyb_DataCall_CallEstablish(void)
{
    u16 u16_HSNumber;
    u32 u32_CallInstance;
    printf("\n Enter Destination HS number\n");
    tcx_scanf("%hd", &u16_HSNumber);
    u32_CallInstance = cmbs_dee_CallInstanceNew(g_cmbsappl.pv_CMBSRef);
    printf("\n Assigned Call Instance is 0x%X", u32_CallInstance);
    printf("\n Press any key to continue \n");
    tcx_getch();
    cmbs_dsr_dc_SessionStart(g_cmbsappl.pv_CMBSRef, u16_HSNumber, u32_CallInstance);
}
void keyb_DataCall_CallConclude(void)
{
    u16 u16_HSNumber;
    u32 u32_CallInstance;
    printf("\n Enter Destination HS number\n");
    tcx_scanf("%hd", &u16_HSNumber);
    printf("\n Enter Call Instance (in hex format)\n");
    tcx_scanf("%x", &u32_CallInstance);
    cmbs_dsr_dc_SessionStop(g_cmbsappl.pv_CMBSRef, u16_HSNumber, u32_CallInstance);
}

void keyb_DataCall_SendData(void)
{
    u32 u32_CallInstance;
    u8 pu8_IWUData[1000] = { 0 }; //two ASCII characters consume one byte
    u8 pu8_IWUData_HEX[CMBS_IWU_DATA_MAX_TRANSMIT_LENGTH];
    u16 u16_DataLength;
    u16 u16_HSNumber;
    u8 u8_NumOfMessages = 0;
    u16 u16_total_len;
    st_DataCall_Header st_Header;
    int i;

    printf("\n Enter Call Instance  (in hex format)\n");
    tcx_scanf("%x", &u32_CallInstance);
    printf("\n Enter HS number\n");
    tcx_scanf("%hd", &u16_HSNumber);
    printf("\n Enter Hex Data (maximum 500 bytes)\n");
    tcx_gets((char *)pu8_IWUData, sizeof(pu8_IWUData));
    u16_DataLength = strlen((char *)pu8_IWUData) / 2;
    printf("u16_DataLength = %d\n", u16_DataLength);
    if ( u16_DataLength <= CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH )
    {
        u8_NumOfMessages = 1;
    }
    else
    {
        u8_NumOfMessages = (u16_DataLength - CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH) / CMBS_IWU_DATA_MAX_NEXT_PAYLOAD_LENGTH + 1;
        if ( u16_DataLength % CMBS_IWU_DATA_MAX_NEXT_PAYLOAD_LENGTH )
            u8_NumOfMessages++;
    }

    printf("u8_NumOfMessages = %d\n", u8_NumOfMessages);

    // Prepare header
    st_Header.u8_prot_disc = PROTOCOL_DISCRIMINATOR;
    st_Header.u8_desc_type = DESCRIMINATOR_TYPE;
    st_Header.u8_EMC_high = EMC_HIGH;
    st_Header.u8_EMC_low = EMC_LOW;
    st_Header.u8_command = FIRST_PACKET_COMMAND;
    st_Header.u8_length = (MIN(u16_DataLength,  CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH) + 4); //data length + 4 bytes reserved for protocol discriminator, descriptor type and EMC
    u16_total_len = u16_DataLength + 2 + u8_NumOfMessages * 6;
    printf("u16_total_len = %d\n", u16_total_len);
    st_Header.u8_total_len_low = (u16_total_len)&0x00FF;
    st_Header.u8_total_len_high = (u16_total_len << 8) & 0xFF00;
    memcpy(pu8_IWUData_HEX, &st_Header, HEADER_SIZE);

    for (i = 0; i < (CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH * 2); i += 2)
    {
        pu8_IWUData_HEX[i / 2 + HEADER_SIZE] = app_ASC2HEX((char *)pu8_IWUData + i);
    }

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_dc_DataSend(g_cmbsappl.pv_CMBSRef, u16_HSNumber, u32_CallInstance, pu8_IWUData_HEX, st_Header.u8_length + 4);
    appcmbs_WaitForEvent(CMBS_EV_DSR_DC_DATA_SEND_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    u16_DataLength -= CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH;
    u8_NumOfMessages--;
    while (u8_NumOfMessages > 0)
    {
        st_Header.u8_command = NEXT_PACKET_COMMAND;
        st_Header.u8_length = (MIN(u16_DataLength,  CMBS_IWU_DATA_MAX_NEXT_PAYLOAD_LENGTH)) + 2;
        memcpy(pu8_IWUData_HEX, &st_Header, HEADER_SIZE - 2);
        for (i = 0; i < (CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH * 2); i += 2)
        {
            pu8_IWUData_HEX[i / 2 + HEADER_SIZE - 2] = app_ASC2HEX((char *)pu8_IWUData + i);
        }
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        cmbs_dsr_dc_DataSend(g_cmbsappl.pv_CMBSRef, u16_HSNumber, u32_CallInstance, pu8_IWUData_HEX, st_Header.u8_length + 4);
        appcmbs_WaitForEvent(CMBS_EV_DSR_DC_DATA_SEND_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
        u16_DataLength -= CMBS_IWU_DATA_MAX_FIRST_PAYLOAD_LENGTH;
        u8_NumOfMessages--;
    }
}

void keyb_DataCall_StressTest(void)
{
    u16 u16_HSNumber;
    u32 u32_CallInstance;
    u16 u16_Delay;
    u16 u16_Iterations;
    u16 u16_i;
    u8 pu8_IWUData_HEX[CMBS_IWU_DATA_MAX_TRANSMIT_LENGTH] =
    {
        0xC0, 0x81, 0x03, 0x04, 0x37, 0x31, 0x00, 0x3B,
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A,
        0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
        0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x61, 0x62, 0x63, 0x64,
        0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
        0x6F, 0x70, 0x61, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79
    };

    printf("\n Enter Destination HS number\n");
    tcx_scanf("%hd", &u16_HSNumber);
    printf("\n Enter delay (in msec) between transmissions\n");
    tcx_scanf("%hd", &u16_Delay);
    printf("\n Enter number of iterations\n");
    tcx_scanf("%hd", &u16_Iterations);

    u32_CallInstance = cmbs_dee_CallInstanceNew(g_cmbsappl.pv_CMBSRef);
    printf("\n Assigned Call Instance is 0x%X", u32_CallInstance);
    printf("\n Press any key to continue \n");

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_dc_SessionStart(g_cmbsappl.pv_CMBSRef, u16_HSNumber, u32_CallInstance);

    appcmbs_WaitForEvent(CMBS_EV_DSR_DC_SESSION_START_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)


    for (u16_i = 0; u16_i < u16_Iterations; u16_i++)
    {
        SleepMs(u16_Delay);
		appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
        cmbs_dsr_dc_DataSend(g_cmbsappl.pv_CMBSRef, u16_HSNumber, u32_CallInstance, pu8_IWUData_HEX, CMBS_IWU_DATA_MAX_TRANSMIT_LENGTH);

        // wait for CMBS target message
        appcmbs_WaitForEvent(CMBS_EV_DSR_DC_DATA_SEND_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    }

    cmbs_dsr_dc_SessionStop(g_cmbsappl.pv_CMBSRef, u16_HSNumber, u32_CallInstance);

}

void keypb_DataCallLoop(void)
{
    u8 u8_Ans = 0;
    tcx_appClearScreen();
    while (u8_Ans != 'q')
    {
        printf("Data Call Operation\n");
        printf("=====================\n\n");
        printf("Select option:\n");
        printf("1: Establish Data Call\n");
        printf("2: Conclude Data Call\n");
        printf("3: Send Data\n");
        printf("4: Stress Test Data Call\n ");
        printf("\n Press 'q' to return to menu\n");
        printf("\n");
        u8_Ans = tcx_getch();
        switch (u8_Ans)
        {
        case '1':
            keyb_DataCall_CallEstablish();
            break;
        case '2':
            keyb_DataCall_CallConclude();
            break;
        case '3':
            keyb_DataCall_SendData();
            break;
        case '4':
            keyb_DataCall_StressTest();
            break;
        default:
            printf("\nError parameter !\n");
            break;
        }
    }
    tcx_appClearScreen();
}

void keyb_AFESelectSlotAndCodec(void)
{
    char c_Codec = 0;
    u32 StartSlot = 0;
    char tmp[5];

    printf("Enter start slot:\n");

    tcx_gets(tmp, sizeof(tmp));
    StartSlot = (u32)atoi(tmp);


    printf("Enter codec ('n' for LINEAR NB or 'w' for LINEAR WB) ):\n");
    tcx_scanf("%c", &c_Codec);

    if ( (c_Codec == 'n') || (c_Codec == 'N') )
    {
        AFEconfig.e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_NB;
        AFEconfig.u32_SlotMask = 0x3 << StartSlot;

    }
    else if ( (c_Codec == 'w') || (c_Codec == 'W') )
    {
        AFEconfig.e_Codec = CMBS_AUDIO_CODEC_PCM_LINEAR_WB;
        AFEconfig.u32_SlotMask = 0xF << StartSlot;
    }

    g_u32_UsedSlots |= AFEconfig.u32_SlotMask;

    printf("\nEnter resource type: '0' for DAC/ADC, '1' for DCLASS: \n");
    tcx_scanf("%hhX", &AFEconfig.u8_Resource);
}

void keyb_AFEAllocateChannel(void)
{
    appsrv_AFEAllocateChannel();
}

void keyb_AFESelectEndpoints(void)
{
    u32 u32_temp;
    u8 u8_Offset = 0;
    u8 u8_Endpoint = 0;
    printf("\nChoose Audio sink: \n");
    printf("0.\tAMP_OUT0\n");
    printf("1.\tAMP_OUT1\n");
    printf("2.\tAMP_OUT2\n");
    printf("3.\tAMP_OUT3\n");
    //	printf("4.\tCMBS_AFE_AMP_SINGIN0\n");
    //	printf("5.\tCMBS_AFE_AMP_SINGIN1\n");
    //	printf("6.\tCMBS_AFE_AMP_DIFFIN0\n");
    //	printf("7.\tCMBS_AFE_AMP_DIFFIN1\n");
    printf("4.\tADC0\n");
    printf("5.\tADC1\n");

    tcx_scanf("%u", &u32_temp);
    AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = u32_temp;
    u8_Endpoint = AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT;
    if ( AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT >= 4 )
    {
        AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT += 4;
    }
    AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT += CMBS_AFE_AMP_START; // add CMBS_AFE_AMP_START to each chosen number

    printf("\nChoose Audio source: \n");
    //	printf("0.\tCMBS_AFE_CHANNEL_IN_SINGIN0\n");
    //	printf("1.\tCMBS_AFE_CHANNEL_IN_SINGIN1\n");
    if ( u8_Endpoint >= 4 )
    {
        printf("0.\tDIFFIN0\n");
        printf("1.\tDIFFIN1\n");
        printf("2.\tSINGIN0_MINUS_SINGIN1\n");
        printf("3.\tVREF_OFFSET\n");
        printf("4.\tMUTE_VREF\n");
        printf("5.\tMUTE_FLOAT\n");
        printf("6.\tDIGMIC1\n");
        printf("7.\tDIGMIC2\n");
        u8_Offset = CMBS_AFE_ADC_IN_DIFFIN0;
    }
    else
    {
        printf("0.\tDAC0\n");
        printf("1.\tDAC1\n");
        printf("2.\tDAC0_INV\n");
        printf("3.\tDAC1_INV\n");
        printf("4.\tSINGIN\n");
        printf("5.\tDIFFIN0\n");
        printf("6.\tDIFFIN1\n");
        printf("7.\tMUTE\n");
        u8_Offset = CMBS_AFE_AMPOUT_IN_START;
    }
    tcx_scanf("%u", &u32_temp);
    AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = u32_temp;
    AFEconfig.st_AFEEndpoints.e_AFEEndPointIN += u8_Offset;     //CMBS_AFE_ADC_IN_START  CMBS_AFE_AMPOUT_IN_START

    app_AFEConnectEndpoints();
}
#define CMBS_AFE_AMP_END_OFFSET	7
#define CMBS_AFE_MIC_START_OFFSET	(CMBS_AFE_AMP_END_OFFSET + 1)
#define CMBS_AFE_MIC_END_OFFSET	9
#define CMBS_AFE_DIGMIC_START_OFFSET	(CMBS_AFE_MIC_END_OFFSET + 1)
#define CMBS_AFE_DIGMIC_END_OFFSET	11

void keyb_AFE_EnableDisableEndpoints(void)
{
    ST_IE_AFE_ENDPOINT st_Endpoint;
    u16 u16_Enable = 0;

    memset(&st_Endpoint, 0, sizeof(st_Endpoint));

    printf("\nChoose AFE end point: \n");
    printf("0. AMP_OUT0\n");
    printf("1. AMP_OUT1\n");
    printf("2. AMP_OUT2\n");
    printf("3. AMP_OUT3\n");
    printf("4. SINGIN0\n");
    printf("5. SINGIN1\n");
    printf("6. DIFFIN0\n");
    printf("7. DIFFIN1\n");
    printf("8. MIC0\n");
    printf("9. MIC1\n");
    printf("10. DIGMIC on ADC0\n");
    printf("11. DIGMIC on ADC1\n");

    tcx_scanf("%hd", &u16_Enable);
    st_Endpoint.e_AFEChannel = u16_Enable;

    if ( st_Endpoint.e_AFEChannel <= CMBS_AFE_AMP_END_OFFSET )
    {
        st_Endpoint.e_AFEChannel += CMBS_AFE_AMP_START;
    }
    else if ( (st_Endpoint.e_AFEChannel > CMBS_AFE_AMP_END_OFFSET) && (st_Endpoint.e_AFEChannel <= CMBS_AFE_MIC_END_OFFSET) )
    {
        st_Endpoint.e_AFEChannel += (CMBS_AFE_MIC_START - CMBS_AFE_MIC_START_OFFSET);
    }
    else if ( (st_Endpoint.e_AFEChannel >= CMBS_AFE_DIGMIC_START_OFFSET) && (st_Endpoint.e_AFEChannel <= CMBS_AFE_DIGMIC_END_OFFSET) )
    {
        st_Endpoint.e_AFEChannel += (CMBS_DIGMIC_START - CMBS_AFE_DIGMIC_START_OFFSET);
    }
    printf("\nChoose '1' to Enable or '0' to disable: \n");
    tcx_scanf("%hd", &u16_Enable);

    app_AFEEnableDisableEndpoint(&st_Endpoint, u16_Enable);


}


void keyb_AFESetEndpointGain(void)
{
    ST_IE_AFE_ENDPOINT_GAIN 	st_AFEEndpointGain;
    u16 u16_Selection;
    u16 u16_tmp;

    printf("\nChoose '1' to set Input gain or '0' to set Output gain: \n");
    tcx_scanf("%hd", &u16_Selection);

    printf("Choose endpoint: \n");
    printf("0.\tAMP_OUT0\n");
    printf("1.\tAMP_OUT1\n");
    printf("2.\tAMP_OUT2\n");
    printf("3.\tAMP_OUT3\n");
    printf("4.\tSINGIN0\n");
    printf("5.\tSINGIN1\n");
    printf("6.\tDIFFIN0\n");
    printf("7.\tDIFFIN1\n");
    tcx_scanf("%hd", &u16_tmp);
    st_AFEEndpointGain.e_AFEChannel = u16_tmp + CMBS_AFE_AMP_START;

    if ( u16_Selection == TRUE )
    {
        //Select Input gain
        printf("\nSelect Input gain:\n");
        printf("0.\tMINUS_4DB\n");
        printf("1.\tMINUS_2DB\n");
        printf("2.\t0DB\n");
        printf("3.\t11KRIN\n");
        printf("4.\t2DB\n");
        printf("5.\t4DB\n");
        printf("6.\t6DB\n");
        printf("7.\t22_5KRIN\n");
        printf("8.\t8DB\n");
        printf("9.\t10DB\n");
        printf("10.\t12B\n");
        printf("11.\t45KRIN\n");
        printf("12.\t14DB\n");
        printf("13.\t16DB\n");
        printf("14.\t18DB\n");
        printf("15.\t90KRIN\n");
        printf("16.\t20DB\n");
        printf("17.\t22DB\n");
        printf("18.\t24DB\n");
        printf("19.\t180KRIN\n");
        printf("20.\t26DB\n");
        printf("21.\t28DB\n");
        printf("22.\t30DB\n");
        printf("23.\t360KRIN\n");
        printf("24.\t32DB\n");
        printf("25.\t34DB\n");
        printf("26.\t36DB\n");
        printf("27.\t720KRIN\n");
        printf("28.\tOLOOP1\n");
        printf("29.\tOLOOP2\n");
        printf("30.\tOLOOP3\n");
        printf("31.\tOLOOP4\n");

        tcx_scanf("%hd", &u16_tmp);
        st_AFEEndpointGain.s16_NumOfSteps = u16_tmp;

    }
    else
    {
        // Select Output gain
        printf("\nSelect Output gain:\n");
        printf("0.0DB\n");
        printf("1.MINUS_6DB\n");
        printf("2.MINUS_12DB\n");
        printf("3.MINUS_18DB\n");
        printf("4.MINUS_24DB\n");
        tcx_scanf("%hd", &u16_tmp);
        st_AFEEndpointGain.s16_NumOfSteps = u16_tmp + CMBS_AFE_OUT_GAIN_START;
    }

    appsrv_AFESetEndpointGain(&st_AFEEndpointGain, u16_Selection);
}
void keyb_AFEAUXMeasurement(void)
{
    u16 scanfInput = 0;
    ST_IE_AFE_AUX_MEASUREMENT_SETTINGS	st_AFEMeasurementSettings;

    printf("\nEnter AUX Input to measure  (in HEX): ");
    tcx_scanf("%hX", &scanfInput);
    st_AFEMeasurementSettings.e_AUX_Input = (E_CMBS_AUX_INPUT)scanfInput;

    //	printf("\nEnter '1' to measure via BMP, '0' to measure manually: ");
    st_AFEMeasurementSettings.b_Bmp = 0;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    cmbs_dsr_AFE_AUXMeasurement(g_cmbsappl.pv_CMBSRef, &st_AFEMeasurementSettings);

    appcmbs_WaitForEvent(CMBS_EV_DSR_AFE_AUX_MEASUREMENT_RES); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
}

void keyb_AFEOpenAudioChannel(void)
{
    appsrv_AFEOpenAudioChannel();
}

void keyb_AFECloseAudioChannel(void)
{
    u32 u32_ChannelID;
    u32 HSID;

    printf("Enter Channel ID:\n");
    tcx_scanf("%X", &u32_ChannelID);

    g_u32_UsedSlots &= ~(AFEconfig.u32_SlotMask);

    appsrv_AFECloseAudioChannel(u32_ChannelID);

    if ( u32_ChannelID != 0xF2 ) //for DCLASS there are no EP's to close
    {
        printf("\nTo Disable All related Endpoints, please enter HS number [1, 2, 3 (for HandsFree)] or 0 for skipping\n");
        tcx_scanf("%d", &HSID);

        keyb_DiableEPFast((u8)HSID);
    }

}

static void keyb_SetAFEFast(E_CMBS_AUDIO_CODEC codec, u32 slotMask, u8 resource, u8 CordedHSID)
{
    ST_IE_AFE_ENDPOINT st_Endpoint;
    E_CMBS_AFE_CHANNEL AMPOUT_IN;
    E_CMBS_AFE_CHANNEL ADCID;

    AFEconfig.e_Codec = codec;
    AFEconfig.u32_SlotMask = slotMask;
    g_u32_UsedSlots |= AFEconfig.u32_SlotMask;
    AFEconfig.u8_Resource = resource;



    appsrv_AFEAllocateChannel();

    if ( AFEconfig.u8_InstanceNum == 0 )
    {
        AMPOUT_IN = CMBS_AFE_AMPOUT_IN_DAC0;
        ADCID = CMBS_AFE_ADC0;
    }
    else
    {
        AMPOUT_IN = CMBS_AFE_AMPOUT_IN_DAC1;
        ADCID = CMBS_AFE_ADC1;
    }

    if ( CordedHSID == 0 )
    {
        AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = CMBS_AFE_AMP_OUT0;
        AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = AMPOUT_IN;
        app_AFEConnectEndpoints();

        AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = CMBS_AFE_AMP_OUT1;
        AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = (E_CMBS_AFE_CHANNEL)(AMPOUT_IN + 2);
        app_AFEConnectEndpoints();

        AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = ADCID;
        AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = CMBS_AFE_ADC_IN_DIFFIN1;
        app_AFEConnectEndpoints();



        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT0;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT1;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_DIFFIN1;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);

        st_Endpoint.e_AFEChannel = CMBS_AFE_MIC0;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);
    }
    else
    {
        AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = CMBS_AFE_AMP_OUT2;
        AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = AMPOUT_IN;
        app_AFEConnectEndpoints();

        AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = CMBS_AFE_AMP_OUT3;
        AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = (E_CMBS_AFE_CHANNEL)(AMPOUT_IN + 2);
        app_AFEConnectEndpoints();

        AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = ADCID;
        AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = CMBS_AFE_ADC_IN_DIFFIN0;
        app_AFEConnectEndpoints();



        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT2;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT3;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_DIFFIN0;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);

        st_Endpoint.e_AFEChannel = CMBS_AFE_MIC1;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 1);

    }


    appsrv_AFEOpenAudioChannel();


}

static void keyb_DiableEPFast(u8 HSID)
{
    ST_IE_AFE_ENDPOINT st_Endpoint;


    if ( HSID == 1 )
    {
        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT0;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT1;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_DIFFIN1;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

        st_Endpoint.e_AFEChannel = CMBS_AFE_MIC0;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);
    }
    else if ( HSID == 2 )
    {
        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT2;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_OUT3;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

        st_Endpoint.e_AFEChannel = CMBS_AFE_AMP_DIFFIN0;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

        st_Endpoint.e_AFEChannel = CMBS_AFE_MIC1;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

    }
    else if ( HSID == 3 ) // Handsfree ( DIGMIC + DCLASS)
    {
        st_Endpoint.e_AFEChannel = CMBS_DIGMIC_ON_CHANNEL1;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);

        st_Endpoint.e_AFEChannel = CMBS_DIGMIC_ON_CHANNEL2;
        app_AFEEnableDisableEndpoint(&st_Endpoint, 0);
    }
}

static void keyb_SetDIGMICFast(E_CMBS_AUDIO_CODEC codec, u32 slotMask, u8 DigmicID)
{
    ST_IE_AFE_ENDPOINT st_Endpoint;
    E_CMBS_AFE_CHANNEL ADCID;

    AFEconfig.e_Codec = codec;
    AFEconfig.u32_SlotMask = slotMask;
    g_u32_UsedSlots |= AFEconfig.u32_SlotMask;

    // First connect the DIGMIC to ADC
    AFEconfig.u8_Resource = 0; // DAC/ADC


    appsrv_AFEAllocateChannel();

    if ( AFEconfig.u8_InstanceNum == 0 )
    {
        ADCID = CMBS_AFE_ADC0;
    }
    else
    {
        ADCID = CMBS_AFE_ADC1;
    }


    AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = ADCID;
    AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = DigmicID - 1 + CMBS_AFE_ADC_IN_DIGMIC1;
    app_AFEConnectEndpoints();


    st_Endpoint.e_AFEChannel = (ADCID == CMBS_AFE_ADC0) ? CMBS_DIGMIC_ON_CHANNEL1 : CMBS_DIGMIC_ON_CHANNEL2;
    app_AFEEnableDisableEndpoint(&st_Endpoint, 1);



    appsrv_AFEOpenAudioChannel();

}

static void keyb_SetDCLASSFast(E_CMBS_AUDIO_CODEC codec, u32 slotMask)
{

    AFEconfig.e_Codec = codec;
    AFEconfig.u32_SlotMask = slotMask;
    g_u32_UsedSlots |= AFEconfig.u32_SlotMask;


    AFEconfig.u8_Resource = 1; // DAC/ADC
    appsrv_AFEAllocateChannel();
    appsrv_AFEOpenAudioChannel();


}



static void keyb_SetHandsFreeFast(E_CMBS_AUDIO_CODEC codec, u32 slotMask, u8 DigmicID)
{
    ST_IE_AFE_ENDPOINT st_Endpoint;
    E_CMBS_AFE_CHANNEL ADCID;

    AFEconfig.e_Codec = codec;
    AFEconfig.u32_SlotMask = slotMask;
    g_u32_UsedSlots |= AFEconfig.u32_SlotMask;

    // First connect the DIGMIC to ADC
    AFEconfig.u8_Resource = 0; // DAC/ADC


    appsrv_AFEAllocateChannel();

    if ( AFEconfig.u8_InstanceNum == 0 )
    {
        ADCID = CMBS_AFE_ADC0;
    }
    else
    {
        ADCID = CMBS_AFE_ADC1;
    }


    AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = ADCID;
    AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = DigmicID - 1 + CMBS_AFE_ADC_IN_DIGMIC1;
    app_AFEConnectEndpoints();


    st_Endpoint.e_AFEChannel = (ADCID == CMBS_AFE_ADC0) ? CMBS_DIGMIC_ON_CHANNEL1 : CMBS_DIGMIC_ON_CHANNEL2;
    app_AFEEnableDisableEndpoint(&st_Endpoint, 1);



    appsrv_AFEOpenAudioChannel();

    SleepMs(2000);

    // Second step, commect the DCLASS
    AFEconfig.u8_Resource = 1; // DAC/ADC
    if ( CMBS_AUDIO_CODEC_PCM_LINEAR_WB == codec )
    {
        AFEconfig.u32_SlotMask = 0xF00;
    }
    else
    {
        AFEconfig.u32_SlotMask = 0xC00;
    }
    appsrv_AFEAllocateChannel();
    appsrv_AFEOpenAudioChannel();


}

void keyb_AFEDHSGValueSend(void)
{
    u8  u8_Value;
    u32 u32_Value;

    printf("\nEnter byte value:\n");

    tcx_scanf("%X", &u32_Value);
    u8_Value = (u8)u32_Value;

    appsrv_AFEDHSGSendByte(u8_Value);
}

void keyb_AFEDHSGEnable(void)
{
    u8  u8_Value;
    u32 u32_Value;


    do
    {
        printf("\nSelect 1-Enable 0-Ignore:\n");
        tcx_scanf("%d", &u32_Value);
        u8_Value = (u8)u32_Value;
    }while ((u32_Value != 0) && (u32_Value != 1));

    if ( u8_Value )
    {
        app_SrvParamSet(CMBS_PARAM_DHSG_ENABLE, &u8_Value, CMBS_PARAM_DHSG_ENABLE_LENGTH, 1);
    }
}



void keyb_GPIOControl(void)
{
    u32 u32_temp;
    u8 u8_Selection;
    ST_IE_GPIO_ID st_GPIOId;
    tcx_appClearScreen();

    memset(&st_GPIOId, 0, sizeof(ST_IE_GPIO_ID));

    printf("\nSelect action:\n");
    printf("1.\tEnable GPIO\n");
    printf("2.\tDisable GPIO\n");
    printf("3.\tControl GPIO Settings\n");
    printf("4.\tGet GPIO Settings\n");
    u8_Selection = tcx_getch();

    switch (u8_Selection)
    {
    case '1':
        {
            printf("\nEnter GPIO BANK (0 - 4) [A=0...E=4]: \n");
            tcx_scanf("%u", &u32_temp);
            st_GPIOId.e_GPIOBank = u32_temp;
            if ( st_GPIOId.e_GPIOBank >= CMBS_GPIO_BANK_MAX )
                printf("ERROR! Invalid GPIO BANK\n");
            printf("\nEnter GPIO #: \n");
            tcx_scanf("%d", &st_GPIOId.u32_GPIO);
            app_SrvGPIOEnable(&st_GPIOId);
        }
        break;
    case '2':
        {
            printf("\nEnter GPIO BANK (0 - 4) [A=0...E=4]: \n");
            tcx_scanf("%u", &u32_temp);
            st_GPIOId.e_GPIOBank = u32_temp;
            if ( st_GPIOId.e_GPIOBank >= CMBS_GPIO_BANK_MAX )
                printf("ERROR! Invalid GPIO BANK\n");
            printf("\nEnter GPIO #: \n");
            tcx_scanf("%d", &st_GPIOId.u32_GPIO);
            app_SrvGPIODisable(&st_GPIOId);
        }
        break;
    case '3':
        {
            ST_GPIO_Properties st_Prop[4];

            printf("\nEnter GPIO BANK (0 - 4) [A=0...E=4]: \n");
            tcx_scanf("%u", &u32_temp);
            st_GPIOId.e_GPIOBank = u32_temp;
            if ( st_GPIOId.e_GPIOBank >= CMBS_GPIO_BANK_MAX )
                printf("ERROR! Invalid GPIO BANK\n");
            printf("Enter GPIO #: \n");
            tcx_scanf("%d", &st_GPIOId.u32_GPIO);
            printf("Enter Mode ('0' for GPIO_MODE_INPUT, '1' for GPIO_MODE_OUTPUT, 'F' to skip configuration)\n");
            tcx_scanf("%u", &u32_temp);
            st_Prop[0].u8_Value = u32_temp;
            st_Prop[0].e_IE = CMBS_IE_GPIO_MODE;
            printf("Enter Pull Type ('0' for GPIO_PULL_DN, '1' for GPIO_PULL_UP, 'F' to skip configuration)\n");
            tcx_scanf("%u", &u32_temp);
            st_Prop[1].u8_Value = u32_temp;
            st_Prop[1].e_IE = CMBS_IE_GPIO_PULL_TYPE;
            printf("Enter Pull Enable ('0' for GPIO_PULL_DIS, '1' GPIO_PULL_ENA, 'F' to skip configuration)\n");
            tcx_scanf("%u", &u32_temp);
            st_Prop[2].u8_Value = u32_temp;
            st_Prop[2].e_IE = CMBS_IE_GPIO_PULL_ENA;
            printf("Enter Value ('0' for DATA CLR, '1' for DATA SET, 'F' to skip configuration)\n");
            tcx_scanf("%u", &u32_temp);
            st_Prop[3].u8_Value = u32_temp;
            st_Prop[3].e_IE = CMBS_IE_GPIO_VALUE;

            app_SrvGPIOSet(&st_GPIOId, st_Prop);
        }
        break;
    case '4':
        {
            ST_GPIO_Properties st_Prop[4];
            u8 u8_Ans = 0;
            u8 u8_Idx;
            for (u8_Idx = 0; u8_Idx < 4; u8_Idx++)
            {
                st_Prop[u8_Idx].e_IE = 0xF;
            }
            printf("Enter GPIO BANK (0 - 4) [A=0...E=4]: \n");
            tcx_scanf("%u", &u32_temp);
            st_GPIOId.e_GPIOBank = u32_temp;
            if ( st_GPIOId.e_GPIOBank >= CMBS_GPIO_BANK_MAX )
                printf("ERROR! Invalid GPIO BANK\n");
            printf("Enter GPIO #: \n");
            tcx_scanf("%d", &st_GPIOId.u32_GPIO);

            tcx_appClearScreen();

            while (u8_Ans != 'q')
            {
                printf("Choose parameters:\n");
                printf("1. GPIO mode (input/output)\n");
                printf("2. GPIO value \n");
                printf("3. GPIO pull type \n");
                printf("4. GPIO pull enable\n");
                printf("Press 'q' to send the message\n ");

                u8_Ans = tcx_getch();

                switch (u8_Ans)
                {
                case '1':
                    st_Prop[0].e_IE = CMBS_IE_GPIO_MODE;
                    break;

                case '2':
                    st_Prop[1].e_IE = CMBS_IE_GPIO_VALUE;
                    break;

                case '3':
                    st_Prop[2].e_IE = CMBS_IE_GPIO_PULL_TYPE;
                    break;

                case '4':
                    st_Prop[3].e_IE = CMBS_IE_GPIO_PULL_ENA;
                    break;

                default:
                    break;
                }
            }
            app_SrvGPIOGet(&st_GPIOId, st_Prop);
        }

        break;

    default:
        break;
    }
}
void keypb_AFESettingsMenu(void)
{
    u8 u8_Ans = 0;
    tcx_appClearScreen();
    memset(&AFEconfig, 0, sizeof(AFEconfig));

    while (u8_Ans != 'q')
    {
        printf("=====================\n");
        printf("Chosen AFE Settings:\n");
        printf("=====================\n\n");

        printf("Preffered Codec: %s	\n", app_GetCodecString(AFEconfig.e_Codec));
        printf("Slot Mask: %2X		\n", g_u32_UsedSlots);
        printf("Resource: %s\n", AFEconfig.u8_Resource == CMBS_AFE_RESOURCE_DACADC ? "DAC/ADC" : "DCLASS");
        printf("Instance Number (received from target): %x\n", AFEconfig.u8_InstanceNum);
        printf("Channel ID (received from target): %x\n", AFEconfig.u32_ChannelID);
        printf("Point: Input: %s	Output: %s	\n", app_GetAFEEndpointString(AFEconfig.st_AFEEndpoints.e_AFEEndPointIN), app_GetAFEEndpointString(AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT));


        printf("=====================\n\n");
        printf("Select option:\n");
        printf("1: Select slot and codec\n");
        printf("2: Allocate channel\n");
        printf("3: Connect AFE Endpoints\n");
        printf("4: Enable or Disable AFE Endpoint\n");
        printf("5: Set Endpoint gain\n");
        printf("6: AFE AUX measurement\n");
        printf("7: AFE Open Audio Channel\n");
        printf("8: AFE Close Audio Channel\n");

        printf("9: Open Conversation 1: WB slot 0-3, HS1 [AMPOUT0 & AMPOUT1 + DIFFIN1]\n");
        printf("A: Open Conversation 2: WB slot 4-7, HS2 [AMPOUT2 & AMPOUT3 + DIFFIN0]\n");
        printf("B: Open Conversation 3: WB slot 2-5, HS2 [AMPOUT2 & AMPOUT3 + DIFFIN0]\n");
        printf("C: Open Conversation 4: NB slot 2-3, HS2 [AMPOUT2 & AMPOUT3 + DIFFIN0]\n");
        printf("D: Open Conversation 5: NB slot 4-5, HS2 [AMPOUT2 & AMPOUT3 + DIFFIN0]\n");
        printf("E: Open Conversation 6: WB slot 4-7, Hands-Free [DCLASS + DIGMIC1]\n");
        printf("F: Open Conversation 7: NB slot 6-7, Hands-Free [DCLASS + DIGMIC2]\n");
        printf("G: Open Conversation 8: NB slot 2-3, DCLASS\n");
        printf("H: Open Conversation 8: NB slot 0-1, DIGMIC1\n");
        printf("N: Incoming NB DECT call\n");

        printf("\n Press 'q' to return to menu\n");
        printf("\n");
        u8_Ans = tcx_getch();
        printf("\n");

        switch (u8_Ans)
        {
        case '1':
            keyb_AFESelectSlotAndCodec();
            break;
        case '2':
            keyb_AFEAllocateChannel();
            break;
        case '3':
            keyb_AFESelectEndpoints();
            break;
        case '4':
            keyb_AFE_EnableDisableEndpoints();
            break;
        case '5':
            keyb_AFESetEndpointGain();
            break;
        case '6':
            keyb_AFEAUXMeasurement();
            break;
        case '7':
            keyb_AFEOpenAudioChannel();
            break;
        case '8':
            keyb_AFECloseAudioChannel();
            break;
        case '9':
            keyb_SetAFEFast(CMBS_AUDIO_CODEC_PCM_LINEAR_WB, 0xF, 0, 0);
            break;
        case 'a':
        case 'A':
            keyb_SetAFEFast(CMBS_AUDIO_CODEC_PCM_LINEAR_WB, 0xF0, 0, 1);
            break;
        case 'b':
        case 'B':
            keyb_SetAFEFast(CMBS_AUDIO_CODEC_PCM_LINEAR_WB, 0x3C, 0, 1);
            break;
        case 'c':
        case 'C':
            keyb_SetAFEFast(CMBS_AUDIO_CODEC_PCM_LINEAR_NB, 0x0C, 0, 1);
            break;
        case 'd':
        case 'D':
            keyb_SetAFEFast(CMBS_AUDIO_CODEC_PCM_LINEAR_NB, 0x30, 0, 1);
            break;
        case 'e':
        case 'E':
            keyb_SetHandsFreeFast(CMBS_AUDIO_CODEC_PCM_LINEAR_WB, 0xF0, 1);
            break;
        case 'f':
        case 'F':
            keyb_SetHandsFreeFast(CMBS_AUDIO_CODEC_PCM_LINEAR_NB, 0xC0, 2);
            break;
        case 'g':
        case 'G':
            keyb_SetDCLASSFast(CMBS_AUDIO_CODEC_PCM_LINEAR_NB, 0x0C);
            break;
        case 'h':
        case 'H':
            keyb_SetDIGMICFast(CMBS_AUDIO_CODEC_PCM_LINEAR_NB, 0x03, 1);
            break;

        case 'n':
        case 'N':
            keyb_IncCallNB();
            break;
        default:
            break;
        }
    }
}


#ifdef CHECKSUM_SUPPORT
//		========== keyb_SrvChecksumTest ===========
/*!
        \brief         keyboard loop for checksum test
      \param[in,ou]  <none>
        \return <none>

*/
void keyb_SrvChecksumTest(void)
{
    int n_Keep = TRUE;
    char c;

    while (n_Keep)
    {
        printf("\n-----------------------------\n");
        printf("Choose Checksum Test:\n");
        printf("1 => Send Event with wrong checksum \n");
        printf("2 => Send Event without checksum\n");
        printf("3 => Send event with distroyed CHECKSUM IE\n\n");

        printf("5 => send CMBS_EV_CHECKSUM_FAILURE with CMBS_CHECKSUM_ERROR\n");
        printf("6 => send CMBS_EV_CHECKSUM_FAILURE with CMBS_CHECKSUM_NO_EVENT_ID\n");

        printf("- - - - - - - - - - - - - - - \n");
        printf("q => Return to Interface Menu\n");

        c = tcx_getch();

        switch (c)
        {
        case '1':
            // Send wrong checksum in send message
        case '2':
            // Send event without checksum
        case '3':
            // Send event with distroyed CHECKSUM IE

            cmbs_int_SimulateChecksumError(c - 0x30);
            // send any event with an IE included
            app_SrvTestModeSet();
            // reset error simulation
            cmbs_int_SimulateChecksumError(0);

            break;

        case '5':
            // send CMBS_EV_CHECKSUM_FAILURE with CMBS_CHECKSUM_ERROR
        case '6':
            // send CMBS_EV_CHECKSUM_FAILURE with CMBS_CHECKSUM_NO_EVENT_ID
            cmbs_int_SimulateChecksumError(c - 0x30);
            break;

        case 'q':
            n_Keep = FALSE;
            break;
        }
    }
}
#endif


void keyb_ExternalInterruptsControl(void)
{
    u32 u32_temp;
    u8 u8_Ans = 0;
    u16 u16_Ans = 0;
    u8 u8_IntNumber = 0;
    ST_IE_INT_CONFIGURATION st_INTConfiguration;
    ST_IE_GPIO_ID st_GPIOId;

    tcx_appClearScreen();

    memset(&st_GPIOId, 0, sizeof(ST_IE_GPIO_ID));
    memset(&st_INTConfiguration, 0, sizeof(ST_IE_INT_CONFIGURATION));


    while (u8_Ans != 'q')
    {
        printf("=====================\n\n");
        printf("Select option:\n");
        printf("1: External Interrupt Configure\n");
        printf("2: External Interrupt Enable\n");
        printf("3: External Interrupt Disable\n");
        printf("\n Press 'q' to return to menu\n");
        printf("\n");
        u8_Ans = tcx_getch();

        switch (u8_Ans)
        {
        case '1':
            {
                printf("\nEnter Interrupt Number:\n");
                tcx_scanf("%hd", &u16_Ans);
                u8_IntNumber = (u8)u16_Ans;

                printf("\nEnter GPIO BANK (0 - 4) [A=0...E=4]:\n");
                tcx_scanf("%u", &u32_temp);
                st_GPIOId.e_GPIOBank = u32_temp;
                if ( st_GPIOId.e_GPIOBank >= CMBS_GPIO_BANK_MAX )
                {
                    printf("ERROR! Invalid GPIO BANK\n");
                    break;
                }
                printf("\nEnter GPIO #: \n");
                tcx_scanf("%d", &st_GPIOId.u32_GPIO);

                printf("\nEnter Interrupt polarity: (active_low=0 / active_high=1)\n");
                tcx_scanf("%hd", &u16_Ans);
                st_INTConfiguration.u8_INTPolarity = (u8)u16_Ans;

                printf("\nEnter Interrupt Type: (Mode: Level=0 / edge=1)\n");
                tcx_scanf("%hd", &u16_Ans);
                st_INTConfiguration.u8_INTType = (u8)u16_Ans;

                app_SrvExtIntConfigure(&st_GPIOId, &st_INTConfiguration, u8_IntNumber);
            }
            break;


        case '2':
            {
                printf("\nEnter Interrupt Number:\n");
                tcx_scanf("%hd", &u16_Ans);
                u8_IntNumber = (u8)u16_Ans;
                app_SrvExtIntEnable(u8_IntNumber);
            }
            break;

        case '3':
            {
                printf("\nEnter Interrupt Number:\n");
                tcx_scanf("%hd", &u16_Ans);
                u8_IntNumber = (u8)u16_Ans;
                app_SrvExtIntDisable(u8_IntNumber);
            }
            break;

        default:
            break;
        }
    }

}

void keypb_HWControlMenu(void)
{
    u8 u8_Ans = 0;
    tcx_appClearScreen();

    while (u8_Ans != 'q')
    {
        printf("=====================\n\n");
        printf("Select option:\n");
        printf("1: AFE Settings Menu\n");
        printf("2: GPIO Control\n");
        printf("3: External Interrupts Control\n");
        printf("4: Send DHSG Value\n");
        printf("5: Enable DHSG (once only)\n");
        printf("\n Press 'q' to return to menu\n");
        printf("\n");
        u8_Ans = tcx_getch();
        switch (u8_Ans)
        {
        case '1':
            keypb_AFESettingsMenu();
            break;
        case '2':
            keyb_GPIOControl();
            break;
        case '3':
            keyb_ExternalInterruptsControl();
            break;
        case '4':
            keyb_AFEDHSGValueSend();
            break;
        case '5':
            keyb_AFEDHSGEnable();

        default:
            break;
        }
    }
}

void keyb_SetCallStateFilter(void)
{
    u32 u32_Value;
    u8 u8_FilterType;

    printf("Enter Call State Filter: 0 - to send all\n");
    printf("0 - to send all states indications\n");
    printf("1 - for internal calls only\n");
    printf("2 - for external calls only\n");
    printf("3 - do not send any states indications\n");

    tcx_scanf("%d", &u32_Value);
    u8_FilterType = (u8)u32_Value;

    appsrv_SetCallStateFilter(u8_FilterType);
}


void  keyb_SRVLoop(void)
{
    int n_Keep = TRUE;

    while (n_Keep)
    {
        //      tcx_appClearScreen();
        printf("-----------------------------\n");
        printf("Choose service:\n");
        printf("1 => EEProm  Param Get\n");
        printf("2 => EEProm  Param Set\n");
        printf("3 => Production  Param Get\n");
        printf("4 => Production  Param Set\n");
        printf("5 => Handset Page\n");
        printf("6 => Stop Paging\n");
        printf("7 => Handset Delete\n");
        printf("0 => Handset Delete from INL\n");
        printf("8 => System  Registration Mode\n");
        printf("9 => System  Restart\n");
        printf("a => Handset Add to INL\n");
        printf("A => System  FW/HW/EEPROM/JEDEC (JEDEC ID is only for DCX81/DHX91) Version Get\n");
        printf("B => System  Testmode Get\n");
        printf("C => System  Testmode Set\n");
        printf("D => SysLog  Start\n");
        printf("E => SysLog  Stop\n");
        printf("F => SysLog  Read\n");
        printf("G => List Of Registered Handsets\n");
        printf("H => Set new handset name\n");
        printf("I => RF Control and carrier fix\n");
        printf("J => Turn On/Off NEMo mode\n");
        printf("K => System  Power Off\n");
        printf("L => Get Line Settings\n");
        printf("M => Set Line Settings\n");
        printf("N => ATE test start\n");
        printf("O => ATE test leave\n");
        printf("P => List Access\n");
		printf("r => RAM  Param Get\n");
        printf("R => Set CMBS messages logging options\n");
        printf("s => Get and set Repeater options  options\n");
        printf("t/T => Get/Set DECT settings\n");
        printf("u => Add New Extension\n");
        printf("V => Get subs. HS, EEPROM reset, Set subs. HS\n");
        printf("v => Target <-> Host communication stress test\n");
        printf("W => Encryption Disable/Enable if feature supported\n");
        printf("w => FP Customer settings (Int call, ConfCall) change \n");
        printf("x => Set Base Name\n");
        printf("z => Get Base Name\n");
        printf("X => Set NEMo Control\n");        
        printf("Z => HW Control menu (AFE, GPIO, External Interrupts configuration)\n");
        printf("+ => Start DECT logger \n");
        printf("- => Stop and read DECT logs \n");
        printf("y => Data Call\n");
        printf("Y => Display subscription Data\n");
        printf(". => Ping\n");
        printf("/ => Create EEPROM backup file\n");
        printf("# => ULE Auto Subscription\n");
        printf("~ => Set Call State filter \n");
        printf("* => FXS/SLIC Service Menu\n");
        printf("@ => Set date time valid flag\n");

        printf("- - - - - - - - - - - - - - - \n");
        printf("q => Return to Interface Menu\n");

        switch (tcx_getch())
        {
        case ' ':
            tcx_appClearScreen();
            break;

        case '1':
            keypb_EEPromParamGet();
            break;
        case '2':
            keypb_EEPromParamSet();
            break;
        case '3':
            keypb_ProductionParamGet();
            break;
        case '4':
            keypb_ProductionParamSet();
            break;
        case '5':
            keypb_HandsetPage();
            break;
        case '6':
            keypb_HandsetStopPaging();
            break;
        case '7':
            keypb_HandsetDelete();
            break;
        case '0':
            keypb_HandsetDeleteINL();
            break;
        case '8':
            keypb_SYSRegistrationMode();
            break;
        case '9':
            keypb_SYSRestart();
            break;
        case 'a':
            keypb_HandsetAddINL();
            break;
        case 'A':
            {
                u16 u16_TargetVersion = 0;
            	keypb_SYSFWVersionGet();
                u16_TargetVersion = cmbs_api_ModuleVersionGet();

                if ( (u16_TargetVersion & 0xFF00) != 0x0000 ) //Booter does not support these events
                {
		            keypb_SYSEEPROMVersionGet();
		            keypb_SYSHWVersionGet();
		            if((appcmbs_get_CMBSAppInstance()->u8_HwChip == CMBS_HW_CHIP_DCX81) || (appcmbs_get_CMBSAppInstance()->u8_HwChip == CMBS_HW_CHIP_DHX91))
		            	keypb_JEDECIdGet();
                }
				printf("\n\nPress Any Key!\n");
			    tcx_getch();
            }
            break;
        case 'b':
        case 'B':
            keypb_SYSTestModeGet();
            break;
        case 'c':
        case 'C':
            keypb_SYSTestModeSet();
            break;
        case 'd':
        case 'D':
            keypb_SysLogStart();
            break;
        case 'e':
        case 'E':
            keypb_SysLogStop();
            break;
        case 'f':
        case 'F':
            keypb_SysLogRead();
            break;
        case 'g':
        case 'G':
            keypb_RegisteredHandsets();
            break;
        case 'h':
        case 'H':
            keyb_SetNewHandsetName();
            break;
        case 'i':
        case 'I':
            keypb_RF_Control();
            break;
        case 'j':
        case 'J':
            keypb_Turn_On_Off_NEMo_mode();
            break;
        case 'k':
        case 'K':
            keypb_SYSPowerOff();
            break;
        case 'l':
        case 'L':
            keypb_LineSettingsGet();
            break;
        case 'm':
        case 'M':
            keypb_LineSettingsSet();
            break;

        case 'n':
        case 'N':
            keyb_StartATETest();
            break;
        case 'o':
        case 'O':
            keyb_LeaveATETest();
            break;

        case 'p':
        case 'P':
            keyb_ListAccess();
            break;

        case 'r':
			keyb_RAMParamGet();
	        break;
			
        case 'R':
            keyb_CmbsLoggerOptions();
            break;

        case 's':
            keyb_RepeaterSupport();
            break;
       case 'S':
            //Currently free
            break;

        case 't':
            keypb_DectSettingsGet();
            break;

        case 'T':
            keypb_DectSettingsSet();
            break;

        case 'u':
            keypb_AddNewExtension();
            break;

        case 'V':
            keyb_SubsHSTest();
            break;

        case 'v':
            keyb_CommStress();
            break;

        case 'W':
            keypb_SYSEncryptionMode();
            break;

        case 'w':
            keypb_SYSFPCustomFeatures();
            break;

        case 'x':
            keypb_SetBaseName();
            break;
            
        case 'X':
            keypb_SYSNemoContol();
            break;

        case 'z':
            keypb_GetBaseName();
            break;

        case 'Z':
            keypb_HWControlMenu();
            break;

        case '+':
            keyb_DECTLoggerStart();
            break;

        case '-':
            keyb_DECTLoggerStop();
            break;
        case 'y':
            keypb_DataCallLoop();
            break;
        case 'Y':
            keyb_DisplaySubscriptionData();
            break;
        case '.':
            cmbs_dsr_Ping(g_cmbsappl.pv_CMBSRef);
            break;
        case '/':
            keyb_EEPROMBackupCreate();
            break;
        case '#':
            keyb_ULEDevicesAutoSubscription();
            break;
        case '~':
            keyb_SetCallStateFilter();
            break;
        case '*':
            keyb_FXS_Service_Menu();
            break;
		case '@':
			keyb_SetDateTimeValidFlag();
			break;
        case 'q':
            n_Keep = FALSE;
            break;
        default:
            break;

        }
    }
}


const char* keyb_dbg_GetListTypeName(u16 u16_ListType)
{
    switch ( u16_ListType )
    {
    case SMS_TYPE_INCOMING:
    case LIST_TYPE_INCOMING_SMS:
        return "INCOMING SMS";

    case SMS_TYPE_OUTGOING:
    case LIST_TYPE_OUTGOING_SMS:
        return "OUTGOING SMS";

    case SMS_TYPE_SENT:
    case LIST_TYPE_SENT_SMS:
        return "SENT SMS";

    case SMS_TYPE_DRAFT:
    case LIST_TYPE_DRAFT_SMS:
        return "DRAFT SMS";
    default:
        return "List type not defined";
    }
}

void keypb_SetEEPROMFromFile()
{
    E_CMBS_RC e_status;
    static u8 pu8_EEPROM[EEPROM_FILE_MAX_SIZE] = { 0 };
    char file_name[FILENAME_MAX];
    
    u32 u32_size; 
    FILE *f_binFile = 0;

    printf("\nEnter file name: ");
    tcx_gets(file_name, sizeof(file_name));

    // Open file
    e_status = tcx_fileOpen(&f_binFile, (u8 *)file_name, (u8 *)"r+b");
    if ( e_status != CMBS_RC_OK )
    {
        printf("\nError: Can't Open File: %s", file_name);
    }
    else
    {
        // get the size of the file in bytes
        u32_size = tcx_fileSize(f_binFile);
        tcx_fileClose(f_binFile);
        
        memset(&pu8_EEPROM, 0, sizeof(pu8_EEPROM));
        
        //Retrieve data from file
        e_status = app_ParamAreaGetFromFile(pu8_EEPROM, (u8 *)file_name, NULL);
        if ( e_status == CMBS_RC_ERROR_GENERAL )
        {
            printf("Failed \nExiting...");
            return;
        }
        else
            app_ParamAreaSetBySegments(0, u32_size, pu8_EEPROM, g_CMBSMaxTransferSize);
   }
}

void  keyb_AddDandAContactList(void)
{
	const stDandAContactListEntry* p_stDandAContactListEntry = g_pst_DandAContactsList;
	stContactListEntry st_Entry;
	u32 pu32_FiledIDs[FIELD_ID_MAX];
	u32 u32_FieldsSize, u32_EntryId ;
	char InputBuffer[10];
	u8 u8_NumberOfContacts, u8_Idx;

	memset(InputBuffer, 0x00, sizeof(InputBuffer));

	printf(" Available Contacts = %d\n", (int)CONTACT_LIST_SIZE);

	printf(" Enter number of contacts to insert or press a/A to enter all\n");

	tcx_gets(InputBuffer, sizeof(InputBuffer)-1);

	if((InputBuffer[0] == 'a') || (InputBuffer[0] == 'A'))
	{
		u8_NumberOfContacts = CONTACT_LIST_SIZE;
	}
	else
	{
		u8_NumberOfContacts = (u8) atoi(InputBuffer);
	}

	List_DeleteAllEntries(LIST_TYPE_CONTACT_LIST);

	printf("Old Contact list cleared!");

	printf("\nInserting %d new Contacts .... \n", u8_NumberOfContacts);

	for(u8_Idx=0; u8_Idx < u8_NumberOfContacts; u8_Idx++)
	{
		memset(&st_Entry, 0x00, sizeof(st_Entry));
		memset(pu32_FiledIDs, 0x00, sizeof(pu32_FiledIDs));
		u32_FieldsSize = 0;

		/* Last Name*/
		if(p_stDandAContactListEntry[u8_Idx].psLastName)
		{
			pu32_FiledIDs[u32_FieldsSize++] = FIELD_ID_LAST_NAME;
			strcpy(st_Entry.sLastName, p_stDandAContactListEntry[u8_Idx].psLastName);
		}

		/* First Name*/
		if(p_stDandAContactListEntry[u8_Idx].psFirstName)
		{
			pu32_FiledIDs[u32_FieldsSize++] = FIELD_ID_FIRST_NAME;
			strcpy(st_Entry.sFirstName, p_stDandAContactListEntry[u8_Idx].psFirstName);
		}

		/* Number 1*/
		if(p_stDandAContactListEntry[u8_Idx].psNumber1)
		{
			pu32_FiledIDs[u32_FieldsSize++] = FIELD_ID_CONTACT_NUM_1;
			strcpy(st_Entry.sNumber1, p_stDandAContactListEntry[u8_Idx].psNumber1);
			st_Entry.cNumber1Type = p_stDandAContactListEntry[u8_Idx].cNumber1Type;
			st_Entry.bNumber1Default = p_stDandAContactListEntry[u8_Idx].bNumber1Default;
			st_Entry.bNumber1Own = p_stDandAContactListEntry[u8_Idx].bNumber1Own;
		}
		
		/* Number 2*/
		if(p_stDandAContactListEntry[u8_Idx].psNumber2)
		{
			pu32_FiledIDs[u32_FieldsSize++] = FIELD_ID_CONTACT_NUM_2;
			strcpy(st_Entry.sNumber2, p_stDandAContactListEntry[u8_Idx].psNumber2);
			st_Entry.cNumber2Type = p_stDandAContactListEntry[u8_Idx].cNumber2Type;
			st_Entry.bNumber2Default = p_stDandAContactListEntry[u8_Idx].bNumber2Default;
			st_Entry.bNumber2Own = p_stDandAContactListEntry[u8_Idx].bNumber2Own;
		}
		
		/* Number 3*/
		if(p_stDandAContactListEntry[u8_Idx].psNumber3)
		{
			pu32_FiledIDs[u32_FieldsSize++] = FIELD_ID_CONTACT_NUM_3;
			strcpy(st_Entry.sNumber3, p_stDandAContactListEntry[u8_Idx].psNumber3);
			st_Entry.cNumber3Type = p_stDandAContactListEntry[u8_Idx].cNumber3Type;
			st_Entry.bNumber3Default = p_stDandAContactListEntry[u8_Idx].bNumber3Default;
			st_Entry.bNumber3Own = p_stDandAContactListEntry[u8_Idx].bNumber3Own;
		}

		/* Associated Melody*/
		pu32_FiledIDs[u32_FieldsSize++] = FIELD_ID_ASSOCIATED_MELODY;
		st_Entry.u32_AssociatedMelody =  p_stDandAContactListEntry[u8_Idx].u32_AssociatedMelody;

		/* Associated Line ID*/
		st_Entry.u32_LineId=  p_stDandAContactListEntry[u8_Idx].u32_LineId;
		pu32_FiledIDs[u32_FieldsSize++] = FIELD_ID_LINE_ID;
		
		/* Insert Contact */
		List_InsertEntry(LIST_TYPE_CONTACT_LIST, &st_Entry, pu32_FiledIDs, u32_FieldsSize, &u32_EntryId);
	}

	printf("SUCCESS\n");

}

//*/
