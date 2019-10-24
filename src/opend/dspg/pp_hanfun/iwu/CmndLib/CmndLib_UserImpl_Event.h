#ifndef _CMNDLIB_USERIMPL_EVENTS_H
#define _CMNDLIB_USERIMPL_EVENTS_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "TypeDefs.h"
#include "CmndLib_UserImpl.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///	@brief	Event handle type
DECLARE_OPAQUE(HEVENT);


///////////////////////////////////////////////////////////////////////////////
/// @brief      Initialize module to working with events
///
///////////////////////////////////////////////////////////////////////////////
void p_CmndLib_UserImpl_Event_Init( void );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Create an event object
///
///	@param		None
/// 
///	@note		This function shouldn't be called from an ISR context
///
///	@return		An handle to the created event object
///
/// @see		OSL_EVENT_Close
///////////////////////////////////////////////////////////////////////////////
HEVENT p_CmndLib_UserImpl_Event_Create( void );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Destroy an event object
///
///	@param[in]	hEvent		- Handle to a valid event object for destroying
/// 
///	@note		Destroyig of event on which a task is waiting is forbidden
///	@n			This function shouldn't be called from an ISR context
///
///	@return		None
///
/// @see		p_CmndLib_UserImpl_Event_Create
///////////////////////////////////////////////////////////////////////////////
void p_CmndLib_UserImpl_Event_Destroy( HEVENT hEvent );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Set (Signal) an event
///
///	@param[in]	hEvent		- Handle to a valid event object
/// 
///	@note		When calling this function, all tasks which are currently waiting for the event
///				(using p_CmndLib_UserImpl_Event_Wait) will be waken up. The state of the event after this call is
///				dependent on the parameters sent by the waiters tasks to the p_CmndLib_UserImpl_Event_Wait function:
///
///	@return		None
///
/// @see		OSL_EVENT_Test
/// @see		p_CmndLib_UserImpl_Event_Wait
///////////////////////////////////////////////////////////////////////////////
void p_CmndLib_UserImpl_Event_Set  ( HEVENT hEvent );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Wait for an event to be signaled
///
///	@param[in]	hEvent		- Handle to a valid event object
///	@param[in]	Clear		- Should the event be cleared (reset) right after it is being signaled?
///	@param[in]	TimeoutMs	- Timeout to wait for the event, OSL_TIMEOUT_INFINITE for infinity
/// 
///	@note		This function allows a task to be blocked until an event object is being signaled,
///				or a timeout is expired. In case the event was signaled before the timeout is
///				expired, the event will be cleared (reset) if we passed True in the Clear parameter
///				to this function.
///	@n			This function shouldn't be called from an ISR context
///
///	@return		True if the event is signaled, or False if the timeout expired before the event was
///				signaled
///
/// @see		p_CmndLib_UserImpl_Event_Set
/// @see		p_CmndLib_UserImpl_Event_Reset
/// @see		OSL_EVENT_Test
///////////////////////////////////////////////////////////////////////////////
bool p_CmndLib_UserImpl_Event_Wait(	HEVENT hEvent ,
						bool Clear,
						u32 TimeoutMs );

extern_c_end

#endif // _CMNDLIB_USERIMPL_EVENTS_H

