/*!
*   \file       AppHanMW.c
*   \brief      HAN MW API
*   \author     HAN Team
*
*
*******************************************************************************/

#include "appHanMw.h"
#include "cmbs_api.h"
#include "apphan.h"
#include "appsrv.h"
#include "apphanDb.h"
#include "cmbs_han_ie.h"
#include "appcmbs.h"
#include "cfr_debug.h"
#include "tcx_hostlog.h"
#include "tcx_util.h"

void app_HanMWOnDsrMsgSendRes 	(void* pv_EventData);
void app_HanMWOnDeviceBlackListDeleted( void* pv_EventData );
void app_HanMWOnDeviceRegistered( void* pv_EventData );
void app_HanMWOnSendTxReady( void* pv_EventData );
void app_HanMWOnStartTxRequestRes(void* pv_EventData);
void app_HanMWOnTxEnded( void* pv_EventData );
void app_HanMWOnDevicePartialDeleted( void* pv_EventData );
E_MSG_IN_QUEUE_STATUS	app_HanMWSendDsrMsgFromFifo(u16 u16_deviceId, u16 u16_RequestId);
void app_HanMWOnOpenRegRes(void *pv_List);
void app_HanMWOnCloseRegRes(void *pv_List);
void app_HanMWOnHandsetRegistered(void *pv_List);
void app_HanMWOnParamGetRes(void *pv_List);
void app_HanMWOnParamSetRes(void *pv_List);
void app_HanMWOnParamAreaGetRes(void *pv_List);
void app_HanMWOnParamAreaSetRes(void *pv_List);
void app_HanMWOnEepromSizeGetRes(void *pv_List);
void app_OnHandsetRegistered(void *pv_List);
void app_HanMWOnHwVersionGetRsp(void *pv_List);
void app_HanMWOnDeviceDeleted( void* pv_EventData );

static u32 g_u32_MessageRequestId = 1;

//callback function
static t_fptrHANClientOnRegClosed 					g_fptrHANClientOnRegClosed=NULL;
static t_fptrHANClientOnReadDeviceTableRes 			g_fptrHANClientOnReadDeviceTableRes=NULL;
static t_fptrHANClientOnReadDeviceSubInfoRes 		g_fptrHANClientOnReadDeviceSubInfoRes=NULL;
static t_fptrHANClientOnMsgRecv 					g_fptrHANClientOnMsgRecv=NULL;
static t_fptrHANClientOnDeviceFullDeleted 			g_fptrHANClientOnDeviceFullDeleted=NULL;
static t_fptrHANClientOnDeviceRegistered 			g_fptrHANClientOnDeviceRegistered =NULL;
static t_fptrHANClientOnDevicePartialDeleted 		g_fptrHANClientOnDevicePartialDeleted=NULL;
static t_fptrHANClientOnOpenRegRes 					g_fptrHANClientOnOpenRegRes=NULL;
static t_fptrHANClientOnCloseRegRes 				g_fptrHANClientOnCloseRegRes=NULL;
static t_fptrHANClientOnHandsetRegistered 			g_fptrHANClientOnHandsetRegistered=NULL;
static t_fptrHANClientOnTargetUpIndication 			g_fptrHANClientOnTargetUpIndication=NULL;
static t_fptrHANClientOnEepParamGetRes 				g_fptrHANClientOnEepParamGetRes = NULL;
static t_fptrHANClientOnEepParamSetRes 				g_fptrHANClientOnEepParamSetRes = NULL;
static t_fptrHANClientOnEepAreaGetRes 				g_fptrHANClientOnEepAreaGetRes = NULL;
static t_fptrHANClientOnEepAreaSetRes 				g_fptrHANClientOnEepAreaSetRes = NULL;
static t_fptrHANClientOnProdParamGetRes 			g_fptrHANClientOnProdParamGetRes = NULL;
static t_fptrHANClientOnProdParamSetRes 			g_fptrHANClientOnProdParamSetRes = NULL;
static t_fptrHANClientOnRamAreaGetRes 				g_fptrHANClientOnRamAreaGetRes = NULL;
static t_fptrHANClientOnRamAreaSetRes 				g_fptrHANClientOnRamAreaSetRes = NULL;
static t_fptrHANClientOnEepromSizeGetRes 			g_fptrHANClientOnEepromSizeGetRes = NULL;
static t_fptrHANClientOnHwVersionGetRes				g_fptrHANClientOnHwVersionGetRes = NULL;
static t_fptrHANClientOnSwVersionGetRes				g_fptrHANClientOnSwVersionGetRes = NULL;
static t_fptrHANClientOnGetNumOfFunMsgInQRes 		g_fptrHANClientOnGetNumOfFunMsgInQRes=NULL;
static t_fptrHANClientOnMessageSendRes 				g_fptrHANClientOnMessageSendRes =NULL;
static t_fptrHANClientOnReadDeviceTablePhase2Res 	g_fptrHANClientOnReadDeviceTablePhase2Res=NULL;
static t_fptrHANClientOnReadDeviceSubInfoRes 		g_fptrHANClientOnReadDeviceSubInfoPhase2Res=NULL;
static t_fptrHANClientOnReadBlackListDeviceTableRes g_fptrHANClientOnReadBlackListDeviceTable=NULL;
static t_fptrHANClientOnDeviceDeleted 				g_fptrHANClientOnDeviceDeleted = NULL;

#define HAN_APP_MW_INFO_PRINT(format, ...) 						tcx_WriteLog(HAN_MW_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define HAN_APP_MW_TRACE_PRINT(format, ...) 					tcx_WriteLog(HAN_MW_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define HAN_APP_MW_WARNING_PRINT(format, ...) 					tcx_WriteLog(HAN_MW_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define HAN_APP_MW_ERROR_PRINT(format, ...) 					tcx_WriteLog(HAN_MW_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define HAN_APP_MW_RT_PRINT(format, ...) 						tcx_WriteLog(HAN_MW_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define HAN_APP_MW_PRINT_DATA(pu8_Buffer, u16_Length) 		tcx_WriteLogData(HAN_MW_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)


#define APPHANMW_DEVICE_ID_MASK 0x7FFF
#define APPHANMW_DEL_TYPE_MASK 	0x8000


