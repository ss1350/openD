///////////////////////////////////////////////////////////////////////////////
///	
///	@file		CmndPubSub.c
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

#include "CmndPubSub.h"
#include "Logger.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma pack(1)
#endif

typedef PACK_STRUCT
{
	t_hanCmndServiceId	u16_ServiceId;
	t_hanCmndMsgId		u8_MessageId;
} t_st_CmndPubSub_Filter;

#ifdef _MSC_VER
#pragma pack()
#endif

STATIC_ASSERT( sizeof(t_st_CmndPubSub_Filter) <= PUB_SUB_MAX_FILTER_SIZE,
		sizeof_t_st_CmndPubSub_Filter_must_fit_into_PUB_SUB_MAX_FILTER_SIZE );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// Compare two filters
///
/// @param[in]		p_Item1 pointer to first item
/// @param[in]		u32_Item1Size size of first item
/// @param[in]		p_Item2 pointer to second item
/// @param[in]		u32_Item2Size size of second item
/// @return			true if equal
//////////////////////////////////////////////////////////////////////////
static bool p_CmndPubSub_Compare(	const p_DictItemData p_Item1,
									u32					u32_Item1Size, 
									const void*			p_Item2, 
									u32					u32_Item2Size)
{	

	t_st_KeyValuePair* pst_Item2 = (t_st_KeyValuePair*)p_Item2;

	t_st_CmndPubSub_Filter* pst_Filter1 = (t_st_CmndPubSub_Filter*) p_Dictionary_GetKeyPointerFromDictItem( p_Item1 );
	t_st_CmndPubSub_Filter* pst_Filter2 = (t_st_CmndPubSub_Filter*)pst_Item2->p_Key;

	// check by service id
	bool Matched =	( u32_Item1Size >= sizeof( t_st_CmndPubSub_Filter ) );
	Matched = Matched &&(( pst_Filter1->u16_ServiceId == pst_Filter2->u16_ServiceId )
						||( pst_Filter1->u16_ServiceId == CMND_SERVICE_ANY )
						||( pst_Filter2->u16_ServiceId == CMND_SERVICE_ANY ));

	// check by service msg id
	Matched = Matched && ( ( pst_Filter1->u8_MessageId == pst_Filter2->u8_MessageId )
		||( pst_Filter1->u8_MessageId == CMND_MSG_ANY )
		||( pst_Filter2->u8_MessageId == CMND_MSG_ANY ) );
	

	return Matched;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndPubSub_InitStatic( t_st_PubSub_Channel* pst_Channel, u8* pu8_Buffer, u32 u32_BufferSize )
{
	return p_PubSub_ChannelInitStatic( pst_Channel, (t_pf_PubSubComparer) p_CmndPubSub_Compare, pu8_Buffer, u32_BufferSize );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndPubSub_Subscribe(	t_st_PubSub_Channel* pst_Channel,
								t_hanCmndServiceId serviceId,
								t_hanCmndMsgId messageId,
								t_pf_PubSub_EventHandler pfn_Handler,
								void* p_Parameter,
								t_en_PubSubFlags en_Flags )
{
	return p_CmndPubSub_SubscribeEx( pst_Channel, serviceId, messageId, pfn_Handler, p_Parameter, en_Flags, NULL );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndPubSub_SubscribeEx(	t_st_PubSub_Channel*		pst_Channel,
								t_hanCmndServiceId			serviceId,
								t_hanCmndMsgId				messageId,
								t_pf_PubSub_EventHandler	pfn_Handler,
								void*						p_Parameter,
								t_en_PubSubFlags			en_Flags,
								void*						pv_KeyParam)
{
	bool b_Result;
	t_st_CmndPubSub_Filter st_Filter;

	st_Filter.u8_MessageId = messageId;
	st_Filter.u16_ServiceId = serviceId;

	b_Result = p_PubSub_Subscribe( pst_Channel, (void*) &st_Filter, sizeof(st_Filter), pfn_Handler, p_Parameter, en_Flags, pv_KeyParam );

	//LOG_INFO( "%d = p_CmndPubSub_Subscribe(%u)", b_Result, pst_Channel->st_Table.st_Dict.st_List.u16_Count );

	return b_Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_CmndPubSub_Unsubscribe(	t_st_PubSub_Channel* pst_Channel,
								t_hanCmndServiceId serviceId,
								t_hanCmndMsgId messageId,
								t_pf_PubSub_EventHandler pfn_Handler )
{
	p_CmndPubSub_UnsubscribeEx( pst_Channel, serviceId, messageId, pfn_Handler, NULL );
}


void p_CmndPubSub_UnsubscribeEx(	t_st_PubSub_Channel*		pst_Channel,
									t_hanCmndServiceId			serviceId,
									t_hanCmndMsgId				messageId,
									t_pf_PubSub_EventHandler	pfn_Handler,
									void*						pv_KeyParam )
{
	t_st_CmndPubSub_Filter st_Filter;

	st_Filter.u8_MessageId = messageId;
	st_Filter.u16_ServiceId = serviceId;

	p_PubSub_Unsubscribe( pst_Channel, (void*)&st_Filter, sizeof(st_Filter), pfn_Handler, pv_KeyParam );

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndPubSub_Publish( t_st_PubSub_Channel* pst_Channel, const t_st_hanCmndApiMsg* pst_Message )
{
	t_st_CmndPubSub_Filter st_Filter;

	st_Filter.u8_MessageId = pst_Message->messageId;
	st_Filter.u16_ServiceId = pst_Message->serviceId;

	return p_PubSub_Publish ( pst_Channel, (void*)&st_Filter, sizeof(st_Filter), (void *)pst_Message );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
