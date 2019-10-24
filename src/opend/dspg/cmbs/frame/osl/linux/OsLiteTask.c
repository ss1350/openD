///////////////////////////////////////////////////////////////////////////////////////////////////
///
///	@file		OsLiteTask.c
///	@brief		This file provides the implementation of the Task kernel object
/// @n
///	@n			This interface allows:
///	@n				Creation and destroying of tasks
///	@n				Sleep functionality
///	@n				Disaling and enabling of preemption (task switching) mechanism
///
///	@internal
///	@author		Kovalchuk Sergiy
///	@date		06/02/2008
///	@version	version 1.0
///
///	Copyright (C) 2008 DSP GROUP, INC .   All Rights Reserved
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Smartlog.h"

#include "OsLiteTask.h"

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>


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
						uint16				StackSize )
{
	int _result;
	pthread_t   threadID;
    pthread_attr_t attr;
	
	// Init attribute
	if(pthread_attr_init(&attr))
		return NULL;
  	
    // Do not allocate a stack on Linux (the stack is dynamically allocated)
    assert( pStack == NULL );
	StackSize = 0;
	UNUSED_PARAMETER(StackSize); 

	if((_result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)))
	{
		log_genericerror(LogError, "Unable to set thread detached state", _result);
		pthread_attr_destroy(&attr);
		return NULL;
	}
	
	// Crete thread
	if((_result = pthread_create(&threadID, &attr, (void *(*)(void*))Proc, ProcParam)))
	{
		log_genericerror(LogError, "Unable to create new thread", _result);
		return NULL;
	}
	
	// Set thread priority
	struct sched_param _schedulerParam = { Priority };
	if((_result = pthread_setschedparam(threadID, SCHED_RR, &_schedulerParam)))
	{
		log_genericerror(LogWarning, "Unable to set thread priority", _result);
	}
	
	log_message(LogInfo, "Thread initialization completed successfully, new thread ID = 0x%x", 
		(unsigned int)threadID);

	return (HTASK)threadID;
}

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
void OSL_TASK_Kill( HTASK hTask )
{
	pthread_t threadID = (pthread_t)hTask;
        
	assert(hTask != NULL);
	
#ifndef __BIONIC__
	// Soft cancel the thread
	pthread_cancel(threadID);
#endif // __BIONIC__
}

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
void OSL_TASK_Sleep( uint16 TimeoutMs )
{
	struct timespec _timeToSleep, _remainedTimeToSleep;
	
	// Do nothing if 0 timout is requested
	if(!TimeoutMs) return;
	
	_timeToSleep.tv_sec = TimeoutMs/1000;
	_timeToSleep.tv_nsec = (TimeoutMs%1000)*1000000;
	
	while(nanosleep(&_timeToSleep, &_remainedTimeToSleep))
	{
		log_message(LogInfo, "Thread 0x%x was interrupted by signal, %lu.%09lu seconds remains...", 
			(unsigned int)pthread_self(), _remainedTimeToSleep.tv_sec, _remainedTimeToSleep.tv_nsec);
		
		_timeToSleep.tv_sec = _remainedTimeToSleep.tv_sec;
		_timeToSleep.tv_nsec = _remainedTimeToSleep.tv_nsec;
	}
	
	log_message(LogInfo, "Thread 0x%x has sleeped successfully for %lu.%09lu seconds...", 
		(unsigned int)pthread_self(), _timeToSleep.tv_sec, _timeToSleep.tv_nsec);
}

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
HTASK OSL_TASK_GetCurrentTask( void )
{
	return (HTASK)pthread_self();
}

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
void OSL_TASK_GetStacksUsage(    StackUsageProc StackUsageCb ) { ARGUSED(StackUsageCb); }