void app_HanMWRegisterCallBacks(s_fptrHanClient* HanClientRegisterCallBacks)
{
	
	g_fptrHANClientOnRegClosed = HanClientRegisterCallBacks->g_fptrHANClientOnRegClosed;
	g_fptrHANClientOnReadDeviceTableRes = HanClientRegisterCallBacks->g_fptrHANClientOnReadDeviceTableRes;
	g_fptrHANClientOnReadDeviceSubInfoRes = HanClientRegisterCallBacks->g_fptrHANClientOnReadDeviceSubInfoRes;
	g_fptrHANClientOnMsgRecv = HanClientRegisterCallBacks->g_fptrHANClientOnMsgRecv;
	g_fptrHANClientOnDeviceFullDeleted = HanClientRegisterCallBacks->g_fptrHANClientOnDeviceFullDeleted;
	g_fptrHANClientOnDeviceRegistered = HanClientRegisterCallBacks->g_fptrHANClientOnDeviceRegistered;
	g_fptrHANClientOnDevicePartialDeleted = HanClientRegisterCallBacks->g_fptrHANClientOnDevicePartialDeleted;
	g_fptrHANClientOnOpenRegRes = HanClientRegisterCallBacks->g_fptrHANClientOnOpenRegRes;
	g_fptrHANClientOnCloseRegRes = HanClientRegisterCallBacks->g_fptrHANClientOnCloseRegRes;
	g_fptrHANClientOnHandsetRegistered = HanClientRegisterCallBacks->g_fptrHANClientOnHandsetRegistered;
	g_fptrHANClientOnTargetUpIndication = HanClientRegisterCallBacks->g_fptrHANClientOnTargetUpIndication;
	g_fptrHANClientOnEepParamGetRes = HanClientRegisterCallBacks->g_fptrHANClientOnEepParamGetRes;
	g_fptrHANClientOnEepParamSetRes = HanClientRegisterCallBacks->g_fptrHANClientOnEepParamSetRes;
	g_fptrHANClientOnEepAreaGetRes = HanClientRegisterCallBacks->g_fptrHANClientOnEepAreaGetRes;
	g_fptrHANClientOnEepAreaSetRes = HanClientRegisterCallBacks->g_fptrHANClientOnEepAreaSetRes;
	g_fptrHANClientOnProdParamGetRes = HanClientRegisterCallBacks->g_fptrHANClientOnProdParamGetRes;
	g_fptrHANClientOnProdParamSetRes = HanClientRegisterCallBacks->g_fptrHANClientOnProdParamSetRes;
	g_fptrHANClientOnRamAreaGetRes = HanClientRegisterCallBacks->g_fptrHANClientOnRamAreaGetRes;
	g_fptrHANClientOnRamAreaSetRes = HanClientRegisterCallBacks->g_fptrHANClientOnRamAreaSetRes;
	g_fptrHANClientOnEepromSizeGetRes = HanClientRegisterCallBacks->g_fptrHANClientOnEepromSizeGetRes;
	g_fptrHANClientOnHwVersionGetRes = HanClientRegisterCallBacks->g_fptrHANClientOnHwVersionGetRes;
	g_fptrHANClientOnSwVersionGetRes = HanClientRegisterCallBacks->g_fptrHANClientOnSwVersionGetRes;
	g_fptrHANClientOnGetNumOfFunMsgInQRes = HanClientRegisterCallBacks->g_fptrHANClientOnGetNumOfFunMsgInQRes;
	g_fptrHANClientOnMessageSendRes = HanClientRegisterCallBacks->g_fptrHANClientOnMessageSendRes;
	g_fptrHANClientOnReadDeviceTablePhase2Res = HanClientRegisterCallBacks->g_fptrHANClientOnReadDeviceTablePhase2Res;
	g_fptrHANClientOnReadDeviceSubInfoPhase2Res = HanClientRegisterCallBacks->g_fptrHANClientOnReadDeviceSubInfoPhase2Res;
	g_fptrHANClientOnReadBlackListDeviceTable = HanClientRegisterCallBacks->g_fptrHANClientOnReadBlackListDeviceTable;
	g_fptrHANClientOnDeviceDeleted = HanClientRegisterCallBacks->g_fptrHANClientOnDeviceDeleted;
}


