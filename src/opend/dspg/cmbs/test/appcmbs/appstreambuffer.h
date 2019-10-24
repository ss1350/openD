/*!
*   \file       tcxutil.h
*   \brief      
*   \author     Dana Kronfeld
*
*   @(#)        tcxutil.h~1
*
*******************************************************************************/

#ifndef CMBSSTREAMBUFFER_H
#define CMBSSTREAMBUFFER_H

#include "cmbs_api.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This structure is a Stream buffer object
// it contains a pointer to memory block and maintains pointers to head of data, 
// last position of data and the maximal size of the memory block
// The "Add" operations append data and move the u16_DataSize by the number of bytes added
// The "Get" operation returns the requested number of bytes and move the u16_DataSize pointer back
typedef struct
{
	u8*			pu8_Data;					// points to block of data
	u16			u16_DataSize;				// current size - "add" operation appends data to the end of the buffer
	u16			u16_HeadPointer;			// current point in pu8_Data, next read operation starts from it
	u16			u16_MaxSize;				// Max possible data size
	u8			u8_State;					// Underrun and Overwrite states
}
t_st_streamBuffer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// brief		Create new Stream buffer object which does not convey any payload at the moment
// 				Use this function when constructing a new stream 
//
// param[out]	pst_streamBuffer 	Pointer to stream object which should be initialized
// param[in]	pu8_Data			Pointer to allocated buffer which will be used by 
//									the Stream object to store the payload
// param[in]	u16_BufferSize		Buffer size in bytes
//
// Return:	 None
///////////////////////////////////////////////////////////////////////////////
void app_streamBuffer_CreateEmpty(	OUT t_st_streamBuffer*		pst_streamBuffer,
									IN	u8*						pu8_Data,
										u16						u16_BufferSize );

///////////////////////////////////////////////////////////////////////////////
// brief		Create new Stream buffer object using an existing payload
// 				Use this function when there's a need to parse existing array of bytes
//
// param[out]	pst_streamBuffer 	Pointer to stream object which should be initialized
// param[in]	pu8_Data			Pointer to allocated buffer which will be used by 
//									the Stream object to store the payload
// param[in]	u16_BufferSize		Size of pu8_Data
// param[in]	u16_DataSize		The length of actual data in pu8_Data
//
// note			It is possible that pu8_Data might be allocated as large buffer
//				but only a fraction of it is filled with valuable payload
//
// Return:	 None
///////////////////////////////////////////////////////////////////////////////
void app_streamBuffer_CreateWithPayload(	OUT	t_st_streamBuffer*		pst_streamBuffer,
											IN	u8*						pu8_Data,
												u16						u16_BufferSize,
												u16						u16_DataSize 		);

///////////////////////////////////////////////////////////////////////////////
// brief		Set the data cursor to the begining of the stream
//				and clear the underrun and overrun states 
//				(start using the buffer from scratch...)
///////////////////////////////////////////////////////////////////////////////
void app_streamBuffer_Reset( t_st_streamBuffer* pst_streamBuffer );


///////////////////////////////////////////////////////////////////////////////
// brief		Return the number of bytes left in the stream
///////////////////////////////////////////////////////////////////////////////
u16 app_streamBuffer_GetDataSize( const t_st_streamBuffer*	pst_streamBuffer );

///////////////////////////////////////////////////////////////////////////////
// brief		Get reference to stream buffer (the non read part)
///////////////////////////////////////////////////////////////////////////////
u8*	app_streamBuffer_GetPointer( t_st_streamBuffer*	pst_streamBuffer );


///////////////////////////////////////////////////////////////////////////////
// brief		Returns true if there's no unread data in the stream
//				Points to the part which was not read yet from the stream
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_IsEmpty( const t_st_streamBuffer* pst_streamBuffer );

