/*******************************************************************
*
*    DESCRIPTION: CMBS HAN header file
*
*    AUTHOR: CMBS Team
*
*    © DSP Group Ltd 2011
*
*******************************************************************/
#ifndef CMBS_HAN_H
#define CMBS_HAN_H

#include "cmbs_api.h"
#include "cmbs_fun.h"

/************************************************************************************************
*
*
*       HAN Unit Types
*
*
*************************************************************************************************/

// Unit Types as define in the Profile document
#define HAN_UNIT_TYPE_ONOFF_SWITCHABLE						0x0100      
#define HAN_UNIT_TYPE_ONOFF_SWITCH							0x0101      
#define HAN_UNIT_TYPE_LEVEL_CONTROLABLE						0x0102      
#define HAN_UNIT_TYPE_LEVEL_CONTROL							0x0103      
#define HAN_UNIT_TYPE_LEVEL_CONTROLABLE_SWITCHABLE			0x0104
#define HAN_UNIT_TYPE_LEVEL_CONTROL_SWITCH					0x0105
#define HAN_UNIT_TYPE_AC_OUTLET								0x0106
#define HAN_UNIT_TYPE_AC_OUTLET_WITH_POWER_METERING		0x0107
#define HAN_UNIT_TYPE_LIGHT									0x0108
#define HAN_UNIT_TYPE_DIMMABLE_LIGHT							0x0109
#define HAN_UNIT_TYPE_DIMMER_SWITCH							0x010A
#define HAN_UNIT_TYPE_DOOR_LOCK								0x010B
#define HAN_UNIT_TYPE_DOOR_BELL								0x010C
#define HAN_UNIT_TYPE_POWER_METER							0x010D

#define HAN_UNIT_TYPE_DETECTOR								0x0200
#define HAN_UNIT_TYPE_DOOR_OPEN_CLOSE_DETECTOR				0x0201
#define HAN_UNIT_TYPE_WINDOW_OPEN_CLOSE_DETECTOR			0x0202
#define HAN_UNIT_TYPE_MOTION_DETECTOR						0x0203
#define HAN_UNIT_TYPE_SMOKE_DETECTOR						0x0204
#define HAN_UNIT_TYPE_GAS_DETECTOR							0x0205
#define HAN_UNIT_TYPE_FLOOD_DETECTOR							0x0206
#define HAN_UNIT_TYPE_GLASS_BREAK_DETECTOR					0x0207
#define HAN_UNIT_TYPE_VIBRATION_DETECTOR					0x0208

#define HAN_UNIT_TYPE_SIREN									0x0280

#define HAN_UNIT_TYPE_PENDANT									0x0300

#define HAN_UNIT_TYPE_USER_INTERFACE							0x0410
#define HAN_UNIT_TYPE_GENERIC_APPLCIATION_LOGIC				0x0411		



/************************************************************************************************
*
*
*       HAN Interfaces
*
*
*************************************************************************************************/
/*
   Device Management Device Id for all devices
*/
#define DEVICE_MGMT_All_DEVICES    0xFFFF

/*
   Device Management Interface ID 
====================================
This interface is used for registering Devices and Units with their associated data
such as Unit Type and the list of interfaces they are supporting.
It is also used for discovering all device and units by a configuration application
*/
#define DEVICE_MGMT_INTERFACE_ID    0x0001

/* Device Management Interface - Server */
/* Attributes */
#define DEVICE_MGMT_IF_SERVER_ATTR_ID_NUM_OF_DEVICES    0x0001

/* Commands */
#define DEVICE_MGMT_IF_SERVER_CMD_ID_REGISTER_DEVICE    		0x0001
#define FUN_IF_DEV_MNGT_CMD_REGISTER_DEVICE 				 	0x0001
#define FUN_IF_DEV_MNGT_CMD_DEREGISTER_DEVICE		 		0x0002
#define FUN_IF_DEV_MNGT_CMD_START_SESS_READ_REG_INFO		0x0003
#define FUN_IF_DEV_MNGT_CMD_END_SESS_READ_REG_INFO	 		0x0004
#define FUN_IF_DEV_MNGT_CMD_GET_ENTRIES				 		0x0005


/* Device Management Interface - Client */
/* Attributes */
/* Commands */



#define 	CMBS_HAN_MASK	0x3000

typedef enum
{
	CMBS_EV_DSR_HAN_MNGR_INIT	=	 		CMBS_EV_DSR_HAN_DEFINED_START,  /*!< 12288 Init HAN Manager */
    CMBS_EV_DSR_HAN_MNGR_INIT_RES, 	        /*!<12289 Response to cmbs_dsr_han_mngr_Init */
    CMBS_EV_DSR_HAN_MNGR_START,             /*!<12290 Start HAN Manager */
    CMBS_EV_DSR_HAN_MNGR_START_RES, 	    /*!<12291 Response to cmbs_dsr_han_mngr_Start */
    CMBS_EV_DSR_HAN_DEVICE_READ_TABLE,      /*!<12292 Read HAN device table */
    CMBS_EV_DSR_HAN_DEVICE_READ_TABLE_RES, 	/*!<12293 Response to cmbs_dsr_han_device_ReadTable */
    CMBS_EV_DSR_HAN_DEVICE_WRITE_TABLE,     /*!<12294 Write HAN device table */
    CMBS_EV_DSR_HAN_DEVICE_WRITE_TABLE_RES, /*!<12295 Response to cmbs_dsr_han_device_WriteTable */
    CMBS_EV_DSR_HAN_BIND_READ_TABLE,        /*!<12296 Read HAN bind table */
    CMBS_EV_DSR_HAN_BIND_READ_TABLE_RES, 	/*!<12297 Response to cmbs_dsr_han_bind_ReadTable */
    CMBS_EV_DSR_HAN_BIND_WRITE_TABLE,       /*!<12298 Write HAN bind table */
    CMBS_EV_DSR_HAN_BIND_WRITE_TABLE_RES, 	/*!<12299 Response to cmbs_dsr_han_bind_WriteTable */
    CMBS_EV_DSR_HAN_GROUP_READ_TABLE,       /*!<12300 Read HAN group table */
    CMBS_EV_DSR_HAN_GROUP_READ_TABLE_RES, 	/*!<12301 Response to cmbs_dsr_han_group_ReadTable */
    CMBS_EV_DSR_HAN_GROUP_WRITE_TABLE,      /*!<12302 Write HAN group table */
    CMBS_EV_DSR_HAN_GROUP_WRITE_TABLE_RES, 	/*!<12303 Response to cmbs_dsr_han_group_WriteTable */
    CMBS_EV_DSR_HAN_MSG_RECV_REGISTER,      /*!<12304 Register unit in HAN message service */
    CMBS_EV_DSR_HAN_MSG_RECV_REGISTER_RES, 	/*!<12305 Response to cmbs_dsr_han_msg_RecvRegister */
    CMBS_EV_DSR_HAN_MSG_RECV_UNREGISTER,    /*!<12306 Unregister unit in HAN message service */
    CMBS_EV_DSR_HAN_MSG_RECV_UNREGISTER_RES,/*!<12307 Response to cmbs_dsr_han_msg_RecvUnregister */
    CMBS_EV_DSR_HAN_MSG_SEND,               /*!<12308 Send message to a destination HAN unit */
    CMBS_EV_DSR_HAN_MSG_SEND_RES, 	        /*!<12309 Response to cmbs_dsr_han_msg_Send */
    CMBS_EV_DSR_HAN_MSG_RECV, 	            /*!<12310 When a device send a message to Host */
    
    CMBS_EV_DSR_HAN_MSG_SEND_TX_START_REQUEST,    	/*!<12311 Host sends Tx Request to a battery operated device (keep wake-up request) */
    CMBS_EV_DSR_HAN_MSG_SEND_TX_START_REQUEST_RES,	/*!<12312 response to CMBS_EV_DSR_HAN_MSG_SEND_TX_START_REQUEST */
    CMBS_EV_DSR_HAN_MSG_SEND_TX_END_REQUEST,        /*!<12313 Host sends Tx end Request to a battery operated device (ok to go to sleep) */
    CMBS_EV_DSR_HAN_MSG_SEND_TX_END_REQUEST_RES,    /*!<12314 response to CMBS_EV_DSR_HAN_MSG_SEND_TX_END_REQUEST */
	CMBS_EV_DSR_HAN_MSG_SEND_TX_ENDED,				/*!<12315 Unsolicited event for cases when the device closed the link or did not contact */
	CMBS_EV_DSR_HAN_MSG_SEND_TX_READY,      		/*!<12316 device reports to host: Clear to send */

	CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_1_NOTIFICATION,	/*!<12317 Notification status about 1st stage registration. Has parameters */
	CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_2_NOTIFICATION,	/*!<12318 Notification status about 2nd stage registration. Has parameters */
	CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_3_NOTIFICATION,	/*!<12319 Notification status about 3rd stage registration. Has parameters */

	CMBS_EV_DSR_HAN_DEVICE_REG_DELETED,					/*!<12320 Unsolicited event. Notifies about device being deleted without host intervention */
	
	CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE,				/*!<12321 Sent by host to ask to remove certain ULE/HAN device. Has no impact on non-ULE devices*/
	CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE_RES,	   		/*!<12322 Notifies about the status of the actual deletion */

	CMBS_EV_DSR_HAN_DEVICE_UNKNOWN_DEV_CONTACT, 		/*!<12323 Unsolicited event. Notify the host about intruder for security reasons */

	CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS,		/* 12324 */
	CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS_RES,	/* 12325 */

	CMBS_EV_DSR_HAN_FUN_REG_MSG_RECV_RES ,     /*!<12326 When a device send a FUN Registration message to Host */

	CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO, 			/*!<12327 Read single HAN device table */
	CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_RES, 	/*!<12328 Response to read single HAN device table */

	CMBS_EV_DSR_HAN_GENERAL_LOGGER_ENABLED, /*!<12329 HAN log Control> */
	CMBS_EV_DSR_HAN_GENERAL_LOGGER_DISABLED, /*!<12330 HAN log Control> */
	
	CMBS_EV_DSR_HAN_GENERAL_LOGGER_IND, /*!<12331 HAN log indication> */

	CMBS_EV_DSR_HAN_BIND_ADD_ENTRY, 	/* 12332 */
	CMBS_EV_DSR_HAN_BIND_ADD_ENTRY_RES,	/* 12333 */
	CMBS_EV_DSR_HAN_BIND_REMOVE_ENTRY,	/* 12334 */
	CMBS_EV_DSR_HAN_BIND_REMOVE_ENTRY_RES,	/* 12335 */
	CMBS_EV_DSR_HAN_BIND_CLEAR_TABLE,     		/* 12336 */
	CMBS_EV_DSR_HAN_BIND_CLEAR_TABLE_RES,		/* 12337 */
	
	CMBS_EV_DSR_HAN_NOTIFY_ON_VOICE_CALL_REQUEST,		/*!< 12338 Host asks device to switch to circuit mode on incoming voice call*/
	CMBS_EV_DSR_HAN_NOTIFY_ON_VOICE_CALL_RES,		/* 12339 */

	CMBS_EV_DSR_HAN_SET_LOG_LEVEL,		/* 12340 */
	CMBS_EV_DSR_HAN_GET_LOG_LEVEL,		/* 12341 */
	CMBS_EV_DSR_HAN_GET_LOG_LEVEL_RES,		/* 12342 */

	CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ,		/* 12343 */
	CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_RES,		/* 12344 */

	CMBS_EV_DSR_HAN_INTERFERENCES_DETECT_IND,		/* 12345 */

	CMBS_EV_DSR_HAN_GET_RX_GAIN,		/* 12346 */
	CMBS_EV_DSR_HAN_GET_RX_GAIN_RES,		/* 12347 */
	
	CMBS_EV_DSR_HAN_SET_RX_GAIN,		/* 12348 */
	CMBS_EV_DSR_HAN_SET_RX_GAIN_RES,		/* 12349 */

	CMBS_EV_DSR_HAN_DUMMY_BEARER_MOVED_IND,	/* 12350 */

	CMBS_EV_DSR_HAN_CP_LOGGER_ENABLED,			/* 12351 */
	CMBS_EV_DSR_HAN_CP_LOGGER_DISABLED,		/* 12352 */
	
	CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_FULL_REGISTRATION_INFO,  			/* 12353 */
	CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_FULL_REGISTRATION_INFO_RES,		/* 12354 */

	CMBS_EV_DSR_HAN_DEVICE_MANGER_WRITE_DEVICE_FULL_REGISTRATION_INFO,  			/* 12355 */
	CMBS_EV_DSR_HAN_DEVICE_MANGER_WRITE_DEVICE_FULL_REGISTRATION_INFO_RES,		/* 12356 */

	CMBS_EV_DSR_HAN_TBR6_SEND_PM,														/* 12357 */	
	CMBS_EV_DSR_HAN_TBR6_SEND_PM_RES,													/* 12358 */

	CMBS_EV_DSR_HAN_TBR6_SET_GUCI,														/* 12359 */	
	CMBS_EV_DSR_HAN_TBR6_SET_GUCI_RES,													/* 12360 */
	
	CMBS_EV_DSR_HAN_TBR6_RESET_STATISTICS,														/* 12361 */	
	CMBS_EV_DSR_HAN_TBR6_RESET_STATISTICS_RES,													/* 12362 */	

	CMBS_EV_DSR_HAN_TBR6_STATISTICS_IND,													/* 12363 */	

	CMBS_EV_DSR_HAN_MODIFICATION_ULE_PAGING_INTERVAL,																/* 12364 */	
	CMBS_EV_DSR_HAN_MODIFICATION_ULE_PAGING_INTERVAL_RES,															/* 12365 */	

	CMBS_EV_DSR_HAN_PVC_RESET_IND, 																		/* 12366 */	

	CMBS_EV_DSR_HAN_NODE_SETUP_ATTEMPTS,													/* 12367 */	
	CMBS_EV_DSR_HAN_NODE_SETUP_ATTEMPTS_RES,												/* 12368 */	
	CMBS_EV_DSR_HAN_LINK_RELEASED_IND,														/* 12369 */

	CMBS_EV_DSR_HAN_GET_MAX_NUM_OF_DEVICES,														/* 12370 */
	CMBS_EV_DSR_HAN_GET_MAX_NUM_OF_DEVICES_RES,														/* 12371 */
	
	CMBS_EV_DSR_HAN_GROUP_CREATE_GROUP, 											/* 12372 */
    	CMBS_EV_DSR_HAN_GROUP_CREATE_GROUP_RES,										/* 12373 */

	CMBS_EV_DSR_HAN_GROUP_REMOVE_GROUP, 										/* 12374 */
	CMBS_EV_DSR_HAN_GROUP_REMOVE_GROUP_RES, 									/*12375*/

	CMBS_EV_DSR_HAN_GROUP_ADD_DEVICE_TO_GROUP, 									/*12376*/
	CMBS_EV_DSR_HAN_GROUP_ADD_DEVICE_TO_GROUP_RES, 								/*12377*/
	CMBS_EV_DSR_HAN_GROUP_ADD_ENTRY_TO_DEVICE_RES,	 						/*12378*/
	CMBS_EV_DSR_HAN_GROUP_REMOVE_DEVICE_FROM_GROUP, 							/*12379*/
	CMBS_EV_DSR_HAN_GROUP_REMOVE_DEVICE_FROM_GROUP_RES, 						/*12380*/
	CMBS_EV_DSR_HAN_GROUP_REMOVE_ENTRY_FROM_DEVICE_RES, 						/*12381*/

	CMBS_EV_DSR_HAN_GROUP_READ_LIST,											/*12382*/
	CMBS_EV_DSR_HAN_GROUP_READ_LIST_RES,										/*12383*/
	CMBS_EV_DSR_HAN_SEND_BASE_UPDATED_NOTIFICATION,								/*12384*/
	CMBS_EV_DSR_HAN_SEND_BASE_UPDATED_NOTIFICATION_RES,							/*12385*/
	CMBS_EV_DSR_HAN_GET_FUN_PROTOCOL_INFO,										/*12386*/
	CMBS_EV_DSR_HAN_GET_FUN_PROTOCOL_INFO_RES,									/*12387*/

	CMBS_EV_DSR_HAN_DEVICE_BLACK_LIST_DELETE,										/*12388*/
	CMBS_EV_DSR_HAN_DEVICE_BLACK_LIST_DELETE_RES,									/*12389*/
	
	CMBS_EV_DSR_HAN_BROADCAST_READ_CHANNEL_TABLE,								/* 12390 */
	CMBS_EV_DSR_HAN_BROADCAST_READ_CHANNEL_TABLE_RES,							/* 12391 */	

	CMBS_EV_DSR_HAN_PVC_RESET_REQ,												/* 12392 */
	CMBS_EV_DSR_HAN_PVC_RESET_REQ_RES,											/* 12393 */

	CMBS_EV_DSR_HAN_PVC_RESET_REQ_STATE,										/* 12394 */
	CMBS_EV_DSR_HAN_PVC_RESET_REQ_STATE_RES,									/* 12395 */

	CMBS_EV_DSR_HAN_DEVICE_READ_EXTENDED_TABLE_PHASE_2,							/* 12396 */
	CMBS_EV_DSR_HAN_DEVICE_READ_EXTENDED_TABLE_PHASE_2_RES,						/* 12397 */

	CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_PHASE_2, 						/*!<12398 */
	CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_PHASE_2_RES, 					/*!<12399 */

	CMBS_EV_DSR_HAN_RAW_MSG_SEND,												/*!<12400 When the base sends a raw message to a device */
	CMBS_EV_DSR_HAN_RAW_MSG_SEND_RES,											/*!<12401 When the base receives a raw message response from a device */
	CMBS_EV_DSR_HAN_RAW_MSG_RECV, 	            								/*!<12402 When a device sends a raw message to Host */
	
	
	CMBS_EV_DSR_HAN_DEVICE_READ_BLACK_LIST_DEVICE_TABLE,						/* 12403 */
	CMBS_EV_DSR_HAN_DEVICE_READ_BLACK_LIST_DEVICE_TABLE_RES,					/* 12404 */

	CMBS_EV_DSR_HAN_DEVICE_DELETED_IND,											/* 12405 */    

	CMBS_EV_DSR_UPDATE_DEVICE_PM_SEQ_NUMBERS,				 					/*!<12406 */
	CMBS_EV_DSR_UPDATE_DEVICE_PM_SEQ_NUMBERS_RES,				 				/*!<12407 */
												
	
	
	CMBS_EV_DSR_HAN_MAX															/* 12408 */

} E_CMBS_HAN_EVENT_ID;

