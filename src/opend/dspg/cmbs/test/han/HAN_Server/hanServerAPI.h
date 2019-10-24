#if	!defined( HAN_SERVER_H )
#define	HAN_SERVER_H

#if defined( __cplusplus )
extern "C"
{
#endif

/**********************************************************************
*  Protocol - CB Functions for each Command received by Remote
***********************************************************************/

typedef u32 t_fptr_ProxyCB(const char *pCommand);

typedef struct {
    const char *pCmd;
    t_fptr_ProxyCB *pProxyCB;
} t_CmdCb;

// Send HAN message from App. to all connected HAN clients
int p_HanServer_AppSendMessage(const char *pMessage);

// Register callback table for APP Service HAN messages
int p_HanServer_RegAppMessageCallbackTbl(const char *pServiceName, const t_CmdCb *tbl);

// parser functions
u32 p_HanServer_GetIntFieldByStr(const char *pCommand, u32 u32_fieldId, s32 *ps32_Value);
u32 p_HanServer_GetStrFieldByStr(const char *pCommand, u32 u32_fieldId, char **ppData);

u32 p_HanServer_Dummy(const char *pCommand);

#if defined( __cplusplus )
}
#endif

#endif	//	HAN_SERVER_H


