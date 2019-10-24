///////////////////////////////////////////////////////////////////////////////
///	
///	@file		varCmndMsg_DeviceManagement.c
///	@brief		Used to create a CMND API packets of DeviceManagement service
///
///	@internal
///	@author		Iurov Maxim
///	@date		25/02/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "CmndMsg_DeviceManagement.h"
#include "CmndApiIe.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Create packet of registration device message
bool p_Cmnd_DeviceManagement_CreateRegisterDeviceReq(	OUT t_st_hanCmndApiMsg*			pst_hanCmndApiMsg,
														const t_st_hanCmndIeBaseWanted* pst_baseWanted )
{
	bool ok = true;

	pst_hanCmndApiMsg->serviceId = CMND_SERVICE_ID_DEVICE_MANAGEMENT;
	pst_hanCmndApiMsg->messageId = CMND_MSG_DEV_MGNT_REGISTER_DEVICE_REQ;
	pst_hanCmndApiMsg->unitId = 0;
	

	// Add IeRegisterBaseWanted if rfpi was set
	if ( pst_baseWanted )
	{
		t_st_hanIeList st_IeList;

		// IE list is used to easily add IE object to the list
		p_hanIeList_CreateEmpty( pst_hanCmndApiMsg->data, sizeof( pst_hanCmndApiMsg->data ), &st_IeList );

		// Add Base Wanted IE
		ok = p_hanCmndApi_IeBaseWantedAdd( &st_IeList, pst_baseWanted );

		// update data length field
		pst_hanCmndApiMsg->dataLength = p_hanIeList_GetListSize( &st_IeList );
		
		
	}
	else
	{
		pst_hanCmndApiMsg->dataLength = 0;
	}
	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Create packet of Deregisteration device message
bool p_Cmnd_DeviceManagement_CreateDeregisterDeviceReq( OUT t_st_hanCmndApiMsg*	pst_hanCmndApiMsg )
{
	pst_hanCmndApiMsg ->serviceId = CMND_SERVICE_ID_DEVICE_MANAGEMENT;
	pst_hanCmndApiMsg ->messageId = CMND_MSG_DEV_MGNT_DEREGISTER_DEVICE_REQ;
	pst_hanCmndApiMsg->unitId	  = 0;
	pst_hanCmndApiMsg->dataLength = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
