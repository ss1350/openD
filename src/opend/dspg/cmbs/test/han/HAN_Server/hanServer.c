#include "hanServer.h"


#define MAX_APPLICATIVE_DEV_ENTRIES 5    // Max entries allowed in a single GET TABLE

#define CMBS_SUBSCRIPTION_TIMEOUT 240




#define HAN_SERVER_PROTOCOL_VERSION_NUMBER 1



bool RemoteConnected = FALSE;

u8 u8_BaseRfpi[CMBS_PARAM_RFPI_LENGTH] = { 0 };


static unsigned char AsciiMsgToClient[10500];                 // ascii message toward remote
static u8 u8_Buffer[CMBS_HAN_MAX_MSG_LEN*2];

void p_HanServer_FillSingleDeviceString(PST_HAN_EXTENDED_DEVICE_INFO pDeviceInfo, u8 *pTmpBufPtr);
// Simulate FUN message of AC status report towards remote
void p_HanServer_SendCameraStatusToRemote(u16 u16_CameraDevice, u8 u8_status);

s32 p_HanServer_PopulateBinData(char *pCommand, u16 u16_dataLength, unsigned char *pBinOut);
void p_HanServer_PopulateHexValue(unsigned char *pInBinData, u16 *pu16_len, unsigned char *pOutHexBuf);
void p_HanServer_PopulateHexValueNoSpaces(unsigned char *pInBinData, u16 *pu16_len, unsigned char *pOutHexBuf);


// HAN messages callbacks
static u32 p_HanServer_InitRemote(const char *pCommand);
static u32 p_HanServer_RegOpen(const char *pCommand);
static u32 p_HanServer_RegClose(const char *pCommand);
static u32 p_HanServer_GetRegTable(const char *pCommand);
static u32 p_HanServer_FunMessage(const char *pCommand);
static u32 p_HanServer_CallDelete(const char *pCommand);
static u32 p_HanServer_GetDeviceInfo(const char *pCommand);
static u32 p_HanServer_GetDeviceInfoPhase2(const char *pCommand);
static u32 p_HanServer_ReleaseLink(const char *pCommand);
static u32 p_HanServer_GetTargetState(const char *pCommand);
static u32 p_HanServer_GetRegTablePhase2(const char *pCommand);
static u32 p_HanServer_GetEepromParam(const char *pCommand);
static u32 p_HanServer_SetEepromParam(const char *pCommand);
static u32 p_HanServer_GetEepromBytes(const char *pCommand);
static u32 p_HanServer_SetEepromBytes(const char *pCommand);
static u32 p_HanServer_GetProductionParam(const char *pCommand);
static u32 p_HanServer_SetProductionParam(const char *pCommand);
static u32 p_HanServer_GetEepromSize(const char *pCommand);
static u32 p_HanServer_SetRFState(const char *pCommand);
static u32 p_HanServer_GetTargetHwVersion(const char *pCommand);
static u32 p_HanServer_GetSwVersion(const char *pCommand);
static u32 p_HanServer_ResetTarget(const char *pCommand);
static u32 p_HanServer_GetBlackListTable(const char *pCommand);

static u32 p_HanServer_GetRamBytes(const char *pCommand);
static u32 p_HanServer_SetRamBytes(const char *pCommand);
static u32 p_HanServer_GetNumOfFunMsgInQ(const char *pCommand);
static u32 p_HanServer_ClearFunMsgQ(const char *pCommand);

static u32 p_HanServer_EventCB(const char *pCommand);


void p_HanServer_OnReadDeviceSubInfoRes(void *pv_EventData);
u32 p_HanServer_OnMsgRecv(void* pv_EventData,ST_IE_HAN_MSG * pstIe_Msg);
u32 p_HanServer_OnDeviceFullDeleted(u16 u16_DeviceId, ST_IE_RESPONSE 	st_Resp, u16 u16_Reason);
u32 p_HanServer_OnDeviceRegistered(u16 u16_DeviceId);
u32 p_HanServer_OnDevicePartialDeleted(u16 u16_deviceId);
u32 p_HanServer_OnOpenRegistrationRes(int succeess);
u32 p_HanServer_OnCloseRegistrationRes(int succeess);
u32 p_HanServer_OnTargetAliveIndication();
u32 p_HanServer_OnTargetUpIndication();
u32 p_HanServer_OnEepParamGetRes(bool success, u8 *pu8_EepParaName,u16 u16_DataLen, u8 * pu8_Data);
u32 p_HanServer_OnEepParamSetRes(bool success, u8 *pu8_EepParaName);
u32 p_HanServer_OnEepAreaGetRes(bool success, u32 u32_Offset,u16 u16_DataLen, u8 * pu8_Data);
u32 p_HanServer_OnEepAreaSetRes(bool success);
u32 p_HanServer_OnProdParamGetRes(bool success, u8 *pu8_ProdParaName, u16 u16_DataLen, u8 * pu8_Data);
u32 p_HanServer_OnProdParamSetRes(bool success, u8 *pu8_ProdParaName);
u32 p_HanServer_OnRamAreaGetRes(bool success, u32 u32_Offset,u16 u16_DataLen, u8 * pu8_Data);
u32 p_HanServer_OnRamAreaSetRes(bool success);
u32 p_HanServer_OnEepromSizeGetRes(bool success, u32 u32_EepSize);
u32 p_HanServer_OnTargetHwVersionGetRes(bool success, u8 *pu8_HwChipName, u8 *pu8_HwChipVersion, u8 *pu8_HwBoard, u8 *pu8_HwComType);
u32 p_HanServer_OnSwVersionGetRes(bool success, u16 u16_TargetVersion, u16 u16_TargetBuildNum, u16 u16_HostVersion, u16 u16_HostBuildNum);
u32 p_HanServer_OnGetNumOfFunMsgInQRes(bool success, u16 u16_DeviceId, u8 u8_NumOfPendingMsg, u8 u8_NumOfMaxMsg);
u32 p_HanServer_OnMessageSendRes(bool success, u16 u16_DeviceId);
void p_HanServer_OnReadDeviceTablePhase2Res(void *pv_EventData);
void p_HanServer_OnReadDeviceSubInfoPhase2Res(void *pv_EventData);
void p_HanServer_OnReadBlackListDeviceTableRes(void *pv_EventData);
u32 p_HanServer_OnDeviceDeleted(u16 u16_DeviceId);


typedef enum
{
	DEVICE_DEREGISTRATION_UNKOWN,
	DEVICE_DEREGISTRATION_FAILURE_INVALID_ID,
	DEVICE_DEREGISTRATION_FAILURE_DEV_IN_REG,
	DEVICE_DEREGISTRATION_FAILURE_BUSY
}
t_en_DeRegistrationStatus;


#ifdef CONVERT_FUN

static u32 p_HanServer_ConvertFunData(PST_IE_HAN_MSG pstIe_Msg);

#define    REPORT_INTRF 0x6

#define    THERMO_INTRF1 0x301
#define    THERMO_INTRF2 0x7F15
#define    THERMO_INTRF3 0x7F12

#define    HUM_INTRF1 0x302
#define    HUM_INTRF2 0x7F14

#endif //CONVERT_FUN


#ifdef CAMERA_CONVERTION    

#define CAMERA_UNIT_TYPE        0xFF05
#define AC_UNIT_TYPE            0x106
#define TEMP_UNIT_TYPE            0x200
#define EVER_TEMP_UNIT_TYPE        0xFF01

/////////////////////////////////////////////////////////////////////////////////////////////
/// UID converstion
///

typedef struct {
	u16 u16_ID1;
	u16 u16_ID2;
} stIDPair;

static const stIDPair st_UnitIDConvTbl[] =
{
    { EVER_TEMP_UNIT_TYPE, TEMP_UNIT_TYPE },
#ifdef CAMERA_CONVERTION
    { CAMERA_UNIT_TYPE, AC_UNIT_TYPE },
#endif // CAMERA_CONVERTION
};







/////////////////////////////////////////////////////////////////////////////



