/*!
*   \file       appfun.c
*   \brief      HAN FUN API
*   \author     ULE BS Team
*
*
*******************************************************************************/

#include "cfr_ie.h"
#include "cmbs_han.h"
#include "cmbs_fun.h"
#include "appfun.h"
#include "appstreambuffer.h"
#include "tcx_hostlog.h"

#define HAN_FUN_APP_INFO_PRINT(format, ...) 						tcx_WriteLog(FUN_APP, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define HAN_FUN_APP_TRACE_PRINT(format, ...) 					tcx_WriteLog(FUN_APP, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define HAN_FUN_APP_WARNING_PRINT(format, ...) 				tcx_WriteLog(FUN_APP, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define HAN_FUN_APP_ERROR_PRINT(format, ...) 					tcx_WriteLog(FUN_APP, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define HAN_FUN_APP_RT_PRINT(format, ...) 						tcx_WriteLog(FUN_APP, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define HAN_FUN_APP_PRINT_DATA(pu8_Buffer, u16_Length) 		tcx_WriteLogData(FUN_APP, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// TODO: use serialize and deserialize CMBS methods and remove these hardcoded methods
///////////////////////////////////////////////////////////////////////////////////
u16 pr_hanEndian_SwapBytesU16( u16 u16_Value )
{
   return (	((u16_Value & 0x00FF) << 8) |
			((u16_Value & 0xFF00) >> 8) );
}

u32 pr_hanEndian_SwapBytesU32( u32 u32_Value )
{
   return (	( (u32_Value & 0x000000FF) << 24	) |
			( (u32_Value & 0x0000FF00) <<  8	) |
			( (u32_Value & 0x00FF0000) >>  8	) |
			( (u32_Value & 0xFF000000) >> 24	) );
}
u32 pr_hanEndian_HtoN_u32( u32 u32_Value )
{
	return pr_hanEndian_SwapBytesU32( u32_Value );
}

u32 pr_hanEndian_NtoH_u32( u32 u32_Value )
{
	return pr_hanEndian_SwapBytesU32( u32_Value );
}

u16 pr_hanEndian_HtoN_u16( u16 u16_Value )
{
	return pr_hanEndian_SwapBytesU16( u16_Value );
}

