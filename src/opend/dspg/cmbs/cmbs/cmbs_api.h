/*!
*  \file       cmbs_api.h
*  \brief
*  \author     several - andreas
*
*  @(#)  %filespec: cmbs_api.h~GIT %
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================
*  date        name     version   action                                          
*  ---------------------------------------------------------------------------
* 05-May-2015 tcmc_asa  PR 14071: added new CMBS_REL_REASON enums
* 15-Apr-2015 tcmc_asa  Defect 14073: added new CMBS_DECT_SETTINGS_PROP_FIELD parameters
* 15-Apr-2015 tcmc_asa  Defect 14073:added u8_Pref_OutgoingCall_LineId
* 18-Mar-2015 tcmc_asa            added CMBS_IE_REG_ENABLE_TYPE
* 01-Dec-2014 tcmc_asa            added CMBS_IE_TONE_VOLUME
* 21-Nov-2014 tcmc_asa --GIT-- added parameters to cmbs_dsr_hs_StopPaging
* 17-Oct-2014 tcmc_asa  ---GIT-- added CMBS_PARAM_INL_DEL(_LENGHT)
* 07-Mar-2014 tcmc_asa  ---GIT--   added CMBS_INBAND_ITALY
* 27-Jan-14 tcmc_asa  ---GIT--   use u8 instead of E_CMBS_CHECKSUM_ERROR for e_CheckSumError
* 13-Jan-2014 tcmc_asa  -- GIT--  take checksum changes from 3.46.x to 3_x main (3.5x)
* 20-Dec-2013 tcmc_asa  -- GIT--  copy CHECKSUM changes from 2.99.C
* 20-Nov-2013 tcmc_asa  -- GIT--  Added cmbs_api_ie_ChecksumErrorGet/Add, removed option
* 12-Nov-2013 tcmc_asa  -- GIT--  Added checksum to CMBS API
* 12-Jun-2013 tcmc_asa --GIT--    merged 3.27.4 to 3.3x (Git) for release 3.36
* 24-May-2013 tcmc_asa 76.3.5     added CMBS_TONE_MWI_OR_CONGESTION
* 03-May-2013 tcmc_asa 76.3.4     removed swiss tones and added country selection
* 30-Apr-2013 tcmc_asa 76.3.3     Add swiss tones
* 12-Jun-2013 tcmc_asa --GIT--    merged 2.99.x to 3. xfor release 3.36
* 24-Jan-2013 tcmc_asa 52.1.16.2.17 PR3615: CNIP from Contact list
* 17-Jan-2013 tcmc_asa 52.1.16.2.16 PR3613/3614 added CMBS_PARAM_FP_CUSTOM_FEATURES(_LENGTH )
* 17-Jun-2012 tcmc_asa 54         See PR 3320: Add new outband tones
* 23-May-2012 tcmc_asa 52.1.16.2.15 added CMBS_TONE_CALL_WAITING_OUTBAND
* 22-May-2012 tcmc_asa 52.1.16.2.14 added CLIP type CMBS_ADDR_PROPPLAN_NAT_STD
* 02-Feb-2012 tcmc_asa 52.1.16.2.9 added new tone definitions
* 02-Feb-2012 tcmc_asa 52.1.16.2.8 added CMBS_ADDR_PRESENT_CNIP_USER
*              Stein    1         Initialize \n
*  09-Apr-09   Kelbch   161       Update of Media Configuration during start-up 
*  14-Dec-09   sergiym  ?         Add start/stop log events \n
* 11-Oct-11 tcmc_asa  52.1.12     PR 2893: Add Disabling encryption
* 11-Oct-11 tcmc_asa  52.1.13     PR 2892: Add VIP Melody (other alerting patterns)
* 26-Oct-11 tcmc_asa  52.1.14     Add fixed carrier selection: CMBS_EV_DSR_FIXED_CARRIER
* 26-Oct-11 tcmc_asa  52.1.15     merged BLR 52.1.16 and NBG 52.1.14
*******************************************************************************/

#if   !defined( CMBS_API_H )
#define  CMBS_API_H

#if   !defined( u8 )
typedef unsigned char   u8;   /* unsigned int [0,255] */
#endif

#if   !defined( s8 )
typedef signed char     s8;   /* signed int [-128,127] */
#endif

#if   !defined( u16 )
typedef unsigned short  u16;  /* unsigned int [0,65535] */
#endif

#if   !defined( s16 )
typedef signed short    s16;  /* signed int [-32768,32767] */
#endif

#if   !defined( u32 )
typedef unsigned int    u32;  /* unsigned int [0,4294967295] */
#endif

#if   !defined( s32 )
typedef signed int      s32;  /* signed int [-2147483648, 2147483647] */
#endif

#if   !defined( u64 )
typedef long long unsigned int    u64;  /* unsigned int [0,2^64 - 1] */
#endif

#if   !defined( s32 )
typedef long long signed int      s64;  /* signed int [-2^32, 2^32 - 1] */
#endif

#if   !defined( FALSE )
#define  FALSE 0
#endif

#if   !defined( TRUE )
#define  TRUE  1
#endif

#if   !defined( MAX )
#define  MAX(a,b) ((a)>(b)?(a):(b))
#endif

#if   !defined( MIN )
#define  MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#if !defined( __cplusplus )

#ifndef bool
typedef u8 bool;
#endif

#endif

#define UNUSED_PARAMETER(x) (void)(x)
#define UNUSED_PARAMETER2(x,y) UNUSED_PARAMETER(x),UNUSED_PARAMETER(y)

#define COPY_FLOAT_TO_INT(a,b) memcpy(&a, &b, 4)
#define COPY_INT_TO_FLOAT(a,b) memcpy(&a, &b, 4)

/* Number of elements in an array */
#define LENGTHOF(x) (sizeof(x) / sizeof((x)[0]))

/*****************************************************************************
 * General API Enumerations
 *****************************************************************************/

#define     CMBS_CMD_MASK  0xFF00
/*! \brief CMBS API Commands */
typedef enum
{
    CMBS_CMD_HELLO = 0x01,       /*!< Host send this command to target for registration*/
    CMBS_CMD_HELLO_RPLY,         /*!< Target replies on an Hello with this command */
    CMBS_CMD_FLOW_NOK,           /*!< The reception was not successful */
    CMBS_CMD_FLOW_RESTART,       /*!< Restart with contained packet number */
    CMBS_CMD_RESET,              /*!< Reset the communication between host and target */
    CMBS_CMD_FLASH_START_REQ,    /*!< CMBS requests to enter flashing state */
    CMBS_CMD_FLASH_START_RES,    /*!< Host approves / reject request to enter flashing state */
    CMBS_CMD_FLASH_STOP_REQ,     /*!< CMBS requests to leave flashing state */
    CMBS_CMD_FLASH_STOP_RES,     /*!< Host approves / reject request to leave flashing state */
    CMBS_CMD_CAPABILITIES,       /*!< Host sends Capabilities, e.g. Checksum support (bit 1) */
    CMBS_CMD_CAPABILITIES_RPLY,  /*!< Target replies Capabilities, e.g. Checksum support (bit 1) */
    CMBS_CMD_STORE_RAM_DUMP      /*!< This is to forcefully reset the CMBS to capture RAM dump */
	
} CMBS_CMD;

/*! \brief Types of events generated by the API */
typedef enum
{
    CMBS_EV_UNDEF,
    CMBS_EV_DSR_HS_PAGE_START,             /*!< Performs paging handsets */
    CMBS_EV_DSR_HS_PAGE_START_RES,         /*!< Response to CMBS_EV_DSR_HS_PAGE_START */
    CMBS_EV_DSR_HS_PAGE_STOP,              /*!< Performs stop paging handsets */
    CMBS_EV_DSR_HS_PAGE_STOP_RES,          /*!< Response to CMBS_EV_DSR_HS_PAGE_STOP */
    CMBS_EV_DSR_HS_DELETE,                 /*!< Delete one or more handsets from the base's database */
    CMBS_EV_DSR_HS_DELETE_RES,             /*!< Response to CMBS_EV_DSR_HS_DELETE */
    CMBS_EV_DSR_HS_REGISTERED,             /*!< Unsolicited event generated on successful register/unregister operation of a handset */
    CMBS_EV_DSR_HS_IN_RANGE,               /*!< Generated when a handset in range */
    CMBS_EV_DSR_CORD_OPENREG,              /*!< Starts registration mode on the base station */
    CMBS_EV_DSR_CORD_OPENREG_RES,          /*!< Response to CMBS_EV_DSR_CORD_OPENREG */
    CMBS_EV_DSR_CORD_CLOSEREG,             /*!< Stops registration mode on the base station */
    CMBS_EV_DSR_CORD_CLOSEREG_RES,         /*!< Response to CMBS_EV_DSR_CORD_CLOSEREG */
    CMBS_EV_DSR_PARAM_GET,                 /*!< Get a parameter value */
    CMBS_EV_DSR_PARAM_GET_RES,             /*!< Response to CMBS_EV_DSR_PARAM_GET */
    CMBS_EV_DSR_PARAM_SET,                 /*!< Sets / updates a parameter value */
    CMBS_EV_DSR_PARAM_SET_RES,             /*!< Response to CMBS_EV_DSR_PARAM_SET */
    CMBS_EV_DSR_FW_UPD_START,              /*!< Starts firmware update on the base station */
    CMBS_EV_DSR_FW_UPD_START_RES,          /*!< Response to CMBS_EV_DSR_FW_UPD_START */
    CMBS_EV_DSR_FW_UPD_PACKETNEXT,         /*!< Sends a chunk of firmware to the base station */
    CMBS_EV_DSR_FW_UPD_PACKETNEXT_RES,     /*!< Response to CMBS_EV_DSR_FW_UPD_PACKETNEXT */
    CMBS_EV_DSR_FW_UPD_END,                /*!< Ending firmware update process with last chunk of data */
    CMBS_EV_DSR_FW_UPD_END_RES,            /*!< Response to CMBS_EV_DSR_FW_UPD_END */
    CMBS_EV_DSR_FW_VERSION_GET,            /*!< Gets the base's current firmware version of a particular module */
    CMBS_EV_DSR_FW_VERSION_GET_RES,        /*!< Response to CMBS_EV_DSR_FW_VERSION_GET */
    CMBS_EV_DSR_SYS_START,                 /*!< Starts the base station's CMBS after parameters were set */
    CMBS_EV_DSR_SYS_START_RES,             /*!< Response to CMBS_EV_DSR_SYS_START  */
    CMBS_EV_DSR_SYS_SEND_RAWMSG,           /*!< Event containing a raw message to the target */
    CMBS_EV_DSR_SYS_SEND_RAWMSG_RES,       /*!< Response to CMBS_EV_DSR_SYS_SEND_RAWMSG */
    CMBS_EV_DSR_SYS_STATUS,                /*!< Announce current target status, e.g. up, down, removed */
    CMBS_EV_DSR_SYS_LOG,                   /*!< Event containing target system logs */
    CMBS_EV_DSR_SYS_RESET,                 /*!< Performs a base station reboot */
    CMBS_EV_DSR_SYS_POWER_OFF,             /*!< Performs a base station power off */

    CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET,    /*!< Get list of subscribed handsets */
    CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET_RES, /*!< result of CMBS_EV_DSR_HS_SUBSCRIBED_LIST_GET */
    CMBS_EV_DSR_HS_SUBSCRIBED_LIST_SET,    /*!< Set list of subscribed handsets */
    CMBS_EV_DSR_HS_SUBSCRIBED_LIST_SET_RES, /*!< result of CMBS_EV_DSR_HS_SUBSCRIBED_LIST_SET */

    CMBS_EV_DSR_LINE_SETTINGS_LIST_GET,    /*!< Get list of subscribed handsets */
    CMBS_EV_DSR_LINE_SETTINGS_LIST_GET_RES, /*!< result of CMBS_EV_DSR_LINE_SETTINGS_LIST_GET */
    CMBS_EV_DSR_LINE_SETTINGS_LIST_SET,    /*!< Set list of subscribed handsets */
    CMBS_EV_DSR_LINE_SETTINGS_LIST_SET_RES, /*!< result of CMBS_EV_DSR_LINE_SETTINGS_LIST_SET */

    CMBS_EV_DSR_RF_SUSPEND,                /*!< RF Suspend on CMBS target */
    CMBS_EV_DSR_RF_RESUME,                 /*!< RF Resume on CMBS target */
    CMBS_EV_DSR_TURN_ON_NEMO,              /*!< Turn On NEMo mode for the CMBS base */
    CMBS_EV_DSR_TURN_OFF_NEMO,             /*!< Turn Off NEMo mode for the CMBS base */
    CMBS_EV_DCM_CALL_STATE,                /*!< Call state */
    CMBS_EV_DEE_CALL_ESTABLISH,            /*!< Event generated on start of a new call( incoming or outgoing ) */
    CMBS_EV_DEE_CALL_PROGRESS,             /*!< Events for various call progress states */
    CMBS_EV_DEE_CALL_ANSWER,               /*!< Generated when a call is answered */
    CMBS_EV_DEE_CALL_RELEASE,              /*!< Generated when a call is released */
    CMBS_EV_DEE_CALL_RELEASECOMPLETE,      /*!< Generated when call instance deleted */
    CMBS_EV_DEE_CALL_INBANDINFO,           /*!< Events created for inband keys */
    CMBS_EV_DEE_CALL_MEDIA_OFFER,          /*!< Offer media */
    CMBS_EV_DEE_CALL_MEDIA_OFFER_RES,      /*!< Response to CMBS_EV_DEE_CALL_MEDIA_OFFER */
    CMBS_EV_DEE_CALL_MEDIA_UPDATE,         /*!< Received when cordless module updated the media */
    CMBS_EV_DEE_CALL_HOLD,                 /*!< Generated on call HOLD */
    CMBS_EV_DEE_CALL_RESUME,               /*!< Generated on call RESUME */
    CMBS_EV_DEE_CALL_HOLD_RES,             /*!< this is response for the call hold request. */
    CMBS_EV_DEE_CALL_RESUME_RES,           /*!< this is response for the call resume request */
    CMBS_EV_DSR_HS_PAGE_PROGRESS,          /*!< Events for various handset locator progress states */
    CMBS_EV_DSR_HS_PAGE_ANSWER,            /*!< Generated when a handset locator is answered */
    CMBS_EV_DEM_CHANNEL_START,             /*!< Start sending (voice) data on a particular channel */
    CMBS_EV_DEM_CHANNEL_START_RES,         /*!< Response to CMBS_EV_DEM_CHANNEL_START */
    CMBS_EV_DEM_CHANNEL_INTERNAL_CONNECT,  /*!< Modify the IN/Out Connection for Media Channels */
    CMBS_EV_DEM_CHANNEL_INTERNAL_CONNECT_RES, /*!< Response to CMBS_EV_DEM_CHANNEL_INTERNAL_CONNECT */
    CMBS_EV_DEM_CHANNEL_STOP,              /*!< Stop sending data on a particular channel */
    CMBS_EV_DEM_CHANNEL_STOP_RES,          /*!< Response to CMBS_EV_DEM_CHANNEL_STOP */
    CMBS_EV_DEM_TONE_START,                /*!< Start the tone generation on a particular media channel */
    CMBS_EV_DEM_TONE_START_RES,            /*!< Response to CMBS_EV_DEM_TONE_START */
    CMBS_EV_DEM_TONE_STOP,                 /*!< Stop tone generation on a particular media channel */
    CMBS_EV_DEM_TONE_STOP_RES,             /*!< Response to CMBS_EV_DEM_TONE_STOP */
    CMBS_EV_DSR_SYS_LOG_START,             /*!< Start system logging */
    CMBS_EV_DSR_SYS_LOG_STOP,              /*!< Stop system logging */
    CMBS_EV_DSR_SYS_LOG_REQ,               /*!< Request to get content of the log buffer */
    CMBS_EV_DSR_PARAM_UPDATED,             /*!< Unsolicited event from the CMBS target when a parameter was internally modified */
    CMBS_EV_DSR_PARAM_AREA_UPDATED,        /*!< Unsolicited event from the CMBS target when a parameter area was internally modified */
    CMBS_EV_DSR_PARAM_AREA_GET,            /*!< Get parameter area data */
    CMBS_EV_DSR_PARAM_AREA_GET_RES,        /*!< Response to CMBS_EV_DSR_PARAM_AREA_GET */
    CMBS_EV_DSR_PARAM_AREA_SET,            /*!< Sets / updates data in parameter area */
    CMBS_EV_DSR_PARAM_AREA_SET_RES,        /*!< Response to CMBS_EV_DSR_PARAM_AREA_SET */

    // CAT-iq 2 events
    CMBS_EV_DSR_GEN_SEND_MWI,              /*!< Send Voice/SMS/Email Message Waiting Indication to one or more handsets */
    CMBS_EV_DSR_GEN_SEND_MWI_RES,          /*!< Response to CMBS_EV_DSR_GEN_SEND_MWI */
    CMBS_EV_DSR_GEN_SEND_MISSED_CALLS,     /*!< Send Missed Calls Indication to one or more handsets */
    CMBS_EV_DSR_GEN_SEND_MISSED_CALLS_RES, /*!< Response to CMBS_EV_DSR_GEN_SEND_MISSED_CALLS */
    CMBS_EV_DSR_GEN_SEND_LIST_CHANGED,     /*!< Send List Changed event to one or more handsets */
    CMBS_EV_DSR_GEN_SEND_LIST_CHANGED_RES, /*!< Response to CMBS_EV_DSR_GEN_SEND_LIST_CHANGED */
    CMBS_EV_DSR_GEN_SEND_WEB_CONTENT,      /*!< Send Web Content event to one or more handsets */
    CMBS_EV_DSR_GEN_SEND_WEB_CONTENT_RES,  /*!< Response to CMBS_EV_DSR_GEN_SEND_WEB_CONTENT */
    CMBS_EV_DSR_GEN_SEND_PROP_EVENT,       /*!< Send Escape to Proprietary event to one or more handsets */
    CMBS_EV_DSR_GEN_SEND_PROP_EVENT_RES,   /*!< Response to CMBS_EV_DSR_GEN_SEND_PROP_EVENT */
    CMBS_EV_DSR_TIME_UPDATE,               /*!< Send Time-Date update event to one or more handsets */
    CMBS_EV_DSR_TIME_UPDATE_RES,           /*!< Response to CMBS_EV_DSR_TIME_UPDATE */
    CMBS_EV_DSR_TIME_INDICATION,           /*!< Event received when a handset has updated its Time-Date setting */
    CMBS_EV_DSR_HS_DATA_SESSION_OPEN,
    CMBS_EV_DSR_HS_DATA_SESSION_OPEN_RES,
    CMBS_EV_DSR_HS_DATA_SESSION_CLOSE,
    CMBS_EV_DSR_HS_DATA_SESSION_CLOSE_RES,
    CMBS_EV_DSR_HS_DATA_SEND,
    CMBS_EV_DSR_HS_DATA_SEND_RES,
    CMBS_EV_DSR_LA_SESSION_START,
    CMBS_EV_DSR_LA_SESSION_START_RES,
    CMBS_EV_DSR_LA_SESSION_END,
    CMBS_EV_DSR_LA_SESSION_END_RES,
    CMBS_EV_DSR_LA_QUERY_SUPP_ENTRY_FIELDS,
    CMBS_EV_DSR_LA_QUERY_SUPP_ENTRY_FIELDS_RES,
    CMBS_EV_DSR_LA_READ_ENTRIES,
    CMBS_EV_DSR_LA_READ_ENTRIES_RES,
    CMBS_EV_DSR_LA_SEARCH_ENTRIES,
    CMBS_EV_DSR_LA_SEARCH_ENTRIES_RES,
    CMBS_EV_DSR_LA_EDIT_ENTRY,
    CMBS_EV_DSR_LA_EDIT_ENTRY_RES,
    CMBS_EV_DSR_LA_SAVE_ENTRY,
    CMBS_EV_DSR_LA_SAVE_ENTRY_RES,
    CMBS_EV_DSR_LA_DELETE_ENTRY,
    CMBS_EV_DSR_LA_DELETE_ENTRY_RES,
    CMBS_EV_DSR_LA_DELETE_LIST,
    CMBS_EV_DSR_LA_DELETE_LIST_RES,
    CMBS_EV_DSR_LA_DATA_PACKET_RECEIVE,
    CMBS_EV_DSR_LA_DATA_PACKET_RECEIVE_RES,
    CMBS_EV_DSR_LA_DATA_PACKET_SEND,
    CMBS_EV_DSR_LA_DATA_PACKET_SEND_RES,
    CMBS_EV_DCM_CALL_TRANSFER,
    CMBS_EV_DCM_CALL_TRANSFER_RES,
    CMBS_EV_DCM_CALL_CONFERENCE,
    CMBS_EV_DCM_CALL_CONFERENCE_RES,
    CMBS_EV_DSR_TARGET_UP,
    CMBS_EV_DEE_HANDSET_LINK_RELEASE,
    CMBS_EV_DSR_GPIO_CONNECT,        /*!< Request to connect GPIO for RXTUN */
    CMBS_EV_DSR_GPIO_CONNECT_RES,    /*!< Response to CMBS_EV_DSR_GPIO_CONNECT */
    CMBS_EV_DSR_GPIO_DISCONNECT,     /*!< Request to disconnect and restore old configuration for RXTUN  */
    CMBS_EV_DSR_GPIO_DISCONNECT_RES, /*!< Response to CMBS_EV_DSR_GPIO_DISCONNECT */
    CMBS_EV_DSR_ATE_TEST_START,      /*!< Request to start ATE test */
    CMBS_EV_DSR_ATE_TEST_START_RES,  /*!< Response to CMBS_EV_DSR_ATE_TEST_START */
    CMBS_EV_DSR_ATE_TEST_LEAVE,      /*!< Request to leave ATE test */
    CMBS_EV_DSR_ATE_TEST_LEAVE_RES,  /*!< Response to CMBS_EV_DSR_ATE_TEST_LEAVE */
    CMBS_EV_DSR_SUOTA_HS_VERSION_RECEIVED,  /*!< HS version message was received from HS */
    CMBS_EV_DSR_SUOTA_URL_RECEIVED,         /*!< URL message was received from HS */
    CMBS_EV_DSR_SUOTA_NACK_RECEIVED,        /*!< NACK indication message was received from HS */
    CMBS_EV_DSR_SUOTA_SEND_SW_UPD_IND,      /*!< Host requested to send SW update IND */
    CMBS_EV_DSR_SUOTA_SEND_SW_UPD_IND_RES,  /*!< Response to CMBS_EV_DSR_SUOTA_SEND_SW_UPD_IND*/
    CMBS_EV_DSR_SUOTA_SEND_VERS_AVAIL,      /*!< Host requested to send Version available IND  */
    CMBS_EV_DSR_SUOTA_SEND_VERS_AVAIL_RES,  /*!< Response CMBS_EV_DSR_SUOTA_SEND_VERS_AVAIL */
    CMBS_EV_DSR_SUOTA_SEND_URL,             /*!< Host requested to send URL message */
    CMBS_EV_DSR_SUOTA_SEND_URL_RES,         /*!< Response to CMBS_EV_DSR_SUOTA_SEND_URL */
    CMBS_EV_DSR_SUOTA_SEND_NACK,            /*!< Host requested to send NACK message  */
    CMBS_EV_DSR_SUOTA_SEND_NACK_RES,        /*!< Responce to CMBS_EV_DSR_SUOTA_SEND_NACK */
    CMBS_EV_DSR_TARGET_LIST_CHANGE_NOTIF,   /*!< Target notifies Host on a change in LA list maintained on Target side */
    CMBS_EV_DSR_HW_VERSION_GET,             /*!< Gets the HW version of a particular module */
    CMBS_EV_DSR_HW_VERSION_GET_RES,         /*!< Response to CMBS_EV_DSR_HW_VERSION_GET */
    CMBS_EV_DSR_DECT_SETTINGS_LIST_GET,     /*!< Get DECT Settings */
    CMBS_EV_DSR_DECT_SETTINGS_LIST_GET_RES, /*!< result of CMBS_EV_DSR_DECT_SETTINGS_LIST_GET */
    CMBS_EV_DSR_DECT_SETTINGS_LIST_SET,     /*!< Set DECT Settings */
    CMBS_EV_DSR_DECT_SETTINGS_LIST_SET_RES, /*!< result of CMBS_EV_DSR_DECT_SETTINGS_LIST_SET */

    // RTP Extension
    CMBS_EV_RTP_SESSION_START,          /*!< Start a RTP session */
    CMBS_EV_RTP_SESSION_START_RES,      /*!< Response to CMBS_EV_RTP_SESSION_START */
    CMBS_EV_RTP_SESSION_STOP,           /*!< Stop an ongoing RTP session */
    CMBS_EV_RTP_SESSION_STOP_RES,       /*!< Response to CMBS_EV_RTP_SESSION_STOP */
    CMBS_EV_RTP_SESSION_UPDATE,         /*!< Update a RTP session */
    CMBS_EV_RTP_SESSION_UPDATE_RES,     /*!< Response to CMBS_EV_RTP_SESSION_UPDATE */
    CMBS_EV_RTCP_SESSION_START,         /*!< Start a RTCP session */
    CMBS_EV_RTCP_SESSION_START_RES,     /*!< Response to CMBS_EV_RTCP_SESSION_START */
    CMBS_EV_RTCP_SESSION_STOP,          /*!< Stop an ongoing RTCP session */
    CMBS_EV_RTCP_SESSION_STOP_RES,      /*!< Response to CMBS_EV_RTCP_SESSION_STOP */
    CMBS_EV_RTP_SEND_DTMF,              /*!< Send a RTP DTMF */
    CMBS_EV_RTP_SEND_DTMF_RES,          /*!< Response to CMBS_EV_RTP_SEND_DTMF */
    CMBS_EV_RTP_DTMF_NOTIFICATION,      /*!< Signaling out of band DTMF event */
    CMBS_EV_RTP_FAX_TONE_NOTIFICATION,  /*!< Signaling out of detected fax tone */
    CMBS_EV_RTP_ENABLE_FAX_AUDIO_PROCESSING_MODE,       /*!< Disable Audio Processing during a fax session */
    CMBS_EV_RTP_ENABLE_FAX_AUDIO_PROCESSING_MODE_RES,   /*!< Response to CMBS_EV_RTP_ENABLE_FAX_AUDIO_PROCESSING_MODE */
    CMBS_EV_RTP_DISABLE_FAX_AUDIO_PROCESSING_MODE,      /*!< Resume Audio Processing after a fax session */
    CMBS_EV_RTP_DISABLE_FAX_AUDIO_PROCESSING_MODE_RES,  /*!< Response to CMBS_EV_RTP_DISABLE_FAX_AUDIO_PROCESSING_MODE */

    CMBS_EV_DCM_INTERNAL_TRANSFER,                      /*!< Informative event to host - internal transfer performed */
    CMBS_EV_DSR_ADD_EXTENSION,          /*!< Add new extension (FXS) to internal names list*/
    CMBS_EV_DSR_ADD_EXTENSION_RES,      /*!< Add new extension (FXS) to internal names list*/
    CMBS_EV_DSR_RESERVED,
    CMBS_EV_DSR_RESERVED_RES,
    CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS,       /*!< Add non-mandatory LiA List ID to List of supported lists */
    CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS_RES,   /* response to CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS */
    CMBS_EV_DSR_LA_PROP_CMD,                /*!< PP Performs proprietary List Access command */
    CMBS_EV_DSR_LA_PROP_CMD_RES,            /*!< FP responds to proprietary List Access command */
    CMBS_EV_DSR_ENCRYPT_DISABLE,            /*!< Disables encryption on the base station */
    CMBS_EV_DSR_ENCRYPT_ENABLE,             /*!< Enables encryption on the base station */
    CMBS_EV_DSR_SET_BASE_NAME,              /*!< Set new base name */
    CMBS_EV_DSR_SET_BASE_NAME_RES,          /*!< Set new base name responce */
    CMBS_EV_DSR_FIXED_CARRIER,              /*!< Set a fixed frequency for test  */
    CMBS_EV_DSR_FIXED_CARRIER_RES,          /*!< Set a fixed frequency for test - response */
    CMBS_EV_DEE_HS_CODEC_CFM_FAILED,
    CMBS_EV_DSR_EEPROM_SIZE_GET,            /*!< Get eeprom size 						*/
    CMBS_EV_DSR_EEPROM_SIZE_GET_RES,        /*!< Get eeprom size - response 			*/
    CMBS_EV_DSR_RECONNECT_REQ,              /*!< Reconnect request from target to host 	*/
    CMBS_EV_DSR_RECONNECT_RES,              /*!< Reconnect request responce 		   	*/
    CMBS_EV_DSR_GET_BASE_NAME,
    CMBS_EV_DSR_GET_BASE_NAME_RES,
    CMBS_EV_DSR_EEPROM_VERSION_GET,         /*!< Gets the base's current EEPROM version of a particular module */
    CMBS_EV_DSR_EEPROM_VERSION_GET_RES,     /*!< Response to CMBS_EV_DSR_EEPROM_VERSION_GET */
    CMBS_EV_DSR_START_DECT_LOGGER,          /*!< */
    CMBS_EV_DSR_START_DECT_LOGGER_RES,      /*!< */
    CMBS_EV_DSR_STOP_AND_READ_DECT_LOGGER,  /*!< */
    CMBS_EV_DSR_STOP_AND_READ_DECT_LOGGER_RES,    /*!< */
    CMBS_EV_DSR_DECT_DATA_IND,
    CMBS_EV_DSR_DECT_DATA_IND_RES,
    CMBS_EV_DSR_DC_SESSION_START,           /* !< Start a Data Call Session */
    CMBS_EV_DSR_DC_SESSION_START_RES,       /* !< PP responds to Data Call start command */
    CMBS_EV_DSR_DC_SESSION_STOP,            /* !< PP responds to Data Call stop command */
    CMBS_EV_DSR_DC_SESSION_STOP_RES,        /* !< Stop a Data Call Session */
    CMBS_EV_DSR_DC_DATA_SEND,               /* !< Send data in the Data Call Session */
    CMBS_EV_DSR_DC_DATA_SEND_RES,           /* !< Send data in the Data Call Session response */

    CMBS_EV_DSR_PING,                       /* !< Ping request */
    CMBS_EV_DSR_PING_RES,                   /* !< Ping response */

    CMBS_EV_DSR_SUOTA_SESSION_CREATE,           //GMEP_US_SESSION_CREATE,
    CMBS_EV_DSR_SUOTA_SESSION_CREATE_ACK,       //GMEP_US_SESSION_CREATE_ACK,
    CMBS_EV_DSR_SUOTA_OPEN_SESSION,             //GMEP_US_OPEN_SESSION,
    CMBS_EV_DSR_SUOTA_OPEN_SESSION_ACK,         //GMEP_US_OPEN_SESSION_ACK,
    CMBS_EV_DSR_SUOTA_DATA_SEND,                //GMEP_US_DATA_SEND,
    CMBS_EV_DSR_SUOTA_DATA_SEND_ACK,            //GMEP_US_DATA_SEND_ACK,
    CMBS_EV_DSR_SUOTA_REG_CPLANE_CB,            //GMEP_US_REG_CPLANE_CB,
    CMBS_EV_DSR_SUOTA_REG_CPLANE_CB_ACK,        //GMEP_US_REG_CPLANE_CB_ACK,
    CMBS_EV_DSR_SUOTA_REG_APP_CB,               //GMEP_US_REG_APP_CB,
    CMBS_EV_DSR_SUOTA_REG_APP_CB_ACK,           //GMEP_US_REG_APP_CB_ACK,
    CMBS_EV_DSR_SUOTA_DATA_RECV,                //GMEP_US_DATA_RECV,
    CMBS_EV_DSR_SUOTA_DATA_RECV_ACK,            //GMEP_US_DATA_RECV_ACK,
    CMBS_EV_DSR_SUOTA_HS_VER_IND_ACK,           //GMEP_US_HS_VER_IND_ACK,
    CMBS_EV_DSR_SUOTA_SESSION_CLOSE,            //GMEP_US_SESSION_CLOSE,
    CMBS_EV_DSR_SUOTA_SESSION_CLOSE_ACK,        //GMEP_US_SESSION_CLOSE_ACK,
    CMBS_EV_DSR_SUOTA_CONTROL_SET,              //GMEP_US_CONTROL_SET,
    CMBS_EV_DSR_SUOTA_CONTROL_SET_ACK,          //GMEP_US_CONTROL_SET_ACK,
    CMBS_EV_DSR_SUOTA_COTROL_RESET,             //GMEP_US_COTROL_RESET,
    CMBS_EV_DSR_SUOTA_COTROL_RESET_ACK,         //GMEP_US_COTROL_RESET_ACK,
    CMBS_EV_DSR_SUOTA_UPDATE_OPTIONAL_GRP,      //GMEP_US_UPDATE_OPTIONAL_GRP,
    CMBS_EV_DSR_SUOTA_UPDATE_OPTIONAL_GRP_ACK,  //GMEP_US_UPDATE_OPTIONAL_GRP_ACK,
    CMBS_EV_DSR_SUOTA_FACILITY_CB,              //GMEP_US_FACILITY_CB,
    CMBS_EV_DSR_SUOTA_PUSH_MODE,                //GMEP_US_PUSH_MODE
    CMBS_EV_DSR_SUOTA_UPLANE_COMMANDS_END,

    CMBS_EV_DSR_HS_PROP_EVENT,                  /*!< Handset proprietary event */
    CMBS_EV_DSR_FW_APP_INVALIDATE,              /*!< Special event to invalidate the application before FW upgrade */
    CMBS_EV_DSR_FW_APP_INVALIDATE_RES,          /*!< Response to the invalidate event */

    CMBS_EV_DSR_AFE_ENDPOINT_CONNECT,           /*!< Notify target which two end points need to be connected */
    CMBS_EV_DSR_AFE_ENDPOINT_CONNECT_RES,       /*!< Response to CMBS_EV_DSR_AFE_ENDPOINT_CONNECT */
    CMBS_EV_DSR_AFE_ENDPOINT_ENABLE,            /*!< To enable the path between two endpoints */
    CMBS_EV_DSR_AFE_ENDPOINT_ENABLE_RES,        /*!< Response to CMBS_EV_DSR_AFE_ENDPOINT_ENABLE */
    CMBS_EV_DSR_AFE_ENDPOINT_DISABLE,           /*!< To disable the path between two endpoints */
    CMBS_EV_DSR_AFE_ENDPOINT_DISABLE_RES,       /*!< Response to CMBS_EV_DSR_AFE_ENDPOINT_DISABLE */
    CMBS_EV_DSR_AFE_ENDPOINT_GAIN,              /*!< Define the gain to AFE end point  */
    CMBS_EV_DSR_AFE_ENDPOINT_GAIN_RES,          /*!< Response to CMBS_EV_DSR_AFE_ENDPOINT_GAIN */
    CMBS_EV_DSR_AFE_AUX_MEASUREMENT,            /*!< Define: input, define manually measure/via BMP, activate measurement. */
    CMBS_EV_DSR_AFE_AUX_MEASUREMENT_RES,        /*!< Response to CMBS_EV_DSR_AFE_AUX_MEASUREMENT */
    CMBS_EV_DSR_AFE_AUX_MEASUREMENT_RESULT,     /*!< An event from CMBS indicating the AUX measurement result */
    CMBS_EV_DSR_AFE_CHANNEL_ALLOCATE,           /*!< Allocate AFE Channel and codec */
    CMBS_EV_DSR_AFE_CHANNEL_ALLOCATE_RES,       /*!< Response to resource allocation request, includes channel and codec or error */
    CMBS_EV_DSR_AFE_CHANNEL_DEALLOCATE,         /*!< Free AFE Channel and codec */
    CMBS_EV_DSR_AFE_CHANNEL_DEALLOCATE_RES,     /*!< Response to resource de-allocation request */
    CMBS_EV_DSR_DHSG_SEND_BYTE,                 /*!< Send byte to the DSHG */
    CMBS_EV_DSR_DHSG_SEND_BYTE_RES,             /*!< Response to send byte to the DSHG */
    CMBS_EV_DSR_DHSG_NEW_DATA_RCV,              /*!< Unsolicited event to pass the DSHG data to the host */
    CMBS_EV_DSR_GPIO_ENABLE,                    /*!< Enable specifyed GPIO */
    CMBS_EV_DSR_GPIO_ENABLE_RES,                /*!< Response to enable specifyed GPIO */
    CMBS_EV_DSR_GPIO_DISABLE,                   /*!< Disable specifyed GPIO */
    CMBS_EV_DSR_GPIO_DISABLE_RES,               /*!< Response to disable specifyed GPIO */
    CMBS_EV_DSR_GPIO_CONFIG_SET,                /*!< Configure following parameters of specifyed GPIO: OUT/IN, SET/RESET, PULLUP/DOWN */
    CMBS_EV_DSR_GPIO_CONFIG_SET_RES,            /*!< Response to config specifyed GPIO */
    CMBS_EV_DSR_GPIO_CONFIG_GET,                /*!< Get current configuration of specifyed GPIO */
    CMBS_EV_DSR_GPIO_CONFIG_GET_RES,            /*!< Response to get current configuration of specifyed GPIO */
    CMBS_EV_DSR_TURN_ON_NEMO_RES,               /*!< Response to NEMO TURN ON event */
    CMBS_EV_DSR_TURN_OFF_NEMO_RES,              /*!< Response to NEMO TURN OFF event */
    CMBS_EV_DSR_EXT_INT_CONFIG,                 /*!< Request to configure an external interrupt (GPIO INT) */
    CMBS_EV_DSR_EXT_INT_CONFIG_RES,             /*!< Response to external INT configuration */
    CMBS_EV_DSR_EXT_INT_ENABLE,                 /*!< Request to enable external interrupt */
    CMBS_EV_DSR_EXT_INT_ENABLE_RES,             /*!< Reesponse to external interrupt enable */
    CMBS_EV_DSR_EXT_INT_DISABLE,                /*!< Request to disable external interrupt */
    CMBS_EV_DSR_EXT_INT_DISABLE_RES,            /*!< Reesponse to external interrupt disable */
    CMBS_EV_DSR_EXT_INT_INDICATION,             /*!< Unsolicited event indicating external interrupt occured (on GPIO) */
    CMBS_EV_DSR_LOCATE_SUGGEST_REQ,             /*!< Enforce the HS to perform locate */
    CMBS_EV_DSR_LOCATE_SUGGEST_RES,             /*!< Responce to CMBS_EV_DSR_LOCATE_SUGGEST_REQ */
    CMBS_EV_DSR_TERMINAL_CAPABILITIES_IND,      /*!< Transfer to Host termonal capabilities of a HS */
    CMBS_EV_DSR_HS_PROP_DATA_RCV_IND,               /*!< Indication that proprietary data received */
    CMBS_EV_CHECKSUM_FAILURE,                   /*!< indicates a checksum error to thesending side of an event */
    CMBS_EV_DSR_HS_REGISTRATION_IN_PROGRESS,    /*!< indicates HS registration is started */
    CMBS_EV_DSR_HS_DEREGISTRATION_IN_PROGRESS,  /*!< indicates HS de-registration is started */
    CMBS_EV_DEE_MERGE_CALLS,               /*!< Generated on merge calls */
    CMBS_EV_DEE_MERGE_CALLS_RES,           /*!< this is response for the merge calls request */

    CMBS_EV_DEM_TDM_LOOPBACK_START,             /* !< TDM loopback start */
    CMBS_EV_DEM_TDM_LOOPBACK_START_RES,         /* !< TDM loopback start response */
    CMBS_EV_DEM_TDM_LOOPBACK_STOP,              /* !< TDM loopback stop */
    CMBS_EV_DEM_TDM_LOOPBACK_STOP_RES,          /* !< TDM loopback stop response */

    CMBS_EV_DSR_GEN_SEND_LINE_USE_STATUS_IND,	/* !< Send Line Use Notification */
    CMBS_EV_DSR_GEN_SEND_HS_USE_STATUS_IND,		/* !< Send HS Use Notification */
    CMBS_EV_DSR_GEN_SEND_DIAGNOSTIC_STATUS_IND, /* !< Send Diagnostic Status Notification */
    CMBS_EV_DSR_GEN_LINE_USE_STATUS_RES,		/* !< Response to Line Use Notification */
    CMBS_EV_DSR_GEN_HS_USE_STATUS_RES,			/* !< Response to HS Use Notification */
    CMBS_EV_DSR_GEN_DIAGNOSTIC_STATUS_RES, 		/* !< Response to Diagnostic Status Notification */
	
    CMBS_EV_DSR_DTAM_START_SESSION,             	/*!< Request to start DTAM session */
    CMBS_EV_DSR_DTAM_START_SESSION_CFM,         	/*!< Confirm or Reject DTAM session */
    CMBS_EV_DSR_DTAM_COMMAND_NACK,              	/*!< Negative response to previous DTAM command */
    CMBS_EV_DSR_DTAM_STATUS,                    	/*!< Command to inform the PP of the current command status */
    CMBS_EV_DSR_DTAM_SELECT_NEIGHBOUR_MESSAGE,  	/*!< Allows the PP to select the next or previous message */
    CMBS_EV_DSR_DTAM_SELECT_NEIGHBOUR_MESSAGE_CFM,  /*!< Confirms the next or previous DTAM message selection */
    CMBS_EV_DSR_DTAM_PLAY_MESSAGE,      			/*!< Play DTAM message */
    CMBS_EV_DSR_DTAM_PLAY_MESSAGE_CFM,  			/*!< Play DTAM message confirm */
    CMBS_EV_DSR_DTAM_DELETE_MESSAGE,        		/*!< Delete indicated DTAM incoming message or welcome message */
    CMBS_EV_DSR_DTAM_DELETE_MESSAGE_CFM, 			/*!< Confirm to Delete indicated DTAM incoming message or welcome message */
    CMBS_EV_DSR_DTAM_PAUSE_RESUME_MESSAGE,   		/*!< Request that the current playing of a message be paused or resumed */
    CMBS_EV_DSR_DTAM_PAUSE_RESUME_MESSAGE_CFM, 		/*!< Confirm request that the current playing of a message be paused or resumed */
    CMBS_EV_DSR_DTAM_STOP_MESSAGE_PLAY,             /*!< Request from the PP that the current playing of message will be stopped */
    CMBS_EV_DSR_DTAM_STOP_MESSAGE_PLAY_CFM,  		/*!< Confirm request from the PP that the current playing of message will be stopped */
    CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE,  		/*!< Request the recording of a new welcome message at the indicated welcome message index */
    CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE_CFM, 	/*!< Confirm request the recording of a new welcome message at the indicated welcome message index */
    CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE_STOP, 	/*!< Indicate to the DTAM that recording shall be terminated  */
    CMBS_EV_DSR_DTAM_RECORD_WELCOME_MESSAGE_STOP_CFM, /*!< Confirmation from the DTAM that recording shall be terminated  */

    CMBS_EV_DSR_GEN_SEND_SMS_MSG_NOTIFICATION,      /*!< SMS message notification  */
    CMBS_EV_DSR_GEN_SEND_SMS_MSG_NOTIFICATION_RES,  /*!< Response to SMS message notification  */

    CMBS_EV_DSR_HS_RSSI_REQ,                    	/*!<  Request for RSSI value of a specific HS*/
    CMBS_EV_DSR_HS_RSSI_RES,                    	/*!<  Response for RSSI value of a specific HS*/

    CMBS_EV_DSR_CALL_STATE_SET_FILTER,              /*!< Setting filter to send only specific call states to the Host*/
    CMBS_EV_DSR_CALL_STATE_SET_FILTER_RES,      	/*!< Indicates if the filter for call states was set successfully*/

    CMBS_EV_DSR_SLIC_LINE_TEST_REQ,             /*!< Request to start SLIC Line Tests */
    CMBS_EV_DSR_SLIC_LINE_TEST_RES,             /*!< Reesponse to SLIC Line Tests with results */
    CMBS_EV_DSR_SLIC_LINE_TEST_STOP_REQ,        /*!< Request to stop the SLIC Line Tests */
    CMBS_EV_DSR_SLIC_LINE_TEST_STOP_RES,        /*!< Reesponse to stop request of SLIC Line tests */
    CMBS_EV_DEE_FXS_EVENT,                      /* FXS Simulation Event*/
    CMBS_EV_DSR_FXS_STATUS_REQ,					/*!> Request to get FXS channel status*/
    CMBS_EV_DSR_FXS_STATUS_RES,					/*!> Response FXS channel status request */
    CMBS_EV_DEE_CALL_EMERGENCY_RELEASE,         /*!< Generated by host when Emergency call is finally disconnected from network */
    CMBS_EV_DEE_CALL_EMERGENCY_RELEASE_RES,     /*!< Response by target after Emergency call is finally disconnected*/
    CMBS_EV_DSR_FXS_STATUS_IND,					/*!< Indication of Offhook / Onhook of FXS */
    CMBS_EV_DSR_FXS_RING_TEST_START_REQ, 		/*!< Request to start Ring Test */
    CMBS_EV_DSR_FXS_RING_TEST_START_RES, 		/*!< Reesponse to start Ring Test */
    CMBS_EV_DSR_FXS_RING_TEST_STOP_REQ,			/*!< Request to stop Ring Test */
    CMBS_EV_DSR_FXS_RING_TEST_STOP_RES,			/*!< Reesponse to stop Ring Test */
   	CMBS_EV_DSR_FXS_OPEN_LOOP,					/*!< Open Loop to FXS port */

    CMBS_EV_DSR_SUOTA_UNREG_APP_CB,             /*!< GMEP_US_UNREG_APP_CB */
    CMBS_EV_DSR_SUOTA_UNREG_APP_CB_ACK,         /*!< GMEP_US_UNREG_APP_CB_ACK */

    CMBS_EV_DSR_JEDEC_ID_GET,                   /*!< Get JEDEC information */
    CMBS_EV_DSR_JEDEC_ID_GET_RES,               /*!< Response by target with information about JEDEC ID*/

    CMBS_EV_DSR_EEPROM_EXTENDED_SIZE_GET,            /*!< Get eeprom size - used for systems with EEPROM size > 64KB         */
    CMBS_EV_DSR_EEPROM_EXTENDED_SIZE_GET_RES,        /*!< Get eeprom size - response             */	
	CMBS_EV_DSR_GEN_SEND_PROP_EVENT_NOTIFY,       /*!< Send Escape to Proprietary event to one or more handsets */
	CMBS_EV_DSR_GEN_SEND_PROP_EVENT_NOTIFY_RES,   /*!< Response to CMBS_EV_DSR_GEN_SEND_PROP_EVENT */
  
	CMBS_EV_DSR_GET_LIST_OF_ACTIVE_CALLS,			/*!< Host reads call info table from Target */
	CMBS_EV_DSR_GET_LIST_OF_ACTIVE_CALLS_RES,		

	CMBS_EV_DEM_CONFIG_TDM_SLOTS,               /*! TDM Slots Configuration*/
	CMBS_EV_DEM_CONFIG_TDM_SLOTS_RES,           /*! Response for CMBS_EV_DEM_CONFIG_TDM_SLOTS*/

	CMBS_EV_DSR_SLIC_NLT_CAP_TEST_START_REQ,			/*!< Request to start SLIC NLT Capacitance Tests */
	CMBS_EV_DSR_SLIC_NLT_CAP_TEST_START_RES,			/*!< Reesponse to SLIC NLT Capacitance Tests with results */
	CMBS_EV_DSR_SLIC_NLT_CAP_TEST_STOP_REQ,	/*!< Request to stop the SLIC NLT Capacitance Tests */
	CMBS_EV_DSR_SLIC_NLT_CAP_TEST_STOP_RES,	/*!< Reesponse to stop request of SLIC NLT Capacitance tests */

    CMBS_EV_DSR_NO_VALID_DATA_TIME_AVAIL,           /*!< Event received when a handset send no valid Time-Date available as response for Date-Time request */

    CMBS_EV_DEE_CALL_SCREENING,                 /*!< Generated on call screening Accept / Intercept */

    CMBS_EV_DSR_LA_READ_SELECTED_ENTRIES,
    CMBS_EV_DSR_LA_READ_SELECTED_ENTRIES_RES,

    CMBS_EV_DSR_LA_WRITE_ENTRY,
    CMBS_EV_DSR_LA_WRITE_ENTRY_RES,
    CMBS_EV_DEE_CALL_DEFLECTION,					 /*!< Generated on call deflection */
    CMBS_EV_DEE_CALL_DEFLECTION_RES,				 /*!< Generated on call deflection response */






	/* Crash events will start at 0x2900*/
    CMBS_EV_DSR_CRASH_DUMP_CONFIG = 0x2900,					/*! CMBS CRASH DUMP Configuration command*/
    CMBS_EV_DSR_CRASH_DUMP_CONFIG_RES,				/*! CMBS CRASH DUMP Configuration command Response */    
    CMBS_EV_DSR_CRASH_DUMP_START,					/*! CMBS CRASH DUMP start command from Host*/
    CMBS_EV_DSR_CRASH_DUMP_START_RES,				/*! Response for CMBS CRASH DUMP start command */    
    CMBS_EV_DSR_CRASH_DUMP_PACKETSEND,				/*! CMBS CRASH DUMP packet send command to Host*/
    CMBS_EV_DSR_CRASH_DUMP_PACKETSEND_RES,			/*! Response from host to CMBS CRASH DUMP packet send command*/
    CMBS_EV_DSR_CRASH_DUMP_END,						/*! CMBS CRASH DUMP End command from Host*/
    CMBS_EV_DSR_CRASH_DUMP_END_RES,					/*! Response for CMBS CRASH DUMP End command */
    CMBS_EV_DSR_CRASH_DUMP_READ_CONFIG,				/*! CMBS CRASH DUMP Read Configuration command*/
    CMBS_EV_DSR_CRASH_DUMP_READ_CONFIG_RES,			/*! Response for CMBS CRASH DUMP Read Configuration command*/

		

#ifdef PNCAP
	CMBS_EV_DSR_PNCAP_DATA_TX = 0x2950,				/* Carries PNCAP ETP from HOST ==> Target*/
	CMBS_EV_DSR_PNCAP_DATA_TX_RES,				/* Carries PNCAP ETP from HOST ==> Target*/
	CMBS_EV_DSR_PNCAP_DATA_RX,						/* Carries PNCAP ETP from Target ==> HOST*/
#endif	

    CMBS_EV_DSR_HAN_DEFINED_START	= 0x3000,   /*!< Home Area Network start of values */
    CMBS_EV_DSR_HAN_DEFINED_END		= 0x3FFF,   /*!< Home Area Network end of values */

    CMBS_EV_DSR_USER_DEFINED_START = 0x4000,  /*!< User defined start of values */
    CMBS_EV_INFO_SUGGEST,                     /*!<  send MM-INFO-SUGGEST */
    CMBS_EV_INFO_SUGGEST_RES,                 /*!<  response to MM-INFO-SUGGEST */

    CMBS_EV_DSR_USER_DEFINED_END = CMBS_EV_DSR_USER_DEFINED_START + 9,    /*!< User defined end of values */

    CMBS_EV_MAX
} E_CMBS_EVENT_ID;

/*! \brief CMBS Operation Modes */
typedef enum
{
    CMBS_MODE_MLB,                   /*!< Multi Line Base mode */
    CMBS_MODE_AMLB,                  /*!< Advanced Multiline Base mode */
    CMBS_MODE_DECT_PIPE,             /*!< DECT Pipe mode */
    CMBS_MODE_DECT_MAC_PIPE,         /*!< DECT MAC Pipe mode */
    CMBS_MODE_MAX
} E_CMBS_API_MODE;

/*! \brief Return codes */
typedef enum
{
    CMBS_RC_OK,                         /*!< The function was successful */
    CMBS_RC_ERROR_OUT_OF_MEM,           /*!< The needed memory was not enough */
    CMBS_RC_ERROR_MEDIA_NOT_AVAILABLE,  /*!< The media path is not available, or the CMBS is not available */
    CMBS_RC_ERROR_MEDIA_BUSY,           /*!< The destination side of communication is busy and not able to respond, yet */
    CMBS_RC_ERROR_PARAMETER,            /*!< Wrong parameter set */
    CMBS_RC_ERROR_GENERAL,              /*!< Not specified error occurred */

    CMBS_RC_ERROR_NOT_SUPPORTED,        /*!< Function not supported (i.e. CATIq in non CATIq-capable versions) */
    CMBS_RC_ERROR_PP_OUT_OF_RANGE,
    CMBS_RC_ERROR_OPERATION_TIMEOUT,

    CMBS_RC_MAX
} E_CMBS_RC;

typedef enum
{
    CMBS_DTAM_NO_RESOURCES,	        /*!< not enough resources */
    CMBS_DTAM_NO_DTAM_ON_LINE,	    /*!< Line id not associated with any DTAM */
    CMBS_DTAM_UNAVAILABLE,	        /*!< DTAM not available */
    CMBS_DTAM_MAX_SESSIONS_ON_FP,	/*!< maximum number of DTAM sessions supported reached */
    CMBS_DTAM_PINCODE_REQUIRED,	    /*!< PIN code required */
    CMBS_DTAM_REJ_REASON_MAX	
} E_CMBS_DTAM_START_SESSION_REJECT_REASON;

typedef enum
{
    CMBS_DTAM_NAK_UNKNOWN_ERROR,		 /*!< Unknown error */
    CMBS_DTAM_NAK_MSG_LIST_EMPTY,		 /*!< Message list empty */
    CMBS_DTAM_NAK_MSG_INDEX_NOT_ALLOWED, /*!< Message index 0 not allowed */
    CMBS_DTAM_NAK_MSG_INDEX_TOO_BIG, 	 /*!< Message index too big */
    CMBS_DTAM_NAK_NO_CURRENT_MSG,		 /*!< No current message defined */
    CMBS_DTAM_NAK_NO_MSG_PLAYED,		 /*!< No message was being played */
    CMBS_DTAM_NAK_NO_MSG_RECORDED,		 /*!< No message was being recorded */
    CMBS_DTAM_NAK_LIST_END,				 /*!< End of message list reached */
    CMBS_DTAM_NAK_LIST_START,			 /*!< Beginning of message list reached */
    CMBS_DTAM_NAK_INVALID_COMMAND,		 /*!< Command not implemented */
    	
    CMBS_DTAM_NAK_MAX	
} E_CMBS_DTAM_NACK_REASON;

typedef enum
{
    CMBS_DTAM_STATUS_RESERVED,			 		 /*!< Reserved */
    CMBS_DTAM_STATUS_MSG_PLAY_ENDED,	 		 /*!< Message playing has ended */
    CMBS_DTAM_STATUS_MAX_RECORDING_TIME_REACHED, /*!< Maximum recording time was  reached*/
    CMBS_DTAM_STATUS_MAX	
} E_CMBS_DTAM_STATUS;

typedef  struct
{
  u8 u8_LineId;       				//Valid DTAM Id in the range 0...63
  u16 u16_SessionId;    			//Id of newly created DTAM session,  0 if start session rejected
  u8 u8_DiscType;        			//Discriminator Type
  u16 u16_Disc;       				//EMC value
  E_CMBS_DTAM_START_SESSION_REJECT_REASON e_RejectReason; //Reason for start session reject
} ST_CMBS_DTAM_START_SESSION_CFM, *PST_CMBS_DTAM_START_SESSION_CFM;

#define CMBS_MAX_DTAM_CFM_LEN 10

typedef  struct
{
  u16 u16_SessionId;    
} ST_CMBS_DTAM_COMMAND_CFM, *PST_CMBS_DTAM_COMMAND_CFM;

typedef  struct
{
    u16 u16_SessionId;      				//Id of DTAM session
    E_CMBS_DTAM_NACK_REASON e_NackReason;	//NACK Reason
} ST_CMBS_DTAM_NACK, *PST_CMBS_DTAM_NACK;

typedef struct
{
    u16 u16_SessionId;       		//Id of DTAM session, 
    E_CMBS_DTAM_STATUS e_Status; 	//DTAM status
}ST_CMBS_DTAM_STATUS, *PST_CMBS_DTAM_STATUS;

typedef enum
{
  CMBS_DTAM_START_SESSION           		  = 0x00,
  CMBS_DTAM_START_SESSION_CFM       		  = 0x01,
  CMBS_DTAM_SELECT_NEIGHBOUR        		  = 0x04,
  CMBS_DTAM_SELECT_NEIGHBOUR_CFM    		  = 0x05,
  CMBS_DTAM_PLAY_MESSAGE            		  = 0x08,
  CMBS_DTAM_PLAY_MESSAGE_CFM        		  = 0x09,
  CMBS_DTAM_DELETE_MESSAGE          		  = 0x0A,
  CMBS_DTAM_DELETE_MESSAGE_CFM 				  = 0x0B,
  CMBS_DTAM_PAUSE_RESUME_MESSAGE    		  = 0x0C,
  CMBS_DTAM_PAUSE_RESUME_MESSAGE_CFM 		  = 0x0D,
  CMBS_DTAM_STOP_MESSAGE_PLAY       		  = 0x10,
  CMBS_DTAM_STOP_MESSAGE_PLAY_CFM   		  = 0x11,
  CMBS_DTAM_RECORD_WELCOME_MESSAGE 			  = 0x12,
  CMBS_DTAM_RECORD_WELCOME_MESSAGE_CFM  	  = 0x13,
  CMBS_DTAM_STOP_RECORD_WELCOME_MESSAGE 	  = 0x14,
  CMBS_DTAM_STOP_RECORD_WELCOME_MESSAGE_CFM   = 0x15,
  CMBS_DTAM_COMMAND_NACK                      = 0x40,
  CMBS_DTAM_STATUS                            = 0x41,
  CMBS_DTAM_INVALID_COMMAND             	  = 0xFF
}E_CMBS_DtamCommand;

typedef struct
{
    u8 u8_ManufacturerID;
    u8 u8_MemoryType;
    u8 u8_MemoryDensity;
}ST_IE_CMBS_JEDEC_ID, *PST_IE_CMBS_JEDEC_ID;
/*****************************************************************************
 * General API structures
 *****************************************************************************/

/*! \brief Serialization message header
    \note shall not use enumerator because then the size depends on compiler
         and its flags */
typedef  struct
{
    u16            u16_TotalLength;        /*!< total length without synchronisation dword */
    u16            u16_PacketNr;           /*!< packet number of message */
    u16            u16_EventID;            /*!< CMBS Event ID or internal Command ID */
    u16            u16_ParamLength;        /*!< Parameter length of followed IEs */
} ST_CMBS_SER_MSGHDR, *PST_CMBS_SER_MSGHDR;

/*! \brief Serialization message; Size = CMBS_BUF_SIZE */
typedef  struct
{
    ST_CMBS_SER_MSGHDR
       st_MsgHdr;
    u8             u8_Param[CMBS_BUF_SIZE - sizeof(ST_CMBS_SER_MSGHDR)];
} ST_CMBS_SER_MSG, *PST_CMBS_SER_MSG;

/*! \brief DECT message; Unsolicited, from target -> host */
typedef  struct
{
    u16            u16_EventID;
    void *pv_Msg;
} ST_CMBS_DECT_MSG, *PST_CMBS_DECT_MSG;


/*! \brief IE structure for generic events */
typedef  struct
{
    u8       u8_SubType;
    u16      u16_MultiPlicity;
    u8       u8_LineId;
} ST_IE_GEN_EVENT, *PST_IE_GEN_EVENT;


typedef enum
{
    CMBS_CHANGE_EDIT = 0,			 		 	/*!< List change due to edit/modification */
	CMBS_CHANGE_ADD,							/*!< List change due to addition */
	CMBS_CHANGE_DELETE,						 	/*!< List change due to deletion */
    CMBS_CHANGE_MAX	
} E_CMBS_CHANGE_REASON;


#define CMBS_MAX_LISTCHNAGE_AGGREGATION 5

/*! \brief IE structure for generic events */
typedef  struct
{
    u16       u16_Change;				//E_CMBS_LIST_CHANGE_REASON
    u16       u16_EntryId;
    u16       u16_PosIndex;
} ST_IE_GEN_EVENT_SUB_DETAILS, *PST_IE_GEN_EVENT_SUB_DETAILS;

/*! \brief IE structure for generic events */
typedef  struct
{
    u8        u8_Count;
    u16       u16_OrgHs;
    ST_IE_GEN_EVENT_SUB_DETAILS        st_SubDetails[CMBS_MAX_LISTCHNAGE_AGGREGATION];
} ST_IE_GEN_EVENT_DETAILS, *PST_IE_GEN_EVENT_DETAILS;


#define CMBS_PROP_EVENT_LENGTH 40   // same as LA10_MAC_GENERIC_DATA

/*! \brief IE structure for proprietary events */
typedef  struct
{
    u16      u16_Reserved;
    u8       u8_DataLen;
    u8       u8_Data[CMBS_PROP_EVENT_LENGTH];
} ST_IE_PROP_EVENT, *PST_IE_PROP_EVENT;


typedef  struct
{
	u16      u16_PropEvent;
	u8		 emc[2];
	u8       u8_DataLen;
	u8       u8_Data[CMBS_PROP_EVENT_LENGTH];
} ST_IE_PROP_EVENT_NOTIFY, *PST_IE_PROP_EVENT_NOTIFY;

#define CMBS_CAPABILITY_MASK   0x01

/*! \brief Capabilities */
typedef  struct
{
    u8 u8_Checksum;  // might get replaced by Bitwise definition in future
} ST_CAPABLITIES;

/*! \brief Configuration parameters for a UART device */
typedef  struct
{
    u8    u8_Port;                   /*!< COM port */
    const u8 *psz_ComDevName;            /*!< COM device name */
    u32   u32_BaudRate;              // Baud rate; trying autobauding on host side
} ST_UART_CONFIG, *PST_UART_CONFIG;
#ifdef CMBS_COMA
#ifdef USE_UDP_SOCK
/*! \brief Configuration parameters for a COMA device */
typedef  struct
{
    char ipaddr[16];
    char port[8];                   /*!< UDP port */
} ST_COMA_CONFIG, * PST_COMA_CONFIG;
#endif // USE_UDP_SOCK
#endif // CMBS_COMA

/*! \brief IOM/PCM bus mode configuration */
typedef enum
{
    CMBS_TDM_TYPE_MASTER,            /*!< CMBS PCM/IOM bus is configured as master */
    CMBS_TDM_TYPE_SLAVE,             /*!< CMBS PCM/IOM bus is configured as slave */
    CMBS_TDM_TYPE_MASTER_LOOP,       /*!< CMBS PCM/IOM bus is configured as master with slot loop-back*/
    CMBS_TDM_TYPE_SLAVE_LOOP,        /*!< CMBS PCM/IOM bus is configured as slave with slot loop-back*/
} E_CMBS_TDM_TYPE;

/*! \brief IOM/PCM bus speed configuration */
typedef enum
{
    CMBS_TDM_PCM_512,                /*!< PCM cycle  512 kHz,  64 bit per frame */
    CMBS_TDM_PCM_768,                /*!< PCM cycle  768 kHz,  96 bit per frame */
    CMBS_TDM_PCM_1536,               /*!< PCM cycle 1536 kHz, 192 bit per frame */
    CMBS_TDM_PCM_2048,               /*!< PCM cycle 2048 kHz, 256 bit per frame */
    CMBS_TDM_IOM_512,                /*!< IOM cycle  512 kHz,  64 bit per frame */
    CMBS_TDM_IOM_768,                /*!< IOM cycle  768 kHz,  96 bit per frame */
    CMBS_TDM_IOM_1536,               /*!< IOM cycle 1536 kHz, 192 bit per frame */
    CMBS_TDM_IOM_2048,               /*!< IOM cycle 2048 kHz, 256 bit per frame */
} E_CMBS_TDM_SPEED;

/*! brief IOM/PCM bus synchronization way */
typedef enum
{
    CMBS_TDM_SYNC_SHORT_FR,          /*!< Short frame-sync FR enclosing the first rising clock edge */
    CMBS_TDM_SYNC_SHORT_FF,          /*!< Short frame-sync FF enclosing the first falling clock edge */
    CMBS_TDM_SYNC_SHORT_LF,          /*!< Short frame-sync LF enclosing the last falling clock edge */
    CMBS_TDM_SYNC_LONG               /*!< Long  frame-sync over the first slot (8 bit) */
} E_CMBS_TDM_SYNC;

/*! \brief Configuration parameters for a PCM/IOM device */
typedef struct
{
    E_CMBS_TDM_TYPE   e_Type;        /*!< Master/Slave _LOOP */
    E_CMBS_TDM_SPEED  e_Speed;       /*!< IOM/PCM 512,768,1536,2048 */
    E_CMBS_TDM_SYNC   e_Sync;        /*!< SYNC_LONG, SYNC_SHORT_FR, SYNC_SHORT_FF, SYNC_SHORT_LF */
    u16               u16_SlotEnable; /*!< number of slot enabled */

} ST_TDM_CONFIG, *PST_TDM_CONFIG;

/*! \brief Device types */
typedef enum
{
    CMBS_DEVTYPE_UART,               /*!< UART communication device */
    CMBS_DEVTYPE_USB,                /*!< USB device */
    CMBS_DEVTYPE_SPI,                /*!< SPI */
    CMBS_DEVTYPE_TDM,                /*!< TDM */
#ifdef CMBS_COMA
    CMBS_DEVTYPE_COMA,               /*!< COMA */
#endif
    CMBS_DEVTYPE_MAX
} E_CMBS_DEVTYPE;

#define LEGACY_CMD_HELLO_LENGTH	5

/*! \brief UART Flow control types */
typedef enum
{
    CMBS_FLOW_CTRL_NONE = 0,        //must be zero
    CMBS_FLOW_CTRL_XON_XOFF,
    CMBS_FLOW_CTRL_RTS_CTS,
    CMBS_FLOW_CTRL_DSR_DTR
}E_CMBS_FLOW_CTRL;

/*! \brief Communication device */
typedef  struct
{
    E_CMBS_DEVTYPE    e_DevType;     /*!< Device type */

    union
    {
        PST_UART_CONFIG
           pUartCfg;      /*!< UART configuration parameters */
        PST_TDM_CONFIG pTdmCfg;       /*!< TDM configuration parameters */
#ifdef CMBS_COMA
#ifdef USE_UDP_SOCK
        PST_COMA_CONFIG
        pComaCfg;      /*!< COMA configuration parameters */
#endif
#endif
    } u_Config;                      /*!< Device configuration parameter set */

    E_CMBS_FLOW_CTRL e_FlowCTRL;        /*!<Flow control type */

} ST_CMBS_DEV, *PST_CMBS_DEV;


/*****************************************************************************
 * IE - Param structures and enumerations
 *****************************************************************************/

/*! \brief Information element types */
typedef enum
{
    CMBS_IE_UNDEF,
    CMBS_IE_CALLINSTANCE,            /* !< Call Instance identifier */
    CMBS_IE_CALLERPARTY,             /* !< Caller ID and its properties */
    CMBS_IE_CALLERNAME,              /* !< Caller Name */
    CMBS_IE_CALLEDPARTY,             /* !< Called ID, e.g. handset, group, line number */
    CMBS_IE_CALLPROGRESS,            /* !< Progress information e.g. ringing, alert, tone selection, etc. */
    CMBS_IE_CALLINFO,                /* !< Out of band information, e.g. digits, keys, etc. */
    CMBS_IE_DISPLAY_STRING,          /* !< String container to display content on handset */
    CMBS_IE_CALLRELEASE_REASON,      /* !< Call Release Reason */
    CMBS_IE_CALLSTATE,               /* !< Call State */
    CMBS_IE_MEDIACHANNEL,            /* !< Channel ID and Type */
    CMBS_IE_MEDIA_INTERNAL_CONNECT,  /* !< Channel ID and Type */
    CMBS_IE_MEDIADESCRIPTOR,         /* !< Description of media properties, e.g. codec, priority, etc. */
    CMBS_IE_TONE,                    /* !< Tone properties for the tone generator */
    CMBS_IE_TIMEOFDAY,               /* !< Time in ms since 01.01.1970 */
    CMBS_IE_HANDSETINFO,             /* !< HS number, status, type, model and IPEI */
    CMBS_IE_PARAMETER,               /* !< CMBS parameter */
    CMBS_IE_SUBSCRIBED_HS_LIST,      /* !< List of subscribed handsets */
    CMBS_IE_LINE_SETTINGS_LIST,      /* !< List of Line settings */
    CMBS_IE_RESERVED_1,              /* !< Reserved */
    CMBS_IE_FW_VERSION,              /* !< CMBS firmware module and version */
    CMBS_IE_SYS_LOG,                 /* !< Base station system log string */
    CMBS_IE_RESPONSE,                /* !< Response (ok or error) used in response events */
    CMBS_IE_STATUS,                  /* !< Contain the CMBS status up, down, removed, etc. */
    CMBS_IE_INTEGER_VALUE,           /* !< Used for internal purposes */
    CMBS_IE_LINE_ID,                 /* !< External line Id */
    CMBS_IE_PARAMETER_AREA,          /* !< CMBS parameter area */
    CMBS_IE_REQUEST_ID,              /* !< Unique Id for requests sent from Host to CMBS */
    CMBS_IE_HANDSETS,                /* !< Bit mask of one or more handsets */
    CMBS_IE_GEN_EVENT,               /* !< Generic event; for internal usage */
    CMBS_IE_PROP_EVENT,              /* !< Proprietary event; for internal usage */
    CMBS_IE_DATETIME,                /* !< Holds the date and time of day (Y/M/D/H/M/S/Zone) */
    CMBS_IE_DATA,                    /* !< Holds data and length */
    CMBS_IE_DATA_SESSION_ID,         /* !< Id of the data session */
    CMBS_IE_DATA_SESSION_TYPE,       /* !< Type of data (IWU-INFO,CF-Channel, etc.) */
    CMBS_IE_LA_SESSION_ID,           /* !< Id of the List Access Session */
    CMBS_IE_LA_LIST_ID,              /* !< Id of the list being accessed via Access List Service */
    CMBS_IE_LA_FIELDS,               /* !< Holds fields idetifiers of a list. */
    CMBS_IE_LA_SORT_FIELDS,          /* !< Holds sort fields of a list. */
    CMBS_IE_LA_EDIT_FIELDS,          /* !< Holds editable fields of a list. */
    CMBS_IE_LA_CONST_FIELDS,         /* !< Holds non-edit fields of a list. */
    CMBS_IE_LA_SEARCH_CRITERIA,      /* !< The criteria for the search */
    CMBS_IE_LA_ENTRY_ID,             /* !< The Id of an entry in a list */
    CMBS_IE_LA_ENTRY_INDEX,          /* !< The index of an entry in a list */
    CMBS_IE_LA_ENTRY_COUNT,          /* !< The number of entries in a list */
    CMBS_IE_LA_IS_LAST,              /* !< Boolean to hold indicate last data segment */
    CMBS_IE_LA_REJECT_REASON,        /* !< Reject Reason (0 means "No Error") */
    CMBS_IE_LA_NR_OF_ENTRIES,        /* !< Nr of Entries */
    CMBS_IE_CALLTRANSFERREQ,         /* !< Request for call transfer approve from host*/
    CMBS_IE_HS_NUMBER,               /* !< Handset Number */
    CMBS_IE_SHORT_VALUE,             /* !< used for internal purpose */
    CMBS_IE_ATE_SETTINGS,            /* !< used for ATE test */
    CMBS_IE_LA_READ_DIRECTION,       /* !< Read direction (forward / backward) when browsing lists */
    CMBS_IE_LA_MARK_REQUEST,         /* !< mark entries as read / unread / leave unchanged */
    CMBS_IE_LINE_SUBTYPE,            /* !< Line SubType */
    CMBS_IE_AVAIL_VERSION_DETAILS,   /* !< Available version details */
    CMBS_IE_HS_VERSION_BUFFER,       /* !< Structure for representing HS version string  */
    CMBS_IE_HS_VERSION_DETAILS,      /* !< Handset version details */
    CMBS_IE_SU_SUBTYPE,              /* !< ID for transferring SU subtype */
    CMBS_IE_URL,                     /* !< URL message */
    CMBS_IE_NUM_OF_URLS,             /* !< ID for transferring Num of URLs */
    CMBS_IE_REJECT_REASON,           /* !< ID for transferring reject reason */
    CMBS_IE_NB_CODEC_OTA,            /* !< NarrowBand Codec to be used Over-The-Air */
    CMBS_IE_TARGET_LIST_CHANGE_NOTIF, /* !< Information regarding list change notification (Towards Host) */
    CMBS_IE_HW_VERSION,              /* !< CMBS hardware module */
    CMBS_IE_DECT_SETTINGS_LIST,      /* !< DECT Settings */
    CMBS_IE_RTP_SESSION_INFORMATION, /* !< RTP Session Information */
    CMBS_IE_RTCP_INTERVAL,           /* !< RTCP Interval value */
    CMBS_IE_RTP_DTMF_EVENT,          /* !< RTP DTMF Event */
    CMBS_IE_RTP_DTMF_EVENT_INFO,     /* !< RTP DTMF Event extended information */
    CMBS_IE_RTP_FAX_TONE_TYPE,       /* !< Fax Tone Type */
    CMBS_IE_INTERNAL_TRANSFER,       /* !< Internal Transfer */
    CMBS_IE_LA_PROP_CMD,             /* !< Proprietary List Access Command */
    CMBS_IE_MELODY,                  /* !< Ringing/alerting pattern for external incoming calls */
    CMBS_IE_BASE_NAME,               /* !< FP Base station name (For CAT-iq 2.0 feature base station name selection) */
    CMBS_IE_REG_CLOSE_REASON,        /* !< used for sending the reason why registration was closed */
    CMBS_IE_EEPROM_VERSION,          /* !< CMBS EEPROM version */
    CMBS_IE_DC_REJECT_REASON,        /* !< CMBS data call reject reason */
    CMBS_IE_SUOTA_APP_ID,            /* !< Identify the type of application*/
    CMBS_IE_SUOTA_SESSION_ID,        /* !< Identify GMEP session ID. */
    CMBS_IE_HS_PROP_EVENT,           /* !< Handset Proprietary Event */
    CMBS_IE_SYPO_SPECIFICATION,      /*!< Parameters to configure the SYPO feature */
    CMBS_IE_AFE_ENDPOINT_CONNECT,    /*!< The two AFE endpoints that should be connected */
    CMBS_IE_AFE_ENDPOINT,            /*!< Endpoint specification */
    CMBS_IE_AFE_ENDPOINT_GAIN,       /*!< Endpoint gain specification */
    CMBS_IE_AFE_ENDPOINT_GAIN_DB,    /*!< Endpoint gain specification in dB */
    CMBS_IE_AFE_AUX_MEASUREMENT_SETTINGS,   /*!< Measurement settings for AUX measurement */
    CMBS_IE_AFE_AUX_MEASUREMENT_RESULT,     /*!< Measurement result for AUX measurement */
    CMBS_IE_AFE_RESOURCE_TYPE,              /*!< Resource type - DAC or DCALSS  */
    CMBS_IE_AFE_INSTANCE_NUM,               /*!< Instance NUM - DAC0 or DAC1 */
    CMBS_IE_DHSG_VALUE,                     /*!< Value to pass to DHSG */
    CMBS_IE_GPIO_ID,                        /*!< Define which GPIO bank and pin to use */
    CMBS_IE_GPIO_MODE,                      /*!< GPIO set to OUTput or INput */
    CMBS_IE_GPIO_VALUE,                     /*!< GPIO value set / reset */
    CMBS_IE_GPIO_PULL_TYPE,                 /*!< GPIO PULL UP/DOWN */
    CMBS_IE_GPIO_PULL_ENA,                  /*!< GPIO PULL Enable / Disable */
    CMBS_IE_GPIO_ENA,                       /*!< GPIO Enable / Disable */
    CMBS_IE_CALLEDNAME,                     /* !< Called party Name (matching dialed number in contact list) */
    CMBS_IE_EXT_INT_CONFIGURATION,          /*!< polarity, type (edge/level) */
    CMBS_IE_EXT_INT_NUM,                    /*!< number of external interrupt to configure/activate */
    CMBS_IE_TERMINAL_CAPABILITIES,          /*!< Terminal Capabilities of the HS */
    CMBS_IE_CHECKSUM_ERROR,                 /*!< CHECKSUM is not correct or not received */
    CMBS_IE_CALL_HOLD_REASON,               /*!< Reason due to which call is put on hold*/
    CMBS_IE_SUOTA_FILE_INFO,    			/*!< The total size of the file for SUOTA, "to" and "from" bytes*/
	CMBS_IE_SPLASH_TIMER,					/*!< Duration of Splash ring */
	CMBS_IE_AUDIO_SHMEM,                    /*!< Audio via Shared Memory */
    CMBS_IE_TONE_VOLUME,                    /*!< TOG volume for inband tones */
    CMBS_IE_REG_ENABLE_TYPE,                /*!< Registration enable type: All, handset or ULE */
    CMBS_IE_PROP_FIELDS_LIST,        		/*!< Proprietary DECT Settings */
    CMBS_IE_DTAM_SESSION_ID,                /*!< DTAM session ID */        
    CMBS_IE_DTAM_SELECT_NEXT_MSG,   		/*!< DTAM Select next message */
    CMBS_IE_DTAM_MSG_TYPE,                  /*!< DTAM message type (Welcome/Incoming)  */
    CMBS_IE_DTAM_MSG_INDEX,            		/*!< DTAM message Index  */
    CMBS_IE_DTAM_PLAY_MODE,     			/*!< DTAM message play mode (play or Restart/Rewind  */
    CMBS_IE_DTAM_NUM_OF_SECONDS, 			/*!< Number of secods to play  */
    CMBS_IE_DTAM_START_SESSION_CFM, 		/*!< Structure that holds paramters of start session confirm message  */
    CMBS_IE_DTAM_COMMAND,       			/*!< Structure that holds parameters that are common for DTAM commands  */
    CMBS_IE_DTAM_COMMAND_NACK, 				/*!< Structure that holds paramters of command NACK message  */
    CMBS_IE_DTAM_STATUS, 					/*!< Structure that holds paramters of DTAM status message  */
    CMBS_IE_RSSI_VALUE,                     /*!< RSSI of a HS */
    CMBS_IE_STATE_FILTER_TYPE,         		/*!< Enables to define which kind of Call State massages is visible to the Host */
    CMBS_IE_SLIC_LINE_TEST_RESULTS,         /*!< Slic Line Test Results */    
    CMBS_IE_FXS_HOOK_STATUS,				/*!< FXS port hook status */
    CMBS_IE_CALL_DIRECTION,					/*!< Call type or direction (Incoming / outgoing etc.) */	
    CMBS_IE_JEDEC_ID,                       /*!< JDEC ID (3 bytes - Manufacturer ID & Device ID) */
    CMBS_IE_PROP_EVENT_NOTIFY,              /* !< Proprietary event; for internal usage */
    CMBS_IE_UPDATED_DEVICE_STATE,               /* !< ULE Device Call State */
    CMBS_IE_DTMF_TONE_DUR,					/* !< Allows to modify DTMF Tone Play Duration */
    CMBS_IE_CALL_LIST,						/* !< CALL INFO of active call in Target */
    CMBS_IE_TDM_SLOTS,                      /*!< TDM slots required to be configured */
    CMBS_IE_SLIC_NLT_CAP_TEST_RESULTS,         /*!< Slic Nlt Cpapbilities Test Results */    
    CMBS_IE_DTMF_TONE_TO_HS,				/* !< Allows to play DTMF tone on HS Side*/
    CMBS_IE_DEVICE_LIST_STATUS,               /* !< ULE Device List in call */
    CMBS_IE_HSRELEASE_REASON,
    CMBS_IE_CRASH_DUMP_ENABLE,				/*!< Crash Dump Enable/Disable */
    CMBS_IE_PACKET_NUMBER,					/*!< Packet number for data packet tracking */

    CMBS_IE_CALL_SCREENING,               /*!< Call Screening Accept / Intercept */

    CMBS_IE_LA_SELECTION_TYPE,          /* !< Selection type for Read Selected Entries */
    CMBS_IE_LA_ENTRY_RANGE,		   		/* !< Selection from entry id with range */
    CMBS_IE_LA_ENTRY_IDS,				/* !< Selection from entry ids */

    CMBS_IE_GEN_EVENT_DETAILS,				/* !< List change details */
    CMBS_IE_PRODUCT_ID,						/* !< List change details */

	// !! When adding a new IE -- change HARD CODED counter in Ietest.c
	
	CMBS_IE_MAX_REGULAR_IE,                 /*!< Last non user defined / ULE / Checksum - used for IETest project.*/
    CMBS_IE_CHECKSUM = 0x0F00,       /* !< Checksum IE. NOTE: For compatibility reasons: Never change this IE number */
	
    
    	
    CMBS_IE_HAN_DEFINED_START	= 0x3000,		/*!< HAN defined start */
    CMBS_IE_HAN_DEFINED_END		= 0x3FFF,		/*!< HAN defined end */
     
    CMBS_IE_USER_DEFINED_START = 0x4000,      /* !< User defined start */
    CMBS_IE_USER_DEFINED_END = CMBS_IE_USER_DEFINED_START + 9, /*!< User defined end */

    CMBS_IE_MAX
} E_CMBS_IE_TYPE;


/*! \brief Call info types */
typedef enum
{
    CMBS_CALL_INFO_TYPE_UNDEF,
    CMBS_CALL_INFO_TYPE_DISPLAY,     /*!< In-band progress information is available */
    CMBS_CALL_INFO_TYPE_DIGIT,       /*!< Inband digit/control information available: '0-9,*,#,Hook' */
    CMBS_CALL_INFO_TYPE_MAX
} E_CMBS_CALL_INFO_TYPE;


/*! \brief IE structure for CMBS_IE_CALLINFO */
typedef  struct
{
    E_CMBS_CALL_INFO_TYPE
       e_Type;           /*!< Call info type */
    u8             u8_DataLen;       /*!< Length of data in bytes */
    u8 *pu8_Info;         /*!< Info data byte array e.g. digits, keys, etc. */
} ST_IE_CALLINFO, *PST_IE_CALLINFO;


/*! \brief Call progress info */
typedef enum
{
    CMBS_CALL_PROGR_UNDEF,
    CMBS_CALL_PROGR_SETUP_ACK,          /*!< Establish acknowledge */
    CMBS_CALL_PROGR_PROCEEDING,         /*!< Routing the call to destination */
    CMBS_CALL_PROGR_RINGING,            /*!< Destination is ringing */
    CMBS_CALL_PROGR_BUSY,               /*!< Destination is busy */
    CMBS_CALL_PROGR_CALLWAITING,        /*!< Call waiting is available */
    CMBS_CALL_PROGR_INBAND,             /*!< Inband signal available */
    CMBS_CALL_PROGR_DISCONNECTING,      /*!< Call is disconnecting */
    CMBS_CALL_PROGR_IDLE,               /*!< Call is released */
    CMBS_CALL_PROGR_HOLD,               /*!< Call on hold */
    CMBS_CALL_PROGR_CONNECT,            /*!< Call is connected */
    CMBS_CALL_PROGR_PROCEED_EMERGENCY,	/*!< Call is connected */
	CMBS_CALL_PROGR_FAX,				/*!< Call is connected */

    
    CMBS_CALL_PROGR_CHGDIROG,
    CMBS_CALL_PROGR_CHGDIRIC,
    CMBS_CALL_PROGR_FXS_NORMAL_TO_BYPASS,
    CMBS_CALL_PROGR_FXS_BYPASS_TO_NORMAL,
    CMBS_CALL_PROGR_FAX_TX,
    CMBS_CALL_PROGR_FAX_RX,
    CMBS_CALL_PROGR_SCREEN_SETUP, 		/*!< Call Screening Setup */
    CMBS_CALL_PROGR_SCREEN_CONNECT, 	/*!< Call Screening Connect */
    
    CMBS_CALL_PROGR_REMOTE_HOLD,
    CMBS_CALL_PROGR_REMOTE_CONNECT,

	CMBS_CALL_PROGR_EXIT_CONFERENCE,	/*!< Inform HS leave conference state */
    CMBS_CALL_PROGR_CONFERENCE,         /*!< Inform HS enter conference state */


    // must be last!
    CMBS_CALL_PROGR_MAX,
} E_CMBS_CALL_PROGRESS;


#define	CMBS_EMERGENCY_CALL     1
#define	CMBS_NOT_EMERGENCY_CALL 0

/*! \brief IE structure for CMBS_IE_CALLPROGRESS */
typedef  struct
{
    E_CMBS_CALL_PROGRESS
       e_Progress;       /*!< Call progress info */
} ST_IE_CALLPROGRESS, *PST_IE_CALLPROGRESS;


#define  CMBS_DISPLAY_STRING_LENGTH 50

/*! \brief IE structure for CMBS_IE_DISPLAY_STRING */
typedef  struct
{
    u8             u8_DataLen;       /*!< Length of data in bytes */
    u8 *pu8_Info;         /*!< String e.g. alphanumeric, etc. */
} ST_IE_DISPLAY_STRING, *PST_IE_DISPLAY_STRING;


/*! \brief Call release reasons */
typedef  enum
{
    /* Call Release Reason : General Messages */
    CMBS_REL_REASON_NORMAL = 0,
    CMBS_REL_REASON_UNEXPECTED_MESSAGE,
    CMBS_REL_REASON_UNKNOWN_TRANC_ID,
    CMBS_REL_REASON_MANDATORY_IE_MISSING,
    CMBS_REL_REASON_INVALID_IE_CONTENT,
    CMBS_REL_REASON_INCOMAPTIBLE_SERVICE,
    CMBS_REL_REASON_SERVICE_NOT_IMPLEMENTED,
    CMBS_REL_REASON_NEGOTIATION_NOT_SUPPORTED,
    CMBS_REL_REASON_INVALID_IDENTITY,
    CMBS_REL_REASON_AUTHENTICATION_FAILED,
    CMBS_REL_REASON_UNKNOWN_IDENTITY,
    CMBS_REL_REASON_NEGOTIATION_FAILED,
    CMBS_REL_REASON_GENERAL_RESERVED,
    CMBS_REL_REASON_TIMER_EXPIRY,
    CMBS_REL_REASON_PARTIAL_RELEASE,
    CMBS_REL_REASON_UNKNOWN,

    /* Call Release Reason : User Messages */
    CMBS_REL_REASON_USER_DETACHED = 0x10,
    CMBS_REL_REASON_USER_NOT_IN_RANGE,
    CMBS_REL_REASON_USER_UNKNOWN,
    CMBS_REL_REASON_USER_ALREADY_ACTIVE,
    CMBS_REL_REASON_USER_BUSY,
    CMBS_REL_REASON_USER_REJECTION,
    CMBS_REL_REASON_USER_CALL_MODIFY,
    CMBS_REL_REASON_USER_CALL_TRANSFER,
    CMBS_REL_REASON_USER_RESERVED,

    /* Call Release Reason : External Handover Messages */
    CMBS_REL_REASON_EXT_HANDOVER_RESERVED = 0x20,
    CMBS_REL_REASON_EXT_HANDOVER_NOT_SUPPORTED,
    CMBS_REL_REASON_NETWORK_PARAMETER_MISSING,
    CMBS_REL_REASON_EXT_HANDOVER_RELEASE,
    CMBS_REL_REASON_NETWORK_NO_RESPONSE,
    CMBS_REL_REASON_NETWORK_ERROR,
    CMBS_REL_REASON_NETWORK_FORBIDDEN,

    /* Call Release Reason : Temporary Overload Messages */
    CMBS_REL_REASON_TEMP_OVERLOAD_RESERVED = 0x30,
    CMBS_REL_REASON_OVERLOAD,
    CMBS_REL_REASON_INSUFFICIENT_RESOURCES,
    CMBS_REL_REASON_INSUFFICIENT_BEARERS_AVAILABLE,
    CMBS_REL_REASON_IWU_CONGESTION,

    /* Call Release Reason : Security Messages */
    CMBS_REL_REASON_SECURITY_ATTACK_ASSUMED = 0x40,
    CMBS_REL_REASON_ENCYPTION_ACTIVATION_FAILED,
    CMBS_REL_REASON_RE_KEYING_FAILED,
    CMBS_REL_REASON_NO_CIPHER_KEY_AVL,

    /* Call status reason in release reason: Proprietary*/
    CMBS_REL_REASON_LINE_IN_USE = 0xA0,
    CMBS_REL_REASON_NUMBER_NOT_AVL,
	
    CMBS_REL_SPLASH_RING_TIMEOUT,
    CMBS_REL_REASON_ABNORMAL,
    CMBS_REL_REASON_UNSUPPORTED_MEDIA,
    CMBS_REL_REASON_REDIRECT,
    CMBS_REL_REASON_CTRL_CODE_NOT_SUP,
    CMBS_REL_REASON_CTRL_CODE_FAILED,

	
    CMBS_REL_REASON_MAX = 0xFF

} E_CMBS_REL_REASON;


/*! \brief IE structure for CMBS_IE_CALLRELEASE_REASON */
typedef  struct
{
    E_CMBS_REL_REASON
       e_Reason;         /*!< Internal mapped reason */
    u32            u32_ExtReason;    /*!< Extended reason to allow notification of network specific release reason */
} ST_IE_RELEASE_REASON, *PST_IE_RELEASE_REASON;

typedef struct
{
  u8 u8_HsHr;
  E_CMBS_REL_REASON e_Reason;
}ST_IE_HS_RELEASE_REASON;

/*! \brief Audio codecs */
typedef  enum
{
    CMBS_AUDIO_CODEC_UNDEF,
    CMBS_AUDIO_CODEC_PCMU,           /*!< Payload type PCMU 8 bit 8kHz */
    CMBS_AUDIO_CODEC_PCMA,           /*!< Payload type PCMA 8 bit 8kHz */
    CMBS_AUDIO_CODEC_PCMU_WB,        /*!< Payload type PCMU 8 bit 16kHz */
    CMBS_AUDIO_CODEC_PCMA_WB,        /*!< Payload type PCMA 8 bit 16kHz */
    CMBS_AUDIO_CODEC_PCM_LINEAR_WB,  /*!< Payload type PCM 16 bit 16kHz */
    CMBS_AUDIO_CODEC_PCM_LINEAR_NB,  /*!< Payload type PCM 16 bit 8kHz */
    CMBS_AUDIO_CODEC_PCM8,           /*!< Payload type PCM  8 bit 8kHz */
    CMBS_AUDIO_CODEC_MAX
} E_CMBS_AUDIO_CODEC;


/*! \brief IE structure for CMBS_IE_MEDIADESCRIPTOR */
typedef  struct
{
    E_CMBS_AUDIO_CODEC   e_Codec;    /*!< Used codec number according RFC 3551 */
    u8                   pu8_CodecsList[CMBS_AUDIO_CODEC_MAX];   /*!< Codecs list */
    u8                   u8_CodecsLength;
} ST_IE_MEDIA_DESCRIPTOR, *PST_IE_MEDIA_DESCRIPTOR;


/*! \brief Media types */
typedef  enum
{
    CMBS_MEDIA_TYPE_AUDIO_IOM,       /*!< Media channel type audio IOM */
    CMBS_MEDIA_TYPE_AUDIO_USB,       /*!< Media channel type audio USB pipe*/
    CMBS_MEDIA_TYPE_DATA,            /*!< Media channel type general data */
    CMBS_MEDIA_TYPE_RTP,
    CMBS_MEDIA_TYPE_AUDIO_NODE,      /*!< Media channel type audio IOM */
    CMBS_MEDIA_TYPE_AUDIO_INTERN,    /*!< Media channel type Internal call (no IOM) */
    CMBS_MEDIA_TYPE_AUDIO_SHMEM,    /*!< Media channel type audio Shared Memory */	
    CMBS_MEDIA_TYPE_MAX
} E_CMBS_MEDIA_TYPE;


/*! \brief IE structure for CMBS_IE_MEDIACHANNEL */
typedef  struct
{
    u32            u32_ChannelID;    /*!< Channel ID of referred media type, lower 16bit reserved, higher 16bit for application itself */
    u32            u32_ChannelParameter; /*!< Channel Parameter provides information about the parameter settings, e.g. IOM - used slots */
    E_CMBS_MEDIA_TYPE
       e_Type;           /*!< Defines the media channel type */

} ST_IE_MEDIA_CHANNEL, *PST_IE_MEDIA_CHANNEL;


/*! \brief NB Codec OTA types */
typedef enum
{
    CMBS_NB_CODEC_OTA_G726,     /*!< G.726 Codec */
    CMBS_NB_CODEC_OTA_G711A,    /*!< G.711 A-law Codec */
    CMBS_NB_CODEC_OTA_G711U,    /*!< G.711 µ-law Codec */

    CMBS_NB_CODEC_OTA_MAX
} E_CMBS_NB_CODEC_OTA_TYPE;


/*! \brief IE structure for CMBS_IE_NB_CODEC_OTA */
typedef  struct
{
    E_CMBS_NB_CODEC_OTA_TYPE e_Codec;           /*!< Defines the codec to be used OTA in case the call is established as NB */

} ST_IE_NB_CODEC_OTA, *PST_IE_NB_CODEC_OTA;

/*! \brief Internal Connection Operation Type */
typedef  enum
{
    CMBS_MEDIA_IC_DISCONNECT,       /*!< Disconnect to a Node */
    CMBS_MEDIA_IC_CONNECT,          /*!< Connect to a Node    */
} E_CMBS_MEDIA_IC_TYPE;

#define NUM_CMBS_CONFERENCE 2

/*! \brief IE structure for CMBS_IE_MEDIA_INTERNAL_CONNECT */
typedef  struct
{
    u32            u32_ChannelID;    /*!< Channel ID of referred media type, lower 16bit reserved, higher 16bit for application itself */
    u32            u32_NodeId;       /*!< 0.. NUM_CMBS_CONFERENCE-1  */
    E_CMBS_MEDIA_IC_TYPE
       e_Type;           /*!< Defines the Internal Connection Type */

} ST_IE_MEDIA_INTERNAL_CONNECT, *PST_IE_MEDIA_INTERNAL_CONNECT;




/*! \brief Tones */
typedef  enum
{
    CMBS_TONE_DIAL,                  /*!< Dial tone according country spec. */
    CMBS_TONE_MWI,                   /*!< Message waiting country spec. */
    CMBS_TONE_RING_BACK,             /*!< Ring-back tone according country spec. */
    CMBS_TONE_BUSY,                  /*!< Busy tone according country spec. */
    CMBS_TONE_CALL_WAITING,          /*!< Call waiting tone according country spec. */
    CMBS_TONE_DIAL_CALL_FORWARD,     /*!< Call forwarding, only for Swiss */
    CMBS_TONE_MWI_OR_CONGESTION,     /*!< Congestion - inband, only for Swiss */

    // Added for French tone style
    CMBS_TONE_DIAL_FT_FR = CMBS_TONE_DIAL_CALL_FORWARD, /*!< Dial tone french style */
    CMBS_TONE_MWI_FT_FR = CMBS_TONE_MWI_OR_CONGESTION,  /*!< Message waiting tone french style */
    CMBS_TONE_RING_BACK_FT_FR,             /*!< Ring-back tone french style */
    CMBS_TONE_BUSY_FT_FR,                  /*!< Busy tone french style */
    CMBS_TONE_CALL_WAITING_FT_FR,          /*!< Call waiting tone french style */

    // Added for Polish tone style
    CMBS_TONE_DIAL_FT_PL,                  /*!< Dial tone polish style */
    CMBS_TONE_MWI_FT_PL,                   /*!< Message waiting tone polish style */
    CMBS_TONE_RING_BACK_FT_PL,             /*!< Ring-back tone polish style */
    CMBS_TONE_BUSY_FT_PL,                  /*!< Busy tone polish style */
    CMBS_TONE_CALL_WAITING_FT_PL,          /*!< Call waiting tone polish style */

    /*! outband tones - Sent with <<SIGNAL>> IE to handsets */

    CMBS_TONE_FIRST_TONE_OUTBAND,
    CMBS_TONE_DIAL_OUTBAND  = CMBS_TONE_FIRST_TONE_OUTBAND,
    CMBS_TONE_RING_BACK_OUTBAND,
    CMBS_TONE_BUSY_OUTBAND,
    CMBS_TONE_CALL_WAITING_OUTBAND,
    CMBS_TONE_OFF_OUTBAND,
    CMBS_TONE_LAST_TONE_OUTBAND = CMBS_TONE_OFF_OUTBAND,

    CMBS_TONE_INTERCEPT,             /*!< Interception tone */
    CMBS_TONE_NWK_CONGESTION,        /*!< Network congestion tone */
    CMBS_TONE_CONFIRM,               /*!< Confirmation tone */
    CMBS_TONE_ANSWER,                /*!< Answering tone */
    CMBS_TONE_OFF_HOOK_WARN,         /*!< Off Hook warning tone */
    CMBS_TONE_HINT,                  /*!< Hint tone */
    CMBS_TONE_OK,                    /*!< OK tone */
    CMBS_TONE_NOK,                   /*!< Not OK tone */
    CMBS_TONE_DTMF_0,                /*!< DTMF tone 0 */
    CMBS_TONE_DTMF_1,                /*!< DTMF tone 1 */
    CMBS_TONE_DTMF_2,                /*!< DTMF tone 2 */
    CMBS_TONE_DTMF_3,                /*!< DTMF tone 3 */
    CMBS_TONE_DTMF_4,                /*!< DTMF tone 4 */
    CMBS_TONE_DTMF_5,                /*!< DTMF tone 5 */
    CMBS_TONE_DTMF_6,                /*!< DTMF tone 6 */
    CMBS_TONE_DTMF_7,                /*!< DTMF tone 7 */
    CMBS_TONE_DTMF_8,                /*!< DTMF tone 8 */
    CMBS_TONE_DTMF_9,                /*!< DTMF tone 9 */
    CMBS_TONE_DTMF_STAR,             /*!< DTMF tone * */
    CMBS_TONE_DTMF_HASH,             /*!< DTMF tone # */
    CMBS_TONE_DTMF_A,                /*!< DTMF tone A */
    CMBS_TONE_DTMF_B,                /*!< DTMF tone B */
    CMBS_TONE_DTMF_C,                /*!< DTMF tone C */
    CMBS_TONE_DTMF_D,                /*!< DTMF tone D */
    CMBS_TONE_USER_DEF,              /*!< User defined tone, not supported, yet. */
    CMBS_TONE_MAX
} E_CMBS_TONE;


/*! \brief INBAND tones country selection */
/* see also CS01_INBAND* */
typedef enum
{
    CMBS_INBAND_DEFAULT = 0,
    CMBS_INBAND_FRENCH,
    CMBS_INBAND_POLISH,
    CMBS_INBAND_SWISS,
    CMBS_INBAND_ITALY,
    CMBS_INBAND_KOREA
} E_CMBS_INBAND_COUNTRY;


/*! \brief IE structure for CMBS_IE_TONE */
typedef  struct
{
    E_CMBS_TONE    e_Tone;
} ST_IE_TONE, *PST_IE_TONE;


/* Address properties */
#define  CMBS_ADDR_PROPTYPE_UNKNOWN       0x00  /*!< \brief Unknown number type */
#define  CMBS_ADDR_PROPTYPE_INTERNATIONAL 0x10  /*!< \brief International addressing */
#define  CMBS_ADDR_PROPTYPE_NATIONAL      0x20  /*!< \brief National addressing */
#define  CMBS_ADDR_PROPTYPE_NET           0x30  /*!< \brief Network specific address */
#define  CMBS_ADDR_PROPTYPE_SUBSCRIBER    0x40  /*!< \brief Subscriber address */
#define  CMBS_ADDR_PROPTYPE_MASK          0xF0  /*!< address type mask */

#define  CMBS_ADDR_PROPPLAN_UNKNOWN       0x00  /*!< \brief Unknown address plan */
#define  CMBS_ADDR_PROPPLAN_E164          0x01  /*!< \brief E.164 format */
#define  CMBS_ADDR_PROPPLAN_NAT_STD       0x08  /*!< \brief National Standard plan */
#define  CMBS_ADDR_PROPPLAN_PRIVATE       0x09  /*!< \brief Private plan */
#define  CMBS_ADDR_PROPPLAN_SIP           0x0A  /*!< \brief SIP address, as defined in RFC 3261 */
#define  CMBS_ADDR_PROPPLAN_INTHS         0x0B  /*!< \brief Internal DECT handset */
#define  CMBS_ADDR_PROPPLAN_INTGRP        0x0C  /*!< \brief Internal DECT group */
#define  CMBS_ADDR_PROPPLAN_INTLINE       0x0E  /*!< \brief Internal DECT line attachment,
                                                            the default handset line assignment is used */
#define  CMBS_ADDR_PROPPLAN_MASK          0x0F  /*!< address plan mask */

/* Address presentation */
#define  CMBS_ADDR_PRESENT_ALLOW          0x00  /*!< \brief The address can be displayed */
#define  CMBS_ADDR_PRESENT_DENIED         0x20  /*!< \brief The address can not be displayed */
#define  CMBS_ADDR_PRESENT_NOTAVAIL       0x40  /*!< \brief The address is not available */
#define  CMBS_ADDR_PRESENT_MASK           0xE0  /*!< address presentation mask */

#define  CMBS_ADDR_PRESENT_CNIP_USER      0x01  /*!< CNIP to be sent with User Provided */

/* Ringing type */
#define  CMBS_ADDR_PRESENT_RINGTYPE_1     0x04
#define  CMBS_ADDR_PRESENT_RINGTYPE_2     0x08
#define  CMBS_ADDR_PRESENT_RINGTYPE_3     0x10
#define  CMBS_ADDR_PRESENT_RINGTYPE_4     0x0C
#define  CMBS_ADDR_PRESENT_RINGTYPE_5     0x14
#define  CMBS_ADDR_PRESENT_RINGTYPE_6     0x18
#define  CMBS_ADDR_PRESENT_RINGTYPE_7     0x1C
#define  CMBS_ADDR_PRESENT_RINGTYPE_MASK  0x1C
#define  CMBS_ADDR_PRESENT_CALLTYPE_INT   0x02

/* Used Alphabet and Screening in CNIP */
#define  CMBS_ADDR_SCREENING_UTF8_USER    0x04
#define  CMBS_ADDR_SCREENING_UTF8_NWK     0x07

#define CMBS_CALLED_NUM_MAX_LEN    24
#define CMBS_CALLER_NUM_MAX_LEN    24
/*! \brief IE address structure for CMBS_IE_CALLERPARTY and CMBS_IE_CALLEDPARTY */
typedef  struct
{
    u8          u8_AddressProperties;            /*!< Bits 7-4 proptype, bits 3-0 propplan */
    u8          u8_AddressPresentation;          /*!< Bits 7-5 presentation, bits 4-2 for ring type, bits 1-0 reserved */
    u8          u8_AddressLen;                   /*!< Address length */
    u8 *pu8_Address;                     /*!< Address byte array */
} ST_IE_CALLERPARTY, *PST_IE_CALLERPARTY,
ST_IE_CALLEDPARTY, *PST_IE_CALLEDPARTY;

#define CMBS_CALLER_NAME_MAX_LEN    31
/*! \brief IE structure for CMBS_IE_CALLERNAME */
typedef  struct
{
    u8          u8_DataLen;          /*!< Length of Caller Name */
    u8 *pu8_Name;            /*!< Caller Name */
} ST_IE_CALLERNAME, *PST_IE_CALLERNAME;

/*! \brief IE structure for CMBS_IE_CALLEDNAME */
typedef  struct
{
#define CMBS_CALLED_NAME_MAX_LEN    20

    u8          u8_AlphabetAndScreening; /*!<  Bits 2 1 = Screening, Bits 5 4 3 = Used Alphabet */
    u8          u8_DataLen;              /*!< Length of Called Party Name */
    u8 *pu8_Name;                /*!< Called Party Name */
    u8          u8_DataLenFirst;         /*!< Length of Called PartyFirst Name */
    u8 *pu8_FirstName;           /*!< Called Party First Name */
} ST_IE_CALLEDNAME, *PST_IE_CALLEDNAME;

/*! \brief IE structure for CMBS_IE_TIMEOFDAY */
typedef struct
{
    u32         u32_Timestamp;       /*!< Timestamp in msec since 01.01.1970 */
} ST_IE_TIMEOFDAY, *PST_IE_TIMEOFDAY;


#if !defined ( CMBS_API_TARGET )
#define  CMBS_HS_SUBSCRIBED_MAX_NUM  10
//#define  CMBS_CRFP_SUBSCRIBED_MAX_NUM  28    // 4 Repeaer + 6 HS per repeater - only for ENC_WRS_XTR_IOP
#define  CMBS_CRFP_SUBSCRIBED_MAX_NUM  16    // 4 Repeaer + 3 HS per repeater (4 + 4*3)
#define  CMBS_U16_SUBSCRIBED_HS_MASK 0x03FF
#else
#if (NBS==1)
#define  CMBS_HS_SUBSCRIBED_MAX_NUM  10
#define  CMBS_U16_SUBSCRIBED_HS_MASK 0x03FF
#else
    #if (VDSW3_FEATURES & VDSW3_FXS_CATIQ)
        #define  CMBS_HS_SUBSCRIBED_MAX_NUM  7		// Normal case, inc. FXS
        #define  CMBS_U16_SUBSCRIBED_HS_MASK 0x007F
    #else
        #define  CMBS_HS_SUBSCRIBED_MAX_NUM  5		// Normal case, inc. FXS
        #define  CMBS_U16_SUBSCRIBED_HS_MASK 0x001F
    #endif
#endif
#endif

#define  CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS_LENGTH 48
#define  CMBS_PARAM_RFPI_LENGTH           5
#define  CMBS_PARAM_RVBG_LENGTH           1
#define  CMBS_PARAM_RVREF_LENGTH          1
#define  CMBS_PARAM_RXTUN_LENGTH          1
#define  CMBS_PARAM_PIN_CODE_LENGTH       4
#define  CMBS_PARAM_TEST_MODE_LENGTH      1
#define  CMBS_PARAM_COUNTRY_LENGTH        0  ///\todo check this length
#define  CMBS_PARAM_SIGNALTONE_LENGTH     0  ///\todo check this length
#define  CMBS_PARAM_AUTO_REGISTER_LENGTH  0  ///\todo check this length
#define  CMBS_PARAM_NTP_LENGTH            0  ///\todo check this length
#define  CMBS_PARAM_ECO_MODE_LENGTH       1
#define  CMBS_PARAM_GFSK_LENGTH           10
#define  CMBS_PARAM_RESET_ALL_LENGTH			1
#ifdef NBS
# define  CMBS_PARAM_SUBS_DATA_LENGTH      50*10 //50 bytes per HS, 10 HS in case of NBS
# define  CMBS_PARAM_SUBS_DATA_EX_LENGTH   17*10 //17 bytes per HS, 10 HS in case of NBS
#else
# define  CMBS_PARAM_SUBS_DATA_LENGTH      50*5  //50 bytes per HS, 5 HS in normal case
# define  CMBS_PARAM_SUBS_DATA_EX_LENGTH      17*5  //17 bytes per HS, 5 HS in normal case
#endif
// #define  CMBS_PARAM_REPEATER_SUBS_DATA_LENGTH  50*28  //50 bytes per Repeater and handset over repeater, 28 in maximum case
#define  CMBS_PARAM_REPEATER_SUBS_ADDR_LENGTH  4


#define  CMBS_PARAM_AUXBGPROG_LENGTH		2
#define  CMBS_PARAM_AUXBGPROG_DIRECT_LENGTH	1
#define  CMBS_PARAM_ADC_MEASUREMENT_LENGTH	4
#define  CMBS_PARAM_PMU_MEASUREMENT_LENGTH	4
#define  CMBS_PARAM_RSSI_VALUE_LENGTH		4
#define  CMBS_PARAM_DECT_TYPE_LENGTH			1
#define  CMBS_PARAM_MAX_NUM_ACT_CALLS_PT_LENGTH 1
#define  CMBS_PARAM_FP_CUSTOM_FEATURES_LENGTH   1
#define  CMBS_PARAM_ANT_SWITCH_MASK_LENGTH		1
#define  CMBS_PARAM_PORBGCFG_LENGTH				1
#define  CMBS_PARAM_BERFER_VALUE_LENGTH			6
#define  CMBS_PARAM_INBAND_COUNTRY_LENGTH		1
#define  CMBS_PARAM_HAN_DB_ADDR_LENGTH                   4
#define  CMBS_PARAM_HAN_ULE_NEXT_TPUI_LENGTH        3
#define  CMBS_PARAM_DHSG_ENABLE_LENGTH          1

#define  CMBS_PARAM_PREAM_NORM_LENGTH				1
#define  CMBS_PARAM_RF_FULL_POWER_LENGTH			1
#define  CMBS_PARAM_RF_LOW_POWER_LENGTH				1
#define  CMBS_PARAM_RF_LOWEST_POWER_LENGTH			1
#define  CMBS_PARAM_RF19APU_MLSE_LENGTH				1
#define  CMBS_PARAM_RF19APU_KCALOVR_LENGTH			1
#define  CMBS_PARAM_RF19APU_KCALOVR_LINEAR_LENGTH	1
#define  CMBS_PARAM_RF19APU_SUPPORT_FCC_LENGTH		1
#define  CMBS_PARAM_RF19APU_DEVIATION_LENGTH		1
#define  CMBS_PARAM_RF19APU_PA2_COMP_LENGTH			1
#define  CMBS_PARAM_RFIC_SELECTION_LENGTH			1
#define  CMBS_PARAM_MAX_USABLE_RSSI_LENGTH			1
#define  CMBS_PARAM_LOWER_RSSI_LIMIT_LENGTH			1
#define  CMBS_PARAM_PHS_SCAN_PARAM_LENGTH			1
#define  CMBS_PARAM_JDECT_LEVEL1_M82_LENGTH			1
#define  CMBS_PARAM_JDECT_LEVEL2_M62_LENGTH			1
#define  CMBS_PARAM_AUXBGP_DCIN_LENGTH			    1
#define  CMBS_PARAM_AUXBGP_RESISTOR_FACTOR_LENGTH	1
#define  CMBS_PARAM_DAC_VOL_LENGTH                  2
#define  CMBS_PARAM_INL_DEL_LENGTH	                2
#define  CMBS_PARAM_SYPO_GPIO_LENGTH                1 			 
#define  CMBS_PARAM_SYPO_WAIT_FOR_SYNC_LENGTH       1
#define  CMBS_PARAM_SYPO_MODE_LENGTH                1
#define  CMBS_PARAM_ENC_DISABLE_LENGTH              1
#define  CMBS_PARAM_UART_DELAY_TIMER_LENGTH			1
#define  CMBS_PARAM_MAX_TRANSFER_SIZE_LENGTH        2
#define  CMBS_PARAM_IOM_TEST_MODE_LENGTH            1
#define  CMBS_PARAM_INT_START_CALL_TO_HOST_LENGTH   1

#define  CMBS_PARAM_NEMO_MODE_LENGTH		        1
#define  CMBS_PARAM_RING_ON_OFF_LENGTH		        1
#define  CMBS_PARAM_HS_CW_DISABLED_LENGTH           2
#define  CMBS_PARAM_INL_ADD_LENGTH					2
#define  CMBS_PARAM_BBD_UPDATE_LENGTH               3
#define  CMBS_PARAM_FXS_TIMER_LENGTH                1
#define  CMBS_PARAM_CLOCK_MASTER_EDIT_LENGTH   		1
#define  CMBS_PARAM_PREP_QSPI_FOR_HW_RESET_LENGTH   1
#define  CMBS_PARAM_FXS_TONE_CONFIG_LENGTH          42		// size of ST_IE_FXS_TONE_CFG
// CMBS_PARAM_MAX_LENGTH should be ~1/2.5 of CMBS_BUF_SIZE to allow 2 CMBS messages + headers
// it should also be a multiply of 8 because of EEPROM marking & missing alignment (EEPONRAM_MIN_POSTPONED_UPDATE_BLOCK_SIZE)
#define  CMBS_PARAM_MAX_LENGTH       CMBS_BUF_SIZE * 2 / 5  /*! maximum size of parameter data */
#if (CMBS_PARAM_MAX_LENGTH%8 != 0)
#Error: CMBS_BUF_SIZE must be a multiply of 8
#endif

#define  CMBS_PARAM_MAX_LENGTH_DEFAULT       512  /*! maximum size of parameter data for default targets*/
#define  MAX_BBD_TRANSACTION_SIZE           550 // CMBS_PARAM_MAX_LENGTH
#define  CMBS_PARAM_NEMO_CONTROL_LENGTH             1
#define  CMBS_PARAM_REPEATER_TYPE_LENGTH            1



/*! \brief CMBS parameters */
typedef enum
{
    CMBS_PARAM_UNKNOWN,
    CMBS_PARAM_RFPI,                 /*!< Base identity */
    CMBS_PARAM_RVBG,                 /*!< VBG register */
    CMBS_PARAM_RVREF,                /*!< VREF register */
    CMBS_PARAM_RXTUN,                /*!< RTUN register */
    CMBS_PARAM_MASTER_PIN,           /*!< Base master PIN code */
    CMBS_PARAM_AUTH_PIN,             /*!< Authentication PIN code */
    CMBS_PARAM_COUNTRY,              /*!< Configure cordless module to specific country settings */
    CMBS_PARAM_SIGNALTONE_DEFAULT,   /*!< Define the default behavior for outgoing calls */
    CMBS_PARAM_TEST_MODE,            /*!< Test mode. 0x00: Normal operation; 0x81: TBR6; 0x82: TBR10 (see E_CMBS_TEST_MODE) */
    CMBS_PARAM_ECO_MODE,             /*!< Eco mode. See E_CMBS_ECO_MODE_TYPE values */
    CMBS_PARAM_AUTO_REGISTER,        /*!< Automatic registration */
    CMBS_PARAM_NTP,                  /*!< NTP */
    CMBS_PARAM_GFSK,                 /*!< Gaussian frequency shift keying calibration */
    CMBS_PARAM_RESET_ALL,            /*!< Reset complete parameter area (EEprom) to default settings, RFPI and tuning parameter are kept */
    CMBS_PARAM_RESERVED,             /*!< Reserved >*/
    CMBS_PARAM_SUBS_DATA,            /*!< Returns subscription data of CMBS (this data contains alll the registered HS>*/
    CMBS_PARAM_AUXBGPROG,            /*!< BG Calibrate */
    CMBS_PARAM_ADC_MEASUREMENT,      /*!< ADCMeasurement */
    CMBS_PARAM_PMU_MEASUREMENT,      /*!< PMUMeasurement */
    CMBS_PARAM_RSSI_VALUE,           /*!< RSSI value */
    CMBS_PARAM_DECT_TYPE,            /*!< DECT type (EU, Japan , US). See E_CMBS_DECT_TYPE values; */
    CMBS_PARAM_MAX_NUM_ACT_CALLS_PT, /*!< Maximum number of active calls of 1 PT.*/
    CMBS_PARAM_ANT_SWITCH_MASK,      /*!< ANTENNA select in TBR6 Test Mode. 0x0: null, 0x1: antenna 0; 0x2: antenna 1; 0x3: antenna 0 && 1 */
    CMBS_PARAM_PORBGCFG,             /*!< PORBGCFG */
    CMBS_PARAM_AUXBGPROG_DIRECT,     /*!< AUXBGPROG set in EEPROM without calibration */
    CMBS_PARAM_BERFER_VALUE,         /*!< BER-FER value */
    CMBS_PARAM_FP_CUSTOM_FEATURES,   /*!< Internal Call disable/enable  */
    CMBS_PARAM_INBAND_COUNTRY,       /*!< country selection for inband tones, e.g. Default, French, polish and swiss */
    CMBS_PARAM_HAN_DECT_SUB_DB_START, /*!< HAN DECT subscription data base start address */
    CMBS_PARAM_HAN_DECT_SUB_DB_END,  /*!< HAN DECT subscription data base end address */
    CMBS_PARAM_HAN_ULE_SUB_DB_START, /*!< HAN ULE  subscription data base start address */
    CMBS_PARAM_HAN_ULE_SUB_DB_END,   /*!< HAN ULE  subscription data base end address */
    CMBS_PARAM_HAN_FUN_SUB_DB_START, /*!< HAN FUN  subscription data base start address */
    CMBS_PARAM_HAN_FUN_SUB_DB_END,   /*!< HAN FUN  subscription data base end address */
    CMBS_PARAM_HAN_ULE_NEXT_TPUI,       /*!< HAN ULE next TPUI to be used for subscription */
    CMBS_PARAM_DHSG_ENABLE,             /*!< Enable DHSG, Initialize DHSG GPIO's */
    CMBS_PARAM_PREAM_NORM,              /*!< Enable DHSG, Initialize DHSG GPIO's */
    CMBS_PARAM_RF_FULL_POWER,           /*!< RF full power */
    CMBS_PARAM_RF_LOW_POWER,            /*!< RF Low power */
    CMBS_PARAM_RF_LOWEST_POWER,         /*!< RF Lowest power */
    CMBS_PARAM_RF19APU_MLSE,            /*!< RF19APU MLSE */
    CMBS_PARAM_RF19APU_KCALOVR,         /*!< RF19APU KCALOVR */
    CMBS_PARAM_RF19APU_KCALOVR_LINEAR,  /*!< RF19APU KCALOVR_Linear */
    CMBS_PARAM_RF19APU_SUPPORT_FCC,     /*!< RF19APU Support FCC */
    CMBS_PARAM_RF19APU_DEVIATION,       /*!< RF19APU Deviation */
    CMBS_PARAM_RF19APU_PA2_COMP,        /*!< RF19APU PA2 compatibility */
    CMBS_PARAM_RFIC_SELECTION,          /*!< RFIC Selectionr */
    CMBS_PARAM_MAX_USABLE_RSSI,         /*!< MAX usable RSSI */
    CMBS_PARAM_LOWER_RSSI_LIMIT,        /*!< Lower RSSI Limit */
    CMBS_PARAM_PHS_SCAN_PARAM,          /*!< PHS scan */
    CMBS_PARAM_JDECT_LEVEL1_M82,        /*!< L1 - minus 82 dBm RSSI threshold for Japan regulation */
    CMBS_PARAM_JDECT_LEVEL2_M62,        /*!< L2 - minus 62 dBm RSSI threshold for Japan regulation */
    CMBS_PARAM_AUXBGP_DCIN,             /*!< Auxiliary BG DCIN input (DCIN0/DCIN1...) */
    CMBS_PARAM_AUXBGP_RESISTOR_FACTOR,  /*!< Auxiliary BG Resistor Factor */
    CMBS_PARAM_DAC1_VOL,                /*!< DAC1 sidetone volume */
    CMBS_PARAM_DAC2_VOL,                /*!< DAC2 Sidetone volume */
    CMBS_PARAM_INL_DEL,                 /*!< Delete 1, several or all handsets from internal name list */
    CMBS_PARAM_SYPO_GPIO,           	/*!< Holds the number of GPIO that will be used as SYPO input */
    CMBS_PARAM_SYPO_WAIT_FOR_SYNC, 		/*!< Defines if the target should wait for synchronization before starting DECT stack */
    CMBS_PARAM_SYPO_MODE, 				/*!< Holds SYPO mode (slave/master/none) */
    CMBS_PARAM_UART_DELAY_TIMER,        /*!< UART delay (in resolution of 10msec, i.e if value = 2 => delay = 20msec) */
    CMBS_PARAM_MAX_TRANSFER_SIZE,       /*!< MAX transfer according to targets buffer size limitation */    
    CMBS_PARAM_IOM_TEST_MODE,           /*!< IOM/PCM loopback test mode. 0x1: enabled; 0x0: disabled */
    CMBS_PARAM_INT_START_CALL_TO_HOST,  /*!< Enable or Disable route internal * call to host. 0x0: disabled; 0x1: enabled(default) Used for CatIQ2.0 certificate */
    
    CMBS_PARAM_RING_ON_OFF,			    /*!< Enable/Disable ring on/off procedure */
    CMBS_PARAM_NEMO_MODE,			    /*!< Enable/Disable the no emission mode */
    CMBS_PARAM_HS_CW_DISABLED,			/*!< Enable/Disable Call Waiting per handset */
    CMBS_PARAM_INL_ADD,					/*!< Used to ADD FXS-1 & FXS-2 only (HS-1, HS-2) */  
    CMBS_PARAM_BBD_UPDATE,           	/*!< Perform BBD download from Host */  
    CMBS_PARAM_CLOCK_MASTER_EDIT,       /*!< Disable/Enable clock master editing by handset */

    CMBS_PARAM_FXS_CALLEE_REGRET_TIME,	/*!< Regret Timer configuration */
    CMBS_PARAM_FXS_FIRST_DIGIT_TIMER,	/*!< First digit timer configuration */
    CMBS_PARAM_FXS_INTER_DIGIT_TIMER,	/*!< Inter digit timer configuration */
    CMBS_PARAM_FXS_STAR_HASH_CON_TIMER,
    CMBS_PARAM_PREP_QSPI_FOR_HW_RESET,  /*!< Requests sending of a reset command to SST QSPI flash before performing a HW reset */

    CMBS_PARAM_FXS_TONE_CONFIG,
    CMBS_PARAM_ENC_DISABLE, 			/*!< Status of subscription disable (testmode for Air Sniffer), not for product */
    CMBS_PARAM_SUBS_DATA_EX,            /*!< Returns extended subscription data of CMBS (this data contains alll the registered HS>*/
    CMBS_PARAM_HAN_FUN_GROUP_LIST_START,/*!< HAN ULE  group list start address */
	CMBS_PARAM_HAN_FUN_GROUP_LIST_END,  /*!< HAN ULE  group list end address */
	CMBS_PARAM_HAN_FUN_GROUP_TABLE_START,/*!< HAN ULE  group table start address */
	CMBS_PARAM_HAN_FUN_GROUP_TABLE_END,  /*!< HAN ULE  group table end address */
	CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_START,/*!< HAN ULE  broadcast conversion table start address */
	CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_END,  /*!< HAN ULE  broadcast conversion table end address */
	CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS, /*!< ULE  multicast encryption params */

    CMBS_PARAM_NEMO_CONTROL, 			/*!< MSB enables HM00_NEMO_DUMMY_USE_RSSI_FOR_IF_CHECK, others are for extending NEMO WA bearer time */
    CMBS_PARAM_REPEATER_TYPE, 			/*!< repeater type for enabling or disabling enhanced repeater support */
    CMBS_PARAM_REPEATER_SUBS_START, 	/*!< start address of repeater subscriptiopn records, including handset records over repeater (4+4*6) */
    CMBS_PARAM_REPEATER_SUBS_END, 	    /*!< end address of repeater subscriptiopn records, including handset records over repeater (4+4*6) */
	
    CMBS_PARAM_MAX
} E_CMBS_PARAM;



/*!< \brief CMBS parameter types for system parameters */
typedef enum
{
    CMBS_PARAM_TYPE_EEPROM,          /*!< Parameter type EEPROM */
    CMBS_PARAM_TYPE_PRODUCTION,      /*!< Parameter type Production */
    CMBS_PARAM_TYPE_FXS,            /*!< Parameter type FXS */
    CMBS_PARAM_TYPE_MAX
} E_CMBS_PARAM_TYPE;

/*!< \brief CMBS parameter types for EEPROM feature CMBS_PARAM_FP_CUSTOM_FEATURES */
#define CMBS_CUSTOM_INT_DIS_MASK         0x01
#define CMBS_CUSTOM_CALL_TRANS_DIS_MASK  0x02
// #define CMBS_CUSTOM_CONF_DIS_MASK        0x04
#define CMBS_CUSTOM_CONF_RPL_ENBL_MASK   0x04
// #define CMBS_CUSTOM_AUTO_RESUME_MASK     0x08 
// #define CMBS_CUSTOM_HOLD_INT_REL_MASK    0x10
#define CMBS_CUSTOM_INTRUSION_DIS_MASK   0x20
#define CMBS_CUSTOM_HOLD_DIS_MASK        0x40
#define CMBS_CUSTOM_CATIQ_TESTER_MASK    0x80

/*!< \brief CMBS line settings type */
typedef enum
{
    CMBS_LINE_SETTINGS_TYPE_LINE_NAME,
    CMBS_LINE_SETTINGS_TYPE_HS_MASK,
    CMBS_LINE_SETTINGS_TYPE_CALL_INTRUSION,
    CMBS_LINE_SETTINGS_TYPE_MULTIPLE_CALLS,
    CMBS_LINE_SETTINGS_TYPE_MAX

} E_CMBS_LINE_SETTINGS_TYPE;

#define CMBS_HS_NAME_MAX_LENGTH     32
#define CMBS_BS_NAME_MAX_LENGTH     13

#define CMBS_FXS_NUMBER_MAX_LENGTH 4
#ifndef CMBS_MAX_FXS_EXTENSIONS
#define CMBS_MAX_FXS_EXTENSIONS	  2
#endif
#define CMBS_FIRST_FXS_EXTENSION_ID  (CMBS_HS_SUBSCRIBED_MAX_NUM + 1)

/*! \brief IE structure for CMBS_IE_PARAMETER */
typedef  struct
{
    E_CMBS_PARAM		e_Param;        /*!< Parameter identifier */
    E_CMBS_PARAM_TYPE	e_ParamType;    /*!< Parameter type EEPROM, PRODUCTION */
    u16					u16_DataLen;    /*!< Length of data */
    u8 *pu8_Data;       /*!< Parameter data */
} ST_IE_PARAMETER, *PST_IE_PARAMETER;

/*! \brief IE structure for CMBS_IE_SUBSCRIBED_HS_LIST
u16_HsCapabilities is a bit mask:
bit0 - CF_SUPPORTED,
bit1 - CATIQ_1_0_SUPPORTED,
bit2 - CATIQ_2_0_SUPPORTED,
bit3 - CATIQ_2_1_SUPPORTED,
bit4 - DTAM_SUPPORTED,
bit5 - CATIQ_3_0_SUPPORTED,
bit6 - SUPPORT_OF_ASSOCIATED_MELODY,
bit7 - ENCRYPT_DISABLED,
bit8 - ULE_SUPPORTED
*/
typedef  struct
{
    u16    u16_HsID;
    u16    u16_NameLength;
    u8     u8_HsName[CMBS_HS_NAME_MAX_LENGTH*3];
    u16    u16_HsCapabilities;  /* Bit mask */
    u8 	u8_FXS_ExNumLen;
    u8 	u8_FXS_ExNum[CMBS_FXS_NUMBER_MAX_LENGTH];  //FXS Extension number
    u8  u8_Pref_OutgoingCall_LineId;  //proprietary field for preferred outgoing call 
} ST_IE_SUBSCRIBED_HS_LIST, *PST_IE_SUBSCRIBED_HS_LIST;

/*! \brief Structure Subscription Data */
#define CMBS_IPUI_LEN 5
#define CMBS_TPUI_LEN 3
#define CMBS_DCK_LEN 16
#define CMBS_UAK_LEN 16
#define CMBS_AC_LEN 4

typedef struct
{
    u8 pu8_IPUI[CMBS_IPUI_LEN];         /* Data: IPUI */
    u8 pu8_TPUI[CMBS_TPUI_LEN];         /* Data: TPUI */
    u8 pu8_DCK[CMBS_DCK_LEN];           /* Data: DCK */
    u8 pu8_UAK[CMBS_UAK_LEN];           /* Data: UAK */
    u8 pu8_AC[CMBS_AC_LEN];             /* Data: authentication code AC */
    u8 u8_UAK_AUTH;                     /* Data: UAK authorization */
    u8 u8_SUB_STATUS;                   /* Data: status */
    u8 u8_HANDSET_NR;                   /* Data: handset number */
    u8 u8_DCK_ASSIGNED;                 /* Data: DCK assigned */
    u8 u8_CK_LEN;                       /* Data: Cipher Key Length */
    u8 u8_FEATURES;                     /* Data: Handset ULE feature support */
}
ST_CMBS_SUBS_DATA, *PST_CMBS_SUBS_DATA;

// #ifdef CATIQ_2_1
#define CMBS_EX_DEFCK_LEN       16
typedef struct
{
    u8 u8_DEFCK[CMBS_EX_DEFCK_LEN];     /* Data: Default DCK */
    u8 u8_DCKAssigned;                  /* Data: Default DCK assigned */  
}
ST_CMBS_SUBS_DATA_EX, *PST_CMBS_SUBS_DATA_EX;
// #endif // (CATIQ_2_1)

/*! \brief Line types */
/*
Line type information. An 'off hook CLIP enabled DCIBS line' line shall respect the following rules for the line type information value:
- The 'Network delay type ('NDT') should be '0'B, indicating that the line is a 'low delay' line; however the use of procedure 'Off hook CLIP enabled double call with in band signalling' lines' for lines with significant delay (NDT = 1) is still possible.
- 'Second call type' = 'SCT' = '1'B, indicating that second calls are handled (mostly) with in band signalling.
EXAMPLE 1: An off hook CLIP enabled PSTN line uses line type information '10'B ('Off hook CLIP enabled DCIBS' line with low delay).                                               
EXAMPLE 2: AVoIP line mimicking the PSTN line behaviour including off hook CLIP would use line type information '11'B ('Off hook CLIP enabled DCIBS' line with significant delay).
*/
typedef enum
{
    CMBS_LINE_TYPE_PSTN_DOUBLE_CALL,
    CMBS_LINE_TYPE_PSTN_PARALLEL_CALL,
    CMBS_LINE_TYPE_VOIP_DOUBLE_CALL,
    CMBS_LINE_TYPE_VOIP_PARALLEL_CALL,
    CMBS_LINE_TYPE_MAX
} E_CMBS_LINE_TYPE;

/*! \brief IE structure for CMBS_IE_LINE_SETTINGS_LIST */
typedef  struct
{
    u8                  u8_Line_Id;
    u8                  u8_Call_Intrusion;
    u8                  u8_Multiple_Calls;
    u16                 u16_Attached_HS;
    E_CMBS_LINE_TYPE    e_LineType;
} ST_IE_LINE_SETTINGS_LIST, *PST_IE_LINE_SETTINGS_LIST;

/*! \brief IE structure for CMBS_IE_CALLTRANSFERREQ */
typedef  struct
{
    u32          u32_CallInstanceFrom;          /*!< Call Instance to transfer from*/
    u32          u32_CallInstanceTo;          /*!< Call Instance to transfer to */
    u8         u8_TermInstance;            /*!< Terminal requesting transfer */
} ST_IE_CALLTRANSFERREQ, *PST_IE_CALLTRANSFERREQ;

/*!< \brief CMBS parameter area types for system parameters */
typedef enum
{
    CMBS_PARAM_AREA_TYPE_EEPROM,     /*!< Parameter area is EEPROM */
    CMBS_PARAM_AREA_TYPE_RAM,        /*!< Parameter area is RAM */
    CMBS_PARAM_AREA_TYPE_MAX
} E_CMBS_PARAM_AREA_TYPE;

#define CMBS_PARAM_AREA_MAX_SIZE  CMBS_PARAM_MAX_LENGTH

/*! \brief IE structure for CMBS_IE_PARAMETER_AREA */
typedef  struct
{
    E_CMBS_PARAM_AREA_TYPE
       e_AreaType;             /*!< Parameter area EEPROM, RAM */
    u32      u32_Offset;             /*!< Offset in memory area */
    u16      u16_DataLen;            /*!< Length of data */
    u8 *pu8_Data;               /*!< Parameter data */
} ST_IE_PARAMETER_AREA, *PST_IE_PARAMETER_AREA;

/*! \brief Handset types */
typedef enum
{
    CMBS_HS_TYPE_GAP,                /*!< GAP handset */
    CMBS_HS_TYPE_CATIQ_1,            /*!< CATiq 1.0 compliant handset */
    CMBS_HS_TYPE_CATIQ_2,            /*!< CATiq 2.0 compliant handset */
    CMBS_HS_TYPE_DSPG,               /*!< DSPG handset */
    CMBS_HS_TYPE_HAN,                /*!< HAN Device */
    CMBS_HS_TYPE_MAX,
} E_CMBS_HS_TYPE;

#define  CMBS_IPEI_SIZE 5           /*!< \brief Length of IPEI */
#define  CMBS_EMC_SIZE 2            /* suppose to be same size of HL00_EMC_SIZE */

/* Handset Registration state */
typedef enum
{
    CMBS_HS_REG_STATE_UNREGISTERED  = 0,
    CMBS_HS_REG_STATE_REGISTERED    = 1,
    CMBS_HS_REG_STATE_BLACKLISTED   = 2,
    CMBS_HS_REG_STATE_SUBSCRIBING   = 3,

} E_CMBS_HS_REGISTER_STATE;
    
/* Handset Registration enable type for CMBS_IE_REG_ENABLE_TYPE */
typedef enum
{
  // must be aligned with E_HL01_HS_REGISTER_ENABLE in bhl01dat.h
    CMBS_HS_REG_ENABLE_ALL = 1,
    CMBS_HS_REG_ENABLE_HANDSET,
    CMBS_HS_REG_ENABLE_ULE,
    CMBS_HS_REG_ENABLE_ALL_RESUB  
            
} E_CMBS_HS_REGISTER_ENABLE;

#define CMBS_DONT_RESTART_TIMEOUT 0xFF   // use for timeout in OPEN_REGISTRATION to indicate no restart
    
/*! \brief IE structure for CMBS_IE_HANDSETINFO */
typedef  struct
{
    u8				u8_Hs;                  /*!< Handset number */
    u8				u8_State;               /*!< State: 0 = unregistered, 1 = registered, 2 = blacklisted, 3 = subscribing */
    E_CMBS_HS_TYPE	e_Type;                 /*!< Handset type */
    u32				u32_ModelNr;            /*!< Model number assigned by product owner or manufacturer */
    u8				u8_IPEI[CMBS_IPEI_SIZE]; /*!< Handset identity */
    u8				u8_EMC[CMBS_EMC_SIZE];  /* !< Handset EMC */
} ST_IE_HANDSETINFO, *PST_IE_HANDSETINFO;

/*! \brief FW modules */
typedef enum
{
    CMBS_MODULE_CMBS,                /*!< Cordless Module Base */
    CMBS_MODULE_DECT,                /*!< DECT Module */
    CMBS_MODULE_DSP,                 /*!< DSP library */
    CMBS_MODULE_EEPROM,              /*!< EEPROM driver */
    CMBS_MODULE_USB,                 /*!< USB stack */
    CMBS_MODULE_BUILD,               /*!< Build version of target */
    CMBS_MODULE_BOOTER,              /*!< Booter version, not official only in D&A build */
    CMBS_MODULE_MAX
} E_CMBS_MODULE;

/*! \brief IE structure for CMBS_IE_FW_VERSION */
typedef  struct
{
    E_CMBS_MODULE
       e_SwModule;             /*!< Software module identifier */
    u16      u16_FwVersion;          /*!< Version in BCD coded format */
} ST_IE_FW_VERSION, *PST_IE_FW_VERSION;

/*! \brief HW chips */
typedef enum
{
    CMBS_HW_CHIP_VEGAONE,
    CMBS_HW_CHIP_DCX78,
    CMBS_HW_CHIP_DCX79,
    CMBS_HW_CHIP_DCX81,
    CMBS_HW_CHIP_DVF99,
    CMBS_HW_CHIP_DHX91
} E_CMBS_HW_CHIP;

/*! \brief HW chip versions */
typedef enum
{
    CMBS_HW_CHIP_VERSION_C,
    CMBS_HW_CHIP_VERSION_D,
    CMBS_HW_CHIP_VERSION_B
} E_CMBS_HW_CHIP_VERSION;

/*! \brief HW boards */
typedef enum
{
    CMBS_HW_BOARD_MOD,
    CMBS_HW_BOARD_DEV
}   E_CMBS_HW_BOARD;

/*! \brief HW communication type */
typedef enum
{
    CMBS_HW_COM_TYPE_UART,
    CMBS_HW_COM_TYPE_USB,
    CMBS_HW_COM_TYPE_SPI0,
    CMBS_HW_COM_TYPE_SPI3,
}   E_CMBS_HW_COM_TYPE;

/*! \brief IE structure for CMBS_IE_HW_VERSION */
typedef  struct
{
    E_CMBS_HW_CHIP          u8_HwChip;          /*!< HW chip */
    E_CMBS_HW_CHIP_VERSION  u8_HwChipVersion;   /*!< HW chip version */
    E_CMBS_HW_BOARD         u8_HwBoard;         /*!< HW board type */
    E_CMBS_HW_COM_TYPE      u8_HwComType;       /*!< HW communication */
} ST_IE_HW_VERSION, *PST_IE_HW_VERSION;

/*! \brief IE structure for CMBS_IE_EEPROM_VERSION */
typedef struct
{
    u32 u32_EEPROM_Version;
}ST_IE_EEPROM_VERSION, *PST_IE_EEPROM_VERSION;


/*! \brief IE structure for CMBS_IE_SYS_LOG */
#define CMBS_SYS_LOG_MAX_SIZE  128
typedef  struct
{
    u8       u8_DataLen;             /*!< Length of data */
    u8       u8_Data[CMBS_SYS_LOG_MAX_SIZE];           /*!< Data Array */
} ST_IE_SYS_LOG, *PST_IE_SYS_LOG;

/*! \brief IE structure for CMBS_IE_USER_DEFINED */
typedef  struct
{
    u16      u16_Opcode;             /*!< Subtype for following data */
    u8       u8_DataLen;             /*!< Length of data */
    u8       u8_Data[128];           /*!< Data Array */
} ST_IE_USER_DEFINED, *PST_IE_USER_DEFINED;

/*! \brief Module states */
typedef enum
{
    CMBS_MODULE_STATUS_UP,           /*!< CMBS is running with stack, etc. */
    CMBS_MODULE_STATUS_DOWN,         /*!< CMBS is not running */
    CMBS_MODULE_STATUS_REMOVED       /*!< CMBS is removed from host */
} E_CMBS_MODULE_STATUS;

/*! \brief Hook states */
typedef enum
{
	CMBS_STATUS_ONHOOK,			 /*!< Fxs is onhook */
	CMBS_STATUS_OFFHOOK		 /*!< Fxs is offhook */
} E_CMBS_HOOK_STATUS;

typedef  struct
{
    E_CMBS_HOOK_STATUS	e_FxsHookStatus;        
} ST_IE_FXS_HOOK_STATUS, *PST_IE_FXS_HOOK_STATUS;

typedef enum
{
   CMBS_FXS_TONE_DSC	,
   CMBS_FXS_TONE_DIAL,
   CMBS_FXS_TONE_BUSY,
   CMBS_FXS_TONE_RINGBACK,
   CMBS_FXS_TONE_HOWLER,
   CMBS_FXS_TONE_MSG_WAIT,				// Msg Waiting tone Composed (whole) tone
   CMBS_FXS_TONE_REORDER,			
   CMBS_FXS_TONE_MSG_WAIT_SIMPLE0,		// Msg Waiting tone first part: beeps before dial tone
   CMBS_FXS_TONE_MAX,
} E_CMBS_FXS_TONE;

typedef enum
{
   FXS_TONE_NEW_CONFIGURATION,
   FXS_TONE_EXIST_CONFIGURATION,
} E_CMBS_FXS_TONE_TYPE;


/*! \brief Call Direction */
typedef enum
{
	CMBS_CALL_DIRECTION_INCOMING,		 /*!< Incoming Call */
	CMBS_CALL_DIRECTION_OUTGOING		 /*!< Outgoing Call */
} E_CMBS_CALL_DIRECTION;

typedef  struct
{
    E_CMBS_CALL_DIRECTION	e_CallDirection;        
} ST_IE_CALL_DIRECTION, *PST_IE_CALL_DIRECTION;

/*! \brief IE structure for CMBS_IE_STATUS */
typedef  struct
{
    E_CMBS_MODULE_STATUS	e_ModuleStatus;         /*!< Module status e.g. up, down, etc. */
} ST_IE_SYS_STATUS, *PST_IE_SYS_STATUS;

/*! \brief Response values */
typedef  enum
{
    CMBS_RESPONSE_OK,                /*!< OK */
    CMBS_RESPONSE_ERROR,             /*!< Error */
    CMBS_RESPONSE_FIFO_FULL,
    CMBS_RESPONSE_NO_LINK_AVL,
    CMBS_RESPONSE_MAX
} E_CMBS_RESPONSE;

/*! \brief IE structure for CMBS_IE_RESPONSE */
typedef  struct
{
    E_CMBS_RESPONSE
       e_Response;             /*!< Response: OK or Error */
} ST_IE_RESPONSE, *PST_IE_RESPONSE;

/*! \brief Registration Response values */
typedef  enum
{
    CMBS_REG_CLOSE_TIMEOUT,             /*!< Registration closed due to timeout */
    CMBS_REG_CLOSE_HS_REGISTERED,       /*!< Registration closed because HS was successfully registered */

    CMBS_REG_CLOSE_MAX
} E_CMBS_REG_CLOSE_REASON;

/*! \brief IE structure for CMBS_IE_REG_CLOSE_REASON */
typedef  struct
{
    E_CMBS_REG_CLOSE_REASON    e_Reg_Close_Reason;
} ST_IE_REG_CLOSE_REASON, *PST_IE_REG_CLOSE_REASON;

typedef enum
{
    CMBS_DC_RESPONSE_OK,                /*!< OK */
    CMBS_DC_RESPONSE_ERROR,           /*!< Error */
    CMBS_DC_RESPONSE_MAX
}E_CMBS_DC_REJECT_REASON;

typedef struct
{
    E_CMBS_DC_REJECT_REASON e_DC_Reject_Reason;
}ST_IE_DC_REJECT_REASON, *PST_IE_DC_REJECT_REASON;
/*!< \brief CMBS slot types for ATE test */
typedef enum
{
    CMBS_ATE_SLOT_TYPE,
    CMBS_ATE_SLOT_TYPE_DOUBLE,
    CMBS_ATE_SLOT_TYPE_LONG,
    CMBS_ATE_SLOT_TYPE_MAX

} E_CMBS_ATE_SLOT_TYPE;


/*!< \brief CMBS ATE test type */
typedef enum
{
    CMBS_ATE_TYPE_TX,
    CMBS_ATE_TYPE_CONTINUOUS_TX,
    CMBS_ATE_TYPE_RX,
    CMBS_ATE_TYPE_CONTINUOUS_RX,
    CMBS_ATE_TYPE_MAX

} E_CMBS_ATE_TYPE;


/*! \brief IE structure for CMBS_IE_ATE_SETTINGS */
typedef  struct
{
    E_CMBS_ATE_SLOT_TYPE
       e_ATESlotType;
    E_CMBS_ATE_TYPE
       e_ATEType;
    u8   u8_Instance;           /* 0..9 */
    u8   u8_Slot;               /* 0..11 */
    u8   u8_Carrier;            /* 0..9 for ETSI DECT, 0..94 for WDCT, 23:27 for US DECT */
    u8   u8_Ant;                /* 0,1 */
    u8   u8_Pattern;            /* 0..4: [0,0x22,0xF0,0xDD,Fig31] */
    u8   u8_NormalPreamble;     /* 1 - yes, 0 - No */
    u8   u8_PowerLevel;         /* 0,1 or 2 */
    u8   u8_Gpio;
    u8	 u8_BERFEREnabled;
    u8   u8_BERFERFrameCount;
    u8   u8_DeviceIdentity;
} ST_IE_ATE_SETTINGS, *PST_IE_ATE_SETTINGS;

typedef enum
{
    CMBS_GPIO_BANK_A,
    CMBS_GPIO_BANK_B,
    CMBS_GPIO_BANK_C,
    CMBS_GPIO_BANK_D,
    CMBS_GPIO_BANK_E,

    CMBS_GPIO_BANK_MAX
}E_CMBS_GPIO_BANK;

typedef struct
{
    E_CMBS_GPIO_BANK	e_GPIOBank;
    u32					u32_GPIO;
}ST_IE_GPIO_ID, *PST_IE_GPIO_ID;

typedef struct
{
    u8		u8_INTPolarity; // Polarity: active_low=0 / active_high=1
    u8		u8_INTType;     // Mode: Level=0 / edge=1
}ST_IE_INT_CONFIGURATION, *PST_IE_INT_CONFIGURATION;

/*****************************************************************************
 * CAT-iq 2.0 features
 *****************************************************************************/

typedef enum
{
    CMBS_TIME      = 0x1,            /*!< Contains only time information */
    CMBS_DATE      = 0x2,            /*!< Contains only date information */
    CMBS_DATE_TIME = 0x3             /*!< Contains both date and time information */
} E_CMBS_DATETIME_CODING;


typedef enum
{
    CMBS_CURRENT_TIME                = 0x00,  /*!< The current time/date */
    CMBS_TIME_DURATION               = 0x01,  /*!< Time duration (in Years, Months, Days and/or Hours, Minutes, Seconds, Time Zone = 0 */
    CMBS_REQUEST_CURR_TIME			 = 0x04,  /*!< Request to send current time/date */
    CMBS_NO_VAILD_TIME_AVAIL		 = 0x05,  /*!< No valid time/date available */
    CMBS_DATE_TIME_MMS_DELIVER       = 0x20,  /*!< The time/date at which to start forwarding/delivering the MMS message */
    CMBS_DATE_TIME_MMS_DATA_CREATED  = 0x21,  /*!< The time/date the MMS user data was created */
    CMBS_DATE_TIME_MMS_DATA_MODIFIED = 0x22,  /*!< The time/date the MMS user data was last modified */
    CMBS_DATE_TIME_RECEIVED_MCE      = 0x23,  /*!< The time/date the message was received by the MCE */
    CMBS_DATE_TIME_END_ENTITY        = 0x24,  /*!< The time/date the message was delivered/accessed by the End Entity */
    CMBS_DATE_TIME_IDENTIFIER        = 0x28   /*!< The time/date stamp for use as an identifier */
} E_CMBS_DATETIME_INTERPRETATION;

/*! \brief structure Time-Date update */
typedef struct
{
    E_CMBS_DATETIME_CODING
       e_Coding;
    E_CMBS_DATETIME_INTERPRETATION
       e_Interpretation;
    u8       u8_Year;
    u8       u8_Month;
    u8       u8_Day;
    u8       u8_Hours;
    u8       u8_Mins;
    u8       u8_Secs;
    u8       u8_Zone;                /*!< The Time Zone indicates the difference,
                                          expressed in quarters of an hour, between the local time and GMT.
                                          The first bit represents the algebraic sign: 0=positive, 1=negative */
} ST_IE_DATETIME, *PST_IE_DATETIME,
ST_DATE_TIME,   *PST_DATE_TIME;


typedef enum
{
    CMBS_DATA_CHANNEL_NULL,
    CMBS_DATA_CHANNEL_IWU,           /*!< Channel type: IWU Info Channel using SMS */
    CMBS_DATA_CHANNEL_WB,            /*!< Channel type: Wide Band Mixed slot */
    CMBS_DATA_CHANNEL_MAX
} E_CMBS_DATA_CHANNEL_TYPE;


typedef enum
{
    CMBS_DATA_SERVICE_TRANSPARENT,   /*!<  */
    CMBS_DATA_SERVICE_IP_TANSPARENT, /*!<  */
    CMBS_DATA_SERVICE_MAX
} E_CMBS_DATA_SERVICE_TYPE;


/*! \brief Data session type structure */
typedef struct
{
    E_CMBS_DATA_CHANNEL_TYPE
       e_ChannelType;          /*!< IWU Info Channel currently supported */
    E_CMBS_DATA_SERVICE_TYPE
       e_ServiceType;          /*!< Transparent currently supported */
    /* E_CMBS_DATA_BAND_WIDTH
                e_BandWidth             Bandwidth / Slot type */
} ST_IE_DATA_SESSION_TYPE, *PST_IE_DATA_SESSION_TYPE,
ST_DATA_SESSION_TYPE, *PST_DATA_SESSION_TYPE;


/*! \brief IE structure for CMBS_IE_DATA */
typedef  struct
{
    u16         u16_DataLen;         /*!< Length of data */
    u8 *pu8_Data;            /*!< Data pointer */
} ST_IE_DATA, *PST_IE_DATA;


typedef enum
{
    CMBS_CALL_STATE_TYPE_UNUSED,
    CMBS_CALL_STATE_TYPE_INTERNAL,
    CMBS_CALL_STATE_TYPE_EXT_INCOMING,
    CMBS_CALL_STATE_TYPE_EXT_OUTGOING,
    CMBS_CALL_STATE_TYPE_TRANSFER,
    CMBS_CALL_STATE_TYPE_CONFERENCE,
    CMBS_CALL_STATE_TYPE_SERVICE,
    CMBS_CALL_STATE_TYPE_HS_LOCATOR
} E_CMBS_CALL_STATE_TYPE;
typedef enum
{
    CMBS_CALL_STATE_STATUS_IDLE,
    CMBS_CALL_STATE_STATUS_CALL_SETUP,
    CMBS_CALL_STATE_STATUS_CALL_SETUP_ACK,
    CMBS_CALL_STATE_STATUS_CALL_PROCEEDING,
    CMBS_CALL_STATE_STATUS_CALL_ALERTING,
    CMBS_CALL_STATE_STATUS_CALL_CONNECTED,
    CMBS_CALL_STATE_STATUS_CALL_DISCONNECTING,
    CMBS_CALL_STATE_STATUS_CALL_HOLD,
    CMBS_CALL_STATE_STATUS_CALL_UNDER_TRANSFER,
    CMBS_CALL_STATE_STATUS_CONF_CONNECTED,
    CMBS_CALL_STATE_STATUS_CALL_INTERCEPTED,
    CMBS_CALL_STATE_STATUS_CALL_WAITING,
    CMBS_CALL_STATE_STATUS_CALL_REINJECTED,
    CMBS_CALL_STATE_STATUS_IDLE_PENDING,
    CMBS_CALL_STATE_STATUS_CONF_SECONDARY
} E_CMBS_CALL_STATE_STATUS;


#define  CMBS_MAX_DEVICE_IDS 10

typedef struct
{
    u8                        u8_NumDevices;
    u16                      u16_DeviceIds[CMBS_MAX_DEVICE_IDS];
    E_CMBS_CALL_STATE_STATUS  e_CallStatus;
} ST_IE_DEVICE_LIST_STATUS, *PST_IE_DEVICE_LIST_STATUS;

typedef struct
{
    u8                        u8_ActCallID;
    E_CMBS_CALL_STATE_TYPE    e_CallType;
    E_CMBS_CALL_STATE_STATUS  e_CallStatus;
    u16                       u16_HandsetsMask;    /* bit mask of handsets involved to call */
    u8                        u8_LinesMask;        /* bit mask of lines involved to call */
    ST_IE_DEVICE_LIST_STATUS st_DevListStatus;
} ST_IE_CALL_STATE, *PST_IE_CALL_STATE;

typedef struct
{
    u16 						u16_DeviceID;
    E_CMBS_CALL_STATE_STATUS 	e_CallStatus;
} ST_IE_UPDATED_DEVICE_STATE, *PST_IE_UPDATED_DEVICE_STATE;

typedef enum
{
    CMBS_HS_RANGE_STATUS_OUT_OF_RANGE,
    CMBS_HS_RANGE_STATUS_IN_RANGE
} E_CMBS_HS_RANGE_STATUS;


typedef enum
{
    CMBS_LINE_DIAG_OK_STATUS_DOWN,			/*!< OK status 'down' */
    CMBS_LINE_DIAG_OK_STATUS_REGISTER,		/*!< OK status 'registering'*/
    CMBS_LINE_DIAG_OK_STATUS_INITIALIZE,	/*!< OK status 'initializing' */
    CMBS_LINE_DIAG_OK_STATUS_UP,			/*!< OK status 'up' */
	
    CMBS_LINE_DIAG_OK_STATUS_MAX	
} E_CMBS_LINE_DIAG_OK_STATUS;

#define CMBS_LINE_RELATED_CHANGE         1
#define CMBS_NON_LINE_RELATED_CHANGE     2

typedef enum
{
    CMBS_LINE_DIAG_LINE_IDLE,				/*!< Line is idle (no active call) */
    CMBS_LINE_DIAG_LINE_IN_USE,				/*!< Line is in use */
    CMBS_LINE_DIAG_LINE_SYSTEM_BUSY,		/*!< Line or system is busy */
    	
    CMBS_LINE_DIAG_LINE_MAX	
} E_CMBS_LINE_DIAG_STATUS; 


typedef enum
{
    CMBS_LINE_DIAG_ERROR_UNKNOWN,			/*!< unknown error */
    CMBS_LINE_DIAG_NETWORK_ERROR,			/*!< Network error */
    CMBS_LINE_DIAG_LOCAL_ERROR,				/*!< Local error */
    	
    CMBS_LINE_DIAG_ERROR_MAX	
} E_CMBS_LINE_DIAG_ERROR_TYPE; 

typedef enum
{
    CMBS_LINE_DIAG_LOCAL_ERROR_UNKNOWN,		/*!< unknown local error */
    CMBS_LINE_DIAG_LOCAL_LAN_ERROR,			/*!< LAN error */
    CMBS_LINE_DIAG_LOCAL_LAN_2_ERROR,		/*!< LAN level 2 (e.g Ethernet) */
    CMBS_LINE_DIAG_LOCAL_LAN_3_ERROR,		/*!< LAN level 3 (e.g. IP error) */
    CMBS_LINE_DIAG_LOCAL_CABLE_ERROR,		/*!< Cable error */
    CMBS_LINE_DIAG_LOCAL_MODEM_HGW_ERROR,	/*!< Modem or Home Gateway error */
    	
    CMBS_LINE_DIAG_LOCAL_ERROR_MAX	
} E_CMBS_LINE_DIAG_LOCAL_ERROR_NUMBER; 

typedef enum
{
    CMBS_LINE_DIAG_NETWORK_ERROR_UNKNOWN,		/*!< unknown Network error */
    CMBS_LINE_DIAG_NETWORK_WAN_ERROR,			/*!< WAN error */
    	
	CMBS_LINE_DIAG_NETWORK_ERROR_MAX
} E_CMBS_LINE_DIAG_NETWORK_ERROR_NUMBER; 

typedef enum
{
    CMBS_CALL_STATE_FILTER_SEND_ALL,  
    CMBS_CALL_STATE_FILTER_SEND_INT_CALLS_ONLY,  
    CMBS_CALL_STATE_FILTER_SEND_EXT_CALLS_ONLY,  
    CMBS_CALL_STATE_FILTER_SEND_NONE 
} E_CMBS_CALL_STATE_FILTER_TYPE;

/*! \brief List identifiers */
typedef enum
{
    CMBS_LA_LIST_SUPPORTED_LISTS,
    CMBS_LA_LIST_MISSED_CALLS,
    CMBS_LA_LIST_OUTGOING_CALLS,
    CMBS_LA_LIST_INCOMING_ACC,
    CMBS_LA_LIST_ALL_CALLS,
    CMBS_LA_LIST_CONTACT,
    CMBS_LA_LIST_INTERNAL_NAMES,
    CMBS_LA_LIST_DECT_SETTINGS,
    CMBS_LA_LIST_LINE_SETTINGS,
    CMBS_LA_LIST_ALL_INCOMING_CALLS,
    CMBS_LA_LIST_LINE_AND_DIAGNOSTIC_STATUS,   
  	CMBS_LA_LIST_SMS_SETTINGS,
  	CMBS_LA_LIST_INCOMING_SMS,
  	CMBS_LA_LIST_SENT_SMS,
  	CMBS_LA_LIST_OUTGOING_SMS,
  	CMBS_LA_LIST_DRAFT_SMS,
  	CMBS_LA_LIST_DTAM_SETTINGS,  
  	CMBS_LA_LIST_DTAM_INCOMING_MESSAGES,
    CMBS_LA_LIST_DTAM_WELCOME_MESSAGE,        
    CMBS_LA_LIST_MAX
} E_CMBS_LA_LIST_ID;

#define CMBS_SMS_NEW_ARRIVED 		1
#define CMBS_SMS_OTHER_LIST_CHANGE  2

#define CMBS_FLD_UNDEFINED -1
// CMBS_LA_LIST_SUPPORTED_LISTS fields
#define CMBS_SL_FLD_LIST_ID                1

// CMBS_LA_LIST_MISSED_CALLS fields
#define CMBS_MCL_FLD_NUMBER                1
#define CMBS_MCL_FLD_NAME                  2
#define CMBS_MCL_FLD_DATETIME              3
#define CMBS_MCL_FLD_NEW                   4
#define CMBS_MCL_FLD_LINE_NAME             5
#define CMBS_MCL_FLD_LINE_ID               6
#define CMBS_MCL_FLD_NR_OF_CALLS           7
#define CMBS_MCL_FLD_MAX		           7

// CMBS_LA_LIST_OUTGOING_CALLS fields
#define CMBS_OCL_FLD_NUMBER                1
#define CMBS_OCL_FLD_NAME                  2
#define CMBS_OCL_FLD_DATETIME              3
#define CMBS_OCL_FLD_LINE_NAME             4
#define CMBS_OCL_FLD_LINE_ID               5
#define CMBS_OCL_FLD_MAX	               5

// CMBS_LA_LIST_INCOMING_ACC fields
#define CMBS_IACL_FLD_NUMBER               1
#define CMBS_IACL_FLD_NAME                 2
#define CMBS_IACL_FLD_DATETIME             3
#define CMBS_IACL_FLD_LINE_NAME            4
#define CMBS_IACL_FLD_LINE_ID              5
#define CMBS_IACL_FLD_MAX				   5

// CMBS_LA_LIST_ALL_CALLS fields
#define CMBS_ACL_FLD_CALL_TYPE             1
#define CMBS_ACL_FLD_NUMBER                2
#define CMBS_ACL_FLD_NAME                  3
#define CMBS_ACL_FLD_DATETIME              4
#define CMBS_ACL_FLD_LINE_NAME             5
#define CMBS_ACL_FLD_LINE_ID               6
#define CMBS_ACL_FLD_MAX	               6

// CMBS_LA_LIST_CONTACT fields
#define CMBS_CL_FLD_NAME                   1
#define CMBS_CL_FLD_FIRST_NAME             2
#define CMBS_CL_FLD_CONTACT_NR             3
#define CMBS_CL_FLD_ASSOC_MDY              4
#define CMBS_CL_FLD_LINE_ID                5
#define CMBS_CL_FLD_MAX		               5

// CMBS_LA_LIST_INTERNAL_NAMES fields
#define CMBS_INL_FLD_NUMBER                1
#define CMBS_INL_FLD_NAME                  2
#define CMBS_INL_FLD_MAX	               2

// CMBS_LA_LIST_DECT_SETTINGS fields
#define CMBS_DSSL_FLD_PIN_CODE             1
#define CMBS_DSSL_FLD_CLOCK_MASTER         2
#define CMBS_DSSL_FLD_BASE_RESET           3
#define CMBS_DSSL_FLD_IP_TYPE              4
#define CMBS_DSSL_FLD_IP_VALUE             5
#define CMBS_DSSL_FLD_IP_SUB_NET_MASK      6
#define CMBS_DSSL_FLD_IP_GATEWAY           7
#define CMBS_DSSL_FLD_IP_DNS_SERVER        8
#define CMBS_DSSL_FLD_FIRMWARE_VERSION     9
#define CMBS_DSSL_FLD_EEPROM_VERSION      10
#define CMBS_DSSL_FLD_HARDWARE_VERSION    11
#define CMBS_DSSL_FLD_EMISSION_MODE    	  12
#define CMBS_DSSL_FLD_NEW_PIN_CODE    	  13
#define CMBS_DSSL_FLD_FP_POWER_LEVEL      14

#define CMBS_DSSL_FLD_MAX				  14

// CMBS_LA_LIST_LINE_SETTINGS fields
#define CMBS_LSL_FLD_LINE_NAME             1
#define CMBS_LSL_FLD_LINE_ID               2
#define CMBS_LSL_FLD_ATTACHED_HANDSETS     3
#define CMBS_LSL_FLD_DIALING_PREFIX        4
#define CMBS_LSL_FLD_FP_MELODY             5
#define CMBS_LSL_FLD_FP_VOLUME             6
#define CMBS_LSL_FLD_BLOCKED_NUMBER        7
#define CMBS_LSL_FLD_MULTI_CALL_MODE       8
#define CMBS_LSL_FLD_INTRUSION_CALL        9
#define CMBS_LSL_FLD_PERMANENT_CLIR       10
#define CMBS_LSL_FLD_CALL_FORWARDING      11
#define CMBS_LSL_FLD_CFNA				  12
#define CMBS_LSL_FLD_CFB				  13
#define CMBS_LSL_FLD_MAX				  13

// CMBS_LA_LIST_ALL_INC_CALLS fields
#define CMBS_AICL_FLD_NUMBER                1
#define CMBS_AICL_FLD_NAME                  2
#define CMBS_AICL_FLD_DATETIME              3
#define CMBS_AICL_FLD_NEW                   4
#define CMBS_AICL_FLD_LINE_NAME             5
#define CMBS_AICL_FLD_LINE_ID               6
#define CMBS_AICL_FLD_NR_OF_CALLS           7
#define CMBS_AICL_FLD_MAX		            7

// CMBS_LA_LIST_LINE_AND_DIAGNOSTIC_STATUS
#define CMBS_LADSL_FLD_LINE_ID              1
#define CMBS_LADSL_FLD_OK_STATUS         	2
#define CMBS_LADSL_FLD_LINE_USE_STATUS   	3
#define CMBS_LADSL_FLD_HS_USE_STATUS     	4
#define CMBS_LADSL_FLD_CALL_FORWARDING   	5
#define CMBS_LADSL_FLD_DIAGNOSTIC_ERROR     6
#define CMBS_LADSL_FLD_MAX                  6

//CMBS_LA_LIST_SMS_SETTINGS
#define CMBS_SSL_FLD_SMS_ID             	1
#define CMBS_SSL_FLD_LINE_ID              	2
#define CMBS_SSL_FLD_ENABLE_SMS            	3
#define CMBS_SSL_FLD_MAX_SMS_SIZE        	4
#define CMBS_SSL_FLD_SMSC_SEND_SERVER      	5
#define CMBS_SSL_FLD_SMSC_RECEIVE_SERVER    6
#define CMBS_SSL_FLD_SMS_DELIVERY_REPORT   	7
#define CMBS_SSL_FLD_SMS_VALIDITY_PERIOD   	8
#define CMBS_SSL_FLD_ALLOWED_ENCODING       9

// CMBS_LA_LIST_INCOMING_SMS
#define CMBS_ISMSL_FLD_NUMBER       		1
#define CMBS_ISMSL_FLD_NAME         		2
#define CMBS_ISMSL_FLD_DATETIME     		3
#define CMBS_ISMSL_FLD_READ_STATUS  		4
#define CMBS_ISMSL_FLD_SMS_ID       		5
#define CMBS_ISMSL_FLD_SMS_SIZE     		6
#define CMBS_ISMSL_FLD_SMS_CONTENT  		7

// CMBS_LA_LIST_SENT_SMS
#define CMBS_SSMSL_FLD_NUMBER   			1
#define CMBS_SSMSL_FLD_NAME     			2
#define CMBS_SSMSL_FLD_DATETIME 			3
#define CMBS_SSMSL_FLD_SMS_ID       		4
#define CMBS_SSMSL_FLD_NETWORK_ENC  		5
#define CMBS_SSMSL_FLD_SMS_SIZE 			6
#define CMBS_SSMSL_FLD_SMS_CONTENT  		7

// CMBS_LA_LIST_OUTGOING_SMS
#define CMBS_OSMSL_FLD_NUMBER   			1
#define CMBS_OSMSL_FLD_NAME    	 			2
#define CMBS_OSMSL_FLD_DATETIME 			3
#define CMBS_OSMSL_FLD_SMS_ID       		4
#define CMBS_OSMSL_FLD_NETWORK_ENC  		5
#define CMBS_OSMSL_FLD_SMS_SIZE 			6
#define CMBS_OSMSL_FLD_SMS_CONTENT  		7

// CMBS_LA_LIST_DRAFT_SMS
#define CMBS_DSMSL_FLD_NUMBER   			1
#define CMBS_DSMSL_FLD_NAME     			2
#define CMBS_DSMSL_FLD_DATETIME 			3
#define CMBS_DSMSL_FLD_SMS_ID       		4
#define CMBS_DSMSL_FLD_SEND_REQUEST    	 	5
#define CMBS_DSMSL_FLD_NETWORK_ENC  		6
#define CMBS_DSMSL_FLD_SMS_SIZE 			7
#define CMBS_DSMSL_FLD_SMS_CONTENT  		8

// CMBS_LA_LIST_DTAM_SETTINGS
#define CMBS_DSL_FLD_LINE_ID             		1
#define CMBS_DSL_FLD_DTAM_ID              		2
#define CMBS_DSL_FLD_DTAM_NUMBER            	3
#define CMBS_DSL_FLD_DTAM_ACTIVATION        	5
#define CMBS_DSL_FLD_DTAM_WEBLINK           	6
#define CMBS_DSL_FLD_DTAM_WELCOME_MSG_PARAMS    7

// CMBS_LA_LIST_DTAM_WELCOME_MESSAGE
#define CMBS_DWML_FLD_DTAM_ID              	1
#define CMBS_DWML_FLD_POSITION_IDX          2
#define CMBS_DWML_FLD_MSG_NAME              3
#define CMBS_DWML_FLD_TIME_DURATION         4

// Max number of Fields among the above lists
#define CMBS_LA_MAX_FLD_COUNT             13

// Max number of Entry Ids in Read Selected Entries
#define CMBS_LA_MAX_ENTRY_IDS_COUNT             15


/*! \brief List change notification type */
typedef enum
{
    CMBS_LIST_CHANGE_ENTRY_INSERTED,
    CMBS_LIST_CHANGE_ENTRY_DELETED,
    CMBS_LIST_CHANGE_ENTRY_UPDATED,

    CMBS_LIST_CHANGE_MAX
} E_CMBS_LIST_CHANGE_TYPE;


/*! \brief IE structure for CMBS_IE_TARGET_LIST_CHANGE_NOTIF */
typedef  struct
{
    E_CMBS_LA_LIST_ID           e_ListId;           /* List ID in which the change has been performed */
    u32                         u32_NumOfEntries;   /* Total num of entries in the list (After change) */
    u32                         u32_EntryId;        /* Entry ID of Entry inserted / deleted / updated */
    E_CMBS_LIST_CHANGE_TYPE     e_ChangeType;

} ST_IE_TARGET_LIST_CHANGE_NOTIF, *PST_IE_TARGET_LIST_CHANGE_NOTIF;

/*! \brief IE structure for
           CMBS_IE_LA_SORT_FIELDS, CMBS_IE_LA_EDIT_FIELDS and CMBS_IE_LA_CONST_FIELDS */
typedef  struct
{
    u16         u16_Length;
    u16         pu16_FieldId[CMBS_LA_MAX_FLD_COUNT];
} ST_IE_LA_FIELDS, *PST_IE_LA_FIELDS,
ST_LA_FIELDS, *PST_LA_FIELDS;


/*! \brief IE structure for
           CMBS_IE_LA_SORT_FIELDS, CMBS_IE_LA_EDIT_FIELDS and CMBS_IE_LA_CONST_FIELDS */
typedef  struct
{
    u16         u16_EntryId;
    u16         u16_FieldId;
    u16         u16_LowerBound;
    u16         u16_UpperBound;
} ST_IE_LA_ENTRY_RANGE, *PST_IE_LA_ENTRY_RANGE,
ST_LA_ENTRY_RANGE, *PST_LA_ENTRY_RANGE;


/*! \brief IE structure for
           CMBS_IE_LA_SORT_FIELDS, CMBS_IE_LA_EDIT_FIELDS and CMBS_IE_LA_CONST_FIELDS */
typedef  struct
{
    u16         u16_Length;
    u16         pu16_EntryId[CMBS_LA_MAX_ENTRY_IDS_COUNT];
} ST_IE_LA_ENTRY_IDS, *PST_IE_LA_ENTRY_IDS,
ST_LA_ENTRY_IDS, *PST_LA_ENTRY_IDS;


/*! \brief List Access Search Criteria matching types */
typedef enum
{
    CMBS_LA_SEARCH_MATCHING_NONE,
    CMBS_LA_SEARCH_MATCHING_EXACT,      /*!< Exact match */
    CMBS_LA_SEARCH_MATCHING_EXACT_NEXT, /*!< Exact match, return next if fail */
    CMBS_LA_SEARCH_MATCHING_EXACT_PREV, /*!< Exact match, return previous if fail */
} E_CMBS_LA_SEARCH_MATCHING;

#define LA_SEARCH_CRITERIA_PATTERN_MAX_SIZE		32
/*! \brief IE structure for CMBS_IE_LA_SEARCH_CRITERIA */
typedef  struct
{
    E_CMBS_LA_SEARCH_MATCHING
       e_MatchingType;     /*!< Search criteria matching type */
    u8               u8_CaseSensitive;   /*!< Flag for Case Sensitive (Yes=1 No=0) */
    u8               u8_Direction;       /*!< Direction of the read (0 - forward, 1 - backward) */
    u8               u8_MarkEntriesReq;  /*!< Flag for requesting reseting (or setting) of the "Read status" field for all read entries */
    u8               u8_PatternLength;   /*!< Search pattern length */
    u8               pu8_Pattern[LA_SEARCH_CRITERIA_PATTERN_MAX_SIZE];    /*!< Search pattern */

} ST_IE_LA_SEARCH_CRITERIA, *PST_IE_LA_SEARCH_CRITERIA;


/*! \brief Read Direction */
typedef enum
{
    CMBS_READ_DIRECTION_FORWARD,
    CMBS_READ_DIRECTION_BACKWARD

} E_CMBS_READ_DIRECTION;


/*! \brief IE structure for CMBS_IE_LA_READ_DIRECTION */
typedef struct
{
    E_CMBS_READ_DIRECTION e_ReadDirection;
} ST_IE_READ_DIRECTION, *PST_IE_READ_DIRECTION;

/*! \brief Mark Request */
typedef enum
{
    CMBS_MARK_REQUEST_LEAVE_UNCHANGED,
    CMBS_MARK_REQUEST_SET_READ_STATUS,
    CMBS_MARK_REQUEST_RESET_READ_STATUS

} E_CMBS_MARK_REQUEST;


/*! \brief IE structure for CMBS_IE_LA_MARK_REQUEST */
typedef struct
{
    E_CMBS_MARK_REQUEST e_MarkRequest;
} ST_IE_MARK_REQUEST, *PST_IE_MARK_REQUEST;




/*! \brief List Access Start Session reject reasons */
typedef enum
{
    CMBS_LA_START_SESSION_ERROR_NOT_ENOUGH_RESOURCES,    /*!< not enough resources */
    CMBS_LA_START_SESSION_ERROR_LIST_IN_USE,             /*!< list already in use by another session */
    CMBS_LA_START_SESSION_ERROR_LIST_NOT_SUPPORTED,      /*!< list not supported */
    CMBS_LA_START_SESSION_ERROR_MAX_SESSIONS_REACHED,    /*!< maximum number of sessions supported by the FP reached */
    CMBS_LA_START_SESSION_ERROR_MAX
} E_CMBS_LA_START_SESSION_ERRORS;


/*! \brief List Access Negative Acknowledgement reject reasons */
typedef enum
{
    CMBS_LA_NACK_INVALID_RANGE,           /*!< invalid range */
    CMBS_LA_NACK_ENTRY_NOT_AVAILABLE,     /*!< entry not available */
    CMBS_LA_NACK_INVALID_SESSION,         /*!< invalid session */
    CMBS_LA_NACK_TEMP_NOT_POSSIBLE,       /*!< temporary not possible */
    CMBS_LA_NACK_INCORRECT_FORMAT,        /*!< incorrect format */
    CMBS_LA_NACK_INVALID_START_INDEX,     /*!< invalid start index */
    CMBS_LA_NACK_PROC_NOT_SUPTD,          /*!< procedure not supported */
    CMBS_LA_NACK_PROC_NOT_ALLOWED,        /*!< procedure not allowed */
    CMBS_LA_NACK_CONTENT_NOT_ACCEPTED,    /*!< content not accepted */
    CMBS_LA_NACK_LIST_FULL,               /*!< list is full */
    CMBS_LA_NACK_MAX
} E_CMBS_LA_NACK_ERRORS;


#define  CMBS_LA_ATTRIB_EDITABLE 0x40  /*!< Field editable attribute */
#define  CMBS_LA_ATTRIB_NEW      0x20  /*!< 'New' field attribute */
#define  CMBS_LA_ATTRIB_INTERNAL 0x20  /*!< 'Number' field attribute for internal numbers */
#define  CMBS_LA_ATTRIB_DEFAULT  0x20  /*!< 'Contact number' field attribute */
#define  CMBS_LA_ATTRIB_MISSED   0x20  /*!< 'Call type' field attribute */
#define  CMBS_LA_ATTRIB_OWN      0x10  /*!< 'Number' and 'Contact number' field attribute */
#define  CMBS_LA_ATTRIB_ACCEPTED 0x10  /*!< 'Call type' field attribute */
#define  CMBS_LA_ATTRIB_OUTGOING 0x08  /*!< 'Call type' field attribute */
#define  CMBS_LA_ATTRIB_FIXED    0x08  /*!< 'Contact number' field attribute */
#define  CMBS_LA_ATTRIB_MOBILE   0x04  /*!< 'Contact number' field attribute */
#define  CMBS_LA_ATTRIB_WORK     0x02  /*!< 'Contact number' field attribute */


/*! SUOTA reject reason enum, taken from t_en_SUOTA_RejectReason enum */
typedef enum
{
    CMBS_SUOTA_RETRY_LATER_CON_REFUSED = 1,
    CMBS_SUOTA_RETRY_LATER_FP_RES_OVERFLOW,
    CMBS_SUOTA_FILE_DOESNOT_EXIST,
    CMBS_SUOTA_INVALID_URL1_FORMAT,
    CMBS_SUOTA_UNREACHABLE_URL1,
    CMBS_SUOTA_CMD_FORMAT_ERROR
}E_SUOTA_RejectReason;

/*! SUOTA Software Update subtype, taken from en_ge00_SU_SubType */
typedef enum
{
    SUOTA_GE00_SU_UNKNOWN       = 0x80,
    SUOTA_GE00_SU_FW_UPGRAGE    = 0x81
}
E_SUOTA_SU_SubType;

/*! Version string type */
typedef enum
{
    CMBS_SUOTA_SW_VERSION = 0,
    CMBS_SUOTA_HW_VERSION,
}
E_SUOTA_VersionType;

#define SUOTA_MAX_HEADER_SIZE	140

/*! SUOTA Upgrade package details, used in CMBS_EV_DSR_SUOTA_SEND_VERS_AVAIL message. Originally defined in t_st_SUOTA_HSVerAvailHdr */
typedef struct
{
    u16 u16_delayInMin;
    u8  u8_URLStoFollow;
    u8  u8_Spare            :4;
    u8  u8_UserInteraction  :4;
}
ST_SUOTA_UPGRADE_DETAILS;

/*! SUOTA Hs Version indication details, taken from t_st_SUOTA_HSVerIndHdr*/
typedef struct
{
    u16 u16_EMC;
    u8  u8_URLStoFollow;
    u8  u8_FileNumber;
    u8  u8_Flags;
    u8  u8_Reason;
}
ST_SUOTA_HS_VERSION_IND;

/*! maximum length of SW version. Must be equal to MAX_SW_VER_ID_SIZE 	*/
#define CMBS_MAX_VERSION_LENGTH 	20
/*! maximum length of URL string. Must be equal to MAX_URL_SIZE 		*/
#define CMBS_MAX_URL_SIZE			58


/*! Structure for carrying version buffer, taken from t_st_SUOTA_StrBuffer*/
typedef struct
{
    u8                  u8_VerLen;
    u8                  pu8_VerBuffer[CMBS_MAX_VERSION_LENGTH];
    E_SUOTA_VersionType type;
}
ST_VERSION_BUFFER;

/*! Structure for carrying URL, taken from t_st_SUOTA_UrlBuffer*/
typedef struct
{
    u8  u8_UrlLen;
    u8  pu8_UrlBuffer[CMBS_MAX_URL_SIZE];
}
ST_URL_BUFFER;


/*! maximum length of PIN code. */
#define CMBS_MAX_PIN_CODE_LENGTH			4

/*! maximum length of IP address. */
#define CMBS_MAX_IP_ADDRESS_LENGTH			16

/*! maximum length of Version. Note: use of version numbers with length > 6 characters requires EEPROM size >= 4096B */
#define CMBS_MAX_DECT_SETTINGS_VERSION_LENGTH	19

/*! Proprietary fields length */

#define CMBS_MAX_PROP_FIELD_LENGTH   22   // HS model name is currently the longest with 20 bytes + 2 bytes for headers
/*! Number of supported Proprietary fields  */
#if !defined(CMBS_MAX_NUM_PROP_FIELDS)
#define CMBS_MAX_NUM_PROP_FIELDS   3
#endif

/*! DECT Settings Fields */
typedef enum
{
    CMBS_DECT_SETTINGS_FIELD_BASE_RESET = 1,
    CMBS_DECT_SETTINGS_FIELD_CLCK_MASTER,
    CMBS_DECT_SETTINGS_FIELD_EMISSION_MODE,
    CMBS_DECT_SETTINGS_FIELD_IP_ADDR_TYPE,
    CMBS_DECT_SETTINGS_FIELD_IP_ADDR_VAL,
    CMBS_DECT_SETTINGS_FIELD_IP_ADDR_SUBNET_MASK,
    CMBS_DECT_SETTINGS_FIELD_IP_GATEWAY,
    CMBS_DECT_SETTINGS_FIELD_IP_DNS_SERVER,
    CMBS_DECT_SETTINGS_FIELD_FIRMWARE_VERSION,
    CMBS_DECT_SETTINGS_FIELD_SOFTWARE_VERSION,
    CMBS_DECT_SETTINGS_FIELD_EEPROM_VERSION,
    CMBS_DECT_SETTINGS_FIELD_PIN_CODE,
    CMBS_DECT_SETTINGS_FIELD_HARDWARE_VERSION,
    /*
    CMBS_DECT_SETTINGS_PROP_FIELD_BASE_IDENTIFICATION = 0x81,
    CMBS_DECT_SETTINGS_PROP_FIELD_CAPABILITIES,
    CMBS_DECT_SETTINGS_PROP_FIELD_FLAGS
    */
    CMBS_DECT_SETTINGS_FIELD_FP_POWER_LEVEL,
    CMBS_DECT_SETTINGS_PROP_FIELD_1 = 0x81
}
E_DECT_SETTINGS_FIELDS;


/*! \brief IE structure for CMBS_IE_DECT_SETTINGS_LIST */
typedef  struct
{
    u16  u16_ClockMaster;                                       /*!< Clock master. Defines the entity which sets date an time for the DECT system (PP or FP). */
    u16  u16_BaseReset;                                         /*!< Base reset. Sets settings back to default factory values. */
    u16  u16_EmissionMode;                                      /*!< Bitmap for activating/deactivating the 'No Emission mode', etc. */
    u8   u8_IPAddressType;                                      /*!< FP IP address type. DHCP or static. */
    u8   pu8_IPAddressValue[CMBS_MAX_IP_ADDRESS_LENGTH];        /*!< FP IP address value. Editable only for static IP address. */
    u8   pu8_IPAddressSubnetMask[CMBS_MAX_IP_ADDRESS_LENGTH];   /*!< FP IP address subnet mask. Editable only for static IP address. */
    u8   pu8_IPAddressGateway[CMBS_MAX_IP_ADDRESS_LENGTH];      /*!< FP IP address gateway. Only for static IP address. */
    u8   pu8_IPAddressDNSServer[CMBS_MAX_IP_ADDRESS_LENGTH];    /*!< FP IP address DNS Server. Only for static IP address. */

    u8   pu8_FirmwareVersion[CMBS_MAX_DECT_SETTINGS_VERSION_LENGTH];        /*!< Software version of the FP. Read only. */
    u8   u8_FirmwareVersionLength;
    u8   pu8_EepromVersion[CMBS_MAX_DECT_SETTINGS_VERSION_LENGTH];          /*!< Eeprom version of the FP. Read only. */
    u8   u8_EepromVersionLength;
    u8   pu8_HardwareVersion[CMBS_MAX_DECT_SETTINGS_VERSION_LENGTH];        /*!< Hardware version of the FP Read only. */
    u8   u8_HardwareVersionLength;

    u8   pu8_PinCode[CMBS_MAX_PIN_CODE_LENGTH];

    u16  u16_FieldsMask;                                        /* Bitmask of used fields in this structure */
#ifdef CATIQ_2_1
		 u16  u16_FPPowerLevel;										/*!< FP Power Level. '0' = Normal power level. '1' = Reduced power level */
#endif //(CATIQ_2_1)

} ST_IE_DECT_SETTINGS_LIST, *PST_IE_DECT_SETTINGS_LIST;

typedef  struct
{
    u8  u8_Len;                                       /*!< Proprietary field length */
    u8  u8_Data[CMBS_MAX_PROP_FIELD_LENGTH];          /*!< Proprietary field data */
} ST_PROP_FIELD;

/*! \brief IE structure for CMBS_IE_PROP_FIELDS_LIST */
typedef  struct
{
    ST_PROP_FIELD st_PropField[CMBS_MAX_NUM_PROP_FIELDS];	 
    u16  u16_PropFieldsMask;                                        /* Bitmask of used fields in this structure */

} ST_IE_PROP_FIELDS_LIST, *PST_IE_PROP_FIELDS_LIST;

#define CMBS_MAX_NUM_OF_CODECS      6
#define CMBS_MAX_DYNAMIC_CODEC_LEN  15
#define CMBS_RTCP_MAX_SDES          50


/*! \brief RTP Jitter Buffer Mode */
typedef enum
{
    CMBS_RTP_JB_MODE_FIXED          = 0,
    CMBS_RTP_JB_MODE_ADAPTIVE,
}
E_CMBS_RTP_JB_MODE;

/*! \brief Voice Loop Type */
typedef enum
{
    CMBS_VOICE_LOOP_NONE            = 0,
    CMBS_VOICE_LOOP_DSP_LEVEL       = 2,
    CMBS_VOICE_LOOP_RTP_LEVEL       = 3
}
E_CMBS_VOICE_LOOP;

/*! \brief Audio Mode */
typedef enum
{
    CMBS_AUDIO_MODE_ACTIVE          = 0,
    CMBS_AUDIO_MODE_REC_ONLY,
    CMBS_AUDIO_MODE_SEND_ONLY,
    CMBS_AUDIO_MODE_INACTIVE
}
E_CMBS_AUDIO_MODE;

/*! \brief Message Waiting Indication Type */
typedef enum
{
    CMBS_MWI_UNKNOWN    = 0x80,
    CMBS_MWI_VOICE      = 0x81,
    CMBS_MWI_SMS        = 0x82,
    CMBS_MWI_EMAIL      = 0x83,

    CMBS_MWI_MAX
}
E_CMBS_MWI_TYPE;

/*! \brief Fax Tone Type */
typedef enum
{
    CMBS_FAX_TONE_TYPE_CNG          = 1,
    CMBS_FAX_TONE_TYPE_CED          = 2,
    CMBS_FAX_TONE_TYPE_V21_PREAMBLE = 4,
    CMBS_FAX_TONE_TYPE_ANS_PHASEREV = 8
}
E_CMBS_FAX_TONE_TYPE;

/*! \brief CallLeg Type */
typedef enum
{
    CMBS_CALL_LEG_TYPE_PP,
    CMBS_CALL_LEG_TYPE_LINE,

    CMBS_CALL_LEG_TYPE_MAX
}
E_CMBS_CALL_LEG_TYPE;

/*! \brief EcoMode Type */
typedef enum
{
    CMBS_ECO_MODE_TYPE_NONE,                // SD02_FULL_POWER 132(0x84) DTPC Full Power
    CMBS_ECO_MODE_TYPE_LOW_POWER,           // SD02_LOW_POWER 133(0x85) DTPC Low(medium) Power
    CMBS_ECO_MODE_TYPE_LOWEST_POWER,        // SD02_LOWEST_POWER 134(0x86) DTPC Lowest Power

    CMBS_ECO_MODE_TYPE_MAX
}
E_CMBS_ECO_MODE_TYPE;

/*! \brief DectType Type */
typedef enum
{
    CMBS_DECT_TYPE_EU,                      // EU DECT
    CMBS_DECT_TYPE_US,                      // US DECT
    CMBS_DECT_TYPE_US_FCC,                  // US DECT FCC compliant
    CMBS_DECT_TYPE_JAPAN,                   // JAPAN DECT
    CMBS_DECT_TYPE_JAPAN_FCC,               // JAPAN DECT FCC compliant
    CMBS_DECT_TYPE_KOREAN,                   // KOREAN DECT     

    CMBS_DECT_TYPE_MAX
}
E_CMBS_DECT_TYPE;

/*! \brief TestMode Type */
typedef enum
{
    CMBS_TEST_MODE_NORMAL = 0x0,          			// NORMAL
    CMBS_TEST_MODE_TBR6  = 0x81,            			// TBR6
    CMBS_TEST_MODE_TBR10 = 0x82,            			// TBR10
    CMBS_TEST_MODE_ULE_TBR6_NOREG = 0x91,		//TBR6 for ULE

    CMBS_CMBS_TEST_MODE_MAX
}
E_CMBS_TEST_MODE;

/*! \brief Antenna switch Type */
typedef enum
{
    CMBS_ANT_SWITCH_NONE,           // None
    CMBS_ANT_SWITCH_ANT0,           // Antenna 1
    CMBS_ANT_SWITCH_ANT1,           // Antenna 2

    CMBS_ANT_SWITCH_MAX
}
E_CMBS_ANT_SWITCH_MASK;

/*! \brief Structure for Internal Transfer IE */
typedef struct
{
    u16                         u16_HsNum;              /* HS initiated the transfer */
    E_CMBS_CALL_LEG_TYPE        eTransferToType;        /* Type of Transfer-To Leg (Line / PP) */
    u32                         u32_TransferToID;       /* ID of Transfer-To Leg (Line Id or HS Num) */
    E_CMBS_CALL_LEG_TYPE        eTransferFromType;      /* Type of Transfer-From Leg (Line / PP) */
    u32                         u32_TransferFromID      /* ID of Transfer-From Leg (Line Id or HS Num) */;
}
ST_IE_INTERNAL_TRANSFER, *PST_IE_INTERNAL_TRANSFER;


/*! \brief Structure for RTP Codec */
typedef struct
{
    char                sz_CodecStr[CMBS_MAX_DYNAMIC_CODEC_LEN];
    u8                  u8_CodecPt;
}
ST_RTP_CODEC, *PST_RTP_CODEC;

/*! \brief IE structure for CMBS_IE_RTP_SESSION_INFORMATION */
typedef struct
{
    ST_RTP_CODEC        ast_RxCodecList[CMBS_MAX_NUM_OF_CODECS];
    u8                  u8_RxCodecEventPt;
    ST_RTP_CODEC        st_TxCodec;
    u8                  u8_TxCodecEventPt;
    u32                 u32_Capabilities;
    char                sz_SDesName[CMBS_RTCP_MAX_SDES];
    u16                 u16_Duration;
    u32                 u32_CurrentTime;
    u32                 u32_Timestamp;
    u32                 u32_SSRC;
    u8                  u8_JBMinLen;
    u8                  u8_JBMaxLen;
    E_CMBS_RTP_JB_MODE  e_JBMode;
    u32                 u32_DTMFEndPackets;
    E_CMBS_VOICE_LOOP   e_MediaLoopLevel;
    u16                 u16_T38LsRedundancy;
    u16                 u16_T38HsRedundancy;
    u8                  u8_T38EcnOn;
    E_CMBS_AUDIO_MODE   e_AudioMode;
}
ST_IE_RTP_SESSION_INFORMATION, *PST_IE_RTP_SESSION_INFORMATION;

/*! \brief IE structure for CMBS_IE_RTP_DTMF_EVENT */
typedef struct
{
    u8                  u8_Event;
    u16                 u16_Volume;
    u16                 u16_Duration;
}
ST_IE_RTP_DTMF_EVENT, *PST_IE_RTP_DTMF;

/*! \brief IE structure for CMBS_IE_RTP_DTMF_EVENT_INFO */
typedef struct
{
    u16                 u16_EventDuration;
    u16                 u16_MaxEventDuration;
}
ST_IE_RTP_DTMF_EVENT_INFO, *PST_IE_RTP_DTMF_INFO;

/*! \brief IE structure for
     CMBS_IE_LA_PROP_CMD
*/
#define CMBS_LA_MAX_CMD_LEN 64
typedef struct
{
    u16 u16_Length;
    u8 pu8_data[CMBS_LA_MAX_CMD_LEN];
} ST_LA_PROP_CMD, *PST_LA_PROP_CMD;

typedef  struct
{
    u8    u8_BaseNameLen;
    u8    u8_BaseName[CMBS_BS_NAME_MAX_LENGTH];
} ST_IE_BASE_NAME, *PST_IE_BASE_NAME;

/*! \brief IE structure CMBS_IE_CHECKSUM_ERROR  */

typedef enum
{
    CMBS_CHECKSUM_ERROR,
    CMBS_CHECKSUM_NOT_FOUND,
    CMBS_CHECKSUM_NO_EVENT_ID,
} E_CMBS_CHECKSUM_ERROR;

typedef  struct
{
    u16   u16_ReceivedEvent;  // 0xF0 for no event received - CMBS_ CHECKSUM_NO_EVENT_ID
    u8    e_CheckSumError;
} ST_IE_CHECKSUM_ERROR, *PST_IE_CHECKSUM_ERROR;


/*! \brief IE structure for Handset proprietary events */
typedef  struct
{
    u8		u8_HsNumber;
    u8		u8_PropEvent;
    u8		u8_EMC[CMBS_EMC_SIZE];
    u8		u8_LineId;
    u8		u8_LineIdSubType;
    u8      u8_DataLen;
    u8 *pu8_Data;
} ST_IE_HS_PROP_EVENT, *PST_IE_HS_PROP_EVENT;


typedef  struct
{
    u32				u32_WaitForSync;
    u8				u8_GPIO;
} ST_IE_SYPO_SPECIFICATION, *PST_IE_SYPO_SPECIFICATION;

#define CMBS_SYPO_MODE_NONE		 0
#define CMBS_SYPO_MODE_SLAVE	 1
#define CMBS_SYPO_MODE_MASTER    2

#define		APP_SWUP_BOOT_UPGRADE_MASK	(0x80)	// Booter flag in header file

#define CMBS_AFE_RESOURCE_DACADC			0
#define CMBS_AFE_RESOURCE_D_CLASS			1

#define CMBS_AFE_INST_DAC0		0
#define CMBS_AFE_INST_DAC1		1

typedef enum
{
    CMBS_AFE_CHANNEL_SP_OUT,        /* 0 */
    CMBS_AFE_CHANNEL_HS_SP_OUT,     /* 1 */
    CMBS_AFE_CHANNEL_LINE0_OUT,     /* 2 */
    CMBS_AFE_CHANNEL_MIC_IN,        /* 3 */
    CMBS_AFE_CHANNEL_HS_MIC_IN,     /* 4 */
    CMBS_AFE_CHANNEL_LINE1_IN,      /* 5 */
    CMBS_AFE_CHANNEL_LINE0_IN,      /* 6 */
    CMBS_AFE_ADC_IN_START,
    CMBS_AFE_CHANNEL_IN_SINGIN0 = CMBS_AFE_ADC_IN_START,    /* 7 */
    CMBS_AFE_CHANNEL_IN_SINGIN1,        /* 8 */
    CMBS_AFE_ADC_IN_DIFFIN0,            /* 9 */
    CMBS_AFE_ADC_IN_DIFFIN1,            /* A */
    CMBS_AFE_ADC_IN_SINGIN0_MINUS_SINGIN1,                  /* B */
    CMBS_AFE_ADC_IN_VREF_OFFSET,        /* C */
    CMBS_AFE_ADC_IN_MUTE_VREF,          /* D */
    CMBS_AFE_ADC_IN_MUTE_FLOAT,         /* E */
    CMBS_AFE_ADC_IN_DIGMIC1,            /* F */
    CMBS_AFE_ADC_IN_DIGMIC2,            /* 10 */
    CMBS_AFE_ADC_IN_END,
    CMBS_AFE_AMPOUT_IN_START = CMBS_AFE_ADC_IN_END,
    CMBS_AFE_AMPOUT_IN_DAC0 = CMBS_AFE_AMPOUT_IN_START,     /* 11 */
    CMBS_AFE_AMPOUT_IN_DAC1,            /* 12 */
    CMBS_AFE_AMPOUT_IN_DAC0_INV,        /* 13 */
    CMBS_AFE_AMPOUT_IN_DAC1_INV,        /* 14 */
    CMBS_AFE_AMPOUT_IN_SINGIN,          /* 15 */
    CMBS_AFE_AMPOUT_IN_DIFFIN0,         /* 16 */
    CMBS_AFE_AMPOUT_IN_DIFFIN1,         /* 17 */
    CMBS_AFE_AMPOUT_IN_MUTE,            /* 18 */
    CMBS_AFE_AMPOUT_IN_END,
    CMBS_AFE_AMP_START = CMBS_AFE_AMPOUT_IN_END,
    CMBS_AFE_AMP_OUT0 = CMBS_AFE_AMP_START,                 /* 19 */
    CMBS_AFE_AMP_OUT1,                  /* 1A */
    CMBS_AFE_AMP_OUT2,                  /* 1B */
    CMBS_AFE_AMP_OUT3,                  /* 1C */
    CMBS_AFE_AMP_SINGIN0,               /* 1D */
    CMBS_AFE_AMP_SINGIN1,               /* 1E */
    CMBS_AFE_AMP_DIFFIN0,               /* 1F */
    CMBS_AFE_AMP_DIFFIN1,               /* 20 */
    CMBS_AFE_AMP_END,
    CMBS_AFE_ADC_START = CMBS_AFE_AMP_END,
    CMBS_AFE_ADC0 = CMBS_AFE_ADC_START,                     /* 21 */
    CMBS_AFE_ADC1,                      /* 22 */
    CMBS_AFE_ADC2,                      /* 23 */
    CMBS_AFE_ADC_END,
    CMBS_AFE_MIC_START = CMBS_AFE_ADC_END,
    CMBS_AFE_MIC0 = CMBS_AFE_MIC_START,                     /* 24 */
    CMBS_AFE_MIC1,                                          /* 25 */
    CMBS_AFE_MIC_END,
    CMBS_DIGMIC_START = CMBS_AFE_MIC_END,
	CMBS_DIGMIC_ON_CHANNEL1 = CMBS_DIGMIC_START,					/* 26 */
	CMBS_DIGMIC_ON_CHANNEL2,											/* 27 */
    CMBS_DIGMIC_END,

    CMBS_AFE_CHANNEL_MAX
}
E_CMBS_AFE_CHANNEL;

typedef enum
{
    CMBS_AFE_IN_GAIN_START,
    CMBS_AFE_IN_GAIN_MINUS_4DB = CMBS_AFE_IN_GAIN_START,
    CMBS_AFE_IN_GAIN_MINUS_2DB,
    CMBS_AFE_IN_GAIN_0DB,
    CMBS_AFE_IN_GAIN_11KRIN,
    CMBS_AFE_IN_GAIN_2DB,
    CMBS_AFE_IN_GAIN_4DB,
    CMBS_AFE_IN_GAIN_6DB,
    CMBS_AFE_IN_GAIN_22_5KRIN,
    CMBS_AFE_IN_GAIN_8DB,
    CMBS_AFE_IN_GAIN_10DB,
    CMBS_AFE_IN_GAIN_12B,
    CMBS_AFE_IN_GAIN_45KRIN,
    CMBS_AFE_IN_GAIN_14DB,
    CMBS_AFE_IN_GAIN_16DB,
    CMBS_AFE_IN_GAIN_18DB,
    CMBS_AFE_IN_GAIN_90KRIN,
    CMBS_AFE_IN_GAIN_20DB,
    CMBS_AFE_IN_GAIN_22DB,
    CMBS_AFE_IN_GAIN_24DB,
    CMBS_AFE_IN_GAIN_180KRIN,
    CMBS_AFE_IN_GAIN_26DB,
    CMBS_AFE_IN_GAIN_28DB,
    CMBS_AFE_IN_GAIN_30DB,
    CMBS_AFE_IN_GAIN_360KRIN,
    CMBS_AFE_IN_GAIN_32DB,
    CMBS_AFE_IN_GAIN_34DB,
    CMBS_AFE_IN_GAIN_36DB,
    CMBS_AFE_IN_GAIN_720KRIN,
    CMBS_AFE_IN_GAIN_OLOOP1,
    CMBS_AFE_IN_GAIN_OLOOP2,
    CMBS_AFE_IN_GAIN_OLOOP3,
    CMBS_AFE_IN_GAIN_OLOOP4,
    CMBS_AFE_IN_GAIN_END,
    CMBS_AFE_OUT_GAIN_START = CMBS_AFE_IN_GAIN_END,
    CMBS_AFE_OUT_SING_GAIN_0DB = CMBS_AFE_OUT_GAIN_START,
    CMBS_AFE_OUT_SING_GAIN_MINUS_6DB,
    CMBS_AFE_OUT_SING_GAIN_MINUS_12DB,
    CMBS_AFE_OUT_SING_GAIN_MINUS_18DB,
    CMBS_AFE_OUT_SING_GAIN_MINUS_24DB,
    CMBS_AFE_OUT_GAIN_END,

    CMBS_AFE_IN_GAIN_MAX
} E_CMBS_AFE_AMP_GAIN;


typedef enum
{
    CMBS_AUX_MUXINPUT_VLED,
    CMBS_AUX_MUXINPUT_VCCA,
    CMBS_AUX_MUXINPUT_VIBAT,
    CMBS_AUX_MUXINPUT_VTEMP0,
    CMBS_AUX_MUXINPUT_DCIN1_AMP,
    CMBS_AUX_MUXINPUT_DCIN2_AMP,
    CMBS_AUX_MUXINPUT_VBGP,
    CMBS_AUX_MUXINPUT_DCIN0,
    CMBS_AUX_MUXINPUT_DCIN02,
    CMBS_AUX_MUXINPUT_DCIN1,
    CMBS_AUX_MUXINPUT_DCIN12,
    CMBS_AUX_MUXINPUT_DCIN2,
    CMBS_AUX_MUXINPUT_DCIN22,
    CMBS_AUX_MUXINPUT_DCIN3,
    CMBS_AUX_MUXINPUT_DCIN32,
    CMBS_AUX_MUXINPUT_VILED_ATT,
    CMBS_AUX_MUXINPUT_VDD,
    CMBS_AUX_MUXINPUT_VIBAT_ATT,
    CMBS_AUX_MUXINPUT_VTEMP0_DIFF,
    CMBS_AUX_MUXINPUT_VBG_12V,
    CMBS_AUX_MUXINPUT_VBG_REG_12V,
    CMBS_AUX_MUXINPUT_VBG_TEST_REGA,
    CMBS_AUX_MUXINPUT_PMU,

    CMBS_AUX_MUXINPUT_VTEMP_2_200_DIFF,
    CMBS_AUX_MUXINPUT_TOUCH_PANEL_INPUT,
    CMBS_AUX_MUXINPUT_VCC5V_5_DCDC,
    CMBS_AUX_MUXINPUT_DCIN1_ATT,
    CMBS_AUX_MUXINPUT_DCIN2_ATT,
    CMBS_AUX_MUXINPUT_VTEMP_200_CONST,
    CMBS_AUX_MUXINPUT_TEST_LED_CTRL,
    CMBS_AUX_MUXINPUT_ISRC_AUX_1U_FROM_DCDC,
    CMBS_AUX_MUXINPUT_TEST_DCDC_CTRL,
    CMBS_AUX_MUXINPUT_1_2V_BANDGAP,

    CMBS_AUX_MUXINPUT_MAX
}
E_CMBS_AUX_INPUT;

#define AUX_MEASURE_BUSY_ERR 		-2
#define AUX_MEASURE_MUXINPUT_ERR 	-1


typedef  struct
{
    E_CMBS_AFE_CHANNEL        	e_AFEEndPointIN;      /*!< AFE end point 	*/
    E_CMBS_AFE_CHANNEL			e_AFEEndPointOUT;     /*!< AFE end point  	*/
} ST_IE_AFE_ENDPOINTS_CONNECT, *PST_IE_AFE_ENDPOINTS_CONNECT;

typedef  struct
{
    E_CMBS_AFE_CHANNEL	       	e_AFEChannel;         /*!< AFE AFEEndPoint (channel ID)	*/
} ST_IE_AFE_ENDPOINT, *PST_IE_AFE_ENDPOINT;

typedef  struct
{
    E_CMBS_AFE_CHANNEL			e_AFEChannel;        /*!< AFE AFEEndPoint	*/
    s16 							s16_NumOfSteps;      /*!< Gain-Negative number indicates down direction */
} ST_IE_AFE_ENDPOINT_GAIN, *PST_IE_AFE_ENDPOINT_GAIN;

typedef  struct
{
    E_CMBS_AFE_CHANNEL	        e_AFEChannel;        /*!< AFE AFEEndPoint	*/
    s16							s16_GainInDB;        /*!< Actual Gain in DB that was set */
} ST_IE_AFE_ENDPOINT_GAIN_DB, *PST_IE_AFE_ENDPOINT_GAIN_DB;

typedef  struct
{
    E_CMBS_AUX_INPUT				e_AUX_Input;         /*!< MUX Input		*/
    bool							b_Bmp;               /*!< measure via BMP or manually	*/
} ST_IE_AFE_AUX_MEASUREMENT_SETTINGS, *PST_IE_AFE_AUX_MEASUREMENT_SETTINGS;

typedef  struct
{
    s16							s16_Measurement_Result;          /*!< Result in[mV]	*/
} ST_IE_AFE_AUX_MEASUREMENT_RESULT, *PST_IE_AFE_AUX_MEASUREMENT_RESULT;

/*! \brief IE structure for CMBS_IE_AUDIO_SHMEM */

typedef struct 
{
    u32  u32_SampleSize;    /* size of a sample in bits */
    u32  u32_PeriodSize;    /* number of bytes in a period, 0 if no elapsed message required */
    u32  u32_RxBufAddr;    /* physical address of rx sample buffer */ 
    u32  u32_RxBufSize;     /*
                             * size of rx buffer in bytes (should be a multiple of
                             * channels * sample_size (sample_size in bytes))
                             * including the 4 bytes for the progress index
                             */  
    u32  u32_TxBufAddr;    /* physical address of tx sample buffer */ 
    u32  u32_TxBufSize;     /*
	                         * size of tx buffer in bytes (should be a multiple of
	                         * channels * sample_size (sample_size in bytes))
	                         * including the 4 bytes for the progress index
	                         */
} ST_IE_AUDIO_SHMEM, *PST_IE_AUDIO_SHMEM; 


#ifndef CMBS_BIG_ENDIAN_SYSTEM
/*! \brief IE structure for CMBS_IE_TERMINAL_CAPABILITIES */
typedef  struct
{
    u8  u8_IE_Type;                            /*!< IE Type - TERMINAL-CAPABILITY  Octet 1 */
    u8  u8_Length;                             /*!< Length of Contents  Octet 2 */
    u8  u8_DisplayCapabilities	:4;            /*! display capabilities Octet 3 */
    u8  u8_ToneCapabilities	:3;                /*! tone capabilities 	Octet 3 */
    u8  u8_Spare3	:1;                        /*! Spare3				Octet 3 */

    u8  u8_A_VOL	:2;                         /*! A-VOL 				Octet 3b */
    u8  u8_N_REJ	:2;                         /*! N-REJ 				Octet 3b */
    u8  u8_EchoParameter	:3;                 /*! echo parameter 		Octet 3b */
    u8  u8_Spare3b	:1;                         /*! Spare3b				Octet 3b */

    u8  u8_SlotTypeCapability;                  /*! slot type capability 	Octet 3c */
    u8  u8_StoredDisplayCharactersMS;           /*! Number of stored display characters (MS)	Octet 3d */
    u8  u8_StoredDisplayCharactersLS;           /*! Number of stored display characters (LS)	Octet 3e */
    u8  u8_NumberOfLinesInDisplay;              /*! Number of lines in (physical) display		Octet 3f */
    u8  u8_NumberOfCharactersInLine;            /*! Number of characters/line		Octet 3g */
    u8  u8_ScrollingBehaviour;                  /*! Scrolling behaviour field		Octet 3h */
    u8  u8_ProfileIndicator_1;                  /*! Profile indicator_1		Octet 4 */
    u8  u8_ProfileIndicator_2;                  /*! Profile indicator_2		Octet 4a */
    u8  u8_ProfileIndicator_3;                  /*! Profile indicator_3		Octet 4b */
    u8  u8_ProfileIndicator_4;                  /*! Profile indicator_4		Octet 4c */
    u8  u8_ProfileIndicator_5;                  /*! Profile indicator_5		Octet 4d */
    u8  u8_ProfileIndicator_6;                  /*! Profile indicator_6		Octet 4e */
    u8  u8_ProfileIndicator_7;                  /*! Profile indicator_7		Octet 4f */
    u8  u8_ProfileIndicator_8;                  /*! Profile indicator_8		Octet 4g */
    u8  u8_ProfileIndicator_9;                  /*! Profile indicator_9		Octet 4h */
    u8  u8_ProfileIndicator_10;                 /*! Profile indicator_10	Octet 4i */
    u8  u8_ControlCodes	:3;                     /*! control codes			Octet 5 */
    u8  u8_Spare	:2;                         /*! spare				Octet 5 */
    u8  u8_DSC2	:1;                             /*! DSC2				Octet 5 */
    u8  u8_DSAA2	:1;                         /*! DSAA2				Octet 5 */
    u8  u8_Spare5	:1;                         /*! spare5				Octet 5 */

    u8  u8_EscapeTo8BitCharacter;               /*! escape to 8 bit character sets_1 (see note 12) Octet 5a */
    u8  u8_sp4	:1;                             /*! sp4				Octet 6 */
    u8  u8_sp3	:1;                             /*! sp3				Octet 6 */
    u8  u8_sp2	:1;                             /*! sp2				Octet 6 */
    u8  u8_sp1	:1;                             /*! sp1				Octet 6 */
    u8  u8_sp0	:1;                             /*! sp0				Octet 6 */
    u8  u8_BlindSlotIndication	:2;             /*! Blind slot indication	Octet 6 */
    u8  u8_Spare6	:1;                         /*! Spare6			Octet 6 */

    u8  u8_sp11	:1;                            /*! sp11				Octet 6a */
    u8  u8_sp10	:1;                            /*! sp10				Octet 6a */
    u8  u8_sp9	:1;                            /*! sp9				Octet 6a */
    u8  u8_sp8	:1;                            /*! sp8				Octet 6a */
    u8  u8_sp7	:1;                            /*! sp7				Octet 6a */
    u8  u8_sp6	:1;                            /*! sp6				Octet 6a */
    u8  u8_sp5	:1;                            /*! sp5				Octet 6a */
    u8  u8_Spare6a	:1;                        /*! Spare6a			Octet 6a */
} ST_IE_TERMINAL_CAPABILITIES, *PST_IE_TERMINAL_CAPABILITIES;
#else
/*! \brief IE structure for CMBS_IE_TERMINAL_CAPABILITIES */
typedef  struct
{
    u8  u8_IE_Type;                            /*!< IE Type - TERMINAL-CAPABILITY  Octet 1 */
    u8  u8_Length;                             /*!< Length of Contents  Octet 2 */
    u8  u8_Spare3    :1;                            /*! Spare3                Octet 3 */
    u8  u8_ToneCapabilities    :3;                     /*! tone capabilities     Octet 3 */
    u8  u8_DisplayCapabilities    :4;               /*! display capabilities Octet 3 */

    u8  u8_Spare3b    :1;                            /*! Spare3b                Octet 3b */
    u8  u8_EchoParameter    :3;                    /*! echo parameter         Octet 3b */
    u8  u8_N_REJ    :2;                            /*! N-REJ                 Octet 3b */
    u8  u8_A_VOL    :2;                            /*! A-VOL                 Octet 3b */

    u8  u8_SlotTypeCapability;                    /*! slot type capability     Octet 3c */
    u8  u8_StoredDisplayCharactersMS;            /*! Number of stored display characters (MS)    Octet 3d */
    u8  u8_StoredDisplayCharactersLS;            /*! Number of stored display characters (LS)    Octet 3e */
    u8  u8_NumberOfLinesInDisplay;                /*! Number of lines in (physical) display        Octet 3f */
    u8  u8_NumberOfCharactersInLine;            /*! Number of characters/line        Octet 3g */
    u8  u8_ScrollingBehaviour;                      /*! Scrolling behaviour field        Octet 3h */
    u8  u8_ProfileIndicator_1;                      /*! Profile indicator_1        Octet 4 */
    u8  u8_ProfileIndicator_2;                      /*! Profile indicator_2        Octet 4a */
    u8  u8_ProfileIndicator_3;                      /*! Profile indicator_3        Octet 4b */
    u8  u8_ProfileIndicator_4;                      /*! Profile indicator_4        Octet 4c */
    u8  u8_ProfileIndicator_5;                      /*! Profile indicator_5        Octet 4d */
    u8  u8_ProfileIndicator_6;                      /*! Profile indicator_6        Octet 4e */
    u8  u8_ProfileIndicator_7;                      /*! Profile indicator_7        Octet 4f */
    u8  u8_ProfileIndicator_8;                      /*! Profile indicator_8        Octet 4g */
    u8  u8_ProfileIndicator_9;                      /*! Profile indicator_9        Octet 4h */
    u8  u8_ProfileIndicator_10;                     /*! Profile indicator_10       Octet 4i */
    u8  u8_Spare5    :1;                               /*! spare5                Octet 5 */
    u8  u8_DSAA2    :1;                               /*! DSAA2                Octet 5 */
    u8  u8_DSC2    :1;                                   /*! DSC2                Octet 5 */
    u8  u8_Spare    :2;                               /*! spare                Octet 5 */
    u8  u8_ControlCodes    :3;                        /*! control codes            Octet 5 */

    u8  u8_EscapeTo8BitCharacter;                    /*! escape to 8 bit character sets_1 (see note 12) Octet 5a */
    u8  u8_Spare6    :1;                            /*! Spare6            Octet 6 */
    u8  u8_BlindSlotIndication    :2;                 /*! Blind slot indication    Octet 6 */
    u8  u8_sp0    :1;                                   /*! sp0                Octet 6 */
    u8  u8_sp1    :1;                                   /*! sp1                Octet 6 */
    u8  u8_sp2    :1;                                   /*! sp2                Octet 6 */
    u8  u8_sp3    :1;                                   /*! sp3                Octet 6 */
    u8  u8_sp4    :1;                                   /*! sp4                Octet 6 */

    u8  u8_Spare6a    :1;                              /*! Spare6a            Octet 6a */
    u8  u8_sp5    :1;                                  /*! sp5                Octet 6a */
    u8  u8_sp6    :1;                                  /*! sp6                Octet 6a */
    u8  u8_sp7    :1;                                  /*! sp7                Octet 6a */
    u8  u8_sp8    :1;                                  /*! sp8                Octet 6a */
    u8  u8_sp9    :1;                                  /*! sp9                Octet 6a */
    u8  u8_sp10    :1;                                  /*! sp10                Octet 6a */
    u8  u8_sp11    :1;                                  /*! sp11                Octet 6a */
} ST_IE_TERMINAL_CAPABILITIES, *PST_IE_TERMINAL_CAPABILITIES;
#endif

#define TERM_CAPABILITIES_IE_TYPE 63
#define TERM_CAPABILITIES_IE_TYPE_OFFSET  0
#define TERM_CAPABILITIES_CONTENT_LENGHT_OFFSET   1
#define TERM_CAPABILITIES_OCTET3_OFFSET 2 /* IE + Length */
#define TERM_CAPABILITIES_OCTET3_MAX_LEN 8
#define TERM_CAPABILITIES_OCTET4_MAX_LEN 10
#define TERM_CAPABILITIES_OCTET5_MAX_LEN 2
#define TERM_CAPABILITIES_OCTET6_MAX_LEN 2
#define TERM_CAPABILITIES_OCTET4_OFFSET (TERM_CAPABILITIES_OCTET3_OFFSET + TERM_CAPABILITIES_OCTET3_MAX_LEN)
#define TERM_CAPABILITIES_OCTET5_OFFSET (TERM_CAPABILITIES_OCTET4_OFFSET + TERM_CAPABILITIES_OCTET4_MAX_LEN)
#define TERM_CAPABILITIES_OCTET6_OFFSET (TERM_CAPABILITIES_OCTET5_OFFSET + TERM_CAPABILITIES_OCTET5_MAX_LEN)


typedef enum
{
    E_CMBS_CALL_HOLD_REASON_CALL_WAITING_ACCEPT,
    E_CMBS_CALL_HOLD_REASON_USER_REQUEST,
    E_CMBS_CALL_HOLD_REASON_CALL_TOGGLE,
    E_CMBS_CALL_HOLD_REASON_PARALLEL_CALL,
    E_CMBS_CALL_HOLD_REASON_SETTINGS_MISMATCH,

    E_CMBS_CALL_HOLD_REASON_MAX,
}E_CMBS_CALL_HOLD_REASON;

typedef struct
{
    E_CMBS_CALL_HOLD_REASON eReason;
}ST_IE_CALL_HOLD_REASON, *PST_IE_CALL_HOLD_REASON;


typedef enum
{
    E_CMBS_CALL_SCREENING_ACCEPT,
    E_CMBS_CALL_SCREENING_INTERCEPT,

    E_CMBS_CALL_SCREENING_MAX,
}E_CMBS_CALL_SCREENING;

typedef struct
{
    E_CMBS_CALL_SCREENING eScreen;
}ST_IE_CALL_SCREENING, *PST_IE_CALL_SCREENING;



/*! \brief IE structure for CMBS_IE_SUOTA_FILE_INFO */
typedef struct
{
    u32 u32_FileSize;   	/*! Total size of the file to be transfered to the HS */
    u32 u32_To;             /*! Range of the requested bytes - to */
    u32 u32_From;       	/*! Range of the requested bytes - from */
#ifdef SUOTA_SDU_CHOPPING        
    u8 u8_SequenceNum;       	/*! Sequence number including MSB if another sequence follows */
#endif
}ST_IE_SUOTA_FILE_INFO, *PST_IE_SUOTA_FILE_INFO;


/** Hazardous potential test results. */
typedef struct
{
   /** HPT result, passed or failed. */
   s32 b_result;
   /** HPT AC ring wire to gnd value, [Vrms]. */
   float   f_hpt_ac_r2g;
   /** HPT AC TIP wire to GND value, [Vrms]. */
   float   f_hpt_ac_t2g;
   /** HPT AC tip wire to ring wire value, [Vrms]. */
   float   f_hpt_ac_t2r;
   /** HPT DC ring wire to gnd value, [V]. */
   float   f_hpt_dc_r2g;
   /** HPT DC tip wire to gnd value, [V]. */
   float   f_hpt_dc_t2g;
   /** HPT DC tip wire to ring wire value, [V].*/
   float   f_hpt_dc_t2r;
} LT_GR909_HPT_t;

/** Foreign electromotive forces test results. */
typedef struct
{
   /** FEMF result, passed or failed. */
   s32 b_result;
   /** FEMF ac ring wire to gnd value, [Vrms]. */
   float   f_femf_ac_r2g;
   /** fFEMF AC tip wire to gnd value, [Vrms]. */
   float   f_femf_ac_t2g;
   /** FEMF AC tip wire to ring wire value, [Vrms]. */
   float   f_femf_ac_t2r;
   /** FEMF DC ring wire to gnd value, [V]. */
   float   f_femf_dc_r2g;
   /** FEMF DC tip wire to gnd value, [V]. */
   float   f_femf_dc_t2g;
   /** FEMF DC tip wire to ring wire value, [V]. */
   float   f_femf_dc_t2r;
} LT_GR909_FEMF_t;

/** Resistive faults test results. */
typedef struct
{
   /** RFT result, passed or failed. */
   s32 b_result;
   /** RFT ring wire to gnd value, [Ohm]. */
   float   f_rft_r2g;
   /** RFT tip wire to gnd value, [Ohm]. */
   float   f_rft_t2g;
   /** RFT tip wire to ring wire value, [Ohm]. */
   float   f_rft_t2r;
} LT_GR909_RFT_t;

/** Receiver off-hook test results. */
typedef struct
{
   /** ROH result, passed or failed. */
   s32 b_result;
   /** ROH tip wire to ring wire value for low voltage, [Ohm]. */
   float   f_roh_t2r_l;
   /** ROH tip wire to ring wire value for high voltage, [Ohm]. */
   float   f_roh_t2r_h;
} LT_GR909_ROH_t;

/** Ringer impedance test results. */
typedef struct
{
   /** RIT result, passed or failed. */
   s32 b_result;
   /** RIT value, , [Ohm]. */
   float   f_rit_res;
} LT_GR909_RIT_t;

/** GR-909 results structure. */
typedef struct
{
   /** Valid results mask, set with \ref IFX_LT_GR909_MASK_t. */
   u32         valid_mask;
   /** Hazardous potential test results. */
   LT_GR909_HPT_t   hpt;
   /** Foreign electromotive forces test results. */
   LT_GR909_FEMF_t  femf;
   /** Resistive faults test results. */
   LT_GR909_RFT_t   rft;
   /** Receiver off-hook test results. */
   LT_GR909_ROH_t   roh;
   /** Ringer impedance test results. */
   LT_GR909_RIT_t   rit;
} LT_GR909_RESULT_t;


/*! \brief IE structure for CMBS_IE_SLIC_LINE_TEST_RESULTS */
typedef struct
{
#ifdef OLD_OPTION
    /** Valid results mask */
    u32         valid_mask; // bit i is set for valid result.  bit0 is for hpt,  bit1 is for femf,  bit2 is for rft,  bit3 is for roh,  bit4 is for rit,
    /** Hazardous potential test results. */
    u8   hpt; // 1 = OK, 0 = fail
    /** Foreign electromotive forces test results. */
    u8  femf; // 1 = OK, 0 = fail
    /** Resistive faults test results. */
    u8   rft; // 1 = OK, 0 = fail
    /** Receiver off-hook test results. */
    u8   roh; // 1 = OK, 0 = fail
    /** Ringer impedance test results. */
    u8   rit; // 1 = OK, 0 = fail
#elif RAW_DATA_OPTION

    u8 u8_LineTestResultLen;                   /*!< Line Test Result length */
//    u8 *pu8_data;                     /*!< Line Test Result byte array */
    u8 pu8_data[100];                     /*!< Line Test Result byte array */
#else

   u32         valid_mask; // bit i is set for valid result.  bit0 is for hpt,  bit1 is for femf,  bit2 is for rft,  bit3 is for roh,  bit4 is for rit,
   /** HPT result, passed or failed. */
   s32     b_hpt_result;
   /** HPT AC ring wire to gnd value, [Vrms]. */
   s32   f_hpt_ac_r2g;
   /** HPT AC TIP wire to GND value, [Vrms]. */
   s32   f_hpt_ac_t2g;
   /** HPT AC tip wire to ring wire value, [Vrms]. */
   s32   f_hpt_ac_t2r;
   /** HPT DC ring wire to gnd value, [V]. */
   s32   f_hpt_dc_r2g;
   /** HPT DC tip wire to gnd value, [V]. */
   s32   f_hpt_dc_t2g;
   /** HPT DC tip wire to ring wire value, [V].*/
   s32   f_hpt_dc_t2r;

   
   /** FEMF result, passed or failed. */
   s32 b_femf_result;
   /** FEMF ac ring wire to gnd value, [Vrms]. */
   s32   f_femf_ac_r2g;
   /** fFEMF AC tip wire to gnd value, [Vrms]. */
   s32   f_femf_ac_t2g;
   /** FEMF AC tip wire to ring wire value, [Vrms]. */
   s32   f_femf_ac_t2r;
   /** FEMF DC ring wire to gnd value, [V]. */
   s32   f_femf_dc_r2g;
   /** FEMF DC tip wire to gnd value, [V]. */
   s32   f_femf_dc_t2g;
   /** FEMF DC tip wire to ring wire value, [V]. */
   s32   f_femf_dc_t2r;

   /** RFT result, passed or failed. */
   s32 b_rft_result;
   /** RFT ring wire to gnd value, [Ohm]. */
   s32   f_rft_r2g;
   /** RFT tip wire to gnd value, [Ohm]. */
   s32   f_rft_t2g;
   /** RFT tip wire to ring wire value, [Ohm]. */
   s32   f_rft_t2r;

   /** ROH result, passed or failed. */
   s32 b_roh_result;
   /** ROH tip wire to ring wire value for low voltage, [Ohm]. */
   s32   f_roh_t2r_l;
   /** ROH tip wire to ring wire value for high voltage, [Ohm]. */
   s32   f_roh_t2r_h;

   /** RIT result, passed or failed. */
   s32 b_rit_result;
   /** RIT value, , [Ohm]. */
   s32   f_rit_res;


#endif
}
ST_IE_SLIC_LINE_TEST_RES, *PST_IE_SLIC_LINE_TEST_RES;

/*! \brief IE structure for CMBS_IE_SLIC_NLT_CAP_TEST_RESULTS */
typedef struct
{
   /** Validity of measurement result tip to ring. */
   s32  b_tip2ring_valid;
   /** Measured capacitance tip to ring [nF]. */
   s32    f_tip2ring_cap;
   /** Validity of measurement results tip to ground and ring to ground. */
   s32  b_line2gnd_valid;
   /** Measured capacitance tip to ground [nF]. */
   s32    f_tip2gnd_cap;
   /** Measured capacitance ring to ground [nF]. */
   s32  f_ring2gnd_cap;
}
ST_IE_SLIC_NLT_CAP_TEST_RES, *PST_IE_SLIC_NLT_CAP_TEST_RES;

/** Results of the NLT capacitance measurement.*/
typedef struct
{
   /** Validity of measurement result tip to ring. */
   bool  b_tip2ring_valid;
   /** Measured capacitance tip to ring [nF]. */
   float    f_tip2ring_cap;
   /** Validity of measurement results tip to ground and ring to ground. */
   bool  b_line2gnd_valid;
   /** Measured capacitance tip to ground [nF]. */
   float    f_tip2gnd_cap;
   /** Measured capacitance ring to ground [nF]. */
   float    f_ring2gnd_cap;
} NLT_Capacitance_Result_t;


typedef struct
{
    u16  tone_Id;                   // One of the above tones
    u16  loop;                          // Number of complex tone to be played. 0 = Infinite.
    u16  Counter;                   // Number of simple tones in this message. Today: 1 or 2

                                                // Simple tone 1:
    u16  tone_Id1;                 // For complex tone - Option to refer to an existing tone as simple.
                                                // If 0 - new tone. If any existing tone Id - next parameters are not read. 
    u16  tone_type1;		  // New configuration or existing configuration
    u16  level1A;                    
    u16  level1B;                    //  Power level for frequency B in 0.1 dB steps; -300 < levelA < 0. 
    u16  freq1A;                     // Tone frequency A in Hz; 0 <= Hz < 4000.
    u16  freq1B;                     // Tone frequency B in Hz; 0 <= Hz < 4000.
    u16  cadence1On;
    u16  cadence1Off;
    u16  loop1;                       // Number of times to play this simple tone, 0 < loop < 8. (Last tone may be 0 = infinite)

                                                // Simple tone 2:
    u16  tone_Id2;                 // For complex tone - Option to refer to an existing tone as simple.
                                                // If 0 - new tone. If any existing tone Id - next parameters are not read. 
    u16  tone_type2;		  // New configuration or existing configuration
    u16  level2A;                    // Power level for frequency A in 0.1 dB steps; -300 < levelB < 0.
    u16  level2B;                    // Power level for frequency B in 0.1 dB steps; -300 < levelB < 0.
    u16  freq2A;                     // Tone frequency A in Hz; 0 <= Hz < 4000. 
    u16  freq2B;                     // Tone frequency B in Hz; 0 <= Hz < 4000. 
    u16  cadence2On;
    u16  cadence2Off;
    u16  loop2;                       // Number of times to play this simple tone, 0 < loop < 8 (Last tone may be 0 = infinite).
} 
ST_IE_FXS_TONE_CFG, *PST_IE_FXS_TONE_CFG;

/*
typedef enum
{
	E_CMBS_CALLLIST_CALL_CLOSE,         // 0 E_APPCMBS_CALL_CLOSE
	E_CMBS_CALLLIST_CALL_OUT_PEND,      // 3 E_APPCMBS_CALL_OUT_PEND
	E_CMBS_CALLLIST_CALL_OUT_RING,      // 7 E_APPCMBS_CALL_OUT_RING				   
	E_CMBS_CALLLIST_CALL_ACTIVE,        // 8 E_APPCMBS_CALL_ACTIVE
	E_CMBS_CALLLIST_CALL_HOLD,       	// 10 E_APPCMBS_CALL_ON_HOLD
	E_CMBS_CALLLIST_CALL_CONFERENCE,    // 11  E_APPCMBS_CALL_CONFERENCE
}E_CMBS_CALLLIST_CALL_STATE;

typedef enum
{
    E_CMBS_CALLLIST_CALL_INTERNAL,
	E_CMBS_CALLLIST_CALL_EXTERNAL
} E_CMBS_CALLLIST_CALL_TYPE;
*/

/*! \brief CBS API layer media state */
typedef enum
{
   E_CMBS_MEDIA_CLOSE,        /*!< Media entity is closed */
   E_CMBS_MEDIA_PEND,         /*!< Media entity is prepared, codec negotiated and channel ID from Target available */
   E_CMBS_MEDIA_ACTIVE        /*!< Media entity is started to stream */
} E_CMBS_MEDIA;

/*! \brief IE structure for ST_IE_CALL_LIST */
typedef  struct
{
	u32 	u32_CallInstanse;
	u8    	u8_CallId; /* callID of call in CMBS */
	u8    	u8_LineId; 
	u8   	u8_CallerNumberLen;
	u8      u8_CallerParty[CMBS_CALLER_NUM_MAX_LEN];
	u8   	u8_CalledNumberLen;
	u8      u8_CalledParty[CMBS_CALLED_NUM_MAX_LEN];
	u16 	u16_HSMask; 
	u32   	u32_ChannelID;
	u32	 	u32_ChannelParameter;	
	u8   	u8_Codec;
	u8		pu8_CodecsList[CMBS_AUDIO_CODEC_MAX];
	u8		u8_CodecsLength;    
	//E_CMBS_CALLLIST_CALL_STATE u8_CallStatus;
	E_CMBS_CALL_STATE_STATUS u8_Callstatus;
	E_CMBS_CALL_STATE_TYPE u8_CallType;
	u8      u8_Media;
} ST_IE_CALL_LIST, *PST_IE_CALL_LIST;

/*! \brief Crash Dump Enable status values */
typedef  enum
{
    CRASH_DUMP_DISABLE,			/*!< Disabled */
    CRASH_DUMP_ENABLE,			/*!< Enabled */
    CRASH_DUMP_STATUS_INVALID,	/*!< Invalid */
} E_CRASH_DUMP_ENABLE_STATUS;

/*! \brief IE structure for CMBS_IE_CRASH_DUMP_ENABLE */
typedef  struct
{
    E_CRASH_DUMP_ENABLE_STATUS
       e_CrashDumpEnble;             /*!< Response: OK or Error */
} ST_IE_CRASH_DUMP_ENABLE_STATUS, *PST_IE_CRASH_DUMP_ENABLE_STATUS;

/*! \brief IE structure for CMBS_IE_PACKET_NUMBER */
typedef  struct
{
    u16 u16_PktNr;         /*!< Current Packet Number */
    u16 u16_TotalPkts;     /*!< Total packets */
} ST_IE_PACKET_NUMBER, *PST_IE_PACKET_NUMBER;

/*! \brief IE structure for CMBS_IE_PRODUCT_ID */
#define CMBS_PRODUCT_ID_LENGTH 5
typedef  struct
{
    u8 u8_ProductID[CMBS_PRODUCT_ID_LENGTH];	/*Including NULL*/
    u8 u8_Length;					    /*Length is fixed to 5 currently*/
} ST_IE_PRODUCT_ID, *PST_IE_PRODUCT_ID;

/*! \brief Echo cancellation status values */
#define PROPRIETARY_EVENT_DECT_HS_EC_VOICE	0x30 /* 0x0 to 0x3F is the valid range*/
typedef  enum
{
    DECT_EC_OFF_EC,    /*!< EC is off */
    DECT_EC_ON_VOICE,  /*!< EC is on */
    DECT_EC_OFF_FAX    /*!< Special EC for FAX is On*/
} E_DECT_HS_EC_STATUS;

/* Line Identifier SubType*/
typedef enum 
{
    CMBS_LINE_SUBTYPE_EXTERNAL,
    CMBS_LINE_SUBTYPE_INTERNAL,
    CMBS_LINE_SUBTYPE_INVALID
} eLineSubType;


/*****************************************************************************
 * Callback functions
 *****************************************************************************/

/*!
      \brief
         Callback function for upcoming events to the application

      \param[in]        pv_AppRef         Reference pointer of application provided in cmbs_api_RegisterCb()
      \param[in]        e_EventID         Type of event
      \param[in]        pv_EventIEListRef Pointer to IE List returned in the event

 */
typedef  void  (*PFN_CMBS_API_CB)(void *pv_AppRef,
                                  E_CMBS_EVENT_ID e_EventID,
                                  void *pv_EventIEListRef);

/*!
      \brief
         Callback function called by CMBS to prepare writing an outgoing packet
 */
typedef  void  (*PFN_CMBS_API_LOG_OUTGOING_PACKET_PREPARE_CB)(void);

/*!
      \brief
         Callback function called by CMBS to write outgoing data to a log file

      \param[in]        pu8_Buffer        Pointer to buffer, which contains part of the packet to transmit
      \param[in]        u16_Size          Size of buffer
 */
typedef  void  (*PFN_CMBS_API_LOG_OUTGOING_PACKET_PART_WRITE_CB)(u16 u16_Event, u8 *pBuf, u16 u16_Length);

/*!
      \brief
         Callback function called by CMBS to store the outgoing packet which was previously created in the logfile
 */
typedef  void  (*PFN_CMBS_API_LOG_OUTGOING_PACKET_WRITE_FINISH_CB)(void);

/*!
      \brief
         Callback function called by CMBS to store an incoming packet in the logfile

      \param[in]        pu8_Buffer        Pointer to buffer, which contains the received packet
      \param[in]        u16_Size          Size of buffer
 */
typedef  void  (*PFN_CMBS_API_LOG_INCOMING_PACKET_WRITE_FINISH_CB)(u8 *pu8_Buffer, u16 u16_Size);

/*!
      \brief
         Callback function called by CMBS to store the content of the log buffer in the logfile

      \param[in]        pu8_Buffer        Pointer to buffer, which contains the content of log buffer
      \param[in]        u16_Size          Size of buffer
 */
typedef  void  (*PFN_CMBS_API_LOG_WRITE_LOG_BUFFER_CB)(u8 *pu8_Buffer, u16 u16_Size);

/*!
      \brief
         Table of callback functions called by CMBS to access the logfile
 */
typedef struct
{
    PFN_CMBS_API_LOG_OUTGOING_PACKET_PREPARE_CB      pfn_cmbs_api_log_outgoing_packet_prepare_cb;
    PFN_CMBS_API_LOG_OUTGOING_PACKET_PART_WRITE_CB   pfn_cmbs_api_log_outgoing_packet_part_write_cb;
    PFN_CMBS_API_LOG_OUTGOING_PACKET_WRITE_FINISH_CB pfn_cmbs_api_log_outgoing_packet_write_finish_cb;
    PFN_CMBS_API_LOG_INCOMING_PACKET_WRITE_FINISH_CB pfn_cmbs_api_log_incoming_packet_write_finish_cb;
    PFN_CMBS_API_LOG_WRITE_LOG_BUFFER_CB             pfn_cmbs_api_log_write_log_buffer_cb;
} ST_CB_LOG_BUFFER, *PST_CB_LOG_BUFFER;


#if defined( __cplusplus )
extern "C"
{
#endif


/*****************************************************************************
 * API Maintenance functions
 *****************************************************************************/

//*/
//    ==========  cmbs_api_Init  ===========
/*!
     \brief
         Initializes the API for Host <-> DECT Module communication.

      <h2>cmbs_api_Init</h2>

      <h3>Introduction</h3>
         Initializes the API for DM communication.<br>
         It opens the devices for control and media connectivity.<br>
         Used communication devices ad their properties will be set by function parameters,<br>
         e.g. UART, USB, SPI, IOM etc.<br>
         This function is locked until the target does respond or a timeout occurs.<BR>

      <h3>Use cases</h3>
         Startup sequence of the application.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_Init ( E_CMBS_API_MODE e_Mode, ST_CMBS_DEV * pst_DevCtl, ST_CMBS_DEV * pst_DevMedia )
        </b><br>
        <br>
        This function must be called before any other API functions.<br>

      \param[in]        e_Mode            api mode( reserved for the future:
                                                   multiline, adv_multiline, dect_pipe, etc. )
      \param[in]        pst_DevCtl        pointer to device control (Serial, USB, SPI, etc.) definition
      \param[in]        pst_DevMedia      pointer to media control (IOM, USB Audio Class, etc.) definition

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_Init(E_CMBS_API_MODE e_Mode, ST_CMBS_DEV *pst_DevCtl, ST_CMBS_DEV *pst_DevMedia);

//*/
//    ==========  cmbs_api_UnInit  ===========
/*!
      \brief
         Finalizes the API for Host <-> DECT Module communication.

      <h2>cmbs_api_UnInit</h2>

      <h3>Introduction</h3>
         Finalizes the API for DM communication.<br>
         It closes the devices for control and media connectivity and releases any resources being
         used by the API.

      <h3>Use cases</h3>
         End of application.

      <h3>API Functions description</h3>
         <b>
         void cmbs_api_UnInit ( void )
         </b><br><br>
         This function should be called on shutdown of the CMBS communication application.

      \par Parameters:
                        none

      \return           none

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
void              cmbs_api_UnInit(void);

//*/
//    ==========  cmbs_api_RegisterCb  ===========
/*!
      \brief
         Registers the callback function.

      <h2>cmbs_api_RegisterCb</h2>

      <h3>Introduction</h3>
         This function is called to register the event callback.<br>
         The callback function will be called after the communication device received an event,<br>
         thus being an essential part of the communication mechanism.

      <h3>Use cases</h3>

      <h3>API Functions description</h3>
         <b>
         void * cmbs_api_RegisterCb ( void * pv_AppRef, PFN_CMBS_API_CB pfn_api_Cb, u16 u16_bcdVersion )
         </b><br><br>
         This function should be called after CMBS communication initialization.
         <br>
      \param[in]        pv_AppRef         reference pointer of application, will be passed in the callback function
      \param[in]        pfn_api_Cb        callback function pointer
      \param[in]        u16_bcdVersion    api version in BCD coded format,
                                          which the application wants to use, e.g. 0x0100 for version 1.0

      \return           pointer to AppRefHandle( pv_AppRefHandle ), otherwise in case version is not supported NULL.<br>
                        pv_AppRefHandle should be used in all subsequent calls to the API.


      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
void*            cmbs_api_RegisterCb(void *pv_AppRef, PFN_CMBS_API_CB pfn_api_Cb, u16 u16_bcdVersion);

//*/
//    ==========  cmbs_api_UnregisterCb  ===========
/*!
      \brief
         Unregisters the callback function.

         <h2>cmbs_api_UnregisterCb</h2>

      <h3>Introduction</h3>
         Unregisters the callback function.

      <h3>Use cases</h3>

      <h3>API Functions description</h3>
         <b>
         void cmbs_api_UnregisterCb ( void * pv_AppRefHandle )
        </b><br><br>
         This function is called to unregister the event callback function.<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           none

      \see              cmbs_api_RegisterCb

      <b>Sample Code:</b><br>
      <code></code>
*/
void              cmbs_api_UnregisterCb(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_api_ModuleVersionGet  ===========
/*!
      \brief
         Returns the version of the CMBS communication API currently in use by the Cordless Module.

         <h2>cmbs_api_ModuleVersionGet</h2>

      <h3>Introduction</h3>
         Returns the version of the CMBS communication API currently in use by the Cordless Module.

      <h3>Use cases</h3>
         Request the current API version of CMBS C-API on the Cordless Module.<br>
         Note: The version was provided in the HELLO event and therefore
         function will return immediately with the version.<br>

      <h3>API Functions description</h3>
         <b>
         u16 cmbs_api_ModuleVersionGet ( void )
         </b><br><br>
         Returns the API version in BCD format,<br>
         e.g. 0X0101 representing version 1.01<br><br>

      \par Parameters:
                        none

      \return           current API version in BCD format<br>

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
u16               cmbs_api_ModuleVersionGet(void);

//*/
//    ==========  cmbs_api_ModuleVersionBuildGet  ===========
/*!
      \brief
         Returns the build version of the CMBS communication API currently in use by the Cordless Module.

         <h2>cmbs_api_ModuleVersionBuildGet</h2>

      <h3>Introduction</h3>
         Returns the build version of the CMBS communication API currently in use by the Cordless Module.

      <h3>Use cases</h3>
         Request the current API version of CMBS C-API on the Cordless Module.<br>
         Note: The version was provided in the HELLO event and therefore
         function will return immediately with the version.<br>

      <h3>API Functions description</h3>
         <b>
         u16 cmbs_api_ModuleVersionBuildGet ( void )
         </b><br><br>
         Returns the build version in BCD format,<br>
         e.g. 0X0101 representing version 1.01<br><br>

      \par Parameters:
                        none

      \return           current API version in BCD format<br>

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
u16               cmbs_api_ModuleVersionBuildGet(void);

//*/
//    ==========  cmbs_api_ie_GetList  ===========
/*!
      \brief
         Provides a reference to Information Element (IE) list to be used in subsequent function calls.

         <h2>cmbs_api_ie_GetList</h2>

      <h3>Introduction</h3>
         Provides a reference to Information Element (IE) list to be used in subsequent function calls.

      <h3>Use cases</h3>
         Any function call, using the list of Information Elements (IEs).

      <h3>API Functions description</h3>
         <b>
         void * cmbs_api_ie_GetList ( void )
         </b><br><br>
         This function allocates internal buffer to store IEs.

      \par Parameters:
                        none

      \return           Reference pointer to IE list

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
void*            cmbs_api_ie_GetList(void);

//*/
//    ==========  cmbs_api_ie_FreeList  ===========
/*!
      \brief
         Free the IE list.

         <h2>cmbs_api_ie_FreeList</h2>

      <h3>Introduction</h3>
         Free the IE list.

      <h3>Use cases</h3>
         Any function call, using the list of Information Elements (IEs).

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_FreeList ( void * pv_RefIEList )
         </b><br><br>
         This function frees the IE List.

      \param[in]        pv_RefIEList      pointer to IE list to be freed

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_FreeList(void *pv_RefIEList);

//*/
//    ==========  cmbs_api_ie_GetFirst  ===========
/*!
      \brief
         Get first IE from IE List.

         <h2>cmbs_api_ie_GetFirst</h2>

      <h3>Introduction</h3>
         Get first IE from IE List.

      <h3>Use cases</h3>
         Any function call, using the list of Information Elements (IEs).

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_GetFirst ( void * pv_RefIEList, void ** ppv_RefIE, u16 * pu16_IEType )
         </b><br><br>
         This function returns the first IE from the IE List.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in,out]    ppv_RefIE         pointer to first IE
      \param[in,out]    pu16_IEType       pointer to IE Type

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_GetFirst(void *pv_RefIEList, void **ppv_RefIE, u16 *pu16_IEType);

//*/
//    ==========  cmbs_api_ie_GetNext  ===========
/*!
      \brief
         Get next IE from IE List.

         <h2>cmbs_api_ie_GetNext</h2>

      <h3>Introduction</h3>
         Get next IE from IE List.

      <h3>Use cases</h3>
         Any function call, using the list of Information Elements (IEs).

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_GetNext ( void * pv_RefIEList, void ** ppv_RefIE, u16 * pu16_IEType )
         </b><br><br>
         This function retrieves the next IE from the IE List.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in,out]    ppv_RefIE         pointer to next IE
      \param[in,out]    pu16_IEType       pointer to IE Type

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_GetNext(void *pv_RefIEList, void **ppv_RefIE, u16 *pu16_IEType);


/*****************************************************************************
 * Information Elements ADD / GET functions
 *****************************************************************************/

//*/
//    ==========  cmbs_api_ie_LineIdAdd  ===========
/*!
      \brief
         Add Line Id Information Element to IE List.

         <h2>cmbs_api_ie_LineIdAdd</h2>

      <h3>Introduction</h3>
         Add Line Id Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting Line Id by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LineIdAdd ( void * pv_RefIEList, u8 u8_LineId )
         </b><br><br>

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        u8_LineId         external line Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LineIdAdd(void *pv_RefIEList, u8 u8_LineId);

//*/
//    ==========  cmbs_api_ie_LineIdGet  ===========
/*!
      \brief
         Get Line Id Information Element from IE List element.

         <h2>cmbs_api_ie_LineIdGet</h2>

      <h3>Introduction</h3>
         Get Line Id Information Element from IE List element.

      <h3>Use cases</h3>
         Received Line Id IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LineIdGet ( void * pv_RefIE, u8 * pu8_LineId )
         </b><br><br>
         The function extracts the Line Id data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu8_LineId        pointer to Line Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LineIdGet(void *pv_RefIE, u8 *pu8_LineId);

//*/
//    ==========  cmbs_api_ie_MelodyGet  ===========
/*!
      \brief
         Get Melody Information Element from IE List element.

         <h2>cmbs_api_ie_MelodyGet</h2>

      <h3>Introduction</h3>
         Get Melody Information Element from IE List element.

      <h3>Use cases</h3>
         Received Melody Id IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MelodyGet ( void * pv_RefIE, u8 * pu8_Melody )
         </b><br><br>
         The function extracts the Melody data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu8_Melody        pointer to Melody (alerting pattern selection)

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MelodyGet(void *pv_RefIEList, u8 *pu8_Melody);

//*/
//    ==========  cmbs_api_ie_MelodyAdd  ===========
/*!
      \brief
         Add Melody Information Element to IE List element.

         <h2>cmbs_api_ie_MelodyAdd</h2>

      <h3>Introduction</h3>
         Get Melody Information Element from IE List element.

      <h3>Use cases</h3>
         Sending Melody IE to an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MelodyAdd ( void * pv_RefIE, u8 u8_Melody )
         </b><br><br>
         The function adds the Melody data IE to the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    u8_Melody         Melody (alerting pattern selection)

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MelodyAdd(void *pv_RefIE, u8 u8_Melody);

//*/
//    ==========  cmbs_api_ie_CallInstanceAdd  ===========
/*!
      \brief
         Add Call Instance Information Element to IE List.

         <h2>cmbs_api_ie_CallInstanceAdd</h2>

      <h3>Introduction</h3>
         Add Call Instance Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting Call Instance by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallInstanceAdd ( void * pv_RefIEList, u32 u32_CallInstance )
         </b><br><br>

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        u32_CallInstance  Call Instance ID

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallInstanceAdd(void *pv_RefIEList, u32 u32_CallInstance);

//*/
//    ==========  cmbs_api_ie_CallInstanceGet  ===========
/*!
      \brief
         Get Call Instance Information Element from IE List element.

         <h2>cmbs_api_ie_CallInstanceGet</h2>

      <h3>Introduction</h3>
         Get Call Instance Information Element from IE List element.

      <h3>Use cases</h3>
         Received Call Instance IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallInstanceGet ( void * pv_RefIE, u32 * pu32_CallInstance )
         </b><br><br>
         The function extracts the Call Instance IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu32_CallInstance pointer to Call Instance ID

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallInstanceGet(void *pv_RefIE, u32 *pu32_CallInstance);

//*/
//    ==========  cmbs_api_ie_CallerPartyAdd  ===========
/*!
      \brief
         Add Caller ID to IE List.

         <h2>cmbs_api_ie_CallerPartyAdd</h2>

      <h3>Introduction</h3>
         Add Caller ID to IE List.

      <h3>Use cases</h3>
         Transmitting Caller ID by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallerPartyAdd ( void * pv_RefIEList, ST_IE_CALLERPARTY * pst_CallerParty )
         </b><br><br>
         The application has to populate a ST_IE_CALLERPARTY structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_CallerParty   pointer to ST_IE_CALLERPARTY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallerPartyAdd(void *pv_RefIEList, ST_IE_CALLERPARTY *pst_CallerParty);

E_CMBS_RC         cmbs_api_ie_CallTransferReqAdd(void *pv_RefIEList, PST_IE_CALLTRANSFERREQ pst_CallTrf);

E_CMBS_RC         cmbs_api_ie_CallTransferReqGet(void *pv_RefIE, PST_IE_CALLTRANSFERREQ pst_CallTrf);


//*/
//    ==========  cmbs_api_ie_InternalCallTransferReqAdd  ===========
/*!
      \brief
         Add Internal Call Transfer to IE List.

         <h2>cmbs_api_ie_InternalCallTransferReqAdd</h2>

      <h3>Introduction</h3>
         Add Internal Call Transfer to IE List.

      <h3>Use cases</h3>
            An internal call transfer is a call transfer involving at most 1 line.
            Possible scenarios:
            1.	PP1 is involved with a call with PP2 and a call with Line1, and wants to transfer PP2 to Line1.
            2.	PP1 is involved with a call with PP2 and a call with PP3, and wants to transfer PP2 to PP3.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_InternalCallTransferReqAdd ( void * pv_RefIEList, ST_IE_INTERNAL_TRANSFER * pst_InternalTransfer )
         </b><br><br>
         The application has to populate a ST_IE_INTERNAL_TRANSFER structure first.

      \param[in]        pv_RefIEList            pointer to IE List
      \param[in]        pst_InternalTransfer    pointer to ST_IE_INTERNAL_TRANSFER structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_InternalCallTransferReqAdd(void *pv_RefIEList, PST_IE_INTERNAL_TRANSFER pst_InternalTransfer);


//*/
//    ==========  cmbs_api_ie_InternalCallTransferReqGet  ===========
/*!
      \brief
         Get Internal Call Transfer Information Element from IE List element.

         <h2>cmbs_api_ie_InternalCallTransferReqGet</h2>

      <h3>Introduction</h3>
         Get Internal Call Transfer Information Element from IE List element.

      <h3>Use cases</h3>
         Received Internal Call Transfer IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_InternalCallTransferReqGet ( void * pv_RefIE, ST_IE_INTERNAL_TRANSFER * pst_InternalTransfer )
         </b><br><br>
         The function extracts the Internal Call Transfer IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE              pointer to IE list element returned by<br>
                                              cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_InternalTransfer  pointer to ST_IE_INTERNAL_TRANSFER

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_InternalCallTransferReqGet(void *pv_RefIE, PST_IE_INTERNAL_TRANSFER pst_InternalTransfer);



//*/
//    ==========  cmbs_api_ie_CallerPartyGet  ===========
/*!
      \brief
         Get Caller ID Information Element from IE List element.

         <h2>cmbs_api_ie_CallerPartyGet</h2>

      <h3>Introduction</h3>
         Get Caller ID Information Element from IE List element.

      <h3>Use cases</h3>
         Received Caller ID IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallerPartyGet ( void * pv_RefIE, ST_IE_CALLERPARTY * pst_CallerParty )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_CallerParty   pointer to ST_IE_CALLERPARTY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallerPartyGet(void *pv_RefIE, ST_IE_CALLERPARTY *pst_CallerParty);

//*/
//    ==========  cmbs_api_ie_CalledPartyAdd  ===========
/*!
      \brief
         Add Called ID to IE List.

         <h2>cmbs_api_ie_CalledPartyAdd</h2>

      <h3>Introduction</h3>
         Add Called ID to IE List.

      <h3>Use cases</h3>
         Transmitting Called ID by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CalledPartyAdd ( void * pv_RefIEList, ST_IE_CALLEDPARTY * pst_CalledParty )
         </b><br><br>
         The application has to populate a ST_IE_CALLEDPARTY structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_CalledParty   pointer to ST_IE_CALLEDPARTY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CalledPartyAdd(void *pv_RefIEList, ST_IE_CALLEDPARTY *pst_CalledParty);

//*/
//    ==========  cmbs_api_ie_CalledPartyGet  ===========
/*!
      \brief
         Get Called ID Information Element from IE List element.

         <h2>cmbs_api_ie_CalledPartyGet</h2>

      <h3>Introduction</h3>
         Get Called ID Information Element from IE List element.

      <h3>Use cases</h3>
         Received Called ID IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CalledPartyGet ( void * pv_RefIE, ST_IE_CALLEDPARTY * pst_CalledParty )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_CalledParty   pointer to ST_IE_CALLEDPARTY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CalledPartyGet(void *pv_RefIE, ST_IE_CALLEDPARTY *pst_CalledParty);


//*/
//    ==========  cmbs_api_ie_NBOTACodecAdd  ===========
/*!
      \brief
         Add NB OTA Codec to IE List.

         <h2>cmbs_api_ie_NBOTACodecAdd</h2>

      <h3>Introduction</h3>
         Add NB OTA Codec to IE List.

      <h3>Use cases</h3>
         Host offers (or answers with) list of codecs to target, and requests a specific NB codec to be used OTA.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_NBOTACodecAdd ( void * pv_RefIEList, PST_IE_NB_CODEC_OTA pst_Codec )
         </b><br><br>
         The application has to populate a ST_IE_NB_CODEC_OTA structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_Codec         pointer to ST_IE_NB_CODEC_OTA structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_NBOTACodecAdd(void *pv_RefIEList, PST_IE_NB_CODEC_OTA pst_Codec);

//*/
//    ==========  cmbs_api_ie_NBOTACodecGet  ===========
/*!
      \brief
         Get NB OTA Codec from IE List.

         <h2>cmbs_api_ie_NBOTACodecGet</h2>

      <h3>Introduction</h3>
         Get NB OTA Codec from IE List.

      <h3>Use cases</h3>
         Host offers (or answers with) list of codecs to target, and requests a specific NB codec to be used OTA.
         Target extracts Received NB OTA Codec IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_NBOTACodecGet ( void *pv_RefIE, PST_IE_NB_CODEC_OTA pst_Codec )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_Codec         pointer to ST_IE_NB_CODEC_OTA structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_NBOTACodecGet(void *pv_RefIE, PST_IE_NB_CODEC_OTA pst_Codec);



//*/
//    ==========  cmbs_api_ie_TargetListChangeNotifAdd  ===========
/*!
      \brief
         Add Target List Change notif to IE List.

         <h2>cmbs_api_ie_TargetListChangeNotifAdd</h2>

      <h3>Introduction</h3>
         Add Target List Change notif to IE List.

      <h3>Use cases</h3>
         A list access list which is maintained on Target side is modified by PP, Target notifies host about the change.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_TargetListChangeNotifAdd ( void * pv_RefIEList, PST_IE_TARGET_LIST_CHANGE_NOTIF pst_Notif )
         </b><br><br>
         The application has to populate a ST_IE_TARGET_LIST_CHANGE_NOTIF structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_Notif         pointer to ST_IE_TARGET_LIST_CHANGE_NOTIF structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_TargetListChangeNotifAdd(void *pv_RefIEList, PST_IE_TARGET_LIST_CHANGE_NOTIF pst_Notif);

//*/
//    ==========  cmbs_api_ie_TargetListChangeNotifGet  ===========
/*!
      \brief
         Get Target List Change notif from IE List.

         <h2>cmbs_api_ie_TargetListChangeNotifGet</h2>

      <h3>Introduction</h3>
         Get Target List Change notif from IE List.

      <h3>Use cases</h3>
         A list access list which is maintained on Target side is modified by PP, Target notifies host about the change.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_TargetListChangeNotifGet ( void *pv_RefIE, PST_IE_TARGET_LIST_CHANGE_NOTIF pst_Notif )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_Notif         pointer to ST_IE_TARGET_LIST_CHANGE_NOTIF structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_TargetListChangeNotifGet(void *pv_RefIE, PST_IE_TARGET_LIST_CHANGE_NOTIF pst_Notif);

//*/
//    ==========  cmbs_api_ie_CallerNameAdd  ===========
/*!
      \brief
         Add Caller Name to IE List.

         <h2>cmbs_api_ie_CallerNameAdd</h2>

      <h3>Introduction</h3>
         Add Caller Name to IE List.

      <h3>Use cases</h3>
         Transmitting Caller Name by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallerNameAdd ( void * pv_RefIEList, ST_IE_CALLERNAME * pst_CallerName )
         </b><br><br>
         The application has to populate a ST_IE_CALLERNAME structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_CallerName    pointer to ST_IE_CALLERNAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallerNameAdd(void *pv_RefIEList, ST_IE_CALLERNAME *pst_CallerName);

//*/
//    ==========  cmbs_api_ie_CallerNameGet  ===========
/*!
      \brief
         Get Caller Name Information Element from IE List element.

         <h2>cmbs_api_ie_CallerNameGet</h2>

      <h3>Introduction</h3>
         Get Caller Name Information Element from IE List element.

      <h3>Use cases</h3>
         Received Caller Name IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallerNameGet ( void * pv_RefIE, ST_IE_CALLERNAME * pst_CallerName )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_CallerName    pointer to ST_IE_CALLERNAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallerNameGet(void *pv_RefIE, ST_IE_CALLERNAME *pst_CallerName);



//*/
//    ==========  cmbs_api_ie_CalledNameAdd  ===========
/*!
      \brief
         Add Called Name to IE List.

         <h2>cmbs_api_ie_CalledNameAdd</h2>

      <h3>Introduction</h3>
         Add Called Name to IE List.

      <h3>Use cases</h3>
         Transmitting Called Name by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CalledNameAdd ( void * pv_RefIEList, ST_IE_CALLEDNAME * pst_CalledName )
         </b><br><br>
         The application has to populate a ST_IE_CALLEDNAME structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_CalledName    pointer to ST_IE_CALLEDNAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CalledNameAdd(void *pv_RefIEList, ST_IE_CALLEDNAME *pst_CalledName);

//*/
//    ==========  cmbs_api_ie_CalledNameGet  ===========
/*!
      \brief
         Get Called Name Information Element from IE List element.

         <h2>cmbs_api_ie_CalledNameGet</h2>

      <h3>Introduction</h3>
         Get Called Name Information Element from IE List element.

      <h3>Use cases</h3>
         Received Called Name IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CalledNameGet ( void * pv_RefIE, ST_IE_CALLEDNAME * pst_CalledName )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_CalledName    pointer to ST_IE_CALLEDNAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CalledNameGet(void *pv_RefIE, ST_IE_CALLEDNAME *pst_CalledName);




//*/
//    ==========  cmbs_api_ie_CallProgressAdd  ===========
/*!
      \brief
         Add Call Progress information to IE List.

         <h2>cmbs_api_ie_CallProgressAdd</h2>

      <h3>Introduction</h3>
         Add Call Progress information to IE List.

      <h3>Use cases</h3>
         Transmitting Call Progress Information by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallProgressAdd ( void * pv_RefIEList, ST_IE_CALLPROGRESS * pst_CallProgress )
         </b><br><br>
         The application has to populate a ST_IE_CALLPROGRESS structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_CallProgress  pointer to ST_IE_CALLPROGRESS structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallProgressAdd(void *pv_RefIEList, ST_IE_CALLPROGRESS *pst_CallProgress);

//*/
//    ==========  cmbs_api_ie_CallProgressGet  ===========
/*!
      \brief
         Get Call Progress Information Element from IE List element.

         <h2>cmbs_api_ie_CallProgressGet</h2>

      <h3>Introduction</h3>
         Get Call Progress Information Element from IE List element.

      <h3>Use cases</h3>
         Received Call Progress IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallProgressGet ( void * pv_RefIE, ST_IE_CALLPROGRESS * pst_CallProgress )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_CallProgress  pointer to ST_IE_CALLPROGRESS structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallProgressGet(void *pv_RefIE, ST_IE_CALLPROGRESS *pst_CallProgress);

//*/
//    ==========  cmbs_api_ie_CallInfoAdd  ===========
/*!
      \brief
         Add Call Info to IE List.

         <h2>cmbs_api_ie_CallInfoAdd</h2>

      <h3>Introduction</h3>
         Add Call Info to IE List.

      <h3>Use cases</h3>
         Transmitting Call Info by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallInfoAdd ( void * pv_RefIEList, ST_IE_CALLINFO * pst_CallInfo )
         </b><br><br>
         The application has to populate a ST_IE_CALLINFO structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_CallInfo      pointer to ST_IE_CALLINFO structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallInfoAdd(void *pv_RefIEList, ST_IE_CALLINFO *pst_CallInfo);

//*/
//    ==========  cmbs_api_ie_CallInfoGet  ===========
/*!
      \brief
         Get Call Info Information Element from IE List element.

         <h2>cmbs_api_ie_CallInfoGet</h2>

      <h3>Introduction</h3>
         Get Call Info Information Element from IE List element.

      <h3>Use cases</h3>
         Received Call Info IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallInfoGet ( void * pv_RefIE, ST_IE_CALLINFO * pst_CallInfo )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_CallInfo      pointer to ST_IE_CALLINFO structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallInfoGet(void *pv_RefIE, ST_IE_CALLINFO *pst_CallInfo);

//*/
//    ==========  cmbs_api_ie_DisplayStringAdd  ===========
/*!
      \brief
         Add Display string to IE List.

         <h2>cmbs_api_ie_DisplayStringAdd</h2>

      <h3>Introduction</h3>
         Add Display string to IE List.
         The display string will be displayed by the handset MMI, if it supports DECT Display IE.

      <h3>Use cases</h3>
         Transmitting display string by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_DisplayStringAdd ( void * pv_RefIEList, ST_IE_DISPLAY_STRING * pst_DisplayString )
         </b><br><br>
         The application has to populate a ST_IE_DISPLAY_STRING structure first.
         The maximum string size is 30.<br><br>
         NOTE: Cordless handset may not support display message.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_DisplayString pointer to ST_IE_DISPLAY_STRING structure

      \return           Return Code

      \see              CMBS_DISPLAY_STRING_LENGTH

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_DisplayStringAdd(void *pv_RefIEList, ST_IE_DISPLAY_STRING *pst_DisplayString);

//*/
//    ==========  cmbs_api_ie_CallReleaseReasonAdd  ===========
/*!
      \brief
         Add Call Release Reason to IE List.

         <h2>cmbs_api_ie_CallReleaseReasonAdd</h2>

      <h3>Introduction</h3>
         Add Call Release Reason to IE List.

      <h3>Use cases</h3>
         Transmitting Call Release Reason by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallReleaseReasonAdd ( void * pv_RefIEList, ST_IE_RELEASE_REASON * pst_RelReason )
         </b><br><br>
         The application has to populate a ST_IE_RELEASE_REASON structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_RelReason     pointer to ST_IE_RELEASE_REASON structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallReleaseReasonAdd(void *pv_RefIEList, ST_IE_RELEASE_REASON *pst_RelReason);

//*/
//    ==========  cmbs_api_ie_CallReleaseReasonGet  ===========
/*!
      \brief
         Get Call Release Reason Information Element from IE List element.

         <h2>cmbs_api_ie_CallReleaseReasonGet</h2>

      <h3>Introduction</h3>
         Get Call Release Reason Information Element from IE List element.

      <h3>Use cases</h3>
         Received Call Release Reason IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallReleaseReasonGet ( void * pv_RefIE, ST_IE_RELEASE_REASON * pst_RelReason )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_RelReason     pointer to ST_IE_RELEASE_REASON structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallReleaseReasonGet(void *pv_RefIE, ST_IE_RELEASE_REASON *pst_RelReason);

//*/
//    ==========  cmbs_api_ie_CallStateGet  ===========
/*!
      \brief
         Get Call State Information Element from IE List element.

         <h2>cmbs_api_ie_CallStateGet</h2>

      <h3>Introduction</h3>
         Get Call State Information Element from IE List element.

      <h3>Use cases</h3>
         Received Call State IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_CallStateGet( void * pv_RefIE, ST_IE_CALL_STATE * pst_CallState )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_CallState     pointer to ST_IE_CALL_STATE structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_CallStateGet(void *pv_RefIE, ST_IE_CALL_STATE *pst_CallState);


//*/
//    ==========  cmbs_api_ie_MediaChannelAdd  ===========
/*!
      \brief
         Add Media Channel Information Element to IE List.

         <h2>cmbs_api_ie_MediaChannelAdd</h2>

      <h3>Introduction</h3>
         Add Media Channel Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting Media Channel IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MediaChannelAdd ( void * pv_RefIEList, ST_IE_MEDIA_CHANNEL * pst_MediaChannel )
         </b><br><br>
         The application has to populate a ST_IE_MEDIA_CHANNEL structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_MediaChannel  pointer to ST_IE_MEDIA_CHANNEL structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MediaChannelAdd(void *pv_RefIEList, ST_IE_MEDIA_CHANNEL *pst_MediaChannel);

//*/
//    ==========  cmbs_api_ie_MediaChannelGet  ===========
/*!
      \brief
         Get Media Channel Information Element from IE List element.

         <h2>cmbs_api_ie_MediaChannelGet</h2>

      <h3>Introduction</h3>
         Get Media Channel Information Element from IE List element.

      <h3>Use cases</h3>
         Received Media Channel IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MediaChannelGet ( void * pv_RefIE, ST_IE_MEDIA_CHANNEL * pst_MediaChannel )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_MediaChannel  pointer to ST_IE_MEDIA_CHANNEL structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MediaChannelGet(void *pv_RefIE, ST_IE_MEDIA_CHANNEL *pst_MediaChannel);


#ifdef CMBS_MEDIA_ALSA

//*/
//    ==========  cmbs_api_ie_AudioShmemAdd  ===========
/*!
      \brief
         Add Media Channel Shared memory audio extention Information Element to IE List.

         <h2>cmbs_api_ie_AudioShmemAdd</h2>

      <h3>Introduction</h3>
         Add Media Channel Shared memory audio extention Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting Shared memory audio IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_api_ie_AudioShmemAdd( void * pv_RefIEList, ST_IE_AUDIO_SHMEM * pst_ShmemParams )
         </b><br><br>
         The application has to populate a ST_IE_AUDIO_SHMEM structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_ShmemParams  pointer to ST_IE_AUDIO_SHMEM structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_AudioShmemAdd( void * pv_RefIEList, ST_IE_AUDIO_SHMEM * pst_ShmemParams );

//*/
//    ==========  cmbs_api_ie_MediaChannelGet  ===========
/*!
      \brief
         Get Media Channel Shared memory audio extention Information Element from IE List element.

         <h2>cmbs_api_ie_AudioShmemGet</h2>

      <h3>Introduction</h3>
         Get Media Channel Shared memory audio extention Information Element from IE List element.

      <h3>Use cases</h3>
         Received Media Channel Shared memory audio extention IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_AudioShmemGet( void * pv_RefIE, ST_IE_AUDIO_SHMEM * pst_ShmemParams )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_ShmemParams  pointer to ST_IE_AUDIO_SHMEM structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC cmbs_api_ie_AudioShmemGet( void * pv_RefIE, ST_IE_AUDIO_SHMEM * pst_ShmemParams );

#endif // CMBS_MEDIA_ALSA

//*/
//    ==========  cmbs_api_ie_MediaDescAdd  ===========
/*!
      \brief
         Add Media Descriptor Information Element to IE List.

         <h2>cmbs_api_ie_MediaDescAdd</h2>

      <h3>Introduction</h3>
         Add Media Descriptor Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting Media Descriptor IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MediaDescAdd ( void * pv_RefIEList, ST_IE_MEDIA_DESCRIPTOR * pst_MediaDesc )
         </b><br><br>
         The application has to populate a ST_IE_MEDIA_DESCRIPTOR structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_MediaDesc     pointer to ST_IE_MEDIA_DESCRIPTOR structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MediaDescAdd(void *pv_RefIEList, ST_IE_MEDIA_DESCRIPTOR *pst_MediaDesc);



//*/
//    ==========  cmbs_api_ie_MediaICGet  ===========
/*!
      \brief
         Get Media Channel Internal connection Information Element from IE List element.

         <h2>cmbs_api_ie_MediaICGet</h2>

      <h3>Introduction</h3>
         Get Media Channel Internal connection Information Element from IE List element.

      <h3>Use cases</h3>
         Received Media Channel Internal connection IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MediaICGet ( void * pv_RefIEList, ST_IE_MEDIA_INTERNAL_CONNECT * pst_MediaIC );
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_MediaIC       pointer to ST_IE_MEDIA_INTERNAL_CONNECT structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MediaICGet(void *pv_RefIE, ST_IE_MEDIA_INTERNAL_CONNECT *pst_MediaIC);

//*/
//    ==========  cmbs_api_ie_MediaICAdd  ===========
/*!
      \brief
         Add Media Channel Internal Connection Information Element to IE List.

         <h2>cmbs_api_ie_MediaICAdd</h2>

      <h3>Introduction</h3>
         Add Media Channel Internal Connection Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting Media Channel Internal Connection IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MediaICAdd ( void * pv_RefIEList, ST_IE_MEDIA_INTERNAL_CONNECT * pst_MediaIC )
         </b><br><br>
         The application has to populate a ST_IE_MEDIA_INTERNAL_CONNECT structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_MediaIC       pointer to ST_IE_MEDIA_INTERNAL_CONNECT structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MediaICAdd(void *pv_RefIEList, ST_IE_MEDIA_INTERNAL_CONNECT *pst_MediaIC);



//*/
//    ==========  cmbs_api_ie_MediaDescGet  ===========
/*!
      \brief
         Get Media Descriptor Information Element from IE List element.

         <h2>cmbs_api_ie_MediaDescGet</h2>

      <h3>Introduction</h3>
         Get Media Descriptor Information Element from IE List element.

      <h3>Use cases</h3>
         Received Media Descriptor IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_MediaDescGet ( void * pv_RefIE, ST_IE_MEDIA_DESCRIPTOR * pst_MediaDesc )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_MediaDesc     pointer to ST_IE_MEDIA_DESCRIPTOR structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_MediaDescGet(void *pv_RefIE, ST_IE_MEDIA_DESCRIPTOR *pst_MediaDesc);

//*/
//    ==========  cmbs_api_ie_ToneAdd  ===========
/*!
      \brief
         Add Tone Information Element to IE List element.

         <h2>cmbs_api_ie_ToneAdd</h2>

      <h3>Introduction</h3>
         Add Tone Information Element to IE List element.

      <h3>Use cases</h3>
         Used to generate a local tone on the cordless system.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ToneAdd ( void * pv_RefIEList, ST_IE_TONE * pst_Tone )
         </b><br><br>
         The function adds tone properties to the IE list.
         The application has to populate a ST_IE_TONE structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_Tone          pointer to ST_IE_TONE structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
    E_CMBS_RC         cmbs_api_ie_ToneAdd( void * pv_RefIEList, ST_IE_TONE * pst_Tone );


//*/
//    ==========  cmbs_api_ie_ToneVolumeAdd  ===========
/*!
      \brief
         Add Tone Volume Information Element to IE List element.

         <h2>cmbs_api_ie_ToneVolumeAdd</h2>

      <h3>Introduction</h3>
         Add Tone Information Element to IE List element.

      <h3>Use cases</h3>
         Used to generate a local tone on the cordless system.with the vlume given here

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ToneVolumeAdd ( void * pv_RefIEList, u16 u16_TOGVOlume )
         </b><br><br>
         The function adds tone volume properties to the IE list.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        u16_TOGVOlume     Volume according to DSP spec

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_ToneVolumeAdd(void *pv_RefIEList, u16 u16_TOGVOlume);


//*/
//    ==========  cmbs_api_ie_ToneVolumeGet  ===========
/*!
      \brief
         Get TOne Volume Information Element from IE List element.

         <h2>cmbs_api_ie_ToneVolumeGet</h2>

      <h3>Introduction</h3>
         Get CMBS Parameter Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS Parameter IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ToneVolumeGet (void *pv_RefIE, u16 *pu16_TOGVOlume)
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_TOGVOlume     pointer u16 Volume

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_ToneVolumeGet(void *pv_RefIE, u16 *pu16_TOGVOlume);


//*/
//    ==========  cmbs_api_ie_TimeAdd  ===========
/*!
      \brief
         Add Time Of Day Information Element to IE List.

         <h2>cmbs_api_ie_TimeAdd</h2>

      <h3>Introduction</h3>
         Add Time Of Day Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting Time Of Day IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_TimeAdd ( void * pv_RefIEList, ST_IE_TIMEOFDAY * pst_TimeOfDay )
         </b><br><br>
         The application has to populate a ST_IE_TIMEOFDAY structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_TimeOfDay     pointer to ST_IE_TIMEOFDAY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_TimeAdd(void *pv_RefIEList, ST_IE_TIMEOFDAY *pst_TimeOfDay);

//*/
//    ==========  cmbs_api_ie_TimeGet  ===========
/*!
      \brief
         Get Time Of Day Information Element from IE List element.

         <h2>cmbs_api_ie_TimeGet</h2>

      <h3>Introduction</h3>
         Get Time Of Day Information Element from IE List element.

      <h3>Use cases</h3>
         Received Time Of Day IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_TimeGet ( void * pv_RefIE, ST_IE_TIMEOFDAY * pst_TimeOfDay )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_TimeOfDay     pointer to ST_IE_TIMEOFDAY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_TimeGet(void *pv_RefIE, ST_IE_TIMEOFDAY *pst_TimeOfDay);

//*/
//    ==========  cmbs_api_ie_HandsetInfoGet  ===========
/*!
      \brief
         Get Handset Information Element from IE List element.

         <h2>cmbs_api_ie_HandsetInfoGet</h2>

      <h3>Introduction</h3>
         Get Handset Information Element from IE List element.

      <h3>Use cases</h3>
         Received Handset IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_HandsetInfoGet ( void * pv_RefIE, ST_IE_HANDSETINFO * pst_HandsetInfo )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_HandsetInfo   pointer to ST_IE_HANDSETINFO structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_HandsetInfoGet(void *pv_RefIE, ST_IE_HANDSETINFO *pst_HandsetInfo);

//*/
//    ==========  cmbs_api_ie_ParameterGet  ===========
/*!
      \brief
         Get CMBS Parameter Information Element from IE List element.

         <h2>cmbs_api_ie_ParameterGet</h2>

      <h3>Introduction</h3>
         Get CMBS Parameter Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS Parameter IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ParameterGet ( void * pv_RefIE, ST_IE_PARAMETER * pst_Parameter )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_Parameter     pointer to ST_IE_PARAMETER structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_ParameterGet(void *pv_RefIE, ST_IE_PARAMETER *pst_Parameter);

//*/
//    ==========  cmbs_api_ie_ParameterAreaGet  ===========
/*!
      \brief
         Get CMBS Parameter Area Information Element from IE List element.

         <h2>cmbs_api_ie_ParameterGet</h2>

      <h3>Introduction</h3>
         Get CMBS Parameter Area Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS Parameter Area IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ParameterGet ( void * pv_RefIE, ST_IE_PARAMETER_AREA * pst_ParameterArea )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_ParameterArea pointer to ST_IE_PARAMETER_AREA structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_ParameterAreaGet(void *pv_RefIE, ST_IE_PARAMETER_AREA *pst_ParameterArea);

//*/
//    ==========  cmbs_api_ie_SubscribedHSListGet  ===========
/*!
      \brief
         Get subscribed handsets list from IE List element.

         <h2>cmbs_api_ie_SubscribedHSListGet</h2>

      <h3>Introduction</h3>
         Get subscribed handsets list Information Element from IE List element.

      <h3>Use cases</h3>
         Received subscribed handsets list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_SubscribedHSListGet( void * pv_RefIE, ST_IE_SUBSCRIBED_HS_LIST * pst_SubscribedHsList )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_SubscribedHsList pointer to ST_IE_SUBSCRIBED_HS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_SubscribedHSListGet(void *pv_RefIE, ST_IE_SUBSCRIBED_HS_LIST *pst_SubscribedHsList);

//*/
//    ==========  cmbs_api_ie_SubscribedHSListAdd  ===========
/*!
      \brief
         Get subscribed handsets list from IE List element.

         <h2>cmbs_api_ie_SubscribedHSListAdd</h2>

      <h3>Introduction</h3>
         Get subscribed handsets list Information Element from IE List element.

      <h3>Use cases</h3>
         Received subscribed handsets list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_SubscribedHSListAdd( void * pv_RefIE, ST_IE_SUBSCRIBED_HS_LIST * pst_SubscribedHsList )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_SubscribedHsList pointer to ST_IE_SUBSCRIBED_HS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_SubscribedHSListAdd(void *pv_RefIE, ST_IE_SUBSCRIBED_HS_LIST *pst_SubscribedHsList);


//*/
//    ==========  cmbs_api_ie_LAPropCmdGet  ===========
/*!
      \brief
         Get List Access Proprietary Command from IE List element.

         <h2>cmbs_api_ie_LAPropCmdGet</h2>

      <h3>Introduction</h3>
         Get List Access Proprietary Command from IE List element.

      <h3>Use cases</h3>
         PP Performs List Access proprietary activity.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAPropCmdGet( void * pv_RefIE, ST_LA_PROP_CMD * pst_Cmd )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE            pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_Cmd             pointer to ST_LA_PROP_CMD structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_LAPropCmdGet(void *pv_RefIE, ST_LA_PROP_CMD *pst_Cmd);

//*/
//    ==========  cmbs_api_ie_LAPropCmdAdd  ===========
/*!
      \brief
         Add List Access Proprietary Command to IE List element.

         <h2>cmbs_api_ie_LAPropCmdAdd</h2>

      <h3>Introduction</h3>
         Add List Access Proprietary Command to IE List element.

      <h3>Use cases</h3>
         PP Performs List Access proprietary activity.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAPropCmdAdd( void * pv_RefIE, ST_LA_PROP_CMD * pst_Cmd )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_Cmd              pointer to ST_LA_PROP_CMD structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_LAPropCmdAdd(void *pv_RefIE, ST_LA_PROP_CMD *pst_Cmd);


//*/
//    ==========  cmbs_api_ie_LineSettingsListGet  ===========
/*!
      \brief
         Get line settings list from IE List element.

         <h2>cmbs_api_ie_LineSettingsListGet</h2>

      <h3>Introduction</h3>
         Get line settings list Information Element from IE List element.

      <h3>Use cases</h3>
         Received line settings list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_api_ie_LineSettingsListGet( void * pv_RefIE, ST_IE_LINE_SETTINGS_LIST * pst_LineSettingsList )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LineSettingsList pointer to ST_IE_LINE_SETTINGS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LineSettingsListGet(void *pv_RefIE, ST_IE_LINE_SETTINGS_LIST *pst_LineSettingsList);

//*/
//    ==========  cmbs_api_ie_LineSettingsListAdd  ===========
/*!
      \brief
         Set line settings list from IE List element.

         <h2>cmbs_api_ie_LineSettingsListAdd</h2>

      <h3>Introduction</h3>
         Set line settings list Information Element from IE List element.

      <h3>Use cases</h3>
         Added line settings list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_api_ie_LineSettingsListAdd( void * pv_RefIE, ST_IE_LINE_SETTINGS_LIST * pst_LineSettingsList )
         </b><br><br>
         The function add the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LineSettingsList pointer to ST_IE_LINE_SETTINGS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LineSettingsListAdd(void *pv_RefIE, ST_IE_LINE_SETTINGS_LIST *pst_LineSettingsList);


//*/
//    ==========  cmbs_api_ie_DectSettingsListGet  ===========
/*!
      \brief
         Get line settings list from IE List element.

         <h2>cmbs_api_ie_DectSettingsListGet</h2>

      <h3>Introduction</h3>
         Get line settings list Information Element from IE List element.

      <h3>Use cases</h3>
         Received line settings list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_api_ie_DectSettingsListGet( void * pv_RefIE, ST_IE_DECT_SETTINGS_LIST * pst_DectSettings )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_DectSettings pointer to ST_IE_DECT_SETTINGS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_DectSettingsListGet(void *pv_RefIE, ST_IE_DECT_SETTINGS_LIST *pst_DectSettings);

//*/
//    ==========  cmbs_api_ie_DectSettingsListAdd  ===========
/*!
      \brief
         Set line settings list from IE List element.

         <h2>cmbs_api_ie_DectSettingsListAdd</h2>

      <h3>Introduction</h3>
         Set DECT settings list Information Element from IE List element.

      <h3>Use cases</h3>
         Added DECT settings list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_api_ie_DectSettingsListAdd( void * pv_RefIE, ST_IE_DECT_SETTINGS_LIST * pst_DectSettings )
         </b><br><br>
         The function add the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_DectSettings pointer to ST_IE_DECT_SETTINGS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_DectSettingsListAdd(void *pv_RefIE, ST_IE_DECT_SETTINGS_LIST *pst_DectSettings);


//*/
//    ==========  cmbs_api_ie_PropDectSettingsListGet  ===========
/*!
      \brief
         Get line settings list from IE List element.

         <h2>cmbs_api_ie_PropDectSettingsListGet</h2>

      <h3>Introduction</h3>
         Get line settings list Information Element from IE List element.

      <h3>Use cases</h3>
         Received line settings list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_PropDectSettingsListGet(void *pv_RefIE, ST_IE_PROP_FIELDS_LIST *pst_PropSettings)

         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_DectSettings pointer to ST_IE_DECT_SETTINGS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_PropDectSettingsListGet(void *pv_RefIE, ST_IE_PROP_FIELDS_LIST *pst_PropSettings);

//*/
//    ==========  cmbs_api_ie_PropDectSettingsListAdd  ===========
/*!
      \brief
         Set proprietary elements of DECT settings list from IE List element.

         <h2>cmbs_api_ie_PropDectSettingsListAdd</h2>

      <h3>Introduction</h3>
         Set proprietary elements settings list Information Element from IE List element.

      <h3>Use cases</h3>
         Added proprietary elements settings list IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_api_ie_PropDectSettingsListAdd(void *pv_RefIEList, ST_IE_PROP_FIELDS_LIST *pst_PropSettings)
         </b><br><br>
         The function add the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_DectSettings pointer to ST_IE_DECT_SETTINGS_LIST structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_PropDectSettingsListAdd(void *pv_RefIEList, ST_IE_PROP_FIELDS_LIST *pst_PropSettings);


//*/
//    ==========  cmbs_api_ie_FwVersionGet  ===========
/*!
      \brief
         Get CMBS Firmware Version Information Element from IE List element.

         <h2>cmbs_api_ie_FwVersionGet</h2>

      <h3>Introduction</h3>
         Get CMBS Firmware Version Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS Firmware Version IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_FwVersionGet ( void * pv_RefIE, ST_IE_FW_VERSION * pst_FwVersion )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_FwVersion     pointer to ST_IE_FW_VERSION structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_FwVersionGet(void *pv_RefIE, ST_IE_FW_VERSION *pst_FwVersion);


//*/
//    ==========  cmbs_api_ie_HwVersionGet  ===========
/*!
      \brief
         Get CMBS Hardware Version Information Element from IE List element.

         <h2>cmbs_api_ie_HwVersionGet</h2>

      <h3>Introduction</h3>
         Get CMBS Hardware Version Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS Hardware Version IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_HwVersionGet ( void * pv_RefIE, ST_IE_HW_VERSION * pst_HwVersion )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_FwVersion     pointer to ST_IE_HW_VERSION structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_HwVersionGet(void *pv_RefIE, ST_IE_HW_VERSION *pst_HwVersion);
//*/
//    ==========  cmbs_api_ie_EEPROMVersionGet  ===========
/*!
      \brief
         Get CMBS EEPROM Version Information Element from IE List element.

         <h2>cmbs_api_ie_EEPROMVersionGet</h2>

      <h3>Introduction</h3>
         Get CMBS EEPROM Version Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS EEPROM Version IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_EEPROMVersionGet ( void * pv_RefIE, ST_IE_EEPROM_VERSION * pst_EEPROMVersion )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_FwVersion     pointer to ST_IE_EEPROM_VERSION structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC	cmbs_api_ie_EEPROMVersionGet(void *pv_RefIE, ST_IE_EEPROM_VERSION *pst_EEPROMVersion);

//*/
//    ==========  cmbs_api_ie_SysLogGet  ===========
/*!
      \brief
         Get CMBS System Logging Information Element from IE List element.

         <h2>cmbs_api_ie_SysLogGet</h2>

      <h3>Introduction</h3>
         Get CMBS System Logging Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS System Logging IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_SysLogGet ( void * pv_RefIE, ST_IE_SYS_LOG * pst_SysLog )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_SysLog        pointer to ST_IE_SYS_LOG structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_SysLogGet(void *pv_RefIE, ST_IE_SYS_LOG *pst_SysLog);

//*/
//    ==========  cmbs_api_ie_SysStatusGet  ===========
/*!
      \brief
         Get CMBS System Information Element from IE List element.

         <h2>cmbs_api_ie_SysStatusGet</h2>

      <h3>Introduction</h3>
         Get CMBS System Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS System status from the module in the Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_SysStatusGet ( void * pv_RefIE, ST_IE_SYS_STATUS * pst_SysStatus )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_SysStatus     pointer to ST_IE_SYS_STATUS structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_SysStatusGet(void *pv_RefIE, ST_IE_SYS_STATUS *pst_SysStatus);

//*/
//    ==========  cmbs_api_ie_ResponseGet  ===========
/*!
      \brief
         Get CMBS Response Information Element from IE List element.

         <h2>cmbs_api_ie_ResponseGet</h2>

      <h3>Introduction</h3>
         Get Response Information Element from IE List element.

      <h3>Use cases</h3>
         Received CMBS Response IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ResponseGet ( void * pv_RefIE, ST_IE_RESPONSE * pst_Response )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_Response      pointer to ST_IE_RESPONSE structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_ResponseGet(void *pv_RefIE, ST_IE_RESPONSE *pst_Response);
E_CMBS_RC         cmbs_api_ie_ResponseAdd(void *pv_RefIE, ST_IE_RESPONSE *pst_Response);

//*/
//    ==========  cmbs_api_ie_RequestIdGet  ===========
/*!
      \brief
         Get Request Id Information Element from IE List element.

         <h2>cmbs_api_ie_RequestIdGet</h2>

      <h3>Introduction</h3>
         Get Request Id Information Element from IE List element.

      <h3>Use cases</h3>
         Received Request Id IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RequestIdGet ( void * pv_RefIE, u16* pu16_RequestId )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_RequestId    Request Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_RequestIdGet(void *pv_RefIE, u16 *pu16_RequestId);

//*/
//    ==========  cmbs_api_ie_HandsetsGet  ===========
/*!
      \brief
         Get handsets Information Element from IE List element.

         <h2>cmbs_api_ie_HandsetsGet</h2>

      <h3>Introduction</h3>
         Get handsets Information Element from IE List element.

      <h3>Use cases</h3>
         Received Handset IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_HandsetsGet ( void * pv_RefIE, u16* pu16_Handsets )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_Handsets     bitmask for handsets : bit 0 = handset 1,<br>
                                          bit 1 = handset 2, etc. 0xFFFF for all

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_HandsetsGet(void *pv_RefIE, u16 *pu16_Handsets);


//*/
//    ==========  cmbs_api_ie_HandsetsAdd  ===========
/*!
      \brief
         Get handsets Information Element from IE List element.

         <h2>cmbs_api_ie_HandsetsAdd</h2>

      <h3>Introduction</h3>
         Set handsets Information Element from IE List element.

      <h3>Use cases</h3>
         Sending Handset IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_HandsetsAdd ( void * pv_RefIE, u16  u16_Handsets )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in]    u8_Handsets     bitmask for handsets : bit 0 = handset 1,<br>
                                          bit 1 = handset 2, etc. 0xFFFF for all

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_HandsetsAdd(void *pv_RefIE, u16  u16_Handsets);


//*/
//    ==========  cmbs_api_ie_DateTimeGet  ===========
/*!
      \brief
         Get Date & Time Information Element from IE List element.

         <h2>cmbs_api_ie_DateTimeGet</h2>

      <h3>Introduction</h3>
         Get Date & Time Information Element from IE List element.

      <h3>Use cases</h3>
         Received Date and Time IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_DateTimeGet ( void * pv_RefIE, ST_IE_DATETIME * pst_DateTime )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_DateTime      DateTime (Y/M/D/H/M/S/Zone)

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_DateTimeGet(void *pv_RefIE, ST_IE_DATETIME *pst_DateTime);

//*/
//    ==========  cmbs_api_ie_DataGet  ===========
/*!
      \brief
         Get Data Information Element from IE List element.

         <h2>cmbs_api_ie_DataGet</h2>

      <h3>Introduction</h3>
         Get Data Information Element from IE List element.

      <h3>Use cases</h3>
         Received Data IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_DataGet ( void * pv_RefIE, ST_IE_DATA * pst_Data )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_Data          holds Data and length

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_DataGet(void *pv_RefIE, ST_IE_DATA *pst_Data);

//*/
//    ==========  cmbs_api_ie_DataSessionIdGet  ===========
/*!
      \brief
         Get Data Session Id Information Element from IE List element.

         <h2>cmbs_api_ie_DataSessionIdGet</h2>

      <h3>Introduction</h3>
         Get Data Session Id Information Element from IE List element.

      <h3>Use cases</h3>
         Received Data Session Id IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_DataSessionIdGet ( void * pv_RefIE, u16 * pu16_DataSessionId )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_DataSessionId   the Data Session Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_DataSessionIdGet(void *pv_RefIE, u16 *pu16_DataSessionId);

//*/
//    ==========  cmbs_api_ie_DataSessionTypeGet  ===========
/*!
      \brief
         Get Data Session Type Information Element from IE List element.

         <h2>cmbs_api_ie_DataSessionTypeGet</h2>

      <h3>Introduction</h3>
         Get Data Session Type Information Element from IE List element.

      <h3>Use cases</h3>
         Received Data Session Type IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_DataSessionTypeGet ( void * pv_RefIE, ST_IE_DATA_SESSION_TYPE * pst_DataSessionType )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_DataSessionType  holds Data Session Type (IWU-INFO, CF-Channel,etc.)<br>
                                             Note: Currently only IWU-INFO is supported

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_DataSessionTypeGet(void *pv_RefIE, ST_IE_DATA_SESSION_TYPE *pst_DataSessionType);

//*/
//    ==========  cmbs_api_ie_LASessionIdGet  ===========
/*!
      \brief
         Get List Access Session Id Information Element from IE List element.

         <h2>cmbs_api_ie_LASessionIdGet</h2>

      <h3>Introduction</h3>
         Get List Access Session Id Information Element from IE List element.

      <h3>Use cases</h3>
         Received List Access Session Id IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LASessionIdGet ( void * pv_RefIE, u16 * pu16_LASessionId )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_LASessionId  pointer to the List Access Session Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LASessionIdGet(void *pv_RefIE, u16 *pu16_LASessionId);


//*/
//    ==========  cmbs_api_ie_LAListIdGet  ===========
/*!
      \brief
         Get List Id Information Element from IE List element.

         <h2>cmbs_api_ie_LAListIdGet</h2>

      <h3>Introduction</h3>
         Get List Id Information Element from IE List element.

      <h3>Use cases</h3>
         Received List Id IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAListIdGet ( void * pv_RefIE, u8 * pu8_LAListId )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu8_LAListId      the List Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LAListIdGet(void *pv_RefIE, u16 *pu16_LAListId);


//*/
//    ==========  cmbs_api_ie_LAListIdAdd  ===========
/*!
      \brief
         Adds List Id Information Element to IE List.

         <h2>cmbs_api_ie_LAListIdAdd</h2>

      <h3>Introduction</h3>
         Adds List Id Information Element to IE List.

      <h3>Use cases</h3>
         Set List Id IE in an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAListIdAdd ( void * pv_RefIE, u8 * pu8_LAListId )
         </b><br><br>
         The function sets the IE data into the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in]        u8_LAListId      the List Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LAListIdAdd(void *pv_RefIE, u16 u16_LAListId);

//*/
//    ==========  cmbs_api_ie_LAFieldsGet  ===========
/*!
      \brief
         Get List Access Fields Information Element from IE List element.

         <h2>cmbs_api_ie_LAFieldsGet</h2>

      <h3>Introduction</h3>
         Get List Access Fields Information Element from IE List element.

      <h3>Use cases</h3>
         Received List Access Fields IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAFieldsGet ( void * pv_RefIE, ST_IE_LA_FIELDS * pst_LAFields )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LAFields      holds List Access fields

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LAFieldsGet(void *pv_RefIE, ST_IE_LA_FIELDS *pst_LAFields);

//*/
//    ==========  cmbs_api_ie_LASearchCriteriaGet  ===========
/*!
      \brief
         Get Search Criteria Information Element from IE List element.

         <h2>cmbs_api_ie_LASearchCriteriaGet</h2>

      <h3>Introduction</h3>
         Get Search Criteria Information Element from IE List element.

      <h3>Use cases</h3>
         Received Search Criteria IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LASearchCriteriaGet ( void * pv_RefIE, ST_IE_LA_SEARCH_CRITERIA * pst_LASearchCriteria )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LASearchCriteria one of three:<br>
                                             1.Exact Match<br>
                                             2.Exact Match, return next if fail<br>
                                             3.Exact Match, return previous if fail<br>
                                             And also a flag for Case Sensitive (Yes/No)

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LASearchCriteriaGet(void *pv_RefIE, ST_IE_LA_SEARCH_CRITERIA *pst_LASearchCriteria);

//*/
//    ==========  cmbs_api_ie_LAEntryIdGet  ===========
/*!
      \brief
         Get Entry Id Information Element from IE List element.

         <h2>cmbs_api_ie_LAEntryIdGet</h2>

      <h3>Introduction</h3>
         Get Entry Id Information Element from IE List element.

      <h3>Use cases</h3>
         Received Entry Id IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAEntryIdGet ( void * pv_RefIE, u16 * pu16_LAEntryId )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_LAEntryId    the Entry Id

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LAEntryIdGet(void *pv_RefIE, u16 *pu16_LAEntryId);

//*/
//    ==========  cmbs_api_ie_LAEntryIndexGet  ===========
/*!
      \brief
         Get Entry Index Information Element from IE List element.

         <h2>cmbs_api_ie_LAEntryIndexGet</h2>

      <h3>Introduction</h3>
         Get Entry Index Information Element from IE List element.

      <h3>Use cases</h3>
         Received Entry Index IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAEntryIndexGet ( void * pv_RefIE, u16 * pu16_LAEntryIndex )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_LAEntryIndex pointer to the Entry Index

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LAEntryIndexGet(void *pv_RefIE, u16 *pu16_LAEntryIndex);

//*/
//    ==========  cmbs_api_ie_LAEntryCountGet  ===========
/*!
      \brief
         Get Entry Count Information Element from IE List element.

         <h2>cmbs_api_ie_LAEntryCountGet</h2>

      <h3>Introduction</h3>
         Get Entry Count Information Element from IE List element.

      <h3>Use cases</h3>
         Received Entry Count IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAEntryCountGet ( void * pv_RefIE, u16 * pu16_LAEntryCount )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu16_LAEntryCount pointer to the Entry Count variable

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LAEntryCountGet(void *pv_RefIE, u16 *pu16_LAEntryCount);

//*/
//    ==========  cmbs_api_ie_LAIsLastGet  ===========
/*!
      \brief
         Get Is Last Information Element from IE List element.

         <h2>cmbs_api_ie_LAIsLastGet</h2>

      <h3>Introduction</h3>
         Get Is Last Information Element from IE List element.

      <h3>Use cases</h3>
         Received Is Last IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_LAIsLastGet ( void * pv_RefIE, u8 * pu8_LAIsLast )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE       pointer to IE list element returned by<br>
                                       cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu8_LAIsLast   pointer to IsLast variable. Values: 0 if not last, 1 if last

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_api_ie_LAIsLastGet(void *pv_RefIE, u8 *pu8_LAIsLast);

//*/
//    ==========  cmbs_api_ie_LAIsLastGet  ===========
E_CMBS_RC         cmbs_api_ie_HsNumberAdd(void *pv_RefIEList, u8 u8_HsNumber);


//*/
//    ==========  cmbs_api_ie_RTPSessionInformationAdd  ===========
/*!
      \brief
         Add RTP Session Information to IE List.

         <h2>cmbs_api_ie_RTPSessionInformationAdd</h2>

      <h3>Introduction</h3>
         Add RTP Session Information to IE List.

      <h3>Use cases</h3>
         Transmitting RTP Session Information by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPSessionInformationAdd ( void * pv_RefIEList, const ST_IE_RTP_SESSION_INFORMATION * pst_RTPSessionInformation );
         </b><br><br>
         The application has to populate a ST_IE_RTP_SESSION_INFORMATION structure first.

      \param[in]        pv_RefIEList                pointer to IE List
      \param[in]        pst_RTPSessionInformation   pointer to ST_IE_RTP_SESSION_INFORMATION structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPSessionInformationAdd(void *pv_RefIEList, const ST_IE_RTP_SESSION_INFORMATION *pst_RTPSessionInformation);

//*/
//    ==========  cmbs_api_ie_RTPSessionInformationGet  ===========
/*!
      \brief
         Get RTP Session Information from IE List element.

         <h2>cmbs_api_ie_RTPSessionInformationGet</h2>

      <h3>Introduction</h3>
         Get RTP Session Information from IE List element.

      <h3>Use cases</h3>
         Received RTP Session Information IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPSessionInformationGet ( void * pv_RefIE, ST_IE_RTP_SESSION_INFORMATION * pst_RTPSessionInformation );
         </b><br><br>
         The function extracts the IE data from the List element and populates a ST_IE_RTP_SESSION_INFORMATION structure.<br>

      \param[in]        pv_RefIE                    pointer to IE list element returned by<br>
                                                    cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_RTPSessionInformation   pointer to ST_IE_RTP_SESSION_INFORMATION structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPSessionInformationGet(void *pv_RefIE, ST_IE_RTP_SESSION_INFORMATION *pst_RTPSessionInformation);

//*/
//    ==========  cmbs_api_ie_RTCPIntervalAdd  ===========
/*!
      \brief
         Add RTCP Interval value to IE List.

         <h2>cmbs_api_ie_RTCPIntervalAdd</h2>

      <h3>Introduction</h3>
         Add RTCP Interval value to IE List.

      <h3>Use cases</h3>
         Transmitting RTCP Interval value by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTCPIntervalAdd ( void * pv_RefIEList, u32 u32_RTCPInterval );
         </b><br><br>

      \param[in]        pv_RefIEList        pointer to IE List
      \param[in]        u32_RTCPInterval    value of RTCP Interval

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTCPIntervalAdd(void *pv_RefIEList, u32 u32_RTCPInterval);

//*/
//    ==========  cmbs_api_ie_RTCPIntervalGet  ===========
/*!
      \brief
         Get RTCP Interval value from IE List element.

         <h2>cmbs_api_ie_RTCPIntervalGet</h2>

      <h3>Introduction</h3>
         Get RTCP Interval value from IE List element.

      <h3>Use cases</h3>
         Received RTCP Interval IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTCPIntervalGet ( void * pv_RefIE, u32 * pu32_RTCPInterval );
         </b><br><br>
         The function extracts the IE data from the List element and populates a pst_RTPSessionInformation.<br>

      \param[in]        pv_RefIE            pointer to IE list element returned by<br>
                                            cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pu32_RTCPInterval   pointer to u32 variable to store RTCP Interval

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTCPIntervalGet(void *pv_RefIE, u32 *pu32_RTCPInterval);

//*/
//    ==========  cmbs_api_ie_RTPDTMFEventAdd  ===========
/*!
      \brief
         Add RTP DTMF Event to IE List.

         <h2>cmbs_api_ie_RTPDTMFEventAdd</h2>

      <h3>Introduction</h3>
         Add RTP DTMF Event to IE List.

      <h3>Use cases</h3>
         Transmitting RTP DTMF Event by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPDTMFEventAdd ( void * pv_RefIEList, const ST_IE_RTP_DTMF_EVENT * pst_RTPDTMFEvent );
         </b><br><br>
         The application has to populate a ST_IE_RTP_DTMF_EVENT structure first.

      \param[in]        pv_RefIEList        pointer to IE List
      \param[in]        pst_RTPDTMFEvent    pointer to ST_IE_RTP_DTMF_EVENT structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPDTMFEventAdd(void *pv_RefIEList, const ST_IE_RTP_DTMF_EVENT *pst_RTPDTMFEvent);

//*/
//    ==========  cmbs_api_ie_RTPDTMFEventGet  ===========
/*!
      \brief
         Get RTP DTMF Event from IE List element.

         <h2>cmbs_api_ie_RTPDTMFEventGet</h2>

      <h3>Introduction</h3>
         Get RTP DTMF Event from IE List element.

      <h3>Use cases</h3>
         Received RTP DTMF Event IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPDTMFEventGet ( void * pv_RefIE, ST_IE_RTP_DTMF_EVENT * pst_RTPDTMFEvent );
         </b><br><br>
         The function extracts the IE data from the List element and populates a ST_IE_RTP_DTMF_EVENT.<br>

      \param[in]        pv_RefIE            pointer to IE list element returned by<br>
                                            cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_RTPDTMFEvent    pointer to ST_IE_RTP_DTMF_EVENT structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPDTMFEventGet(void *pv_RefIE, ST_IE_RTP_DTMF_EVENT *pst_RTPDTMFEvent);

//*/
//    ==========  cmbs_api_ie_RTPDTMFEventInfoAdd  ===========
/*!
      \brief
         Add RTP DTMF Event Extended Information to IE List.

         <h2>cmbs_api_ie_RTPDTMFEventInfoAdd</h2>

      <h3>Introduction</h3>
         Add RTP DTMF Event Extended Information to IE List.

      <h3>Use cases</h3>
         Transmitting RTP DTMF Event Extended Information by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPDTMFEventInfoAdd ( void * pv_RefIEList, const ST_IE_RTP_DTMF_EVENT_INFO * pst_RTPDTMFEventInfo );
         </b><br><br>
         The application has to populate a ST_IE_RTP_DTMF_EVENT_INFO structure first.

      \param[in]        pv_RefIEList            pointer to IE List
      \param[in]        pst_RTPDTMFEventInfo    pointer to ST_IE_RTP_DTMF_EVENT_INFO structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPDTMFEventInfoAdd(void *pv_RefIEList, const ST_IE_RTP_DTMF_EVENT_INFO *pst_RTPDTMFEventInfo);

//*/
//    ==========  cmbs_api_ie_RTPDTMFEventInfoGet  ===========
/*!
      \brief
         Get RTP DTMF Event Extended Information from IE List element.

         <h2>cmbs_api_ie_RTPDTMFEventInfoGet</h2>

      <h3>Introduction</h3>
         Get RTP DTMF Event Extended Information from IE List element.

      <h3>Use cases</h3>
         Received RTP DTMF Event Extended Information IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPDTMFEventInfoGet ( void * pv_RefIE, ST_IE_RTP_DTMF_EVENT_INFO * pst_RTPDTMFEventInfo );
         </b><br><br>
         The function extracts the IE data from the List element and populates a ST_IE_RTP_DTMF_EVENT_INFO.<br>

      \param[in]        pv_RefIE                pointer to IE list element returned by<br>
                                                cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_RTPDTMFEventInfo    pointer to ST_IE_RTP_DTMF_EVENT_INFO structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPDTMFEventInfoGet(void *pv_RefIE, ST_IE_RTP_DTMF_EVENT_INFO *pst_RTPDTMFEventInfo);

//*/
//    ==========  cmbs_api_ie_RTPFaxToneTypeAdd  ===========
/*!
      \brief
         Add Fax Tone Type to IE List.

         <h2>cmbs_api_ie_RTPFaxToneTypeAdd</h2>

      <h3>Introduction</h3>
         Add Fax Tone Type to IE List.

      <h3>Use cases</h3>
         Transmitting Fax Tone Type by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPFaxToneTypeAdd ( void * pv_RefIEList, E_CMBS_FAX_TONE_TYPE e_FaxToneType );
         </b><br><br>

      \param[in]        pv_RefIEList            pointer to IE List
      \param[in]        e_FaxToneType           Fax Tone Type value

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPFaxToneTypeAdd(void *pv_RefIEList, E_CMBS_FAX_TONE_TYPE e_FaxToneType);

//*/
//    ==========  cmbs_api_ie_RTPFaxToneTypeGet  ===========
/*!
      \brief
         Get Fax Tone Type from IE List element.

         <h2>cmbs_api_ie_RTPFaxToneTypeGet</h2>

      <h3>Introduction</h3>
         Get Fax Tone Type from IE List element.

      <h3>Use cases</h3>
         Received Fax Tone Type IE from an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RTPFaxToneTypeGet ( void * pv_RefIEList, E_CMBS_FAX_TONE_TYPE * pe_FaxToneType );
         </b><br><br>

      \param[in]        pv_RefIE                pointer to IE list element returned by<br>
                                                cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pe_FaxToneType          pointer to variable to store received Fax Tone Type

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/

E_CMBS_RC           cmbs_api_ie_RTPFaxToneTypeGet(void *pv_RefIEList, E_CMBS_FAX_TONE_TYPE *pe_FaxToneType);

//*/
//    ==========  cmbs_api_ie_BaseNameAdd  ===========
/*!
      \brief
         Set Base Name from IE List element.

         <h2>cmbs_api_ie_BaseNameAdd</h2>

      <h3>Introduction</h3>
         Set lBase Name Information Element from IE List element.

      <h3>Use cases</h3>
         Set Base Name IE in Information Elements List.

      <h3>API Functions description</h3>
         <b>
          E_CMBS_RC cmbs_api_ie_BaseNameAdd(void* pv_RefIE, ST_IE_BASE_NAME* pst_BaseName );
         </b><br><br>
         The function adds the Base Name data to IE.<br>
  
      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_BaseName pointer to ST_IE_BASE_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_BaseNameAdd(void *pv_RefIE, ST_IE_BASE_NAME *pst_BaseName);


//*/
//    ==========  cmbs_api_ie_BaseNameGet  ===========
/*!
      \brief
         Get Base Name from IE List element.

         <h2>cmbs_api_ie_BaseNameGet</h2>

      <h3>Introduction</h3>
         Get Base Name Information Element from IE List element.

      <h3>Use cases</h3>
         Get Base Name IE from Information Elements List.

      <h3>API Functions description</h3>
         <b>
E_CMBS_RC cmbs_api_ie_BaseNameGet(void* pv_RefIE, ST_IE_BASE_NAME* pst_BaseName );
         </b><br><br>
         The function gets the Base Name data from IE.<br>
  
      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_BaseName pointer to ST_IE_BASE_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_api_ie_BaseNameGet(void *pv_RefIE, ST_IE_BASE_NAME *pst_BaseName);



/*****************************************************************************
 * DSR (service) functions
 *****************************************************************************/

//*/
//    ==========  cmbs_dsr_sys_Start  ===========
/*!
      \brief
         Start CMBS on the target.

         <h2>cmbs_dsr_sys_Start</h2>

      <h3>Introduction</h3>
         Start CMBS on the target.

      <h3>Use cases</h3>
        Startup sequence of the application.

      <h3>API Functions description</h3>
         <b>
         ECMBS_ERROR cmbs_dsr_sys_Start ( void * pv_AppRefHandle )
         </b><br><br>
         On startup of the target, the host has to configure some parameters for the CMBS.<br>
         The target's CMBS is waiting in its boot sequence for all necessary parameters.<br>
         This function sends a CMBS_EV_DSR_SYS_START event to the target
         after CMBS parameters were set.<br><br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]		p_SYPOSpecification pointer to SYPO specification parameters

      \return           Return code

      \see              CMBS_EV_DSR_SYS_START_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_Start(void *pv_AppRefHandle, ST_IE_SYPO_SPECIFICATION *p_SYPOSpecification);

//*/
//    ==========  cmbs_dsr_SYSReset  ===========
/*!
      \brief
         Resets the CMBS module.

         <h2>cmbs_dsr_SYSReset</h2>

      <h3>Introduction</h3>
         Resets the CMBS module

      <h3>Use cases</h3>
         System reboot.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_SYSReset ( void * pv_AppRefHandle )
         </b><br><br>
         This function requests the base station to perform a system reboot.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_Reset(void *pv_AppRefHandle);

/* == ALTDV == */

//*/
//    ==========  cmbs_dsr_sys_PowerOff  ===========
/*!
      \brief
         Power off the CMBS module.

         <h2>cmbs_dsr_sys_PowerOff</h2>

      <h3>Introduction</h3>
         Power off the CMBS module

      <h3>Use cases</h3>
         System power off.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_sys_PowerOff ( void * pv_AppRefHandle )
         </b><br><br>
         This function requests the base station to perform a system switch to suspend mode.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_PowerOff(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_GET_InternalnameList  ===========
/*!
      \brief
         Request information about subcribed handsets.

         <h2>cmbs_dsr_GET_InternalnameList</h2>

      <h3>Introduction</h3>
         List of subcribed handsets

      <h3>Use cases</h3>
         Get list of subcribed handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_GET_InternalnameList( void * pv_AppRefHandle, u16 u16_HsMask  )
         </b><br><br>
         This function requests the base station to get information about list of registered handsets.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u16_HsMask        Mask of handsets (for all use 0xFF)
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_GET_InternalnameList(void *pv_AppRefHandle, u16 u16_HsMask);


//*/
//    ==========  cmbs_dsr_SET_InternalnameList  ===========
/*!
      \brief
         Set new name for subcribed handset.

         <h2>cmbs_dsr_SET_InternalnameList</h2>

      <h3>Introduction</h3>
         Set new name for subcribed handset.

      <h3>Use cases</h3>
         Set new name for subcribed handset.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_SET_InternalnameList( void * pv_AppRefHandle, ST_IE_SUBSCRIBED_HS_LIST * pst_SubscribedHsList )
         </b><br><br>
         This function requests the base station to get information about list of registered handsets.
        <br>
      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pst_SubscribedHsList    pointer to ST_IE_SUBSCRIBED_HS_LIST structure
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_SET_InternalnameList(void *pv_AppRefHandle, ST_IE_SUBSCRIBED_HS_LIST *pst_SubscribedHsList);

//*/
//    ==========  cmbs_dsr_SET_InternalnamePropList  ===========
/*!
      \brief
         Set new name for subcribed handset including proprietary fields.

         <h2>cmbs_dsr_SET_InternalnamePropList</h2>

      <h3>Introduction</h3>
         Set new name for subcribed handset including proprietary fields.

      <h3>Use cases</h3>
         Set new name for subcribed handset including proprietary fields.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_SET_InternalnamePropList( void * pv_AppRefHandle, ST_IE_SUBSCRIBED_HS_LIST * pst_SubscribedHsList, ST_IE_PROP_FIELDS_LIST * pst_PropFields )
         </b><br><br>
         This function requests the base station to get information about list of registered handsets.
        <br>
      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pst_SubscribedHsList    pointer to ST_IE_SUBSCRIBED_HS_LIST structure
      \param[in]        pst_PropFields          pointer to ST_IE_PROP_FIELDS_LIST structure
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_SET_InternalnamePropList( void * pv_AppRefHandle, ST_IE_SUBSCRIBED_HS_LIST * pst_SubscribedHsList, ST_IE_PROP_FIELDS_LIST * pst_PropFields);


//*/
//    ==========  cmbs_dsr_GET_Line_setting_list  ===========
/*!
      \brief
         Request information about line settings.

         <h2>cmbs_dsr_GET_Line_setting_list</h2>

      <h3>Introduction</h3>
         List of line settings

      <h3>Use cases</h3>
         Get list of line settings.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_GET_Line_setting_list( void * pv_AppRefHandle, u16 u16_LineMask )
         </b><br><br>
         This function requests the base station to get information about list of line settings.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u16_LineMask      Mask of lines (for all use 0xFF)
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_GET_Line_setting_list(void *pv_AppRefHandle, u16 u16_LineMask);


//*/
//    ==========  cmbs_dsr_SET_Line_setting_list  ===========
/*!
      \brief
         Set information about line settings.

         <h2>cmbs_dsr_SET_Line_setting_list</h2>

      <h3>Introduction</h3>
         List of line settings

      <h3>Use cases</h3>
         Set list of line settings.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_SET_Line_setting_list ( void * pv_AppRefHandle,
                                                            ST_IE_LINE_SETTINGS_LIST* pst_LineSettingsList )
         </b><br><br>
         This function send to the base station to list of line settings that must be set.
        <br>
      \param[in]        pv_AppRefHandle           reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pst_LineSettingsList      pointer to ST_IE_LINE_SETTINGS_LIST structure
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_SET_Line_setting_list(void *pv_AppRefHandle, ST_IE_LINE_SETTINGS_LIST *pst_LineSettingsList);


//*/
//    ==========  cmbs_dsr_sys_RFSuspend  ===========
/*!
      \brief
         This function toggles the RF in Suspend mode.

         <h2>cmbs_dsr_sys_RFSuspend</h2>

      <h3>Introduction</h3>
         List of subcribed handsets

      <h3>Use cases</h3>
         Get list of subcribed handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_sys_RFSuspend( void * pv_AppRefHandle )
         </b><br><br>
         This function toggles the RF in Suspend mode.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_Param   Parameter identifier
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_RFSuspend(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_sys_RFResume  ===========
/*!
      \brief
         This function wakes up the RF.

         <h2>cmbs_dsr_sys_RFResume</h2>

      <h3>Introduction</h3>
         List of subcribed handsets

      <h3>Use cases</h3>
         Get list of subcribed handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_sys_RFResume( void * pv_AppRefHandle )
         </b><br><br>
         This function wakes up the RF.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_Param   Parameter identifier
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_RFResume(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_sys_TurnOnNEMo  ===========
/*!
      \brief
         This function turn on NEMo mode for CMBS base.

         <h2>cmbs_dsr_sys_TurnOnNEMo</h2>

      <h3>Introduction</h3>
         List of subcribed handsets

      <h3>Use cases</h3>
         Get list of subcribed handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_sys_TurnOnNEMo( void * pv_AppRefHandle )
         </b><br><br>
         This function toggle CMBS base into NEMo mode.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_Param   Parameter identifier
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_TurnOnNEMo(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_sys_TurnOffNEMo  ===========
/*!
      \brief
         This function turn off NEMo mode for CMBS base.

         <h2>cmbs_dsr_sys_TurnOffNEMo</h2>

      <h3>Introduction</h3>
         List of subcribed handsets

      <h3>Use cases</h3>
         Get list of subcribed handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_sys_TurnOffNEMo( void * pv_AppRefHandle )
         </b><br><br>
         This function turn off NEMo mode for CMBS base.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_Param   Parameter identifier
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_TurnOffNEMo(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_sys_SendRawMsg  ===========
/*!
      \brief
         Sends a raw message.

         <h2>cmbs_dsr_sys_SendRawMsg</h2>

      <h3>Introduction</h3>
         Sends a raw OS message.

      <h3>Use cases</h3>
         Sending raw OS messages.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_sys_SendRawMsg ( void * pv_AppRefHandle, u8 u8_Process, u8 u8_Instance, u8 u8_Event,
                                            u16 u16_Info, u8 u16_DataLen, u8 *pu8_DataPtr )
         </b><br><br>
         This function enables the host to send a raw OS message to the CMBS.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u8_Process        destination process
      \param[in]        u8_Instance       instance of destination process
      \param[in]        u8_Event          event to send
      \param[in]        u16_Info          16 bit additional info
      \param[in]        u16_DataLen       length of message data
      \param[in]        pu8_Data          pointer to message data

      \return           Return code

      \see              CMBS_EV_DSR_SYS_SEND_RAWMSG_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_sys_SendRawMsg(void *pv_AppRefHandle, u8 u8_Process, u8 u8_Instance,
                                          u8 u8_Event, u16 u16_Info, u8 u16_DataLen, u8 *pu8_Data);

//*/
//    ==========  cmbs_dsr_param_area_Get  ===========
/*!
      \brief
         Requests CMBS memory, as RAM or EEProm, S-Flash, etc.

         <h2>cmbs_dsr_param_area_Get</h2>

      <h3>Introduction</h3>
         Requests CMBS memory, as RAM or EEProm, S-Flash, etc.

      <h3>Use cases</h3>
         Reading memory area.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_param_area_Get( void * pv_AppRefHandle, E_CMBS_PARAM_AREA_TYPE e_AreaType, u16 u16_Offset, u16 u16_DataLen );
         </b><br><br>
         This function requests the destination side to read data from the specified CMBS memory area (e.g. EEPROM, RAM, etc. ).<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_AreaType        type of memory area (EEPROM, RAM, etc. )
      \param[in]        u16_Offset        offset in memory area
      \param[in]        u16_DataLen       data length in bytes, maximum size depends on target buffer size.
      \return           Return code

      \see              CMBS_EV_DSR_PARAM_AREA_GET_RES <br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_param_area_Get(void *pv_AppRefHandle, E_CMBS_PARAM_AREA_TYPE e_AreaType, u32 u32_Offset, u16 u16_DataLen);

//*/
//    ==========  cmbs_dsr_param_area_Set  ===========
/*!
      \brief
         Writes data to CMBS memory area, as RAM or EEProm, S-Flash, etc.

         <h2>cmbs_dsr_param_area_Set</h2>

      <h3>Introduction</h3>
         Writes data to CMBS memory area, as RAM or EEProm, S-Flash, etc.

      <h3>Use cases</h3>
         Setting or updating data in the base station's memory.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_param_area_Set( void * pv_AppRefHandle, E_CMBS_PARAM_AREA_TYPE e_AreaType, u16 u16_Offset, u8* pu8_Data, u16 u16_DataLen );
         </b><br><br>
         This function updates data in memory at a specific location in memory area.<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_AreaType        type of memory area (EEPROM, RAM, etc. )
      \param[in]        u16_Offset        offset in memory area
      \param[in]        pu8_Data          the new data
      \param[in]        u16_DataLen       data length in bytes, maximum size depends on target buffer size.
      \return           Return code

      \see              CMBS_EV_DSR_PARAM_AREA_SET_RES <br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_param_area_Set(void *pv_AppRefHandle, E_CMBS_PARAM_AREA_TYPE e_AreaType, u32 u32_Offset, u8 *pu8_Data, u16 u16_DataLen);


//*/
//    ==========  cmbs_dsr_param_Get  ===========
/*!
      \brief
         Requests a particular CMBS parameter value.

         <h2>cmbs_dsr_param_Get</h2>

      <h3>Introduction</h3>
         Requests a particular CMBS parameter value.

      <h3>Use cases</h3>
         Reading CMBS parameter data from base station.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_param_Get ( void * pv_AppRefHandle, E_CMBS_PARAM_TYPE e_ParamType, E_CMBS_PARAM e_Param )
         </b><br><br>
         This function requests the destination side to read data for the specified CMBS parameter
         ( e.g. RFPI. See E_CMBS_PARAM enumeration ).<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_ParamType       type of parameter (EEPROM, PRODUCTION, LA, etc.)
      \param[in]        e_Param           parameter identifier

      \return           Return code

      \see              CMBS_EV_DSR_PARAM_GET_RES<br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_param_Get(void *pv_AppRefHandle, E_CMBS_PARAM_TYPE e_ParamType, E_CMBS_PARAM e_Param);

//*/
//    ==========  cmbs_dsr_param_Set  ===========
/*!
      \brief
         Sets / updates a particular CMBS parameter value.

         <h2>cmbs_dsr_param_Set</h2>

      <h3>Introduction</h3>
         Sets / updates a particular CMBS parameter value.

      <h3>Use cases</h3>
         Setting or updating data in the base station's memory.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_param_Set ( void * pv_AppRefHandle, E_CMBS_PARAM_TYPE e_ParamType, E_CMBS_PARAM e_Param, u8 * pu8_Data, u16 u16_DataLen )
         </b><br><br>
         This function updates data for the specified CMBS parameter( see E_CMBS_PARAM enumeration ).<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_ParamType       type of parameter (EEPROM, PRODUCTION, LA, etc.)
      \param[in]        e_Param           specifies which value to be written
      \param[in]        pu8_Data          the new data
      \param[in]        u16_DataLen       data length in bytes, maximum size depends on target buffer size.

      \return           Return code

      \see              CMBS_EV_DSR_PARAM_SET_RES<br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_param_Set(void *pv_AppRefHandle, E_CMBS_PARAM_TYPE e_ParamType, E_CMBS_PARAM e_Param, u8 *pu8_Data, u16 u16_DataLen);

//*/
//    ==========  cmbs_dsr_cord_OpenRegistration  ===========
/*!
      \brief
         Opens the Base station for registration.

         <h2>cmbs_dsr_cord_OpenRegistration</h2>

      <h3>Introduction</h3>
         Opens the Base station for registration.

      <h3>Use cases</h3>
         Used to open registration so handsets can be registered.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_cord_OpenRegistration ( void * pv_AppRefHandle, u32 timeout)
         </b><br><br>
         In this mode, new handsets can be subscribed to the base.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        timeout           time for which registration is open

      \return           Return code

      \see              CMBS_EV_DSR_CORD_OPENREG_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_cord_OpenRegistration(void *pv_AppRefHandle, u32 timeout);

//*/
//    ==========  cmbs_dsr_cord_OpenRegistrationExt  ===========
/*!
      \brief
         Opens the Base station for registration.

         <h2>cmbs_dsr_cord_OpenRegistrationExt</h2>

      <h3>Introduction</h3>
         Opens the Base station for registration.

      <h3>Use cases</h3>
         Used to open registration so handsets can be registered.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_cord_OpenRegistrationExt ( void * pv_AppRefHandle, u32 timeout, E_CMBS_HS_REGISTER_ENABLE e_RegEnable)
         </b><br><br>
         In this mode, new handsets and/or ULE devices can be subscribed to the base.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        timeout           time for which registration is open
      \param[in]        e_RegEnable       Type of registration enable: all, ULE or handset

      \return           Return code

      \see              CMBS_EV_DSR_CORD_OPENREG_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_cord_OpenRegistrationExt(void *pv_AppRefHandle, u32 timeout, E_CMBS_HS_REGISTER_ENABLE e_RegEnable);

//*/
//    ==========  cmbs_dsr_cord_CloseRegistration  ===========
/*!
      \brief
         Closes the Base station for registration.

         <h2>cmbs_dsr_cord_CloseRegistration</h2>

      <h3>Introduction</h3>
         Closes the Base station for registration.

      <h3>Use cases</h3>
         Used to close registration after opening it.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_cord_CloseRegistration ( void * pv_AppRefHandle )
         </b><br><br>
         This function will close the registration mode previously opened by the
         cmbs_dsr_cord_OpenRegistration() function.<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_CORD_CLOSEREG_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_cord_CloseRegistration(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_cord_DisableEncryption  ===========
/*!
      \brief
         Disables Encryption of calls on the Base station

         <h2>cmbs_dsr_cord_DisableEncryption</h2>

      <h3>Introduction</h3>
         Disables Encryption of calls on the Base station.

      <h3>Use cases</h3>
         Used to disable encryption for CC calls

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_cord_DisableEncryption ( void * pv_AppRefHandle )
         </b><br><br>
         In this mode, Call setups don't start encryption.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              none

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_cord_DisableEncryption(void *pv_AppRefHandle);


//*/
//    ==========  cmbs_dsr_cord_EnableEncryption  ===========
/*!
      \brief
         Enables Encryption of calls on the Base station

         <h2>cmbs_dsr_cord_EnableEncryption</h2>

      <h3>Introduction</h3>
         Enables Encryption of calls on the Base station.

      <h3>Use cases</h3>
         Used to (re)enable encryption for CC calls

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_cord_EnableEncryption ( void * pv_AppRefHandle )
         </b><br><br>
         In this mode, Call setups don't start encryption.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              none

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_cord_EnableEncryption(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_FixedCarrierSet  ===========
/*!
      \brief
         Sets a fixed carrier for RF meassurement purpose

         <h2>cmbs_dsr_FixedCarrierSet</h2>

      <h3>Introduction</h3>
         Sets a fixed carrier for RF meassurement purpose.

      <h3>Use cases</h3>
         Used to fixe the carrier (frequency) for RF meassurements

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_FixedCarrierSet ( void * pv_AppRefHandle, u8 u8_Carrier )
         </b><br><br>
         In this mode, Frequenca selection is disabled.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
                        u8_Carrier    fixed Carrier used for RF links

      \return           Return code

      \see              

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_FixedCarrierSet(void *pv_AppRefHandle, u8 u8_Carrier);


//    ==========  cmbs_dsr_hs_Page  ===========
/*!
      \brief
         Activates the ringer on the specified handsets.

         <h2>cmbs_dsr_hs_Page</h2>

      <h3>Introduction</h3>
         Activates the ringer on the specified handsets.

      <h3>Use cases</h3>
         Paging function.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_hs_Page ( void * pv_AppRefHandle, u16 u16_Handsets )
         </b><br><br>
         This function will let the specified handsets ring.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u16_Handsets      bitmask for handsets to be paged:
                                          bit 0 = handset 1, bit 1 = handset 2, etc.
                                          0xFFFF = all handsets

      \return           Return code

      \see              CMBS_EV_DSR_HS_PAGE_START_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_hs_Page(void *pv_AppRefHandle, u16 u16_Handsets);
//E_CMBS_RC	      cmbs_dee_HostConfigureFxsPort(u8 u8_SrxFxs, u8 u8_FxsChannel, u8 u8_ChannelMode,u32 u32_SlotScheme);

//    ==========  cmbs_dsr_hs_StopPaging  ===========
/*!
      \brief
         Activates the ringer on the specified handsets.

         <h2>cmbs_dsr_hs_StopPaging</h2>

      <h3>Introduction</h3>
         Activates the ringer on the specified handsets.

      <h3>Use cases</h3>
         Paging function.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_hs_StopPaging ( void * pv_AppRefHandle )
         </b><br><br>
         This function will let the specified handsets ring.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           E_CMBS_RC

      \see              CMBS_EV_DSR_HS_PAGE_STOP_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
    E_CMBS_RC         cmbs_dsr_hs_StopPaging( void * pv_AppRefHandle, u16 u16_Handsets,  u32 u32_CallInstance );

//    ==========  cmbs_dsr_hs_Range  ===========
/*!
      \brief
         Indicates Range Status of the handsets.

*/
E_CMBS_RC         cmbs_dsr_hs_Range(void *pv_AppRefHandle, u8 u8_Hs, E_CMBS_HS_RANGE_STATUS e_Status);

//*/
//    ==========  cmbs_dsr_hs_Delete  ===========
/*!
      \brief
         Deletes one or more handsets.

         <h2>cmbs_dsr_hs_Delete</h2>

      <h3>Introduction</h3>
         Deletes one or more handsets.

      <h3>Use cases</h3>
         Delete handsets from the base's database.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_hs_Delete ( void * pv_AppRefHandle, u16 u16_Handsets )
         </b><br><br>
         This function will delete the specified handsets on the base station.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u16_Handsets      bitmask for handsets to be deleted:
                                          bit 0 = handset 1, bit 1 = handset 2, etc.
                                          0xFFFF = all handsets

      \return           Return code

      \see              CMBS_EV_DSR_HS_DELETE_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_hs_Delete(void *pv_AppRefHandle, u16 u16_Handsets);

//*/
//    ==========  cmbs_dsr_hs_DataSessionOpen  ===========
/*!
      \brief
         Open a Data Session with a specific handset.

         <h2>cmbs_dsr_hs_DataSessionOpen</h2>

      <h3>Introduction</h3>
         Open a Data Session with a specific handset.

      <h3>Use cases</h3>
         Open data session with a specific handset to be used for data applications.
         Currently only the LBR A-Filed IWU-INFO messages is available.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_hs_DataSessionOpen ( void * pv_AppRefHandle,
                                                 ST_DATA_SESSION_TYPE * pst_DataSessionType,
                                                 u16 u16_Handsets )
         </b><br><br>
         This function will attempt to open a data session with the specified handset.
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     pst_DataSessionType  the type of data session: A-Filed IWU-INFO is currently supported
      \param[in]     u16_Handsets         bitmask for handsets: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                          only one handset ( one bit ) supported

      \return        Return code

      \see           CMBS_EV_DSR_HS_DATA_SESSION_OPEN_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_hs_DataSessionOpen(void *pv_AppRefHandle,
                                              ST_DATA_SESSION_TYPE *pst_DataSessionType,
                                              u16 u16_Handsets);


//*/
//    ==========  cmbs_dsr_hs_DataSessionOpenRes  ===========
/*!
      \brief
         Response to a data session request from PP.

         <h2>cmbs_dsr_hs_DataSessionOpenRes</h2>

      <h3>Introduction</h3>
         Response to a data session request from PP.

      <h3>Use cases</h3>
        PP opens a data session (e.g. SUOTA application).
        Host can accept or reject the request.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_hs_DataSessionOpenRes( void * pv_AppRefHandle, u16 u16_SessionId, bool bAccept)
         </b><br><br>
         This function will send a response to the PP request to start a data session.
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        Session Id as received in CMBS_EV_DSR_HS_DATA_SESSION_OPEN event
      \param[in]     bAccept              Accepted / Rejected

      \return        Return code

      \see           CMBS_EV_DSR_HS_DATA_SESSION_OPEN

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_hs_DataSessionOpenRes(void *pv_AppRefHandle, u16 u16_SessionId, bool bAccept);

//*/
//    ==========  cmbs_dsr_hs_DataSessionClose  ===========
/*!
      \brief
         Close a Data Session with a specific handset.

         <h2>cmbs_dsr_hs_DataSessionClose</h2>

      <h3>Introduction</h3>
         Close a Data Session with a specific handset.

      <h3>Use cases</h3>
         Close a previously opened data session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_hs_DataSessionClose ( void * pv_AppRefHandle, u16 u16_SessionId )
         </b><br><br>
         This function will close a previous opened data session.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u16_SessionId     the session Id received in:<br>
                                          CMBS_EV_DSR_HS_DATA_SESSION_OPEN_RES or<br>
                                          CMBS_EV_DSR_HS_DATA_SESSION_OPEN

      \return           Return code

      \see              CMBS_EV_DSR_HS_DATA_SESSION_CLOSE_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_hs_DataSessionClose(void *pv_AppRefHandle, u16 u16_SessionId);

//*/
//    ==========  cmbs_dsr_hs_DataSend  ===========
/*!
      \brief
         Send data to a handset using an opened data session.

         <h2>cmbs_dsr_hs_DataSend</h2>

      <h3>Introduction</h3>
         Send data to a handset using an opened data session.

      <h3>Use cases</h3>
         Use this function to send data to a handset using an opened data session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_hs_DataSend ( void * pv_AppRefHandle, u16 u16_SessionId, u8 * pu8_Data, u16 u16_DataLen )
         </b><br><br>
         This function is sending data to a handset using an opened data session.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId     the session Id received in:<br>
                                       CMBS_EV_DSR_HS_DATA_SESSION_OPEN_RES or<br>
                                       CMBS_EV_DSR_HS_DATA_SESSION_OPEN
      \param[in]     pu8_Data          the data to be sent
      \param[in]     u16_DataLen       the length of the data to be sent

      \return        Return code

      \see           CMBS_EV_DSR_HS_DATA_SESSION_CLOSE

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_hs_DataSend(void *pv_AppRefHandle, u16 u16_SessionId, u8 *pu8_Data, u16 u16_DataLen);

//*/
//    ==========  cmbs_dsr_fw_UpdateStart  ===========
/*!
      \brief
         Start firmware update.

         <h2>cmbs_dsr_fw_UpdateStart</h2>

      <h3>Introduction</h3>
         Start firmware update.

      <h3>Use cases</h3>
         Firmware update process.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_fw_UpdateStart ( void * pv_AppRefHandle, u8* pu8_Data, u16 u16_DataLen )
         </b><br><br>
         This function starts the firmware update process and sends the first data packet to the base station.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pu8_Data          first firmware data packet
      \param[in]        u16_DataLen       length of data packet: a value of power of two, but maximum size depends on target buffer size.

      \return           Return code

      \see              CMBS_EV_DSR_FW_UPD_START_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_fw_UpdateStart(void *pv_AppRefHandle, u8 *pu8_Data, u16 u16_DataLen);

//*/
//    ==========  cmbs_dsr_fw_UpdatePacketNext  ===========
/*!
      \brief
         Provides next firmware data packet.

         <h2>cmbs_dsr_fw_UpdatePacketNext</h2>

      <h3>Introduction</h3>
         Provides next firmware data packet.

      <h3>Use cases</h3>
         Firmware management.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_fw_UpdatePacketNext ( void * pv_AppRefHandle, u8* pu8_Data, u16 u16_DataLen )
         </b><br><br>
         This function sends the next firmware data packet to the base station.<br>
         It should be called after cmbs_dsr_fw_UpdateStart() if the number of data packets is greater than 2.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pu8_Data          next firmware data packet
      \param[in]     u16_DataLen       length of data packet: a value of power of two, but maximum size depends on target buffer size.

      \return           Return code

      \see              CMBS_EV_DSR_FW_UPD_PACKETNEXT_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_fw_UpdatePacketNext(void *pv_AppRefHandle, u8 *pu8_Data, u16 u16_DataLen);

//*/
//    ==========  cmbs_dsr_fw_UpdateEnd  ===========
/*!
      \brief
         Last firmware data packet.

         <h2>cmbs_dsr_fw_UpdateEnd</h2>

      <h3>Introduction</h3>
         Last firmware data packet.

      <h3>Use cases</h3>
         Firmware management.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_fw_UpdateEnd ( void * pv_AppRefHandle, u8* pu8_Data, u16 u16_DataLen )
         </b><br><br>
         This function sends the last firmware data packet to the base station<br>
         and finalizes the update process.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pu8_Data          last firmware data packet
      \param[in]     u16_DataLen       length of data packet, maximum size depends on target buffer size.

      \return           Return code

      \see              CMBS_EV_DSR_FW_UPD_END_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_fw_UpdateEnd(void *pv_AppRefHandle, u8 *pu8_Data, u16 u16_DataLen);

//*/
//    ==========  cmbs_dsr_fw_VersionGet  ===========
/*!
      \brief
         Get target firmware version for a specific software module.

         <h2>cmbs_dsr_fw_VersionGet</h2>

      <h3>Introduction</h3>
         Get target firmware version for a specific software module.

      <h3>Use cases</h3>
         Firmware management.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_fw_VersionGet ( void * pv_AppRefHandle, E_CMBS_MODULE e_FwModule )
         </b><br><br>
         This function sends a get firmware version request to the base station.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        e_FwModule        firmware module identifier

      \return           Return code

      \see              CMBS_EV_DSR_FW_VERSION_GET_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_fw_VersionGet(void *pv_AppRefHandle, E_CMBS_MODULE e_FwModule);

//*/
//    ==========  cmbs_dsr_hw_VersionGet  ===========
/*!
      \brief
         Get target hardware version.

         <h2>cmbs_dsr_hw_VersionGet</h2>

      <h3>Introduction</h3>
         Get target hardwar version.

      <h3>Use cases</h3>
         Firmware management.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_hw_VersionGet ( void * pv_AppRefHandle )
         </b><br><br>
         This function sends a get hardware version request to the base station.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_HW_VERSION_GET_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_hw_VersionGet(void *pv_AppRefHandle);
//*/
//    ==========  cmbs_dsr_EEPROM_VersionGet  ===========
/*!
      \brief
         Get target EEPROM version.

         <h2>cmbs_dsr_EEPROM_VersionGet</h2>

      <h3>Introduction</h3>
         Get target EEPROM version.

      <h3>Use cases</h3>
         EEPROM management.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_EEPROM_VersionGet ( void * pv_AppRefHandle )
         </b><br><br>
         This function sends a get EEPROM version request to the base station.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_EEPROM_VERSION_GET_RES

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC	cmbs_dsr_EEPROM_VersionGet(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_RxtunGpioConnect  ===========
/*!
      \brief
         Set GPIO to output the clock signal for RXTUN.

         <h2>cmbs_dsr_RxtunGpioConnect</h2>

      <h3>Introduction</h3>
         Set GPIO to output the clock signal for RXTUN.

      <h3>Use cases</h3>
         Set GPIO to output the clock signal for RXTUN.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_RxtunGpioConnect( void * pv_AppRefHandle, u16 u16_GPIO )
         </b><br><br>
         This function set GPIO to output the clock signal for RXTUN.<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u16_GPIO          GPIO

      \return           Return code

      \see              CMBS_EV_DSR_GPIO_CONNECT_RES<br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_RxtunGpioConnect(void *pv_AppRefHandle, u16 u16_GPIO);

//*/
//    ==========  cmbs_dsr_RxtunGpioDisconnect  ===========
/*!
      \brief
         Reset GPIO to output the clock signal for RXTUN.

         <h2>cmbs_dsr_RxtunGpioConnect</h2>

      <h3>Introduction</h3>
         Reset GPIO to output the clock signal for RXTUN.

      <h3>Use cases</h3>
         Reset GPIO to output the clock signal for RXTUN.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_RxtunGpioDisconnect( void * pv_AppRefHandle )
         </b><br><br>
         This function set GPIO to output the clock signal for RXTUN.<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_GPIO_DISCONNECT_RES<br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_RxtunGpioDisconnect(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dsr_ATETestStart  ===========
/*!
      \brief
         Start ATE test.

         <h2>cmbs_dsr_ATETestStart</h2>

      <h3>Introduction</h3>
         Start ATE test.

      <h3>Use cases</h3>
         Start ATE test.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_ATETestStart( void * pv_AppRefHandle, ST_IE_ATE_SETTINGS * pst_AteSettings )
         </b><br><br>
         This function Start ATE test.<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pst_AteSettings	  ATE settings

      \return           Return code

      \see              CMBS_EV_DSR_ATE_TEST_START_RES<br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_ATETestStart(void *pv_AppRefHandle, ST_IE_ATE_SETTINGS *pst_AteSettings);

//*/
//    ==========  cmbs_dsr_ATETestLeave  ===========
/*!
      \brief
         Leave ATE test.

         <h2>cmbs_dsr_ATETestStart</h2>

      <h3>Introduction</h3>
         Leave ATE test.

      <h3>Use cases</h3>
         Leave ATE test.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_ATETestLeave( void * pv_AppRefHandle )
         </b><br><br>
         This function Leave ATE test.<br>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_ATE_TEST_LEAVE_RES<br>

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_ATETestLeave(void *pv_AppRefHandle);






//*/
//    ==========  cmbs_dsr_la_SessionStartRes  ===========
/*!
      \brief
         Host response for a request to start List Access Session.

         <h2>cmbs_dsr_la_SessionStartRes</h2>

      <h3>Introduction</h3>
         Host response for a request to start List Access Session.

      <h3>Use cases</h3>
         A handset is requesting to start a List Access session and the Host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_SessionStartRes ( void * pv_AppRefHandle,
                                                 u16 u16_SessionId,
                                                 u16 u16_NumOfEntries,
                                                 ST_LA_FIELDS * pst_LASortFields,
                                                 E_CMBS_RESPONSE e_Response,
                                                 E_CMBS_LA_START_SESSION_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_SESSION_START event.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId     the session Id received in CMBS_EV_DSR_LA_SESSION_START
      \param[in]     u16_NumOfEntries  the number of entries in the list
      \param[in]     pst_LASortFields  the sort fields of the list
      \param[in]     e_Response        CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason    If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_SESSION_START

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_SessionStartRes(void *pv_AppRefHandle,
                                              u16 u16_SessionId,
                                              u16 u16_NumOfEntries,
                                              ST_LA_FIELDS *pst_LASortFields,
                                              E_CMBS_RESPONSE e_Response,
                                              E_CMBS_LA_START_SESSION_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_SessionEndRes  ===========
/*!
      \brief
         Host response for a request to end List Access Session.

         <h2>cmbs_dsr_la_SessionEndRes</h2>

      <h3>Introduction</h3>
         Host response for a request to end List Access Session.

      <h3>Use cases</h3>
         A handset is requesting to end a List Access session and the Host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_SessionEndRes ( void * pv_AppRefHandle,
                                                 u16 u16_SessionId,
                                                 E_CMBS_RESPONSE e_Response,
                                                 E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_SESSION_END event.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId     the session Id received in CMBS_EV_DSR_LA_SESSION_END
      \param[in]     e_Response        CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason    If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_SESSION_END

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_SessionEndRes(void *pv_AppRefHandle,
                                            u16 u16_SessionId,
                                            E_CMBS_RESPONSE e_Response,
                                            E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_QuerySupEntryFieldsRes  ===========
/*!
      \brief
         Host response for a request to query the supported entry fields.

         <h2>cmbs_dsr_la_QuerySupEntryFieldsRes</h2>

      <h3>Introduction</h3>
         Host response for a request to query the supported entry fields.

      <h3>Use cases</h3>
         A handset is querying the Supported Entry Fields of a list and the host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_QuerySupEntryFieldsRes ( void * pv_AppRefHandle,
                                                        u16 u16_SessionId,
                                                        ST_LA_FIELDS * pst_LANonEditFields,
                                                        ST_LA_FIELDS * pst_LAEditFields,
                                                        E_CMBS_RESPONSE e_Response,
                                                        E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_QUERY_SUP_ENTRY_FIELDS event.
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_QUERY_SUP_ENTRY_FIELDS
      \param[in]     pst_LANonEditFields  the non-editable fields of the list
      \param[in]     pst_LAEditFields     the editable fields of the list
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_QUERY_SUP_ENTRY_FIELDS

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_QuerySupEntryFieldsRes(void *pv_AppRefHandle,
                                                     u16 u16_SessionId,
                                                     ST_LA_FIELDS *pst_LANonEditFields,
                                                     ST_LA_FIELDS *pst_LAEditFields,
                                                     E_CMBS_RESPONSE e_Response,
                                                     E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_SearchEntriesRes  ===========
/*!
      \brief
         Host response for a request to Search Entries in a list.

         <h2>cmbs_dsr_la_SearchEntriesRes</h2>

      <h3>Introduction</h3>
         Host response for a request to Search Entries in a list.

      <h3>Use cases</h3>
         A handset is requesting to search entries in a list and the Host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_SearchEntriesRes ( void * pv_AppRefHandle,
                                                        u16 u16_SessionId,
                                                        u16 u16_EntryStartIndex,
                                                        u16 u16_NumOfEntries,
                                                        E_CMBS_RESPONSE e_Response,
                                                        E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_SEARCH_ENTRIES event.<br><br>
         It should be followed by series of cmbs_dsr_la_DataPacketSend() calls,
         that actually return the entries of the Search results
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_QUERY_SUP_ENTRY_FIELDS
      \param[in]     u16_EntryStartIndex  the starting index of range of returned entries
      \param[in]     u16_NumOfEntries     the number of entries returned.<br>
                                          Note that since the list is sorted, the entries are continuous.
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_SEARCH_ENTRIES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_SearchEntriesRes(void *pv_AppRefHandle,
                                               u16 u16_SessionId,
                                               u16 u16_EntryStartIndex,
                                               u16 u16_NumOfEntries,
                                               E_CMBS_RESPONSE e_Response,
                                               E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_ReadEntriesRes  ===========
/*!
      \brief
         Host response for a request to Read Entries of a list.

         <h2>cmbs_dsr_la_ReadEntriesRes</h2>

      <h3>Introduction</h3>
         Host response for a request to Read Entries of a list.

      <h3>Use cases</h3>
         A handset is requesting to read entries from a list and the Host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_ReadEntriesRes ( void * pv_AppRefHandle,
                                                        u16 u16_SessionId,
                                                        u16 u16_EntryStartIndex,
                                                        u16 u16_NumOfEntries,
                                                        E_CMBS_RESPONSE e_Response,
                                                        E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_READ_ENTRIES event.<br><br>
         It should be followed by series of cmbs_dsr_la_DataPacketSend() calls,
         that actually return the entries of the Read request.
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_READ_ENTRIES
      \param[in]     u16_EntryStartIndex  the starting index of where reading has started
      \param[in]     u16_NumOfEntries     the number of entries returned<br>
                                          (can be smaller than the number of entries requested).
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_READ_ENTRIES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_ReadEntriesRes(void *pv_AppRefHandle,
                                             u16 u16_SessionId,
                                             u16 u16_EntryStartIndex,
                                             u16 u16_NumOfEntries,
                                             E_CMBS_RESPONSE e_Response,
                                             E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_EditEntryRes  ===========
/*!
      \brief
         Host response for a request to edit an entry in a list.

         <h2>cmbs_dsr_la_EditEntryRes</h2>

      <h3>Introduction</h3>
         Host response for a request to edit an entry in a list.

      <h3>Use cases</h3>
         A handset is requesting to edit an entry in a list and the host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_EditEntryRes ( void * pv_AppRefHandle, u16 u16_SessionId, u8 u8_Response )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_EDIT_ENTRY event.<br><br>

         It should be followed by series of cmbs_dsr_la_DataPacketSend() calls,
         that actually return the entry of the Edit request.<br><br>

         Note that Edit Entry is the same as Read Entry, but limited to one entry
         and the entry is Locked till Save Entry with same Entry Id is sent.

        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_EDIT_ENTRY
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_EDIT_ENTRY

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_EditEntryRes(void *pv_AppRefHandle,
                                           u16 u16_SessionId,
                                           E_CMBS_RESPONSE e_Response,
                                           E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_SaveEntryRes  ===========
/*!
      \brief
         Host response for a request to save an entry in a list.

         <h2>cmbs_dsr_la_SaveEntryRes</h2>

      <h3>Introduction</h3>
         Host response for a request to save an entry in a list.

      <h3>Use cases</h3>
         A handset is requesting to save an entry in a list and the host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_SaveEntryRes ( void * pv_AppRefHandle,
                                              u16 u16_SessionId,
                                              u16 u16_EntryId,
                                              u16 u16_EntryIndex,
                                              u16 u16_NumOfEntries,
                                              E_CMBS_RESPONSE e_Response,
                                              E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_SAVE_ENTRY event.<br><br>

         The response should be send after the the last received CMBS_EV_DSR_LA_DATA_RECEIVE event
         (the One that has the IsLast set to TRUE).<br>

         If the Save Entry request EntryId is not zero, it means that a previous Edit Entry has happened
         and not that Entry is Un locked again (It was locked by the Edit Entry).<br>

         If the Save Entry request EntryId is zero, this means that this is a new Entry
         and the function should retune the EntryId for this new Entry.<br>

         If the CMBS_EV_DSR_LA_DATA_RECEIVE event was empty,
         this means to cancel the Save and unlock the entry.

        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_EDIT_ENTRY
      \param[in]     u16_EntryId          the Id of the new entry if the Save Request was for a new entry<br>
                                          (EntryId in request was Zero).<br>
                                          If it is not a new entry but a Save of an exiting entry,<br>
                                          it returns the same EntryId that came in the request.
      \param[in]     u16_EntryIndex       the index of the new entry if the Save Request was for a new entry<br>
                                          (EntryId in request was zero).
                                          If it is not a new entry but a Save of an exiting entry,<br>
                                          it returns the index of that entry.
      \param[in]     u16_NumOfEntries     the total number of entries returned after save
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_SAVE_ENTRY

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_SaveEntryRes(void *pv_AppRefHandle,
                                           u16 u16_SessionId,
                                           u16 u16_EntryId,
                                           u16 u16_EntryIndex,
                                           u16 u16_NumOfEntries,
                                           E_CMBS_RESPONSE e_Response,
                                           E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_DeleteEntryRes  ===========
/*!
      \brief
         Host response for a request to delete an entry from a list.

         <h2>cmbs_dsr_la_DeleteEntryRes</h2>

      <h3>Introduction</h3>
         Host response for a request to delete an entry from a list.

      <h3>Use cases</h3>
         A handset is requesting to delete an entry from a list and the host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_DeleteEntryRes ( void * pv_AppRefHandle,
                                                u16 u16_SessionId,
                                                u16 u16_TotalNoOfEntries,
                                                E_CMBS_RESPONSE e_Response,
                                                E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_DELETE_ENTRY event.<br><br>
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_DELETE_ENTRY
      \param[in]     u16_TotalNoOfEntries the total number of entries
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_DELETE_ENTRY

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_DeleteEntryRes(void *pv_AppRefHandle,
                                             u16 u16_SessionId,
                                             u16 u16_TotalNoOfEntries,
                                             E_CMBS_RESPONSE e_Response,
                                             E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_DeleteListRes  ===========
/*!
      \brief
         Host response for a request to delete the entire list.

         <h2>cmbs_dsr_la_DeleteListRes</h2>

      <h3>Introduction</h3>
         Host response for a request to delete the entire list.

      <h3>Use cases</h3>
         A handset is requesting to delete the entire list and the host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_DeleteListRes ( void * pv_AppRefHandle,
                                               u16 u16_SessionId,
                                               E_CMBS_RESPONSE e_Response,
                                               E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_DELETE_LIST event.<br><br>
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_DELETE_LIST
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_DELETE_LIST

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_DeleteListRes(void *pv_AppRefHandle,
                                            u16 u16_SessionId,
                                            E_CMBS_RESPONSE e_Response,
                                            E_CMBS_LA_NACK_ERRORS e_RejectReason);

//*/
//    ==========  cmbs_dsr_la_SessionEnd  ===========
/*!
      \brief
         Host is terminating the List Access session.

         <h2>cmbs_dsr_la_SessionEnd</h2>

      <h3>Introduction</h3>
         Host is terminating the List Access session.

      <h3>Use cases</h3>
         Host is ending the List Access session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_SessionEnd ( void * pv_AppRefHandle,
                                               u16 u16_SessionId )
         </b><br><br>
         This function sends a CMBS_EV_DSR_LA_SESSION_END request to the base station.<br><br>
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_SESSION_START

      \return        Return code

      \see           CMBS_EV_DSR_LA_SESSION_END_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_SessionEnd(void *pv_AppRefHandle, u16 u16_SessionId);

//*/
//    ==========  cmbs_dsr_la_DataPacketSend  ===========
/*!
      \brief
         Host sends Data Packets to the handset.
         Usually in a response to Read/Edit/Search requests.

         <h2>cmbs_dsr_la_DataPacketSend</h2>

      <h3>Introduction</h3>
         Host sends Data Packets to the handset.
         Usually in a response to Read/Edit/Search requests.

      <h3>Use cases</h3>
         A handset is requesting to Read/Edit/Search entries and
         the host sends the data using this function call.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_DataPacketSend ( void * pv_AppRefHandle,
                                                u16 u16_SessionId,
                                                u8 u8_IsLast,
                                                u8 * pu8_Data,
                                                u16 u16_DataLen )
         </b><br><br>
         The host calls this function and the function returns immediately (non blocking).<br>
         The host should not free the data until receiving CMBS_EV_DSR_LA_DATA_SEND_RES.<br><br>
        <br>        
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId     the session Id received in CMBS_EV_DSR_LA_SESSION_START
      \param[in]     u8_IsLast         0 if not last, 1 if last
      \param[in]     pu8_Data          pointer to data
      \param[in]     u16_DataLen       length of data

      \return        Return code

      \see           CMBS_EV_DSR_LA_DATA_SEND_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_DataPacketSend(void *pv_AppRefHandle,
                                             u16 u16_SessionId,
                                             u8 u8_IsLast,
                                             u8 *pu8_Data,
                                             u16 u16_DataLen);


E_CMBS_RC         cmbs_dsr_la_DataPacketReceivedRes(void *pv_AppRefHandle,
                                                    u16 u16_SessionId,
                                                    E_CMBS_RESPONSE e_Response);


//*/
//    ==========  cmbs_dsr_la_AddSupportedList  ===========
/*!
      \brief
         Host adds a specific (non-mandatory) List to the list of supported lists.

         <h2>cmbs_dsr_la_AddSupportedList</h2>

      <h3>Introduction</h3>
         Host adds a specific (non-mandatory) List to the list of supported lists.

      <h3>Use cases</h3>
         Host supports non-mandatory Lia Lists.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_la_AddSupportedList( void * pv_AppRefHandle, u16 u16_ListId );

         </b><br><br>
         This function sends a CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS request to the base station.<br><br>
        <br>
      \param[in]     pv_AppRefHandle    reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_ListId         the List Id to add to list of supported lists

      \return        Return code

      \see           CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_la_AddSupportedList(void *pv_AppRefHandle, u16 u16_ListId);

E_CMBS_RC cmbs_dsr_la_AddSupportedListRes(void *pv_AppRefHandle, u16 u16_ListId, E_CMBS_RESPONSE e_Response);


//*/
//    ==========  cmbs_dsr_la_PropCmdRes  ===========
/*!
      \brief
         Host responds to a proprietary list access command.

         <h2>cmbs_dsr_la_PropCmdRes</h2>

      <h3>Introduction</h3>
         Host responds to a proprietary list access command.

      <h3>Use cases</h3>
         A handset is performing a proprietary list access operation.

      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_la_PropCmdRes( void * pv_AppRefHandle, PST_LA_PROP_CMD pst_Cmd );

         </b><br><br>
         This function sends a CMBS_EV_DSR_LA_PROP_CMD_RES event to the base station.<br><br>
        <br>
      \param[in]     pv_AppRefHandle    reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     pst_Cmd            Structure holding the response data bytes

      \return        Return code

      \see           CMBS_EV_DSR_LA_PROP_CMD_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_la_PropCmdRes(void *pv_AppRefHandle, PST_LA_PROP_CMD pst_Cmd);


//*/
//    ==========  cmbs_dsr_gen_SendVMWI  ===========
/*!
      \brief
         Sends MWI (Voice/SMS/Email Message Waiting Indication) event to one or more handsets.

         <h2>cmbs_dsr_gen_SendMWI</h2>

      <h3>Introduction</h3>
         Sends MWI (Voice/SMS/Email Message Waiting Indication) event to one or more handsets.

      <h3>Use cases</h3>
         When host needs to send a MWI to handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_gen_SendMWI ( void * pv_AppRefHandle,
                                          u16 u16_RequestId,
                                          u8 u8_Line,
                                          u16 u16_Handsets,
                                          u16 u16_NumOfMsg,
                                          E_CMBS_MWI_TYPE eType )
         </b><br><br>
         This function will send MWI event to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId     host should put unique values, as this will be returned in the response
      \param[in]     u8_Line           external line Id
      \param[in]     u16_Handsets      bitmask for handset: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                       0xFFFF for all
      \param[in]     u16_NumOfMsg      number of messages; allowable values: 0 to 127
      \param[in]     eType             Type of message

      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_gen_SendMWI(void *pv_AppRefHandle,
                               u16 u16_RequestId,
                               u8 u8_Line,
                               u16 u16_Handsets,
                               u16 u16_NumOfMsg,
                               E_CMBS_MWI_TYPE eType);

//*/
//    ==========  cmbs_dsr_gen_SendMissedCalls  ===========
/*!
      \brief
         Sends Missed Calls event to one or more handsets.

         <h2>cmbs_dsr_gen_SendMissedCalls</h2>

      <h3>Introduction</h3>
         Sends Missed Calls event to one or more handsets.

      <h3>Use cases</h3>
         When host needs to send a Missed Calls event to handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_gen_SendMissedCalls ( void * pv_AppRefHandle,
                                                  u16 u16_RequestId,
                                                  u8 u8_Line,
                                                  u16 u16_Handsets,
                                                  u16 u16_NumOfCalls )
         </b><br><br>
         This function will send Missed Calls event to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle        reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId          host should put unique values as this will be returned in the response
      \param[in]     u8_Line                external line Id
      \param[in]     u16_Handsets           bitmask for handset: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                            0xFFFF for all
      \param[in]     u16_NumOfCalls          holds the number of unread missed calls; allowable values: 0 to 127
      \param[in]     bNewMissedCall         Indicates whether or not this notification is sent because of new missed call 
                                            added to missed call list
      \param[in]     u16_TotalNumOfCalls    holds the number of read + unread missed calls;


      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_gen_SendMissedCalls(void *pv_AppRefHandle,
                                               u16 u16_RequestId,
                                               u8 u8_Line,
                                               u16 u16_Handsets,
                                               u16 u16_NumOfCalls,
                                               bool bNewMissedCall,
                                               u16 u16_TotalNumOfCalls);
//*/
//    ==========  cmbs_dsr_gen_SendListChanged  ===========
/*!
      \brief
         Sends List Changed event to one or more handsets.

         <h2>cmbs_dsr_gen_SendListChanged</h2>

      <h3>Introduction</h3>
         Sends List Changed event to one or more handsets.

      <h3>Use cases</h3>
         When one of the lists is changed, host uses this function to send a List Changed event to handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_gen_SendListChanged ( void * pv_AppRefHandle,
                                                  u16 u16_RequestId,
                                                  u16 u16_Handsets,
                                                  u8  u8_ListId,
                                                  u16 u16_NumOfEntries )
         </b><br><br>
         This function will send List Changed event to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId     host should put unique values as this will be return in the response
      \param[in]     u16_Handsets      bitmask for handset : bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                       0xFFFF for all
      \param[in]     u8_ListId         the identifier of the list that has changed
      \param[in]     u16_NumOfEntries  total number of elements in the list
      \param[in]     u8_Line           external line Id
      \param[in]     u8_LineSubType    0 - line id for external call, 3 - relating to, 4 - all lines
      
      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_gen_SendListChanged(void *pv_AppRefHandle,
                                               u16 u16_RequestId,
                                               u16 u16_Handsets,
                                               u8  u8_ListId,
                                               u16 u16_NumOfEntries,
                                               u8  u8_Line,
                                               u8  u8_LineSubType);
//*/
//    ==========  cmbs_dsr_gen_SendWebContent  ===========
/*!
      \brief
         Sends Web Content event to one or more handsets.

         <h2>cmbs_dsr_gen_SendWebContent</h2>

      <h3>Introduction</h3>
         Sends Web Content event to one or more handsets.

      <h3>Use cases</h3>
         When Web Content is available, host use the functions to send a Web Content event to handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_gen_SendWebContent ( void * pv_AppRefHandle,
                                                 u16 u16_RequestId,
                                                 u16 u16_Handsets,
                                                 u8  u8_NumOfWebCont );
         </b><br><br>
         This function will send Web Content event to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId     host should put unique values as this will be return in the response
      \param[in]     u16_Handsets      bitmask for handset: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                       0xFFFF for all
      \param[in]     u8_NumOfWebCont   holds the number of web contents; allowable values: 0 to 127

      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_gen_SendWebContent(void *pv_AppRefHandle,
                                              u16 u16_RequestId,
                                              u16 u16_Handsets,
                                              u8  u8_NumOfWebCont);

//*/
//    ==========  cmbs_dsr_gen_SendPropEvent  ===========
/*!
      \brief
         Sends Escape to Proprietary event to one or more handsets.

         <h2>cmbs_dsr_gen_SendPropEvent</h2>

      <h3>Introduction</h3>
         Sends proprietary event to one or more handsets.

      <h3>Use cases</h3>
         Host uses the function to send a proprietary event to handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_gen_SendPropEvent ( void * pv_AppRefHandle,
                                                u16 u16_RequestId,
                                                u16 u16_PropEvent,
                                                u8 * pu8_Data,
                                                u8  u8_DataLen,
                                                u16 u16_Handsets )
         </b><br><br>
         This function will send proprietary event to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId     host should put unique values as this will be return in the response
      \param[in]     u16_PropEvent     the proprietary event
      \param[in]     pu8_Data          proprietary data (NULL if not needed)
      \param[in]     u8_DataLen        proprietary data length (0 if not needed)
      \param[in]     u16_Handsets      bitmask for handset : bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                       0xFFFF for all

      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_gen_SendPropEvent(void *pv_AppRefHandle,
                                             u16 u16_RequestId,
                                             u16 u16_Reserved,
                                             u8 *pu8_Data,
                                             u8  u8_DataLen,
                                             u16 u16_Handsets);

//*/
//    ==========  cmbs_dsr_gen_SendPropEventNotification  ===========
/*!
      \brief
         Sends Escape to Proprietary event to one or more handsets.

         <h2>cmbs_dsr_gen_SendPropEventNotification</h2>

      <h3>Introduction</h3>
         Sends proprietary event notifications to one or more handsets.

      <h3>Use cases</h3>
         Host uses the function to send a proprietary event to handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_gen_SendPropEvent ( void * pv_AppRefHandle,
                                                u16 u16_RequestId,
                                                u16 u16_PropEvent,
                                                u8 * pu8_Data,
                                                u8  u8_DataLen,
                                                u16 u16_Handsets )
         </b><br><br>
         This function will send proprietary event to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId     host should put unique values as this will be return in the response
      \param[in]     u16_PropEvent     the proprietary event
      \param[in]     pu8_Data          proprietary event notification data (NULL if not needed)
      \param[in]     u8_DataLen        proprietary event notificaiton data length (0 if not needed)
      \param[in]     u16_Handsets      bitmask for handset : bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                       0xFFFF for all

      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC	cmbs_dsr_gen_SendPropEventNotification(void * pv_AppRefHandle,
	u16 u16_RequestId,
	u16 u16_PropEvent,
	u8 * pu8_Data,
	u8  u8_DataLen,
	u16 u16_Handsets);

//*/
//    ==========  cmbs_dsr_time_Update  ===========
/*!
      \brief
         Sends update of date and time to one or more handsets.

         <h2>cmbs_dsr_time_Update</h2>

      <h3>Introduction</h3>
         Sends update of date and time to one or more handsets.

      <h3>Use cases</h3>
         Host uses the function to update date and time in handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_time_Update ( void * pv_AppRefHandle,
                                          u16 u16_RequestId,
                                          ST_DATE_TIME * pst_DateTime,
                                          u16 u16_Handsets )
         </b><br><br>
         This function will send an update of date and time to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId     host should put unique values as this will be return in the response
      \param[in]     pst_DateTime      updated date and time
      \param[in]     u16_Handsets      bitmask for handset : bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                       0xFFFF for all

      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_time_Update(void *pv_AppRefHandle,
                                       u16 u16_RequestId,
                                       ST_DATE_TIME *pst_DateTime,
                                       u16 u16_Handsets);


//    ==========  cmbs_dsr_time_Indication  ===========
/*!
      \brief
         Indication of date and time update from handset.

*/
E_CMBS_RC         cmbs_dsr_time_Indication(void *pv_AppRefHandle,
                                           ST_DATE_TIME *pst_DateTime,
                                           u16 u16_Handset);

//*/
//    ==========  cmbs_dsr_LogBufferStart  ===========
/*!
      \brief
         Start system log.

         <h2>cmbs_dsr_LogBufferStart</h2>

      <h3>Introduction</h3>
         Start system log.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_LogBufferStart ( void * pv_AppRefHandle )
         </b><br><br>
         This function sends a request to a target to start system logging.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_SYS_LOG_START

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_LogBufferStart(void *pv_AppRefHandle);


//*/
//    ==========  cmbs_dsr_gen_SendListChangedEx  ===========
/*!
      \brief
         Sends List Changed event with details to one or more handsets.

         <h2>cmbs_dsr_gen_SendListChangedEx</h2>

      <h3>Introduction</h3>
         Sends List Changed event with details to one or more handsets.

      <h3>Use cases</h3>
         When one of the lists is changed, host uses this function to send a List Changed event with details to handsets.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_gen_SendListChangedEx (void *pv_AppRefHandle,
	                                               u16 u16_RequestId,
	                                               u16 u16_Handsets,
	                                               u8  u8_ListId,
	                                               u16 u16_NumOfEntries,
	                                               u8  u8_Line,
	                                               u8  u8_LineSubType,
	                                               PST_IE_GEN_EVENT_DETAILS pst_GenEventDetails)
         </b><br><br>
         This function will send List Changed event with details to the specified handsets.
        <br>
      \param[in]     pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_RequestId     host should put unique values as this will be return in the response
      \param[in]     u16_Handsets      bitmask for handset : bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                       0xFFFF for all
      \param[in]     u8_ListId         the identifier of the list that has changed
      \param[in]     u16_NumOfEntries  total number of elements in the list
      \param[in]     u8_Line           external line Id
      \param[in]     u8_LineSubType    0 - line id for external call, 3 - relating to, 4 - all lines
      \param[in]     pst_GenEventDetails    Details of List Change
      
      \return        Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_gen_SendListChangedEx(void *pv_AppRefHandle,
                                               u16 u16_RequestId,
                                               u16 u16_Handsets,
                                               u8  u8_ListId,
                                               u16 u16_NumOfEntries,
                                               u8  u8_Line,
                                               u8  u8_LineSubType,
                                               PST_IE_GEN_EVENT_DETAILS pst_GenEventDetails);

//*/
//    ==========  cmbs_dsr_LogBufferStop  ===========
/*!
      \brief
         Stop system log.

         <h2>cmbs_dsr_LogBufferStop</h2>

      <h3>Introduction</h3>
         Stop system log.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_LogBufferStop ( void * pv_AppRefHandle )
         </b><br><br>
         This function sends a request to a target to stop system logging.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_SYS_LOG_STOP

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_LogBufferStop(void *pv_AppRefHandle);


//*/
//    ==========  cmbs_dsr_LogBufferRead  ===========
/*!
      \brief
         Get log buffer.

         <h2>cmbs_dsr_LogBufferRead</h2>

      <h3>Introduction</h3>
         Get log buffer.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_LogBufferRead ( void * pv_AppRefHandle )
         </b><br><br>
         This function request a log buffer from target.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              CMBS_EV_DSR_SYS_LOG_REQ

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_LogBufferRead(void *pv_AppRefHandle);





//*/
//    ==========  cmbs_dsr_slic_LineTest_Start  ===========
/*!
      \brief
         Start system log.

         <h2>cmbs_dsr_slic_LineTest_Start</h2>

      <h3>Introduction</h3>
         Start FXS LineTests.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_slic_LineTest_Start ( void * pv_AppRefHandle,  u8 u8_Channel  )
         </b><br><br>
         This function sends a request to FXS to start Line Tests .
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u8 u8_Channel FXS channel ID (0 or 1)

      \return           Return code

      \see              CMBS_EV_DSR_SLIC_LINE_TEST_START, CMBS_EV_DSR_SLIC_LINE_TEST_START_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_slic_LineTest_Start(void *pv_AppRefHandle, u8 u8_Channel /* 0 or 1 */);


//*/
//    ==========  cmbs_dsr_slic_LineTest_Stop  ===========
/*!
      \brief
         Start system log.

         <h2>cmbs_dsr_slic_LineTest_Stop</h2>

      <h3>Introduction</h3>
         Stop FXS LineTests (optional in case that Host want to stop the tests before it is completed. 
         Normally the tests stops automatically when they are completed.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_slic_LineTest_Stop ( void * pv_AppRefHandle,  u8 u8_Channel  )
         </b><br><br>
         This function sends a request to FXS to stop Line Tests .
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u8 u8_Channel FXS channel ID (0 or 1)

      \return           Return code

      \see              CMBS_EV_DSR_SLIC_LINE_TEST_STOP, CMBS_EV_DSR_SLIC_LINE_TEST_STOP_RES

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC cmbs_dsr_slic_LineTest_Stop(void *pv_AppRefHandle, u8 u8_Channel /* 0 or 1 */); // optional, only if the host wants to stop the test in the middle

//	  ==========  cmbs_dsr_Fxs Ring Start / Stop Request  ===========

E_CMBS_RC cmbs_dsr_FxsRingTest_Start(void *pv_AppRefHandle, u8 u8_Channel /* 0 or 1 */); 

E_CMBS_RC cmbs_dsr_FxsRingTest_Stop(void *pv_AppRefHandle, u8 u8_Channel /* 0 or 1 */); 

E_CMBS_RC cmbs_dsr_FxsOpenLoop_Request(void *pv_AppRefHandle, u8 u8_Channel );


//	  ==========  cmbs_dsr_Fxs Nlt Capacitance test: Start / Stop Requests  ===========

E_CMBS_RC cmbs_dsr_slic_NltCapTest_Start(void *pv_AppRefHandle, u8 u8_Channel /* 0 or 1 */); // optional, only if the host wants to stop the test in the middle

E_CMBS_RC cmbs_dsr_slic_NltCapTest_Stop(void *pv_AppRefHandle, u8 u8_Channel /* 0 or 1 */); // optional, only if the host wants to stop the test in the middle


//*/
//	  ==========  cmbs_dsr_FxsStatus_Request  ===========
/*!
	  \brief
		 Start system log.

		 <h2>cmbs_dsr_FxsStatus_Request</h2>

	  <h3>Introduction</h3>
		 Get Fxs port status, if onhook or offhook

	  <h3>Use cases</h3>
		 Debugging.

	  <h3>API Functions description</h3>
		 <b>
		 E_CMBS_RC cmbs_dsr_FxsStatus_Request ( void * pv_AppRefHandle,  u8 u8_Channel  )
		 </b><br><br>
		 This function sends a request to FXS to stop Line Tests .
		<br>
	  \param[in]		pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
	  \param[in]		u8 u8_Channel FXS channel ID (0 or 1)

	  \return			Return code

	  \see				CMBS_EV_DSR_FXS_STATUS_REQ, CMBS_EV_DSR_FXS_STATUS_RES

	  <b>Sample Code:</b><br>
	  <code></code>
*/

E_CMBS_RC cmbs_dsr_FxsStatus_Request(void *pv_AppRefHandle, u8 u8_Channel /* 0 or 1 */); 


/*****************************************************************************
 * DEE (external endpoint) functions
 *****************************************************************************/

//*/
//    ==========  cmbs_dee_CallInstanceNew  ===========
/*!
      \brief
         Provide a new unique call instance ID to be used when establishing new call.

         <h2>cmbs_dee_CallInstanceNew</h2>

      <h3>Introduction</h3>
         Provide a new unique call instance ID to be used when establishing new call.

      <h3>Use cases</h3>
         The application wants to establish a new call.
         For any call establishment an unique call instance ID is required.

      <h3>API Functions description</h3>
         <b>
         u32 cmbs_dee_CallInstanceNew ( void * pv_AppRefHandle )
         </b><br><br>
         Returns a unique Call Instance ID which will be used in any call related function,
         from Call Establish to Call Release Complete.<br>
         Any cmbs_dee_ function must get this ID via a CMBS_IE_CALLINSTANCE Information Element.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Unique call instance ID

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
u32               cmbs_dee_CallInstanceNew(void *pv_AppRefHandle);

//*/
//    ==========  cmbs_dee_CallEstablish  ===========
/*!
      \brief
         Starts a new incoming or outgoing call.

         <h2>cmbs_dee_CallEstablish</h2>

      <h3>Introduction</h3>
         Starts a new incoming or outgoing call.

      <h3>Use cases</h3>
         Setting up a new call.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallEstablish ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function should be called by the application on detection of an incoming call.<br>
         It will also be called by the CMBS when the handset user has dialed an external number.<br>
         At this point, the application might instantiate the resources needed for a new call.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, X, , }
      \TABR4C{CMBS_IE_CALLERNAME, , X, }
      \TABR4C{CMBS_IE_CALLEDPARTY, X, , }
      \TABR4C{CMBS_IE_CALLEDNAME, , ,X}
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, X, , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , X,  }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallEstablish(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallProgress  ===========
/*!
      \brief
         Sends call progress event.

         <h2>cmbs_dee_CallProgress</h2>

      <h3>Introduction</h3>
         Sends call progress event.

      <h3>Use cases</h3>
         Call progress as a response to Call establish event.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallProgress ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function sends a call progress event together with call progress information.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, X, , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , X, If not used in this function\, it must be sent in cmbs_dee_CallAnswer()}
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallProgress(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallAnswer  ===========
/*!
      \brief
         Answer a call.

         <h2>cmbs_dee_CallAnswer</h2>

      <h3>Introduction</h3>
         Answer a call.

      <h3>Use cases</h3>
         Answer an establish call.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallAnswer ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function should be called as a response to Call Establish event.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , ,X}
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, X, , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , X, }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallAnswer(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallRelease  ===========
/*!
      \brief
         Release an established call.

         <h2>cmbs_dee_CallRelease</h2>

      <h3>Introduction</h3>
         Release an established call.

      <h3>Use cases</h3>
         When call is to be released.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallRelease ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function should be called to release a Call.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , X, }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, X, , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , X, }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallRelease(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallReleaseComplete  ===========
/*!
      \brief
         Release Complete after receiving Release event.

         <h2>cmbs_dee_CallReleaseComplete</h2>

      <h3>Introduction</h3>
         Release Complete after receiving Release event.

      <h3>Use cases</h3>
         This function is called after receiving Release event.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallReleaseComplete ( void * pv_AppRefHandle, u32 u32_CallInstance )
         </b><br><br>
         This function should be called after receiving Release event.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u32_CallInstance  call instance identifier

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallReleaseComplete(void *pv_AppRefHandle, u32 u32_CallInstance);

//*/
//    ==========  cmbs_dee_CallInbandInfo  ===========
/*!
      \brief
         Sends an in-band call information event.

         <h2>cmbs_dee_CallInbandInfo</h2>

      <h3>Introduction</h3>
         Sends an in-band call information event.

      <h3>Use cases</h3>
         Send In-Band Call event.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallInbandInfo ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function sends a call in-band info event.<br>
       <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, ,X,In case of Call Waiting}
      \TABR4C{CMBS_IE_CALLERNAME, ,X,In case of Call Waiting}
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , X In case of number matching in Contact list for outgoing calls}
      \TABR4C{CMBS_IE_CALLINFO, X, , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, ,X,In case of Call Waiting}
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallInbandInfo(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallMediaOffer  ===========
/*!
      \brief
         Send media offering.

         <h2>cmbs_dee_CallMediaOffer</h2>

      <h3>Introduction</h3>
         Send media offering.

      <h3>Use cases</h3>
         When media is to be renegotiated.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallMediaOffer ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function sends a media offer event with media descriptor IE.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, X, , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      \return           Return code

      \see              CMBS_EV_DEE_CALL_MEDIA_OFFER_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallMediaOffer(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallMediaOfferRes  ===========
/*!
      \brief
         Sends media offer response.

         <h2>cmbs_dee_CallMediaOfferRes</h2>

      <h3>Introduction</h3>
         Sends media offer response.

      <h3>Use cases</h3>
         Send a response to Media Offer event specifying the desired media descriptor.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallMediaOfferRes ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function sends a call media offer response.<br>
         Application has selected a particular media for this call.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, X, , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallMediaOfferRes(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallHold  ===========
/*!
      \brief
         Sends a call hold event.

         <h2>cmbs_dee_CallHold</h2>

      <h3>Introduction</h3>
         Sends a call hold event.

      <h3>Use cases</h3>
         Puts the current call on hold.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallHold ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function sends a call hold event, to put the current call on hold.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , X, }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallHold(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallResume  ===========
/*!
      \brief
         Sends a call resume event.

         <h2>cmbs_dee_CallResume</h2>

      <h3>Introduction</h3>
         Sends a call resume event.

      <h3>Use cases</h3>
         Resumes the previously held call.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_CallResume ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function sends a call resume event.<br>
         Used after a call hold.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , X, }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallResume(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_MergeCalls  ===========
/*!
      \brief
         Sends merge call event.

         <h2>cmbs_dee_MergeCalls</h2>

      <h3>Introduction</h3>
         Sends merge calls event.

      <h3>Use cases</h3>
         Merge two calls in one. If a callid has two party involved, then it will become muti party conference.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dee_MergeCalls ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         This function sends merge calls event.<br>
         Used to merge two or more party in to a call.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, X, , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, , , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , X, }
      \TABEND

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_MergeCalls(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dee_CallHoldRes  ===========
/*!
      \brief
         Sends a call hold response event.

         <h2>cmbs_dee_CallHoldRes</h2>

      <h3>Introduction</h3>
         Sends a call hold response event.

      <h3>Use cases</h3>
         Accept or not call hold.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dee_CallHoldRes( void * pv_AppRefHandle, u32 u32_CallInstance, E_CMBS_RESPONSE e_Response )
         </b><br><br>
         This function sends a call hold event, to put the current call on hold.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u32_CallInstance  call instance
      \param[in]        u16_HsNum         HS number
      \param[in]        e_Response        response

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallHoldRes(void *pv_AppRefHandle, u32 u32_CallInstance, u16 u16_HsNum, E_CMBS_RESPONSE e_Response);

//*/
//    ==========  cmbs_dee_CallResumeRes  ===========
/*!
      \brief
         Sends a call resume response event.

         <h2>cmbs_dee_CallResumeRes</h2>

      <h3>Introduction</h3>
         Sends a call resume response event.

      <h3>Use cases</h3>
         Accept or not call resume.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dee_CallResumeRes( void * pv_AppRefHandle, u32 u32_CallInstance, u16 u16_HsNum, E_CMBS_RESPONSE e_Response )
         </b><br><br>
         This function sends a call resume event.<br>
         Used after a call hold.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u32_CallInstance  call instance
      \param[in]        u16_HsNum         HS number
      \param[in]        e_Response        response

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallResumeRes(void *pv_AppRefHandle, u32 u32_CallInstance, u16 u16_HsNum, E_CMBS_RESPONSE e_Response);


/*
*/
E_CMBS_RC         cmbs_dsr_HsLocProgress(void *pv_AppRefHandle, void *pv_RefIEList);

/*
*/
E_CMBS_RC         cmbs_dsr_HsLocAnswer(void *pv_AppRefHandle, void *pv_RefIEList);

/*
*/
E_CMBS_RC         cmbs_dsr_HsLocRelease(void *pv_AppRefHandle, void *pv_RefIEList);


/*****************************************************************************
 * DEM (external media) functions
 *****************************************************************************/

//*/
//    ==========  cmbs_dem_ChannelStart  ===========
/*!
      \brief
         Start the media channel.

         <h2>cmbs_dem_ChannelStart</h2>

      <h3>Introduction</h3>
         Start the media channel.

      <h3>Use cases</h3>
         Called when media needs to be started.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dem_ChannelStart ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         Application wants to start the media.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, , , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      NOTE: Media Channel received in one of the following callback events: <br>
         @li CMBS_EV_DEE_CALL_PROGRESS,
         @li CMBS_EV_DEE_CALL_ANSWER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER_RES,
         @li CMBS_EV_DEE_CALL_MEDIA_UPDATE

      \return           Return code

      \see              CMBS_EV_DEM_CHANNEL_START_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dem_ChannelStart(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dem_ChannelStop  ===========
/*!
      \brief
         Stops media channel.

         <h2>cmbs_dem_ChannelStop</h2>

      <h3>Introduction</h3>
         Stops media channel.

      <h3>Use cases</h3>
         When media is to be stopped.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dem_ChannelStop ( void * pv_AppRefHandle, u32 u32_ChannelID )
         </b><br><br>
         Application wants to stop media flow.<br>
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, , , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      NOTE: Media Channel received in one of the following callback events: <br>
         @li CMBS_EV_DEE_CALL_PROGRESS,
         @li CMBS_EV_DEE_CALL_ANSWER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER_RES,
         @li CMBS_EV_DEE_CALL_MEDIA_UPDATE

      \return           Return code

      \see              CMBS_EV_DEM_CHANNEL_STOP_RES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dem_ChannelStop(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dem_ChannelInternalConnect  ===========
/*!
      \brief
         Connect or disconnect a channel to a internal context 

         <h2>cmbs_dem_ChannelInternalConnect</h2>

      <h3>Introduction</h3>
         Connect or disconnect a channel to a internal context to make internal media connnection

      <h3>Use cases</h3>
         to make internal media connnection

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dem_ChannelInternalConnect( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         Application wants to make or release internal media connection 
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIA_INTERNAL_CONNECT, X, , Contains Channel ID and the target context}
      \TABEND

      Media Channel received in one of the following callback events: <br>
         @li CMBS_EV_DEE_CALL_PROGRESS,
         @li CMBS_EV_DEE_CALL_ANSWER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER_RES,
         @li CMBS_EV_DEE_CALL_MEDIA_UPDATE

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC         cmbs_dem_ChannelInternalConnect(void *pv_AppRefHandle, void *pv_RefIEList);


//*/
//    ==========  cmbs_dem_ToneStart  ===========
/*!
      \brief
         Start the tone generation on CMBS.

         <h2>cmbs_dem_ToneStart</h2>

      <h3>Introduction</h3>
         Start the tone generation on CMBS.

      <h3>Use cases</h3>
         To generate dial-tone, call waiting tone, ring back-tone, etc.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dem_ToneStart ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         Application wants to start the tone generation.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, , , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TONE, X, ,Contains the tone properties}
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      NOTE: The IE Tone select pre-defined tones according country specification,
            e.g. dial-tone, ring back tone, busy tone, hint tone, call waiting tone, etc.<br>

      Media Channel received in one of the following callback events: <br>
         @li CMBS_EV_DEE_CALL_PROGRESS,
         @li CMBS_EV_DEE_CALL_ANSWER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER_RES,
         @li CMBS_EV_DEE_CALL_MEDIA_UPDATE

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dem_ToneStart(void *pv_AppRefHandle, void *pv_RefIEList);

//*/
//    ==========  cmbs_dem_ToneStop  ===========
/*!
      \brief
         Stops CMBS tone generator.

         <h2>cmbs_dem_ToneStop</h2>

      <h3>Introduction</h3>
         Stops CMBS tone generator.

      <h3>Use cases</h3>
         When the destination has picked up the call and the CMBS was generating
         the ring-back tone, the application shall stop the tone generation.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dem_ToneStop ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br><br>
         Application wants to stop the tone generation.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference to IE List

      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_CALLINSTANCE, , , }
      \TABR4C{CMBS_IE_CALLERPARTY, , , }
      \TABR4C{CMBS_IE_CALLERNAME, , , }
      \TABR4C{CMBS_IE_CALLEDPARTY, , , }
      \TABR4C{CMBS_IE_CALLEDNAME, , , }
      \TABR4C{CMBS_IE_CALLINFO, , , }
      \TABR4C{CMBS_IE_CALLPROGRESS, , , }
      \TABR4C{CMBS_IE_CALLRELEASE_REASON, , , }
      \TABR4C{CMBS_IE_DISPLAY_STRING, , , }
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_MEDIADESCRIPTOR, , , }
      \TABR4C{CMBS_IE_TONE, , , }
      \TABR4C{CMBS_IE_TIMEOFDAY, , , }
      \TABEND

      NOTE: Media Channel received in one of the following callback events: <br>
         @li CMBS_EV_DEE_CALL_PROGRESS,
         @li CMBS_EV_DEE_CALL_ANSWER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER,
         @li CMBS_EV_DEE_CALL_MEDIA_OFFER_RES,
         @li CMBS_EV_DEE_CALL_MEDIA_UPDATE

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dem_ToneStop(void *pv_AppRefHandle, void *pv_RefIEList);

/*** Coding examples ***/
/*
void              ApplicationEstablishCall( void * pv_AppRefHandle )
{
   u32            u32_CallInstance;
   void *         pv_RefIEList = NULL;
   ST_IE_CALLEDPARTY
                  st_CalledParty;
   ST_IE_CALLERPARTY
                  st_CallerParty;

   char           szCallerAddress[] = "+49891234567";
   char           szCalledAddress[] = "1";

   // Create a new Call ID
   u32_CallInstance = cmbs_dee_CallInstanceNew( pv_AppRefHandle );

   // Initialize IE List
   pv_RefIEList = cmbs_api_ie_GetList();

   if( pv_RefIEList )
   {
      // Add Call Instance IE
      cmbs_api_ie_CallInstanceAdd( pv_RefIEList, u32_CallInstance );

      // Add Called ID IE
      st_CalledParty.u8_AddressProperties = CMBS_ADDR_PROPTYPE_UNKNOWN | CMBS_ADDR_PROPPLAN_INTHS;
      st_CalledParty.u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
      st_CalledParty.u8_AddressLen = sizeof( szCalledAddress );
      st_CalledParty.pu8_Address = szCalledAddress;

      cmbs_api_ie_CalledPartyAdd( pv_RefIEList, &st_CalledParty );

      // Add Caller ID IE
      st_CallerParty.u8_AddressProperties = CMBS_ADDR_PROPTYPE_INTERNATIONAL | CMBS_ADDR_PROPPLAN_E164;
      st_CallerParty.u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
      st_CallerParty.u8_AddressLen = sizeof( szCallerAddress );
      st_CallerParty.pu8_Address = szCallerAddress;

      cmbs_api_ie_CallerPartyAdd( pv_RefIEList, &st_CallerParty );
      .
      // add more Information Elements
      .

      // Establish the Call
      cmbs_dee_CallEstablish( pv_AppRefHandle, pv_RefIEList );

      // Housekeeping: free the IEList
      cmbs_api_ie_FreeList( pv_RefIEList );
   }
}



typedef struct _ST_EV_DEE
{
   u32  u32_InstanceID;
   pu8* pu8_IEBuffer;
   u16  u16_Length;
} ST_EV_DEE;

void  OnCallEstablish ( void * pv_Reference, ST_EV_DEE * pst_Param )
{
   E_CMBS_IE_TYPE e_IE;

   pCallInstance = CallInstanceFindByID ( pst_Param->u32_InstanceID );

   if ( !pCallInstance )
   {
         pCallInstance = CallInstanceNew ( pst_Param->u32_InstanceID );
   }

   pu8_Tmp = pu8_IEBuffer;
   pu8_End = pu8_IEBuffer + pst_Param->u16_Length;

   while ( pu8_Tmp < pu8_End )
   {
      e_IE = cmbs_api_ParamGetNext( pu8_Tmp, pu8_End, u16 * pu8_ContentLength, &e_Error );
      switch (e_IE )
      {
         case CMBS_IE_CALLERPARTY:
            cmbs_api_ie_CallerPartyGet( pu8_Tmp, pCallInstance->pst_CallerParty );
         break;
         case CMBS_IE_CALLEDPARTY:
            cmbs_api_ie_CalledPartyGet( pu8_Tmp, pCallInstance->pst_CalledParty );
         break;
         case CMBS_IE_MEDIA_TYPE:
         case CMBS_IE_MEDIA_AUDIODESCR:
         case CMBS_IE_MEDIA_IOM:
      }

           pTmp += *pu8_ContentLength
   }

   // call callmanager to provide establish request

}


E_CMBS_RC fct_CMBSReceiveCallback ( void * pv_Reference, E_CMBS_EVENT_ID e_EventID, void * pv_EventParam )
{
   switch ( e_EventID )
   {
      case CMBS_EV_DEE_CALL_ESTABLISH:
         OnCallEstablish ( pv_Reference, (ST_EV_DEE*)pv_EventParam );
      break;
   }
}
*/



//	  ==========  cmbs_dsr_suota_Init  ===========
/*!
\brief
Initialize suota 

<h2>cmbs_dsr_suota_Init</h2>

<h3>Introduction</h3>
Initialize suota

<h3>Use cases</h3>

<h3>API Functions description</h3>
<b>
E_CMBS_RC cmbs_dsr_suota_Init	 ( void *pv_AppRefHandle);
</b><br><br>
This function will Initialize suota
<br>
\param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
\return		 Return code

\see
<b>Sample Code:</b><br>
<code></code>
*/

E_CMBS_RC cmbs_dsr_suota_Init(void *pv_AppRefHandle);



//	  ==========  cmbs_dsr_suota_SendHSVersionAvail  ===========
/*!
      \brief
         Send Version Available message to specific HS. Hs Version indication should be received before.

         <h2>cmbs_dsr_suota_SendHSVersionAvail</h2>

      <h3>Introduction</h3>
         Send Version Available message to specific HS.

      <h3>Use cases</h3>
      

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_SendHSVersionAvail	 ( void *pv_AppRefHandle, 
                                                            ST_SUOTA_UPGRADE_DETAILS	pHSVerAvail, u16 u16_Handset, 
                                                            ST_VERSION_BUFFER* pst_SwVersion );
         </b><br><br>
         This function will Send Version Available message to specific HS.
        <br>
      \param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]	 pHSVerAvail		  pointer to structure with HS version available details
      \param[in]	 u16_Handsets		  bitmask for handsets: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                          only one handset ( one bit ) supported
      \param[in]	 pst_SwVersion		  pointer to Version string buffer
      \param[in]     u16_RequestId        host should put unique values as this will be returned in the response

      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_suota_SendHSVersionAvail(void *pv_AppRefHandle, ST_SUOTA_UPGRADE_DETAILS  pHSVerAvail, u16 u16_Handset, ST_VERSION_BUFFER *pst_SwVersion, u16 u16_RequestId);

//	  ==========  cmbs_dsr_suota_SendSWUpdateInd  ===========
/*!
      \brief
         Send SW Upd indication message to specific HS.

         <h2>cmbs_dsr_suota_SendSWUpdateInd</h2>

      <h3>Introduction</h3>
         Send SW Upd indication message to specific HS.

      <h3>Use cases</h3>
      

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_SendSWUpdateInd		 ( void *pv_AppRefHandle, u16 u16_Handset, E_SUOTA_SU_SubType enSubType);
         </b><br><br>
         This function will Send SW Upd indication message to specific HS.
        <br>

      \param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]	 u16_Handsets		  bitmask for handsets: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                          only one handset ( one bit ) supported
      \param[in]	 enSubType		  	  Type of available SW
      \param[in]     u16_RequestId        host should put unique values as this will be returned in the response

      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_suota_SendSWUpdateInd(void *pv_AppRefHandle, u16 u16_Handset, E_SUOTA_SU_SubType enSubType, u16 u16_RequestId);

//	  ==========  cmbs_dsr_suota_SendURL  ===========
/*!
      \brief
         Send URL message to specific HS.

         <h2>cmbs_dsr_suota_SendURL</h2>

      <h3>Introduction</h3>
         Send URL message to specific HS.

      <h3>Use cases</h3>
            Several URL messages could be sent after HS available version. This message contain location of SW update files.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_SendURL		( void *pv_AppRefHandle, u16 u16_Handset, u8	u8_URLToFollow,   ST_URL_BUFFER* pst_Url);
         </b><br><br>
         This function will Send URL message to specific HS.
        <br>

      \param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]	 u16_Handsets		  bitmask for handsets: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                          only one handset ( one bit ) supported
      \param[in]	 u8_URLToFollow		  number of URL messages, which will follow by given message
      \param[in]	 pst_Url		  	  pointer to URL address
      \param[in]     u16_RequestId        host should put unique values as this will be returned in the response

      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_suota_SendURL(void *pv_AppRefHandle, u16 u16_Handset, u8   u8_URLToFollow,   ST_URL_BUFFER *pst_Url, u16 u16_RequestId);

//	  ==========  cmbs_dsr_suota_SendNack  ===========
/*!
      \brief
         Send NACK message to specific HS.

         <h2>cmbs_dsr_suota_SendNack</h2>

      <h3>Introduction</h3>
         Send NACK message to specific HS.

      <h3>Use cases</h3>
        This message could be sent in any moment of SUOTA process, indicating that some error happens

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_SendNack		( void *pv_AppRefHandle, u16 u16_Handset, E_SUOTA_RejectReason RejectReason);
         </b><br><br>
         This function will Send NACK message to specific HS.
        <br>

      \param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]	 u16_Handsets		  bitmask for handsets: bit 0 = handset 1, bit 1 = handset 2, etc.<br>
                                          only one handset ( one bit ) supported
      \param[in]	 RejectReason		  Type of failture
      \param[in]     u16_RequestId        host should put unique values as this will be returned in the response

      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_suota_SendNack(void *pv_AppRefHandle, u16 u16_Handset, E_SUOTA_RejectReason RejectReason, u16 u16_RequestId);

//	  ==========  cmbs_dsr_suota_Session_Close  ===========
/*!
      \brief
         Send SUOTA Abort message to specific HS.

         <h2>cmbs_dsr_suota_Session_Close</h2>

      <h3>Introduction</h3>
         Send SUOTA Abort message to specific HS.

      <h3>Use cases</h3>
        This message can be sent at any moment during the SUOTA process, indicating that the process should be aborted.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_Session_Close		( void *pv_AppRefHandle, u32 u32_SessionId);
         </b><br><br>
         This function will Send Suota Abort message to specific HS.
        <br>

      \param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      
      \param[in]     u32 u32_SessionId    Session ID for Suota - set to 0

      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_suota_Session_Close(void *pv_AppRefHandle, u32 u32_SessionId);



//	  ==========  cmbs_dsr_suota_Unreg_App_CB  ===========
/*!
      \brief
         Unregister the application callback on CMBS target.

         <h2>cmbs_dsr_suota_Unreg_App_CB</h2>

      <h3>Introduction</h3>
         Unregister the application callback on CMBS target.

      <h3>Use cases</h3>
        Useful for projects who needs GMEP on target for SUOTA type 1 (e.g. for ULE device) and GMEP on host for SUOTA type 2 (e.g. for CAT-iq 3.0 handset).

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_Unreg_App_CB		( void );
         </b><br><br>
         This function will Send Unreg_App_CB message.
        <br>

      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_suota_Unreg_App_CB(void);


//	  ==========  cmbs_dsr_suota_DataSend  ===========
/*!
      \brief
         Send Suota data.

         <h2>cmbs_dsr_suota_DataSend</h2>

      <h3>Introduction</h3>
         Send SUOTA File Data to specific HS.

      <h3>Use cases</h3>
        This message is sent while transfering file to HS

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_DataSend		( void *pv_AppRefHandle, u32 u32_Handset, u16 u16_RequestId);
         </b><br><br>
         This function will Send Data to specific HS.
        <br>

      \param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]	 u32_appId
      \param[in]	 u32_SessionId		  Session used to transfer the data
      \param[in]	 pSdu		  		  SDU - buffer to send
      \param[in]	 u32_SduLength 		  SDU - buffer length
      \param[in]     u16_RequestId        host should put unique values as this will be returned in the response

      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_suota_DataSend(void *pv_AppRefHandle, u32 u32_appId, u32 u32_SessionId,
                                  char *pSdu, u32 u32_SduLength, u16 u16_RequestId);

//	  ==========  cmbs_dsr_suota_ExtendedDataSend  ===========
/*!
      \brief
         Send Suota data.

         <h2>cmbs_dsr_suota_ExtendedDataSend</h2>

      <h3>Introduction</h3>
         Send SUOTA File Data to specific HS.

      <h3>Use cases</h3>
        This message is sent while transfering file to HS

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_suota_ExtendedDataSend		( void *pv_AppRefHandle, u32 u32_appId, u32 u32_SessionId,
								   char *pSdu, u32 u32_SduLength,u16 u16_RequestId, PST_IE_SUOTA_FILE_INFO pst_FileInfo);
         </b><br><br>
         This function will Send Data to specific HS.
        <br>

      \param[in]	 pv_AppRefHandle	  reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]	 u32_appId
      \param[in]	 u32_SessionId		  Session used to transfer the data
      \param[in]	 pSdu		  		  SDU - buffer to send
      \param[in]	 u32_SduLength 		  SDU - buffer length
      \param[in]     u16_RequestId        host should put unique values as this will be returned in the response
      \param[in]     PST_IE_SUOTA_FILE_INFO        Information of the transfered file
      \return		 Return code

      \see			 

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC cmbs_dsr_suota_ExtendedDataSend (void *pv_AppRefHandle, u32 u32_appId, u32 u32_SessionId,
								   char *pSdu, u32 u32_SduLength,u16 u16_RequestId, PST_IE_SUOTA_FILE_INFO pst_FileInfo);
//*/
//    ==========  cmbs_dsr_DectSettingsList_Get  ===========
/*!
      \brief
         Request information about DECT settings.

         <h2>cmbs_dsr_DectSettingsList_Get</h2>

      <h3>Introduction</h3>
         DECT settings

      <h3>Use cases</h3>
         Get DECT settings.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_DectSettingsList_Get( void * pv_AppRefHandle )
         </b><br><br>
         This function requests the base station to get information about DECT settings.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_DectSettingsList_Get(void *pv_AppRefHandle);


//*/
//    ==========  cmbs_dsr_DectSettingsList_Set  ===========
/*!
      \brief
         Set information about DECT settings.

         <h2>cmbs_dsr_DectSettingsList_Set</h2>

      <h3>Introduction</h3>
         List of DECT settings

      <h3>Use cases</h3>
         Set list of DECT settings.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_DectSettingsList_Set ( void * pv_AppRefHandle,
                                                            ST_IE_DECT_SETTINGS_LIST* pst_DectSettings )
         </b><br><br>
         This function send to the base station to list of line settings that must be set.
        <br>
      \param[in]        pv_AppRefHandle           reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pst_LineSettingsList      pointer to ST_IE_DECT_SETTINGS_LIST structure
      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC         cmbs_dsr_DectSettingsList_Set(void *pv_AppRefHandle, ST_IE_DECT_SETTINGS_LIST *pst_DectSettings);

//*/
//    ==========  cmbs_dsr_PropDectSettingsList_Set  ===========
/*!
      \brief
         Set information about DECT settings including proprietary fields

         <h2>cmbs_dsr_DectSettingsList_Set</h2>

      <h3>Introduction</h3>
         List of DECT settings, standard and proprietary elements

      <h3>Use cases</h3>
         Set list of DECT settings.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dsr_PropDectSettingsList_Set ( void * pv_AppRefHandle,
                                                            ST_IE_DECT_SETTINGS_LIST* pst_DectSettings 
                                                            ST_IE_PROP_FIELDS_LIST* pst_PropSettings)
         </b><br><br>
         This function send to the base station to list of line settings that must be set.
        <br>
      \param[in]        pv_AppRefHandle           reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pst_DectSettings          pointer to ST_IE_DECT_SETTINGS_LIST structure
      \param[in]        pst_PropSettings      pointer to ST_IE_PROP_FIELDS_LIST structure
     \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_PropDectSettingsList_Set(void *pv_AppRefHandle, ST_IE_DECT_SETTINGS_LIST *pst_DectSettings, ST_IE_PROP_FIELDS_LIST* pst_PropSettings );

E_CMBS_RC cmbs_dsr_AddNewExtension(void *pv_AppRefHandle, ST_IE_SUBSCRIBED_HS_LIST *pst_ExtensionData);

E_CMBS_RC cmbs_dsr_SetBaseName(void *pv_AppRefHandle, ST_IE_BASE_NAME *pst_BaseName);

E_CMBS_RC cmbs_dee_HsCodecCfmFailed(void *pv_AppRefHandle, void *pv_RefIEList);


//    ==========  cmbs_rtp_SessionStart  ===========
/*!
      \brief
         Start a RTP session.

      <h2>cmbs_rtp_SessionStart</h2>

      <h3>Introduction</h3>
         Start a RTP session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtp_SessionStart ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a RTP session start request to the CMBS target. The CMBS host has to wait until CMBS_EV_RTP_SESSION_START_RES is received.
         <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List
         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_RTP_SESSION_INFORMATION, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtp_SessionStart(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_rtp_SessionStop  ===========
/*!
      \brief
         Stop an ongoing RTP session.

      <h2>cmbs_rtp_SessionStop</h2>

      <h3>Introduction</h3>
         Stop an ongoing RTP session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtp_SessionStop ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a RTP session stop request to the CMBS target. The CMBS host has to wait for CMBS_EV_RTP_SESSION_STOP_RES before doing further RTP operations.
         <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List

         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtp_SessionStop(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_rtp_SessionUpdate  ===========
/*!
      \brief
         Update a RTP session.

      <h2>cmbs_rtp_SessionUpdate</h2>

      <h3>Introduction</h3>
         Update a RTP session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtp_SessionUpdate ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a RTP session update request to the CMBS target. The response event CMBS_EV_RTP_SESSION_UPDATE_RES will provide the result.
         <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List

         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_RTP_SESSION_INFORMATION, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtp_SessionUpdate(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_rtcp_SessionStart  ===========
/*!
      \brief
         Start a RTCP session.

      <h2>cmbs_rtcp_SessionStart</h2>

      <h3>Introduction</h3>
         Start a RTCP session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtcp_SessionStart ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a RTCP session start request to the CMBS target. The response event CMBS_EV_RTCP_SESSION_START_RES will provide the result.
         <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List

         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_RTCP_INTERVAL, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtcp_SessionStart(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_rtcp_SessionStop  ===========
/*!
      \brief
         Stop an ongoing RTCP session.

      <h2>cmbs_rtcp_SessionStop</h2>

      <h3>Introduction</h3>
         Stop an ongoing RTCP session.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtcp_SessionStop ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a RTCP session stop request to the CMBS target. The response event CMBS_EV_RTCP_SESSION_STOP_RES will provide the result.
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List

         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtcp_SessionStop(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_rtp_SendDTMF  ===========
/*!
      \brief
         Send RTP DTMF.

      <h2>cmbs_rtp_SendDTMF</h2>

      <h3>Introduction</h3>
         Send RTP DTMF.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtp_SendDTMF ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a RTP send DTMF request to the CMBS target. The RTP capabilities have to be properly configured during a start RTP session for selecting either the RFC 2833 (Out-of-band) or inband mode of DTMF. It is assumed that transmitting DTMF using SIP INFO will be managed by the HOST itself. The response event CMBS_EV_RTP_SEND_DTMF_RES will provide the result.
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List

         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABR4C{CMBS_IE_RTP_DTMF_EVENT, X, , }
      \TABR4C{CMBS_IE_RTP_DTMF_EVENT_INFO, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtp_SendDTMF(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_rtp_EnableFaxAudioProcessingMode  ===========
/*!
      \brief
         Switch off Audio Processing during a FAX call.

      <h2>cmbs_rtp_EnableFaxAudioProcessingMode</h2>

      <h3>Introduction</h3>
         Switch off Audio Processing during a FAX call.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtp_EnableFaxAudioProcessingMode ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a request to the CMBS target to switch off Audio Processing during a FAX call. This essentially switches off the LEC (and possibly OTF) in the FXS flow in order to receive a FAX properly. CMBS_EV_RTP_ENABLE_FAX_AUDIO_PROCESSING_MODE_RES will provide the result.
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List

         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtp_EnableFaxAudioProcessingMode(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_rtp_DisableFaxAudioProcessingMode  ===========
/*!
      \brief
         Switch on Audio Processing after a FAX call.

      <h2>cmbs_rtp_DisableFaxAudioProcessingMode</h2>

      <h3>Introduction</h3>
         Switch on Audio Processing after a FAX call.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_rtp_DisableFaxAudioProcessingMode ( void * pv_AppRefHandle, void * pv_RefIEList )
         </b><br/><br/>
         This function sends a request to the CMBS target to switch on Audio Processing after a FAX call when normal voice conversation is resumed. This essentially switches on the LEC (and possibly OTF) in the FXS flow. CMBS_EV_RTP_DISABLE_FAX_AUDIO_PROCESSING_MODE_RES will provide the result.
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        pv_RefIEList      reference pointer to IE List

         <br/><b>IE:</b><br/>
      \TABSTART{Information Elements:}
      \TABR4C{CMBS_IE_MEDIACHANNEL, X, , }
      \TABEND

      \return           Return code
*/

E_CMBS_RC   cmbs_rtp_DisableFaxAudioProcessingMode(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_dsr_GetEepromSize  ===========
/*!
      \brief
         Request target for eeprom size

      <h2>cmbs_dsr_GetEepromSize</h2>

      <h3>Introduction</h3>
         Request target for eeprom size

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC	cmbs_dsr_GetEepromSize( void * pv_AppRefHandle)
         </b><br/><br/>
         This function sends a request to the CMBS target for eeprom size
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC	cmbs_dsr_GetEepromSize(void *pv_AppRefHandle);


//    ==========  cmbs_dsr_GetEepromSize  ===========
/*!
      \brief
         Request target for eeprom size

      <h2>cmbs_dsr_GetEepromExtSize</h2>

      <h3>Introduction</h3>
         Request target for eeprom size

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC	cmbs_dsr_GetEepromExtSize( void * pv_AppRefHandle)
         </b><br/><br/>
         This function sends a request to the CMBS target for eeprom size
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC	cmbs_dsr_GetEepromExtSize(void *pv_AppRefHandle);

//    ==========  cmbs_dsr_GetBaseName  ===========
/*!
      \brief
         Host wants to retrieve the current name of the DECT base station

      <h2>cmbs_dsr_GetBaseName</h2>

      <h3>Introduction</h3>
         Host wants to retrieve the current name of the DECT base station

      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_GetBaseName(void * pv_AppRefHandle);
         </b><br/><br/>
         This function sends a request to the CMBS target for base name
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_GetBaseName(void *pv_AppRefHandle);


//    ==========  cmbs_dsr_ReconnectResp  ===========
/*!
      \brief
         Confirm reconnect request to target

      <h2>cmbs_dsr_ReconnectResp</h2>

      <h3>Introduction</h3>
         Confirm reconnect request to target

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_ReconnectResp( void * pv_AppRefHandle)
         </b><br/><br/>
         This function sends a reconnect response to target
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_ReconnectResp(void *pv_AppRefHandle);

//    ==========  cmbs_api_ie_RegCloseReasonGet  ===========
/*!
      \brief
          Get CMBS_REG_CLOSE_REASON Information Element from IE List element

      <h2>cmbs_api_ie_RegCloseReasonGet</h2>

      <h3>Introduction</h3>
         Get CMBS_REG_CLOSE_REASON Information Element from IE List element

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RegCloseReasonGet( void * pv_RefIE, ST_IE_REG_CLOSE_REASON * st_Reg_Close_Reason)
         </b><br/><br/>
         This function gets close reason response
         <br/>
      \param[in]        pv_RefIE     pointer to IE list element
      \param[in,out]    st_Reg_Close_Reason      pointer to Close reason structure


         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_api_ie_RegCloseReasonGet(void *pv_RefIE, ST_IE_REG_CLOSE_REASON *st_Reg_Close_Reason);

//    ==========  cmbs_api_ie_RegEnableTypeAdd  ===========
/*!
      \brief
          Add CMBS_IE_REG_ENABLE_TYPE Information Element to IE List element

      <h2>cmbs_api_ie_RegEnableTypeAdd</h2>

      <h3>Introduction</h3>
         Add E_CMBS_HS_REGISTER_ENABLE Information Element tp IE List element

      <h3>API Functions description</h3>
         <b>
         cmbs_api_ie_RegEnableTypeAdd(void *pv_RefIE, E_CMBS_HS_REGISTER_ENABLE e_RegEnable)
         </b><br/><br/>
         This function adds register enable type IE
         <br/>
      \param[in]        pv_RefIE     pointer to IE list element
      \param[in]        e_RegEnable  type of registration open (ULE and/or DECT handsets)


         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_api_ie_RegEnableTypeAdd(void *pv_RefIE, E_CMBS_HS_REGISTER_ENABLE e_RegEnable);

//    ==========  cmbs_api_ie_RegEnableTypeGet  ===========
/*!
      \brief
          Get CMBS_IE_REG_ENABLE_TYPE Information Element from IE List element

      <h2>cmbs_api_ie_RegEnableTypeGet</h2>

      <h3>Introduction</h3>
         Get CMBS_IE_REG_ENABLE_TYPE Information Element from IE List element

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_RegEnableTypeGet(void *pv_RefIE, u8 *u8_RegEnable)
         </b><br/><br/>
         This function gets close reason response
         <br/>
      \param[in]        pv_RefIE     pointer to IE list element
      \param[in,out]    u8_RegEnable pointer to type of registration open (ULE and/or DECT handsets)


         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_api_ie_RegEnableTypeGet(void *pv_RefIE, u8 *u8_RegEnable);


//    ==========  cmbs_api_ie_ChecksumErrorAdd  ===========
/*!
      \brief
         Add CMBS_IE_CHECKSUM_ERROR IE to event

      <h2>cmbs_api_ie_ChecksumErrorAdd</h2>

      <h3>Introduction</h3>
         Checksum error detected in IEs of an event

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ChecksumErrorAdd( void * pv_RefIEList, PST_IE_CHECKSUM_ERROR  pst_CheckSumError)
         </b><br/><br/>
         This function adds the checksum error IE
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_api_ie_ChecksumErrorAdd(void *pv_RefIEList, PST_IE_CHECKSUM_ERROR  pst_CheckSumError);

//    ==========  cmbs_api_ie_ChecksumErrorGet  ===========
/*!
      \brief
          Get CMBS_IE_CHECKSUM_ERROR Information Element from IE List element

      <h2>cmbs_api_ie_ChecksumErrorAdd</h2>

      <h3>Introduction</h3>
         Get CMBS_IE_CHECKSUM_ERROR Information Element from IE List element

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_api_ie_ChecksumErrorGet( void * pv_RefIE, PST_IE_CHECKSUM_ERROR  pst_CheckSumError )
         </b><br/><br/>
          The function extracts the checksum error IE data from the List element.<br/>
      \param[in]        pv_RefIE     pointer to IE list element
      \param[in,out]    pst_CheckSumError      pointer to checksum error structure


         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_api_ie_ChecksumErrorGet(void *pv_RefIE, PST_IE_CHECKSUM_ERROR  pst_CheckSumError);

//    ==========  cmbs_dsr_StartDectLogger  ===========
/*!
      \brief
         Start DECT logger engine.

         <h2>cmbs_dsr_StartDectLogger</h2>

      <h3>Introduction</h3>
         Start DECT logger engine.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_StartDectLogger( void * pv_AppRefHandle )
         </b><br><br>
         This function start DECT logger engine.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC cmbs_api_ie_HSReleaseReasonAdd(void *pv_RefIEList, ST_IE_HS_RELEASE_REASON *pst_RelReason);
E_CMBS_RC cmbs_api_ie_HSReleaseReasonGet(void *pv_RefIE, ST_IE_HS_RELEASE_REASON *pst_RelReason);

E_CMBS_RC		  cmbs_dsr_StartDectLogger(void *pv_AppRefHandle);
//*/
//    ==========  cmbs_dsr_StopDectLoggerAndRead  ===========
/*!
      \brief
         Stop DECT logger engine on target and initiate reading logged messages.

         <h2>cmbs_dsr_StopDectLoggerAndRead</h2>

      <h3>Introduction</h3>
         Stop DECT logger engine on target and initiate reading logged messages.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_StopDectLoggerAndRead ( void * pv_AppRefHandle )
         </b><br><br>
         This function stops DECT logger engine on target and initiate reading logged messages.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC		  cmbs_dsr_StopDectLoggerAndRead(void *pv_AppRefHandle);
//*/
//    ==========  cmbs_dsr_StopDectLoggerAndRead  ===========
/*!
      \brief
         Send response on data ind message from host.

         <h2>cmbs_dsr_DectLoggerDataIndRes</h2>

      <h3>Introduction</h3>
         Send response on data ind message from host.

      <h3>Use cases</h3>
         Debugging.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_DectLoggerDataIndRes ( void * pv_AppRefHandle )
         </b><br><br>
         This function sends response on data ind message from host.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

      \return           Return code

      \see              

      <b>Sample Code:</b><br>
      <code></code>
*/

E_CMBS_RC cmbs_dsr_DectLoggerDataIndRes(void *pv_AppRefHandle);

//    ==========  cmbs_dsr_dc_SessionStart  ===========
/*!
      \brief
       Host request to start a Data Call Session 

      <h2>cmbs_dsr_dc_SessionStart</h2>

      <h3>Introduction</h3>
       Host is initiator of the Data Call session 

      <h3>API Functions description</h3>
         <b>
      E_CMBS_RC cmbs_dsr_dc_SessionStart (void *pv_AppRefHandle, u8 u8_HsNr)         </b><br/><br/>
         This function sends CMBS_EV_DSR_DC_SESSION_START 
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
    u8_HsNr	Destination HS
         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_dc_SessionStart(void *pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance);
//    ==========  cmbs_dsr_dc_SessionStop  ===========
/*!
      \brief
         Host request to stop a Data Call Session 

      <h2>cmbs_dsr_dc_SessionStop</h2>

      <h3>Introduction</h3>
         Host is initiator of the Data Call session conclusion

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_dc_SessionStop( void * pv_AppRefHandle, u32 u32_CallInstance)
         </b><br/><br/>
         This function sends CMBS_EV_DSR_DC_SESSION_STOP to target
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in] 	u8_HsNr	Destination HS
           [in]	u8_CallId	Call Instance identifier

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_dc_SessionStop(void *pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance);
//    ==========  cmbs_dsr_dc_DataSend  ===========
/*!
      \brief
         Host request to send data in a Data Call Session. 
         There is a limitation of the data size - no more than 50 bytes can be transferred in one IWU message

      <h2>cmbs_dsr_dc_DataSend</h2>

      <h3>Introduction</h3>
         Confirm reconnect request to target

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_dc_DataSend( void * pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance, u8 *pu8_IWUData, u8 u8_DataLength)
         </b><br/><br/>
         This function sends CMBS_EV_DSR_DC_DATA_SEND to target
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
        [in] 	u8_HsNr	The originating handset
        [in]	u8_CallId	Call Instance identifier
        [in] 	pu8_IWUData	Pointer to IWU data

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_dc_DataSend(void *pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance, u8 *pu8_IWUData, u8 u8_DataLength);
//    ==========  cmbs_dsr_dc_SessionStartRes  ===========
/*!
      \brief
         Host response for a request to Data Call Session 

      <h2>cmbs_dsr_dc_SessionStartRes</h2>

      <h3>Introduction</h3>
         A handset is requesting to start a Data Call session and the Host responds to this request

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_dc_SessionStartRes ( void *pv_AppRefHandle, u8 u8_HsNr, u8 u8_CallId, bool Response, e_reject_reason reason)
         </b><br/><br/>
         This function sends CMBS_EV_DSR_DC_SESSION_START_RES to target
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()

         <br/><b>IE:</b><br/>
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_dc_SessionStartRes(void *pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance, bool b_Response);
//    ==========  cmbs_dsr_dc_SessionStopRes  ===========
/*!
      \brief
         Confirm reconnect request to target

      <h2>cmbs_dsr_dc_SessionStopRes</h2>

      <h3>Introduction</h3>
         Host response for a request to Data Call Session conclusion

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_dc_SessionStopRes( void * pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance, bool b_Response)
         </b><br/><br/>
         This function sends CMBS_EV_DSR_DC_SESSION_STOP_RES to target
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
        [in] 	u8_HsNr	Destination HS
        [in]	u8_CallId	Call Instance identifier
        [in] 	b_Response 	Confirm or Reject
        [in] 	reject_reason	Reject reason

         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_dc_SessionStopRes(void *pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance, bool b_Response);
//    ==========  cmbs_dsr_dc_DataSendRes  ===========
/*!
      \brief
         Confirm reconnect request to target

      <h2>cmbs_dsr_dc_DataSendRes</h2>

      <h3>Introduction</h3>
         A handset is requesting to send data in a Data Call session and the Host responds to this request

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_dc_SessionStopRes( void * pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance, bool b_Response)
         </b><br/><br/>
         This function sends CMBS_EV_DSR_DC_DATA_SEND_RES to target
         <br/>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
    [in] 	b_Response 	Confirm or Reject
         <br/><b>IE:</b><br/>
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_dc_DataSendRes(void *pv_AppRefHandle, u16 u16_HSNumber, u32 u32_CallInstance,  bool b_Response);


//    ==========  cmbs_dsr_Ping  ===========
/*!
      \brief
         Send ping request to the target

      <h2>cmbs_dsr_Ping</h2>

      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
    
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_Ping(void *pv_AppRefHandle);

//    ==========  cmbs_dsr_fw_AppInvalidate  ===========
/*!
      \brief
         Send application invalidation request to the target

      <h2>cmbs_dsr_fw_AppInvalidate</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
    
      \return           Return code
*/

E_CMBS_RC	cmbs_dsr_fw_AppInvalidate(void *pv_AppRefHandle);

//    ==========  cmbs_dsr_AFE_EndpointConnect  ===========
/*!
      \brief
         Notify target which two end points need to be connected

      <h2>cmbs_dsr_AFE_EndpointConnect</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	pst_AFEEndpointsConnect
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_AFE_EndpointConnect(void *pv_AppRefHandle, ST_IE_AFE_ENDPOINTS_CONNECT *pst_AFEEndpointsConnect);

//    ==========  cmbs_dsr_AFE_EndpointEnable  ===========
/*!
      \brief
         Enable AFE endpoint

      <h2>cmbs_dsr_AFE_EndpointEnable</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	pst_AEEndpoint
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_AFE_EndpointEnable(void *pv_AppRefHandle, ST_IE_AFE_ENDPOINT *pst_AEEndpoint);

//    ==========  cmbs_dsr_AFE_EndpointDisable  ===========
/*!
      \brief
         Disable AFE endpoint

      <h2>cmbs_dsr_AFE_EndpointDisable</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
          [in]	pst_AEEndpoint
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_AFE_EndpointDisable(void *pv_AppRefHandle, ST_IE_AFE_ENDPOINT *pst_AEEndpoint);

//    ==========  cmbs_dsr_AFE_SetEndpointGain  ===========
/*!
      \brief
        Define the gain to AFE end point (channel).

      <h2>cmbs_dsr_AFE_SetEndpointGain</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
             [in]	pst_AFEEndpointGain
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_AFE_SetEndpointGain(void *pv_AppRefHandle, ST_IE_AFE_ENDPOINT_GAIN *pst_AFEEndpointGain);

//    ==========  cmbs_dsr_AFE_AUXMeasurement  ===========
/*!
      \brief
         Define the input we need to measure, define manually measure/via BMP and activate measurement.

      <h2>cmbs_dsr_AFE_AUXMeasurement</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	pst_AUXMeasurementSettings
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_AFE_AUXMeasurement(void *pv_AppRefHandle, ST_IE_AFE_AUX_MEASUREMENT_SETTINGS *pst_AUXMeasurementSettings);

//    ==========  cmbs_dsr_AFE_ChannelAllocate  ===========
/*!
      \brief
         Allocate new channel on target side in order to open audio path

      <h2>cmbs_dsr_AFE_ChannelAllocate</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	u8_ResourceType	Type of the resource for which channel allocation is needed 
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_AFE_ChannelAllocate(void *pv_AppRefHandle, u8 u8_ResourceType);

//    ==========  cmbs_dsr_AFE_ChannelDeallocate  ===========
/*!
      \brief
         Release channel on target side in order to close audio path

      <h2>cmbs_dsr_AFE_ChannelDeallocate</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	pst_MediaChannel	Channel ID to release  
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_AFE_ChannelDeallocate(void *pv_AppRefHandle, ST_IE_MEDIA_CHANNEL *pst_MediaChannel);

//    ==========  cmbs_dsr_DHSGValueSend  ===========
/*!
      \brief
         Send value to set DHSG

      <h2>cmbs_dsr_DHSGValueSend</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	u8_DHSGValue	DHSG value to set   
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_DHSGValueSend(void *pv_AppRefHandle, u8 u8_DHSGValue);

//    ==========  cmbs_dsr_DHSGNewValueRcv  ===========
/*!
      \brief
         Unsolicited event to pass the DHSG data to the host

      <h2>cmbs_dsr_DHSGNewValueRcv</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	u8_DHSGValue	Current DHSG value    
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_DHSGNewValueRcv(void *pv_AppRefHandle, u8 u8_DHSGValue);

//    ==========  cmbs_dsr_GPIOEnable  ===========
/*!
      \brief
         Enable specified GPIO

      <h2>cmbs_dsr_GPIOEnable</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	st_GPIOId		pointer to GPIO Bank and number     
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_GPIOEnable(void *pv_AppRefHandle, PST_IE_GPIO_ID st_GPIOId);

//    ==========  cmbs_dsr_GPIODisable  ===========
/*!
      \brief
         Disable specified GPIO

      <h2>cmbs_dsr_GPIODisable</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	st_GPIOId		pointer to GPIO Bank and number     
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_GPIODisable(void *pv_AppRefHandle, PST_IE_GPIO_ID st_GPIOId);

//    ==========  cmbs_dsr_GPIOConfigSet  ===========
/*!
      \brief
         Set configuration for specified GPIO.

      <h2>cmbs_dsr_GPIOConfigSet</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	pv_RefIEList		fields that need to be configured   
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_GPIOConfigSet(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_dsr_GPIOConfigGet  ===========
/*!
      \brief
         Get configuration for specified GPIO.

      <h2>cmbs_dsr_GPIOConfigGet</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	pv_RefIEList		fields that need to be read   
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_GPIOConfigGet(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_dsr_ExtIntConfigure  ===========
/*!
      \brief
         Set configuration for specified external interrupt (level and type)

      <h2>cmbs_dsr_ExtIntConfigure</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	st_GPIOId		fields that need to be read   
           [in]	st_Configuration	GPIO bank and number
           [in]	u8_IntNumber		External interrupt level and type
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_ExtIntConfigure(void *pv_AppRefHandle, PST_IE_GPIO_ID st_GPIOId, PST_IE_INT_CONFIGURATION st_Configuration, u8 u8_IntNumber);

//    ==========  cmbs_dsr_ExtIntEnable  ===========
/*!
      \brief
         Enable specified external interrupt

      <h2>cmbs_dsr_ExtIntEnable</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	u8_IntNumber		External interrupt number   
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_ExtIntEnable(void *pv_AppRefHandle, u8 u8_IntNumber);

//    ==========  cmbs_dsr_ExtIntDisable  ===========
/*!
      \brief
         Disable specified external interrupt

      <h2>cmbs_dsr_ExtIntDisable</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	u8_IntNumber		External interrupt number   
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_ExtIntDisable(void *pv_AppRefHandle, u8 u8_IntNumber);

//    ==========  cmbs_dsr_ExtIntIndication  ===========
/*!
      \brief
         Unsolicited event to indicate external interrupt occurred on specific GPIO

      <h2>cmbs_dsr_ExtIntIndication</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	u8_IntNumber		External interrupt number   
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_ExtIntIndication(void *pv_AppRefHandle, u8 u8_IntNumber);

//    ==========  cmbs_dsr_LocateSuggestReq  ===========
/*!
      \brief
         This event enforces the HS to perform "locate"

      <h2>cmbs_dsr_LocateSuggestReq</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	u16_Handsets		HS mask   
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_LocateSuggestReq(void *pv_AppRefHandle, u16 u16_Handsets);

//    ==========  cmbs_dsr_TerminalCapabilitiesInd  ===========
/*!
      \brief
         Unsolicited event from target side to transfer HS capabilities to the Host

      <h2>cmbs_dsr_TerminalCapabilitiesInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	PST_IE_TERMINAL_CAPABILITIES		Terminal Capabilities
           [in]     u8  u8_HSNum
      \return           Return code
*/

//    ==========  cmbs_dsr_InfoSuggest  ===========
/*!
      \brief
         This event enforces the HS to perform "locate request", compared to cmbs_dsr_LocateSuggestReq it includes additionally also data

      <h2>cmbs_dsr_InfoSuggest</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u8_HsNumber  handset number for the locate request
      \param[in]      pu8_Data     pointer to data to be sent with the MM_INFO_SUGGEST
      \param[in]      u16_DataLen  number of bytes to be sent
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_InfoSuggest(void * pv_AppRefHandle, u8 u8_HsNumber, u8 * pu8_Data, u16 u16_DataLen );

//    ==========  cmbs_dsr_TerminalCapabilitiesInd  ===========
/*!
      \brief
         Unsolicited event from target side to transfer HS capabilities to the Host

      <h2>cmbs_dsr_TerminalCapabilitiesInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
           [in]	PST_IE_TERMINAL_CAPABILITIES		Terminal Capabilities
           [in]     u8  u8_HSNum
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_TerminalCapabilitiesInd(void *pv_AppRefHandle, PST_IE_TERMINAL_CAPABILITIES pst_TermCapability, u8 u8_HSNum);

//    ==========  cmbs_dem_TDMLoopbackStart  ===========
/*!
      \brief
         This event starts TDM loopback mode, all input data is copied to output

      <h2>cmbs_dem_TDMLoopbackStart</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \return           Return code
*/
E_CMBS_RC         cmbs_dem_TDMLoopbackStart(void *pv_AppRefHandle, void *pv_RefIEList);

//    ==========  cmbs_dem_TDMLoopbackStop  ===========
/*!
      \brief
         This event stops TDM loopback mode

      <h2>cmbs_dem_TDMLoopbackStop</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \return           Return code
*/
E_CMBS_RC         cmbs_dem_TDMLoopbackStop(void *pv_AppRefHandle, void *pv_RefIEList);
//    ==========  cmbs_dsr_gen_SendLineUseStatusInd  ===========
/*!
      \brief
         This event notifies regarding Line use status change

      <h2>cmbs_dsr_gen_SendLineUseStatusInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u16_RequestId
      \param[in]      u8_LineId
      \param[in]      e_LineStatus
      \param[in]      u16_Hs
	  
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_gen_SendLineUseStatusInd(void * pv_AppRefHandle, u16 u16_RequestId, u8 u8_LineId,  E_CMBS_LINE_DIAG_STATUS e_LineStatus, u16 u16_Hs );
//    ==========  cmbs_dsr_gen_SendHSUseStatusInd  ===========
/*!
      \brief
         This event notifies regarding Line use status change

      <h2>cmbs_dsr_gen_SendHSUseStatusInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u16_RequestId
      \param[in]      u8_LineId
      \param[in]      u16_Hs
      
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_gen_SendHSUseStatusInd(void * pv_AppRefHandle, u16 u16_RequestId, u8 u8_LineId, u16 u16_Hs);
//    ==========  cmbs_dsr_gen_SendDiagnosticStatusInd  ===========
/*!
      \brief
         This event notifies regarding Diagnostic status change

      <h2>cmbs_dsr_gen_SendDiagnosticStatusInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u16_RequestId
      \param[in]      u8_LineId
      \param[in]      e_LineStatus
      \param[in]      u16_Hs
     
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_gen_SendDiagnosticStatusInd(void * pv_AppRefHandle, u16 u16_RequestId, u8 u8_LineId, u8 u8_Type, u16 u16_Hs);

//    ==========  cmbs_dcm_CallTransferRes  ===========
/*!
      \brief
         
      <h2>cmbs_dcm_CallTransferRes</h2>

      \param[in]      
      \return           Return code
*/
E_CMBS_RC    	  cmbs_dcm_CallTransferRes( PST_IE_CALLTRANSFERREQ pst_TrfReq, PST_IE_RESPONSE pst_Resp);

//    ==========  cmbs_dcm_CallConferenceRes  ===========
/*!
      \brief
         
      <h2>cmbs_dcm_CallConferenceRes</h2>

      \param[in]      
      \return           Return code
*/
E_CMBS_RC    	  cmbs_dcm_CallConferenceRes( PST_IE_CALLTRANSFERREQ pst_TrfReq, PST_IE_RESPONSE pst_Resp );

//    ==========  cmbs_dsr_ParamAreaGetRes  ===========
/*!
      \brief
         
      <h2>cmbs_dsr_ParamAreaGetRes</h2>

      \param[in]      
      \return           Return code
*/
E_CMBS_RC 		  cmbs_dsr_ParamAreaGetRes(PST_IE_PARAMETER_AREA st_ParamArea, PST_IE_RESPONSE st_Resp);

//    ==========  cmbs_dsr_ParamAreaSetRes  ===========
/*!
      \brief
         
      <h2>cmbs_dsr_ParamAreaSetRes</h2>

      \param[in]      
      \return           Return code
*/
E_CMBS_RC 		  cmbs_dsr_ParamAreaSetRes(PST_IE_RESPONSE st_Resp);

//    ==========  cmbs_dsr_gen_SendLineUseStatusInd  ===========
/*!
      \brief
         This event notifies regarding Line use status change

      <h2>cmbs_dsr_gen_SendLineUseStatusInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u16_RequestId
      \param[in]      u8_LineId
      \param[in]      e_LineStatus
      \param[in]      u16_Hs
	  
      \return         Return code
*/
E_CMBS_RC cmbs_dsr_gen_SendLineUseStatusInd(void * pv_AppRefHandle, u16 u16_RequestId, u8 u8_LineId,  E_CMBS_LINE_DIAG_STATUS e_LineStatus, u16 u16_Hs );

//    ==========  cmbs_dsr_gen_SendHSUseStatusInd  ===========
/*!
      \brief
         This event notifies regarding Line use status change

      <h2>cmbs_dsr_gen_SendHSUseStatusInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u16_RequestId
      \param[in]      u8_LineId
      \param[in]      u16_Hs
      
      \return         Return code
*/
E_CMBS_RC cmbs_dsr_gen_SendHSUseStatusInd(void * pv_AppRefHandle, u16 u16_RequestId, u8 u8_LineId, u16 u16_Hs);

//    ==========  cmbs_dsr_gen_SendDiagnosticStatusInd  ===========
/*!
      \brief
         This event notifies regarding Diagnostic status change

      <h2>cmbs_dsr_gen_SendDiagnosticStatusInd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u16_RequestId
      \param[in]      u8_LineId
      \param[in]      e_LineStatus
      \param[in]      u16_Hs
     
      \return         Return code
*/

E_CMBS_RC cmbs_dsr_gen_SendDiagnosticStatusInd(void * pv_AppRefHandle, u16 u16_RequestId, u8 u8_LineId, u8 u8_Type, u16 u16_Hs);

//    ==========  cmbs_dsr_DTAMStartSession  ===========
/*!
      \brief
         Request to start DTAM session
         
      <h2>cmbs_dsr_DTAMStartSession</h2>

      \param[in]      u8_LineId
      \param[in]      u8_HsNr
      \param[in]      u16_DTAMSessionID
	  
      \return         Return code
*/
E_CMBS_RC    cmbs_dsr_DTAMStartSession(u8 u8_LineId, u8 u8_HsNr, u16 u16_DTAMSessionID);

//    ==========  cmbs_dsr_DTAMSelectNeighbourMsg  ===========
/*!
      \brief
         Allows the PP to select the next or previous message
         
      <h2>cmbs_dsr_DTAMSelectNeighbourMsg</h2>

      \param[in]      bNextMsg
      \param[in]      u16_SessionId
      \param[in]      u8_HsNr

      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMSelectNeighbourMsg(bool bNextMsg, u16 u16_SessionId, u8 u8_HsNr);

//    ==========  cmbs_dsr_DTAMPlayMsg  ===========
/*!
      \brief
          Play DTAM message
         
      <h2>cmbs_dsr_DTAMPlayMsg</h2>

      \param[in]      bMsgType
      \param[in]      u8_MsgIndex
      \param[in]      bPlayMode
      \param[in]      u8_NumOfSeconds
      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMPlayMsg(bool bMsgType, u8 u8_MsgIndex,bool bPlayMode, u8 u8_NumOfSeconds, u16 u16_SessionId, u8 u8_HsNr);

//    ==========  cmbs_dsr_DTAMDeleteMsg  ===========
/*!
      \brief
          Delete indicated DTAM incoming message or welcome message
         
      <h2>cmbs_dsr_DTAMDeleteMsg</h2>

      \param[in]      bMsgType
      \param[in]      u8_MsgIndex
      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMDeleteMsg(bool bMsgType, u8 u8_MsgIndex, u16 u16_SessionId, u8 u8_HsNr);

//    ==========  cmbs_dsr_DTAMPauseResume  ===========
/*!
      \brief
          Request from the PP that the current playing of message will be Paused or Resumed
         
      <h2>cmbs_dsr_DTAMPauseResume</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC cmbs_dsr_DTAMPauseResume(u16 u16_SessionId, u8 u8_HsNr);

//    ==========  cmbs_dsr_DTAMStopMsgPlay  ===========
/*!
      \brief
          Request from the PP that the current playing of message will be stopped
         
      <h2>cmbs_dsr_DTAMStopMsgPlay</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMStopMsgPlay(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMRecordWelcomeMsg  ===========
/*!
      \brief
          Request the recording of a new welcome message at the indicated welcome message index
         
      <h2>cmbs_dsr_DTAMRecordWelcomeMsg</h2>

      \param[in]      u8_MsgPositionIdx
      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMRecordWelcomeMsg(u8 u8_MsgPositionIdx, u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMRecordWelcomeMsgStop  ===========
/*!
      \brief
          Indicate to the DTAM that recording shall be terminated
         
      <h2>cmbs_dsr_DTAMRecordWelcomeMsgStop</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMRecordWelcomeMsgStop(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMStartSessionCfm  ===========
/*!
      \brief
          Confirm or Reject DTAM session
         
      <h2>cmbs_dsr_DTAMStartSessionCfm</h2>

      \param[in]      pst_DTAMStartSessionCfm
      \param[in]      u8_HsNr      
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMStartSessionCfm(PST_CMBS_DTAM_START_SESSION_CFM pst_DTAMStartSessionCfm, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMSelectNeighbourMsgCfm  ===========
/*!
      \brief
          Confirms the next or previous DTAM message selection
         
      <h2>cmbs_dsr_DTAMSelectNeighbourMsgCfm</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMSelectNeighbourMsgCfm(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMPlayMsgCfm  ===========
/*!
      \brief
          Play DTAM message confirm
         
      <h2>cmbs_dsr_DTAMPlayMsgCfm</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMPlayMsgCfm(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMDeleteMsgCfm  ===========
/*!
      \brief
          Confirm to Delete indicated DTAM incoming message or welcome message 
         
      <h2>cmbs_dsr_DTAMDeleteMsgCfm</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMDeleteMsgCfm(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMStopMsgPlayCfm  ===========
/*!
      \brief
          Confirm request from the PP that the current playing of message will be stopped 
         
      <h2>cmbs_dsr_DTAMStopMsgPlayCfm</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMStopMsgPlayCfm(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMRecordWelcomeMsgCfm  ===========
/*!
      \brief
          Confirm request the recording of a new welcome message at the indicated welcome message index  
         
      <h2>cmbs_dsr_DTAMRecordWelcomeMsgCfm</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMRecordWelcomeMsgCfm(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMRecordWelcomeMsgStopCfm  ===========
/*!
      \brief
          Confirmation from the DTAM that recording shall be terminated  
         
      <h2>cmbs_dsr_DTAMRecordWelcomeMsgStopCfm</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return           Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMRecordWelcomeMsgStopCfm(u16 u16_SessionId, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMNACK  ===========
/*!
      \brief
          Negative response to previous DTAM command
         
      <h2>cmbs_dsr_DTAMNACK</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_RejectReason
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMNACK(u16 u16_SessionId, u8 u8_RejectReason, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMStatus  ===========
/*!
      \brief
         Command to inform the PP of the current command status   
         
      <h2>cmbs_dsr_DTAMStatus</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_DTAMStatus
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC   cmbs_dsr_DTAMStatus(u16 u16_SessionId, u8 u8_DTAMStatus, u8 u8_HsNr);
//    ==========  cmbs_dsr_DTAMPauseResumeCfm  ===========
/*!
      \brief
         Confirm request that the current playing of a message be paused or resumed   
         
      <h2>cmbs_dsr_DTAMPauseResumeCfm</h2>

      \param[in]      u16_SessionId
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC cmbs_dsr_DTAMPauseResumeCfm(u16 u16_SessionId, u8 u8_HsNr);

//    ==========  cmbs_dsr_gen_SendSMSNotification  ===========
/*!
      \brief
         Send Notification regarding Incoming SMS   
         
      <h2>cmbs_dsr_gen_SendSMSNotification</h2>

      \param[in]      u16_RequestId
      \param[in]      u8_SMSType
      \param[in]      u16_NumOfUnreadMsgs
      \param[in]      u16_TotalNumOfMsgs      
      \param[in]      u8_HsNr
      
      \return         Return code
*/
E_CMBS_RC cmbs_dsr_gen_SendSMSNotification(void * pv_AppRefHandle, u16 u16_RequestId, u8 u8_SMSType, u16 u16_NumOfUnreadMsgs, u16 u16_TotalNumOfMsgs, u16 u16_Hs);

//    ==========  cmbs_dsr_HSRSSIReq  ===========
/*!
      \brief
         This event requests for HS RSSI

      <h2>cmbs_dsr_HSRSSIReq</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u8_HSNum
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_HSRSSIReq(void * pv_AppRefHandle, u8 u8_HSNum);
      
//    ==========  cmbs_dsr_DeleteINLEntries  ===========
/*!
      \brief
         This event requests deleting entries in teh Internal Name list

      <h2>cmbs_dsr_DeleteINLEntries</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u16_Handsets      Handset mask of handsets to be deleted form the list
      \return           Return code
*/

E_CMBS_RC cmbs_dsr_DeleteINLEntries(void * pv_AppRefHandle, u16 u16_Handsets);

//    ==========  cmbs_dsr_CallStateFilterSet  ===========
/*!
      \brief
         This event sets Call State Filter

      <h2>cmbs_dsr_CallStateFilterSet</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]      u8_FilterType
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_CallStateFilterSet(void * pv_AppRefHandle, u8 u8_FilterType);

//    ==========  cmbs_api_RegisterLogBufferCb  ===========
/*!
      \brief
         
      <h2>cmbs_api_RegisterLogBufferCb</h2>

      \param[in]      
      \return           Return code
*/
void cmbs_api_RegisterLogBufferCb(void* pv_AppRef, PST_CB_LOG_BUFFER pfn_log_buffer_Cb);

//    ==========  cmbs_api_ParseDectMsg  ===========
/*!
      \brief
         
      <h2>cmbs_api_ParseDectMsg</h2>

      \param[in]      
      \return           Return code
*/
void cmbs_api_ParseDectMsg(u8 * buff, u8 u8_ILen, u8 u8_HandsetNumber);

//    ==========  cmbs_api_get_LogBufferCb  ===========
/*!
      \brief
         
      <h2>cmbs_api_get_LogBufferCb</h2>

      \param[in]      
      \return           Return code
*/
PST_CB_LOG_BUFFER cmbs_api_get_LogBufferCb(void);

//    ==========  cmbs_dee_EmergencyCallRelease  ===========
/*!
      \brief
         This event notifies target that the emergency call was released by network

      <h2>cmbs_dee_EmergencyCallRelease</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \return           Return code
*/
E_CMBS_RC	cmbs_dee_EmergencyCallRelease( void * pv_AppRefHandle, void * pv_RefIEList );

//    ==========  cmbs_dsr_JEDECiDGet  ===========
/*!
      \brief
         This event requests JEDEC ID (in case of DCX81/DHX91)

      <h2>cmbs_dsr_JEDECiDGet</h2>

      \param[in]      none
      \return         Return code
*/
E_CMBS_RC cmbs_dsr_JEDECiDGet(void);

//    ==========  cmbs_dsr_GetActiveCalls  ===========
/*!
      \brief
         This event requests call info table from CMBS, in case Host restart, and to referesh host's call objects.

      <h2>cmbs_dsr_GetActiveCalls</h2>

      \param[in]      none
      \return         Return code
*/
E_CMBS_RC	cmbs_dsr_GetActiveCalls(void * pv_AppRefHandle);

//    ==========  cmbs_dem_ConfigTDMSlots  ===========
/*!
      \brief
         This event is to configure TDM Slot Scheme

      <h2>cmbs_dem_ConfigTDMSlots</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \param[in]      u32_Slots
      \return           Return code
*/
E_CMBS_RC cmbs_dem_ConfigTDMSlots(void *pv_AppRefHandle, u32 u32_TDMSlots);


//    ==========  cmbs_dsr_ConfigCrashDump  ===========
/*!
      \brief
         This event is to configure crash dump feature

      <h2>cmbs_dsr_ConfigCrashDump</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \param[in]      u8_EnableStatus   Crash Dump feature enable status
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_ConfigCrashDump(void *pv_AppRefHandle, E_CRASH_DUMP_ENABLE_STATUS u8_EnableStatus);

//    ==========  cmbs_dsr_ReadCrashDumpConfig  ===========
/*!
      \brief
         This event is to read crash dump configuration (enable/disable)

      <h2>cmbs_dsr_ReadCrashDumpConfig</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_ReadCrashDumpConfig(void *pv_AppRefHandle);


//    ==========  cmbs_dsr_CrashDumpStart  ===========
/*!
      \brief
         This event is to intiate crash dump procedure

      <h2>cmbs_dsr_CrashDumpStart</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_CrashDumpStart(void *pv_AppRefHandle);

//    ==========  cmbs_dsr_CrashDumpDataPacketSend  ===========
/*!
      \brief
         This event is to request specific crash dump packet from target

      <h2>cmbs_dsr_CrashDumpDataPacketSend</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \param[in]      pst_PktNr         reference pointer to the packet number
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_CrashDumpDataPacketSend(void *pv_AppRefHandle, PST_IE_PACKET_NUMBER pst_PktNr);

//    ==========  cmbs_dsr_CrashDumpDataPacketSendRes  ===========
/*!
      \brief
         This event is the response for data packet send event received from CMBS target

      <h2>cmbs_dsr_CrashDumpDataPacketSendRes</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \param[in]      st_Response       Response code
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_CrashDumpDataPacketSendRes(void *pv_AppRefHandle, ST_IE_RESPONSE st_Response);

//    ==========  cmbs_dsr_CrashDumpEnd  ===========
/*!
      \brief
         This event is to terminate the crash dump procedure

      <h2>cmbs_dsr_CrashDumpEnd</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_CrashDumpEnd(void *pv_AppRefHandle);

//    ==========  cmbs_dsr_CrashDumpEndRes  ===========
/*!
      \brief
         This event is to end the  crash dump procedure

      <h2>cmbs_dsr_CrashDumpEndRes</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle
      \param[in]      st_Response       Response code
      \return           Return code
*/
E_CMBS_RC cmbs_dsr_CrashDumpEndRes(void *pv_AppRefHandle, ST_IE_RESPONSE st_Response);



//    ==========  cmbs_dsr_No_Vaild_DateTime_Avail  ===========
/*!
      \brief
         Indication of No Vaild date and time available from handset.

*/
E_CMBS_RC	cmbs_dsr_No_Vaild_DateTime_Avail( void * pv_AppRefHandle,
										 u16 u16_Handset );


//    ==========  cmbs_dee_CallScreening  ===========
/*!
      \brief
         Sends Call Screening Event

      <h2>cmbs_dee_CallScreening</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \return           Return code
*/
E_CMBS_RC	cmbs_dee_CallScreening( void * pv_AppRefHandle, void * pv_RefIEList );


//    ==========  cmbs_dee_CallDeflection  ===========
/*!
      \brief
         Sends Call Deflection Event

      <h2>cmbs_dee_CallDeflection</h2>

      \param[in]      pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \return           Return code
*/
E_CMBS_RC	cmbs_dee_CallDeflection( void * pv_AppRefHandle, void * pv_RefIEList );


//*/
//    ==========  cmbs_dee_CallDeflectionRes  ===========
/*!
      \brief
         Sends a call deflection response event.

         <h2>cmbs_dee_CallDeflectionRes</h2>

      <h3>Introduction</h3>
         Sends a call deflection response event.

      <h3>Use cases</h3>
         Call deflection success or not.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC         cmbs_dee_CallDeflectionRes( void * pv_AppRefHandle, u32 u32_CallInstance, E_CMBS_RESPONSE e_Response )
         </b><br><br>
         This function sends a call hold event, to put the current call on hold.
        <br>
      \param[in]        pv_AppRefHandle   reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]        u32_CallInstance  call instance
      \param[in]        u16_HsNum         HS number
      \param[in]        e_Response        response

      \return           Return code

      \see

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dee_CallDeflectionRes(void *pv_AppRefHandle, u32 u32_CallInstance, u16 u16_HsNum, E_CMBS_RESPONSE e_Response);


//*/
//    ==========  cmbs_dsr_la_ReadSelectedEntriesRes  ===========
/*!
      \brief
         Host response for a request to Read Selected Entries of a list.

         <h2>cmbs_dsr_la_ReadSelectedEntriesRes</h2>

      <h3>Introduction</h3>
         Host response for a request to Read Selected Entries of a list.

      <h3>Use cases</h3>
         A handset is requesting to read selected entries from a list and the Host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_ReadSelectedEntriesRes ( void * pv_AppRefHandle,
                                                        u16 u16_SessionId,
                                                        u16 u16_EntryStartIndex,
                                                        u16 u16_NumOfEntries,
                                                        E_CMBS_RESPONSE e_Response,
                                                        E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_READ_SELECTED_ENTRIES event.<br><br>
         It should be followed by series of cmbs_dsr_la_DataPacketSend() calls,
         that actually return the entries of the Read request.
        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_READ_SELECTED_ENTRIES
      \param[in]     u16_NumOfEntries     the number of entries returned<br>
                                          (can be smaller than the number of entries requested).
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_READ_SELECTED_ENTRIES

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_ReadSelectedEntriesRes(void *pv_AppRefHandle,
                                             u16 u16_SessionId,
                                             u16 u16_NumOfEntries,
                                             E_CMBS_RESPONSE e_Response,
                                             E_CMBS_LA_NACK_ERRORS e_RejectReason);


//*/
//    ==========  cmbs_dsr_la_WriteEntryRes  ===========
/*!
      \brief
         Host response for a request to write an entry in a list.

         <h2>cmbs_dsr_la_WriteEntryRes</h2>

      <h3>Introduction</h3>
         Host response for a request to write an entry in a list.

      <h3>Use cases</h3>
         A handset is requesting to write an entry in a list and the host responds to this request.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_dsr_la_WriteEntryRes ( void * pv_AppRefHandle,
                                              u16 u16_SessionId,
                                              u16 u16_EntryId,
                                              u16 u16_EntryIndex,
                                              u16 u16_NumOfEntries,
                                              E_CMBS_RESPONSE e_Response,
                                              E_CMBS_LA_NACK_ERRORS e_RejectReason )
         </b><br><br>
         This function responds to CMBS_EV_DSR_LA_WRITE_ENTRY event.<br><br>

         The response should be send after the the last received CMBS_EV_DSR_LA_DATA_RECEIVE event
         (the One that has the IsLast set to TRUE).<br>

         If the write Entry request EntryId is not zero, it means that a previous Edit Entry has happened
         and not that Entry is Un locked again (It was locked by the Edit Entry).<br>

         If the CMBS_EV_DSR_LA_DATA_RECEIVE event was empty,
         this means to cancel the write and unlock the entry.

        <br>
      \param[in]     pv_AppRefHandle      reference pointer to AppRefHandle received in cmbs_api_RegisterCb()
      \param[in]     u16_SessionId        the session Id received in CMBS_EV_DSR_LA_EDIT_ENTRY
      \param[in]     u16_EntryId          it returns the same EntryId that came in the request.
      \param[in]     u16_EntryIndex       the index of the new entry if the Save Request was for a new entry<br>
                                          (EntryId in request was zero).
                                          If it is not a new entry but a Save of an exiting entry,<br>
                                          it returns the index of that entry.
      \param[in]     u16_NumOfEntries     the total number of entries returned after save
      \param[in]     e_Response           CMBS_RESPONSE_OK or CMBS_RESPONSE_ERROR
      \param[in]     e_RejectReason       If e_Response is CMBS_RESPONSE_ERROR, the e_RejectReason contains the reject reason.

      \return        Return code

      \see           CMBS_EV_DSR_LA_WRITE_ENTRY

      <b>Sample Code:</b><br>
      <code></code>
*/
E_CMBS_RC         cmbs_dsr_la_WriteEntryRes(void *pv_AppRefHandle,
                                           u16 u16_SessionId,
                                           u16 u16_EntryId,
                                           u16 u16_EntryIndex,
                                           u16 u16_NumOfEntries,
                                           E_CMBS_RESPONSE e_Response,
                                           E_CMBS_LA_NACK_ERRORS e_RejectReason);


#if defined( __cplusplus )
}
#endif

#endif   // CMBS_API_H