#ifdef CONVERT_FUN
static u16 p_HanServer_ConvertUnitType(u16 u16_UType)
{
    u32 i;
    u16 u16_ResUType;

    u16_ResUType = u16_UType;

    for (i = 0; i < LENGTHOF(st_UnitIDConvTbl); i++)
    {
        if ( u16_UType == st_UnitIDConvTbl[i].u16_ID1 )
        {
            u16_ResUType = st_UnitIDConvTbl[i].u16_ID2;
            break;
        }
    }

    return u16_ResUType; 
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
/// Unit converstion
///

#ifdef CONVERT_FUN

typedef struct {
    u16 u16_IID;
    u8 u8_Offset;
    u8 u8_NumBytes;
    u8 u8_Multy;
} stUnitConvert;

static const stUnitConvert st_ThermoConvert[] =
{
    { THERMO_INTRF1, 3, 2, 10  },
    { THERMO_INTRF2, 0, 2, 1   },
    { THERMO_INTRF3, 0, 1, 100 }
};

static u16 u16_IfFilter[] = 
{
    HUM_INTRF1,
    HUM_INTRF2
};

#endif // CONVERT_FUN

// g_u16_CameraDevId is initialized only at reset.

static u16 g_u16_CameraDevId = 0;

#endif //CAMERA_CONVERTION

/**
*
*
* @brief
*  Command Used from Remote Client
*/
const t_CmdCb g_hanProxyCB[] =
{
    { "INIT", p_HanServer_InitRemote},
    { "OPEN_REG", p_HanServer_RegOpen },
    { "CLOSE_REG", p_HanServer_RegClose },
	{ "GET_DEV_TABLE_PHASE_2", p_HanServer_GetRegTablePhase2},
    { "GET_DEV_TABLE", p_HanServer_GetRegTable },
    { "GET_BLACK_LIST_DEV_TABLE", p_HanServer_GetBlackListTable },
    { "FUN_MSG", p_HanServer_FunMessage },
    { "DELETE_DEV", p_HanServer_CallDelete },
    { "GET_DEV_INFO_PHASE_2", p_HanServer_GetDeviceInfoPhase2 },
    { "GET_DEV_INFO", p_HanServer_GetDeviceInfo },
    { "RELEASE_LINK", p_HanServer_ReleaseLink },
    { "GET_TARGET_STATE", p_HanServer_GetTargetState},
    { NULL, p_HanServer_Dummy },
};

const t_CmdCb g_ServiceProxyCB[] =
{
	{ "GET_EEPROM_PARAM", p_HanServer_GetEepromParam },
	{ "SET_EEPROM_PARAM", p_HanServer_SetEepromParam },
	{ "GET_EEPROM_BYTES", p_HanServer_GetEepromBytes },
	{ "SET_EEPROM_BYTES", p_HanServer_SetEepromBytes },
	{ "GET_PRODUCTION_PARAM", p_HanServer_GetProductionParam },
	{ "SET_PRODUCTION_PARAM", p_HanServer_SetProductionParam },
	{ "GET_EEPROM_SIZE", p_HanServer_GetEepromSize },
	{ "SET_RF_STATE", p_HanServer_SetRFState},
	{ "GET_TARGET_HW_VERSION", p_HanServer_GetTargetHwVersion},
	{ "GET_SW_VERSION", p_HanServer_GetSwVersion},	
	{ "RESET_TARGET", p_HanServer_ResetTarget},
    { NULL, p_HanServer_Dummy },
};

const t_CmdCb g_DebugProxyCB[] =
{
	{ "GET_RAM_BYTES", p_HanServer_GetRamBytes },
	{ "SET_RAM_BYTES", p_HanServer_SetRamBytes },
	{ "GET_NUM_OF_FUN_MSG_IN_Q", p_HanServer_GetNumOfFunMsgInQ },
	{ "CLEAR_FUN_MSG_Q", p_HanServer_ClearFunMsgQ},
    { NULL, p_HanServer_Dummy },
};


#define APP_SERVICE_PROXY_MAX    16

typedef struct {
    const char *pSvcName;
    const t_CmdCb *pTbl;
} t_AppCmdCb;

// dynamic dispacth table for HAN Apps registration
static t_AppCmdCb g_AppServiceTblCB[APP_SERVICE_PROXY_MAX];

static const t_CmdCb * p_HanServer_FindAppMessageCallbackTbl(const char *pServiceName);

const t_CmdCb *p_HanServer_getHandler(const char* message)
{
    const t_CmdCb *handler;
	
    if (!strncmp(message, "[SRV]", strlen("[SRV]")))
	{
        handler = g_ServiceProxyCB;
	}
    else if (!strncmp(message, "[DBG]", strlen("[DBG]")))
	{
        handler = g_DebugProxyCB;
	}
    else if ((handler = p_HanServer_FindAppMessageCallbackTbl(message)) != NULL)
    {
        // private App. HAN table is found
    }
	else
	{
        handler = g_hanProxyCB;
	}

	return handler;

}



/**
 * Protocol - Fields and helper functions to retrieve specific 
 * fields 
 * 
 * @brief
 */
char *g_fieldID[] =
{
    "SRC_DEV_ID: ",
    "SRC_UNIT_ID: ",
    "DST_DEV_ID: ",
    "DST_UNIT_ID: ",
    "DEST_ADDRESS_TYPE: ",
    "MSG_TRANSPORT: ",
    "MSG_SEQ: ",
    "MSGTYPE: ",
    "INTRF_TYPE: ",
    "INTRF_ID: ",
    "INTRF_MEMBER: ",
    "DATALEN: ",
    "DATA: ",
    "VERSION: ",
    "TIME: ",
    "REASON: ",
    "DEV_INDEX: ",
    "HOW_MANY: ",
    "NO_OF_DEVICES: ",
    "DEV_ID: ",
    "DEV_IPUI: ",
    "NO_UNITS: ",
    "UNIT_TYPE: ",
    "UNIT_ID: ",
    "NO_OF_INTRF: ",
    "DEV_EMC: ",
    "BASE_RFPI: ",
    "NAME: ",
    "OFFSET: ",
    "SIZE: "  ,
    "STATE: ",
    "HW_CHIP: ",
    "HW_CHIP_VERSION: ",
    "HW_BOARD: ",
    "HW_COM_TYPE: ",
    "TARGET_VERSION: ",
	"TARGET_BUILD_NUMBER: ",
	"HOST_VERSION: ",
	"HOST_BUILD_NUMBER: ",
	"NUM_OF_PENDING_MSG: ",
	"MAX_MSG_IN_Q: ",
	"DEL_TYPE: "
};


enum e_fieldId
{
    HAN_SERVER_SRC_DEV_ID = 0,
    HAN_SERVER_SRC_UNIT_ID,
    HAN_SERVER_DST_DEV_ID,
    HAN_SERVER_DST_UNIT_ID,
    HAN_SERVER_DST_ADDRESS_TYPE,
    HAN_SERVER_MSG_TRANSPORT,
    HAN_SERVER_MSG_SEQUENCE,
    HAN_SERVER_MSG_TYPE,
    HAN_SERVER_INTRF_TYPE,
    HAN_SERVER_INTRF_ID,
    HAN_SERVER_INTRF_MEMBER,
    HAN_SERVER_DATALEN,
    HAN_SERVER_DATA,
    HAN_SERVER_PROTOCOL_VERSION,
    HAN_SERVER_TIME,
    HAN_SERVER_REASON,
    HAN_SERVER_DEVICE_INDEX,
    HAN_SERVER_HOW_MANY,
    HAN_SERVER_NO_OF_DEVICES,
    HAN_SERVER_DEV_ID,
    HAN_SERVER_DEVICE_IPUI,
    HAN_SERVER_NO_UNITS,
    HAN_SERVER_UNIT_TYPE,
    HAN_SERVER_UNIT_ID,
    HAN_SERVER_NO_OF_INTRF,
    HAN_SERVER_DEVICE_EMC,
    HAN_SERVER_BASE_RFPI,
    HAN_SERVER_NAME,
    HAN_SERVER_OFFSET,
    HAN_SERVER_SIZE,
    HAN_SERVER_STATE,  
    HAN_SERVER_HW_CHIP,  
    HAN_SERVER_HW_CHIP_VERSION,  
    HAN_SERVER_HW_BOARD,  
    HAN_SERVER_HW_COM_TYPE, 
    HAN_SERVER_TARGET_VERSION, 
    HAN_SERVER_TARGET_BUILD_NUMBER, 
    HAN_SERVER_HOST_VERSION, 
    HAN_SERVER_HOST_BUILD_NUMBER, 
    HAN_SERVER_NUM_OF_MSG,
    HAN_SERVER_MAX_NUM_OF_MSG,
    HAN_SERVER_DEL_TYPE,
};

char *g_EepromParamName[] =
{
	"RFPI",
	"RXTUN",
	"RF_FULL_POWER",
	"PREAM_NORM",
	"RF19APU_SUPPORT_FCC",
	"RF19APU_DEVIATION",
	"RF19APU_PA2_COMP",
	"MAX_USABLE_RSSI",
	"LOWER_RSSI_LIMIT",
	"PHS_SCAN_PARAM",
	"JDECT_LEVEL1_M82",
	"JDECT_LEVEL2_M62",
	"SUBS_DATA",
	"RVREF",
	"GFSK",
	"HAN_DECT_SUB_DB_START",
	"HAN_DECT_SUB_DB_END",  
	"HAN_ULE_SUB_DB_START", 
	"HAN_ULE_SUB_DB_END",   
	"HAN_FUN_SUB_DB_START", 
	"HAN_FUN_SUB_DB_END",   
	"HAN_ULE_NEXT_TPUI",
	"MAX_TRANSFER_SIZE",
	"HAN_FUN_GROUP_LIST_START",
	"HAN_FUN_GROUP_LIST_END",
	"HAN_FUN_GROUP_TABLE_START",
	"HAN_FUN_GROUP_TABLE_END",
	"HAN_ULE_BROADCAST_CONVERSION_TABLE_START",
	"HAN_ULE_BROADCAST_CONVERSION_TABLE_END",
	"ULE_MULTICAST_ENC_PARAMS",

    NULL
};

u32 g_EepromParamId[] =
{
	CMBS_PARAM_RFPI,
	CMBS_PARAM_RXTUN,
	CMBS_PARAM_RF_FULL_POWER,
	CMBS_PARAM_PREAM_NORM,
	CMBS_PARAM_RF19APU_SUPPORT_FCC,
	CMBS_PARAM_RF19APU_DEVIATION,
	CMBS_PARAM_RF19APU_PA2_COMP,
	CMBS_PARAM_MAX_USABLE_RSSI,
	CMBS_PARAM_LOWER_RSSI_LIMIT,
	CMBS_PARAM_PHS_SCAN_PARAM,
	CMBS_PARAM_JDECT_LEVEL1_M82,
	CMBS_PARAM_JDECT_LEVEL2_M62,
	CMBS_PARAM_SUBS_DATA,
	CMBS_PARAM_RVREF,
	CMBS_PARAM_GFSK,
	CMBS_PARAM_HAN_DECT_SUB_DB_START,
	CMBS_PARAM_HAN_DECT_SUB_DB_END,  
	CMBS_PARAM_HAN_ULE_SUB_DB_START, 
	CMBS_PARAM_HAN_ULE_SUB_DB_END,   
	CMBS_PARAM_HAN_FUN_SUB_DB_START, 
	CMBS_PARAM_HAN_FUN_SUB_DB_END,   
	CMBS_PARAM_HAN_ULE_NEXT_TPUI,
	CMBS_PARAM_MAX_TRANSFER_SIZE,
	CMBS_PARAM_HAN_FUN_GROUP_LIST_START,
	CMBS_PARAM_HAN_FUN_GROUP_LIST_END,
	CMBS_PARAM_HAN_FUN_GROUP_TABLE_START,
	CMBS_PARAM_HAN_FUN_GROUP_TABLE_END,
	CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_START,
	CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_END,
	CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS
};

char *g_ProdParamName[] =
{
	"DECT_TYPE",

    NULL
};

u32 g_ProdParamId[] =
{
	CMBS_PARAM_DECT_TYPE
};

u16 p_HanServer_serByte(t_st_paramList *pst_paramList, u8 u8_Value)
{
	if(pst_paramList->u16_MaxSize <= pst_paramList->u16_CurSize + sizeof(u8))
		return 0;

	memset(&(pst_paramList->pu8_Buffer[pst_paramList->u16_CurSize]),u8_Value,sizeof(u8_Value));
	pst_paramList->u16_CurSize = pst_paramList->u16_CurSize + sizeof(u8_Value);
	
    return sizeof(u8);
}

u16 p_HanServer_dserByte(unsigned char* pu8_Buffer, u8 *pu8_Value)
{
    if ( !pu8_Buffer || !pu8_Value )
        return 0;

    *pu8_Value = pu8_Buffer[0];
    return sizeof(u8);
}

u16 p_HanServer_serString(t_st_paramList *pst_paramList, unsigned char* value)
{
	if(pst_paramList->u16_MaxSize <= pst_paramList->u16_CurSize + sizeof(*value))
		return 0;

	memcpy(&(pst_paramList->pu8_Buffer[pst_paramList->u16_CurSize]),value,strlen((const char*)value));
	pst_paramList->u16_CurSize = pst_paramList->u16_CurSize + strlen((const char*)value);
	
	return strlen((const char*)value);
}

u16 p_HanServer_dserString(unsigned char* dest, unsigned char* src)
{
    int len;
    
    if ( !dest || !src )
        return 0;

    len = strlen((const char*)src);
	memcpy(dest, src, len);
	return len;
}

u16 p_HanServer_serSockAddr(t_st_paramList *pst_paramList, struct sockaddr_in* addres)
{
	if(pst_paramList->u16_MaxSize <= pst_paramList->u16_CurSize + sizeof(*addres))
		return 0;

	memcpy(&(pst_paramList->pu8_Buffer[pst_paramList->u16_CurSize]),addres,sizeof(*addres));
	pst_paramList->u16_CurSize = pst_paramList->u16_CurSize + sizeof(*addres);
	
    return sizeof(*addres);
}

u16 p_HanServer_dserSockAddr(struct sockaddr_in* addres, unsigned char* pu8_Buffer)
{
    if ( !pu8_Buffer || !addres )
        return 0;

    memcpy(addres,pu8_Buffer,sizeof(*addres));
    return sizeof(*addres);
}



u8 p_HanServer_Asc2Hex(char *psz_Digits)
{
    u8 u8_Value = 0;
    s32 i = 0, j = 1;
    if ( strlen(psz_Digits) < 2 )
        j = 0;

    for (i = 0; (i < 2) && (j >= 0); i++)
    {
        if ( psz_Digits[i] >= '0' && psz_Digits[i] <= '9' )
        {
            u8_Value |= (psz_Digits[i] - '0') << 4 * j;
        }
        else if ( psz_Digits[i] >= 'a' && psz_Digits[i] <= 'f' )
        {
            u8_Value |= (0x0a + (psz_Digits[i] - 'a')) << 4 * j;
        }
        else if ( psz_Digits[i] >= 'A' && psz_Digits[i] <= 'F' )
        {
            u8_Value |= (0x0a + (psz_Digits[i] - 'A')) << 4 * j;
        }
        j--;
    }

    return u8_Value;
}

u32 p_HanServer_GetIntFieldByStr(const char *pCommand, u32 u32_fieldId, s32 *ps32_Value)
{
    char *pCommandStart = NULL;
    u32 u32_commandLen = strlen(g_fieldID[u32_fieldId]);
#define MAX_VALUE_LEN 20
    char valueStr[MAX_VALUE_LEN + 1];
    u32 i,j=0;

    if ( pCommand == NULL )
    {
        HAN_SERVER_WARNING_PRINT("hanServer: p_HanServer_GetIntFieldByStr %d NOT FOUND ------- pCommand =NULL \n", u32_fieldId);
        return 0;
    }

    pCommandStart = strstr(pCommand, g_fieldID[u32_fieldId]);
    if ( pCommandStart != NULL )
    {
        //HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_GetIntFieldByStr Found CMD <%s>\n", g_fieldID[fieldId]);
        // to add here: if strcompare pCommandStart == "ALL" -- change *pValue = 0xFFFF
        for (i = 0; i < MAX_VALUE_LEN && pCommandStart[i + u32_commandLen] != '\r'; i++)
        {
        	if(pCommandStart[i + u32_commandLen] == ' ')
				continue;
            valueStr[j] = pCommandStart[i + u32_commandLen];
			j++;
            //HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_GetIntFieldByStr %d\n", valueStr[i]);
        }
        valueStr[j] = 0;
        *ps32_Value = atoi(valueStr);
        //HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_GetIntFieldByStr valueStr %d\n", *pValue);
        return 1;
    }
    else
    {
        //HAN_SERVER_WARNING_PRINT("hanServer: p_HanServer_GetIntFieldByStr NOT FOUND\n");
    }

    return 0;
}

void p_HanServer_RegisterCallBacks()
{
	
	s_fptrHanClient HanClientRegisterCallBacks = {p_HanServer_OnRegClosed,
												  p_HanServer_OnReadDeviceTableRes,
												  p_HanServer_OnReadDeviceSubInfoRes,
												  p_HanServer_OnMsgRecv,
												  p_HanServer_OnDeviceFullDeleted,
												  p_HanServer_OnDeviceRegistered,
												  p_HanServer_OnDevicePartialDeleted,
												  p_HanServer_OnOpenRegistrationRes,
												  p_HanServer_OnCloseRegistrationRes,
												  p_HanServer_OnHsRegistered, 
												  p_HanServer_OnTargetUpIndication, 
												  p_HanServer_OnEepParamGetRes, 
												  p_HanServer_OnEepParamSetRes, 
												  p_HanServer_OnEepAreaGetRes, 
												  p_HanServer_OnEepAreaSetRes,
												  p_HanServer_OnProdParamGetRes, 
												  p_HanServer_OnProdParamSetRes, 
												  p_HanServer_OnRamAreaGetRes, 
												  p_HanServer_OnRamAreaSetRes, 
												  p_HanServer_OnEepromSizeGetRes,
												  p_HanServer_OnTargetHwVersionGetRes,
												  p_HanServer_OnSwVersionGetRes,
												  p_HanServer_OnGetNumOfFunMsgInQRes,
												  p_HanServer_OnMessageSendRes,
												  p_HanServer_OnReadDeviceTablePhase2Res,
												  p_HanServer_OnReadDeviceSubInfoPhase2Res,
												  p_HanServer_OnReadBlackListDeviceTableRes,
												  p_HanServer_OnDeviceDeleted};
	
	app_HanMWRegisterCallBacks(&HanClientRegisterCallBacks);
	appcmbs_RegisterTargetAliveIndicationCB(p_HanServer_OnTargetAliveIndication);
}

u32 p_HanServer_GetStrFieldByStr(const char *pCommand, u32 u32_fieldId, char **ppData)
{
	char *pCommandStart = NULL;
    u32 u32_commandLen = strlen(g_fieldID[u32_fieldId]);
#define MAX_VALUE_LEN 20
    u32 i = 0;

    if ( pCommand == NULL )
    {
        HAN_SERVER_WARNING_PRINT("hanServer: p_HanServer_GetStrFieldByStr %d NOT FOUND ------- pCommand =NULL \n", u32_fieldId);
        return 0;
    }

    //HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_GetStrFieldByStr commandLen %d   cmd %s\n", u32_commandLen, g_fieldID[fieldId]);
    if ( (pCommandStart = strstr(pCommand, g_fieldID[u32_fieldId])) )
    {
        //HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_GetStrFieldByStr Found Start Of Data \n");
        *ppData = &(pCommandStart[i + u32_commandLen]);
    }

    return 0;
}

// Registration open request by remote
static u32 p_HanServer_RegOpen(const char *pCommand)
{
    s32 s32_value;

    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_RegOpen Received from Remote Client\n");

    if ( !p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_TIME, &s32_value) )    // update value if exists
    {
        s32_value = CMBS_SUBSCRIPTION_TIMEOUT;
    }

    // open only ULE subscription for HAN/ULE
    app_SrvSubscriptionOpen(s32_value);

    return 0;

}

