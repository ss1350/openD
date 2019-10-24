/*!
*  \file       cmbs_han_ie.h
*  \brief      Information Elements List functions for HAN
*  \author     andriig
*
*  @(#)  %filespec: cmbs_han_ie.h~ILD53#3 %
*
*******************************************************************************/

#if   !defined( CMBS_HAN_IE_H )
#define  CMBS_HAN_IE_H

#include "cmbs_han.h"               /* CMBS HAN API definition */
#include "cmbs_fun.h"

E_CMBS_RC cmbs_api_ie_HanCfgAdd( void * pv_RefIEList, ST_IE_HAN_CONFIG* pst_Cfg);
E_CMBS_RC cmbs_api_ie_HanCfgGet( void * pv_RefIEList, ST_IE_HAN_CONFIG* pst_Cfg);

E_CMBS_RC		  cmbs_api_ie_HanUleBaseInfoAdd( void * pv_RefIEList, ST_IE_HAN_BASE_INFO* pst_HanBaseInfo);
E_CMBS_RC		  cmbs_api_ie_HanUleBaseInfoGet( void * pv_RefIEList, ST_IE_HAN_BASE_INFO* pst_HanBaseInfo);

E_CMBS_RC  cmbs_api_ie_HANDeviceSubInfoAdd( void * pv_RefIEList,  ST_HAN_EXTENDED_DEVICE_INFO * pst_HANExtendedDeviceEntry);
E_CMBS_RC  cmbs_api_ie_HANDeviceSubInfoGet( void * pv_RefIEList,   ST_HAN_EXTENDED_DEVICE_INFO * pst_HANExtendedDeviceEntry);

E_CMBS_RC cmbs_api_ie_HANDeviceTableBriefAdd	( void * pv_RefIEList, ST_IE_HAN_BRIEF_DEVICE_ENTRIES* pst_HanDeviceEntries);
E_CMBS_RC cmbs_api_ie_HANDeviceTableBriefGet (void * pv_RefIE, ST_IE_HAN_BRIEF_DEVICE_ENTRIES * pst_HANDeviceTable);

E_CMBS_RC cmbs_api_ie_HANDeviceTableExtendedAdd	( void * pv_RefIEList, ST_IE_HAN_EXTENDED_DEVICE_ENTRIES* pst_HANExtendedDeviceEntries);
E_CMBS_RC cmbs_api_ie_HANDeviceTableExtendedGet (void * pv_RefIE, ST_IE_HAN_EXTENDED_DEVICE_ENTRIES* pst_HANExtendedDeviceEntries);
	
E_CMBS_RC cmbs_api_ie_HANRegStage1OKResParamsAdd	( void * pv_RefIEList, ST_HAN_REG_STAGE_1_STATUS* pst_RegStatus);
E_CMBS_RC cmbs_api_ie_HANRegStage1OKResParamsGet (void * pv_RefIE, ST_HAN_REG_STAGE_1_STATUS* pst_RegStatus);

E_CMBS_RC cmbs_api_ie_HANRegStage2OKResParamsAdd	( void * pv_RefIEList,ST_HAN_REG_STAGE_2_STATUS* pst_RegStatus);
E_CMBS_RC cmbs_api_ie_HANRegStage2OKResParamsGet (void * pv_RefIE, ST_HAN_REG_STAGE_2_STATUS* pst_RegStatus);

E_CMBS_RC cmbs_api_ie_HANBindTableAdd	( void * pv_RefIEList, ST_IE_HAN_BIND_ENTRIES*	 pst_HanBinds);
E_CMBS_RC cmbs_api_ie_HANBindTableGet (void * pv_RefIE, ST_IE_HAN_BIND_TABLE * pst_HANBindTable);

E_CMBS_RC cmbs_api_ie_HANGroupTableAdd	( void * pv_RefIEList, ST_IE_HAN_GROUP_ENTRIES*  pst_HanGroups);
E_CMBS_RC cmbs_api_ie_HANGroupTableGet (void * pv_RefIE, ST_IE_HAN_GROUP_TABLE * pst_HANGroupTable);

