/*!
*  \file       cmbs_han_ie.c
*  \brief      Information Elements List functions for HAN
*  \author     andriig
*
*  @(#)  %filespec: cmbs_han_ie.c~BLRD53#9 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
*	27-05-14	ronenw		GIT		moved cmbs_han_ie_params_Union members into common union U_Buffer @ cfr_ie.h
*
*******************************************************************************/

#include "cfr_ie.h"
#include "cfr_debug.h"
#include "cmbs_han_ie.h"
#include "cmbs_han.h"
#include "cmbs_int.h"

#if defined(__arm)
	#include "tclib.h"
	#include "embedded.h"
#else
	#include <string.h>
#endif

#if defined ( CMBS_API_TARGET )
extern U_Buffer s_Buffer;
#else
extern U_Buffer s_Buffer[CMBS_NUM_OF_HOST_THREADS];
#endif

#if defined ( CMBS_API_TARGET )
#define CMBS_IE_GET_BUFFER(NAME) 							    \
	u8 *pu8_Buffer = (u8 *)&s_Buffer.NAME;					    \
	memset(pu8_Buffer,0,sizeof(s_Buffer.NAME));                       
#else                                                           
#define CMBS_IE_GET_BUFFER(NAME) 							    \
	u8 * pu8_Buffer; 										    \
	u8 u8_ThreadIdx = cfr_ie_getThreadIdx(); 				    \
	if ( u8_ThreadIdx == CMBS_UNKNOWN_THREAD ) 				    \
	{														    \
	   CFR_DBG_ERROR("Unknown Thread Id!!");				    \
	   return CMBS_RC_ERROR_GENERAL;						    \
	}														    \
	else													    \
	{														    \
		pu8_Buffer = (u8 *)&(s_Buffer[u8_ThreadIdx].NAME);       \
	}                                                           \
    memset(pu8_Buffer,0,sizeof(s_Buffer[u8_ThreadIdx].NAME));
#endif



//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HanCfgAdd( void * pv_RefIEList, ST_IE_HAN_CONFIG* pst_Cfg)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIEList, pst_Cfg->st_HanCfg.u8_HANServiceConfig, CMBS_IE_HAN_CFG);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HanCfgGet( void * pv_RefIEList, ST_IE_HAN_CONFIG* pst_Cfg)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIEList, &pst_Cfg->st_HanCfg.u8_HANServiceConfig, CMBS_IE_HAN_CFG);
}

E_CMBS_RC		  cmbs_api_ie_HanUleBaseInfoAdd( void * pv_RefIEList, ST_IE_HAN_BASE_INFO* pst_HanBaseInfo)
{	
	u16 u16_Size = 0;

	CMBS_IE_GET_BUFFER(HanBaseInfoAdd);

	// put data
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HanBaseInfo->u8_UleAppProtocolId);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HanBaseInfo->u8_UleAppProtocolVersion);

	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_BASE_INFO);
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;	
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HanUleBaseInfoGet( void * pv_RefIEList, ST_IE_HAN_BASE_INFO* pst_HanBaseInfo)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;

	memset(pst_HanBaseInfo, 0, sizeof(ST_IE_HAN_BASE_INFO));
	
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_BASE_INFO);	

	u16_Size += cfr_ie_dser_u8 ( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HanBaseInfo->u8_UleAppProtocolId);
	u16_Size += cfr_ie_dser_u8 ( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HanBaseInfo->u8_UleAppProtocolVersion);

	return CMBS_RC_OK;
}

E_CMBS_RC  cmbs_ie_HANGroupTableAdd( void * pv_RefIEList, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8* pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u16 u16_Size = 0;
	int Devices;
	ST_HAN_GROUP_TABLE_ENTRY * GroupEntries = (ST_HAN_GROUP_TABLE_ENTRY *) pArrayPtr;
	CMBS_IE_GET_BUFFER(HanTableAdd);

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_NumOfEntries );
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_IndexOfFirstEntry );

	UNUSED_PARAMETER(u8_entrySize);
	for ( Devices = 0; Devices < u16_NumOfEntries; Devices++ )
	{
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, GroupEntries[Devices].u16_GroupId);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, GroupEntries[Devices].u16_DeviceId);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, GroupEntries[Devices].u8_UnitId);
	}
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_ie_HANGroupTableGet( void * pv_RefIEList,u16* pu16_NumOfEntries, u16* pu16_IndexOfFirstEntry, u8 * pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;
	u16		u16_Length = 0;
	int 	Devices;
	ST_HAN_GROUP_TABLE_ENTRY * GroupEntries = (ST_HAN_GROUP_TABLE_ENTRY *)pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_NumOfEntries );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_IndexOfFirstEntry );
	UNUSED_PARAMETER(u8_entrySize);

	for ( Devices = 0; Devices < *pu16_NumOfEntries; Devices++ )
	{
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &GroupEntries[Devices].u16_GroupId);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &GroupEntries[Devices].u16_DeviceId);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &GroupEntries[Devices].u8_UnitId);
	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC  cmbs_ie_HANBindTableAdd( void * pv_RefIEList, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8* pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u16 u16_Size = 0;
	int Devices;
	ST_HAN_BIND_ENTRY * BindEntries = (ST_HAN_BIND_ENTRY *) pArrayPtr;
	CMBS_IE_GET_BUFFER(HanTableAdd);

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_NumOfEntries );
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_IndexOfFirstEntry );

	UNUSED_PARAMETER(u8_entrySize);
	for ( Devices = 0; Devices < u16_NumOfEntries; Devices++ )
	{

		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BindEntries[Devices].u16_SrcDeviceID);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BindEntries[Devices].u8_SrcUnitID);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BindEntries[Devices].u16_DstInterfaceID);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BindEntries[Devices].u8_DstInterfaceType); 	
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BindEntries[Devices].u16_DstDeviceID);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BindEntries[Devices].u8_DstUnitID);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BindEntries[Devices].u8_DstAddressType);

	}
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static E_CMBS_RC cmbs_ie_HANBindTableGet( void * pv_RefIEList,u16* pu16_NumOfEntries, u16* pu16_IndexOfFirstEntry, u8 * pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;
	u16		u16_Length = 0;
	int 	Devices;
	ST_HAN_BIND_ENTRY * BindEntries = (ST_HAN_BIND_ENTRY *) pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_NumOfEntries );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_IndexOfFirstEntry );
	UNUSED_PARAMETER(u8_entrySize);

	for ( Devices = 0; Devices < *pu16_NumOfEntries; Devices++ )
	{

		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BindEntries[Devices].u16_SrcDeviceID);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BindEntries[Devices].u8_SrcUnitID);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BindEntries[Devices].u16_DstInterfaceID);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BindEntries[Devices].u8_DstInterfaceType); 	
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BindEntries[Devices].u16_DstDeviceID);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BindEntries[Devices].u8_DstUnitID);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BindEntries[Devices].u8_DstAddressType);

	
	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC  cmbs_ie_HanDeviceTableBriefAdd( void * pv_RefIEList, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8* pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u16 u16_Size = 0;
	int Devices;
	int Units;
	ST_HAN_BRIEF_DEVICE_INFO* BriefEntries = (ST_HAN_BRIEF_DEVICE_INFO*)pArrayPtr;
	CMBS_IE_GET_BUFFER(HanTableAdd);

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_NumOfEntries );
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_IndexOfFirstEntry );

	UNUSED_PARAMETER(u8_entrySize);
	for ( Devices = 0; Devices < u16_NumOfEntries; Devices++ )
	{

		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BriefEntries[Devices].u16_DeviceId);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BriefEntries[Devices].u8_RegistrationStatus);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BriefEntries[Devices].u16_RequestedPageTime);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BriefEntries[Devices].u16_PageTime); 	//final page time, after negotiation
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BriefEntries[Devices].u8_NumberOfUnits);

		for ( Units = 0; Units < BriefEntries[Devices].u8_NumberOfUnits; Units++ )
		{
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BriefEntries[Devices].st_UnitsInfo[Units].u8_UnitId);
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, BriefEntries[Devices].st_UnitsInfo[Units].u16_UnitType);
		}

	}
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General implementation 
static E_CMBS_RC  cmbs_ie_HanTableAdd( void * pv_RefIEList, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8* pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u8  u8_Buffer[CFR_IE_HEADER_SIZE + CMBS_HAN_IE_TABLE_SIZE + 2 * sizeof(u16)] = { 0 };
	u16 u16_Size = 0;

	// put data
	u16_Size += cfr_ie_ser_u16( u8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_NumOfEntries );
	u16_Size += cfr_ie_ser_u16( u8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_IndexOfFirstEntry );
	u16_Size +=cfr_ie_ser_pu8( u8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pArrayPtr, u16_NumOfEntries * u8_entrySize);

	// put header
	cfr_ie_ser_u16( u8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( u8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, u8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static E_CMBS_RC cmbs_ie_HanTableGet( void * pv_RefIEList,u16* pu16_NumOfEntries, u16* pu16_IndexOfFirstEntry, u8 * pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)

{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;
	u16		u16_Length = 0;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_NumOfEntries );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_IndexOfFirstEntry );
	
	if (u16_Length != ( u16_Size + u8_entrySize * (*pu16_NumOfEntries)))
		return CMBS_RC_ERROR_PARAMETER;

	u16_Size += cfr_ie_dser_pu8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pArrayPtr, (*pu16_NumOfEntries) * u8_entrySize );

	return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC  cmbs_api_ie_HanFUNDeviceInfoGet( void * pv_RefIEList, ST_FUN_DEVICE_INFO* FunDevInfo)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;
	u16		u16_Length = 0;
	int  Units, Interfaces;

	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_FUN_DEVICE_INFO);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );
	
	// get data
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &FunDevInfo->DeviceId);
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &FunDevInfo->DeviceEMC);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &FunDevInfo->NumOfUnits);

	for ( Units = 0; Units < FunDevInfo->NumOfUnits; Units++ )
	{

		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &FunDevInfo->Units[Units].UnitId);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &FunDevInfo->Units[Units].UnitType);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &FunDevInfo->Units[Units].NumberOfOptionalInterfaces);

		
		for ( Interfaces = 0; Interfaces < FunDevInfo->Units[Units].NumberOfOptionalInterfaces; Interfaces++ )
		{
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &FunDevInfo->Units[Units].OptionalInterfaces[Interfaces]);	
		}
	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC  cmbs_api_ie_HanFUNDeviceInfoAdd( void * pv_RefIEList, ST_FUN_DEVICE_INFO* FunDevInfo)
{
//	CMBS_IE_HAN_FUN_DEVICE_INFO
	u16 u16_Size = 0;
	int  Units, Interfaces;

	CMBS_IE_GET_BUFFER(FunDeviceInfo);

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, FunDevInfo->DeviceId);
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, FunDevInfo->DeviceEMC);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, FunDevInfo->NumOfUnits);

	for ( Units = 0; Units < FunDevInfo->NumOfUnits; Units++ )
	{

		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, FunDevInfo->Units[Units].UnitId);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, FunDevInfo->Units[Units].UnitType);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, FunDevInfo->Units[Units].NumberOfOptionalInterfaces);

		
		for ( Interfaces = 0; Interfaces < FunDevInfo->Units[Units].NumberOfOptionalInterfaces; Interfaces++ )
		{
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, FunDevInfo->Units[Units].OptionalInterfaces[Interfaces]);	
		}

	}
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_FUN_DEVICE_INFO );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