typedef enum
{
    CMBS_IE_HAN_MSG = CMBS_IE_HAN_DEFINED_START,        /* !< Holds a HAN Message structure */
    CMBS_IE_HAN_DEVICE_TABLE_BRIEF_ENTRIES,             /* !< Holds Brief Device table entries */
    CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_ENTRIES,          /* !< Holds Extended Device table entries */
    CMBS_IE_HAN_BIND_TABLE_ENTRIES,                     /* !< Holds Bind table entries */
    CMBS_IE_HAN_GROUP_TABLE_ENTRIES,                    /* !< Holds Group table entries */
    CMBS_IE_HAN_DEVICE,                                 /* !< HAN Device */
    CMBS_IE_HAN_CFG,                                    /* !< HAN Init Config */
    CMBS_IE_HAN_NUM_OF_ENTRIES,                         /* !< Number of entries */
    CMBS_IE_HAN_INDEX_1ST_ENTRY,                        /* !< Index of first entry*/
    CMBS_IE_HAN_TABLE_ENTRY_TYPE,                       /* !< Is it a basic entry structure or an extended*/
    CMBS_IE_HAN_MSG_REG_INFO,                           /* !< Register info */
    CMBS_IE_HAN_TABLE_UPDATE_INFO,                      /* !< ULE Device table change */
    CMBS_IE_HAN_SEND_FAIL_REASON,
    CMBS_IE_HAN_TX_ENDED_REASON,


	CMBS_IE_HAN_DEVICE_REG_STAGE1_OK_STATUS_PARAMS,
	CMBS_IE_HAN_DEVICE_REG_STAGE2_OK_STATUS_PARAMS,
	
	CMBS_IE_HAN_DEVICE_FORCEFUL_DELETE_ERROR_STATUS_REASON,

	CMBS_IE_HAN_GENERAL_STATUS,

	CMBS_IE_HAN_UNKNOWN_DEVICE_CONTACTED_PARAMS,
	CMBS_IE_HAN_BASE_INFO,
	CMBS_IE_HAN_DEVICE_CONNECTION_STATUS,
	CMBS_IE_HAN_DEVICE_REG_ERROR_REASON,
	CMBS_IE_HAN_MSG_CONTROL,

	CMBS_IE_HAN_FUN_DEVICE_INFO,
	
	CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO,          /* !< Holds Single Extended Device table entries */

	CMBS_IE_HAN_BIND_TABLE_ENTRY, 
	
	CMBS_IE_HAN_TX_REQ_RES_REASON,

	CMBS_IE_HAN_LOG_LEVEL,

	CMBS_IE_HAN_CERTIFICATION_OPERATION, 
	CMBS_IE_HAN_CCM_KEY_USED,
	CMBS_IE_HAN_RX_SEQUENCE_NUMBER,
	CMBS_IE_HAN_TX_SEQUENCE_NUMBER,
	CMBS_IE_HAN_TX_ACK_DROP_NUMBER,
	CMBS_IE_HAN_PVC_STATE,
	CMBS_IE_HAN_RX_GAIN,
	
	CMBS_IE_HAN_DEVICE_FULL_REGISTRATION_INFO,

	CMBS_IE_HAN_READ_DEVICE_FULL_REGISTRATION_INFO_RES_REASON,
	
	CMBS_IE_HAN_ULE_TBR6_CARRIER,
	CMBS_IE_HAN_ULE_TBR6_PERIOD,
	CMBS_IE_HAN_ULE_DEVICE_IPUI,
	CMBS_IE_HAN_ULE_TBR6_STATISTICS,

	CMBS_IE_HAN_ULE_CARRIER,
	CMBS_IE_HAN_ULE_SLOT,

	CMBS_IE_HAN_REQ_FAILURE_REASON,

	CMBS_IE_HAN_ULE_PAGING_INTERVAL,

	CMBS_IE_HAN_ULE_PAGING_INTERVAL_MODIFICATION_FAILURE_REASON,

	CMBS_IE_HAN_NODE_SETUP_ATTEMPTS_ENABLE,
	
	CMBS_IE_HAN_LINK_RELEASE_REASON,

	CMBS_IE_HAN_MAX_NUM_OF_DEVICES,
	
	CMBS_IE_HAN_GROUP_ID,
	
	CMBS_IE_HAN_GROUP_COMMAND_FAIL_REASON,
	CMBS_IE_HAN_UNIT_ID,
	CMBS_IE_HAN_GROUP_LIST_ENTRIES,

	CMBS_IE_HAN_MSG_RSSI,

	CMBS_IE_HAN_DEVICE_BLACK_LIST_DELETE_ERROR_STATUS_REASON,

	CMBS_IE_HAN_ULE_CAPABILITIES,

	CMBS_IE_HAN_BROADCAST_CHANNEL_ID,
	
	CMBS_IE_HAN_BROADCAST_CHANNEL_TABLE_ENTRIES,

	CMBS_IE_HAN_PVC_RESET_REQ_FAIL_REASON,

	CMBS_IE_HAN_PVC_RESET_REQ_STATE_FAIL_REASON,

	CMBS_IE_HAN_PVC_RESET_REQ_STATE,

	CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_PHASE_2_ENTRIES,

	CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_PHASE_2_INFO,

	CMBS_IE_HAN_MAX

} E_CMBS_IE_HAN_TYPE;

