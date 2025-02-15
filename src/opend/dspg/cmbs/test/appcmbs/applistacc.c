/*************************** [Includes] *******************************************************/
#ifdef __linux
    #include <pthread.h>	
	#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include "cmbs_api.h"
#include "cmbs_ie.h"
#include "LASessionMgr.h"
#include "appcmbs.h"
#include "appmsgparser.h"

#include "appla2.h"

/*************************** [Globals] ********************************************************/
u8 u8_APP_LA_Initialized = 0;


/*************************** [List access protocol implementation] ****************************/
void app_LaInitialize( void )
{
    LASessionMgr_Init();
}

int app_LaIsValidList ( u16 u16_ListId )
{
    if ( ( u16_ListId == CMBS_LA_LIST_MISSED_CALLS )    ||   
         ( u16_ListId == CMBS_LA_LIST_OUTGOING_CALLS )  ||
         ( u16_ListId == CMBS_LA_LIST_INCOMING_ACC)     ||
         ( u16_ListId == CMBS_LA_LIST_ALL_CALLS )       ||      
         ( u16_ListId == CMBS_LA_LIST_CONTACT )         ||       
         ( u16_ListId == CMBS_LA_LIST_LINE_SETTINGS )   ||
         ( u16_ListId == CMBS_LA_LIST_ALL_INCOMING_CALLS )          ||
         ( u16_ListId == CMBS_LA_LIST_SMS_SETTINGS)					|| 
         ( u16_ListId == CMBS_LA_LIST_OUTGOING_SMS) 				||
         ( u16_ListId == CMBS_LA_LIST_INCOMING_SMS) 				||
         ( u16_ListId == CMBS_LA_LIST_DRAFT_SMS) 					||
         ( u16_ListId == CMBS_LA_LIST_SENT_SMS) 					||         
         ( u16_ListId == CMBS_LA_LIST_LINE_AND_DIAGNOSTIC_STATUS)   ||
         ( u16_ListId == CMBS_LA_LIST_DTAM_SETTINGS)         		||
         ( u16_ListId == CMBS_LA_LIST_DTAM_WELCOME_MESSAGE) )
    {
        return 1;
    }

    return 0;
}

void app_SendDataPackets(void* pv_AppRefHandle, u16 u16_SessionId, u8 pu8_Data[], u16* pu16_Marker, u16* pu16_Len)
{
    bool IsLast;
    u16 u16_DataPacketSize;
    u16 u16_BytesLeft = (*pu16_Len - *pu16_Marker);

    if ( u16_BytesLeft < 57 )
    {
        u16_DataPacketSize = u16_BytesLeft;
        IsLast = TRUE;
    }
    else
    {
        u16_DataPacketSize = 56;
        IsLast = FALSE;
    }

    cmbs_dsr_la_DataPacketSend( pv_AppRefHandle,
                                u16_SessionId,
                                IsLast,
                                pu8_Data + *pu16_Marker,
                                u16_DataPacketSize);

    *pu16_Marker += u16_DataPacketSize;
}

int app_OnLaSessionStart(void * pv_AppRef, void * pv_EventData)
{
    void *          pv_IE = NULL;
    u16             u16_IE, u16_SessionId = 0, u16_ListId = 0, u16_HandsetId = 0, u16_NrOfEntries, i = 0;
    LA_SESSION_MGR_RC session_rc;
    ST_IE_LA_FIELDS st_LaFields;
    E_CMBS_RESPONSE e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_START_SESSION_ERRORS
    e_RejectReason = CMBS_LA_START_SESSION_ERROR_MAX;

    memset(&st_LaFields, 0, sizeof(st_LaFields)); 

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            break;

        case CMBS_IE_HANDSETS:          /*!< Bit mask of one or more handsets */

            cmbs_api_ie_HandsetsGet( pv_IE, &u16_HandsetId );
            for ( i = 0; i < 16; i++ )
            {
                if ( u16_HandsetId & (1<<i ) )
                    break;
            }
            u16_HandsetId = i + 1;
            break;

        case CMBS_IE_LA_LIST_ID:        /*!< Id of the list being accessed via Access List Service */

            cmbs_api_ie_LAListIdGet( pv_IE, &u16_ListId );
            break;

        case CMBS_IE_LA_SORT_FIELDS:    /*!< Holds sort fields of a list. */
            cmbs_api_ie_LAFieldsGet( pv_IE, &st_LaFields );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT( "\nStart Session SessionId=%x\n", u16_SessionId );

    session_rc = LASessionMgr_StartSession(u16_SessionId, u16_ListId, u16_HandsetId, &st_LaFields, &u16_NrOfEntries);

    if ( session_rc == LA_SESSION_MGR_RC_NO_FREE_SESSIONS )
    {
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_START_SESSION_ERROR_MAX_SESSIONS_REACHED;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_UNSUPPORTED_LIST )
    {
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_START_SESSION_ERROR_LIST_NOT_SUPPORTED;
    }

    if ( !app_LaIsValidList ( u16_ListId ) )
    {
        APP_LA_ERROR_PRINT("ERROR: No valid list selected\n");
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_START_SESSION_ERROR_LIST_NOT_SUPPORTED;
    }

    if ( e_Response != CMBS_RESPONSE_OK )
    {
        u16_NrOfEntries = 0;
        st_LaFields.u16_Length = 0;
    }

    // for debug: enable next 2 lines to block all LA sessions
	//e_Response	   = CMBS_RESPONSE_ERROR;
	//e_RejectReason = CMBS_LA_START_SESSION_ERROR_LIST_NOT_SUPPORTED;

    return cmbs_dsr_la_SessionStartRes( pv_AppRef, u16_SessionId, u16_NrOfEntries, &st_LaFields, e_Response, e_RejectReason );
}

