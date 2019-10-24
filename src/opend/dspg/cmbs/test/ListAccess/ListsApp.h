/*************************************************************************************************************
*** ListsApp
*** An implementation of lists API over SQL database
**
**************************************************************************************************************/
#ifndef __LISTS_APP_H__
#define __LISTS_APP_H__


/*******************************************
Includes
********************************************/
#include "cmbs_api.h"
#include "SQLiteWrapper.h"
#include "cfr_debug.h"
#include "appfacility.h"
/*******************************************
Defines
********************************************/
#define LIST_NAME_MAX_LEN       30
#define LIST_NUMBER_MAX_LEN     30
#define LIST_LINE_ID_NONE       0xFFFF


/*******************************************
Types
********************************************/
typedef enum
{
    MATCH_EXACT,
    MATCH_RETURN_NEXT_ON_FAIL,
    MATCH_RETURN_PREV_ON_FAIL
}eMatchOption;

typedef enum
{
    LIST_RC_OK = 0,
    LIST_RC_FAIL,
    LIST_RC_UNSUPPORTED_LIST,
    LIST_RC_ARRAY_TOO_SMALL,
    LIST_RC_UNKNOWN_FIELD,
    LIST_RC_NO_SORT,
    LIST_RC_UNSUPPORTED_MATCH_OPTION,
    LIST_RC_INVALID_START_INDEX,
    LIST_RC_ENTRY_NOT_AVAILABLE

}LIST_RC;

typedef enum
{
    LIST_TYPE_CONTACT_LIST,
    LIST_TYPE_LINE_SETTINGS_LIST,
    LIST_TYPE_MISSED_CALLS,       
    LIST_TYPE_OUTGOING_CALLS,     
    LIST_TYPE_INCOMING_ACCEPTED_CALLS,       
    LIST_TYPE_ALL_CALLS,          
    LIST_TYPE_ALL_INCOMING_CALLS,
    LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS,
    LIST_TYPE_SMS_SETTINGS_LIST,
    LIST_TYPE_INCOMING_SMS,
    LIST_TYPE_SENT_SMS,
    LIST_TYPE_OUTGOING_SMS,
    LIST_TYPE_DRAFT_SMS,
    LIST_TYPE_DTAM_SETTINGS_LIST,
    LIST_TYPE_DTAM_WELCOME_MESSAGE,
    
    LIST_TYPE_MAX

}LIST_TYPE;

typedef enum
{
    FIELD_TYPE_CHAR,
    FIELD_TYPE_INT,
    FIELD_TYPE_TEXT,

    FIELD_TYPE_MAX

} FIELD_TYPE;