/****************************** Home Area Network (start)**************************/
/*! \brief HAN message Type */
typedef enum
{
    CMBS_HAN_MSG_TYPE_CMD = 0x01,     /* Message is a command */
    CMBS_HAN_MSG_TYPE_CMD_WITH_RES,   /* Message is a command with response */
    CMBS_HAN_MSG_TYPE_CMD_RES,	      /* Message is a command response */
    CMBS_HAN_MSG_TYPE_ATTR_GET,	      /* Message is get attribute */
    CMBS_HAN_MSG_TYPE_ATTR_GET_RES,   /* Message is get attribute response */
    CMBS_HAN_MSG_TYPE_ATTR_SET,	      /* Message is set attribute */
    CMBS_HAN_MSG_TYPE_ATTR_SET_WITH_RES,  /* Message is set attribute with response */
    CMBS_HAN_MSG_TYPE_ATTR_SET_RES,       /* Message is set attribute response */
    CMBS_HAN_MSG_TYPE_ATTR_GET_PACK,
    CMBS_HAN_MSG_TYPE_ATTR_GET_PACK_RES,
    CMBS_HAN_MSG_TYPE_ATTR_SET_PACK,
    CMBS_HAN_MSG_TYPE_ATTR_SET_PACK_WITH_RES,    
    CMBS_HAN_MSG_TYPE_ATTR_SET_PACK_RES,
    CMBS_HAN_MSG_TYPE_ATTR_SET_PACK_ATOMIC,
    CMBS_HAN_MSG_TYPE_ATTR_SET_PACK_ATOMIC_WITH_RES,
    CMBS_HAN_MSG_TYPE_ATTR_SET_PACK_ATOMIC_RES,

    CMBS_HAN_MSG_TYPE_MAX
}E_CMBS_HAN_MSG_TYPE;



/*! \brief HAN command response */
typedef enum
{
    CMBS_IE_HAN_MSG_CMD_RES_OK,
    CMBS_IE_HAN_MSG_CMD_RES_FAIL,

    CMBS_IE_HAN_MSG_CMD_RES_MAX
}E_CMBS_HAN_MSG_CMD_RES;


typedef enum
{
    CMBS_HAN_TABLE_DEVICE = 0x01,  //Device table
    CMBS_HAN_TABLE_BIND,           //Bind table
    CMBS_HAN_TABLE_GROUP,          //Group table
}E_CMBS_HAN_TABLE;

#define CMBS_HAN_MSG_GROUP_NAME_MAX_LEN 0x30

#define CMBS_HAN_DEVICE_TABLE_ENTRY_TYPE_BRIEF 		0
#define CMBS_HAN_DEVICE_TABLE_ENTRY_TYPE_EXTENDED 	1

#define CMBS_HAN_LOG_LEVEL_INFO					(0x01)
#define CMBS_HAN_LOG_LEVEL_TRACE					(0x02)
#define CMBS_HAN_LOG_LEVEL_ERROR					(0x04)


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_INVALID_OPERATION = 0,
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_MARK_CCM_KEY_AS_USED,
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_MIC_CORRUPTION_NUMBER,
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_SEQUENCE_NUMBER,
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_GET_TX_SEQUENCE_NUMBER,
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_GET_RX_SEQUENCE_NUMBER,
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_ACK_DROP_NUMBER,
	CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_PVC_STATE

} E_CMBS_HAN_CERTIFICATION_OPERATION;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


// !!! CAUTION: Next define describes MAX DEVICES in a single GET_ENTRY RESPOSE and not number of ULE devices in system
#ifndef CMBS_HAN_MAX_DEVICES
#define CMBS_HAN_MAX_DEVICES			5 //CAUTION: This is NOT number of ULE devices !!!
										   // todo: change for Scorpion constant
#endif										   
#define CMBS_HAN_MAX_BINDS				5 //todo: change for Scorpion constant
#define CMBS_HAN_MAX_GROUPS				5 //todo: change for Scorpion constant

#define CMBS_HAN_MAX_MSG_HEADER_LEN		15 //todo: change for Scorpion constant
#if !defined ( CMBS_API_TARGET )
#define CMBS_HAN_MAX_MSG_LEN			500
#define CMBS_HAN_MAX_MSG_DATA_LEN		(CMBS_HAN_MAX_MSG_LEN - CMBS_HAN_MAX_MSG_HEADER_LEN) 	
#else
#define CMBS_HAN_MAX_MSG_LEN				152 //todo: change for Scorpion constant
#define CMBS_HAN_MAX_MSG_DATA_LEN			(CMBS_HAN_MAX_MSG_LEN > SUOTA_SDU_SIZE ? \
											(CMBS_HAN_MAX_MSG_LEN - CMBS_HAN_MAX_MSG_HEADER_LEN) : (SUOTA_SDU_SIZE - CMBS_HAN_MAX_MSG_HEADER_LEN)) //todo: change for Scorpion constant
#endif
#define CMBS_HAN_MAX_UNITS_IN_DEVICE	 6  //todo: change for Scorpion constant
#define CMBS_HAN_MAX_OPTIONAL_INTERFACES_IN_UNIT 8  //todo: change for Scorpion constant

#define CMBS_HAN_DIRECTION_CLIENT_2_SERVER		(0)
#define CMBS_HAN_DIRECTION_SERVER_2_CLIENT		(1)

/*! \brief ST_HAN_CONFIG
HAN configuration structure. Configuration parameters for a HAN services for cmbs_dsr_han_mngr_Init() */
#define CMBS_HAN_EXTERNAL			1
#define CMBS_HAN_INTERNAL			0

#define CMBS_HAN_DEVICE_MNGR_EXT	(1)
#define CMBS_HAN_BIND_MNGR_EXT		(1<<1)
#define CMBS_HAN_BIND_LOOKUP_EXT	(1<<2)
#define CMBS_HAN_GROUP_MNGR_EXT		(1<<3)
#define CMBS_HAN_GROUP_LOOKUP_EXT	(1<<4)
#define CMBS_HAN_RELAY_MODE_ENABLED	(1<<5)
#define CMBS_HAN_RAW_MODE_ENABLED	(1<<6)
#define CMBS_HAN_SSN_HOST_STORE_MODE_ENABLED (1<<7)

#define CMBS_HAN_DEVICE_MNGR(a)	 (a&CMBS_HAN_DEVICE_MNGR_EXT)
#define CMBS_HAN_BIND_MNGR(a)	 (a&CMBS_HAN_BIND_MNGR_EXT)
#define CMBS_HAN_BIND_LOOKUP(a)	 (a&CMBS_HAN_BIND_LOOKUP_EXT)
#define CMBS_HAN_GROUP_MNGR(a)	 (a&CMBS_HAN_GROUP_MNGR_EXT)
#define CMBS_HAN_GROUP_LOOKUP(a) (a&CMBS_HAN_GROUP_LOOKUP_EXT)
#define CMBS_HAN_RELAY_MODE(a) (a&CMBS_HAN_RELAY_MODE_ENABLED)
#define CMBS_HAN_RAW_MODE(a) (a&CMBS_HAN_RAW_MODE_ENABLED)
#define CMBS_HAN_SSN_HOST_STORE_MODE(a) (a&CMBS_HAN_SSN_HOST_STORE_MODE_ENABLED)

typedef struct
{
   	u8 u8_HANServiceConfig;   	
	// Bit 0 Device Manager Ext (1) or Int (0)
	// Bit 1 Bind Manager Ext (1) or Int (0)
	// Bit 2 Bind Lookup Ext (1) or Int (0)
	// Bit 3 Group Manager Ext(1) or Int(0)
	// Bit 4 Group Lookup Ext(1) or Int(0)
	// Bit 5 Relay Mode Ena(1) or Dis(0)
	// Bit 6 Raw Mode Ena(1) or Dis(0) 
	// Bit 7 SSN Host Store Mode Ena(1) or Dis(0)

} ST_HAN_CONFIG, * PST_HAN_CONFIG;

/*! \brief ST_HAN_MSG_REG_INFO
Parameter to be used with cmbs_dsr_han_msg_RecvRegister()
Used with the cmbs_dsr_han_msg_RecvRegister and cmbs_dsr_han_msg_RecvUnregister 
Note: Device is not needed since Device=0 is assumed for Base Station
*/
typedef struct
{
    u8  u8_UnitId;          // 0 and 1 are reserved (Internal Units)
    u16 u16_InterfaceId;	// 0xFFFF means All Interfaces
} ST_HAN_MSG_REG_INFO, * PST_HAN_MSG_REG_INFO;

typedef struct
{	
	u8	u8_IPUI[5];
} ST_HAN_REG_STAGE_1_STATUS;

typedef struct
{	
	u8 SetupType;
	u8 NodeResponse;
	u8	u8_IPUI[3];
} ST_HAN_UNKNOWN_DEVICE_CONTACT_PARAMS;

#define CMBS_HAN_GENERAL_STATUS_OK			0
#define CMBS_HAN_GENERAL_STATUS_ERROR		1

typedef struct
{
	u16 u16_Status;
} ST_HAN_GENERAL_STATUS;

typedef struct
{	
	u16	u16_UleAppProtocolVersion;
	u16	u16_UleAppProtocolId;
	u32	u32_OriginalDevicePagingInterval;
	u32	u32_ActualDevicePagingInterval;
} ST_HAN_REG_STAGE_2_STATUS;


#define CMBS_HAN_REG_FAILED_REASON_DECT_REG_FAILED 					0x1
#define CMBS_HAN_REG_FAILED_REASON_PARAMS_NEGOTIATION_FAILURE		0x2
#define CMBS_HAN_REG_FAILED_REASON_COULD_NOT_FINISH_PVC_RESET 		0x3
#define CMBS_HAN_REG_FAILED_REASON_DID_NOT_RECEIVE_FUN_MSG	 		0x4
#define CMBS_HAN_REG_FAILED_REASON_PROBLEM_IN_FUN_MSG	 			0x5
#define CMBS_HAN_REG_FAILED_REASON_COULD_NOT_ACK_FUN_MSG	 		0x6
#define CMBS_HAN_REG_FAILED_REASON_INTERNAL_AFTER_3RD_STAGE	 		0x7
#define CMBS_HAN_REG_FAILED_REASON_UNEXPECTED	 						0x8
#define CMBS_HAN_REG_FAILED_REASON_FUN_DEV_ID_ALLOCATION_FAILURE	0x9


#define CMBS_HAN_REG_FAILED_REASON_WRONG_ULE_VERSION 	( 0x1 << 2 )
#define CMBS_HAN_REG_FAILED_REASON_WRONG_FUN_VERSION 	( 0x2 << 2 )


/*! \brief ST_HAN_ADDRESS */
typedef struct
{
	u16     u16_DeviceId;	
    u8      u8_UnitId;
	u8      u8_AddressType; //(0x00=Individual Address, 0x01=Group Address)
} ST_HAN_ADDRESS, * PST_HAN_ADDRESS;

	
#define CMBS_HAN_ULE_DEVICE_NOT_REGISTERED									0
#define CMBS_HAN_ULE_DEVICE_REGISTERED_1ST_PHASE							1
#define CMBS_HAN_ULE_DEVICE_REGISTERED_2ND_PHASE_PARAMETERS_NEGOTIATION		2
#define CMBS_HAN_ULE_DEVICE_REGISTERED_2ND_PHASE_PVC_RESET_COMPLETED		3
#define CMBS_HAN_ULE_DEVICE_REGISTERED_3RD_PHASE_RECEIVED_FUN_REGISTER		4
#define CMBS_HAN_ULE_DEVICE_REGISTERED_3RD_PHASE_SENT_FUN_REGISTER_RESP		5
#define CMBS_HAN_ULE_DEVICE_REGISTERED_3RD_PHASE							6