int app_OnLaSessionEnd(void * pv_AppRef, void * pv_EventData)
{
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    LA_SESSION_MGR_RC       session_rc;
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */

            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            break;
        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("End Session SessionId=%x\n", u16_SessionId);

    session_rc = LASessionMgr_EndSession(u16_SessionId);

    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        APP_LA_ERROR_PRINT("ERROR: u16_SessionId=%x not known\n", u16_SessionId );
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }

    return cmbs_dsr_la_SessionEndRes( pv_AppRef, u16_SessionId, e_Response, e_RejectReason );
}

int app_OnLaSessionEndRes(void * pv_AppRef, void * pv_EventData)
{
    UNUSED_PARAMETER(pv_AppRef);
    UNUSED_PARAMETER(pv_EventData);

    return TRUE;
}

int app_OnLaQuerySuppEntryFields(void * pv_AppRef, void * pv_EventData)
{
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;
    ST_LA_FIELDS            st_LANonEditFields;
    ST_LA_FIELDS            st_LAEditFields;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    LA_SESSION_MGR_RC       session_rc;

    memset(&st_LANonEditFields, 0, sizeof(st_LANonEditFields));
    memset(&st_LAEditFields, 0, sizeof(st_LAEditFields));

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */

            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            break;
        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("Query Supported Fields\n");

    session_rc = LASessionMgr_GetSupportedFields(u16_SessionId, &st_LAEditFields, &st_LANonEditFields);

    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        APP_LA_ERROR_PRINT("ERROR: Wrong SessionId=%x\n", u16_SessionId );
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }

    return cmbs_dsr_la_QuerySupEntryFieldsRes( pv_AppRef, u16_SessionId,
                                               &st_LANonEditFields, &st_LAEditFields, e_Response, e_RejectReason );
}

