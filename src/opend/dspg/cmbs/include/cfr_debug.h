/*!
*	\file			cfr_debug.h
*	\brief
*	\Author		kelbch
*
*	@(#)	%filespec: cfr_debug.h-1 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
* 14-feb-09		kelbch		1		 Initialize \n
*******************************************************************************/

#if	!defined( CFR_DEBUG_H )
#define	CFR_DEBUG_H

#if defined ( CMBS_API_TARGET )
extern int   printf(const char *format, ...);
extern int   prj_printf(const char *format, ...);
#endif

#ifdef TCXLOG_FILE
#include "tcx_hostlog.h"
#endif

#if defined (CMBS_MSG_DEBUG)
#define	DEBUG		0x7
#else
#define	DEBUG		0x4
#endif

#if defined ( CMBS_API_TARGET )

#define	_CMBS_PRINTF	printf
#define	CFR_DBG_OUT		printf

#else

#if defined ( CMBS_SYS_LOG )
#define	_CMBS_PRINTF	cmbs_intDebugOUT
#define	CFR_DBG_OUT		cmbs_intDebugOUT

#else
	#include <stdio.h>
#define	_CMBS_PRINTF	printf
#define	CFR_DBG_OUT		printf
#endif

#endif // defined ( CMBS_API_TARGET )

#define caseretstr(x) case x: return #x

#define	CFR_DBG_MASK_INFO    0x00000001L
#define	CFR_DBG_MASK_WARN    0x00000002L
#define	CFR_DBG_MASK_ERROR   0x00000004L


#if	defined( DEBUG ) && (DEBUG & CFR_DBG_MASK_INFO )
#if defined ( TCXLOG_FILE )
#define	CFR_DBG_INFO				GENERAL_LOG_INFO_PRINT
#else
#define	CFR_DBG_INFO				_CMBS_PRINTF
#endif
#else
#define	CFR_DBG_INFO(...)
#endif

#if	defined( DEBUG ) && (DEBUG & CFR_DBG_MASK_ERROR )
#if defined ( TCXLOG_FILE )
#define	CFR_DBG_ERROR				GENERAL_LOG_ERROR_PRINT
#else
#define	CFR_DBG_ERROR				_CMBS_PRINTF
#endif
#else
#define	CFR_DBG_ERROR(...)
#endif

#if	defined( DEBUG ) && (DEBUG & CFR_DBG_MASK_WARN )
#if defined ( TCXLOG_FILE )
#define	CFR_DBG_WARN				GENERAL_LOG_WARNING_PRINT
#else
#define	CFR_DBG_WARN				_CMBS_PRINTF
#endif
#else
#define	CFR_DBG_WARN(...)
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

void     cmbs_intDebugOUT ( const char *pszFormat, ... );

#if defined( __cplusplus )
}
#endif

#endif	//	CFR_DEBUG_H
