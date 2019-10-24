

#ifndef SUOTA_PROCESS
#define SUOTA_PROCESS

#include "gmep-us.h"

typedef struct
{
	int 		 threadWrFd;
	int			 threadRdFd;
	int  		 sessionId;
	int 		 socket;
	pthread_t	 receiveThread;
	char 		 local[32];
	t_st_GmepSdu httpRequest;
	u8			 sesStatus;
#ifdef SUOTA_SDU_CHOPPING    
    u32          u32_To;           // used for local storage
    u32          u32_From;       // used for local storage
    bool         SDUChoppingHS;   // does the handset support SDU chopping?
    bool         ContinueSend;        // Continue sending on next SEND_DATA_ACK
#endif
}t_Suota_GmepSessionTbl;

ST_IE_SUOTA_FILE_INFO g_st_FileInfo;
bool b_suota_local;

// These flags select between 4 modes:
// Header created by Host selects whether the artificial header is added by Host or Target
// File Download selects whether the file will be taken from HTTP server or provided by user
typedef struct
{
   bool b_HeaderCreatedbyHost; 
   bool b_FileDownload;
   bool b_EarlyChunkPrepare;
   u32 u32_AbortSessionId;
}t_st_SUOTAFlags;

#define SUOTA_MAX_PAYLOAD_SIZE      (MAX_GMEP_SDU_SIZE - SUOTA_MAX_HEADER_SIZE)

typedef struct
{
   u8 responseBuff[MAX_GMEP_SDU_SIZE];
   u32 content_length;
   u32 responseBuffLen;
}t_st_httpResponse;


typedef enum
{
    CMBS_URL_SENDING_STATE_IDLE,
    CMBS_URL_SENDING_STATE_IN_PROGRESS,
        
    CMBS_URL_SENDING_STATE_MAX
}e_URLSendState;
    
unsigned long suota_createThread(char *pIpAddr, u16 portNumber,int pushMode,u16 u16_Handset);
unsigned long suota_quitThread();
void suota_send_gmep();
void Thread_Fifo_Send(t_st_GmepAppMsgCmd   *httpMsg);
void Suota_fifo_send(int fd, t_st_GmepAppMsgCmd *pMsg);
void cplane_Sendnegack(t_st_GmepSuotaNegAck *Nack);
void suota_PrintHex(u8 *buf, u32 len);
void suota_app();
void thread_send_suota(t_Suota_GmepSessionTbl *pSession, t_st_GmepSdu *HttpResp );
void suota_ConstructMessage(t_st_httpResponse* st_Response, bool b_CreateHeader, t_Suota_GmepSessionTbl *pSession);
void getNextChunk(t_st_GmepAppMsgCmd *pMsgRx,t_Suota_GmepSessionTbl *pSession,t_st_GmepSdu *pHttpresp);
#endif


