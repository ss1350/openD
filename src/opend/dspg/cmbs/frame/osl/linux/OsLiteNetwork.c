#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "OsLiteTask.h"
#include "OsLiteNetwork.h"
#include "log.h"

typedef struct {
    struct sockaddr_in serv_addr;
} SocketConnectHandle;

#define HAN_PRINT_ERROR printf
#define HAN_PRINT_DEBUG printf


#define URL_MAX_LEN 100
#define IP_ADDR_LEN 100
#define MAC_ADDR_LEN 6


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- INIT --
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int InitNetwork(void)
{
    return 0;
}

void ReleaseNetwork(void)
{}


int SocketError(void)
{
    int rc;

    rc = errno;

    return rc;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Socket --
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int SockCreate(SOCKET_TYPE eType)
{
    int sock;

    if ( eType == SOCK_TYPE_TCP )
    {
        // create TCP socket
        if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        {
            HAN_PRINT_ERROR("TCP socket error %d\n", SocketError()); perror(0);
            return -1;
        }
    }
    else
    {
        // create UDP socket
        if ( (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 )
        {
            HAN_PRINT_ERROR("UDP socket error %d\n", SocketError()); perror(0);
            return -1;
        }
    }

    return sock;
}


void SocketClose(int sock)
{
    close(sock);
}

int SockBind(int sock, CONNECT_HANDLE pst_Handle)
{
    SocketConnectHandle *pst_ConnectHandle = (SocketConnectHandle *)pst_Handle;

    // bind to address
    if ( bind(sock, (const struct sockaddr *)&pst_ConnectHandle->serv_addr, sizeof(pst_ConnectHandle->serv_addr)) < 0 )
    {
        HAN_PRINT_ERROR("TCP socket bind error %d\n", SocketError()); perror(0);
        return 1;
    }

    return 0;
}

int SockBindAnyIP(int sock, unsigned int port)
{
    struct sockaddr_in myaddr;      /* our address */

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port);

    // bind to address
    if ( bind(sock, (const struct sockaddr *)&myaddr, sizeof(myaddr)) < 0 )
    {
        HAN_PRINT_ERROR("TCP socket bind error %d\n", SocketError()); perror(0);
        return 1;
    }

    return 0;
}

int SocketSend(int sock, const unsigned char *pu8Buffer, unsigned int size)
{
    int sent;

    // Send buffer till the end

    while (size > 0)
    {
        if ( (sent = send(sock, pu8Buffer, (int)size, 0)) < 0 )
        {
            HAN_PRINT_ERROR("TCP socket send error %d\n", SocketError()); perror(0);
            perror(0);
            return 1;
        }

        size -= sent;
    }

    return 0;
}


int SocketSendTo(int sock, const unsigned char *pu8_Buffer, unsigned int size, CONNECT_HANDLE pst_Handle)
{
    SocketConnectHandle *pst_ConnectHandle = (SocketConnectHandle *)pst_Handle;

    // Send buffer till the end
    if ( sendto(sock, pu8_Buffer, size, 0, (struct sockaddr *)&pst_ConnectHandle->serv_addr, sizeof(pst_ConnectHandle->serv_addr)) == -1 )
    {
        HAN_PRINT_ERROR("sendto failed! %d\n", SocketError()); perror(0);
        return 1;
    }

    return 0;
}


// if > 0,  Bytes received
// if == 0, Connection closed
// if < 0,  recv failed with error
int SocketRecv(int sock, unsigned char *pu8Buffer, unsigned int size,SOCKET_TYPE eType,struct sockaddr *cliaddres)
{
    int rec, err,len;

	if(eType == SOCK_TYPE_UDP)
	{
		len = sizeof(*cliaddres);

		rec = recvfrom(sock, pu8Buffer, size, 0, cliaddres, (socklen_t *)&len);
	}
	else
	{
    	rec = recv(sock, pu8Buffer, size, 0);
	}

    if ( rec < 0 )
    {
        err = SocketError();

        if (err == ENOTCONN)
        {
            HAN_PRINT_ERROR("Socket is not connected error\n");
            rec = 0;
        }
        else

            HAN_PRINT_ERROR("Socket receive error %d\n", err);
    }
    else if ( rec == 0 )
    {
        HAN_PRINT_ERROR("Socket closed\n");
    }

    return rec;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Address --
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CONNECT_HANDLE SocketConnectCreate(const char *IP, unsigned short  u16_Port)
{
    SocketConnectHandle *pst_ConnectHandle;

    if ( (pst_ConnectHandle = (SocketConnectHandle *)malloc(sizeof(SocketConnectHandle))) == NULL )
    {
        HAN_PRINT_ERROR("SocketSelectCreate: out of memory\n");
        return NULL;
    }

    memset(pst_ConnectHandle, 0, sizeof(*pst_ConnectHandle));

    pst_ConnectHandle->serv_addr.sin_family = AF_INET;

    if ( (pst_ConnectHandle->serv_addr.sin_addr.s_addr = inet_addr(IP)) == 0 )
    {
        HAN_PRINT_ERROR("inte_addr failed\n");
        free(pst_ConnectHandle);
        return NULL;
    }

    pst_ConnectHandle->serv_addr.sin_port = htons(u16_Port);

    return (CONNECT_HANDLE)pst_ConnectHandle;
}

void SocketConnectRelease(CONNECT_HANDLE pst_Handle)
{
    free(pst_Handle);
}

int SockConnect(int sock, CONNECT_HANDLE pst_Handle)
{
    SocketConnectHandle *pst_ConnectHandle = (SocketConnectHandle *)pst_Handle;

    // Connect to server.
    if ( connect(sock, (const struct sockaddr *)&pst_ConnectHandle->serv_addr, sizeof(pst_ConnectHandle->serv_addr)) < 0 )
    {
        HAN_PRINT_ERROR("TCP socket connect error %d\n", SocketError()); perror(0);
        return 1;
    }

    return 0;
}

// 0 - block
// 1 - non block
int SockNonBlock(int sock, unsigned int u32_NonBlock)
{

    long arg;

    // read mode
    if( (arg = fcntl(sock, F_GETFL, NULL)) < 0)
    {
        HAN_PRINT_ERROR("TCP socket fcntl get error %d\n", SocketError()); perror(0);
        return 1;
    }

    if (u32_NonBlock)
    arg |= O_NONBLOCK;
    else
    arg &= (~O_NONBLOCK);

    if( fcntl(sock, F_SETFL, arg) < 0)
    {
        HAN_PRINT_ERROR("TCP socket fcntl set error %d\n", SocketError()); perror(0);
        return 1;
    }

    return 0;
}


typedef struct {
    int sock;
    pfnConnectCB pfn_ConnCB;
    pfnStop pfn_Stop;
    char s_Host[URL_MAX_LEN];
    unsigned int u32_Timeout;
    unsigned short  u16_Port;
} stConnectCfg;

// allocator
static stConnectCfg* AllocConnectAsyncCfg(void)
{
    return (stConnectCfg *)malloc(sizeof(stConnectCfg));
}

static void FreeConnectAsyncCfg(stConnectCfg *pst_Cfg)
{
    free(pst_Cfg);
}

#define RECONNECT_COUNT     3

static void ConnectTaskProc(void *ProcParam)
{
    int rc;
    int err;
    socklen_t lon;
    int valopt;
    int res;
    struct timeval tv;
    fd_set set;
    stConnectCfg *pst_ConnCfg;
    SocketConnectHandle *pst_ConnectHandle = NULL;
    char sIP[IP_ADDR_LEN];
    int ReconnectCount = RECONNECT_COUNT;

    res = 0;

    pst_ConnCfg = (stConnectCfg *)ProcParam;

    // set socket non blocking
    SockNonBlock(pst_ConnCfg->sock, 1);

    HAN_PRINT_DEBUG("ConnectTaskProc"); 

reconnect:

    OSL_TASK_Sleep(3000);

    // try to resolve host name to IP
    if ( IpAddressByHostName(pst_ConnCfg->s_Host, sIP) != 0 )
    {
        HAN_PRINT_ERROR("ConnectTaskProc - could not resolve name to IP... retrying...");
        OSL_TASK_Sleep(3000);
        goto reconnect;
    }

    if ( pst_ConnectHandle )
    {
        SocketConnectRelease(pst_ConnectHandle);
    }

    if ( (pst_ConnectHandle = SocketConnectCreate(sIP, pst_ConnCfg->u16_Port)) == NULL )
    {
        HAN_PRINT_ERROR("ConnectTaskProc - SocketConnectCreate failed.");
        goto reconnect;
    }

    HAN_PRINT_DEBUG("ConnectTaskProc SocketConnectCreate OK"); 

    // trying to connect with timeout
    if ( (rc = connect(pst_ConnCfg->sock, (const struct sockaddr *)&pst_ConnectHandle->serv_addr, sizeof(pst_ConnectHandle->serv_addr))) < 0 )
    {
        if ((err = SocketError()) == EINPROGRESS || err == EWOULDBLOCK)
        {
            // wait for connection
            do
            {
                tv.tv_sec = 1;
                tv.tv_usec = 0;
                FD_ZERO(&set);
                FD_SET(pst_ConnCfg->sock, &set);

                rc = select(pst_ConnCfg->sock + 1, NULL, &set, NULL, &tv);

                HAN_PRINT_DEBUG("ConnectTaskProc select returned %d", rc); 

                if ( rc < 0 && (err = SocketError()) != EINTR)
                {
                    HAN_PRINT_ERROR("TCP async socket connect select error %d\n", err);
                    res = 1;
                    goto reconnect;
                }
                else if ( rc > 0 )
                {
                    // Socket selected for write
                    lon = sizeof(int);

                    if ( getsockopt(pst_ConnCfg->sock, SOL_SOCKET, SO_ERROR, (void *)(&valopt), &lon) < 0 )
                    {
                        HAN_PRINT_ERROR("TCP async socket connect getsockopt error %d\n", SocketError()); perror(0);
                        res = 1;
                        goto reconnect;
                    }

                    // Check the value returned...
                    if ( valopt )
                    {
                        HAN_PRINT_ERROR("TCP async socket connect getsockopt return error %d\n", valopt);
                        res = 1;
                        goto reconnect;
                    }
                    else
                        // connected
                        break;
                }
                else
                {
                    // timeout
                    HAN_PRINT_ERROR("TCP async socket connect timeout %d\n", 1);
                }

                if ( ReconnectCount == 0 )
                {
                    HAN_PRINT_DEBUG("ConnectTaskProc reconnecting..."); 
                    ReconnectCount = RECONNECT_COUNT;
                    goto reconnect;
                }

                ReconnectCount--;

            } while (!pst_ConnCfg->pfn_Stop());
        }
        else
        {
            HAN_PRINT_ERROR("TCP async socket connect error %d\n", SocketError()); perror(0);
            // error
            res = 1;
            goto reconnect; 
        }
    }
    else
    {
        HAN_PRINT_DEBUG("ConnectTaskProc connect returned %d",rc); 
    }

    // set socket blocking
    SockNonBlock(pst_ConnCfg->sock, 0);

    // free connection handler
    SocketConnectRelease(pst_ConnectHandle);

    // return the result
    pst_ConnCfg->pfn_ConnCB(res);

    FreeConnectAsyncCfg(pst_ConnCfg);
}

// Asynchronous connect with timeout (-1 infinite)
// create separate thread
// calls pfn_ConnectCB when connected
// check pfn_StopCB if should stop
int SockConnectAsync(int sock, const char *sHost, unsigned short  u16_Port, pfnConnectCB pfn_ConnectCB, pfnStop pfn_StopCB, unsigned int u32_Timeout)
{
    stConnectCfg *pst_Cfg;

    pst_Cfg = AllocConnectAsyncCfg();

    memset(pst_Cfg, 0, sizeof(*pst_Cfg));

    pst_Cfg->sock = sock;
    pst_Cfg->pfn_ConnCB = pfn_ConnectCB;
    pst_Cfg->pfn_Stop = pfn_StopCB;
    pst_Cfg->u32_Timeout = u32_Timeout;
    strncpy(pst_Cfg->s_Host, sHost, sizeof(pst_Cfg->s_Host));
    pst_Cfg->s_Host[sizeof(pst_Cfg->s_Host) - 1] = 0;
    pst_Cfg->u16_Port = u16_Port;

    // schedule connection task
    OSL_TASK_Create(ConnectTaskProc, pst_Cfg, 0, 0, 0);

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// -- Select --
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int MaxFD;
    fd_set ReadFDsSet;
    fd_set WriteFDsSet;
    int ReadFDs[FD_SETSIZE];
    unsigned int u32_ReadFDsSize;
    int WriteFDs[FD_SETSIZE];
    unsigned int u32_WriteFDsSize;
} SocketSelectHandle;


// create select handle
SELECT_HANDLE SocketSelectCreate(void)
{
    SocketSelectHandle *pSelHandle;

    if ( (pSelHandle = (SocketSelectHandle *)malloc(sizeof(SocketSelectHandle))) == NULL )
    {
        HAN_PRINT_ERROR("Select out of memory\n");
        return NULL;
    }

    return pSelHandle;
}

void SocketSelectSet(SELECT_HANDLE hHandle, const int *pReadFDs, unsigned int u32_ReadFDsSize, const int *pWriteFDs, unsigned int u32_WriteFDsSize)
{
    int i;
    SocketSelectHandle *pSelHandle = (SocketSelectHandle *)hHandle;

    memset(pSelHandle, 0, sizeof(*pSelHandle));
    // fill Read descriptors in

    FD_ZERO(&pSelHandle->ReadFDsSet);

    if ( u32_ReadFDsSize > 0 )
    {
        for (i = 0; i < (int)u32_ReadFDsSize; i++)
        {
            FD_SET(pReadFDs[i], &pSelHandle->ReadFDsSet);
            if ( pReadFDs[i] > pSelHandle->MaxFD )
                pSelHandle->MaxFD = pReadFDs[i];
        }

        memcpy(pSelHandle->ReadFDs, pReadFDs, u32_ReadFDsSize * sizeof(pSelHandle->ReadFDs[0]));
        pSelHandle->u32_ReadFDsSize = u32_ReadFDsSize;
    }

    // fill Write descriptors in in

    FD_ZERO(&pSelHandle->WriteFDsSet);

    if ( u32_WriteFDsSize > 0 )
    {
        for (i = 0; i < (int)u32_WriteFDsSize; i++)
        {
            FD_SET(pWriteFDs[i], &pSelHandle->WriteFDsSet);
            if ( pWriteFDs[i] > pSelHandle->MaxFD )
                pSelHandle->MaxFD = pWriteFDs[i];
        }

        memcpy(pSelHandle->WriteFDs, pWriteFDs, u32_WriteFDsSize * sizeof(pSelHandle->WriteFDs[0]));
        pSelHandle->u32_WriteFDsSize = u32_WriteFDsSize;
    }
}

// release select handle
// all FDs should be stopped
void SocketSelectRelease(SELECT_HANDLE handle)
{
    SocketSelectHandle *pSelHandle = (SocketSelectHandle *)handle;
    free(pSelHandle);
}

// Select for requested sockets
// pu32_Entries with FD_SETSIZE size
// timeout im ms
// return the number of selected
int SocketSelect(SELECT_HANDLE handle, unsigned int *pu32_ReadEntries, unsigned int *pu32_ReadSize, unsigned int *pu32_WriteEntries, unsigned int *pu32_WriteSize, unsigned int u32_Timeout)
{
    int i, j;
    int res;
    fd_set TmpWFDs;
    fd_set *pTmpWFDs;
    fd_set TmpRFDs;
    fd_set *pTmpRFDs;
    struct timeval tm;
    SocketSelectHandle *pSelHandle = (SocketSelectHandle *)handle;

    // set reads
    if ( pSelHandle->u32_ReadFDsSize != 0 )
    {
        TmpRFDs = pSelHandle->ReadFDsSet;
        pTmpRFDs = &TmpRFDs;
    }
    else
    {
        pTmpRFDs = NULL;
    }

    // set writes
    if ( pSelHandle->u32_WriteFDsSize != 0 )
    {
        TmpWFDs = pSelHandle->WriteFDsSet;
        pTmpWFDs = &TmpWFDs;
    }
    else
    {
        pTmpWFDs = NULL;
    }

    // timeout
    if ( u32_Timeout != 0 )
    {
        memset(&tm, 0, sizeof(tm));
        tm.tv_usec = u32_Timeout * 1000;
    }

    if ( (res = select(pSelHandle->MaxFD + 1, pTmpRFDs, pTmpWFDs, NULL, u32_Timeout ? &tm : NULL)) < 0 )
    {
        HAN_PRINT_ERROR("Select error %d\n", SocketError()); perror(0);
        return -1;
    }
    else if ( !res )
    {
        // timeout
        return 0;
    }

    // check reads
    if ( pu32_ReadEntries != NULL )
    {
        j = 0;

        for (i = 0; i < (int)pSelHandle->u32_ReadFDsSize; i++)
        {
            if ( FD_ISSET(pSelHandle->ReadFDs[i], &TmpRFDs) )
            {
                // save Socket ID
                pu32_ReadEntries[j++] = i;
            }
        }

        *pu32_ReadSize = j;
    }

    // check writes
    if ( pu32_WriteEntries != NULL )
    {
        j = 0;

        for (i = 0; i < (int)pSelHandle->u32_WriteFDsSize; i++)
        {
            if ( FD_ISSET(pSelHandle->WriteFDs[i], &TmpWFDs) )
            {
                // save Socket ID
                pu32_WriteEntries[j++] = i;
            }
        }

        *pu32_WriteSize = j;
    }

    return res;
}

int MACAddress(unsigned char *pu8_MAC)
{
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) // don't count loopback
    { /* handle error*/ };

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1)
    { /* handle error */ }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it)
    {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
        {
            if (! (ifr.ifr_flags & IFF_LOOPBACK))
            {
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
                {
                    success = 1;
                    break;
                }
            }
        }
        else
        { /* handle error */ }
    }

    if (success) memcpy(pu8_MAC, ifr.ifr_hwaddr.sa_data, 6);

    if ( sock >= 0 )
    {
        close(sock);
    }

    return 0;
}

