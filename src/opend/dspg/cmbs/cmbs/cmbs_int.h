/*!
*  \file       cmbs_int.h
*  \brief      This file contains internal structures and definitions of CMBS
*
*  \author     stein
*
*  @(#)  %filespec: cmbs_int.h~12.1.12 %
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================\n
*  date        name     version   action                                          \n
*  ----------------------------------------------------------------------------\n
* 27-Jan-14 tcmc_asa  ---GIT-- change type of cmbs_int_SendChecksumError
* 13-Jan-2014 tcmc_asa  -- GIT--  take checksum changes from 3.46.x to 3_x main (3.5x)
* 20-Dec-13 tcmc_asa   GIT  Added CHECKSUM_SUPPORT    
* 25-Feb-09    stein    61       Restructuration                             \n
* 18-Feb-09    kelbch   3        add target build version to HOST API structure \n
* 16-Feb-09    kelbch   2        Integration to host/target build\n
*******************************************************************************/

#if   !defined( CMBS_INT_H )
#define  CMBS_INT_H

#include "cmbs_api.h"               /* CMBS API definition */
#include "cfr_ie.h"                 /* CMBS IE handling */
#include "cfr_mssg.h"

#if defined( __linux__ )
	#include <pthread.h>
	#include <unistd.h>
#endif

#ifdef WIN32
	#include "windows.h"
#endif

/* Local definitions */
#if defined(CHECKSUM_SUPPORT) && defined(CMBS_DEBUG)
# define CHECKSUMPRINT(x) printf x
#else
# define CHECKSUMPRINT(x)
#endif

#if !defined ( CMBS_API_TARGET )
#ifndef CMBS_NUM_OF_HOST_THREADS
#define CMBS_NUM_OF_HOST_THREADS 8
/* Current host threads:
-1- Callback
-2- HAN
-3- UART
-4- Reconnect (temporary thread)
-5- SUOTA
-6- Log
-7- main
-8- RESERVED/FUTURE
*/
#endif

#define CMBS_UNKNOWN_THREAD 0xFF

///////////////////////////


#endif
/*!
   brief endianess
*/
typedef enum
{
   E_CMBS_ENDIAN_LITTLE,            /*!< little endian */
   E_CMBS_ENDIAN_BIG,               /*!< big endian */
   E_CMBS_ENDIAN_MIXED              /*!< mixed endian */
} E_CMBS_ENDIAN;


/*!
   \brief return value of DnA CMBS framework
*/
typedef enum
{
   CFR_E_RETVAL_OK,                 /*!< successful return value*/
   CFR_E_RETVAL_ERR,                /*!< general error occured */
   CFR_E_RETVAL_ERR_MEM,            /*!< not enough memory available */

} CFR_E_RETVAL;

/*! currently default value of target module */

#define  B0		0000000		/* hang up */
#define  B50	0000001
#define  B75	0000002
#define  B110	0000003
#define  B134	0000004
#define  B150	0000005
#define  B200	0000006
#define  B300	0000007
#define  B600	0000010
#define  B1200	0000011
#define  B1800	0000012
#define  B2400	0000013
#define  B4800	0000014
#define  B9600	0000015
#define  B19200	0000016
#define  B38400	0000017

#define  B57600   0010001
#define  B115200  0010002
#define  B230400  0010003
#define  B460800  0010004
#define  B500000  0010005
#define  B576000  0010006
#define  B921600  0010007
#define  B1000000 0010010
#define  B1152000 0010011
#define  B1500000 0010012
#define  B2000000 0010013
#define  B2500000 0010014
#define  B3000000 0010015
#define  B3500000 0010016
#define  B4000000 0010017


/*!   \brief local application slot */
typedef struct
{
   void *            pv_AppRefHandle;        /*!< store application reference pointer */
   PFN_CMBS_API_CB   pFnAppCb;               /*!< store to be called function (reception of CMBS events)*/
   ST_CB_LOG_BUFFER  pFnCbLogBuffer;         /*!< storage for callback of log buffer */
   u16               u16_AppAPIVersion;      /*!< requested API version of application (further needed to get compatibility)*/
#if defined ( CMBS_API_TARGET )
   ST_CFR_IE_LIST    st_TransmitterIEList;   /*!< IE list of transmit direction */
   u8                u8_IEBuffers[1][CMBS_BUF_SIZE];  /*!< 1 buffer of a complete IE list (one buffer for each thread, max. 1 thread) */
#else
   ST_CFR_IE_LIST    st_TransmitterIEList[CMBS_NUM_OF_HOST_THREADS];          /*!< IE list of transmit direction */
   u8                u8_IEBuffers[CMBS_NUM_OF_HOST_THREADS][CMBS_BUF_SIZE];   /*!< 4 buffers of a complete IE list (one buffer for each thread, max. 4 threads) */
   u32               u32_ThreadIdArray[CMBS_NUM_OF_HOST_THREADS];             /*!< Array to store thread id */
#endif // defined ( CMBS_API_TARGET )
} ST_CMBS_API_SLOT, * PST_CMBS_API_SLOT;

