/*!
*	\file       cmbs_han.c
*	\brief
*	\Author		CMBS Team
*
*
*******************************************************************************/
#if defined(__arm)
    #include "tclib.h"
    #include "embedded.h"
#else
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
#endif

#include "cmbs_int.h"
#include "cmbs_han_ie.h"

E_CMBS_RC cmbs_dsr_han_mngr_Init(void *pv_AppRefHandle, ST_HAN_CONFIG * pst_HANConfig)
{
	PST_CFR_IE_LIST    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	ST_IE_HAN_CONFIG   st_HanCfgIe;

	UNUSED_PARAMETER(pv_AppRefHandle);

	st_HanCfgIe.st_HanCfg = *pst_HANConfig;
	cmbs_api_ie_HanCfgAdd(p_List,&st_HanCfgIe);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MNGR_INIT,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_mngr_Start (void *pv_AppRefHandle)
{
	UNUSED_PARAMETER(pv_AppRefHandle);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MNGR_START,NULL, 0);  
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8 IsBrief )
{
	PST_CFR_IE_LIST    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_NumOfEntries,		CMBS_IE_HAN_NUM_OF_ENTRIES);
	cmbs_api_ie_ShortValueAdd(p_List,u16_IndexOfFirstEntry,	CMBS_IE_HAN_INDEX_1ST_ENTRY);
	cmbs_api_ie_ByteValueAdd(p_List, IsBrief ? CMBS_HAN_DEVICE_TABLE_ENTRY_TYPE_BRIEF : CMBS_HAN_DEVICE_TABLE_ENTRY_TYPE_EXTENDED,	CMBS_IE_HAN_TABLE_ENTRY_TYPE);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_READ_TABLE,p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_ReadExtendedDeviceTablePhase2 (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	PST_CFR_IE_LIST    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_NumOfEntries,		CMBS_IE_HAN_NUM_OF_ENTRIES);
	cmbs_api_ie_ShortValueAdd(p_List,u16_IndexOfFirstEntry,	CMBS_IE_HAN_INDEX_1ST_ENTRY);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_READ_EXTENDED_TABLE_PHASE_2, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_ReadBlackListedDeviceTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	PST_CFR_IE_LIST    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_NumOfEntries,		CMBS_IE_HAN_NUM_OF_ENTRIES);
	cmbs_api_ie_ShortValueAdd(p_List,u16_IndexOfFirstEntry,	CMBS_IE_HAN_INDEX_1ST_ENTRY);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_READ_BLACK_LIST_DEVICE_TABLE, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_DEVICE_ENTRY * pst_HANDeviceEntriesArray)
{
	PST_CFR_IE_LIST						p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	UNUSED_PARAMETER(u16_NumOfEntries);
	UNUSED_PARAMETER(pv_AppRefHandle);
	UNUSED_PARAMETER(u16_IndexOfFirstEntry);
	UNUSED_PARAMETER(pst_HANDeviceEntriesArray);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_WRITE_TABLE,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_bind_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_NumOfEntries,		CMBS_IE_HAN_NUM_OF_ENTRIES);
	cmbs_api_ie_ShortValueAdd(p_List,u16_IndexOfFirstEntry,	CMBS_IE_HAN_INDEX_1ST_ENTRY);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_BIND_READ_TABLE,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_bind_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_BIND_ENTRY * pst_HANBindEntriesArray)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	ST_IE_HAN_BIND_ENTRIES	stIe_Binds;

	UNUSED_PARAMETER(pv_AppRefHandle);

	stIe_Binds.u16_NumOfEntries		= u16_NumOfEntries;
	stIe_Binds.u16_StartEntryIndex	= u16_IndexOfFirstEntry;
	stIe_Binds.pst_BindEntries		= pst_HANBindEntriesArray;
	cmbs_api_ie_HANBindTableAdd(p_List,&stIe_Binds);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_BIND_WRITE_TABLE,p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_bind_AddEntry (void *pv_AppRefHandle, PST_HAN_BIND_ENTRY pst_HANBindEntry)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANBindEntryAdd(p_List, pst_HANBindEntry);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_BIND_ADD_ENTRY, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_bind_RemoveEntry (void *pv_AppRefHandle, PST_HAN_BIND_ENTRY pst_HANBindEntry)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANBindEntryAdd(p_List, pst_HANBindEntry);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_BIND_REMOVE_ENTRY ,p_List->pu8_Buffer, p_List->u16_CurSize);  
}


