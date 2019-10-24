/*!
*	\file			cmbs_suota.c
*	\brief
*	\Author		stein
*
*	@(#)	%filespec: cmbs_suota.c~7 %
*
*******************************************************************************/
#if defined(__arm)
    #include "tclib.h"
    #include "embedded.h"
#else
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
#endif

#include "cmbs_int.h"

#define SUOTA_APPLICATION_ID	0x437

//effi
E_CMBS_RC cmbs_dsr_suota_Init(void *pv_AppRefHandle)
{
	PST_CFR_IE_LIST p_List;
	PST_CFR_IE_LIST p_List2;
	E_CMBS_EVENT_ID e_EventID;
	ST_IE_DATA      st_Data;
	char Buffer[4];

    UNUSED_PARAMETER( pv_AppRefHandle );
    
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	e_EventID = CMBS_EV_DSR_SUOTA_REG_CPLANE_CB;

	// Add IEs
	cmbs_api_ie_SuotaAppIdAdd(p_List, SUOTA_APPLICATION_ID);
	cmbs_api_ie_SuotaSessionIdAdd(p_List, 0);
	
	st_Data.u16_DataLen = 0;
	st_Data.pu8_Data = 0;
	cmbs_api_ie_DataAdd(p_List, &st_Data);
	
	cmbs_int_EventSend(e_EventID, p_List->pu8_Buffer, p_List->u16_CurSize);

	// app cb
	p_List2 = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	e_EventID = CMBS_EV_DSR_SUOTA_REG_APP_CB;
	memset(Buffer,0,4);

	// Add IEs
	cmbs_api_ie_SuotaAppIdAdd(p_List, SUOTA_APPLICATION_ID);
	cmbs_api_ie_SuotaSessionIdAdd(p_List, 0);
	
	st_Data.u16_DataLen = 4;
	st_Data.pu8_Data = (u8*)Buffer;
	cmbs_api_ie_DataAdd(p_List, &st_Data);
	
	cmbs_int_EventSend(e_EventID, p_List2->pu8_Buffer, p_List2->u16_CurSize);
	return 0;
}

E_CMBS_RC cmbs_dsr_suota_SendHSVersionAvail (void *pv_AppRefHandle, ST_SUOTA_UPGRADE_DETAILS  st_HSVerAvail, u16 u16_Handset, ST_VERSION_BUFFER* pst_SwVersion, u16 u16_RequestId)
{
	PST_CFR_IE_LIST    p_List;
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
    
    UNUSED_PARAMETER( pv_AppRefHandle );

	cmbs_api_ie_ShortValueAdd( p_List, u16_Handset, CMBS_IE_HANDSETS);
	cmbs_api_ie_VersionAvailAdd(p_List, &st_HSVerAvail);
	cmbs_api_ie_VersionBufferAdd( p_List, pst_SwVersion);
    cmbs_api_ie_RequestIdAdd( p_List, u16_RequestId );

	return cmbs_int_EventSend( CMBS_EV_DSR_SUOTA_SEND_VERS_AVAIL, p_List->pu8_Buffer, p_List->u16_CurSize );
}

E_CMBS_RC cmbs_dsr_suota_SendSWUpdateInd ( void *pv_AppRefHandle, u16 u16_Handset, E_SUOTA_SU_SubType enSubType, u16 u16_RequestId )
{
	PST_CFR_IE_LIST    p_List;

    UNUSED_PARAMETER( pv_AppRefHandle );
	
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	cmbs_api_ie_ShortValueAdd( p_List, u16_Handset, CMBS_IE_HANDSETS);
	cmbs_api_ie_ByteValueAdd(p_List,enSubType,CMBS_IE_SU_SUBTYPE);
    cmbs_api_ie_RequestIdAdd( p_List, u16_RequestId );
	return cmbs_int_EventSend( CMBS_EV_DSR_SUOTA_SEND_SW_UPD_IND, p_List->pu8_Buffer, p_List->u16_CurSize );
}

E_CMBS_RC cmbs_dsr_suota_SendURL ( void *pv_AppRefHandle, u16 u16_Handset, u8   u8_URLToFollow, ST_URL_BUFFER* pst_Url, u16 u16_RequestId )
{
	PST_CFR_IE_LIST    p_List;
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    UNUSED_PARAMETER( pv_AppRefHandle );

	cmbs_api_ie_ShortValueAdd( p_List, u16_Handset, CMBS_IE_HANDSETS);
	cmbs_api_ie_ByteValueAdd(p_List,u8_URLToFollow,CMBS_IE_NUM_OF_URLS);
	cmbs_api_ie_UrlAdd(p_List,pst_Url);
    cmbs_api_ie_RequestIdAdd( p_List, u16_RequestId );

	return cmbs_int_EventSend( CMBS_EV_DSR_SUOTA_SEND_URL, p_List->pu8_Buffer, p_List->u16_CurSize );
}