typedef enum
{
    FIELD_ID_INVALID = -1,
    FIELD_ID_ENTRY_ID,
    FIELD_ID_LAST_NAME,
    FIELD_ID_FIRST_NAME,
    FIELD_ID_CONTACT_NUM_1,
    FIELD_ID_CONTACT_NUM_1_TYPE,
    FIELD_ID_CONTACT_NUM_1_OWN,
    FIELD_ID_CONTACT_NUM_1_DEFAULT,
    FIELD_ID_CONTACT_NUM_2,
    FIELD_ID_CONTACT_NUM_2_TYPE,
    FIELD_ID_CONTACT_NUM_2_OWN,
    FIELD_ID_CONTACT_NUM_2_DEFAULT,
    FIELD_ID_CONTACT_NUM_3,
    FIELD_ID_CONTACT_NUM_3_TYPE,
    FIELD_ID_CONTACT_NUM_3_OWN,
    FIELD_ID_CONTACT_NUM_3_DEFAULT,
    FIELD_ID_ASSOCIATED_MELODY,
    FIELD_ID_LINE_ID,
    FIELD_ID_NUMBER,
    FIELD_ID_DATE_AND_TIME,
    FIELD_ID_READ_STATUS,
    FIELD_ID_LINE_NAME,
    FIELD_ID_NUM_OF_CALLS,
    FIELD_ID_CALL_TYPE,
    FIELD_ID_ATTACHED_HANDSETS,
    FIELD_ID_DIALING_PREFIX,
    FIELD_ID_FP_MELODY,
    FIELD_ID_FP_VOLUME,
    FIELD_ID_BLOCKED_NUMBER,
    FIELD_ID_MULTIPLE_CALLS_MODE,
    FIELD_ID_INTRUSION_CALL,
    FIELD_ID_PERMANENT_CLIR,
    FIELD_ID_PERMANENT_CLIR_ACTIVATION_CODE,
    FIELD_ID_PERMANENT_CLIR_DEACTIVATION_CODE,
    FIELD_ID_CALL_FWD_UNCOND,
    FIELD_ID_CALL_FWD_UNCOND_ACTIVATION_CODE,
    FIELD_ID_CALL_FWD_UNCOND_DEACTIVATION_CODE,
    FIELD_ID_CALL_FWD_UNCOND_TARGET_NUMBER,
    FIELD_ID_CALL_FWD_NO_ANSWER,
    FIELD_ID_CALL_FWD_NO_ANSWER_ACTIVATION_CODE,
    FIELD_ID_CALL_FWD_NO_ANSWER_DEACTIVATION_CODE,
    FIELD_ID_CALL_FWD_NO_ANSWER_TARGET_NUMBER,
    FIELD_ID_CALL_FWD_NO_ANSWER_TIMEOUT,
    FIELD_ID_CALL_FWD_BUSY,
    FIELD_ID_CALL_FWD_BUSY_ACTIVATION_CODE,
    FIELD_ID_CALL_FWD_BUSY_DEACTIVATION_CODE,
    FIELD_ID_CALL_FWD_BUSY_TARGET_NUMBER,
    FIELD_ID_OK_STATUS,
    FIELD_ID_LINE_USE_STATUS,
    FIELD_ID_HS_USE_STATUS,
    FIELD_ID_CALL_FWD_CFU_STATUS,
    FIELD_ID_CALL_FWD_CFNA_STATUS,
    FIELD_ID_CALL_FWD_CFB_STATUS,
    FIELD_ID_DIAGNOSTIC_ERROR_STATUS,
    FIELD_ID_DIAGNOSTIC_ERROR_TYPE,
    FIELD_ID_DIAGNOSTIC_ERROR_NUMBER,
    FIELD_ID_DTAM_IDENTIFIER,
    FIELD_ID_DTAM_TYPE,
    FIELD_ID_DTAM_LOCATION,
    FIELD_ID_DTAM_NUMBER,
    FIELD_ID_DTAM_ACTIVATION,
    FIELD_ID_DTAM_DEFAULT_TIMEOUT,
    FIELD_ID_DTAM_TIMEOUT,
    FIELD_ID_DTAM_WEB_LINK,
    FIELD_ID_DTAM_WELCOME_MSG_PARAMETERS,
    FIELD_ID_DTAM_POSITION_INDEX,
    FIELD_ID_DTAM_RECORDED_MSG_NAME,
    FIELD_ID_DTAM_TIME_DURATION_HOURS,
    FIELD_ID_DTAM_TIME_DURATION_MINUTES,
    FIELD_ID_DTAM_TIME_DURATION_SECONDS,
    FIELD_ID_NAME,
    FIELD_ID_SMS_IDENTIFIER,
    FIELD_ID_SMS_ENABLE,
    FIELD_ID_SMS_MAX_SIZE,
    FIELD_ID_SMSC_SEND_SERVER,
    FIELD_ID_SMSC_RECEIVE_SERVER,
    FIELD_ID_SMS_DELIVERY_REPORT,
    FIELD_ID_SMS_VALIDITY_PERIOD,
    FIELD_ID_SMS_CHAR_ALLOWED_ENCODING,
    FIELD_ID_SMS_ALLOWED_ENCODING_0,
 	FIELD_ID_SMS_ALLOWED_ENCODING_1,
 	FIELD_ID_SMS_ALLOWED_ENCODING_2,
 	FIELD_ID_SMS_ALLOWED_ENCODING_3,
 	FIELD_ID_SMS_ALLOWED_ENCODING_4,
 	FIELD_ID_SMS_ALLOWED_ENCODING_5,
 	FIELD_ID_SMS_ALLOWED_ENCODING_6,
 	FIELD_ID_SMS_ALLOWED_ENCODING_7,
 	FIELD_ID_SMS_ALLOWED_ENCODING_8,
 	FIELD_ID_SMS_ALLOWED_ENCODING_9,
    FIELD_ID_SMS_ALLOWED_ENCODING_10,
    FIELD_ID_SMS_ALLOWED_ENCODING_11,
    FIELD_ID_SMS_ALLOWED_ENCODING_12,
    FIELD_ID_SMS_ALLOWED_ENCODING_13,
    FIELD_ID_SMS_ALLOWED_ENCODING_14,
 	FIELD_ID_SMS_CHAR_ALLOWED_ENCODING_2,
	FIELD_ID_SMS_CHAR_ALLOWED_ENCODING_3,
	FIELD_ID_SMS_CHAR_ALLOWED_ENCODING_4,
    FIELD_ID_SMS_NETWORK_ENCODING,
    FIELD_ID_SMS_ENCODING_VARIANT_1,
    FIELD_ID_SMS_ENCODING_VARIANT_2,
    FIELD_ID_SMS_SEND_REQUEST,
    FIELD_ID_SMS_SIZE,
    FIELD_ID_SMS_CONTENT,
    FIELD_ID_SMS_TYPE,
    
    FIELD_ID_MAX

} FIELD_ID;