#define CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_INVALID_ID						1
#define CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_DEV_IN_MIDDLE_OF_REG			2
#define CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_BUSY							3
#define CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_UNEXPECTED						4
#define CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_NOT_SUPPORTED						5

#define 	CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_NOT_IN_LINK_AND_NOT_REQUESTED	0
#define 	CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_NOT_IN_LINK_BUT_REQUESTED		1
#define 	CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_IN_LINK							2 
#define 	CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_NOT_REGISTERED					10

#define	CMBS_HAN_DM_DEVICE_REGISTERED_2ND_PHASE_PVC_RESET_COMPLETED		3
#define	CMBS_HAN_DM_DEVICE_REGISTERED_3RD_PHASE							6
#define	CMBS_HAN_DM_DEVICE_BLACK_LISTED									8


#define	CMBS_HAN_ULE_TX_REQ_RES_INVALID_DEVICE_ID 			0
#define	CMBS_HAN_ULE_TX_REQ_RES_PREVIOUS_TX_REQ_PENDING	1
#define	CMBS_HAN_ULE_TX_REQ_RES_NO_FREE_SPACE 			2
#define	CMBS_HAN_ULE_TX_REQ_RES_DEVICE_NOT_REGISTERED 	3
#define	CMBS_HAN_ULE_TX_REQ_RES_DEVICE_ALREADY_IN_LINK 	4


#define CMBS_HAN_READ_REGISTRATION_INFO_NO_FAILURE				0
#define CMBS_HAN_READ_REGISTRATION_INFO_DEVICE_NOT_REGISTERED	1
#define CMBS_HAN_READ_REGISTRATION_INFO_INVALID_DEVICE_ID			2

#define CMBS_HAN_NOT_INITIALIZED 0

#define CMBS_HAN_ULE_PAGING_INTERVAL_BELOW_MIN 0
#define CMBS_HAN_ULE_PAGING_INTERVAL_ABOVE_MAX 1
#define CMBS_HAN_ULE_PAGING_INTERVAL_DEVICE_NOT_REGISTERED 2
#define CMBS_HAN_ULE_PAGING_INTERVAL_DEVICE_NOT_PAGEABLE 3
#define CMBS_HAN_ULE_PAGING_INTERVAL_MODIFICATION_UNKNOWN_FAILED 4

#define CMBS_HAN_NODE_SETUP_ATTEMPTS_ENABLE  1
#define CMBS_HAN_NODE_SETUP_ATTEMPTS_DISABLE 0


#define CMBS_HAN_CP_BEARER_RELEASE                     0x01
#define CMBS_HAN_CP_BASE_STATION_BUSY                  0x0A
#define CMBS_HAN_CP_UNACCEPTABLE_PMID                  0x0D
#define CMBS_HAN_CP_STAY_IN_LCE_PAGING_DETECTION       0x0F
#define CMBS_HAN_CP_SWITCH_TO_CM                       0x10
#define CMBS_HAN_CP_STAY_IN_HIGHER_PAGING_DETECTION    0x11
#define CMBS_HAN_CP_SETUP_AGAIN                        0x12
#define CMBS_HAN_CP_NO_CONNECTION_OR_VC                0x14
#define CMBS_HAN_CP_LU13_LU14_ERROR                    0x15


#define	CMBS_HAN_GM_GENERAL_RES_UNKNOWN_FAILURE 								0				
#define	CMBS_HAN_GM_CREATE_GROUP_RES_NO_FREE_SPACE 								1				
#define	CMBS_HAN_GM_CREATE_GROUP_RES_BC_CHANNEL_ALLOCATION_FAILURE  			2				
#define	CMBS_HAN_GM_DELETE_GROUP_RES_GROUP_NOT_EXIST  							3				
#define	CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_GROUP_NOT_EXIST 					4				
#define	CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_DEVICE_NOT_REGISTERED  				5				
#define	CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_DEVICE_NOT_BROADCASTABLE  			6				
#define	CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_NO_FREE_SPACE  						7				
#define	CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_DUPLICATED_RECORD  					8				
#define	CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_GROUP_NOT_EXIST  				9				
#define	CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_DEVICE_NOT_REGISTERED  		10				
#define	CMBS_HAN_GM_DELETE_GROUP_RES_GROUP_NOT_EMPTY  			 				11				
#define	CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_DEVICE_NOT_INCLUDED_IN_GROUP  	12
#define	CMBS_HAN_GM_SEND_ON_OFF_MSG_TO_GROUP_RES_GROUP_NOT_EXIST 				13	
#define	CMBS_HAN_GM_READ_DEVICE_GROUP_LIST_RES_DEVICE_NOT_REGISTERED  			14
#define	CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_BUSY								15
#define	CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_BUSY							16	
#define	CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_NOT_AUTHORIZED 			17	
#define	CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_INVALID_ARGUMENT			18	
#define	CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_NO_RESOURCE 				19	
#define	CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_UNKNOWN_ERR				20	

#define	CMBS_HAN_BROADCAST_SERVICE_CHANNEL_UKNOWN						(0xFF)	

typedef struct
{
	u8 	u8_UnitId;
	u16	u16_UnitType;
}
ST_HAN_BRIEF_UNIT_INFO;

typedef struct
{
	u16 u16_DeviceId;
	u8	u8_RegistrationStatus; /* CMBS_HAN_ULE_DEVICE_REGISTERED.... */
	u16	u16_RequestedPageTime;
	u16 u16_PageTime; 	//final page time, after negotiation
	u8	u8_NumberOfUnits;
	ST_HAN_BRIEF_UNIT_INFO	st_UnitsInfo[CMBS_HAN_MAX_UNITS_IN_DEVICE];	
}
ST_HAN_BRIEF_DEVICE_INFO, *PST_HAN_BRIEF_DEVICE_INFO;

typedef struct
{
	u8 	u8_UnitId;
	u16	u16_UnitType;
	u16 	u16_NumberOfOptionalInterfaces;
	u16	u16_OptionalInterfaces[CMBS_HAN_MAX_OPTIONAL_INTERFACES_IN_UNIT];
}
ST_HAN_EXTENDED_UNIT_INFO;

typedef struct
{
	u16 u16_DeviceId;
	u8   u8_IPUI[5];
	u8	u8_RegistrationStatus; /* CMBS_HAN_ULE_DEVICE_REGISTERED.... */ 
	u16	u16_RequestedPageTime;
	u16 u16_DeviceEMC;
	u16 u16_PageTime; 	//final page time, after negotiation
	u8	u8_NumberOfUnits;
	u8	u8_ULECapabilities;
	u8	u8_UleAppProtocolId;
	u8	u8_UleAppProtocolVersion;
	ST_HAN_EXTENDED_UNIT_INFO	st_UnitsInfo[CMBS_HAN_MAX_UNITS_IN_DEVICE];	
}
ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2, *PST_HAN_EXTENDED_DEVICE_INFO_PHASE_2;

typedef struct
{
	u16 u16_DeviceId;
	u8   u8_IPUI[5];
	u8	u8_RegistrationStatus; /* CMBS_HAN_ULE_DEVICE_REGISTERED.... */ 
	u16	u16_RequestedPageTime;
	u16 u16_DeviceEMC;
	u16 u16_PageTime; 	//final page time, after negotiation
	u8	u8_NumberOfUnits;
	ST_HAN_EXTENDED_UNIT_INFO	st_UnitsInfo[CMBS_HAN_MAX_UNITS_IN_DEVICE];	
}
ST_HAN_EXTENDED_DEVICE_INFO, *PST_HAN_EXTENDED_DEVICE_INFO;


#define	CMBS_HAN_DECT_SUB_IPUI_SIZE 								5
#define	CMBS_HAN_DECT_SUB_TPUI_SIZE 							3
#define	CMBS_HAN_DECT_SUB_DCK_SIZE 								16
#define 	CMBS_HAN_DECT_SUB_UAK_SIZE 								16
#define 	CMBS_HAN_DECT_SUB_AC_SIZE 								4
#define	CMBS_HAN_DECT_SUB_DEFCK_SIZE 							16
#define 	CMBS_HAN_DECT_SUB_CCM_CK_SIZE 							16
#define 	CMBS_HAN_DECT_SUB_RESERVED_SIZE 						4
#define 	CMBS_HAN_DECT_SUB_NEXT_RX_SEQUENCE_NUMBER_SIZE 		6
#define 	CMBS_HAN_DECT_SUB_NEXT_TX_SEQUENCE_NUMBER_SIZE 		6

// DECT registration information
typedef struct 
{
	u8		pu8_Ipui [ CMBS_HAN_DECT_SUB_IPUI_SIZE ];  // Unique value[N], the 4 MSBits must be set 0. Must be the same value as in the device.
	u8		pu8_Tpui [ CMBS_HAN_DECT_SUB_TPUI_SIZE ]; // SD09_ULE_TPUI_OFFSET + N
	u8		pu8_Dck [ CMBS_HAN_DECT_SUB_DCK_SIZE ];  // all ff
	u8	 	pu8_Uak [ CMBS_HAN_DECT_SUB_UAK_SIZE ]; // Random [N] – must be the same as in device
	u8	 	pu8_AuthenticationCode [ CMBS_HAN_DECT_SUB_AC_SIZE ];  // ff ff 00 00
	u8	 	u8_UakAuth;  // 1
	u8	 	u8_Status;  // 1
	u8	 	u8_HsNumber; // Least significant byte of SD02_ULE_SUB_TPUI
	u8	 	u8_DckAssigned;  // 0
	u8	 	u8_CkLen; // ff
	u8	 	u8_Features; //4 (if DSAA2 supported in the device)
	u8	 	pu8_Defck [ CMBS_HAN_DECT_SUB_DEFCK_SIZE ]; // all 0
	u8	 	u8_DefckAssigned;  // 0
	u8	 	pu8_CcmCK [ CMBS_HAN_DECT_SUB_CCM_CK_SIZE ]; // Random [N] – must be the same as in the device
	u8	 	u8_DlcOperatingMode;  // 23
	u8	 	u8_HlFeatures;  
	u8	 	pu8_Reserved[ CMBS_HAN_DECT_SUB_RESERVED_SIZE ]; // all 0
	u8	 	pu8_NextRxSequenceNumber[CMBS_HAN_DECT_SUB_NEXT_RX_SEQUENCE_NUMBER_SIZE];
	u8	 	pu8_NextTxSequenceNumber[CMBS_HAN_DECT_SUB_NEXT_TX_SEQUENCE_NUMBER_SIZE];
}ST_HAN_DECT_DEVICE_REGISTRATION_INFO, *PST_HAN_DECT_DEVICE_REGISTRATION_INFO;

#define CMBS_HAN_EMC_EXTERNED_APP_SIZE_IN_BYTES 	3
#define CMBS_HAN_ULE_APP_PROTOCOLS					1


// ULE Application protocol parameters
typedef  struct 
{
	u8		u8_UleAppProtocolId;
	u8		u8_UleCapabilities;
	u8		u8_IsProprietaryProtocolId;
	u8		u8_Reserved;		
	u8		u8_UleAppProtocolVersion;									// ULE application Protocol Version	
	u8		pu8_EmcOrExtendedData[CMBS_HAN_EMC_EXTERNED_APP_SIZE_IN_BYTES];	// This field is either EMC (16bit only)
																		// or extended Application Protocol Identifier (20bit only)
}ST_HAN_ULE_APP_PROTOCOL, *PST_HAN_ULE_APP_PROTOCOL;


