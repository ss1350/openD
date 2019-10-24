/*************************************************************************************************************
*** List Change Notification
**
**************************************************************************************************************/

/*******************************************
Includes
********************************************/
#include "ListChangeNotif.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "ListsApp.h"
#include "appsrv.h"
#include "appfacility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************
Defines
********************************************/

/*******************************************
Types
********************************************/

/*******************************************
Globals
********************************************/

/*******************************************
Auxiliary
********************************************/

extern E_CMBS_RC GetLineIdBySMSId(IN u32 u32_SMSId, OUT u32 *pu32_LineId);

/* ***************** Auxiliary end ***************** */

/*******************************************
List Change Notification API

 u16_HsId may be a single HS ID or all handsets which are attached to the given line (CMBS_ALL_RELEVANT_HS_ID)

********************************************/

void ListChangeNotif_MissedCallListChanged(IN u32 u32_LineId, IN bool bNewEntryAdded, IN u16 u16_HsId)
{
    u32 u32_HandsetMask = 0, u32_NumOfRead = 0, u32_NumOfUnread = 0;    
    LIST_RC rc;
    
    APP_FACILITY_INFO_PRINT("ListChangeNotif_MissedCallListChanged LineID %d u16_HsId %d \n",u32_LineId, u16_HsId);

    rc = List_GetAttachedHs(u32_LineId, &u32_HandsetMask);

    if(rc == LIST_RC_OK)
    {
	    APP_FACILITY_INFO_PRINT("ListChangeNotif_MissedCallListChanged u32_HandsetMask1 %x \n", u32_HandsetMask);

	    // if only one HS has to be notified, mask other handsets:
	    if ( u16_HsId != CMBS_ALL_RELEVANT_HS_ID )
	    {
	        u32_HandsetMask &= (1 << (u16_HsId - 1));
	    }

	    APP_FACILITY_INFO_PRINT("ListChangeNotif_MissedCallListChanged u32_HandsetMask2 %x \n", u32_HandsetMask);

	    if ( u32_HandsetMask )
	    {
	        List_GetMissedCallsNumOfEntries(u32_LineId, &u32_NumOfUnread, &u32_NumOfRead);
	        app_FacilityMissedCalls(0, (u8)u32_LineId, (u16)u32_NumOfUnread, (u16)u32_HandsetMask, bNewEntryAdded, (u16)(u32_NumOfUnread + u32_NumOfRead));
	    }
    }
    else
        APP_FACILITY_ERROR_PRINT("List_GetAttachedHs failed \n");
}

void ListChangeNotif_ListChanged(IN u32 u32_LineId, IN eLineType tLineType, IN u32 u32_HandsetMask,
                                 IN u32 u32_TotalNumOfEntries, IN eLIST_CHANGE_NOTIF_LIST_TYPE tListId)
{
    app_FacilityListChanged(0, tListId, (u8)u32_TotalNumOfEntries, (u16)u32_HandsetMask, (u8)u32_LineId, tLineType);
}


void ListChangeNotif_OutgoingCallListChangedEx(IN u32 u32_LineId, IN bool bNewEntryAdded, IN u16 u16_HsId)
{
    u32 u32_HandsetMask = 0, u32_NumOfEntries = 0;    
    LIST_RC rc;
	ST_APP_FACILITY_LCN_DETAILS st_Details;
    
    APP_FACILITY_INFO_PRINT("ListChangeNotif_OutgoingCallListChangedEx LineID %d u16_HsId %d \n",u32_LineId, u16_HsId);

    rc = List_GetAttachedHs(u32_LineId, &u32_HandsetMask);

    if(rc == LIST_RC_OK)
    {
	    APP_FACILITY_INFO_PRINT("ListChangeNotif_OutgoingCallListChangedEx u32_HandsetMask1 %x \n", u32_HandsetMask);

	    if ( u32_HandsetMask )
	    {
	        List_GetOutgoingCallsNumOfEntries(u32_LineId, &u32_NumOfEntries);

			st_Details.u8_Count = 1; // one change
	    	st_Details.u16_OrgHs = u16_HsId; //HS num from which outgoing call is made
			st_Details.st_SubDetails[0].u16_Change = 1; //Add
	    	st_Details.st_SubDetails[0].u16_EntryId = 1; //last outgoing call will have entry id 1
			st_Details.st_SubDetails[0].u16_PosIndex = u32_NumOfEntries-1; // considering last outgoing call is added to the end of list.

	        app_FacilityListChangedEx(0, LIST_CHANGE_NOTIF_LIST_TYPE_OUTGOING_CALLS, u32_NumOfEntries, 
										(u16)u32_HandsetMask, u32_LineId, LINE_TYPE_EXTERNAL, &st_Details);
	    }
    }
    else
        APP_FACILITY_ERROR_PRINT("List_GetAttachedHs failed \n");
}