E_CMBS_RC cmbs_api_ie_HanMsgRegInfoAdd	( void * pv_RefIEList, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);
E_CMBS_RC cmbs_api_ie_HanMsgRegInfoGet	( void * pv_RefIEList, ST_HAN_MSG_REG_INFO * pst_HANMsgRegInfo);

E_CMBS_RC cmbs_api_ie_HANMsgAdd	( void * pv_RefIE, ST_IE_HAN_MSG * pst_HANMsg);
E_CMBS_RC cmbs_api_ie_HANMsgGet (void * pv_RefIE, ST_IE_HAN_MSG * pst_HANMsg);

E_CMBS_RC cmbs_api_ie_HANDeviceAdd (void * pv_RefIE, u16 pu16_HANDevice);
E_CMBS_RC cmbs_api_ie_HANDeviceGet (void * pv_RefIE, u16* pu16_HANDevice);

E_CMBS_RC cmbs_api_ie_HANSendErrorReasonAdd (void * pv_RefIE, u16 u16_Reason);
E_CMBS_RC cmbs_api_ie_HANSendErrorReasonGet (void * pv_RefIE, u16* pu16_Reason);

E_CMBS_RC cmbs_api_ie_HANTxEndedReasonAdd (void * pv_RefIE, u16 u16_Reason);
E_CMBS_RC cmbs_api_ie_HANTxEndedReasonGet (void * pv_RefIE, u16* pu16_Reason);

E_CMBS_RC cmbs_api_ie_HANNumOfEntriesAdd( void * pv_RefIEList, u16 u16_NumOfEntries);
E_CMBS_RC cmbs_api_ie_HANNumOfEntriesGet( void * pv_RefIEList, u16 * pu16_NumOfEntries);

E_CMBS_RC cmbs_api_ie_HANIndex1stEntryAdd( void * pv_RefIEList, u16 u16_IndexOfFirstEntry);
E_CMBS_RC cmbs_api_ie_HANIndex1stEntryGet( void * pv_RefIEList, u16 * pu16_IndexOfFirstEntry);

E_CMBS_RC cmbs_api_ie_HANTableUpdateInfoAdd (void * pv_RefIE, ST_IE_HAN_TABLE_UPDATE_INFO * pst_HANTableUpdateInfo);
E_CMBS_RC cmbs_api_ie_HANTableUpdateInfoGet (void * pv_RefIE, ST_IE_HAN_TABLE_UPDATE_INFO * pst_HANTableUpdateInfo);

E_CMBS_RC cmbs_api_ie_HANGeneralStatusAdd( void * pv_RefIEList, ST_HAN_GENERAL_STATUS* pst_Status);
E_CMBS_RC cmbs_api_ie_HANGeneralStatusGet( void * pv_RefIEList, ST_HAN_GENERAL_STATUS* pst_Status);

E_CMBS_RC cmbs_api_ie_HANUnknownDeviceContactedParamsAdd( void * pv_RefIEList, ST_HAN_UNKNOWN_DEVICE_CONTACT_PARAMS* pst_Params);
E_CMBS_RC cmbs_api_ie_HANUnknownDeviceContactedParamsGet( void * pv_RefIEList, ST_HAN_UNKNOWN_DEVICE_CONTACT_PARAMS* pst_Params);

E_CMBS_RC cmbs_api_ie_HANConnectionStatusAdd (void * pv_RefIE, u16 pu16_ConnectionStatus);
E_CMBS_RC cmbs_api_ie_HANConnectionStatusGet (void * pv_RefIE, u16* pu16_ConnectionStatus);

E_CMBS_RC cmbs_api_ie_HANForcefulDeRegErrorReasonAdd (void * pv_RefIE, u16 u16_Reason);
E_CMBS_RC cmbs_api_ie_HANForcefulDeRegErrorReasonGet (void * pv_RefIE, u16* pu16_Reason);

E_CMBS_RC cmbs_api_ie_HANRegErrorReasonAdd (void * pv_RefIE, u16 u16_Reason);
E_CMBS_RC cmbs_api_ie_HANRegErrorReasonGet (void * pv_RefIE, u16* pu16_Reason);

E_CMBS_RC cmbs_api_ie_HANReqIDAdd (void * pv_RefIE, u16 u16_RequestID );
E_CMBS_RC cmbs_api_ie_HANReqIDGet (void * pv_RefIE, u16* pu16_RequestID);