// Registration close request by remote
static u32 p_HanServer_RegClose(const char *pCommand)
{
    UNUSED_PARAMETER(pCommand);
    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_RegClose Received from Remote Client\n");
    app_SrvSubscriptionClose();

    return 0;
}

static u32 p_HanServer_GetRegTable(const char *pCommand)
{
    s32 s32_NumOfEntries;
    s32 s32_IndexOfFirstEntry;

    if ( !p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEVICE_INDEX,  &s32_IndexOfFirstEntry) )
    {
        HAN_SERVER_WARNING_PRINT("Warning: Illegal index of entry from Remote \n");
        s32_IndexOfFirstEntry = 0;
    }

	if (!p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_HOW_MANY,  &s32_NumOfEntries))
    {
        HAN_SERVER_WARNING_PRINT("Warning: Illegal number of entries from Remote: (must not be 0 or > %d \n", MAX_APPLICATIVE_DEV_ENTRIES);
        s32_NumOfEntries = MAX_APPLICATIVE_DEV_ENTRIES;
    }


    ////    for testing: -- simulate single device event/////
    ////    app_DsrHanMwReadSingleDeviceRegistrationInformation(1);
    app_HanMwDeviceReadTable(s32_NumOfEntries, s32_IndexOfFirstEntry, FALSE);

    return 0;
}

static u32 p_HanServer_GetBlackListTable(const char *pCommand)
{
    s32 s32_NumOfEntries;
    s32 s32_IndexOfFirstEntry;

    if ( !p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEVICE_INDEX,  &s32_IndexOfFirstEntry) )
    {
        HAN_SERVER_WARNING_PRINT("Warning: Illegal index of entry from Remote \n");
        s32_IndexOfFirstEntry = 0;
    }

	if (!p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_HOW_MANY,  &s32_NumOfEntries))
    {
        HAN_SERVER_WARNING_PRINT("Warning: Illegal number of entries from Remote: (must not be 0 or > %d \n", MAX_APPLICATIVE_DEV_ENTRIES);
        s32_NumOfEntries = MAX_APPLICATIVE_DEV_ENTRIES;
    }

    app_HanMwDeviceReadBlackListTable(s32_NumOfEntries, s32_IndexOfFirstEntry);

    return 0;
}


static u32 p_HanServer_GetRegTablePhase2(const char *pCommand)
{
    s32 s32_NumOfEntries;
    s32 s32_IndexOfFirstEntry;

    if ( !p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEVICE_INDEX,  &s32_IndexOfFirstEntry) )
    {
        HAN_SERVER_WARNING_PRINT("Warning: Illegal index of entry from Remote \n");
        s32_IndexOfFirstEntry = 0;
    }

	if (!p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_HOW_MANY,  &s32_NumOfEntries))
    {
        HAN_SERVER_WARNING_PRINT("Warning: Illegal number of entries from Remote: (must not be 0 or > %d \n", MAX_APPLICATIVE_DEV_ENTRIES);
        s32_NumOfEntries = MAX_APPLICATIVE_DEV_ENTRIES;
    }

    app_HanMwDeviceReadTablePhase2(s32_NumOfEntries, s32_IndexOfFirstEntry);

    return 0;
}

// get device info by DECT ID
static u32 p_HanServer_GetDeviceInfo(const char *pCommand)
{
    s32 s32_deviceId = 0;

	if (p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEV_ID,  &s32_deviceId))
    {
        app_HanMwReadSingleDeviceRegistrationInformation(s32_deviceId);
    }
    return 0;
}

// get phase 2 device info by DECT ID
static u32 p_HanServer_GetDeviceInfoPhase2(const char *pCommand)
{
    s32 s32_deviceId = 0;
	
	if (p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEV_ID,  &s32_deviceId))
    {
        app_HanMwReadSingleDeviceRegistrationInformationPhase2(s32_deviceId);
    }
    return 0;
}

static u32 p_HanServer_ReleaseLink(const char *pCommand)
{
    s32 s32_deviceId = 0;

	if (p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEV_ID,  &s32_deviceId))
    {
        app_HanMWReleaseLinkRequest(s32_deviceId);
    }
    return 0;
}

static u32 p_HanServer_GetTargetState(const char *pCommand)
{
	sprintf((char *)AsciiMsgToClient, "TARGET_STATE\r\n STATE: %s\r\n\r\n", g_en_TargetState ? "UP" : "DOWN");
	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
    return 0;
}


static u32 p_HanServer_GetEepromParam(const char *pCommand)
{
	char *pValPtr;
	u32 Index = 0;
	
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_NAME, &pValPtr);

	// convert parameter name to CMBS API enum
	for(Index = 0; g_EepromParamName[Index]; Index++)
	{
		if(!strncmp(pValPtr, g_EepromParamName[Index], strlen(g_EepromParamName[Index])))
		{
			app_SrvParamGet(g_EepromParamId[Index], 0);
		}
	}

	return 0;
}

static u32 p_HanServer_SetEepromParam(const char *pCommand)
{
	char *pNameField, *pDataField, pHexBuffer[500] = {0}, *pTempStr;
	u32 Index = 0, j = 0, i;
	
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_NAME, &pNameField);
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_DATA, &pDataField);

	// remove '\r\n'
	pTempStr = strchr(pNameField, '\r');
	if(pTempStr)
	{
		pTempStr[0] = 0;
	}
	
	pTempStr = strchr(pDataField, '\r');
	if(pTempStr)
	{
		pTempStr[0] = 0;
	}
	
	for (i = 0; i <= strlen(pDataField); i += 2)
    {
        pHexBuffer[j] = p_HanServer_Asc2Hex(pDataField + i);
        j++;
    }
	
	// convert parameter name to CMBS API enum
	for(Index = 0; g_EepromParamName[Index]; Index++)
	{
		if (!strncmp(pNameField, g_EepromParamName[Index], strlen(g_EepromParamName[Index])))
		{
			app_SrvParamSet(g_EepromParamId[Index], (unsigned char*) &pHexBuffer[0], strlen(pDataField)/2, 0);
		}
	}

	return 0;
}

static u32 p_HanServer_GetProductionParam(const char *pCommand)
{
	char *pValPtr;
	u32 Index = 0;
	
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_NAME, &pValPtr);

	// convert parameter name to CMBS API enum
	for(Index = 0; g_ProdParamName[Index]; Index++)
	{
		if(!strncmp(pValPtr, g_ProdParamName[Index], strlen(g_ProdParamName[Index])))
		{
			app_ProductionParamGet(g_ProdParamId[Index], 0);
		}
	}

	return 0;
}

static u32 p_HanServer_SetProductionParam(const char *pCommand)
{
	char *pNameField, *pDataField, pHexBuffer[150] = {0}, *pTempStr;
	u32 Index = 0, j = 0, i;
	
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_NAME, &pNameField);
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_DATA, &pDataField);

	// remove '\r\n'
	pTempStr = strchr(pNameField, '\r');
	if(pTempStr)
	{
		pTempStr[0] = 0;
	}
	
	pTempStr = strchr(pDataField, '\r');
	if(pTempStr)
	{
		pTempStr[0] = 0;
	}
	
	for (i = 0; i <= strlen(pDataField); i += 2)
    {
        pHexBuffer[j] = p_HanServer_Asc2Hex(pDataField + i);
        j++;
    }
	
	// convert parameter name to CMBS API enum
	for(Index = 0; g_ProdParamName[Index]; Index++)
	{
		if (!strncmp(pNameField, g_ProdParamName[Index], strlen(g_ProdParamName[Index])))
		{
			app_ProductionParamSet(g_ProdParamId[Index], (unsigned char*) &pHexBuffer[0], strlen(pDataField)/2, 0);
		}
	}

	return 0;
}

static u32 p_HanServer_GetEepromBytes(const char *pCommand)
{
	s32 s32_Offset = 0, s32_Size = 0;

	p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_OFFSET, &s32_Offset);	
    p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_SIZE, &s32_Size);

	 app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_EEPROM, s32_Offset, (u16) s32_Size, 0);
 
	return 0;
}
    
static u32 p_HanServer_SetEepromBytes(const char *pCommand)
{
	char *pDataField, *pTempStr, pHexBuffer[1024] = {0};
	s32 s32_Offset = 0;
	u32 i, j = 0;

	p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_OFFSET,  &s32_Offset);	
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_DATA, &pDataField);

	pTempStr = strchr(pDataField, '\r');
	if(pTempStr)
	{
		pTempStr[0] = 0;
	}
	
	for (i = 0; i <= strlen(pDataField); i += 2)
    {
        pHexBuffer[j] = p_HanServer_Asc2Hex(pDataField + i);
        j++;
    }

	app_SrvParamAreaSet(CMBS_PARAM_AREA_TYPE_EEPROM, s32_Offset, strlen(pDataField)/2, (unsigned char *) &pHexBuffer[0], 0);

	return 0;
}

static u32 p_HanServer_SetRFState(const char *pCommand)
{
	char *pValPtr;
	
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_STATE, &pValPtr);

	if(!strncmp(pValPtr, "SUSPENDED", strlen("SUSPENDED")))
	{
		app_SrvRFSuspend();
	}
	
	else if(!strncmp(pValPtr, "RESUMED", strlen("RESUMED")))
	{
		app_SrvRFResume();
	}

	return 0;
}

static u32 p_HanServer_GetRamBytes(const char *pCommand)
{
	s32 s32_Offset = 0, s32_Size = 0;

	p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_OFFSET, &s32_Offset);	
    p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_SIZE, &s32_Size);

	 app_SrvParamAreaGet(CMBS_PARAM_AREA_TYPE_RAM, s32_Offset, (u16) s32_Size, 0);
 
	return 0;
}
    
