///////////////////////////////////////////////////////////////////////////////
///	
///	@file		FunPubSub.h
///	@brief		FUN Reference code for Publish/Subscribe implementation
///
/// Used for subscribing to incoming RAW FUN messages.
/// It should be used to subscribe for incoming RAW FUN messages
///
///	@internal
///	@author		Strunin Andrey
///	@date		05/04/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _FUN_PUB_SUB_H
#define _FUN_PUB_SUB_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "PubSub.h"
#include "CmndApiExported.h"


#define FUN_IFACE_MEMBER_ANY			( 0xFF )
#define FUN_IFACE_ID_ANY				( 0xFFFF )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
/// Init PubSub channel with preallocated buffer. Please, call it before work with fun channel
///
/// @param[in]	pst_Channel		A pointer to Fun PubSub channel
/// @param[in]  pu8_Buffer		A buffer size
/// @param[in]	u32_BufSize		A size of buffer
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
void p_FunPubSub_InitStatic ( t_st_PubSub_Channel* pst_Channel, u8* pu8_Buffer, u32 u32_BufSize );

///////////////////////////////////////////////////////////////////////////////
/// Initiate a call of subscribers handlers
///
/// @param[in]	pst_Channel		A CMND channel
/// @param[in]	u16_FunIfaceId	Service Id of the event to subscribe for
/// @param[in]	u8_IfaceType	0 - client, 1 - server
/// @param[in]	u8_MessageType	@see t_en_hanCmndFunMsgType	
/// @param[in]	u8_IfaceMember	depending on message type, Command or attribute id. Message ID analog for RAW FUN Services.
/// @param[in]	pst_IeFun		A pointer to RAW FUN message (CMND_IE_FUN)
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
bool p_FunPubSub_Publish(	t_st_PubSub_Channel*	pst_Channel,
							u16						u16_FunIfaceId,
							u8						u8_IfaceType,
							u8						u8_MessageType,
							u8						u8_IfaceMember,
							t_st_hanCmndIeFun*		pst_IeFun );

///////////////////////////////////////////////////////////////////////////////
/// Adds a subscription for the event (serviceId, messageId) to the channel.
///
/// @param[in]	pst_Channel		A channel for subscription
/// @param[in]	u16_FunIfaceId	Service Id of the event to subscribe for
/// @param[in]	u8_IfaceType	0 - client, 1 - server
/// @param[in]	u8_MessageType	@see t_en_hanCmndFunMsgType	
/// @param[in]	u8_IfaceMember	depending on message type, Command or attribute id. Message ID analog for RAW FUN Services.
/// @param[in]	pfn_Handler		A handler to be called once the published event matches
///								to the ServiceId and MessageId.
/// @param[in]	p_Parameter		Any parameter. It will be passed as is on the pfn_Handler() call
/// @param[in]	en_Flags		Bit mask value (see t_en_PubSubFlags)
///
/// @return		true on success, otherwise on fail.
/// @warning	The method can fail if the channel capacity is exceeded.
///////////////////////////////////////////////////////////////////////////////
bool p_FunPubSub_Subscribe(		t_st_PubSub_Channel*		pst_Channel,
    							u16							u16_FunIfaceId,
								u8							u8_IfaceType,
								u8							u8_MessageType,
								u8							u8_IfaceMember,
								t_pf_PubSub_EventHandler	pfn_Handler,
								void*						p_Parameter,
								t_en_PubSubFlags			en_Flags);

///////////////////////////////////////////////////////////////////////////////
/// Removes the subscription from the channel. The subscription identified
///		by it's Service Id, Message Id and a handler (pointer to a function).
///     The call has no effect if specified subscription does not found.
///
/// @param[in]	pst_Channel				A channel for unsubscription.
/// @param[in]	u16_FunIfaceId			Service Id of the event to subscribe for
/// @param[in]	u8_IfaceType			0 - client, 1 - server
/// @param[in]	u8_MessageType			@see t_en_hanCmndFunMsgType	
/// @param[in]	u8_IfaceMember			depending on message type, Command or attribute id. Message ID analog for RAW FUN Services.
/// @param[in]	pfn_Handler		An event handler to identify the subscription.
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
void p_FunPubSub_Unsubscribe(	t_st_PubSub_Channel*		pst_Channel,
								u16							u16_FunIfaceId,
								u8							u8_IfaceType,
								u8							u8_MessageType,
								u8							u8_IfaceMember,
								t_pf_PubSub_EventHandler	pfn_Handler );

extern_c_end

#endif // _FUN_PUB_SUB_H