E_CMBS_RC cmbs_api_ie_HANMsgCtlAdd (void * pv_RefIE, PST_IE_HAN_MSG_CTL pst_MessageControl);
E_CMBS_RC cmbs_api_ie_HANMsgCtlGet (void * pv_RefIE, PST_IE_HAN_MSG_CTL pst_MessageControl);

E_CMBS_RC  cmbs_api_ie_HanFUNDeviceInfoAdd( void * pv_RefIEList, ST_FUN_DEVICE_INFO* FunDevInfo);
E_CMBS_RC  cmbs_api_ie_HanFUNDeviceInfoGet( void * pv_RefIEList, ST_FUN_DEVICE_INFO* FunDevInfo);

E_CMBS_RC cmbs_api_ie_HANBindEntryAdd	( void * pv_RefIEList, PST_HAN_BIND_ENTRY pst_HANBindEntry);
E_CMBS_RC cmbs_api_ie_HANBindEntryGet	( void * pv_RefIEList, PST_HAN_BIND_ENTRY pst_HANBindEntry);

E_CMBS_RC cmbs_api_ie_HANTxReqResReasonAdd (void * pv_RefIE, u16  u16_TxReqResReason);
E_CMBS_RC cmbs_api_ie_HANTxReqResReasonGet (void * pv_RefIE, u16* pu16_TxReqResReason);

E_CMBS_RC cmbs_api_ie_HANLogLevelAdd (void * pv_RefIE, u8 u8_LogLevel);
E_CMBS_RC cmbs_api_ie_HANLogLevelGet (void * pv_RefIE, u8 *pu8_LogLevel);

E_CMBS_RC cmbs_api_ie_HANCertificationOperationAdd (void * pv_RefIE, u8 u8_CertificationOperationValue);
E_CMBS_RC cmbs_api_ie_HANCertificationOperationGet (void * pv_RefIE, u8 *pu8_CertificationOperationValue);

E_CMBS_RC cmbs_api_ie_HANCcmKeyAsUsedAdd(void * pv_RefIE, u8 u8_CcmKeyUsed);
E_CMBS_RC cmbs_api_ie_HANCcmKeyAsUsedGet(void * pv_RefIE, u8 *pu8_CcmKeyUsed);

E_CMBS_RC cmbs_api_ie_HANRxSequenceNumberAdd(void * pv_RefIE, u16 u16_RxSequenceNumber);
E_CMBS_RC cmbs_api_ie_HANRxSequenceNumberGet (void * pv_RefIE, u16 *pu16_RxSequenceNumber);

E_CMBS_RC cmbs_api_ie_HANTxSequenceNumberAdd(void * pv_RefIE, u16 u16_TxSequenceNumber);
E_CMBS_RC cmbs_api_ie_HANTxSequenceNumberGet (void * pv_RefIE, u16 *pu16_TxSequenceNumber);

E_CMBS_RC cmbs_api_ie_HANTxAckDropNumberAdd(void * pv_RefIE, u16 u16_TxAckDropNumber);
E_CMBS_RC cmbs_api_ie_HANTxAckDropNumberGet(void * pv_RefIE, u16 *pu16_TxAckDropNumber);

E_CMBS_RC cmbs_api_ie_HANPvcStateAdd(void * pv_RefIE, u8 u8_PVCState);
E_CMBS_RC cmbs_api_ie_HANPvcStateGet (void * pv_RefIE, u8 *pu8_PVCState);

E_CMBS_RC cmbs_api_ie_HANRxGainGet (void * pv_RefIE, u8 *pu8_RxGain);
E_CMBS_RC cmbs_api_ie_HANRxGainAdd (void * pv_RefIE, u8 u8_RxGain);

E_CMBS_RC cmbs_api_ie_HANDeviceFullRegistrationInfoAdd( void * pv_RefIEList, ST_HAN_FULL_DEVICE_REGISTRATION_INFO* pst_HANCompleteRegistrationDeviceEntry);
E_CMBS_RC cmbs_api_ie_HANDeviceFullRegistrationInfoGet( void * pv_RefIEList, ST_HAN_FULL_DEVICE_REGISTRATION_INFO* pst_HANCompleteRegistrationDeviceEntry);