int app_OnLaReadEntries(void * pv_AppRef, void * pv_EventData)
{
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    ST_IE_LA_FIELDS         st_LaFields;
    u16                     u16_EntryIndex = 0;
    u16                     u16_EntryCountRequested = 0;
    LA_SESSION_MGR_RC       session_rc;
    ST_IE_READ_DIRECTION    st_ReadDirection;
    ST_IE_MARK_REQUEST      st_Mark;
    u8*                     pu8_Data;
    u16                     *pu16_Marker, *pu16_TotalLen;
    bool                    bForward = TRUE;

    memset(&st_Mark, 0, sizeof(ST_IE_MARK_REQUEST));
    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */

            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_FIELDS:
            //APP_LA_INFO_PRINT("LA_FIELDS:\n");
            cmbs_api_ie_LAFieldsGet( pv_IE, &st_LaFields );
            /*APP_LA_INFO_PRINT( "Length:%d\n", st_LaFields.u16_Length );
            APP_LA_INFO_PRINT( "FieldIds:" );

            if ( st_LaFields.u16_Length == 0 )
            {
                APP_LA_INFO_PRINT(" (none)\n");
            }
            else
            {
                for ( i = 0; i < st_LaFields.u16_Length; i++ )
                {
                    APP_LA_INFO_PRINT( " %2d", st_LaFields.pu16_FieldId[i] );
                }
                APP_LA_INFO_PRINT( "\n" );
            }*/
            break;

        case CMBS_IE_LA_ENTRY_INDEX:
            cmbs_api_ie_LAEntryIndexGet( pv_IE, &u16_EntryIndex );
            //APP_LA_INFO_PRINT( "u16_EntryIndex=%d\n", u16_EntryIndex );
            break;

        case CMBS_IE_LA_ENTRY_COUNT:
            cmbs_api_ie_LAEntryCountGet( pv_IE, &u16_EntryCountRequested );
            //APP_LA_INFO_PRINT( "u16_EntryCountRequested=%d\n", u16_EntryCountRequested );
            break;

        case CMBS_IE_LA_READ_DIRECTION:
            cmbs_api_ie_ReadDirectionGet( pv_IE, &st_ReadDirection );
            //APP_LA_INFO_PRINT( "Read Direction=%d\n", st_ReadDirection.e_ReadDirection );
            if ( st_ReadDirection.e_ReadDirection == CMBS_READ_DIRECTION_FORWARD )
            {
                bForward = TRUE;
            }
            else
            {
                bForward = FALSE;
            }
            break;

        case CMBS_IE_LA_MARK_REQUEST:
            cmbs_api_ie_MarkRequestGet( pv_IE, &st_Mark );
            //APP_LA_INFO_PRINT( "Mark=%d\n", st_Mark.e_MarkRequest );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    LASessionMgr_GetDataBuffer(u16_SessionId, &pu8_Data, &pu16_Marker, &pu16_TotalLen);

    session_rc = LASessionMgr_ReadEntries(u16_SessionId, &u16_EntryIndex, bForward, st_Mark.e_MarkRequest,
                                          &st_LaFields, &u16_EntryCountRequested, pu8_Data, pu16_TotalLen);

    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_NOT_ENOUGH_MEMORY )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_TEMP_NOT_POSSIBLE;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_INVALID_START_INDEX )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_START_INDEX;
    }

    cmbs_dsr_la_ReadEntriesRes(pv_AppRef, u16_SessionId, u16_EntryIndex, u16_EntryCountRequested, e_Response, e_RejectReason);

    /* Possibly need to send Missed call notification */
    LASessionMgr_SendMissedCallNotification(u16_SessionId, st_Mark.e_MarkRequest);

    /* Possibly need to send SMS notification */    
    LASessionMgr_SendSMSNotification(u16_SessionId, st_Mark.e_MarkRequest);
    
    if ( e_Response == CMBS_RESPONSE_OK )
    {
        app_SendDataPackets(pv_AppRef, u16_SessionId, pu8_Data, pu16_Marker, pu16_TotalLen);
    }

    return TRUE;
}

int app_OnLaReadSelectedEntries(void * pv_AppRef, void * pv_EventData)
{
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    ST_IE_LA_FIELDS         st_LaFields;
    u16                     u16_EntryIndex = 1;
    u16                     u16_SelectionType = 0;
    u16                     u16_EntryCountRequested = 1;
    LA_SESSION_MGR_RC       session_rc;
    ST_IE_MARK_REQUEST      st_Mark;
    u8*                     pu8_Data;
    u16                     *pu16_Marker, *pu16_TotalLen;
    bool                    bForward = TRUE;
	ST_IE_LA_ENTRY_RANGE st_EntryRange;
	ST_IE_LA_ENTRY_IDS st_EntryIds;

    memset(&st_Mark, 0, sizeof(ST_IE_MARK_REQUEST));
    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */

            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_FIELDS:
            //APP_LA_INFO_PRINT("LA_FIELDS:\n");
            cmbs_api_ie_LAFieldsGet( pv_IE, &st_LaFields );
            /*APP_LA_INFO_PRINT( "Length:%d\n", st_LaFields.u16_Length );
            APP_LA_INFO_PRINT( "FieldIds:" );

            if ( st_LaFields.u16_Length == 0 )
            {
                APP_LA_INFO_PRINT(" (none)\n");
            }
            else
            {
                for ( i = 0; i < st_LaFields.u16_Length; i++ )
                {
                    APP_LA_INFO_PRINT( " %2d", st_LaFields.pu16_FieldId[i] );
                }
                APP_LA_INFO_PRINT( "\n" );
            }*/
            break;

        case CMBS_IE_LA_SELECTION_TYPE:
            cmbs_api_ie_LASelectionTypeGet( pv_IE, &u16_SelectionType );
            //APP_LA_INFO_PRINT( "u16_SelectionType=%d\n", u16_SelectionType );
            break;

        case CMBS_IE_LA_ENTRY_RANGE:
            cmbs_api_ie_LAEntryRangeGet( pv_IE, &st_EntryRange );
            break;

        case CMBS_IE_LA_ENTRY_IDS:
            cmbs_api_ie_LAEntryIdsGet( pv_IE, &st_EntryIds );
            break;

        case CMBS_IE_LA_MARK_REQUEST:
            cmbs_api_ie_MarkRequestGet( pv_IE, &st_Mark );
            //APP_LA_INFO_PRINT( "Mark=%d\n", st_Mark.e_MarkRequest );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    LASessionMgr_GetDataBuffer(u16_SessionId, &pu8_Data, &pu16_Marker, &pu16_TotalLen);

    session_rc = LASessionMgr_ReadEntries(u16_SessionId, &u16_EntryIndex, bForward, st_Mark.e_MarkRequest,
                                          &st_LaFields, &u16_EntryCountRequested, pu8_Data, pu16_TotalLen);

    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_NOT_ENOUGH_MEMORY )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_TEMP_NOT_POSSIBLE;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_INVALID_START_INDEX )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_START_INDEX;
    }

    cmbs_dsr_la_ReadSelectedEntriesRes(pv_AppRef, u16_SessionId, u16_EntryCountRequested, e_Response, e_RejectReason);

    /* Possibly need to send Missed call notification */
    LASessionMgr_SendMissedCallNotification(u16_SessionId, st_Mark.e_MarkRequest);

    /* Possibly need to send SMS notification */    
    LASessionMgr_SendSMSNotification(u16_SessionId, st_Mark.e_MarkRequest);
    
    if ( e_Response == CMBS_RESPONSE_OK )
    {
        app_SendDataPackets(pv_AppRef, u16_SessionId, pu8_Data, pu16_Marker, pu16_TotalLen);
    }

    return TRUE;
}

