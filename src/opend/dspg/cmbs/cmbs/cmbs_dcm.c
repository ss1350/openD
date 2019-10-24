/*!
*	\file			cmbs_dcm.c
*	\brief			API cakk media functions
*	\Author			CMBS Team 
*
*	@(#)	%filespec: cmbs_dcm.c~DMZD53#7 %
*
*******************************************************************************/


#include "cmbs_int.h"


E_CMBS_RC cmbs_dcm_CallTransferRes(PST_IE_CALLTRANSFERREQ pst_TrfReq, PST_IE_RESPONSE pst_Resp)
{
	PST_CFR_IE_LIST p_List;
	ALLOCATE_IE_LIST(p_List);

	cmbs_api_ie_CallTransferReqAdd((void*)p_List, pst_TrfReq);
    cmbs_api_ie_ResponseAdd((void*)p_List, pst_Resp);

	return cmbs_int_EventSend( CMBS_EV_DCM_CALL_TRANSFER_RES, p_List->pu8_Buffer, p_List->u16_CurSize );
}

E_CMBS_RC cmbs_dcm_CallConferenceRes(PST_IE_CALLTRANSFERREQ pst_TrfReq, PST_IE_RESPONSE pst_Resp)
{
	PST_CFR_IE_LIST p_List;
	ALLOCATE_IE_LIST(p_List);
		cmbs_api_ie_CallTransferReqAdd((void*)p_List, pst_TrfReq);
    cmbs_api_ie_ResponseAdd((void*)p_List, pst_Resp);


	return cmbs_int_EventSend( CMBS_EV_DCM_CALL_CONFERENCE_RES, p_List->pu8_Buffer, p_List->u16_CurSize );
}