//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_bind_ClearTable (void *pv_AppRefHandle)
{
	UNUSED_PARAMETER(pv_AppRefHandle);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_BIND_CLEAR_TABLE, NULL, 0);  
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_group_ReadTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u16 u16_GroupId)
{
	PST_CFR_IE_LIST    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANNumOfEntriesAdd(p_List, u16_NumOfEntries);
	cmbs_api_ie_HANIndex1stEntryAdd(p_List, u16_IndexOfFirstEntry);
	cmbs_api_ie_HANGroupIdAdd(p_List, u16_GroupId);
		
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GROUP_READ_TABLE, p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_group_ReadList (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	PST_CFR_IE_LIST    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_NumOfEntries,		CMBS_IE_HAN_NUM_OF_ENTRIES);
	cmbs_api_ie_ShortValueAdd(p_List,u16_IndexOfFirstEntry,	CMBS_IE_HAN_INDEX_1ST_ENTRY);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GROUP_READ_LIST, p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Broadcast_ReadChannelTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	PST_CFR_IE_LIST    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_NumOfEntries,		CMBS_IE_HAN_NUM_OF_ENTRIES);
	cmbs_api_ie_ShortValueAdd(p_List,u16_IndexOfFirstEntry,	CMBS_IE_HAN_INDEX_1ST_ENTRY);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_BROADCAST_READ_CHANNEL_TABLE, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_group_WriteTable (void *pv_AppRefHandle, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_GROUP_TABLE_ENTRY * pst_HANGroupEntriesArray)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	ST_IE_HAN_GROUP_ENTRIES	stIe_Groups;

	UNUSED_PARAMETER(pv_AppRefHandle);

	stIe_Groups.u16_NumOfEntries	= u16_NumOfEntries;
	stIe_Groups.u16_StartEntryIndex	= u16_IndexOfFirstEntry;
	stIe_Groups.pst_GroupEntries	= pst_HANGroupEntriesArray;
	cmbs_api_ie_HANGroupTableAdd(p_List,&stIe_Groups);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GROUP_WRITE_TABLE,p_List->pu8_Buffer, p_List->u16_CurSize);  

}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_group_CreateGroup  (void *pv_AppRefHandle, u8 u8_BroadcastChannelId)
{
	PST_CFR_IE_LIST				p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANBroadcastChannelIdAdd(p_List, u8_BroadcastChannelId);
		
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GROUP_CREATE_GROUP, p_List->pu8_Buffer, p_List->u16_CurSize );  

}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_group_RemoveGroup  (void *pv_AppRefHandle, u16 u16_GroupId )
{
	PST_CFR_IE_LIST				p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);
	
	cmbs_api_ie_HANGroupIdAdd(p_List, u16_GroupId);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GROUP_REMOVE_GROUP, p_List->pu8_Buffer, p_List->u16_CurSize );  

}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC  cmbs_dsr_han_group_AddDeviceToGroup  (void *pv_AppRefHandle, u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId )
{
	PST_CFR_IE_LIST				p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);
	
	cmbs_api_ie_HANGroupIdAdd(p_List, u16_GroupId);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceId);
	cmbs_api_ie_HANUnitIdAdd(p_List, u8_UnitId);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GROUP_ADD_DEVICE_TO_GROUP, p_List->pu8_Buffer, p_List->u16_CurSize );  

}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC   cmbs_dsr_han_group_RemoveDeviceFromGroup   (void *pv_AppRefHandle, u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId )
{
	PST_CFR_IE_LIST				p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);
	
	cmbs_api_ie_HANGroupIdAdd(p_List, u16_GroupId);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceId);
	cmbs_api_ie_HANUnitIdAdd(p_List, u8_UnitId);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GROUP_REMOVE_DEVICE_FROM_GROUP, p_List->pu8_Buffer, p_List->u16_CurSize );  

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_msg_RecvRegister (void *pv_AppRefHandle, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HanMsgRegInfoAdd(p_List,pst_HANMsgRegInfo);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MSG_RECV_REGISTER,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_msg_RecvUnregister (void *pv_AppRefHandle, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HanMsgRegInfoAdd(p_List,pst_HANMsgRegInfo);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MSG_RECV_UNREGISTER,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_msg_SendTxRequest (void *pv_AppRefHandle, u16 u16_DeviceId)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd(p_List,u16_DeviceId);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MSG_SEND_TX_START_REQUEST,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_msg_SendTxEnd(void *pv_AppRefHandle, u16 u16_DeviceId)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd(p_List,u16_DeviceId);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MSG_SEND_TX_END_REQUEST,p_List->pu8_Buffer, p_List->u16_CurSize);  

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_msg_Send (void *pv_AppRefHandle, u16 u16_RequestId, PST_IE_HAN_MSG_CTL pst_MsgCtrl, ST_IE_HAN_MSG * pst_HANMsg)
{
	PST_CFR_IE_LIST		p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_RequestId, CMBS_IE_REQUEST_ID);
	cmbs_api_ie_HANMsgAdd(p_List,pst_HANMsg);

	cmbs_api_ie_HANMsgCtlAdd(p_List, pst_MsgCtrl );

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MSG_SEND,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_raw_msg_Send (void *pv_AppRefHandle, u16 u16_RequestId, ST_IE_HAN_MSG_CTL* pst_MsgCtrl, ST_IE_DATA * pst_HANRawMsg, u16 u16_DeviceId) 
{
	PST_CFR_IE_LIST		p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_RequestId, CMBS_IE_REQUEST_ID);
	cmbs_api_ie_DataAdd(p_List, pst_HANRawMsg);
	cmbs_api_ie_HANDeviceAdd(p_List,u16_DeviceId);

	cmbs_api_ie_HANMsgCtlAdd(p_List, pst_MsgCtrl );

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_RAW_MSG_SEND,p_List->pu8_Buffer, p_List->u16_CurSize);  
}

