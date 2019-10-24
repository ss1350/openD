/*!
*   \file       apphan.c
*   \brief      HAN API
*   \author     CMBS Team
*
*
*******************************************************************************/

#include <string.h>
#include "cmbs_han.h"
#include "cmbs_fun.h"
#include "appfun.h"
#include "apphan.h"
#include "cmbs_han_ie.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "appmsgparser.h"
#include "cmbs_api.h"
#include "cmbs_dbg.h"
#include "cfr_ie.h"
#include "cmbs_han_ie.h"
#include "cfr_mssg.h"
#include "cfr_debug.h"
#include "hanfun_protocol_defs.h"
#include "cmbs_fifo.h"
#include "tcx_util.h"
#include "tcx_hostlog.h"
#include "cmbs_util.h"

//////////////////////////////////////////////////////////////////////////
void app_HanOnReadDeviceTableRes		(void* pv_EventData									);
void app_HanOnInitStartRes				(void* pv_EventData,	E_CMBS_EVENT_ID e_EventID	);
void app_HanOnDsrGeneralLogEvent		(void* pv_EventData);
//////////////////////////////////////////////////////////////////////////

void app_OnHandsetRegistered( void *pv_List );

void app_HanOnFunMsgReceived(void *pv_EventData, ST_IE_HAN_MSG * pstIe_Msg );
void app_HanOnStartTxRequestRes(void* pv_EventData);
void app_HanOnGetConnectionStatusRes(void* pv_EventData);
void app_HanOnRegStage3Notification(void* pv_EventData);
void app_HanOnForcefulDeleteRes(void* pv_EventData);
void app_HanOnPVCResetIndication(void* pv_EventData);
void app_HanOnFUNMsgSendRes(void* pv_EventData);
void app_HanOnUpdateDevicePMSeqNumberRes( void );




u16 app_HanPopPendingDeviceToDelete();

E_CMBS_RC cmbs_dsr_han_logger_Control (void *pv_AppRefHandle, bool b_enable);
int app_HanMWEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData );



#define HAN_APP_INFO_PRINT(format, ...) 						tcx_WriteLog(HAN_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define HAN_APP_TRACE_PRINT(format, ...) 					tcx_WriteLog(HAN_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define HAN_APP_WARNING_PRINT(format, ...) 					tcx_WriteLog(HAN_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define HAN_APP_ERROR_PRINT(format, ...) 					tcx_WriteLog(HAN_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define HAN_APP_RT_PRINT(format, ...) 						tcx_WriteLog(HAN_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define HAN_APP_PRINT_DATA(pu8_Buffer, u16_Length) 		tcx_WriteLogData(HAN_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)




static ST_HAN_CONFIG	stHanCfg = {0};


typedef struct {
    u16 rx;
    u16 tx;
} t_st_ULESeqNum;

static t_st_ULESeqNum G_st_ULESeqNum[MAX_ULE_DEVICES_HOST];


static u8 u8_Buffer[CMBS_HAN_MAX_MSG_LEN*2];
//////////////////////////////////////////////////

typedef enum
{
	HAN_ATTRIBUTE_TYPE_UNKNOWN,
	HAN_ATTRIBUTE_TYPE_U8,
	HAN_ATTRIBUTE_TYPE_U16,
	HAN_ATTRIBUTE_TYPE_S16,
	HAN_ATTRIBUTE_TYPE_U32,
	HAN_ATTRIBUTE_TYPE_U8_U32,
	HAN_ATTRIBUTE_TYPE_U8_U16,
	HAN_ATTRIBUTE_TYPE_U40,
	HAN_ATTRIBUTE_TYPE_STRING,
	HAN_ATTRIBUTE_TYPE_FRIENDLY_NAMES,
	HAN_ATTRIBUTE_TYPE_UID
} E_Attribute_Type;

typedef struct s_AttributeData
{
	char attributeName[40];
	E_Attribute_Type attributeType;	
}s_AttributeData;



int app_GetAttributeValueAsString(u8 * pu8_Data,u16 Location,E_Attribute_Type AttributeType,char* AttributeValue, u16 pu8_Data_Length);


void app_GetAttributeNameAndType(u8 InterfaceType, u16 InterfaceID, u8 AttributeID, s_AttributeData *attributeData);
//////////////////////////////////////////////////


