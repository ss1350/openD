#include "OsLiteTask.h"
#include "hanServer.h"

static void p_HanSever_CloseSocket(s32 sock);
void* p_HanServer_PingTargetThread(void *args);
void p_HanServer_UdpCmbsProxy(s32 s32_externalSocketFd,s32 s32_internalSocketFd);
void* p_HanServer_Thread(void *args);



#define INTERNAL_PORT 3491     // the port remotes will be connecting to



char g_interfaceName[20] = { 0 };
unsigned short g_externalBindPort;


#define TIME_TO_WAIT_FOR_TARGET_TO_REPLY         10000
#define TIME_UNTIL_NEXT_PING_TO_TARGET           30000
#define NUM_OF_RETRIES_TO_PING          		 2
#define INTERNAL_SOCKET							 0

#define HAN_SELECT_TIMEOUT						 1000 // 1 sec

HTASK g_hanProcessThreadId;
HTASK g_hanCleanNonresponsiveClientsThreadId;
HTASK g_hanPingTargetThreadId;

volatile int g_isTargetPingsDisabled;

#ifdef AVS_AUDIO_APP
void app_AudioTimeoutHandler(void);
extern int avs_call_release_timeout;
#endif // AVS_AUDIO_APP

void* p_HanServer_GetInAddr(struct sockaddr *sa)
{
    if ( sa->sa_family == AF_INET )
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

#ifndef WIN32
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
#endif
    return NULL;
}


/*
	


*/
void p_HanServer_UdpCmbsProxy(s32 s32_externalSocketFd,s32 s32_internalSocketFd)
{
	u32 pu32_readySockets[2],u32_numOfReadySocket,i;
	struct sockaddr_in cliaddres;
	char externalMsgBuf[CMBS_STRING_SIZE + 1] = { 0 };
	unsigned char internalMsgBuf[INTERNAL_BUFFER_SIZE + 1] = { 0 };
    s32 rc= 0;
	s32 s32_fatalError = 0;
	s32 ps32_sockets[2] = {s32_internalSocketFd,s32_externalSocketFd};
	SELECT_HANDLE selectHandle;
	u32_timeStamp = 0;
	u32_HANClientTimeout = 0;
	

    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_UdpCmbsProxy external port chosen %d\n", s32_externalSocketFd);
	HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_UdpCmbsProxy internal port chosen %d\n", s32_internalSocketFd);
	
	//initialize DB.
	p_HanServer_ServerDbInitialize();

	//initialize state
	g_en_TargetState = HOST_SERVER_TARGET_STATE_UP;


	selectHandle = SocketSelectCreate();
	SocketSelectSet(selectHandle,ps32_sockets,2,NULL,0);
    while (1)
    {
        // receive commands from remote client and keep its address

        if ((rc = SocketSelect(selectHandle, pu32_readySockets, &u32_numOfReadySocket, NULL, NULL, HAN_SELECT_TIMEOUT)) < 0)
			break;
		if (rc == 0) //timeout
		{
            if (u32_HANClientTimeout)
            {
                p_HanServer_checkClientsStatus();
                u32_HANClientTimeout = 0;
            }
#ifdef AVS_AUDIO_APP
            app_AudioTimeoutHandler();
#endif // AVS_AUDIO_APP
			continue;
		}

		for(i =0; i<u32_numOfReadySocket; i++)
		{
			if(pu32_readySockets[i]==INTERNAL_SOCKET) //internalSocket
			{
				if((rc = SocketRecv(s32_internalSocketFd, internalMsgBuf, INTERNAL_BUFFER_SIZE, SOCK_TYPE_UDP, (struct sockaddr *) &cliaddres)) <= 0)
		        {
		        	s32_fatalError = 1;
		            break;
		        }

				HAN_SERVER_INFO_PRINT("%d bytes arrived\n", rc);

				// Make sure buf is null terminated
		        internalMsgBuf[rc] = 0;
				
				p_HanServer_InternalHandler(internalMsgBuf);

			}
			else //externalSocket
			{
				if((rc = SocketRecv(s32_externalSocketFd,(unsigned char *) externalMsgBuf, CMBS_STRING_SIZE,SOCK_TYPE_UDP,(struct sockaddr *) &cliaddres)) < 0)
		        {
		        	s32_fatalError = 1;
		            break;
		        }

				if (rc == 0) // When we tring to send message to close socket we get 'close socket' back with rc=0.
				{
					//delete closed socket:
					p_HanServer_HandleDeleteClient(&cliaddres);
					break;
				}
					

				HAN_SERVER_INFO_PRINT("%d bytes arrived\n", rc);
				// Make sure buf is null terminated
		        externalMsgBuf[rc] = 0;

				p_HanServer_ExternalHandler(externalMsgBuf,&cliaddres);
		
			}
		}

		//close this thread and re-open it.
		if(s32_fatalError)
		{
			break;
		}
	}
	SocketSelectRelease(selectHandle);

    HAN_SERVER_WARNING_PRINT("hanServer: p_HanServer_UdpCmbsProxy Exiting Loop \n\n");
}





