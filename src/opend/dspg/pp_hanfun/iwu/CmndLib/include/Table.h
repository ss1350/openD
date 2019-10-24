//////////////////////////////////////////////////////////////////////////////
///	
///	@file		Table.h
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

#ifndef _TABLE_H_
#define _TABLE_H_

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "CmndLib_UserImpl_CritSect.h"
#include "Dictionary.h"
#include "TypeDefs.h"

#define STATIC_TABLE_BUF_SIZE(KeySize, ValSize, ItemsCount) STATIC_DICT_BUF_SIZE(KeySize, ValSize, ItemsCount)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// constants
enum 
{
	TABLE_CS_ENTER_TIMEOUT = 1000, 
};

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// A table declare. definition is below
struct st_Table;
typedef struct st_Table t_st_Table;

///////////////////////////////////////////////////////////////////////////////
/// @brief		This is a function prototype of table OnAction handler
///
/// @param[in]	pst_Table			a pointer to the table
/// @param[in]	p_Key				a pointer to the item key
/// @param[in]	u32_KeySize			size of item key	
/// @param[in]	p_Data				a pointer to the item data
/// @param[in]	u32_DataSize		size of item data	
/// @param[in]	p_OnActionParam		a pointer to OnAction param.
///////////////////////////////////////////////////////////////////////////////
typedef void (*t_pf_TableHandler)(	const t_st_Table*	pst_Table,
									const void*			p_Key, 
									u32					u32_KeySize,
									const void*			p_Data,
									u32					u32_DataSize, 
									void*				p_OnActionParam );

/// A table
PACK_STRUCT st_Table
{
	t_st_Dictionary		st_Dict;			//!< A dictionary
	
	HCRITICAL_SECTION	h_CriticalSection;	//!< A critical section handle

	t_pf_TableHandler	pf_OnAdd;			//!< A pointer to the OnAdd Handler
	void*				p_OnAddParam;		//!< A pointer to the OnAdd Param

	t_pf_TableHandler	pf_OnUpdate;		//!< A pointer to the OnUpdate Handler
	void*				p_OnUpdateParam;	//!< A pointer to the OnUpdate Param
	
	t_pf_TableHandler	pf_OnRemove;		//!< A pointer to the OnRemove Handler
	void*				p_OnRemoveParam;	//!< A pointer to the OnRemove Param
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function creates a new table with key size <u32_KeySize> 
///				using heap.
///
/// @param[in]	u32_KeySize			table key size
/// @param[in]	u32_MaxValueSize	value size limit
/// @param[in]	pu8_Buffer			prealloocated buffer
/// @param[in]	u32_BufferSize		size of buffer
///
/// @return		a pointer to the table created. In case of failure the function returns NULL
///////////////////////////////////////////////////////////////////////////////
bool p_Table_InitStatic( t_st_Table* pst_Table, u32 u32_KeySize, u32 u32_MaxValueSize, u8* pu8_Buffer, u32 u32_BufferSize );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function deletes a table <pst_Table>. A memory is returned in heap.
///
/// @param[in]	pst_Table			a pointer to the table
///
/// @return		None
///////////////////////////////////////////////////////////////////////////////
void p_Table_Delete( t_st_Table* pst_Table);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function creates a new item with key <p_Key> and data <p_Data> 
///					from heap and adds it to table <pst_Table>.
///					A size of key must be exactly the same as table key size.
///
/// @param[inout]	pst_Table		a pointer to the table
/// @param[in]		p_Key			a pointer to the item key
/// @param[in]		u32_KeySize		size of item key	
/// @param[in]		p_Data			a pointer to the item data
/// @param[in]		u32_DataSize	size of item data	
///
/// @return			true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_Table_Add(	INOUT t_st_Table*	pst_Table, 
					const void*			p_Key,
					u32					u32_KeySize,
					const void*			p_Data,
					u32					u32_DataSize);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function removes item with key <p_Key> from table <pst_Table>
///					A size of key must be exactly the same as a table key size 
///
/// @param[inout]	pst_Table			a pointer to the table
/// @param[in]		p_Key				a pointer to the item key
/// @param[in]		u32_KeySize			size of item key	
///
/// @return			true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_Table_Remove(	INOUT t_st_Table*	pst_Table, 
						const void*			p_Key,
						u32					u32_KeySize);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function updates item with key <p_Key> in table <pst_Table> 
///					by new data <p_Data>.
///					A size of key must be exactly the same as size table key.
///					A size of updating data must be exactly the same as item data size  
///
/// @param[inout]	pst_Table			a pointer to the table.
/// @param[in]		p_Key				a pointer to the item key
/// @param[in]		u32_KeySize			size of item key	
/// @param[in]		p_Data				a pointer to the update data
/// @param[in]		u32_DataSize		Size of update data	
///
/// @return			true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_Table_Update(	INOUT t_st_Table*	pst_Table,
						const void*			p_Key,
						u32					u32_KeySize,
						const void*			p_Data,
						u32					u32_DataSize);

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function tries to get item with key <p_Key> in table <pst_Table>. 
///				Size of key must be exactly the same as table key size.			
///
/// @param[in]	pst_Table			a pointer to the dictionary.
/// @param[in]	p_Key				a pointer to the item key
/// @param[in]	u32_KeySize			size of item key	
/// @param[out]	p_Data				a pointer to the return data	
/// @param[out]	pu32_DataSize		Size of return data	
///
/// @return		on success function returns true and pointer to item data <p_Data>, 
///				item size <pu32_DataSize> 
///				In case of failure function returns false, <p_Data> and <pu32_DataSize> don't 
///				changes.
///////////////////////////////////////////////////////////////////////////////
bool p_Table_Get(	const t_st_Table*		pst_Table,
					const void*				p_Key,
					u32						u32_KeySize,
					OUT void**				pp_Data,
					OUT u32*				pu32_DataSize);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function tries to find item with specified key value 