u16 pr_hanEndian_NtoH_u16( u16 u16_Value )
{
	return pr_hanEndian_SwapBytesU16( u16_Value );
}
///////////////////////////////////////////////////////////////////////////////////

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////		  app_fun_ParseFUNRegistrationMessage		/////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void app_fun_ParseFUNRegistrationMessage(IN u8 *p_Msg, OUT PST_FUN_DEVICE_INFO pst_FunInfoEepromStructure)
{

	u8 							u, i, length;
	t_st_streamBuffer			st_OtaStream;
	u8							u8_Field;
	u8							DiscriminatorType;
	u8							DeviceUIDSize;
	PST_IE_HAN_MSG  pstIe_Msg = (PST_IE_HAN_MSG) p_Msg;
	
	app_streamBuffer_CreateWithPayload(&st_OtaStream, pstIe_Msg->pu8_Data, pstIe_Msg->u16_DataLen, pstIe_Msg->u16_DataLen);
	
	// Discriminator Type and Dev UID Type
	u8_Field = app_streamBuffer_GetData8(&st_OtaStream);

	DiscriminatorType = GET_BITFIELD_VAL( u8_Field, FUN_IF_DEV_MNGT_REG_DISCRIMINATOR_TYPE );
	DeviceUIDSize = app_streamBuffer_GetData8(&st_OtaStream);

	// Device UID ( variable length ) - we are going to just skip it for now
	app_streamBuffer_SkipData8Array(&st_OtaStream, DeviceUIDSize);

	// Discriminator Value Optional
	if ( DiscriminatorType == FUN_IF_DEV_MNGT_REG_RESP_DISCR_TYPE_PROPRIETARY )
	{
		// Need to read and store the Descriminator Value ( EMC )
		pst_FunInfoEepromStructure->DeviceEMC = pr_hanEndian_NtoH_u16(app_streamBuffer_GetData16(&st_OtaStream));

	}
	else
	{
		pst_FunInfoEepromStructure->DeviceEMC = 0;
	}
	
	pst_FunInfoEepromStructure->NumOfUnits = app_streamBuffer_GetData8(&st_OtaStream);
	
	if(pst_FunInfoEepromStructure->NumOfUnits > MAX_NUM_OF_UNITS_IN_DEVICE)
	{
		pst_FunInfoEepromStructure->NumOfUnits = MAX_NUM_OF_UNITS_IN_DEVICE;
	}
	
	for (u=0; u < pst_FunInfoEepromStructure->NumOfUnits; u++)
	{
		//read Unit #u
		//if Unit is not supporting optional interfaces, we know it only according to the length=3 (there will be no NumberOfOptionalInterfaces field at all) 
		length = app_streamBuffer_GetData8(&st_OtaStream);
		
		//read Unit Id
		pst_FunInfoEepromStructure->Units[u].UnitId = app_streamBuffer_GetData8(&st_OtaStream);

		//read Unit type
		pst_FunInfoEepromStructure->Units[u].UnitType = pr_hanEndian_NtoH_u16(app_streamBuffer_GetData16(&st_OtaStream));

		if(length > 3 )	//sizeof unitId + sizeof UnitType
		{
			//read number of optional interfaces
			pst_FunInfoEepromStructure->Units[u].NumberOfOptionalInterfaces = app_streamBuffer_GetData8(&st_OtaStream);

			if(pst_FunInfoEepromStructure->Units[u].NumberOfOptionalInterfaces > MAX_NUM_OF_OPTIONAL_INTERFACES_IN_UNIT)
			{
				pst_FunInfoEepromStructure->Units[u].NumberOfOptionalInterfaces = MAX_NUM_OF_OPTIONAL_INTERFACES_IN_UNIT;
			}
			
			//read all optional interface
			for(i=0; i < pst_FunInfoEepromStructure->Units[u].NumberOfOptionalInterfaces; i++)
			{
				pst_FunInfoEepromStructure->Units[u].OptionalInterfaces[i] = pr_hanEndian_NtoH_u16(app_streamBuffer_GetData16(&st_OtaStream));				
			}
		}
		else
		{
			pst_FunInfoEepromStructure->Units[u].NumberOfOptionalInterfaces = 0;
		}	
	}

	pst_FunInfoEepromStructure->DeviceId  = pstIe_Msg->u16_SrcDeviceId;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////		app_fun_BuildFUNRegistrationResponse		/////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void app_fun_BuildFUNRegistrationResponse(IN u8 *p_Msg,  OUT u8 *p_ResMsg, OUT u8 *p_ResMsgCtl)

{
	
	t_st_streamBuffer	st_OtaStream;
	u8					RegResponse = 0;
	PST_IE_HAN_MSG  pstIe_RegMsg = (PST_IE_HAN_MSG) p_Msg;
	PST_IE_HAN_MSG pstIe_ResMsg = (PST_IE_HAN_MSG) p_ResMsg;
	
	PST_IE_HAN_MSG_CTL pst_HANResMsgCtl = (PST_IE_HAN_MSG_CTL) p_ResMsgCtl;

	// initialize pst_HANResMsgCtl
	pst_HANResMsgCtl->ImmediateSend = 0;
	pst_HANResMsgCtl->IsLast = 0;
	pst_HANResMsgCtl->Reserved = 0;

	app_streamBuffer_CreateEmpty(&st_OtaStream, pstIe_ResMsg->pu8_Data, CMBS_HAN_MAX_MSG_LEN);
	
	// fill the network part
	pstIe_ResMsg->u16_SrcDeviceId = OTA_BASE_DEVICE_ID;
	pstIe_ResMsg->u8_SrcUnitId = OTA_BASE_UNIT_MANAGMENT_UNIT_ID;

	pstIe_ResMsg->u16_DstDeviceId = pstIe_RegMsg->u16_SrcDeviceId;
	pstIe_ResMsg->u8_DstUnitId = OTA_DST_UNIT_MANAGMENT_UNIT_ID;
	pstIe_ResMsg->u8_DstAddressType = OTA_ADDR_TYPE_INDIVIDUAL;

	// fill the application part
	pstIe_ResMsg->u8_MsgSequence = pstIe_RegMsg->u8_MsgSequence;
	pstIe_ResMsg->e_MsgType = OTA_MSG_TYPE_COMMAND_RESPONSE;
	pstIe_ResMsg->u8_InterfaceType = OTA_INTERFACE_TYPE_SERVER;
	pstIe_ResMsg->u16_InterfaceId = FUN_INTERFACE_DEVICE_MGNT;
	pstIe_ResMsg->u8_InterfaceMember = FUN_IF_DEV_MNGT_CMD_REGISTER_DEVICE;

	app_streamBuffer_AddData8(&st_OtaStream, RegResponse);
	app_streamBuffer_AddData16(&st_OtaStream, pr_hanEndian_HtoN_u16(pstIe_ResMsg->u16_DstDeviceId | (FUN_IF_DEV_MNGT_REG_RESP_DISCR_TYPE_NON_PROPRIETARY << (sizeof(pstIe_ResMsg->u16_DstDeviceId)-1))));

	// update length of data
	pstIe_ResMsg->u16_DataLen = app_streamBuffer_GetDataSize(&st_OtaStream);

	pstIe_ResMsg->st_MsgTransport.u16_Reserved	= 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*---------[End Of File]---------------------------------------------------------------------------------------------------------------------------*/