static u32 p_HanServer_SetRamBytes(const char *pCommand)
{
	char *pDataField, *pTempStr, pHexBuffer[150] = {0};
	s32 s32_Offset = 0;
	u32 i, j = 0;

	p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_OFFSET,  &s32_Offset);	
    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_DATA, &pDataField);

	pTempStr = strchr(pDataField, '\r');
	if(pTempStr)
	{
		pTempStr[0] = 0;
	}
	
	for (i = 0; i <= strlen(pDataField); i += 2)
    {
        pHexBuffer[j] = p_HanServer_Asc2Hex(pDataField + i);
        j++;
    }

	app_SrvParamAreaSet(CMBS_PARAM_AREA_TYPE_RAM, s32_Offset, strlen(pDataField)/2, (unsigned char *) &pHexBuffer[0], 0);

	return 0;
}


static u32 p_HanServer_GetEepromSize(const char *pCommand)
{
	app_SrvGetEepromExtSize();

	return 0;
}

static u32 p_HanServer_GetTargetHwVersion(const char *pCommand)
{
	app_HanMWHwVersionGet();
	
	return 0;
}

static u32 p_HanServer_GetSwVersion(const char *pCommand)
{
	app_HanMWSwVersionGet();
	
	return 0;
}	

static u32 p_HanServer_ResetTarget(const char *pCommand)
{
	if(app_SrvSystemReboot() == CMBS_RC_OK)
	{
		p_HanSever_HandleSendMessageToClient("TARGET_STATE\r\n STATE: DOWN\r\n\r\n");
	}
	return 0;
}


static u32 p_HanServer_GetNumOfFunMsgInQ(const char *pCommand)
{
    s32 s32_deviceId = 0;

	if (p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEV_ID,  &s32_deviceId))
    {
        app_HanMWGetNumOfFunMsgInQ(s32_deviceId);
    }
    return 0;
}


static u32 p_HanServer_ClearFunMsgQ(const char *pCommand)
{
    s32 s32_deviceId = 0;

	if (p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEV_ID,  &s32_deviceId))
    {
        app_HanMWClearFunMsgQ(s32_deviceId);
    }
    return 0;
}

// MUST be called from main thread (due to container). Skipped for now.
//Init Locals that are target related (must be renewed after Target reset
u32 initHanServerLocals(void)
{
    ST_APPCMBS_CONTAINER st_Container;

	appcmbs_PrepareRecvAdd ( TRUE); // This function must be called before sending messages if you want to wait for event/container.
    app_SrvParamGet(CMBS_PARAM_RFPI, 1);
    appcmbs_WaitForContainer(CMBS_EV_DSR_PARAM_GET_RES, &st_Container); // This function must be called only after appcmbs_PrepareRecvAdd ( TRUE)
    // fill RFPI for future useage
    memcpy(&u8_BaseRfpi[0], st_Container.ch_Info, CMBS_PARAM_RFPI_LENGTH);
    //HAN_SERVER_INFO_PRINT("Current RFPI: \t %.2X%.2X%.2X%.2X%.2X\n", u8_BaseRfpi[0], u8_BaseRfpi[1], u8_BaseRfpi[2], u8_BaseRfpi[3], u8_BaseRfpi[4]);

    return 0;

}

// A remote sent an INIT message
static u32 p_HanServer_InitRemote(const char *pCommand)
{
    s32 s32_version;
    RemoteConnected = TRUE;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_PROTOCOL_VERSION, &s32_version) )
    {
        HAN_SERVER_INFO_PRINT("INIT Remote -- Remote Protocol Version = %d\n", s32_version);
    }
    else
    {
        HAN_SERVER_WARNING_PRINT("INIT Remote -- Remote Protocol Version unknown\n");
    }

    // Send INIT_RES
    // For now: no RFPI is sent
    //    sprintf(temp_buffer, "INIT_RES\r\n %s%d\r\n %s %d %d %d %d %d\r\n\r\n",

    sprintf((char *)AsciiMsgToClient, "INIT_RES\r\n %s%d\r\n\r\n",

            g_fieldID[HAN_SERVER_PROTOCOL_VERSION],
            HAN_SERVER_PROTOCOL_VERSION_NUMBER
            //    , g_fieldID[HAN_SERVER_BASE_RFPI],
            //    u8_BaseRfpi[0], u8_BaseRfpi[1], u8_BaseRfpi[2], u8_BaseRfpi[3], u8_BaseRfpi[4]
           );

	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
    return 0;

}

s32 p_HanServer_PopulateBinData(char *pCommand, u16 u16_dataLength, unsigned char *pBinOut)
{
    u32 inx;
    unsigned char *pAsciiData;
    u32 u32_BytesRead = 0;           // Total read
    u32 u32_DataBytesRead = 0;       // This iteration read

    pAsciiData = (unsigned char *)pCommand;

    if ( pAsciiData )
    {
        //HAN_SERVER_INFO_PRINT("pAsciiData=%s\n", pAsciiData);
        for (inx = 0; inx < u16_dataLength; inx++)
        {

            if ( sscanf((const char *)pAsciiData + u32_BytesRead, " %hhx %n", &pBinOut[inx], &u32_DataBytesRead) < 0 )            
            {
                HAN_SERVER_WARNING_PRINT("failed to read data byte %d \n", inx);
                return -1;
            }

            u32_BytesRead += u32_DataBytesRead;

            // HAN_SERVER_INFO_PRINT("p_HanServer_PopulateBinData: inx= %d pBinOut[inx] = %x\n",inx, pBinOut[inx]);
        }
    }
    else
    {
        return -1;
    }
    return 0;
}

void p_HanServer_PopulateHexValue(unsigned char *pInBinData, u16 *pu16_len, unsigned char *pOutHexBuf)
{
    u32 inx;
    unsigned char *pHexOut;
    pHexOut = pOutHexBuf;
    for (inx = 0; inx < *pu16_len; inx++)
    {
        sprintf((char *)pHexOut, " %02x", pInBinData[inx]);
        pHexOut += 3;

        //        // Old: Decimal notation:
        //       sprintf((char*)pHexOut, " %03d", pInBinData[inx]);
        //       pHexOut += 4;

    }
    //    *pu16_len = (*pu16_len) * 4;    // Decimal length + 1 space
    *pu16_len = (*pu16_len) * 3;    // Hex lenth + 1 space
}

void p_HanServer_PopulateHexValueNoSpaces(unsigned char *pInBinData, u16 *pu16_len, unsigned char *pOutHexBuf)
{
    u32 inx;
    unsigned char *pHexOut;
    pHexOut = pOutHexBuf;
    for (inx = 0; inx < *pu16_len; inx++)
    {
        sprintf((char *)pHexOut, "%02x", pInBinData[inx]);
        pHexOut += 2;
    }
    *pu16_len = (*pu16_len) * 2;    // Hex lenth + 1 space
}

static u32 p_HanServer_CallDelete(const char *pCommand)
{
    s32 s32_value;
	char *pDelType = NULL;

	bool b_BlackList = TRUE;
		
    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_CallDelete from remote: Command: %s\n", pCommand);

	if (p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DEV_ID,  &s32_value))
    {
        HAN_SERVER_INFO_PRINT("Device Id = %d\n", s32_value);
    }

	p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_DEL_TYPE,  &pDelType);

	if(pDelType)
    {
        HAN_SERVER_INFO_PRINT("Del type = %s\n", pDelType);

		if(!strncmp(pDelType, "LOCAL", strlen("LOCAL")))
		{
			b_BlackList = FALSE;
		}
    }

    app_HanMWDeleteDeviceViaQueue(s32_value, b_BlackList);
	
    return 0;
}


/*

FUN MSG STRUCTURE:

 u16                 u16_SrcDeviceId;     
 u8                  u8_SrcUnitId;
 u16                 u16_DstDeviceId;     
 u8                  u8_DstUnitId;
 u8                  u8_DstAddressType; //(0x00=Individual Address, 0x01=Group Address)

ST_HAN_MSG_TRANSPORT st_MsgTransport;     // transport layer (Encryption, Reliable Connection, etc.)
u8                     u8_MsgSequence;     // will be returned in the response
u8                     e_MsgType;          // message type
u8                     u8_InterfaceType;     // 1=server, 0=client
u16                  u16_InterfaceId;
u8                     u8_InterfaceMember; // depending on message type, Command or attribute id or attribute pack id
u16                  u16_DataLen;
u8 *                 pu8_Data;

*/
// fun message arrived from remote side

static char outBinBuf[CMBS_HAN_MAX_MSG_DATA_LEN+20];        // Binary buffer to be sent to device with some spare

static u32 p_HanServer_FunMessage(const char *pCommand)
{
    ST_IE_HAN_MSG pst_HANMsg;
    ST_IE_HAN_MSG_CTL st_HANMsgCtl = { 0, 0, 0 };

    char *pValPtr;
    s32 s32_value;

    HAN_SERVER_INFO_PRINT("hanServer: FUN Message Received from Remote Client \n");
    memset(&pst_HANMsg, 0, sizeof(ST_IE_HAN_MSG));

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_SRC_DEV_ID, &s32_value) )
        pst_HANMsg.u16_SrcDeviceId = (short)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_SRC_UNIT_ID, &s32_value) )
        pst_HANMsg.u8_SrcUnitId = (u8)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DST_DEV_ID, &s32_value) )
        pst_HANMsg.u16_DstDeviceId = (short)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DST_UNIT_ID, &s32_value) )
        pst_HANMsg.u8_DstUnitId = (u8)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DST_ADDRESS_TYPE, &s32_value) )
        pst_HANMsg.u8_DstAddressType = (u8)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_MSG_TRANSPORT, &s32_value) )
        pst_HANMsg.st_MsgTransport.u16_Reserved = (u16)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_MSG_SEQUENCE, &s32_value) )
        pst_HANMsg.u8_MsgSequence = (u8)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_MSG_TYPE, &s32_value) )
        pst_HANMsg.e_MsgType = s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_INTRF_TYPE, &s32_value) )
        pst_HANMsg.u8_InterfaceType = s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_INTRF_ID, &s32_value) )
        pst_HANMsg.u16_InterfaceId = (short)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_INTRF_MEMBER, &s32_value) )
        pst_HANMsg.u8_InterfaceMember = (u8)s32_value;

    if ( p_HanServer_GetIntFieldByStr(pCommand, HAN_SERVER_DATALEN, &s32_value) )
        pst_HANMsg.u16_DataLen = (u16)s32_value;

    p_HanServer_GetStrFieldByStr(pCommand, HAN_SERVER_DATA, &pValPtr);

    /// for testing: Simulate additional data for on/off FUN message of an AC
    ///    pst_HANMsg.u16_DataLen = 8;
    ///
    if ( pst_HANMsg.u16_DataLen )
    {
        /// for testing:
        ///        char temp_data[40]="1C A  50 0 03   11 BB 99";
        ///        pValPtr = temp_data;

        HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_FunMessage Data Length %d\n", pst_HANMsg.u16_DataLen);
        if ( pst_HANMsg.u16_DataLen < CMBS_HAN_MAX_MSG_DATA_LEN )
        {
            if ( pValPtr )
            {
                if ( p_HanServer_PopulateBinData(pValPtr, pst_HANMsg.u16_DataLen, (unsigned char *)outBinBuf) < 0 )
                {
                    // Failure in parsing
                    HAN_SERVER_ERROR_PRINT("hanServer: p_HanServer_FunMessage was not sent to device! \n");
                    return -1;
                }
                pst_HANMsg.pu8_Data = (u8 *)outBinBuf;
            }
            else
                pst_HANMsg.u16_DataLen = 0;
        }
        else
            pst_HANMsg.u16_DataLen = 0;
    }

#ifdef CAMERA_CONVERSION
#ifdef CMBS_PLUGIN
printf("1 Dev ID %d Int Type %d Int ID 0x%X\n", pst_HANMsg.u16_DstDeviceId, pst_HANMsg.u8_InterfaceType, pst_HANMsg.u16_InterfaceId);
    // convert AC on/off to Camera on/off and send to plugin instead of to device
    if (g_u16_CameraDevId == pst_HANMsg.u16_DstDeviceId &&
        pst_HANMsg.u8_InterfaceType == 1 &&
        pst_HANMsg.u16_InterfaceId == 0x200)
    {
        HAN_SERVER_INFO_PRINT("Send Message to plugin: Dev ID %u, event = %d 1=on/2=off \n", pst_HANMsg.u16_DstDeviceId, pst_HANMsg.u8_InterfaceMember);
        // convert AC on/off to Camera on/off and send to plugin instead of to device
        // send to plugin pst_HANMsg.u8_InterfaceMember    : 1 = on ; 2 = off
        CMBS_HandleEventPlugins(NULL, pst_HANMsg.u16_InterfaceId, &pst_HANMsg);
        
        return 0;
    }