E_CMBS_RC  cmbs_ie_HanDeviceTableExtendedAdd( void * pv_RefIEList, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8* pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
//	u8  u8_Buffer[ CFR_IE_HEADER_SIZE + CMBS_HAN_IE_TABLE_SIZE] = {0};
	u16 u16_Size = 0;
	int Devices, Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO* ExtendedEntries = (ST_HAN_EXTENDED_DEVICE_INFO*)pArrayPtr;
	CMBS_IE_GET_BUFFER(HanTableAdd);

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_NumOfEntries );
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_IndexOfFirstEntry );
	UNUSED_PARAMETER(u8_entrySize);

	for ( Devices = 0; Devices < u16_NumOfEntries; Devices++ )
	{

		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_DeviceId);
		
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[0]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[1]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[2]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[3]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[4]);
		
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_RegistrationStatus);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_RequestedPageTime);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_PageTime); 	//final page time, after negotiation
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_DeviceEMC);

		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_NumberOfUnits);
				
		for ( Units = 0; Units < ExtendedEntries[Devices].u8_NumberOfUnits; Units++ )
		{
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u8_UnitId);
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u16_UnitType);
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);
			for(Interfaces=0; Interfaces<ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
			{
				u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
			}
		}

	}
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}


E_CMBS_RC  cmbs_ie_HanDeviceTableExtendedPhase2Add( void * pv_RefIEList, u16 u16_NumOfEntries, u16 u16_IndexOfFirstEntry, u8* pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
//	u8  u8_Buffer[ CFR_IE_HEADER_SIZE + CMBS_HAN_IE_TABLE_SIZE] = {0};
	u16 u16_Size = 0;
	int Devices, Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2* ExtendedEntries = (ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2*)pArrayPtr;
	CMBS_IE_GET_BUFFER(HanTableAdd);

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_NumOfEntries );
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u16_IndexOfFirstEntry );
	UNUSED_PARAMETER(u8_entrySize);

	for ( Devices = 0; Devices < u16_NumOfEntries; Devices++ )
	{

		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_DeviceId);
		
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[0]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[1]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[2]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[3]);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_IPUI[4]);
		
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_RegistrationStatus);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_RequestedPageTime);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_PageTime); 	//final page time, after negotiation
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u16_DeviceEMC);

		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_ULECapabilities);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_UleAppProtocolId);
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_UleAppProtocolVersion);

		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].u8_NumberOfUnits);
				
		for ( Units = 0; Units < ExtendedEntries[Devices].u8_NumberOfUnits; Units++ )
		{
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u8_UnitId);
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u16_UnitType);
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);
			for(Interfaces=0; Interfaces<ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
			{
				u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, ExtendedEntries[Devices].st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
			}
		}

	}
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}


E_CMBS_RC  cmbs_ie_HanDeviceSubInfoAdd( void * pv_RefIEList,  u8* pArrayPtr, E_CMBS_IE_HAN_TYPE e_IE)
{
	u16 u16_Size = 0;
	int Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO* SingleExtendedEntry = (ST_HAN_EXTENDED_DEVICE_INFO*)pArrayPtr;

	CMBS_IE_GET_BUFFER(HanTableAdd);	

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_DeviceId);
	
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[0]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[1]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[2]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[3]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[4]);
	
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_RegistrationStatus);
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_RequestedPageTime);
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_PageTime); 	//final page time, after negotiation
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_DeviceEMC);

	
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_NumberOfUnits);

	for ( Units = 0; Units < SingleExtendedEntry->u8_NumberOfUnits; Units++ )
	{
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u8_UnitId);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u16_UnitType);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);
		for(Interfaces=0; Interfaces<SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
		{
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
		}
	}
	
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

E_CMBS_RC  cmbs_ie_HanDeviceSubInfoPhase2Add( void * pv_RefIEList,  u8* pArrayPtr, E_CMBS_IE_HAN_TYPE e_IE)
{
	u16 u16_Size = 0;
	int Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2* SingleExtendedEntry = (ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2*)pArrayPtr;

	CMBS_IE_GET_BUFFER(HanTableAdd);	

	// put data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_DeviceId);
	
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[0]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[1]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[2]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[3]);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_IPUI[4]);
	
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_RegistrationStatus);
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_RequestedPageTime);
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_PageTime); 	//final page time, after negotiation
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u16_DeviceEMC);

	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_ULECapabilities);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_UleAppProtocolId);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_UleAppProtocolVersion);
		
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->u8_NumberOfUnits);

	for ( Units = 0; Units < SingleExtendedEntry->u8_NumberOfUnits; Units++ )
	{
		u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u8_UnitId);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u16_UnitType);
		u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);
		for(Interfaces=0; Interfaces<SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
		{
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, SingleExtendedEntry->st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
		}
	}
	
	
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

