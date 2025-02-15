/*!
*	\file		cmbs_dem.c
*	\brief		DEM (external media) functions	
*	\Author		stein 
*
*	@(#)	%filespec: cmbs_dem.c~DMZD53#4 %
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
#include "cfr_ie.h"
#include "cfr_debug.h"

E_CMBS_RC	cmbs_dem_ChannelStart( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEM_CHANNEL_START, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dem_ChannelStop( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEM_CHANNEL_STOP, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dem_ChannelInternalConnect( void * pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEM_CHANNEL_INTERNAL_CONNECT, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dem_ToneStart( void *pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEM_TONE_START, p_List->pu8_Buffer, p_List->u16_CurSize );  
}


E_CMBS_RC	cmbs_dem_ToneStop( void *pv_AppRefHandle, void * pv_RefIEList )
{
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

	UNUSED_PARAMETER( pv_AppRefHandle );

	return cmbs_int_EventSend( CMBS_EV_DEM_TONE_STOP, p_List->pu8_Buffer, p_List->u16_CurSize );  
}

E_CMBS_RC	cmbs_dem_TDMLoopbackStart( void *pv_AppRefHandle, void * pv_RefIEList )
{
    PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

    UNUSED_PARAMETER( pv_AppRefHandle );

    return cmbs_int_EventSend( CMBS_EV_DEM_TDM_LOOPBACK_START, p_List->pu8_Buffer, p_List->u16_CurSize );
}

E_CMBS_RC	cmbs_dem_TDMLoopbackStop( void *pv_AppRefHandle, void * pv_RefIEList )
{
    PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)pv_RefIEList;

    UNUSED_PARAMETER( pv_AppRefHandle );

    return cmbs_int_EventSend( CMBS_EV_DEM_TDM_LOOPBACK_STOP, p_List->pu8_Buffer, p_List->u16_CurSize );
}

E_CMBS_RC cmbs_dem_ConfigTDMSlots(void *pv_AppRefHandle, u32 u32_TDMSlots)
{
	PST_CFR_IE_LIST p_List;

	UNUSED_PARAMETER( pv_AppRefHandle );

	ALLOCATE_IE_LIST(p_List);

	cmbs_api_ie_TDMSlotsAdd( (void*)p_List, u32_TDMSlots );

	return cmbs_int_EventSend( CMBS_EV_DEM_CONFIG_TDM_SLOTS, p_List->pu8_Buffer, p_List->u16_CurSize );
}

//*/
