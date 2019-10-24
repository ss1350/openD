/*!
*   \file       apphan.h
*   \brief      HAN API
*   \author     CMBS Team
*
*
*******************************************************************************/

#if	!defined( _APPHAN_H )
#define	_APPHAN_H

#include "cmbs_han.h"
#include "hanfun_protocol_defs.h"
#include "cmbs_fifo.h"

int app_HANEntity ( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData );
E_CMBS_RC	app_DsrHanMngrInit			( ST_HAN_CONFIG * pst_HANConfig																	);
E_CMBS_RC	app_DsrHanMngrStart			( void																							);
E_CMBS_RC	app_DsrHanDeviceReadTable	(u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8 isBrief										);
E_CMBS_RC app_DsrHanReadExtendedDeviceTablePhase2 (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
E_CMBS_RC	app_DsrHanDeviceWriteTable	(u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_DEVICE_ENTRY * pst_HANDeviceEntriesArray);
E_CMBS_RC	app_DsrHanBindReadTable		(u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry												);
E_CMBS_RC	app_DsrHanBindWriteTable		(u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_BIND_ENTRY * pst_HANBindEntriesArray	);
E_CMBS_RC app_DsrHanBindAddEntry (PST_HAN_BIND_ENTRY pst_HANBindEntry);
E_CMBS_RC app_DsrHanBindRemoveEntry(PST_HAN_BIND_ENTRY pst_HANBindEntry);
E_CMBS_RC app_DsrHanClearBindTable( void );
E_CMBS_RC app_DsrHanGroupReadTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u16 u16_GroupId);
E_CMBS_RC 	app_DsrHanGroupReadList (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry														);
E_CMBS_RC app_DsrHanReadBroadcastCahnnelTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
E_CMBS_RC	app_DsrHanGroupWriteTable	(u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, ST_HAN_GROUP_TABLE_ENTRY * pst_HANGroupEntriesArray	);
E_CMBS_RC app_DsrHanGroupCreateGroup ( u8 u8_BroadcastChannelId );
E_CMBS_RC app_DsrHanGroupRemoveGroup ( u16 u16_GroupId );
E_CMBS_RC app_DsrHanGroupAddDeviceToGroup(u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId);
E_CMBS_RC app_DsrHanGroupRemoveDeviceFromGroup(u16 u16_GroupId, u16 u16_DeviceId, u8 u8_UnitId);
E_CMBS_RC	app_DsrHanMsgRecvRegister	(ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);
E_CMBS_RC	app_DsrHanMsgRecvUnregister (ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);
E_CMBS_RC	app_DsrHanMsgSendTxRequest	(u16 u16_DeviceId						);
E_CMBS_RC	app_DsrHanMsgSendTxEnd		(u16 u16_DeviceId						);
E_CMBS_RC	app_DsrHanMsgSend			(u16 u16_RequestId, u16 u16_DestDeviceId, ST_IE_HAN_MSG_CTL* pst_HANMsgCtl ,ST_IE_HAN_MSG * pst_HANMsg);
E_CMBS_RC	app_DsrHanRawMsgSend		(u16 u16_RequestId, u16 u16_DestDeviceId, ST_IE_HAN_MSG_CTL* pst_HANMsgCtl ,ST_IE_DATA * pst_HANRawMsg);
E_CMBS_RC	app_DsrHanDeleteDevice		(u16 u16_DeviceId , bool b_BlackList);
E_CMBS_RC app_DsrHanSendBaseUpdatedNotification ( u16 u16_DeviceId );
E_CMBS_RC app_DsrHanGetFunProtocolInfo ();
E_CMBS_RC app_DsrHanGetDeviceConnectionStatus ( u16 u16_DeviceId );
E_CMBS_RC app_DsrHanReadSingleDeviceRegistrationInformation(u16 u16_DeviceId);
E_CMBS_RC app_DsrHanReadSingleDeviceRegistrationInformationPhase2(u16 u16_DeviceId);
E_CMBS_RC app_DsrHan_HanSetLogLevel(u8 u8_LogLevel);
E_CMBS_RC app_DsrHan_HanGetLogLevel(void);
E_CMBS_RC app_DsrHanLoggerEnable(bool b_enable);
E_CMBS_RC app_DsrHanCpLoggerEnable(bool b_enable);
E_CMBS_RC app_DsrHanNotifyOnVoiceCall(u16 u16_DeviceId);
E_CMBS_RC app_DsrHanGetMaxNumOfDevices(void);
E_CMBS_RC app_DsrHanReadBlackListedDeviceTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
E_CMBS_RC app_DsrHanMarkCcmKeyAsUsed( u16 u16_DeviceNumber , u8 u8_CCMKeyUsed);
E_CMBS_RC app_DsrHanSetTxMicCorruptNum(u16 u16_DeviceNumber);
E_CMBS_RC app_DsrHanSetTxSeqNum(u16 u16_DeviceNumber, u16 u16_TxSequenceNumber );
E_CMBS_RC app_DsrHanGetTxSeqNum(u16 u16_DeviceNumber);
E_CMBS_RC app_DsrHanGetRxSeqNum(u16 u16_DeviceNumber);
E_CMBS_RC app_DsrHanSetTxAckDropNum(u16 u16_TxAckDropNumber);
E_CMBS_RC app_DsrHanSetPvcState(u16 u16_DeviceNumber, u8 u8_PVCState );

