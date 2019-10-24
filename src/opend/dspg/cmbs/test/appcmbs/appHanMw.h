/*!
*   \file       appHanMW.h
*   \brief      HAN MW API
*   \author     HAN Team
*
*
*******************************************************************************/

#if	!defined( _APPHANMW_H )
#define	_APPHANMW_H

#include "cmbs_han.h"


E_CMBS_RC app_HanMwMsgSend_ImplicitLinkSetup (u16 u16_RequestId, u16 u16_DestDeviceId, ST_IE_HAN_MSG_CTL* pst_HANMsgCtl , ST_IE_HAN_MSG * pst_HANMsg );
void app_HanMWDeleteDeviceViaQueue(u16 u16_deviceId, bool b_BlackList);
void app_HanMWInitialize(void);
void app_HanMWReleaseLinkRequest(u16 u16_deviceId);
void app_HanMWHwVersionGet(void);
void app_HanMWSwVersionGet(void);
void app_HanMWGetNumOfFunMsgInQ(u16 u16_deviceId);
void app_HanMWClearFunMsgQ(u16 u16_deviceId);

typedef void (*t_fptrHANClientOnRegClosed)( E_CMBS_EVENT_ID , void * );
typedef void (*t_fptrHANClientOnReadDeviceTableRes)( void * );
typedef void (*t_fptrHANClientOnReadDeviceSubInfoRes)( void * );
typedef u32 (*t_fptrHANClientOnMsgRecv)( void * , ST_IE_HAN_MSG *);
typedef u32 (*t_fptrHANClientOnDeviceFullDeleted)( u16, ST_IE_RESPONSE, u16 );
typedef u32 (*t_fptrHANClientOnDeviceRegistered)( u16 );
typedef u32 (*t_fptrHANClientOnDevicePartialDeleted)( u16 );
typedef u32(*t_fptrHANClientOnOpenRegRes)(int);
typedef u32(*t_fptrHANClientOnCloseRegRes)(int);
typedef u32(*t_fptrHANClientOnHandsetRegistered)(u8);
typedef u32(*t_fptrHANClientOnTargetUpIndication)();
typedef u32(*t_fptrHANClientOnEepParamGetRes)(bool, u8 *, u16,u8 *);
typedef u32(*t_fptrHANClientOnEepParamSetRes)(bool, u8 *);
typedef u32(*t_fptrHANClientOnEepAreaGetRes)(bool, u32, u16, u8 *);
typedef u32(*t_fptrHANClientOnEepAreaSetRes)(bool);
typedef u32(*t_fptrHANClientOnProdParamGetRes)(bool, u8 *, u16,u8 *);
typedef u32(*t_fptrHANClientOnProdParamSetRes)(bool, u8 *);
typedef u32(*t_fptrHANClientOnRamAreaGetRes)(bool, u32, u16, u8 *);
typedef u32(*t_fptrHANClientOnRamAreaSetRes)(bool);
typedef u32(*t_fptrHANClientOnEepromSizeGetRes)(bool, u32);
typedef u32(*t_fptrHANClientOnHwVersionGetRes)(bool, u8 *, u8 *, u8 *, u8 *);
typedef u32(*t_fptrHANClientOnSwVersionGetRes)(bool, u16, u16, u16, u16);
typedef u32(*t_fptrHANClientOnGetNumOfFunMsgInQRes)(bool, u16, u8, u8);
typedef u32(*t_fptrHANClientOnMessageSendRes)(bool, u16);
typedef void (*t_fptrHANClientOnReadDeviceTablePhase2Res)( void * );
typedef void (*t_fptrHANClientOnReadDeviceSubInfoPhase2Res)( void * );
typedef void (*t_fptrHANClientOnReadBlackListDeviceTableRes)( void * );
typedef u32 (*t_fptrHANClientOnDeviceDeleted)( u16 );

