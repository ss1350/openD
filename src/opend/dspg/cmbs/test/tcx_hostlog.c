/*!
*  \file       tcx_hostlog.c
*  \brief      Logging functionality for the host applications
*  \Author     Moria
*
*  @(#)  %filespec: tcx_hostlog.c~12.2.1.1 %
*
*******************************************************************************
*  \par  History
*  \n==== History =============================================================\n
*  date        		name     version   action                                       \n
*  ----------------------------------------------------------------------------\n
*   30-may-16  	moria	1.0       Initial version                            \n
*******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "cmbs_platf.h"
#include "cmbs_api.h"
#include "cfr_mssg.h"
#include "tcx_hostlog.h"
#include "tcx_util.h"
#include <time.h>

#ifdef __linux__
#include <pthread.h>
#endif

#define LOG_LIMIT   1024 // in KB
int loglim = LOG_LIMIT;

static char gpsz_HostLogFile[256];
static FILE *g_fpHostLogFile = NULL;


CFR_CMBS_CRITICALSECTION	G_hHostPrintingCriticalSection;

#ifdef WIN32
HANDLE						G_tcx_hHostLoggerThread		= NULL;
ULONG						G_tcx_HostLoggerThreadId		= 0;
// Forward declaration 
DWORD tcx_HostLoggerPrintThread(LPVOID lpThreadParameter);
#elif __linux__
pthread_t					G_tcx_hHostLoggerThread = 0;
// Forward declaration 
pthread_cond_t  host_condition_cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t host_condition_mutex = PTHREAD_MUTEX_INITIALIZER;
void *print_HostLog_function( void *ptr );
#endif // __linux__

#define LOG_MODULE_INFO_PRINT(format, ...) 						tcx_WriteLog(LOG_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define LOG_MODULE_TRACE_PRINT(format, ...) 					tcx_WriteLog(LOG_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define LOG_MODULE_WARNING_PRINT(format, ...) 					tcx_WriteLog(LOG_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define LOG_MODULE_ERROR_PRINT(format, ...) 					tcx_WriteLog(LOG_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define LOG_MODULE_RT_PRINT(format, ...) 						tcx_WriteLog(LOG_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define LOG_MODULE_PRINT_DATA(pu8_Buffer, u16_Length) 		tcx_WriteLogData(LOG_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define	CMBS_PRINT_FIFO_SIZE		100
#define 	CMBS_MAX_LOG_MESSAGE	15000

typedef struct
{
    char		message[CMBS_MAX_LOG_MESSAGE];
    u16		length;
}
CMBS_host_print_msg;


typedef struct
{
    CMBS_host_print_msg messages[CMBS_PRINT_FIFO_SIZE];
    u8		nReadMsg;
    u8		nWriteMsg;
    u8		nCount;
}
CMBS_G_tcx_host_print_queue;

CMBS_G_tcx_host_print_queue G_tcx_host_print_queue;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

u8 pu8_AppLogLevelMask[NUM_OF_APPS] = {0};

void tcx_PushToPrintToFileQueue(char *pMessage, u16 nLength)
{
    CFR_CMBS_ENTER_CRITICALSECTION(G_hHostPrintingCriticalSection);
    // Effi, there is a check for the length, giong to use CMBS_MAX_LOG_MESSAGE instead of CMBS_MAX_IE_PRINT_SIZE
    if ( (G_tcx_host_print_queue.nCount < CMBS_PRINT_FIFO_SIZE) && (nLength < CMBS_MAX_LOG_MESSAGE) )
    {
        CMBS_host_print_msg *pMsg = &G_tcx_host_print_queue.messages[G_tcx_host_print_queue.nWriteMsg];
        memcpy(pMsg->message, pMessage, nLength);
        pMsg->length = nLength;
        G_tcx_host_print_queue.nCount++;

        // increment write index
        G_tcx_host_print_queue.nWriteMsg++;
        if ( G_tcx_host_print_queue.nWriteMsg >= CMBS_PRINT_FIFO_SIZE )
            G_tcx_host_print_queue.nWriteMsg = 0;

    }
    CFR_CMBS_LEAVE_CRITICALSECTION(G_hHostPrintingCriticalSection);
}


//////////////////////////////////////////////////////////////////////////
u16	tcx_PopFromPrintToFileQueue(char **pMessage)
{
    CMBS_host_print_msg *pMsg;
    CFR_CMBS_ENTER_CRITICALSECTION(G_hHostPrintingCriticalSection);
    pMsg = &G_tcx_host_print_queue.messages[G_tcx_host_print_queue.nReadMsg];
    if ( G_tcx_host_print_queue.nCount )
    {
        *pMessage = pMsg->message;
        G_tcx_host_print_queue.nCount--;
        // increment read index
        G_tcx_host_print_queue.nReadMsg++;
        if ( G_tcx_host_print_queue.nReadMsg >= CMBS_PRINT_FIFO_SIZE )
            G_tcx_host_print_queue.nReadMsg = 0;
    }
    else
    {
        *pMessage = NULL;
        pMsg->length = 0;
    }
    CFR_CMBS_LEAVE_CRITICALSECTION(G_hHostPrintingCriticalSection);

    return pMsg->length;
}


void tcx_ForcePrintLog(void)
{
#ifdef WIN32
    if ( G_tcx_HostLoggerThreadId != 0 )
    {
        PostThreadMessage(G_tcx_HostLoggerThreadId, CMBS_TCX_PRINT_LOG_TO_FILE, 0, 0);
    }
#elif __linux__
    pthread_mutex_lock( &host_condition_mutex );
    pthread_cond_signal( &host_condition_cond);
    pthread_mutex_unlock( &host_condition_mutex );
#endif

}

int rename_file(char *old, char *new, int *error)
{

    if (rename(old, new) < 0)
    {
        if (error)
            *error = errno;

        return -1;
    }

    return 0;
}

int WriteLogFile(FILE **fp, const char *buffer, long len, long limit, const char *fname)
{
    int err;
    char filename[256];

    if (limit > 0 && ftell(*fp) >= limit)
    {
        fflush(*fp);
        fclose(*fp);

        strcpy(filename, fname);
        strcat(filename, ".bak");

        unlink(filename);

        if (rename_file((char *)fname, filename, &err) < 0)
        {
             printf("ERROR: Can't rname file err %d\n", err);
            // can't rename log file, reopen 
        }

        *fp = fopen(fname, "w");
    }

    if (!*fp)
        return -1;

    err = fwrite(buffer, 1, len, *fp);
    fflush(*fp);

    return err;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
DWORD tcx_HostLoggerPrintThread(LPVOID lpThreadParameter)
{
    MSG msg;

    while (1)
    {
        if ( GetMessage(&msg, NULL, 0, 0) )
        {
            char *pMessage = NULL;
            u16 nLength;
            nLength = tcx_PopFromPrintToFileQueue(&pMessage);
            if ( pMessage )
            {
                pMessage[nLength] = 0;

                if ( g_fpHostLogFile )
                {
//                    fwrite(pMessage, 1, nLength, g_fpHostLogFile);
//                    fflush(g_fpHostLogFile);
                    WriteLogFile(&g_fpHostLogFile, pMessage, nLength, loglim * 1000, gpsz_HostLogFile);
                }
                else
				{
		  			printf("%s",pMessage);
                }
            }
        }

    }
}

#elif __linux__

void *print_HostLog_function( void *ptr )
{
    UNUSED_PARAMETER(ptr);
    while(1)
    {
        char *pMessage;
        u16 nLength;

        pthread_mutex_lock( &host_condition_mutex );
        pthread_cond_wait( &host_condition_cond, &host_condition_mutex );
        pthread_mutex_unlock( &host_condition_mutex );

        do
        {
            nLength = tcx_PopFromPrintToFileQueue(&pMessage);
            if (pMessage)
            {
                pMessage[nLength] = 0;
                if( g_fpHostLogFile)
                {
//                    fwrite(pMessage, 1, nLength, g_fpHostLogFile);
//                    fflush(g_fpHostLogFile);
                    WriteLogFile(&g_fpHostLogFile, pMessage, nLength, loglim * 1000, gpsz_HostLogFile);
					printf("%s", pMessage);
                }
                else
				{
					printf("%s", pMessage);
                }
            }
        } while (nLength != 0);
    }
}
#endif //__linux__
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


int tcx_HostLogOpenLogfile(char * psz_HostLogFile)
{


    time_t CurrentTime = time(NULL);
    struct tm CurrentLocalTime = *localtime(&CurrentTime);

    u16 u16_Pos = 0;
    char strOutBuffer[100];
	
    g_fpHostLogFile = fopen(psz_HostLogFile, "w");

    if ( g_fpHostLogFile == NULL )
    {
        return 0;
    }

    strcpy(gpsz_HostLogFile, psz_HostLogFile);
    gpsz_HostLogFile[sizeof(gpsz_HostLogFile) - 1] = 0;


   // GetLocalTime(&stSystemTime);
    u16_Pos = sprintf(strOutBuffer, "DSP Group - CMBS Host Logfile\n");
    fwrite(strOutBuffer, 1, u16_Pos, g_fpHostLogFile);

    // print version
    u16_Pos = sprintf(strOutBuffer, "Version %02x.%02x - Build %d\n", (tcx_ApplVersionGet() >> 8), (tcx_ApplVersionGet() & 0xFF), tcx_ApplVersionBuildGet());
    fwrite(strOutBuffer, 1, u16_Pos, g_fpHostLogFile);

    // print timestamp 
    u16_Pos = sprintf(strOutBuffer, "Log started at: %02d-%02d-%d  %02d:%02d\n\n", CurrentLocalTime.tm_mday,
                      CurrentLocalTime.tm_mon + 1,  CurrentLocalTime.tm_year + 1900,  CurrentLocalTime.tm_hour,  CurrentLocalTime.tm_min);
    fwrite(strOutBuffer, 1, u16_Pos, g_fpHostLogFile);
 

    fflush(g_fpHostLogFile);

    //enable all logs, but RT by default
    memset(pu8_AppLogLevelMask, 0x0F, NUM_OF_APPS);
	
    return 1;
}

void tcx_HostLogCloseLogfile(void)
{
    if ( g_fpHostLogFile != NULL )
    {
        fclose(g_fpHostLogFile);
        g_fpHostLogFile = NULL;
    }
}


E_CMBS_RC tcx_HostLogOutputCreate( void )
{
    CFR_CMBS_INIT_CRITICALSECTION(G_hHostPrintingCriticalSection);

#ifdef WIN32
    G_tcx_hHostLoggerThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tcx_HostLoggerPrintThread, 0, 0, &G_tcx_HostLoggerThreadId);
    if ( NULL == G_tcx_hHostLoggerThread )
    {
        return CMBS_RC_ERROR_GENERAL;
    }
#elif __linux__
    {
        int i_result = pthread_create( &G_tcx_hHostLoggerThread, NULL, print_HostLog_function, (void*) NULL);
        if (0 != i_result)
        {
            return CMBS_RC_ERROR_GENERAL;
        }
    }
#endif

    return CMBS_RC_OK;
}

void tcx_HostLogOutputDestroy( void )
{
#ifdef WIN32
    if ( G_tcx_hHostLoggerThread )
    {
        TerminateThread(G_tcx_hHostLoggerThread, 0);
        G_tcx_hHostLoggerThread = NULL;
    }
#elif __linux__	
    if ( G_tcx_hHostLoggerThread )
    {
		pthread_cancel( G_tcx_hHostLoggerThread );
		pthread_join(G_tcx_hHostLoggerThread, NULL); 
    }	
#endif
    CFR_CMBS_DELETE_CRITICALSECTION(G_hHostPrintingCriticalSection);
}

/*void tcx_WriteLog(u8 u8_ModuleId, u8 u8_LogLevel, const char * format, ...)
{
	return;
}*/