E_CMBS_RC cmbs_dsr_han_ModificationULEPagingInterval(u16 u16_DeviceId, u32 u32_ULEPagingIntervalMs)
{
	PST_CFR_IE_LIST		p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	cmbs_api_ie_HANDeviceAdd(p_List,u16_DeviceId);
	cmbs_api_ie_HanULEPagingIntervalAdd(p_List,u32_ULEPagingIntervalMs);
	

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_MODIFICATION_ULE_PAGING_INTERVAL,p_List->pu8_Buffer, p_List->u16_CurSize);  
}

E_CMBS_RC cmbs_dsr_han_NodeSetupAttemptsEnable(bool b_enable)
{
	PST_CFR_IE_LIST		p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	if(b_enable)
	{
		cmbs_api_ie_HanNodeSetupAttemptsEnableAdd(p_List,CMBS_HAN_NODE_SETUP_ATTEMPTS_ENABLE);
	}
	else
	{
		cmbs_api_ie_HanNodeSetupAttemptsEnableAdd(p_List,CMBS_HAN_NODE_SETUP_ATTEMPTS_DISABLE);
	}
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_NODE_SETUP_ATTEMPTS,p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_dsr_han_GetMaxNumOfDevices (void *pv_AppRefHandle)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);
    
	return cmbs_int_EventSend(CMBS_EV_DSR_HAN_GET_MAX_NUM_OF_DEVICES, p_List->pu8_Buffer, p_List->u16_CurSize);
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_dsr_han_device_Delete (void *pv_AppRefHandle, u16 u16_DeviceId )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd( p_List, u16_DeviceId );

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE,p_List->pu8_Buffer, p_List->u16_CurSize);  
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_dsr_han_DeviceBlackListDelete(void *pv_AppRefHandle, u16 u16_DeviceId)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd( p_List, u16_DeviceId );

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_BLACK_LIST_DELETE,p_List->pu8_Buffer, p_List->u16_CurSize);  
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_dsr_han_send_base_updated_notification (void *pv_AppRefHandle, u16 u16_DeviceId )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd( p_List, u16_DeviceId );

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_SEND_BASE_UPDATED_NOTIFICATION,p_List->pu8_Buffer, p_List->u16_CurSize);  
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_dsr_han_get_fun_protocol_info (void *pv_AppRefHandle)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GET_FUN_PROTOCOL_INFO, p_List->pu8_Buffer, p_List->u16_CurSize);  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_dsr_han_device_GetConnectionStatus (void *pv_AppRefHandle, u16 u16_DeviceId )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd( p_List, u16_DeviceId );

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS ,p_List->pu8_Buffer, p_List->u16_CurSize);  
	
}

