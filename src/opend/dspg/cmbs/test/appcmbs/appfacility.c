/*!
*  \file       appfacility.c
*	\brief		handles CAT-iq facilities functionality
*	\Author		stein
*
*	@(#)	%filespec: appfacility.c~10 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
*
*******************************************************************************/

#if ! defined ( WIN32 )
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#endif

#include "cmbs_api.h"
#include "cfr_mssg.h"
#include "cmbs_fifo.h"
#include "appcmbs.h"
#include "stdio.h"
#include "appfacility.h"

#include "cmbs_util.h"

#define MAX_FACILITY_MSG_FOR_HS 10
#define NEW_MSG_ADDED 		1
#define MSG_SEND_RESPONSE 	0

extern u16 app_HandsetMap(char *psz_Handsets);
/* Data structures for Fifo handling */
typedef struct
{
    u16 u16_RequestId;
    u8 u8_LineId;
    u16 u16_Messages;
    E_CMBS_MWI_TYPE eType;
} ST_APP_FACILITY_MWI;

typedef struct
{
    u16 u16_RequestId;
    u8 u8_LineId;
    u16 u16_NewMissedCalls;
    bool bNewMissedCall;
    u16 u16_TotalMissedCalls;
} ST_APP_FACILITY_MCN;

typedef struct
{
    u16 u16_RequestId;
    u8 u8_ListId;
    u8 u8_ListEntries;
    u8 u8_LineId;
    u8 u8_LineSubtype;
	ST_APP_FACILITY_LCN_DETAILS st_Details;
} ST_APP_FACILITY_LCN;

typedef struct
{
    u16 u16_RequestId;
    u8 u8_NumOfWebCont;
} ST_APP_FACILITY_WC;

typedef struct
{
    u16 u16_RequestId;
    u16 u16_Reserved;
    u8 pu8_Data[CMBS_PROP_EVENT_LENGTH];
    u8 u8_DataLen;
} ST_APP_FACILITY_PROP;

typedef struct
{
    u16 u16_RequestId;
    ST_DATE_TIME st_DateTime;
} ST_APP_FACILITY_DT;

typedef struct
{
    u16 u16_RequestId;
    u8 u8_LineId;
    u32 u32_LineUseStatus;
} ST_APP_FACILITY_LUI;

typedef struct
{
    u16 u16_RequestId;
    u8 u8_LineId;
} ST_APP_FACILITY_HUI;

typedef struct
{
    u16 u16_RequestId;
    u8 u8_LineId;
    u8 u8_Type;
} ST_APP_FACILITY_DI;

typedef struct
{
    u16 u16_RequestId;
    u8 u8_SMSType;
    u16 u16_TotalNumOfMsgs;
    u16 u16_NumOfUnreadMsgs;
}ST_APP_FACILITY_SMS;

typedef struct
{
    u16 u16_RequestId;
    u16 u16_PropEvent;
    u8 pu8_Data[CMBS_PROP_EVENT_LENGTH];
    u8 u8_DataLen;
} ST_APP_FACILITY_PROP_EVENT_NOTIFICATION;

typedef enum
{
    FACILITY_TYPE_MWI,
    FACILITY_TYPE_MISSED_CALL,
    FACILITY_TYPE_LIST_CHANGED,
    FACILITY_TYPE_WEB_CONTENT,
    FACILITY_TYPE_PROP_EVENT,
    FACILITY_TYPE_DATE_TIME,
    FACILITY_TYPE_LINE_USE,
    FACILITY_TYPE_HANDSET_USE,
    FACILITY_TYPE_DIAGNOSTICS,
    FACILITY_TYPE_SMS,
    FACILITY_PROP_EVENT_NOTIFICATION,
	FACILITY_TYPE_LIST_CHANGED_EX
} E_APP_FACILITY_TYPE;

