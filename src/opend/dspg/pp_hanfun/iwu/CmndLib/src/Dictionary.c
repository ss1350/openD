//////////////////////////////////////////////////////////////////////////////
///	
///	@file		Dictionary.c
///	@brief		This file provides API of abstract Dictionary
///
///	@internal
///	@author		Iurov Maxim
///	@date		24/03/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "Dictionary.h"
#include "TypeDefs.h"
#include <string.h> //memcpy
#include "Logger.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// constants
enum
{
	DICT_MAX_KEYSIZE	= 0x40,	// Max key size
	DICT_MAX_VALUESIZE	= 0xFF,	// Max data size
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


static bool p_Dictionary_AddKeyValuePairToList( t_st_Dictionary* pst_Dict, const void* p_Key, u32 u32_KeySize, const void* p_Value, u32 u32_ValueSize );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Dictionary_InitStatic( t_st_Dictionary*	pst_Dict,
							  u32				u32_KeySize,
							  u32				u32_MaxValueSize,
							  u8*				pu8_MemoryBuff,
							  u32				u32_BuffSize )
{
	return p_Dictionary_InitCustomStatic(	pst_Dict,
											u32_KeySize,
											u32_MaxValueSize,
											(t_pf_KvpComparer) p_Dictionary_DefaultKeyComparer,
											NULL,
											pu8_MemoryBuff,
											u32_BuffSize);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_Dictionary_InitCustomStatic( t_st_Dictionary*					pst_Dict,
									u32									u32_KeySize,
									u32									u32_MaxValueSize,
									const t_pf_KvpComparer				pf_Comparer,
									const t_pf_OnDictionaryItemRemove	pf_OnRemove,
									u8*									pu8_MemoryBuff,
									u32									u32_MemoryBuffSize )
{
	u32 u32_MaxListItemSize = u32_KeySize + sizeof(u16) + u32_MaxValueSize;
	if (!u32_KeySize
		 || ( u32_KeySize > DICT_MAX_KEYSIZE ))
	{
		return false;
	}

	if (!p_StaticList_Init( &pst_Dict->st_List, u32_MaxListItemSize, pu8_MemoryBuff, u32_MemoryBuffSize ))
	{
		return false;
	}

	if (pf_OnRemove)
	{
		p_StaticList_SetOnRemoveHandler( &pst_Dict->st_List,
			(t_pf_OnListItemRemove) pf_OnRemove );
	}
	else
	{
		p_StaticList_SetOnRemoveHandler( &pst_Dict->st_List,
								   NULL );
	}

	pst_Dict->u32_KeySize = u32_KeySize;
	pst_Dict->u32_MaxValueSize = u32_MaxValueSize;
	pst_Dict->pf_KeyEqComparer = pf_Comparer;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_h_Iter p_Dictionary_Find(	const t_st_Dictionary*		pst_Dict, 
							INOUT t_h_Iter*				ph_Iter,
							const t_pf_KvpComparer		pf_Filter,
							const t_st_KeyValuePair*	pst_Needle,
							OUT	  p_DictItemData*		p_FoundItem )
{
	u32					u32_FoundSize = 0;

	t_h_ListIter*		pst_Found = (t_h_ListIter*)
	p_StaticList_FindCustom(	&pst_Dict->st_List,
							(t_st_StaticListItem**)ph_Iter,
							(void*)pst_Needle,
							sizeof(t_st_KeyValuePair),
							(t_pf_ItemComparer)pf_Filter,
							OUT p_FoundItem,
							&u32_FoundSize);
	
	return (t_h_Iter)pst_Found;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This function delete dictionary <pst_Dict>. Memory returns in heap.
void p_Dictionary_Delete( t_st_Dictionary* pst_Dict )
{
	if ( !pst_Dict )
	{
		return;
	}

	p_StaticList_Delete( &pst_Dict->st_List ); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static bool p_Dictionary_AddKeyValuePairToList(t_st_Dictionary* pst_Dict, const void* p_Key, u32 u32_KeySize, const void* p_Value, u32 u32_ValueSize)
{
	bool ok;

	u8 u8_ItemData[DICT_MAX_KEYSIZE + DICT_MAX_VALUESIZE] = { 0 }; 

	u16	u16_Offset			= 0;

	memcpy( u8_ItemData, p_Key, u32_KeySize );
	u16_Offset += u32_KeySize;

	memcpy( ( u8_ItemData + u16_Offset ), &u32_ValueSize, sizeof(u16));
	u16_Offset += sizeof( u16 );

	memcpy( ( u8_ItemData + u16_Offset ) , p_Value, u32_ValueSize);
	u16_Offset += u32_ValueSize;

	ok = p_StaticList_Append( &pst_Dict->st_List,  u8_ItemData, u16_Offset);
	return ok;
}

// This function create new item with key <p_Key> and data <p_Value>
// and add it to dictionary <pst_Dict>.
bool p_Dictionary_Add(	INOUT t_st_Dictionary*	pst_Dict, 
						const void* 			p_Key, 
						u32						u32_KeySize, 
						const void*				p_Value, 
						u32						u32_ValueSize)
{
	t_h_Iter h_Found = NULL;
	t_h_Iter h_Iter = ITER_BEGIN;
	t_st_KeyValuePair st_NewKey;
	void* p_ExistingItem;

	st_NewKey.p_Key = (u8*) p_Key;
	st_NewKey.u16_KeySize = (u16) u32_KeySize;

	if (		!pst_Dict 
			||	!(u32)p_Key 
			||	!(u32)p_Value 
			||	!u32_KeySize 
			||	(u32_KeySize > DICT_MAX_KEYSIZE)
			||	!u32_ValueSize
			||	(u32_ValueSize > DICT_MAX_VALUESIZE)
			||	u32_KeySize != pst_Dict->u32_KeySize )
	{
		return false;
	}

	h_Found = p_Dictionary_Find( pst_Dict, &h_Iter, pst_Dict->pf_KeyEqComparer, &st_NewKey,  (void**) &p_ExistingItem );
	if ( h_Found ) 
	{
		// The key is already exists in the dictionary
		return false;
	}

	{
		if( p_Dictionary_AddKeyValuePairToList( pst_Dict, p_Key, u32_KeySize, p_Value, u32_ValueSize ) == false)
		{
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This function removes item with key <p_Key> from dictionary <pst_Dict>
bool p_Dictionary_Remove(	INOUT t_st_Dictionary*	pst_Dict, 
							const void*				p_Key, 
							u32						u32_KeySize )
{
	void* p_FoundItem;
	t_h_Iter h_Iter = ITER_BEGIN;
	t_h_Iter h_Found = NULL;
	t_st_KeyValuePair st_Key;
	st_Key.p_Key = (u8*) p_Key;
	st_Key.u16_KeySize = (u16) u32_KeySize;
	
	if (		!pst_Dict 
			||	!(u32)p_Key 
			||	!u32_KeySize 
			||	u32_KeySize > DICT_MAX_KEYSIZE
			||	u32_KeySize != pst_Dict->u32_KeySize )
	{
		return false;
	}

	h_Found = p_Dictionary_Find( pst_Dict, &h_Iter, pst_Dict->pf_KeyEqComparer, &st_Key, (void**) &p_FoundItem );
	if ( !h_Found ) 
	{
		// Item not found
		return false;
	}

	p_StaticList_Remove( &pst_Dict->st_List, ( t_st_StaticListItem* )h_Found );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// this function updates item with key <p_Key> in dictionary <pst_Dict> 
bool p_Dictionary_Update(	INOUT t_st_Dictionary* pst_Dict, 
							const void* p_Key, 
							u32 u32_KeySize,
							const void* p_Data, 
							u32 u32_DataSize, 
							OUT p_DictItemData* p_UpdatedValue )
{
	p_DictItemData p_FoundItem;
	bool Failed = false;
	t_h_Iter h_Iter = ITER_BEGIN;
	t_h_Iter h_Found = NULL;
	t_st_KeyValuePair st_Key;

	st_Key.p_Key = (u8*) p_Key;
	st_Key.u16_KeySize = (u16) u32_KeySize;
	
	

	if (		!pst_Dict 
			||	!(u32)p_Key 
			||	!(u32)p_Data 
			||	!u32_KeySize
			||	u32_KeySize > DICT_MAX_KEYSIZE
			||  u32_KeySize != pst_Dict->u32_KeySize
			||	!u32_DataSize 
			||	u32_DataSize > DICT_MAX_VALUESIZE)
	{
		return false;
	}

	h_Found = p_Dictionary_Find( pst_Dict, &h_Iter, pst_Dict->pf_KeyEqComparer, &st_Key, (void**) &p_FoundItem);
	if ( !h_Found ) 
	{
		// Item not found
		return false;
	}

	{
		t_st_KeyValuePair st_KeyValuePair = p_Dictionary_GetKeyValuePairFromDictItem( p_FoundItem, pst_Dict->u32_KeySize );
		// enlarge value size if required TODO
		if( st_KeyValuePair.u16_ValueSize < u32_DataSize )
		{
			if( u32_DataSize > pst_Dict->u32_MaxValueSize )
			{
				LOG_ERROR( "%s Value size more than limit!", __func__ );
				Failed = true;
			}
			else
			{
				// clear memory
				memset( st_KeyValuePair.p_Value, 0, pst_Dict->u32_MaxValueSize );
			}
		}
		
		if( !Failed )
		{
			p_Dictionary_UpdateValueSizeInDictItem( p_FoundItem, pst_Dict->u32_KeySize, (u16) u32_DataSize );
			
			memcpy( st_KeyValuePair.p_Value, p_Data, u32_DataSize );
			if( p_UpdatedValue != NULL)
			{
				*p_UpdatedValue = p_FoundItem;
			}
		}
	}

	return !Failed;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This function try to get item with key <p_Key> in dictionary <pst_Dict>. 
// on success function returns true and poiner to item data <p_Data>, 
// item size <pu32_DataSize>. In case of failure function returns false.
bool p_Dictionary_Get(	const t_st_Dictionary*	pst_Dict, 
						const void*				p_Key, 
						u32						u32_KeySize,
						OUT p_DictItemData*		pp_Data, 
						OUT u32*				pu32_DataSize )
{
	void* p_FoundItem;
	t_h_Iter h_Iter = ITER_BEGIN;
	t_h_Iter h_Found = NULL;

	t_st_KeyValuePair st_Key;
	st_Key.p_Key = (u8*) p_Key;
	st_Key.u16_KeySize = (u16) u32_KeySize;
	
	

	if (		!pst_Dict 
			||	!u32_KeySize 
			||	u32_KeySize != pst_Dict->u32_KeySize
			||  !pp_Data
			||	!pu32_DataSize)
	{
		return false;
	}

	h_Found = p_Dictionary_Find(	pst_Dict,
									&h_Iter,
									pst_Dict->pf_KeyEqComparer,
									&st_Key,
									(void**) &p_FoundItem );

	if ( !h_Found ) 
	{
		// Item not found
		return false;
	}

	*pp_Data = p_Dictionary_GetValuePointerFromDictItem( p_FoundItem, pst_Dict->u32_KeySize);
	*pu32_DataSize = (u32) p_Dictionary_GetValueSizePointerFromDictItem(p_FoundItem, pst_Dict->u32_KeySize);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// this function return dictionary <pst_Dict> key size
u32 p_Dictionary_GetKeySize( const t_st_Dictionary *pst_Dict )
{
	if ( !pst_Dict )
	{
		return 0;
	}

	return pst_Dict->u32_KeySize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//this function return dictionary <pst_Dict> count
u32 p_Dictionary_GetCount( const t_st_Dictionary* pst_Dict )
{
	return p_StaticList_GetCount( &pst_Dict->st_List );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This function delete all items in the dictionary <pst_Dict>
bool p_Dictionary_Clear( INOUT t_st_Dictionary* pst_Dict )
{
	if ( !pst_Dict )
	{
		return false;
	}

	return p_StaticList_Clear( &pst_Dict->st_List );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This function set OnRemove Handler <pf_Handler> for dictionary <pst_Dict>
void p_Dictionary_SetOnRemoveHandler(	INOUT t_st_Dictionary*			pst_Dict,
										IN t_pf_OnDictionaryItemRemove	pf_Handler )
{
	if( !pst_Dict 
		|| !pf_Handler)
	{
		return;
	}

	p_StaticList_SetOnRemoveHandler(	&pst_Dict->st_List,
								(t_pf_OnListItemRemove)pf_Handler );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void* p_Dictionary_GetValuePointerFromDictItem(const p_DictItemData p_Item, u32 u32_KeySize)
{
	return ((u8*) p_Item + u32_KeySize + sizeof( u16 ));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u16 p_Dictionary_GetValueSizePointerFromDictItem( const p_DictItemData p_Item, u32 u32_KeySize )
{
	u16 u16_ValSize;
	memcpy(&u16_ValSize,( (u8*) p_Item + u32_KeySize ), sizeof(u16) );
	return u16_ValSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_Dictionary_UpdateValueSizeInDictItem( const p_DictItemData p_Item, u32 u32_KeySize, u16 u16_ValueLength )
{
	memcpy(( (u8*) p_Item + u32_KeySize ),&u16_ValueLength, sizeof(u16) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void* p_Dictionary_GetKeyPointerFromDictItem(const p_DictItemData p_Item)
{
	return (void*) p_Item;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_st_KeyValuePair p_Dictionary_GetKeyValuePairFromDictItem(const p_DictItemData p_Item, u32 u32_KeySize)
{
	t_st_KeyValuePair st_Kvp;

	st_Kvp.p_Key = p_Dictionary_GetKeyPointerFromDictItem( p_Item );
	st_Kvp.u16_KeySize = u32_KeySize;
	st_Kvp.u16_ValueSize = p_Dictionary_GetValueSizePointerFromDictItem( p_Item, u32_KeySize );
	st_Kvp.p_Value = p_Dictionary_GetValuePointerFromDictItem( p_Item, u32_KeySize );
	return st_Kvp;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief		this function tries to find item with key <p_Key> in dictionary
///				<pst_Dict> using iterator <h_Iter> and key compare
///				function <pf_Comparer>
///				size of key must be in the range from 1 to dictionary key size.
///
/// @param[in]	p_Item				A pointer of item A.
/// @param[in]	u32_ItemSize		Size of item A data.
/// @param[in]	p_Needle			A pointer of item B.
/// @param[in]	u32_NeedleSize		Size of item B data.
///
/// @return		function return true if two items are the same, or one of them contains 
///				other item. function return false if they do not match
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_DefaultKeyComparer(			const p_DictItemData	p_Item,
												u32						u32_ItemSize,
												const void*				p_Needle,
												u32						u32_NeedleSize )
{
	t_st_KeyValuePair st_TempItem;
	const t_st_KeyValuePair* pst_Needle = (const t_st_KeyValuePair*) p_Needle;

	st_TempItem.p_Key = p_Dictionary_GetKeyPointerFromDictItem( p_Item );

	if(	( u32_NeedleSize < sizeof( t_st_KeyValuePair ) ) )
	{
		return false;
	}

	if(		!p_Item
		||	!st_TempItem.p_Key
		||	!pst_Needle
		||	!pst_Needle->p_Key ) 
	{
		return false;
	}
	return ( memcmp( st_TempItem.p_Key, pst_Needle->p_Key, pst_Needle->u16_KeySize ) == 0 );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
