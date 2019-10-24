///////////////////////////////////////////////////////////////////////////////
///	
///	@file		varCmndMsg_Alert.c
///	@brief		Used to create a CMND API packets of Alert service
///
///	@internal
///	@author		Iurov Maxim
///	@date		25/02/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "CmndMsg_Alert.h"
#include "CmndApiIe.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Create packet of Alert message
bool p_CmndMsg_Alert_CreateNotifyStatusReq(	OUT t_st_hanCmndApiMsg*		pst_hanCmndApiMsg,
											u8							u8_UnitId,
											const t_st_hanCmndIeAlert*	pst_Alert )
{
	bool ok = false;
	t_st_hanIeList st_IeList;

	pst_hanCmndApiMsg->serviceId	= CMND_SERVICE_ID_ALERT;
	pst_hanCmndApiMsg->messageId	= CMND_MSG_ALERT_NOTIFY_STATUS_REQ;
	pst_hanCmndApiMsg->unitId		= u8_UnitId;

	// IE list is used to easily add IE object to the list
	p_hanIeList_CreateEmpty( pst_hanCmndApiMsg->data, sizeof(pst_hanCmndApiMsg->data), &st_IeList );

	// add the Alert IE
	ok = p_hanCmndApi_IeAlertAdd( &st_IeList, pst_Alert );

	// update data length field
	pst_hanCmndApiMsg->dataLength = p_hanIeList_GetListSize(&st_IeList);

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Create packet of Alert response message
bool p_CmndMsg_Alert_CreateNotifyStatusRes( OUT t_st_hanCmndApiMsg*		pst_hanCmndApiMsg,
											u8								u8_UnitId,
											const t_st_hanCmndIeResponse*	pst_Response )
{
	bool ok = false;
	t_st_hanIeList st_IeList;

	pst_hanCmndApiMsg->serviceId = CMND_SERVICE_ID_ALERT;
	pst_hanCmndApiMsg->messageId = CMND_MSG_ALERT_NOTIFY_STATUS_RES;
	pst_hanCmndApiMsg->unitId = u8_UnitId;

	// IE list is used to easily add IE object to the list
	p_hanIeList_CreateEmpty( pst_hanCmndApiMsg->data, sizeof( pst_hanCmndApiMsg->data ), &st_IeList );

	// add the Alert IE
	ok = p_hanCmndApi_IeResponseAdd( &st_IeList, pst_Response );

	// update data length field
	pst_hanCmndApiMsg->dataLength = p_hanIeList_GetListSize( &st_IeList );

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
