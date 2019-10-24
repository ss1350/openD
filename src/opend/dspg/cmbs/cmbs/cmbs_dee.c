/*!
*	\file			cmbs_dee.c
*	\brief		DEE (external endpoint) functions			
*	\Author		stein 
*
*	@(#)	%filespec: cmbs_dee.c~DMZD53#9 %
*
*******************************************************************************/

#if defined(__arm)
# include "tclib.h"
# include "embedded.h"
#else
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
#endif

#include "cmbs_int.h"
typedef enum
{
  FXS_EV_UNDEF,
  FXS_EV_OFFHOOK, /*!<  OffHook event       */
  FXS_EV_ONHOOK,  /*!<  OnHook event        */
  FXS_EV_KEYPAD,  /*!<  Keypad event        */
  FXS_EV_OFFHOOK_INT, /* Off hook to make inter com call (call class)*/
  FXS_EV_OFFHOOK_EXT, /* Off hook to make External call (call class)*/
  FXS_EV_PARL_EXT_CALL,
  FXS_EV_PARL_INT_CALL,
  FXS_EV_CW_ACCEPT,
  FXS_EV_CALL_HOLD,
  FXS_EV_CALL_RESUME,
  FXS_EV_CALL_TRFR,
  FXS_EV_CALL_CONF,
  FXS_EV_CALL_RELEASE,
  FXS_EV_CALL_TOGGLE,
  FXS_EV_HOST_CONFIG,
  FXS_EV_REG_FXS,
  FXS_EV_MAX
} FXS_EV;

typedef enum
{
    E_TAPP_AUD_CHANNEL_MODE_UNDEF,          /*!< channel is not used */
    E_TAPP_AUD_CHANNEL_MODE_PCM_16_NB,      /*!< channel is used as PCM 16 bit linear narrow-band 8 KHz*/
    E_TAPP_AUD_CHANNEL_MODE_PCM_16_WB,      /*!< channel is used as PCM 16 bit linear wide-band 16 KHz  */
    E_TAPP_AUD_CHANNEL_MODE_PCM_8_ALAW,     /*!< channel is used as PCM 8 bit a-law 8 KHz */
    E_TAPP_AUD_CHANNEL_MODE_PCM_8_ULAW,     /*!< channel is used as PCM 8 bit u-law 8 KHz */
    E_TAPP_AUD_CHANNEL_MODE_PCM_8,          /*!< channel is used as PCM 8 bit linear 8 KHz */
    E_TAPP_AUD_CHANNEL_MODE_WB_16_ALAW,     /*!< channel is used as PCM 8 bit a-law 16 KHz */
    E_TAPP_AUD_CHANNEL_MODE_WB_16_ULAW,		/*!< channel is used as PCM 8 bit u-law 16 KHz */
    E_TAPP_AUD_CHANNEL_MODE_MAX             /*!< channel mode max definition */
} E_TAPP_AUD_CHANNEL_MODE;

u32	cmbs_dee_CallInstanceNew( void * pv_AppRefHandle )
{
	UNUSED_PARAMETER( pv_AppRefHandle );

	return ++g_CMBSInstance.u32_CallInstanceCount;
}