E_CMBS_RC cmbs_api_ie_HANReadDeviceFullRegInfoResReasonAdd (void * pv_RefIE, u8  u8_ReadDeviceFullRegInfoResReason);
E_CMBS_RC cmbs_api_ie_HANReadDeviceFullRegInfoResReasonGet (void * pv_RefIE, u8  * pu8_ReadDeviceFullRegInfoResReason);

E_CMBS_RC cmbs_api_ie_HANTBR6CarrierGet (void * pv_RefIE, u8 *pu8_TBR6Carrier);
E_CMBS_RC cmbs_api_ie_HANTBR6CarrierAdd (void * pv_RefIE, u8 u8_TBR6Carrier);

E_CMBS_RC cmbs_api_ie_HANTBR6PeriodGet (void * pv_RefIE, u32 *pu32_TBR6Period);
E_CMBS_RC cmbs_api_ie_HANTBR6PeriodAdd (void * pv_RefIE, u32 u32_TBR6Period);

E_CMBS_RC cmbs_api_ie_HANULEDeviceIPUIAdd	( void * pv_RefIEList, ST_IE_HAN_ULE_DEVICE_IPUI* pst_Ipui);
E_CMBS_RC cmbs_api_ie_HANULEDeviceIPUIGet (void * pv_RefIE,ST_IE_HAN_ULE_DEVICE_IPUI* pst_Ipui);

E_CMBS_RC cmbs_api_ie_HANULETBR6StatisticsAdd (void * pv_RefIE,ST_IE_HAN_ULE_TBR6_STATISTICS * p_cp_st_ULETBR6Stats);
E_CMBS_RC cmbs_api_ie_HANULETBR6StatisticsGet (void * pv_RefIE,ST_IE_HAN_ULE_TBR6_STATISTICS * p_cp_st_ULETBR6Stats);

E_CMBS_RC cmbs_api_ie_HANCarrierGet (void * pv_RefIE, u8 *pu8_Carrier);
E_CMBS_RC cmbs_api_ie_HANCarrierAdd (void * pv_RefIE, u8 u8_Carrier);

E_CMBS_RC cmbs_api_ie_HANSlotGet (void * pv_RefIE, u8 *pu8_Slot);
E_CMBS_RC cmbs_api_ie_HANSlotAdd (void * pv_RefIE, u8 u8_Slot);

E_CMBS_RC cmbs_api_ie_HanReqFailureReasonGet (void * pv_RefIE, u8 *u8_Reason);
E_CMBS_RC cmbs_api_ie_HanReqFailureReasonAdd (void * pv_RefIE, u8 u8_Reason);

E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalGet (void * pv_RefIE, u32 *u32_ULEPagingInterval);
E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalAdd (void * pv_RefIE, u32 u32_ULEPagingInterval);

E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalModificationFailureReasonGet (void * pv_RefIE, u8 *u8_Reason);
E_CMBS_RC cmbs_api_ie_HanULEPagingIntervalModificationFailureReasonAdd (void * pv_RefIE, u8 u8_Reason);

E_CMBS_RC cmbs_api_ie_HanNodeSetupAttemptsEnableGet (void * pv_RefIE, u8 *u8_Enable);
E_CMBS_RC cmbs_api_ie_HanNodeSetupAttemptsEnableAdd (void * pv_RefIE, u8 u8_Enable);

E_CMBS_RC cmbs_api_ie_HanDeviceLinkReleaseReasonGet (void * pv_RefIE, u8 *pu8_ReleaseReason);
E_CMBS_RC cmbs_api_ie_HanDeviceLinkReleaseReasonAdd (void * pv_RefIE, u8 u8_ReleaseReason);

E_CMBS_RC cmbs_api_ie_HanMaxNumOfDevicesGet (void * pv_RefIE, u16 *pu16_MaxNumOfDevices);
E_CMBS_RC cmbs_api_ie_HanMaxNumOfDevicesAdd (void * pv_RefIE, u16 u16_MaxNumOfDevices);

E_CMBS_RC cmbs_api_ie_HANGroupIdAdd (void * pv_RefIE, u16 u16_GroupId);
E_CMBS_RC cmbs_api_ie_HANGroupIdGet (void * pv_RefIE, u16* pu16_GroupId);