E_CMBS_RC cmbs_dsr_suota_SendNack ( void *pv_AppRefHandle, u16 u16_Handset, E_SUOTA_RejectReason RejectReason, u16 u16_RequestId )
{
	PST_CFR_IE_LIST    p_List;
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    UNUSED_PARAMETER( pv_AppRefHandle );

	cmbs_api_ie_ShortValueAdd( p_List, u16_Handset, CMBS_IE_HANDSETS);
	cmbs_api_ie_ByteValueAdd(p_List,RejectReason,CMBS_IE_REJECT_REASON);
    cmbs_api_ie_RequestIdAdd( p_List, u16_RequestId );

	return cmbs_int_EventSend( CMBS_EV_DSR_SUOTA_SEND_NACK, p_List->pu8_Buffer, p_List->u16_CurSize );
}



E_CMBS_RC cmbs_dsr_suota_Unreg_App_CB(void)
{
    PST_CFR_IE_LIST    p_List;
    p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    return cmbs_int_EventSend(CMBS_EV_DSR_SUOTA_UNREG_APP_CB, p_List->pu8_Buffer, p_List->u16_CurSize); 
}


E_CMBS_RC cmbs_dsr_suota_Session_Close ( void *pv_AppRefHandle, u32 u32_SessionId )
{
    PST_CFR_IE_LIST    p_List;
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    UNUSED_PARAMETER( pv_AppRefHandle );

	cmbs_api_ie_SuotaSessionIdAdd(p_List, u32_SessionId );
	
	return cmbs_int_EventSend( CMBS_EV_DSR_SUOTA_SESSION_CLOSE, p_List->pu8_Buffer, p_List->u16_CurSize );
}


E_CMBS_RC cmbs_dsr_suota_DataSend (void *pv_AppRefHandle, u32 u32_appId, u32 u32_SessionId,
								   char *pSdu, u32 u32_SduLength,u16 u16_RequestId)
{
	PST_CFR_IE_LIST    	p_List;
	ST_IE_DATA      	st_Data;
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    UNUSED_PARAMETER( pv_AppRefHandle );
	cmbs_api_ie_RequestIdAdd( p_List, u16_RequestId );
    cmbs_api_ie_SuotaAppIdAdd(p_List, u32_appId);
	cmbs_api_ie_SuotaSessionIdAdd(p_List, u32_SessionId);
	st_Data.u16_DataLen = u32_SduLength;
	st_Data.pu8_Data    = (u8*)pSdu;
	cmbs_api_ie_DataAdd( p_List, &st_Data );
	return cmbs_int_EventSend(CMBS_EV_DSR_SUOTA_DATA_SEND, p_List->pu8_Buffer, p_List->u16_CurSize);
}

E_CMBS_RC cmbs_dsr_suota_ExtendedDataSend (void *pv_AppRefHandle, u32 u32_appId, u32 u32_SessionId,
								   char *pSdu, u32 u32_SduLength,u16 u16_RequestId, PST_IE_SUOTA_FILE_INFO pst_FileInfo)
{
	PST_CFR_IE_LIST    	p_List;
	ST_IE_DATA      	st_Data;
	p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();

    UNUSED_PARAMETER( pv_AppRefHandle );
	cmbs_api_ie_RequestIdAdd( p_List, u16_RequestId );
    cmbs_api_ie_SuotaAppIdAdd(p_List, u32_appId);
	cmbs_api_ie_SuotaSessionIdAdd(p_List, u32_SessionId);
	cmbs_api_ie_SuotaFileInfoAdd(p_List, pst_FileInfo);
	st_Data.u16_DataLen = u32_SduLength;
	st_Data.pu8_Data    = (u8*)pSdu;
	cmbs_api_ie_DataAdd( p_List, &st_Data );
	return cmbs_int_EventSend(CMBS_EV_DSR_SUOTA_DATA_SEND, p_List->pu8_Buffer, p_List->u16_CurSize);
}


/*---------[End Of File]---------------------------------------------------------------------------------------------------------------------------*/
