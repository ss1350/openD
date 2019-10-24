/*!
*  \file       tcx_hostlog.h
*  \brief
*  \Author     moria
*
*  @(#)  %filespec: tcx_hostlog.h~DMZD53#6 %
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================\n
*  date        name     version   action                                          \n
*  ----------------------------------------------------------------------------\n
*******************************************************************************/

#if   !defined( TCX_HOSTLOG_H )
#define  TCX_HOSTLOG_H


#if defined( __cplusplus )
extern "C"
{
#endif

#include "cmbs_api.h"

int tcx_HostLogOpenLogfile(char * psz_LogFile);
void tcx_HostLogCloseLogfile(void);
int  tcx_IsHostLoggerEnabled(void);
E_CMBS_RC tcx_HostLogOutputCreate( void );
void tcx_HostLogOutputDestroy( void );
void tcx_PushToPrintToFileQueue(char * pMessage, u16 nLength);
u16	tcx_PopFromPrintToFileQueue(char **pMessage);


//int tcx_WriteLog(u8 *pu8_Buffer);

#ifdef WIN32
#define CMBS_TCX_PRINT_LOG_TO_FILE		(WM_USER + 2)
#else
#define CMBS_TCX_PRINT_LOG_TO_FILE		(0x501)
#endif 


typedef enum
{
	LOG_LEVEL_INFO 		= 0x01,
	LOG_LEVEL_TRACE 		= 0x02,
	LOG_LEVEL_WARNING 	= 0x04,
	LOG_LEVEL_ERROR 		= 0x08,
	LOG_LEVEL_REAL_TIME 	= 0x10
}E_LOG_LEVEL;


// Note: when adding a new module, make sure also to add it to tcx_GetModuleName() in tcx_hostlog.c
typedef enum
{
    SRV_MODULE = 0x0,	
    CALL_MODULE,	
    SWUP_MODULE,
    FACILITY_MODULE,
    PLUGIN_MODULE,
    DATA_MODULE,	
    LA_MODULE,
    SUOTA_FILE_MODULE,
    SUOTA_MODULE,
    HAN_MODULE,	
    RTP_MODULE,
    LOG_MODULE,
    CMD_MODULE,
    HAN_SERVER_MODULE,
    HAN_MW_MODULE,
    FUN_MODULE,
	CMBS_LIB_MODULE,
	UDP_STORAGE_MODULE,
	GMEP_MODULE,
	GENERAL_LOG_MODULE,
	ROUTER_MODULE,
    AUDIO_APP_MODULE,
    MAX_MODULE
} E_MODULE_ID;

#define NUM_OF_APPS 	MAX_MODULE

void tcx_WriteLog(E_MODULE_ID e_ModuleId, E_LOG_LEVEL e_LogLevel, const char * format, ...);
void tcx_WriteLogData(E_MODULE_ID e_ModuleId, E_LOG_LEVEL u8_LogLevel, u8 *pu8_Buffer, u16 u16_Length);

const char * tcx_GetLogLevelName( E_LOG_LEVEL e_LogLevel );
const char * tcx_GetModuleName( E_MODULE_ID e_AppId );

void tcx_ConfigLogLevelForApp(E_MODULE_ID e_ModuleId, E_LOG_LEVEL e_LogLevel, bool b_EnableFlag);
u8 tcx_GetLogLevelConfigForApp(E_MODULE_ID e_ModuleId);
void tcx_EnableAllLogLevels( void );
void tcx_DisableAllLogLevels( void );

#define GENERAL_LOG_INFO_PRINT(format, ...) 						tcx_WriteLog(GENERAL_LOG_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define GENERAL_LOG_TRACE_PRINT(format, ...) 						tcx_WriteLog(GENERAL_LOG_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define GENERAL_LOG_WARNING_PRINT(format, ...) 					tcx_WriteLog(GENERAL_LOG_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define GENERAL_LOG_ERROR_PRINT(format, ...) 						tcx_WriteLog(GENERAL_LOG_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define GENERAL_LOG_RT_PRINT(format, ...) 						tcx_WriteLog(GENERAL_LOG_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define GENERAL_LOG_PRINT_DATA(pu8_Buffer, u16_Length) 			tcx_WriteLogData(GENERAL_LOG_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)

#if defined( __cplusplus )
}
#endif

#endif   // TCX_HOSTLOG_H
//*/