int app_OnLaEditEntry(void * pv_AppRef, void * pv_EventData)
{
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;
    ST_IE_LA_FIELDS         st_LaFields;
    u16                     u16_EntryID = 0;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    LA_SESSION_MGR_RC       session_rc;
    u8*                     pu8_Data;
    u16                     *pu16_Marker, *pu16_TotalLen;

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_EDIT_FIELDS:
            //APP_LA_INFO_PRINT("LA_FIELDS:\n");
            cmbs_api_ie_LAFieldsGet( pv_IE, &st_LaFields );
            /*APP_LA_INFO_PRINT( "Length:%d\n", st_LaFields.u16_Length );
            APP_LA_INFO_PRINT( "FieldIds:" );

            if ( st_LaFields.u16_Length == 0 )
            {
                APP_LA_INFO_PRINT(" (none)\n");
            }
            else
            {
                for ( i = 0; i < st_LaFields.u16_Length; i++ )
                {
                    APP_LA_INFO_PRINT( " %2d", st_LaFields.pu16_FieldId[i] );
                }
                APP_LA_INFO_PRINT( "\n" );
            }*/
            break;

        case CMBS_IE_LA_ENTRY_ID:
            cmbs_api_ie_LAEntryIdGet( pv_IE, &u16_EntryID );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }
    APP_LA_INFO_PRINT("Edit EntryID %d\n", u16_EntryID);

    LASessionMgr_GetDataBuffer(u16_SessionId, &pu8_Data, &pu16_Marker, &pu16_TotalLen);

    session_rc = LASessionMgr_EditEntry(u16_SessionId, u16_EntryID, &st_LaFields, pu8_Data, pu16_TotalLen);

    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        APP_LA_ERROR_PRINT("ERROR: Wrong SessionId=%x\n", u16_SessionId );
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_ENTRY_NOT_AVAILABLE )
    {
        APP_LA_ERROR_PRINT("ERROR: Entry Not Available=%x\n", u16_EntryID );
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_ENTRY_NOT_AVAILABLE;
    }

    cmbs_dsr_la_EditEntryRes( pv_AppRef, u16_SessionId, e_Response, e_RejectReason );

    if ( e_Response == CMBS_RESPONSE_OK )
    {
        app_SendDataPackets(pv_AppRef, u16_SessionId, pu8_Data, pu16_Marker, pu16_TotalLen);
    }

    return TRUE;
}

int app_OnLaSaveEntry(void * pv_AppRef, void * pv_EventData)
{
    u16                     u16_EntryID = 0;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;

    UNUSED_PARAMETER(pv_AppRef);

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */

            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_ENTRY_ID:
            cmbs_api_ie_LAEntryIdGet( pv_IE, &u16_EntryID );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("Save Entry %d\n", u16_EntryID);

    LASessionMgr_SetSaveEntryID(u16_SessionId, u16_EntryID);

    return TRUE;
}

int app_OnLaWriteEntry(void * pv_AppRef, void * pv_EventData)
{
    u16                     u16_EntryID = 0;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
	ST_IE_LA_ENTRY_RANGE st_EntryRange;

    UNUSED_PARAMETER(pv_AppRef);

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */

            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_ENTRY_RANGE:
            cmbs_api_ie_LAEntryRangeGet( pv_IE, &st_EntryRange );
			u16_EntryID = st_EntryRange.u16_EntryId;
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("Write Entry %d\n", u16_EntryID);

    LASessionMgr_SetWriteEntryID(u16_SessionId, u16_EntryID);

    return TRUE;
}

