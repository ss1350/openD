//////////////////////////////////////////////////////////////////////////////
///	
///	@file		Table.c
///	@brief		This file provides API of abstract Table
///
///	@internal
///	@author		Iurov Maxim
///	@date		25/03/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "Table.h"
#include "TypeDefs.h"
#include "Logger.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_InitStatic( t_st_Table* pst_Table, u32 u32_KeySize, u32 u32_MaxValueSize, u8* pu8_Buffer, u32 u32_BufferSize )
{
	bool		ok;

	if (!pst_Table)
	{
		return false;
	}

	memset( pst_Table, 0, sizeof( t_st_Table ) );

	pst_Table->h_CriticalSection = p_CmndLib_UserImpl_CritSect_Create();

	ok = p_Dictionary_InitStatic( &pst_Table->st_Dict, u32_KeySize, u32_MaxValueSize, pu8_Buffer, u32_BufferSize );
	if (!ok)
	{

		return false;
	}

	pst_Table->st_Dict.u32_KeySize = u32_KeySize;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_Table_Delete( t_st_Table* pst_Table )
{
	if ( !pst_Table )
	{
		return;
	}

	if ( ! p_Table_Lock(pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return;
	}

	p_Dictionary_Delete( &pst_Table->st_Dict ); 

	p_Table_Unlock( pst_Table );

	p_CmndLib_UserImpl_CritSect_Destroy( pst_Table->h_CriticalSection );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_Add(	INOUT t_st_Table*	pst_Table, 
					const void*			p_Key,
					u32					u32_KeySize,
					const void*			p_Data,
					u32					u32_DataSize )
{
	bool b_Return = true;

	if ( !pst_Table )
	{
		return false;
	}

	if ( ! p_Table_Lock(pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return false;
	}

	b_Return = p_Dictionary_Add(	&pst_Table->st_Dict, 
									p_Key,
									u32_KeySize,
									p_Data,
									u32_DataSize );
	if ( b_Return == false )
	{
		p_Table_Unlock( pst_Table );
		return b_Return;
	}

	if ( pst_Table->pf_OnAdd )
	{
		pst_Table->pf_OnAdd( pst_Table, p_Key, u32_KeySize, p_Data, u32_DataSize, pst_Table->p_OnAddParam );
	}

	p_Table_Unlock( pst_Table );

	return b_Return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_Remove(	INOUT t_st_Table*	pst_Table, 
						const void*			p_Key,
						u32					u32_KeySize )
{
	bool b_Return;
	void* p_Data;
	u32 u32_DataSize;

	if ( !pst_Table )
	{
		return false;
	}

	if ( ! p_Table_Lock(pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return false;
	}

	if ( p_Table_Get( pst_Table, p_Key, u32_KeySize, &p_Data, &u32_DataSize) == false )
	{
		p_Table_Unlock( pst_Table );
		return false;
	}

	if ( pst_Table->pf_OnRemove )
	{
		pst_Table->pf_OnRemove(	pst_Table, 
								p_Key, 
								u32_KeySize, 
								p_Data, 
								u32_DataSize,
								pst_Table->p_OnRemoveParam );
	}

	b_Return = p_Dictionary_Remove(	&pst_Table->st_Dict,
									p_Key,
									u32_KeySize );

	p_Table_Unlock( pst_Table );

	return b_Return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_Update(	INOUT t_st_Table*	pst_Table,
						const void*			p_Key,
						u32					u32_KeySize,
						const void*			p_Data,
						u32					u32_DataSize )
{
	bool b_Return;
	t_st_KeyValuePair	st_UpdatedItem;
	void* p_UpdatedItem;
	if ( !pst_Table )
	{
		return false;
	}

	if ( ! p_Table_Lock(pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return false;
	}

	b_Return = p_Dictionary_Update( &pst_Table->st_Dict,
									p_Key,
									u32_KeySize,
									p_Data,
									u32_DataSize,
									(void**) &p_UpdatedItem );
	if ( b_Return == false )
	{
		p_Table_Unlock( pst_Table );
		return b_Return;
	}

	st_UpdatedItem = p_Dictionary_GetKeyValuePairFromDictItem( p_UpdatedItem, u32_KeySize );

	if ( pst_Table->pf_OnUpdate )
	{
		pst_Table->pf_OnUpdate( pst_Table,
								st_UpdatedItem.p_Key, 
								st_UpdatedItem.u16_KeySize, 
								st_UpdatedItem.p_Value, 
								st_UpdatedItem.u16_ValueSize, 
								pst_Table->p_OnUpdateParam );
	}

	p_Table_Unlock( pst_Table );

	return b_Return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_Get(	const t_st_Table*	pst_Table,
					const void*			p_Key,
					u32					u32_KeySize,
					OUT void**			pp_Data,
					OUT u32*			pu32_DataSize )
{
	bool b_Return;

	if ( !pst_Table )
	{
		return false;
	}

	if ( ! p_Table_Lock((t_st_Table*)pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return false;
	}

	b_Return = p_Dictionary_Get(	&pst_Table->st_Dict,
									p_Key,
									u32_KeySize,
									pp_Data,
									pu32_DataSize );

	p_Table_Unlock( (t_st_Table*) pst_Table );

	return b_Return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_h_Iter p_Table_Find(	const t_st_Table*			pst_Table,
						INOUT t_h_Iter*				ph_Iter,
						const t_st_KeyValuePair*	pst_Needle,
						OUT t_st_KeyValuePair*		pst_Found,
						const t_pf_KvpComparer		pf_Comparer )
{
	t_h_Iter h_Found = NULL;
	void*	p_Found;
	if (	!pst_Table
			||	!ph_Iter
			||	!pst_Needle
			||	!pst_Found
			||	!pf_Comparer )
	{
		if( ph_Iter )
		{
			*ph_Iter = NULL;
		}
		return NULL;
	}

	if ( ! p_Table_Lock( (t_st_Table*) pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return NULL;
	}

	h_Found = p_Dictionary_Find(	&pst_Table->st_Dict,
									ph_Iter,
									pf_Comparer,
									pst_Needle,
									(void**) &p_Found );
	if( h_Found )
	{
		*pst_Found = p_Dictionary_GetKeyValuePairFromDictItem( p_Found, pst_Table->st_Dict.u32_KeySize );
	}

	
	p_Table_Unlock( (t_st_Table*) pst_Table );

	return h_Found;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u32 p_Table_GetKeySize( const t_st_Table* pst_Table )
{
	u32 u32_TableKeySize = 0;

	if ( !pst_Table )
	{
		return 0;
	}

	if ( ! p_Table_Lock( (t_st_Table*) pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return 0;
	}

	u32_TableKeySize = p_Dictionary_GetKeySize( &pst_Table->st_Dict );

	p_Table_Unlock( (t_st_Table*) pst_Table );

	return u32_TableKeySize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u32 p_Table_GetCount( const t_st_Table*	pst_Table )
{
	u32 u32_TableCount = 0;

	if ( !pst_Table )
	{
		return 0;
	}

	if ( ! p_Table_Lock( (t_st_Table*) pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return 0;
	}

	u32_TableCount = p_Dictionary_GetCount( &pst_Table->st_Dict );

	p_Table_Unlock( (t_st_Table*) pst_Table );

	return u32_TableCount;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_IsNotFull( const t_st_Table*	pst_Table )
{
	if( p_Dictionary_GetCount( &pst_Table->st_Dict ) < pst_Table->st_Dict.st_List.u16_NumOfElements )
	{
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u32 p_Table_GetCapacity( const t_st_Table*	pst_Table )
{
	return (u32) pst_Table->st_Dict.st_List.u16_NumOfElements;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_SetOnAdd(	INOUT t_st_Table*			pst_Table,
						const t_pf_TableHandler		pf_OnAdd,
						void*						p_OnAddParam )
{
	if (		!pst_Table
			||	( pf_OnAdd == NULL ) )
	{
		return false;
	}

	if ( ! p_Table_Lock(pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return false;
	}

	pst_Table->pf_OnAdd = pf_OnAdd;
	pst_Table->p_OnAddParam = p_OnAddParam;

	p_Table_Unlock( pst_Table );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_SetOnUpdate(	INOUT t_st_Table*			pst_Table,
							const t_pf_TableHandler		pf_OnUpdate,
							void*						p_OnUpdateParam)
{
	if (		!pst_Table
			||	( pf_OnUpdate == NULL ) )
	{
		return false;
	}

	if ( ! p_Table_Lock(pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return false;
	}

	pst_Table->pf_OnUpdate = pf_OnUpdate;
	pst_Table->p_OnUpdateParam = p_OnUpdateParam;

	p_Table_Unlock( pst_Table );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_SetOnRemove(	INOUT t_st_Table*			pst_Table,
							const t_pf_TableHandler		pf_OnRemove,
							void*						p_OnRemoveParam )
{
	if (		!pst_Table
			||	( pf_OnRemove == NULL ) )
	{
		return false;
	}

	if ( ! p_Table_Lock(pst_Table, TABLE_CS_ENTER_TIMEOUT ) )
	{
		return false;
	}

	pst_Table->pf_OnRemove = pf_OnRemove;
	pst_Table->p_OnRemoveParam = p_OnRemoveParam;

	p_Table_Unlock( pst_Table );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Table_Lock( INOUT t_st_Table* pst_Table, u32 u32_Timeout )
{
	if ( p_CmndLib_UserImpl_CritSect_Enter( pst_Table->h_CriticalSection, u32_Timeout ) == false )
	{
		LOG_ERROR(	"Couldn't Enter critical section of Table %p. Timeout %d ms", 
					pst_Table, 
					(int)u32_Timeout );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_Table_Unlock( INOUT t_st_Table* pst_Table )
{
	p_CmndLib_UserImpl_CritSect_Leave( pst_Table->h_CriticalSection );
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