E_CMBS_RC	cmbs_dee_CallEstablish( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_ESTABLISH, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallProgress( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_PROGRESS, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallAnswer( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_ANSWER, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallRelease( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_RELEASE, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallReleaseComplete( void * pv_AppRefHandle, u32 u32_CallInstance )
{
	PST_CFR_IE_LIST p_List = cmbs_api_ie_GetList();

	UNUSED_PARAMETER( pv_AppRefHandle );

	cmbs_api_ie_CallInstanceAdd( p_List, u32_CallInstance );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_RELEASECOMPLETE, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallInbandInfo(void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_INBANDINFO, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallMediaOffer( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_MEDIA_OFFER, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallMediaOfferRes( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_MEDIA_OFFER_RES, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_HsCodecCfmFailed( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_HS_CODEC_CFM_FAILED, p_List->pu8_Buffer, p_List->u16_CurSize );
}

E_CMBS_RC	cmbs_dee_CallHold( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_HOLD, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallResume( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_RESUME, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallHoldRes( void * pv_AppRefHandle, u32 u32_CallInstance, u16 u16_HsNum, E_CMBS_RESPONSE e_Response )
{
	ST_IE_RESPONSE   st_Response;
	PST_CFR_IE_LIST  pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );
	ALLOCATE_IE_LIST(pv_RefIEList);

	st_Response.e_Response = e_Response;

	// Add call Instance IE
	cmbs_api_ie_CallInstanceAdd( (void*)pv_RefIEList, u32_CallInstance );
	cmbs_api_ie_ResponseAdd( (void*)pv_RefIEList, &st_Response );
	cmbs_api_ie_HandsetsAdd( (void*)pv_RefIEList, u16_HsNum);

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_HOLD_RES, pv_RefIEList->pu8_Buffer, pv_RefIEList->u16_CurSize );
}

E_CMBS_RC	cmbs_dee_CallResumeRes( void * pv_AppRefHandle, u32 u32_CallInstance, u16 u16_HsNum, E_CMBS_RESPONSE e_Response )
{
	ST_IE_RESPONSE   st_Response;
	PST_CFR_IE_LIST  pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );
	ALLOCATE_IE_LIST(pv_RefIEList);

	st_Response.e_Response = e_Response;

	// Add call Instance IE
	cmbs_api_ie_CallInstanceAdd( (void*)pv_RefIEList, u32_CallInstance );
	cmbs_api_ie_ResponseAdd( (void*)pv_RefIEList, &st_Response );
	cmbs_api_ie_HandsetsAdd( (void*)pv_RefIEList, u16_HsNum);

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_RESUME_RES, pv_RefIEList->pu8_Buffer, pv_RefIEList->u16_CurSize );
}

E_CMBS_RC	cmbs_dee_HandsetLinkRelease(u8 Handset)
{
	PST_CFR_IE_LIST  pv_RefIEList;

	ALLOCATE_IE_LIST(pv_RefIEList);

	cmbs_api_ie_HsNumberAdd((void*)pv_RefIEList, Handset);
	return cmbs_int_EventSend( CMBS_EV_DEE_HANDSET_LINK_RELEASE , pv_RefIEList->pu8_Buffer, pv_RefIEList->u16_CurSize );
}

E_CMBS_RC	cmbs_dee_MergeCalls( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_MERGE_CALLS, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_EmergencyCallRelease( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_EMERGENCY_RELEASE, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dee_CallScreening( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_SCREENING, p_List->pu8_Buffer, p_List->u16_CurSize );  
}


E_CMBS_RC	cmbs_dee_CallDeflection( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_DEFLECTION, p_List->pu8_Buffer, p_List->u16_CurSize );  
}


E_CMBS_RC	cmbs_dee_CallDeflectionRes( void * pv_AppRefHandle, u32 u32_CallInstance, u16 u16_HsNum, E_CMBS_RESPONSE e_Response )
{
	ST_IE_RESPONSE   st_Response;
	PST_CFR_IE_LIST  pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );
	ALLOCATE_IE_LIST(pv_RefIEList);

	st_Response.e_Response = e_Response;

	// Add call Instance IE
	cmbs_api_ie_CallInstanceAdd( (void*)pv_RefIEList, u32_CallInstance );
	cmbs_api_ie_ResponseAdd( (void*)pv_RefIEList, &st_Response );
	cmbs_api_ie_HandsetsAdd( (void*)pv_RefIEList, u16_HsNum);

	return cmbs_int_EventSend( CMBS_EV_DEE_CALL_DEFLECTION_RES, pv_RefIEList->pu8_Buffer, pv_RefIEList->u16_CurSize );
}



//*/