int app_OnLaDeleteEntry(void * pv_AppRef, void * pv_EventData)
{
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;
    u16                     u16_TotalNoOfEntries;
    u16                     u16_EntryID = 0;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    LA_SESSION_MGR_RC       session_rc;

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_ENTRY_ID:
            cmbs_api_ie_LAEntryIdGet( pv_IE, &u16_EntryID );
            //APP_LA_INFO_PRINT( "u16_EntryID=%d\n", u16_EntryID );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("Delete EntryID %d\n", u16_EntryID);

    session_rc = LASessionMgr_DeleteEntry(u16_SessionId, u16_EntryID, &u16_TotalNoOfEntries);
    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        APP_LA_ERROR_PRINT("ERROR: Wrong SessionId=%x\n", u16_SessionId );
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_ENTRY_NOT_AVAILABLE )
    {
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_ENTRY_NOT_AVAILABLE;
    }

    cmbs_dsr_la_DeleteEntryRes( pv_AppRef, u16_SessionId, u16_TotalNoOfEntries, e_Response, e_RejectReason );

    /* Possibly need to send Missed call notification / List Change notification */
    LASessionMgr_SendListChangedNotificationOnDelete(u16_SessionId);

    return TRUE;
}

int app_OnLaProprietaryCommand( void * pv_AppRef, void * pv_EventData )
{
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId;
    ST_LA_PROP_CMD          st_PropCmd;

	UNUSED_PARAMETER(pv_AppRef);

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_PROP_CMD:
            cmbs_api_ie_LAPropCmdGet( pv_IE, &st_PropCmd );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("Proprietary Command\n");

    //cmbs_dsr_la_PropCmdRes(  );

    return TRUE;
}

int app_OnLaDeleteList(void * pv_AppRef, void * pv_EventData)
{
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    LA_SESSION_MGR_RC       session_rc;

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("Delete List\n");

    session_rc = LASessionMgr_DeleteAllEntries(u16_SessionId);

    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        APP_LA_ERROR_PRINT("ERROR: Wrong SessionId=%x\n", u16_SessionId );
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_NOT_ALLOWED )
    {
        APP_LA_ERROR_PRINT("ERROR: Cannot delete list\n");
        e_Response     = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_PROC_NOT_ALLOWED;
    }

    cmbs_dsr_la_DeleteListRes( pv_AppRef, u16_SessionId, e_Response, e_RejectReason );

    /* Possibly need to send Missed call notification */
    LASessionMgr_SendListChangedNotificationOnDelete(u16_SessionId);

    return TRUE;
}

