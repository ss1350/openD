#ifndef _LIB_INSTANCE_H
#define _LIB_INSTANCE_H

#include "TypeDefs.h"
#include "MsgQueue.h"
#include "PubSub.h"
#include "CmndApiPacket.h"
#include "PortHandle.h"

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define RX_PUBSUB_BUFFER_SIZE	(STATIC_PUBSUB_BUFF_SIZE( CMNDLIB_SUBSCRIBERS_CAPACITY ))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef int (*tpf_IoWrite)( t_Handle h_Port, const void* buffer, size_t bufferSize );
typedef int (*tpf_IoRead) ( t_Handle h_Port, void* buffer, size_t bufferSize );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef MULTI_THREAD
	#include "CmndLibData.h"
	#define MSG_QUEUE_CAPACITY		(1)
#else
	#include "CmndLibMT_Config.h"
	#include "CmndLibDataMT.h"
	#define MSG_QUEUE_CAPACITY		(3)
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/// For internal use
bool p_CmndLib_InitTransport( OUT t_st_CmndLibData* pst_Instance, t_Handle h_Port );
void p_CmndLib_DeinitTransport( INOUT t_st_CmndLibData* pst_Instance );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// Initializes static single instance
bool p_CmndLib_Init( t_Handle hUart );

/// Returns pointer to static single instance
t_st_CmndLibData* p_CmndLib_Instance( void );

/// Initializes specified instance
bool p_CmndLib_InitEx( t_st_CmndLibData* pst_Instance, t_Handle hUart );

/// Set writer function to single static instance of CmndLib
void p_CmndLib_SetWriter( tpf_IoWrite pf_WriterFunc );

/// Set writer function to specified instance of CmndLib
void p_CmndLib_SetWriterEx( INOUT t_st_CmndLibData* pst_Instance, tpf_IoWrite pf_WriterFunc );

/// Set reader function to single static instance of CmndLib.
/// Not used if p_CmndLib_HandleChunk() is called manually.
/// Must be set if p_CmndLib_RxTaskProc() is used.
void p_CmndLib_SetReader( tpf_IoRead pf_ReaderFunc );

/// Set reader function to specified instance of CmndLib.
/// Not used if p_CmndLib_HandleChunkEx() is called manually.
void p_CmndLib_SetReaderEx( INOUT t_st_CmndLibData* pst_Instance, tpf_IoRead pf_ReaderFunc );

/// Handles incoming data using single static instance of CmndLib.
/// This should not be called when RxTask is used
void p_CmndLib_HandleChunk( const u8* pu8_Chunk, int length );

/// Handles incoming data using specified instance of CmndLib.
/// This function should not be called directly when p_CmndLib_RxTaskProc() is used
void p_CmndLib_HandleChunkEx( INOUT t_st_CmndLibData* pst_Instance, const u8* pu8_Chunk, int length );

// Calls user Subscribers
void p_CmndLib_HandlePubSub( void );
void p_CmndLib_HandlePubSubEx( t_st_CmndLibData* pst_Instance );

/// Support cmnd hibernation
void p_CmndLib_SupportCmndHibernation( t_st_CmndLibData* pst_Instance, t_pf_WakeupCmnd pf_WakeupCmnd, t_pf_OnCmndSleep pf_OnSleep );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_end

#endif  //_LIB_INSTANCE_H
