#ifndef _CMNDLIB_USERIMPL_CRITSECT_H
#define _CMNDLIB_USERIMPL_CRITSECT_H

#include "TypeDefs.h"
#include "CmndLib_UserImpl.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_begin

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief Critical Section handle
DECLARE_OPAQUE(HCRITICAL_SECTION);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///	@brief		Create a critical section object
///
///	@param		None
/// 
///	@note		This function shouldn't be called from an ISR context
///
///	@return		An handle to a critical section object
///
/// @see		OSL_CS_Close
///////////////////////////////////////////////////////////////////////////////
HCRITICAL_SECTION p_CmndLib_UserImpl_CritSect_Create( void );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Destroy a critical section object
///
///	@param[in]	hCriticalSection	- An handle to a valid critical section object
/// 
///	@note		Destroying a critical section which is taken by a task is forbidden
///	@n			This function shouldn't be called from an ISR context
///
///	@return		None
///
/// @see		p_CmndLib_UserImpl_CritSect_Create
///////////////////////////////////////////////////////////////////////////////
void p_CmndLib_UserImpl_CritSect_Destroy( HCRITICAL_SECTION hCriticalSection );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Enter (take) into a critical section
///
///	@param[in]	hCriticalSection	- An handle to a valid critical section object
///	@param[in]	TimeoutMs			- Timeout (in milli seconds) to wait for the critical section,
///									  OSL_TIMEOUT_INFINITE for infinity
/// 
///	@note		A task can enter the same critical section more than once, but it must leave it the
///				exact amount of times that it entered it.
///	@n			This function shouldn't be called from an ISR context
///
///	@return		True if the critical section is entered successfully, or False if not and the
///				timeout expired
///
/// @see		p_CmndLib_UserImpl_CritSect_Leave
///////////////////////////////////////////////////////////////////////////////
bool p_CmndLib_UserImpl_CritSect_Enter(	HCRITICAL_SECTION hCriticalSection, u32 TimeoutMs );

///////////////////////////////////////////////////////////////////////////////
///	@brief		Leave (give) a critical section
///
///	@param[in]	hCriticalSection	- An handle to a valid critical section object
/// 
///	@note		A task is allowed to leave a critical section only if it currently holds it
///	@n			This function shouldn't be called from an ISR context
///
///	@return		None
///
/// @see		p_CmndLib_UserImpl_CritSect_Enter
///////////////////////////////////////////////////////////////////////////////
void p_CmndLib_UserImpl_CritSect_Leave(	HCRITICAL_SECTION hCriticalSection );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern_c_end

#endif // _CMNDLIB_USERIMPL_CRITSECT_H