int app_OnLaSearchEntries(void * pv_AppRef, void * pv_EventData)
{
    u32                         u32_EntryStartIndex;
    u16                         u16_EntryCount = 0;
    ST_IE_LA_SEARCH_CRITERIA    st_LASearchCriteria;
    ST_IE_LA_FIELDS             st_LaFields;
    E_CMBS_RESPONSE             e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS       e_RejectReason = CMBS_LA_NACK_MAX;
    void *                      pv_IE = NULL;
    u16                         u16_IE;
    u16                         u16_SessionId = 0;
    u8*                         pu8_Data;
    u16                         *pu16_Marker, *pu16_TotalLen;
    LA_SESSION_MGR_RC           session_rc;

    memset ( &st_LASearchCriteria, 0, sizeof(st_LASearchCriteria) );
    memset ( &st_LaFields, 0, sizeof(st_LaFields) );

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_SEARCH_CRITERIA:
            cmbs_api_ie_LASearchCriteriaGet( pv_IE, &st_LASearchCriteria );
            /*APP_LA_INFO_PRINT( "MatchingType:%d\n", st_LASearchCriteria.e_MatchingType );
            APP_LA_INFO_PRINT( "CaseSensitive:%d\n", st_LASearchCriteria.u8_CaseSensitive );
            APP_LA_INFO_PRINT( "Direction:%d\n", st_LASearchCriteria.u8_Direction );
            APP_LA_INFO_PRINT( "MarkEntriesReq:%d\n", st_LASearchCriteria.u8_MarkEntriesReq );
            APP_LA_INFO_PRINT( "Pattern:%s\n", st_LASearchCriteria.pu8_Pattern );*/
            break;

        case CMBS_IE_LA_ENTRY_COUNT:
            cmbs_api_ie_LAEntryCountGet( pv_IE, &u16_EntryCount );
            //APP_LA_INFO_PRINT( "Entry counter:%d\n", u16_EntryCount );
            break;

        case CMBS_IE_LA_FIELDS:
            cmbs_api_ie_LAFieldsGet( pv_IE, &st_LaFields );
            /*APP_LA_INFO_PRINT( "LA_FIELDS:\n");
            APP_LA_INFO_PRINT( "Length:%d\n", st_LaFields.u16_Length );
            APP_LA_INFO_PRINT( "FieldIds:" );
            if ( st_LaFields.u16_Length == 0 )
            {
                APP_LA_INFO_PRINT(" (none)\n");
            }
            else
            {
                for ( i = 0; i < st_LaFields.u16_Length; i++ )
                {
                    APP_LA_INFO_PRINT( " %2d", st_LaFields.pu16_FieldId[i] );
                }
                APP_LA_INFO_PRINT( "\n" );
            }*/
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    APP_LA_INFO_PRINT("Search for entry\n");

    LASessionMgr_GetDataBuffer(u16_SessionId, &pu8_Data, &pu16_Marker, &pu16_TotalLen);

    session_rc = LASessionMgr_SearchEntries(u16_SessionId, st_LASearchCriteria.e_MatchingType, st_LASearchCriteria.u8_CaseSensitive,
                                            (const char*)(st_LASearchCriteria.pu8_Pattern), !(st_LASearchCriteria.u8_Direction),
                                            st_LASearchCriteria.u8_MarkEntriesReq, &st_LaFields, &u16_EntryCount, 
                                            pu8_Data, pu16_TotalLen, &u32_EntryStartIndex);

    if ( session_rc == LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_INVALID_SESSION;
    }
    else if ( session_rc == LA_SESSION_MGR_RC_NOT_ENOUGH_MEMORY )
    {
        e_Response = CMBS_RESPONSE_ERROR;
        e_RejectReason = CMBS_LA_NACK_TEMP_NOT_POSSIBLE;
    }

    cmbs_dsr_la_SearchEntriesRes(pv_AppRef, u16_SessionId, u32_EntryStartIndex, u16_EntryCount, e_Response, e_RejectReason );

    /* Possibly need to send Missed call notification */
    LASessionMgr_SendMissedCallNotification(u16_SessionId, st_LASearchCriteria.u8_MarkEntriesReq);

    /* Possibly need to send SMS notification */    
    LASessionMgr_SendSMSNotification(u16_SessionId, st_LASearchCriteria.u8_MarkEntriesReq);

    if ( (e_Response == CMBS_RESPONSE_OK) && (u16_EntryCount > 0))
    {
        app_SendDataPackets(pv_AppRef, u16_SessionId, pu8_Data, pu16_Marker, pu16_TotalLen);
    }

    return TRUE;
}

int app_OnLaDataPacketReceive(void * pv_AppRef, void * pv_EventData)
{
    u32                     u32_NumOfEntries, u32_EntryId, u32_PositionIdx;
    E_CMBS_RESPONSE         e_Response = CMBS_RESPONSE_OK;
    E_CMBS_LA_NACK_ERRORS   e_RejectReason = CMBS_LA_NACK_MAX;
    E_CMBS_RC               r_res = CMBS_RC_OK;
    ST_IE_DATA              st_DataGet;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    u8                      u8_IsLast = 0;
    u8*                     pu8_DataBuffer;
    u16                     *pu16_Marker, *pu16_TotalLen;
    LA_SESSION_MGR_RC       session_rc;

    memset(&st_DataGet, 0, sizeof(ST_IE_DATA));
    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_LA_IS_LAST:
            cmbs_api_ie_LAIsLastGet(pv_IE, &u8_IsLast );
            //APP_LA_INFO_PRINT( "IsLAst:%d\n", u8_IsLast );
            break;

        case CMBS_IE_DATA:
            //APP_LA_INFO_PRINT("IE_DATA:\n");
            cmbs_api_ie_DataGet(pv_IE, &st_DataGet);
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    /* get the packet into the buffer related to the sessionID */
    session_rc = LASessionMgr_GetDataBuffer(u16_SessionId, &pu8_DataBuffer, &pu16_Marker, &pu16_TotalLen);
    if ( session_rc != LA_SESSION_MGR_RC_OK )
    {
        r_res = CMBS_RC_ERROR_GENERAL;
    }

    /* append data */
    memcpy(pu8_DataBuffer + *pu16_TotalLen, st_DataGet.pu8_Data, st_DataGet.u16_DataLen);
    *pu16_TotalLen += st_DataGet.u16_DataLen;

    /* send response */
    cmbs_dsr_la_DataPacketReceivedRes( pv_AppRef, u16_SessionId, r_res );

    /* if it was the last then start the parsing */
    if ( u8_IsLast && (session_rc == LA_SESSION_MGR_RC_OK) )
    {
        session_rc = LASessionMgr_SaveEntry(u16_SessionId, &u32_EntryId, &u32_PositionIdx, &u32_NumOfEntries);

        if ( session_rc == LA_SESSION_MGR_RC_ENTRY_NOT_AVAILABLE )
        {
            APP_LA_ERROR_PRINT("Error: ENTRY NOT AVAILABLE\n");

            e_Response     = CMBS_RESPONSE_ERROR;
            e_RejectReason = CMBS_LA_NACK_ENTRY_NOT_AVAILABLE;
        }
        else if ( session_rc == LA_SESSION_MGR_RC_NOT_ALLOWED )
        {
            APP_LA_ERROR_PRINT("Error: PROC NOT ALLOWED\n");

            e_Response     = CMBS_RESPONSE_ERROR;
            e_RejectReason = CMBS_LA_NACK_PROC_NOT_ALLOWED;
        }
        else
        {
            APP_LA_INFO_PRINT("Sending SaveEntry CFM: Session:%d, EntryId: %d, PositionIndex: %d, TotalEntriesNum: %d\n",
                   u16_SessionId, u32_EntryId, u32_PositionIdx, u32_NumOfEntries);
        }

        if (LA_INVALID_ENTRY_ID != LASessionMgr_GetSaveEntryID(u16_SessionId))
        {
            LASessionMgr_SetSaveEntryID(u16_SessionId, LA_INVALID_ENTRY_ID);

            cmbs_dsr_la_SaveEntryRes( pv_AppRef, u16_SessionId, u32_EntryId, u32_PositionIdx, u32_NumOfEntries, e_Response, e_RejectReason );
        }
        else if (LA_INVALID_ENTRY_ID != LASessionMgr_GetWriteEntryID(u16_SessionId))
        {
            LASessionMgr_SetWriteEntryID(u16_SessionId, LA_INVALID_ENTRY_ID);

            cmbs_dsr_la_WriteEntryRes( pv_AppRef, u16_SessionId, u32_EntryId, u32_PositionIdx, u32_NumOfEntries, e_Response, e_RejectReason );
        }

        /* Possibly need to send List Change notification */
        LASessionMgr_SendListChangedNotification(u16_SessionId, u32_EntryId);
    }

    return TRUE;
}