typedef union
{
    ST_APP_FACILITY_MWI st_MWI;
    ST_APP_FACILITY_MCN st_MCN;
    ST_APP_FACILITY_LCN st_LCN;
    ST_APP_FACILITY_WC st_WC;
    ST_APP_FACILITY_PROP st_PROP;
    ST_APP_FACILITY_DT st_DT;
    ST_APP_FACILITY_LUI st_LUI;
    ST_APP_FACILITY_HUI st_HUI;
    ST_APP_FACILITY_DI st_DI;
    ST_APP_FACILITY_SMS st_SMS;
    ST_APP_FACILITY_PROP_EVENT_NOTIFICATION st_PropEventNotify;
} U_APP_FACILITY_MSG;

typedef struct
{
    E_APP_FACILITY_TYPE e_type;
    u16 u16_Handsets;
    U_APP_FACILITY_MSG u_msg;
} ST_APP_FACILITY_MSG;

static CFR_CMBS_CRITICALSECTION FifoCriticalSection;
ST_CMBS_FIFO app_FacilityFifos[CMBS_HS_SUBSCRIBED_MAX_NUM];
u8 app_FacilityBuffers[CMBS_HS_SUBSCRIBED_MAX_NUM][sizeof(ST_APP_FACILITY_MSG)* MAX_FACILITY_MSG_FOR_HS];
u16 app_HsMask2HsNum(u16 u16_HsMask);


static E_CMBS_RC app_Facility_PushToFifos(ST_APP_FACILITY_MSG *pst_msg)
{
    u8 handset;
    u16 handsetMask;

    if ( !pst_msg )
    {
        APP_FACILITY_ERROR_PRINT("app_Facility_PushToFifos NULL pointer provided\n");
        return CMBS_RC_ERROR_PARAMETER;
    }

    handsetMask = pst_msg->u16_Handsets;

    for (handset = 0; handset < CMBS_HS_SUBSCRIBED_MAX_NUM; ++handset)
    {
        if ( (1 << handset) & handsetMask )
        {
            pst_msg->u16_Handsets = (1 << handset);
            if ( !cmbs_util_FifoPush(&app_FacilityFifos[handset], pst_msg) )
            {
                APP_FACILITY_ERROR_PRINT("FIFO %hhu full !!\n", handset);
            }
        }
    }

    return CMBS_RC_OK;

}

