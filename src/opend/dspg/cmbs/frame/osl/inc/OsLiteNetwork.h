#ifndef __NETWORK__
#define __NETWORK__


#ifndef WIN32
#include <netinet/in.h>
#else
#include <Winsock2.h>
#endif

typedef void * SELECT_HANDLE;
typedef void * CONNECT_HANDLE; 


int InitNetwork(void);
void ReleaseNetwork(void);
int SocketError(void);

typedef enum
{
    SOCK_TYPE_TCP,
    SOCK_TYPE_UDP
} SOCKET_TYPE; 


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Socket
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int SockCreate(SOCKET_TYPE eType);
void SocketClose(int sock);
CONNECT_HANDLE SocketConnectCreate(const char *IP, unsigned short u16_Port);
void SocketConnectRelease(CONNECT_HANDLE pst_Handle);

int IpAddressByHostName(IN const char *sHostName, OUT char *sIP);

typedef void (*pfnConnectCB)(int res);
typedef int (*pfnStop)(void);

// Asynchronous connect with timeout (-1 infinite)
// create separate thread
// calls pfn_ConnectCB when connected
// check pfn_StopCB if should stop 
int SockConnectAsync(int sock, const char* sHost, unsigned short u16_Port,pfnConnectCB pfn_ConnectCB, pfnStop pfn_StopCB, unsigned int u32_Timeout);

int SockConnect(int sock, CONNECT_HANDLE pst_Handle);
int SockBind(int sock, CONNECT_HANDLE pst_Handle);
int SockBindAnyIP(int sock, unsigned intport);
int SocketSend(int sock, const unsigned char*pu8Buffer, unsigned intsize);
int SocketSendTo(int sock, const unsigned char*pu8_Buffer, unsigned intsize, CONNECT_HANDLE pst_Handle);
int SocketSetRxBuffer(int sock, unsigned intSize); 
int SocketSetTxBuffer(int sock, unsigned intSize); 

// if > 0,  Bytes received
// if == 0, Connection closed
// if < 0,  recv failed with error
int SocketRecv(int sock, unsigned char *pu8Buffer, unsigned int size,SOCKET_TYPE eType,struct sockaddr *cliaddres);

/////////////////////////////////////////
//// Select
/////////////////////////////////////////

// create select handle
SELECT_HANDLE SocketSelectCreate(void);

// set FDs
void SocketSelectSet(SELECT_HANDLE hHandle, const int *pReadFDs, unsigned int u32_ReadFDsSize, const int *pWriteFDs, unsigned int u32_WriteFDsSize);

// release select handle
// all FDs should be stopped
void SocketSelectRelease(SELECT_HANDLE handle);

// Select for requested sockets
// pu32_Entries with FD_SETSIZE size
// timeout im ms
// return the number of selected
int SocketSelect(SELECT_HANDLE handle, unsigned int*pu32_ReadEntries, unsigned int*pu32_ReadSize, unsigned int*pu32_WriteEntries, unsigned int*pu32_WriteSize, unsigned int u32_Timeout);

// set timeout for socket recieve (0 for infinite)
int SetSocketRecvTimeout(int sock, unsigned int u32_TimeoutSec);

#endif // __NETWORK__
