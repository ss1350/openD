//////////////////////////////////////////////////////////////////////////////
///	
///	@file		MsgQueue.c
///	@brief		This file provides API CmndApiMsg Queue
///
///	@internal
///	@author		Iurov Maxim
///	@date		20/12/2016
///	@version	version 1.0
///
///	Copyright (C) 2016 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////

#include "MsgQueue.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_MsgQueue_Init( t_st_MsgQueue* pst_MsgQueue, u16 u16_Capacity, void* p_Buffer, u32 u32_BufferSize, t_pf_IdleProc pf_IdleTasks, void* pv_IdleProcParam )
{
	pst_MsgQueue->hCritSection = p_CmndLib_UserImpl_CritSect_Create();
	pst_MsgQueue->hEvent = p_CmndLib_UserImpl_Event_Create();
	pst_MsgQueue->pf_IdleProc = pf_IdleTasks;
	pst_MsgQueue->pv_IdleTasksParam = pv_IdleProcParam;
	return p_RingBuffer_Init( &pst_MsgQueue->st_RingBuffer, u16_Capacity , sizeof(t_st_Msg), p_Buffer, u32_BufferSize );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_MsgQueue_Destroy( t_st_MsgQueue* pst_MsgQueue )
{
	if ( pst_MsgQueue )
	{
		p_RingBuffer_Clear( &pst_MsgQueue->st_RingBuffer );
		p_CmndLib_UserImpl_Event_Destroy(pst_MsgQueue->hEvent);
		p_CmndLib_UserImpl_CritSect_Destroy( pst_MsgQueue->hCritSection );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_MsgQueue_Push( t_st_MsgQueue* pst_MsgQueue, const t_st_Msg* pst_hanCmndApiMsg )
{
	if ( pst_MsgQueue )
	{
		p_CmndLib_UserImpl_CritSect_Enter( pst_MsgQueue->hCritSection, CMNDLIB_TIMEOUT_INFINITE );
		
		p_RingBuffer_Push( &pst_MsgQueue->st_RingBuffer, pst_hanCmndApiMsg );

		p_CmndLib_UserImpl_Event_Set( pst_MsgQueue->hEvent );

		p_CmndLib_UserImpl_CritSect_Leave( pst_MsgQueue->hCritSection );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_MsgQueue_Pop( t_st_MsgQueue* pst_MsgQueue, t_st_Msg* pst_hanCmndApiMsg )
{
	bool ok = false;
	if ( pst_MsgQueue )
	{
		p_CmndLib_UserImpl_CritSect_Enter( pst_MsgQueue->hCritSection, CMNDLIB_TIMEOUT_INFINITE );

		ok = p_RingBuffer_Pop( &pst_MsgQueue->st_RingBuffer, pst_hanCmndApiMsg );

		p_CmndLib_UserImpl_CritSect_Leave( pst_MsgQueue->hCritSection );
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_MsgQueue_MsgComparer(	const void* p_ItemA_Data, u32 u32_ItemA_DataSize, 
									const void* p_ItemB_Data, u32 u32_ItemB_DataSize )
{
	t_st_Msg* p_MsgA = NULL;
	t_st_Msg* p_MsgB = NULL;
	
	if(			u32_ItemA_DataSize <= 0 
			||	u32_ItemB_DataSize <= 0 
			||	( p_ItemA_Data == NULL ) 
			||	( p_ItemB_Data == NULL ) ) 
	{
		return false;
	}

	if ( u32_ItemA_DataSize != u32_ItemB_DataSize )
	{
		return false;
	}
	
	p_MsgA = STATIC_CAST(t_st_Msg*, p_ItemA_Data);
	p_MsgB = STATIC_CAST(t_st_Msg*, p_ItemB_Data);

	if ( (p_MsgA->messageId == p_MsgB->messageId) &&
		 (p_MsgA->serviceId == p_MsgB->serviceId) )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static bool p_MsgQueue_TryPop(t_st_MsgQueue* pst_MsgQueue, const t_st_Msg* pst_ExphanCmndApiMsg, OUT t_st_Msg* pst_hanCmndApiMsg)
{
	bool found = false;

	p_CmndLib_UserImpl_CritSect_Enter(pst_MsgQueue->hCritSection, CMNDLIB_TIMEOUT_INFINITE);

	if( pst_MsgQueue )
	{
		if ( p_RingBuffer_Pop( &pst_MsgQueue->st_RingBuffer, pst_hanCmndApiMsg) )
		{
			if( ( pst_ExphanCmndApiMsg->serviceId == pst_hanCmndApiMsg->serviceId )
				&&( pst_ExphanCmndApiMsg->messageId == pst_hanCmndApiMsg->messageId ) )
			{
					found = true;
			}	
		}
	}
	
	p_CmndLib_UserImpl_CritSect_Leave(pst_MsgQueue->hCritSection);

	return found;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_MsgQueue_WaitAny(t_st_MsgQueue* pst_MsgQueue, OUT t_st_Msg* pst_hanCmndApiMsg, u32 TimeoutMs)
{
	bool found = false;
	u64 u64_StartTime = p_CmndLib_UserImpl_GetTickCountMs();
	u64 u64_CurrentTime = u64_StartTime;
	p_CmndLib_UserImpl_CritSect_Enter(pst_MsgQueue->hCritSection, CMNDLIB_TIMEOUT_INFINITE);

	if( pst_MsgQueue )
	{
#ifdef MULTI_THREAD
		u64 u64_TimeDiff = 0;
		while ( false == (found = p_RingBuffer_Pop( &pst_MsgQueue->st_RingBuffer, pst_hanCmndApiMsg)) )
		{
			if ( p_CmndLib_UserImpl_Event_Wait( pst_MsgQueue->hEvent, true, TimeoutMs ) )
			{
				found = p_RingBuffer_Pop( &pst_MsgQueue->st_RingBuffer, pst_hanCmndApiMsg);

				if(found)
				{
					break;
				}
			}
			u64_CurrentTime = p_CmndLib_UserImpl_GetTickCountMs();
			u64_TimeDiff = u64_CurrentTime-u64_StartTime;
			if(TimeoutMs>u64_TimeDiff)
			{
				TimeoutMs-=(u32) u64_TimeDiff;
			}
			else
			{
				break;
			}
		}
#else
		while ( ((u64_CurrentTime - u64_StartTime) < TimeoutMs) && !found )
		{
			// do idle tasks
			if (pst_MsgQueue->pf_IdleProc)
			{
				pst_MsgQueue->pf_IdleProc( pst_MsgQueue->pv_IdleTasksParam );
			}
			found = p_RingBuffer_Pop(&pst_MsgQueue->st_RingBuffer, pst_hanCmndApiMsg);
			u64_CurrentTime = p_CmndLib_UserImpl_GetTickCountMs();
		}
#endif
	}

	p_CmndLib_UserImpl_CritSect_Leave(pst_MsgQueue->hCritSection);

	return found;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool p_MsgQueue_Wait ( t_st_MsgQueue* pst_MsgQueue, u16 u16_ExpServiceID, u8 u8_ExpCmndMsgID, t_st_Msg* pst_hanCmndApiMsg, u32 TimeoutMs )
{
	bool ok = false;
	u64 u64_StartTime = p_CmndLib_UserImpl_GetTickCountMs();
	u64 u64_CurrentTime = u64_StartTime;

	t_st_Msg st_ExpectedMsg;
	memset( &st_ExpectedMsg, 0, sizeof(t_st_Msg) );

	st_ExpectedMsg.serviceId = u16_ExpServiceID;
	st_ExpectedMsg.messageId = u8_ExpCmndMsgID;
	
	if( pst_MsgQueue )
	{
#ifdef MULTI_THREAD
		u64 u64_TimeDiff = 0;
		while ( false == ( ok = p_MsgQueue_TryPop( pst_MsgQueue, &st_ExpectedMsg, pst_hanCmndApiMsg ) ) )
		{
			if ( p_CmndLib_UserImpl_Event_Wait( pst_MsgQueue->hEvent, true, TimeoutMs ) )
			{
				ok = p_MsgQueue_TryPop( pst_MsgQueue, &st_ExpectedMsg, pst_hanCmndApiMsg );
				if(ok)
				{
					break;
				}
			}
			u64_CurrentTime = p_CmndLib_UserImpl_GetTickCountMs();
			u64_TimeDiff = u64_CurrentTime-u64_StartTime;
			if(TimeoutMs>u64_TimeDiff)
			{
				TimeoutMs-=(u32) u64_TimeDiff;
			}
			else
			{
				break;
			}
		}
#else
		while ( ((u64_CurrentTime - u64_StartTime) < TimeoutMs) && !ok )
		{
			// do idle tasks
			if (pst_MsgQueue->pf_IdleProc)
			{
				pst_MsgQueue->pf_IdleProc( pst_MsgQueue->pv_IdleTasksParam );
			}
			ok = p_MsgQueue_TryPop( pst_MsgQueue, &st_ExpectedMsg, pst_hanCmndApiMsg );
			u64_CurrentTime = p_CmndLib_UserImpl_GetTickCountMs();
		}
#endif
	}

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void p_MsgQueue_Clear( t_st_MsgQueue* pst_MsgQueue )
{
	p_RingBuffer_Clear( &pst_MsgQueue->st_RingBuffer );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