// ULE registration information
typedef  struct
{
	u16 							DeviceId;
	u8							RegistrationStatus;
	u16							RequestedPageTime;
	u16 							NegotiatedPageTime; 	//final page time, after negotiation
	u16							u16_MaxSduSize;	
	u8							u8_NumAppProtocols;
	ST_HAN_ULE_APP_PROTOCOL	st_AppProtocols[CMBS_HAN_ULE_APP_PROTOCOLS];
}ST_HAN_ULE_DEVICE_REGISTRATION_INFO, *PST_HAN_ULE_DEVICE_REGISTRATION_INFO;


// FUN registration info
typedef  struct 
{
	u16							DeviceId;
	u16							DeviceEMC;
	u8							NumOfUnits;
	ST_HAN_EXTENDED_UNIT_INFO	Units[CMBS_HAN_MAX_UNITS_IN_DEVICE];	
}ST_HAN_FUN_DEVICE_REGISTRATION_INFO;


// Registration data entry structure
typedef struct {
	ST_HAN_DECT_DEVICE_REGISTRATION_INFO	 	st_DECT_RegistrationInfo;
	ST_HAN_ULE_DEVICE_REGISTRATION_INFO	 	st_ULE_RegistrationInfo;
	ST_HAN_FUN_DEVICE_REGISTRATION_INFO	 	st_FUN_RegistrationInfo;
} ST_HAN_FULL_DEVICE_REGISTRATION_INFO, *PST_HAN_FULL_DEVICE_REGISTRATION_INFO;



/*! \brief ST_HAN_DEVICE_ENTRY
Used in ST_IE_HAN_TABLE_ENTRIES 

Note 1:
U16_DeviceId is the same as HandsetId used in the DECT registration

Note 2:
The list of Interfaces is implicitly known from the u16_UnitType 
A unit of a certain type implements a well defined set of interfaces
In the future the API will add List of Interfaces so that a device can implement additional optional interfaces and inform about them
*/
typedef struct
{
    ST_HAN_ADDRESS  st_DeviceUnit;	
    u16	            u16_UnitType; 	
} ST_HAN_DEVICE_ENTRY, * PST_HAN_DEVICE_ENTRY;


/*! \brief ST_HAN_BIND_ENTRY
Used in ST_IE_HAN_TABLE_ENTRIES 
*/
typedef struct
{
  	u16 	u16_SrcDeviceID; 
	u8   	u8_SrcUnitID;
	u16 	u16_DstInterfaceID;
	u8 		u8_DstInterfaceType;
	u16 	u16_DstDeviceID; 
	u8  	u8_DstUnitID;
	u8  	u8_DstAddressType;
} ST_HAN_BIND_ENTRY, * PST_HAN_BIND_ENTRY;


typedef struct
{
	u16   TxReqStatus;
	u16	 TxReqReason;	
} ST_HAN_TX_REQ_RES; 


#define CMBS_HAN_IE_BIND_ENTRY_SIZE			sizeof(ST_HAN_BIND_ENTRY)


/*! \brief ST_HAN_GROUP_TABLE_ENTRY
Used in ST_IE_HAN_TABLE_ENTRIES 
*/
typedef struct
{
	u16						u16_GroupId;
	u16						u16_DeviceId;
	u8 						u8_UnitId;

} ST_HAN_GROUP_TABLE_ENTRY, * PST_HAN_GROUP_TABLE_ENTRY;

typedef struct
{
	u16						u16_GroupId;
	u16						u16_NumOfDevices;
	u8 						u8_Channel;
} ST_HAN_GROUP_LIST_ENTRY, * PST_HAN_GROUP_LIST_ENTRY;


typedef struct
{
	u16	u16_ChannelXInst;   
	u8	u8_ChannelId;
	u8	u8_ChannelStartMMFrame;  
	u8	u8_ChannelStartMFrame;   
	u8	u8_ChannelPeriodicity; 
	
} ST_HAN_BROADCAST_CHANNEL_ENTRY, * PST_HAN_BROADCAST_CHANNEL_ENTRY;

typedef struct
{
    u16                    		 	u16_NumOfEntries;		// number of entries in this struct
    u16                     		u16_StartEntryIndex;	// Index of first Entry in this struct  
	ST_HAN_BROADCAST_CHANNEL_ENTRY	*pst_broadcastChannelEntries; 
	
} ST_IE_HAN_BROADCAST_CHANNEL_ENTRIES, *PST_IE_HAN_BROADCAST_CHANNEL_ENTRIES;

#define CMBS_HAN_IE_GROUP_TABLE_ENTRY_SIZE			sizeof(ST_HAN_GROUP_TABLE_ENTRY)
#define CMBS_HAN_IE_GROUP_LIST_ENTRY_SIZE			sizeof(ST_HAN_GROUP_LIST_ENTRY)


typedef enum
{
	CMBS_HAN_ULE_CALL_CODEC_NB_BASIC	= 0x00,
	CMBS_HAN_ULE_CALL_CODEC_WB_G722		= 0x01,

	CMBS_HAN_ULE_CALL_CODEC_LAST,
}
EN_HAN_VOICE_CALL_CODEC;

typedef enum
{
	CMBS_HAN_OTHER_PARTY_TYPE_HS	= 0x00,
	CMBS_HAN_OTHER_PARTY_TYPE_DEVICE,
	CMBS_HAN_OTHER_PARTY_TYPE_NUMBER,
	CMBS_HAN_OTHER_PARTY_TYPE_SERVICE,
	
	CMBS_HAN_OTHER_PARTY_TYPE_LAST,
}
EN_HAN_OTHER_PARTY_TYPE;

#define CMBS_HAN_DIALED_DIGITS_MAX_LEN				30
#define CMBS_HAN_OTHER_PARTY_NAME_MAX_LEN			30
#define CMBS_HAN_OTHER_PARTY_ID_MAX_LEN				30

#define CMBS_HAN_PREFFERED_CODEC_MASK				0x000001
#define CMBS_HAN_DIGITS_MASK						0x000010
#define CMBS_HAN_OTHER_PARTY_TYPE_MASK				0x000100
#define CMBS_HAN_OTHER_PARTY_NAME_MASK				0x001000
#define CMBS_HAN_OTHER_PARTY_ID_MASK				0x010000
#define CMBS_HAN_CALL_TYPE_MASK						0x100000


#define CMBS_HAN_CALL_TYPE_CALL_DIRECTION_OFFSET	0
#define CMBS_HAN_CALL_TYPE_RING_FLAG_OFFSET			1
#define CMBS_HAN_CALL_TYPE_AUOT_ANSWER_FLAG_OFFSET	2

typedef struct
{

    u32     u32_FieldMask;
    u8		pu8_Digits[CMBS_HAN_DIALED_DIGITS_MAX_LEN];				//first byte stores len
    u8		pu8_OtherPartyName[CMBS_HAN_OTHER_PARTY_NAME_MAX_LEN];	//first byte stores len
    u8		pu8_OtherPartyId[CMBS_HAN_OTHER_PARTY_ID_MAX_LEN];		//first byte stores len
    u8		u8_PreferredCodec;
    u8		u8_OtherPartyType;
	u8		u8_CallType;
	
} ST_HAN_VOICE_CALL_REQ_PARAMS, * PST_HAN_VOICE_CALL_REQ_PARAMS;

/*! \brief ST_HAN_MSG_CONTROL
	Control flags for message
*/
typedef struct
{
   u16   u16_Reserved; 
} ST_HAN_MSG_TRANSPORT, * PST_HAN_MSG_TRANSPORT;


#define 		CMBS_HAN_SEND_MSG_REASON_DATA_TOO_BIG_ERROR					0
#define 		CMBS_HAN_SEND_MSG_REASON_DEVICE_NOT_IN_LINK_ERROR			1
#define 		CMBS_HAN_SEND_MSG_REASON_TRANSMIT_FAILED_ERROR				2
#define 		CMBS_HAN_SEND_MSG_REASON_DECT_ERROR							3
#define 		CMBS_HAN_SEND_MSG_REASON_BUSY_WITH_PREVIOUS_MESSAGES		4
#define   		CMBS_HAN_SEND_MSG_REASON_INVALID_DST_DEVICE_LIST			5
#define   		CMBS_HAN_SEND_MSG_REASON_NO_TX_REQUEST						6
#define   		CMBS_HAN_SEND_MSG_REASON_UNKNOWN_ERROR						7
#define   		CMBS_HAN_SEND_MSG_REASON_NO_FREE_SPACE						8	
#define   		CMBS_HAN_SEND_MSG_REASON_GROUP_EMPTY						9
#define   		CMBS_HAN_SEND_MSG_REASON_GROUP_NOT_EXIST					10
#define   		CMBS_HAN_SEND_MSG_REASON_ULE_SDU_TOO_BIG					11
#define   		CMBS_HAN_SEND_MSG_REASON_BUSY_WITH_PREV_MSG					12
#define   		CMBS_HAN_SEND_MSG_REASON_ZERO_DATA_LEN						13

#define 		CMBS_HAN_TX_ENDED_REASON_LINK_FAILED_ERROR					1	//LINK_MANAGER_SERVICE_LINK_FAILED, Device not contacted during the time it was supposed to contact	
#define 		CMBS_HAN_TX_ENDED_REASON_LINK_DROPPED_ERROR					2	//LINK_MANAGER_SERVICE_LINK_DROPPED, Link terminated due to host inacctivity. Happens after a timeout


#define 		CMBS_HAN_CAPABILITIES_ULE_NOT_SUPPORTED						0
#define 		CMBS_HAN_CAPABILITIES_ULE_PHASE_1							1
#define 		CMBS_HAN_CAPABILITIES_ULE_PHASE_1_2							3
#define 		CMBS_HAN_CAPABILITIES_ULE_PHASE_2							5
#define 		CMBS_HAN_CAPABILITIES_ULE_PHASE_3							7

// ULE Protocol Id
#define CMBS_HAN_ULE_APPLICATION_PROTOCOL_ID_UNDEFINED						0
#define CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_1					1		// ULE HAN-FUN Protocol Version 1
#define CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_2					2		//ULE HAN-FUN Protocol Version 2
#define CMBS_HAN_ULE_APPLICATION_PROTOCOL_PROPRIETARY_MASK					0x40	// 1 x x x x x x 	Proprietary ULE protocols	

// ULE Protocol version
#define	CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_UNDEFINED					0		// A value of 0 shall be used when there is no specific requirement to specify the version
#define	CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_0						1		// Aligned to FUN version 1.0
#define	CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_4						2		// Aligned to FUN version 1.4


#define	CMBS_HAN_PVC_RESET_REQ_DEVICE_NOT_REGISTERED						0
#define	CMBS_HAN_PVC_RESET_REQ_UNKNOWN_FAILURE								1

#define	CMBS_HAN_PVC_RESET_REQ_STATE_PENDING								0
#define	CMBS_HAN_PVC_RESET_REQ_STATE_NO_REQ									1


/*! \brief ST_IE_HAN_MSG
IE to hold HAN message structure for CMBS_IE_HAN_MSG

Send using binding table:
If u16_DstDevice=0xFFFF and u8_DstUnit=0xFF, then binding table will be used to determine the destination
*/
typedef  struct
{
   	u16     			u16_SrcDeviceId;	
    u8      			u8_SrcUnitId;
   	u16     			u16_DstDeviceId;	
    u8      			u8_DstUnitId;
	u8      			u8_DstAddressType; //(0x00=Individual Address, 0x01=Group Address)

   ST_HAN_MSG_TRANSPORT st_MsgTransport;    // transport layer (Encryption, Reliable Connection, etc.)
   u8                   u8_MsgSequence;     // will be returned in the response
   u8                   e_MsgType;          // message type
   u8                   u8_InterfaceType;   // 1=server, 0=client
   u16                  u16_InterfaceId;
   u8                   u8_InterfaceMember; // depending on message type, Command or attribute id or attribute pack id
   u16                  u16_DataLen;
   u8 *                 pu8_Data;
} ST_IE_HAN_MSG , * PST_IE_HAN_MSG;