int tcx_IsHostLoggerEnabled(void)
{
    return (g_fpHostLogFile != NULL);
}


const char * tcx_GetLogLevelName( E_LOG_LEVEL e_LogLevel )
{
	switch ( e_LogLevel )
	{
		case LOG_LEVEL_INFO:
			return "Info";
			
		case LOG_LEVEL_TRACE:
			return "Trace";
			
		case LOG_LEVEL_WARNING:
			return "Warn";
			
		case LOG_LEVEL_ERROR:
			return "Err";

		case LOG_LEVEL_REAL_TIME:
			return "RT";
			
		default:
			return "Unkwn";
	}
}

const char * tcx_GetModuleName( E_MODULE_ID e_AppId )
{
	switch ( e_AppId )
	{
		case SRV_MODULE:
			return "Srv App";
			
		case CALL_MODULE:
			return "Call App";
			
		case SWUP_MODULE:
			return "SWUP App";
			
		case FACILITY_MODULE:
			return "Facility App";  

		case PLUGIN_MODULE:
			return "Plugin App";
			
		case DATA_MODULE:
			return "Data App";
			
		case LA_MODULE:
			return "LA App";
			
		case SUOTA_FILE_MODULE:
			return "SUOTA File module";  

		case SUOTA_MODULE:
			return "SUOTA module";
			
		case HAN_MODULE:
			return "HAN module";
			
		case RTP_MODULE:
			return "RTP module";
			
		case LOG_MODULE:
			return "Log module";
			
		case CMD_MODULE:
			return "CMD module";  

		case HAN_SERVER_MODULE:
			return "Han Server";

		case HAN_MW_MODULE:
			return "HAN MW module";

		case FUN_MODULE:
			return "FUN module";

		case CMBS_LIB_MODULE:
			return "CMBS Lib module";

		case UDP_STORAGE_MODULE:
			return "UDP Storage";

		case GMEP_MODULE:
			return "GMEP-US";

		case GENERAL_LOG_MODULE:
			return "General";

		case ROUTER_MODULE:
			return "CallRouter";
			
		case AUDIO_APP_MODULE:
			return "Audio App";

		default:
			return "Unkown module";
	}
}


