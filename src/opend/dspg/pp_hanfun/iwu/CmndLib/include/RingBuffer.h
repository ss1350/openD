///////////////////////////////////////////////////////////////////////////////
///	
///	@file		RingBuffer.h
///	@brief		This file provides API of abstract overlapping Ring Buffer
///
///	@internal
///	@author		Iurov Maxim
///	@date		27/12/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "TypeDefs.h"

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define STATIC_RINGBUFFER_BUFF_SIZE( items_count, item_size ) ( ( items_count ) * ( item_size ) )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// An array of items
typedef PACK_STRUCT 
{
	void*					p_Buffer;		//!< Pointer to memory block
	u16						u16_Capacity;	//!< Max Size of Buffer
	u16						u16_Size;		//!< Current Size of Buffer
	u16						u16_ElemSize;	//!< Size of each element in Buffer
	void*					p_Head;			//!< Pointer to first valid element
	void*					p_Tail;			//!< Pointer to last element
}
t_st_RingBuffer;

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
typedef bool (*t_pf_ItemComparer)(	const void*	p_ItemA_Data,
									u32			u32_ItemA_DataSize, 
									const void*	p_ItemB_Data,
									u32			u32_ItemB_DataSize );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///	@brief		Create a Ring Buffer object
///
///	@param[in]	u16_Size		- Size of Ring Buffer
///	@param[in]	u16_ElemSize	- Size of each element in Buffer
///
/// @see		p_Queue_Close
///////////////////////////////////////////////////////////////////////////////
bool p_RingBuffer_Init( t_st_RingBuffer* pst_Buffer, u16 u16_Size, u16 u16_ElemSize, void* p_Buffer, u32 u32_BufferSize );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Destroy Ring Buffer object
///
///	@param[in]	pst_RingBuffer	- A pointer of object for destroying
///
///	@return		None
///////////////////////////////////////////////////////////////////////////////
void p_RingBuffer_Destroy( t_st_RingBuffer* pst_RingBuffer );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Push element to back of Ring Buffer
/// @note		If Buffer is full, overwrite first element
///
///	@param[in]	pst_RingBuffer	- A pointer of Ring Buffer
///	@param[in]	p_Data			- A pointer to Data
///
///	@return		None
///////////////////////////////////////////////////////////////////////////////
void p_RingBuffer_Push( t_st_RingBuffer* pst_RingBuffer, const void* p_Data );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Pop element from top of Ring Buffer
///
///	@param[in]	pst_RingBuffer	- A pointer of Message Queue
///	@param[in]	p_Data			- A pointer to Data
///
///	@return		true if success
///////////////////////////////////////////////////////////////////////////////
bool p_RingBuffer_Pop( t_st_RingBuffer* pst_RingBuffer, void* p_Data );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Clear Ring Buffer object
///
///	@param[in]	pst_RingBuffer	- A pointer to object
///
///	@return		None
///////////////////////////////////////////////////////////////////////////////
void p_RingBuffer_Clear( t_st_RingBuffer* pst_RingBuffer );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function search item in Ring Buffer using pf_Comparer
///				function
///
/// @param[in]	pst_RingBuffer		A pointer of Ring Buffer
/// @param[in]	p_ItemData			A pointer of item data to be searched
/// @param[in]	u16_ItemDataSize	Size of searching data
/// @param[in]	pf_Comparer			Specific compare function
/// @param[out]	p_FoundData			A pointer for found item
///
/// @return		true if search succeeds
///////////////////////////////////////////////////////////////////////////////
bool p_RingBuffer_FindCustom(	t_st_RingBuffer*	pst_RingBuffer,
								const void*			p_ItemData,
								u16					u16_ItemDataSize,
								t_pf_ItemComparer	pf_Comparer,
								OUT void*			p_FoundData );

///////////////////////////////////////////////////////////////////////////////
/// @brief		This function search item in Ring Buffer
///
/// @param[in]	pst_RingBuffer		A pointer of Ring Buffer
/// @param[in]	p_ItemData			A pointer of item data to be searched
/// @param[in]	u16_ItemDataSize	Size of searching data
/// @param[out]	p_FoundData			A pointer for found item
///
/// @return		true if search succeeds
///////////////////////////////////////////////////////////////////////////////
bool p_RingBuffer_Find(	t_st_RingBuffer*	pst_RingBuffer,
						const void*			p_ItemData,
						u16					u16_ItemDataSize,
						OUT void*			p_FoundData );

extern_c_end

#endif // _RING_BUFFER_H_