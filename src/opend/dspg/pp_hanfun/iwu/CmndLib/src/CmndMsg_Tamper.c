///////////////////////////////////////////////////////////////////////////////
///	
///	@file		varCmndMsgCreator_Tamper.c
///	@brief		Used to create a CMND API packets of Tamper service
///
///	@internal
///	@author		Iurov Maxim
///	@date		25/02/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "CmndMsg_Tamper.h"
#include "CmndApiIe.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Create packet of Tamper Alert message
bool p_CmndMsg_TamperAlert_CreateSendNotifyStatusReq( OUT t_st_hanCmndApiMsg* pst_hanCmndApiMsg, u8 u8_UnitId, const t_st_hanCmndIeTamperAlert* pst_Tamper )
{
	t_st_hanIeList st_IeList;
	bool ok = false;
	pst_hanCmndApiMsg->serviceId		= CMND_SERVICE_ID_TAMPER_ALERT;
	pst_hanCmndApiMsg->messageId		= CMND_MSG_TAMPER_ALERT_NOTIFY_STATUS_REQ;

	// IE list is used to easily add IE object to the list
	p_hanIeList_CreateEmpty( pst_hanCmndApiMsg->data, sizeof( pst_hanCmndApiMsg->data ), &st_IeList );
	
	// Add simple IE
	ok = p_hanIeList_AddIeSimple( &st_IeList, CMND_IE_TAMPER_ALERT, (u8*)pst_Tamper, sizeof(t_st_hanCmndIeTamperAlert) );
	
	pst_hanCmndApiMsg->dataLength = p_hanIeList_GetListSize( &st_IeList );
	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Create packet of Alert response message
bool p_CmndMsg_TamperAlert_CreateSendNotifyStatusRes( OUT t_st_hanCmndApiMsg* pst_hanCmndApiMsg, u8 u8_UnitId, const t_st_hanCmndIeResponse* pst_Response )
{
	t_st_hanIeList st_IeList;
	bool ok = false;
	pst_hanCmndApiMsg->serviceId = CMND_SERVICE_ID_TAMPER_ALERT;
	pst_hanCmndApiMsg->messageId = CMND_MSG_TAMPER_ALERT_NOTIFY_STATUS_RES;

	// IE list is used to easily add IE object to the list
	p_hanIeList_CreateEmpty( pst_hanCmndApiMsg->data, sizeof( pst_hanCmndApiMsg->data ), &st_IeList );

	// Add simple IE
	ok = p_hanCmndApi_IeResponseAdd( &st_IeList, pst_Response );

	// update data length field
	pst_hanCmndApiMsg->dataLength = p_hanIeList_GetListSize( &st_IeList );

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