void ListChangeNotif_SMSListChanged(IN u32 u32_SMSId, IN u8 ListType, IN u16 u16_HsId)
{
    u32 u32_HandsetMask = 0, u32_TotalNumOfEntries = 0, u32_LineId = 0;
    eLIST_CHANGE_NOTIF_LIST_TYPE tListId = 0;
    LIST_RC rc;
    
    if ( GetLineIdBySMSId(u32_SMSId, &u32_LineId) == CMBS_RC_OK )
    {
        if(List_GetAttachedHs(u32_LineId, &u32_HandsetMask) == LIST_RC_OK)
        {
	        rc = List_GetCount(ListType, &u32_TotalNumOfEntries);

	        if ( rc != LIST_RC_OK )
	            APP_FACILITY_ERROR_PRINT("List_GetCount failed!\n");

	        if ( u16_HsId != CMBS_ALL_RELEVANT_HS_ID )
	        {
	            u32_HandsetMask &= (1 << (u16_HsId - 1));
	        }
	        switch ((LIST_TYPE)ListType)
	        {
	        case LIST_TYPE_INCOMING_SMS:
	            tListId = LIST_CHANGE_NOTIF_LIST_TYPE_INCOMING_SMS; break;
	        case LIST_TYPE_SENT_SMS:
	            tListId = LIST_CHANGE_NOTIF_LIST_TYPE_SENT_SMS; break;
	        case LIST_TYPE_OUTGOING_SMS:
	            tListId = LIST_CHANGE_NOTIF_LIST_TYPE_OUTGOING_SMS; break;
	        case LIST_TYPE_DRAFT_SMS:
	            tListId = LIST_CHANGE_NOTIF_LIST_TYPE_DRAFT_SMS; break;
	        default:
	            APP_FACILITY_WARNING_PRINT("Not SMS list, aborting");
	            return;
	        }
            
	        app_FacilityListChanged(0, tListId, (u8)u32_TotalNumOfEntries, (u16)u32_HandsetMask, (u8)u32_LineId, 0);
         }
		 else
			 APP_FACILITY_ERROR_PRINT("List_GetAttachedHs failed \n");
    }
    else
    {
        APP_FACILITY_ERROR_PRINT("GetLineIdBySMSId failed!\n");
    }
}

void ListChangeNotif_IncomingSMSListChanged(IN u32 u32_SMSId, IN u8 u8_SMSType, IN u16 u16_HsId)
{
    u32 u32_NumOfRead, u32_NumOfUnread, u32_HandsetMask, u32_LineId;
    
    if ( GetLineIdBySMSId(u32_SMSId, &u32_LineId) == CMBS_RC_OK )
    {
        if(List_GetAttachedHs(u32_LineId, &u32_HandsetMask) == LIST_RC_OK)
        {
	        APP_FACILITY_INFO_PRINT("ListChangeNotif_IncomingSMSListChanged \n");
	        if ( u16_HsId != CMBS_ALL_RELEVANT_HS_ID )
	        {
	            u32_HandsetMask &= (1 << (u16_HsId - 1));
	        }
	        List_GetSMSNumOfEntries(u32_SMSId, &u32_NumOfUnread, &u32_NumOfRead);
	        app_FacilitySMSMessageNotification(u8_SMSType, (u16)(u32_NumOfUnread + u32_NumOfRead), (u16)u32_NumOfUnread, u32_HandsetMask);
	    }
        else
            APP_FACILITY_ERROR_PRINT("List_GetAttachedHs failed! \n");
    }
    else
    {
        APP_FACILITY_ERROR_PRINT("GetLineIdBySMSId failed!\n");
    }
}


void ListChangeNotif_SendListChangeDetails( IN u8 ListType)
{
	LIST_RC rc = LIST_RC_OK;
	ST_APP_FACILITY_LCN_DETAILS *pst_LcnDetails = NULL;

	if (List_GetListChangeDetails(ListType, &pst_LcnDetails) == LIST_RC_OK)
	{
		eLIST_CHANGE_NOTIF_LIST_TYPE tListId = 0;
		u32 u32_Count;

		switch ((LIST_TYPE)ListType)
		{
			case	LIST_TYPE_CONTACT_LIST:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_CONTACT_LIST; break;
			case	LIST_TYPE_LINE_SETTINGS_LIST:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_LINE_SETTINGS_LIST; break;
			case	LIST_TYPE_MISSED_CALLS		:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_MISSED_CALLS; break;
			case	LIST_TYPE_OUTGOING_CALLS	:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_OUTGOING_CALLS; break;
			case	LIST_TYPE_INCOMING_ACCEPTED_CALLS	   :
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_INCOMING_ACCEPTED_CALLS; break;
			case	LIST_TYPE_ALL_CALLS 		:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_ALL_CALLS; break;
			case	LIST_TYPE_ALL_INCOMING_CALLS:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_ALL_INCOMING_CALLS; break;
			case	LIST_TYPE_INCOMING_SMS:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_INCOMING_SMS; break;
			case	LIST_TYPE_SENT_SMS:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_SENT_SMS; break;
			case	LIST_TYPE_OUTGOING_SMS:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_OUTGOING_SMS; break;
			case	LIST_TYPE_DRAFT_SMS:
				tListId = LIST_CHANGE_NOTIF_LIST_TYPE_DRAFT_SMS; break;
			case	LIST_TYPE_DTAM_SETTINGS_LIST:
			case	LIST_TYPE_SMS_SETTINGS_LIST:
			case	LIST_TYPE_DTAM_WELCOME_MESSAGE:
			case	LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS:
			default :
				APP_FACILITY_WARNING_PRINT("send list change details, aborting list type %d", ListType);
			return;
		}

		/* Get Count */
		rc = List_GetCount(ListType, &u32_Count);
		if ((rc != LIST_RC_OK) && (pst_LcnDetails == NULL))
		{
			return;
		}

		app_FacilityListChangedEx (0, tListId, u32_Count, CMBS_ALL_HS_MASK, 0, LINE_TYPE_ALL_LINES, pst_LcnDetails);
	}

	List_ResetListChangeDetails (ListType);
}	

void ListChangeNotif_SendAllListChangeDetails( void )
{
	LIST_TYPE ListType = 0;

	while (ListType < LIST_TYPE_MAX)
	{
		ListChangeNotif_SendListChangeDetails(ListType);
		ListType++;
	}
}

/* End Of File *****************************************************************************************************************************/