static E_CMBS_RC app_Facility_SendToTarget(u16 u16_HsMask, bool bReason)
{
    u8 handset;

    APP_FACILITY_ERROR_PRINT("app_Facility_SendToTarget u16_HsMask<%hu> bReason<%u>\n", u16_HsMask,(u32)bReason);

    if ( !u16_HsMask )
    {
        return CMBS_RC_OK;
    }

    for (handset = 0; handset < CMBS_HS_SUBSCRIBED_MAX_NUM; ++handset)
    {
        if ( (1 << handset) & u16_HsMask )
        {
            bool bSendMsg = FALSE;

            // if the fifo is of size 1 and we entered this function because of new message added, we can send it immediately
            if ( bReason == NEW_MSG_ADDED )
            {
                if ( cmbs_util_FifoCount(&app_FacilityFifos[handset]) == 1 )
                    bSendMsg = TRUE;
            }

            // if we have messages in the fifo and we entered this function becasue transmission of previous message is over, we can send another message
            else if ( cmbs_util_FifoCount(&app_FacilityFifos[handset]) )
            {
                bSendMsg = TRUE;
            }

            if ( bSendMsg )
            {
                E_CMBS_RC u8_return;
                
                ST_APP_FACILITY_MSG *pst_Msg = cmbs_util_FifoGet(&app_FacilityFifos[handset]);

                if ( !pst_Msg )
                {
                    APP_FACILITY_ERROR_PRINT("app_Facility_SendToTarget failed getting a message from FIFO %hhu\n", handset);
                    return CMBS_RC_ERROR_PARAMETER;
                }

                switch (pst_Msg->e_type)
                {
                case FACILITY_TYPE_MWI:
                    APP_FACILITY_INFO_PRINT("Sending MWI\n");
                    u8_return = cmbs_dsr_gen_SendMWI(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_MWI.u16_RequestId, pst_Msg->u_msg.st_MWI.u8_LineId,
                                         pst_Msg->u16_Handsets, pst_Msg->u_msg.st_MWI.u16_Messages, pst_Msg->u_msg.st_MWI.eType);
                    break;

                case FACILITY_TYPE_MISSED_CALL:
                    APP_FACILITY_INFO_PRINT("Sending MISSED CALL NOTIF\n");
                    u8_return = cmbs_dsr_gen_SendMissedCalls(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_MCN.u16_RequestId, pst_Msg->u_msg.st_MCN.u8_LineId, pst_Msg->u16_Handsets, pst_Msg->u_msg.st_MCN.u16_NewMissedCalls,
                                                 pst_Msg->u_msg.st_MCN.bNewMissedCall, pst_Msg->u_msg.st_MCN.u16_TotalMissedCalls);
                    break;

                case FACILITY_TYPE_LIST_CHANGED:
                    APP_FACILITY_INFO_PRINT("Sending LIST CHANGED NOTIF\n");
                    u8_return = cmbs_dsr_gen_SendListChanged(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_LCN.u16_RequestId, pst_Msg->u16_Handsets,
                                                 pst_Msg->u_msg.st_LCN.u8_ListId, pst_Msg->u_msg.st_LCN.u8_ListEntries, pst_Msg->u_msg.st_LCN.u8_LineId, pst_Msg->u_msg.st_LCN.u8_LineSubtype);
                    break;

                case FACILITY_TYPE_WEB_CONTENT:
                    APP_FACILITY_INFO_PRINT("Sending WEB CONTENT\n");
                    u8_return = cmbs_dsr_gen_SendWebContent(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_WC.u16_RequestId, pst_Msg->u16_Handsets, pst_Msg->u_msg.st_WC.u8_NumOfWebCont);
                    break;

                case FACILITY_TYPE_PROP_EVENT:
                    APP_FACILITY_INFO_PRINT("Sending PROP. EVENT\n");
                    u8_return = cmbs_dsr_gen_SendPropEvent(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_PROP.u16_RequestId, pst_Msg->u_msg.st_PROP.u16_Reserved, pst_Msg->u_msg.st_PROP.pu8_Data,
                                               pst_Msg->u_msg.st_PROP.u8_DataLen, pst_Msg->u16_Handsets);
                    break;

                case FACILITY_TYPE_DATE_TIME:
                    APP_FACILITY_INFO_PRINT("Sending DATE AND TIME UPDATE\n");
                    u8_return = cmbs_dsr_time_Update(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_DT.u16_RequestId, &pst_Msg->u_msg.st_DT.st_DateTime, pst_Msg->u16_Handsets);
                    break;

                case FACILITY_TYPE_LINE_USE:
                    APP_FACILITY_INFO_PRINT("Sending LINE USE INDICATION\n");
                    u8_return = cmbs_dsr_gen_SendLineUseStatusInd(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_LUI.u16_RequestId, pst_Msg->u_msg.st_LUI.u8_LineId, pst_Msg->u_msg.st_LUI.u32_LineUseStatus, pst_Msg->u16_Handsets);
                    break;

                case FACILITY_TYPE_HANDSET_USE:
                    APP_FACILITY_INFO_PRINT("Sending HANDSET USE INDICATION\n");
                    u8_return = cmbs_dsr_gen_SendHSUseStatusInd(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_HUI.u16_RequestId, pst_Msg->u_msg.st_HUI.u8_LineId, pst_Msg->u16_Handsets);
                    break;

                case FACILITY_TYPE_DIAGNOSTICS:
                    APP_FACILITY_INFO_PRINT("Sending DIAGNOSTICS INDICATION\n");
                    u8_return = cmbs_dsr_gen_SendDiagnosticStatusInd(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_DI.u16_RequestId, pst_Msg->u_msg.st_DI.u8_LineId, pst_Msg->u_msg.st_DI.u8_Type, pst_Msg->u16_Handsets);
                    break;
                    
                case FACILITY_TYPE_SMS:
                    APP_FACILITY_INFO_PRINT("Sending SMS INDICATION\n");                    
                    u8_return = cmbs_dsr_gen_SendSMSNotification(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_SMS.u16_RequestId, pst_Msg->u_msg.st_SMS.u8_SMSType, pst_Msg->u_msg.st_SMS.u16_NumOfUnreadMsgs, pst_Msg->u_msg.st_SMS.u16_TotalNumOfMsgs, pst_Msg->u16_Handsets);
                    break;

                case FACILITY_PROP_EVENT_NOTIFICATION:
                    APP_FACILITY_INFO_PRINT("Sending Prop event notification\n");                    
                    u8_return = cmbs_dsr_gen_SendPropEventNotification(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_PropEventNotify.u16_RequestId, pst_Msg->u_msg.st_PropEventNotify.u16_PropEvent, pst_Msg->u_msg.st_PropEventNotify.pu8_Data, pst_Msg->u_msg.st_PropEventNotify.u8_DataLen, pst_Msg->u16_Handsets);
                    break;
                    
                case FACILITY_TYPE_LIST_CHANGED_EX:
                    APP_FACILITY_INFO_PRINT("Sending LIST CHANGED NOTIF with Details\n");
                    u8_return = cmbs_dsr_gen_SendListChangedEx(g_cmbsappl.pv_CMBSRef, pst_Msg->u_msg.st_LCN.u16_RequestId, pst_Msg->u16_Handsets,
												 pst_Msg->u_msg.st_LCN.u8_ListId, pst_Msg->u_msg.st_LCN.u8_ListEntries, pst_Msg->u_msg.st_LCN.u8_LineId, pst_Msg->u_msg.st_LCN.u8_LineSubtype, (PST_IE_GEN_EVENT_DETAILS)&pst_Msg->u_msg.st_LCN.st_Details);
                    break;

                default:
                    APP_FACILITY_ERROR_PRINT("Unkown facility message type\n");
                    break;
                }
                if (u8_return == CMBS_RC_ERROR_NOT_SUPPORTED)
                {
                    // remove message from FIFO
                    cmbs_util_FifoPop(&app_FacilityFifos[handset]);
                }
            }
        }
    }

    return CMBS_RC_OK;
}