///					pair <pst_Needle> starting from item specified by iterator 
///					<ph_Iter>. If <ph_Iter> is ITER_BEGIN, then the search will 
///					be started from first item.
///
/// @param[in]		pst_Table			a pointer to the table.
/// @param[inout]	ph_Iter				an iterator, can be NULL
/// @param[in]		pst_Needle			a pointer to key value pair used as filter in comparer
/// @param[out]		pst_Found			a pointer
/// @param[in]		pf_Comparer			a pointer to compare function
///
/// @return			non-NULL iterator if item found.
///					NULL if item not found.
///////////////////////////////////////////////////////////////////////////////
t_h_Iter p_Table_Find(	const t_st_Table*			pst_Table,
						INOUT t_h_Iter*				ph_Iter,
						const t_st_KeyValuePair*	pst_Needle,
						OUT t_st_KeyValuePair*		pst_Found,
						const t_pf_KvpComparer		pf_Comparer );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function returns a key size of the table <pst_Table>
///
/// @param[in]	pst_Table			a pointer to the table.
///
/// @return		table key size  
///////////////////////////////////////////////////////////////////////////////
u32 p_Table_GetKeySize( const t_st_Table* pst_Table );

///////////////////////////////////////////////////////////////////////////////
/// @brief		this function returns a table <pst_Table> items count 
///
/// @param[in]	pst_Table			a pointer to the table.
///
/// @return		table count
///////////////////////////////////////////////////////////////////////////////
u32 p_Table_GetCount( const t_st_Table*	pst_Table );

///////////////////////////////////////////////////////////////////////////////
/// @brief		check empty space existing in table
///
/// @param[in]	pst_Table			a pointer to the table.
///
/// @return		true if table has empty space to add new items
///////////////////////////////////////////////////////////////////////////////
bool p_Table_IsNotFull( const t_st_Table*	pst_Table );

///////////////////////////////////////////////////////////////////////////////
/// @brief		Get capacity of table
///
/// @param[in]	pst_Table			a pointer to the table.
///
/// @return		returns maximum number of items in table
///////////////////////////////////////////////////////////////////////////////
u32 p_Table_GetCapacity( const t_st_Table*	pst_Table );

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function sets table <pst_Table> OnAdd function pointer by 
///					handler <pf_OnAdd> also set OnAddParam by value <p_OnAddParam>  
///
/// @param[inout]	pst_Table			a pointer to the table.
/// @param[in]		pf_OnAdd			a pointer to OnAdd handle.
/// @param[in]		pst_Table			an OnAdd parameter.
///
/// @return			true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_Table_SetOnAdd(	INOUT t_st_Table*			pst_Table,
						const t_pf_TableHandler		pf_OnAdd,
						void*						p_OnAddParam);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function sets table <pst_Table> OnUpdate function pointer by 
///					handler <pf_OnUpdate> also set OnUpdateParam by value <p_OnUpdateParam>  
///
/// @param[inout]	pst_Table			a pointer to the table.
/// @param[in]		pf_OnUpdate			a pointer to OnUpdate handle.
/// @param[in]		p_OnUpdateParam		an OnUpdate parameter.
///
/// @return			true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_Table_SetOnUpdate(	INOUT t_st_Table*			pst_Table,
							const t_pf_TableHandler		pf_OnUpdate,
							void*						p_OnUpdateParam);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function sets table <pst_Table> OnRemove function pointer by 
///					handler <pf_OnRemove> also set OnRemoveParam by value <p_OnRemoveParam>  
///
/// @param[inout]	pst_Table			a pointer to the table.
/// @param[in]		pf_OnRemove			a pointer to OnRemove handle.
/// @param[in]		p_OnRemoveParam		an OnRemove parameter.
///
/// @return			true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_Table_SetOnRemove(	INOUT t_st_Table*			pst_Table,
							const t_pf_TableHandler		pf_OnRemove,
							void*						p_OnRemoveParam);

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function tries to capture table crit section <pst_Table>.
///
/// @param[inout]	pst_Table		a pointer to the table.
/// @param[in]		u32_Timeout		a crit section capture timeout
///
/// @return			true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_Table_Lock( INOUT t_st_Table* pst_Table, u32 u32_Timeout );

///////////////////////////////////////////////////////////////////////////////
/// @brief			This function leaves table <pst_Table> crit section  
///
/// @param[inout]	pst_Table	a pointer to the table.
///////////////////////////////////////////////////////////////////////////////
void p_Table_Unlock( INOUT t_st_Table* pst_Table );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_end

#endif // _TABLE_H_