E_CMBS_RC  cmbs_ie_HanDeviceFullRegistrationInfoAdd( void * pv_RefIEList,  u8* pArrayPtr, E_CMBS_IE_HAN_TYPE e_IE)
{
	u16 u16_Size=0, i = 0, j=0;
	ST_HAN_FULL_DEVICE_REGISTRATION_INFO* pst_HANCompleteRegistrationDeviceEntry = (ST_HAN_FULL_DEVICE_REGISTRATION_INFO*)pArrayPtr;

	CMBS_IE_GET_BUFFER(HanFullRegistrationInfo);	

	// DECT data
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Ipui[0], CMBS_HAN_DECT_SUB_IPUI_SIZE);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Tpui[0], CMBS_HAN_DECT_SUB_TPUI_SIZE);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Dck[0], CMBS_HAN_DECT_SUB_DCK_SIZE);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Uak[0], CMBS_HAN_DECT_SUB_UAK_SIZE);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_AuthenticationCode[0], CMBS_HAN_DECT_SUB_AC_SIZE);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_UakAuth);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_Status);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_HsNumber);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_DckAssigned);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_CkLen);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_Features);	
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Defck[0], CMBS_HAN_DECT_SUB_DEFCK_SIZE);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_DefckAssigned);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_CcmCK[0], CMBS_HAN_DECT_SUB_CCM_CK_SIZE);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_DlcOperatingMode);
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_HlFeatures);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Reserved[0], CMBS_HAN_DECT_SUB_RESERVED_SIZE);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[0], CMBS_HAN_DECT_SUB_NEXT_RX_SEQUENCE_NUMBER_SIZE);
	u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[0], CMBS_HAN_DECT_SUB_NEXT_TX_SEQUENCE_NUMBER_SIZE);

	// ULE data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.DeviceId); 
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.RegistrationStatus); 
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.RequestedPageTime); 
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.NegotiatedPageTime); 
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.u16_MaxSduSize); 
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.u8_NumAppProtocols); 
		
	//App protocols
	for (i=0; i<CMBS_HAN_ULE_APP_PROTOCOLS; i++)
		{
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_UleAppProtocolId);
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_UleCapabilities);
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_IsProprietaryProtocolId);
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_Reserved);
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_UleAppProtocolVersion); 
			u16_Size += cfr_ie_ser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].pu8_EmcOrExtendedData[0], CMBS_HAN_EMC_EXTERNED_APP_SIZE_IN_BYTES);
		}


	// FUN data
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.DeviceId); 
	u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.DeviceEMC); 
	u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.NumOfUnits); 
	
	for (i=0; i<CMBS_HAN_MAX_UNITS_IN_DEVICE; i++)
		{
			u16_Size += cfr_ie_ser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u8_UnitId); 
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u16_UnitType); 
			u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u16_NumberOfOptionalInterfaces); 

			for (j=0; j<CMBS_HAN_MAX_OPTIONAL_INTERFACES_IN_UNIT; j++)
				{
					u16_Size += cfr_ie_ser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u16_OptionalInterfaces[j]); 
				}
		}

	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, e_IE );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static E_CMBS_RC cmbs_ie_HanDeviceTableBriefGet( void * pv_RefIEList,u16* pu16_NumOfEntries, u16* pu16_IndexOfFirstEntry, u8 * pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;
	u16		u16_Length = 0;
	int 	Devices;
	int 	Units;
	ST_HAN_BRIEF_DEVICE_INFO* BriefEntries = (ST_HAN_BRIEF_DEVICE_INFO*)pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_NumOfEntries );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_IndexOfFirstEntry );
	UNUSED_PARAMETER(u8_entrySize);

	for ( Devices = 0; Devices < *pu16_NumOfEntries; Devices++ )
	{
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BriefEntries[Devices].u16_DeviceId);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BriefEntries[Devices].u8_RegistrationStatus);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BriefEntries[Devices].u16_RequestedPageTime);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BriefEntries[Devices].u16_PageTime); 	//final page time, after negotiation
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BriefEntries[Devices].u8_NumberOfUnits);

		for ( Units = 0; Units < BriefEntries[Devices].u8_NumberOfUnits; Units++ )
		{
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BriefEntries[Devices].st_UnitsInfo[Units].u8_UnitId);
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &BriefEntries[Devices].st_UnitsInfo[Units].u16_UnitType);
		}

	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static E_CMBS_RC cmbs_ie_HanDeviceTableExtendedGet( void * pv_RefIEList,u16* pu16_NumOfEntries, u16* pu16_IndexOfFirstEntry, u8 * pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;
	u16		u16_Length = 0;
	int 	Devices, Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO* ExtendedEntries = (ST_HAN_EXTENDED_DEVICE_INFO*)pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_NumOfEntries );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_IndexOfFirstEntry );
	UNUSED_PARAMETER(u8_entrySize);

	for ( Devices = 0; Devices < *pu16_NumOfEntries; Devices++ )
	{
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_DeviceId);

		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[0]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[1]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[2]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[3]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[4]);
		
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_RegistrationStatus);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_RequestedPageTime);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_PageTime); 	//final page time, after negotiation
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_DeviceEMC);
		
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_NumberOfUnits);

		for ( Units = 0; Units < ExtendedEntries[Devices].u8_NumberOfUnits; Units++ )
		{
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u8_UnitId);
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u16_UnitType);
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);

			for(Interfaces=0; Interfaces<ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
			{
				u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
			}

		}

	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static E_CMBS_RC cmbs_ie_HanDeviceTableExtendedPhase2Get( void * pv_RefIEList,u16* pu16_NumOfEntries, u16* pu16_IndexOfFirstEntry, u8 * pArrayPtr, u8 u8_entrySize, E_CMBS_IE_HAN_TYPE e_IE)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;
	u16		u16_Length = 0;
	int 	Devices, Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2* ExtendedEntries = (ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2*)pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_NumOfEntries );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pu16_IndexOfFirstEntry );
	UNUSED_PARAMETER(u8_entrySize);

	for ( Devices = 0; Devices < *pu16_NumOfEntries; Devices++ )
	{
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_DeviceId);

		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[0]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[1]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[2]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[3]);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_IPUI[4]);
		
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_RegistrationStatus);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_RequestedPageTime);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_PageTime); 	//final page time, after negotiation
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u16_DeviceEMC);

		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_ULECapabilities);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_UleAppProtocolId);
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_UleAppProtocolVersion);
		
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].u8_NumberOfUnits);

		for ( Units = 0; Units < ExtendedEntries[Devices].u8_NumberOfUnits; Units++ )
		{
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u8_UnitId);
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u16_UnitType);
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);

			for(Interfaces=0; Interfaces<ExtendedEntries[Devices].st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
			{
				u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &ExtendedEntries[Devices].st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
			}

		}

	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static E_CMBS_RC cmbs_ie_HanDeviceSubInfoGet( void * pv_RefIEList, u8 * pArrayPtr,  E_CMBS_IE_HAN_TYPE e_IE)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16	   u16_Size = 0;
	u16	   u16_Length = 0;
	int 	   Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO* SingleExtendedEntry = (ST_HAN_EXTENDED_DEVICE_INFO*)pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );	
	
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_DeviceId);

	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[0]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[1]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[2]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[3]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[4]);
	
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_RegistrationStatus);
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_RequestedPageTime);
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_PageTime); 	//final page time, after negotiation
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_DeviceEMC);

	
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_NumberOfUnits);

	for ( Units = 0; Units < SingleExtendedEntry->u8_NumberOfUnits; Units++ )
	{
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u8_UnitId);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u16_UnitType);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);

		for(Interfaces=0; Interfaces<SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
		{
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
		}

	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static E_CMBS_RC cmbs_ie_HanDeviceSubInfoPhase2Get( void * pv_RefIEList, u8 * pArrayPtr,  E_CMBS_IE_HAN_TYPE e_IE)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16	   u16_Size = 0;
	u16	   u16_Length = 0;
	int 	   Units, Interfaces;
	ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2* SingleExtendedEntry = (ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2*)pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );	
	
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_DeviceId);

	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[0]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[1]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[2]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[3]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_IPUI[4]);
	
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_RegistrationStatus);
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_RequestedPageTime);
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_PageTime); 	//final page time, after negotiation
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u16_DeviceEMC);

	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_ULECapabilities);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_UleAppProtocolId);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_UleAppProtocolVersion);
		
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->u8_NumberOfUnits);

	for ( Units = 0; Units < SingleExtendedEntry->u8_NumberOfUnits; Units++ )
	{
		u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u8_UnitId);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u16_UnitType);
		u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces);

		for(Interfaces=0; Interfaces<SingleExtendedEntry->st_UnitsInfo[Units].u16_NumberOfOptionalInterfaces; Interfaces++)
		{
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &SingleExtendedEntry->st_UnitsInfo[Units].u16_OptionalInterfaces[Interfaces]);
		}

	}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;
}