typedef struct
{
    const char* s_Name;
    FIELD_TYPE  e_Type;
    bool        b_AutoInc;
    bool        b_PrimaryKey;
    bool        b_Mandatory;
    bool        b_Editable;
    FIELD_ID    e_FieldId;
}stListField;

typedef enum 
{
    NUM_TYPE_FIXED,
    NUM_TYPE_MOBILE,
    NUM_TYPE_WORK
}eNumberType;

typedef struct
{
    u32     u32_EntryId;
    char    sLastName[LIST_NAME_MAX_LEN];
    char    sFirstName[LIST_NAME_MAX_LEN];
    char    sNumber1[LIST_NUMBER_MAX_LEN];
    char    cNumber1Type;
    bool    bNumber1Default;
    bool    bNumber1Own;
    char    sNumber2[LIST_NUMBER_MAX_LEN];
    char    cNumber2Type;
    bool    bNumber2Default;
    bool    bNumber2Own;
    char    sNumber3[LIST_NUMBER_MAX_LEN];
    char    cNumber3Type;
    bool    bNumber3Default;
    bool    bNumber3Own;
    u32     u32_AssociatedMelody;
    u32     u32_LineId;

}stContactListEntry;

typedef struct
{
    u32     u32_EntryId;
    const char    *psLastName;
    const char    *psFirstName;
    char    *psNumber1;
    char    cNumber1Type;
    bool    bNumber1Default;
    bool    bNumber1Own;
    const char    *psNumber2;
    char    cNumber2Type;
    bool    bNumber2Default;
    bool    bNumber2Own;
    const char    *psNumber3;
    char    cNumber3Type;
    bool    bNumber3Default;
    bool    bNumber3Own;
    u32     u32_AssociatedMelody;
    u32     u32_LineId;

}stDandAContactListEntry;

typedef enum
{
    CALL_TYPE_MISSED,
    CALL_TYPE_OUTGOING,
    CALL_TYPE_INCOMING

}eCallType;

typedef enum
{
    SMS_TYPE_INCOMING,
    SMS_TYPE_OUTGOING,
    SMS_TYPE_DRAFT,
	SMS_TYPE_SENT
}eSMSType;


typedef struct
{
    u32     u32_EntryId;
    char    sNumber[LIST_NUMBER_MAX_LEN];
    time_t  t_DateAndTime;
    bool    bRead;
    char    sLineName[LIST_NAME_MAX_LEN];   /* Stored in LineSettingsList */
    u32     u32_LineId;
    u32     u32_NumOfCalls;
    char    cCallType;
    char    sLastName[LIST_NAME_MAX_LEN];   /* stored in ContactList */
    char    sFirstName[LIST_NAME_MAX_LEN];  /* stored in ContactList */

}stCallsListEntry;

