#include "appCallRouter.h"
#include "ListsApp.h"
#include "cmbs_voipline_media5.h"
#include "string.h"
#include "stdlib.h"

#define APP_ROUTER_INFO_PRINT(format, ...) 						tcx_WriteLog(ROUTER_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define APP_ROUTER_ERROR_PRINT(format, ...) 					tcx_WriteLog(ROUTER_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )



ST_APP_FNC_TABLE g_st_AppTable[MAX_NUM_OF_APPS];
u32 g_u32_AppCallIDMap[APPCALL_CALLOBJ_MAX];

ST_CMBS_ROUTE_TABLE g_st_cmbs_route_table[MAX_NUM_OF_APPS] = {
#ifdef MEDIA5_SIP_SUPPORT
														{0,	0, 0, 	"H*",	NULL,			NULL		}
#endif /*MEDIA5_SIP_SUPPORT*/
#ifdef PHONER_SUPPORT
														{1, 10, 10, 	"H*",	NULL,			NULL 	},
#endif /*PHONER_SUPPORT*/
#ifdef SKYPE_SUPPORT
														{2, 11, 11, 	"H*",	NULL,			NULL 	}
#endif /*SKYPE_SUPPORT*/
														/*{1,  1,  2, 	"H1",	NULL,			NULL		},
														{2,	 3,	 9,		NULL,	"D*",			NULL	},
														{3,  -1, -1, 	NULL,	"D0001U001",	NULL	},
														{4,  -1, -1, 	NULL,	NULL,			"911"		}	*/
#ifdef WIN32
														{-1,	-1, -1, 	NULL,	NULL,			NULL		}
#endif
														};



//		========== CallRouter_Init ===========
/*!
    \brief				Callback function of all registered apps being called
    \param[in]			None
    \return				None
*/


void AppCallRouter_Init (void)
{
	memset(g_u32_AppCallIDMap, -1, sizeof(g_u32_AppCallIDMap));	
	memset(g_st_AppTable, 0, sizeof(g_st_AppTable));	
}


u8 AppCallRouter_AppInit (u32 u32_AppId, ST_APP_FNC_TABLE *pst_AppTable)
{
	int i;
	
	
	for (i=0;i<MAX_NUM_OF_APPS;i++)
	{
		if ((g_st_cmbs_route_table[i].u32_AppId == u32_AppId) && (g_st_cmbs_route_table[i].u32_AppId < MAX_NUM_OF_APPS))
		{
			APP_ROUTER_INFO_PRINT("g_st_AppTable %d 0x%x\n",i,g_st_AppTable[i]);
			memcpy (&g_st_AppTable[g_st_cmbs_route_table[i].u32_AppId], pst_AppTable, sizeof(ST_APP_FNC_TABLE));		
			return EXTAPP_RC_OK;
		}
	}
	return EXTAPP_RC_FAIL;
}



//		========== CallRouter_AppSelection ===========
/*!
    \brief				Selects matching app based on the condition provided in the ST_CMBS_ROUTE_TABLE
    \param[in]			u32_CallID         CallID from CMBS
    \param[in]			psz_CalledId		       pointer parameter buffer
    \param[in]			psz_CallerHandset		 pointer parameter buffer
    \param[in]			u32_LineId				expected lineID
    \return				< EXTAPP_RC >
*/

