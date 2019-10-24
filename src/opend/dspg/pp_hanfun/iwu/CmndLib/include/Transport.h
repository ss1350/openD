#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#include "TypeDefs.h"
#include "MsgQueue.h"
#include "CmndApiPacket.h"
#include "LibInstance.h"

extern_c_begin

/// size of statically allocated message queue buffer
#define MSG_QUEUE_BUFFER_SIZE (sizeof(t_st_Msg) * MSG_QUEUE_CAPACITY)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Transport context structure
/// @note Use separated instance of st_Transport per thread
///////////////////////////////////////////////////////////////////////////////
typedef struct 
{
	u8						au8_MsgQueueBuffer[MSG_QUEUE_BUFFER_SIZE];
	t_st_MsgQueue			st_MsgQueue; // Message queue that allows to wait for desired message with timeout
	t_st_CmndLibData*		pst_CmndLibInstance;
}
t_st_CmndTransport;

///////////////////////////////////////////////////////////////////////////////
/// @brief      This is a function prototype of channel item event handler
///
/// @param[in]  p_Msg           A pointer to the message
/// @param[in]  p_KeyParameter  A pointer to the optional param 1
/// @param[in]  p_Parameter     A pointer to the optional param 2
///////////////////////////////////////////////////////////////////////////////
// WARNING: function arguments must be compatible with t_pf_PubSub_EventHandler
typedef void (*t_pf_CmndMsgHandler)( const t_st_hanCmndApiMsg* p_Msg, void* p_KeyParameter, void* p_Parameter );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize transport context with single static instance of CmndLib
/// @param[in]	pst_Transport pointer to transport context
///
/// @return true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_CmndTransport_Init( OUT t_st_CmndTransport* pst_Transport );

///////////////////////////////////////////////////////////////////////////////
/// @brief Initialize transport context with specified instance of CmndLib
/// @param[in]	pst_Transport 	pointer to transport context
/// @param[in]	pst_Instance 	pointer to CmndLibData instance
/// @return true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_CmndTransport_InitEx( OUT t_st_CmndTransport* pst_Transport, IN t_st_CmndLibData* pst_Instance );

///////////////////////////////////////////////////////////////////////////////
/// @brief Deinitialize transport context
/// @param[in]	pst_Transport pointer to transport context
///
/// @return true if ok
///////////////////////////////////////////////////////////////////////////////
void p_CmndTransport_Deinit( t_st_CmndTransport* pst_Transport );

///////////////////////////////////////////////////////////////////////////////
/// @brief Send a message to CMND
/// @param[in]	pst_Transport pointer to transport context
///	@param[in]	pst_PacketToSend packet to send
/// @return true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_CmndTransport_Send( IN t_st_CmndTransport*	pst_Transport, IN const t_st_Packet*	pst_PacketToSend );

///////////////////////////////////////////////////////////////////////////////
/// @brief Synchronous message handling: wait for a message from CMND
/// @note CMND_SERVICE_ANY and CMND_MSG_ANY are acceptable
/// @param[in]	pst_Transport pointer to transport context
/// @param[out]	pst_MsgRecv received message
/// @param[in] 	u16_ExpServiceID expected service id
/// @param[in] 	u16_ExpCmndMsgID expected message id
/// @param[in]	TimeoutMs	timeout in milliseconds
///
/// @return true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_CmndTransport_Wait(
	IN t_st_CmndTransport*	pst_Transport,
	OUT t_st_Msg*			pst_MsgRecv,
	t_en_hanCmndServiceId	en_ExpServiceID,
	t_hanCmndMsgId  		en_ExpCmndMsgID,
	u32						TimeoutMs );
///////////////////////////////////////////////////////////////////////////////
/// @brief Asynchronous message handling: subscribes for a message.
/// Once message has been received, a callback pfn_Handler will be called.
/// In multithreaded environment the callback will be called from PubSub thread context.
/// In singlethreaded environment the callback will be called from p_CmndLib_HandleChunk().
/// @note CMND_SERVICE_ANY and CMND_MSG_ANY are acceptable
///	@param[in]	pst_Transport pointer to transport context
/// @param[in]	serviceId service id to subscribe
/// @param[in]	messageId message id to subscribe
/// @param[in]	pfn_Handler handler to call when message will be received
/// @param[in]	pv_Parameter parameter to passing into handler
///	@param[in]	en_Flags subscribe type (persistent or not)
///
/// @return true if ok
///////////////////////////////////////////////////////////////////////////////
bool p_CmndTransport_Subscribe(
	IN t_st_CmndTransport*		pst_Transport,
	t_en_hanCmndServiceId		serviceId,
	t_hanCmndMsgId				messageId,
	t_pf_CmndMsgHandler         pfn_Handler,
	void*						pv_Parameter,
	t_en_PubSubFlags			en_Flags );

///////////////////////////////////////////////////////////////////////////////
/// @brief	Asynchronous message handling: subscribes for a message
/// With p_CmndTransport_SubscribeEx it can be valid to subscribe to one message more than one time with the same handler
/// Once message has been received, a callback pfn_Handler will be called.
/// In multithreaded environment the callback will be called from PubSub thread context.
/// In singlethreaded environment the callback will be called from p_CmndLib_HandleChunk().
///	@param[in]	pst_Transport pointer to transport context
/// @param[in]	serviceId service id to subscribe
/// @param[in]	messageId message id to subscribe
/// @param[in]	pfn_Handler handler to call when message will be received
/// @param[in]	p_Parameter parameter to passing into handler
///	@param[in]	en_Flags subscribe type (persistent or not)
/// @param[in]	pv_KeyParameter it is like user parameter, but it used also like a part of key
///
/// @return true if success
///////////////////////////////////////////////////////////////////////////////
bool p_CmndTransport_SubscribeEx(
    IN t_st_CmndTransport*      pst_Transport,
    t_en_hanCmndServiceId       serviceId,
    t_hanCmndMsgId              messageId,
    t_pf_CmndMsgHandler         pfn_Handler,
    void*                       p_Parameter,
    t_en_PubSubFlags            en_Flags,
    void*                       pv_KeyParameter );

///////////////////////////////////////////////////////////////////////////////
/// @brief Stop receiving callbacks for specified messages
///	@param[in]	pst_Transport pointer to transport context
/// @param[in]	serviceId service id to subscribe
/// @param[in]	messageId message id to subscribe
/// @param[in]	pfn_Handler handler to call when message will be received
///
///////////////////////////////////////////////////////////////////////////////
void p_CmndTransport_Unsubscribe(
	IN t_st_CmndTransport*		pst_Transport,
	t_en_hanCmndServiceId		serviceId,
	t_hanCmndMsgId				messageId,
	t_pf_CmndMsgHandler     	pfn_Handler );

///////////////////////////////////////////////////////////////////////////////
/// @brief Stop receiving EX callbacks for specified messages
/// @note use it to unsubscribe after p_CmndTransport_SubscribeEx
///	@param[in]	pst_Transport pointer to transport context
/// @param[in]	serviceId service id to subscribe
/// @param[in]	messageId message id to subscribe
/// @param[in]	pfn_Handler handler to call when message will be received
///
///////////////////////////////////////////////////////////////////////////////
void p_CmndTransport_UnsubscribeEx(
    IN t_st_CmndTransport*      pst_Transport,
    t_en_hanCmndServiceId       serviceId,
    t_hanCmndMsgId              messageId,
    t_pf_CmndMsgHandler         pfn_Handler,
    void*                       pv_KeyParameter );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_CmndTransport_Dispatch( void* pv_CmndTransport );


extern_c_end

#endif  //_TRANSPORT_H
