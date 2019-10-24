/*!
*   \file       tcxutil.c
*   \brief      
*   \author     DanaKronfeld
*
*   @(#)        tcxutil.c~1
*
*******************************************************************************/

#include <string.h>
#include "appstreambuffer.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: ADD BIG/ENDIAN SUPPORT

#define STREAM_BUF_OVERRUN_MASK		(0x1)
#define STREAM_BUF_UNDERRUN_MASK	(0x2)

#define SET_OVERRUN( Buf )		( (Buf)->u8_State |= STREAM_BUF_OVERRUN_MASK )
#define SET_UNDERRUN( Buf )		( (Buf)->u8_State |= STREAM_BUF_UNDERRUN_MASK )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void app_streamBuffer_CreateEmpty(	t_st_streamBuffer*	pst_streamBuffer,
									u8*					pu8_Data,
									u16					u16_BufferSize )
{
	pst_streamBuffer->pu8_Data 			= pu8_Data;
	pst_streamBuffer->u16_MaxSize		= u16_BufferSize;
	pst_streamBuffer->u16_DataSize 		= 0;	
	pst_streamBuffer->u16_HeadPointer	= 0;
	pst_streamBuffer->u8_State			= 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void app_streamBuffer_CreateWithPayload(	t_st_streamBuffer*		pst_streamBuffer,
											u8*						pu8_Data,
											u16						u16_BufferSize,
											u16						u16_DataSize )
{
	app_streamBuffer_CreateEmpty( pst_streamBuffer, pu8_Data, u16_BufferSize );
	pst_streamBuffer->u16_DataSize = u16_DataSize;	 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void app_streamBuffer_Reset( t_st_streamBuffer* pst_streamBuffer )
{	
	pst_streamBuffer->u16_HeadPointer	= 0;
	pst_streamBuffer->u8_State			= 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u16 app_streamBuffer_GetDataSize( const t_st_streamBuffer* pst_streamBuffer )
{
	// return the number of bytes which were still not read from the buffer
	// DataSize is the index of the last byte added
	// HeadPonter is the index of the last byte read
	return pst_streamBuffer->u16_DataSize - pst_streamBuffer->u16_HeadPointer;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u8*	app_streamBuffer_GetPointer( t_st_streamBuffer* pst_streamBuffer )
{
	return pst_streamBuffer->pu8_Data + pst_streamBuffer->u16_HeadPointer;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_IsEmpty( const t_st_streamBuffer*	pst_streamBuffer )
{
	return (pst_streamBuffer->u16_HeadPointer >= pst_streamBuffer->u16_DataSize) 
		? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_CheckUnderrun( const t_st_streamBuffer*	pst_streamBuffer )
{
	return ( pst_streamBuffer->u8_State & STREAM_BUF_UNDERRUN_MASK ) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_CheckOverrun( const t_st_streamBuffer* pst_streamBuffer )
{
	return ( pst_streamBuffer->u8_State & STREAM_BUF_OVERRUN_MASK ) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_AddData8( t_st_streamBuffer* pst_streamBuffer, u8 u8_Data )
{
	bool RetVal = 0;
	if ( pst_streamBuffer->u16_DataSize < pst_streamBuffer->u16_MaxSize )
	{
		pst_streamBuffer->pu8_Data[pst_streamBuffer->u16_DataSize] = u8_Data;
		pst_streamBuffer->u16_DataSize++;

		RetVal = 1;
	}
	else
	{
		SET_OVERRUN(pst_streamBuffer);
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_AddData8Array(	t_st_streamBuffer*	pst_streamBuffer, 
										const u8* 			pu8_Data,
										u16					u16_SizeInBytes )
{
	bool RetVal = 0;
	
	if ( (pst_streamBuffer->u16_DataSize + u16_SizeInBytes) <= pst_streamBuffer->u16_MaxSize )
	{
		memcpy(	pst_streamBuffer->pu8_Data + pst_streamBuffer->u16_DataSize, 
				pu8_Data, 
				u16_SizeInBytes );
		
		pst_streamBuffer->u16_DataSize += u16_SizeInBytes;
		RetVal = 1;
	}
	else
	{
		SET_OVERRUN(pst_streamBuffer);
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_AddData16( t_st_streamBuffer* pst_streamBuffer, u16 u16_Data )
{
	bool RetVal = 0;
	
	if ( (pst_streamBuffer->u16_DataSize + sizeof(u16_Data)) <= pst_streamBuffer->u16_MaxSize )
	{
		// host byte order
		memcpy(	pst_streamBuffer->pu8_Data + pst_streamBuffer->u16_DataSize, 
				&u16_Data, 
				sizeof(u16_Data) );				
		
		pst_streamBuffer->u16_DataSize += sizeof(u16_Data);
		RetVal = 1;
	}
	else
	{
		SET_OVERRUN(pst_streamBuffer);
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_AddData32( t_st_streamBuffer* pst_streamBuffer, u32 u32_Data )
{
	bool RetVal = 0;
	if ( (pst_streamBuffer->u16_DataSize + sizeof(u32_Data)) <= pst_streamBuffer->u16_MaxSize )
	{
		// host byte order
		memcpy(	pst_streamBuffer->pu8_Data + pst_streamBuffer->u16_DataSize, 
				&u32_Data, 
				sizeof(u32_Data) );				
		
		pst_streamBuffer->u16_DataSize += sizeof(u32_Data);
		RetVal = 1;
	}
	else
	{
		SET_OVERRUN(pst_streamBuffer);
	}
	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u8 app_streamBuffer_GetData8	( t_st_streamBuffer*	pst_streamBuffer )
{
	u8	u8_Data = 0;

	if ( pst_streamBuffer->u16_HeadPointer < pst_streamBuffer->u16_DataSize )
	{
		u8_Data = pst_streamBuffer->pu8_Data[pst_streamBuffer->u16_HeadPointer];		
		pst_streamBuffer->u16_HeadPointer++;
	}
	else
	{
		SET_UNDERRUN( pst_streamBuffer );
	}
	return u8_Data;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_GetData8Array(	t_st_streamBuffer*	pst_streamBuffer, 
										u8* 				pu8_Dst,
										u16 				u16_SizeInBytes )
{
	bool RetVal = 0;
	if ( (pst_streamBuffer->u16_HeadPointer + u16_SizeInBytes) <= pst_streamBuffer->u16_DataSize )
	{
		memcpy( pu8_Dst, 
				pst_streamBuffer->pu8_Data + pst_streamBuffer->u16_HeadPointer, 
				u16_SizeInBytes );
		
		pst_streamBuffer->u16_HeadPointer += u16_SizeInBytes;
		RetVal = 1;
	}
	else
	{
		SET_UNDERRUN( pst_streamBuffer );
	}

	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u16 app_streamBuffer_GetData16( t_st_streamBuffer*	pst_streamBuffer )
{
	u16 u16_Data16 = 0;
	if ( (pst_streamBuffer->u16_HeadPointer + sizeof(u16)) <= pst_streamBuffer->u16_DataSize )
	{
		memcpy( &u16_Data16, 
				pst_streamBuffer->pu8_Data + pst_streamBuffer->u16_HeadPointer, 
				sizeof(u16) );
		
		pst_streamBuffer->u16_HeadPointer += sizeof(u16);
	}
	else
	{
		SET_UNDERRUN( pst_streamBuffer );
	}
	return u16_Data16;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u32 app_streamBuffer_GetData32	( t_st_streamBuffer*	pst_streamBuffer )
{
	u32	u32_Data = 0;

	if ( (pst_streamBuffer->u16_HeadPointer + sizeof(u32)) <= pst_streamBuffer->u16_DataSize )
	{
		memcpy( &u32_Data, 
				pst_streamBuffer->pu8_Data + pst_streamBuffer->u16_HeadPointer, 
				sizeof(u32) );

		pst_streamBuffer->u16_HeadPointer += sizeof(u32);
	}
	else
	{
		SET_UNDERRUN( pst_streamBuffer );
	}
	return u32_Data;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool app_streamBuffer_SkipData8Array( t_st_streamBuffer* pst_streamBuffer, u16 u16_SizeInBytes )
{
	if ( (pst_streamBuffer->u16_HeadPointer + u16_SizeInBytes) <= pst_streamBuffer->u16_DataSize )
	{	
		pst_streamBuffer->u16_HeadPointer += u16_SizeInBytes;
		return 1;
	}
	else
	{
		SET_UNDERRUN( pst_streamBuffer );
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

u16 app_streamBuffer_GetFreeSpace( const t_st_streamBuffer*	pst_streamBuffer )
{
	return pst_streamBuffer->u16_MaxSize - pst_streamBuffer->u16_DataSize;
}


//--------[End of file]---------------------------------------------------------------------------------------------------------------------------------