E_CMBS_RC cmbs_dsr_han_fun_RegistationMsgRecv  (void *pv_AppRefHandle, PST_FUN_DEVICE_INFO pst_FunDeviceInfo)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HanFUNDeviceInfoAdd( p_List, pst_FunDeviceInfo );

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_FUN_REG_MSG_RECV_RES, p_List->pu8_Buffer, p_List->u16_CurSize);  
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_ReadDeviceRegistrationInformation (void *pv_AppRefHandle, u16 u16_DeviceId)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd(p_List,u16_DeviceId);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO,p_List->pu8_Buffer, p_List->u16_CurSize);  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_ReadDeviceRegistrationInformationPhase2 (void *pv_AppRefHandle, u16 u16_DeviceId)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd(p_List,u16_DeviceId);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_PHASE_2,p_List->pu8_Buffer, p_List->u16_CurSize);  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Logger_SetLogLevel (void *pv_AppRefHandle, u8 u8_LogLevel)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANLogLevelAdd(p_List,u8_LogLevel);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_SET_LOG_LEVEL,p_List->pu8_Buffer, p_List->u16_CurSize);  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Logger_GetLogLevel (void *pv_AppRefHandle)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GET_LOG_LEVEL,p_List->pu8_Buffer, p_List->u16_CurSize);  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_ReadDeviceFullRegistrationInfo (void *pv_AppRefHandle, u16 u16_DeviceId)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_DeviceId, CMBS_IE_HAN_DEVICE);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_FULL_REGISTRATION_INFO,p_List->pu8_Buffer, p_List->u16_CurSize);  

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_device_WriteDeviceFullRegistrationInfo (void *pv_AppRefHandle, u16 u16_DeviceId, ST_HAN_FULL_DEVICE_REGISTRATION_INFO *pst_DeviceRegistrationInfo)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_ShortValueAdd(p_List,u16_DeviceId, CMBS_IE_HAN_DEVICE);
	cmbs_api_ie_HANDeviceFullRegistrationInfoAdd(p_List ,pst_DeviceRegistrationInfo);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_DEVICE_MANGER_WRITE_DEVICE_FULL_REGISTRATION_INFO, p_List->pu8_Buffer, p_List->u16_CurSize);  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_logger_Control (void *pv_AppRefHandle, bool b_enable)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	if (b_enable)
		{
			return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GENERAL_LOGGER_ENABLED, p_List->pu8_Buffer, p_List->u16_CurSize);
		}
	else
		{
			return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GENERAL_LOGGER_DISABLED, p_List->pu8_Buffer, p_List->u16_CurSize);
		}	  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_cp_logger_Control (void *pv_AppRefHandle, bool b_enable)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	if (b_enable)
		{
			return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CP_LOGGER_ENABLED, p_List->pu8_Buffer, p_List->u16_CurSize);
		}
	else
		{
			return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CP_LOGGER_DISABLED, p_List->pu8_Buffer, p_List->u16_CurSize);
		}	  

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Certification_MarkCcmKeyAsUsed(void *pv_AppRefHandle, u16 u16_DeviceNumber , u8 u8_CCMKeyUsed)
{

	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANCertificationOperationAdd(p_List, (u8) CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_MARK_CCM_KEY_AS_USED);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);
	cmbs_api_ie_HANCcmKeyAsUsedAdd(p_List, u8_CCMKeyUsed);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Certification_SetTxMicCorruptNum(void *pv_AppRefHandle,u16 u16_DeviceNumber)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANCertificationOperationAdd(p_List, (u8) CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_MIC_CORRUPTION_NUMBER);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Certification_SetTxSeqNum(void *pv_AppRefHandle, u16 u16_DeviceNumber, u16 u16_TxSequenceNumber )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANCertificationOperationAdd(p_List, (u8) CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_SEQUENCE_NUMBER);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);
	cmbs_api_ie_HANTxSequenceNumberAdd(p_List, u16_TxSequenceNumber);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Certification_GetTxSeqNum(void *pv_AppRefHandle, u16 u16_DeviceNumber)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANCertificationOperationAdd(p_List, (u8) CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_GET_TX_SEQUENCE_NUMBER);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Certification_GetRxSeqNum(void *pv_AppRefHandle, u16 u16_DeviceNumber)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANCertificationOperationAdd(p_List, (u8) CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_GET_RX_SEQUENCE_NUMBER);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Certification_SetTxAckDropNum(void *pv_AppRefHandle, u16 u16_TxAckDropNumber)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANCertificationOperationAdd(p_List, (u8) CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_ACK_DROP_NUMBER);
	cmbs_api_ie_HANTxAckDropNumberAdd(p_List, u16_TxAckDropNumber);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_Certification_SetPvcState(void *pv_AppRefHandle, u16 u16_DeviceNumber, u8 u8_PVCState )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANCertificationOperationAdd(p_List, (u8) CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_PVC_STATE);
	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);
	cmbs_api_ie_HANPvcStateAdd(p_List, u8_PVCState);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_CERTIFICATION_OPERATION_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_SetRxGain(void *pv_AppRefHandle,  u8 u8_RxGainVal )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);
	cmbs_api_ie_HANRxGainAdd(p_List, u8_RxGainVal);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_SET_RX_GAIN, p_List->pu8_Buffer, p_List->u16_CurSize);  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_GetRxGain(void *pv_AppRefHandle)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_GET_RX_GAIN, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_SendPvcResetReq(void *pv_AppRefHandle, u16 u16_DeviceNumber)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_PVC_RESET_REQ, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_GetPvcResetReqState(void *pv_AppRefHandle, u16 u16_DeviceNumber)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	cmbs_api_ie_HANDeviceAdd(p_List, u16_DeviceNumber);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_PVC_RESET_REQ_STATE, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_NotifyOnVoiceCall (void *pv_AppRefHandle, u16 u16_DeviceId)
{
	PST_CFR_IE_LIST	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd(p_List,u16_DeviceId);
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_NOTIFY_ON_VOICE_CALL_REQUEST,
                               p_List->pu8_Buffer, 
                               p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_TBR6SendPM(void *pv_AppRefHandle, u8  u8_Carrier , u32 u32_Period  )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANTBR6CarrierAdd(p_List, u8_Carrier);
	cmbs_api_ie_HANTBR6PeriodAdd(p_List, u32_Period);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_TBR6_SEND_PM, p_List->pu8_Buffer, p_List->u16_CurSize);  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_TBR6SetGUCI(void *pv_AppRefHandle, u8 * pu8_IPUI )
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANULEDeviceIPUIAdd(p_List, (ST_IE_HAN_ULE_DEVICE_IPUI *)pu8_IPUI);
	
	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_TBR6_SET_GUCI, p_List->pu8_Buffer, p_List->u16_CurSize);  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_dsr_han_TBR6ResetStatistics (void *pv_AppRefHandle)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	return cmbs_int_EventSend( CMBS_EV_DSR_HAN_TBR6_RESET_STATISTICS,p_List->pu8_Buffer, p_List->u16_CurSize);  

}

E_CMBS_RC cmbs_dsr_han_UpdateDevicePMSeqNumbers (void *pv_AppRefHandle, u16 u16_DeviceId , u16 u16_RxSeqNum, u16 u16_TxSeqNum)
{
	PST_CFR_IE_LIST			p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

	UNUSED_PARAMETER(pv_AppRefHandle);

	cmbs_api_ie_HANDeviceAdd( p_List, u16_DeviceId );
				
	cmbs_api_ie_HANRxSequenceNumberAdd (p_List,u16_RxSeqNum);
	cmbs_api_ie_HANTxSequenceNumberAdd (p_List,u16_TxSeqNum);	

	return cmbs_int_EventSend( CMBS_EV_DSR_UPDATE_DEVICE_PM_SEQ_NUMBERS,p_List->pu8_Buffer, p_List->u16_CurSize);  

}

/*---------[End Of File]---------------------------------------------------------------------------------------------------------------------------*/