int SetSocketRecvTimeout(int sock, unsigned int u32_TimeoutSec)
{
	struct timeval timeout;
	timeout.tv_sec = u32_TimeoutSec;
    timeout.tv_usec = 0;
	
    if ( setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0 )
    {
        HAN_PRINT_ERROR("setsockopt failed\n");
        return 1;
    }

    return 0;
}

int SocketSetRxBuffer(int sock, unsigned int Size)
{
    if ( setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)(&Size), sizeof(Size)) == -1 )
    {
        HAN_PRINT_ERROR("Error getting socket opts:\n");
        return 1;
    }

    return 0;
}

int SocketSetTxBuffer(int sock, unsigned int Size)
{
    if ( setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)(&Size), sizeof(Size)) == -1 )
    {
        HAN_PRINT_ERROR("Error getting socket opts:\n");
        return 1;
    }

    return 0;
}


int IpAddressByHostName(IN const char *sHostName, OUT char *sIP)
{
    int rc = 1;

    struct addrinfo *result = NULL;
    struct sockaddr_in  *sockaddr_ipv4;
    int error;

    // resolve the domain name into a list of addresses
    error = getaddrinfo(sHostName, NULL, NULL, &result);
    if (error != 0)
    {
        HAN_PRINT_ERROR("error in getaddrinfo: %s\n", gai_strerror(error));
    }
    else
    {
        // result may contain more than one address, we will take the first one...
        {
            char *IP;
            sockaddr_ipv4 = (struct sockaddr_in *)result->ai_addr;
            IP = inet_ntoa(sockaddr_ipv4->sin_addr);
            if ( IP )
            {
                strncpy(sIP, IP, IP_ADDR_LEN);
                rc = 0;
            }
            else
            {
                HAN_PRINT_ERROR("inet_ntoa failed");
            }
        }
    }

    // free resource
    if ( result )
    {
        freeaddrinfo(result);
    }

    return rc;
}

/* End Of File *****************************************************************************************************************************/