typedef struct s_fptrHanClient
{
	t_fptrHANClientOnRegClosed 						g_fptrHANClientOnRegClosed;
	t_fptrHANClientOnReadDeviceTableRes 			g_fptrHANClientOnReadDeviceTableRes;
	t_fptrHANClientOnReadDeviceSubInfoRes 			g_fptrHANClientOnReadDeviceSubInfoRes;
	t_fptrHANClientOnMsgRecv 						g_fptrHANClientOnMsgRecv;
	t_fptrHANClientOnDeviceFullDeleted 				g_fptrHANClientOnDeviceFullDeleted;
	t_fptrHANClientOnDeviceRegistered 				g_fptrHANClientOnDeviceRegistered;
	t_fptrHANClientOnDevicePartialDeleted 			g_fptrHANClientOnDevicePartialDeleted;
	t_fptrHANClientOnOpenRegRes 					g_fptrHANClientOnOpenRegRes;
	t_fptrHANClientOnCloseRegRes 					g_fptrHANClientOnCloseRegRes;
	t_fptrHANClientOnHandsetRegistered 				g_fptrHANClientOnHandsetRegistered;
	t_fptrHANClientOnTargetUpIndication 			g_fptrHANClientOnTargetUpIndication;
	t_fptrHANClientOnEepParamGetRes					g_fptrHANClientOnEepParamGetRes;
	t_fptrHANClientOnEepParamSetRes					g_fptrHANClientOnEepParamSetRes;
	t_fptrHANClientOnEepAreaGetRes					g_fptrHANClientOnEepAreaGetRes;
	t_fptrHANClientOnEepAreaSetRes					g_fptrHANClientOnEepAreaSetRes;
	t_fptrHANClientOnProdParamGetRes				g_fptrHANClientOnProdParamGetRes;
	t_fptrHANClientOnProdParamSetRes				g_fptrHANClientOnProdParamSetRes;
	t_fptrHANClientOnRamAreaGetRes					g_fptrHANClientOnRamAreaGetRes;
	t_fptrHANClientOnRamAreaSetRes					g_fptrHANClientOnRamAreaSetRes;	
	t_fptrHANClientOnEepromSizeGetRes 				g_fptrHANClientOnEepromSizeGetRes;
	t_fptrHANClientOnHwVersionGetRes				g_fptrHANClientOnHwVersionGetRes;	
	t_fptrHANClientOnSwVersionGetRes				g_fptrHANClientOnSwVersionGetRes;
	t_fptrHANClientOnGetNumOfFunMsgInQRes			g_fptrHANClientOnGetNumOfFunMsgInQRes;
	t_fptrHANClientOnMessageSendRes					g_fptrHANClientOnMessageSendRes;
	t_fptrHANClientOnReadDeviceTablePhase2Res 		g_fptrHANClientOnReadDeviceTablePhase2Res;
	t_fptrHANClientOnReadDeviceSubInfoPhase2Res 	g_fptrHANClientOnReadDeviceSubInfoPhase2Res;
	t_fptrHANClientOnReadBlackListDeviceTableRes	g_fptrHANClientOnReadBlackListDeviceTable;
	t_fptrHANClientOnDeviceDeleted 					g_fptrHANClientOnDeviceDeleted ;
	
}s_fptrHanClient;


void app_HanMWRegisterCallBacks(s_fptrHanClient* HanClientRegisterCallBacks);

E_CMBS_RC app_HanMwDeviceReadTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8 isBrief);
E_CMBS_RC app_HanMwReadSingleDeviceRegistrationInformation(u16 u16_DeviceId);
E_CMBS_RC app_HanMwDeviceReadTablePhase2(u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);
E_CMBS_RC app_HanMwReadSingleDeviceRegistrationInformationPhase2(u16 u16_DeviceId);
E_CMBS_RC app_HanMwDeviceReadBlackListTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry);


typedef enum
{
	NO_MSG_IN_QUEUE = 0,
	FOUND_MSG_IN_QUEUE,
} E_MSG_IN_QUEUE_STATUS;





#endif // _APPHANMW_H

/**********************[End Of File]**********************************************************************************************************/