typedef struct
{
    u32     u32_EntryId;
    char    sLineName[LIST_NAME_MAX_LEN];
    u32     u32_LineId;
    u32     u32_AttachedHsMask;
    char    sDialPrefix[LIST_NUMBER_MAX_LEN];
    u32     u32_FPMelody;
    u32     u32_FPVolume;
    char    sBlockedNumber[LIST_NUMBER_MAX_LEN];
    bool    bMultiCalls;
    bool    bIntrusionCall;
    bool    bPermanentCLIR;
    char    sPermanentCLIRActCode[LIST_NUMBER_MAX_LEN];
    char    sPermanentCLIRDeactCode[LIST_NUMBER_MAX_LEN];
    bool    bCallFwdUncond;
    char    sCallFwdUncondActCode[LIST_NUMBER_MAX_LEN];
    char    sCallFwdUncondDeactCode[LIST_NUMBER_MAX_LEN];
    char    sCallFwdUncondNum[LIST_NUMBER_MAX_LEN];
    bool    bCallFwdNoAns;
    char    sCallFwdNoAnsActCode[LIST_NUMBER_MAX_LEN];
    char    sCallFwdNoAnsDeactCode[LIST_NUMBER_MAX_LEN];
    char    sCallFwdNoAnsNum[LIST_NUMBER_MAX_LEN];
    u32     u32_CallFwdNoAnsTimeout;
    bool    bCallFwdBusy;
    char    sCallFwdBusyActCode[LIST_NUMBER_MAX_LEN];
    char    sCallFwdBusyDeactCode[LIST_NUMBER_MAX_LEN];
    char    sCallFwdBusyNum[LIST_NUMBER_MAX_LEN];

}stLineSettingsListEntry;

typedef struct
{
    u32     u32_EntryId;
    u32     u32_LineId;
    u32     u32_OKStatus;
    u32     u32_LineUseStatus;    
    u32     u32_HSUseStatus;
    bool    b_CallFwdUncondStatus;
    bool    b_CallFwdNoAnswerStatus;
    bool    b_CallFwdBusyStatus;
    bool    b_DiagnosticErrorStatus;
    u32     u32_DiagnosticErrorType;
    u32     u32_DiagnosticErrorNumber;
}stLineAndDiagnosticListEntry;


typedef struct
{
    u32     u32_EntryId;
    u32     u32_LineId;
    u32     u32_DTAMIdentifier;
    bool    b_DTAMType;
    bool    b_DTAMLocation;
    char    sDTAMNumber[LIST_NUMBER_MAX_LEN];
    bool    b_DTAMActivation;
    bool    b_DTAMDefaultTimeout;
    u32     u32_DTAMTimeout;
    char    sDTAMWebLink[LIST_NUMBER_MAX_LEN];
    u32     u32_WelcomeMsgParams;
}stDTAMSettingsListEntry;

typedef struct
{
    u32     u32_EntryId;
    u32     u32_DTAMIdentifier;
    bool    b_DTAMType;
    bool    b_DTAMLocation;
    u32     u32_PositionIdx;
    char    sRecordedMsgName[LIST_NAME_MAX_LEN];
    u32     u32_TimeDurationHours;
    u32     u32_TimeDurationMinutes;
    u32     u32_TimeDurationSeconds;
}stDTAMWelcomeMessageListEntry;