int app_OnLaDataPacketSendRes(void * pv_AppRef, void * pv_EventData)
{
    ST_IE_RESPONSE          st_Response;
    void *                  pv_IE = NULL;
    u16                     u16_IE;
    u16                     u16_SessionId = 0;
    u8*                     pu8_DataBuffer;
    u16                     *pu16_Marker, *pu16_TotalLen;

    cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
    while ( pv_IE != NULL )
    {
        switch ( u16_IE )
        {
        case CMBS_IE_LA_SESSION_ID:     /*!< Id of the List Access Session */
            cmbs_api_ie_LASessionIdGet( pv_IE, &u16_SessionId );
            //APP_LA_INFO_PRINT( "%d\n", u16_SessionId );
            break;

        case CMBS_IE_RESPONSE:
            cmbs_api_ie_ResponseGet( pv_IE, &st_Response );
            //APP_LA_INFO_PRINT( "%s\n", st_Response.e_Response == CMBS_RESPONSE_OK ? "Ok":"Error" );
            break;

        default:
            break;
        }
        cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
    }

    LASessionMgr_GetDataBuffer(u16_SessionId, &pu8_DataBuffer, &pu16_Marker, &pu16_TotalLen);

    if ( *pu16_Marker == *pu16_TotalLen )
    {
        APP_LA_INFO_PRINT("Last packet, No need to send more\n");

        *pu16_Marker = *pu16_TotalLen = 0;
    }
    else
    {
        APP_LA_INFO_PRINT("It was not last packet, need to send more\n");
        app_SendDataPackets(pv_AppRef, u16_SessionId, pu8_DataBuffer, pu16_Marker, pu16_TotalLen);
    }

    return TRUE;
}

int app_OnLaAddSuppListRes(void * pv_AppRef, void * pv_EventData)
{
    UNUSED_PARAMETER( pv_AppRef );
	UNUSED_PARAMETER( pv_EventData );

	if ( g_cmbsappl.n_Token )
	{
    	appcmbs_ObjectSignal( NULL, 0, 0, CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS_RES );
	}
	 
    return TRUE;
}

