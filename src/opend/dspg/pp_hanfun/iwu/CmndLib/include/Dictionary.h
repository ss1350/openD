//////////////////////////////////////////////////////////////////////////////
///	
///	@file		Dictionary.h
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

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "StaticList.h"
#include "TypeDefs.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define STATIC_DICT_BUF_SIZE(KeySize, ValSize, ItemsCount) (ItemsCount*(KeySize + sizeof(u16) + ValSize + sizeof(t_st_StaticListItem)))

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Declare an iterator handler
DECLARE_OPAQUE(t_h_Iter);

// Dictionary Key Value pair
typedef PACK_STRUCT 
{
	u8* p_Key;			//!< A constant pointer to allocated non-mutable key
	u16 u16_KeySize;	//!< A size of key
	u8* p_Value;		//!< A constant pointer to allocated mutable value
	u16 u16_ValueSize;	//!< A size of Value
} 
t_st_KeyValuePair;

typedef void *p_DictItemData;

///////////////////////////////////////////////////////////////////////////////
/// @brief		Function prototype for Dictionary Remove callback.
///
/// @param[in]	pst_Pair	a pointer to the key value pair
///////////////////////////////////////////////////////////////////////////////
typedef void (*t_pf_OnDictionaryItemRemove)( t_st_KeyValuePair* pst_Pair );

///////////////////////////////////////////////////////////////////////////////
/// @brief		Function Prototype for Dictionary comparer function
///
/// @param[in]	p_ItemData			a pointer to the testable key value pair
/// @param[in]	u32_ItemSize		a size of testable key_value pair
/// @param[in]	pst_Needle			a pointer to the sought key value pair	
/// @param[in]	u32_NeedleSize		a size of sought key value pair
///////////////////////////////////////////////////////////////////////////////
typedef bool (  *t_pf_KvpComparer )(	const p_DictItemData		p_ItemData,
										u32							u32_ItemSize,
										const t_st_KeyValuePair*	pst_Needle,
										u32							u32_NeedleSize );

// This function is default comparer of two keys

bool p_Dictionary_DefaultKeyComparer(			const p_DictItemData	p_Item, 
									 u32						u32_ItemSize,
									 const void*				p_Needle,
									 u32						u32_NeedleSize );

/// A Dictionary
typedef PACK_STRUCT 
{
	t_st_StaticList		st_List;			//!< A pointer to the List which stores key-value pairs
	u32					u32_KeySize;		//!< A size of key for the dictionary
	t_pf_KvpComparer	pf_KeyEqComparer;	//!< A key equality comparer
	u32					u32_MaxValueSize;	//!< A size limit of value
} 
t_st_Dictionary;

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function creates new dictionary with key size <u32_KeySize> 
///				using static memory buffer.
///
/// @param[in]	pst_Dict			dictionary to init
/// @param[in]	u32_KeySize			dictionary key size
/// @param[in]	u32_MaxValueSize	value size limit
/// @param[in]	pu8_MemoryBuff		preallocated memory buffer
/// @param[in]	u32_BuffSize		size of buffer
///
/// @return		a pointer to the dictionary created. In case of failure function return NULL
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_InitStatic( t_st_Dictionary*	pst_Dict, 
							  u32				u32_KeySize, 
							  u32				u32_MaxValueSize, 
							  u8*				pu8_MemoryBuff,
							  u32				u32_BuffSize );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function creates new dictionary with key size <u32_KeySize> 
///				using heap. The dictionary uses the specified function
///				to compare custom keys.

///
/// @param[in]	u32_KeySize		dictionary key size
/// @param[in]	pf_KeyEqComparer	a pointer to function which compares two keys
///				for equality
///
/// @return		a pointer to the dictionary created. In case of failure function return NULL
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_InitCustom(	t_st_Dictionary* pst_Dict,
                                u32									u32_KeySize,
								u32									u32_MaxValueSize,
                                const t_pf_KvpComparer				pf_KeyEqComparer, 
								const t_pf_OnDictionaryItemRemove	pf_OnRemove );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function creates new dictionary with key size <u32_KeySize> 
///				using static memory buffer. The dictionary uses the specified function
///				to compare custom keys.