int app_HanMWEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData )
{
	E_CMBS_HAN_EVENT_ID e_HanEventId;
	bool			ret_code = TRUE;
	bool			isCMBSEvent = TRUE;
	ST_IE_HAN_MSG stIe_Msg;
	UNUSED_PARAMETER(pv_AppRef);

	//search for CMBS Event
	switch(e_EventID)
	{
		case CMBS_EV_DSR_CORD_CLOSEREG:
			if(g_fptrHANClientOnRegClosed != NULL)
			{
				g_fptrHANClientOnRegClosed(e_EventID,pv_EventData);
			}
			break;
		case CMBS_EV_DSR_HS_REGISTERED:
			app_HanMWOnHandsetRegistered(pv_EventData);
			break;
		case CMBS_EV_DSR_CORD_OPENREG_RES:
			app_HanMWOnOpenRegRes(pv_EventData);
			break;
		case CMBS_EV_DSR_CORD_CLOSEREG_RES:
			app_HanMWOnCloseRegRes(pv_EventData);
			break;
		case CMBS_EV_DSR_PARAM_GET_RES:
			app_HanMWOnParamGetRes(pv_EventData);
			break;
		case CMBS_EV_DSR_PARAM_SET_RES:
			app_HanMWOnParamSetRes(pv_EventData);
			break;
		case CMBS_EV_DSR_PARAM_AREA_GET_RES:
			app_HanMWOnParamAreaGetRes(pv_EventData);
			break;
		case CMBS_EV_DSR_PARAM_AREA_SET_RES:
			app_HanMWOnParamAreaSetRes(pv_EventData);
			break;
		case CMBS_EV_DSR_EEPROM_EXTENDED_SIZE_GET_RES:
			app_HanMWOnEepromSizeGetRes(pv_EventData);
			break;
		case CMBS_EV_DSR_SYS_START_RES:
			if (g_fptrHANClientOnTargetUpIndication != NULL)
			{
				g_fptrHANClientOnTargetUpIndication();
			}
			break;
		default:
			isCMBSEvent = FALSE;
			break;
	}

	if(isCMBSEvent == TRUE)
		return isCMBSEvent;

	//search for HAN Event
	e_HanEventId = (E_CMBS_HAN_EVENT_ID)e_EventID;
	
	switch(e_HanEventId)
	{
		case CMBS_EV_DSR_HAN_DEVICE_READ_TABLE_RES:
			if(g_fptrHANClientOnReadDeviceTableRes != NULL)
			{
				g_fptrHANClientOnReadDeviceTableRes(pv_EventData);
			}
			break;
		case CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_RES:
			if(g_fptrHANClientOnReadDeviceSubInfoRes != NULL)
			{
				g_fptrHANClientOnReadDeviceSubInfoRes(pv_EventData);
			}
			break;
		case CMBS_EV_DSR_HAN_MSG_SEND_RES:
			app_HanMWOnDsrMsgSendRes(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_MSG_RECV:
			if(g_fptrHANClientOnMsgRecv != NULL)
			{
				g_fptrHANClientOnMsgRecv(pv_EventData,&stIe_Msg);
			}
			break;
		case CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE_RES:
		case CMBS_EV_DSR_HAN_DEVICE_BLACK_LIST_DELETE_RES:
			app_HanMWOnDeviceBlackListDeleted( pv_EventData );
			break;
		case CMBS_EV_DSR_HAN_DEVICE_REG_STAGE_3_NOTIFICATION:
			app_HanMWOnDeviceRegistered( pv_EventData );
			break;
		case CMBS_EV_DSR_HAN_MSG_SEND_TX_READY:
			app_HanMWOnSendTxReady(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_MSG_SEND_TX_START_REQUEST_RES:
			app_HanMWOnStartTxRequestRes(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_MSG_SEND_TX_ENDED:
			app_HanMWOnTxEnded(pv_EventData);
			break;
		case CMBS_EV_DSR_HAN_DEVICE_REG_DELETED:
			app_HanMWOnDevicePartialDeleted( pv_EventData );
			break;
		case CMBS_EV_DSR_HAN_DEVICE_READ_EXTENDED_TABLE_PHASE_2_RES:
			if(g_fptrHANClientOnReadDeviceTablePhase2Res != NULL)
			{
				g_fptrHANClientOnReadDeviceTablePhase2Res(pv_EventData);
			}
			break;
		case CMBS_EV_DSR_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO_PHASE_2_RES:
			if(g_fptrHANClientOnReadDeviceSubInfoPhase2Res != NULL)
			{
				g_fptrHANClientOnReadDeviceSubInfoPhase2Res(pv_EventData);
			}
			break;
			
		case CMBS_EV_DSR_HAN_DEVICE_READ_BLACK_LIST_DEVICE_TABLE_RES:
			if(g_fptrHANClientOnReadBlackListDeviceTable != NULL)
			{
				g_fptrHANClientOnReadBlackListDeviceTable (pv_EventData);
			}
			break;

		case CMBS_EV_DSR_HAN_DEVICE_DELETED_IND:
			app_HanMWOnDeviceDeleted(pv_EventData);
			break;
			
		default:
			ret_code = FALSE;		
		
	}

	

	return ret_code;
}

void app_HanMwReverseBuffer(u8 *pu8_Buffer, u16 u16_Length)
{
	u8 pu8_temp[50];
	u16 u16_Index = 0, u16_TempIndex = u16_Length-1;
	
	for(u16_Index = 0; u16_Index < u16_Length; u16_Index++)
	{
		pu8_temp[u16_TempIndex - u16_Index] = pu8_Buffer[u16_Index];
	}

	memcpy(pu8_Buffer, pu8_temp, u16_Length);
}

void app_HanMwGetEepParamName(E_CMBS_PARAM e_Param, u8 *pu8_Buffer, bool *b_ReverseFlag)
{
	switch(e_Param)
	{
		case CMBS_PARAM_RFPI:
			memcpy(pu8_Buffer, "RFPI", strlen("RFPI"));
			break;
			
		case CMBS_PARAM_RF_FULL_POWER:
			memcpy(pu8_Buffer, "RF_FULL_POWER", strlen("RF_FULL_POWER"));
			break;
			
		case CMBS_PARAM_PREAM_NORM:
			memcpy(pu8_Buffer, "PREAM_NORM", strlen("PREAM_NORM"));
			break;
			
		case CMBS_PARAM_RF19APU_SUPPORT_FCC:
			memcpy(pu8_Buffer, "RF19APU_SUPPORT_FCC", strlen("RF19APU_SUPPORT_FCC"));
			break;
			
		case CMBS_PARAM_RF19APU_DEVIATION:
			memcpy(pu8_Buffer, "RF19APU_DEVIATION", strlen("RF19APU_DEVIATION"));
			break;
			
		case CMBS_PARAM_RF19APU_PA2_COMP:
			memcpy(pu8_Buffer, "RF19APU_PA2_COMP", strlen("RF19APU_PA2_COMP"));
			break;
			
		case CMBS_PARAM_MAX_USABLE_RSSI:
			memcpy(pu8_Buffer, "MAX_USABLE_RSSI", strlen("MAX_USABLE_RSSI"));
			break;
			
		case CMBS_PARAM_LOWER_RSSI_LIMIT:
			memcpy(pu8_Buffer, "LOWER_RSSI_LIMIT", strlen("LOWER_RSSI_LIMIT"));
			break;
			
		case CMBS_PARAM_PHS_SCAN_PARAM:
			memcpy(pu8_Buffer, "PHS_SCAN_PARAM", strlen("PHS_SCAN_PARAM"));
			break;
			
		case CMBS_PARAM_JDECT_LEVEL1_M82:
			memcpy(pu8_Buffer, "JDECT_LEVEL1_M82", strlen("JDECT_LEVEL1_M82"));
			break;
			
		case CMBS_PARAM_JDECT_LEVEL2_M62:
			memcpy(pu8_Buffer, "JDECT_LEVEL2_M62", strlen("JDECT_LEVEL2_M62"));
			break;
			
		case CMBS_PARAM_SUBS_DATA:
			memcpy(pu8_Buffer, "SUBS_DATA", strlen("SUBS_DATA"));
			break;
			
		case CMBS_PARAM_RVREF:
			memcpy(pu8_Buffer, "RVREF", strlen("RVREF"));
			break;
			
		case CMBS_PARAM_GFSK:
			memcpy(pu8_Buffer, "GFSK", strlen("GFSK"));
			break;
			
		case CMBS_PARAM_RXTUN:
			memcpy(pu8_Buffer, "RXTUN", strlen("RXTUN"));
			break;
			
		case CMBS_PARAM_HAN_DECT_SUB_DB_START:
			memcpy(pu8_Buffer, "HAN_DECT_SUB_DB_START", strlen("HAN_DECT_SUB_DB_START"));
			*b_ReverseFlag = TRUE;
			break;

		case CMBS_PARAM_HAN_DECT_SUB_DB_END:
			memcpy(pu8_Buffer, "HAN_DECT_SUB_DB_END", strlen("HAN_DECT_SUB_DB_END"));
			*b_ReverseFlag = TRUE;
			break;

		case CMBS_PARAM_HAN_ULE_SUB_DB_START:
			memcpy(pu8_Buffer, "HAN_ULE_SUB_DB_START", strlen("HAN_ULE_SUB_DB_START"));
			*b_ReverseFlag = TRUE;
			break;

		case CMBS_PARAM_HAN_ULE_SUB_DB_END:
			memcpy(pu8_Buffer, "HAN_ULE_SUB_DB_END", strlen("HAN_ULE_SUB_DB_END"));
			*b_ReverseFlag = TRUE;
			break;

		case CMBS_PARAM_HAN_FUN_SUB_DB_START:
			memcpy(pu8_Buffer, "HAN_FUN_SUB_DB_START", strlen("HAN_FUN_SUB_DB_START"));
			*b_ReverseFlag = TRUE;
			break;

		case CMBS_PARAM_HAN_FUN_SUB_DB_END:
			memcpy(pu8_Buffer, "HAN_FUN_SUB_DB_END", strlen("HAN_FUN_SUB_DB_END"));
			*b_ReverseFlag = TRUE;
			break;

		case CMBS_PARAM_HAN_ULE_NEXT_TPUI:
			memcpy(pu8_Buffer, "HAN_ULE_NEXT_TPUI", strlen("HAN_ULE_NEXT_TPUI"));
			break;
			
		case CMBS_PARAM_MAX_TRANSFER_SIZE:
			memcpy(pu8_Buffer, "MAX_TRANSFER_SIZE", strlen("MAX_TRANSFER_SIZE"));
			*b_ReverseFlag = TRUE;
			break;
			
		case CMBS_PARAM_DECT_TYPE:
			memcpy(pu8_Buffer, "DECT_TYPE", strlen("DECT_TYPE"));
			*b_ReverseFlag = TRUE;	
			break;
			
		case CMBS_PARAM_HAN_FUN_GROUP_LIST_START:
			memcpy(pu8_Buffer, "HAN_FUN_GROUP_LIST_START", strlen("HAN_FUN_GROUP_LIST_START"));
			*b_ReverseFlag = TRUE;
			break;
			
		case CMBS_PARAM_HAN_FUN_GROUP_LIST_END:
			memcpy(pu8_Buffer, "HAN_FUN_GROUP_LIST_END", strlen("HAN_FUN_GROUP_LIST_END"));
			*b_ReverseFlag = TRUE;
			break;
			
		case CMBS_PARAM_HAN_FUN_GROUP_TABLE_START:
			memcpy(pu8_Buffer, "HAN_FUN_GROUP_TABLE_START", strlen("HAN_FUN_GROUP_TABLE_START"));
			*b_ReverseFlag = TRUE;
			break;
			
		case CMBS_PARAM_HAN_FUN_GROUP_TABLE_END:
			memcpy(pu8_Buffer, "HAN_FUN_GROUP_TABLE_END", strlen("HAN_FUN_GROUP_TABLE_END"));
			*b_ReverseFlag = TRUE;
			break;
			
		case CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_START:
			memcpy(pu8_Buffer, "HAN_ULE_BROADCAST_CONVERSION_TABLE_START", strlen("HAN_ULE_BROADCAST_CONVERSION_TABLE_START"));
			*b_ReverseFlag = TRUE;
			break;
			
		case CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_END:
			memcpy(pu8_Buffer, "HAN_ULE_BROADCAST_CONVERSION_TABLE_END", strlen("HAN_ULE_BROADCAST_CONVERSION_TABLE_END"));
			*b_ReverseFlag = TRUE;
			break;
			
		case CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS:
			memcpy(pu8_Buffer, "ULE_MULTICAST_ENC_PARAMS", strlen("ULE_MULTICAST_ENC_PARAMS"));
			break;
			
		default:
			memcpy(pu8_Buffer, "UNKNOWN", strlen("UNKNOWN"));
			break;
	}
}

void app_HanMwGetHwChipName(E_CMBS_HW_CHIP u8_HwChip, u8 *pu8_Buffer)
{
	switch(u8_HwChip)
	{
		case CMBS_HW_CHIP_VEGAONE:
			memcpy(pu8_Buffer, "HW_CHIP_VEGAONE", strlen("HW_CHIP_VEGAONE"));
			break;
			
		case CMBS_HW_CHIP_DCX78:
			memcpy(pu8_Buffer, "HW_CHIP_DCX78", strlen("HW_CHIP_DCX78"));
			break;
			
		case CMBS_HW_CHIP_DCX79:
			memcpy(pu8_Buffer, "HW_CHIP_DCX79", strlen("HW_CHIP_DCX79"));
			break;
			
		case CMBS_HW_CHIP_DCX81:
			memcpy(pu8_Buffer, "HW_CHIP_DCX81", strlen("HW_CHIP_DCX81"));
			break;
			
		case CMBS_HW_CHIP_DVF99:
			memcpy(pu8_Buffer, "HW_CHIP_DVF99", strlen("HW_CHIP_DVF99"));
			break;
			
		case CMBS_HW_CHIP_DHX91:
			memcpy(pu8_Buffer, "HW_CHIP_DHX91", strlen("HW_CHIP_DHX91"));
			break;
			
		default:
			memcpy(pu8_Buffer, "NONE", strlen("NONE"));
			break;
	}
}

void app_HanMwGetHwChipVersionName(E_CMBS_HW_CHIP_VERSION u8_HwChipVersion, u8 *pu8_Buffer)
{
	switch(u8_HwChipVersion)
	{
		case CMBS_HW_CHIP_VERSION_C:
			memcpy(pu8_Buffer, "HW_CHIP_VERSION_C", strlen("HW_CHIP_VERSION_C"));
			break;
			
		case CMBS_HW_CHIP_VERSION_D:
			memcpy(pu8_Buffer, "HW_CHIP_VERSION_D", strlen("HW_CHIP_VERSION_D"));
			break;
			
		case CMBS_HW_CHIP_VERSION_B:
			memcpy(pu8_Buffer, "HW_CHIP_VERSION_B", strlen("HW_CHIP_VERSION_B"));
			break;

		default:
			memcpy(pu8_Buffer, "NONE", strlen("NONE"));
			break;
	}
}

void app_HanMwGetHwBoardName(E_CMBS_HW_BOARD u8_HwBoard, u8 *pu8_Buffer)
{
	switch(u8_HwBoard)
	{
		case CMBS_HW_BOARD_MOD:
			memcpy(pu8_Buffer, "HW_BOARD_MOD", strlen("HW_BOARD_MOD"));
			break;
			
		case CMBS_HW_BOARD_DEV:
			memcpy(pu8_Buffer, "HW_BOARD_DEV", strlen("HW_BOARD_DEV"));
			break;
			
		default:
			memcpy(pu8_Buffer, "NONE", strlen("NONE"));
			break;
	}
}

void app_HanMwGetHwComTypeName( E_CMBS_HW_COM_TYPE u8_HwComType, u8 *pu8_Buffer)
{
	switch(u8_HwComType)
	{
		case CMBS_HW_COM_TYPE_UART:
			memcpy(pu8_Buffer, "HW_COM_TYPE_UART", strlen("HW_COM_TYPE_UART"));
			break;
			
		case CMBS_HW_COM_TYPE_USB:
			memcpy(pu8_Buffer, "HW_COM_TYPE_USB", strlen("HW_COM_TYPE_USB"));
			break;
			
		case CMBS_HW_COM_TYPE_SPI0:
			memcpy(pu8_Buffer, "HW_COM_TYPE_SPI0", strlen("HW_COM_TYPE_SPI0"));
			break;
			
		case CMBS_HW_COM_TYPE_SPI3:
			memcpy(pu8_Buffer, "HW_COM_TYPE_SPI3", strlen("HW_COM_TYPE_SPI3"));
			break;		
		default:
			memcpy(pu8_Buffer, "NONE", strlen("NONE"));
			break;
	}
}
void app_HanMWOnOpenRegRes(void *pv_List)
{
	int success = FALSE;

    if ( !app_ResponseCheck(pv_List) )
    {
        success = TRUE;
    }
	
	g_fptrHANClientOnOpenRegRes( success );
}

void app_HanMWOnCloseRegRes(void *pv_List)
{
	int success = FALSE;
    if ( !app_ResponseCheck(pv_List) )
    {
        success = TRUE;
    }	
	g_fptrHANClientOnCloseRegRes( success );
}

void app_HanMWOnParamGetRes(void *pv_List)
{
	bool b_success = FALSE, b_ReverseDataFlag = FALSE;
	ST_IE_RESPONSE st_Response = {0};
	ST_IE_PARAMETER st_Param = {0};
	u8 pu8_EepParaName[150] = {0};
    void *pv_IE;
    u16 u16_IE;
	
	if(!g_fptrHANClientOnEepParamGetRes || !g_fptrHANClientOnProdParamGetRes)
	{
		return;
	}
	
    cmbs_api_ie_GetFirst( pv_List, &pv_IE, &u16_IE );

    while ( pv_IE )
    {
        switch ( u16_IE )
        {
	        case  CMBS_IE_PARAMETER:
				cmbs_api_ie_ParameterGet(pv_IE,&st_Param);
	            break;
			case CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
				break;
        }
		
        cmbs_api_ie_GetNext( pv_List, &pv_IE, &u16_IE );
	}

	b_success   = st_Response.e_Response == CMBS_RESPONSE_OK ? 1 : 0;
	
	app_HanMwGetEepParamName(st_Param.e_Param, &pu8_EepParaName[0], &b_ReverseDataFlag);
	
	if (b_ReverseDataFlag)
	{
		app_HanMwReverseBuffer(st_Param.pu8_Data, st_Param.u16_DataLen);
	}

	if(st_Param.e_ParamType == CMBS_PARAM_TYPE_EEPROM)
	{
		g_fptrHANClientOnEepParamGetRes(b_success, &pu8_EepParaName[0], st_Param.u16_DataLen, st_Param.pu8_Data);
	}
	else if (st_Param.e_ParamType == CMBS_PARAM_TYPE_PRODUCTION)
	{
		g_fptrHANClientOnProdParamGetRes(b_success, &pu8_EepParaName[0], st_Param.u16_DataLen, st_Param.pu8_Data);
	}
}

void app_HanMWOnParamSetRes(void *pv_List)
{
	bool b_success = FALSE, b_ReverseDataFlag = FALSE;
	ST_IE_RESPONSE st_Response = {0};
	ST_IE_PARAMETER st_Param = {0};
	u8 pu8_EepParaName[150] = {0};
    void *pv_IE;
    u16 u16_IE;

	if(!g_fptrHANClientOnEepParamSetRes || !g_fptrHANClientOnProdParamSetRes)
	{
		return;
	}
	
    cmbs_api_ie_GetFirst( pv_List, &pv_IE, &u16_IE );

    while ( pv_IE )
    {
        switch ( u16_IE )
        {
	        case  CMBS_IE_PARAMETER:
				cmbs_api_ie_ParameterGet(pv_IE,&st_Param);
	            break;
			case CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
				break;
        }
		
        cmbs_api_ie_GetNext( pv_List, &pv_IE, &u16_IE );
	}

	b_success   = st_Response.e_Response == CMBS_RESPONSE_OK ? 1 : 0;
	
	app_HanMwGetEepParamName(st_Param.e_Param, &pu8_EepParaName[0], &b_ReverseDataFlag);
	
	if (b_ReverseDataFlag)
	{
		app_HanMwReverseBuffer(st_Param.pu8_Data, st_Param.u16_DataLen);
	}

	if(st_Param.e_ParamType == CMBS_PARAM_TYPE_EEPROM)
	{
		g_fptrHANClientOnEepParamSetRes(b_success, &pu8_EepParaName[0]);
	}
	else if(st_Param.e_ParamType == CMBS_PARAM_TYPE_PRODUCTION)
	{
		g_fptrHANClientOnProdParamSetRes(b_success, &pu8_EepParaName[0]);
	}
}
void app_HanMWOnParamAreaGetRes(void *pv_List)
{
	bool b_success = FALSE;
	ST_IE_RESPONSE st_Response = {0};
	ST_IE_PARAMETER_AREA st_ParamArea = {0};
    void *pv_IE;
    u16 u16_IE;

	if(!g_fptrHANClientOnEepAreaGetRes || !g_fptrHANClientOnRamAreaGetRes)
	{
		return;
	}
	
    cmbs_api_ie_GetFirst( pv_List, &pv_IE, &u16_IE );

    while ( pv_IE )
    {
        switch ( u16_IE )
        {
	        case  CMBS_IE_PARAMETER_AREA:
				cmbs_api_ie_ParameterAreaGet(pv_IE,&st_ParamArea);
	            break;
			case CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
				break;
        }
		
        cmbs_api_ie_GetNext( pv_List, &pv_IE, &u16_IE );
	}

	b_success   = st_Response.e_Response == CMBS_RESPONSE_OK ? 1 : 0;

	if(st_ParamArea.e_AreaType == CMBS_PARAM_AREA_TYPE_EEPROM)
	{
		g_fptrHANClientOnEepAreaGetRes(b_success, st_ParamArea.u32_Offset, st_ParamArea.u16_DataLen, st_ParamArea.pu8_Data);
	}
	else if (st_ParamArea.e_AreaType == CMBS_PARAM_AREA_TYPE_RAM)
	{
		g_fptrHANClientOnRamAreaGetRes(b_success, st_ParamArea.u32_Offset, st_ParamArea.u16_DataLen, st_ParamArea.pu8_Data);
	}
}
void app_HanMWOnParamAreaSetRes(void *pv_List)
{
	bool b_success = FALSE;
	ST_IE_RESPONSE st_Response = {0};
	ST_IE_PARAMETER_AREA st_ParamArea = {0};
	
    void *pv_IE;
    u16 u16_IE;

	if (!g_fptrHANClientOnEepAreaSetRes || !g_fptrHANClientOnRamAreaSetRes)
	{
		return;
	}
	
    cmbs_api_ie_GetFirst( pv_List, &pv_IE, &u16_IE );

    while ( pv_IE )
    {
        switch ( u16_IE )
        {
			case CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
				break;
				
			case  CMBS_IE_PARAMETER_AREA:
				cmbs_api_ie_ParameterAreaGet(pv_IE,&st_ParamArea);
        }
		
        cmbs_api_ie_GetNext( pv_List, &pv_IE, &u16_IE );
	}

	b_success   = st_Response.e_Response == CMBS_RESPONSE_OK ? 1 : 0;
		
	if(st_ParamArea.e_AreaType == CMBS_PARAM_AREA_TYPE_RAM)
	{
		g_fptrHANClientOnRamAreaSetRes(b_success);
	}
	else
	{
		g_fptrHANClientOnEepAreaSetRes(b_success);
	}	

}


void app_HanMWOnEepromSizeGetRes(void *pv_List)
{
    u32 u32_EepromSize = 0;
    void *pv_IE;
    u16      u16_IE;
	bool b_success;

	if(!g_fptrHANClientOnEepromSizeGetRes)
	{
		return;
	}
	
    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
    cmbs_api_ie_u32ValueGet(pv_IE, &u32_EepromSize, CMBS_IE_INTEGER_VALUE);

	b_success = TRUE;
	
	// it is not possible to distinguish between EEPROM and RAM types at this point 
	g_fptrHANClientOnEepromSizeGetRes(b_success, u32_EepromSize);
}

void app_HanMWHwVersionGet(void)
{
	bool b_success = TRUE;
	u8 pu8_HwChipName[50] = {0};
	u8 pu8_HwChipVersion[50] = {0};
	u8 pu8_HwBoard[50] = {0};
	u8 pu8_HwComType[50] = {0};

	if(!g_fptrHANClientOnHwVersionGetRes)
	{
		return;
	}

	if(b_success)
	{
		app_HanMwGetHwChipName(appcmbs_get_CMBSAppInstance()->u8_HwChip, &pu8_HwChipName[0]);
		app_HanMwGetHwChipVersionName(appcmbs_get_CMBSAppInstance()->u8_HwChipVersion, &pu8_HwChipVersion[0]);
		app_HanMwGetHwBoardName(appcmbs_get_CMBSAppInstance()->u8_HwBoard,&pu8_HwBoard[0]);
		app_HanMwGetHwComTypeName(appcmbs_get_CMBSAppInstance()->u8_HwComType, &pu8_HwComType[0]);
	}
	
	g_fptrHANClientOnHwVersionGetRes(b_success, &pu8_HwChipName[0], &pu8_HwChipVersion[0], &pu8_HwBoard[0], &pu8_HwComType[0]);
	
}

void app_HanMWSwVersionGet(void)
{
	bool b_success = TRUE;
	u16 u16_TargetVersion, u16_TargetBuildNum;
	u16 u16_HostVersion, u16_HostBuildNum;

	if(!g_fptrHANClientOnSwVersionGetRes)
	{
		return;
	}

	if(b_success)
	{
		// Get Target version
    	u16_TargetVersion = cmbs_api_ModuleVersionGet();
    	u16_TargetBuildNum = cmbs_api_ModuleVersionBuildGet();

		// Get Host version
		u16_HostVersion = tcx_ApplVersionGet();
		u16_HostBuildNum = tcx_ApplVersionBuildGet();
	}
	
	g_fptrHANClientOnSwVersionGetRes(b_success, u16_TargetVersion, u16_TargetBuildNum, u16_HostVersion, u16_HostBuildNum);
}


void app_HanMWGetNumOfFunMsgInQ(u16 u16_deviceId)
{
	bool b_success = TRUE;
	u8 u8_NumOfPendingMsg = 0, u8_NumOfMaxMsg = 0;

	if(!g_fptrHANClientOnGetNumOfFunMsgInQRes)
	{
		return;
	}

	if(u16_deviceId >= MAX_ULE_DEVICES_HOST)
	{
		b_success = FALSE;
	}
	else
	{
		u8_NumOfPendingMsg = (u8)cmbs_util_FifoCount(Get_UleMsgFifo(u16_deviceId));
		u8_NumOfMaxMsg = app_HanDbGetFunMsgQSizeForDevice();
	}
	
	g_fptrHANClientOnGetNumOfFunMsgInQRes(b_success, u16_deviceId, u8_NumOfPendingMsg, u8_NumOfMaxMsg);
}

void app_HanMWClearFunMsgQ(u16 u16_deviceId)
{
	if (u16_deviceId >= MAX_ULE_DEVICES_HOST)
	{
		return;
	}

	// verify queue is not empty
	if(cmbs_util_FifoCount(Get_UleMsgFifo(u16_deviceId)) > 0)
	{
		// clear pending tx request
		SetWaitForReady(u16_deviceId, FALSE);
		app_DsrHanMsgSendTxEnd(u16_deviceId);

		cmbs_util_FifoClear(Get_UleMsgFifo(u16_deviceId));
	}
}

void app_HanMWOnHandsetRegistered(void *pv_List)
{
	ST_IE_HANDSETINFO st_HsInfo;
    ST_IE_RESPONSE st_Res;
    void *pv_IE = NULL;
    u16      u16_IE;

    memset(&st_HsInfo, 0, sizeof(ST_IE_HANDSETINFO));
    cmbs_api_ie_GetFirst(pv_List, &pv_IE, &u16_IE);
    cmbs_api_ie_HandsetInfoGet(pv_IE, &st_HsInfo);
    cmbs_api_ie_ResponseGet(pv_IE, &st_Res);

    if ( st_Res.e_Response == CMBS_RESPONSE_OK )
    {
        if ( st_HsInfo.u8_State == CMBS_HS_REG_STATE_REGISTERED )
        {
			g_fptrHANClientOnHandsetRegistered( st_HsInfo.u8_Hs );
        }
    }

}



void app_HanMWOnDsrMsgSendRes 	(void* pv_EventData)
{
	void *			pv_IE = NULL;
	u16 			u16_IE, u16_deviceId=0;
	ST_IE_RESPONSE 	st_Resp;
	u16				u16_ReqID=0;
	u16				u16_Reason =0;
	E_MSG_IN_QUEUE_STATUS e_ret;
	
	memset(&st_Resp, 0, sizeof(ST_IE_RESPONSE));
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while( pv_IE != NULL )
    {
			switch ( u16_IE )
	        {
	        case  CMBS_IE_HAN_DEVICE:
					cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
	            	break;
	        case  CMBS_IE_RESPONSE:
					cmbs_api_ie_ResponseGet( pv_IE, &st_Resp);
	            	break;

			case CMBS_IE_REQUEST_ID:
					cmbs_api_ie_HANReqIDGet (pv_IE, &u16_ReqID);
					break;

			case CMBS_IE_HAN_SEND_FAIL_REASON:
					cmbs_api_ie_HANSendErrorReasonGet (pv_IE, &u16_Reason);
					break;	
			}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}
	
	if (u16_deviceId >= MAX_ULE_DEVICES_HOST)
	{
		return;
	}

	// printf("reason = %d response = %d \n",u16_Reason ,st_Resp.e_Response);
	if (( st_Resp.e_Response != CMBS_RESPONSE_OK )
		&& ( u16_Reason == CMBS_HAN_SEND_MSG_REASON_DEVICE_NOT_IN_LINK_ERROR )
		&& ( cmbs_util_FifoCount(Get_UleMsgFifo(u16_deviceId)) > 0))
	{
		// Do not extract message from queue, send Tx Request again
		app_DsrHanMsgSendTxRequest (u16_deviceId);
		SetWaitForReady(u16_deviceId, TRUE);
	}
	else
	{
		// Message sent to device, extract it from queue
		cmbs_util_FifoPop(Get_UleMsgFifo(u16_deviceId));

		// Previous message was treated, check now if another message is waiting in queue
		e_ret = app_HanMWSendDsrMsgFromFifo(u16_deviceId, u16_deviceId);

		// If no additional message in queue, release the link
		if ((NO_MSG_IN_QUEUE == e_ret) && GetReleaseLink(u16_deviceId))
		{
			app_DsrHanMsgSendTxEnd(u16_deviceId);

			// Clear link release request after link has been released
			SetReleaseLink(u16_deviceId, FALSE);
		}

		if(g_fptrHANClientOnMessageSendRes)
		{
			bool b_status = st_Resp.e_Response == CMBS_RESPONSE_OK ? TRUE : FALSE;
			
			g_fptrHANClientOnMessageSendRes(b_status , u16_deviceId);
		}
	}
}

void app_HanMWOnDeviceBlackListDeleted( void* pv_EventData )
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	u16 u16_index;
	ST_IE_RESPONSE 	st_Resp;
	u16				u16_Reason = 0;

	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
			    break;
			case  CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet(pv_IE, &st_Resp);
				break;
			case CMBS_IE_HAN_DEVICE_FORCEFUL_DELETE_ERROR_STATUS_REASON:
				cmbs_api_ie_HANDeviceBlackListDeleteErrorReasonGet(pv_IE, &u16_Reason);

		}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	SetDuringDeleteReq(FALSE);		// Caution: This line must not be conditioned!

	// In all cases: report DELETED to Client. 
	// Queue of Delete requests eliminated the "paralll delete" failure. Other failures we want to report "DELETED".
	
	if(g_fptrHANClientOnDeviceFullDeleted != NULL)
	{
		g_fptrHANClientOnDeviceFullDeleted(u16_deviceId,st_Resp,u16_Reason);		// u16_deviceId may be ALL devices
	}
		if (u16_deviceId != ALL_ULE_DEVICES_HOST)
		{
			if (u16_deviceId < MAX_ULE_DEVICES_HOST)
			{
				SetWaitForReady(u16_deviceId, FALSE);
				cmbs_util_FifoClear(Get_UleMsgFifo(u16_deviceId));
			}
		}
		else
		{
			// All devices deleted
			for (u16_index = 0; u16_index < MAX_ULE_DEVICES_HOST; u16_index++)
			{
				SetWaitForReady(u16_index, FALSE);
				cmbs_util_FifoClear(Get_UleMsgFifo(u16_index));
			}
		}

		// if another delete req is waiting to be sent to target, send it now
		u16_deviceId = app_HanPopPendingDeviceToDelete();
		if (u16_deviceId != NO_ULE_DEVICE_HOST)
		{
			if(u16_deviceId == ALL_ULE_DEVICES_HOST)
			{
				app_HanMWDeleteDeviceViaQueue(u16_deviceId, FALSE);
			}
			else
			{
				app_HanMWDeleteDeviceViaQueue(u16_deviceId & APPHANMW_DEVICE_ID_MASK, u16_deviceId & APPHANMW_DEL_TYPE_MASK ? TRUE : FALSE);
			}
		}
		
}



void app_HanMWOnDeviceRegistered( void* pv_EventData )
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	
	ST_HAN_GENERAL_STATUS st_Status;
 
	memset(&st_Status, 0, sizeof(ST_HAN_GENERAL_STATUS));
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
			    break;
			case  CMBS_IE_HAN_GENERAL_STATUS:
				cmbs_api_ie_HANGeneralStatusGet( pv_IE, &st_Status);								
	       		break;
		}
	cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );

	}

	if (st_Status.u16_Status == CMBS_HAN_GENERAL_STATUS_OK)
	{
		if(g_fptrHANClientOnDeviceRegistered != NULL)
		{
			g_fptrHANClientOnDeviceRegistered( u16_deviceId );
		}
	}
	else
	{
		if(g_fptrHANClientOnDevicePartialDeleted != NULL)
		{
			g_fptrHANClientOnDevicePartialDeleted( u16_deviceId );
		}
	}

	if (u16_deviceId < MAX_ULE_DEVICES_HOST)
	{
	// Clear FIFO, to be on safe side
		SetWaitForReady(u16_deviceId, FALSE);
		cmbs_util_FifoClear(Get_UleMsgFifo(u16_deviceId));
	}
}


