///////////////////////////////////////////////////////////////////////////////
///	
///	@file		CmndMsg_Alert.h
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

#ifndef _CMND_MSG_ALERT_H
#define _CMND_MSG_ALERT_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "TypeDefs.h"
#include "CmndApiExported.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///	@brief	Create Alert packet buffer based on given parameters.
///			The <pst_hanCmndApiMsg> will contain all CMND API message fields.
///			Data is stored in network order.
///
///	@param[out]		pst_hanCmndApiMsg	- pointer to message
///	@param[in]		u8_UnitId			- unit ID
///	@param[in]		pst_Alert			- pointer to IeAlert
///
/// @note	If response is needed then add CMND_IE_RESPONSE_REQUIRED with using
///			#p_CmndCreator_AddResponseRequiredIe.
///
///	@return		true when success
///////////////////////////////////////////////////////////////////////////////
bool p_CmndMsg_Alert_CreateNotifyStatusReq(OUT t_st_hanCmndApiMsg*		pst_hanCmndApiMsg,
										u8							u8_UnitId,
										const t_st_hanCmndIeAlert*	pst_Alert );

///////////////////////////////////////////////////////////////////////////////
///	@brief	Create Alert packet buffer based on given parameters.
///			The <pst_hanCmndApiMsg> will contain all CMND API message fields.
///			Data is stored in network order.
///
///	@param[out]		pst_hanCmndApiMsg	- pointer to message
///	@param[in]		u8_UnitId			- unit ID
///	@param[in]		pst_Response		- pointer to IeResponse
///
/// @note	If response is needed then add CMND_IE_RESPONSE_REQUIRED with using
///			#p_CmndCreator_AddResponseRequiredIe.
///
///	@return		true when success
///////////////////////////////////////////////////////////////////////////////
bool p_CmndMsg_Alert_CreateNotifyStatusRes( OUT t_st_hanCmndApiMsg*		pst_hanCmndApiMsg,
										 u8								u8_UnitId,
										 const t_st_hanCmndIeResponse*	pst_Response );


extern_c_end

#endif  //_CMND_MSG_ALERT_H