#endif // CMBS_PLUGIN
#endif // CAMERA_CONVERSION

    // Delay till Tx ready arrives
    app_HanMwMsgSend_ImplicitLinkSetup(12, pst_HANMsg.u16_DstDeviceId, &st_HANMsgCtl, &pst_HANMsg);

    return 0;
}

u32 p_HanServer_Dummy(const char *pCommand)
{
    HAN_SERVER_WARNING_PRINT("hanServer: p_HanServer_Dummy couldnt find command %s\n", pCommand);

    return 0;
}


/**********************************************************************
 *  Callbacks from CMBS Target
 ***********************************************************************/

t_CmdCb g_hanOnTargetEvRcvdCB[] =
{
    { "TARGET_SYS_READY", p_HanServer_EventCB },
    { "", p_HanServer_Dummy },
};

static u32 p_HanServer_EventCB(const char *pCommand)
{
    PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
    //    ST_HAN_MSG_REG_INFO     pst_HANMsgRegInfo;
    UNUSED_PARAMETER(p_List);
    UNUSED_PARAMETER(pCommand);
    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_EventCB\n");

    return 0;
}

void p_HanServer_OnRegClosed(E_CMBS_EVENT_ID e_EventID, void *pv_EventData)
{
    void *pv_IE = NULL;
    u16 u16_IE;
    //    ST_IE_RESPONSE          st_Response;
    ST_IE_REG_CLOSE_REASON st_RegCloseReason;

    st_RegCloseReason.e_Reg_Close_Reason = CMBS_REG_CLOSE_MAX;
    UNUSED_PARAMETER(e_EventID);
    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        switch (u16_IE)
        {
        case CMBS_IE_REG_CLOSE_REASON:
            app_IEToString(pv_IE, u16_IE);
            cmbs_api_ie_RegCloseReasonGet(pv_IE, &st_RegCloseReason);
            break;
        case CMBS_IE_RESPONSE:
            break;
        }

        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);
    }


    if ( st_RegCloseReason.e_Reg_Close_Reason == CMBS_REG_CLOSE_HS_REGISTERED )
    {
        // Do not send indication from her, only in timeout case!
        HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_OnRegClosed Registration Succeeded \n");
    }
    else
    {
        HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_OnRegClosed Registration TimeOut \n");
		p_HanSever_HandleSendMessageToClient("REG_CLOSED\r\n REASON: TIMEOUT\r\n\r\n");
    }
}

// Fill a string which describes a single device.
// Input parameters: pointer to EXTENDED_DEVICE_INFO structure; pointer to a predefined initialized buffer
void p_HanServer_FillSingleDeviceString(PST_HAN_EXTENDED_DEVICE_INFO pDeviceInfo, u8 *pTmpBufPtr)
{
    u8 	u8_Units, u8_no_units = 0;
    u16 u16_Interfaces;
    u16 u16_ReportedUnitType;

    sprintf((char*)pTmpBufPtr, " %s %d\r\n",
            g_fieldID[HAN_SERVER_DEV_ID],
            pDeviceInfo->u16_DeviceId);
    
    pTmpBufPtr += strlen((char*)pTmpBufPtr);
    
    sprintf((char*)pTmpBufPtr, " %s %d %d %d %d %d\r\n",
            g_fieldID[HAN_SERVER_DEVICE_IPUI],
            pDeviceInfo->u8_IPUI[0],pDeviceInfo->u8_IPUI[1], pDeviceInfo->u8_IPUI[2], pDeviceInfo->u8_IPUI[3], pDeviceInfo->u8_IPUI[4]);
    
    pTmpBufPtr += strlen((char*)pTmpBufPtr);
    
    sprintf((char*)pTmpBufPtr, " %s %hhu %hhu\r\n",
            g_fieldID[HAN_SERVER_DEVICE_EMC],
            pDeviceInfo->u16_DeviceEMC & 0xFF,
            (pDeviceInfo->u16_DeviceEMC >> 8) & 0xFF);
    
    pTmpBufPtr += strlen((char*)pTmpBufPtr);

	u8_no_units = pDeviceInfo->u8_NumberOfUnits;
    sprintf((char*)pTmpBufPtr, " NO_UNITS: %d\r\n", u8_no_units);
    pTmpBufPtr += strlen((char*)pTmpBufPtr);
    
    for (u8_Units = 0; u8_Units < u8_no_units; u8_Units++)
    {
        u16 u16_no_intrf;
                            
        sprintf((char*)pTmpBufPtr, " %s %d\r\n",
            g_fieldID[HAN_SERVER_UNIT_ID],
            pDeviceInfo->st_UnitsInfo[u8_Units].u8_UnitId);
                            
        pTmpBufPtr += strlen((char*)pTmpBufPtr);    


        u16_ReportedUnitType = pDeviceInfo->st_UnitsInfo[u8_Units].u16_UnitType;
    
#ifdef CAMERA_CONVERTION    
        if (u16_ReportedUnitType == CAMERA_UNIT_TYPE)
        {
            g_u16_CameraDevId = pDeviceInfo->u16_DeviceId;

            HAN_SERVER_INFO_PRINT("g_u16_CameraDevId was set to device %d \n", pDeviceInfo->u16_DeviceId);

            u16_ReportedUnitType = AC_UNIT_TYPE;
        }
#endif 

#ifdef CONVERT_FUN
        u16_ReportedUnitType = p_HanServer_ConvertUnitType(u16_ReportedUnitType);
        HAN_SERVER_INFO_PRINT("Convert UNIT ID 0x%X -> 0x%X\n", pDeviceInfo->st_UnitsInfo[u8_Units].u16_UnitType, u16_ReportedUnitType);
#endif // CONVERT_FUN

        sprintf((char*)pTmpBufPtr, " %s %d\r\n",
            g_fieldID[HAN_SERVER_UNIT_TYPE],
            u16_ReportedUnitType);
    
        pTmpBufPtr += strlen((char*)pTmpBufPtr);

        u16_no_intrf = pDeviceInfo->st_UnitsInfo[u8_Units].u16_NumberOfOptionalInterfaces;
    
        sprintf((char*)pTmpBufPtr, " NO_OF_INTRF: %d\r\n",u16_no_intrf);
                            
        pTmpBufPtr += strlen((char*)pTmpBufPtr);
                            
        for (u16_Interfaces = 0; u16_Interfaces < u16_no_intrf; u16_Interfaces++)
        {
            u8 intrf_type;
            u16 intrf_id;

            // Divide Interface field into Interface type (ms bit) and Interface Id
            intrf_id = pDeviceInfo->st_UnitsInfo[u8_Units].u16_OptionalInterfaces[u16_Interfaces];
            intrf_type = (intrf_id & 0xffff)? 1:0;
            intrf_id &= 0x7fff;

            sprintf((char*)pTmpBufPtr, " %s %d\r\n",
                    g_fieldID[HAN_SERVER_INTRF_TYPE],
                    intrf_type);
            pTmpBufPtr += strlen((char*)pTmpBufPtr);

            sprintf((char*)pTmpBufPtr, " %s %d\r\n",
                    g_fieldID[HAN_SERVER_INTRF_ID],
                    intrf_id);
            pTmpBufPtr += strlen((char*)pTmpBufPtr);                        
        }
    }
}

void p_HanServer_FillSingleDevicePhase2String(PST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 pDeviceInfo, u8 *pTmpBufPtr)
{
    u8 	u8_Units, u8_no_units = 0;
    u16 u16_Interfaces;
    u16 u16_ReportedUnitType;

    sprintf((char*)pTmpBufPtr, " %s %d\r\n",
            g_fieldID[HAN_SERVER_DEV_ID],
            pDeviceInfo->u16_DeviceId);
    
    pTmpBufPtr += strlen((char*)pTmpBufPtr);
    
    sprintf((char*)pTmpBufPtr, " %s %d %d %d %d %d\r\n",
            g_fieldID[HAN_SERVER_DEVICE_IPUI],
            pDeviceInfo->u8_IPUI[0],pDeviceInfo->u8_IPUI[1], pDeviceInfo->u8_IPUI[2], pDeviceInfo->u8_IPUI[3], pDeviceInfo->u8_IPUI[4]);
    
    pTmpBufPtr += strlen((char*)pTmpBufPtr);
    
    sprintf((char*)pTmpBufPtr, " %s %hhu %hhu\r\n",
            g_fieldID[HAN_SERVER_DEVICE_EMC],
            pDeviceInfo->u16_DeviceEMC & 0xFF,
            (pDeviceInfo->u16_DeviceEMC >> 8) & 0xFF);
    
    pTmpBufPtr += strlen((char*)pTmpBufPtr);
    
	sprintf((char*)pTmpBufPtr, " ULE_CAPABILITIES: %d\r\n", pDeviceInfo->u8_ULECapabilities);
	pTmpBufPtr += strlen((char*)pTmpBufPtr);
		
	sprintf((char*)pTmpBufPtr, " ULE_PROTOCOL_ID: %d\r\n", pDeviceInfo->u8_UleAppProtocolId);
	pTmpBufPtr += strlen((char*)pTmpBufPtr);
		
	sprintf((char*)pTmpBufPtr, " ULE_PROTOCOL_VERSION: %d\r\n", pDeviceInfo->u8_UleAppProtocolVersion);
	pTmpBufPtr += strlen((char*)pTmpBufPtr);

	u8_no_units = pDeviceInfo->u8_NumberOfUnits;
    sprintf((char*)pTmpBufPtr, " NO_UNITS: %d\r\n", u8_no_units);
    pTmpBufPtr += strlen((char*)pTmpBufPtr);
    
    for (u8_Units = 0; u8_Units < u8_no_units; u8_Units++)
    {
        u16 u16_no_intrf;
                            
        sprintf((char*)pTmpBufPtr, " %s %d\r\n",
            g_fieldID[HAN_SERVER_UNIT_ID],
            pDeviceInfo->st_UnitsInfo[u8_Units].u8_UnitId);
                            
        pTmpBufPtr += strlen((char*)pTmpBufPtr);    


        u16_ReportedUnitType = pDeviceInfo->st_UnitsInfo[u8_Units].u16_UnitType;
    
#ifdef CAMERA_CONVERTION    
        if (u16_ReportedUnitType == CAMERA_UNIT_TYPE)
        {
            g_u16_CameraDevId = pDeviceInfo->u16_DeviceId;

            HAN_SERVER_INFO_PRINT("g_u16_CameraDevId was set to device %d \n", pDeviceInfo->u16_DeviceId);

            u16_ReportedUnitType = AC_UNIT_TYPE;
        }
#endif 

#ifdef CONVERT_FUN
        u16_ReportedUnitType = p_HanServer_ConvertUnitType(u16_ReportedUnitType);
        HAN_SERVER_INFO_PRINT("Convert UNIT ID 0x%X -> 0x%X\n", pDeviceInfo->st_UnitsInfo[u8_Units].u16_UnitType, u16_ReportedUnitType);
#endif // CONVERT_FUN

        sprintf((char*)pTmpBufPtr, " %s %d\r\n",
            g_fieldID[HAN_SERVER_UNIT_TYPE],
            u16_ReportedUnitType);
    
        pTmpBufPtr += strlen((char*)pTmpBufPtr);

        u16_no_intrf = pDeviceInfo->st_UnitsInfo[u8_Units].u16_NumberOfOptionalInterfaces;
    
        sprintf((char*)pTmpBufPtr, " NO_OF_INTRF: %d\r\n",u16_no_intrf);
                            
        pTmpBufPtr += strlen((char*)pTmpBufPtr);
                            
        for (u16_Interfaces = 0; u16_Interfaces < u16_no_intrf; u16_Interfaces++)
        {
            u8 intrf_type;
            u16 intrf_id;

            // Divide Interface field into Interface type (ms bit) and Interface Id
            intrf_id = pDeviceInfo->st_UnitsInfo[u8_Units].u16_OptionalInterfaces[u16_Interfaces];
            intrf_type = (intrf_id & 0xffff)? 1:0;
            intrf_id &= 0x7fff;

            sprintf((char*)pTmpBufPtr, " %s %d\r\n",
                    g_fieldID[HAN_SERVER_INTRF_TYPE],
                    intrf_type);
            pTmpBufPtr += strlen((char*)pTmpBufPtr);

            sprintf((char*)pTmpBufPtr, " %s %d\r\n",
                    g_fieldID[HAN_SERVER_INTRF_ID],
                    intrf_id);
            pTmpBufPtr += strlen((char*)pTmpBufPtr);                        
        }
    }
}