//		========== app_FacilityMWI  ===========
/*!
        \brief				 sending Voice/SMS/Email Message Waiting Indication
        \param[in,out]		 psz_Handsets     pointer to parameter string,e.g."1234" or "all"
        \return				 <E_CMBS_RC>
*/
E_CMBS_RC app_FacilityMWI(u16 u16_RequestId, u8 u8_LineId, u16 u16_Messages, u16 u16_HSMask, E_CMBS_MWI_TYPE eType)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityMWI\n");

    msg.e_type = FACILITY_TYPE_MWI;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_MWI.u16_RequestId = u16_RequestId;
    msg.u_msg.st_MWI.u8_LineId = u8_LineId;
    msg.u_msg.st_MWI.u16_Messages = u16_Messages;
    msg.u_msg.st_MWI.eType = eType;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);

}

E_CMBS_RC app_FacilityMissedCalls(u16 u16_RequestId, u8 u8_LineId, u16 u16_NewMissedCalls, u16 u16_HSMask, bool bNewMissedCall, u16 u16_TotalMissedCalls)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityMissedCalls\n");

    msg.e_type = FACILITY_TYPE_MISSED_CALL;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_MCN.u16_RequestId = u16_RequestId;
    msg.u_msg.st_MCN.u8_LineId = u8_LineId;
    msg.u_msg.st_MCN.u16_NewMissedCalls = u16_NewMissedCalls;
    msg.u_msg.st_MCN.bNewMissedCall = bNewMissedCall;
    msg.u_msg.st_MCN.u16_TotalMissedCalls = u16_TotalMissedCalls;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityListChanged(u16 u16_RequestId, u8 u8_ListId, u8 u8_ListEntries, u16 u16_HSMask, u8 u8_LineId, u8 u8_LineSubtype)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityListChanged\n");

    msg.e_type = FACILITY_TYPE_LIST_CHANGED;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_LCN.u16_RequestId = u16_RequestId;
    msg.u_msg.st_LCN.u8_ListId = u8_ListId;
    msg.u_msg.st_LCN.u8_ListEntries = u8_ListEntries;
    msg.u_msg.st_LCN.u8_LineId = u8_LineId;
    msg.u_msg.st_LCN.u8_LineSubtype = u8_LineSubtype;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityWebContent(u16 u16_RequestId, u8 u8_NumOfWebCont, u16 u16_HSMask)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityWebContent\n");

    msg.e_type = FACILITY_TYPE_WEB_CONTENT;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_WC.u16_RequestId = u16_RequestId;
    msg.u_msg.st_WC.u8_NumOfWebCont = u8_NumOfWebCont;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityPropEvent(u16 u16_RequestId, u16 u16_Reserved, u8 *pu8_Data, u8 u8_DataLen, u16 u16_HSMask)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityPropEvent\n");

    msg.e_type = FACILITY_TYPE_PROP_EVENT;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_PROP.u16_RequestId = u16_RequestId;
    msg.u_msg.st_PROP.u16_Reserved = u16_Reserved;
    memcpy(msg.u_msg.st_PROP.pu8_Data, pu8_Data, sizeof(msg.u_msg.st_PROP.pu8_Data));
    msg.u_msg.st_PROP.u8_DataLen = u8_DataLen;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityDateTime(u16 u16_RequestId, ST_DATE_TIME *pst_DateTime, u16 u16_HSMask)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityDateTime\n");

    msg.e_type = FACILITY_TYPE_DATE_TIME;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_DT.u16_RequestId = u16_RequestId;
    memcpy(&msg.u_msg.st_DT.st_DateTime, pst_DateTime, sizeof(msg.u_msg.st_DT.st_DateTime));

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityPropEventNotification(u16 u16_RequestId, u16 u16_PropEvent, u8 * pu8_Data, u8 u8_DataLen, char * psz_Handsets)
{
	ST_APP_FACILITY_MSG msg;
	E_CMBS_RC e_rc;
	u16  u16_Handsets = app_HandsetMap(psz_Handsets);

	msg.e_type = FACILITY_PROP_EVENT_NOTIFICATION;
	msg.u16_Handsets = u16_Handsets;
	msg.u_msg.st_PropEventNotify.u16_RequestId = u16_RequestId;
	msg.u_msg.st_PropEventNotify.u16_PropEvent = u16_PropEvent;

	if (u8_DataLen + FACILITY_PROP_EVENT_HEADER_LEN <= MAX_FACILITY_PROP_EVENT_LEN)
	{
		msg.u_msg.st_PropEventNotify.pu8_Data[0] = (CUSTOMER_DESCRIMINATOR >> 8) & 0xFF;  //Discriminator Upper Byte
		msg.u_msg.st_PropEventNotify.pu8_Data[1] = CUSTOMER_DESCRIMINATOR & 0xFF;  //Discriminator Lower Byte
		
		memcpy(&msg.u_msg.st_PropEventNotify.pu8_Data[2], pu8_Data, u8_DataLen);
		msg.u_msg.st_PropEventNotify.u8_DataLen = u8_DataLen;

		// push to Fifos
		app_Facility_PushToFifos(&msg);

		e_rc = app_Facility_SendToTarget(u16_Handsets, NEW_MSG_ADDED);
	}
	else
	{
		e_rc = CMBS_RC_ERROR_OUT_OF_MEM;
	}

	return e_rc;
}

