#include "Transport.h"
#include "CmndPacketDetector.h"
#include "CmndPubSub.h"
#include "Logger.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndTransport_Init( INOUT t_st_CmndTransport* pst_Transport )
{
	return p_CmndTransport_InitEx( pst_Transport, p_CmndLib_Instance());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndTransport_Send(	IN t_st_CmndTransport*	pst_Transport,
							IN const t_st_Packet*	pst_PacketToSend )
{
	int written = 0;
	t_st_CmndLibData* pst_Instance = pst_Transport->pst_CmndLibInstance;

	p_MsgQueue_Clear( &pst_Transport->st_MsgQueue );

	p_CmndHibernationMng_EnsureAwake( &pst_Instance->st_HibernationMng );

	if( pst_Instance && pst_Instance->pf_IoWrite )
	{
		written = pst_Instance->pf_IoWrite( pst_Instance->h_Port, pst_PacketToSend->buffer, pst_PacketToSend->length );
	}
	else
	{
		LOG_ERROR( "%s: pst_Instance <%p> and pf_IoWrite <%p> should not be NULL", __func__, pst_Instance, pst_Instance->pf_IoWrite );
	}
	return ( written == pst_PacketToSend->length );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndTransport_Wait(
	IN t_st_CmndTransport*	pst_TransportObj,
	OUT t_st_Msg*			pst_MsgRecv,
	t_en_hanCmndServiceId   en_ExpServiceID,
	t_hanCmndMsgId      	en_ExpCmndMsgID,
	u32						TimeoutMs )
{
	bool ok = p_MsgQueue_Wait( &pst_TransportObj->st_MsgQueue, (u16)en_ExpServiceID, (u8)en_ExpCmndMsgID, pst_MsgRecv, TimeoutMs );
	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndTransport_Subscribe(
	IN t_st_CmndTransport*		pst_Transport,
	t_en_hanCmndServiceId		serviceId,
	t_hanCmndMsgId				messageId,
	t_pf_CmndMsgHandler         pfn_Handler,
	void*						p_Parameter,
	t_en_PubSubFlags			en_Flags )
{

	return p_CmndPubSub_Subscribe(
		&pst_Transport->pst_CmndLibInstance->st_RxPubSub,
		serviceId,
		messageId,
		(t_pf_PubSub_EventHandler)pfn_Handler,
		p_Parameter,
		en_Flags );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndTransport_SubscribeEx(
    IN t_st_CmndTransport*      pst_Transport,
    t_en_hanCmndServiceId       serviceId,
    t_hanCmndMsgId              messageId,
    t_pf_CmndMsgHandler         pfn_Handler,
    void*                       p_Parameter,
    t_en_PubSubFlags            en_Flags,
    void*                       pv_KeyParameter )
{
    return p_CmndPubSub_SubscribeEx(
        &pst_Transport->pst_CmndLibInstance->st_RxPubSub,
        serviceId,
        messageId,
        (t_pf_PubSub_EventHandler)pfn_Handler,
        p_Parameter,
        en_Flags,
        pv_KeyParameter );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_CmndTransport_Unsubscribe(
	IN t_st_CmndTransport*		pst_Transport,
	t_en_hanCmndServiceId		serviceId,
	t_hanCmndMsgId				messageId,
	t_pf_CmndMsgHandler	pfn_Handler )
{
	p_CmndPubSub_Unsubscribe(
		&pst_Transport->pst_CmndLibInstance->st_RxPubSub,
		serviceId,
		messageId,
		(t_pf_PubSub_EventHandler)pfn_Handler );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_CmndTransport_UnsubscribeEx(
    IN t_st_CmndTransport*      pst_Transport,
    t_en_hanCmndServiceId       serviceId,
    t_hanCmndMsgId              messageId,
    t_pf_CmndMsgHandler         pfn_Handler,
    void*                       pv_KeyParameter )
{
    p_CmndPubSub_UnsubscribeEx(
        &pst_Transport->pst_CmndLibInstance->st_RxPubSub,
        serviceId,
        messageId,
        (t_pf_PubSub_EventHandler)pfn_Handler,
        pv_KeyParameter );
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#ifndef MULTI_THREAD

//static void p_CmndTransport_Dispatch( void* pv_CmndTransport );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_CmndTransport_InitEx(
	 INOUT t_st_CmndTransport* pst_Transport,
	 IN t_st_CmndLibData* pst_Instance )
{
	pst_Transport->pst_CmndLibInstance = pst_Instance;
	// Initialize message queue for Wait
	return p_MsgQueue_Init( &pst_Transport->st_MsgQueue, MSG_QUEUE_CAPACITY, pst_Transport->au8_MsgQueueBuffer, sizeof(pst_Transport->au8_MsgQueueBuffer), p_CmndTransport_Dispatch, pst_Transport );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_CmndTransport_Deinit( t_st_CmndTransport* pst_Transport )
{
	ARGUSED( pst_Transport );
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Called by MsgQueue as a handler
// Dispatch Cmnd Messages: read from Io Port, accumulate incoming message
void p_CmndTransport_Dispatch( void* pv_CmndTransport )
{
	t_st_CmndTransport* pst_Transport = (t_st_CmndTransport*)pv_CmndTransport;
	t_st_CmndLibData* pst_Instance = pst_Transport->pst_CmndLibInstance;

	// Context to receive data
	static t_stReceiveData recvData = {(t_en_CmndApi_DetectCode)0};
	static t_st_Msg rcvdmsg;

	bool haveMessage = false;
	char chunk[1];

	int length = pst_Instance->pf_IoRead( pst_Instance->h_Port, chunk, sizeof(chunk) );
	int i=0;

	for(i=0; i<length; ++i)
	{
		haveMessage = p_hanCmndApi_HandleByte(&recvData, chunk[i], &rcvdmsg);

		if(haveMessage)
		{
			//p_CmndMsgLog_PrintRxMsg(&rcvdmsg);
			p_CmndPubSub_Publish(&pst_Instance->st_RxPubSub, &rcvdmsg);
			p_MsgQueue_Push( &pst_Transport->st_MsgQueue, &rcvdmsg );
		}
	}
}

#endif // #ifndef MULTI_THREAD