/*!
   brief enum of commuication flow control
*/
typedef enum
{
   E_CMBS_FLOW_STATE_GO,            /*!< transmitter/receiver works fine */
   E_CMBS_FLOW_STATE_STOP,          /*!< transmitter/receiver is busy, no packet transmission*/
   E_CMBS_FLOW_STATE_MAX
} E_CMBS_FLOW_STATE;

/*! \brief CMBS API Instance (local)*/
typedef  struct
{
   ST_CMBS_API_SLOT  st_ApplSlot;
   u16               u16_TargetVersion;      /*!< target CMBS API version */
   E_CMBS_HW_CHIP    u8_HwChip;      		 /*!< HW chip */
   E_CMBS_HW_COM_TYPE      u8_HwComType;     /*!< HW communication */
   u32               u32_CallInstanceCount;
//#if !defined ( CMBS_API_TARGET )
   u16               u16_TargetBuild;        /*!< contains the build version of target side*/
//#endif
   E_CMBS_API_MODE   e_Mode;                 /*!< request CMBSMode */
   E_CMBS_FLOW_STATE e_OrigFlowState;        /*!< Originator transmission state */
   E_CMBS_FLOW_STATE e_DestFlowState;        /*!< Destination transmission state */
   E_CMBS_ENDIAN     e_Endian;               /*!< endianess: 0 = little */
#if defined( __linux__ )
   E_CMBS_DEVTYPE    eDevCtlType;            /*!< control device type properties*/
   int               fdDevCtl;               /*!< handle of such device */

   E_CMBS_DEVTYPE    eDevMediaType;          /*!< media device type properties */

   pthread_t         serialThreadId;         /*!< thread ID of serial pump */
   pthread_t         callbThreadId;          /*!< thread ID of callback function */

   pthread_mutex_t   cond_Mutex;
   pthread_cond_t    cond_UnLock;

   int               msgQId;                 /*!< message queue information*/
#else
# if defined ( WIN32 )
	E_CMBS_DEVTYPE    eDevCtlType;            /*!< control device type properties*/
	E_CMBS_DEVTYPE    eDevMediaType;          /*!< media device type properties */
	HANDLE            h_InitBlock;            /*!< handle to wait until CMBS is connected */
	HANDLE			  h_RecPath;			      /*!< handle of synchronization path */
	HANDLE			  h_RecThread;	  	      /*!< handle of receive data collector thread */
	DWORD			  dw_ThreadID;
	BOOL			  bo_Run;
# endif
#endif

   CFR_CMBS_CRITICALSECTION		h_CriticalSectionTransmission;  /*!< Critical section for transmission of packets */
   CFR_CMBS_CRITICALSECTION		h_TxThreadCriticalSection;

} ST_CMBS_API_INST, * PST_CMBS_API_INST;

/*!
   The following serialized message format shall be used:
      0    1    2    3      4    5      6    7      8    9      10   11
   | 0xda 0xda 0xda 0xda | 0xLO 0xHI | 0xLO 0xHI | 0xXX 0xYY | 0xLO 0xHI | parameter data
          sync.             total len   packet nr.   command     param len
*/
#define  CMBS_SYNC_LENGTH  4              /*!< Synchronization size */
#define  CMBS_SYNC          0xDADADADA    /*!< Synchronization value */

#define  CMBS_RCV_STATE_IDLE    0
#define  CMBS_RCV_STATE_SYNC    1
#define  CMBS_RCV_STATE_DATA    2

/*! \brief CMBS API Instance (local)*/


/*! \brief Union representing a complete serial cmbs api message
   the union uses to work on the message with byte or structure access */
typedef  union
{
   char           serialBuf[sizeof(u32)+sizeof(ST_CMBS_SER_MSG)];
   struct
   {
      u32         u32_Sync;
      ST_CMBS_SER_MSG
                  st_Msg;
   }st_Data;
} U_CMBS_SER_DATA, * PU_CMBS_SER_DATA;

#if defined(__linux__)
typedef  struct
{
   int            nLength;
   u8             u8_Data[CMBS_BUF_SIZE + sizeof(u32)];
} ST_CMBS_LIN_MSGDATA, * PST_CMBS_LIN_MSGDATA;

typedef  struct
{
   long           msgType;
   ST_CMBS_LIN_MSGDATA
                  msgData;
} ST_CMBS_LIN_MSG, * PST_CMBS_LIN_MSG;
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

extern ST_CMBS_API_INST g_CMBSInstance;
extern ST_CAPABLITIES   g_st_CMBSCapabilities;