E_CMBS_RC app_FacilityLineUseIndication(u16 u16_RequestId, u8 u8_LineId, u32 u32_LineUseStatus, u16 u16_Hs)
{

    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityLineUseIndication\n");

    msg.e_type = FACILITY_TYPE_LINE_USE;
    msg.u16_Handsets = u16_Hs;
    msg.u_msg.st_LUI.u16_RequestId = u16_RequestId;
    msg.u_msg.st_LUI.u8_LineId = u8_LineId;
    msg.u_msg.st_LUI.u32_LineUseStatus = u32_LineUseStatus;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_Hs, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityHSUseIndication(u16 u16_RequestId, u8 u8_LineId, u16 u16_Hs)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityHSUseIndication\n");

    msg.e_type = FACILITY_TYPE_HANDSET_USE;
    msg.u16_Handsets = u16_Hs;
    msg.u_msg.st_HUI.u16_RequestId = u16_RequestId;
    msg.u_msg.st_HUI.u8_LineId = u8_LineId;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_Hs, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityDiagnosticIndication(u16 u16_RequestId, u8 u8_LineId, u8 u8_Type, u16 u16_Hs)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityDiagnosticIndication\n");

    msg.e_type = FACILITY_TYPE_DIAGNOSTICS;
    msg.u16_Handsets = u16_Hs;
    msg.u_msg.st_DI.u16_RequestId = u16_RequestId;
    msg.u_msg.st_DI.u8_LineId = u8_LineId;
    msg.u_msg.st_DI.u8_Type = u8_Type;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_Hs, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilitySMSMessageNotification(u8 u8_SMSType, u16 u16_TotalNumOfMsgs, u16 u16_NumOfUnreadMsgs, u16 u16_HSMask)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilitySMSMessageNotification\n");

    msg.e_type = FACILITY_TYPE_SMS;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_SMS.u16_RequestId = 0;
    msg.u_msg.st_SMS.u16_NumOfUnreadMsgs = u16_NumOfUnreadMsgs;
    msg.u_msg.st_SMS.u16_TotalNumOfMsgs = u16_TotalNumOfMsgs;
    msg.u_msg.st_SMS.u8_SMSType = u8_SMSType;

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);
}

