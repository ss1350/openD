//////////////////////////////////////////////////////////////////////////////
///	
///	@file		RingBuffer.c
///	@brief		This file provides implementation of abstract overlapping Ring
///				Buffer
///
///	@internal
///	@author		Iurov Maxim
///	@date		27/12/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "RingBuffer.h"
#include "string.h"
#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_RingBuffer_Init( OUT t_st_RingBuffer* pst_RingBuffer, u16 u16_Size, u16 u16_ElemSize, void* p_Buffer, u32 u32_BufferSize )
{
	if( (u32)(u16_ElemSize * u16_Size) <= u32_BufferSize)
	{
		pst_RingBuffer->p_Buffer = p_Buffer;
		pst_RingBuffer->p_Head = pst_RingBuffer->p_Buffer;
		pst_RingBuffer->p_Tail = pst_RingBuffer->p_Buffer;
		pst_RingBuffer->u16_Capacity = u16_Size;
		pst_RingBuffer->u16_ElemSize = u16_ElemSize;
		pst_RingBuffer->u16_Size = 0;	
		return true;
	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_RingBuffer_Destroy( t_st_RingBuffer* pst_RingBuffer )
{
	// stub
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static size_t p_TailOffset( t_st_RingBuffer* pst_RingBuffer )
{
	return (	STATIC_CAST(size_t, pst_RingBuffer->p_Tail) - 
				STATIC_CAST(size_t, pst_RingBuffer->p_Buffer) + 
				pst_RingBuffer->u16_ElemSize ) % ( pst_RingBuffer->u16_Capacity * pst_RingBuffer->u16_ElemSize );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static size_t p_HeadOffset( t_st_RingBuffer* pst_RingBuffer )
{
	return (	STATIC_CAST(size_t, pst_RingBuffer->p_Head) - 
				STATIC_CAST(size_t, pst_RingBuffer->p_Buffer) + 
				pst_RingBuffer->u16_ElemSize ) % ( pst_RingBuffer->u16_Capacity * pst_RingBuffer->u16_ElemSize );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_RingBuffer_Push( t_st_RingBuffer* pst_RingBuffer, const void* p_Data )
{
	if ( pst_RingBuffer )
	{
		size_t NewTailOffset = 0;
		size_t NewHeadOffset = 0;
		u8* p_NewTail = NULL;

		// if overlaping occurs, move head pointer as well
		if ( (pst_RingBuffer->p_Tail == pst_RingBuffer->p_Head) && (pst_RingBuffer->u16_Size != 0) )
		{
			NewHeadOffset = p_HeadOffset(pst_RingBuffer);
			pst_RingBuffer->p_Head = STATIC_CAST(u8*, pst_RingBuffer->p_Buffer) + NewHeadOffset;
		}

		memcpy(pst_RingBuffer->p_Tail, p_Data, pst_RingBuffer->u16_ElemSize);

		NewTailOffset = p_TailOffset(pst_RingBuffer);
		p_NewTail = STATIC_CAST(u8*, pst_RingBuffer->p_Buffer) + NewTailOffset;
		pst_RingBuffer->p_Tail = p_NewTail;

		if(pst_RingBuffer->u16_Size < pst_RingBuffer->u16_Capacity)
		{
			++pst_RingBuffer->u16_Size;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_RingBuffer_Pop( t_st_RingBuffer* pst_RingBuffer, void* p_Data )
{
	bool ok = false;
	if ( pst_RingBuffer )
	{
		size_t NewHeadOffset = 0;

		if ( pst_RingBuffer->u16_Size )
		{
			memcpy( p_Data, pst_RingBuffer->p_Head, pst_RingBuffer->u16_ElemSize );

			NewHeadOffset = p_HeadOffset(pst_RingBuffer);
			pst_RingBuffer->p_Head = STATIC_CAST(u8*, pst_RingBuffer->p_Buffer) + NewHeadOffset;

			ok = true;
			--pst_RingBuffer->u16_Size;
		}
	}

	return ok;
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
static bool p_RingBuf_DefaultComparer(	const void *p_ItemA_Data, u32 u32_ItemA_DataSize, 
										const void *p_ItemB_Data, u32 u32_ItemB_DataSize)
{
	if(		u32_ItemA_DataSize <= 0 
		||	u32_ItemB_DataSize <= 0 
		||	!(u32)p_ItemA_Data 
		||	!(u32)p_ItemB_Data) 
	{
		return false;
	}

	if ( u32_ItemA_DataSize != u32_ItemB_DataSize )
	{
		return false;
	}

	if ( !memcmp( p_ItemA_Data, p_ItemB_Data, u32_ItemA_DataSize ) ) 
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_RingBuffer_FindCustom(	t_st_RingBuffer*	pst_RingBuffer,
								const void*			p_ItemData,
								u16					u16_ItemDataSize,
								t_pf_ItemComparer	pf_Comparer,
								OUT void*			p_FoundData )
{
	bool ok = false;
	if (	!pst_RingBuffer
			|| !p_ItemData
			|| !u16_ItemDataSize
			|| !pf_Comparer
			|| !pst_RingBuffer->u16_Size )
	{
		return false;
	}

	{
		u8* p_BufStart = (pst_RingBuffer->p_Head > pst_RingBuffer->p_Tail) ? pst_RingBuffer->p_Tail : 
																			 pst_RingBuffer->p_Head;
		
		u8* p_BufEnd = (pst_RingBuffer->p_Head < pst_RingBuffer->p_Tail) ?	pst_RingBuffer->p_Tail : 
																			pst_RingBuffer->p_Head;
		u8* p_BufPos = NULL;
		for ( p_BufPos = p_BufStart; p_BufPos < p_BufEnd; p_BufPos += pst_RingBuffer->u16_ElemSize )
		{
			if ( pf_Comparer(	p_BufPos, pst_RingBuffer->u16_ElemSize,
								p_ItemData, u16_ItemDataSize ) ) 
			{
				ok = true;
				memcpy(p_FoundData, p_BufPos, pst_RingBuffer->u16_ElemSize);
				memset(p_BufPos, 0, pst_RingBuffer->u16_ElemSize);
				break;
			}
		}
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_RingBuffer_Find(	t_st_RingBuffer*	pst_RingBuffer,
						const void*			p_ItemData,
						u16					u16_ItemDataSize,
						OUT void*			p_FoundData )
{
	return p_RingBuffer_FindCustom( pst_RingBuffer, 
									p_ItemData,
									u16_ItemDataSize, 
									p_RingBuf_DefaultComparer, 
									p_FoundData );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_RingBuffer_Clear( t_st_RingBuffer* pst_RingBuffer )
{
	memset( pst_RingBuffer->p_Buffer, 0, pst_RingBuffer->u16_Capacity );
	pst_RingBuffer->p_Head = pst_RingBuffer->p_Buffer;
	pst_RingBuffer->p_Tail = pst_RingBuffer->p_Buffer;
	pst_RingBuffer->u16_Size = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