void tcx_WriteLog(E_MODULE_ID e_ModuleId, E_LOG_LEVEL e_LogLevel, const char * format, ...)
{
       char			tcx_LogPacketString[CMBS_MAX_LOG_MESSAGE];
	u32			u32_Pos = 0;
	va_list 		args;

#ifdef WIN32
    SYSTEMTIME stSystemTime;
    time_t curTime;
    int curTimeInteger;
#else
    time_t CurrentTime = time(NULL);
    struct tm CurrentLocalTime = *localtime(&CurrentTime);
   struct timeval  st_TimeVal;
#endif

	// filter according to module and log level
	if((e_ModuleId < MAX_MODULE) && 
		!(pu8_AppLogLevelMask[e_ModuleId] & e_LogLevel))
		{
			return;
		}
    //  Added Timestamp 
#ifdef WIN32
    curTime = time(NULL);
    curTimeInteger = (int)curTime;
    GetLocalTime(&stSystemTime);
    u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, "[%02d-%02d-%d  %02d:%02d:%02d:%03d] ", stSystemTime.wDay, stSystemTime.wMonth,
                       stSystemTime.wYear, stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond, stSystemTime.wMilliseconds);
#else
    gettimeofday(&st_TimeVal, NULL);
    u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, "[%02d-%02d-%d  %02d:%02d:%02d:%03ld] ", CurrentLocalTime.tm_mday,
                      CurrentLocalTime.tm_mon + 1,  CurrentLocalTime.tm_year + 1900,  CurrentLocalTime.tm_hour,  CurrentLocalTime.tm_min, CurrentLocalTime.tm_sec, st_TimeVal.tv_usec / 1000);