E_CMBS_RC  cmbs_ie_HanDeviceFullRegistrationInfoGet( void * pv_RefIEList,  u8* pArrayPtr, E_CMBS_IE_HAN_TYPE e_IE)
{
	u16 u16_Size=0, i = 0, j=0;
	u16	   u16_Length = 0;
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	ST_HAN_FULL_DEVICE_REGISTRATION_INFO* pst_HANCompleteRegistrationDeviceEntry = (ST_HAN_FULL_DEVICE_REGISTRATION_INFO*)pArrayPtr;

	CHECK_IE_TYPE(pu8_Buffer, e_IE);	

	cfr_ie_dser_u16( pu8_Buffer + CFR_IE_SIZE_POS, &u16_Length );	
// DECT data
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Ipui[0], CMBS_HAN_DECT_SUB_IPUI_SIZE);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Tpui[0], CMBS_HAN_DECT_SUB_TPUI_SIZE);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Dck[0], CMBS_HAN_DECT_SUB_DCK_SIZE);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Uak[0], CMBS_HAN_DECT_SUB_UAK_SIZE);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_AuthenticationCode[0], CMBS_HAN_DECT_SUB_AC_SIZE);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_UakAuth);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_Status);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_HsNumber);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_DckAssigned);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_CkLen);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_Features);	
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Defck[0], CMBS_HAN_DECT_SUB_DEFCK_SIZE);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_DefckAssigned);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_CcmCK[0], CMBS_HAN_DECT_SUB_CCM_CK_SIZE);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_DlcOperatingMode);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.u8_HlFeatures);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_Reserved[0], CMBS_HAN_DECT_SUB_RESERVED_SIZE);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[0], CMBS_HAN_DECT_SUB_NEXT_RX_SEQUENCE_NUMBER_SIZE);
	u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANCompleteRegistrationDeviceEntry->st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[0], CMBS_HAN_DECT_SUB_NEXT_TX_SEQUENCE_NUMBER_SIZE);



	// ULE data
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.DeviceId); 
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.RegistrationStatus); 
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.RequestedPageTime); 
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.NegotiatedPageTime); 
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.u16_MaxSduSize); 
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.u8_NumAppProtocols); 
		
	//App protocols
	for (i=0; i<CMBS_HAN_ULE_APP_PROTOCOLS; i++)
		{
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_UleAppProtocolId);
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_UleCapabilities);	
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_IsProprietaryProtocolId);
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_Reserved);
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].u8_UleAppProtocolVersion); 
			u16_Size += cfr_ie_dser_pu8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_ULE_RegistrationInfo.st_AppProtocols[i].pu8_EmcOrExtendedData[0], CMBS_HAN_EMC_EXTERNED_APP_SIZE_IN_BYTES);
		}


	// FUN data
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.DeviceId); 
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.DeviceEMC); 
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.NumOfUnits); 
	
	for (i=0; i<CMBS_HAN_MAX_UNITS_IN_DEVICE; i++)
		{
			u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u8_UnitId); 
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u16_UnitType); 
			u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u16_NumberOfOptionalInterfaces); 

			for (j=0; j<CMBS_HAN_MAX_OPTIONAL_INTERFACES_IN_UNIT; j++)
				{
					u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,  &pst_HANCompleteRegistrationDeviceEntry->st_FUN_RegistrationInfo.Units[i].u16_OptionalInterfaces[j]); 
				}
		}

	if (u16_Length != u16_Size )
		return CMBS_RC_ERROR_PARAMETER;
	else
		return CMBS_RC_OK;

}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceTableBriefAdd( void * pv_RefIEList, ST_IE_HAN_BRIEF_DEVICE_ENTRIES* pst_HANDeviceEntries)
{
	return cmbs_ie_HanDeviceTableBriefAdd(	pv_RefIEList,
								pst_HANDeviceEntries->u16_NumOfEntries,
								pst_HANDeviceEntries->u16_StartEntryIndex,
								(u8*)(pst_HANDeviceEntries->pst_DeviceEntries),
								sizeof(ST_HAN_DEVICE_ENTRY),
								CMBS_IE_HAN_DEVICE_TABLE_BRIEF_ENTRIES);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceTableBriefGet( void * pv_RefIEList,ST_IE_HAN_BRIEF_DEVICE_ENTRIES* pst_HANDeviceEntries)
{
	return cmbs_ie_HanDeviceTableBriefGet(	pv_RefIEList,
								&(pst_HANDeviceEntries->u16_NumOfEntries),
								&(pst_HANDeviceEntries->u16_StartEntryIndex),
								(u8*)pst_HANDeviceEntries->pst_DeviceEntries,
								sizeof(ST_HAN_BRIEF_DEVICE_INFO),
								CMBS_IE_HAN_DEVICE_TABLE_BRIEF_ENTRIES);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceTableExtendedAdd( void * pv_RefIEList, ST_IE_HAN_EXTENDED_DEVICE_ENTRIES* pst_HANExtendedDeviceEntries)
{
	return cmbs_ie_HanDeviceTableExtendedAdd(	pv_RefIEList,
								pst_HANExtendedDeviceEntries->u16_NumOfEntries,
								pst_HANExtendedDeviceEntries->u16_StartEntryIndex,
								(u8*)(pst_HANExtendedDeviceEntries->pst_DeviceEntries),
								sizeof(ST_HAN_DEVICE_ENTRY),
								CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_ENTRIES);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceTableExtendedGet( void * pv_RefIEList,ST_IE_HAN_EXTENDED_DEVICE_ENTRIES* pst_HANExtendedDeviceEntries)
{
	return cmbs_ie_HanDeviceTableExtendedGet(	pv_RefIEList,
								&(pst_HANExtendedDeviceEntries->u16_NumOfEntries),
								&(pst_HANExtendedDeviceEntries->u16_StartEntryIndex),
								(u8*)pst_HANExtendedDeviceEntries->pst_DeviceEntries,
								sizeof(ST_HAN_EXTENDED_DEVICE_INFO),
								CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_ENTRIES);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceTableExtendedPhase2Get( void * pv_RefIEList,ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES* pst_HANExtendedDeviceEntries)
{
	return cmbs_ie_HanDeviceTableExtendedPhase2Get(	pv_RefIEList,
								&(pst_HANExtendedDeviceEntries->u16_NumOfEntries),
								&(pst_HANExtendedDeviceEntries->u16_StartEntryIndex),
								(u8*)pst_HANExtendedDeviceEntries->pst_DeviceEntries,
								sizeof(ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2),
								CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_PHASE_2_ENTRIES);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceTableExtendedPhase2Add( void * pv_RefIEList, ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES* pst_HANExtendedDeviceEntries)
{
	return cmbs_ie_HanDeviceTableExtendedPhase2Add(	pv_RefIEList,
								pst_HANExtendedDeviceEntries->u16_NumOfEntries,
								pst_HANExtendedDeviceEntries->u16_StartEntryIndex,
								(u8*)(pst_HANExtendedDeviceEntries->pst_DeviceEntries),
								sizeof(ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2),
								CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_PHASE_2_ENTRIES);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceSubInfoAdd( void * pv_RefIEList, ST_HAN_EXTENDED_DEVICE_INFO* pst_HANExtendedDeviceEntry)
{
	
	return cmbs_ie_HanDeviceSubInfoAdd(	pv_RefIEList,								
								(u8*)pst_HANExtendedDeviceEntry,								
								CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceSubInfoGet( void * pv_RefIEList,ST_HAN_EXTENDED_DEVICE_INFO* pst_HANExtendedDeviceEntry)
{
	return cmbs_ie_HanDeviceSubInfoGet(	pv_RefIEList,								
								(u8*)pst_HANExtendedDeviceEntry,								
								CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceSubInfoPhase2Add( void * pv_RefIEList, ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2* pst_HANExtendedDeviceEntry)
{
	
	return cmbs_ie_HanDeviceSubInfoPhase2Add(	pv_RefIEList,								
								(u8*)pst_HANExtendedDeviceEntry,								
								CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_PHASE_2_INFO);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceSubInfoPhase2Get( void * pv_RefIEList,ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2* pst_HANExtendedDeviceEntry)
{
	return cmbs_ie_HanDeviceSubInfoPhase2Get(	pv_RefIEList,								
								(u8*)pst_HANExtendedDeviceEntry,								
								CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_PHASE_2_INFO);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceFullRegistrationInfoAdd( void * pv_RefIEList, ST_HAN_FULL_DEVICE_REGISTRATION_INFO* pst_HANCompleteRegistrationDeviceEntry)
{
	
	return cmbs_ie_HanDeviceFullRegistrationInfoAdd(	pv_RefIEList,								
								(u8*)pst_HANCompleteRegistrationDeviceEntry,								
								CMBS_IE_HAN_DEVICE_FULL_REGISTRATION_INFO);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANDeviceFullRegistrationInfoGet( void * pv_RefIEList, ST_HAN_FULL_DEVICE_REGISTRATION_INFO* pst_HANCompleteRegistrationDeviceEntry)
{
	return cmbs_ie_HanDeviceFullRegistrationInfoGet(	pv_RefIEList,								
								(u8*)pst_HANCompleteRegistrationDeviceEntry,								
								CMBS_IE_HAN_DEVICE_FULL_REGISTRATION_INFO);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANBindTableAdd	( void * pv_RefIEList, ST_IE_HAN_BIND_ENTRIES*	 pst_HanBinds)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_HanBinds);
	//printf("cmbs_api_ie_HANBindTableAdd not implemented yet!!!!!!!!\n");

	// Using a general implementation 
	return cmbs_ie_HANBindTableAdd(	pv_RefIEList,
								pst_HanBinds->u16_NumOfEntries,
								pst_HanBinds->u16_StartEntryIndex, 
								(u8*)pst_HanBinds->pst_BindEntries,	
								sizeof(ST_HAN_BIND_ENTRY),
								CMBS_IE_HAN_BIND_TABLE_ENTRIES);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANBindTableGet	( void * pv_RefIEList, ST_IE_HAN_BIND_ENTRIES*   pst_HanBinds)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_HanBinds);
	//printf("cmbs_api_ie_HANBindTableGet not implemented yet!!!!!!!!\n");

	// Using a general implementation 
	return cmbs_ie_HANBindTableGet(	pv_RefIEList,
								&(pst_HanBinds->u16_NumOfEntries),
								&(pst_HanBinds->u16_StartEntryIndex),
								(u8*)pst_HanBinds->pst_BindEntries,	
								sizeof(ST_HAN_BIND_ENTRY),
								CMBS_IE_HAN_BIND_TABLE_ENTRIES);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC		  cmbs_api_ie_HANBindEntryAdd	( void * pv_RefIEList, PST_HAN_BIND_ENTRY pst_HANBindEntry)
{
	//u8  u8_Buffer[ CFR_IE_HEADER_SIZE + sizeof(ST_HAN_MSG_REG_INFO)] = {0};
	u16 u16_Size = 0;
	CMBS_IE_GET_BUFFER(BindRecordAdd);
	
	// put data
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANBindEntry->u16_SrcDeviceID);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANBindEntry->u8_SrcUnitID);

	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANBindEntry->u16_DstDeviceID);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANBindEntry->u8_DstUnitID);

	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANBindEntry->u8_DstAddressType);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANBindEntry->u8_DstInterfaceType);
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANBindEntry->u16_DstInterfaceID);


	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_BIND_TABLE_ENTRY );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC		  cmbs_api_ie_HANBindEntryGet	( void * pv_RefIEList, PST_HAN_BIND_ENTRY pst_HANBindEntry)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;

	memset(pst_HANBindEntry, 0, sizeof(ST_HAN_BIND_ENTRY));
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_BIND_TABLE_ENTRY);	

	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANBindEntry->u16_SrcDeviceID );
	u16_Size += cfr_ie_dser_u8 ( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANBindEntry->u8_SrcUnitID );

	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANBindEntry->u16_DstDeviceID );
	u16_Size += cfr_ie_dser_u8 ( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANBindEntry->u8_DstUnitID);

	u16_Size += cfr_ie_dser_u8 ( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANBindEntry->u8_DstAddressType );
	u16_Size += cfr_ie_dser_u8 ( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANBindEntry->u8_DstInterfaceType );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANBindEntry->u16_DstInterfaceID );

	return CMBS_RC_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC		  cmbs_api_ie_HANGroupTableAdd	( void * pv_RefIEList, ST_IE_HAN_GROUP_ENTRIES*  pst_HanGroups)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_HanGroups);
	//printf("cmbs_api_ie_HANGroupTableAdd not implemented yet!!!!!!!!\n");

	// Using a general implementation 
	return cmbs_ie_HanTableAdd(	pv_RefIEList,
								pst_HanGroups->u16_NumOfEntries,
								pst_HanGroups->u16_StartEntryIndex, 
								(u8*)pst_HanGroups->pst_GroupEntries,
								sizeof(ST_HAN_GROUP_TABLE_ENTRY),
									CMBS_IE_HAN_GROUP_TABLE_ENTRIES);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANGroupTableGet	( void * pv_RefIEList, ST_IE_HAN_GROUP_ENTRIES*  pst_HanGroups)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_HanGroups);
	//printf("cmbs_api_ie_HANGroupTableGet not implemented yet!!!!!!!!\n");
	
	// Using a general implementation 
	return cmbs_ie_HanTableGet(	pv_RefIEList,
								&(pst_HanGroups->u16_NumOfEntries),
								&(pst_HanGroups->u16_StartEntryIndex),
								(u8*)pst_HanGroups->pst_GroupEntries,
								sizeof(ST_HAN_GROUP_TABLE_ENTRY),
								CMBS_IE_HAN_GROUP_TABLE_ENTRIES);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC		  cmbs_api_ie_HANGroupListAdd	( void * pv_RefIEList, ST_IE_HAN_GROUP_LIST_ENTRIES*  pst_HanGroupList)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_HanGroupList);
	//printf("cmbs_api_ie_HANGroupTableAdd not implemented yet!!!!!!!!\n");

	// Using a general implementation 
	return cmbs_ie_HanTableAdd(	pv_RefIEList,
								pst_HanGroupList->u16_NumOfEntries,
								pst_HanGroupList->u16_StartEntryIndex, 
								(u8*)pst_HanGroupList->pst_GroupListEntries,
								sizeof(ST_HAN_GROUP_LIST_ENTRY),
									CMBS_IE_HAN_GROUP_LIST_ENTRIES);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANGroupListGet	( void * pv_RefIEList, ST_IE_HAN_GROUP_LIST_ENTRIES*  pst_HanGroupList)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_HanGroupList);
	//printf("cmbs_api_ie_HANGroupTableGet not implemented yet!!!!!!!!\n");
	
	// Using a general implementation 
	return cmbs_ie_HanTableGet(	pv_RefIEList,
								&(pst_HanGroupList->u16_NumOfEntries),
								&(pst_HanGroupList->u16_StartEntryIndex),
								(u8*)pst_HanGroupList->pst_GroupListEntries,
								sizeof(ST_HAN_GROUP_LIST_ENTRY),
								CMBS_IE_HAN_GROUP_LIST_ENTRIES);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC		  cmbs_api_ie_HANBroadcastChannelTableAdd	( void * pv_RefIEList, ST_IE_HAN_BROADCAST_CHANNEL_ENTRIES*  pst_BroadcastChannelTable)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_BroadcastChannelTable);

	// Using a general implementation 
	return cmbs_ie_HanTableAdd(	pv_RefIEList,
								pst_BroadcastChannelTable->u16_NumOfEntries,
								pst_BroadcastChannelTable->u16_StartEntryIndex, 
								(u8*)pst_BroadcastChannelTable->pst_broadcastChannelEntries,
								sizeof(ST_HAN_BROADCAST_CHANNEL_ENTRY),
								CMBS_IE_HAN_BROADCAST_CHANNEL_TABLE_ENTRIES);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HANBroadcastChannelTableGet	( void * pv_RefIEList, ST_IE_HAN_BROADCAST_CHANNEL_ENTRIES*  pst_BroadcastChannelTable)
{
	UNUSED_PARAMETER(pv_RefIEList);
	UNUSED_PARAMETER(pst_BroadcastChannelTable);
	
	// Using a general implementation 
	return cmbs_ie_HanTableGet(	pv_RefIEList,
								&(pst_BroadcastChannelTable->u16_NumOfEntries),
								&(pst_BroadcastChannelTable->u16_StartEntryIndex),
								(u8*)pst_BroadcastChannelTable->pst_broadcastChannelEntries,
								sizeof(ST_HAN_BROADCAST_CHANNEL_ENTRY),
								CMBS_IE_HAN_BROADCAST_CHANNEL_TABLE_ENTRIES);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HanMsgRegInfoAdd	( void * pv_RefIEList, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo)
{
	//u8  u8_Buffer[ CFR_IE_HEADER_SIZE + sizeof(ST_HAN_MSG_REG_INFO)] = {0};
	u16 u16_Size = 0;
	CMBS_IE_GET_BUFFER(RegInfoAdd);
	// put data
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANMsgRegInfo->u8_UnitId);
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_HANMsgRegInfo->u16_InterfaceId);

	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_MSG_REG_INFO );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC		  cmbs_api_ie_HanMsgRegInfoGet	( void * pv_RefIEList, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;

	memset(pst_HANMsgRegInfo, 0, sizeof(ST_HAN_MSG_REG_INFO));
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_MSG_REG_INFO);	

	u16_Size += cfr_ie_dser_u8 ( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANMsgRegInfo->u8_UnitId );
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_HANMsgRegInfo->u16_InterfaceId );

	return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANConnectionStatusAdd (void * pv_RefIE, u16 pu16_ConnectionStatus)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, pu16_ConnectionStatus, CMBS_IE_HAN_DEVICE_CONNECTION_STATUS);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANConnectionStatusGet (void * pv_RefIE, u16* pu16_ConnectionStatus)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_ConnectionStatus, CMBS_IE_HAN_DEVICE_CONNECTION_STATUS);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANDeviceAdd (void * pv_RefIE, u16 pu16_HANDevice)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, pu16_HANDevice, CMBS_IE_HAN_DEVICE);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANDeviceGet (void * pv_RefIE, u16* pu16_HANDevice)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_HANDevice, CMBS_IE_HAN_DEVICE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANUleCapabilitiesAdd (void * pv_RefIE, u16 u16_UleCapabilities)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_UleCapabilities, CMBS_IE_HAN_ULE_CAPABILITIES);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANUleCapabilitiesGet (void * pv_RefIE, u16* pu16_UleCapabilities)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_UleCapabilities, CMBS_IE_HAN_ULE_CAPABILITIES);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANUnitIdAdd (void * pv_RefIE, u8 pu8_UnitId)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, pu8_UnitId, CMBS_IE_HAN_UNIT_ID);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANUnitIdGet (void * pv_RefIE, u8* pu8_UnitId)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_UnitId, CMBS_IE_HAN_UNIT_ID);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANLogLevelAdd (void * pv_RefIE, u8 u8_LogLevel)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_LogLevel, CMBS_IE_HAN_LOG_LEVEL);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANLogLevelGet (void * pv_RefIE, u8 *pu8_LogLevel)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_LogLevel, CMBS_IE_HAN_LOG_LEVEL);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANCertificationOperationAdd(void * pv_RefIE, u8 u8_CertificationOperationValue)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_CertificationOperationValue, CMBS_IE_HAN_CERTIFICATION_OPERATION);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANCertificationOperationGet (void * pv_RefIE, u8 *pu8_CertificationOperationValue)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_CertificationOperationValue, CMBS_IE_HAN_CERTIFICATION_OPERATION);
}