int app_HANEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData )
{
	E_CMBS_HAN_EVENT_ID e_HanEventId = (E_CMBS_HAN_EVENT_ID)e_EventID;
	bool			ret_code = TRUE;
	ST_IE_HAN_MSG stIe_Msg;
	ST_IE_DATA stIe_RawMsg;
	bool hanUtilsEntityRes = FALSE;

	if(appcmbs_GetExternalClientInformation() != NO_EXTERNAL_CLIENT)
	{
		hanUtilsEntityRes = app_HanMWEntity(pv_AppRef,e_EventID,pv_EventData);
	}

	HAN_APP_INFO_PRINT("Messages in app queue %d\n", appcmbs_ObjectSignalTrace());
	switch(e_HanEventId)
	{
		case CMBS_EV_DSR_HAN_MSG_RECV:
			app_HanOnFunMsgReceived(pv_EventData, &stIe_Msg);
			break;
		case CMBS_EV_DSR_HAN_RAW_MSG_RECV:
			app_HanOnRawMsgReceived(pv_EventData, &stIe_RawMsg);
			break;
		case CMBS_EV_DSR_HAN_GENERAL_LOGGER_IND:
			app_HanOnDsrGeneralLogEvent(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_MNGR_INIT_RES:
		case CMBS_EV_DSR_HAN_MNGR_START_RES:
			app_HanOnInitStartRes(pv_EventData,e_EventID);
			break;
		case CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS_RES:
			app_HanOnGetConnectionStatusRes(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_3_NOTIFICATION:
			app_HanOnRegStage3Notification(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE_RES:
			app_HanOnForcefulDeleteRes(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_PVC_RESET_IND:	
			app_HanOnPVCResetIndication(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_MSG_SEND_RES:
			app_HanOnFUNMsgSendRes(pv_EventData);
			break;
		case CMBS_EV_DSR_UPDATE_DEVICE_PM_SEQ_NUMBERS_RES:
			app_HanOnUpdateDevicePMSeqNumberRes();				
			break;	
		case CMBS_EV_DSR_HAN_DEVICE_READ_TABLE_RES:
		case CMBS_EV_DSR_HAN_DEVICE_WRITE_TABLE_RES:
		case CMBS_EV_DSR_HAN_BIND_READ_TABLE_RES:
		case CMBS_EV_DSR_HAN_BIND_WRITE_TABLE_RES:
		case CMBS_EV_DSR_HAN_GROUP_READ_TABLE_RES:
		case CMBS_EV_DSR_HAN_GROUP_WRITE_TABLE_RES:
		case CMBS_EV_DSR_HAN_MSG_RECV_REGISTER_RES:
		case CMBS_EV_DSR_HAN_MSG_RECV_UNREGISTER_RES:
		case CMBS_EV_DSR_HAN_FUN_REG_MSG_RECV_RES:
		case CMBS_EV_DSR_HAN_MSG_SEND_TX_START_REQUEST_RES:
		case CMBS_EV_DSR_HAN_MSG_SEND_TX_END_REQUEST_RES:
		case CMBS_EV_DSR_HAN_MSG_SEND_TX_ENDED:
		case CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_1_NOTIFICATION:
		case CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_2_NOTIFICATION:
		case CMBS_EV_DSR_HAN_DEVICE_REG_DELETED:
		case CMBS_EV_DSR_HAN_DEVICE_UNKNOWN_DEV_CONTACT:
		case CMBS_EV_DSR_HAN_RAW_MSG_SEND_RES:
		case CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_RES:
		case CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_PHASE_2_RES:
		case CMBS_EV_DSR_HAN_BIND_ADD_ENTRY_RES:
		case CMBS_EV_DSR_HAN_BIND_REMOVE_ENTRY_RES:
		case CMBS_EV_DSR_HAN_BIND_CLEAR_TABLE_RES:
		case CMBS_EV_DSR_HAN_GET_LOG_LEVEL_RES:
		case CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_RES:
		case CMBS_EV_DSR_HAN_INTERFERENCES_DETECT_IND:
		case CMBS_EV_DSR_HAN_GET_RX_GAIN_RES:
		case CMBS_EV_DSR_HAN_SET_RX_GAIN_RES:
		case CMBS_EV_DSR_HAN_DUMMY_BEARER_MOVED_IND:
		case CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_FULL_REGISTRATION_INFO_RES:
		case CMBS_EV_DSR_HAN_DEVICE_MANGER_WRITE_DEVICE_FULL_REGISTRATION_INFO_RES:
		case CMBS_EV_DSR_HAN_TBR6_SEND_PM_RES:
		case CMBS_EV_DSR_HAN_TBR6_SET_GUCI_RES:
		case CMBS_EV_DSR_HAN_TBR6_RESET_STATISTICS_RES:
		case CMBS_EV_DSR_HAN_TBR6_STATISTICS_IND:
		case CMBS_EV_DSR_HAN_MODIFICATION_ULE_PAGING_INTERVAL_RES:		
		case CMBS_EV_DSR_HAN_NODE_SETUP_ATTEMPTS_RES:		
        case CMBS_EV_DSR_HAN_GROUP_CREATE_GROUP_RES:
        case CMBS_EV_DSR_HAN_GROUP_REMOVE_GROUP_RES:
        case CMBS_EV_DSR_HAN_GROUP_ADD_DEVICE_TO_GROUP_RES:
        case CMBS_EV_DSR_HAN_GROUP_REMOVE_DEVICE_FROM_GROUP_RES:
		case CMBS_EV_DSR_HAN_GROUP_ADD_ENTRY_TO_DEVICE_RES:
		case CMBS_EV_DSR_HAN_GROUP_REMOVE_ENTRY_FROM_DEVICE_RES:
		case CMBS_EV_DSR_HAN_DEVICE_BLACK_LIST_DELETE_RES:
		case CMBS_EV_DSR_HAN_SEND_BASE_UPDATED_NOTIFICATION_RES:
		case CMBS_EV_DSR_HAN_BROADCAST_READ_CHANNEL_TABLE_RES:
		case CMBS_EV_DSR_HAN_GET_FUN_PROTOCOL_INFO_RES:
		case CMBS_EV_DSR_HAN_PVC_RESET_REQ_RES:
		case CMBS_EV_DSR_HAN_PVC_RESET_REQ_STATE_RES:
		case CMBS_EV_DSR_HAN_DEVICE_READ_EXTENDED_TABLE_PHASE_2_RES:
		case CMBS_EV_DSR_HAN_DEVICE_READ_BLACK_LIST_DEVICE_TABLE_RES:
		case CMBS_EV_DSR_HAN_DEVICE_DELETED_IND:
		
		if ( g_cmbsappl.n_Token )
	        	{
	            	appcmbs_ObjectSignal( NULL, 0, 1, e_EventID );
	        	}
			break;	
		case CMBS_EV_DSR_HAN_MSG_SEND_TX_READY:
			if ( g_cmbsappl.n_Token )
        	{
				appcmbs_ObjectSignal(NULL, 0 ,0 , e_EventID);
			}
			break;
		default:
			ret_code = FALSE;
		
	}
	     
	return ret_code || hanUtilsEntityRes;
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMngrInit( ST_HAN_CONFIG * pst_HANConfig)
{
	app_HAN_SetHANConfiguration(pst_HANConfig->u8_HANServiceConfig);
	return cmbs_dsr_han_mngr_Init( g_cmbsappl.pv_CMBSRef,pst_HANConfig);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMngrStart( void )
{
	return cmbs_dsr_han_mngr_Start( g_cmbsappl.pv_CMBSRef);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanDeviceReadTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8 isBrief)
{
	return cmbs_dsr_han_device_ReadTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry, isBrief);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanReadExtendedDeviceTablePhase2 (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_device_ReadExtendedDeviceTablePhase2(g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanReadBlackListedDeviceTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_device_ReadBlackListedDeviceTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanDeviceReadTableExtended (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_device_ReadTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry, (u8)FALSE);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanReadSingleDeviceRegistrationInformation(u16 u16_DeviceId)
{
	return cmbs_dsr_han_device_ReadDeviceRegistrationInformation(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanReadSingleDeviceRegistrationInformationPhase2(u16 u16_DeviceId)
{
	return cmbs_dsr_han_device_ReadDeviceRegistrationInformationPhase2(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHan_HanSetLogLevel(u8 u8_LogLevel)
{
	return cmbs_dsr_han_Logger_SetLogLevel(g_cmbsappl.pv_CMBSRef, u8_LogLevel);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHan_HanGetLogLevel(void)
{
	return cmbs_dsr_han_Logger_GetLogLevel(g_cmbsappl.pv_CMBSRef);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanLoggerEnable(bool b_enable)
{
	return cmbs_dsr_han_logger_Control(g_cmbsappl.pv_CMBSRef, b_enable);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanCpLoggerEnable(bool b_enable)
{
	return cmbs_dsr_han_cp_logger_Control(g_cmbsappl.pv_CMBSRef, b_enable);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanReadFullDeviceRegistrationInfo(u16 u16_DeviceId)
{
    return cmbs_dsr_han_device_ReadDeviceFullRegistrationInfo(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanWriteFullDeviceRegistrationInfo(u16 u16_DeviceId, ST_HAN_FULL_DEVICE_REGISTRATION_INFO *pst_DeviceRegistrationInfo)
{
    return cmbs_dsr_han_device_WriteDeviceFullRegistrationInfo(g_cmbsappl.pv_CMBSRef, u16_DeviceId, pst_DeviceRegistrationInfo);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanDeviceWriteTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_DEVICE_ENTRY * pst_HANDeviceEntriesArray)
{
	return cmbs_dsr_han_device_WriteTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry, pst_HANDeviceEntriesArray);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanBindReadTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_bind_ReadTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC app_DsrHanBindAddEntry (PST_HAN_BIND_ENTRY pst_HANBindEntry)
{
	return cmbs_dsr_han_bind_AddEntry (g_cmbsappl.pv_CMBSRef, pst_HANBindEntry);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC app_DsrHanBindRemoveEntry(PST_HAN_BIND_ENTRY pst_HANBindEntry)
{
	return cmbs_dsr_han_bind_RemoveEntry (g_cmbsappl.pv_CMBSRef, pst_HANBindEntry);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC app_DsrHanClearBindTable( void )
{
	return cmbs_dsr_han_bind_ClearTable (g_cmbsappl.pv_CMBSRef);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanBindWriteTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_BIND_ENTRY * pst_HANBindEntriesArray)
{
	return cmbs_dsr_han_bind_WriteTable(g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry, pst_HANBindEntriesArray);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGroupReadTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u16 u16_GroupId)
{
	return cmbs_dsr_han_group_ReadTable (g_cmbsappl.pv_CMBSRef,u16_NumOfEntries,u16_IndexOfFirstEntry, u16_GroupId);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGroupReadList (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_group_ReadList (g_cmbsappl.pv_CMBSRef,u16_NumOfEntries,u16_IndexOfFirstEntry);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanReadBroadcastCahnnelTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_Broadcast_ReadChannelTable(g_cmbsappl.pv_CMBSRef,u16_NumOfEntries,u16_IndexOfFirstEntry);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGroupWriteTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_GROUP_TABLE_ENTRY * pst_HANGroupEntriesArray)
{
	return cmbs_dsr_han_group_WriteTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry, pst_HANGroupEntriesArray);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC app_DsrHanGroupCreateGroup ( u8 u8_BroadcastChannelId )
{
	return cmbs_dsr_han_group_CreateGroup (g_cmbsappl.pv_CMBSRef, u8_BroadcastChannelId);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC app_DsrHanGroupRemoveGroup ( u16 u16_GroupId  )
{
	return cmbs_dsr_han_group_RemoveGroup (g_cmbsappl.pv_CMBSRef, u16_GroupId);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC app_DsrHanGroupAddDeviceToGroup ( u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId )
{
	return cmbs_dsr_han_group_AddDeviceToGroup (g_cmbsappl.pv_CMBSRef, u16_GroupId, u16_DeviceId, u8_UnitId);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC app_DsrHanGroupRemoveDeviceFromGroup ( u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId  )
{
	return cmbs_dsr_han_group_RemoveDeviceFromGroup (g_cmbsappl.pv_CMBSRef, u16_GroupId, u16_DeviceId, u8_UnitId);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMsgRecvRegister (ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo)
{
	return cmbs_dsr_han_msg_RecvRegister (g_cmbsappl.pv_CMBSRef, pst_HANMsgRegInfo);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMsgRecvUnregister (ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo)
{
	return cmbs_dsr_han_msg_RecvUnregister (g_cmbsappl.pv_CMBSRef,pst_HANMsgRegInfo);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMsgSendTxRequest (u16 u16_DeviceId)
{
	return cmbs_dsr_han_msg_SendTxRequest (g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMsgSendTxEnd(u16 u16_DeviceId)
{
	return cmbs_dsr_han_msg_SendTxEnd(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMsgSend (u16 u16_RequestId, u16 u16_DestDeviceId, ST_IE_HAN_MSG_CTL* pst_HANMsgCtl , ST_IE_HAN_MSG * pst_HANMsg )
{
	UNUSED_PARAMETER(u16_DestDeviceId);
	return cmbs_dsr_han_msg_Send (g_cmbsappl.pv_CMBSRef, u16_RequestId, pst_HANMsgCtl, pst_HANMsg);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanRawMsgSend (u16 u16_RequestId, u16 u16_DestDeviceId, ST_IE_HAN_MSG_CTL* pst_HANMsgCtl ,ST_IE_DATA * pst_HANRawMsg)
{
	return cmbs_dsr_han_raw_msg_Send (g_cmbsappl.pv_CMBSRef, u16_RequestId, pst_HANMsgCtl, pst_HANRawMsg, u16_DestDeviceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


E_CMBS_RC app_DsrHanGetMaxNumOfDevices(void)
{
	return cmbs_dsr_han_GetMaxNumOfDevices(g_cmbsappl.pv_CMBSRef);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanDeleteDevice ( u16 u16_DeviceId, bool b_BlackList )
{
	if(b_BlackList)
		return cmbs_dsr_han_DeviceBlackListDelete(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
	else
		return cmbs_dsr_han_device_Delete(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanSendBaseUpdatedNotification ( u16 u16_DeviceId )
{
	return cmbs_dsr_han_send_base_updated_notification (g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGetFunProtocolInfo ()
{
	return cmbs_dsr_han_get_fun_protocol_info(g_cmbsappl.pv_CMBSRef);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGetDeviceConnectionStatus ( u16 u16_DeviceId )
{
	return cmbs_dsr_han_device_GetConnectionStatus(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanFunRegResMsgSend( PST_FUN_DEVICE_INFO pst_FunDeviceInfo )
{
	return cmbs_dsr_han_fun_RegistationMsgRecv (g_cmbsappl.pv_CMBSRef, pst_FunDeviceInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanNotifyOnVoiceCall(u16 u16_DeviceId)
{
	return cmbs_dsr_han_NotifyOnVoiceCall (g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanMarkCcmKeyAsUsed( u16 u16_DeviceNumber , u8 u8_CCMKeyUsed)
{
	return cmbs_dsr_han_Certification_MarkCcmKeyAsUsed(g_cmbsappl.pv_CMBSRef, u16_DeviceNumber, u8_CCMKeyUsed);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanSetTxMicCorruptNum(u16 u16_DeviceNumber)
{
	return cmbs_dsr_han_Certification_SetTxMicCorruptNum(g_cmbsappl.pv_CMBSRef, u16_DeviceNumber);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanSetTxSeqNum(u16 u16_DeviceNumber, u16 u16_TxSequenceNumber )
{
	return cmbs_dsr_han_Certification_SetTxSeqNum (g_cmbsappl.pv_CMBSRef, u16_DeviceNumber, u16_TxSequenceNumber);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGetTxSeqNum(u16 u16_DeviceNumber)
{
	return cmbs_dsr_han_Certification_GetTxSeqNum(g_cmbsappl.pv_CMBSRef, u16_DeviceNumber);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGetRxSeqNum(u16 u16_DeviceNumber)
{
	return cmbs_dsr_han_Certification_GetRxSeqNum(g_cmbsappl.pv_CMBSRef, u16_DeviceNumber);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanSetTxAckDropNum(u16 u16_TxAckDropNumber)
{
	return cmbs_dsr_han_Certification_SetTxAckDropNum(g_cmbsappl.pv_CMBSRef, u16_TxAckDropNumber);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanSetPvcState(u16 u16_DeviceNumber, u8 u8_PVCState )
{
	return cmbs_dsr_han_Certification_SetPvcState(g_cmbsappl.pv_CMBSRef, u16_DeviceNumber, u8_PVCState);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanSetRxGain(u8 u8_RxGainVal )
{
	return cmbs_dsr_han_SetRxGain(g_cmbsappl.pv_CMBSRef, u8_RxGainVal);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGetRxGain(void )
{
	return cmbs_dsr_han_GetRxGain(g_cmbsappl.pv_CMBSRef);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanSendPvcResetReq( u16 u16_DeviceNumber )
{
	return cmbs_dsr_han_SendPvcResetReq(g_cmbsappl.pv_CMBSRef, u16_DeviceNumber);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_DsrHanGetPvcResetReqState( u16 u16_DeviceNumber )
{
	return cmbs_dsr_han_GetPvcResetReqState(g_cmbsappl.pv_CMBSRef, u16_DeviceNumber);
}

//////////////////////////////////////////////////////////////////////////

void app_HanOnReadDeviceTableRes(void *pv_EventData)
{
	UNUSED_PARAMETER(pv_EventData);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void app_HanOnInitStartRes				(void* pv_EventData,	E_CMBS_EVENT_ID e_EventID	)
{
	char chRetCode =  app_ResponseCheck( pv_EventData ) ? CMBS_RC_ERROR_GENERAL : CMBS_RC_OK;

	if ( g_cmbsappl.n_Token )
    {
		appcmbs_ObjectSignal( &chRetCode, 1, 0, e_EventID);
	}
}


E_CMBS_RC app_DsrHanTBR6SetPM(u8  u8_Carrier , u32 u32_Period )
{
	return cmbs_dsr_han_TBR6SendPM(g_cmbsappl.pv_CMBSRef, u8_Carrier, u32_Period);
}

E_CMBS_RC app_DsrHanTBR6SetGUCI(u8 *  pu8_IPUI )
{
	return cmbs_dsr_han_TBR6SetGUCI(g_cmbsappl.pv_CMBSRef, pu8_IPUI);
}

E_CMBS_RC app_DsrHanTBR6ResetStatistics(void )
{
	return cmbs_dsr_han_TBR6ResetStatistics(g_cmbsappl.pv_CMBSRef);
}

E_CMBS_RC app_DsrHanModificationULEPagingInterval (u16 u16_DeviceId, u32 u32_ULEPagingIntervalMs )
{
	return cmbs_dsr_han_ModificationULEPagingInterval(u16_DeviceId, u32_ULEPagingIntervalMs);
}

E_CMBS_RC app_DsrHanNodeSetupAttemptsEnable (bool b_enable)
{
	return cmbs_dsr_han_NodeSetupAttemptsEnable(b_enable);
}


typedef struct
{
	u8 		SizeInBytes;
	int		NumOfElements;
	char* 	Name;
}keyb_han_AttributeDefStruct;

#define STRING_WITH_SIZE_ELEMENT			0	// 0 is a special case where the first byte is the size and the rest is a string
#define ONE_BYTE_ELEMENT					1
#define TWO_BYTES_ELEMENT					2
#define FOUR_BYTES_ELEMENT				4
#define CONTAINER_FOR_FRIENDLY_NAMES 	5
#define CONTAINER_FOR_UID					6

keyb_han_AttributeDefStruct g_DeviceIFAttributes[] =

{
    {ONE_BYTE_ELEMENT,				1,	"HF Core Release version "},					// HAN_IF_DEVICE_INFORMATION_ATTR_HF_RELEASE_VER
    {ONE_BYTE_ELEMENT,				1,	"Profile Release version "},					// HAN_IF_DEVICE_INFORMATION_ATTR_PROF_RELEASE_VER
    {ONE_BYTE_ELEMENT,				1,	"Interface Release version "},					// HAN_IF_DEVICE_INFORMATION_ATTR_IF_RELEASE_VER
    {ONE_BYTE_ELEMENT,				1,	"Paging Capabilities "},						// HAN_IF_DEVICE_INFORMATION_ATTR_PAGING_CAPS
    {ONE_BYTE_ELEMENT,				4,	"Minimum Sleep Time "},							// HAN_IF_DEVICE_INFORMATION_ATTR_MIN_SLEEP_TIME
    {ONE_BYTE_ELEMENT,				4,	"Actual Response Time (Paging Interval)  "},	// HAN_IF_DEVICE_INFORMATION_ATTR_ACT_RESP_TIME 	
    {STRING_WITH_SIZE_ELEMENT,		1,	"Application version "},						// HAN_IF_DEVICE_INFORMATION_ATTR_APPL_VER
    {STRING_WITH_SIZE_ELEMENT,		1,	"Hardware version "},							// HAN_IF_DEVICE_INFORMATION_ATTR_HW_VER
    {ONE_BYTE_ELEMENT,				2,	"EMC "},										// HAN_IF_DEVICE_INFORMATION_ATTR_EMC
    {ONE_BYTE_ELEMENT,				5,	"IPUI/ RFPI "},									// HAN_IF_DEVICE_INFORMATION_ATTR_IPUE
    {STRING_WITH_SIZE_ELEMENT,		1,	"Manufacture Name "},							// HAN_IF_DEVICE_INFORMATION_ATTR_MANUF_NAME
    {STRING_WITH_SIZE_ELEMENT,		1,	"Location "},									// HAN_IF_DEVICE_INFORMATION_ATTR_LOCATION
    {ONE_BYTE_ELEMENT,				1,	"Device Enable "},								// HAN_IF_DEVICE_INFORMATION_ATTR_DEV_ENABLE
    {CONTAINER_FOR_FRIENDLY_NAMES,	0,	"Friendly Name "},								// HAN_IF_DEVICE_INFORMATION_ATTR_FRIENDLY_NAME
    {CONTAINER_FOR_UID,				0,	"UID "},										// HAN_IF_DEVICE_INFORMATION_ATTR_UID
    {STRING_WITH_SIZE_ELEMENT,		1,	"Serial Number "}								// HAN_IF_DEVICE_INFORMATION_ATTR_SERIAL_NUMBER
};


char* g_GetPackResponceDataValues[]=
{
	"OK",						// CMBS_HAN_DEV_INFO_RESP_VAL_OK
	"Fail:Not Supported",		// CMBS_HAN_DEV_INFO_RESP_VAL_FAIL_NOT_SUPPORTED	
	"Fail:Unknown Reason",		// CMBS_HAN_DEV_INFO_RESP_VAL_FAIL_UNKNOWN
};

#define KEYB_HAN_GET_PACK_RESP_RESP_VALUE_INDEX 	0
#define KEYB_HAN_GET_PACK_RESP_NUM_OF_ATTR_INDEX 	1
#define KEYB_HAN_GET_PACK_RESP_ATTR_START_INDEX 	2

#define KEYB_HAN_GET_ATTR_RESP_ATTR_START_INDEX 	1

int app_msg_parser_Parse_Interface_DeviceInfo_AttrGetResParse(u8* AttrData,u16 AttrId )
{
	
	u8 		u8_AttrVal;
	u8 		u8_NumOfUnits;
	u8 		u8_UnitId, u8_Size;
	u8 		u8_UidType;
	u16		u16_AttrVal;
	u32		u32_AttrVal;
	int 	DataElement;
	int 	count = 0;
	
	HAN_APP_INFO_PRINT("\t\t%s : \n",g_DeviceIFAttributes[AttrId].Name);

	switch(g_DeviceIFAttributes[AttrId].SizeInBytes)
	{
		case STRING_WITH_SIZE_ELEMENT:
			// First element (size)
			cfr_ie_dser_u8(AttrData+count,&u8_AttrVal );
			count++;
			// Now read the data
			HAN_APP_INFO_PRINT("\t\t\t%.*s\n",u8_AttrVal,AttrData+count);
			count += u8_AttrVal;
			break;
			
		case ONE_BYTE_ELEMENT:
			for ( DataElement = 0;  DataElement <  g_DeviceIFAttributes[AttrId].NumOfElements; DataElement++)
			{
				// we are going to de-serialize N elements of u8
				cfr_ie_dser_u8(AttrData+count,&u8_AttrVal );
				HAN_APP_INFO_PRINT("\t\t\t%d  \n",u8_AttrVal);
				count++;
			}
			break;
			
		case TWO_BYTES_ELEMENT:
			for ( DataElement = 0;  DataElement <  g_DeviceIFAttributes[AttrId].NumOfElements; DataElement++)
			{
				// we are going to de-serialize N elements of u16
				cfr_ie_dser_u16(AttrData+count,&u16_AttrVal );
				HAN_APP_INFO_PRINT("\t\t\t%d  \n",u16_AttrVal);
				count += 2;
			}
			break;
			
		case FOUR_BYTES_ELEMENT:
			for ( DataElement = 0;  DataElement <  g_DeviceIFAttributes[AttrId].NumOfElements; DataElement++)
			{
				// we are going to de-serialize N elements of u32
				cfr_ie_dser_u32(AttrData+count,&u32_AttrVal );
				HAN_APP_INFO_PRINT("\t\t\t%d  \n",u32_AttrVal);
				count += 4;
			}
			break;

		case CONTAINER_FOR_FRIENDLY_NAMES:
			
			// First element - number of unit that currently have friendly names defined.
			cfr_ie_dser_u8(AttrData+count,&u8_NumOfUnits );
			count++;

			for ( DataElement = 0;  DataElement <  u8_NumOfUnits; DataElement++)
			{
				// next byte stores unit identifier to whom Name field refers to
				cfr_ie_dser_u8(AttrData+count,&u8_UnitId);
				HAN_APP_INFO_PRINT("\t\t\tUnit %d : \n",u8_UnitId);
				count++;
				
				// next byte stores length, in byte, of Name field. 
				cfr_ie_dser_u8(AttrData+count,&u8_AttrVal );
				count++;
				
				// Now read the data
				HAN_APP_INFO_PRINT("\t\t\t\t%.*s\n",u8_AttrVal,AttrData+count);
				count += u8_AttrVal;
			}
			break;
			
		case CONTAINER_FOR_UID:
							
			// First element - UID Type
			cfr_ie_dser_u8(AttrData+count,&u8_UidType );
			count++;

			switch (u8_UidType)
				{
					case DEV_INF_IF_ATTR_UID_UID_TYPE_MAC:
							HAN_APP_INFO_PRINT("[MAC]  \n");
						break;

					case DEV_INF_IF_ATTR_UID_UID_TYPE_DECT:
							HAN_APP_INFO_PRINT("[IPUI/ RFPI] \n");
						break;

					case DEV_INF_IF_ATTR_UID_UID_TYPE_URI:
							HAN_APP_INFO_PRINT("[URI] \n");
						break;

					default:
						break;
				}

			// Second element (size)
			cfr_ie_dser_u8(AttrData+count,&u8_Size);
			count++;

			for ( DataElement = 0;  DataElement<u8_Size; DataElement++)
			{
				cfr_ie_dser_u8(AttrData+count,&u8_AttrVal );
				HAN_APP_INFO_PRINT("%d  \n",u8_AttrVal);
				count++;
			}

			
		default:
			break;
	}

	return count;

}

void app_msg_parser_Parse_Interface_DeviceInfo_AttrGetRes(ST_IE_HAN_MSG* stIe_Msg)
{
	u8*		AttrData;
	u8*		Payload = stIe_Msg->pu8_Data;

		
	HAN_APP_INFO_PRINT( "DEVICE INFO GET ATTR RES) === : \n");
	HAN_APP_INFO_PRINT( "\tDevice Id: %u\n", stIe_Msg->u16_SrcDeviceId);
	HAN_APP_INFO_PRINT( "\tUnit Id: %u\n", stIe_Msg->u8_SrcUnitId);
	HAN_APP_INFO_PRINT( "\tInterface Id: %u\n", stIe_Msg->u16_InterfaceId);
	HAN_APP_INFO_PRINT( "\tAttribute Id: %u\n", stIe_Msg->u8_InterfaceMember);	
	
	HAN_APP_INFO_PRINT( "\tResponse Value : %s\n", g_GetPackResponceDataValues[Payload[KEYB_HAN_GET_PACK_RESP_RESP_VALUE_INDEX]]);

	
	AttrData = &Payload[KEYB_HAN_GET_ATTR_RESP_ATTR_START_INDEX];
	AttrData += app_msg_parser_Parse_Interface_DeviceInfo_AttrGetResParse(	AttrData,
																			stIe_Msg->u8_InterfaceMember-1 );
}

void app_msg_parser_Parse_Interface_DeviceInfo_PackGetRes(ST_IE_HAN_MSG* stIe_Msg)
{
	int 	Attr;
	u8*		AttrData;
	u8*		Payload = stIe_Msg->pu8_Data;
	int		NumOfAttributes;
	u8		AttrId;

		
	HAN_APP_INFO_PRINT( "DEVICE INFO PACK RES) === : \n");
	HAN_APP_INFO_PRINT( "\tDevice Id: %u\n", stIe_Msg->u16_SrcDeviceId);
	HAN_APP_INFO_PRINT( "\tUnit Id: %u\n", stIe_Msg->u8_SrcUnitId);
	HAN_APP_INFO_PRINT( "\tInterface Id: %u\n", stIe_Msg->u16_InterfaceId);
	
	HAN_APP_INFO_PRINT( "\tResponse Value : %s\n", g_GetPackResponceDataValues[Payload[KEYB_HAN_GET_PACK_RESP_RESP_VALUE_INDEX]]);
	HAN_APP_INFO_PRINT( "\tNumber of Attributes : %u\n", NumOfAttributes= Payload[KEYB_HAN_GET_PACK_RESP_NUM_OF_ATTR_INDEX]);

	
	AttrData = &Payload[KEYB_HAN_GET_PACK_RESP_ATTR_START_INDEX];
	for ( Attr = 0;  Attr < NumOfAttributes; Attr++ )
	{
		AttrId = (*AttrData-1); // Get The AttribId and advance the pointer. One Based
		AttrData++;
		AttrData += app_msg_parser_Parse_Interface_DeviceInfo_AttrGetResParse(	AttrData,
																				AttrId );

		
	}
	
}

void app_msg_parser_Parse_Interface_AttrRep_Report(ST_IE_HAN_MSG* stIe_Msg)
{
	// 1 Byte - Report id (MSB bit is type) 
	// Entry 1
		//	1 Byte - Sender Unit Id
		//  2 Bytes - IF type + IF Id
		//  1 Byte - num of attributes
		//	{
		// 		1 Byte - attribute Id
		// 		if event report - 1 Byte - Type (COV, HT, LT, EQUAL)
		// 		n bytes - value
		//	}
		//	{
		//		...
		//	}
	// Entry 2
		//	1 Byte - Sender Unit Id
		//  2 Bytes - IF type + IF Id
		//  1 Byte - num of attributes
		//	{
		// 		1 Byte - attribute Id
		// 		if event report - 1 Byte - Type (COV, HT, LT, EQUAL)
		// 		n bytes - value
		//	}
		//	{
		//		...
		//	}
	//...


	u16 Location = 0;
	u32 NumOfAttributes = 0;
	u32 i;
	u32 dataLeftForAttribute = 0;
	u32 indexEntry = 1;
	u16 fullInterfaceInfo = 0;
	u8 InterfaceType = INTERFACE_TYPE_CLIENT;
	u16 InterfaceID = 0;
	u8 AttributeID = 0;
	u16  temp;
	s_AttributeData attributeData;
	char AttributeValue[CMBS_HAN_MAX_MSG_LEN];
	u32 AttributeValueLen = 0;
	u32 defaultBytesInEntry = ENTRY_UNIT_ID_SIZE_IN_BYTES + ENTRY_IF_SIZE_IN_BYTES + ENTRY_NUM_OF_ATTRIBUTES_SIZE_IN_BYTES ;
	
	HAN_APP_INFO_PRINT("=== ATTRIBUTE REPORT INTERFACE REPORT RECEIVED ===\n");

	if ( stIe_Msg->u8_InterfaceMember == CMBS_HAN_ATTR_REPORT_S2C_CMD_ID_PERIODIC_REPORT_NOTIFICATION )
	{
		HAN_APP_INFO_PRINT("Report Type : Periodic \n");
		dataLeftForAttribute = 2;
	}
	else if ( stIe_Msg->u8_InterfaceMember == CMBS_HAN_ATTR_REPORT_S2C_CMD_ID_EVENT_REPORT_NOTIFICATION )
	{
		HAN_APP_INFO_PRINT("Report Type : Event \n");
		dataLeftForAttribute = 3;
	}
	else
	{
		HAN_APP_INFO_PRINT("UNKNOWN REPORT TYPE \n");
	}

	if (stIe_Msg->u16_DataLen)
	{
		HAN_APP_INFO_PRINT("Report ID 		(hex): 0x%x \n", stIe_Msg->pu8_Data[Location++]); 
	}

	while (Location < stIe_Msg->u16_DataLen)
	{
		HAN_APP_INFO_PRINT("Entry number #%d :\n",indexEntry );
		if (Location + defaultBytesInEntry <  stIe_Msg->u16_DataLen)
		{
		    HAN_APP_INFO_PRINT("\tUnit ID	  		(dec): %d \n",stIe_Msg->pu8_Data[Location++]);
		    temp = stIe_Msg->pu8_Data[Location++] << 8;
			fullInterfaceInfo = temp | stIe_Msg->pu8_Data[Location++];
			if((fullInterfaceInfo & INTERFACE_TYPE_MASK) == INTERFACE_TYPE_MASK)
			{
				InterfaceType = INTERFACE_TYPE_SERVER;
			}
			HAN_APP_INFO_PRINT("\tInterface Type 	(hex): 0x%x \n",InterfaceType);
		    HAN_APP_INFO_PRINT("\tInterface ID 	(hex): 0x%x \n",InterfaceID = (fullInterfaceInfo & INTERFACE_ID_MASK));
		    HAN_APP_INFO_PRINT("\tNumber of Attributes : %d \n", NumOfAttributes=stIe_Msg->pu8_Data[Location++] );
		}
		else
		{
		    HAN_APP_ERROR_PRINT("WRONG REPORT FORMAT \n");
		}

		for ( i = 1;  i <= NumOfAttributes; i++ )
		{
		    if ((Location + dataLeftForAttribute) <= stIe_Msg->u16_DataLen)
		    {
				HAN_APP_INFO_PRINT("\t\tAttribute number #%d :\n",i );
				HAN_APP_INFO_PRINT("\t\tAttribute ID (dec) : %d \n",AttributeID = stIe_Msg->pu8_Data[Location++]);
				if(stIe_Msg->u8_InterfaceMember == CMBS_HAN_ATTR_REPORT_S2C_CMD_ID_EVENT_REPORT_NOTIFICATION)
				{
					HAN_APP_INFO_PRINT("\t\tType of reporting :\n");
					switch ( stIe_Msg->pu8_Data[Location++] )
					{
						case CMBS_HAN_ATTR_REPORT_REPORT_TYPE_COV:
							HAN_APP_INFO_PRINT("\t\tCOV \n");
						break;
						case CMBS_HAN_ATTR_REPORT_REPORT_TYPE_HT:
							HAN_APP_INFO_PRINT("\t\tHT \n");
						break;
						case CMBS_HAN_ATTR_REPORT_REPORT_TYPE_LH:
							HAN_APP_INFO_PRINT("\t\tLH \n");
						break;
						case CMBS_HAN_ATTR_REPORT_REPORT_TYPE_EQUAL:
							HAN_APP_INFO_PRINT("\t\tEQUAL \n");
						break;
						default:
							HAN_APP_INFO_PRINT("\t\tUNKNOWN - Error \n");
						break;
					}
				}

				app_GetAttributeNameAndType(InterfaceType,InterfaceID,AttributeID, &attributeData);
				HAN_APP_INFO_PRINT("\t\tAttribute Name: %s\n", attributeData.attributeName);
				AttributeValueLen = app_GetAttributeValueAsString(stIe_Msg->pu8_Data,Location,attributeData.attributeType,&AttributeValue[0],stIe_Msg->u16_DataLen);
				switch(attributeData.attributeType)
				{
					case HAN_ATTRIBUTE_TYPE_U8:
					case HAN_ATTRIBUTE_TYPE_U16:
					case HAN_ATTRIBUTE_TYPE_S16:	
					case HAN_ATTRIBUTE_TYPE_U32:
					case HAN_ATTRIBUTE_TYPE_U8_U32:
					case HAN_ATTRIBUTE_TYPE_U8_U16:
					case HAN_ATTRIBUTE_TYPE_U40:
						HAN_APP_INFO_PRINT("\t\tAttribute Value (hex) : 0x%s \n", AttributeValue); 
						break;
					case HAN_ATTRIBUTE_TYPE_STRING:
					case HAN_ATTRIBUTE_TYPE_FRIENDLY_NAMES:
					case HAN_ATTRIBUTE_TYPE_UID:
					{
						HAN_APP_INFO_PRINT("\t\tAttribute Value (String) : 0x%s \n", AttributeValue); 
						break;
					}
					default:
					{
						HAN_APP_INFO_PRINT("\t\tAttribute Value (hex):\n");
						HAN_APP_PRINT_DATA((u8*)AttributeValue,AttributeValueLen);
					}
						
				}			
				Location += AttributeValueLen;
		    }
		    else
		    {
				HAN_APP_ERROR_PRINT("WRONG REPORT FORMAT \n");
		    }
		}
		indexEntry++;
	}

}

void app_GetAttributeNameAndType(u8 InterfaceType,u16 InterfaceID,u8 AttributeID,s_AttributeData *attributeData)
{
	attributeData->attributeType=HAN_ATTRIBUTE_TYPE_UNKNOWN;

	// default member name
	attributeData->attributeName[0] = '\0';

	switch(InterfaceID)
	{
		case ATTRIBUTE_REPORTING_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_ATTR_REPORT_ATTR_NUM_OF_REPORTS: sprintf(attributeData->attributeName," (Num Of Reports)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_ATTR_REPORT_ATTR_NUM_OF_PERIODIC_REPORTS: sprintf(attributeData->attributeName, " (Num Of Periodic Reports)"); attributeData->attributeType = HAN_ATTRIBUTE_TYPE_U16; break;
					case HAN_IF_ATTR_REPORT_ATTR_NUM_OF_EVENT_REPORTS: sprintf(attributeData->attributeName," (Num Of Events Reports)"); attributeData->attributeType = HAN_ATTRIBUTE_TYPE_U16; break;
				}
			}
			break;		
		}
		case DEVICE_INFORMATION_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_DEVICE_INFORMATION_ATTR_HF_RELEASE_VER: sprintf(attributeData->attributeName," (HF Core Release)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_PROF_RELEASE_VER: sprintf(attributeData->attributeName," (Profile Release)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_IF_RELEASE_VER: sprintf(attributeData->attributeName," (Interface Release)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_PAGING_CAPS: sprintf(attributeData->attributeName," (Extended Caps)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_MIN_SLEEP_TIME: sprintf(attributeData->attributeName," (Min Sleep Time)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U32;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_ACT_RESP_TIME: sprintf(attributeData->attributeName," (Actual Response Time)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U32;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_APPL_VER: sprintf(attributeData->attributeName," (Application Version)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_STRING;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_HW_VER: sprintf(attributeData->attributeName," (Hardware Version)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_STRING;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_EMC: sprintf(attributeData->attributeName," (EMC)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_IPUE: sprintf(attributeData->attributeName," (IPUI)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U40;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_MANUF_NAME: sprintf(attributeData->attributeName," (Manufacture)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_STRING;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_LOCATION: sprintf(attributeData->attributeName," (Location)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_STRING;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_DEV_ENABLE: sprintf(attributeData->attributeName," (Device Enable)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_FRIENDLY_NAME: sprintf(attributeData->attributeName," (Friendly Name)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_FRIENDLY_NAMES;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_UID:sprintf(attributeData->attributeName," (Device UID)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_UID;break;
					case HAN_IF_DEVICE_INFORMATION_ATTR_SERIAL_NUMBER:sprintf(attributeData->attributeName," (Serial)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_STRING;break;
				}
			}
			break;
		}
		case ON_OFF_INTERFACE_ID:		
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				// attribute Id's of interface
				switch(AttributeID)
				{
					case ON_OFF_IF_SERVER_ATTR_STATE: sprintf(attributeData->attributeName," (ON OFF State)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
				}
			}
			break;	
		}
		case LEVEL_CONTROL_INTERFACE_ID:		
		{
			if(InterfaceType==LEVEL_CONTROL_ATTR_LEVEL_INTERFACE_TYPE)
			{
				// attribute Id's of interface
				switch(AttributeID)
				{
					case HAN_IF_LEVEL_CONTROL_ATTR_LEVEL: sprintf(attributeData->attributeName," (Level)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
				}
			}
			break;	
		}

		
		case ALERT_INTERFACE_ID:		
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				// attribute Id's of interface
				switch(AttributeID)
				{
					case HAN_IF_ALERT_ATTR_STATE: sprintf(attributeData->attributeName," (Alert State)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U32;break;
				}
			}
			break;
		}
		case TAMPER_INTERFACE_ID:		
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				// attribute Id's of interface
				switch(AttributeID)
				{
					case HAN_IF_TAMPER_ATTR_STATE: sprintf(attributeData->attributeName," (Tamper State)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
				}
			}
			break;
		}
		case POWER_INTERFACE_ID:
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				// attribute Id's of interface
				switch(AttributeID)
				{
					case HAN_IF_POWER_ATTR_BATTERY_VOLTAGE: sprintf(attributeData->attributeName," (Battery Voltage)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_POWER_ATTR_BATTERY_VOLTAGE_STATUS: sprintf(attributeData->attributeName," (Batetry status)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_POWER_ATTR_BATTERY_VOLTAGE_MIN_TH: sprintf(attributeData->attributeName," (Batetry Min TH)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_POWER_ATTR_BATTERY_VOLTAGE_MAX_TH: sprintf(attributeData->attributeName," (Batetry Max TH)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
				}
			}
			break;
		}
		case RSSI_INTERFACE_ID:
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				// attribute Id's of interface
				switch(AttributeID)
				{
					case HAN_IF_RSSI_ATTR_VALUE: sprintf(attributeData->attributeName," (RSSI Value)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
				}
			}
			break;
		}

		case KEEP_ALIVE_INTERFACE_ID:	
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				// attribute Id's of interface
				switch(AttributeID)
				{
					case HAN_IF_KEEP_ALIVE_ATTR_INTERVAL: sprintf(attributeData->attributeName," (Keep Alive Interval)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U32;break;
				}
			}
			break;		
		}
		case TESTMODE_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_TESTMODE_ATTR_BULK_SIZE:	sprintf(attributeData->attributeName," (Bulk Size)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;		
					case HAN_IF_TESTMODE_ATTR_BULK_INTERVAL:	sprintf(attributeData->attributeName," (Bulk Interval in Seconds)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;		
					case HAN_IF_TESTMODE_ATTR_PACKET_INTERVAL: 	sprintf(attributeData->attributeName," (Packet Interval in 10msec Units)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;		
				}
			}
			break;
		}
		case PRICE_UPDATE_MANAGER_INTERFACE_ID: 
		{
//			if(InterfaceType==INTERFACE_TYPE_SERVER)
//			{
				switch(AttributeID)
				{

					case HAN_IF_PRICE_UPDATE_MANAGER_ATTR_SCHEDULED_TIME: sprintf(attributeData->attributeName," (Scheduled Time)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U32;break;					
					case HAN_IF_PRICE_UPDATE_MANAGER_ATTR_CURRNET_FMF: sprintf(attributeData->attributeName," (Current Multiframe+Frame)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U32;break;					
					case HAN_IF_PRICE_UPDATE_MANAGER_ATTR_RETRY: sprintf(attributeData->attributeName," (Retry)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;					
				}
//			}
			break;
		}
		case GNRGY_METERING_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_CLIENT)
			{
				switch(AttributeID)
				{
					case HAN_IF_GNRGY_METERING_ATTR_KEEPALIVE: sprintf(attributeData->attributeName," (Metering report interval seconds)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;					
				}
			}
			break;
		}
		case SIMPLE_POWER_METERING_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_ENERGY: sprintf(attributeData->attributeName," (Energy)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;									
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_ENERGY_LAST_RESET	: sprintf(attributeData->attributeName," (Energy Last Reset)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;									
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_TIME_LAST_RESET: sprintf(attributeData->attributeName," (Time Last Reset)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_INSTANT_POWER	: sprintf(attributeData->attributeName," (Instantanous Power)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;	
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_AVG_POWER		: sprintf(attributeData->attributeName," (Average Power)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;	
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_AVG_POWER_INTERVAL: sprintf(attributeData->attributeName," (Average Power interval)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_VOLTAGE: sprintf(attributeData->attributeName," (Voltage)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;		
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_CURRENT: sprintf(attributeData->attributeName," (Current)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;					
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_FREQUENCY:	sprintf(attributeData->attributeName," (Frequency)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U32;break;						
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_POWER_FACOR: sprintf(attributeData->attributeName," (Power Factor)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;		
					case HAN_IF_SIMPLE_POWER_METERING_ATTR_KEEPALIVE: sprintf(attributeData->attributeName," (Metering report interval)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;					
				}
			}
			break;
		}
		case TEMPERATURE_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_TEMPERATURE_ATTR_TEMPERATURE: sprintf(attributeData->attributeName," (Temp)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;									
					case HAN_IF_TEMPERATURE_ATTR_MIN_TEMPERATURE: sprintf(attributeData->attributeName," (Min Temp)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;													
					case HAN_IF_TEMPERATURE_ATTR_MAX_TEMPERATURE: sprintf(attributeData->attributeName," (Max Temp)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;													
					case HAN_IF_TEMPERATURE_ATTR_TOLERANCE: sprintf(attributeData->attributeName," (Tolernace)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;														
				}
			}
			break;
		}
		case AIR_PRESSURE_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_AIR_PRESSURE_ATTR_MEASURED_PRESSURE: sprintf(attributeData->attributeName," (Air Pressure)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;									
					case HAN_IF_AIR_PRESSURE_ATTR_MEASURED_MIN_PRESSURE: sprintf(attributeData->attributeName," (Min Pressure)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;													
					case HAN_IF_AIR_PRESSURE_ATTR_MEASURED_MAX_PRESSURE: sprintf(attributeData->attributeName," (Max Pressure)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;													
					case HAN_IF_AIR_PRESSURE_ATTR_MEASURED_TOLERANCE: sprintf(attributeData->attributeName," (Tolernace)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;														
				}
			}
			break;
		}
		case EVS_HUMIDITY_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_EVS_HUMIDITY_ATTR_HUMIDITY: sprintf(attributeData->attributeName," (Humidity)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U16;break;									
					default : sprintf(attributeData->attributeName," (unknown)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U16;break;
				}
			}
			break;
		}
		case BUTTON_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_BUTTON_ATTR_SHORT_MAX_DURAION: sprintf(attributeData->attributeName," (Short Max Duration)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;									
					case HAN_IF_BUTTON_ATTR_EXTRA_LONG_MAX_DURAION: sprintf(attributeData->attributeName," (Extra Long Max Duration)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;									
					case HAN_IF_BUTTON_ATTR_DOUBLE_GAP_DURATION: sprintf(attributeData->attributeName," (Double Gap Duration)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;									
					default : sprintf(attributeData->attributeName," (unknown)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U16;break;
				}
			}
			break;
		}				
		case THERMOSTAT_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_THERMOSTAT_ATTR_SUPPORTED_MODES: sprintf(attributeData->attributeName," (Supported Modes)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;									
					case HAN_IF_THERMOSTAT_ATTR_CURRENT_MODE: sprintf(attributeData->attributeName," (Current Mode)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;									
					case HAN_IF_THERMOSTAT_ATTR_FAN_MODE: sprintf(attributeData->attributeName," (Fan Mode)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;											
					case HAN_IF_THERMOSTAT_ATTR_HEAT_MODE_TEMP: sprintf(attributeData->attributeName," (Heat Mode Temp)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;														
					case HAN_IF_THERMOSTAT_ATTR_COOL_MODE_TEMP: sprintf(attributeData->attributeName," (Cool Mode Temp)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;														
					case HAN_IF_THERMOSTAT_ATTR_AUTO_MODE_HEAT_TEMP: sprintf(attributeData->attributeName," (Auto Mode Heat Temp)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;																
					case HAN_IF_THERMOSTAT_ATTR_AUTO_MODE_COOL_TEMP: sprintf(attributeData->attributeName," (Auto Mode Cool Temp)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;																
					case HAN_IF_THERMOSTAT_ATTR_HEAT_MODE_TEMP_OFFSET: sprintf(attributeData->attributeName," (Heat Mode Temp Offset)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;	
					case HAN_IF_THERMOSTAT_ATTR_COOL_MODE_TEMP_OFFSET: sprintf(attributeData->attributeName," (Cool Mode Temp Offset)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_S16;break;	
					case HAN_IF_THERMOSTAT_ATTR_BOOST_DURATION: sprintf(attributeData->attributeName," (Boost Duration)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;	
					default : sprintf(attributeData->attributeName," (unknown)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8_U16;break;
				}				
			}
			break;
		}
		case DEVICE_STATISTICS_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_DEVICE_STATISTICS_ATTR_TX_SUCC: sprintf(attributeData->attributeName," (TX Succ)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_STATISTICS_ATTR_TX_FAIL: sprintf(attributeData->attributeName," (TX Fail)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_STATISTICS_ATTR_RX_PACKETS: sprintf(attributeData->attributeName," (RX Packets)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_STATISTICS_ATTR_LOCK_SUCC: sprintf(attributeData->attributeName," (Lock Succ)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_STATISTICS_ATTR_LOCK_FAIL: sprintf(attributeData->attributeName," (Lock Fail)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_STATISTICS_ATTR_PINGS: sprintf(attributeData->attributeName," (Pings)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_STATISTICS_ATTR_ADD_QUEUE_FAIL: sprintf(attributeData->attributeName," (Add Queue Fail)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
					case HAN_IF_DEVICE_STATISTICS_ATTR_SETUP_AGAIN: sprintf(attributeData->attributeName," (Setup Again)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U16;break;
				}
			}
			break;
		}
		case WEEKLY_SCHEDULE_INTERFACE_ID: 
		{
			if(InterfaceType==INTERFACE_TYPE_SERVER)
			{
				switch(AttributeID)
				{
					case HAN_IF_WEEKLY_SCHEDULE_ATTR_MAX_NUM_OF_ENTRIES: sprintf(attributeData->attributeName," (Max num of weekly entries)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_WEEKLY_SCHEDULE_ATTR_NUM_OF_ENTRIES: sprintf(attributeData->attributeName," (Num of weekly entries)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
					case HAN_IF_WEEKLY_SCHEDULE_ATTR_STATUS: sprintf(attributeData->attributeName," (Weekly Scheduler status)");attributeData->attributeType=HAN_ATTRIBUTE_TYPE_U8;break;
				}
			}
			break;
		}
	}
}

int app_GetAttributeValueAsString(u8 * pu8_Data,u16 Location,E_Attribute_Type AttributeType,char* AttributeValue, u16 pu8_Data_Length)
{
	int ParsedLen=0;
	u8   i = 0;
	u8   j = 0;
	u8   temp_8;
	u8	 exponent;
	u8   nameLen;
	u16  temp_16;
	u32  temp_32;

	switch(AttributeType)
	{
		case HAN_ATTRIBUTE_TYPE_U8:
		{
			sprintf(&AttributeValue[0],"%02X",pu8_Data[Location]);
			ParsedLen = 1;
			break;
		}
		case HAN_ATTRIBUTE_TYPE_U16:
		case HAN_ATTRIBUTE_TYPE_S16:
		{
			temp_16 = pu8_Data[Location++] << 8;
			temp_16 = (temp_16) | pu8_Data[Location];
			sprintf(&AttributeValue[0],"%04X",temp_16);
			ParsedLen = 2;
			break;
		}
		case HAN_ATTRIBUTE_TYPE_U32:
		{
			temp_32 = pu8_Data[Location++] << 24;
			temp_32 = (temp_32) | (pu8_Data[Location++] << 16);
			temp_32 = (temp_32) | (pu8_Data[Location++] << 8);
			temp_32 = (temp_32) | (pu8_Data[Location]);
			
			sprintf(&AttributeValue[0],"%08X",temp_32);
			ParsedLen = 4;
			break;
		}
		case HAN_ATTRIBUTE_TYPE_U8_U32:
		{
			temp_8 = pu8_Data[Location++];
			
			temp_32 = pu8_Data[Location++] << 24;
			temp_32 = (temp_32) | (pu8_Data[Location++] << 16);
			temp_32 = (temp_32) | (pu8_Data[Location++] << 8);
			temp_32 = (temp_32) | (pu8_Data[Location]);

			switch(temp_8)
			{
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_MILI:	sprintf(&AttributeValue[0],"(mili)%d",temp_32);break;
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_MICRO:	sprintf(&AttributeValue[0],"(micro)%d",temp_32);break;
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_NANO:	sprintf(&AttributeValue[0],"(nano)%d",temp_32);break;
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_PICO:	sprintf(&AttributeValue[0],"(pico)%d",temp_32);break;
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_KILO:	sprintf(&AttributeValue[0],"(kilo)%d",temp_32);break;
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_MEGA:	sprintf(&AttributeValue[0],"(mega)%d",temp_32);break;
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_GIGA:	sprintf(&AttributeValue[0],"(giga)%d",temp_32);break;
				case HAN_IF_SIMPLE_POWER_METERING_ATTRVALUE_TERA:	sprintf(&AttributeValue[0],"(tera)%d",temp_32);break;
				default: 											sprintf(&AttributeValue[0],"%d",temp_32);break;
			}
			
			ParsedLen=5;
			break;
		}
		case HAN_ATTRIBUTE_TYPE_U8_U16:
		{

			exponent = pu8_Data[Location++];

			temp_16 = pu8_Data[Location++] << 8;
			temp_16 = (temp_16) | pu8_Data[Location];
			
			switch(exponent)
			{
				case 0:  sprintf(&AttributeValue[0],"%d",temp_16);break;
				default: sprintf(&AttributeValue[0],"(10^-%d)%d",exponent,temp_16);break;
			}
			
			ParsedLen=3;
			break;
		}
		case HAN_ATTRIBUTE_TYPE_U40:
		{
			u8 AttributeValueTypeU40[5];
			AttributeValueTypeU40[0] = pu8_Data[Location++];
			AttributeValueTypeU40[1] = pu8_Data[Location++];
			AttributeValueTypeU40[2] = pu8_Data[Location++];
			AttributeValueTypeU40[3] = pu8_Data[Location++];
			AttributeValueTypeU40[4] = pu8_Data[Location++];
			sprintf(&AttributeValue[0],"%02X%02X%02X%02X%02X",AttributeValueTypeU40[0],AttributeValueTypeU40[1],AttributeValueTypeU40[2],AttributeValueTypeU40[3],AttributeValueTypeU40[4]);
			ParsedLen=10;
			break;
		}
		case HAN_ATTRIBUTE_TYPE_STRING:
		{
			// get string len in bytes
			temp_8 = pu8_Data[Location++];
			
			//get string itself
			memcpy(AttributeValue,&pu8_Data[Location],temp_8);
			AttributeValue[temp_8] = '\0';		
			
			ParsedLen= temp_8 + 1;
			break;
		}
		case HAN_ATTRIBUTE_TYPE_FRIENDLY_NAMES:
		{
			// get number of unit names
			temp_8 = pu8_Data[Location++];
			j=0;
			
			ParsedLen=1;
			for(i=0; i < temp_8; i++)
			{
				// skip UnitID
				Location++;
				
				// get string len in bytes
				nameLen = pu8_Data[Location++];
		
				// get string itself
				memcpy(&AttributeValue[j],&pu8_Data[Location],nameLen);
				j += nameLen;
				Location += nameLen;

				// add \r\n only if not last unit
				if((i+1)!=temp_8)
				{
					AttributeValue[j] = '\r';
					AttributeValue[j+1] = '\n';
					j=j+2;
				}

			}
			AttributeValue[j] = '\0';
			break;
		}
		case HAN_ATTRIBUTE_TYPE_UID:
		{
			// get type of UID
			temp_8 = pu8_Data[Location++];
			
			switch(temp_8)
			{
				case DEV_INF_IF_ATTR_UID_UID_TYPE_NONE: sprintf(AttributeValue,"%s","(None) ");j=7;break;
				case DEV_INF_IF_ATTR_UID_UID_TYPE_DECT: sprintf(AttributeValue,"%s","(DECT) ");j=7;break;
				case DEV_INF_IF_ATTR_UID_UID_TYPE_MAC: sprintf(AttributeValue,"%s","(MAC) ");j=6;break;
				case DEV_INF_IF_ATTR_UID_UID_TYPE_URI: sprintf(AttributeValue,"%s","(URI) ");j=6;break;
				default: sprintf(AttributeValue,"%s","(Unknown) ");j=10;break;
			}
			

			// get uid len
			temp_8 = pu8_Data[Location++];

			//get string itself
			memcpy(&AttributeValue[j],&pu8_Data[Location],temp_8);
			AttributeValue[temp_8+j] = '\0';
			
			ParsedLen = 2 + temp_8;
			break;
		}
		default:// we will assume the rest of the payload is the full value
		{	
			if(pu8_Data_Length - Location < CMBS_HAN_MAX_MSG_LEN)
			{
				memcpy(AttributeValue,&pu8_Data[Location],pu8_Data_Length- Location);
				ParsedLen = pu8_Data_Length - Location;
				AttributeValue[ParsedLen] = '\0';
			}
		}

	}// switch
	return ParsedLen;
}



void app_msg_parser_Parse_Interface_AttrRep_Res(ST_IE_HAN_MSG* stIe_Msg)
{
	HAN_APP_INFO_PRINT("=== ATTRIBUTE REPORT INTERFACE RESPONCE ===\n");
	switch(stIe_Msg->u8_InterfaceMember)
	{
		case CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_CREATE_PERIODIC_REP:
			HAN_APP_INFO_PRINT("\tOperation : Create Periodic Report\n");
			break;
		case CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_CREATE_EVENT_REP:
			HAN_APP_INFO_PRINT("\tOperation : Create Event Report\n");
			break;
		case CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_DELETE_REP:
			HAN_APP_INFO_PRINT("\tOperation : Delete Report\n");
			break;
		case CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_ADD_ENTRY_PERIODIC_REP:
			HAN_APP_INFO_PRINT("\tOperation : Add Entry to Periodic Report\n");
			break;
	case CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_ADD_ENTRY_EVENT_REP:
			HAN_APP_INFO_PRINT("\tOperation : Add Entry to Event Report\n");
			break;
	case CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_GET_PERIODIC_REP_ENTRIES:
			HAN_APP_INFO_PRINT("\tOperation : Get Periodic Report Entries\n");
			break;
	case CMBS_HAN_ATTR_REPORT_C2S_CMD_ID_GET_EVENT_REP_ENTRIES:
			HAN_APP_INFO_PRINT("\tOperation : Get Event Report Entries\n");
			break;

		default:
			HAN_APP_ERROR_PRINT("\tWrong Operation Response ( u8_InterfaceMember=%d ) field \n",stIe_Msg->u8_InterfaceMember);
			break;
	}

	if (stIe_Msg->u16_DataLen > 1)
	{
	    switch (stIe_Msg->pu8_Data[0]) 
	    {
		    case CMBS_HAN_ATTR_REPORT_RESP_VAL_OK:
			    HAN_APP_INFO_PRINT("\tResponse Value : OK\n");
			    HAN_APP_INFO_PRINT ("\t** REPORT ID : 0x%x **\n", stIe_Msg->pu8_Data[1]);
			    break;
		    case CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_NOT_AUTHORIZED:
			    HAN_APP_WARNING_PRINT("\tResponce Value : Fail -- Not Authorized \n");
			    break;
		    case CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_INV_ARG:
			    HAN_APP_WARNING_PRINT("\tResponce Value : Fail -- Invalid Argument\n");
			    break;
		    case CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_NOT_ENOUGH_RES:
			    HAN_APP_WARNING_PRINT("\tResponse Value : Fail -- Not Enough Resources\n");
			    break;
		    case CMBS_HAN_ATTR_REPORT_RESP_VAL_FAIL_UNKNOWN:
			    HAN_APP_WARNING_PRINT("\tResponse Value : Fail -- Unknown\n");
			    break;
		    default:
			    HAN_APP_ERROR_PRINT("\tResponce Value : Error in field\n");
			    break;
	    }
	}
}

void app_msg_parser_Parse_DevInfo_Res(ST_IE_HAN_MSG* stIe_Msg)
{
	
	if ( 	(stIe_Msg->e_MsgType == CMBS_HAN_MSG_TYPE_ATTR_SET_RES ) || 
			(stIe_Msg->e_MsgType == CMBS_HAN_MSG_TYPE_ATTR_GET_RES ) || 
			(stIe_Msg->e_MsgType == CMBS_HAN_MSG_TYPE_ATTR_GET_PACK_RES ) ||
			(stIe_Msg->e_MsgType == CMBS_HAN_MSG_TYPE_ATTR_SET_PACK_RES ) )
	{
				
		switch(stIe_Msg->e_MsgType)
		{
			case CMBS_HAN_MSG_TYPE_ATTR_SET_RES:
				HAN_APP_INFO_PRINT("=== DEVICE INTERFACE ATTRIBUTE SET RESPONSE (\n");
			break;
			case CMBS_HAN_MSG_TYPE_ATTR_GET_RES:
				HAN_APP_INFO_PRINT("=== DEVICE INTERFACE ATTRIBUTE GET RESPONSE (\n");
				app_msg_parser_Parse_Interface_DeviceInfo_AttrGetRes(stIe_Msg);
			break;
			case CMBS_HAN_MSG_TYPE_ATTR_GET_PACK_RES:
				HAN_APP_INFO_PRINT("=== DEVICE INTERFACE ATTRIBUTE GET PACK RESPONSE (\n");
				app_msg_parser_Parse_Interface_DeviceInfo_PackGetRes(stIe_Msg);
			break;
			case CMBS_HAN_MSG_TYPE_ATTR_SET_PACK_RES:
				HAN_APP_INFO_PRINT("=== DEVICE INTERFACE ATTRIBUTE SET PACK RESPONSE (\n");
			break;
			default:
				HAN_APP_WARNING_PRINT("=== DEVICE INTERFACE ERROR IN RES TYPE FIELD ( e_MsgType )\n");
			break;
		}


	}
	
}

void app_HanFunRegisterMsgResSend(ST_IE_HAN_MSG * stIe_RegMsg)
{

	ST_IE_HAN_MSG			stIe_ResMsg;
	ST_IE_HAN_MSG_CTL 		st_HANResMsgCtl;
	u8				u8_Buffer[CMBS_HAN_MAX_MSG_LEN];

	stIe_ResMsg.pu8_Data = u8_Buffer;
	app_fun_BuildFUNRegistrationResponse((u8 *) stIe_RegMsg, (u8 *) &stIe_ResMsg, (u8 *) &st_HANResMsgCtl);

	HAN_APP_INFO_PRINT("\n\nSending FUN Registration Message Response\n\n");

	//print message summary
	/*printf("\n--------------- MESSAGE SUMMARY -------------------\n");
	printf("Source address      : (Unit type = %d, D'%dU'%d)\n", 
		stIe_ResMsg.u8_DstAddressType,
		stIe_ResMsg.u16_SrcDeviceId,
		stIe_ResMsg.u8_SrcUnitId);

	printf("Destination address : (Unit type = %d, D'%dU'%d)\n", 
		stIe_ResMsg.u8_DstAddressType,
		stIe_ResMsg.u16_DstDeviceId,
		stIe_ResMsg.u8_DstUnitId);

	printf("Transport : %d\n",stIe_ResMsg.st_MsgTransport.u16_Reserved);
	printf("Message Sequence : %d\n",stIe_ResMsg.u8_MsgSequence);
	printf("Message Type : %d \n", stIe_ResMsg.e_MsgType);
	printf("Interface Type : %d, Interface ID : %d, Interface Member : %d \n",stIe_ResMsg.u8_InterfaceType,stIe_ResMsg.u16_InterfaceId,stIe_ResMsg.u8_InterfaceMember);
	printf("Payload : %d bytes \n",stIe_ResMsg.u16_DataLen);
	for (u8_Index=0;u8_Index<stIe_ResMsg.u16_DataLen;u8_Index++)
	{
		printf(" %d",stIe_ResMsg.pu8_Data[u8_Index]);
	}

	printf("\n\n"); */
	
	app_DsrHanMsgSend(CMBS_HAN_FUN_REGISTRATION_REQ_ID, stIe_ResMsg.u16_DstDeviceId, &st_HANResMsgCtl,&stIe_ResMsg);	

}

void app_HanOnFunRegisterMsgReceived(ST_IE_HAN_MSG * stIe_Msg)
{
	
	ST_FUN_DEVICE_INFO		 	st_FunInfoEepromStructure;
	
	memset(&st_FunInfoEepromStructure, 0, sizeof(ST_FUN_DEVICE_INFO));
	app_fun_ParseFUNRegistrationMessage((u8 *) stIe_Msg, &st_FunInfoEepromStructure);

	// TODO: handle race between msg sent and event sent
	// Send response to device 
	app_HanFunRegisterMsgResSend(stIe_Msg );

	// Send CMBS_EV_DSR_HAN_FUN_REG_MSG_RECV_RES  event to target
	app_DsrHanFunRegResMsgSend(&st_FunInfoEepromStructure);
	
		
}

void app_HanOnFunDeviceMgmtMsgReceived(ST_IE_HAN_MSG * stIe_Msg)
{

	switch (stIe_Msg->u8_InterfaceMember)
	{
		case FUN_IF_DEV_MNGT_CMD_REGISTER_DEVICE:
			app_HanOnFunRegisterMsgReceived(stIe_Msg);
			break;

		case FUN_IF_DEV_MNGT_CMD_DEREGISTER_DEVICE:
			HAN_APP_INFO_PRINT("GOT FUN_IF_DEV_MNG_CMD_REGISTER_DEVICE - not supported\n\n");
			break;

		case FUN_IF_DEV_MNGT_CMD_START_SESS_READ_REG_INFO:
			HAN_APP_INFO_PRINT("GOT FUN_IF_DEV_MNG_CMD_START_SESS_READ_REG_INFO - not supported\n\n");
			break;

		case FUN_IF_DEV_MNGT_CMD_END_SESS_READ_REG_INFO:
			HAN_APP_INFO_PRINT("GOT FUN_IF_DEV_MNG_CMD_END_SESS_READ_REG_INFO - not supported\n\n");
			break;

		case FUN_IF_DEV_MNGT_CMD_GET_ENTRIES:
			HAN_APP_INFO_PRINT("GOT FUN_IF_DEV_MNG_CMD_GET_ENTRIES - not supported\n\n");
			break;

		default:
			HAN_APP_ERROR_PRINT("Command Not Supported!\n\n");
			break;
	}
			
}


void app_HanOnFunSuotaUpgradeCompleteReceived(ST_IE_HAN_MSG * stIe_Msg )
{

	u8 u8_UpgradeStatus;
	u8 u8_length;
	u8 u8_index = 0;

	cfr_ie_dser_u8(&stIe_Msg->pu8_Data[u8_index], &u8_UpgradeStatus);
	u8_index++;
	
	HAN_APP_INFO_PRINT("\n****************************************\n");
	if (u8_UpgradeStatus == CMBS_HAN_SUOTA_CTS_CMD_ID_UPGRADE_COMPLETE_RESULT_OK)
		{
			HAN_APP_INFO_PRINT("SUOTA upgrade finished successfully!  \nNew SW version: ");
			
			// First element (size)
			cfr_ie_dser_u8(&stIe_Msg->pu8_Data[u8_index], &u8_length);
			u8_index++;

			// Now read the data
			HAN_APP_INFO_PRINT("%.*s",u8_length, &stIe_Msg->pu8_Data[u8_index]);			
			u8_index+=u8_length;
	
		}
	else
		{
			HAN_APP_ERROR_PRINT("SUOTA upgrade Failure (0x%x)",u8_UpgradeStatus);

		}

	HAN_APP_INFO_PRINT("\n****************************************\n\n");

}

void app_HanOnFunSuotaMsgReceived(ST_IE_HAN_MSG * stIe_Msg )
{

	switch (stIe_Msg->u8_InterfaceMember)
	{
		case CMBS_HAN_SUOTA_CTS_CMD_ID_UPGRADE_COMPLETE:
			app_HanOnFunSuotaUpgradeCompleteReceived(stIe_Msg);
			break;

		default:
			HAN_APP_WARNING_PRINT("Command Not Supported!\n\n");
			break;
	}
			
}


void app_ULEFileStoreSquenceNumber( void )
{
	E_CMBS_RC e_status;
	FILE *f_binFile = 0;
	u8 pu8_filename_SeqNumbers[] = "SeqNumbers.bin"; 
	
	//Check binary files can be created
	e_status = tcx_fileOpen(&f_binFile, pu8_filename_SeqNumbers, (u8 *)"w+b");

	if ( e_status != CMBS_RC_OK )
	{
		printf("\nError: Could not create file: %s", pu8_filename_SeqNumbers);
		return;
	}

	tcx_fileWrite(f_binFile, (u8*)G_st_ULESeqNum, 0, sizeof(G_st_ULESeqNum));
    fflush(f_binFile);
    tcx_fileClose(f_binFile);
}

void app_ULESetRxSequenceNumber(u16 u16_deviceNumber, u16 u16_RxSequenceNumber)
{   
	if(MAX_ULE_DEVICES_HOST>u16_deviceNumber)
	{
    	G_st_ULESeqNum[u16_deviceNumber].rx = u16_RxSequenceNumber;
		app_ULEFileStoreSquenceNumber();
	}
}

void app_ULESetTxSequenceNumber(u16 u16_deviceNumber, u16 u16_TxSequenceNumber)
{   
	if(MAX_ULE_DEVICES_HOST>u16_deviceNumber)
	{
    	G_st_ULESeqNum[u16_deviceNumber].tx = u16_TxSequenceNumber;
		app_ULEFileStoreSquenceNumber();
		
	}
}

void app_ULEGetRxSequenceNumber(u16 u16_deviceNumber, u16 * pu16_RxSequenceNumber)
{   
	if(MAX_ULE_DEVICES_HOST>u16_deviceNumber)
	{
    	*pu16_RxSequenceNumber = G_st_ULESeqNum[u16_deviceNumber].rx ;
		
	}
}

void app_ULEGetTxSequenceNumber(u16 u16_deviceNumber, u16 * pu16_TxSequenceNumber)
{   
	if(MAX_ULE_DEVICES_HOST>u16_deviceNumber)
	{
    	*pu16_TxSequenceNumber = G_st_ULESeqNum[u16_deviceNumber].tx ;
	}
}


u8  app_HAN_GetHANConfiguration( void )
{
	return stHanCfg.u8_HANServiceConfig;
}

void app_HAN_SetHANConfiguration(u8 u8_HANServiceConfig)
{
	stHanCfg.u8_HANServiceConfig = u8_HANServiceConfig;
}



void app_HanOnFunMsgReceived(void *pv_EventData, ST_IE_HAN_MSG *pstIe_Msg)
{
    ST_IE_FULL_HAN_MSG st_full_msg;
    void *pv_IE = NULL;
    u16 u16_IE, u16_seqNum=0;
    bool b_IEReceived = FALSE , b_recSeqNum = FALSE;

    pstIe_Msg->pu8_Data = u8_Buffer;
    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_HAN_MSG )
        {
            cmbs_api_ie_HANMsgGet(pv_IE, pstIe_Msg);
            b_IEReceived = TRUE;
        }
		
		if ( u16_IE == CMBS_IE_HAN_RX_SEQUENCE_NUMBER )
		{
			cmbs_api_ie_HANRxSequenceNumberGet(pv_IE,&u16_seqNum);
			b_recSeqNum = TRUE;
		}
		
        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);
    }

    if (!b_IEReceived)
    {
        HAN_APP_ERROR_PRINT("CMBS_IE_HAN_MSG not received !");
        return;
    }

 	switch( pstIe_Msg->u16_InterfaceId)
	{
		case ATTRIBUTE_REPORTING_INTERFACE_ID:
			if ( pstIe_Msg->u8_InterfaceType == 0 ) // client
			{
				app_msg_parser_Parse_Interface_AttrRep_Report(pstIe_Msg);
			}
			else
			{
				app_msg_parser_Parse_Interface_AttrRep_Res(pstIe_Msg);
			}
			break;
				
		case DEVICE_INFORMATION_INTERFACE_ID:
			app_msg_parser_Parse_DevInfo_Res(pstIe_Msg);
			break;
				
		case DEVICE_MGMT_INTERFACE_ID:
			app_HanOnFunDeviceMgmtMsgReceived(pstIe_Msg);
			break;

		case CMBS_HAN_SUOTA_INTERFACE_ID:
			app_HanOnFunSuotaMsgReceived(pstIe_Msg);
			break;
				
		default:
			break;
		}
		
		
	if( (pstIe_Msg->u16_InterfaceId == HAN_IFACE_MEMORY_ACCESS) || (pstIe_Msg->u16_InterfaceId == HAN_IFACE_TESTMODE) || (pstIe_Msg->u16_InterfaceId == CMBS_HAN_SUOTA_1_2_INTERFACE_ID))
	{
		memcpy(&st_full_msg.st_HANmsg, pstIe_Msg, sizeof(st_full_msg.st_HANmsg) );
		if(pstIe_Msg->u16_DataLen > sizeof(st_full_msg.paylod))
		{
			HAN_APP_ERROR_PRINT("Not enough memory space!\n\n");
			return;
		}
		memcpy(&st_full_msg.paylod,pstIe_Msg->pu8_Data, pstIe_Msg->u16_DataLen);
		st_full_msg.st_HANmsg.pu8_Data = st_full_msg.paylod;

		if ( g_cmbsappl.n_Token )
        {
			appcmbs_ObjectSignal((char*)&st_full_msg, sizeof(st_full_msg),0,CMBS_EV_DSR_HAN_MSG_RECV);
		}
	}

	if(CMBS_HAN_SSN_HOST_STORE_MODE(app_HAN_GetHANConfiguration()))
	{
		if(b_recSeqNum)
		{
			app_ULESetRxSequenceNumber(pstIe_Msg->u16_SrcDeviceId,u16_seqNum);
		}
	}
	
}

// Received the Raw Msg host side, can now parse the message according to protocol
void app_HanOnRawMsgReceived(void *pv_EventData, ST_IE_DATA * pstIe_RawMsg)
{
    void *pv_IE = NULL;
    u16 u16_IE, u16_deviceId=0;
    bool b_IEReceived = FALSE;

    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_DATA )
        {
        	cmbs_api_ie_DataGet(pv_IE, pstIe_RawMsg);
            b_IEReceived = TRUE;
        }
		else if ( u16_IE == CMBS_IE_HAN_DEVICE )
        {
            cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
        }
        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);
    }

    if (!b_IEReceived)
    {
        HAN_APP_ERROR_PRINT("CMBS_IE_DATA not received !");
        return;
    }

	// To Do: Parse the protocol (FUN/6LowPAN/etc) and the message
	
}



void app_HanOnGetConnectionStatusRes(void* pv_EventData)
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	ST_IE_RESPONSE 	st_Resp;
	ST_CMBS_IE_RES connectionStatusGet = {0,0,0};
	
	memset(&st_Resp, 0, sizeof(ST_IE_RESPONSE));
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );

         while ( pv_IE )
	    {
	        switch ( u16_IE )
	        {
	        case  CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet( pv_IE, &st_Resp);								
	            	break;
	        case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
	            	break;
			case CMBS_IE_HAN_DEVICE_CONNECTION_STATUS:
				cmbs_api_ie_HANConnectionStatusGet(pv_IE, &connectionStatusGet.u16_status);
			break;	
	        }		
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	connectionStatusGet.b_Response = st_Resp.e_Response;
	connectionStatusGet.u16_deviceID = u16_deviceId;

	 if ( g_cmbsappl.n_Token )
     {
		appcmbs_ObjectSignal((char*)&connectionStatusGet, sizeof(connectionStatusGet),0,CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS_RES);	 
	 }
}

void app_HanOnRegStage3Notification(void* pv_EventData)
{
	void *	pv_IE = NULL;
	ST_IE_RESPONSE	st_Resp;
	u16 u16_IE, u16_deviceId=0;
	ST_CMBS_IE_RES registrationStatus;
	ST_HAN_GENERAL_STATUS st_Status;

	memset(&st_Resp, 0, sizeof(ST_IE_RESPONSE));
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );

	while ( pv_IE )
	{
		switch ( u16_IE )
		{
			case CMBS_IE_HAN_GENERAL_STATUS:
				cmbs_api_ie_HANGeneralStatusGet( pv_IE, &st_Status);
				break;
			case CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
				break;
		}		
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	registrationStatus.u16_deviceID = u16_deviceId;

	if (st_Status.u16_Status == CMBS_HAN_GENERAL_STATUS_OK)
	{
		registrationStatus.b_Response = CMBS_RESPONSE_OK;
	}
	else
	{
		registrationStatus.b_Response = CMBS_RESPONSE_ERROR;
	}

	if ( g_cmbsappl.n_Token )
    {
		appcmbs_ObjectSignal((char*)&registrationStatus, sizeof(registrationStatus),0,CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_3_NOTIFICATION);	 
	}
}

void app_HanOnForcefulDeleteRes(void* pv_EventData)
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	ST_IE_RESPONSE 	st_Resp = {0};
	u16				u16_Reason = 0;
	ST_CMBS_IE_RES forcefulDeleteStatus;
	
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
			    break;
			case  CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Resp);
				break;
			case CMBS_IE_HAN_DEVICE_FORCEFUL_DELETE_ERROR_STATUS_REASON:
				cmbs_api_ie_HANForcefulDeRegErrorReasonGet(pv_IE, &u16_Reason);

		}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	forcefulDeleteStatus.u16_deviceID = u16_deviceId;
	forcefulDeleteStatus.b_Response = st_Resp.e_Response;
	forcefulDeleteStatus.u16_status= u16_Reason;
	
	if ( g_cmbsappl.n_Token )
	{
		appcmbs_ObjectSignal((char*)&forcefulDeleteStatus, sizeof(forcefulDeleteStatus),0,CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE_RES);	
	}
}





void app_HanOnDsrGeneralLogEvent 	(void* pv_EventData)
{
	void *	pv_IE = NULL;
	u16 u16_IE;
	ST_IE_DATA		stData = {0,0};
	
	u8 u8_BufferData[CMBS_PARAM_SUBS_DATA_LENGTH]={0};

	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	cmbs_api_ie_DataGet(pv_IE,&stData);

	// check necessary conditions for continue processing
	if ((stData.pu8_Data == NULL) ||(stData.u16_DataLen > CMBS_PARAM_SUBS_DATA_LENGTH))
		return;
	
	//Copy our string
	memcpy(u8_BufferData, stData.pu8_Data, stData.u16_DataLen);	

	//Print to STD output	
	puts((char*)u8_BufferData);
}






#ifdef WIN32
DWORD app_tempThread(LPVOID lpThreadParameter)
{
	UNUSED_PARAMETER(lpThreadParameter);
	tcx_getch();
	appcmbs_ObjectSignal(NULL, 0, 0xFFFF,CMBS_EV_DSR_HAN_MSG_RECV);
	cfr_ie_DeregisterThread((u32)GetCurrentThreadId()); 
	return 0;
}
#elif __linux__
void* app_tempThread(void *ptr)
{
    UNUSED_PARAMETER(ptr);
    tcx_getch();
    appcmbs_ObjectSignal(NULL, 0, 0xFFFF,CMBS_EV_DSR_HAN_MSG_RECV);
    cfr_ie_DeregisterThread((u32)pthread_self());
    return 0;
}
#endif

void app_HanPrintMessageFields(ST_IE_HAN_MSG* pMsg)
{

	HAN_APP_INFO_PRINT("Source address      : (Unit type = %d, D'%dU'%d)\n", 
		pMsg->u8_DstAddressType,
		pMsg->u16_SrcDeviceId,
		pMsg->u8_SrcUnitId);

	HAN_APP_INFO_PRINT("Destination address : (Unit type = %d, D'%dU'%d)\n", 
		pMsg->u8_DstAddressType,
		pMsg->u16_DstDeviceId,
		pMsg->u8_DstUnitId);

	HAN_APP_INFO_PRINT("Transport : %d\n",pMsg->st_MsgTransport.u16_Reserved);
	HAN_APP_INFO_PRINT("Message Sequence : %d\n",pMsg->u8_MsgSequence);
	HAN_APP_INFO_PRINT("Message Type : %d \n", pMsg->e_MsgType);
	HAN_APP_INFO_PRINT("Interface Type : %d, Interface ID : %d, Interface Member : %d \n",pMsg->u8_InterfaceType,pMsg->u16_InterfaceId,pMsg->u8_InterfaceMember);
	HAN_APP_INFO_PRINT("Payload : %d bytes \n",pMsg->u16_DataLen);
	HAN_APP_PRINT_DATA(pMsg->pu8_Data, pMsg->u16_DataLen);
	
}

void app_HanPrintRawMessageFields(ST_IE_DATA* pMsg, u16 u16_DeviceId)
{
	HAN_APP_INFO_PRINT("Destination Device Id : %d\n", u16_DeviceId);
	HAN_APP_INFO_PRINT("Payload : %d bytes \n",pMsg->u16_DataLen);
	HAN_APP_PRINT_DATA(pMsg->pu8_Data, pMsg->u16_DataLen);
}

void app_HanRegularStart( u8 RegisterForHanMessages )
{
	ST_HAN_MSG_REG_INFO     pst_HANMsgRegInfo;
	ST_APPCMBS_CONTAINER	st_Container;
	ST_HAN_CONFIG			stHanCfg;
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	
	
	
	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.

	if (RegisterForHanMessages)
	{
		app_HAN_SetHANConfiguration (0);
	}


	stHanCfg.u8_HANServiceConfig = app_HAN_GetHANConfiguration(); 
	
	app_DsrHanMngrInit(&stHanCfg);

	appcmbs_WaitForContainer (CMBS_EV_DSR_HAN_MNGR_INIT_RES, &st_Container ); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)

	if ((st_Container.n_InfoLen != 1) || (st_Container.ch_Info[0] != 0))
	{
		HAN_APP_ERROR_PRINT("\n--------------------------------------------");
		HAN_APP_ERROR_PRINT("\n Error during HAN application initialization");
		HAN_APP_ERROR_PRINT("\n--------------------------------------------");
		return;
	}
	else
	{
		HAN_APP_INFO_PRINT("--------------------------------------------\n");
		HAN_APP_INFO_PRINT(" Regular startup procedure performed successfully\n");
		HAN_APP_INFO_PRINT("--------------------------------------------\n");
	}
	
	memset(&st_Container,0x0,sizeof(ST_APPCMBS_CONTAINER));

	if( RegisterForHanMessages == TRUE )
	{
		// Register for Han Messages
	    pst_HANMsgRegInfo.u16_InterfaceId = 0xFFFF;
	    pst_HANMsgRegInfo.u8_UnitId = 2;
	    cmbs_dsr_han_msg_RecvRegister(p_List, &pst_HANMsgRegInfo);
	}
	
}

// Full HAN init without waiting for container
void app_HanStartAndConfig( void )
{
    ST_HAN_MSG_REG_INFO     pst_HANMsgRegInfo;
    PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	ST_HAN_CONFIG			stHanCfg;

	stHanCfg.u8_HANServiceConfig = app_HAN_GetHANConfiguration(); 
	app_DsrHanMngrInit(&stHanCfg);

	SleepMs(80);

	// Register for Han Messages
    pst_HANMsgRegInfo.u16_InterfaceId = 0xFFFF;
    pst_HANMsgRegInfo.u8_UnitId = 2;
    cmbs_dsr_han_msg_RecvRegister(p_List, &pst_HANMsgRegInfo);


}


void app_DsrHanVoiceCallReq (u16 u16_DstDeviceId, u8 u8_DstUnitId, PST_HAN_VOICE_CALL_REQ_PARAMS Pst_CallReqParams)
{
	u8				pu8_Buffer[CMBS_HAN_MAX_MSG_LEN];
	u8 u8_Index = 0;
		
	ST_IE_HAN_MSG	stIe_Msg;
	ST_IE_HAN_MSG_CTL st_HANMsgCtl = { 0,0,0 };
	
	st_HANMsgCtl.ImmediateSend = TRUE;
	st_HANMsgCtl.IsLast = FALSE;

	// Source Address
	stIe_Msg.u16_SrcDeviceId = 0;
	stIe_Msg.u8_SrcUnitId = 2; 

	// Destiantion address
	stIe_Msg.u16_DstDeviceId = u16_DstDeviceId;
	stIe_Msg.u8_DstAddressType = 0;// individual address
	stIe_Msg.u8_DstUnitId = u8_DstUnitId;

	// transport - not used
	stIe_Msg.st_MsgTransport.u16_Reserved = 0;// transport layer not used

	// application layer
	stIe_Msg.u8_MsgSequence = 0x55; 
	stIe_Msg.e_MsgType = CMBS_HAN_MSG_TYPE_CMD;
	stIe_Msg.u8_InterfaceType = HAN_IFACE_DIRECTION_SERVER;    // 1=server, 0=client
	stIe_Msg.u16_InterfaceId = CMBS_HAN_FUN_INTERFACE_AUDIO;		// interface ID
	stIe_Msg.u8_InterfaceMember = CMBS_HAN_FUN_IF_AUDIO_CMD_MAKE_CALL;	// the command to send

	// initialize to 0
	stIe_Msg.u16_DataLen = 0;

	//check wether information regarding preferred codec exists
	if (Pst_CallReqParams->u32_FieldMask & CMBS_HAN_PREFFERED_CODEC_MASK)
		{
			pu8_Buffer[u8_Index++] = CMBS_HAN_PREFERRED_CODEC_FIELD_ID;
			pu8_Buffer[u8_Index++] = Pst_CallReqParams->u8_PreferredCodec;
		}

	//check wether information regarding digits exists
	if (Pst_CallReqParams->u32_FieldMask & CMBS_HAN_DIGITS_MASK)
		{
			u8 u8_DigitsLen;
			
			pu8_Buffer[u8_Index++] = CMBS_HAN_DIGITS_FIELD_ID;

			// length is stored in the first byte
			u8_DigitsLen = Pst_CallReqParams->pu8_Digits[0];
			pu8_Buffer[u8_Index++] = u8_DigitsLen;

			memcpy((void *) &pu8_Buffer[u8_Index], (void *) &Pst_CallReqParams->pu8_Digits[1], u8_DigitsLen);

			u8_Index += u8_DigitsLen;
		}

	//check wether information regarding other party type exists
	if (Pst_CallReqParams->u32_FieldMask & CMBS_HAN_OTHER_PARTY_TYPE_MASK)
		{
			pu8_Buffer[u8_Index++] = CMBS_HAN_OTHER_PARTY_TYPE_FIELD_ID;
			pu8_Buffer[u8_Index++] = Pst_CallReqParams->u8_OtherPartyType;
		}


	//check wether information regarding other party name exists
	if (Pst_CallReqParams->u32_FieldMask & CMBS_HAN_OTHER_PARTY_NAME_MASK)
		{
			u8 u8_OtherPartyNameLen;
			
			pu8_Buffer[u8_Index++] = CMBS_HAN_OTHER_PARTY_NAME_FIELD_ID;

			// length is stored in the first byte
			u8_OtherPartyNameLen = Pst_CallReqParams->pu8_OtherPartyName[0];
			pu8_Buffer[u8_Index++] = u8_OtherPartyNameLen;

			memcpy((void *) &pu8_Buffer[u8_Index], (void *) &Pst_CallReqParams->pu8_OtherPartyName[1], u8_OtherPartyNameLen);

			u8_Index += u8_OtherPartyNameLen;
		}

	//check wether information regarding other party id exists
	if (Pst_CallReqParams->u32_FieldMask & CMBS_HAN_OTHER_PARTY_ID_MASK)
		{
			u8 u8_OtherPartyIdLen;
			
			pu8_Buffer[u8_Index++] = CMBS_HAN_OTHER_PARTY_ID_FIELD_ID;
			// length is stored in the first byte
			u8_OtherPartyIdLen = Pst_CallReqParams->pu8_OtherPartyId[0];
			pu8_Buffer[u8_Index++] = u8_OtherPartyIdLen;

			memcpy((void *) &pu8_Buffer[u8_Index], (void *) &Pst_CallReqParams->pu8_OtherPartyId[1], u8_OtherPartyIdLen);

			u8_Index += u8_OtherPartyIdLen;
		}

	//check wether information regarding call type exists
	if (Pst_CallReqParams->u32_FieldMask & CMBS_HAN_CALL_TYPE_MASK)
		{
			pu8_Buffer[u8_Index++] = CMBS_HAN_CALL_TYPE_FIELD_ID;
			pu8_Buffer[u8_Index++] = Pst_CallReqParams->u8_CallType;
		}
	

	stIe_Msg.u16_DataLen = u8_Index;
	stIe_Msg.pu8_Data = pu8_Buffer;	

	
	app_DsrHanMsgSend(1, stIe_Msg.u16_DstDeviceId, &st_HANMsgCtl, &stIe_Msg);
}

void app_HanOnUpdateDevicePMSeqNumberRes( void )
{	
	_cmbs_int_StartupBlockSignal( &g_CMBSInstance );
}

void app_HanOnPVCResetIndication(void* pv_EventData)
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	
	while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
			    break;
		}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	if(CMBS_HAN_SSN_HOST_STORE_MODE(app_HAN_GetHANConfiguration()))
	{
		app_ULESetRxSequenceNumber(u16_deviceId,0);
		app_ULESetTxSequenceNumber(u16_deviceId,0);
	}
	
	if ( g_cmbsappl.n_Token )
			{
				appcmbs_ObjectSignal( NULL, 0, 1, CMBS_EV_DSR_HAN_PVC_RESET_IND );
			}

}

void app_HanOnFUNMsgSendRes(void* pv_EventData)
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0,u16_seqNum=0;
	bool 	b_recSeqNum = FALSE;
	
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	
	while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
			    break;
			case CMBS_IE_HAN_TX_SEQUENCE_NUMBER:
				cmbs_api_ie_HANTxSequenceNumberGet(pv_IE,&u16_seqNum);
				b_recSeqNum = TRUE;
				break;
		}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	if(CMBS_HAN_SSN_HOST_STORE_MODE(app_HAN_GetHANConfiguration()))
	{
		if(b_recSeqNum)
		{
			app_ULESetTxSequenceNumber(u16_deviceId,u16_seqNum);
		}
	}
	
	if ( g_cmbsappl.n_Token )
			{
				appcmbs_ObjectSignal( NULL, 0, 1, CMBS_EV_DSR_HAN_MSG_SEND_RES );
			}

}


	void app_HANRestoreSequenceNumbersThread (void)
	{

		int i;
		u16 rx_Seqnum = 0, tx_Seqnum = 0;

		E_CMBS_RC e_status;
		FILE *f_binFile = 0;
		u8 pu8_filename_SeqNumbers[] = "SeqNumbers.bin"; 

		e_status = tcx_fileOpen(&f_binFile, pu8_filename_SeqNumbers, (u8 *)"r");
		if ( e_status != CMBS_RC_OK )
	    {
	       	printf("\nError: Could not Open file: %s", pu8_filename_SeqNumbers);
			return ;
	    }

		e_status = tcx_fileRead(f_binFile, (u8 *)G_st_ULESeqNum, 0, sizeof(G_st_ULESeqNum));
		
		
		for (i=1;i<MAX_ULE_DEVICES_HOST;i++)
		{
			app_ULEGetRxSequenceNumber(i,&rx_Seqnum);
			app_ULEGetTxSequenceNumber(i,&tx_Seqnum);

			if (rx_Seqnum || tx_Seqnum)
			{
				// build event and send 
				cmbs_dsr_han_UpdateDevicePMSeqNumbers(g_cmbsappl.pv_CMBSRef, i, rx_Seqnum, tx_Seqnum);

				//wait for response before sending next evenvt
				cmbs_int_WaitForResponse(1000);

			}


		}
		

	tcx_fileClose(f_binFile);
		
	}

void app_HANRestoreSequenceNumbers (void)
{
	if(CMBS_HAN_SSN_HOST_STORE_MODE(app_HAN_GetHANConfiguration()))
	{
#ifdef WIN32
		app_CreateThread((LPTHREAD_START_ROUTINE)app_HANRestoreSequenceNumbersThread);
#elif __linux__
		app_CreateThread((start_routine)app_HANRestoreSequenceNumbersThread);
#endif
	}
}