#define CMBS_HAN_IE_TABLE_SIZE_1	MAX(CMBS_HAN_MAX_DEVICES*sizeof(ST_HAN_BRIEF_DEVICE_INFO),CMBS_HAN_MAX_BINDS*sizeof(ST_HAN_BIND_ENTRY))
#define CMBS_HAN_IE_TABLE_SIZE_2	MAX(CMBS_HAN_IE_TABLE_SIZE_1,CMBS_HAN_MAX_GROUPS*sizeof(ST_HAN_GROUP_TABLE_ENTRY))
#define CMBS_HAN_IE_TABLE_SIZE 		MAX(CMBS_HAN_IE_TABLE_SIZE_2,CMBS_HAN_MAX_GROUPS*sizeof(ST_HAN_GROUP_LIST_ENTRY))
		

/*! \brief ST_IE_HAN_CONFIG
		IE structure for ST_HAN_CONFIG
*/
typedef struct 
{
  ST_HAN_CONFIG	st_HanCfg;	// HAN cfg
}ST_IE_HAN_CONFIG,*PTS_IE_HAN_CONFIG;


/*! \brief ST_IE_HAN_BASE_INFO
		IE structure for ST_IE_HAN_BASE_INFO
*/
typedef struct 
{
  	u8	u8_UleAppProtocolId;	// ULE protocol ID
  	u8 	u8_UleAppProtocolVersion;  // ULE protocol version  
}ST_IE_HAN_BASE_INFO,*PST_IE_HAN_BASE_INFO;

/*! \brief ST_IE_HAN_DEVICE_ENTRIES
IE structure for CMBS_IE_HAN_DEVICE_ENTRIES
*/
typedef struct
{
    u16                     	u16_NumOfEntries;		// number of entries in this struct
    u16                     	u16_StartEntryIndex;	// Index of first Entry in this struct
    ST_HAN_BRIEF_DEVICE_INFO*   pst_DeviceEntries;	    // points to beginning of array 
} ST_IE_HAN_BRIEF_DEVICE_ENTRIES, * PST_IE_HAN_BRIEF_DEVICE_ENTRIES, ST_IE_HAN_BRIEF_DEVICE_TABLE, * PST_IE_HAN_BRIEF_DEVICE_TABLE;

/*! \brief ST_IE_HAN_DEVICE_ENTRIES
IE structure for CMBS_IE_HAN_DEVICE_ENTRIES
*/
typedef struct
{
    u16                     	u16_NumOfEntries;		// number of entries in this struct
    u16                     	u16_StartEntryIndex;	// Index of first Entry in this struct
    ST_HAN_EXTENDED_DEVICE_INFO*   pst_DeviceEntries;	    // points to beginning of array 
} ST_IE_HAN_EXTENDED_DEVICE_ENTRIES, * PST_IE_HAN_EXTENDED_DEVICE_ENTRIES, ST_IE_HAN_EXTENDED_DEVICE_TABLE, * PST_IE_HAN_EXTENDED_DEVICE_TABLE;

typedef struct
{
    u16                     	u16_NumOfEntries;		// number of entries in this struct
    u16                     	u16_StartEntryIndex;	// Index of first Entry in this struct
    ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2*   pst_DeviceEntries;	    // points to beginning of array 
} ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES, * PST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES, ST_IE_HAN_EXTENDED_DEVICE_TABLE_PHASE_2, * PST_IE_HAN_EXTENDED_DEVICE_TABLE_PHASE_2;


/*! \brief ST_IE_HAN_BIND_ENTRIES
IE structure for CMBS_IE_HAN_BIND_ENTRIES
*/
typedef struct
{
    u16                     u16_NumOfEntries;		// number of entries in this struct
    u16                     u16_StartEntryIndex;	// Index of first Entry in this struct
    ST_HAN_BIND_ENTRY *     pst_BindEntries;	    // points to beginning of array 
} ST_IE_HAN_BIND_ENTRIES, * PST_IE_HAN_BIND_ENTRIES, ST_IE_HAN_BIND_TABLE, * PST_IE_HAN_BIND_TABLE;

/*! \brief ST_IE_HAN_GROUP_ENTRIES
IE structure for CMBS_IE_HAN_GROUP_ENTRIES
*/
typedef struct
{
    u16                     u16_NumOfEntries;		// number of entries in this struct
    u16                     u16_StartEntryIndex;	// Index of first Entry in this struct
    ST_HAN_GROUP_TABLE_ENTRY *    pst_GroupEntries;	    // points to beginning of array 
} ST_IE_HAN_GROUP_ENTRIES, * PST_IE_HAN_GROUP_ENTRIES, ST_IE_HAN_GROUP_TABLE, * PST_IE_HAN_GROUP_TABLE;

typedef struct
{
    u16                    		 	u16_NumOfEntries;		// number of entries in this struct
    u16                     		u16_StartEntryIndex;	// Index of first Entry in this struct
    ST_HAN_GROUP_LIST_ENTRY *    	pst_GroupListEntries;	    // points to beginning of array 
} ST_IE_HAN_GROUP_LIST_ENTRIES, * PST_IE_HAN_GROUP_LIST_ENTRIES;

typedef  struct
{
    E_CMBS_HAN_TABLE    e_Table; // The table that was updated
} ST_IE_HAN_TABLE_UPDATE_INFO, * PST_IE_HAN_TABLE_UPDATE_INFO;


typedef struct
{	
	u8	u8_IPUI[5];
} ST_IE_HAN_ULE_DEVICE_IPUI;



typedef struct {
    u32 u32_count;          /* Number of received packets with good SYNC+A-CRC   */
    u32 u32_ber;            /* Bits with error                                   */
    u32 u32_fer;            /* Frames with more than 32 bit errors               */
    u32 u32_bcrc_wrong;     /* Frames with wrong CRC16 in single subfield BField */
    u32 u32_first_rx_frame; /* Holds when measurement was started                */
    u32 u32_last_rx_frame;  /* Last PM transfer. To determine period_min/max     */
    u16 u16_period;         /* Time between this, and last PM transfer           */
    u16 u16_period_min;     /* Min time between PM transfer as seen by base      */
    u16 u16_period_max;     /* Max time between PM transfer as seen by base      */
    u8  u8_period_error;   /* Counts how often period was more that +/- X off   */	
    u8  u8_rssi_local;     /* RSSI the base sees in last PM transfer            */
    u8  u8_rssi_local_min; /* Min RSSI the base ever saw in last PM transfer    */
    u8  u8_rssi_local_max; /* Max RSSI the base ever saw in last PM transfer    */
    u8  u8_rssi;           /* RSSI reported in SENDPM-reply by device           */
    u8  u8_rssi_min;       /* Min RSSI reported in SENDPM-reply by device       */
    u8  u8_rssi_max;       /* Max RSSI reported in SENDPM-reply by device       */
    u8  u8_reserved;	
    u16 u16_missed_acks;    /* Accumulated from what is reported in SENDPM-reply */
    u16 u16_sync_attempts;  /* Accumulated from what is reported in SENDPM-reply */
    u16 u16_sync_frame;     /* Frame device sync'ed as reported in SENDPM-reply  */
}ST_ULE_TBR6_STATISTICS;

typedef struct {
	ST_ULE_TBR6_STATISTICS st_ule_tbr6_statistics;
	u32 u32_Counter;
	u32 u32_Status;	
}ST_IE_HAN_ULE_TBR6_STATISTICS, *PST_IE_HAN_ULE_TBR6_STATISTICS; 

/*! \brief CMBS_IE_HAN_MSG_CONTROL		
*/
typedef struct 
{

  u8 	ImmediateSend 	:1;  // TX request was not submmited before
  u8 	IsLast			:1;  // Release link after confirmtion
  u8  	Reserved 		:6;
  
}ST_IE_HAN_MSG_CTL, * PST_IE_HAN_MSG_CTL;

/****************************** Home Area Network (End)****************************/


