///////////////////////////////////////////////////////////////////////////////
///	
///	@file		FunPubSub.c
///	@brief		FUN Reference code for RAW FUN Publish/Subscribe implementation
///
/// Used for subscribing to incoming RAW FUN messages.
/// It can be use to subscribe for incoming RAW FUN messages
///
///	@internal
///	@author		Strunin Andrey
///	@date		05/04/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "FunPubSub.h"
#include "Logger.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#pragma pack(1)
#endif

typedef PACK_STRUCT
{
	u16		u16_FunIfaceId; //!< Fun interface member ID
	u8		u8_IfaceType;	//!< Fun interface type (0-client or 1-server)
	u8		u8_MessageType; //!< see t_en_hanCmndFunMsgType
	u8		u8_IfaceMember; //!< Pointer to handler function
} t_st_FunPubSub_Filter;

#ifdef _MSC_VER
#pragma pack()
#endif


//////////////////////////////////////////////////////////////////////////
/// Compare two filters
///
/// @param[in]		p_Item1 pointer to first item
/// @param[in]		u32_Item1Size size of first item
/// @param[in]		p_Item2 pointer to second item
/// @param[in]		u32_Item2Size size of second item
/// @return			true if equal
//////////////////////////////////////////////////////////////////////////
static bool p_FunPubSub_Compare(	const p_DictItemData	p_Item1,
									u32						u32_Item1Size, 
									const void*				p_Item2, 
									u32						u32_Item2Size )
{	
	t_st_KeyValuePair* pst_Item2 = (t_st_KeyValuePair*)p_Item2;

	t_st_FunPubSub_Filter* pst_Filter1 = (t_st_FunPubSub_Filter*) p_Dictionary_GetKeyPointerFromDictItem( p_Item1 );
	t_st_FunPubSub_Filter* pst_Filter2 = (t_st_FunPubSub_Filter*) pst_Item2->p_Key;

	bool Matched =	( pst_Filter1->u16_FunIfaceId == pst_Filter2->u16_FunIfaceId )
					||( pst_Filter1->u16_FunIfaceId == FUN_IFACE_ID_ANY )
					||( pst_Filter2->u16_FunIfaceId == FUN_IFACE_ID_ANY );
	
	
	Matched = Matched &&( pst_Filter1->u8_IfaceType == pst_Filter2->u8_IfaceType );
	Matched = Matched && ( pst_Filter1->u8_MessageType == pst_Filter2->u8_MessageType );
	Matched = Matched &&(( pst_Filter1->u8_IfaceMember == pst_Filter2->u8_IfaceMember )
						|| ( pst_Filter1->u8_IfaceMember == FUN_IFACE_MEMBER_ANY )
						|| ( pst_Filter2->u8_IfaceMember == FUN_IFACE_MEMBER_ANY ));
	
	return Matched;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_FunPubSub_InitStatic ( t_st_PubSub_Channel* pst_Channel, u8* pu8_Buffer, u32 u32_BufSize )
{
	p_PubSub_ChannelInitStatic( pst_Channel, (t_pf_PubSubComparer) p_FunPubSub_Compare, pu8_Buffer, u32_BufSize );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_FunPubSub_Subscribe(		t_st_PubSub_Channel*		pst_Channel,
								u16							u16_FunIfaceId,
								u8							u8_IfaceType,
								u8							u8_MessageType,
								u8							u8_IfaceMember,
								t_pf_PubSub_EventHandler	pfn_Handler,
								void*						p_Parameter,
								t_en_PubSubFlags			en_Flags)
{
	t_st_FunPubSub_Filter st_Filter;
	
	st_Filter.u16_FunIfaceId = u16_FunIfaceId;
	st_Filter.u8_IfaceType = u8_IfaceType;
	st_Filter.u8_MessageType = u8_MessageType;
	st_Filter.u8_IfaceMember = u8_IfaceMember;

	return p_PubSub_Subscribe( pst_Channel, (void*) &st_Filter, sizeof(st_Filter), pfn_Handler, p_Parameter, en_Flags, NULL );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_FunPubSub_Unsubscribe(	t_st_PubSub_Channel*		pst_Channel,
								u16							u16_FunIfaceId,
								u8							u8_IfaceType,
								u8							u8_MessageType,
								u8							u8_IfaceMember,
								t_pf_PubSub_EventHandler	pfn_Handler )
{
	t_st_FunPubSub_Filter st_Filter;

	st_Filter.u16_FunIfaceId = u16_FunIfaceId;
	st_Filter.u8_IfaceMember = u8_IfaceMember;
	st_Filter.u8_IfaceType = u8_IfaceType;
	st_Filter.u8_MessageType = u8_MessageType;
	
	p_PubSub_Unsubscribe( pst_Channel, (void*)&st_Filter, sizeof(st_Filter), pfn_Handler, NULL );

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_FunPubSub_Publish(	t_st_PubSub_Channel*	pst_Channel,
							u16						u16_FunIfaceId,
							u8						u8_IfaceType,
							u8						u8_MessageType,
							u8						u8_IfaceMember,
							t_st_hanCmndIeFun*		pst_IeFun )
{
	t_st_FunPubSub_Filter st_Filter;

	st_Filter.u16_FunIfaceId = u16_FunIfaceId;
	st_Filter.u8_IfaceMember = u8_IfaceMember;
	st_Filter.u8_IfaceType = u8_IfaceType;
	st_Filter.u8_MessageType = u8_MessageType;

	return p_PubSub_Publish ( pst_Channel, (void*)&st_Filter, sizeof(st_Filter), (void*) pst_IeFun );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
