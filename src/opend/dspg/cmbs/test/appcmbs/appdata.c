/*!
*  \file       appdata.c
*	\brief		handles CAT-iq data functioality
*	\Author		stein
*
*	@(#)	%filespec: appdata.c~10 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if ! defined ( WIN32 )
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <signal.h>
#endif

#include "cmbs_int.h"
#include "cmbs_api.h"
#include "cfr_ie.h"
#include "cfr_mssg.h"

#include "appcmbs.h"
#include "appmsgparser.h"

extern u16        app_HandsetMap( char * psz_Handsets );



E_CMBS_RC         app_OnDataSessionOpen(void *pvAppRefHandle, void *pv_Param)
{
    ST_IE_RESPONSE st_Response;
    ST_IE_DATA st_Data;
    void *pv_IE = NULL;
    u16 u16_IE;
    u16 u16_SessionId = 0;
	PST_CFR_IE_LIST p_List = (PST_CFR_IE_LIST)cmbs_api_ie_GetList();
	st_Data.u16_DataLen = 0;    

if(pv_Param)
	{
        cmbs_api_ie_GetFirst(pv_Param, &pv_IE, &u16_IE);
        while (pv_IE != NULL)
        {
            switch (u16_IE)
            {
				case CMBS_IE_DATA:
					cmbs_api_ie_DataGet(pv_IE, &st_Data);
					break;
					
				case CMBS_IE_DATA_SESSION_ID:
					cmbs_api_ie_ShortValueGet( pv_IE, &u16_SessionId, CMBS_IE_DATA_SESSION_ID );
					break;
            }
            cmbs_api_ie_GetNext(pv_Param, &pv_IE, &u16_IE);
        }
	}


	if (st_Data.u16_DataLen > 0)
	{
		if (((st_Data.pu8_Data[1] & 0x0f) == 0x00) && (st_Data.pu8_Data[6] == 0x80) && (st_Data.pu8_Data[7] == 0x01))
		{//No IWU_ATTR_PROFSUBTYPE_GMEP  & Application os 0x80 01, Simple Data call not SOUTA session
		       cmbs_api_ie_ShortValueAdd( p_List, u16_SessionId, CMBS_IE_DATA_SESSION_ID );
		       st_Response.e_Response = CMBS_RESPONSE_OK;
			cmbs_api_ie_ResponseAdd( p_List, &st_Response );
			cmbs_int_EventSend( CMBS_EV_DSR_HS_DATA_SESSION_OPEN_RES, p_List->pu8_Buffer, p_List->u16_CurSize );
			return CMBS_RC_OK;
		}
	}
	return CMBS_RC_OK;
}

E_CMBS_RC         app_DataSessionOpen( char * psz_Handset )
{
   ST_DATA_SESSION_TYPE
                  st_DataSessionType;
   u16            u16_Handset = app_HandsetMap( psz_Handset );

   st_DataSessionType.e_ChannelType = CMBS_DATA_CHANNEL_IWU;   
   st_DataSessionType.e_ServiceType = CMBS_DATA_SERVICE_TRANSPARENT;
   
   return cmbs_dsr_hs_DataSessionOpen( g_cmbsappl.pv_CMBSRef,
                                       &st_DataSessionType,
                                       u16_Handset );
}


E_CMBS_RC         app_DataSend( u16 u16_SessionId, ST_IE_DATA * pst_Data )
{   
   return cmbs_dsr_hs_DataSend( g_cmbsappl.pv_CMBSRef,
                                u16_SessionId, pst_Data->pu8_Data, pst_Data->u16_DataLen );
}


E_CMBS_RC         app_DataSessionClose( u16 u16_SessionId )
{
   return cmbs_dsr_hs_DataSessionClose( g_cmbsappl.pv_CMBSRef,
                                        u16_SessionId );
}

                                        
//		========== app_DataEntity ===========
/*!
		\brief		 CMBS entity to handle response information from target side
		\param[in]	 pv_AppRef		 application reference
		\param[in]	 e_EventID		 received CMBS event
		\param[in]	 pv_EventData	 pointer to IE list
		\return	 	 <int>

*/
int app_DataEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData )
{
   UNUSED_PARAMETER( pv_AppRef );
   UNUSED_PARAMETER( pv_EventData );
if( e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_OPEN  )
{//Call New Fn here
      app_OnDataSessionOpen(pv_AppRef,pv_EventData);
	return TRUE;
}else   if(  e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_OPEN_RES  ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_CLOSE     ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SESSION_CLOSE_RES ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SEND              ||
       e_EventID == CMBS_EV_DSR_HS_DATA_SEND_RES )
   {
      return TRUE;
   }
   else
      return FALSE;   
}
//*/