E_CMBS_RC cmbs_api_ie_HANGroupIdAdd (void * pv_RefIE, u16 u16_GroupId)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_GroupId, CMBS_IE_HAN_GROUP_ID);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANGroupIdGet (void * pv_RefIE, u16* pu16_GroupId)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_GroupId, CMBS_IE_HAN_GROUP_ID);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANRxGainGet (void * pv_RefIE, u8 *pu8_RxGain)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_RxGain, CMBS_IE_HAN_RX_GAIN);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANRxGainAdd (void * pv_RefIE, u8 u8_RxGain)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_RxGain, CMBS_IE_HAN_RX_GAIN);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTBR6CarrierGet (void * pv_RefIE, u8 *pu8_TBR6Carrier)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_TBR6Carrier, CMBS_IE_HAN_ULE_TBR6_CARRIER);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTBR6CarrierAdd (void * pv_RefIE, u8 u8_TBR6Carrier)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_TBR6Carrier, CMBS_IE_HAN_ULE_TBR6_CARRIER);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTBR6PeriodGet (void * pv_RefIE, u32 *pu32_TBR6Period)
{
	return cmbs_api_ie_u32ValueGet(pv_RefIE, pu32_TBR6Period, CMBS_IE_HAN_ULE_TBR6_PERIOD);
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTBR6PeriodAdd (void * pv_RefIE, u32 u32_TBR6Period)
{
	return cmbs_api_ie_u32ValueAdd(pv_RefIE, u32_TBR6Period, CMBS_IE_HAN_ULE_TBR6_PERIOD);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANGroupCommandFailureReasonAdd (void * pv_RefIE, u8 u8_GroupCommandFailReason)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_GroupCommandFailReason, CMBS_IE_HAN_GROUP_COMMAND_FAIL_REASON);
}