typedef struct
{
    u32    u32_EntryId;
    u32    u32_SMSServiseId;    
    u32    u32_LineId;
    bool   b_EnableSMS;
    u32    u32_SMSMaxSize;
    char   sSMSCSendServer[LIST_NUMBER_MAX_LEN];
    char   sSMSCReceiveServer[LIST_NUMBER_MAX_LEN];
    bool    b_SMSDeliveryReport;
    u32    u32_SMSValidityPeriod;
    u32    u32_LengthOfAllowedEncoding_1;
    u32    u32_SMSCharacterEncodingValue_1;
    u32    u32_AllowedEncoding_0;
    u32    u32_AllowedEncoding_1;
    u32    u32_AllowedEncoding_2;
    u32    u32_AllowedEncoding_3;
    u32    u32_AllowedEncoding_4;
    u32    u32_AllowedEncoding_5;
    u32    u32_AllowedEncoding_6;
    u32    u32_AllowedEncoding_7;
    u32    u32_AllowedEncoding_8;
    u32    u32_AllowedEncoding_9;
    u32    u32_AllowedEncoding_10;
    u32    u32_AllowedEncoding_11;
    u32    u32_AllowedEncoding_12;
    u32    u32_AllowedEncoding_13;
    u32    u32_AllowedEncoding_14;
    u32    u32_LengthOfAllowedEncoding_2;
    u32    u32_SMSCharacterEncodingValue_2;  
    u32    u32_LengthOfAllowedEncoding_3;
    u32    u32_SMSCharacterEncodingValue_3;    
    u32    u32_LengthOfAllowedEncoding_4;
    u32    u32_SMSCharacterEncodingValue_4;
}stSMSSettingsListEntry;

#define LIST_MAX_SMS_SIZE 100

typedef struct
{
    u32 u32_EntryId;
    char sNumber[LIST_NUMBER_MAX_LEN];
    char sName[LIST_NUMBER_MAX_LEN];
    time_t  t_DateAndTime;
    bool b_Read;
    bool b_SendRequest;
    u32 u32_SMSServiseId; 
    u32 u32_NetworkSideEncoding;
    u32 u32_EncVariant_1;
    u32 u32_EncVariant_2;
    u32 u32_SMSSize;
    char sSMSContent[LIST_MAX_SMS_SIZE];
    char cSMSType;
}stSMSListEntry;

typedef struct
{
     bool    b_Default;         //0x0
     bool    b_Turkish;         //0x1
     bool    b_Spanish;         //0x2
     bool    b_Portuguese;  	//0x3
     bool    b_Bengali;         //0x4
     bool    b_Gujarati;        //0x5
     bool    b_Hindi;           //0x6
     bool    b_Kannada;         //0x7
     bool    b_Malayalam;    	//0x8
     bool    b_Oriya;           //0x9
     bool    b_Punjabi;         //0x10
     bool    b_Tamil;           //0x11
     bool    b_Telugu;          //0x12
     bool    b_Urdu;            //0x13
     bool    b_Reserved;        //0x14
}stAllowedEncodingVariant;

typedef enum
{   
    CMBS_SMS_ENC_TYPE_RESERVED,
    CMBS_SMS_ENC_TYPE_GSM_7_BIT,
    CMBS_SMS_ENC_TYPE_GSM_8_BIT,    
    CMBS_SMS_ENC_TYPE_UCS_2,    
    CMBS_SMS_ENC_TYPE_UTF_8,

    CMBS_SMS_ENC_TYPE_MAX,    
}e_SMSCharacterEncodingType;

typedef enum
{
    MARK_LEAVE_UNCHANGED,
    MARK_CLEAR,
    MARK_SET
} eMarkRequest;

/* Entry Max Size */
typedef union
{
    stContactListEntry          ContactListEntry;
    stCallsListEntry            CallsListEntry;
    stLineSettingsListEntry     LineSettingsListEntry;
    stSMSListEntry              SMSListEntry;
}uEntryTypes;

#define LIST_ENTRY_MAX_SIZE sizeof(uEntryTypes)

/*******************************************
Globals
********************************************/


/*******************************************
List API
********************************************/
/* Initialize Lists Application */
LIST_RC List_Init(void);

/* Destory Lists Application */
LIST_RC List_Close(void);

/* Create List in database */
LIST_RC List_CreateList(IN LIST_TYPE ListType);

/* Get number of entries */
LIST_RC List_GetCount(IN LIST_TYPE ListType, OUT u32* pu32_Count);

/* Get Supported fields */
LIST_RC List_GetSupportedFields(IN LIST_TYPE ListType,
                                OUT u32 pu32_EditableFieldsIds[],  INOUT u32* pu32_EditableSize,       /* Editable Fields */
                                OUT u32 pu32_NonEditableFields[],  INOUT u32* pu32_NonEditableSize     /* Non-Editable Fields */);

