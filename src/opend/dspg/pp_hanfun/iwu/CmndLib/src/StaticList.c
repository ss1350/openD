//////////////////////////////////////////////////////////////////////////////
///	
///	@file		refStaticList.h
///	@brief		This file provides API of abstract static List
///
///	@internal
///	@author		Strunin Andrey
///	@date		09/02/2017
///	@version	version 1.0
///
///	Copyright (C) 2017 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "StaticList.h"
#include "Logger.h"
#include <string.h> //memcpy

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void* p_StaticList_GetElementAddr( t_st_StaticList* pst_List, u32 u32_Index );
static t_st_StaticListItem* p_StaticList_GetFirstFree(t_st_StaticList* pst_List);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_StaticList_Init( t_st_StaticList* pst_StaticList, u32 u32_MaxItemSize, void* p_MemoryPool, u32 u32_MemPoolSize )
{
	if( pst_StaticList == NULL )
	{
		return false;
	}
	pst_StaticList->pu8_MemoryPool = (u8*) p_MemoryPool;
	pst_StaticList->u32_MaxItem_Size = u32_MaxItemSize;
	pst_StaticList->u16_NumOfElements = u32_MemPoolSize / ( (sizeof(t_st_StaticListItem) ) + u32_MaxItemSize ) ;
	pst_StaticList->u16_CurrentItemIndex = 0;
	pst_StaticList->pst_First = NULL;
	pst_StaticList->pst_Last = NULL;
	pst_StaticList->u16_Count = 0;
	pst_StaticList->pf_OnRemove = NULL;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_StaticList_Delete( t_st_StaticList* pst_List )
{
	if (pst_List)
	{
		pst_List->u16_Count = 0;
		if(pst_List->pst_First)
		{
			pst_List->pst_First = NULL;
		}
		if(pst_List->pst_Last)
		{
			pst_List->pst_Last = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_StaticList_Append( t_st_StaticList*	pst_List,
						  const void*		p_ItemData,
						  u32				u32_ItemSize )
{
	t_st_StaticListItem*	pst_Item = NULL;

	if (	!pst_List
		 || !u32_ItemSize
		 || ( u32_ItemSize > ( pst_List->u32_MaxItem_Size + sizeof(t_st_StaticListItem) ) )
		 || ( pst_List->u16_Count >= pst_List->u16_NumOfElements ) )
	{
		return false;
	}

	pst_Item = (t_st_StaticListItem*) p_StaticList_GetFirstFree( pst_List );
	if( pst_Item == NULL )
	{
		return false;
	}
	
	pst_List->u16_CurrentItemIndex++;

	if (p_ItemData)
	{
		memcpy( GET_DATA_BY_SLIST_ITEM( pst_Item ), p_ItemData, u32_ItemSize );
	}
#ifndef VAR_ON_CMND
	LOG_BUFFER( LOG_LEVEL_TRACE, GET_DATA_BY_SLIST_ITEM( pst_Item ), u32_ItemSize,
				"List: added item %p", pst_Item );
#endif //VAR_ON_CMND
	pst_Item->b_IsNotEmpty = true;

	if (!pst_List->pst_First
		 && !pst_List->pst_Last)
	{
		pst_Item->pst_Next = 0;
		pst_Item->pst_Prev = 0;

		pst_List->pst_First = pst_Item;
		pst_List->pst_Last = pst_Item;
		pst_List->u16_Count = 0;
	}
	else if (pst_List->pst_First
			  &&	pst_List->pst_Last)
	{
		pst_Item->pst_Next = 0;
		pst_Item->pst_Prev = pst_List->pst_Last;
		pst_List->pst_Last->pst_Next = pst_Item;
		pst_List->pst_Last = pst_Item;
	}

	pst_Item->u32_Size = u32_ItemSize;
	pst_List->u16_Count++;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_StaticList_Clear( t_st_StaticList* pst_List )
{
	if( pst_List )
	{
		u32 u32_Index;
		u32 u32_ItemSize = ( sizeof( t_st_StaticListItem ) ) + pst_List->u32_MaxItem_Size;

		for( u32_Index = 0; u32_Index < pst_List->u16_NumOfElements; u32_Index++ )
		{
			
			void* pst_Item = p_StaticList_GetElementAddr( pst_List, u32_Index );
			memset( pst_Item, 0, u32_ItemSize );
		}
		pst_List->u16_CurrentItemIndex = 0;
		pst_List->pst_First = NULL;
		pst_List->pst_Last = NULL;
		pst_List->u16_Count = 0;
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_StaticList_SetOnRemoveHandler( t_st_StaticList*		pst_List,
									  t_pf_OnListItemRemove	pf_OnRemove )
{
	pst_List->pf_OnRemove = pf_OnRemove;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_st_StaticListItem* p_StaticList_GetFirst( const t_st_StaticList* pst_List )
{
	if( pst_List )
	{
		return pst_List->pst_First;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_st_StaticListItem* p_StaticList_Find( const t_st_StaticList*	pst_List,
										t_st_StaticListItem**	ppst_Iter,
										const void*				p_ItemData,
										u32						u32_ItemDataSize )
{
	t_st_StaticListItem*	p_FoundItem;
	void*					p_FoundItemData		= NULL;
	u32 					u32_FoundItemSize	= 0;

	p_FoundItem = p_StaticList_FindCustom( pst_List,
											ppst_Iter,
											p_ItemData,
											u32_ItemDataSize,
											p_List_DefaultComparer,
											&p_FoundItemData,
											&u32_FoundItemSize );

	return p_FoundItem;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_st_StaticListItem* p_StaticList_FindCustom(	  const t_st_StaticList*	pst_List,
												  t_st_StaticListItem**		ppst_Iter,
												  const void*				p_ItemData,
												  u32						u32_ItemDataSize,
												  t_pf_ItemComparer			pf_Comparer,
												  OUT void**				pp_FoundData,
												  OUT u32*					u32_FoundDataSize )
{
	if (	!pst_List
		 || !p_ItemData
		 || !u32_ItemDataSize
		 || (u32_ItemDataSize > ( pst_List->u32_MaxItem_Size + sizeof(t_st_StaticListItem)))
		 || !pf_Comparer
		 || !ppst_Iter)
	{
		if (ppst_Iter)
		{
			*ppst_Iter = NULL;
		}
		return NULL;
	}

	if (*ppst_Iter == ITER_BEGIN)
	{
		*ppst_Iter = p_StaticList_GetFirst( pst_List );
	}

	while (*ppst_Iter)
	{
		u8* pu8_CmpItemData = GET_DATA_BY_SLIST_ITEM( *ppst_Iter );
		if (pf_Comparer( pu8_CmpItemData, ( *ppst_Iter )->u32_Size,
						 p_ItemData, u32_ItemDataSize ))
		{
			t_st_StaticListItem* pst_FoundItem = *ppst_Iter;
			*pp_FoundData = GET_DATA_BY_SLIST_ITEM( pst_FoundItem );
			*u32_FoundDataSize = pst_FoundItem->u32_Size;

			*ppst_Iter = p_StaticList_GetNext( *ppst_Iter );
			return pst_FoundItem;
		}

		( *ppst_Iter ) = p_StaticList_GetNext( *ppst_Iter );
	}

	*pp_FoundData = NULL;
	*u32_FoundDataSize = 0;
	*ppst_Iter = NULL;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_st_StaticListItem* p_StaticList_GetNext( const t_st_StaticListItem* pst_Item )
{
	if( pst_Item )
	{
		return pst_Item->pst_Next;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

t_st_StaticListItem* p_StaticList_GetPrev( const t_st_StaticListItem* pst_Item )
{
	if (pst_Item)
	{
		return pst_Item->pst_Prev;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u32 p_StaticList_GetCount( const t_st_StaticList* pst_List )
{
	if( pst_List )
	{
		return pst_List->u16_Count;
	}
	return (u32)-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This function remove list item 
void p_StaticList_Remove(	t_st_StaticList* pst_List,
							t_st_StaticListItem* pst_Item )
{
	if (p_StaticList_Withdraw( pst_List, pst_Item ) == true)
	{
		if ( pst_List->pf_OnRemove )
		{
			pst_List->pf_OnRemove( GET_DATA_BY_SLIST_ITEM( pst_Item ));
		}

		pst_Item->b_IsNotEmpty = false;

		pst_List->u16_Count--;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_StaticList_Withdraw( t_st_StaticList* pst_List, t_st_StaticListItem* pst_Item )
{
	if (( pst_List == NULL ) || ( pst_Item == NULL ))
	{
		return false;
	}

	if (pst_Item != pst_List->pst_First
		 &&	pst_Item != pst_List->pst_Last)
	{
		if( pst_Item->pst_Prev && pst_Item->pst_Next )
		{
			pst_Item->pst_Prev->pst_Next = pst_Item->pst_Next;
			pst_Item->pst_Next->pst_Prev = pst_Item->pst_Prev;
		}
		else
		{
			return false;
		}
	}
	else if (pst_Item == pst_List->pst_First
			  &&	pst_Item == pst_List->pst_Last)
	{
		pst_List->pst_First = 0;
		pst_List->pst_Last = 0;
	}
	else if (pst_Item == pst_List->pst_First
			  &&	pst_Item != pst_List->pst_Last)
	{
		pst_List->pst_First = pst_Item->pst_Next;
		pst_Item->pst_Next->pst_Prev = 0;
	}
	else if (pst_Item != pst_List->pst_First
			  &&	pst_Item == pst_List->pst_Last)
	{
		pst_List->pst_Last = pst_Item->pst_Prev;
		pst_Item->pst_Prev->pst_Next = 0;
	}

	pst_Item->pst_Next = 0;
	pst_Item->pst_Prev = 0;
	pst_Item->b_IsNotEmpty = false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief		This is a function prototype of two items comparing
///
/// @param[in]	p_ItemA_Data			A pointer of item A.
/// @param[in]	u32_ItemA_DataSize		Size of item A data.
/// @param[in]	p_ItemB_Data			A pointer of item B.
/// @param[in]	u32_ItemB_DataSize		Size of item B data.
///
/// @return		true if two items are the same. false if they do not match
///////////////////////////////////////////////////////////////////////////////
bool p_List_DefaultComparer(	const void *p_ItemA_Data, u32 u32_ItemA_DataSize,
							const void *p_ItemB_Data, u32 u32_ItemB_DataSize )
{
	if ( u32_ItemA_DataSize <= 0
		|| u32_ItemB_DataSize <= 0
		|| ( p_ItemA_Data == NULL )
		|| ( p_ItemB_Data == NULL ) ) 
	{
		return false;
	}

	if (u32_ItemA_DataSize != u32_ItemB_DataSize)
	{
		return false;
	}

	if (!memcmp( p_ItemA_Data, p_ItemB_Data, u32_ItemA_DataSize ))
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void* p_StaticList_GetElementAddr( t_st_StaticList* pst_List, u32 u32_Index )
{
	u32 u32_ElemSize = ( sizeof( t_st_StaticListItem ) )  + pst_List->u32_MaxItem_Size;
	return ( pst_List->pu8_MemoryPool + u32_Index * u32_ElemSize );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static t_st_StaticListItem* p_StaticList_GetFirstFree(t_st_StaticList* pst_List)
{
	u32						u32_Index;
	t_st_StaticListItem*	pst_Item;

	if( pst_List->u16_CurrentItemIndex < ( pst_List->u16_NumOfElements ) )
	{
		pst_Item = (t_st_StaticListItem*) p_StaticList_GetElementAddr( pst_List, pst_List->u16_CurrentItemIndex );
		if ( !pst_Item->b_IsNotEmpty )
		{
			return pst_Item;
		}
	}
	else
	{
		// reset current index
		pst_List->u16_CurrentItemIndex = 0;
	}

	for( u32_Index = pst_List->u16_CurrentItemIndex; u32_Index < pst_List->u16_NumOfElements; u32_Index++ )
	{
		pst_Item = ( t_st_StaticListItem* ) p_StaticList_GetElementAddr( pst_List, u32_Index );
		if( !pst_Item->b_IsNotEmpty )
		{
			return pst_Item;
		}
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