/************************************************************************************************
*
*
*       HAN Functions
*
*
*************************************************************************************************/
//*/
//    ==========  cmbs_dsr_han_mngr_Init  ===========
/*!
      \brief
         Initializes the HAN Manager and defines the Services behavior.

         <h2>cmbs_dsr_han_mngr_Init</h2>

      <h3>Introduction</h3>
         Initializes the HAN Manager and defines the Services behavior.
         The configuration defines:
         1. If Device Service is implemented externally or using the internal implementation.
         2. If Group Service Management is implemented externally or internally.
                If Group Service is Internal the Group Lookup is also internal but
                if Group service is external, the Group Lookup can be either external or internal (using cached Group Table).
         3. If Bind Service Management is implemented externally or internally.
                If Bind Service is Internal the Bind Lookup is also internal but
                if Bind service is external, the Bind Lookup can be either external or internal (using cached Binding Table).

      <h3>Use cases</h3>
         Initialize the HAN Manager. Should be called before cmbs_dsr_han_mngr_Start.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_mngr_Init (void *pv_AppRefHandle, ST_HAN_CONFIG * pst_HANConfig) 
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle  received in cmbs_api_registerCB()
      \param[in,out]    pst_HANConfig       pointer to ST_HAN_CONFIG structure

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_MNGR_INIT_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_mngr_Init(void *pv_AppRefHandle, ST_HAN_CONFIG * pst_HANConfig);

//*/
//    ==========  cmbs_dsr_han_mngr_Start  ===========
/*!
      \brief
         Start the HAN Manager.

         <h2>cmbs_dsr_han_mngr_Start</h2>

      <h3>Introduction</h3>
         Start the HAN Manager which in turn starts the other HAN services (Group Service, Bind Service and Device Service, Message Service).

      <h3>Use cases</h3>
         Start the HAN Manager after Initializing the manger itself.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_mngr_Start (void *pv_AppRefHandle);
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle  received in cmbs_api_registerCB()

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_MNGR_START_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_mngr_Start (void *pv_AppRefHandle);


//*/
//    ==========  cmbs_dsr_han_device_ReadTable  ===========
/*!
      \brief
         Read entries from the internal Device table.

         <h2>cmbs_dsr_han_device_ReadTable</h2>

      <h3>Introduction</h3>
        This functions is used for reading the Devices Registered with the Device Management Service.
        Each entry holds the Device address and its Units.
        A Unit is a functional instance of specific Unit Type and supports a list of interfaces of this Unit Type.

        Note: If the Device Management is implemented externally, then this function should not be used and will return Error.

      <h3>Use cases</h3>
         When Device Management Service is implemented internally, this function returns the Device Table to the Host.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_device_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
         </b><br><br>

      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle  received in cmbs_api_registerCB()

      \param[in]        u16_NumOfEntries        Num of entries requested

      \param[in]        u16_IndexOfFirstEntry   First entry to return (index of first entry, e.g. if entries 5-10 are requested, u16_IndexOfFirstEntry=5)

      \param[in]        bool_IsBrief   			Is the required table should be short (brief) or full (extended)

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_DEVICE_READ_TABLE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_device_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8 bool_IsBrief );


//*/
//    ==========  cmbs_dsr_han_device_WriteTable  ===========
/*!
      \brief
         Write entries to the internal Device table.

         <h2>cmbs_dsr_han_device_WriteTable</h2>

      <h3>Introduction</h3>
        This functions is used for writing updated Device Table into the Management Service.
        Note: If the Device Management is implemented externally, then this function should not be used and will return Error.

      <h3>Use cases</h3>
         When Device Management Service is implemented internally but Non Volatile memory is not available, this function can be called upon startup to update Device Table.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_device_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_DEVICE_ENTRY * pst_HANDeviceEntriesArray);
         </b><br><br>

      \param[in]        pv_AppRefHandle             reference pointer to AppRefHandle  received in cmbs_api_registerCB()

      \param[in]        u16_NumOfEntries            Num of entries written

      \param[in]        u16_IndexOfFirstEntry       First entry to write (index of first entry, e.g. if entries 5-10 are written, u16_IndexOfFirstEntry=5)

      \param[in]        pst_HANDeviceEntriesArray   Array of entries to write
            

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_DEVICE_WRITE_TABLE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_device_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_DEVICE_ENTRY * pst_HANDeviceEntriesArray);


//*/
//    ==========  cmbs_dsr_han_bind_ReadTable  ===========
/*!
      \brief
         Read entries from the internal Bind table.

         <h2>cmbs_dsr_han_bind_ReadTable</h2>

      <h3>Introduction</h3>
        This functions is used for reading the Binding Table from the Bind Service.
        Each entry holds Client Device-Unit-Interface & Server Device-Unit-Interface pairs

        Note: If the Bind Lookup is implemented externally, then this function should not be used and will return Error.

      <h3>Use cases</h3>
         This function should be called when Host wants to read the binding table.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_bind_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
         </b><br><br>

      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle  received in cmbs_api_registerCB()

      \param[in]        u16_NumOfEntries        Num of entries requested

      \param[in]        u16_IndexOfFirstEntry   First entry to return (index of first entry, e.g. if entries 5-10 are requested, u16_IndexOfFirstEntry=5)

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_BIND_READ_TABLE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_bind_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);


//*/
//    ==========  cmbs_dsr_han_bind_WriteTable  ===========
/*!
      \brief
         Write entries to the internal Bind table.

         <h2>cmbs_dsr_han_bind_WriteTable</h2>

      <h3>Introduction</h3>
        This functions is used for writing the Binding Table into the Bind Service
        Each entry holds Client Device-Unit-Interface & Server Device-Unit-Interface pairs

      <h3>Use cases</h3>
        If the Bind Management is implemented externally and the Bind Lookup is implemented internally,
        this function must be called so the internal bind lookup will work correctly
      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_han_bind_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_BIND_ENTRY * pst_HANBindEntriesArray);
         </b><br><br>

      \param[in]        pv_AppRefHandle             reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_NumOfEntries            Num of entries written

      \param[in]        u16_IndexOfFirstEntry       First entry to write (index of first entry, e.g. if entries 5-10 are written, u16_IndexOfFirstEntry=5)

      \param[in]        pst_HANBindEntriesArray     Array of entries to write
            

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_BIND_WRITE_TABLE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_bind_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_BIND_ENTRY * pst_HANBindEntriesArray);

//*/
//    ==========  cmbs_dsr_han_group_ReadTable  ===========
/*!
      \brief
         Read entries from the internal Group table.

         <h2>cmbs_dsr_han_group_ReadTable</h2>

      <h3>Introduction</h3>
        This functions is used for reading the Group Table from the Group Service.
        Each entry holds Group Id, Group Name, list of Device-Unit which belongs to this group.

        Note: If the Group Lookup is implemented externally, then this function should not be used and will return Error.

      <h3>Use cases</h3>
         This function should be called when Host wants to read the group table.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_group_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
         </b><br><br>

      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle  received in cmbs_api_registerCB()

      \param[in]        u16_NumOfEntries        Num of entries requested

      \param[in]        u16_IndexOfFirstEntry   First entry to return (index of first entry, e.g. if entries 5-10 are requested, u16_IndexOfFirstEntry=5)

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_GROUP_READ_TABLE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_group_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u16 u16_GroupId);
E_CMBS_RC cmbs_dsr_han_group_ReadList (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
E_CMBS_RC cmbs_dsr_han_Broadcast_ReadChannelTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);

//*/
//    ==========  cmbs_dsr_han_group_WriteTable  ===========
/*!
      \brief
         Write entries to the internal Group table.

         <h2>cmbs_dsr_han_group_WriteTable</h2>

      <h3>Introduction</h3>
        This functions is used for writing the Group Table into the Group Service.
        Each entry holds Group ID, Group Name and a list of Device-Unit belonging to this group.

      <h3>Use cases</h3>
        If the Group Management is implemented externally and the Group Lookup is implemented internally,
        this function must be called so the internal group lookup will work correctly 
      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_han_group_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_GROUP_ENTRY * pst_HANGroupEntriesArray);
         </b><br><br>

      \param[in]        pv_AppRefHandle             reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_NumOfEntries            Num of entries written

      \param[in]        u16_IndexOfFirstEntry       First entry to write (index of first entry, e.g. if entries 5-10 are written, u16_IndexOfFirstEntry=5)

      \param[in]        pst_HANGroupEntriesArray    Array of entries to write
            

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_GROUP_WRITE_TABLE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_group_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_GROUP_TABLE_ENTRY * pst_HANGroupEntriesArray);


//*/
//    ==========  cmbs_dsr_han_msg_RecvRegister  ===========
/*!
      \brief
         Register Units and interfaces with the Message Service.

         <h2>cmbs_dsr_han_msg_RecvRegister</h2>

      <h3>Introduction</h3>
        This functions is used for registering a Device(0)-Unit(yyy)-Interface(zzz) with the Message Service so any message coming to Device 0 Unit yyy interface zzz will be sent to the Host. 
        This allows Host to implement a Unit and interfaces and to exchange messages with other units.
        Example 1:
        In a typical implementation, Host should Implement Unit=2 and register all interfaces of Unit 2 with the Message service.
        Any Message destined to Unit 2 will be sent to Host.
        Also, it should add to the Binding Table a binding to Unit=2 Interfaces=All
        Any message coming form any of the devices with no specified destination (Device-Unit) will automatically sent to the Host. 

        Example 2:
        If Host wants to implement Device Management then it should register with:
        Device=0, Unit=0,Interface=Device Management Server Interface
        If Host wants to implement Group Management then it should register with:
        Device=0, Unit=0,Interface=Group Management Server Interface
        If Host wants to implement Bind Management then it should register with:
        Device=0, Unit=0,Interface=Bind Management Server Interface

      <h3>Use cases</h3>
        This function is used for registering Units and interfaces with the Message Service
      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_han_msg_RecvRegister (void *pv_AppRefHandle, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        pst_HANMsgRegInfo   HAN Message Registration information structure    

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_MSG_RECV_REGISTER_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_msg_RecvRegister (void *pv_AppRefHandle, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);


//*/
//    ==========  cmbs_dsr_han_msg_RecvUnregister  ===========
/*!
      \brief
         Unregister Units and interfaces with the Message Service.

         <h2>cmbs_dsr_han_msg_RecvUnregister</h2>

      <h3>Introduction</h3>
        This functions is used for unregistering a Device(0)-Unit(yyy)-Interface(zzz) in the Message Service.

      <h3>Use cases</h3>
        This function is used for unregistering Units and interfaces from the Message Service
      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_msg_RecvUnregister (void *pv_AppRefHandle, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        pst_HANMsgRegInfo   HAN Message Registration information structure    

      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_MSG_RECV_UNREGISTER_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_msg_RecvUnregister (void *pv_AppRefHandle, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);


//*/
//    ==========  cmbs_dsr_han_msg_SendTxRequest  ===========
/*!
      \brief
            Request to get notified when Device is in reach and can receive messages.

         <h2>cmbs_dsr_han_msg_SendTxRequest</h2>

      <h3>Introduction</h3>
        When the Host wants to send a message to a Device, Since this Device maybe unreachable some of the time,
        this function allows the Host to request that when a Device is in reach (when it sends something to Base for example)
         that the Link will remain open and the Message Service will notify Host that the Device is available.

      <h3>Use cases</h3>
        This function is used for requesting to get notified when Device is in reach and can receive messages.
      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_han_msg_SendTxRequest (void *pv_AppRefHandle, u16 u16_DeviceId);
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_DeviceId        DeviceId, 0xFF=Means Any Device    

      \return           Return Code

      \see
        CMBS_EV_DSR_HAN_MSG_SEND_TX_RES
        CMBS_EV_DSR_HAN_MSG_SEND_TX_READY

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_msg_SendTxRequest (void *pv_AppRefHandle, u16 u16_DeviceId);


//*/
//    ==========  cmbs_dsr_han_msg_SendTxEnd  ===========
/*!
      \brief
        This functions informs the Message Service that the link to a specific Device is not required anymore.
         <h2>cmbs_dsr_han_msg_SendTxEnd</h2>

      <h3>Introduction</h3>
        This functions informs the Message Service that the link to a specific Device is not required anymore.

      <h3>Use cases</h3>
        After the Host sends all messages it needs to a specific Device,
        this function should be called so that the link with the Device will be released (Usually to save Battery).

      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_han_msg_SendTxEnd(void *pv_AppRefHandle, u16 u16_DeviceId);
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_DeviceId        DeviceId, 0xFF=Means Any Device    

      \return           Return Code

      \see
        CMBS_EV_DSR_HAN_MSG_SEND_TX_END_REQUEST_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_msg_SendTxEnd(void *pv_AppRefHandle, u16 u16_DeviceId);

//*/
//    ==========  cmbs_dsr_han_msg_Send  ===========
/*!
      \brief
        This function sends a message to any Device-Unit-interface

         <h2>cmbs_dsr_han_msg_Send</h2>

      <h3>Introduction</h3>
        This function sends a message to any Device-Unit-interface

      <h3>Use cases</h3>
        This function is used for sending messages to other units.
        Note: The CMBS_EV_DSR_HAN_MSG_SEND_RES will include the Request Id that the application put in the message.
        A successful response means that the message was sent successfully to the other Device. 

      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_han_msg_Send (void *pv_AppRefHandle, u16 u16_RequestId, ST_IE_HAN_MSG * pst_HANMsg);
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_RequestId       Host should put unique values as this will be return in the response    

      \param[in]        pst_HANMsg          pointer to HAN Message structure

      \return           Return Code

      \see
        CMBS_EV_DSR_HAN_MSG_SEND_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_msg_Send (void *pv_AppRefHandle, u16 u16_RequestId, PST_IE_HAN_MSG_CTL pst_MsgCtrl, ST_IE_HAN_MSG * pst_HANMsg);

//*/
//    ==========  cmbs_dsr_han_raw_msg_Send  ===========
/*!
      \brief
        This function sends a raw message to any Device-Unit-interface

         <h2>cmbs_dsr_han_raw_msg_Send</h2>

      <h3>Introduction</h3>
        This function sends a raw message to any Device-Unit-interface

      <h3>Use cases</h3>
        This function is used for sending raw messages to other units.
        Note: The CMBS_EV_DSR_HAN_RAW_MSG_SEND_RES will include the Request Id that the application put in the message.
        A successful response means that the message was sent successfully to the other Device. 

      <h3>API Functions description</h3>
         <b>
        E_CMBS_RC cmbs_dsr_han_raw_msg_Send (void *pv_AppRefHandle, u16 u16_RequestId, ST_IE_DATA * pst_HANRawMsg);
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_RequestId       Host should put unique values as this will be return in the response    

      \param[in]        pst_HANRawMsg       pointer to HAN Raw Message structure

      \param[in]        u16_DeviceId        Source device id

      \return           Return Code

      \see
        CMBS_EV_DSR_HAN_RAW_MSG_SEND_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_raw_msg_Send (void *pv_AppRefHandle, u16 u16_RequestId, ST_IE_HAN_MSG_CTL* pst_MsgCtrl, ST_IE_DATA * pst_HANRawMsg, u16 u16_DeviceId);


// ==========  cmbs_dsr_han_device_Delete  ===========
/*!
      \brief
        This function deletes a ULE/HAN Device

         <h2>cmbs_dsr_han_device_Delete</h2>

      <h3>Introduction</h3>
        This function sends a request to the target to delete a registered ULE/HAN device

      <h3>Use cases</h3>
        This function is used for deleting registered ULE/HAN Devices
        This function has no impact on registered non-ULE devices ( handsets )
        This function is a wrapper for sending CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE to the target
		CMBS_EV_DSR_HAN_MSG_SEND_RES will be returned with the status of the deletion

      <h3>API Functions description</h3>
         <b>
		E_CMBS_RC cmbs_dsr_han_device_Delete (void *pv_AppRefHandle, u16 u16_DeviceId )
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_DeviceId       Host should put a DeviceId in question    

      \return           Return Code

      \see
        CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE
        CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_device_Delete (void *pv_AppRefHandle, u16 u16_DeviceId );

// ==========  cmbs_dsr_han_device_GetConnectionStatus  ===========
/*!
      \brief
        This function returns whether a specific device is in link or is expected to be in link any soon

         <h2>cmbs_dsr_han_device_GetConnectionStatus</h2>

      <h3>Introduction</h3>
        This function sends a request to the target to to see check if the device is currently in link or was
        was requested to contact soon

      <h3>Use cases</h3>
        This function is used for checking device connection status ( for instance, prior to sending a message to the device )
        This function has no impact on registered non-ULE devices ( handsets )
        This function is a wrapper for sending CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS to the target
		CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS_RES will be returned with the status of the connection

      <h3>API Functions description</h3>
         <b>
		E_CMBS_RC cmbs_dsr_han_device_GetConnectionStatus (void *pv_AppRefHandle, u16 u16_DeviceId )
         </b><br><br>

      \param[in]        pv_AppRefHandle     reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        u16_DeviceId       Host should put a DeviceId in question    

      \return           Return Code

      \see
        CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE
        CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_device_GetConnectionStatus (void *pv_AppRefHandle, u16 u16_DeviceId );

// ==========  cmbs_dsr_han_fun_RegistationMsgRecv  ===========
/*!
      \brief
        This function is invoked when the host receives FUN registraion msg 

        This function informs the target that FUN Registration msg was received from a specific device

        This function is a wrapper for sending CMBS_EV_DSR_HAN_FUN_REG_MSG_RECV_RES to the target
		
	E_CMBS_RC cmbs_dsr_han_fun_RegistationMsgRecv (void *pv_AppRefHandle, u16 u16_DeviceId )

      \param[in]        pv_AppRefHandle 		    reference pointer to AppRefHandle received in cmbs_api_registerCB()

      \param[in]        pst_FunDeviceInfo      		 Host should send FUN Registration data the was parsed of the registration msg    

      \return           Return Code

*/