void p_HanServer_OnReadDeviceTableRes(void *pv_EventData)
{
    void *pv_IE = NULL;
    u16 u16_IE, u16_Length = 0;
    u8 *pTmpBufPtr;
    u8 u8_ArrEntry = 0;
	ST_IE_RESPONSE st_Response;
    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_OnReadDeviceTableRes--> \n");
    memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
    pTmpBufPtr = AsciiMsgToClient;

    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_ENTRIES )
        {
            ST_HAN_EXTENDED_DEVICE_INFO arr_Devices[MAX_APPLICATIVE_DEV_ENTRIES];
            ST_IE_HAN_EXTENDED_DEVICE_ENTRIES st_HANDeviceEntries;
            st_HANDeviceEntries.pst_DeviceEntries = arr_Devices;
            //HAN_SERVER_INFO_PRINT("st_HANDeviceEntries.pst_DeviceEntries=%d\n",st_HANDeviceEntries.pst_DeviceEntries);

            cmbs_api_ie_HANDeviceTableExtendedGet(pv_IE, &st_HANDeviceEntries);
            u16_Length = st_HANDeviceEntries.u16_NumOfEntries * sizeof(ST_HAN_EXTENDED_DEVICE_INFO);
            memcpy(&arr_Devices, (u8 *)st_HANDeviceEntries.pst_DeviceEntries, u16_Length);

            sprintf((char *)pTmpBufPtr, "DEV_TABLE\r\n DEV_INDEX: %d\r\n NO_OF_DEVICES: %d\r\n",
                    st_HANDeviceEntries.u16_StartEntryIndex,
                    st_HANDeviceEntries.u16_NumOfEntries);

            pTmpBufPtr += strlen((char *)pTmpBufPtr);

            for (u8_ArrEntry = 0; u8_ArrEntry < st_HANDeviceEntries.u16_NumOfEntries && u8_ArrEntry < MAX_APPLICATIVE_DEV_ENTRIES; u8_ArrEntry++)
            {

                p_HanServer_FillSingleDeviceString(&(arr_Devices[u8_ArrEntry]), pTmpBufPtr);
                pTmpBufPtr += strlen((char *)pTmpBufPtr);


            }
            sprintf((char *)pTmpBufPtr, "\r\n");
			p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

            break;
        }

		if ( u16_IE == CMBS_IE_RESPONSE )
		{
        	cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
			if(st_Response.e_Response!= CMBS_RESPONSE_OK)
			{
			
				sprintf((char *)pTmpBufPtr, "DEV_TABLE\r\n DEV_INDEX: 0\r\n NO_OF_DEVICES: 0\r\n\r\n");
				
				p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
				break;
			}
		}
        app_IEToString(pv_IE, u16_IE);
        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);

    }
}


void p_HanServer_OnReadDeviceTablePhase2Res(void *pv_EventData)
{
    void *pv_IE = NULL;
    u16 u16_IE, u16_Length = 0;
    u8 *pTmpBufPtr;
    u8 u8_ArrEntry = 0;
	ST_IE_RESPONSE st_Response;
    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_OnReadDeviceTablePhase2Res--> \n");
    memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
    pTmpBufPtr = AsciiMsgToClient;

    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_PHASE_2_ENTRIES)
        {
            ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 arr_Devices[MAX_APPLICATIVE_DEV_ENTRIES];
            ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES st_HANDeviceEntries;
            st_HANDeviceEntries.pst_DeviceEntries = arr_Devices;
            //HAN_SERVER_INFO_PRINT("st_HANDeviceEntries.pst_DeviceEntries=%d\n",st_HANDeviceEntries.pst_DeviceEntries);

            cmbs_api_ie_HANDeviceTableExtendedPhase2Get(pv_IE, &st_HANDeviceEntries);
            u16_Length = st_HANDeviceEntries.u16_NumOfEntries * sizeof(ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2);
            memcpy(&arr_Devices, (u8 *)st_HANDeviceEntries.pst_DeviceEntries, u16_Length);

            sprintf((char *)pTmpBufPtr, "DEV_TABLE_PHASE_2\r\n DEV_INDEX: %d\r\n NO_OF_DEVICES: %d\r\n",
                    st_HANDeviceEntries.u16_StartEntryIndex,
                    st_HANDeviceEntries.u16_NumOfEntries);

            pTmpBufPtr += strlen((char *)pTmpBufPtr);

            for (u8_ArrEntry = 0; u8_ArrEntry < st_HANDeviceEntries.u16_NumOfEntries && u8_ArrEntry < MAX_APPLICATIVE_DEV_ENTRIES; u8_ArrEntry++)
            {

                p_HanServer_FillSingleDevicePhase2String(&(arr_Devices[u8_ArrEntry]), pTmpBufPtr);
                pTmpBufPtr += strlen((char *)pTmpBufPtr);


            }
            sprintf((char *)pTmpBufPtr, "\r\n");
			p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

            break;
        }

		if ( u16_IE == CMBS_IE_RESPONSE )
		{
        	cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
			if(st_Response.e_Response!= CMBS_RESPONSE_OK)
			{
			
				sprintf((char *)pTmpBufPtr, "DEV_TABLE_PHASE_2\r\n DEV_INDEX: 0\r\n NO_OF_DEVICES: 0\r\n\r\n");
				
				p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
				break;
			}
		}
        app_IEToString(pv_IE, u16_IE);
        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);

    }
}


void p_HanServer_OnReadBlackListDeviceTableRes(void *pv_EventData)
{
    void *pv_IE = NULL;
    u16 u16_IE, u16_Length = 0;
    u8 *pTmpBufPtr;
    u8 u8_ArrEntry = 0;
	ST_IE_RESPONSE st_Response;
    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_OnReadBlackListDeviceTableRes--> \n");
    memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
    pTmpBufPtr = AsciiMsgToClient;

    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_PHASE_2_ENTRIES)
        {
            ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 arr_Devices[MAX_APPLICATIVE_DEV_ENTRIES];
            ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES st_HANDeviceEntries;
            st_HANDeviceEntries.pst_DeviceEntries = arr_Devices;
            //HAN_SERVER_INFO_PRINT("st_HANDeviceEntries.pst_DeviceEntries=%d\n",st_HANDeviceEntries.pst_DeviceEntries);

            cmbs_api_ie_HANDeviceTableExtendedPhase2Get(pv_IE, &st_HANDeviceEntries);
            u16_Length = st_HANDeviceEntries.u16_NumOfEntries * sizeof(ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2);
            memcpy(&arr_Devices, (u8 *)st_HANDeviceEntries.pst_DeviceEntries, u16_Length);

            sprintf((char *)pTmpBufPtr, "BLACK_LIST_DEV_TABLE\r\n DEV_INDEX: %d\r\n NO_OF_DEVICES: %d\r\n",
                    st_HANDeviceEntries.u16_StartEntryIndex,
                    st_HANDeviceEntries.u16_NumOfEntries);

            pTmpBufPtr += strlen((char *)pTmpBufPtr);

            for (u8_ArrEntry = 0; u8_ArrEntry < st_HANDeviceEntries.u16_NumOfEntries && u8_ArrEntry < MAX_APPLICATIVE_DEV_ENTRIES; u8_ArrEntry++)
            {

                p_HanServer_FillSingleDevicePhase2String(&(arr_Devices[u8_ArrEntry]), pTmpBufPtr);
                pTmpBufPtr += strlen((char *)pTmpBufPtr);


            }
            sprintf((char *)pTmpBufPtr, "\r\n");
			p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

            break;
        }

		if ( u16_IE == CMBS_IE_RESPONSE )
		{
        	cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
			if(st_Response.e_Response!= CMBS_RESPONSE_OK)
			{
			
				sprintf((char *)pTmpBufPtr, "BLACK_LIST_DEV_TABLE\r\n DEV_INDEX: 0\r\n NO_OF_DEVICES: 0\r\n\r\n");
				
				p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
				break;
			}
		}
        app_IEToString(pv_IE, u16_IE);
        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);

    }
}

void p_HanServer_OnReadDeviceSubInfoRes(void *pv_EventData)
{

    void *pv_IE = NULL;
    u16 u16_IE;
    u8 *pTmpBufPtr;
	ST_IE_RESPONSE st_Response;

    ST_HAN_EXTENDED_DEVICE_INFO DeviceInfo;

    HAN_SERVER_INFO_PRINT("p_HanServer_OnReadDeviceSubInfoRes -->\n");

    memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
    pTmpBufPtr = AsciiMsgToClient;

    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO )
        {
            cmbs_api_ie_HANDeviceSubInfoGet(pv_IE, &DeviceInfo);

            sprintf((char *)pTmpBufPtr, "DEV_INFO\r\n");

            pTmpBufPtr += strlen((char *)pTmpBufPtr);

            p_HanServer_FillSingleDeviceString(&DeviceInfo, pTmpBufPtr);
            pTmpBufPtr += strlen((char *)pTmpBufPtr);


            sprintf((char *)pTmpBufPtr, "\r\n");
			p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

            break;
        }
		if ( u16_IE == CMBS_IE_RESPONSE )
		{
        	cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
			if(st_Response.e_Response!= CMBS_RESPONSE_OK)
			{
				sprintf((char *)pTmpBufPtr, "DEV_INFO\r\n ERROR: INVALID_DEVICE_ID\r\n");
				p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
				
				break;
			}
        }

        app_IEToString(pv_IE, u16_IE);
        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);

    }

}

void p_HanServer_OnReadDeviceSubInfoPhase2Res(void *pv_EventData)
{

    void *pv_IE = NULL;
    u16 u16_IE;
    u8 *pTmpBufPtr;
	ST_IE_RESPONSE st_Response;

    ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 DeviceInfo;

    HAN_SERVER_INFO_PRINT("p_HanServer_OnReadDeviceSubInfoPhase2Res -->\n");

    memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
    pTmpBufPtr = AsciiMsgToClient;

    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);

    while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_PHASE_2_INFO )
        {
            cmbs_api_ie_HANDeviceSubInfoPhase2Get(pv_IE, &DeviceInfo);

            sprintf((char *)pTmpBufPtr, "DEV_INFO_PHASE_2\r\n");

            pTmpBufPtr += strlen((char *)pTmpBufPtr);

            p_HanServer_FillSingleDevicePhase2String(&DeviceInfo, pTmpBufPtr);
            pTmpBufPtr += strlen((char *)pTmpBufPtr);


            sprintf((char *)pTmpBufPtr, "\r\n");
			p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

            break;
        }
		if ( u16_IE == CMBS_IE_RESPONSE )
		{
        	cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
			if(st_Response.e_Response!= CMBS_RESPONSE_OK)
			{
				sprintf((char *)pTmpBufPtr, "DEV_INFO_PHASE_2\r\n ERROR: INVALID_DEVICE_ID\r\n");
				p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
				
				break;
			}
        }

        app_IEToString(pv_IE, u16_IE);
        cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);

    }

}

u32 p_HanServer_OnTableUpdate(void *pv_EventData)
{
    short u16_NumOfEntries = MAX_APPLICATIVE_DEV_ENTRIES;
    short u16_IndexOfFirstEntry = 0;
    UNUSED_PARAMETER(pv_EventData);
    app_HanMwDeviceReadTable(u16_NumOfEntries, u16_IndexOfFirstEntry, TRUE);

    return 0;
}


u32 p_HanServer_OnMsgRecv(void* pv_EventData,ST_IE_HAN_MSG * pstIe_Msg)
{
    unsigned short binlength;
    static u8 pu8_DataBuf[CMBS_HAN_MAX_MSG_DATA_LEN * 5];        // *4 for decimal, *3 for hex, * 5 for safe
    
    void *pv_IE = NULL;
	u16 	u16_IE;    
    pstIe_Msg->pu8_Data = u8_Buffer;
    cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
	while (pv_IE != NULL)
    {
        if ( u16_IE == CMBS_IE_HAN_MSG )
        {            
      		cmbs_api_ie_HANMsgGet(pv_IE, pstIe_Msg);
        }
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}
	
    HAN_SERVER_INFO_PRINT("hanServer: p_HanServer_OnMsgRecv\n");
    memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));     


#ifdef CONVERT_FUN
    // Special case: convert input FUN message in place (length may be increased)
    // check if If is filered
    if (!p_HanServer_ConvertFunData(pstIe_Msg))
        return 0;