int app_OnTargetListChangeNotif(void * pv_AppRef, void * pv_EventData)
{
	ST_IE_TARGET_LIST_CHANGE_NOTIF  st_Notif;
	void *                          pv_IE = NULL;
	u16                             u16_IE;

	UNUSED_PARAMETER( pv_AppRef );
	memset(&st_Notif, 0, sizeof(ST_IE_TARGET_LIST_CHANGE_NOTIF));
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while ( pv_IE != NULL )
	{
		switch ( u16_IE )
		{
		case CMBS_IE_TARGET_LIST_CHANGE_NOTIF:
			cmbs_api_ie_TargetListChangeNotifGet( pv_IE, &st_Notif );
			break;

		default:
			break;
		}
		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	APP_LA_INFO_PRINT( "\n\n" );
	APP_LA_INFO_PRINT( "\n*****************************************" );
	APP_LA_INFO_PRINT( "\n*** List Change Notif *******************" );
	APP_LA_INFO_PRINT( "\n*****************************************" );
	APP_LA_INFO_PRINT( "\n\n" );

	APP_LA_INFO_PRINT( "\nList:\n" );
	switch ( st_Notif.e_ListId )
	{
	case CMBS_LA_LIST_SUPPORTED_LISTS:
		APP_LA_INFO_PRINT( "List Of Supported Lists" );
		break;

	case CMBS_LA_LIST_INTERNAL_NAMES:
		APP_LA_INFO_PRINT( "Internal Names List" );
		break;

	case CMBS_LA_LIST_DECT_SETTINGS:
		APP_LA_INFO_PRINT( "DECT System settings list" );
		break;

	default:
		APP_LA_INFO_PRINT( "\n\n Error: Unknown List ID %d %s %d", st_Notif.e_ListId, __FILE__, __LINE__ );
		break;
	}

	APP_LA_INFO_PRINT( "\n\n" );
	APP_LA_INFO_PRINT( "\nNumber of entries in list:\n%d", st_Notif.u32_NumOfEntries );

	APP_LA_INFO_PRINT( "\n\n" );
	APP_LA_INFO_PRINT( "\nEntry ID:\n%d", st_Notif.u32_EntryId );

	APP_LA_INFO_PRINT( "\n\n" );
	APP_LA_INFO_PRINT( "\nChange Type:\n" );
	switch ( st_Notif.e_ChangeType )
	{
	case CMBS_LIST_CHANGE_ENTRY_DELETED:
		APP_LA_INFO_PRINT( "Entry Deleted" );
		break;

	case CMBS_LIST_CHANGE_ENTRY_INSERTED:
		APP_LA_INFO_PRINT( "Entry Inserted" );
		break;

	case CMBS_LIST_CHANGE_ENTRY_UPDATED:
		APP_LA_INFO_PRINT( "Entry Updated" );
		break;

	default:
		APP_LA_ERROR_PRINT( "\n\n Error: Unknown Change type ID %d %s %d", st_Notif.e_ChangeType, __FILE__, __LINE__ );
		break;
	}

	APP_LA_INFO_PRINT( "\n\n" );

	return TRUE;
}

/*		========== app_LaEntity ===========
\brief		 CMBS entity to handle response information from target side
\param[in]	 pv_AppRef		 application reference
\param[in]	 e_EventID		 received CMBS event
\param[in]	 pv_EventData	 pointer to IE list
\return	 	 <int>

*/
int app_LaEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData )
{
    int res = FALSE;

    UNUSED_PARAMETER(pv_AppRef);

    switch ( e_EventID )
    {
    case CMBS_EV_DSR_LA_SESSION_START:
        res = app_OnLaSessionStart(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_SESSION_END:
        res = app_OnLaSessionEnd(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_SESSION_END_RES:
        res = app_OnLaSessionEndRes(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_QUERY_SUPP_ENTRY_FIELDS:
        res = app_OnLaQuerySuppEntryFields(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_READ_ENTRIES:
        res = app_OnLaReadEntries(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_EDIT_ENTRY:
        res = app_OnLaEditEntry(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_SAVE_ENTRY:
        res = app_OnLaSaveEntry(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_DELETE_ENTRY:
        res = app_OnLaDeleteEntry(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_DELETE_LIST:
        res = app_OnLaDeleteList(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_SEARCH_ENTRIES:
        res = app_OnLaSearchEntries(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_DATA_PACKET_RECEIVE:
        res = app_OnLaDataPacketReceive(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_DATA_PACKET_SEND_RES:
        res = app_OnLaDataPacketSendRes(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_TARGET_LIST_CHANGE_NOTIF:
        res = app_OnTargetListChangeNotif(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_ADD_TO_SUPP_LISTS_RES:
        res = app_OnLaAddSuppListRes(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_PROP_CMD:
        res = app_OnLaProprietaryCommand(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_READ_SELECTED_ENTRIES:
        res = app_OnLaReadSelectedEntries(pv_AppRef, pv_EventData);
        break;

    case CMBS_EV_DSR_LA_WRITE_ENTRY:
        res = app_OnLaWriteEntry(pv_AppRef, pv_EventData);
        break;

    default:
        return res;

    }
    return res;
}
/************************************* [End Of File] ******************************************************************************/