#endif

	u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, "[%s] [%s] ", tcx_GetModuleName(e_ModuleId), tcx_GetLogLevelName(e_LogLevel));
	
       va_start (args, format);
       u32_Pos += vsprintf (tcx_LogPacketString + u32_Pos, format, args);
	va_end (args);

	tcx_PushToPrintToFileQueue(tcx_LogPacketString, u32_Pos);
	tcx_ForcePrintLog();
}

void tcx_WriteLogData(E_MODULE_ID e_ModuleId, E_LOG_LEVEL e_LogLevel, u8 *pu8_Buffer, u16 u16_Length)
{
       char			tcx_LogPacketString[CMBS_MAX_LOG_MESSAGE];
	u32			u32_Pos = 0;
	u16			u16_Index = 0;
#ifdef WIN32
    SYSTEMTIME stSystemTime;
    time_t curTime;
    int curTimeInteger;
#else
    time_t CurrentTime = time(NULL);
    struct tm CurrentLocalTime = *localtime(&CurrentTime);
   struct timeval  st_TimeVal;
#endif

	// filter according to module and log level
	if((e_ModuleId < MAX_MODULE && 
		!(pu8_AppLogLevelMask[e_ModuleId] & e_LogLevel)) ||
		(u16_Length == 0))
		{
			return;
		}
	
// Add timestamp
#ifdef WIN32
    curTime = time(NULL);
    curTimeInteger = (int)curTime;
    GetLocalTime(&stSystemTime);
    u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, "[%02d-%02d-%d  %02d:%02d:%02d:%03d] ", stSystemTime.wDay, stSystemTime.wMonth,
                       stSystemTime.wYear, stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond, stSystemTime.wMilliseconds);