E_CMBS_RC cmbs_api_ie_HANGroupCommandFailureReasonGet (void * pv_RefIE, u8* pu8_GroupCommandFailReason)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_GroupCommandFailReason, CMBS_IE_HAN_GROUP_COMMAND_FAIL_REASON);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANULEDeviceIPUIAdd	( void * pv_RefIEList, ST_IE_HAN_ULE_DEVICE_IPUI* pst_Ipui)
{

	u16 u16_Size = 0;
	CMBS_IE_GET_BUFFER(HanULEDeviceIPUI);

	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Ipui->u8_IPUI[0]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Ipui->u8_IPUI[1]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Ipui->u8_IPUI[2]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Ipui->u8_IPUI[3]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Ipui->u8_IPUI[4]);

	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_ULE_DEVICE_IPUI );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANULEDeviceIPUIGet (void * pv_RefIE,ST_IE_HAN_ULE_DEVICE_IPUI * pst_Ipui)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIE;
	u16		u16_Size = 0;

	memset(pst_Ipui, 0, sizeof(ST_IE_HAN_ULE_DEVICE_IPUI));
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_ULE_DEVICE_IPUI);	

	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Ipui->u8_IPUI[0]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Ipui->u8_IPUI[1]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Ipui->u8_IPUI[2]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Ipui->u8_IPUI[3]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Ipui->u8_IPUI[4]);

	return CMBS_RC_OK;
	
}


E_CMBS_RC cmbs_api_ie_HANULETBR6StatisticsAdd (void * pv_RefIE, ST_IE_HAN_ULE_TBR6_STATISTICS * p_cp_st_ULETBR6Stats)
{
	u16 u16_Size = 0;
	CMBS_IE_GET_BUFFER(HanULETBR6Statistics);

	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_count);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_ber);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_fer);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_bcrc_wrong);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_first_rx_frame);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_last_rx_frame);

	u16_Size += cfr_ie_ser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_period);	
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_period_min);
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_period_max);
	u16_Size += cfr_ie_ser_u8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_period_error);	
	
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_local);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_local_min);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_local_max);

	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_min);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_max);
	
	u16_Size += cfr_ie_ser_u8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_reserved);	
	
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_missed_acks);
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_sync_attempts);
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_sync_frame);

	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->u32_Counter);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, p_cp_st_ULETBR6Stats->u32_Status);

	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_ULE_TBR6_STATISTICS );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIE, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;	
	
}


E_CMBS_RC cmbs_api_ie_HANULETBR6StatisticsGet (void * pv_RefIE,ST_IE_HAN_ULE_TBR6_STATISTICS * p_cp_st_ULETBR6Stats)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIE;
	u16		u16_Size = 0;

	memset(p_cp_st_ULETBR6Stats, 0, sizeof(ST_IE_HAN_ULE_TBR6_STATISTICS));
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_ULE_TBR6_STATISTICS);	


	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_count);
	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_ber);
	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_fer);
	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_bcrc_wrong);
	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_first_rx_frame);
	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u32_last_rx_frame);

	u16_Size += cfr_ie_dser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_period);		
	u16_Size += cfr_ie_dser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_period_min);
	u16_Size += cfr_ie_dser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_period_max);
	u16_Size += cfr_ie_dser_u8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_period_error);
	
	u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_local);
	u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_local_min);
	u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_local_max);

	u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi);
	u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_min);
	u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_rssi_max);

	u16_Size += cfr_ie_dser_u8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u8_reserved);

	u16_Size += cfr_ie_dser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_missed_acks);
	u16_Size += cfr_ie_dser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_sync_attempts);
	u16_Size += cfr_ie_dser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->st_ule_tbr6_statistics.u16_sync_frame);

	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->u32_Counter);
	u16_Size += cfr_ie_dser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &p_cp_st_ULETBR6Stats->u32_Status);

	return CMBS_RC_OK;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANCcmKeyAsUsedAdd(void * pv_RefIE, u8 u8_CcmKeyUsed)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_CcmKeyUsed, CMBS_IE_HAN_CCM_KEY_USED);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANCcmKeyAsUsedGet(void * pv_RefIE, u8 *pu8_CcmKeyUsed)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_CcmKeyUsed, CMBS_IE_HAN_CCM_KEY_USED);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANRxSequenceNumberAdd(void * pv_RefIE, u16 u16_RxSequenceNumber)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_RxSequenceNumber, CMBS_IE_HAN_RX_SEQUENCE_NUMBER);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANRxSequenceNumberGet(void * pv_RefIE, u16 *pu16_RxSequenceNumber)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_RxSequenceNumber, CMBS_IE_HAN_RX_SEQUENCE_NUMBER);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTxSequenceNumberAdd(void * pv_RefIE, u16 u16_TxSequenceNumber)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_TxSequenceNumber, CMBS_IE_HAN_TX_SEQUENCE_NUMBER);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTxSequenceNumberGet(void * pv_RefIE, u16 *pu16_TxSequenceNumber)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_TxSequenceNumber, CMBS_IE_HAN_TX_SEQUENCE_NUMBER);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTxAckDropNumberAdd(void * pv_RefIE, u16 u16_TxAckDropNumber)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_TxAckDropNumber, CMBS_IE_HAN_TX_ACK_DROP_NUMBER);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTxAckDropNumberGet (void * pv_RefIE, u16 *pu16_TxAckDropNumber)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_TxAckDropNumber, CMBS_IE_HAN_TX_ACK_DROP_NUMBER);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANPvcStateAdd(void * pv_RefIE, u8 u8_PVCState)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_PVCState, CMBS_IE_HAN_PVC_STATE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANPvcStateGet (void * pv_RefIE, u8 *pu8_PVCState)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_PVCState, CMBS_IE_HAN_PVC_STATE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANRegErrorReasonAdd (void * pv_RefIE, u16 u16_Reason)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_Reason, CMBS_IE_HAN_DEVICE_REG_ERROR_REASON);
}

E_CMBS_RC cmbs_api_ie_HANRegErrorReasonGet (void * pv_RefIE, u16* pu16_Reason)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_Reason, CMBS_IE_HAN_DEVICE_REG_ERROR_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANForcefulDeRegErrorReasonAdd (void * pv_RefIE, u16 u16_Reason)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_Reason, CMBS_IE_HAN_DEVICE_FORCEFUL_DELETE_ERROR_STATUS_REASON);
}

E_CMBS_RC cmbs_api_ie_HANForcefulDeRegErrorReasonGet (void * pv_RefIE, u16* pu16_Reason)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_Reason, CMBS_IE_HAN_DEVICE_FORCEFUL_DELETE_ERROR_STATUS_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANReadDeviceFullRegInfoResReasonAdd (void * pv_RefIE, u8  u8_ReadDeviceFullRegInfoResReason)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_ReadDeviceFullRegInfoResReason, CMBS_IE_HAN_READ_DEVICE_FULL_REGISTRATION_INFO_RES_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANReadDeviceFullRegInfoResReasonGet (void * pv_RefIE, u8 * pu8_ReadDeviceFullRegInfoResReason)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_ReadDeviceFullRegInfoResReason, CMBS_IE_HAN_READ_DEVICE_FULL_REGISTRATION_INFO_RES_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANSendErrorReasonAdd (void * pv_RefIE, u16 u16_Reason)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_Reason, CMBS_IE_HAN_SEND_FAIL_REASON);
}

E_CMBS_RC cmbs_api_ie_HANSendErrorReasonGet (void * pv_RefIE, u16* pu16_Reason)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_Reason, CMBS_IE_HAN_SEND_FAIL_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTxEndedReasonAdd (void * pv_RefIE, u16 u16_Reason)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_Reason, CMBS_IE_HAN_TX_ENDED_REASON);
}

