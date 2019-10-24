//////////////////////////////////////////////////////////////////////////////
///	
///	@file		StaticList.h
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

#ifndef _STATIC_LIST_H_
#define _STATIC_LIST_H_

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "TypeDefs.h"

extern_c_begin

DECLARE_OPAQUE(t_h_ListIter);

struct st_ListAddItem;
typedef struct st_ListAddItemPart t_st_ListAddItemPart;

/// An item
PACK_STRUCT st_ListAddItemPart
{
	u32		u32_DataSize;
	u8*		pu8_Data;
};

typedef bool( *t_pf_ItemComparer )( const void*	p_ItemA_Data,
								   u32			u32_ItemA_DataSize,
								   const void*	p_ItemB_Data,
								   u32			u32_ItemB_DataSize );

/// A pointer to a function which calls when list remove function is called
typedef void( *t_pf_OnListItemRemove )( void* p_ItemData );

#define ITER_BEGIN ((void*)1)
// NULL is used to indicate the end


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct st_StaticListItem;
typedef struct st_StaticListItem t_st_StaticListItem;

struct st_StaticList;
typedef struct st_StaticList t_st_StaticList;

PACK_STRUCT st_StaticListItem
{
	bool					b_IsNotEmpty;
	u32						u32_Size;	//!< A size of item data
	t_st_StaticListItem*	pst_Next;	//!< A pointer to next list item
	t_st_StaticListItem*	pst_Prev;	//!< A pointer to previous list item
};

#define GET_DATA_BY_SLIST_ITEM(p) (u8*)( ((u8*)p) + sizeof(t_st_StaticListItem))

PACK_STRUCT st_StaticList
{
	t_st_StaticListItem*	pst_First;	//!< A pointer to first list item
	t_st_StaticListItem*	pst_Last;	//!< A pointer to last list item
	u16						u16_Count;	//!< A number of items in list
	u32						u32_MaxItem_Size;
	u16						u16_NumOfElements; // Max elements that the list can contain
	u8*						pu8_MemoryPool;
	u16						u16_CurrentItemIndex;
	t_pf_OnListItemRemove	pf_OnRemove;
};

#define STATIC_LIST_BUF_SIZE(MaxValueSize, ItemsCount)	(ItemsCount*(sizeof(t_st_StaticListItem) + MaxValueSize))

// Default comparator of two items
bool p_List_DefaultComparer(	const void*	p_ItemA_Data, u32 u32_ItemA_DataSize,
							const void*	p_ItemB_Data, u32 u32_EItemB_DataSize );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_StaticList_Init( t_st_StaticList* pst_StaticList, u32 u32_MaxItemSize, void* MemoryPool, u32 u32_MemPoolSize );

void p_StaticList_Delete( t_st_StaticList* pst_List );

bool p_StaticList_Withdraw( t_st_StaticList* pst_List, t_st_StaticListItem* pst_Item );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function returns number of items in List
///
/// @param[in]	pst_List		A pointer of List.
///
/// @return		number of items in List. -1 if it's failed
///////////////////////////////////////////////////////////////////////////////
u32 p_StaticList_GetCount( const t_st_StaticList* pst_List );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function creates new item using heap and appends it in 
///				the bottom of the list. 
///
/// @param[in]	pst_List			A pointer of list receiver.
/// @param[in]	pst_ItemData		A pointer of item data.
/// @param[in]	u32_ItemSize		Size of item data
///
/// @return		true if creation and appendix success. false if it's failed
///////////////////////////////////////////////////////////////////////////////
bool p_StaticList_Append(	t_st_StaticList*	pst_List,
							const void*			pst_ItemData,
							u32					u32_ItemSize );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function delete all items in list
///
/// @param[in]	pst_List			A pointer of List.
///
/// @return		true if clean successed. false if it's failed
///////////////////////////////////////////////////////////////////////////////
bool p_StaticList_Clear( t_st_StaticList* pst_List );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function remove list item.
///
/// @param[in]	pst_List			A pointer of list to be cleaned.
/// @param[in]	pst_Item			A pointer of item to be withdrawed
///////////////////////////////////////////////////////////////////////////////
void p_StaticList_Remove(	t_st_StaticList*		pst_List,
							t_st_StaticListItem*	pst_Item );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function search item with exact match data in list.
///				Data length also must be the same.
///
/// @param[in]	pst_List			A pointer to the list.
/// @param[inout]	ppst_Iter		A address of a pointer to list Item. Used as iterator.
///									ppst_Iter cannot be NULL,
///									*ppst_Iter can be NULL.
/// @param[in]	p_ItemData			A pointer of item data
/// @param[in]	u32_ItemDataSize	Size of item data	
///
/// @return		a pointer of item found if success. NULL if search failed.
///////////////////////////////////////////////////////////////////////////////
t_st_StaticListItem* p_StaticList_Find( const t_st_StaticList*	pst_List,
										t_st_StaticListItem**	ppst_Iter,
										const void*				p_ItemData,
										u32						u32_ItemDataSize );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function search item in List using pf_Comparer function.
///
/// @param[in]	pst_List			A pointer of list
/// @param[inout]	ppst_Iter		A an address of item pointer. After successful search it points to the next item following
///									to the found item.	It's intended to be an iterator.
///									ppst_Iter cannot be NULL.
///									*ppst_Iter can be NULL, the search will start from the beginning of the list.
/// @param[in]	p_ItemData			A pointer of item data to be searched
/// @param[in]	u32_ItemDataSize	Size of searching data
/// @param[in]	pf_Comparer			Specific compare function.
/// @param[out]	pp_FoundData		An address of a pointer for found item.
/// @param[out]	u32_FoundDataSize	An address of found item size.
///
/// @return		A pointer to the current found item.
///////////////////////////////////////////////////////////////////////////////
t_st_StaticListItem* p_StaticList_FindCustom( const t_st_StaticList*	pst_List,
											  t_st_StaticListItem**		ppst_Iter,
											  const void*				p_ItemData,
											  u32						u32_ItemDataSize,
											  t_pf_ItemComparer			pf_Comparer,
											  OUT void**				pp_FoundData,
											  OUT u32*					u32_FoundDataSize );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function set OnRemove Handler <pf_OnRemove> of the list 
///				<pst_List>
///
/// @param[in]	pst_List			A pointer of List.
///
/// @return		true if clean successed. false if it's failed
///////////////////////////////////////////////////////////////////////////////
void p_StaticList_SetOnRemoveHandler(	t_st_StaticList*		pst_List,
										t_pf_OnListItemRemove	pf_OnRemove );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function gets pointer of first item in list
///
/// @param[in]	pst_List			A pointer of List.
///
/// @return		a pointer of first item if success. NULL if failed, or list is empty
///////////////////////////////////////////////////////////////////////////////
t_st_StaticListItem* p_StaticList_GetFirst( const t_st_StaticList* pst_List );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function gets pointer of next item on base item
///
/// @param[in]	pst_Item			A pointer of base item
///
/// @return		a pointer of next item if success. NULL if no next item.
///////////////////////////////////////////////////////////////////////////////
t_st_StaticListItem* p_StaticList_GetNext( const t_st_StaticListItem* pst_Item );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function gets pointer of previous item on base item
///
/// @param[in]	pst_Item			pointer of base item
///
/// @return		a pointer of previous item if success. NULL if no previous item.
///////////////////////////////////////////////////////////////////////////////
t_st_StaticListItem* p_StaticList_GetPrev( const t_st_StaticListItem* pst_Item );

extern_c_end

#endif // _STATIC_LIST_H_