void app_HanMWOnDeviceDeleted( void* pv_EventData )
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	
    while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
			    break;
		}
	cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );

	}

	if(g_fptrHANClientOnDeviceDeleted != NULL)
	{
		g_fptrHANClientOnDeviceDeleted(u16_deviceId);
	}

}
			
void app_HanMWOnSendTxReady( void* pv_EventData )
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;

	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
	cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
				break;
		}

		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}
	if (u16_deviceId < MAX_ULE_DEVICES_HOST)
	{
		SetWaitForReady(u16_deviceId, FALSE);

		app_HanMWSendDsrMsgFromFifo(u16_deviceId, u16_deviceId);
	} 
}

void app_HanMWOnStartTxRequestRes(void* pv_EventData)
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	ST_IE_RESPONSE 	st_Resp;
	ST_HAN_TX_REQ_RES CmbsTXReqResData = {0,0};
	
	memset(&st_Resp, 0, sizeof(ST_IE_RESPONSE));
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );

         while ( pv_IE )
	    {
	        switch ( u16_IE )
	        {
	        case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
	            	break;
	        case  CMBS_IE_RESPONSE:
				cmbs_api_ie_ResponseGet( pv_IE, &st_Resp);								
	            	break;
			case CMBS_IE_HAN_TX_REQ_RES_REASON:
				cmbs_api_ie_HANTxReqResReasonGet(pv_IE, &CmbsTXReqResData.TxReqReason);
			break;	
	        }
		
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}
		
	if (u16_deviceId >= MAX_ULE_DEVICES_HOST)
	{
		return;
	}
    //printf("\n CMBS_EV_DSR_HAN_MSG_SEND_TX_START_REQUEST_RES received \n");	
	//printf("Device Id: %d Response: %d \n ", u16_deviceId, st_Resp.e_Response);	

	if (st_Resp.e_Response == CMBS_RESPONSE_ERROR)
	{ 
		if ( CmbsTXReqResData.TxReqReason == CMBS_HAN_ULE_TX_REQ_RES_DEVICE_ALREADY_IN_LINK )
		{
			// Similar treatment as in app_HanMWOnSendTxReady:
			SetWaitForReady(u16_deviceId, FALSE);
			app_HanMWSendDsrMsgFromFifo(u16_deviceId, u16_deviceId);
		}
		else
		if ( CmbsTXReqResData.TxReqReason == CMBS_HAN_ULE_TX_REQ_RES_PREVIOUS_TX_REQ_PENDING )
		{
			// Nothing to do, continue waiting for READY
		}
		else
		{
			u16 i, u16_count = 0;
			
			// Any other reason, should not happen, clear queue:
			SetWaitForReady(u16_deviceId, FALSE);

			u16_count = cmbs_util_FifoClear(Get_UleMsgFifo(u16_deviceId));

			if(g_fptrHANClientOnMessageSendRes)
			{				
				for (i=0 ; i<u16_count; i++)
				{
					g_fptrHANClientOnMessageSendRes(FALSE , u16_deviceId);
				}
			}
		}
	}
}