/*****************************************************************************
 * CMBS API Internal functions
 *****************************************************************************/

E_CMBS_ENDIAN     cmbs_int_EndiannessGet( void );
u16               cmbs_int_EndianCvt16( u16 u16_Value );
u32               cmbs_int_EndianCvt32( u32 u32_Value );
void              cmbs_int_HdrEndianCvt( ST_CMBS_SER_MSGHDR *pst_Hdr );

// OS and environment dependent function
E_CMBS_RC         cmbs_int_EnvCreate( E_CMBS_API_MODE e_Mode, ST_CMBS_DEV * pst_DevCtl, ST_CMBS_DEV * pst_DevMedia );
E_CMBS_RC         cmbs_int_EnvDestroy( void );
// start-up blocking function to wait until CMBS is available
void              _cmbs_int_StartupBlockSignal( PST_CMBS_API_INST pst_CMBSInst );

E_CMBS_RC		  cmbs_int_SendHello(ST_CMBS_DEV * pst_DevCtl, ST_CMBS_DEV * pst_DevMedia);
E_CMBS_RC		  cmbs_int_SendRAMDumpStoreCmd(void);
#if !defined( CMBS_API_TARGET )
E_CMBS_RC		  cmbs_int_WaitForResponse(u32 u32_TimeoutMs);
#endif

void *            cmbs_int_RegisterCb( void * pv_AppRef, PFN_CMBS_API_CB pfn_api_Cb, u16 u16_bcdVersion );
void              cmbs_int_RegisterLogBufferCb( void * pv_AppRef, PST_CB_LOG_BUFFER pfn_log_buffer_Cb );
void              cmbs_int_UnregisterCb( void * pv_AppRefHandle );

u16               cmbs_int_ModuleVersionGet( void );
u16				  cmbs_int_ModuleVersionBuildGet( void );

E_CMBS_RC         cmbs_int_EventSend( E_CMBS_EVENT_ID e_EventID, u8 *pBuf, u16 u16_Length );
void              cmbs_int_EventReceive ( u8 * pu8_Mssg, u16 u16_Size );

E_CMBS_RC         cmbs_int_ResponseSend( E_CMBS_EVENT_ID e_ID, E_CMBS_RESPONSE e_RSPCode );
E_CMBS_RC         cmbs_int_ResponseWithCallInstanceSend(	E_CMBS_EVENT_ID e_ID, 
															E_CMBS_RESPONSE e_RSPCode, 
															u32 u32CallInstance);
E_CMBS_RC         cmbs_int_RespWithCallInstChannelID(E_CMBS_EVENT_ID e_ID,
                                                  E_CMBS_RESPONSE e_RSPCode,
                                                  u32 u32CallInstance,
                                                  PST_IE_MEDIA_CHANNEL st_MediaChannel);

E_CMBS_RC         cmbs_int_cmd_Send( u8 u8_Cmd, u8 * pBuf, u16 u16_Length );
void              cmbs_int_cmd_Dispatcher( u8 u8_Cmd, u8 * pu8_Buffer, u16 u16_Size );
void              cmbs_int_cmd_ReceiveEarly ( u8 * pu8_buffer );
void			  _cmbs_int_SuspendTxCommands( void );
void			  _cmbs_int_SuspendTxCommandswithoutSendingRES( void );
void			  _cmbs_int_ResumeTxCommands( void );
void			  _cmbs_int_ResumeTxCommandswithoutSendingRES( void );
void              cmbs_int_cmd_FlowRestartHandle ( u16 u16_Packet );
void              cmbs_int_cmd_FlowNOKHandle ( u16 u16_Packet );
u8                cmbs_int_cmd_FlowStateGet ( void );
u32 			  cmbs_cmd_parser( CMBS_CMD id, u8* pu8_Buffer, u8* pu8_Buffer_parse, u32 u32_Pos);
	
#ifdef CHECKSUM_SUPPORT
void      p_cmbs_int_CalcChecksum(u8 * pCheckSum, u8 * pBuf, u16 u16_Length);
u8        p_cmbs_int_ChecksumVerify(u8 u8_Checksum[2],  u8 * pBuf, u16 u16_Length);
E_CMBS_RC cmbs_int_SendChecksumError(E_CMBS_CHECKSUM_ERROR e_ErrorType, u16 u16_EventID);
void      cmbs_int_SimulateChecksumError(char u8_ErrorType);
#endif
void cmbs_int_cmd_SendCapablities (void);
void cmbs_int_cmd_SendCapablitiesReply (void);
void cmbs_int_ParseDectMsg(u8 * buff, u8 u8_ILen, u8 u8_HandsetNumber);


E_CMBS_RC         cmbs_int_ie_RingTypeGet( void * pv_RefIE, u32 * pu32_Value );
#if defined( __cplusplus )
}
#endif

#endif   // CMBS_INT_H

// EOF