EXTAPP_RC AppCallRouter_AppSelection(IN u32 u32_CallID, IN const char* psz_CalledId, IN const char* psz_CallerHandset, IN u32 u32_LineId)
{
	int i=0;
	char input[50];
	int res=-1;
	printf("\n\n CallRouter_AppSelection \n");
	for (i=0;i<MAX_NUM_OF_APPS;i++)
	{
		
	  	if (NULL == g_st_AppTable[g_st_cmbs_route_table[i].u32_AppId].fncSetupCall )
			continue;
		
		APP_ROUTER_INFO_PRINT("\n %s %d HS/D %s line %d",__FUNCTION__,i,psz_CallerHandset,u32_LineId);
		
		if ( ((g_st_cmbs_route_table[i].u32_StartLineId != -1) && (g_st_cmbs_route_table[i].u32_MaxLineId != -1)) && ((u32_LineId >= g_st_cmbs_route_table[i].u32_StartLineId) && (u32_LineId<=g_st_cmbs_route_table[i].u32_MaxLineId)) )
		{
			APP_ROUTER_INFO_PRINT("\n -> line MAtching AppID %d %d\n",g_u32_AppCallIDMap[u32_CallID],g_st_cmbs_route_table[i].u32_AppId);

			res = AppCallRouter_MakeCall(u32_CallID , g_st_cmbs_route_table[i].u32_AppId);
			if(res != EXTAPP_RC_OK)
				continue;
			else
			{
				g_u32_AppCallIDMap[u32_CallID] = g_st_cmbs_route_table[i].u32_AppId;
				return EXTAPP_RC_OK;
			}
		}
		
		if(psz_CallerHandset[0] == 'D')
		{
			if (g_st_cmbs_route_table[i].psz_DeviceId && 
				((strcmp("D*",g_st_cmbs_route_table[i].psz_DeviceId) == 0) || (strcmp(psz_CallerHandset,g_st_cmbs_route_table[i].psz_DeviceId) == 0)))
			{
				APP_ROUTER_INFO_PRINT("\n -> Dev MAtching AppID %d %d\n",g_u32_AppCallIDMap[u32_CallID],g_st_cmbs_route_table[i].u32_AppId);

				res = AppCallRouter_MakeCall(u32_CallID,g_st_cmbs_route_table[i].u32_AppId);
				if(res != EXTAPP_RC_OK)
					continue;
				else 
				{
					g_u32_AppCallIDMap[u32_CallID] = g_st_cmbs_route_table[i].u32_AppId;
					return EXTAPP_RC_OK;
				}
			}
		}
		else	
		{
			sprintf(input,"H%s",psz_CallerHandset);
			if(g_st_cmbs_route_table[i].psz_HandsetNum &&
				((strcmp("H*",g_st_cmbs_route_table[i].psz_HandsetNum) == 0) || (strcmp(g_st_cmbs_route_table[i].psz_HandsetNum,input) == 0)))
			{
				APP_ROUTER_INFO_PRINT("\n -> HS MAtching AppID %d %d\n",g_u32_AppCallIDMap[u32_CallID],g_st_cmbs_route_table[i].u32_AppId);
		
				res = AppCallRouter_MakeCall(u32_CallID,g_st_cmbs_route_table[i].u32_AppId);
				if(res != EXTAPP_RC_OK)
					continue;
				else 
				{
					g_u32_AppCallIDMap[u32_CallID] = g_st_cmbs_route_table[i].u32_AppId;
					return EXTAPP_RC_OK;
				}
			}
		}

		if(g_st_cmbs_route_table[i].psz_Digits && strcmp(psz_CalledId, g_st_cmbs_route_table[i].psz_Digits) == 0 )
		{
			APP_ROUTER_INFO_PRINT("\n -> Dig MAtching AppID %d %d\n",g_u32_AppCallIDMap[u32_CallID],g_st_cmbs_route_table[i].u32_AppId);

			res = AppCallRouter_MakeCall(u32_CallID,g_st_cmbs_route_table[i].u32_AppId);
			if(res != EXTAPP_RC_OK)
				continue;
			else 
			{
				g_u32_AppCallIDMap[u32_CallID] = g_st_cmbs_route_table[i].u32_AppId;
				return EXTAPP_RC_OK;
			}
		}
		
	}
	APP_ROUTER_ERROR_PRINT("\n -> NOT MAtching with App cond, call id %d\n", u32_CallID);

	g_u32_AppCallIDMap[u32_CallID] = -1;
	return EXTAPP_RC_FAIL;
	
}


//		========== AppCallRouter_MakeCall ===========
/*!
    \brief				Calls the Setup Call API of the selected App
    \param[in]			u32_CallID         CallID from CMBS
    \return				< EXTVOIP_RC >
*/


EXTVOIP_RC AppCallRouter_MakeCall(u32 u32_CallID, u32 u32_AppId)
{	
	PST_CALL_OBJ pstCall;
	EXTVOIP_RC rc = CMBS_RC_OK;
	EXTVOIP_CODEC codecs[CMBS_AUDIO_CODEC_MAX] = {0};


	if ((u32_AppId == -1) || (u32_AppId >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;

	// get call object
	pstCall = _appcall_CallObjGetById(u32_CallID);
	if(!pstCall)
		return EXTVOIP_RC_FAIL;
	
	// make codecs list
	if(pstCall->u8_CodecsLength)
	{
		int i = 0;
		for(i=0;i<pstCall->u8_CodecsLength;i++)
			codecs[i] = (EXTVOIP_CODEC)pstCall->pu8_CodecsList[i];
	}

	
	APP_ROUTER_INFO_PRINT("\n CallRouter_MakeCall %d 0x%x\n",u32_AppId,(unsigned int *)(g_st_AppTable[u32_AppId]).fncSetupCall);
	
	// call SetupCall function
	if((g_st_AppTable[u32_AppId]).fncSetupCall)
		rc = ((g_st_AppTable[u32_AppId]).fncSetupCall)(u32_CallID, pstCall->ch_CalledID, pstCall->ch_CallerID, pstCall->u8_LineId, codecs, pstCall->u8_CodecsLength);
	
	if(rc != EXTVOIP_RC_OK)
		return EXTVOIP_RC_FAIL;

	// set audio channel
	if(g_st_AppTable[u32_AppId].fncSetAudioChannel && (pstCall->u32_ChannelID != APPCALL_NO_MEDIA_CHANNEL))
		(g_st_AppTable[u32_AppId].fncSetAudioChannel)(u32_CallID, pstCall->u32_ChannelID);
	
	return EXTVOIP_RC_OK;
}

