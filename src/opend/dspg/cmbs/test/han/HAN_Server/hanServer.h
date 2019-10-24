/*!
*   \file       hanServer.h
*   \brief      header for HAN Server
*   \author     HAN Team
*
*
*******************************************************************************/

#if	!defined( _HANSERVER_H )
#define	_HANSERVER_H


#include <stdio.h>
#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <sys/wait.h>
//#include <signal.h>

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#else
#include <winsock2.h>
//#include <process.h>
#endif // WIN32

//#include <errno.h>
#include <string.h>
#include "cmbs_api.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "cmbs_han.h"
#include "cfr_ie.h"
#include "apphan.h"
#include "appsrv.h"
#include "appcmbs.h"
#include "appmsgparser.h"
#include "cmbs_han_ie.h"
#include "hanfun_protocol_defs.h"
#include "appHanMw.h"
#include "tcx_hostlog.h"
#include "OsLiteNetwork.h"

#include "hanServerAPI.h"

#ifdef CMBS_PLUGIN
#include "cmbs_plugin.h"
#endif // CMBS_PLUGIN


void p_HanServer_RemovePendingMessageForClient(char* responseMessage);
u32 p_HanServer_HandleInitializeMessagesDB();
u32 p_HanServer_InitializeMessagesDB();
void p_HanServer_ServerDbInitialize();
void* p_HanServer_HandleDBStatus();
int p_HanSever_HandleSendMessageToClient(const char *pCommand);
void p_HanSever_SendMessageToClient(char *pCommand);
s32 p_HanSever_Send2AllRemoteSockets(char *pCommand);
void* p_HanServer_checkClientsStatus();
s32 p_HanSever_Send2RemoteSocket(char *pCommand, struct sockaddr_in* cliaddr);




void p_HanServer_OnRegClosed(E_CMBS_EVENT_ID e_EventID, void *pv_EventData);
void p_HanServer_OnReadDeviceTableRes(void *pv_EventData);
void p_HanServer_RegisterCallBacks();
unsigned long p_HanServer_CreateThread(char *pInterfaceName, unsigned short port );
u32 p_HanServer_OnHsRegistered(u8 u8_Hs);



CONNECT_HANDLE handle;


#define CMBS_STRING_SIZE 12000
#define CMBS_COMMAND_MAX_LEN 50
#define INTERNAL_BUFFER_SIZE (CMBS_STRING_SIZE + sizeof(struct sockaddr_in) + sizeof(t_en_proxyCB) + sizeof(t_en_handleDbAccess))


typedef enum
{
    HAN_SERVER_HANDLE_MESSAGE_FROM_CLIENT = 0,
	HAN_SERVER_INITILIZE_ARRAY,
	HAN_SERVER_SEND_MSG_TO_CLIENT,
	HAN_SERVER_CHECK_CLIENT_STATUS,
	HAN_SERVER_DELETE_CLIENT,
	HAN_SERVER_DB_STATUS
}t_en_handleDbAccess;

typedef enum
{
    HAN_PROXY_CB,
	SERVICE_PROXY_CB,
	DEBUG_PROXY_CB
}t_en_proxyCB;


typedef struct {
	t_en_handleDbAccess handler;
	void* data1;
	void* data2;
	void* data3;
	void* data4;
}s_socketData;

typedef struct
{
	u16				u16_MaxSize;
	u16				u16_CurSize;
	unsigned char	pu8_Buffer[INTERNAL_BUFFER_SIZE];
} t_st_paramList;


u16 p_HanServer_serByte(t_st_paramList *pst_paramList, u8 u8_Value);
u16 p_HanServer_serString(t_st_paramList *pst_paramList, unsigned char* value);
u16 p_HanServer_dserByte(unsigned char* pu8_Buffer, u8 *pu8_Value);
u16 p_HanServer_dserString(unsigned char* dest, unsigned char* src);

void p_HanServer_MessageFromClient(struct sockaddr_in* cliaddres, const t_CmdCb *g_CBHandler, char *buf);
void p_HanServer_DBStat();
void p_HanServer_InternalHandler(unsigned char* internalMsgBuf);
void p_HanServer_ExternalHandler(char* externalMsgBuf, struct sockaddr_in* cliaddres);	
void p_HanServer_HandleDeleteClient(struct sockaddr_in * cliaddres);
u32 p_HanSever_DeleteClient(struct sockaddr_in* cliaddres);



int g_destSocket;   	// Destination, used for UDP)
int g_internalSocket;
u32 u32_timeStamp;
u32 u32_HANClientTimeout;


#ifdef WIN32
#define SleepMs(x) Sleep(x)
#else
#define SleepMs(x) usleep(1000*x)
#endif

#define HAN_SERVER_INFO_PRINT(format, ...) 						tcx_WriteLog(HAN_SERVER_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define HAN_SERVER_TRACE_PRINT(format, ...) 						tcx_WriteLog(HAN_SERVER_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define HAN_SERVER_WARNING_PRINT(format, ...) 					tcx_WriteLog(HAN_SERVER_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define HAN_SERVER_ERROR_PRINT(format, ...) 						tcx_WriteLog(HAN_SERVER_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define HAN_SERVER_RT_PRINT(format, ...) 						tcx_WriteLog(HAN_SERVER_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define HAN_SERVER_PRINT_DATA(pu8_Buffer, u16_Length) 			tcx_WriteLogData(HAN_SERVER_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)

struct sockaddr_in si_me;

typedef enum
{
	HOST_SERVER_TARGET_STATE_DOWN = 0x0,
	HOST_SERVER_TARGET_STATE_UP,
 
} t_en_HostServer_TargetState;



volatile int g_isTargetAlive;
volatile int g_TargetUpIndication;
volatile t_en_HostServer_TargetState g_en_TargetState;


#endif // _HANSERVER_H

/**********************[End Of File]**********************************************************************************************************/
