/*!
*	\file
*	\brief
*	\Author		kelbch
*
*	@(#)	%filespec: appsrv.h~DMZD53#9.1.13 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
* 15-Apr-2015 tcmc_asa  Defect 14073: added app_SrvSetNewHandsetNameAndLinePref
* 23-Mar-2015 tcmc_asa  correct app_SrvSubscriptionOpenExt STATE -> ENABLE
* 23-Mar-2015 tcmc_asa  added app_SrvSubscriptionOpenExt
* 21-Mar-2015 tcmc_asa  added parameters to app_SrvSubscriptionOpen
* 21-Nov-2014 tcmc_asa  ---GIT-- added parameters to app_SrvHandsetStopPaging()
* 17-Oct-2014 tcmc_asa  ---GIT-- added app_SrvHandsetDeleteINL()
* 26-Oct-2001 tcmc_asa NBGD53#9.1.5 added app_SrvFixedCarrierSet
* 26-Oct-2001 tcmc_asa 9.1.6        merged 2 versions 9.1.5
*******************************************************************************/

#if	!defined( APPSRV_H )
#define	APPSRV_H

#include "cmbs_platf.h"

#if defined( __cplusplus )
extern "C"
{
#endif

void			appsrv_Initialize( void );

E_CMBS_RC      app_SrvHandsetDelete ( char * psz_Handsets );
E_CMBS_RC      app_SrvHandsetDeleteINL(u16 u16_HandsetMask);
E_CMBS_RC      app_SrvHandsetAddINL(char *psz_Handsets);
E_CMBS_RC      app_SrvHandsetPage( char * psz_Handsets );
E_CMBS_RC      app_SrvHandsetStopPaging(u16 u16_HsMask, u32 u32_CallInstance  );
E_CMBS_RC      app_SrvCWDisable(u16 u16_HandsetMask);
E_CMBS_RC      app_SrvSubscriptionOpen( u32 u32_timeout);
E_CMBS_RC      app_SrvSubscriptionOpenExt( u32 u32_timeout, E_CMBS_HS_REGISTER_ENABLE e_RegEnable);
E_CMBS_RC      app_SrvSubscriptionClose( void );
E_CMBS_RC      app_SrvEncryptionDisable( void );
E_CMBS_RC      app_SrvEncryptionEnable( void );
E_CMBS_RC      app_SrvFixedCarrierSet( u8 u8_Value );
E_CMBS_RC      app_SrvNemoControlSet(u8 u8_Value);
E_CMBS_RC      app_SrvRepeaterTypeSet(u8 u8_Value);

//E_CMBS_RC      app_SrvPINCodeGet( u32 u32_Token );
E_CMBS_RC      app_SrvPINCodeSet( char * psz_PIN );
//E_CMBS_RC      app_SrvRFPIGet( u32 u32_Token );
E_CMBS_RC      app_SrvTestModeGet( u32 u32_Token );
E_CMBS_RC      app_SrvTestModeSet( void );
E_CMBS_RC      app_SrvFWVersionGet( E_CMBS_MODULE e_Module, u32 u32_Token );
E_CMBS_RC      app_SrvEEPROMVersionGet( u32 u32_Token );
E_CMBS_RC      app_SrvHWVersionGet( u32 u32_Token );
E_CMBS_RC      app_SrvLogBufferStart( void );
E_CMBS_RC      app_SrvLogBufferStop( void );
E_CMBS_RC      app_SrvLogBufferRead( u32 u32_Token );
E_CMBS_RC      app_SrvSystemReboot( void );
E_CMBS_RC      app_SrvSystemPowerOff( void );
E_CMBS_RC      app_SrvRegisteredHandsets( u16 u16_HsMask, u32 u32_Token );
E_CMBS_RC      app_SrvSetNewHandsetName( u16 u16_HsId, u8* pu8_HsName, u16 u16_HsNameSize, u32 u32_Token );
E_CMBS_RC      app_SrvSetNewHandsetNameWithCatIqProp(u16 u16_HsId, u8 *pu8_HsName, u16 u16_HsNameSize, u32 u32_Token, u16 u16_HsCapabilities);
E_CMBS_RC      app_SrvSetBackupedHandsetNames(void );
E_CMBS_RC      app_SrvSetNewHandsetNameAndLinePref(u16 u16_HsId, u8 *pu8_HsName, u16 u16_HsNameSize, u32 u32_Token, u8 u8_LinePref, ST_IE_PROP_FIELDS_LIST *pst_PropSettingsList);
E_CMBS_RC      app_SrvLineSettingsGet( u16 u16_LinesMask, u32 u32_Token );
E_CMBS_RC      app_SrvLineSettingsSet( ST_IE_LINE_SETTINGS_LIST* pst_LineSettingsList, u32 u32_Token );
E_CMBS_RC      app_SrvRFSuspend( void );
E_CMBS_RC      app_SrvRFResume( void );
E_CMBS_RC      app_SrvTurnOnNEMo( void );
E_CMBS_RC      app_SrvTurnOffNEMo( void );
E_CMBS_RC      app_SrvParamGet( E_CMBS_PARAM e_Param, u32 u32_Token );
E_CMBS_RC      app_SrvParamSet( E_CMBS_PARAM e_Param, u8* pu8_Data, u16 u16_Length, u32 u32_Token );
E_CMBS_RC      app_ProductionParamGet( E_CMBS_PARAM e_Param, u32 u32_Token );
E_CMBS_RC      app_ProductionParamSet( E_CMBS_PARAM e_Param, u8* pu8_Data, u16 u16_Length, u32 u32_Token );
E_CMBS_RC      app_FxsParamSet( E_CMBS_PARAM e_Param, u8* pu8_Data, u16 u16_Length, u32 u32_Token );
E_CMBS_RC      app_SrvParamAreaGet( E_CMBS_PARAM_AREA_TYPE e_AreaType, u32 u32_Pos, u16 u16_Length, u32 u32_Token );
E_CMBS_RC      app_SrvParamAreaSet( E_CMBS_PARAM_AREA_TYPE e_AreaType, u32 u32_Pos, u16 u16_Length, u8* pu8_Data, u32 u32_Token );
E_CMBS_RC      app_OnHandsetLinkRelease( void * pv_List );
E_CMBS_RC      app_SrvDectSettingsGet( u32 u32_Token );
E_CMBS_RC      app_SrvPropDectSettingsSet(ST_IE_DECT_SETTINGS_LIST *pst_DectSettingsList, ST_IE_PROP_FIELDS_LIST *pst_PropSettingsList, u32 u32_Token);
E_CMBS_RC      app_SrvDectSettingsSet( ST_IE_DECT_SETTINGS_LIST* pst_DectSettingsList, u32 u32_Token );
E_CMBS_RC      app_SrvAddNewExtension (u8* pu8_Name, u16 u16_NameSize, u8* pu8_Number, u8 u8_NumberSize, u32 u32_Token );
E_CMBS_RC      app_SrvGetBaseName(u32 u32_Token);
E_CMBS_RC      app_SrvSetBaseName( u8* pu8_BaseName, u8 u8_BaseNameSize, u32 u32_Token);	
E_CMBS_RC	   app_SrvGetEepromSize( void );
E_CMBS_RC	   app_SrvGetEepromExtSize( void );
E_CMBS_RC      app_FXSEvent(const ST_IE_DATA *pst_Data);
int            app_ServiceEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData );
bool 		   app_isHsRegistered(u8 u8_HsId);
void 		   appsrv_AFEOpenAudioChannel (void);
void 		   appsrv_AFECloseAudioChannel (u32 u32_ChannelID);
void 		   appsrv_AFEAllocateChannel();
void 		   app_AFEConnectEndpoints();
void 		   app_AFEEnableDisableEndpoint(ST_IE_AFE_ENDPOINT* pst_Endpoint, u16 u16_Enable);
void 		   appsrv_AFESetEndpointGain(ST_IE_AFE_ENDPOINT_GAIN* pst_EndpointGain, u16 u16_Input);
void 		   appsrv_AFEDHSGSendByte(u8 u8_Value);
void 		   app_SrvGPIOEnable(PST_IE_GPIO_ID st_GPIOId);
void 		   app_SrvGPIODisable(PST_IE_GPIO_ID st_GPIOId);
E_CMBS_RC      app_SrvEEPROMBackupRestore(void);
E_CMBS_RC      app_SrvEEPROMBackupCreate(void);
void           appOnDTAMPlayMsg(void *pv_List);
void 		   appOnDTAMStartSession(void *pv_List);
void 		   appOnDTAMSelectNeighbourMsg(void *pv_List);
void 		   appOnDTAMSelectNeighbourMsg(void *pv_List);
void 		   appOnDTAMDeleteMsg(void *pv_List);
void 		   appOnDTAMPauseResume(void *pv_List);
void 		   appOnDTAMStopMsgPlay(void *pv_List);
void 		   appOnDTAMRecordWelcomeMsg(void *pv_List);
void 		   appOnDTAMRecordWelcomeMsgStop(void *pv_List);
E_CMBS_RC 	   app_SrvSendHSRSSIReq(u8 u8_HSNum);
void 		   appsrv_SetCallStateFilter(u8 u8_FilterType);
void 		   app_SrvBBDUpdate (void);
void 		   app_SrvLineTestStart(u8 Channel);
void 		   app_SrvLineTestStop(u8 Channel);
void 		   app_SrvNltCapTestStart(u8 Channel);
void 		   app_SrvNltCapTestStop(u8 Channel);
void		   app_SrvFxsRingTestStart(u8 Channel);
void		   app_SrvFxsRingTestStop(u8 Channel);
void		   app_SrvFxsStatusReq(u8 Channel);
void           appsrv_SetCallStateFilter(u8 u8_FilterType);
void 		   app_SrvFxsOpenLoopReq(u8 Channel);
void           app_ParamAreaGetBySegments(u32 u32_Pos, u32 u32_Length, u8 *pu8_Data, u32 packet_max_size);
void           app_ParamAreaSetBySegments(u32 u32_Pos, u32 u32_Length, u8 *pu8_Data, u16 packet_max_size);
E_CMBS_RC      app_ParamAreaGetFromFile(u8 *pu8_Data, u8 *pu8_path, u32 *pu32_fileSize);
void 		   app_SrvJEDECidGet();
void           app_SrvPing();
E_CMBS_RC 	   app_SrvSendTDMSlotConfiguration(u32 u32_TDMSlots);
E_CMBS_RC 	   app_SrvSendRAMDumpStoreCmd(void);