///
/// @param[in]	u32_KeySize		dictionary key size
/// @param[in]	pf_KeyEqComparer	a pointer to function which compares two keys
///				for equality
/// @param[in]	pu8_MemoryBuff		preallocated memory buffer
/// @param[in]	u32_MemoryBuffSize	size of buffer
///
/// @return		a pointer to the dictionary created. In case of failure function return NULL
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_InitCustomStatic( t_st_Dictionary*					pst_Dict,
									u32									u32_KeySize,
									u32									u32_MaxValueSize,
									const t_pf_KvpComparer				pf_KeyEqComparer,
									const t_pf_OnDictionaryItemRemove	pf_OnRemove,
									u8*									pu8_MemoryBuff,
									u32									u32_MemoryBuffSize );


///////////////////////////////////////////////////////////////////////////////
/// @brief		This function delete dictionary <pst_Dict>. Memory returns in heap.
///
/// @param[in]	pst_Dict			a pointer to the dictionary
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
void p_Dictionary_Delete( t_st_Dictionary* pst_Dict );

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function create new item with key <p_Key> and data <p_Data> 
///					from heap and add it to dictionary <pst_Dict>.
///					size of key must be exactly the same as dictionary key size.
///
/// @param[inout]	pst_Dict		a pointer to the dictionary
/// @param[in]		p_Key			a pointer to the item key
/// @param[in]		u32_KeySize		size of item key	
/// @param[in]		p_Data			a pointer to the item data
/// @param[in]		u32_DataSize	size of item data	
///
/// @return			on success function return true. In case of failure function return false
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_Add(	INOUT t_st_Dictionary*	pst_Dict, 
						const void*				p_Key, 
						u32						u32_KeySize, 
						const void*				p_Data, 
						u32						u32_DataSize );

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function removes item with key <p_Key> from dictionary <pst_Dict>
///					size of key must be exactly the same as dictionary key size 
///
/// @param[inout]	pst_Dict			a pointer to the dictionary
/// @param[in]		p_Key				a pointer to the item key
/// @param[in]		u32_KeySize			size of item key	
///
/// @return			on success function return true. In case of failure function return false
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_Remove(	INOUT t_st_Dictionary*	pst_Dict, 
							const void*				p_Key, 
							u32						u32_KeySize );

///////////////////////////////////////////////////////////////////////////////
/// @brief			this function updates item with key <p_Key> in dictionary <pst_Dict> 
///					by new data <p_Data>.
///					size of key must be exactly the same as size dictionary key
///					size of update data must be exactly the same as item data size  
///
/// @param[inout]	pst_Dict			a pointer to the dictionary.
/// @param[in]		p_Key				a pointer to the item key
/// @param[in]		u32_KeySize			size of item key	
/// @param[in]		p_Data				a pointer to the update data
/// @param[in]		u32_DataSize		Size of update data	
/// @param[out]		p_UpdatedValue		an address of pointer to updated dictionary item. Can be NULL.
///
/// @return			on success function return true. In case of failure function return false
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_Update(	INOUT t_st_Dictionary*	pst_Dict, 
							const void*				p_Key, 
							u32						u32_KeySize, 
							const void*				p_Data, 
							u32						u32_DataSize,
							p_DictItemData*			p_UpdatedValue );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function try to get item with key <p_Key> in dictionary <pst_Dict>. 
///				size of key must be exactly the same as dictionary key size 			
///
/// @param[in]	pst_Dict			a pointer to the dictionary.
/// @param[in]	p_Key				a pointer to the item key
/// @param[in]	u32_KeySize			size of item key	
/// @param[out]	pp_Data				a pointer to the return data	
/// @param[out]	pu32_DataSize		Size of return data	
///
/// @return		on success function return true and pointer to item data <p_Data>, 
///				item size <pu32_DataSize> 
///				In case of failure function return false, <p_Data> and <pu32_DataSize> don't 
///				changes.
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_Get(	const t_st_Dictionary*	pst_Dict, 
						const void*				p_Key, 
						u32						u32_KeySize, 
						OUT p_DictItemData*		pp_Data, 
						OUT u32*				pu32_DataSize );