E_CMBS_RC app_FacilityListChangedEx(u16 u16_RequestId, u8 u8_ListId, u8 u8_ListEntries, u16 u16_HSMask, u8 u8_LineId, u8 u8_LineSubtype, 
												ST_APP_FACILITY_LCN_DETAILS *pst_Details)
{
    ST_APP_FACILITY_MSG msg;

    APP_FACILITY_INFO_PRINT("app_FacilityListChangedEx\n");

    msg.e_type = FACILITY_TYPE_LIST_CHANGED_EX;
    msg.u16_Handsets = u16_HSMask;
    msg.u_msg.st_LCN.u16_RequestId = u16_RequestId;
    msg.u_msg.st_LCN.u8_ListId = u8_ListId;
    msg.u_msg.st_LCN.u8_ListEntries = u8_ListEntries;
    msg.u_msg.st_LCN.u8_LineId = u8_LineId;
    msg.u_msg.st_LCN.u8_LineSubtype = u8_LineSubtype;

	memcpy(&msg.u_msg.st_LCN.st_Details, pst_Details, sizeof(ST_APP_FACILITY_LCN_DETAILS));

    // push to Fifos
    app_Facility_PushToFifos(&msg);

    // send to target if needed
    return app_Facility_SendToTarget(u16_HSMask, NEW_MSG_ADDED);
}


