/*************************************************************************************************************
*** UDP Storage source
**
**************************************************************************************************************/

/*******************************************
Includes
********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include "UDPStorageServer.h"

#include "tcx_hostlog.h"
#define APP_STORAGE_INFO_PRINT(format, ...) 					tcx_WriteLog(UDP_STORAGE_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define APP_STORAGE_TRACE_PRINT(format, ...) 					tcx_WriteLog(UDP_STORAGE_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define APP_STORAGE_WARNING_PRINT(format, ...) 					tcx_WriteLog(UDP_STORAGE_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define APP_STORAGE_ERROR_PRINT(format, ...) 					tcx_WriteLog(UDP_STORAGE_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define APP_STORAGE_RT_PRINT(format, ...) 						tcx_WriteLog(UDP_STORAGE_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define APP_STORAGE_PRINT_DATA(pu8_Buffer, u16_Length) 			tcx_WriteLogData(UDP_STORAGE_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)


/*******************************************
Defines
********************************************/
// socket configuration
#define STORAGE_SERVER_UDP_PORT 3550

#define UDP_RX_MSG_MAX_LEN ( 1024 * 9 )
#define UDP_TX_MSG_MAX_LEN ( 1024 * 9 )

#define UDP_PROTOCOL_TOKEN_LEN 25

/*******************************************
Types
********************************************/


/*******************************************
Locals
********************************************/

/*******************************************
Auxiliary functions
********************************************/

// returns true if the input strings are equal, false otherwise
static bool StringCmp(const u8 *pu8_Str1, const char *pu8_Str2, u32 u32_Len)
{
    return (strncmp((const char *)pu8_Str1, pu8_Str2, u32_Len) == 0) ? TRUE : FALSE;
}

static s32 FileSize(const char *sFilename)
{
    u32 rc = 0;

    FILE *fp = fopen(sFilename, "r");
    if ( fp )
    {
        fseek(fp, 0L, SEEK_END);
        rc = ftell(fp);
	 	fclose(fp);
    }
    else
    {
        APP_STORAGE_INFO_PRINT("ERROR could not open file %s\n", sFilename);
    }
    return rc;
}

static E_CMBS_RC FileWrite(const char *sFilename, u32 u32_DataLen, const u8 *pu8_Data)
{
    E_CMBS_RC rc = CMBS_RC_ERROR_GENERAL;

    FILE *fp = fopen(sFilename, "w");
    if ( fp )
    {
        if ( fwrite(pu8_Data, sizeof(u8), u32_DataLen, fp) > 0 )
        {
            rc = CMBS_RC_OK;
        }
        else
        {
            APP_STORAGE_INFO_PRINT("ERROR fwrite failed\n");
        }
     	fclose(fp);
    }
    else
    {
        APP_STORAGE_INFO_PRINT("ERROR could not open file %s\n", sFilename);
    }   
    return rc;
}

static E_CMBS_RC FileRead(const char *sFilename, u32 *pu32_DataLen, u8 *pu8_Data)
{
    E_CMBS_RC rc = CMBS_RC_ERROR_GENERAL;
    s32 s32_Filesize, num_of_bytes_read=0;
    FILE *fp;

    *pu32_DataLen = 0;

    s32_Filesize = FileSize(sFilename);
	
    fp = fopen(sFilename, "r");
    if ( fp && s32_Filesize > 0)
    {
	num_of_bytes_read = fread(pu8_Data, sizeof(u8), s32_Filesize, fp);
        if ( num_of_bytes_read > 0 )
        {
            *pu32_DataLen = num_of_bytes_read;
            rc = CMBS_RC_OK;
        }
        else
        {
            APP_STORAGE_INFO_PRINT("ERROR fread failed\n");
        }
    }
    else
    {
        APP_STORAGE_INFO_PRINT("ERROR could not open file %s\n", sFilename);
    }
    if(fp)
	{
		fclose(fp);
	}
    return rc;
}