///////////////////////////////////////////////////////////////////////////////
/// @brief		this function try to find item with key value pair <pst_Needle> 
///				in dictionary <pst_Dict> using iterator <ph_Iter> and key value 
///				comparer <pf_Filter>. 
///
/// @param[in]		pst_Dict		A pointer to the dictionary.
/// @param[inout]	ph_Iter			A pointer to iterator. 
///	@p								to start search from the beginning ph_Iter 
/// @p								must be ITER_BEGIN.
/// @param[in]		pf_Filter		A pointer to the return data	
/// @param[in]		pst_Needle		A size of return data		
/// @param[out]		p_FoundItem		A pointer to dict item
///
/// @return		on success this function return iterator and 
///				also key value pair in <pp_FoundItem>.
///				In case of failure function return NULL, <pp_FoundItem> don't 
///				changes.
///////////////////////////////////////////////////////////////////////////////
t_h_Iter p_Dictionary_Find(	const t_st_Dictionary*		pst_Dict, 
							INOUT t_h_Iter*				ph_Iter,
							const t_pf_KvpComparer		pf_Filter,
							const t_st_KeyValuePair*	pst_Needle,
							OUT	p_DictItemData*			p_FoundItem );

///////////////////////////////////////////////////////////////////////////////
/// @brief		this function return key size of dictionary <pst_Dict>
///
/// @param[in]	pst_Dict	a pointer to the dictionary.
///
/// @return		Key size of dictionary
///////////////////////////////////////////////////////////////////////////////
u32 p_Dictionary_GetKeySize( const t_st_Dictionary *pst_Dict );

///////////////////////////////////////////////////////////////////////////////
/// @brief		this function return dictionary <pst_Dict> count 
///
/// @param[in]	pst_Dict	a pointer to the dictionary.
///
/// @return		dictionary count
///////////////////////////////////////////////////////////////////////////////
u32 p_Dictionary_GetCount( const t_st_Dictionary *pst_Dict );

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function delete all items in the dictionary <pst_Dict>
///
/// @param[inout]	pst_Dict	A pointer to the dictionary.
///
/// @return			true if clean successed. false if it's failed
///////////////////////////////////////////////////////////////////////////////
bool p_Dictionary_Clear( INOUT t_st_Dictionary* pst_Dict );

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function set OnRemove Handler <pf_Handler> for dictionary 
///					<pst_Dict>
///
/// @param[inout]	pst_Dict			A pointer to the dictionary.
/// @param[in]		pf_Handler			A pointer to the handler.
///////////////////////////////////////////////////////////////////////////////
void p_Dictionary_SetOnRemoveHandler(	INOUT t_st_Dictionary*			pst_Dict,
										IN t_pf_OnDictionaryItemRemove	pf_Handler );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function get value pointer from item 
///
/// @param[in]	p_Item				An item that may be getted from Find/Update/Get functions
/// @param[in]	u32_KeySize			A size of key
/// @return		a pointer to value
///////////////////////////////////////////////////////////////////////////////
void* p_Dictionary_GetValuePointerFromDictItem(const p_DictItemData p_Item, u32 u32_KeySize);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function get value size pointer from item 
///
/// @param[in]	p_Item				An item that may be getted from Find/Update/Get functions
/// @param[in]	u32_KeySize			A size of key
/// @return		value size
///////////////////////////////////////////////////////////////////////////////
u16 p_Dictionary_GetValueSizePointerFromDictItem( const p_DictItemData p_Item, u32 u32_KeySize );

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function get key value pair from item 
/// @note			be careful, you can't change real value size with this structure. 
///
/// @param[in]	p_Item				An item that may be getted from Find/Update/Get functions
/// @param[in]	u32_KeySize			A size of key
///////////////////////////////////////////////////////////////////////////////
t_st_KeyValuePair p_Dictionary_GetKeyValuePairFromDictItem(const p_DictItemData p_Item, u32 u32_KeySize);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function get key value pair from item 
/// @note			be careful, you can't change real value size with this structure
///
/// @param[in]	p_Item				An item that may be getted from Find/Update/Get functions
/// @return			A pointer to key
///////////////////////////////////////////////////////////////////////////////
void* p_Dictionary_GetKeyPointerFromDictItem(const p_DictItemData p_Item);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function update value size in item
///
/// @param[inout]	p_Item				An item that may be getted from Find/Update/Get functions
/// @param[in]		u32_KeySize			A size of key
/// @param[in]		u16_ValueLength		A new size of value
///////////////////////////////////////////////////////////////////////////////
void p_Dictionary_UpdateValueSizeInDictItem( const p_DictItemData p_Item, u32 u32_KeySize, u16 u16_ValueLength );

extern_c_end

#endif // _DICTIONARY_H_
