///////////////////////////////////////////////////////////////////////////////
///	
///	@file		CmndPubSub.h
///	@brief		This file provides API for Publish/Subscribe to an incoming CMND API message.
///
/// @details	It should be used to subscribe for incoming messages from other CMND service.
///
///	@internal
///	@author		Belkin Evgeny 
///	@date		09/03/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _CMND_PUB_SUB_H
#define _CMND_PUB_SUB_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "PubSub.h"
#include "CmndApiExported.h"

#define CMND_SERVICE_ANY			( (t_hanCmndServiceId) CMND_SERVICE_ID_UNKNOWN )
#define CMND_MSG_ANY				( (t_hanCmndMsgId) 0xFF )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
/// Init PubSub channel. Please, call it before work with cmnd channel
///
/// @param[in]	pst_Channel		A pointer to CMND PubSub channel
///
/// @return		true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_CmndPubSub_InitStatic( t_st_PubSub_Channel* pst_Channel, u8* pu8_Buffer, u32 u32_BufferSize );

///////////////////////////////////////////////////////////////////////////////
/// Initiate a call of subscribers handlers
///
/// @param[in]	pst_Channel		A CMND channel
/// @param[in]	messageId		A pointer to CMND API message structure
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
bool p_CmndPubSub_Publish( t_st_PubSub_Channel* pst_Channel, const t_st_hanCmndApiMsg* pst_Message);

///////////////////////////////////////////////////////////////////////////////
/// Adds a subscription for the event (serviceId, messageId) to the channel.
///
/// @param[in]	pst_Channel		A channel for subscription
/// @param[in]	serviceId		Service Id of the event to subscribe for
/// @param[in]	messageId		Message Id of the event to subscribe for
/// @param[in]	pfn_Handler		A handler to be called once the published event matches
///								to the ServiceId and MessageId.
/// @param[in]	p_Parameter		Any parameter. It will be passed as is on the pfn_Handler() call
/// @param[in]	en_Flags		Bit mask value (see t_en_PubSubFlags)
///
/// @return		true on success, otherwise on fail.
/// @warning	The method can fail if the channel capacity is exceeded.
///////////////////////////////////////////////////////////////////////////////
bool p_CmndPubSub_Subscribe(	t_st_PubSub_Channel*		pst_Channel,
    							t_hanCmndServiceId			serviceId,
								t_hanCmndMsgId				messageId,
								t_pf_PubSub_EventHandler	pfn_Handler,
								void*						p_Parameter,
								t_en_PubSubFlags			en_Flags);

///////////////////////////////////////////////////////////////////////////////
/// Adds a subscription for the event (serviceId, messageId) to the channel.
/// With p_CmndPubSub_SubscribeEx it can be valid to subscribe to one message more than one time with the same handler
///
/// @param[in]	pst_Channel		A channel for subscription
/// @param[in]	serviceId		Service Id of the event to subscribe for
/// @param[in]	messageId		Message Id of the event to subscribe for
/// @param[in]	pfn_Handler		A handler to be called once the published event matches
///								to the ServiceId and MessageId.
/// @param[in]	p_Parameter		Any parameter. It will be passed as is on the pfn_Handler() call
/// @param[in]	en_Flags		Bit mask value (see t_en_PubSubFlags)
/// @param[in]	pv_KeyParam 	it is like user parameter, but it used also like a part of key
///
/// @return		true on success, otherwise on fail.
/// @warning	The method can fail if the channel capacity is exceeded.
///////////////////////////////////////////////////////////////////////////////
bool p_CmndPubSub_SubscribeEx(	t_st_PubSub_Channel*		pst_Channel,
								t_hanCmndServiceId			serviceId,
								t_hanCmndMsgId				messageId,
								t_pf_PubSub_EventHandler	pfn_Handler,
								void*						p_Parameter,
								t_en_PubSubFlags			en_Flags,
								void*						pv_KeyParam);

///////////////////////////////////////////////////////////////////////////////
/// Removes the subscription from the channel. The subscription identified
///		by it's Service Id, Message Id and a handler (pointer to a function).
///     The call has no effect if specified subscription does not found.
///
/// @param[in]	pst_Channel		A channel for unsubscription.
/// @param[in]	serviceId		Service Id to identify the subscription.
/// @param[in]	messageId		Message Id to identify the subscription.
/// @param[in]	pfn_Handler		An event handler to identify the subscription.
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
void p_CmndPubSub_Unsubscribe(	t_st_PubSub_Channel*		pst_Channel,
								t_hanCmndServiceId			serviceId,
								t_hanCmndMsgId				messageId,
								t_pf_PubSub_EventHandler	pfn_Handler );

///////////////////////////////////////////////////////////////////////////////
/// Removes the subscription from the channel. The subscription identified
///		by it's Service Id, Message Id and a handler (pointer to a function).
///     The call has no effect if specified subscription does not found.
/// @note use after p_CmndPubSub_SubscribeEx
///
/// @param[in]	pst_Channel		A channel for unsubscription.
/// @param[in]	serviceId		Service Id to identify the subscription.
/// @param[in]	messageId		Message Id to identify the subscription.
/// @param[in]	pfn_Handler		An event handler to identify the subscription.
/// @param[in]	pv_KeyParam 	it is like user parameter, but it used also like a part of key
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
void p_CmndPubSub_UnsubscribeEx(t_st_PubSub_Channel*		pst_Channel,
								t_hanCmndServiceId			serviceId,
								t_hanCmndMsgId				messageId,
								t_pf_PubSub_EventHandler	pfn_Handler,
								void*						pv_KeyParam );

extern_c_end

#endif // _CMND_PUB_SUB_H