static bool FileExist(const char *sFilename)
{
    bool rc = FALSE;

    FILE *fp = fopen(sFilename, "r");

    if ( fp )
    {
        rc = TRUE;
        fclose(fp); 
    }

    return rc;
}



// handle RX messages
static void HandleMsg(const u8 *u8_RxData, u8 *u8_TxData, u32 *pu32_TxLen)
{
    // UDP API definition
    // ==============
    // STORE <filename> <length decimal> <data raw binary>
    // Response: STORE OK <filename> / STORE NOK <filename>
    //
    // LOAD <filename>
    // Response: LOAD OK <filename> <length decimal> <data raw binary> / LOAD NOK <filename>

    char sCmd[UDP_PROTOCOL_TOKEN_LEN], sFilename[UDP_PROTOCOL_TOKEN_LEN];
    u32 u32_RxPos, u32_Bytes;

    *pu32_TxLen = 0;

    // read command
    sscanf((const char *)u8_RxData, "%s %n ", sCmd, &u32_Bytes);
    u32_RxPos = u32_Bytes;

    if ( StringCmp((u8*)sCmd, "STORE", strlen("STORE")) )
    {
        u32 u32_DataLen;
        E_CMBS_RC rc;

        // read filename
        sscanf((const char *)u8_RxData + u32_RxPos, "%s %n ", sFilename, &u32_Bytes);
        u32_RxPos += u32_Bytes;

        // read data len
        sscanf((const char *)u8_RxData + u32_RxPos, "%u %n ", &u32_DataLen, &u32_Bytes);
        u32_RxPos += u32_Bytes;

        // write to file
        rc = FileWrite(sFilename, u32_DataLen,  u8_RxData + u32_RxPos);

        // write response
        *pu32_TxLen = sprintf((char*)u8_TxData, "%s", "STORE ");
        if ( rc == CMBS_RC_OK )
        {
            *pu32_TxLen += sprintf((char*)u8_TxData + *pu32_TxLen, "%s", "OK ");
        }
        else
        {
            *pu32_TxLen += sprintf((char*)u8_TxData + *pu32_TxLen, "%s", "NOK ");
        }
        *pu32_TxLen += sprintf((char*)u8_TxData + *pu32_TxLen, "%s", sFilename);
    }
    else if ( StringCmp((u8*)sCmd, "LOAD", strlen("LOAD")) )
    {
        u32 u32_RawDataLen;

        // read filename
        sscanf((const char *)u8_RxData + u32_RxPos, "%s %n ", sFilename, &u32_Bytes);
        u32_RxPos += u32_Bytes;

        // write response
        *pu32_TxLen = sprintf((char*)u8_TxData, "%s", "LOAD ");

        if ( FileExist(sFilename) )
        {
            *pu32_TxLen += sprintf((char*)u8_TxData + *pu32_TxLen, "%s %s %u ", "OK ", sFilename, FileSize(sFilename));

            // read file
            FileRead(sFilename, &u32_RawDataLen, u8_TxData + *pu32_TxLen);
            *pu32_TxLen += u32_RawDataLen;
        }
        else
        {
            *pu32_TxLen += sprintf((char*)u8_TxData + *pu32_TxLen, "%s %s", "NOK ", sFilename);
        }
    }
    else
    {
        APP_STORAGE_INFO_PRINT("Unknown command %s ignored", sCmd);
    }
}


