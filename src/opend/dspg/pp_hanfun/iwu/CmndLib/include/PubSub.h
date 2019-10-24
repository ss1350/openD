///////////////////////////////////////////////////////////////////////////////
///	
///	@file		PubSub.h
///	@brief		This file provides API for Publish/Subscribe mechanism.
///
///	@details	Used to deliver a message from a publisher to one or more
///				subscribers in the channel.
///				Messages are filtered using specified handler.
///
///	@internal
///	@author		Strunin Andrey
///	@date		22/03/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _PUB_SUB_H
#define _PUB_SUB_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "Table.h"
#include "TypeDefs.h"
#include "CmndApiExported.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// PubSub flags
typedef enum
{
	PUB_SUB_EMPTY		 = 0, //!< default: onetime subscription
	PUB_SUB_PERSISTENT	 = 1, //!< A persistent subscription
} t_en_PubSubFlags;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// PubSub Item data
typedef PACK_STRUCT
{
	t_en_PubSubFlags			en_Flags;		//!< An options
	void*						p_Parameter;	//!< An optional parameter for handler
} t_st_PubSubValue;

///////////////////////////////////////////////////////////////////////////////
/// @brief		This is a function prototype of channel item event handler
///
/// @param[in]	p_Msg			A pointer to the message
/// @param[in]	p_Parameter		A pointer to the optional param
///////////////////////////////////////////////////////////////////////////////
typedef void (*t_pf_PubSub_EventHandler)( void* p_Msg, void* p_KeyParameter, void* p_Parameter );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma pack(1)
#endif

#ifndef PUB_SUB_MAX_FILTER_SIZE
#define PUB_SUB_MAX_FILTER_SIZE 32
#endif // PUB_SUB_MAX_FILTER_SIZE

/// The key of PubSub channel. Used to identify subscriber.
typedef PACK_STRUCT
{
	u8 au8_Filter[PUB_SUB_MAX_FILTER_SIZE];		//!< 
	u8 u8_FilterSize;							//!< 
	t_pf_PubSub_EventHandler	pf_Handler;		//!< A pointer to event handler
	void *pv_Param;
} t_st_PubSubKey;

#ifdef _MSC_VER
#pragma pack()
#endif

#define STATIC_PUBSUB_BUFF_SIZE(items_count) STATIC_TABLE_BUF_SIZE(sizeof(t_st_PubSubKey), sizeof(t_st_PubSubValue), items_count)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// The field count, their size and location should match to t_st_KeyValuePair
typedef PACK_STRUCT  
{
	t_st_PubSubKey*		p_Key;			//!< 
	u16					u16_KeySize;	//!< 
	t_st_PubSubValue*	p_Value;		//!< 
	u16					u16_ValueSize;	//!< 
} t_st_PubSubKvp;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// The signature should be convertable to t_pf_KvpComparer
typedef bool ( *t_pf_PubSubComparer )(	const p_DictItemData	p_SubscriberItem, 
										u32						u32_SubscriberItemSize,
										const t_st_PubSubKvp*	pst_PublishedItem,
										u32						u32_PublishedItemSize );

/// PubSub channel entity structure. 
/// <pst_Table> Stores the subscriptions. Should be initialized with NULL.
/// 			Will be allocated automatically on any call Subscribe() or Publish().
/// 			Normally it's no need to touch table while using PubSub.
/// <pf_Comparer> Should be initialized with a pointer to comparer function.
/// 			  This function should match data in two t_st_PubSubKvp.
typedef PACK_STRUCT
{
	t_st_Table				st_Table;		//!< A channel table
	t_pf_PubSubComparer		pf_Comparer;	//!< A pointer to specified compare function
} t_st_PubSub_Channel;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function initialize PubSub channel <pst_Channel> and set 
///				it's Comparer <pf_Comparer>
///
/// @param[inout]	pst_Channel		a pointer to PubSub channel
/// @param[in]		pf_Comparer		a pointer to filter data which will be
///									compared against key during Publish()
///
/// @return		on success return true. in case of failure return false.
///////////////////////////////////////////////////////////////////////////////
bool p_PubSub_ChannelInitStatic(	INOUT t_st_PubSub_Channel*	pst_Channel,
									t_pf_PubSubComparer			pf_Comparer,
									u8*							pu8_Buffer,
									u32							u32_BufferSize );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function try to make new subscribe with filter <p_Filter>, 
///				event handler <pf_Handler>, optional param <p_Parameter> and 
///				flags <en_Flags> in channel <pst_Channel>.
///				Make lazy init if the channel has not been initialized
///
/// @param[in]	pst_Channel		a pointer to PubSub channel
/// @param[in]	p_Filter		a pointer to filter data which will be
///								compared against key during Publish()
/// @param[in]	u8_FilterSize	size of filter data
/// @param[in]	pf_Handler		a pointer to item event handler
/// @param[in]	p_Parameter		a pointer to optional parameter
/// @param[in]	en_Flags		flags
///
/// @return		on success return true. in case of failure return false.
///				The method can fail if the channel capacity is exceeded.
///////////////////////////////////////////////////////////////////////////////
bool p_PubSub_Subscribe(	t_st_PubSub_Channel*			pst_Channel,
							const void*						p_Filter, 
							u8								u8_FilterSize,
							const t_pf_PubSub_EventHandler	pf_Handler,
							void*							p_Parameter,
							const t_en_PubSubFlags			en_Flags,
							void*							pv_KeyParam );

///////////////////////////////////////////////////////////////////////////////
/// @brief		this function unsubscribe and remove item with filter <p_Filter> 
///				and event handler <pfn_Handler> from channel <pst_Channel>
///				Make lazy init if the channel has not been initialized
///
/// @param[in]	pst_Channel		a pointer to PubSub channel
/// @param[in]	p_Key			a pointer to key data
/// @param[in]	u8_KeySize		size of key data
/// @param[in]	pf_Handler		a pointer to item event handler
///////////////////////////////////////////////////////////////////////////////
void p_PubSub_Unsubscribe(	t_st_PubSub_Channel*			pst_Channel,
							const void*						p_Key,
							u8								u8_KeySize,
							const t_pf_PubSub_EventHandler	pf_Handler,
							void*							pv_KeyParam  );

///////////////////////////////////////////////////////////////////////////////
/// @brief		this function call item event handler with param <p_Param> for 
///				all items of channel <pst_Channel> match with filter <p_Filter>.
///				Make lazy init if the channel has not been initialized
///			
/// @param[in]	pst_Channel		a pointer to PubSub channel
/// @param[in]	p_Filter		a pointer to filter
/// @param[in]	u8_FilterSize	size of filter
/// @param[in]	p_Message		a pointer to be passed into handler
///////////////////////////////////////////////////////////////////////////////
bool p_PubSub_Publish(	t_st_PubSub_Channel*	pst_Channel, 
						void*					p_Filter, 
						u8						u8_FilterSize, 
						void*					p_Message);


///////////////////////////////////////////////////////////////////////////////
/// @brief		this function remove all subscribes from channel <pst_Channel>
///			
/// @param[in]	pst_Channel		a pointer to PubSub channel
///////////////////////////////////////////////////////////////////////////////
void p_PubSub_Clear( t_st_PubSub_Channel* pst_Channel );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_end

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // _PUB_SUB_H