#endif

    // Output Buf: Convert additional (hex) data if any, as a null terminated string
    pu8_DataBuf[0]=0;
    if ( pstIe_Msg->u16_DataLen && pstIe_Msg->pu8_Data )
    {
         binlength = pstIe_Msg->u16_DataLen;
         p_HanServer_PopulateHexValue(pstIe_Msg->pu8_Data, &binlength, pu8_DataBuf);
         HAN_SERVER_INFO_PRINT("binlength: %d bytes were filled\n", binlength);
         pu8_DataBuf[binlength] = 0;
    }


    sprintf((char *)AsciiMsgToClient, "FUN_MSG\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %s\r\n\r\n",

    g_fieldID[HAN_SERVER_SRC_DEV_ID],
    pstIe_Msg->u16_SrcDeviceId,

    g_fieldID[HAN_SERVER_SRC_UNIT_ID],
    pstIe_Msg->u8_SrcUnitId,

    g_fieldID[HAN_SERVER_DST_DEV_ID],
    pstIe_Msg->u16_DstDeviceId,

    g_fieldID[HAN_SERVER_DST_UNIT_ID],
    pstIe_Msg->u8_DstUnitId,

    g_fieldID[HAN_SERVER_DST_ADDRESS_TYPE],
    pstIe_Msg->u8_DstAddressType,

    g_fieldID[HAN_SERVER_MSG_TRANSPORT],
    pstIe_Msg->st_MsgTransport.u16_Reserved,

    g_fieldID[HAN_SERVER_MSG_SEQUENCE],
    pstIe_Msg->u8_MsgSequence,

    g_fieldID[HAN_SERVER_MSG_TYPE],
    pstIe_Msg->e_MsgType,

    g_fieldID[HAN_SERVER_INTRF_TYPE],
    pstIe_Msg->u8_InterfaceType,

    g_fieldID[HAN_SERVER_INTRF_ID],
    pstIe_Msg->u16_InterfaceId,

    // This Field was added, as remote have to get this field to understand the Thermometer and smoke events
    g_fieldID[HAN_SERVER_INTRF_MEMBER],
    pstIe_Msg->u8_InterfaceMember,

    g_fieldID[HAN_SERVER_DATALEN],
    pstIe_Msg->u16_DataLen,

    g_fieldID[HAN_SERVER_DATA],
    pu8_DataBuf                         // May be a null string
    );

	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

    return 0;
}


u32 p_HanServer_OnOpenRegistrationRes(int succeess)
{
    sprintf((char *)AsciiMsgToClient, "OPEN_RES\r\n %s\r\n\r\n", succeess ? "SUCCEED" : "FAIL");
	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

    return 0;
}

u32 p_HanServer_OnCloseRegistrationRes(int succeess)
{
    sprintf((char *)AsciiMsgToClient, "CLOSE_RES\r\n %s\r\n\r\n", succeess ? "SUCCEED" : "FAIL");
	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

    return 0;
}

// Send here REG_CLOSED with special reason
u32 p_HanServer_OnHsRegistered(u8 u8_Hs)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

	sprintf((char *)AsciiMsgToClient, "REG_CLOSED\r\n REASON: HS_REGISTERED\r\n HS_NUMBER: %d\r\n\r\n", u8_Hs);

	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

    return 0;
}

u32 p_HanServer_OnTargetAliveIndication()
{
    g_isTargetAlive = 1;
    return 0;
}

u32 p_HanServer_OnTargetUpIndication()
{
    g_TargetUpIndication = 1;
	g_en_TargetState = HOST_SERVER_TARGET_STATE_UP;

	p_HanServer_HandleInitializeMessagesDB();
	p_HanSever_HandleSendMessageToClient("TARGET_STATE\r\n STATE: UP\r\n\r\n");


	return 0;
}

u32 p_HanServer_OnEepParamGetRes(bool success, u8 *pu8_EepParaName, u16 u16_DataLen, u8 * pu8_Data)
{

  	static u8 pu8_DataBuf[150 * 5];        // *4 for decimal, *3 for hex, * 5 for safe

	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	p_HanServer_PopulateHexValueNoSpaces(pu8_Data, &u16_DataLen, pu8_DataBuf);
	 
	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_EEPROM_PARAM_RES\r\n STATUS: SUCCEED\r\n %s %s\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_NAME], pu8_EepParaName,
										g_fieldID[HAN_SERVER_DATA], pu8_DataBuf);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_EEPROM_PARAM_RES\r\n STATUS: FAIL\r\n %s %s\r\n\r\n",  g_fieldID[HAN_SERVER_NAME], pu8_EepParaName);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnEepParamSetRes(bool success, u8 *pu8_EepParaName)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nSET_EEPROM_PARAM_RES\r\n STATUS: SUCCEED\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_NAME], pu8_EepParaName);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nSET_EEPROM_PARAM_RES\r\n STATUS: FAIL\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_NAME], pu8_EepParaName);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnProdParamGetRes(bool success, u8 *pu8_ProdParaName, u16 u16_DataLen, u8 * pu8_Data)
{

  	static u8 pu8_DataBuf[150 * 5];        // *4 for decimal, *3 for hex, * 5 for safe

	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	p_HanServer_PopulateHexValueNoSpaces(pu8_Data, &u16_DataLen, pu8_DataBuf);
	 
	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_PRODUCTION_PARAM_RES\r\n STATUS: SUCCEED\r\n %s %s\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_NAME], pu8_ProdParaName,
										g_fieldID[HAN_SERVER_DATA], pu8_DataBuf);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_PRODUCTION_PARAM_RES\r\n STATUS: FAIL\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_NAME], pu8_ProdParaName);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnProdParamSetRes(bool success, u8 *pu8_ProdParaName)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nSET_PRODUCTION_PARAM_RES\r\n STATUS: SUCCEED\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_NAME], pu8_ProdParaName);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nSET_PRODUCTION_PARAM_RES\r\n STATUS: FAIL\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_NAME], pu8_ProdParaName);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnEepAreaGetRes(bool success, u32 u32_Offset,u16 u16_DataLen, u8 * pu8_Data)
{
	static u8 pu8_DataBuf[150 * 5];        // *4 for decimal, *3 for hex, * 5 for safe

	u16 u16_BufferLen = u16_DataLen;
	
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	p_HanServer_PopulateHexValueNoSpaces(pu8_Data, &u16_BufferLen, pu8_DataBuf);
	
	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_EEPROM_BYTES_RES\r\n STATUS: SUCCEED\r\n %s %d\r\n %s %d\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_OFFSET], u32_Offset,
										g_fieldID[HAN_SERVER_SIZE], u16_DataLen, g_fieldID[HAN_SERVER_DATA], pu8_DataBuf);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
	 	sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_EEPROM_BYTES_RES\r\n STATUS: FAIL\r\n %s %d\r\n %s %d\r\n\r\n", g_fieldID[HAN_SERVER_OFFSET], u32_Offset,
										g_fieldID[HAN_SERVER_SIZE], u16_DataLen);
		
		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnEepAreaSetRes(bool success)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

 	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nSET_EEPROM_BYTES_RES\r\n STATUS: SUCCEED\r\n\r\n");

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nSET_EEPROM_BYTES_RES\r\n STATUS: FAIL\r\n\r\n");

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnRamAreaGetRes(bool success, u32 u32_Offset,u16 u16_DataLen, u8 * pu8_Data)
{
	static u8 pu8_DataBuf[150 * 5];        // *4 for decimal, *3 for hex, * 5 for safe

	u16 u16_BufferLen = u16_DataLen;
	
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	p_HanServer_PopulateHexValueNoSpaces(pu8_Data, &u16_BufferLen, pu8_DataBuf);
	
	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[DBG]\r\nGET_RAM_BYTES_RES\r\n STATUS: SUCCEED\r\n %s %d\r\n %s %d\r\n %s %s\r\n\r\n", g_fieldID[HAN_SERVER_OFFSET], u32_Offset,
										g_fieldID[HAN_SERVER_SIZE], u16_DataLen, g_fieldID[HAN_SERVER_DATA], pu8_DataBuf);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
	 	sprintf((char *)AsciiMsgToClient, "[DBG]\r\nGET_RAM_BYTES_RES\r\n STATUS: FAIL\r\n %s %d\r\n %s %d\r\n\r\n", g_fieldID[HAN_SERVER_OFFSET], u32_Offset,
										g_fieldID[HAN_SERVER_SIZE], u16_DataLen);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnRamAreaSetRes(bool success)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

 	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[DBG]\r\nSET_RAM_BYTES_RES\r\n STATUS: SUCCEED\r\n\r\n");

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[DBG]\r\nSET_RAM_BYTES_RES\r\n FAIL: FAIL\r\n\r\n");

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}
	
u32 p_HanServer_OnEepromSizeGetRes(bool success, u32 u32_EepSize)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

 	if(success)
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_EEPROM_SIZE_RES\r\n STATUS: SUCCEED\r\n SIZE: %d\r\n\r\n", u32_EepSize);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_EEPROM_SIZE_RES\r\n STATUS: FAIL\r\n\r\n");

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}	

u32 p_HanServer_OnTargetHwVersionGetRes(bool success, u8 *pu8_HwChipName, u8 *pu8_HwChipVersion, u8 *pu8_HwBoard, u8 *pu8_HwComType)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	 
	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_TARGET_HW_VERSION_RES\r\n STATUS: SUCCEED\r\n %s %s\r\n %s %s\r\n %s %s\r\n %s %s\r\n\r\n", 
		 	g_fieldID[HAN_SERVER_HW_CHIP], pu8_HwChipName,
		 	g_fieldID[HAN_SERVER_HW_CHIP_VERSION], pu8_HwChipVersion,
			 g_fieldID[HAN_SERVER_HW_BOARD], pu8_HwBoard,
		 	g_fieldID[HAN_SERVER_HW_COM_TYPE], pu8_HwComType);


		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_TARGET_HW_VERSION_RES\r\n STATUS: FAIL\r\n\r\n");


		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}	

u32 p_HanServer_OnSwVersionGetRes(bool success, u16 u16_TargetVersion, u16 u16_TargetBuildNum, u16 u16_HostVersion, u16 u16_HostBuildNum)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_SW_VERSION_RES\r\n STATUS: SUCCEED\r\n %s %X\r\n %s %X\r\n %s %X\r\n %s %X\r\n\r\n", 
		 	g_fieldID[HAN_SERVER_TARGET_VERSION], u16_TargetVersion, g_fieldID[HAN_SERVER_TARGET_BUILD_NUMBER], u16_TargetBuildNum,
		 	g_fieldID[HAN_SERVER_HOST_VERSION], u16_HostVersion, g_fieldID[HAN_SERVER_HOST_BUILD_NUMBER], u16_HostBuildNum);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[SRV]\r\nGET_SW_VERSION_RES\r\n STATUS: FAIL\r\n\r\n");

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}	

			
u32 p_HanServer_OnDeviceRegistered(u16 u16_DeviceId)
{
    sprintf((char *)AsciiMsgToClient, "DEV_REGISTERED\r\n %s%d\r\n\r\n",
            g_fieldID[HAN_SERVER_DEV_ID],
            u16_DeviceId);

	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	p_HanSever_HandleSendMessageToClient("REG_CLOSED\r\n REASON: DEV_REGISTERED\r\n\r\n");

    return 0;
}

u32 p_HanServer_OnDeviceDeleted(u16 u16_DeviceId)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	
    if ( u16_DeviceId != ALL_ULE_DEVICES_HOST )
    {
        sprintf((char *)AsciiMsgToClient, "DEV_DELETED\r\n %s%d\r\n\r\n",
                g_fieldID[HAN_SERVER_DEV_ID],
                u16_DeviceId);
    }	
    else    // All devices were deleted
    {
        sprintf((char *)AsciiMsgToClient, "DEV_DELETED\r\n %s%s\r\n\r\n",
                g_fieldID[HAN_SERVER_DEV_ID],
                "ALL");
    }
	
	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

	return 0;
}
		
//Device deleted after being full registered (normal case)
u32 p_HanServer_OnDeviceFullDeleted(u16 u16_DeviceId, ST_IE_RESPONSE 	st_Resp, u16 u16_Reason)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	if(st_Resp.e_Response != CMBS_RESPONSE_OK)
	{
		switch(u16_Reason)
		{
			case DEVICE_DEREGISTRATION_FAILURE_INVALID_ID:
				sprintf((char *)AsciiMsgToClient, "DELETE_DEV_RES\r\n STATUS: FAIL\r\n DEV_ID: %d\r\n REASON: INVALID_ID\r\n\r\n", u16_DeviceId);
			break;
			case DEVICE_DEREGISTRATION_FAILURE_DEV_IN_REG:
				sprintf((char *)AsciiMsgToClient, "DELETE_DEV_RES\r\n STATUS: FAIL\r\n DEV_ID: %d\r\n REASON: DEV_IN_REG\r\n\r\n", u16_DeviceId);
			break;
			case DEVICE_DEREGISTRATION_FAILURE_BUSY:
				sprintf((char *)AsciiMsgToClient, "DELETE_DEV_RES\r\n STATUS: FAIL\r\n DEV_ID: %d\r\n REASON: BUSY\r\n\r\n", u16_DeviceId);
			break;
			default:
				sprintf((char *)AsciiMsgToClient, "DELETE_DEV_RES\r\n STATUS: FAIL\r\n DEV_ID: %d\r\n REASON: UNKOWN\r\n\r\n", u16_DeviceId);
			break;
		}

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}	
	else
	{
		sprintf((char *)AsciiMsgToClient, "DELETE_DEV_RES\r\n STATUS: SUCCESS\r\n DEV_ID: %d\r\n\r\n", u16_DeviceId);
		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);

	}

    return 0;
}