void           app_SrvSetValidDateTime(u8 u8_Valid);
u8             app_SrvGetValidDateTime(void);



#ifdef WIN32
DWORD app_ReconnectThread(LPVOID lpThreadParameter);
#elif __linux__
void* app_ReconnectThread(void *ptr);
#endif

typedef struct
{
    E_CMBS_AUDIO_CODEC e_Codec;
    ST_IE_AFE_ENDPOINTS_CONNECT st_AFEEndpoints;
    u32							u32_SlotMask;
    u32 						u32_ChannelID;
    u8							u8_InstanceNum;
    u8							u8_Resource;
	
}st_AFEConfiguration;

typedef struct
{
   E_CMBS_IE_TYPE e_IE;          /*! IE type */
   u8         u8_Value;     	/*! IE value*/
}ST_GPIO_Properties, * PST_GPIO_Properties;

typedef struct
{
   u32 u32_ULE_dect_size;
   u32 u32_ULE_size;
   u32 u32_FUN_size;
   u32 u32_Group_List_size;
   u32 u32_Group_Table_size;
   u32 u32_Broadcast_Conversion_Table_size;
}st_EEPROMHanSizeBackup;


void 		  app_SrvGPIOSet(PST_IE_GPIO_ID st_GPIOId, PST_GPIO_Properties pst_GPIOProp);
void 		  app_SrvGPIOGet(PST_IE_GPIO_ID st_GPIOId, PST_GPIO_Properties pst_GPIOProp);
void 		  app_SrvExtIntConfigure(PST_IE_GPIO_ID st_GpioId, PST_IE_INT_CONFIGURATION st_Config, u8 u8_IntNumber);
void 		  app_SrvExtIntEnable(u8 u8_IntNumber);
void 		  app_SrvExtIntDisable(u8 u8_IntNumber);
E_CMBS_RC 		  app_SrvLocateSuggest(u16 u16_Handsets);
E_CMBS_RC     app_SrvInfoSuggest(u8 u8_HsNumber, u8 * pu8_Data, u16 u16_DataLen);
#if defined( __cplusplus )
}
#endif

#define  CMBS_ALL_HS_MASK			0x03FF	// Maximal HS mask, the maximal mask of all possible configurations (e.g. NBS)
#define  CMBS_ALL_RELEVANT_HS_ID				0xFFFF	// "ID" for all handsets.

#endif	//APPSRV	_H
//*/