// handles recieved messages
#ifdef WIN32
static DWORD WINAPI UDPStorageThread(LPVOID lpParam)
#else
static void* UDPStorageThread(void *arg)
#endif
{
    u8 u8_RxData[UDP_RX_MSG_MAX_LEN];
    u8 u8_TxData[UDP_TX_MSG_MAX_LEN];
    u32 u32_TxLen;

    // create UDP socket and listen

#ifdef WIN32
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    WSADATA wsa;

    slen = sizeof(si_other);

    // Initialize winsock
    APP_STORAGE_INFO_PRINT("\nInitialising Winsock...");
    if ( WSAStartup(MAKEWORD(2, 2), &wsa) != 0 )
    {
        APP_STORAGE_INFO_PRINT("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    APP_STORAGE_INFO_PRINT("Initialized.\n");

    // Create a socket
    if ( (s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET )
    {
        APP_STORAGE_INFO_PRINT("Could not create socket : %d", WSAGetLastError());
    }
    APP_STORAGE_INFO_PRINT("Socket created.\n");

    // Prepare the sockaddr_in structure
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(STORAGE_SERVER_UDP_PORT);

    // Bind
    if ( bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR )
    {
        APP_STORAGE_INFO_PRINT("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");

    // keep listening for data
    while (1)
    {
        APP_STORAGE_INFO_PRINT("Waiting for data...");
        fflush(stdout);

        // clear the buffer by filling null, it might have previously received data
        memset(u8_RxData, '\0', sizeof(u8_RxData));

        // try to receive some data, this is a blocking call
        if ( (recv_len = recvfrom(s, u8_RxData, sizeof(u8_RxData), 0, (struct sockaddr *)&si_other, &slen)) == SOCKET_ERROR )
        {
            APP_STORAGE_INFO_PRINT("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        // print details of the client/peer and the data received
        APP_STORAGE_INFO_PRINT("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

        HandleMsg(u8_RxData, u8_TxData, &u32_TxLen);

        // now reply the client
        if ( sendto(s, u8_TxData, u32_TxLen, 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR )
        {
            APP_STORAGE_INFO_PRINT("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }

    closesocket(s);
    WSACleanup();
#else
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;

    UNUSED_PARAMETER(arg);

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0)
    {
		APP_STORAGE_INFO_PRINT("hanServer Error bind \n\n ");	
		exit(1);
    }
    bzero(& servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(STORAGE_SERVER_UDP_PORT);    
    if (bind(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) == -1 )
    {
        APP_STORAGE_INFO_PRINT("Bind failed with error code");
		close(sockfd);
        exit(1);
    }
    while ( 1 )
    {
        len = sizeof(cliaddr);       
		if (  recvfrom(sockfd, u8_RxData, sizeof(u8_RxData), 0, (struct sockaddr *)&cliaddr, &len) == -1 )
        {
            APP_STORAGE_INFO_PRINT("recvfrom() failed with error code");
			close(sockfd);
            exit(1);
        }
        HandleMsg(u8_RxData, u8_TxData, &u32_TxLen);
        if(sendto(sockfd, u8_TxData, u32_TxLen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
        {
			APP_STORAGE_INFO_PRINT("sendto() failed with error code");
			close(sockfd);
            exit(1);
        }
    }
#endif
    return 0;
}

/*******************************************
Storage Server protocol Mgr API
********************************************/
E_CMBS_RC UDPStorageMgr_Init(void)
{
    // simply spawn UDP thread

#ifdef WIN32
    DWORD   dwThreadId;

    CreateThread(
       NULL,                   // default security attributes
       0,                      // use default stack size
       UDPStorageThread,       // thread function name
       0,                      // argument to thread function
       0,                      // use default creation flags
       &dwThreadId);   // returns the thread identifier

    if ( dwThreadId == 0 )
    {
        APP_STORAGE_INFO_PRINT("ERROR could not create thread!! ");
        ExitProcess(3);
    }

#else
    int err;
    pthread_t tid;

    err = pthread_create(&tid, NULL, UDPStorageThread, NULL);

    if ( err != 0 )
    {
        APP_STORAGE_INFO_PRINT("ERROR could not create thread!! ");
    }
    else
    {
        pthread_join(tid,NULL);
    }

#endif

    return CMBS_RC_OK;
}


//************* [End of file] ******************************************************************