u32 p_HanServer_OnGetNumOfFunMsgInQRes(bool success, u16 u16_DeviceId, u8 u8_NumOfPendingMsg, u8 u8_NumOfMaxMsg)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	 
	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "[DBG]\r\nGET_NUM_OF_FUN_MSG_IN_Q_RES\r\n STATUS: SUCCEED\r\n %s %d\r\n %s %d\r\n %s %d\r\n\r\n", 
		 	g_fieldID[HAN_SERVER_DEV_ID], u16_DeviceId,
		 	g_fieldID[HAN_SERVER_NUM_OF_MSG], u8_NumOfPendingMsg,
			g_fieldID[HAN_SERVER_MAX_NUM_OF_MSG], u8_NumOfMaxMsg);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "[DBG]\r\nGET_NUM_OF_FUN_MSG_IN_Q_RES\r\n STATUS: FAIL\r\n %s %d\r\n\r\n",
			g_fieldID[HAN_SERVER_DEV_ID], u16_DeviceId);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnMessageSendRes(bool success, u16 u16_DeviceId)
{
	memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));
	 
	if(success)
	{
		 sprintf((char *)AsciiMsgToClient, "FUN_MSG_RES\r\n STATUS: SUCCEED\r\n %s %d\r\n\r\n", 
		 	g_fieldID[HAN_SERVER_DEV_ID], u16_DeviceId);

		 p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	else
	{
		sprintf((char *)AsciiMsgToClient, "FUN_MSG_RES\r\n STATUS: FAIL\r\n %s %d\r\n\r\n",
			g_fieldID[HAN_SERVER_DEV_ID], u16_DeviceId);

		p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
	}
	
    return 0;
}

u32 p_HanServer_OnDevicePartialDeleted(u16 u16_deviceId)
{
    HAN_SERVER_WARNING_PRINT("Partial reg failed of dev ID %d, do not notify remote \n", u16_deviceId);

    // But inform about Close Registration:
	p_HanSever_HandleSendMessageToClient("REG_CLOSED\r\n REASON: TIMEOUT\r\n\r\n");

    return 0;
}

#ifdef CONVERT_FUN

static u32 p_HanServer_GetTemperature(PST_IE_HAN_MSG pstIe_Msg, s16 *ps16_Val)
{
    int i, j;

    // command
    if ( pstIe_Msg->u8_InterfaceMember == 1 )
    {
        for ( i = 0; i < LENGTHOF(st_ThermoConvert); i++ ) 
        {
            if ( pstIe_Msg->u16_InterfaceId == st_ThermoConvert[i].u16_IID )
            {
                if ( pstIe_Msg->u16_DataLen < st_ThermoConvert[i].u8_Offset + st_ThermoConvert[i].u8_NumBytes )
                    // error
                    return -1;

                *ps16_Val = 0;

                for (j = st_ThermoConvert[i].u8_Offset; j < (st_ThermoConvert[i].u8_Offset + st_ThermoConvert[i].u8_NumBytes); j++)
                {
                    *ps16_Val = *ps16_Val * 256 + pstIe_Msg->pu8_Data[j];
                    printf("Temperature 0x%X = 0x%X\n", *ps16_Val, pstIe_Msg->pu8_Data[j]);
                }

                *ps16_Val = *ps16_Val * st_ThermoConvert[i].u8_Multy;
                return 1;
            }
        }
    }

    // not temperature
    return 0;
}

static u32 p_HanServer_CheckIfFilter(u16 u16_IID)
{
    int i;

    for (i = 0; i < LENGTHOF(u16_IfFilter); i++)
    {
        if ( u16_IfFilter[i] == u16_IID )
            return 1;
    }

    return 0; 
}

static u32 p_HanServer_CovertFunTemprature(PST_IE_HAN_MSG pstIe_Msg)
{
    short sTemp;

    if (p_HanServer_GetTemperature(pstIe_Msg, &sTemp) > 0)
    {
        // change some of the fields
        pstIe_Msg->pu8_Data[0]= pstIe_Msg->u8_SrcUnitId;
        pstIe_Msg->pu8_Data[1]= 0x83;
        pstIe_Msg->pu8_Data[2]= 0x01;
        pstIe_Msg->pu8_Data[3]= 0x01;
        pstIe_Msg->pu8_Data[4]= 0x04;
        pstIe_Msg->pu8_Data[5]= (sTemp >> 8 )& 0xFF;
        pstIe_Msg->pu8_Data[6]= sTemp & 0xFF;

        // change input message fields
        pstIe_Msg->u8_SrcUnitId = 0;                        // report is from Unit 0 always
        pstIe_Msg->u8_InterfaceType = 0;
        pstIe_Msg->u16_InterfaceId = REPORT_INTRF;          // report ID
        pstIe_Msg->u16_DataLen = 7 ;                        // data len is always 7
    }

    return 1;
}

//This function converts the INPUT message and data (in place)
// return 0 - the messahe shouldn't be sent - filtered out
static u32 p_HanServer_ConvertFunData(PST_IE_HAN_MSG pstIe_Msg)
{
    // check if the HAN message should be filtered out
    if ( p_HanServer_CheckIfFilter(pstIe_Msg->u16_InterfaceId) )
    {
        HAN_SERVER_INFO_PRINT("Interface ID 0x%X is filtered\n", pstIe_Msg->u16_InterfaceId);
        return 0;
    }

    return p_HanServer_CovertFunTemprature(pstIe_Msg);
 }

#endif

#ifdef CAMERA_CONVERSION

// Simulate FUN message of AC status report towards remote
void p_HanServer_SendCameraStatusToRemote(u16 u16_CameraDevice, u8 u8_status)
{

    unsigned short binlength;
    u8 pu8_Data[10] = { 128, 1, 130, 0, 1, 1, 0, 1, 0, 0 };
    static u8 pu8_DataBuf[CMBS_HAN_MAX_MSG_DATA_LEN * 5];        // *4 for decimal, *3 for hex, * 5 for safe

    pu8_Data[7] = u8_status;

    memset(AsciiMsgToClient, 0x0, sizeof(AsciiMsgToClient));

    // Output Buf: Convert additional (hex) data if any, as a null terminated string
    pu8_DataBuf[0] = 0;

    binlength = 8;
    p_HanServer_PopulateHexValue(pu8_Data, &binlength, pu8_DataBuf);
    HAN_SERVER_INFO_PRINT("binlength: %d bytes were filled\n", binlength);
    pu8_DataBuf[binlength] = 0;

    sprintf((char *)AsciiMsgToClient, "FUN_MSG\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %d\r\n %s %s\r\n\r\n",

            g_fieldID[HAN_SERVER_SRC_DEV_ID],
            u16_CameraDevice,

            g_fieldID[HAN_SERVER_SRC_UNIT_ID],
            0,

            g_fieldID[HAN_SERVER_DST_DEV_ID],
            0,

            g_fieldID[HAN_SERVER_DST_UNIT_ID],
            2,

            g_fieldID[HAN_SERVER_DST_ADDRESS_TYPE],
            0,

            g_fieldID[HAN_SERVER_MSG_TRANSPORT],
            0,

            g_fieldID[HAN_SERVER_MSG_SEQUENCE],
            50,

            g_fieldID[HAN_SERVER_MSG_TYPE],
            1,

            g_fieldID[HAN_SERVER_INTRF_TYPE],
            0,

            g_fieldID[HAN_SERVER_INTRF_ID],
            6,

            // This Field was added, as remote have to get this field to understand the Thermometer and smoke events
            g_fieldID[HAN_SERVER_INTRF_MEMBER],
            2,

            g_fieldID[HAN_SERVER_DATALEN],
            8,

            g_fieldID[HAN_SERVER_DATA],
            pu8_DataBuf                            // May be a null string
           );
	
	p_HanSever_HandleSendMessageToClient((char *)AsciiMsgToClient);
}

#endif // CAMERA_CONVERSION
u32 p_HanServer_HandleInitializeMessagesDB()
{
	t_st_paramList st_paramList;
	
	st_paramList.u16_CurSize = 0;
	st_paramList.u16_MaxSize = INTERNAL_BUFFER_SIZE;
	
	p_HanServer_serByte(&st_paramList,HAN_SERVER_INITILIZE_ARRAY);
	st_paramList.pu8_Buffer[st_paramList.u16_CurSize] = '\0';
	
	SocketSendTo(g_internalSocket,st_paramList.pu8_Buffer,sizeof(st_paramList.pu8_Buffer),handle);

	
	
	return 0;
}

void* p_HanServer_HandleDBStatus()
{
	t_st_paramList st_paramList;

	st_paramList.u16_CurSize = 0;
	st_paramList.u16_MaxSize = INTERNAL_BUFFER_SIZE;

	p_HanServer_serByte(&st_paramList, HAN_SERVER_DB_STATUS);
	st_paramList.pu8_Buffer[st_paramList.u16_CurSize] = '\0';

	SocketSendTo(g_internalSocket, st_paramList.pu8_Buffer, sizeof(st_paramList.pu8_Buffer), handle);

	return NULL;
}

void p_HanServer_HandleDeleteClient(struct sockaddr_in * cliaddres)
{
	t_st_paramList st_paramList;
	st_paramList.u16_CurSize = 0;
	st_paramList.u16_MaxSize = INTERNAL_BUFFER_SIZE;
	p_HanServer_serByte(&st_paramList, HAN_SERVER_DELETE_CLIENT);
	p_HanServer_serSockAddr(&st_paramList, cliaddres);
	st_paramList.pu8_Buffer[st_paramList.u16_CurSize] = '\0';
	SocketSendTo(g_internalSocket, st_paramList.pu8_Buffer, sizeof(st_paramList.pu8_Buffer), handle);
}

void p_HanServer_InternalHandler(unsigned char* internalMsgBuf)
{
	int index;
	u8 u8_Command;
	unsigned char message[CMBS_STRING_SIZE + 1]= { 0 };
	struct sockaddr_in cliaddres;
	index = p_HanServer_dserByte(internalMsgBuf,&u8_Command);

	switch(u8_Command)
	{								
		case HAN_SERVER_INITILIZE_ARRAY:
			p_HanServer_InitializeMessagesDB();
		break;
			
		case HAN_SERVER_SEND_MSG_TO_CLIENT:
			index += p_HanServer_dserString(message, &internalMsgBuf[index]);
			message[index] = '\0';
			p_HanSever_SendMessageToClient((char*)message);
		break;
		case HAN_SERVER_DELETE_CLIENT:
			p_HanServer_dserSockAddr(&cliaddres, &internalMsgBuf[index]);
			p_HanSever_DeleteClient(&cliaddres);
		break;
		
		case HAN_SERVER_DB_STATUS:
			p_HanServer_DBStat();
			break;
			
		default:
			break;
	}
}

void p_HanServer_ExternalHandler(char* externalMsgBuf, struct sockaddr_in* cliaddres)
{
    const t_CmdCb * CBHandler;
	
	CBHandler = p_HanServer_getHandler(externalMsgBuf);
	p_HanServer_MessageFromClient(cliaddres, CBHandler, externalMsgBuf); 

	
}

// Register callback for APP Service HAN messages
int p_HanServer_RegAppMessageCallbackTbl(const char *pServiceName, const t_CmdCb *tbl)
{
    int i;

    for (i = 0; i < LENGTHOF(g_AppServiceTblCB); i++)
    {
        if (g_AppServiceTblCB[i].pSvcName == NULL)
        {
            g_AppServiceTblCB[i].pSvcName = strdup(pServiceName);
            g_AppServiceTblCB[i].pTbl = tbl;
            break;
        }
    }

    if (i >= LENGTHOF(g_AppServiceTblCB))
    {
        HAN_SERVER_ERROR_PRINT("APP service HAN table is full\n");
        return -1;
    }

    return 0;
}

static const t_CmdCb * p_HanServer_FindAppMessageCallbackTbl(const char *pHANmsg)
{
    int i;

    for (i = 0; i < LENGTHOF(g_AppServiceTblCB) && g_AppServiceTblCB[i].pSvcName != NULL; i++)
        if (!strncmp(g_AppServiceTblCB[i].pSvcName, pHANmsg, strlen(g_AppServiceTblCB[i].pSvcName)))
            return g_AppServiceTblCB[i].pTbl;

    return NULL;
}




