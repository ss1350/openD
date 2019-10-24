///////////////////////////////////////////////////////////////////////////////
///	
///	@file		PubSub.c
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

#include "PubSub.h"
#include "Logger.h"
#include <string.h> //memcpy

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define PUB_SUB_MAX_VALUE_SIZE ( sizeof(t_st_PubSubValue) + sizeof(t_st_PubSubKey) )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static bool p_PubSub_Remove(	t_st_PubSub_Channel*	pst_Channel,
								t_st_PubSubKey*			pst_Key,
								u16						u16_KeySize );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_PubSub_ChannelInitStatic( INOUT t_st_PubSub_Channel*		pst_Channel,
								 t_pf_PubSubComparer			pf_Comparer,
								 u8*							pu8_Buffer,
								 u32							u32_BufferSize )
{
	bool Success = false;
	bool Locked = false;

	do
	{
		Success = ( pst_Channel
					&&	pf_Comparer );
		BREAK_IF( !Success );

		Success = p_Table_InitStatic(	&pst_Channel->st_Table, 
										sizeof( t_st_PubSubKey ), 
										sizeof( t_st_PubSubValue ),
										pu8_Buffer, 
										u32_BufferSize );

		BREAK_IF( !Success );

		Success = Locked = p_Table_Lock( &pst_Channel->st_Table, TABLE_CS_ENTER_TIMEOUT );
		BREAK_IF( !Success );

		pst_Channel->pf_Comparer = pf_Comparer;
	} while (false);

	if (Locked)
	{
		p_Table_Unlock( &pst_Channel->st_Table );
	}

	return Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_PubSub_Subscribe(	t_st_PubSub_Channel*			pst_Channel,
							const void*						p_Filter,
							u8								u8_FilterSize,
							const t_pf_PubSub_EventHandler	pf_Handler,
							void*							p_Parameter,
							const t_en_PubSubFlags			en_Flags,
							void*							pv_KeyParam )
{
	t_st_PubSubValue st_Value;
	bool Locked = false;
	bool Success = false;

	t_st_PubSubKey st_Key = {0};
	st_Key.u8_FilterSize = u8_FilterSize;
	st_Key.pf_Handler = pf_Handler;
	st_Key.pv_Param = pv_KeyParam;

	do
	{
		Success = (	pst_Channel 
					&&	( (p_Filter && u8_FilterSize) || (!p_Filter && !u8_FilterSize) )
					&&	( u8_FilterSize <= PUB_SUB_MAX_FILTER_SIZE )
					&&	pf_Handler );
		BREAK_IF( !Success );

		Success = Locked = p_Table_Lock( &pst_Channel->st_Table, TABLE_CS_ENTER_TIMEOUT );
		BREAK_IF( !Success );

		if ( !p_Table_IsNotFull( &pst_Channel->st_Table ) ) 
		{
			LOG_ERROR(	"PubSub %p capacity is insufficient (%lu items used)",
						pst_Channel,
						p_Table_GetCount( &pst_Channel->st_Table ) );
			Success = false;
			break;
		}

		if( p_Filter)
		{
			memcpy( st_Key.au8_Filter, p_Filter, u8_FilterSize );
		}
		st_Value.en_Flags = en_Flags;
		st_Value.p_Parameter = p_Parameter;

		Success = p_Table_Add(	&pst_Channel->st_Table,
								&st_Key,
								sizeof( st_Key ),
								&st_Value,
								sizeof( st_Value ) );
#ifndef VAR_ON_CMND
		LOG_BUFFER( LOG_LEVEL_TRACE, (const u8*)&st_Key, sizeof(t_st_PubSubKey),
			"PubSub: Subscribed" );
#endif //VAR_ON_CMND
	} while(false);

	if(Locked)
	{
		p_Table_Unlock( &pst_Channel->st_Table );
	}

	return Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_PubSub_Unsubscribe(	t_st_PubSub_Channel*			pst_Channel,
							const void*						p_Filter,
							u8								u8_FilterSize,
							const t_pf_PubSub_EventHandler	pf_Handler,
							void*							pv_KeyParam )
{
	bool Locked = false;
	t_st_PubSubKey st_Key = {0};
	st_Key.u8_FilterSize = u8_FilterSize;
	st_Key.pf_Handler = pf_Handler;
	st_Key.pv_Param = pv_KeyParam;

	do 
	{
		BREAK_IF(	!pst_Channel 
					||	!p_Filter
					||	!u8_FilterSize
					||	!pf_Handler );

		BREAK_IF( false == ( Locked/*intended assignment*/= p_Table_Lock( &pst_Channel->st_Table, TABLE_CS_ENTER_TIMEOUT ) ) );

		memcpy( st_Key.au8_Filter, p_Filter, u8_FilterSize );
		if ( !p_PubSub_Remove( pst_Channel, &st_Key, sizeof(st_Key) ) )
		{
			// Log error
		}
	} while (false);
	
	if(Locked)
	{
		p_Table_Unlock( &pst_Channel->st_Table );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_PubSub_Publish(	  t_st_PubSub_Channel*	pst_Channel,
						  void*					p_Filter, 
						  u8					u8_FilterSize,
						  void*					p_Message)
{
	bool b_Success = false;
	bool Locked = false;
	t_h_Iter hIter = ITER_BEGIN;
	t_h_Iter hFound = NULL;

	t_st_PubSubKey st_Key = {0};
	t_st_PubSubKvp st_EventKvp = {0};
	t_st_PubSubKvp st_Found = {0};

	memset( st_Key.au8_Filter, 0, PUB_SUB_MAX_FILTER_SIZE );

	st_Key.u8_FilterSize = u8_FilterSize;
	st_Key.pf_Handler = NULL;

	st_EventKvp.p_Key = &st_Key;
	st_EventKvp.u16_KeySize = sizeof( st_Key );
	st_EventKvp.p_Value = NULL;
	st_EventKvp.u16_ValueSize = 0;

	do 
	{
		b_Success = pst_Channel
			&& ( (p_Filter && u8_FilterSize) || (!p_Filter && !u8_FilterSize));
		BREAK_IF( !b_Success );

		b_Success = Locked = p_Table_Lock( &pst_Channel->st_Table, TABLE_CS_ENTER_TIMEOUT);
		BREAK_IF( !b_Success );
	
		if( p_Filter )
		{
			memcpy( st_Key.au8_Filter, p_Filter, u8_FilterSize );
		}

		// A loop for all items matched to the p_Filter
		do 
		{
			hFound = p_Table_Find(	&pst_Channel->st_Table, 
									&hIter,
									(const t_st_KeyValuePair*) &st_EventKvp, 
									(t_st_KeyValuePair*)&st_Found,
									(t_pf_KvpComparer)pst_Channel->pf_Comparer );
			if(	hFound
				&& st_Found.p_Key
				&& st_Found.p_Key->pf_Handler
				&& st_Found.p_Value )
			{
				// Save found key before handler call
				t_st_PubSubKey st_Key;
				void* p_Parameter = st_Found.p_Value->p_Parameter;
				t_pf_PubSub_EventHandler pf_Handler = st_Found.p_Key->pf_Handler;

				memcpy( &st_Key, st_Found.p_Key, sizeof( t_st_PubSubKey ) );

				if( !( st_Found.p_Value->en_Flags & PUB_SUB_PERSISTENT ) )
				{
					if ( !p_PubSub_Remove(	pst_Channel,
											st_Found.p_Key,
											sizeof(t_st_PubSubKey) ) )
					{
						// Log error
					}
				}

				LOG_TRACE("PubSub: calling handler %p", pf_Handler );				
				
				//Unlock table before calling handler
				p_Table_Unlock( &pst_Channel->st_Table );
				
				//Call handler
				pf_Handler( p_Message, st_Found.p_Key->pv_Param, p_Parameter );
				LOG_TRACE("PubSub: handler %p finished", pf_Handler);

				//Lock table again
				b_Success = Locked = p_Table_Lock( &pst_Channel->st_Table, TABLE_CS_ENTER_TIMEOUT);
				
				BREAK_IF( !b_Success );
				b_Success = true;
			}
		} while ( hIter );
	} while ( false );

	if( Locked )
	{
		p_Table_Unlock( &pst_Channel->st_Table );
	}

	return b_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_PubSub_Clear( t_st_PubSub_Channel* pst_Channel )
{	
	if( pst_Channel )
	{
		p_Table_Delete( &pst_Channel->st_Table );
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static bool p_PubSub_Remove(	t_st_PubSub_Channel*	pst_Channel,
								t_st_PubSubKey*			pst_Key,
								u16						u16_KeySize )
{
	t_st_PubSubValue* pst_PubSubValue;
	u32 u32_PubSubValueSize;

	if( !pst_Channel 
		|| !pst_Key
		|| !u16_KeySize )
	{
		return false;
	}

	if( p_Table_Get(	&pst_Channel->st_Table, 
						pst_Key, 
						u16_KeySize, 
						(void*) &pst_PubSubValue, 
						&u32_PubSubValueSize ) == false )
	{
		return true;
	}

#ifndef VAR_ON_CMND
	LOG_BUFFER( LOG_LEVEL_TRACE, (const u8*)pst_Key, sizeof(t_st_PubSubKey), "PubSub: Removing item %p", pst_Key);
#endif //VAR_ON_CMND

	return p_Table_Remove(	&pst_Channel->st_Table, 
							pst_Key,
							u16_KeySize );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