/* Insert *NEW* Entry
 
   *NOTE: For calls list, the data base automatically sets the 'call type' field, according to list type*
 
   Input Arguments:
   ------------------
   ListType         - List to insert entry
   pv_Entry         - pointer to actual entry
   pu32Fields       - Fields used in provided entry (array of Ids)
   u32_FieldsSize   - size of array
 
   Output Arguments:
   ------------------
   pu32_EntryId     - ID of inserted entry
*/
LIST_RC List_InsertEntry(IN LIST_TYPE ListType, IN void* pv_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT u32* pu32_EntryId);

/* Delete a single entry from the database */
LIST_RC List_DeleteEntry(IN LIST_TYPE ListType, IN u32 u32_EntryId);

/* Delete all entries from the database */
LIST_RC List_DeleteAllEntries(IN LIST_TYPE ListType);

/* Update (existing) Entry */
LIST_RC List_UpdateEntry(IN LIST_TYPE ListType, IN void* pv_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize, IN u32 u32_EntryId);

/* Read entries
 
   Input Arguments:
   ------------------------
   ListType             - requested list
   pu32_StartIndex      - start index in respect to sorting fields  
   bForward             - Direction of read
   eMark                - Flag for requesting resetting (or setting) of the 'Read status' field for all read entries
   pu32Fields           - Array of requested fields
   u32_FieldsSize       - size of pu32Fields
   u32_SortField1       - most significant sort field
   u32_SortField2       - least significant sort field (use invalid_id if not needed)
   u32_EntriesSize      - number of requested entries
 
   Output Arguments:
   ------------------------
    pv_Entries          - array of read entries
    pu32_EntriesSize    - size of pv_Entries
    pu32_StartIndex     - the minimum returned entries' indices
*/
LIST_RC List_ReadEntries(IN LIST_TYPE ListType, INOUT u32* pu32_StartIndex, IN bool bForward, IN eMarkRequest eMark, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                         IN u32 u32_SortField1, IN u32 u32_SortField2, OUT void* pv_Entries, INOUT u32* pu32_EntriesSize);

/* Read entry by ID
 
   Input Arguments:
   ------------------------
   ListType             - requested list
   u32_EntryId          - Id Of requested entry
   pu32Fields           - Array of requested fields
   u32_FieldsSize       - size of pu32Fields
 
   Output Arguments:
   ------------------------
    pv_Entry          - pointer to read entry
*/
LIST_RC List_ReadEntryById(IN LIST_TYPE ListType, IN u32 u32_EntryId, IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entry);

/* Search entries
 
   Input Arguments:
   ------------------------
   ListType             - requested list
   eMatchOption         - what to return in case of no exact match (return nothing, return first greater than, return first lesser than)
   bCaseSensitive       - case sensitivity of the search
   sSearchedVal         - the value to search for
   bForward             - Direction of read
   eMark                - Flag for requesting resetting (or setting) of the 'Read status' field for all read entries
   pu32Fields           - Array of requested fields
   u32_FieldsSize       - size of pu32Fields
   u32_SortField1       - most significant sort field
   u32_SortField2       - least significant sort field (use invalid_id if not needed)
   u32_EntriesSize      - number of requested entries
 
   Output Arguments:
   ------------------------
    pv_Entries          - array of read entries
    pu32_EntriesSize    - size of pv_Entries
    pu32_StartIndex     - index of first returned entry
*/
LIST_RC List_SearchEntries(IN LIST_TYPE ListType, IN eMatchOption eMatching, IN bool bCaseSensitive, IN const char* sSearchedVal, 
                           IN bool bForward, IN eMarkRequest eMark, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                           IN u32 u32_SortField1, IN u32 u32_SortField2, OUT void* pv_Entries, INOUT u32* pu32_EntriesSize,
                           OUT u32* pu32_StartIndex); 


/* Get Entry Index */ 
LIST_RC List_GetEntryIdex(IN LIST_TYPE ListType, IN u32 u32_SortField1, IN u32 u32_SortField2, IN u32 u32_EntryId, OUT u32* pu32_Index);

