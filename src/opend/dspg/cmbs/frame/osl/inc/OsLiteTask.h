///////////////////////////////////////////////////////////////////////////////////////////////////
///
///	@file		OsLiteTask.h
///	@brief		This file exports the Task kernel object functionality
/// @n
///	@n			This interface allows:
///	@n				Creation and destroying of tasks
///	@n				Sleep functionality
///	@n				Disaling and enabling of preemption (task switching) mechanism
///
///	@internal
///	@author		Shlomi Mor
///	@date		26/12/2006
///	@version	version 1.0
///
///	Copyright (C) 2006 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _OS_LITE_TASKS_H
#define _OS_LITE_TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDefs.h"
#include "OsLiteDefs.h"

#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) (void)(x)
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///	@brief	Callback function, which the task starts from
typedef void (*TaskProc)( void* ProcParam );

///	@brief	Callback function for retreiving stacks usage
typedef void (*StackUsageProc)( uint16 Priority, uint8 Percentages );
	
///	@brief	Task handle type
DECLARE_OPAQUE(HTASK);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///	@brief	Application tasks possible priorities range. 0 is highest priority. Lowest priority
/// @n		is set according to number of tasks declares in OsLiteConfig.h. Please note that
/// @n		if timers are in use, a dedicated task for the timers will be created (which is not
/// @n		being count inside OSL_MAX_TASKS definition)
#define OSL_TASK_PRIORITY_HIGHEST		0

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///	@brief		Create and start a task
///
///	@param[in]	Proc		- Entry point procedure of the created task
///	@param[in]	ProcParam	- Parameter for the entry point procedure
///	@param[in]	Priority	- Task priority (two tasks can't have the same priority).
///							  Priority must be in range of OSL_TASK_PRIORITY_HIGHEST to
///							  OSL_TASK_PRIORITY_LOWEST (lower number is higher priority).
///	@param[in]	pStack		- Pointer to the task's stack - cannot be NULL
///	@param[in]	StackSize	- The size of the stack in bytes. Must be greater than 0
/// 
///	@note		Created task runs immediately (according to their priority)
/// @n			Task must call OSL_TASK_Kill( OSL_TASK_GetCurrentTask() ) before it exists
///	@n			This function shouldn't be called from an ISR context
///
///	@return		An handle to a task object. The handle is NULL in case there is already existing
///				task at the same priority. In any other case of error, the function will enter
///				ASSERT internally (so it won't return).
///
/// @see		OSL_TASK_Kill
///////////////////////////////////////////////////////////////////////////////////////////////////
HTASK OSL_TASK_Create(	TaskProc			Proc,
						void*				ProcParam,
						uint16				Priority,
						void*				pStack,
						uint16				StackSize );

///////////////////////////////////////////////////////////////////////////////////////////////////
///	@brief		Kill a task
///
///	@param[in]	hTask	- handle to the task object which should be killed
/// 
///	@note		Each task must call OSL_TASK_Kill( OSL_TASK_GetCurrentTask() ) before it exists
///	@n			This function shouldn't be called from an ISR context
///
///	@return		None
///
/// @see		OSL_TASK_Create
///////////////////////////////////////////////////////////////////////////////////////////////////
void OSL_TASK_Kill( HTASK hTask );

///////////////////////////////////////////////////////////////////////////////////////////////////
///	@brief		Suspend the current task to a given amount of milli seconds
///
///	@param[in]	TimeoutMs	- Number of milli seconds for which we want current task to be suspended
/// 
/// @note		Passing TimeoutMs = 0 will do nothing (no context switch will happen, since there
///				is only	one task per priority)
///	@n			This function shouldn't be called from an ISR context
///
///	@return		None
///////////////////////////////////////////////////////////////////////////////////////////////////
void OSL_TASK_Sleep( uint16 TimeoutMs );

///////////////////////////////////////////////////////////////////////////////////////////////////
///	@brief		Get handle for current task
///
///	@param		None
/// 
///	@note		This function shouldn't be called from an ISR context
///
///	@return		Handle to current task object
///
/// @see		OSL_TASK_Create
///////////////////////////////////////////////////////////////////////////////////////////////////
HTASK OSL_TASK_GetCurrentTask( void );

///////////////////////////////////////////////////////////////////////////////////////////////////
///	@brief		Get stack usage (percentages) for all active tasks
///
///	@param[in]	StackUsageCb - callback function which is called for each active task
///
/// @note		When this function is running, it is looping over all active tasks, compute it's
/// @n			stack's usage, and call the given callback. So, the caller's callback will be
/// @n			called during the runtime of this function.
///
///	@return		None
///////////////////////////////////////////////////////////////////////////////////////////////////
void OSL_TASK_GetStacksUsage(    StackUsageProc StackUsageCb );

///////////////////////////////////////////////////////////////////////////////////////////////////
///	@brief		Set task property which decides whether a timed-delay 
/// @n		    ( sleep, wait on semaphore/queue/event with timeout )
/// @n 			should wake-up the system in case of timeout.
///
///	@param[in]	hTask	- handle to the task object which should be set.
///	@param[in]	WakeUp  - 1 means wakeup, 0 means freeze 
///
/// @note		
///
///	@return		None
///////////////////////////////////////////////////////////////////////////////////////////////////
void OSL_TASK_SetWakeupPolicy( HTASK hTask, uint8 WakeUp );

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // _OS_LITE_TASKS_H

