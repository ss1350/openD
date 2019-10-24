#include "CmndHibernationMng.h"
#include "Transport.h"
#include "CmndPubSub.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void p_OnMsg_Handler( const void* p_Msg, void* p_KeyParameter, void* p_Parameter );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_CmndHibernationMng_EnsureAwake( t_st_CmndHibernationMng* pst_Manager )
{
	// breakable sequence
	do
	{
		BREAK_IF(pst_Manager->en_HbrState == CMND_HBR_STATE_AWAKE);
		BREAK_IF(pst_Manager->pf_WakeupCmnd == NULL);
		BREAK_IF(!pst_Manager->pf_WakeupCmnd());
		pst_Manager->en_HbrState = CMND_HBR_STATE_AWAKE;
	}
	while(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_CmndHibernationMng_Configure(
		t_st_CmndHibernationMng* 	pst_HbrMng,
		t_pf_WakeupCmnd 			pf_WakeupCmnd,
		t_pf_OnCmndSleep			pf_OnCmndSleep )
{
	pst_HbrMng->en_HbrState = CMND_HBR_STATE_SLEEP;
	pst_HbrMng->pf_OnCmndSleep = pf_OnCmndSleep;
	if(pf_OnCmndSleep)
	{
		t_st_CmndTransport st_Transport = {0};
		p_CmndTransport_Init(&st_Transport);

		p_CmndTransport_Subscribe(
				&st_Transport,
				CMND_SERVICE_ID_UNKNOWN,		// any
				CMND_MSG_ANY,
				(t_pf_CmndMsgHandler) p_OnMsg_Handler,
				pst_HbrMng,
				PUB_SUB_PERSISTENT);
		p_CmndTransport_Deinit(&st_Transport);
	}
	pst_HbrMng->pf_WakeupCmnd = pf_WakeupCmnd;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_OnMsg_Handler( const void* p_Msg, void* p_KeyParameter, void* p_Parameter )
{
	t_st_CmndHibernationMng* pst_HibernationMng = (t_st_CmndHibernationMng*) p_Parameter;
	t_st_Msg* pst_Msg = (t_st_Msg*) p_Msg;

	if( (pst_Msg->serviceId == CMND_SERVICE_ID_SLEEP) &&
			( pst_Msg->messageId == CMND_MSG_SLEEP_ENTER_SLEEP_REQ ))
	{
		pst_HibernationMng->en_HbrState = CMND_HBR_STATE_SLEEP;
		if(pst_HibernationMng->pf_OnCmndSleep)
		{
			// call user callback
			pst_HibernationMng->pf_OnCmndSleep();
		}
	}
	else
	{
		// got any another message from CMND. It means CMND doesn't sleep.
		pst_HibernationMng->en_HbrState = CMND_HBR_STATE_AWAKE;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