/*******************************************
Auxiliary
********************************************/
/* prepare strings of fields and values from the given Entry */
LIST_RC List_PrepareContactListEntryForInsert(IN stContactListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                              OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                              OUT char sValues[], INOUT u32* pu32_ValuesSize);
LIST_RC List_PrepareCallsListEntryForInsert(IN LIST_TYPE ListType, IN stCallsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                            OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                            OUT char sValues[], INOUT u32* pu32_ValuesSize);
LIST_RC List_PrepareLineSettingsListEntryForInsert(IN stLineSettingsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                                   OUT char sValues[], INOUT u32* pu32_ValuesSize);

LIST_RC List_PrepareLineAndDiagnosticistEntryForInsert(IN stLineAndDiagnosticListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                                   OUT char sValues[], INOUT u32* pu32_ValuesSize);

LIST_RC List_PrepareSMSSettingsListEntryForInsert(IN stSMSSettingsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                              OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                              OUT char sValues[], INOUT u32* pu32_ValuesSize);

LIST_RC List_PrepareSMSListEntryForInsert(IN LIST_TYPE ListType, IN stSMSListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                            OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                            OUT char sValues[], INOUT u32* pu32_ValuesSize);

LIST_RC List_PrepareDTAMSettingsListEntryForInsert(IN stDTAMSettingsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                              OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                              OUT char sValues[], INOUT u32* pu32_ValuesSize);

LIST_RC List_PrepareDTAMWelcomeMsgListEntryForInsert(IN stDTAMWelcomeMessageListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                              OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                              OUT char sValues[], INOUT u32* pu32_ValuesSize);

LIST_RC List_PrepareContactListEntryForUpdate(IN stContactListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                              OUT char sFields[], INOUT u32* pu32_FieldsSize);
LIST_RC List_PrepareCallsListEntryForUpdate(IN stCallsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                            OUT char sFields[], INOUT u32* pu32_FieldsSize);
LIST_RC List_PrepareLineSettingsListEntryForUpdate(IN stLineSettingsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize);

LIST_RC List_PrepareLineAndDiagnosticEntryForUpdate(IN stLineAndDiagnosticListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize);

LIST_RC List_PrepareSMSSettingsListEntryForUpdate(IN stSMSSettingsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize);

LIST_RC List_PrepareSMSListEntryForUpdate(IN stSMSListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize);

LIST_RC List_PrepareDTAMSettingsListEntryForUpdate(IN stDTAMSettingsListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize);

LIST_RC List_PrepareDTAMWelcomeMsgListEntryForUpdate(IN stDTAMWelcomeMessageListEntry* pst_Entry, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                                   OUT char sFields[], INOUT u32* pu32_FieldsSize);


/* prepare string with fields names from the given field Ids */
LIST_RC List_PrepareFieldsNamesForRead(IN LIST_TYPE ListType, IN u32 pu32Fields[], IN u32 u32_FieldsSize,
                                       OUT char sFields[], INOUT u32* pu32_FieldsSize,
                                       OUT bool* pb_JoinContactList, OUT bool* pb_JoinLineSettingsList);

/* Fill Entry */
LIST_RC List_FillEntry(IN LIST_TYPE ListType, IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                       IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);
LIST_RC List_FillContactListEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                  IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);
LIST_RC List_FillCallsListEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);
LIST_RC List_FillLineSettingsEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                   IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);

LIST_RC List_FillLineDiagnosticEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                   IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);

LIST_RC List_FillSMSSettingsListEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                   IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);

LIST_RC List_FillSMSListEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);

LIST_RC List_FillDTAMSettingsListEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                   IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);

LIST_RC List_FillDTAMWelcomeMsgEntry(IN u32 pu32Fields[], IN u32 u32_FieldsSize, OUT void* pv_Entries, IN u32 u32_EntriesOffset,
                                   IN void* pv_SelectHandle, IN PST_COLUMN_MAP pst_Map);

/* Attached Handsets */
LIST_RC List_GetAttachedHs(IN u32 u32_LineId, OUT u32* pu32_AttachedHsMask);

LIST_RC List_SetAttachedHs(IN u32 u32_LineId, IN u32 u32_AttachedHsMask);