E_CMBS_RC cmbs_api_ie_HANTxEndedReasonGet (void * pv_RefIE, u16* pu16_Reason)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_Reason, CMBS_IE_HAN_TX_ENDED_REASON);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANReqIDAdd (void * pv_RefIE, u16 u16_RequestID )
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_RequestID, CMBS_IE_REQUEST_ID);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANReqIDGet (void * pv_RefIE, u16* pu16_RequestID)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_RequestID, CMBS_IE_REQUEST_ID);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANMsgCtlAdd (void * pv_RefIE,  PST_IE_HAN_MSG_CTL pst_MessageControl)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, *(u8*)pst_MessageControl, CMBS_IE_HAN_MSG_CONTROL);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANMsgCtlGet (void * pv_RefIE, PST_IE_HAN_MSG_CTL pst_MessageControl)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, (u8*)pst_MessageControl, CMBS_IE_HAN_MSG_CONTROL);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANMsgAdd	( void * pv_RefIE, ST_IE_HAN_MSG * pst_HANMsg)
{
  //u8  u8_Buffer[ CFR_IE_HEADER_SIZE + sizeof(ST_IE_HAN_MSG) + CMBS_HAN_MAX_MSG_LEN] = {0};
  u16 u16_Size = 0;
  u8 u8_Temp;
  CMBS_IE_GET_BUFFER(HanMsgAdd);
  // put data
  // check size
  if(pst_HANMsg->u16_DataLen > CMBS_HAN_MAX_MSG_LEN)
      return CMBS_RC_ERROR_OUT_OF_MEM;

  // st_SrcDeviceUnit
  u16_Size += cfr_ie_ser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u16_SrcDeviceId);
  u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u8_SrcUnitId);

  // st_DstDeviceUnit
  u16_Size += cfr_ie_ser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u16_DstDeviceId);
  u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u8_DstUnitId);
  u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u8_DstAddressType);
  // st_MsgTransport
  u16_Size += cfr_ie_ser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->st_MsgTransport.u16_Reserved);
  
  u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u8_MsgSequence);
  
  u8_Temp = pst_HANMsg->e_MsgType;
  u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u8_Temp);
  
  u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u8_InterfaceType);
  u16_Size += cfr_ie_ser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u16_InterfaceId);
  u16_Size += cfr_ie_ser_u8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u8_InterfaceMember);
  u16_Size += cfr_ie_ser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->u16_DataLen);
  u16_Size += cfr_ie_ser_pu8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                             pst_HANMsg->pu8_Data, pst_HANMsg->u16_DataLen);
  
  // put header
  cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_MSG );
  cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );
  
  cfr_ie_ItemAdd( pv_RefIE, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );
  
  return CMBS_RC_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANMsgGet (void * pv_RefIE, ST_IE_HAN_MSG * pst_HANMsg)
{
  u8 *pu8_Buffer = (u8*)pv_RefIE;
  u16 u16_Size = 0;
  u8 u8_Temp;
  
  CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_MSG);	
  pst_HANMsg->e_MsgType = 0;
  
  // st_SrcDeviceUnit
  u16_Size += cfr_ie_dser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                              &pst_HANMsg->u16_SrcDeviceId);
  u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                              &pst_HANMsg->u8_SrcUnitId);

  // st_DstDeviceUnit
  u16_Size += cfr_ie_dser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                              &pst_HANMsg->u16_DstDeviceId);
  u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                              &pst_HANMsg->u8_DstUnitId);
  u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,
                              &pst_HANMsg->u8_DstAddressType);
  // st_MsgTransport
  u16_Size += cfr_ie_dser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,
                               &pst_HANMsg->st_MsgTransport.u16_Reserved);
  
  u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                              &pst_HANMsg->u8_MsgSequence);

  u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &u8_Temp);
  pst_HANMsg->e_MsgType = u8_Temp;
  
  u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
                              &pst_HANMsg->u8_InterfaceType);
  u16_Size += cfr_ie_dser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,
                              &pst_HANMsg->u16_InterfaceId);
  u16_Size += cfr_ie_dser_u8(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,
                              &pst_HANMsg->u8_InterfaceMember);
  u16_Size += cfr_ie_dser_u16(pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size,
                              &pst_HANMsg->u16_DataLen);
  
  if (pst_HANMsg->u16_DataLen && pst_HANMsg->pu8_Data)
  {
  	memcpy(pst_HANMsg->pu8_Data, 
           pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, 
           pst_HANMsg->u16_DataLen);
  }
  return CMBS_RC_OK;
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANNumOfEntriesAdd( void * pv_RefIEList, u16 u16_NumOfEntries)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIEList, u16_NumOfEntries, CMBS_IE_HAN_NUM_OF_ENTRIES);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANNumOfEntriesGet( void * pv_RefIEList, u16 * pu16_NumOfEntries)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIEList, pu16_NumOfEntries, CMBS_IE_HAN_NUM_OF_ENTRIES);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANIndex1stEntryAdd( void * pv_RefIEList, u16 u16_IndexOfFirstEntry)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIEList, u16_IndexOfFirstEntry, CMBS_IE_HAN_INDEX_1ST_ENTRY);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANIndex1stEntryGet( void * pv_RefIEList, u16 * pu16_IndexOfFirstEntry)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIEList, pu16_IndexOfFirstEntry, CMBS_IE_HAN_INDEX_1ST_ENTRY);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANTableUpdateInfoAdd (void * pv_RefIE, 
                          ST_IE_HAN_TABLE_UPDATE_INFO * pst_HANTableUpdateInfo)
{
  //u8  pu8_Buffer[ CFR_IE_HEADER_SIZE + sizeof(ST_IE_HAN_TABLE_UPDATE_INFO)] = {0};
  u16 u16_Size = 0;
  u8 u8_TableId = pst_HANTableUpdateInfo->e_Table;
  CMBS_IE_GET_BUFFER(UpdateInfoAdd);
  
  u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, u8_TableId);
  
  // put header
  cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_TABLE_UPDATE_INFO );
  cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );
  
  cfr_ie_ItemAdd( pv_RefIE, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );
  
  return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANTableUpdateInfoGet (void * pv_RefIE, 
                           ST_IE_HAN_TABLE_UPDATE_INFO * pst_HANTableUpdateInfo)
{
  u8 *    pu8_Buffer = (u8*)pv_RefIE;
  u16	u16_Size = 0;
  u8 	u8_TableId=0;

  CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_TABLE_UPDATE_INFO);	
  u16_Size += cfr_ie_dser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &u8_TableId);
  pst_HANTableUpdateInfo->e_Table = u8_TableId;	
	
  return CMBS_RC_OK;
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANRegStage1OKResParamsAdd	( void * pv_RefIEList, ST_HAN_REG_STAGE_1_STATUS* pst_RegStatus)
{
	//u8  u8_Buffer[ CFR_IE_HEADER_SIZE + sizeof(ST_HAN_REG_STAGE_1_STATUS)] = {0};
	u16 u16_Size = 0;
	CMBS_IE_GET_BUFFER(Stage1ParamsAdd);

	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u8_IPUI[0]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u8_IPUI[1]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u8_IPUI[2]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u8_IPUI[3]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u8_IPUI[4]);

	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_DEVICE_REG_STAGE1_OK_STATUS_PARAMS );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;	
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANUnknownDeviceContactedParamsAdd( void * pv_RefIEList, ST_HAN_UNKNOWN_DEVICE_CONTACT_PARAMS* pst_Params)
{
	//u8  u8_Buffer[ CFR_IE_HEADER_SIZE + sizeof(ST_HAN_REG_STAGE_1_STATUS)] = {0};
	u16 u16_Size = 0;
	CMBS_IE_GET_BUFFER(UnknownDeviceParamsAdd);

	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Params->SetupType);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Params->NodeResponse);
	
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Params->u8_IPUI[0]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Params->u8_IPUI[1]);
	u16_Size += cfr_ie_ser_u8 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_Params->u8_IPUI[2]);


	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_UNKNOWN_DEVICE_CONTACTED_PARAMS );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;	
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANRegStage2OKResParamsAdd	( void * pv_RefIEList, ST_HAN_REG_STAGE_2_STATUS* pst_RegStatus)
{
	//u8  u8_Buffer[ CFR_IE_HEADER_SIZE + sizeof(ST_HAN_REG_STAGE_2_STATUS)] = {0};
	u16 u16_Size = 0;
	CMBS_IE_GET_BUFFER(Stage2ParamsAdd);

	// put data
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u16_UleAppProtocolId);
	u16_Size += cfr_ie_ser_u16 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u16_UleAppProtocolVersion);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u32_OriginalDevicePagingInterval);
	u16_Size += cfr_ie_ser_u32 (pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, pst_RegStatus->u32_ActualDevicePagingInterval);
	// put header
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_TYPE_POS, CMBS_IE_HAN_DEVICE_REG_STAGE2_OK_STATUS_PARAMS );
	cfr_ie_ser_u16( pu8_Buffer + CFR_IE_SIZE_POS, u16_Size );

	cfr_ie_ItemAdd( pv_RefIEList, pu8_Buffer, CFR_IE_HEADER_SIZE + u16_Size );

	return CMBS_RC_OK;	
}

//////////////////////////////////////////////////////////////////////////