//		========== app_FacilityEntity ===========
/*!
        \brief		 CMBS entity to handle response information from target side
        \param[in]	 pv_AppRef		 application reference
        \param[in]	 e_EventID		 received CMBS event
        \param[in]	 pv_EventData	 pointer to IE list
        \return	 	 <int>
*/
int app_FacilityEntity(void *pv_AppRef, E_CMBS_EVENT_ID e_EventID, void *pv_EventData)
{
    ST_IE_RESPONSE st_Response;
	UNUSED_PARAMETER(pv_AppRef);
    UNUSED_PARAMETER(pv_EventData);
    APP_FACILITY_INFO_PRINT("app_FacilityEntity e_EventID %u\n",(u32)e_EventID);
    

    st_Response.e_Response = CMBS_RESPONSE_OK;

    switch (e_EventID)
    {
    case CMBS_EV_DSR_GEN_SEND_MWI_RES:
    case CMBS_EV_DSR_GEN_SEND_MISSED_CALLS_RES:
    case CMBS_EV_DSR_GEN_SEND_LIST_CHANGED_RES:
    case CMBS_EV_DSR_GEN_SEND_WEB_CONTENT_RES:
    case CMBS_EV_DSR_GEN_SEND_PROP_EVENT_RES:
    case CMBS_EV_DSR_TIME_UPDATE_RES:
    case CMBS_EV_DSR_GEN_DIAGNOSTIC_STATUS_RES:
    case CMBS_EV_DSR_GEN_LINE_USE_STATUS_RES:
    case CMBS_EV_DSR_GEN_HS_USE_STATUS_RES:
    case CMBS_EV_DSR_GEN_SEND_SMS_MSG_NOTIFICATION_RES:
    case CMBS_EV_DSR_GEN_SEND_PROP_EVENT_NOTIFY_RES:
        {
            // extract handset number
            void *pv_IE;
            u16 u16_IE;
            u16 u16_Handset = 0;

            if ( pv_EventData )
            {
                cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
                while (pv_IE != NULL)
                {
                    if ( CMBS_IE_HANDSETS == u16_IE )
                    {
                        cmbs_api_ie_HandsetsGet(pv_IE, &u16_Handset);
                    }
                    
                    if ( CMBS_IE_RESPONSE == u16_IE )
                    {
                        cmbs_api_ie_ResponseGet(pv_IE, &st_Response);
                    }

                    cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);
                }
            }
            else
            {
                APP_FACILITY_ERROR_PRINT("null pointer received !\n");
            }

            if (st_Response.e_Response == CMBS_RESPONSE_NO_LINK_AVL)
            {
                // There was no free instance for the handset, delay a bit and retry
                SleepMs(200);
                // send message again, thus don't remove it
            }
            else
            {               
                // remove acked message from fifo
            cmbs_util_FifoPop(&app_FacilityFifos[(app_HsMask2HsNum(u16_Handset)) - 1]);
            }

            // send next message to target
            app_Facility_SendToTarget(u16_Handset, MSG_SEND_RESPONSE);
            
            return TRUE; // event consumed
        }
    default:
        return FALSE; // event not consumed
    }
}

//		========== app_FacilityInit ===========
/*!
        \brief		 Init function for the facility module
        \return	 	 <int>
*/
E_CMBS_RC app_FacilityInit(void)
{
    u8 i;

    // init Fifos
    for (i = 0; i < CMBS_HS_SUBSCRIBED_MAX_NUM; ++i)
    {
        cmbs_util_FifoInit(&app_FacilityFifos[i], app_FacilityBuffers[i], sizeof(ST_APP_FACILITY_MSG), MAX_FACILITY_MSG_FOR_HS, FifoCriticalSection);

    }

    return CMBS_RC_OK;
}



u16 app_HsMask2HsNum(u16 u16_HsMask)
{
    u8 u8_i;
    for (u8_i = 0; u8_i < 16; ++u8_i)
    {
        if ( u16_HsMask & (1 << u8_i) )
        {
            return u8_i + 1;
        }
    }
    return 0;
}

/***** [End Of File] *****************************************************************************************************************************/