///////////////////////////////////////////////////////////////////////////////
// brief		Returns true if there's a case of underrun (reading more than 
//				the size of the stream)
//				This flag is reset when app_streamBuffer_Reset() is called
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_CheckUnderrun( const t_st_streamBuffer*	pst_streamBuffer );

///////////////////////////////////////////////////////////////////////////////
// brief		Returns true if there's a case of overrun 
//				( attempt to write more than the max size of the stream internal buffer)
//				This flag is reset when app_streamBuffer_Reset() is called
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_CheckOverrun( const t_st_streamBuffer* pst_streamBuffer );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// brief		Serialize 8bit value to the stream
//				Use app_streamBuffer_CheckOverrun to check overrun
//
// return		true if serialization succeeded
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_AddData8			( t_st_streamBuffer* pst_streamBuffer, u8 u8_Data );

///////////////////////////////////////////////////////////////////////////////
// brief		Serialize array of bytes to the stream
//				Use app_streamBuffer_CheckOverrun to check overrun
//
// return		true if serialization succeeded
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_AddData8Array( 	t_st_streamBuffer* 	pst_streamBuffer, 
										const u8* 			pu8_Data, 
										u16					u16_SizeInBytes );

///////////////////////////////////////////////////////////////////////////////
// brief		Serialize 16bit value to the stream
//				Data is stored in host byte order
//				Use app_streamBuffer_CheckOverrun to check overrun
//
// return		true if serialization succeeded
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_AddData16( t_st_streamBuffer* pst_streamBuffer, u16 u16_Data );

///////////////////////////////////////////////////////////////////////////////
// brief		Serialize 32bit value to the stream
//				Data is stored in host byte order
//				Use app_streamBuffer_CheckOverrun to check overrun
//
// return		true if serialization succeeded
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_AddData32( t_st_streamBuffer* pst_streamBuffer, u32 u32_Data );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// brief		Deserialize array of bytes from the stream
//				Use app_streamBuffer_CheckUnderrun to check underrun
//
// return		true if deserialization succeeded
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_GetData8Array(	t_st_streamBuffer*	pst_streamBuffer, 
										u8* 				pu8_Dst, 
										u16 				u16_SizeInBytes );

///////////////////////////////////////////////////////////////////////////////
// brief		Deserialize 8bit value from the stream
//				Use app_streamBuffer_CheckUnderrun to check underrun
//
// return		true if deserialization succeeded
///////////////////////////////////////////////////////////////////////////////
u8 app_streamBuffer_GetData8	( t_st_streamBuffer*	pst_streamBuffer );

///////////////////////////////////////////////////////////////////////////////
// brief		Deserialize 16bit value from the stream
//				Use app_streamBuffer_CheckUnderrun to check underrun
//
// return		true if deserialization succeeded
///////////////////////////////////////////////////////////////////////////////
u16 app_streamBuffer_GetData16	( t_st_streamBuffer*	pst_streamBuffer );

///////////////////////////////////////////////////////////////////////////////
// brief		Deserialize 32bit value from the stream
//				Use app_streamBuffer_CheckUnderrun to check underrun
//
// return		true if deserialization succeeded
///////////////////////////////////////////////////////////////////////////////
u32 app_streamBuffer_GetData32	( t_st_streamBuffer*	pst_streamBuffer );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// brief		This is kind of "Deserialize" function but is does not return 
//				the data, just skips the data as specified in u16_SizeInBytes
//				Use app_streamBuffer_CheckUnderrun to check underrun
//
// return		true if skip succeeded
///////////////////////////////////////////////////////////////////////////////
bool app_streamBuffer_SkipData8Array( t_st_streamBuffer* pst_streamBuffer, u16 u16_SizeInBytes );


///////////////////////////////////////////////////////////////////////////////
// brief		Return how much bytes can be added to this stream
///////////////////////////////////////////////////////////////////////////////
u16 app_streamBuffer_GetFreeSpace( const t_st_streamBuffer*	pst_streamBuffer );

#endif // CMBS_STREAM_BUFFER_H