E_CMBS_RC app_DsrHanSetRxGain(u8 u8_RxGainVal );
E_CMBS_RC app_DsrHanGetRxGain(void );	
E_CMBS_RC app_DsrHanSendPvcResetReq( u16 u16_DeviceNumber );
E_CMBS_RC app_DsrHanGetPvcResetReqState( u16 u16_DeviceNumber );
E_CMBS_RC app_DsrHanReadFullDeviceRegistrationInfo(u16 u16_DeviceId);
E_CMBS_RC app_DsrHanWriteFullDeviceRegistrationInfo(u16 u16_DeviceId, ST_HAN_FULL_DEVICE_REGISTRATION_INFO *pst_DeviceRegistrationInfo);

E_CMBS_RC app_DsrHanTBR6SetPM(u8  u8_Carrier , u32 u32_Period );
E_CMBS_RC app_DsrHanTBR6SetGUCI(u8 *  pIPUI );
E_CMBS_RC app_DsrHanTBR6ResetStatistics(void );
E_CMBS_RC app_DsrHanModificationULEPagingInterval (u16 u16_DeviceId, u32 u32_ULEPagingIntervalMs );
E_CMBS_RC app_DsrHanNodeSetupAttemptsEnable (bool b_enable);


void SetWaitForReady(u16 u16_deviceId, bool Mode);
bool GetWaitForReady(u16 u16_deviceId);
void SetDuringDeleteReq(bool status);
bool SetIfNotAlreadyDuringDeleteReq(void);

void SetReleaseLink(u16 u16_deviceId, bool ReleaseLinkMode);
bool GetReleaseLink(u16 u16_deviceId);

PST_CMBS_FIFO Get_UleMsgFifo(u16 u16_deviceId);



void app_HanPushPendingDeviceToDelete(u16 u16_deviceId, bool b_BlackList);
u16 app_HanPopPendingDeviceToDelete();
u8 app_HanDbGetFunMsgQSizeForDevice();
void app_DsrHanVoiceCallReq (u16 u16_DstDeviceId, u8 u8_DstUnitId, PST_HAN_VOICE_CALL_REQ_PARAMS Pst_CallReqParams);

void app_HanOnFunMsgReceived(void *pv_EventData, ST_IE_HAN_MSG * pstIe_Msg );
void app_HanOnRawMsgReceived(void *pv_EventData, ST_IE_DATA * pstIe_RawMsg );
void app_HanRegularStart( u8 RegisterForHanMessages );
void app_HanPrintMessageFields(ST_IE_HAN_MSG* pMsg);
void app_HanPrintRawMessageFields(ST_IE_DATA* pMsg, u16 u16_DeviceId);
u8  app_HAN_GetHANConfiguration( void );
void app_HAN_SetHANConfiguration(u8 u8_HANServiceConfig);


u8  app_HAN_GetHANConfiguration( void );
void app_HAN_SetHANConfiguration(u8 u8_HANServiceConfig);
void app_HANRestoreSequenceNumbers (void);






void app_HanDbInitialize();
#ifdef WIN32
DWORD app_tempThread(LPVOID lpThreadParameter);
#elif __linux__
void* app_tempThread(void *ptr);
#endif


#define		MAX_ULE_DEVICES_HOST	1001	// Maximal number of ULE that host supports + 1 (to include also device 0 that is not used today)
#define		ALL_ULE_DEVICES_HOST	0xFFFF	// All devices as a group
#define		NO_ULE_DEVICE_HOST		0xFFFE	// for internal use only




// Test Mode interface
#define HAN_IFACE_TESTMODE										0x7F01
// server attributes
#define HAN_IFACE_TESTMODE_ATTR_BULK_SIZE						1 
#define HAN_IFACE_TESTMODE_ATTR_BULK_INTERVAL					2 
#define HAN_IFACE_TESTMODE_ATTR_PACKET_INTERVAL					3 
// commands client to server
#define HAN_IFACE_TESTMODE_CMD_ENABLE							1 
#define HAN_IFACE_TESTMODE_CMD_TXTDD_START						2 
// needed for payload
#define HAN_IFACE_TESTMODE_CMD_ENABLE_VALUE_DISABLE				0// payload of one Byte for the command (0 Disable , 1 Enable)
#define HAN_IFACE_TESTMODE_CMD_ENABLE_VALUE_ENABLE				1
// MEMORY Access interface
#define HAN_IFACE_MEMORY_ACCESS									0x7F04
// commands client to server
#define HAN_IFACE_EEPROM_ACCESS_CMD_GET							1 
#define HAN_IFACE_EEPROM_ACCESS_CMD_SET							2 
// Interface direction
#define HAN_IFACE_DIRECTION_SERVER								1
#define HAN_IFACE_DIRECTION_CLIENT								0
#define MEMORY_ACCESS_CMND_PAYLOAD_HEADER_SIZE					7
#define MEMORY_ACCESS_CMND_PAYLOAD_MAX_SIZE						500

#define HAN_TEST_DIALOG			3


typedef  struct
{
	ST_IE_HAN_MSG st_HANmsg;
   	u8 paylod[CMBS_HAN_MAX_MSG_LEN*2];
} ST_IE_FULL_HAN_MSG , * PST_IE_FULL_HAN_MSG;

typedef struct
{
    bool b_Response;
    u16  u16_deviceID;
    u16 u16_status;
}ST_CMBS_IE_RES, *PST_CMBS_IE_RES;




typedef struct								// Msg Struct to be saved in message Queue
{
	ST_IE_HAN_MSG_CTL		st_HANMsgCtl;
	ST_IE_HAN_MSG			st_HANMsg;
	u8 						Payload[CMBS_HAN_MAX_MSG_DATA_LEN];
}ST_FUN_MSG, *PST_FUN_MSG;


#endif	// _APPHAN_H

/**********************[End Of File]**********************************************************************************************************/