E_CMBS_RC cmbs_api_ie_HANGroupCommandFailureReasonAdd (void * pv_RefIE, u8 u8_GroupCommandFailReason);
E_CMBS_RC cmbs_api_ie_HANGroupCommandFailureReasonGet (void * pv_RefIE, u8* pu8_GroupCommandFailReason);

E_CMBS_RC cmbs_api_ie_HANUnitIdAdd (void * pv_RefIE, u8 pu8_UnitId);
E_CMBS_RC cmbs_api_ie_HANUnitIdGet (void * pv_RefIE, u8* pu8_UnitId);

E_CMBS_RC cmbs_api_ie_HANGroupListAdd ( void * pv_RefIEList, ST_IE_HAN_GROUP_LIST_ENTRIES*  pst_HanGroupList);
E_CMBS_RC cmbs_api_ie_HANGroupListGet ( void * pv_RefIEList, ST_IE_HAN_GROUP_LIST_ENTRIES*  pst_HanGroupList);

E_CMBS_RC cmbs_api_ie_HANMsgRSSIAdd ( void * pv_RefIE, u8 u8_RSSI);
E_CMBS_RC cmbs_api_ie_HANMsgRSSIGet ( void * pv_RefIE, u8* u8_RSSI);

E_CMBS_RC cmbs_api_ie_HANDeviceBlackListDeleteErrorReasonAdd (void * pv_RefIE, u16 u16_Reason);
E_CMBS_RC cmbs_api_ie_HANDeviceBlackListDeleteErrorReasonGet (void * pv_RefIE, u16* pu16_Reason);

E_CMBS_RC cmbs_api_ie_HANUleCapabilitiesAdd (void * pv_RefIE, u16 u16_UleCapabilities);
E_CMBS_RC cmbs_api_ie_HANUleCapabilitiesGet (void * pv_RefIE, u16* pu16_UleCapabilities);

E_CMBS_RC cmbs_api_ie_HANBroadcastChannelIdGet (void * pv_RefIE, u8 *pu8_BroadcastChannelId);
E_CMBS_RC cmbs_api_ie_HANBroadcastChannelIdAdd (void * pv_RefIE, u8 u8_BroadcastChannelId);

E_CMBS_RC cmbs_api_ie_HANBroadcastChannelTableAdd	( void * pv_RefIEList, ST_IE_HAN_BROADCAST_CHANNEL_ENTRIES*  pst_BroadcastChannelTable);
E_CMBS_RC cmbs_api_ie_HANBroadcastChannelTableGet	( void * pv_RefIEList, ST_IE_HAN_BROADCAST_CHANNEL_ENTRIES*  pst_BroadcastChannelTable);

E_CMBS_RC cmbs_api_ie_HANPvcResetReqFailureReasonGet (void * pv_RefIE, u8 *pu8_PvcResetReqFailureReason);
E_CMBS_RC cmbs_api_ie_HANPvcResetReqFailureReasonAdd (void * pv_RefIE, u8 u8_PvcResetReqFailureReason);

E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateFailureReasonGet (void * pv_RefIE, u8 *pu8_PvcResetReqStateFailureReason);
E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateFailureReasonAdd (void * pv_RefIE, u8 u8_PvcResetReqStateFailureReason);

E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateGet (void * pv_RefIE, u8 *pu8_PvcResetReqState);
E_CMBS_RC cmbs_api_ie_HANPvcResetReqStateAdd (void * pv_RefIE, u8 u8_PvcResetReqState);

E_CMBS_RC cmbs_api_ie_HANDeviceTableExtendedPhase2Get( void * pv_RefIEList,ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES* pst_HANExtendedDeviceEntries);
E_CMBS_RC cmbs_api_ie_HANDeviceTableExtendedPhase2Add( void * pv_RefIEList, ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES* pst_HANExtendedDeviceEntries);

E_CMBS_RC cmbs_api_ie_HANDeviceSubInfoPhase2Add( void * pv_RefIEList,  ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 * pst_HANExtendedDeviceEntry);
E_CMBS_RC cmbs_api_ie_HANDeviceSubInfoPhase2Get( void * pv_RefIEList,  ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 * pst_HANExtendedDeviceEntry);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif	// CMBS_HAN_IE_H