E_CMBS_RC cmbs_dsr_han_GetMaxNumOfDevices (void *pv_AppRefHandle);
E_CMBS_RC cmbs_dsr_han_get_fun_protocol_info (void *pv_AppRefHandle);
E_CMBS_RC cmbs_dsr_han_fun_RegistationMsgRecv  (void *pv_AppRefHandle, PST_FUN_DEVICE_INFO pst_FunDeviceInfo);
E_CMBS_RC cmbs_dsr_han_send_base_updated_notification (void *pv_AppRefHandle, u16 u16_DeviceId );

//*/
//    ==========  cmbs_dsr_han_device_ReadTable  ===========
/*!
      \brief
         Read device registration info from the internal Device table.

         <h2>cmbs_dsr_han_device_ReadDeviceRegistrationInformation</h2>

      <h3>Introduction</h3>
        This functions is used for reading the Device Registered with the Device Management Service.
        The record holds the Device address and its Units.
        A Unit is a functional instance of specific Unit Type and supports a list of interfaces of this Unit Type.

        Note: If the Device Management is implemented externally, then this function should not be used and will return Error.

      <h3>Use cases</h3>
         When Device Management Service is implemented internally, this function returns the Device Table to the Host.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_device_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
         </b><br><br>

      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle  received in cmbs_api_registerCB()
      
       \param[in]        u16_DeviceId       Host should put a DeviceId in question   
    
      \return           Return Code

      \see
      CMBS_EV_DSR_HAN_SINGLE_DEVICE_READ_TABLE_RES

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC cmbs_dsr_han_device_ReadDeviceRegistrationInformation (void *pv_AppRefHandle, u16 u16_DeviceId);


//*/
//    ==========  cmbs_dsr_han_bind_AddEntry  ===========
/*!
      \brief
         Add record to Bind Table

         <h2>cmbs_dsr_han_bind_AddRecord</h2>

      <h3>Introduction</h3>
        This functions is used for adding bind entry to bind table 
        The record holds the binding record to be added

        Note: If the bind Management is implemented externally, then this function should not be used and will return Error.

      <h3>Use cases</h3>
         When Bind Management Service is implemented internally, this function returns success when adding the record is successfil, otherwise returns error.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_bind_AddRecord (void *pv_AppRefHandle, PST_HAN_BIND_ENTRY pst_HANBindEntry);
         </b><br><br>

      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle  received in cmbs_api_registerCB()
      
       \param[in]        pst_HANBindEntry       pointer to a struct storing sotring bind record to be added  
    
      \return           Return Code


*/
E_CMBS_RC cmbs_dsr_han_bind_AddEntry (void *pv_AppRefHandle, PST_HAN_BIND_ENTRY pst_HANBindEntry);

//*/
//    ==========  cmbs_dsr_han_bind_RemoveEntry  ===========
/*!
      \brief
         Remove record from Bind Table

         <h2>cmbs_dsr_han_bind_RecordRecord</h2>

      <h3>Introduction</h3>
        This functions is used for removing bind entry from bind table 
        The record holds the binding record to be removed

        Note: If the bind Management is implemented externally, then this function should not be used and will return Error.

      <h3>Use cases</h3>
         When Bind Management Service is implemented internally, this function returns success when removing the record is successfil, otherwise returns error.

      <h3>API Functions description</h3>
         <b>
            E_CMBS_RC cmbs_dsr_han_bind_RemoveRecord (void *pv_AppRefHandle, PST_HAN_BIND_ENTRY pst_HANBindEntry);
         </b><br><br>

      \param[in]        pv_AppRefHandle         reference pointer to AppRefHandle  received in cmbs_api_registerCB()
      
       \param[in]        pst_HANBindEntry       pointer to a struct storing sotring bind record to be removed  
    
      \return           Return Code


*/
E_CMBS_RC cmbs_dsr_han_group_CreateGroup  (void *pv_AppRefHandle, u8 u8_BroadcastChannelId);

E_CMBS_RC cmbs_dsr_han_group_RemoveGroup  (void *pv_AppRefHandle, u16 u16_GroupId );

E_CMBS_RC  cmbs_dsr_han_group_AddDeviceToGroup  (void *pv_AppRefHandle, u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId );

E_CMBS_RC   cmbs_dsr_han_group_RemoveDeviceFromGroup   (void *pv_AppRefHandle, u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId );
E_CMBS_RC cmbs_dsr_han_bind_RemoveEntry (void *pv_AppRefHandle, PST_HAN_BIND_ENTRY pst_HANBindEntry);

E_CMBS_RC cmbs_dsr_han_NotifyOnVoiceCall(void *pv_AppRefHandle, u16 u16_DeviceId);

E_CMBS_RC cmbs_dsr_han_logger_Control(void *pv_AppRefHandle, bool b_enable);

E_CMBS_RC cmbs_dsr_han_cp_logger_Control(void *pv_AppRefHandle, bool b_enable);

E_CMBS_RC cmbs_dsr_han_bind_ClearTable (void *pv_AppRefHandle);

E_CMBS_RC cmbs_dsr_han_Logger_SetLogLevel (void *pv_AppRefHandle, u8 u8_LogLevel);

E_CMBS_RC cmbs_dsr_han_Logger_GetLogLevel (void *pv_AppRefHandle);

E_CMBS_RC cmbs_dsr_han_device_ReadDeviceFullRegistrationInfo (void *pv_AppRefHandle, u16 u16_DeviceId);

E_CMBS_RC cmbs_dsr_han_device_WriteDeviceFullRegistrationInfo (void *pv_AppRefHandle, u16 u16_DeviceId, ST_HAN_FULL_DEVICE_REGISTRATION_INFO *pst_DeviceRegistrationInfo);


E_CMBS_RC cmbs_dsr_han_Certification_MarkCcmKeyAsUsed( void *pv_AppRefHandle, u16 u16_DeviceNumber , u8 u8_CCMKeyUsed);

E_CMBS_RC cmbs_dsr_han_Certification_SetTxMicCorruptNum( void *pv_AppRefHandle, u16 u16_DeviceNumber);

E_CMBS_RC cmbs_dsr_han_Certification_SetTxSeqNum( void *pv_AppRefHandle, u16 u16_DeviceNumber, u16 u16_TxSequenceNumber );

E_CMBS_RC cmbs_dsr_han_Certification_GetTxSeqNum( void *pv_AppRefHandle, u16 u16_DeviceNumber);

E_CMBS_RC cmbs_dsr_han_Certification_GetRxSeqNum( void *pv_AppRefHandle, u16 u16_DeviceNumber);

E_CMBS_RC cmbs_dsr_han_Certification_SetTxAckDropNum( void *pv_AppRefHandle, u16 u16_TxAckDropNumber);

E_CMBS_RC cmbs_dsr_han_Certification_SetPvcState( void *pv_AppRefHandle, u16 u16_DeviceNumber, u8 u8_PVCState );

E_CMBS_RC cmbs_dsr_han_SetRxGain(void *pv_AppRefHandle,  u8 u8_RxGainVal );

E_CMBS_RC cmbs_dsr_han_GetRxGain(void *pv_AppRefHandle);

E_CMBS_RC cmbs_dsr_han_device_ReadDeviceFullRegistrationInfo (void *pv_AppRefHandle, u16 u16_DeviceId);

E_CMBS_RC cmbs_dsr_han_device_WriteDeviceFullRegistrationInfo (void *pv_AppRefHandle, u16 u16_DeviceId, ST_HAN_FULL_DEVICE_REGISTRATION_INFO *pst_DeviceRegistrationInfo);

E_CMBS_RC cmbs_dsr_han_TBR6SendPM(void *pv_AppRefHandle, u8  u8_Carrier , u32 u32_Period  );
E_CMBS_RC cmbs_dsr_han_TBR6SetGUCI(void *pv_AppRefHandle, u8 * pu8_IPUI );
E_CMBS_RC cmbs_dsr_han_TBR6ResetStatistics (void *pv_AppRefHandle);
E_CMBS_RC cmbs_dsr_han_ModificationULEPagingInterval(u16 u16_DeviceId, u32 u32_ULEPagingIntervalMs);
E_CMBS_RC cmbs_dsr_han_NodeSetupAttemptsEnable(bool b_enable);
E_CMBS_RC cmbs_dsr_han_DeviceBlackListDelete(void *pv_AppRefHandle, u16 u16_DeviceId);

E_CMBS_RC cmbs_dsr_han_SendPvcResetReq(void *pv_AppRefHandle, u16 u16_DeviceNumber);
E_CMBS_RC cmbs_dsr_han_GetPvcResetReqState(void *pv_AppRefHandle, u16 u16_DeviceNumber);

E_CMBS_RC cmbs_dsr_han_device_ReadExtendedDeviceTablePhase2 (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
E_CMBS_RC cmbs_dsr_han_device_ReadBlackListedDeviceTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
E_CMBS_RC cmbs_dsr_han_device_ReadDeviceRegistrationInformationPhase2 (void *pv_AppRefHandle, u16 u16_DeviceId);
E_CMBS_RC cmbs_dsr_han_UpdateDevicePMSeqNumbers (void *pv_AppRefHandle, u16 u16_DeviceId , u16 u16_RxSeqNum, u16 u16_TxSeqNum);




#endif //CMBS_HAN_H
/****************************[End Of File]****************************************************************************************************/