void app_HanMWOnTxEnded( void* pv_EventData )
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;
	bool	b_queue_is_not_empty = FALSE;

	// If any message is still waiting for this device (should not happen) - renew the link 
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
				cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
				break;
		}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}
	if (u16_deviceId < MAX_ULE_DEVICES_HOST)
	{
		b_queue_is_not_empty = (cmbs_util_FifoGet(Get_UleMsgFifo(u16_deviceId)) != NULL);
		
		if 	(b_queue_is_not_empty)
		{
			// Renew Link
			app_DsrHanMsgSendTxRequest (u16_deviceId);
			SetWaitForReady(u16_deviceId, TRUE);
		}
	}
}



void app_HanMWOnDevicePartialDeleted( void* pv_EventData )
{
	void *	pv_IE = NULL;
	u16 	u16_IE, u16_deviceId=0;

	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while( pv_IE != NULL )
	{
        switch ( u16_IE )
        {
			case  CMBS_IE_HAN_DEVICE:
	cmbs_api_ie_HANDeviceGet(pv_IE,&u16_deviceId);
				break;
		}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	if (u16_deviceId < MAX_ULE_DEVICES_HOST)
	{
		if(g_fptrHANClientOnDevicePartialDeleted != NULL)
		{
			g_fptrHANClientOnDevicePartialDeleted(u16_deviceId);
		}
	}
}

E_CMBS_RC app_HanMwMsgSend_ImplicitLinkSetup (u16 u16_RequestId, u16 u16_DestDeviceId, ST_IE_HAN_MSG_CTL* pst_HANMsgCtl , ST_IE_HAN_MSG * pst_HANMsg )
{
	bool		queue_is_empty = TRUE;
	
	ST_FUN_MSG	st_fun_msg; 
	UNUSED_PARAMETER(u16_RequestId);

	if (u16_DestDeviceId >= MAX_ULE_DEVICES_HOST)
	{
		return CMBS_RC_ERROR_PARAMETER;
	}
	
	// copy message into local queue type element
	memcpy(&st_fun_msg.st_HANMsgCtl, pst_HANMsgCtl , sizeof(ST_IE_HAN_MSG_CTL));
	memcpy(&st_fun_msg.st_HANMsg, pst_HANMsg , sizeof(ST_IE_HAN_MSG));

	memset(&st_fun_msg.Payload, 0, sizeof(st_fun_msg.Payload) );

	if ((pst_HANMsg->u16_DataLen) && (pst_HANMsg->u16_DataLen <CMBS_HAN_MAX_MSG_DATA_LEN))	  
	{
		memcpy(&st_fun_msg.Payload, pst_HANMsg->pu8_Data, pst_HANMsg->u16_DataLen );
		//HAN_QUEUE_DBG("Payload exists\n");
	}
	else
	{
		//HAN_QUEUE_DBG("Payload does not exist\n");
	}
	
	// Push to device queue and send Tx Request. If Queue was not empty (in process), no need to ask again for Tx Request
	queue_is_empty = !((PST_FUN_MSG)cmbs_util_FifoGet(Get_UleMsgFifo(u16_DestDeviceId)));

	if (cmbs_util_FifoPush(Get_UleMsgFifo(u16_DestDeviceId), &st_fun_msg))
	{
		HAN_APP_MW_INFO_PRINT("Fun Message Pushed!\n");
		if (queue_is_empty)
		{	
			
			app_DsrHanMsgSendTxRequest (u16_DestDeviceId);
			SetWaitForReady(u16_DestDeviceId, TRUE);
		}		
	}
	else
	{
		
		HAN_APP_MW_WARNING_PRINT("\n FIFO for Device %d is full, overcome: clear it\n", u16_DestDeviceId);

		cmbs_util_FifoClear(Get_UleMsgFifo(u16_DestDeviceId));

		cmbs_util_FifoPush(Get_UleMsgFifo(u16_DestDeviceId), &st_fun_msg);

		HAN_APP_MW_INFO_PRINT("Queue was cleared and Fun Message Pushed!\n");

		// IF a READY response was lost, try here to resend it, to release the queue
		// Start with TxEnd, to release potential stuck
		app_DsrHanMsgSendTxEnd (u16_DestDeviceId);
		
		app_DsrHanMsgSendTxRequest (u16_DestDeviceId);

		SetWaitForReady(u16_DestDeviceId, TRUE);

		// send error response to Host - for now: no special report for host (TBD)
		// return CMBS_RC_ERROR_OUT_OF_MEM;
	}

	return CMBS_RC_OK;
}

void app_HanMWDeleteDeviceViaQueue(u16 u16_deviceId, bool b_BlackList)
{
	// If no previous request is during handling
	if (SetIfNotAlreadyDuringDeleteReq())
	{
		app_DsrHanDeleteDevice(u16_deviceId, b_BlackList);
	}
	else
	{
		// keep request in pending queue 
		app_HanPushPendingDeviceToDelete(u16_deviceId, b_BlackList);			
	}
}


void app_HanMWInitialize(void)
{
	app_HanDbInitialize();
}

void app_HanMWReleaseLinkRequest(u16 u16_deviceId)
{
	PST_FUN_MSG pst_fun_msg;

	if(u16_deviceId < MAX_ULE_DEVICES_HOST)
	{
		// If Any message in fifo -- delay release link (even sent to device with no response yet)
		pst_fun_msg = (PST_FUN_MSG)cmbs_util_FifoGet(Get_UleMsgFifo(u16_deviceId));

		if (pst_fun_msg)
		{
			// keep request in pending queue 
			SetReleaseLink(u16_deviceId, TRUE);;
		}
		else
		{
			// Release Link directly 
			app_DsrHanMsgSendTxEnd(u16_deviceId);
		}
	}
}

E_MSG_IN_QUEUE_STATUS	app_HanMWSendDsrMsgFromFifo(u16 u16_deviceId, u16 u16_RequestId)
{

	u8		u8_Buffer[CMBS_HAN_MAX_MSG_DATA_LEN];	// buffer for payload
	ST_IE_HAN_MSG			st_HANMsg_temp;
	ST_IE_HAN_MSG_CTL		st_HANMsgCtl_temp = { 0,0,0 };

	PST_FUN_MSG pst_fun_msg;

	// Since we can send only one message per device, we use its Device ID as the cookie for the message.
	// In future, we may add the request ID as part of the data saved in queue per message

	// Get a message from FIFO of the device id, but do not pop it yet

	pst_fun_msg = (PST_FUN_MSG)cmbs_util_FifoGet(Get_UleMsgFifo(u16_deviceId));
	
	if( pst_fun_msg )
	{	
		memcpy(&st_HANMsgCtl_temp, &(pst_fun_msg->st_HANMsgCtl) , sizeof(ST_IE_HAN_MSG_CTL));
		memcpy(&st_HANMsg_temp, &(pst_fun_msg->st_HANMsg) , sizeof(ST_IE_HAN_MSG));
		st_HANMsg_temp.pu8_Data = u8_Buffer;
		if ((st_HANMsg_temp.u16_DataLen) && (st_HANMsg_temp.u16_DataLen <CMBS_HAN_MAX_MSG_DATA_LEN))	
		{
			memcpy(st_HANMsg_temp.pu8_Data, pst_fun_msg->Payload, st_HANMsg_temp.u16_DataLen );
		}

		// Send to device
	
		cmbs_dsr_han_msg_Send (g_cmbsappl.pv_CMBSRef, g_u32_MessageRequestId++, &st_HANMsgCtl_temp, &st_HANMsg_temp);

		return FOUND_MSG_IN_QUEUE;
	}
	else
	{
		return NO_MSG_IN_QUEUE; //HAN_QUEUE_DBG("ULE Device queue is empty\n");
	}

}

E_CMBS_RC app_HanMwDeviceReadTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8 isBrief)
{
	return cmbs_dsr_han_device_ReadTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry, isBrief);
}

E_CMBS_RC app_HanMwReadSingleDeviceRegistrationInformation(u16 u16_DeviceId)
{
	return cmbs_dsr_han_device_ReadDeviceRegistrationInformation(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}

E_CMBS_RC app_HanMwReadSingleDeviceRegistrationInformationPhase2(u16 u16_DeviceId)
{
	return cmbs_dsr_han_device_ReadDeviceRegistrationInformationPhase2(g_cmbsappl.pv_CMBSRef, u16_DeviceId);
}

E_CMBS_RC app_HanMwDeviceReadTablePhase2(u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_device_ReadExtendedDeviceTablePhase2 (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry);
}

E_CMBS_RC app_HanMwDeviceReadBlackListTable (u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry)
{
	return cmbs_dsr_han_device_ReadBlackListedDeviceTable (g_cmbsappl.pv_CMBSRef, u16_NumOfEntries, u16_IndexOfFirstEntry);
}