#else
    gettimeofday(&st_TimeVal, NULL);
    u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, "[%02d-%02d-%d  %02d:%02d:%02d:%03ld] ", CurrentLocalTime.tm_mday,
                      CurrentLocalTime.tm_mon + 1,  CurrentLocalTime.tm_year + 1900,  CurrentLocalTime.tm_hour,  CurrentLocalTime.tm_min, CurrentLocalTime.tm_sec, st_TimeVal.tv_usec / 1000);
#endif


	u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, "[%s] [%s] ", tcx_GetModuleName(e_ModuleId), tcx_GetLogLevelName(e_LogLevel));
	
	for (u16_Index = 0; u16_Index < u16_Length; u16_Index++)
		{
			u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, " 0x%X", pu8_Buffer[u16_Index]);
		}
	u32_Pos += sprintf(tcx_LogPacketString + u32_Pos, "\n");

	tcx_PushToPrintToFileQueue(tcx_LogPacketString, u32_Pos);
	tcx_ForcePrintLog();
}

void tcx_ConfigLogLevelForApp(E_MODULE_ID e_ModuleId, E_LOG_LEVEL u8_LogLevel, bool b_EnableFlag)
{
	if (e_ModuleId < MAX_MODULE)
		{
			if(b_EnableFlag)
				{
					pu8_AppLogLevelMask[e_ModuleId] |= u8_LogLevel;
				}
			else
				{
					pu8_AppLogLevelMask[e_ModuleId] &= ~u8_LogLevel;
				}

			LOG_MODULE_INFO_PRINT("Log level %s for module %s was %s\n", tcx_GetLogLevelName(u8_LogLevel), tcx_GetModuleName(e_ModuleId), b_EnableFlag ? "Enabled" : "Disabled");

		}
}

u8 tcx_GetLogLevelConfigForApp(E_MODULE_ID e_ModuleId)
{
	if (e_ModuleId < MAX_MODULE)
		{
			return pu8_AppLogLevelMask[e_ModuleId];
		}
	return 0;
}

void tcx_EnableAllLogLevels( void )
{
    memset(pu8_AppLogLevelMask, 0xFF, NUM_OF_APPS);
    LOG_MODULE_INFO_PRINT("All log levels for all modules were enabled\n");
}

void tcx_DisableAllLogLevels( void )
{
    memset(pu8_AppLogLevelMask, 0x0, NUM_OF_APPS);
	
    // enable log module to see changed in configuration
    pu8_AppLogLevelMask[LOG_MODULE] = 0x0F;

    LOG_MODULE_INFO_PRINT("All log levels for all modules were disabled\n");
}	
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