void* p_HanServer_Thread(void *args)
{
    // Start listening on port ...
    s32 s32_externalSockFd;
    s32 s32_internalSockFd;
	
    //    ST_HAN_MSG_REG_INFO pst_HANMsgRegInfo;
    struct sockaddr_in externalServaddr;
	struct sockaddr_in internalServaddr;

#ifndef WIN32
    struct ifreq externalIfr;
	struct ifreq internalIfr;

    memset(&externalIfr, 0, sizeof(externalIfr));
	memset(&internalIfr, 0, sizeof(internalIfr));
#endif

    UNUSED_PARAMETER(args);
    HAN_SERVER_INFO_PRINT("HAN: p_HanServer_Thread Process Started \n");

    while (1)
    {
#ifndef WIN32
        int yes = 0;
#endif

		s32_externalSockFd = SockCreate(SOCK_TYPE_UDP);
		s32_internalSockFd = SockCreate(SOCK_TYPE_UDP);
        if ( s32_externalSockFd < 0 || s32_internalSockFd < 0)
        {
            HAN_SERVER_ERROR_PRINT("hanServer Error bind \n\n ");
            exit(1);
        }
#ifndef WIN32
        externalIfr.ifr_addr.sa_family = AF_INET;
        strncpy(externalIfr.ifr_name, g_interfaceName,sizeof(externalIfr.ifr_name));
        externalIfr.ifr_name[sizeof(externalIfr.ifr_name)-1] = '\0';
        ioctl(s32_externalSockFd, SIOCGIFADDR, &externalIfr);

        internalIfr.ifr_addr.sa_family = AF_INET;
        strncpy(internalIfr.ifr_name, g_interfaceName,sizeof(internalIfr.ifr_name));
        internalIfr.ifr_name[sizeof(internalIfr.ifr_name)-1] = '\0';
        ioctl(s32_internalSockFd, SIOCGIFADDR, &internalIfr);		

        if (setsockopt(s32_externalSockFd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
        {
            HAN_SERVER_ERROR_PRINT("hanServer Error setsockopt\n\n ");
            exit(1);
        }
		
        if (setsockopt(s32_internalSockFd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
        {
            HAN_SERVER_ERROR_PRINT("hanServer Error setsockopt\n\n ");
            exit(1);
        }

		

#endif

#ifndef WIN32
        ((struct sockaddr_in *)&externalIfr.ifr_addr)->sin_port = htons(g_externalBindPort);
		((struct sockaddr_in *)&internalIfr.ifr_addr)->sin_port = htons(INTERNAL_PORT);

#else
        HAN_SERVER_INFO_PRINT("hanServer interface %s\n", g_interfaceName);
#endif

        externalServaddr.sin_family = AF_INET;
        externalServaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* WILD CARD FOR IP ADDRESS */
        externalServaddr.sin_port = htons(g_externalBindPort); /* port number */
        memset(externalServaddr.sin_zero, 0, sizeof(externalServaddr.sin_zero));

        internalServaddr.sin_family = AF_INET;
        internalServaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* WILD CARD FOR IP ADDRESS */
        internalServaddr.sin_port = htons(INTERNAL_PORT); /* port number */
        memset(internalServaddr.sin_zero, 0, sizeof(internalServaddr.sin_zero));


        if ( bind(s32_externalSockFd, (struct sockaddr *)&externalServaddr, sizeof(struct sockaddr_in)) == -1 )
        {
            HAN_SERVER_ERROR_PRINT("hanServer Error bind \n\n ");
            p_HanSever_CloseSocket(s32_externalSockFd);
            exit(1);
        }

		if ( bind(s32_internalSockFd, (struct sockaddr *)&internalServaddr, sizeof(struct sockaddr_in)) == -1 )
        {
            HAN_SERVER_ERROR_PRINT("hanServer Error bind \n\n ");
            p_HanSever_CloseSocket(s32_internalSockFd);
            exit(1);
        }

		handle = SocketConnectCreate("127.0.0.1",INTERNAL_PORT);
        HAN_SERVER_INFO_PRINT("binded\n");

		// UDP
		HAN_SERVER_INFO_PRINT("server: waiting for Messages from Remote...\n");
		g_destSocket = s32_externalSockFd;
		g_internalSocket = s32_internalSockFd;
		HAN_SERVER_INFO_PRINT("g_destSocket =%d \n", g_destSocket);

		p_HanServer_UdpCmbsProxy(s32_externalSockFd,s32_internalSockFd); // Loop - we do not call fork - only one connection needed !!!


        p_HanSever_CloseSocket(s32_externalSockFd);
		p_HanSever_CloseSocket(s32_internalSockFd);
		SocketConnectRelease(handle);
		handle = NULL;
    }
}


unsigned long p_HanServer_CreateThread(char *pInterfaceName, unsigned short port )
{
    strncpy(g_interfaceName, pInterfaceName, (sizeof(g_interfaceName) - 1));
    g_externalBindPort = port;
	
    // initHan will be called after target is ready

	g_hanProcessThreadId = OSL_TASK_Create((void (*)(void *))p_HanServer_Thread,NULL,0,NULL,0);
	g_hanPingTargetThreadId = OSL_TASK_Create((void (*)(void *))p_HanServer_PingTargetThread,NULL,0,NULL,0);
	
    return 0;
}

static void p_HanSever_CloseSocket(s32 sock)
{
	SocketClose(sock);
}


void p_HanServer_DisablePingToTarget(bool b_value)
{
    g_isTargetPingsDisabled = b_value;
}

/*
	Send periodic ping from the host to the target, and wait for response.
	If the target did not respond, notify all clients that target is down, and wait for target up.
	when the target is up, notify all clients that target is up.
*/
void* p_HanServer_PingTargetThread(void *args)
{
	u32 i;
	g_TargetUpIndication = 0;
	while(1)
	{
		//sleep until the next ping
		g_isTargetAlive = 0;
		SleepMs(TIME_UNTIL_NEXT_PING_TO_TARGET);

		if ((g_isTargetAlive == 1) || (g_isTargetPingsDisabled))
		{
			continue;
		}
		
		//try to ping target
		for(i = 0; i<NUM_OF_RETRIES_TO_PING; i++)
		{
			app_SrvPing();
			//wait until target respones
			SleepMs(TIME_TO_WAIT_FOR_TARGET_TO_REPLY);
			if(g_isTargetAlive)
			{
				break;
			}
		}

		if(g_isTargetAlive != 1)
		{//target is down, send message to all clients.
			g_en_TargetState = HOST_SERVER_TARGET_STATE_DOWN;
			
			p_HanSever_Send2AllRemoteSockets("TARGET_STATE\r\n STATE: DOWN\r\n\r\n");
							

			//wait for target up.
			while(1)
			{
				SleepMs(TIME_UNTIL_NEXT_PING_TO_TARGET);
				if(g_TargetUpIndication== 1)
				{//target is up, send message to all clients.
					//reset flag
					g_TargetUpIndication = 0;

					//delete all previous messages
					p_HanServer_HandleInitializeMessagesDB();
					break;
				}
			}
		}
	}	
}






