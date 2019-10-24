/*!
*	\file			cmbs_api.c
*	\brief			API Maintenance functions
*	\Author			CMBS Team 
*
*	@(#)	%filespec: cmbs_api.c~DMZD53#7 %
*
*******************************************************************************/
#include <string.h>
#include "cmbs_int.h"

#define TARGET_ANSWER_TIMEOUT	10000
ST_CB_LOG_BUFFER g_LogBufferCb;

#ifdef __linux
extern void _cmbs_int_MsgQDestroy(int nMsgQId);
extern int _cmbs_int_MsgQCreate(void);
#endif

/*****************************************************************************
* API Maintenance functions
*****************************************************************************/
PST_CB_LOG_BUFFER cmbs_api_get_LogBufferCb(void)
{
	return &g_LogBufferCb;
}


E_CMBS_RC cmbs_api_Init( E_CMBS_API_MODE e_Mode, ST_CMBS_DEV * pst_DevCtl, ST_CMBS_DEV * pst_DevMedia )
{
#if !defined ( CMBS_API_TARGET )
  E_CMBS_RC ReturnCode;
#endif

	// initialize host environment
	if(cmbs_int_EnvCreate( e_Mode, pst_DevCtl, pst_DevMedia ) != CMBS_RC_OK)
		return CMBS_RC_ERROR_GENERAL;

	memcpy(&g_CMBSInstance.st_ApplSlot.pFnCbLogBuffer, cmbs_api_get_LogBufferCb(), sizeof(ST_CB_LOG_BUFFER));

#if !defined ( CMBS_API_TARGET )
	// establish communication with the module
	cmbs_int_SendHello(pst_DevCtl, pst_DevMedia);
	// wait for target answer
	ReturnCode = cmbs_int_WaitForResponse(TARGET_ANSWER_TIMEOUT);
    
    if (ReturnCode == CMBS_RC_OK)
    {
        if (g_CMBSInstance.u16_TargetVersion != 0x0001)
        {
            // try to get Target capabilities (if no response, it means target does not support this message)
            cmbs_int_cmd_SendCapablities(); 
            cmbs_int_WaitForResponse(TARGET_ANSWER_TIMEOUT);
        }
    }
    return ReturnCode;
#else
	return CMBS_RC_OK;
#endif
}

void cmbs_api_UnInit( void )
{   
	cmbs_int_EnvDestroy();
}

void* cmbs_api_RegisterCb( void * pv_AppRef, PFN_CMBS_API_CB pfn_api_Cb, u16 u16_bcdVersion )
{   
	return cmbs_int_RegisterCb( pv_AppRef, pfn_api_Cb, u16_bcdVersion );
}

void cmbs_api_UnregisterCb( void * pv_AppRefHandle )
{
	cmbs_int_UnregisterCb( pv_AppRefHandle );
}

u16 cmbs_api_ModuleVersionGet( void )
{
	return cmbs_int_ModuleVersionGet();
}

u16 cmbs_api_ModuleVersionBuildGet( void )
{
	return cmbs_int_ModuleVersionBuildGet();
}

#ifdef __linux
void  cmbs_api_MsgQDestroy(int nMsgQId)
{
	  _cmbs_int_MsgQDestroy(nMsgQId);
}
int  cmbs_api_MsgQCreate(void)
{
	  return _cmbs_int_MsgQCreate();
}
#elif WIN32
void  cmbs_api_MsgQDestroy(PST_ICOM_ENTRY pMsgQId)
{
 	cfr_MQueueDestroy ( pMsgQId);
}		
PST_ICOM_ENTRY  cmbs_api_MsgQCreate(u32 u32_ThreadID, u32 u32_Timeout)
{
	return cfr_MQueueCreate(u32_ThreadID, u32_Timeout);
}
#endif

void cmbs_api_RegisterLogBufferCb(void* pv_AppRef, PST_CB_LOG_BUFFER pfn_log_buffer_Cb)
{
	cmbs_int_RegisterLogBufferCb(pv_AppRef, pfn_log_buffer_Cb);
}
#if !defined( CMBS_API_TARGET )
void cmbs_api_ParseDectMsg(u8 * buff, u8 u8_ILen, u8 u8_HandsetNumber)
{
	cmbs_int_ParseDectMsg(buff, u8_ILen, u8_HandsetNumber);
}
#endif //#if !defined( CMBS_API_TARGET )

//*/