E_CMBS_RC 	cmbs_api_ie_HANRegStage2OKResParamsGet (void * pv_RefIE, ST_HAN_REG_STAGE_2_STATUS* pst_RegStatus)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIE;
	u16		u16_Size = 0;

	memset(pst_RegStatus, 0, sizeof(ST_HAN_REG_STAGE_2_STATUS));
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_DEVICE_REG_STAGE2_OK_STATUS_PARAMS);	

	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u16_UleAppProtocolId);
	u16_Size += cfr_ie_dser_u16( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u16_UleAppProtocolVersion);
	u16_Size += cfr_ie_dser_u32( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u32_OriginalDevicePagingInterval);
	u16_Size += cfr_ie_dser_u32( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u32_ActualDevicePagingInterval);
	return CMBS_RC_OK;
	
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANRegStage1OKResParamsGet (void * pv_RefIE,ST_HAN_REG_STAGE_1_STATUS* pst_RegStatus)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIE;
	u16		u16_Size = 0;

	memset(pst_RegStatus, 0, sizeof(ST_HAN_REG_STAGE_1_STATUS));
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_DEVICE_REG_STAGE1_OK_STATUS_PARAMS);	

	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u8_IPUI[0]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u8_IPUI[1]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u8_IPUI[2]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u8_IPUI[3]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_RegStatus->u8_IPUI[4]);

	return CMBS_RC_OK;
	
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANUnknownDeviceContactedParamsGet( void * pv_RefIEList, ST_HAN_UNKNOWN_DEVICE_CONTACT_PARAMS* pst_Params)
{
	u8 *    pu8_Buffer = (u8*)pv_RefIEList;
	u16		u16_Size = 0;

	memset(pst_Params, 0, sizeof(ST_HAN_UNKNOWN_DEVICE_CONTACT_PARAMS));
	CHECK_IE_TYPE(pu8_Buffer, CMBS_IE_HAN_UNKNOWN_DEVICE_CONTACTED_PARAMS);	

	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Params->SetupType);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Params->NodeResponse);

	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Params->u8_IPUI[0]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Params->u8_IPUI[1]);
	u16_Size += cfr_ie_dser_u8( pu8_Buffer + CFR_IE_HEADER_SIZE + u16_Size, &pst_Params->u8_IPUI[2]);

	return CMBS_RC_OK;
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTxReqResReasonAdd (void * pv_RefIE, u16  u16_TxReqResReason)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_TxReqResReason, CMBS_IE_HAN_TX_REQ_RES_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANTxReqResReasonGet (void * pv_RefIE, u16 * pu16_TxReqResReason)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_TxReqResReason, CMBS_IE_HAN_TX_REQ_RES_REASON);
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANGeneralStatusAdd( void * pv_RefIEList, ST_HAN_GENERAL_STATUS* pst_Status)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIEList, pst_Status->u16_Status, CMBS_IE_HAN_GENERAL_STATUS);
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANGeneralStatusGet( void * pv_RefIEList, ST_HAN_GENERAL_STATUS* pst_Staus)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIEList, &pst_Staus->u16_Status, CMBS_IE_HAN_GENERAL_STATUS);

}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANCarrierGet (void * pv_RefIE, u8 *pu8_Carrier)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_Carrier, CMBS_IE_HAN_ULE_CARRIER);
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANCarrierAdd (void * pv_RefIE, u8 u8_Carrier)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_Carrier, CMBS_IE_HAN_ULE_CARRIER);
}

//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANSlotGet (void * pv_RefIE, u8 *pu8_Slot)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_Slot, CMBS_IE_HAN_ULE_SLOT);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANSlotAdd (void * pv_RefIE, u8 u8_Slot)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_Slot, CMBS_IE_HAN_ULE_SLOT);
}


E_CMBS_RC cmbs_api_ie_HanReqFailureReasonGet (void * pv_RefIE, u8 *u8_Reason)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, u8_Reason, CMBS_IE_HAN_REQ_FAILURE_REASON);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HanReqFailureReasonAdd (void * pv_RefIE, u8 u8_Reason)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_Reason, CMBS_IE_HAN_REQ_FAILURE_REASON);
}

E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalGet (void * pv_RefIE, u32 *u32_ULEPagingInterval)
{
	return cmbs_api_ie_u32ValueGet(pv_RefIE, u32_ULEPagingInterval, CMBS_IE_HAN_ULE_PAGING_INTERVAL);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalAdd (void * pv_RefIE, u32 u32_ULEPagingInterval)
{
	return cmbs_api_ie_u32ValueAdd(pv_RefIE, u32_ULEPagingInterval, CMBS_IE_HAN_ULE_PAGING_INTERVAL);
}

E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalModificationFailureReasonGet (void * pv_RefIE, u8 *u8_Reason)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, u8_Reason, CMBS_IE_HAN_ULE_PAGING_INTERVAL_MODIFICATION_FAILURE_REASON);
}
//////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalModificationFailureReasonAdd (void * pv_RefIE, u8 u8_Reason)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_Reason, CMBS_IE_HAN_ULE_PAGING_INTERVAL_MODIFICATION_FAILURE_REASON);
}

E_CMBS_RC cmbs_api_ie_HanNodeSetupAttemptsEnableGet (void * pv_RefIE, u8 *u8_Enable)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, u8_Enable, CMBS_IE_HAN_NODE_SETUP_ATTEMPTS_ENABLE);
}


E_CMBS_RC cmbs_api_ie_HanNodeSetupAttemptsEnableAdd (void * pv_RefIE, u8 u8_Enable)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_Enable, CMBS_IE_HAN_NODE_SETUP_ATTEMPTS_ENABLE);
}


E_CMBS_RC cmbs_api_ie_HanDeviceLinkReleaseReasonGet (void * pv_RefIE, u8 *pu8_ReleaseReason)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_ReleaseReason, CMBS_IE_HAN_LINK_RELEASE_REASON);
}


E_CMBS_RC cmbs_api_ie_HanDeviceLinkReleaseReasonAdd (void * pv_RefIE, u8 u8_ReleaseReason)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_ReleaseReason, CMBS_IE_HAN_LINK_RELEASE_REASON);
}


E_CMBS_RC cmbs_api_ie_HanMaxNumOfDevicesGet (void * pv_RefIE, u16 *pu16_MaxNumOfDevices)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_MaxNumOfDevices, CMBS_IE_HAN_MAX_NUM_OF_DEVICES);
}

E_CMBS_RC cmbs_api_ie_HanMaxNumOfDevicesAdd (void * pv_RefIE, u16 u16_MaxNumOfDevices)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_MaxNumOfDevices, CMBS_IE_HAN_MAX_NUM_OF_DEVICES);
}

E_CMBS_RC cmbs_api_ie_HANMsgRSSIAdd ( void * pv_RefIE, u8 u8_RSSI)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_RSSI, CMBS_IE_HAN_MSG_RSSI);
}
E_CMBS_RC cmbs_api_ie_HANMsgRSSIGet ( void * pv_RefIE, u8* u8_RSSI)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, u8_RSSI, CMBS_IE_HAN_MSG_RSSI);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
E_CMBS_RC cmbs_api_ie_HANDeviceBlackListDeleteErrorReasonAdd (void * pv_RefIE, u16 u16_Reason)
{
	return cmbs_api_ie_ShortValueAdd(pv_RefIE, u16_Reason, CMBS_IE_HAN_DEVICE_BLACK_LIST_DELETE_ERROR_STATUS_REASON);
}

E_CMBS_RC cmbs_api_ie_HANDeviceBlackListDeleteErrorReasonGet (void * pv_RefIE, u16* pu16_Reason)
{
	return cmbs_api_ie_ShortValueGet(pv_RefIE, pu16_Reason, CMBS_IE_HAN_DEVICE_BLACK_LIST_DELETE_ERROR_STATUS_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANBroadcastChannelIdGet (void * pv_RefIE, u8 *pu8_BroadcastChannelId)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_BroadcastChannelId, CMBS_IE_HAN_BROADCAST_CHANNEL_ID);
}


E_CMBS_RC cmbs_api_ie_HANBroadcastChannelIdAdd (void * pv_RefIE, u8 u8_BroadcastChannelId)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_BroadcastChannelId, CMBS_IE_HAN_BROADCAST_CHANNEL_ID);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANPvcResetReqFailureReasonGet (void * pv_RefIE, u8 *pu8_PvcResetReqFailureReason)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_PvcResetReqFailureReason, CMBS_IE_HAN_PVC_RESET_REQ_FAIL_REASON);
}


E_CMBS_RC cmbs_api_ie_HANPvcResetReqFailureReasonAdd (void * pv_RefIE, u8 u8_PvcResetReqFailureReason)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_PvcResetReqFailureReason, CMBS_IE_HAN_PVC_RESET_REQ_FAIL_REASON);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateFailureReasonGet (void * pv_RefIE, u8 *pu8_PvcResetReqStateFailureReason)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_PvcResetReqStateFailureReason, CMBS_IE_HAN_PVC_RESET_REQ_STATE_FAIL_REASON);
}


E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateFailureReasonAdd (void * pv_RefIE, u8 u8_PvcResetReqStateFailureReason)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_PvcResetReqStateFailureReason, CMBS_IE_HAN_PVC_RESET_REQ_STATE_FAIL_REASON);
}

E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateGet (void * pv_RefIE, u8 *pu8_PvcResetReqState)
{
	return cmbs_api_ie_ByteValueGet(pv_RefIE, pu8_PvcResetReqState, CMBS_IE_HAN_PVC_RESET_REQ_STATE);
}


E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateAdd (void * pv_RefIE, u8 u8_PvcResetReqState)
{
	return cmbs_api_ie_ByteValueAdd(pv_RefIE, u8_PvcResetReqState, CMBS_IE_HAN_PVC_RESET_REQ_STATE);
}
//////////////////////////////////////////////////////////////////////////