/* Missed Calls Num Of Read / Unread Entries */
LIST_RC List_GetMissedCallsNumOfEntries(IN u32 u32_LineId, OUT u32* pu32_NumOfUnread, OUT u32* pu32_NumOfRead);

/* Add HS to first line's Attached HS mask */
LIST_RC List_AddHsToFirstLine(IN u8 u8_HsNum);

/* Remove HS from all lines' Attahced HS mask */
LIST_RC List_RemoveHsFromAllLines(IN u8 u8_HsNum);

/* Returns the first line that the input HS is attached to, 
   or the first line in the system if the HS is not attached to any line */
LIST_RC List_GetDefaultLine(IN u8 u8_HsNum, OUT u32* pu32_LineId);

/* returns list of lines the input HS is attached to */
LIST_RC List_GetLinesOfHS( IN u8 u8_HsNum, OUT u32 pu32_LineId[], INOUT u32* pu32_LineIdLen );

/* returns Call Forward Enabled / Diabled for the specified line */
LIST_RC List_GetCallForwardMode( IN u32 u32_LineID, OUT u8* pb_CFUncondEn, OUT u8* pb_CFNoAnsEn,OUT u8* pb_CFBusyEn );

/* adds default Line entry with 5 attached HS */
LIST_RC List_AddDefaultLine();

/* Checks Line Use status, if changed - updates data base and sends notification */
bool 	LineAndDiagnosticListUpdateLineUse(u8 LineId, E_CMBS_LINE_DIAG_STATUS eLineStatus);

/* Updates HS Use status in data base */
LIST_RC LineAndDiagnosticListUpdateHSUse(u8 LineId, u32 u32_HSNum, bool b_InUse);

/* Updates OK status in data base */
LIST_RC LineAndDiagnosticListUpdateOKStatus(u8 LineId, E_CMBS_LINE_DIAG_OK_STATUS e_DiagErrorStatus);

/* Updates Call Forward status in data base */
LIST_RC LineAndDiagnosticListUpdateCallFWD(u8 LineId, FIELD_ID FieldId, bool b_Status);

/* Updates Diagnostic Error status in data base */
LIST_RC LineAndDiagnosticListUpdateDiagnosticErrorStatus(u8 LineId, bool b_Status, E_CMBS_LINE_DIAG_ERROR_TYPE e_ErrorType, u8 u8_ErrorNumber);

/* Add "system" entry to Line and diagnostic status list */
LIST_RC List_AddLineAndDiagnosticSystemEntry();

/* Clears Line and HS use status fields in line and diagnostic status list entry */
LIST_RC List_ClearLineAndDiagnosticSystemEntry();

/* Return current line use status from line and diagnostic status list */
E_CMBS_LINE_DIAG_STATUS LineAndDiagnosticListGetLineUseStatus(u8 LineId);

/* Return Number of read and unread entries*/
LIST_RC List_GetSMSNumOfEntries(IN u32 u32_SMSId, OUT u32 * pu32_NumOfUnread, OUT u32 * pu32_NumOfRead);

/* Return current HS use status from line and diagnostic status list */
u32 LineAndDiagnosticListGetHSUseStatus(u8 LineId);

LIST_RC List_GetOutgoingCallsNumOfEntries(IN u32 u32_LineId, OUT u32* pu32_NumOfEntries);


/* Set Originating PP for List Change Details */
LIST_RC List_SetListChangeOriginatingHandset( IN LIST_TYPE ListType, IN u8 u8_HsNum);

/* Add List Change Details */
LIST_RC List_AddListChangeDetails( IN LIST_TYPE ListType, IN ST_APP_FACILITY_LCN_SUB_DETAILS *pst_LcnSubDetails);

/* Reset List Change Details for all lists */
LIST_RC List_ResetAllListChangeDetails( void );

/* Reset List Change Details for specific list */
LIST_RC List_ResetListChangeDetails( IN LIST_TYPE ListType);

/* Get List Change Details for specific list */
LIST_RC List_GetListChangeDetails( IN LIST_TYPE ListType, OUT ST_APP_FACILITY_LCN_DETAILS **pst_LcnDetails);


#endif /* __LISTS_APP_H__ */

/* End Of File *****************************************************************************************************************************/


