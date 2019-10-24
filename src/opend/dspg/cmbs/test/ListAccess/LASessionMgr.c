/*************************************************************************************************************
*** List Access Session Manager
**
**************************************************************************************************************/

/*******************************************
Includes
********************************************/
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "LASessionMgr.h"
#include "ListsApp.h"
#include "appla2.h"
#include "ListChangeNotif.h"
#include "appsrv.h"
#include "appfacility.h"

/*******************************************
Defines
********************************************/
#ifndef NBS
#define LA_SESSION_MGR_MAX_SESSIONS 5
#else
#define LA_SESSION_MGR_MAX_SESSIONS 10
#endif

#define LA_SESSION_MGR_MAX_FIELDS   32

#define LA_SESSION_MGR_MAX_ENTRIES_PER_READ  10

#define LA_SESSION_UNDEFINED_SESSION_ID 0xFFFF

#define CHECK_SESSION_ID(u16_SessionId)             \
    s32_Index = GetIndexBySessionId(u16_SessionId); \
                                                    \
    if (s32_Index == -1)                            \
    {                                               \
        return LA_SESSION_MGR_RC_UNKNOWN_SESSION_ID;\
    }

/*******************************************
Types
********************************************/

/*******************************************
Globals
********************************************/
/* Sessions */
static stLASession g_pst_LASessions[LA_SESSION_MGR_MAX_SESSIONS];

/*******************************************
Auxiliary
********************************************/
static bool AppFieldNonEditable(IN u32 u32_AppFieldId)
{
    if ( u32_AppFieldId == FIELD_ID_ENTRY_ID        ||
         u32_AppFieldId == FIELD_ID_NUMBER          ||
         u32_AppFieldId == FIELD_ID_DATE_AND_TIME   ||
         u32_AppFieldId == FIELD_ID_LINE_ID )
    {
        return TRUE;
    }
    return FALSE;
}

static s32 GetIndexBySessionId(IN u32 u32_SessionId)
{
    u32 u32_Index;
    for ( u32_Index = 0; u32_Index < LA_SESSION_MGR_MAX_SESSIONS; ++u32_Index )
    {
        if ( g_pst_LASessions[u32_Index].u32_SessionId == u32_SessionId )
        {
            return u32_Index;
        }
    }

    /* not found */
    return -1;
}

static s32 CATiqListId_2_AppListId(IN u32 u32_CATiqListId)
{
    switch ( u32_CATiqListId )
    {
    case  CMBS_LA_LIST_MISSED_CALLS:      
        return LIST_TYPE_MISSED_CALLS;

    case  CMBS_LA_LIST_OUTGOING_CALLS:     
        return LIST_TYPE_OUTGOING_CALLS;

    case  CMBS_LA_LIST_INCOMING_ACC:       
        return LIST_TYPE_INCOMING_ACCEPTED_CALLS;

    case  CMBS_LA_LIST_ALL_CALLS:          
        return LIST_TYPE_ALL_CALLS;

    case  CMBS_LA_LIST_CONTACT:            
        return LIST_TYPE_CONTACT_LIST;

    case  CMBS_LA_LIST_LINE_SETTINGS:      
        return LIST_TYPE_LINE_SETTINGS_LIST;

    case  CMBS_LA_LIST_ALL_INCOMING_CALLS: 
        return LIST_TYPE_ALL_INCOMING_CALLS;

    case  CMBS_LA_LIST_LINE_AND_DIAGNOSTIC_STATUS: 
        return LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS;
        
    case CMBS_LA_LIST_SMS_SETTINGS:
        return LIST_TYPE_SMS_SETTINGS_LIST;
        
    case  CMBS_LA_LIST_INCOMING_SMS: 
        return LIST_TYPE_INCOMING_SMS;

    case  CMBS_LA_LIST_SENT_SMS: 
        return LIST_TYPE_SENT_SMS;

    case  CMBS_LA_LIST_OUTGOING_SMS: 
        return LIST_TYPE_OUTGOING_SMS;

    case  CMBS_LA_LIST_DRAFT_SMS: 
        return LIST_TYPE_DRAFT_SMS;

    case  CMBS_LA_LIST_DTAM_SETTINGS: 
        return LIST_TYPE_DTAM_SETTINGS_LIST;
        
    case  CMBS_LA_LIST_DTAM_WELCOME_MESSAGE: 
        return LIST_TYPE_DTAM_WELCOME_MESSAGE;
     
    default:
        // unsupported list
        return LIST_TYPE_MAX; 
    }
}

static s32 AppListId_2_CATiqListId(IN u32 u32_AppListId)
{
    switch ( u32_AppListId )
    {
    case  LIST_TYPE_MISSED_CALLS:      
        return CMBS_LA_LIST_MISSED_CALLS;

    case  LIST_TYPE_OUTGOING_CALLS:     
        return CMBS_LA_LIST_OUTGOING_CALLS;

    case  LIST_TYPE_INCOMING_ACCEPTED_CALLS:       
        return CMBS_LA_LIST_INCOMING_ACC;

    case  LIST_TYPE_ALL_CALLS:          
        return CMBS_LA_LIST_ALL_CALLS;

    case  LIST_TYPE_CONTACT_LIST:            
        return CMBS_LA_LIST_CONTACT;

    case  LIST_TYPE_LINE_SETTINGS_LIST:      
        return CMBS_LA_LIST_LINE_SETTINGS;

    case  LIST_TYPE_ALL_INCOMING_CALLS: 
        return CMBS_LA_LIST_ALL_INCOMING_CALLS;

    case  LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS: 
        return CMBS_LA_LIST_LINE_AND_DIAGNOSTIC_STATUS;

    case LIST_TYPE_SMS_SETTINGS_LIST:
        return CMBS_LA_LIST_SMS_SETTINGS;
        
    case  LIST_TYPE_INCOMING_SMS: 
        return CMBS_LA_LIST_INCOMING_SMS;

    case  LIST_TYPE_SENT_SMS: 
        return CMBS_LA_LIST_SENT_SMS;

    case  LIST_TYPE_OUTGOING_SMS: 
        return CMBS_LA_LIST_OUTGOING_SMS;

    case  LIST_TYPE_DRAFT_SMS: 
        return CMBS_LA_LIST_DRAFT_SMS;
        
    case  LIST_TYPE_DTAM_SETTINGS_LIST: 
        return CMBS_LA_LIST_DTAM_SETTINGS;

    case  LIST_TYPE_DTAM_WELCOME_MESSAGE: 
        return CMBS_LA_LIST_DTAM_WELCOME_MESSAGE;        
        
    default:
        return -1;
    }
}

static s32 CATiqFieldId_2_AppFieldId(IN u32 u32_ListId, IN u32 u32_FieldId)
{
    u32_ListId = (u32)AppListId_2_CATiqListId(u32_ListId);
    switch ( u32_ListId )
    {
    case  CMBS_LA_LIST_MISSED_CALLS:  
        switch ( u32_FieldId )
        {
        case CMBS_MCL_FLD_NUMBER:           return FIELD_ID_NUMBER;
        case CMBS_MCL_FLD_NAME:             return FIELD_ID_LAST_NAME;
        case CMBS_MCL_FLD_DATETIME:         return FIELD_ID_DATE_AND_TIME;
        case CMBS_MCL_FLD_NEW:              return FIELD_ID_READ_STATUS;
        case CMBS_MCL_FLD_LINE_NAME:        return FIELD_ID_LINE_NAME; 
        case CMBS_MCL_FLD_LINE_ID:          return FIELD_ID_LINE_ID;
        case CMBS_MCL_FLD_NR_OF_CALLS:      return FIELD_ID_NUM_OF_CALLS;
        default:                            return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_OUTGOING_CALLS:     
        switch ( u32_FieldId )
        {
        case CMBS_OCL_FLD_NUMBER:           return FIELD_ID_NUMBER;
        case CMBS_OCL_FLD_NAME:             return FIELD_ID_LAST_NAME;
        case CMBS_OCL_FLD_DATETIME:         return FIELD_ID_DATE_AND_TIME;
        case CMBS_OCL_FLD_LINE_NAME:        return FIELD_ID_LINE_NAME;
        case CMBS_OCL_FLD_LINE_ID:          return FIELD_ID_LINE_ID; 
        default:                            return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_INCOMING_ACC: 
        switch ( u32_FieldId )
        {
        case CMBS_IACL_FLD_NUMBER:           return FIELD_ID_NUMBER;
        case CMBS_IACL_FLD_NAME:             return FIELD_ID_LAST_NAME;
        case CMBS_IACL_FLD_DATETIME:         return FIELD_ID_DATE_AND_TIME;
        case CMBS_IACL_FLD_LINE_NAME:        return FIELD_ID_LINE_NAME;
        case CMBS_IACL_FLD_LINE_ID:          return FIELD_ID_LINE_ID; 
        default:                             return FIELD_ID_INVALID;
        }      
        break;

    case  CMBS_LA_LIST_ALL_CALLS: 
        switch ( u32_FieldId )
        {
        case CMBS_ACL_FLD_CALL_TYPE:    return FIELD_ID_CALL_TYPE;
        case CMBS_ACL_FLD_NUMBER:       return FIELD_ID_NUMBER;
        case CMBS_ACL_FLD_NAME:         return FIELD_ID_LAST_NAME;
        case CMBS_ACL_FLD_DATETIME:     return FIELD_ID_DATE_AND_TIME;
        case CMBS_ACL_FLD_LINE_NAME:    return FIELD_ID_LINE_NAME; 
        case CMBS_ACL_FLD_LINE_ID:      return FIELD_ID_LINE_ID;
        default:                        return FIELD_ID_INVALID;
        }      
        break;

    case  CMBS_LA_LIST_CONTACT: 
        switch ( u32_FieldId )
        {
        case CMBS_CL_FLD_NAME:          return FIELD_ID_LAST_NAME;
        case CMBS_CL_FLD_FIRST_NAME:    return FIELD_ID_FIRST_NAME;
        case CMBS_CL_FLD_CONTACT_NR:    return FIELD_ID_CONTACT_NUM_1;
        case CMBS_CL_FLD_ASSOC_MDY:     return FIELD_ID_ASSOCIATED_MELODY;
        case CMBS_CL_FLD_LINE_ID:       return FIELD_ID_LINE_ID; 
        default:                        return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_LINE_SETTINGS:
        switch ( u32_FieldId )
        {
        case CMBS_LSL_FLD_LINE_NAME:            return FIELD_ID_LINE_NAME;
        case CMBS_LSL_FLD_LINE_ID:              return FIELD_ID_LINE_ID;
        case CMBS_LSL_FLD_ATTACHED_HANDSETS:    return FIELD_ID_ATTACHED_HANDSETS;
        case CMBS_LSL_FLD_DIALING_PREFIX:       return FIELD_ID_DIALING_PREFIX;
        case CMBS_LSL_FLD_FP_MELODY:            return FIELD_ID_FP_MELODY; 
        case CMBS_LSL_FLD_FP_VOLUME:            return FIELD_ID_FP_VOLUME;
        case CMBS_LSL_FLD_BLOCKED_NUMBER:       return FIELD_ID_BLOCKED_NUMBER;
        case CMBS_LSL_FLD_MULTI_CALL_MODE:      return FIELD_ID_MULTIPLE_CALLS_MODE;
        case CMBS_LSL_FLD_INTRUSION_CALL:       return FIELD_ID_INTRUSION_CALL;
        case CMBS_LSL_FLD_PERMANENT_CLIR:       return FIELD_ID_PERMANENT_CLIR; 
        case CMBS_LSL_FLD_CALL_FORWARDING:      return FIELD_ID_CALL_FWD_UNCOND;
        case CMBS_LSL_FLD_CFNA:                 return FIELD_ID_CALL_FWD_NO_ANSWER;
        case CMBS_LSL_FLD_CFB:                  return FIELD_ID_CALL_FWD_BUSY;
        default:                                return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_ALL_INCOMING_CALLS: 
        switch ( u32_FieldId )
        {
        case CMBS_AICL_FLD_NUMBER:           return FIELD_ID_NUMBER;
        case CMBS_AICL_FLD_NAME:             return FIELD_ID_LAST_NAME;
        case CMBS_AICL_FLD_DATETIME:         return FIELD_ID_DATE_AND_TIME;
        case CMBS_AICL_FLD_NEW:              return FIELD_ID_READ_STATUS;
        case CMBS_AICL_FLD_LINE_NAME:        return FIELD_ID_LINE_NAME; 
        case CMBS_AICL_FLD_LINE_ID:          return FIELD_ID_LINE_ID;
        case CMBS_AICL_FLD_NR_OF_CALLS:      return FIELD_ID_NUM_OF_CALLS;
        default:                             return FIELD_ID_INVALID;
        }
        break;
        
    case  CMBS_LA_LIST_LINE_AND_DIAGNOSTIC_STATUS: 
        switch ( u32_FieldId )
        {
        case CMBS_LADSL_FLD_LINE_ID:           	return FIELD_ID_LINE_ID;
        case CMBS_LADSL_FLD_OK_STATUS:         	return FIELD_ID_OK_STATUS;
        case CMBS_LADSL_FLD_LINE_USE_STATUS:   	return FIELD_ID_LINE_USE_STATUS;
        case CMBS_LADSL_FLD_HS_USE_STATUS:     	return FIELD_ID_HS_USE_STATUS;
        case CMBS_LADSL_FLD_CALL_FORWARDING:   	return FIELD_ID_CALL_FWD_CFU_STATUS; 
        case CMBS_LADSL_FLD_DIAGNOSTIC_ERROR:  	return FIELD_ID_DIAGNOSTIC_ERROR_STATUS;
        default:                             	return FIELD_ID_INVALID;
        }
        break;       

    case  CMBS_LA_LIST_DTAM_SETTINGS: 
        switch ( u32_FieldId )
        {
        case CMBS_DSL_FLD_LINE_ID:           			return FIELD_ID_LINE_ID;
        case CMBS_DSL_FLD_DTAM_ID:         				return FIELD_ID_DTAM_IDENTIFIER;
        case CMBS_DSL_FLD_DTAM_NUMBER:   				return FIELD_ID_DTAM_NUMBER;
        case CMBS_DSL_FLD_DTAM_ACTIVATION:     			return FIELD_ID_DTAM_ACTIVATION;
        case CMBS_DSL_FLD_DTAM_WEBLINK:   				return FIELD_ID_DTAM_WEB_LINK; 
        case CMBS_DSL_FLD_DTAM_WELCOME_MSG_PARAMS:  	return FIELD_ID_DTAM_WELCOME_MSG_PARAMETERS;
        default:                             			return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_DTAM_WELCOME_MESSAGE: 
        switch ( u32_FieldId )
        {
        case CMBS_DWML_FLD_DTAM_ID:           	return FIELD_ID_DTAM_IDENTIFIER;
        case CMBS_DWML_FLD_POSITION_IDX:        return FIELD_ID_DTAM_POSITION_INDEX;
        case CMBS_DWML_FLD_MSG_NAME:   			return FIELD_ID_DTAM_RECORDED_MSG_NAME;
        case CMBS_DWML_FLD_TIME_DURATION:     	return FIELD_ID_DTAM_TIME_DURATION_HOURS;
        default:                             	return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_SMS_SETTINGS: 
        switch ( u32_FieldId )
        {
        case CMBS_SSL_FLD_SMS_ID:             		return FIELD_ID_SMS_IDENTIFIER;
        case CMBS_SSL_FLD_LINE_ID:              	return FIELD_ID_LINE_ID;
        case CMBS_SSL_FLD_ENABLE_SMS:            	return FIELD_ID_SMS_ENABLE;
        case CMBS_SSL_FLD_MAX_SMS_SIZE:        		return FIELD_ID_SMS_MAX_SIZE;
        case CMBS_SSL_FLD_SMSC_SEND_SERVER:     	return FIELD_ID_SMSC_SEND_SERVER; 
        case CMBS_SSL_FLD_SMSC_RECEIVE_SERVER:   	return FIELD_ID_SMSC_RECEIVE_SERVER;
        case CMBS_SSL_FLD_SMS_DELIVERY_REPORT:     	return FIELD_ID_SMS_DELIVERY_REPORT;
        case CMBS_SSL_FLD_SMS_VALIDITY_PERIOD:     	return FIELD_ID_SMS_VALIDITY_PERIOD;
        case CMBS_SSL_FLD_ALLOWED_ENCODING:        	return FIELD_ID_SMS_CHAR_ALLOWED_ENCODING;       
        default:                             		return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_INCOMING_SMS: 
        switch ( u32_FieldId )
        {
        case CMBS_ISMSL_FLD_NUMBER:        		return FIELD_ID_NUMBER;
        case CMBS_ISMSL_FLD_NAME:               return FIELD_ID_NAME;
        case CMBS_ISMSL_FLD_DATETIME:           return FIELD_ID_DATE_AND_TIME;
        case CMBS_ISMSL_FLD_READ_STATUS:        return FIELD_ID_READ_STATUS;
        case CMBS_ISMSL_FLD_SMS_ID:             return FIELD_ID_SMS_IDENTIFIER; 
        case CMBS_ISMSL_FLD_SMS_SIZE:        	return FIELD_ID_SMS_SIZE;
        case CMBS_ISMSL_FLD_SMS_CONTENT:        return FIELD_ID_SMS_CONTENT;
        default:                             	return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_SENT_SMS: 
        switch ( u32_FieldId )
        {
        case CMBS_SSMSL_FLD_NUMBER:   			return FIELD_ID_NUMBER;
        case CMBS_SSMSL_FLD_NAME:         		return FIELD_ID_NAME;
        case CMBS_SSMSL_FLD_DATETIME:       	return FIELD_ID_DATE_AND_TIME;
        case CMBS_SSMSL_FLD_SMS_ID:             return FIELD_ID_SMS_IDENTIFIER;
        case CMBS_SSMSL_FLD_NETWORK_ENC:        return FIELD_ID_SMS_NETWORK_ENCODING; 
        case CMBS_SSMSL_FLD_SMS_SIZE: 			return FIELD_ID_SMS_SIZE;
        case CMBS_SSMSL_FLD_SMS_CONTENT:        return FIELD_ID_SMS_CONTENT;
        default:                             	return FIELD_ID_INVALID;
        }
        break;
        
    case  CMBS_LA_LIST_OUTGOING_SMS: 
        switch ( u32_FieldId )
        {
        case CMBS_OSMSL_FLD_NUMBER:          	return FIELD_ID_NUMBER;
        case CMBS_OSMSL_FLD_NAME:            	return FIELD_ID_NAME;
        case CMBS_OSMSL_FLD_DATETIME:         	return FIELD_ID_DATE_AND_TIME;
        case CMBS_OSMSL_FLD_SMS_ID:             return FIELD_ID_SMS_IDENTIFIER;
        case CMBS_OSMSL_FLD_NETWORK_ENC:        return FIELD_ID_SMS_NETWORK_ENCODING; 
        case CMBS_OSMSL_FLD_SMS_SIZE:           return FIELD_ID_SMS_SIZE;
        case CMBS_OSMSL_FLD_SMS_CONTENT:        return FIELD_ID_SMS_CONTENT;
        default:                             	return FIELD_ID_INVALID;
        }
        break;

    case  CMBS_LA_LIST_DRAFT_SMS: 
        switch ( u32_FieldId )
        {
        case CMBS_DSMSL_FLD_NUMBER:     		return FIELD_ID_NUMBER;
        case CMBS_DSMSL_FLD_NAME:        		return FIELD_ID_NAME;
        case CMBS_DSMSL_FLD_DATETIME:   		return FIELD_ID_DATE_AND_TIME;
        case CMBS_DSMSL_FLD_SMS_ID:          	return FIELD_ID_SMS_IDENTIFIER;
        case CMBS_DSMSL_FLD_SEND_REQUEST:       return FIELD_ID_SMS_SEND_REQUEST; 
        case CMBS_DSMSL_FLD_NETWORK_ENC:  		return FIELD_ID_SMS_NETWORK_ENCODING;
        case CMBS_DSMSL_FLD_SMS_SIZE:       	return FIELD_ID_SMS_SIZE;
        case CMBS_DSMSL_FLD_SMS_CONTENT:  		return FIELD_ID_SMS_CONTENT;        
        default:                             	return FIELD_ID_INVALID;
        }
        break;
        
    default:
        return FIELD_ID_INVALID;
    }
}

static s32 AppFieldId_2_CATiqFieldId(IN u32 u32_ListId, IN u32 u32_FieldId)
{
    u32_ListId = AppListId_2_CATiqListId(u32_ListId);

    switch ( u32_ListId )
    {
    case  CMBS_LA_LIST_MISSED_CALLS:  
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:           return  CMBS_MCL_FLD_NUMBER;
        case FIELD_ID_LAST_NAME:        return  CMBS_MCL_FLD_NAME;     
        case FIELD_ID_DATE_AND_TIME:    return  CMBS_MCL_FLD_DATETIME;
        case FIELD_ID_READ_STATUS:      return  CMBS_MCL_FLD_NEW;   
        case FIELD_ID_LINE_NAME:        return  CMBS_MCL_FLD_LINE_NAME;
        case FIELD_ID_LINE_ID:          return  CMBS_MCL_FLD_LINE_ID;
        case FIELD_ID_NUM_OF_CALLS:     return  CMBS_MCL_FLD_NR_OF_CALLS;
        default:                        return  CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_OUTGOING_CALLS:     
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:               return CMBS_OCL_FLD_NUMBER;   
        case FIELD_ID_LAST_NAME:            return CMBS_OCL_FLD_NAME;     
        case FIELD_ID_DATE_AND_TIME:        return CMBS_OCL_FLD_DATETIME; 
        case FIELD_ID_LINE_NAME:            return CMBS_OCL_FLD_LINE_NAME;
        case FIELD_ID_LINE_ID:              return CMBS_OCL_FLD_LINE_ID;  
        default:                            return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_INCOMING_ACC: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:                return CMBS_IACL_FLD_NUMBER;   
        case FIELD_ID_LAST_NAME:             return CMBS_IACL_FLD_NAME;     
        case FIELD_ID_DATE_AND_TIME:         return CMBS_IACL_FLD_DATETIME; 
        case FIELD_ID_LINE_NAME:             return CMBS_IACL_FLD_LINE_NAME;
        case FIELD_ID_LINE_ID:               return CMBS_IACL_FLD_LINE_ID;  
        default:                             return CMBS_FLD_UNDEFINED;
        }      
        break;

    case  CMBS_LA_LIST_ALL_CALLS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_CALL_TYPE:        return CMBS_ACL_FLD_CALL_TYPE;    
        case FIELD_ID_NUMBER:           return CMBS_ACL_FLD_NUMBER;       
        case FIELD_ID_LAST_NAME:        return CMBS_ACL_FLD_NAME;         
        case FIELD_ID_DATE_AND_TIME:    return CMBS_ACL_FLD_DATETIME;     
        case FIELD_ID_LINE_NAME:        return CMBS_ACL_FLD_LINE_NAME;    
        case FIELD_ID_LINE_ID:          return CMBS_ACL_FLD_LINE_ID;      
        default:                        return CMBS_FLD_UNDEFINED;
        }      
        break;

    case  CMBS_LA_LIST_CONTACT: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_LAST_NAME:         return CMBS_CL_FLD_NAME;          
        case FIELD_ID_FIRST_NAME:        return CMBS_CL_FLD_FIRST_NAME;    
        case FIELD_ID_CONTACT_NUM_1:     return CMBS_CL_FLD_CONTACT_NR;    
        case FIELD_ID_CONTACT_NUM_2:     return CMBS_CL_FLD_CONTACT_NR;    
        case FIELD_ID_CONTACT_NUM_3:     return CMBS_CL_FLD_CONTACT_NR;    
        case FIELD_ID_ASSOCIATED_MELODY: return CMBS_CL_FLD_ASSOC_MDY;     
        case FIELD_ID_LINE_ID:           return CMBS_CL_FLD_LINE_ID;       
        default:                         return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_LINE_SETTINGS:
        switch ( u32_FieldId )
        {
        case FIELD_ID_LINE_NAME:                return CMBS_LSL_FLD_LINE_NAME;            
        case FIELD_ID_LINE_ID:                  return CMBS_LSL_FLD_LINE_ID;              
        case FIELD_ID_ATTACHED_HANDSETS:        return CMBS_LSL_FLD_ATTACHED_HANDSETS;    
        case FIELD_ID_DIALING_PREFIX:           return CMBS_LSL_FLD_DIALING_PREFIX;       
        case FIELD_ID_FP_MELODY:                return CMBS_LSL_FLD_FP_MELODY;            
        case FIELD_ID_FP_VOLUME:                return CMBS_LSL_FLD_FP_VOLUME;            
        case FIELD_ID_BLOCKED_NUMBER:           return CMBS_LSL_FLD_BLOCKED_NUMBER;       
        case FIELD_ID_MULTIPLE_CALLS_MODE:      return CMBS_LSL_FLD_MULTI_CALL_MODE;      
        case FIELD_ID_INTRUSION_CALL:           return CMBS_LSL_FLD_INTRUSION_CALL;       
        case FIELD_ID_PERMANENT_CLIR:           return CMBS_LSL_FLD_PERMANENT_CLIR;       
        case FIELD_ID_CALL_FWD_UNCOND:          return CMBS_LSL_FLD_CALL_FORWARDING;      
        case FIELD_ID_CALL_FWD_NO_ANSWER:       return CMBS_LSL_FLD_CFNA;                 
        case FIELD_ID_CALL_FWD_BUSY:            return CMBS_LSL_FLD_CFB;                  
        default:                                return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_LINE_AND_DIAGNOSTIC_STATUS:
        switch ( u32_FieldId )
        {
        case FIELD_ID_LINE_ID:                  return CMBS_LADSL_FLD_LINE_ID;              
        case FIELD_ID_OK_STATUS:        		return CMBS_LADSL_FLD_OK_STATUS;    
        case FIELD_ID_LINE_USE_STATUS:          return CMBS_LADSL_FLD_LINE_USE_STATUS;       
        case FIELD_ID_HS_USE_STATUS:            return CMBS_LADSL_FLD_HS_USE_STATUS;            
        case FIELD_ID_CALL_FWD_CFU_STATUS:      return CMBS_LADSL_FLD_CALL_FORWARDING;            
        case FIELD_ID_DIAGNOSTIC_ERROR_STATUS:  return CMBS_LADSL_FLD_DIAGNOSTIC_ERROR;       
        default:                                return CMBS_FLD_UNDEFINED;
        }
        break;
        
    case  CMBS_LA_LIST_ALL_INCOMING_CALLS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:                return CMBS_AICL_FLD_NUMBER;     
        case FIELD_ID_LAST_NAME:             return CMBS_AICL_FLD_NAME;       
        case FIELD_ID_DATE_AND_TIME:         return CMBS_AICL_FLD_DATETIME;   
        case FIELD_ID_READ_STATUS:           return CMBS_AICL_FLD_NEW;        
        case FIELD_ID_LINE_NAME:             return CMBS_AICL_FLD_LINE_NAME;  
        case FIELD_ID_LINE_ID:               return CMBS_AICL_FLD_LINE_ID;    
        case FIELD_ID_NUM_OF_CALLS:          return CMBS_AICL_FLD_NR_OF_CALLS;
        default:                             return CMBS_FLD_UNDEFINED;
        }
        break;
        
    case  CMBS_LA_LIST_SMS_SETTINGS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_SMS_IDENTIFIER:         	 return CMBS_SSL_FLD_SMS_ID;     
        case FIELD_ID_LINE_ID:                	 return CMBS_SSL_FLD_LINE_ID;       
        case FIELD_ID_SMS_ENABLE:        	  	 return CMBS_SSL_FLD_ENABLE_SMS;   
        case FIELD_ID_SMS_MAX_SIZE:           	 return CMBS_SSL_FLD_MAX_SMS_SIZE;        
        case FIELD_ID_SMSC_SEND_SERVER:       	 return CMBS_SSL_FLD_SMSC_SEND_SERVER;
        case FIELD_ID_SMSC_RECEIVE_SERVER:       return CMBS_SSL_FLD_SMSC_RECEIVE_SERVER;    
        case FIELD_ID_SMS_DELIVERY_REPORT: 	  	 return CMBS_SSL_FLD_SMS_DELIVERY_REPORT;
        case FIELD_ID_SMS_VALIDITY_PERIOD:    	 return CMBS_SSL_FLD_SMS_VALIDITY_PERIOD;
		case FIELD_ID_SMS_CHAR_ALLOWED_ENCODING: return CMBS_SSL_FLD_ALLOWED_ENCODING;
        default:                             	 return CMBS_FLD_UNDEFINED;
        }
        break;
		
    case  CMBS_LA_LIST_DTAM_SETTINGS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_LINE_ID:                		return CMBS_DSL_FLD_LINE_ID;     
        case FIELD_ID_DTAM_IDENTIFIER:        		return CMBS_DSL_FLD_DTAM_ID;       
        case FIELD_ID_DTAM_NUMBER:            		return CMBS_DSL_FLD_DTAM_NUMBER;   
        case FIELD_ID_DTAM_ACTIVATION:        		return CMBS_DSL_FLD_DTAM_ACTIVATION;        
        case FIELD_ID_DTAM_WEB_LINK:          		return CMBS_DSL_FLD_DTAM_WEBLINK;  
        case FIELD_ID_DTAM_WELCOME_MSG_PARAMETERS:  return CMBS_DSL_FLD_DTAM_WELCOME_MSG_PARAMS;    
        default:                             		return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_DTAM_WELCOME_MESSAGE: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_DTAM_IDENTIFIER:                return CMBS_DWML_FLD_DTAM_ID;     
        case FIELD_ID_DTAM_POSITION_INDEX:            return CMBS_DWML_FLD_POSITION_IDX;       
        case FIELD_ID_DTAM_RECORDED_MSG_NAME:         return CMBS_DWML_FLD_MSG_NAME;   
        case FIELD_ID_DTAM_TIME_DURATION_HOURS:       return CMBS_DWML_FLD_TIME_DURATION;        
        default:                             		  return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_INCOMING_SMS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:                          	return CMBS_ISMSL_FLD_NUMBER;       
        case FIELD_ID_NAME:                  			return CMBS_ISMSL_FLD_NAME;                 
        case FIELD_ID_DATE_AND_TIME:          			return CMBS_ISMSL_FLD_DATETIME;     
        case FIELD_ID_READ_STATUS:        				return CMBS_ISMSL_FLD_READ_STATUS;      
      	case FIELD_ID_SMS_IDENTIFIER:                   return CMBS_ISMSL_FLD_SMS_ID;       
      	case FIELD_ID_SMS_SIZE:               			return CMBS_ISMSL_FLD_SMS_SIZE;     
      	case FIELD_ID_SMS_CONTENT:            			return CMBS_ISMSL_FLD_SMS_CONTENT; 
        default:                             		  	return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_SENT_SMS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:                          	return CMBS_SSMSL_FLD_NUMBER;
        case FIELD_ID_NAME:                             return CMBS_SSMSL_FLD_NAME; 
        case FIELD_ID_DATE_AND_TIME:              		return CMBS_SSMSL_FLD_DATETIME;
        case FIELD_ID_SMS_IDENTIFIER:                 	return CMBS_SSMSL_FLD_SMS_ID; 
      	case FIELD_ID_SMS_NETWORK_ENCODING:             return CMBS_SSMSL_FLD_NETWORK_ENC;
      	case FIELD_ID_SMS_SIZE:                        	return CMBS_SSMSL_FLD_SMS_SIZE;
      	case FIELD_ID_SMS_CONTENT:                  	return CMBS_SSMSL_FLD_SMS_CONTENT; 
        default:                             		    return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_OUTGOING_SMS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:                          	return CMBS_OSMSL_FLD_NUMBER;
        case FIELD_ID_NAME:                            	return CMBS_OSMSL_FLD_NAME;
        case FIELD_ID_DATE_AND_TIME:              		return CMBS_OSMSL_FLD_DATETIME;
        case FIELD_ID_SMS_IDENTIFIER:              		return CMBS_OSMSL_FLD_SMS_ID;
      	case FIELD_ID_SMS_NETWORK_ENCODING:  			return CMBS_OSMSL_FLD_NETWORK_ENC; 
      	case FIELD_ID_SMS_SIZE:                        	return CMBS_OSMSL_FLD_SMS_SIZE;
      	case FIELD_ID_SMS_CONTENT:                  	return CMBS_OSMSL_FLD_SMS_CONTENT; 
        default:                             		    return CMBS_FLD_UNDEFINED;
        }
        break;

    case  CMBS_LA_LIST_DRAFT_SMS: 
        switch ( u32_FieldId )
        {
        case FIELD_ID_NUMBER:                          	return CMBS_DSMSL_FLD_NUMBER;
        case FIELD_ID_NAME:                            	return CMBS_DSMSL_FLD_NAME;
        case FIELD_ID_DATE_AND_TIME:             		return CMBS_DSMSL_FLD_DATETIME;
        case FIELD_ID_SMS_IDENTIFIER:              		return CMBS_DSMSL_FLD_SMS_ID;
      	case FIELD_ID_SMS_SEND_REQUEST:  				return CMBS_DSMSL_FLD_SEND_REQUEST;
      	case FIELD_ID_SMS_NETWORK_ENCODING:             return CMBS_DSMSL_FLD_NETWORK_ENC;
      	case FIELD_ID_SMS_SIZE:                  		return CMBS_DSMSL_FLD_SMS_SIZE;
        case FIELD_ID_SMS_CONTENT:                      return CMBS_DSMSL_FLD_SMS_CONTENT;
        default:                             		    return CMBS_FLD_UNDEFINED;            
        }
        break;

    default:
        return CMBS_FLD_UNDEFINED;
    }
}

static void ConvertTimeToCatiqTime(INOUT ST_UTIL_LA_FLD_DATETIME* pst_DateTime)
{
    ++pst_DateTime->u8_Month;       /* Convert from [0..11] to [1..12] */
    pst_DateTime->u8_Year -= 100;   /* 2-digit in the 21st century */
}

static void PrepareFieldsForRead(IN u32 u32_ListId, IN PST_IE_LA_FIELDS pst_CatiqFields, OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    u8 u8_ContactListNumber = 1;
    u32 u32_i;

    /* Always read Entry ID */
    *pu32_FieldsNum = 0;
    pu32_Fields[(*pu32_FieldsNum)++] = FIELD_ID_ENTRY_ID;

    for ( u32_i = 0; u32_i < pst_CatiqFields->u16_Length; ++u32_i )
    {
        pu32_Fields[(*pu32_FieldsNum)++] = CATiqFieldId_2_AppFieldId(u32_ListId, pst_CatiqFields->pu16_FieldId[u32_i]);

        if ( (s32)pu32_Fields[(*pu32_FieldsNum) - 1] == FIELD_ID_INVALID )
        {
            /* skip unknown fields */
            --(*pu32_FieldsNum);
            continue;
        }

        /* in case of contact list, if field number is requested twice/thrice, we should add num2/num3 field */
        if ( (u32_ListId == LIST_TYPE_CONTACT_LIST) && (pst_CatiqFields->pu16_FieldId[u32_i] == CMBS_CL_FLD_CONTACT_NR) )
        {
            if ( u8_ContactListNumber == 2 )
            {
                /* this is second field */
                pu32_Fields[*pu32_FieldsNum - 1] = FIELD_ID_CONTACT_NUM_2;
            }
            else if ( u8_ContactListNumber == 3 )
            {
                /* this is second field */
                pu32_Fields[*pu32_FieldsNum - 1] = FIELD_ID_CONTACT_NUM_3;
            }

            u8_ContactListNumber++;
        }
    }
}

static void PrepareMissedCallEntry(IN stCallsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                   IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_MCL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = 0;
                st_LA_Fld_Number.u8_FieldId     = CMBS_MCL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);
            }
            break;

        case CMBS_MCL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = 0;
                st_LA_Fld_Name.u8_FieldId   = CMBS_MCL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLastName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sLastName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;

        case CMBS_MCL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_MCL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_MCL_FLD_NEW:
            {
                ST_UTIL_LA_FLD_NEW st_LA_Fld_New;
                st_LA_Fld_New.u8_Attribute  = pst_AppEntry->bRead ? 0x40 : 0x60;
                st_LA_Fld_New.u8_FieldId    = CMBS_MCL_FLD_NEW;
                cmbs_util_LA_Fld_NewAdd(pu8_Buffer, &st_LA_Fld_New, &u16_index);
            }
            break;

        case CMBS_MCL_FLD_LINE_NAME:
            {
                ST_UTIL_LA_FLD_LINE_NAME st_LA_Fld_LineName;
                st_LA_Fld_LineName.u8_Attribute = 0;
                st_LA_Fld_LineName.u8_FieldId   = CMBS_MCL_FLD_LINE_NAME;
                st_LA_Fld_LineName.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLineName);
                st_LA_Fld_LineName.pu8_Data     = (void*)pst_AppEntry->sLineName;
                cmbs_util_LA_Fld_LineNameAdd(pu8_Buffer, &st_LA_Fld_LineName, &u16_index);
            }
            break;

        case CMBS_MCL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = 0;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_MCL_FLD_LINE_ID;
                st_LA_Fld_LineId.Subtype        = 0;
                st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;

        case CMBS_MCL_FLD_NR_OF_CALLS:
            {
                ST_UTIL_LA_FLD_NR_OF_CALLS st_LA_Fld_NrOfCalls;
                st_LA_Fld_NrOfCalls.u8_Attribute = 0;
                st_LA_Fld_NrOfCalls.u8_FieldId   = CMBS_MCL_FLD_NR_OF_CALLS;
                st_LA_Fld_NrOfCalls.u8_Value     = pst_AppEntry->u32_NumOfCalls;
                cmbs_util_LA_Fld_NrOfCallsAdd(pu8_Buffer, &st_LA_Fld_NrOfCalls, &u16_index);
            }
            break;
        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = u16_index - (u16)(pu8_Entry - pu8_Buffer);

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareOutgoingCallEntry(IN stCallsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                     IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_OCL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = 0;
                st_LA_Fld_Number.u8_FieldId     = CMBS_OCL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);
            }
            break;

        case CMBS_OCL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = 0;
                st_LA_Fld_Name.u8_FieldId   = CMBS_OCL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLastName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sLastName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;

        case CMBS_OCL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_OCL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_OCL_FLD_LINE_NAME:
            {
                ST_UTIL_LA_FLD_LINE_NAME st_LA_Fld_LineName;
                st_LA_Fld_LineName.u8_Attribute = 0;
                st_LA_Fld_LineName.u8_FieldId   = CMBS_OCL_FLD_LINE_NAME;
                st_LA_Fld_LineName.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLineName);
                st_LA_Fld_LineName.pu8_Data     = (void*)pst_AppEntry->sLineName;
                cmbs_util_LA_Fld_LineNameAdd(pu8_Buffer, &st_LA_Fld_LineName, &u16_index);
            }
            break;

        case CMBS_OCL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = 0;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_OCL_FLD_LINE_ID;
                st_LA_Fld_LineId.Subtype        = 0;
                st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareIncomingAcceptedCallEntry(IN stCallsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                             IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_IACL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = 0;
                st_LA_Fld_Number.u8_FieldId     = CMBS_IACL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);
            }
            break;

        case CMBS_IACL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = 0;
                st_LA_Fld_Name.u8_FieldId   = CMBS_IACL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLastName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sLastName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;

        case CMBS_IACL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_IACL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_IACL_FLD_LINE_NAME:
            {
                ST_UTIL_LA_FLD_LINE_NAME st_LA_Fld_LineName;
                st_LA_Fld_LineName.u8_Attribute = 0;
                st_LA_Fld_LineName.u8_FieldId   = CMBS_IACL_FLD_LINE_NAME;
                st_LA_Fld_LineName.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLineName);
                st_LA_Fld_LineName.pu8_Data     = (void*)pst_AppEntry->sLineName;
                cmbs_util_LA_Fld_LineNameAdd(pu8_Buffer, &st_LA_Fld_LineName, &u16_index);
            }
            break;

        case CMBS_IACL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = 0;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_IACL_FLD_LINE_ID;
                st_LA_Fld_LineId.Subtype        = 0;
                st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareAllCallEntry(IN stCallsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_ACL_FLD_CALL_TYPE:
            {
                ST_UTIL_LA_FLD_CALL_TYPE st_LA_Fld_CallType;
                switch ( pst_AppEntry->cCallType )
                {
                case CALL_TYPE_MISSED:      st_LA_Fld_CallType.u8_Attribute = 0x20;     break;
                case CALL_TYPE_OUTGOING:    st_LA_Fld_CallType.u8_Attribute = 0x08;     break;
                case CALL_TYPE_INCOMING:    st_LA_Fld_CallType.u8_Attribute = 0x10;     break;
                default:                                                                break;
                }

                st_LA_Fld_CallType.u8_FieldId = CMBS_ACL_FLD_CALL_TYPE;
                cmbs_util_LA_Fld_CallTypeAdd(pu8_Buffer, &st_LA_Fld_CallType, &u16_index);
            }
            break;

        case CMBS_ACL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = 0;
                st_LA_Fld_Number.u8_FieldId     = CMBS_ACL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);
            }
            break;

        case CMBS_ACL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = 0;
                st_LA_Fld_Name.u8_FieldId   = CMBS_ACL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLastName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sLastName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;

        case CMBS_ACL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_ACL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_ACL_FLD_LINE_NAME:
            {
                ST_UTIL_LA_FLD_LINE_NAME st_LA_Fld_LineName;
                st_LA_Fld_LineName.u8_Attribute = 0;
                st_LA_Fld_LineName.u8_FieldId   = CMBS_ACL_FLD_LINE_NAME;
                st_LA_Fld_LineName.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLineName);
                st_LA_Fld_LineName.pu8_Data     = (void*)pst_AppEntry->sLineName;
                cmbs_util_LA_Fld_LineNameAdd(pu8_Buffer, &st_LA_Fld_LineName, &u16_index);
            }
            break;

        case CMBS_ACL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = 0;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_ACL_FLD_LINE_ID;
                st_LA_Fld_LineId.Subtype        = 0;
                st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareIncomingCallEntry(IN stCallsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                     IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_AICL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = 0;
                st_LA_Fld_Number.u8_FieldId     = CMBS_AICL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);
            }
            break;

        case CMBS_AICL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = 0;
                st_LA_Fld_Name.u8_FieldId   = CMBS_AICL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLastName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sLastName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;

        case CMBS_AICL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_AICL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_AICL_FLD_NEW:
            {
                ST_UTIL_LA_FLD_NEW st_LA_Fld_New;
                if ( pst_AppEntry->cCallType ==  CALL_TYPE_INCOMING )
                {
                    st_LA_Fld_New.u8_Attribute = 0x40;
                }
                else
                {
                    st_LA_Fld_New.u8_Attribute  = pst_AppEntry->bRead ? 0x40 : 0x60;
                }
                st_LA_Fld_New.u8_FieldId    = CMBS_AICL_FLD_NEW;
                cmbs_util_LA_Fld_NewAdd(pu8_Buffer, &st_LA_Fld_New, &u16_index);
            }
            break;

        case CMBS_AICL_FLD_LINE_NAME:
            {
                ST_UTIL_LA_FLD_LINE_NAME st_LA_Fld_LineName;
                st_LA_Fld_LineName.u8_Attribute = 0;
                st_LA_Fld_LineName.u8_FieldId   = CMBS_AICL_FLD_LINE_NAME;
                st_LA_Fld_LineName.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sLineName);
                st_LA_Fld_LineName.pu8_Data     = (void*)pst_AppEntry->sLineName;
                cmbs_util_LA_Fld_LineNameAdd(pu8_Buffer, &st_LA_Fld_LineName, &u16_index);
            }
            break;

        case CMBS_AICL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = 0;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_AICL_FLD_LINE_ID;
                st_LA_Fld_LineId.Subtype        = 0;
                st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;

        case CMBS_AICL_FLD_NR_OF_CALLS:
            {
                ST_UTIL_LA_FLD_NR_OF_CALLS st_LA_Fld_NrOfCalls;
                st_LA_Fld_NrOfCalls.u8_Attribute = 0;
                st_LA_Fld_NrOfCalls.u8_FieldId   = CMBS_AICL_FLD_NR_OF_CALLS;
                if ( pst_AppEntry->cCallType ==  CALL_TYPE_INCOMING )
                {
                    st_LA_Fld_NrOfCalls.u8_Value     = 0;
                }
                else
                {
                    st_LA_Fld_NrOfCalls.u8_Value     = pst_AppEntry->u32_NumOfCalls;
                }
                cmbs_util_LA_Fld_NrOfCallsAdd(pu8_Buffer, &st_LA_Fld_NrOfCalls, &u16_index);
            }
            break;
        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareLineSettingsEntry(IN stLineSettingsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                     IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_LSL_FLD_LINE_NAME:
            {
                ST_UTIL_LA_FLD_LINE_NAME st_LA_Fld_LineName;
                st_LA_Fld_LineName.u8_Attribute     = 0x40;
                st_LA_Fld_LineName.u8_FieldId       = CMBS_LSL_FLD_LINE_NAME;
                st_LA_Fld_LineName.u16_DataLen      = (u16) strlen((char*)pst_AppEntry->sLineName);
                st_LA_Fld_LineName.pu8_Data         = (void*)pst_AppEntry->sLineName;
                cmbs_util_LA_Fld_LineNameAdd(pu8_Buffer, &st_LA_Fld_LineName, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = 0;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_LSL_FLD_LINE_ID;
                st_LA_Fld_LineId.Subtype        = 3;
                st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_ATTACHED_HANDSETS:
            {
                ST_UTIL_LA_FLD_ATTACHED_HANDSETS st_LA_Fld_AttachedHandsets;
                st_LA_Fld_AttachedHandsets.u8_Attribute = 0x40;
                st_LA_Fld_AttachedHandsets.u8_FieldId   = CMBS_LSL_FLD_ATTACHED_HANDSETS;
                st_LA_Fld_AttachedHandsets.pu8_Data     = (void*)&pst_AppEntry->u32_AttachedHsMask; 
                cmbs_util_LA_Fld_AttachedHandsetsAdd(pu8_Buffer, &st_LA_Fld_AttachedHandsets, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_DIALING_PREFIX:
            {
                ST_UTIL_LA_FLD_DIALING_PREFIX st_LA_Fld_DialingPrefixe;
                st_LA_Fld_DialingPrefixe.u8_Attribute   = 0x40;
                st_LA_Fld_DialingPrefixe.u8_FieldId     = CMBS_LSL_FLD_DIALING_PREFIX;
                st_LA_Fld_DialingPrefixe.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sDialPrefix);
                st_LA_Fld_DialingPrefixe.pu8_Data       = (void*)pst_AppEntry->sDialPrefix;
                cmbs_util_LA_Fld_DialPrefixeAdd(pu8_Buffer, &st_LA_Fld_DialingPrefixe,  &u16_index);
            }
            break;

        case CMBS_LSL_FLD_FP_MELODY:
            {
                ST_UTIL_LA_FLD_FP_MELODY st_LA_Fld_FPmelody;
                st_LA_Fld_FPmelody.u8_Attribute = 0x40;
                st_LA_Fld_FPmelody.u8_FieldId   = CMBS_LSL_FLD_FP_MELODY;
                st_LA_Fld_FPmelody.u8_Value     = pst_AppEntry->u32_FPMelody;
                cmbs_util_LA_Fld_AssocMdyAdd(pu8_Buffer, &st_LA_Fld_FPmelody, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_FP_VOLUME:
            {
                ST_UTIL_LA_FLD_FP_VOLUME st_LA_Fld_FPvolume;
                st_LA_Fld_FPvolume.u8_Attribute = 0x40;
                st_LA_Fld_FPvolume.u8_FieldId   = CMBS_LSL_FLD_FP_VOLUME;
                st_LA_Fld_FPvolume.u8_Value     = pst_AppEntry->u32_FPVolume;
                cmbs_util_LA_Fld_FPvolumeAdd(pu8_Buffer, &st_LA_Fld_FPvolume, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_BLOCKED_NUMBER:
            {
                ST_UTIL_LA_FLD_BLOCKED_NUMBER st_LA_Fld_BlockedNumber;
                st_LA_Fld_BlockedNumber.u8_Attribute    = 0x40;
                st_LA_Fld_BlockedNumber.u8_FieldId      = CMBS_LSL_FLD_BLOCKED_NUMBER;
                st_LA_Fld_BlockedNumber.u16_DataLen     = (u16) strlen((char*)pst_AppEntry->sBlockedNumber);
                st_LA_Fld_BlockedNumber.pu8_Data        = (void*)pst_AppEntry->sBlockedNumber;
                cmbs_util_LA_Fld_BlockedNumberAdd(pu8_Buffer, &st_LA_Fld_BlockedNumber, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_MULTI_CALL_MODE:
            {
                ST_UTIL_LA_FLD_MULTI_CALL_MODE st_LA_Fld_MultiCallMode;
                st_LA_Fld_MultiCallMode.u8_Attribute    = 0x40;
                st_LA_Fld_MultiCallMode.u8_FieldId      = CMBS_LSL_FLD_MULTI_CALL_MODE;
                st_LA_Fld_MultiCallMode.u8_Value        = pst_AppEntry->bMultiCalls ? 0x31 : 0x30;
                cmbs_util_LA_Fld_MultiCallModeAdd(pu8_Buffer, &st_LA_Fld_MultiCallMode, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_INTRUSION_CALL:
            {
                ST_UTIL_LA_FLD_INTRUSION_CALL st_LA_Fld_IntrusionCall;
                st_LA_Fld_IntrusionCall.u8_Attribute    = 0x40;
                st_LA_Fld_IntrusionCall.u8_FieldId      = CMBS_LSL_FLD_INTRUSION_CALL;
                st_LA_Fld_IntrusionCall.u8_Value        = pst_AppEntry->bIntrusionCall ? 0x31 : 0x30;
                cmbs_util_LA_Fld_IntrusionCallAdd(pu8_Buffer, &st_LA_Fld_IntrusionCall, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_PERMANENT_CLIR:
            {
                ST_UTIL_LA_FLD_PERMANENT_CLIR st_LA_Fld_PermanentCLIR;
                st_LA_Fld_PermanentCLIR.u8_Attribute        = 0x40;
                st_LA_Fld_PermanentCLIR.u8_FieldId          = CMBS_LSL_FLD_PERMANENT_CLIR;
                st_LA_Fld_PermanentCLIR.u8_Value            = pst_AppEntry->bPermanentCLIR ? 0x31 : 0x30;
                st_LA_Fld_PermanentCLIR.u8_ActDataLen       = (u8) strlen((char*)pst_AppEntry->sPermanentCLIRActCode);
                st_LA_Fld_PermanentCLIR.pu8_ActData         = (void*)pst_AppEntry->sPermanentCLIRActCode;
                st_LA_Fld_PermanentCLIR.u8_DesactDataLen    = (u8) strlen((char*)pst_AppEntry->sPermanentCLIRDeactCode);  
                st_LA_Fld_PermanentCLIR.pu8_DesactData      = (void*)pst_AppEntry->sPermanentCLIRDeactCode;
                cmbs_util_LA_Fld_PermanentCLIRAdd(pu8_Buffer, &st_LA_Fld_PermanentCLIR, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_CALL_FORWARDING:
            {
                ST_UTIL_LA_FLD_CALL_FORWARDING st_LA_Fld_CallForwarding;
                st_LA_Fld_CallForwarding.u8_Attribute       = 0x40;
                st_LA_Fld_CallForwarding.u8_FieldId         = CMBS_LSL_FLD_CALL_FORWARDING;
                st_LA_Fld_CallForwarding.u8_NotSetByUSer    = 0;
                st_LA_Fld_CallForwarding.u8_Value           = pst_AppEntry->bCallFwdUncond ? 0x31 : 0x30;
                st_LA_Fld_CallForwarding.u8_ActDataLen      = (u8) strlen((char*)pst_AppEntry->sCallFwdUncondActCode); 
                st_LA_Fld_CallForwarding.pu8_ActData        = (void*)pst_AppEntry->sCallFwdUncondActCode;
                st_LA_Fld_CallForwarding.u8_DesactDataLen   = (u8) strlen((char*)pst_AppEntry->sCallFwdUncondDeactCode); 
                st_LA_Fld_CallForwarding.pu8_DesactData     = (void*)pst_AppEntry->sCallFwdUncondDeactCode;
                st_LA_Fld_CallForwarding.u8_NumberLen       = (u8) strlen((char*)pst_AppEntry->sCallFwdUncondNum);
                st_LA_Fld_CallForwarding.pu8_Number         = (void*)pst_AppEntry->sCallFwdUncondNum;
                cmbs_util_LA_Fld_CallForwardingAdd(pu8_Buffer, &st_LA_Fld_CallForwarding, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_CFNA:
            {
                ST_UTIL_LA_FLD_CFNA st_LA_Fld_CFNA;
                st_LA_Fld_CFNA.u8_Attribute         = 0x40;
                st_LA_Fld_CFNA.u8_FieldId           = CMBS_LSL_FLD_CFNA;
                st_LA_Fld_CFNA.u8_NotSetByUSer      = 0;
                st_LA_Fld_CFNA.u8_Value             = pst_AppEntry->bCallFwdNoAns ? 0x31 : 0x30;
                st_LA_Fld_CFNA.u8_Timeout           = (u8) pst_AppEntry->u32_CallFwdNoAnsTimeout;
                st_LA_Fld_CFNA.u8_ActDataLen        = (u8) strlen((char*)pst_AppEntry->sCallFwdNoAnsActCode);   
                st_LA_Fld_CFNA.pu8_ActData          = (void*)pst_AppEntry->sCallFwdNoAnsActCode;                 
                st_LA_Fld_CFNA.u8_DesactDataLen     = (u8) strlen((char*)pst_AppEntry->sCallFwdNoAnsDeactCode); 
                st_LA_Fld_CFNA.pu8_DesactData       = (void*)pst_AppEntry->sCallFwdNoAnsDeactCode;               
                st_LA_Fld_CFNA.u8_NumberLen         = (u8) strlen((char*)pst_AppEntry->sCallFwdNoAnsNum);       
                st_LA_Fld_CFNA.pu8_Number           = (void*)pst_AppEntry->sCallFwdNoAnsNum;                     
                cmbs_util_LA_Fld_CFNAAdd(pu8_Buffer, &st_LA_Fld_CFNA, &u16_index);
            }
            break;

        case CMBS_LSL_FLD_CFB:
            {
                ST_UTIL_LA_FLD_CFB st_LA_Fld_CFB;
                st_LA_Fld_CFB.u8_Attribute      = 0x40;
                st_LA_Fld_CFB.u8_FieldId        = CMBS_LSL_FLD_CFB;
                st_LA_Fld_CFB.u8_NotSetByUSer   = 0;
                st_LA_Fld_CFB.u8_Value          = pst_AppEntry->bCallFwdBusy ? 0x31 : 0x30;                
                st_LA_Fld_CFB.u8_ActDataLen     = (u8) strlen((char*)pst_AppEntry->sCallFwdBusyActCode);  
                st_LA_Fld_CFB.pu8_ActData       = (void*)pst_AppEntry->sCallFwdBusyActCode;                
                st_LA_Fld_CFB.u8_DesactDataLen  = (u8) strlen((char*)pst_AppEntry->sCallFwdBusyDeactCode);
                st_LA_Fld_CFB.pu8_DesactData    = (void*)pst_AppEntry->sCallFwdBusyDeactCode;              
                st_LA_Fld_CFB.u8_NumberLen      = (u8) strlen((char*)pst_AppEntry->sCallFwdBusyNum);      
                st_LA_Fld_CFB.pu8_Number        = (void*)pst_AppEntry->sCallFwdBusyNum;                    
                cmbs_util_LA_Fld_CFBAdd(pu8_Buffer, &st_LA_Fld_CFB, &u16_index);
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareContactListEntry(IN stContactListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;
    u8 u8_ContactNumber = 1;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_CL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute     = 0x40;
                st_LA_Fld_Name.u8_FieldId       = CMBS_CL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen      = (u16) strlen((char*)pst_AppEntry->sLastName);
                st_LA_Fld_Name.pu8_Data         = (void*)pst_AppEntry->sLastName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;

        case CMBS_CL_FLD_FIRST_NAME:
            {
                ST_UTIL_LA_FLD_FIRST_NAME st_LA_Fld_FirstName;
                st_LA_Fld_FirstName.u8_Attribute    = 0x40;
                st_LA_Fld_FirstName.u8_FieldId      = CMBS_CL_FLD_FIRST_NAME;
                st_LA_Fld_FirstName.u16_DataLen     = (u16) strlen((char*)pst_AppEntry->sFirstName);
                st_LA_Fld_FirstName.pu8_Data        = (void*)pst_AppEntry->sFirstName;
                cmbs_util_LA_Fld_FirstNameAdd(pu8_Buffer, &st_LA_Fld_FirstName, &u16_index);
            }
            break;

        case CMBS_CL_FLD_CONTACT_NR:
            {
                ST_UTIL_LA_FLD_CONTACT_NR st_LA_Fld_ContactNumber;
                st_LA_Fld_ContactNumber.u8_FieldId = CMBS_CL_FLD_CONTACT_NR;


                if ( u8_ContactNumber == 1 )
                {
                    st_LA_Fld_ContactNumber.u8_Attribute = 0x40;
                    if ( pst_AppEntry->bNumber1Default )
                    {
                        st_LA_Fld_ContactNumber.u8_Attribute |= 0x20;
                    }
                    if ( pst_AppEntry->bNumber1Own )
                    {
                        st_LA_Fld_ContactNumber.u8_Attribute |= 0x10;
                    }
                    switch ( pst_AppEntry->cNumber1Type )
                    {
                    case NUM_TYPE_FIXED:    st_LA_Fld_ContactNumber.u8_Attribute |= 0x08;   break;
                    case NUM_TYPE_MOBILE:   st_LA_Fld_ContactNumber.u8_Attribute |= 0x04;   break;
                    case NUM_TYPE_WORK:     st_LA_Fld_ContactNumber.u8_Attribute |= 0x02;   break;
                    default:                                                                break;
                    }
                    st_LA_Fld_ContactNumber.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sNumber1);
                    st_LA_Fld_ContactNumber.pu8_Data     = (void*)pst_AppEntry->sNumber1;
                }
                else if ( u8_ContactNumber == 2 )
                {
                    st_LA_Fld_ContactNumber.u8_Attribute = 0x40;
                    if ( pst_AppEntry->bNumber2Default )
                    {
                        st_LA_Fld_ContactNumber.u8_Attribute |= 0x20;
                    }
                    if ( pst_AppEntry->bNumber2Own )
                    {
                        st_LA_Fld_ContactNumber.u8_Attribute |= 0x10;
                    }
                    switch ( pst_AppEntry->cNumber2Type )
                    {
                    case NUM_TYPE_FIXED:    st_LA_Fld_ContactNumber.u8_Attribute |= 0x08;   break;
                    case NUM_TYPE_MOBILE:   st_LA_Fld_ContactNumber.u8_Attribute |= 0x04;   break;
                    case NUM_TYPE_WORK:     st_LA_Fld_ContactNumber.u8_Attribute |= 0x02;   break;
                    default:                                                                break;
                    }
                    st_LA_Fld_ContactNumber.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sNumber2);
                    st_LA_Fld_ContactNumber.pu8_Data     = (void*)pst_AppEntry->sNumber2;
                }
                else
                {
                    st_LA_Fld_ContactNumber.u8_Attribute = 0x40;
                    if ( pst_AppEntry->bNumber3Default )
                    {
                        st_LA_Fld_ContactNumber.u8_Attribute |= 0x20;
                    }
                    if ( pst_AppEntry->bNumber3Own )
                    {
                        st_LA_Fld_ContactNumber.u8_Attribute |= 0x10;
                    }
                    switch ( pst_AppEntry->cNumber3Type )
                    {
                    case NUM_TYPE_FIXED:    st_LA_Fld_ContactNumber.u8_Attribute |= 0x08;   break;
                    case NUM_TYPE_MOBILE:   st_LA_Fld_ContactNumber.u8_Attribute |= 0x04;   break;
                    case NUM_TYPE_WORK:     st_LA_Fld_ContactNumber.u8_Attribute |= 0x02;   break;
                    default:                                                                break;
                    }
                    st_LA_Fld_ContactNumber.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sNumber3);
                    st_LA_Fld_ContactNumber.pu8_Data     = (void*)pst_AppEntry->sNumber3;
                }

                cmbs_util_LA_Fld_ContactNrAdd(pu8_Buffer, &st_LA_Fld_ContactNumber, &u16_index);

                u8_ContactNumber++;
            }
            break;

        case CMBS_CL_FLD_ASSOC_MDY:
            {
                ST_UTIL_LA_FLD_ASSOC_MDY st_LA_Fld_Associated_melody;
                st_LA_Fld_Associated_melody.u8_Attribute    = 0x40;
                st_LA_Fld_Associated_melody.u8_FieldId      = CMBS_CL_FLD_ASSOC_MDY;
                st_LA_Fld_Associated_melody.u8_Value        = pst_AppEntry->u32_AssociatedMelody;
                cmbs_util_LA_Fld_AssocMdyAdd(pu8_Buffer, &st_LA_Fld_Associated_melody, &u16_index);
            }
            break;

        case CMBS_CL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = 0x40;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_CL_FLD_LINE_ID;
                if ( pst_AppEntry->u32_LineId == LIST_LINE_ID_NONE )
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_ALL_LINES_RELATED_TO;
                }
                else
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_RELATED_TO;
                    st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                }
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareLineDiagnosticStatusEntry(IN stLineAndDiagnosticListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_LADSL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_LADSL_FLD_LINE_ID;
                if ( pst_AppEntry->u32_LineId == LIST_LINE_ID_NONE )
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_ALL_LINES_RELATED_TO;
                }
                else
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_RELATED_TO;
                    st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                }
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;
            
        case CMBS_LADSL_FLD_OK_STATUS:
            {
                ST_UTIL_LA_FLD_OK_STATUS st_LA_fld_OKStatus;
                st_LA_fld_OKStatus.u8_Attribute   = 0;
                st_LA_fld_OKStatus.u8_FieldId     = CMBS_LADSL_FLD_OK_STATUS;
                st_LA_fld_OKStatus.u8_Value        = pst_AppEntry->u32_OKStatus;
                cmbs_util_LA_Fld_OKStatusAdd(pu8_Buffer, &st_LA_fld_OKStatus, &u16_index);
            }
			break;
        case CMBS_LADSL_FLD_LINE_USE_STATUS:
            {
                ST_UTIL_LA_FLD_LINE_USE_STATUS st_LA_fld_LineUseStatus;
                st_LA_fld_LineUseStatus.u8_Attribute   = 0;
                st_LA_fld_LineUseStatus.u8_FieldId     = CMBS_LADSL_FLD_LINE_USE_STATUS;
                st_LA_fld_LineUseStatus.u8_Value        = pst_AppEntry->u32_LineUseStatus;
                cmbs_util_LA_Fld_LineUseStatusAdd(pu8_Buffer, &st_LA_fld_LineUseStatus, &u16_index);
            }
            break;

        case CMBS_LADSL_FLD_HS_USE_STATUS:
            {
                ST_UTIL_LA_FLD_HS_USE_STATUS st_LA_fld_HSUseStatus;
                st_LA_fld_HSUseStatus.u8_Attribute = 0;
                st_LA_fld_HSUseStatus.u8_FieldId   = CMBS_LADSL_FLD_HS_USE_STATUS;
                st_LA_fld_HSUseStatus.pu8_Data     = (void*)&pst_AppEntry->u32_HSUseStatus;
                cmbs_util_LA_Fld_AttachedHandsetsAdd(pu8_Buffer, &st_LA_fld_HSUseStatus, &u16_index);               
            }
            break;

        case CMBS_LADSL_FLD_CALL_FORWARDING:
            {
                ST_UTIL_LA_FLD_CALL_FWRD_STATUS st_LA_fld_CallFRWDStatus;
                st_LA_fld_CallFRWDStatus.u8_Attribute = 0;
                st_LA_fld_CallFRWDStatus.u8_FieldId   = CMBS_LADSL_FLD_CALL_FORWARDING;
                st_LA_fld_CallFRWDStatus.u8_CFUStatus = pst_AppEntry->b_CallFwdUncondStatus;
                st_LA_fld_CallFRWDStatus.u8_CFNAStatus = pst_AppEntry->b_CallFwdNoAnswerStatus;
                st_LA_fld_CallFRWDStatus.u8_CFBStatus = pst_AppEntry->b_CallFwdBusyStatus;
                cmbs_util_LA_Fld_CallForwardingStatusAdd(pu8_Buffer, &st_LA_fld_CallFRWDStatus, &u16_index);
            }
            break;
            
        case CMBS_LADSL_FLD_DIAGNOSTIC_ERROR:     
            {
                ST_UTIL_LA_DIAG_ERROR_STATUS st_LA_fld_DiagErrorStatus;
                st_LA_fld_DiagErrorStatus.u8_Attribute = (pst_AppEntry->b_DiagnosticErrorStatus) << 5;
                st_LA_fld_DiagErrorStatus.u8_FieldId   = CMBS_LADSL_FLD_DIAGNOSTIC_ERROR;
                st_LA_fld_DiagErrorStatus.u8_ErrorType = pst_AppEntry->u32_DiagnosticErrorType;
                st_LA_fld_DiagErrorStatus.u8_ErrorNumber = pst_AppEntry->u32_DiagnosticErrorNumber;
                cmbs_util_LA_Fld_DiagnosticErrorAdd(pu8_Buffer, &st_LA_fld_DiagErrorStatus, &u16_index);
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);

}

static void PrepareSMSSettingsEntry(IN stSMSSettingsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    OUT u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
            
        case CMBS_SSL_FLD_SMS_ID:
            {
                ST_UTIL_LA_FLD_SMS_ID st_LA_fld_SMSId;
                st_LA_fld_SMSId.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_SMSId.u8_FieldId   = CMBS_SSL_FLD_SMS_ID;
                st_LA_fld_SMSId.u8_Value     = pst_AppEntry->u32_SMSServiseId;
                cmbs_util_LA_Fld_SMSIdAdd(pu8_Buffer, &st_LA_fld_SMSId, &u16_index);
            }
		break;
        
        case CMBS_SSL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_SSL_FLD_LINE_ID;
                if ( pst_AppEntry->u32_LineId == LIST_LINE_ID_NONE )
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_ALL_LINES_RELATED_TO;
                }
                else
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_RELATED_TO;
                    st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                }
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;
        
        case CMBS_SSL_FLD_ENABLE_SMS:
            {
                ST_UTIL_LA_SMS_ENABLE st_LA_Fld_SMS_Enable;
                st_LA_Fld_SMS_Enable.u8_Attribute    = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMS_Enable.u8_FieldId      = CMBS_SSL_FLD_ENABLE_SMS;
                st_LA_Fld_SMS_Enable.u8_Value        = (u8) pst_AppEntry->b_EnableSMS? 0x31 : 0x30;
                cmbs_util_LA_Fld_SMSEnableAdd(pu8_Buffer, &st_LA_Fld_SMS_Enable, &u16_index);
            }
            break;

        case CMBS_SSL_FLD_MAX_SMS_SIZE:
            {
                ST_UTIL_LA_FLD_MAX_SMS_SIZE st_LA_fld_SMSMaxSize;
                st_LA_fld_SMSMaxSize.u8_Attribute     = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_SMSMaxSize.u8_FieldId       = CMBS_SSL_FLD_MAX_SMS_SIZE;
                st_LA_fld_SMSMaxSize.u8_Value = pst_AppEntry->u32_SMSMaxSize;
                cmbs_util_LA_Fld_SMSMaxSizeAdd(pu8_Buffer, &st_LA_fld_SMSMaxSize, &u16_index);
            }
            break;
            
        case CMBS_SSL_FLD_SMSC_SEND_SERVER:     
            {
                ST_UTIL_LA_FLD_SMSC_SERVER st_LA_Fld_SMSCSendServer;
                st_LA_Fld_SMSCSendServer.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSCSendServer.u8_FieldId     = CMBS_SSL_FLD_SMSC_SEND_SERVER;
                st_LA_Fld_SMSCSendServer.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sSMSCSendServer);
                st_LA_Fld_SMSCSendServer.pu8_Data       = (void*)pst_AppEntry->sSMSCSendServer;
                cmbs_util_LA_Fld_SMSCServerAdd(pu8_Buffer, &st_LA_Fld_SMSCSendServer, &u16_index);
            }
            break;
            
        case CMBS_SSL_FLD_SMSC_RECEIVE_SERVER:     
            {
                ST_UTIL_LA_FLD_SMSC_SERVER st_LA_Fld_SMSReceiveServer;
                st_LA_Fld_SMSReceiveServer.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSReceiveServer.u8_FieldId     = CMBS_SSL_FLD_SMSC_RECEIVE_SERVER;
                st_LA_Fld_SMSReceiveServer.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sSMSCReceiveServer);
                st_LA_Fld_SMSReceiveServer.pu8_Data       = (void*)pst_AppEntry->sSMSCReceiveServer;
                cmbs_util_LA_Fld_SMSCServerAdd(pu8_Buffer, &st_LA_Fld_SMSReceiveServer, &u16_index);
            }
            break;
        case CMBS_SSL_FLD_SMS_DELIVERY_REPORT:     
            {
                ST_UTIL_LA_FLD_SMS_DELIVERY_REPORT st_LA_fld_SMSDeliveryReport;
                st_LA_fld_SMSDeliveryReport.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_SMSDeliveryReport.u8_FieldId   = CMBS_SSL_FLD_SMS_DELIVERY_REPORT;
                st_LA_fld_SMSDeliveryReport.u8_Value     = pst_AppEntry->b_SMSDeliveryReport ? 0x31 : 0x30;
                cmbs_util_LA_Fld_SMSDeliveryReportAdd(pu8_Buffer, &st_LA_fld_SMSDeliveryReport, &u16_index);
            }
            break;
        case CMBS_SSL_FLD_SMS_VALIDITY_PERIOD:     
            {
                ST_UTIL_LA_FLD_SMS_VALIDITY_PERIOD st_LA_fld_SMSValidityPeriod;
                st_LA_fld_SMSValidityPeriod.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_SMSValidityPeriod.u8_FieldId   = CMBS_SSL_FLD_SMS_VALIDITY_PERIOD;
                st_LA_fld_SMSValidityPeriod.u8_Value     = pst_AppEntry->u32_SMSValidityPeriod;
                cmbs_util_LA_Fld_SMSValidityPeriodAdd(pu8_Buffer, &st_LA_fld_SMSValidityPeriod, &u16_index);
            }
            break;
        case CMBS_SSL_FLD_ALLOWED_ENCODING:     
            {
                #define CMBS_MAX_ALLOWED_ENCODING_LEN 23
                ST_UTIL_LA_FLD_ALLOWED_ENCODING st_LA_fld_AllowedEncoding;
                u8 u8_index = 0;
                u8 u8_buffer[CMBS_MAX_ALLOWED_ENCODING_LEN] = {0};
                memset(&st_LA_fld_AllowedEncoding, 0, sizeof(ST_UTIL_LA_FLD_ALLOWED_ENCODING));
                st_LA_fld_AllowedEncoding.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_AllowedEncoding.u8_FieldId   = CMBS_SSL_FLD_ALLOWED_ENCODING;
                if(pst_AppEntry->u32_SMSCharacterEncodingValue_1 != 0)
                {   
                    u8_index = 1; // Index '0' is reserved for length
                    u8_buffer[u8_index] = pst_AppEntry->u32_SMSCharacterEncodingValue_1; // 1 = GSM 7bit
					u8_index++;

                    if(pst_AppEntry->u32_AllowedEncoding_0 != 0)
                    {
                        u8_buffer[u8_index] = 0x0; //Default
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_1 != 0)
                    {
                        u8_buffer[u8_index] = 0x1; //Turkish
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_2 != 0)
                    {
                        u8_buffer[u8_index] = 0x2; //Spanish
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_3 != 0)
                    {
                        u8_buffer[u8_index] = 0x3; //Portuguese
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_4 != 0)
                    {
                        u8_buffer[u8_index] = 0x4; //Bengali
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_5 != 0)
                    {
                        u8_buffer[u8_index] = 0x5; //Gujarati
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_6 != 0)
                    {
                        u8_buffer[u8_index] = 0x6; //Hindi
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_7 != 0)
                    {
                        u8_buffer[u8_index] = 0x7; //Kannada
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_8 != 0)
                    {
                        u8_buffer[u8_index] = 0x8; //Malayalam
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_9 != 0)
                    {
                        u8_buffer[u8_index] = 0x9; //Oriya
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_10 != 0)
                    {
                        u8_buffer[u8_index] = 0x10; //Punjabi
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_11 != 0)
                    {
                        u8_buffer[u8_index] = 0x11; //Tamil
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_12 != 0)
                    {
                        u8_buffer[u8_index] = 0x12; //Telugu
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_13 != 0)
                    {
                        u8_buffer[u8_index] = 0x13; //Urdu
                        u8_index++;
                    }
                    if(pst_AppEntry->u32_AllowedEncoding_14 != 0)
                    {
                        u8_buffer[u8_index] = 0x14; //Reserved
                        u8_index++;
                    }
                    
                    u8_buffer[0] = u8_index -2; //Length
                }
                if(pst_AppEntry->u32_SMSCharacterEncodingValue_2 != 0)
                 {
                    u8_buffer[u8_index] = 1; // Length
                    u8_buffer[++u8_index] = pst_AppEntry->u32_SMSCharacterEncodingValue_2; // GSM 8 bit
                    u8_index++;
                  }
                if(pst_AppEntry->u32_SMSCharacterEncodingValue_3 != 0)
                 {
                    u8_buffer[u8_index] = 1; // Length
                    u8_buffer[++u8_index] = pst_AppEntry->u32_SMSCharacterEncodingValue_3; // UCS-2
                    u8_index++;
                 }
                if(pst_AppEntry->u32_SMSCharacterEncodingValue_4 != 0)
                 {
                    u8_buffer[u8_index] = 1; // Length
                    u8_buffer[++u8_index] = pst_AppEntry->u32_SMSCharacterEncodingValue_4; // UTF - 8
                    u8_index++;
                 }

             	st_LA_fld_AllowedEncoding.u16_DataLen = u8_index;
            	st_LA_fld_AllowedEncoding.pu8_Data = (void *)u8_buffer;
                
            	cmbs_util_LA_Fld_SMSAllowedEncodingAdd(pu8_Buffer, &st_LA_fld_AllowedEncoding, &u16_index);                
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);

}

static void PrepareIncomingSMSEntry(IN stSMSListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
            
        case CMBS_ISMSL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_Number.u8_FieldId     = CMBS_ISMSL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);                
            }
		break;
        
        case CMBS_ISMSL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_Name.u8_FieldId   = CMBS_ISMSL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;
        
        case CMBS_ISMSL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_MCL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_ISMSL_FLD_READ_STATUS:
            {
                ST_UTIL_LA_FLD_NEW st_LA_Fld_New;
                st_LA_Fld_New.u8_Attribute  = pst_AppEntry->b_Read ? 0x40 : 0x60;
                st_LA_Fld_New.u8_FieldId    = CMBS_ISMSL_FLD_READ_STATUS;
                cmbs_util_LA_Fld_NewAdd(pu8_Buffer, &st_LA_Fld_New, &u16_index);
            }
            break;
            
        case CMBS_ISMSL_FLD_SMS_ID:     
            {
                ST_UTIL_LA_FLD_SMS_ID st_LA_fld_SMSId;
                st_LA_fld_SMSId.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_SMSId.u8_FieldId   = CMBS_ISMSL_FLD_SMS_ID;
                st_LA_fld_SMSId.u8_Value     = pst_AppEntry->u32_SMSServiseId;
                cmbs_util_LA_Fld_SMSIdAdd(pu8_Buffer, &st_LA_fld_SMSId, &u16_index);
            }
            break;
            
        case CMBS_ISMSL_FLD_SMS_SIZE:     
            {
                ST_UTIL_SMS_SIZE st_LA_Fld_SMSSize;
                st_LA_Fld_SMSSize.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSSize.u8_FieldId     = CMBS_ISMSL_FLD_SMS_SIZE;
                st_LA_Fld_SMSSize.u8_Value = pst_AppEntry->u32_SMSSize;
                cmbs_util_LA_Fld_SMSSizeAdd(pu8_Buffer, &st_LA_Fld_SMSSize, &u16_index);
            }
            break;
            
        case CMBS_ISMSL_FLD_SMS_CONTENT:     
            {
                ST_UTIL_LA_FLD_SMS_CONTENT st_LA_Fld_SMSContent;
                st_LA_Fld_SMSContent.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSContent.u8_FieldId     = CMBS_ISMSL_FLD_SMS_CONTENT;
                st_LA_Fld_SMSContent.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sSMSContent);
                st_LA_Fld_SMSContent.pu8_Data     = (void*)pst_AppEntry->sSMSContent;
                cmbs_util_LA_Fld_SMSContentAdd(pu8_Buffer, &st_LA_Fld_SMSContent, &u16_index);                
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareDraftSMSEntry(IN stSMSListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;


    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
            
        case CMBS_DSMSL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_Number.u8_FieldId     = CMBS_DSMSL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);
                
            }
			break;
        
        case CMBS_DSMSL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_Name.u8_FieldId   = CMBS_DSMSL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;
        
        case CMBS_DSMSL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_DSMSL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_DSMSL_FLD_SMS_ID:
            {
                ST_UTIL_LA_FLD_SMS_ID st_LA_fld_SMSId;
                st_LA_fld_SMSId.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_SMSId.u8_FieldId   = CMBS_DSMSL_FLD_SMS_ID;
                st_LA_fld_SMSId.u8_Value     = pst_AppEntry->u32_SMSServiseId;
                cmbs_util_LA_Fld_SMSIdAdd(pu8_Buffer, &st_LA_fld_SMSId, &u16_index);
            }
            break;
            
        case CMBS_DSMSL_FLD_SEND_REQUEST:     
            {
                ST_UTIL_SEND_REQUEST st_LA_fld_SMSSendReq;
                st_LA_fld_SMSSendReq.u8_Attribute = pst_AppEntry->b_SendRequest? 0x40 : 0x60;
                st_LA_fld_SMSSendReq.u8_FieldId   = CMBS_DSMSL_FLD_SEND_REQUEST;
                cmbs_util_LA_Fld_SMSSendRequestAdd(pu8_Buffer, &st_LA_fld_SMSSendReq, &u16_index);                
            }
            break;
            
        case CMBS_DSMSL_FLD_NETWORK_ENC:     
            {
                ST_UTIL_NETWORK_SIDE_ENC st_LA_Fld_NetworkEnc;
                st_LA_Fld_NetworkEnc.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_NetworkEnc.u8_FieldId     = CMBS_DSMSL_FLD_NETWORK_ENC;
                st_LA_Fld_NetworkEnc.u8_UsedEncVariant_1= pst_AppEntry->u32_EncVariant_1;
                st_LA_Fld_NetworkEnc.u8_UsedEncVariant_2     = pst_AppEntry->u32_EncVariant_2;
                st_LA_Fld_NetworkEnc.u8_UsedNetworkSideEnc = pst_AppEntry->u32_NetworkSideEncoding;                
                cmbs_util_LA_Fld_NetworkSideEncAdd(pu8_Buffer, &st_LA_Fld_NetworkEnc, &u16_index);
            }
            break;
            
        case CMBS_DSMSL_FLD_SMS_SIZE:
            {
                ST_UTIL_SMS_SIZE st_LA_Fld_SMSSize;
                st_LA_Fld_SMSSize.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSSize.u8_FieldId     = CMBS_DSMSL_FLD_SMS_SIZE;
                st_LA_Fld_SMSSize.u8_Value = pst_AppEntry->u32_SMSSize;
                cmbs_util_LA_Fld_SMSSizeAdd(pu8_Buffer, &st_LA_Fld_SMSSize, &u16_index);
            }
            break;
                
        case CMBS_DSMSL_FLD_SMS_CONTENT:     
            {
                ST_UTIL_LA_FLD_SMS_CONTENT st_LA_Fld_SMSContent;
                st_LA_Fld_SMSContent.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSContent.u8_FieldId     = CMBS_DSMSL_FLD_SMS_CONTENT;
                st_LA_Fld_SMSContent.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sSMSContent);
                st_LA_Fld_SMSContent.pu8_Data     = (void*)pst_AppEntry->sSMSContent;
                cmbs_util_LA_Fld_SMSContentAdd(pu8_Buffer, &st_LA_Fld_SMSContent, &u16_index);                
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

static void PrepareOutgoingSMSEntry(IN stSMSListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
            
        case CMBS_OSMSL_FLD_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
                st_LA_Fld_Number.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_Number.u8_FieldId     = CMBS_OSMSL_FLD_NUMBER;
                st_LA_Fld_Number.u16_DataLen    = (u16) strlen((char*)pst_AppEntry->sNumber);
                st_LA_Fld_Number.pu8_Data       = (void*)pst_AppEntry->sNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_Number, &u16_index);
                
            }
			break;
        
        case CMBS_OSMSL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                st_LA_Fld_Name.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_Name.u8_FieldId   = CMBS_OSMSL_FLD_NAME;
                st_LA_Fld_Name.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sName);
                st_LA_Fld_Name.pu8_Data     = (void*)pst_AppEntry->sName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_Name, &u16_index);
            }
            break;
        
        case CMBS_OSMSL_FLD_DATETIME:
            {
                ST_UTIL_LA_FLD_DATETIME st_LA_Fld_DateTime;
                struct tm* pst_Time;
                pst_Time = localtime(&(pst_AppEntry->t_DateAndTime));

                st_LA_Fld_DateTime.u8_Attribute         = 0;
                st_LA_Fld_DateTime.u8_FieldId           = CMBS_OSMSL_FLD_DATETIME;
                st_LA_Fld_DateTime.u8_Coding            = 3;
                st_LA_Fld_DateTime.u8_interpretation    = 0;
                st_LA_Fld_DateTime.u8_Year              = pst_Time->tm_year;
                st_LA_Fld_DateTime.u8_Month             = pst_Time->tm_mon;
                st_LA_Fld_DateTime.u8_Day               = pst_Time->tm_mday;
                st_LA_Fld_DateTime.u8_Hours             = pst_Time->tm_hour;
                st_LA_Fld_DateTime.u8_Mins              = pst_Time->tm_min;
                st_LA_Fld_DateTime.u8_Secs              = pst_Time->tm_sec;
                st_LA_Fld_DateTime.u8_TimeZone          = 0;

                ConvertTimeToCatiqTime(&st_LA_Fld_DateTime);
                cmbs_util_LA_Fld_DateTimeAdd(pu8_Buffer, &st_LA_Fld_DateTime,  &u16_index);
            }
            break;

        case CMBS_OSMSL_FLD_SMS_ID:
            {
                ST_UTIL_LA_FLD_SMS_ID st_LA_fld_SMSId;
                st_LA_fld_SMSId.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_SMSId.u8_FieldId   = CMBS_OSMSL_FLD_SMS_ID;
                st_LA_fld_SMSId.u8_Value     = pst_AppEntry->u32_SMSServiseId;
                cmbs_util_LA_Fld_SMSIdAdd(pu8_Buffer, &st_LA_fld_SMSId, &u16_index);
            }
            break;
                        
        case CMBS_OSMSL_FLD_NETWORK_ENC:     
            {
                ST_UTIL_NETWORK_SIDE_ENC st_LA_Fld_NetworkEnc;
                st_LA_Fld_NetworkEnc.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_NetworkEnc.u8_FieldId     = CMBS_OSMSL_FLD_NETWORK_ENC;
                st_LA_Fld_NetworkEnc.u8_UsedEncVariant_1= pst_AppEntry->u32_EncVariant_1;
                st_LA_Fld_NetworkEnc.u8_UsedEncVariant_2     = pst_AppEntry->u32_EncVariant_2;
                st_LA_Fld_NetworkEnc.u8_UsedNetworkSideEnc = pst_AppEntry->u32_NetworkSideEncoding;                
                cmbs_util_LA_Fld_NetworkSideEncAdd(pu8_Buffer, &st_LA_Fld_NetworkEnc, &u16_index);
            }
            break;
            
        case CMBS_OSMSL_FLD_SMS_SIZE:
            {
                ST_UTIL_SMS_SIZE st_LA_Fld_SMSSize;
                st_LA_Fld_SMSSize.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSSize.u8_FieldId     = CMBS_OSMSL_FLD_SMS_SIZE;
                st_LA_Fld_SMSSize.u8_Value = pst_AppEntry->u32_SMSSize;
                cmbs_util_LA_Fld_SMSSizeAdd(pu8_Buffer, &st_LA_Fld_SMSSize, &u16_index);
            }
            break;
                
        case CMBS_OSMSL_FLD_SMS_CONTENT:     
            {
                ST_UTIL_LA_FLD_SMS_CONTENT st_LA_Fld_SMSContent;
                st_LA_Fld_SMSContent.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_SMSContent.u8_FieldId     = CMBS_OSMSL_FLD_SMS_CONTENT;
                st_LA_Fld_SMSContent.u16_DataLen  = (u16) strlen((char*)pst_AppEntry->sSMSContent);
                st_LA_Fld_SMSContent.pu8_Data     = (void*)pst_AppEntry->sSMSContent;
                cmbs_util_LA_Fld_SMSContentAdd(pu8_Buffer, &st_LA_Fld_SMSContent, &u16_index);                
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);
}

                                    
static void PrepareDTAMSettingsListEntry(IN stDTAMSettingsListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {
        case CMBS_DSL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                st_LA_Fld_LineId.u8_Attribute   = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_LineId.u8_FieldId     = CMBS_DSL_FLD_LINE_ID;
                if ( pst_AppEntry->u32_LineId == LIST_LINE_ID_NONE )
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_ALL_LINES_RELATED_TO;
                }
                else
                {
                    st_LA_Fld_LineId.Subtype = CMBS_UTIL_LA_LINE_ID_RELATED_TO;
                    st_LA_Fld_LineId.u16_LineNumber = pst_AppEntry->u32_LineId;
                }
                cmbs_util_LA_Fld_LineIdAdd(pu8_Buffer, &st_LA_Fld_LineId, &u16_index);
            }
            break;
            
        case CMBS_DSL_FLD_DTAM_ID:
            {
                ST_UTIL_LA_FLD_DTAM_ID st_LA_fld_DTAMId;
                st_LA_fld_DTAMId.u8_Attribute = (pst_AppEntry->b_DTAMType << 1) | CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_DTAMId.u8_FieldId   = CMBS_DSL_FLD_DTAM_ID;
                st_LA_fld_DTAMId.u8_Value     = (pst_AppEntry->b_DTAMLocation) << 6 |( ((u8)pst_AppEntry->u32_DTAMIdentifier)&0x3F);
                cmbs_util_LA_Fld_DTAMIdAdd(pu8_Buffer, &st_LA_fld_DTAMId, &u16_index);
            }
		break;
        
        case CMBS_DSL_FLD_DTAM_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_DTAMNumber;
                st_LA_Fld_DTAMNumber.u8_Attribute    = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_DTAMNumber.u8_FieldId      = CMBS_DSL_FLD_DTAM_NUMBER;
                st_LA_Fld_DTAMNumber.u16_DataLen     = (u16) strlen((char*)pst_AppEntry->sDTAMNumber);
                st_LA_Fld_DTAMNumber.pu8_Data        = (void*)pst_AppEntry->sDTAMNumber;
                cmbs_util_LA_Fld_NumberAdd(pu8_Buffer, &st_LA_Fld_DTAMNumber, &u16_index);
            }
            break;

        case CMBS_DSL_FLD_DTAM_ACTIVATION:
            {
                ST_UTIL_LA_FLD_DTAM_ACTIVATION st_LA_fld_DTAMActivation;
                st_LA_fld_DTAMActivation.u8_Attribute     = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_DTAMActivation.u8_FieldId       = CMBS_DSL_FLD_DTAM_ACTIVATION;
                st_LA_fld_DTAMActivation.u8_DTAMActivated = pst_AppEntry->b_DTAMActivation? 0x31 : 0x30;
                st_LA_fld_DTAMActivation.u8_DTAMTimeout   = ((pst_AppEntry->b_DTAMDefaultTimeout) << 7) | ((u8)pst_AppEntry->u32_DTAMTimeout);
                cmbs_util_LA_Fld_DTAMActivationAdd(pu8_Buffer, &st_LA_fld_DTAMActivation, &u16_index);
            }
            break;
            
        case CMBS_DSL_FLD_DTAM_WEBLINK:     
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_DTAMWeblink;
                st_LA_Fld_DTAMWeblink.u8_Attribute    = 0;
                st_LA_Fld_DTAMWeblink.u8_FieldId      = CMBS_DSL_FLD_DTAM_WEBLINK;
                st_LA_Fld_DTAMWeblink.u16_DataLen     = (u16) strlen((char*)pst_AppEntry->sDTAMWebLink);
                st_LA_Fld_DTAMWeblink.pu8_Data        = (void*)pst_AppEntry->sDTAMWebLink;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_DTAMWeblink, &u16_index);
            }
            break;
            
        case CMBS_DSL_FLD_DTAM_WELCOME_MSG_PARAMS:     
            {
                ST_UTIL_LA_FLD_WELCOME_MSG_PARAMS st_LA_fld_WelcomeMsgParams;
                st_LA_fld_WelcomeMsgParams.u8_Attribute = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_WelcomeMsgParams.u8_FieldId   = CMBS_DSL_FLD_DTAM_WELCOME_MSG_PARAMS;
                st_LA_fld_WelcomeMsgParams.u8_Value     = pst_AppEntry->u32_WelcomeMsgParams;
                cmbs_util_LA_Fld_WelcomeMsgParamsAdd(pu8_Buffer, &st_LA_fld_WelcomeMsgParams, &u16_index);
            }
            break;

        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);

}

static void PrepareDTAMWelcomeMsgListEntry(IN stDTAMWelcomeMessageListEntry* pst_AppEntry, IN PST_IE_LA_FIELDS pst_Fields,
                                    IN u8 pu8_Data[], OUT u16* pu16_EntryLen)
{
    u16 u16_index = 0, u16_FieldIdx;
    u8 pu8_Buffer[LIST_ENTRY_MAX_SIZE];
    u8* pu8_Entry;

    memset(pu8_Buffer, 0, LIST_ENTRY_MAX_SIZE);

    /* reserve max 4 bytes for EntryID and Length (each can consume up to 2 bytes), they will be handled at the end of the function */
    u16_index += 4;

    for ( u16_FieldIdx = 0; u16_FieldIdx < pst_Fields->u16_Length; ++u16_FieldIdx )
    {
        switch ( pst_Fields->pu16_FieldId[u16_FieldIdx] )
        {           
        case CMBS_DWML_FLD_DTAM_ID:
            {
				
                ST_UTIL_LA_FLD_DTAM_ID st_LA_fld_DTAMId;
                st_LA_fld_DTAMId.u8_Attribute = (pst_AppEntry->b_DTAMType << 1) | CMBS_LA_ATTRIB_EDITABLE;
                st_LA_fld_DTAMId.u8_FieldId   = CMBS_DSL_FLD_DTAM_ID;
                st_LA_fld_DTAMId.u8_Value     = (pst_AppEntry->b_DTAMLocation) << 6 |( ((u8)pst_AppEntry->u32_DTAMIdentifier)&0x3F);
                cmbs_util_LA_Fld_DTAMIdAdd(pu8_Buffer, &st_LA_fld_DTAMId, &u16_index);
            }
		break;
        
        case CMBS_DWML_FLD_POSITION_IDX:
            {
                ST_UTIL_LA_FLD_POSITION_IDX st_LA_Fld_PositionIdx;
                st_LA_Fld_PositionIdx.u8_Attribute    = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_PositionIdx.u8_FieldId      = CMBS_DWML_FLD_POSITION_IDX;
                st_LA_Fld_PositionIdx.u8_Value        = (u8) pst_AppEntry->u32_PositionIdx;
                cmbs_util_LA_Fld_DTAMPositionIdxAdd(pu8_Buffer, &st_LA_Fld_PositionIdx, &u16_index);
            }
            break;

        case CMBS_DWML_FLD_MSG_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_DTAMMsgName;
                st_LA_Fld_DTAMMsgName.u8_Attribute    = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_DTAMMsgName.u8_FieldId      = CMBS_DWML_FLD_MSG_NAME;
                st_LA_Fld_DTAMMsgName.u16_DataLen     = (u16) strlen((char*)pst_AppEntry->sRecordedMsgName);
                st_LA_Fld_DTAMMsgName.pu8_Data        = (void*)pst_AppEntry->sRecordedMsgName;
                cmbs_util_LA_Fld_NameAdd(pu8_Buffer, &st_LA_Fld_DTAMMsgName, &u16_index);
            }
            break;
            
        case CMBS_DWML_FLD_TIME_DURATION:     
            {
                ST_UTIL_LA_FLD_MSG_DURATION st_LA_Fld_MsgDuration;
                st_LA_Fld_MsgDuration.u8_Attribute       = CMBS_LA_ATTRIB_EDITABLE;
                st_LA_Fld_MsgDuration.u8_FieldId         = CMBS_DWML_FLD_TIME_DURATION;
                st_LA_Fld_MsgDuration.u8_DurationHours   = (u8)pst_AppEntry->u32_TimeDurationHours;
                st_LA_Fld_MsgDuration.u8_DurationMinutes = (u8)pst_AppEntry->u32_TimeDurationMinutes;
                st_LA_Fld_MsgDuration.u8_DurationSeconds = (u8)pst_AppEntry->u32_TimeDurationSeconds;
                cmbs_util_LA_Fld_DTAMMsgDurationAdd(pu8_Buffer, &st_LA_Fld_MsgDuration, &u16_index);
            }
            break;
            
        default:
            break;
        }
    }

    /* Add EntryId, Entry length */
    pu8_Entry = cmbs_util_LA_EntryLenght_EntryIdentifierAdd(pu8_Buffer, u16_index, pst_AppEntry->u32_EntryId);

    *pu16_EntryLen = (u16)(u16_index - (pu8_Entry - pu8_Buffer));

    /* copy to buffer */
    memcpy(pu8_Data, pu8_Entry, *pu16_EntryLen);

}

static void ConvertEntriesToCatiqDataBuffer(IN u32 u32_ListId, IN PST_IE_LA_FIELDS pst_Fields,
                                            IN u8 pu8_Entries[], IN u16 u16_NumOfEntries,
                                            OUT u8 pu8_Data[], INOUT u16* pu16_DataLen)
{
    u16 u16_Index, u16_DataOffset = 0, u16_EntryLen = 0;

    for ( u16_Index = 0; u16_Index < u16_NumOfEntries; ++u16_Index )
    {
        switch ( u32_ListId )
        {
        case LIST_TYPE_MISSED_CALLS:
            PrepareMissedCallEntry((stCallsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_OUTGOING_CALLS:
            PrepareOutgoingCallEntry((stCallsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_INCOMING_ACCEPTED_CALLS:
            PrepareIncomingAcceptedCallEntry((stCallsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_ALL_CALLS:
            PrepareAllCallEntry((stCallsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_ALL_INCOMING_CALLS:
            PrepareIncomingCallEntry((stCallsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_LINE_SETTINGS_LIST:
            PrepareLineSettingsEntry((stLineSettingsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_CONTACT_LIST:
            PrepareContactListEntry((stContactListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS:
            PrepareLineDiagnosticStatusEntry((stLineAndDiagnosticListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_SMS_SETTINGS_LIST:
            PrepareSMSSettingsEntry((stSMSSettingsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_INCOMING_SMS:
            PrepareIncomingSMSEntry((stSMSListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_OUTGOING_SMS:
        case LIST_TYPE_SENT_SMS:            
            PrepareOutgoingSMSEntry((stSMSListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_DRAFT_SMS:
            PrepareDraftSMSEntry((stSMSListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;            
        case LIST_TYPE_DTAM_SETTINGS_LIST:
            PrepareDTAMSettingsListEntry((stDTAMSettingsListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
        case LIST_TYPE_DTAM_WELCOME_MESSAGE:
            PrepareDTAMWelcomeMsgListEntry((stDTAMWelcomeMessageListEntry*)pu8_Entries + u16_Index, pst_Fields, pu8_Data + u16_DataOffset, &u16_EntryLen);
            break;
            
        default:
            break;
        }

        u16_DataOffset += u16_EntryLen;
    }

    *pu16_DataLen = u16_DataOffset;
}

static void ParseCallListEntry(IN u16 u16_SessionIdx, OUT u8 pu8_ParsedEntry[], OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    stCallsListEntry* pEntry = (stCallsListEntry*)pu8_ParsedEntry;
    u16 u16_Index = 0;
    u16 u16_EntryIdentifier;
    u16 u16_EntryLength;
    u8* pu8_FieldId;
    u8* pu8_BufferToParse;

    pu8_BufferToParse = g_pst_LASessions[u16_SessionIdx].pu8_DataBuffer;

    *pu32_FieldsNum = 0;

    /* Get EntryId, Entry Length */
    cmbs_util_LA_EntryLenght_EntryIdentifierGet(pu8_BufferToParse, &u16_EntryIdentifier, &u16_EntryLength, &u16_Index);

    while ( u16_Index < u16_EntryLength )
    {
        pu8_FieldId = pu8_BufferToParse + u16_Index;

        switch ( *pu8_FieldId )
        {
        /* Read only fields are ignored */
        case CMBS_MCL_FLD_NUMBER:
        case CMBS_MCL_FLD_NAME:
        case CMBS_MCL_FLD_DATETIME:
        case CMBS_MCL_FLD_LINE_NAME:
        case CMBS_MCL_FLD_LINE_ID:
        case CMBS_MCL_FLD_NR_OF_CALLS:
            break;

        case CMBS_MCL_FLD_NEW:
            {
                ST_UTIL_LA_FLD_NEW st_LA_Fld_New;
                cmbs_util_LA_Fld_NewGet( pu8_FieldId, &st_LA_Fld_New);

                pEntry->bRead = (st_LA_Fld_New.u8_Attribute & 0x20) ? FALSE : TRUE;

                pu32_Fields[(*pu32_FieldsNum)++] = CATiqFieldId_2_AppFieldId(g_pst_LASessions[u16_SessionIdx].u32_ListId, *pu8_FieldId);
            }
            break;

        default:
            break;
        }

        cmbs_util_la_ShiftIndexToNextFieldID(pu8_FieldId, &u16_Index);
    }
}

static void ParseLineSettingsEntry(IN u16 u16_SessionIdx, OUT u8 pu8_ParsedEntry[], OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    stLineSettingsListEntry* pEntry = (stLineSettingsListEntry*)pu8_ParsedEntry;
    u16 u16_Index = 0;
    u16 u16_EntryIdentifier;
    u16 u16_EntryLength;
    u8* pu8_FieldId;
    u8* pu8_BufferToParse;

    pu8_BufferToParse = g_pst_LASessions[u16_SessionIdx].pu8_DataBuffer;

    *pu32_FieldsNum = 0;

    /* Get EntryId, Entry Length */
    cmbs_util_LA_EntryLenght_EntryIdentifierGet(pu8_BufferToParse, &u16_EntryIdentifier, &u16_EntryLength, &u16_Index);

    while ( u16_Index < u16_EntryLength )
    {
        pu8_FieldId = pu8_BufferToParse + u16_Index;

        pu32_Fields[(*pu32_FieldsNum)++] = CATiqFieldId_2_AppFieldId(g_pst_LASessions[u16_SessionIdx].u32_ListId, *pu8_FieldId);

        switch ( *pu8_FieldId )
        {
        case CMBS_LSL_FLD_LINE_NAME:
            {
                ST_UTIL_LA_FLD_LINE_NAME st_LA_Fld_LineName;
                cmbs_util_LA_Fld_LineNameGet( pu8_FieldId, &st_LA_Fld_LineName );

                memcpy(pEntry->sLineName, st_LA_Fld_LineName.pu8_Data, st_LA_Fld_LineName.u16_DataLen);
                pEntry->sLineName[st_LA_Fld_LineName.u16_DataLen] = 0;
            }
            break;

        case CMBS_LSL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                cmbs_util_LA_Fld_LineIdGet( pu8_FieldId, &st_LA_Fld_LineId );

                pEntry->u32_LineId = st_LA_Fld_LineId.u16_LineNumber;
            }
            break;

        case CMBS_LSL_FLD_ATTACHED_HANDSETS:
            {
                ST_UTIL_LA_FLD_ATTACHED_HANDSETS st_LA_Fld_AttachedHandset;
                cmbs_util_LA_Fld_AttachedHandsetsGet( pu8_FieldId, &st_LA_Fld_AttachedHandset );

                pEntry->u32_AttachedHsMask = st_LA_Fld_AttachedHandset.pu8_Data[0];
            }
            break;

        case CMBS_LSL_FLD_DIALING_PREFIX:
            {
                ST_UTIL_LA_FLD_DIALING_PREFIX st_LA_Fld_DialingPrefixe;
                cmbs_util_LA_Fld_DialPrefixeGet( pu8_FieldId, &st_LA_Fld_DialingPrefixe );

                memcpy(pEntry->sDialPrefix, st_LA_Fld_DialingPrefixe.pu8_Data, st_LA_Fld_DialingPrefixe.u16_DataLen);
                pEntry->sDialPrefix[st_LA_Fld_DialingPrefixe.u16_DataLen] = 0;
            }
            break;

        case CMBS_LSL_FLD_FP_MELODY:
            {
                ST_UTIL_LA_FLD_FP_MELODY st_LA_Fld_FPmelody;
                cmbs_util_LA_Fld_AssocMdyGet( pu8_FieldId, &st_LA_Fld_FPmelody );

                pEntry->u32_FPMelody = st_LA_Fld_FPmelody.u8_Value;
            }
            break;

        case CMBS_LSL_FLD_FP_VOLUME:
            {
                ST_UTIL_LA_FLD_FP_VOLUME st_LA_Fld_FPvolume;
                cmbs_util_LA_Fld_FPvolumeGet( pu8_FieldId, &st_LA_Fld_FPvolume );

                pEntry->u32_FPVolume = st_LA_Fld_FPvolume.u8_Value;
            }
            break;

        case CMBS_LSL_FLD_BLOCKED_NUMBER:
            {
                ST_UTIL_LA_FLD_BLOCKED_NUMBER st_LA_Fld_BlockedNumber;
                cmbs_util_LA_Fld_BlockedNumberGet( pu8_FieldId, &st_LA_Fld_BlockedNumber );

                memcpy(pEntry->sBlockedNumber, st_LA_Fld_BlockedNumber.pu8_Data, st_LA_Fld_BlockedNumber.u16_DataLen);
                pEntry->sBlockedNumber[st_LA_Fld_BlockedNumber.u16_DataLen] = 0;
            }
            break;

        case CMBS_LSL_FLD_MULTI_CALL_MODE:
            {
                ST_UTIL_LA_FLD_MULTI_CALL_MODE st_LA_Fld_MultiCallMode;
                cmbs_util_LA_Fld_MultiCallModeGet( pu8_FieldId, &st_LA_Fld_MultiCallMode );

                pEntry->bMultiCalls = st_LA_Fld_MultiCallMode.u8_Value == 0x31 ? TRUE : FALSE;
            }
            break;

        case CMBS_LSL_FLD_INTRUSION_CALL:
            {
                ST_UTIL_LA_FLD_INTRUSION_CALL st_LA_Fld_IntrusionCall;
                cmbs_util_LA_Fld_IntrusionCallGet( pu8_FieldId, &st_LA_Fld_IntrusionCall );

                pEntry->bIntrusionCall = st_LA_Fld_IntrusionCall.u8_Value == 0x31 ? TRUE : FALSE;
            }
            break;

        case CMBS_LSL_FLD_PERMANENT_CLIR:
            {
                ST_UTIL_LA_FLD_PERMANENT_CLIR st_LA_Fld_CLIR;
                cmbs_util_LA_Fld_PermanentCLIRGet( pu8_FieldId, &st_LA_Fld_CLIR );

                pEntry->bPermanentCLIR = st_LA_Fld_CLIR.u8_Value == 0x31 ? TRUE : FALSE;

                memcpy(pEntry->sPermanentCLIRActCode, st_LA_Fld_CLIR.pu8_ActData, st_LA_Fld_CLIR.u8_ActDataLen);
                pEntry->sPermanentCLIRActCode[st_LA_Fld_CLIR.u8_ActDataLen] = 0;

                memcpy(pEntry->sPermanentCLIRDeactCode, st_LA_Fld_CLIR.pu8_DesactData, st_LA_Fld_CLIR.u8_DesactDataLen);
                pEntry->sPermanentCLIRDeactCode[st_LA_Fld_CLIR.u8_DesactDataLen] = 0;
            }
            break;

        case CMBS_LSL_FLD_CALL_FORWARDING:
            {
                ST_UTIL_LA_FLD_CALL_FORWARDING st_LA_Fld_FW;
                cmbs_util_LA_Fld_CallForwardingGet( pu8_FieldId, &st_LA_Fld_FW );

                pEntry->bCallFwdUncond = st_LA_Fld_FW.u8_Value == 0x31 ? TRUE : FALSE;

                memcpy(pEntry->sCallFwdUncondActCode, st_LA_Fld_FW.pu8_ActData, st_LA_Fld_FW.u8_ActDataLen);
                pEntry->sCallFwdUncondActCode[st_LA_Fld_FW.u8_ActDataLen] = 0;

                memcpy(pEntry->sCallFwdUncondDeactCode, st_LA_Fld_FW.pu8_DesactData, st_LA_Fld_FW.u8_DesactDataLen);
                pEntry->sCallFwdUncondDeactCode[st_LA_Fld_FW.u8_DesactDataLen] = 0;

                memcpy(pEntry->sCallFwdUncondNum, st_LA_Fld_FW.pu8_Number, st_LA_Fld_FW.u8_NumberLen);
                pEntry->sCallFwdUncondNum[st_LA_Fld_FW.u8_NumberLen] = 0;
            }
            break;

        case CMBS_LSL_FLD_CFNA:
            {
                ST_UTIL_LA_FLD_CFNA st_LA_Fld_CFNA;
                cmbs_util_LA_Fld_CFNAGet( pu8_FieldId, &st_LA_Fld_CFNA);

                pEntry->bCallFwdNoAns = st_LA_Fld_CFNA.u8_Value == 0x31 ? TRUE : FALSE;

                memcpy(pEntry->sCallFwdNoAnsActCode, st_LA_Fld_CFNA.pu8_ActData, st_LA_Fld_CFNA.u8_ActDataLen);
                pEntry->sCallFwdNoAnsActCode[st_LA_Fld_CFNA.u8_ActDataLen] = 0;

                memcpy(pEntry->sCallFwdNoAnsDeactCode, st_LA_Fld_CFNA.pu8_DesactData, st_LA_Fld_CFNA.u8_DesactDataLen);
                pEntry->sCallFwdNoAnsDeactCode[st_LA_Fld_CFNA.u8_DesactDataLen] = 0;

                memcpy(pEntry->sCallFwdNoAnsNum, st_LA_Fld_CFNA.pu8_Number, st_LA_Fld_CFNA.u8_NumberLen);
                pEntry->sCallFwdNoAnsNum[st_LA_Fld_CFNA.u8_NumberLen] = 0;

                pEntry->u32_CallFwdNoAnsTimeout = st_LA_Fld_CFNA.u8_Timeout;
            }
            break;

        case CMBS_LSL_FLD_CFB:
            {
                ST_UTIL_LA_FLD_CFB st_LA_Fld_CFB;
                cmbs_util_LA_Fld_CFBGet( pu8_FieldId, &st_LA_Fld_CFB);

                pEntry->bCallFwdBusy = st_LA_Fld_CFB.u8_Value == 0x31 ? TRUE : FALSE;

                memcpy(pEntry->sCallFwdBusyActCode, st_LA_Fld_CFB.pu8_ActData, st_LA_Fld_CFB.u8_ActDataLen);
                pEntry->sCallFwdBusyActCode[st_LA_Fld_CFB.u8_ActDataLen] = 0;

                memcpy(pEntry->sCallFwdBusyDeactCode, st_LA_Fld_CFB.pu8_DesactData, st_LA_Fld_CFB.u8_DesactDataLen);
                pEntry->sCallFwdBusyDeactCode[st_LA_Fld_CFB.u8_DesactDataLen] = 0;

                memcpy(pEntry->sCallFwdBusyNum, st_LA_Fld_CFB.pu8_Number, st_LA_Fld_CFB.u8_NumberLen);
                pEntry->sCallFwdBusyNum[st_LA_Fld_CFB.u8_NumberLen] = 0;
            }
            break;

        default:
            break;
        }

        cmbs_util_la_ShiftIndexToNextFieldID(pu8_FieldId, &u16_Index);
    }
}

static void ParseContactListEntry(IN u16 u16_SessionIdx, OUT u8 pu8_ParsedEntry[], OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    stContactListEntry* pEntry = (stContactListEntry*)pu8_ParsedEntry;
    u16 u16_Index = 0;
    u16 u16_EntryIdentifier;
    u16 u16_EntryLength;
    u8* pu8_FieldId;
    u8* pu8_BufferToParse;
    u8  u8_ContactNumber = 1;

    pu8_BufferToParse = g_pst_LASessions[u16_SessionIdx].pu8_DataBuffer;

    *pu32_FieldsNum = 0;

    /* Get EntryId, Entry Length */
    cmbs_util_LA_EntryLenght_EntryIdentifierGet(pu8_BufferToParse, &u16_EntryIdentifier, &u16_EntryLength, &u16_Index);

    while ( u16_Index < u16_EntryLength )
    {
        pu8_FieldId = pu8_BufferToParse + u16_Index;

        pu32_Fields[(*pu32_FieldsNum)++] = CATiqFieldId_2_AppFieldId(g_pst_LASessions[u16_SessionIdx].u32_ListId, *pu8_FieldId);

        switch ( *pu8_FieldId )
        {
        case CMBS_CL_FLD_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
                cmbs_util_LA_Fld_NameGet( pu8_FieldId, &st_LA_Fld_Name );

                memcpy(pEntry->sLastName, st_LA_Fld_Name.pu8_Data, st_LA_Fld_Name.u16_DataLen);
                pEntry->sLastName[st_LA_Fld_Name.u16_DataLen] = 0;
            }
            break;

        case CMBS_CL_FLD_FIRST_NAME:
            {
                ST_UTIL_LA_FLD_FIRST_NAME st_LA_Fld_FirstName;
                cmbs_util_LA_Fld_FirstNameGet( pu8_FieldId, &st_LA_Fld_FirstName );

                memcpy(pEntry->sFirstName, st_LA_Fld_FirstName.pu8_Data, st_LA_Fld_FirstName.u16_DataLen);
                pEntry->sFirstName[st_LA_Fld_FirstName.u16_DataLen] = 0;
            }
            break;

        case CMBS_CL_FLD_CONTACT_NR:
            {
                ST_UTIL_LA_FLD_CONTACT_NR st_LA_Fld_ContactNumber;
                cmbs_util_LA_Fld_ContactNrGet( pu8_FieldId, &st_LA_Fld_ContactNumber);

                if ( u8_ContactNumber == 1 )
                {
                    /* First Number */
                    memcpy(pEntry->sNumber1, st_LA_Fld_ContactNumber.pu8_Data, st_LA_Fld_ContactNumber.u16_DataLen);
                    pEntry->sNumber1[st_LA_Fld_ContactNumber.u16_DataLen] = 0;

                    pEntry->bNumber1Default = st_LA_Fld_ContactNumber.u8_Attribute & 0x20;
                    pEntry->bNumber1Own     = st_LA_Fld_ContactNumber.u8_Attribute & 0x10;

                    if ( st_LA_Fld_ContactNumber.u8_Attribute & 0x08 )
                    {
                        pEntry->cNumber1Type = NUM_TYPE_FIXED;
                    }
                    else if ( st_LA_Fld_ContactNumber.u8_Attribute & 0x04 )
                    {
                        pEntry->cNumber1Type = NUM_TYPE_MOBILE;
                    }
                    else
                    {
                        pEntry->cNumber1Type = NUM_TYPE_WORK;
                    }
                }
                else if ( u8_ContactNumber == 2 )
                {
                    /* Second number */
                    memcpy(pEntry->sNumber2, st_LA_Fld_ContactNumber.pu8_Data, st_LA_Fld_ContactNumber.u16_DataLen);
                    pEntry->sNumber2[st_LA_Fld_ContactNumber.u16_DataLen] = 0;

                    pEntry->bNumber2Default = st_LA_Fld_ContactNumber.u8_Attribute & 0x20;
                    pEntry->bNumber2Own     = st_LA_Fld_ContactNumber.u8_Attribute & 0x10;

                    if ( st_LA_Fld_ContactNumber.u8_Attribute & 0x08 )
                    {
                        pEntry->cNumber2Type = NUM_TYPE_FIXED;
                    }
                    else if ( st_LA_Fld_ContactNumber.u8_Attribute & 0x04 )
                    {
                        pEntry->cNumber2Type = NUM_TYPE_MOBILE;
                    }
                    else
                    {
                        pEntry->cNumber2Type = NUM_TYPE_WORK;
                    }

                    /* overwrite num1 field id with num2 field id */
                    pu32_Fields[*pu32_FieldsNum - 1] = FIELD_ID_CONTACT_NUM_2;
                }
                else
                {
                    /* Third number */
                    memcpy(pEntry->sNumber3, st_LA_Fld_ContactNumber.pu8_Data, st_LA_Fld_ContactNumber.u16_DataLen);
                    pEntry->sNumber3[st_LA_Fld_ContactNumber.u16_DataLen] = 0;

                    pEntry->bNumber3Default = st_LA_Fld_ContactNumber.u8_Attribute & 0x20;
                    pEntry->bNumber3Own     = st_LA_Fld_ContactNumber.u8_Attribute & 0x10;

                    if ( st_LA_Fld_ContactNumber.u8_Attribute & 0x08 )
                    {
                        pEntry->cNumber3Type = NUM_TYPE_FIXED;
                    }
                    else if ( st_LA_Fld_ContactNumber.u8_Attribute & 0x04 )
                    {
                        pEntry->cNumber3Type = NUM_TYPE_MOBILE;
                    }
                    else
                    {
                        pEntry->cNumber3Type = NUM_TYPE_WORK;
                    }

                    /* overwrite num1 field id with num3 field id */
                    pu32_Fields[*pu32_FieldsNum - 1] = FIELD_ID_CONTACT_NUM_3;
                }

                u8_ContactNumber++;
            }
            break;

        case CMBS_CL_FLD_ASSOC_MDY:
            {
                ST_UTIL_LA_FLD_ASSOC_MDY st_LA_Fld_Associated_melody;
                cmbs_util_LA_Fld_AssocMdyGet( pu8_FieldId, &st_LA_Fld_Associated_melody );

                pEntry->u32_AssociatedMelody = st_LA_Fld_Associated_melody.u8_Value;
            }
            break;

        case CMBS_CL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                cmbs_util_LA_Fld_LineIdGet( pu8_FieldId, &st_LA_Fld_LineId );

                pEntry->u32_LineId = (st_LA_Fld_LineId.Subtype == CMBS_UTIL_LA_LINE_ID_ALL_LINES_RELATED_TO) ? 
                                     LIST_LINE_ID_NONE : st_LA_Fld_LineId.u16_LineNumber;
            }
            break;

        default:
            break;
        }

        cmbs_util_la_ShiftIndexToNextFieldID(pu8_FieldId, &u16_Index);
    }
}

static void ParseSMSSettingsListEntry(IN u16 u16_SessionIdx, OUT u8 pu8_ParsedEntry[], OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    stSMSSettingsListEntry* pEntry = (stSMSSettingsListEntry*)pu8_ParsedEntry;
    u16 u16_Index = 0;
    u16 u16_EntryIdentifier;
    u16 u16_EntryLength;
    u8* pu8_FieldId;
    u8* pu8_BufferToParse;

    pu8_BufferToParse = g_pst_LASessions[u16_SessionIdx].pu8_DataBuffer;

    *pu32_FieldsNum = 0;

    /* Get EntryId, Entry Length */
    cmbs_util_LA_EntryLenght_EntryIdentifierGet(pu8_BufferToParse, &u16_EntryIdentifier, &u16_EntryLength, &u16_Index);

    while ( u16_Index < u16_EntryLength )
    {
        pu8_FieldId = pu8_BufferToParse + u16_Index;

        pu32_Fields[(*pu32_FieldsNum)++] = CATiqFieldId_2_AppFieldId(g_pst_LASessions[u16_SessionIdx].u32_ListId, *pu8_FieldId);

        switch ( *pu8_FieldId )
        {
        case CMBS_SSL_FLD_SMS_ID:
        {
            ST_UTIL_LA_FLD_SMS_ID st_LA_Fld_SMSId;
            cmbs_util_LA_Fld_SMSIdGet( pu8_FieldId, &st_LA_Fld_SMSId );

			pEntry->u32_SMSServiseId = st_LA_Fld_SMSId.u8_Value;
        }
        break;

        case CMBS_SSL_FLD_LINE_ID:
        {
           ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
           cmbs_util_LA_Fld_LineIdGet( pu8_FieldId, &st_LA_Fld_LineId );

           pEntry->u32_LineId = st_LA_Fld_LineId.u16_LineNumber;
        }
        break;

        case CMBS_SSL_FLD_ENABLE_SMS:
        {
           ST_UTIL_LA_SMS_ENABLE st_LA_Fld_SMSEnable;
           cmbs_util_LA_Fld_SMSEnableGet( pu8_FieldId, &st_LA_Fld_SMSEnable);
                
           pEntry->b_EnableSMS = st_LA_Fld_SMSEnable.u8_Value == 0x31 ? TRUE : FALSE;
        }
        break;

        case CMBS_SSL_FLD_MAX_SMS_SIZE:
        {
             ST_UTIL_LA_FLD_MAX_SMS_SIZE st_LA_Fld_MaxSMSSize;
             cmbs_util_LA_Fld_SMSMaxSizeGet( pu8_FieldId, &st_LA_Fld_MaxSMSSize );

			 pEntry->u32_SMSMaxSize = st_LA_Fld_MaxSMSSize.u8_Value;
        }
        break;

        case CMBS_SSL_FLD_SMSC_SEND_SERVER:
        {
            ST_UTIL_LA_FLD_SMSC_SERVER st_LA_Fld_SMSCSendServer;
            cmbs_util_LA_Fld_SMSCServerGet( pu8_FieldId, &st_LA_Fld_SMSCSendServer );

            memcpy(pEntry->sSMSCSendServer, st_LA_Fld_SMSCSendServer.pu8_Data, st_LA_Fld_SMSCSendServer.u16_DataLen);
            pEntry->sSMSCSendServer[st_LA_Fld_SMSCSendServer.u16_DataLen] = 0;
        }
        break;
            
        case CMBS_SSL_FLD_SMSC_RECEIVE_SERVER:
       	{
            ST_UTIL_LA_FLD_SMSC_SERVER st_LA_Fld_SMSCReceiveServer;
            cmbs_util_LA_Fld_SMSCServerGet( pu8_FieldId, &st_LA_Fld_SMSCReceiveServer );

            memcpy(pEntry->sSMSCReceiveServer, st_LA_Fld_SMSCReceiveServer.pu8_Data, st_LA_Fld_SMSCReceiveServer.u16_DataLen);
            pEntry->sSMSCReceiveServer[st_LA_Fld_SMSCReceiveServer.u16_DataLen] = 0;
        }
		break;
     	case CMBS_SSL_FLD_SMS_DELIVERY_REPORT:
        {
             ST_UTIL_LA_FLD_SMS_DELIVERY_REPORT st_LA_Fld_SMSDeliveryReport;
             cmbs_util_LA_Fld_SMSDeliveryReportGet( pu8_FieldId, &st_LA_Fld_SMSDeliveryReport );

             pEntry->b_SMSDeliveryReport = st_LA_Fld_SMSDeliveryReport.u8_Value == 0x31 ? TRUE : FALSE;
         }
		 break;
		 case CMBS_SSL_FLD_SMS_VALIDITY_PERIOD:
         {
              ST_UTIL_LA_FLD_SMS_VALIDITY_PERIOD st_LA_Fld_SMSValidityPeriod;
              cmbs_util_LA_Fld_SMSValidityPeriodGet( pu8_FieldId, &st_LA_Fld_SMSValidityPeriod );

              pEntry->u32_SMSValidityPeriod = st_LA_Fld_SMSValidityPeriod.u8_Value;
         }
		 break;
         case CMBS_SSL_FLD_ALLOWED_ENCODING:
         {
              ST_UTIL_LA_FLD_ALLOWED_ENCODING st_LA_Fld_SMSAllowedEncoding;
              cmbs_util_LA_Fld_SMSAllowedEncodingGet( pu8_FieldId, &st_LA_Fld_SMSAllowedEncoding );
         }
        break;
        
        default:
            break;
        }

        cmbs_util_la_ShiftIndexToNextFieldID(pu8_FieldId, &u16_Index);
    }
}

static void ParseSMSListEntry(IN u16 u16_SessionIdx, OUT u8 pu8_ParsedEntry[], OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    stSMSListEntry* pEntry = (stSMSListEntry*)pu8_ParsedEntry;
    u16 u16_Index = 0;
    u16 u16_EntryIdentifier;
    u16 u16_EntryLength;
    u8* pu8_FieldId;
    u8* pu8_BufferToParse;

    pu8_BufferToParse = g_pst_LASessions[u16_SessionIdx].pu8_DataBuffer;

    *pu32_FieldsNum = 0;

    /* Get EntryId, Entry Length */
    cmbs_util_LA_EntryLenght_EntryIdentifierGet(pu8_BufferToParse, &u16_EntryIdentifier, &u16_EntryLength, &u16_Index);

    while ( u16_Index < u16_EntryLength )
    {
        pu8_FieldId = pu8_BufferToParse + u16_Index;

        switch ( *pu8_FieldId )
        {
        case CMBS_DSMSL_FLD_NUMBER:
          {
               ST_UTIL_LA_FLD_NUMBER st_LA_Fld_Number;
               cmbs_util_LA_Fld_NumberGet( pu8_FieldId, &st_LA_Fld_Number );
               memcpy(pEntry->sNumber, st_LA_Fld_Number.pu8_Data, st_LA_Fld_Number.u16_DataLen);
               pEntry->sNumber[st_LA_Fld_Number.u16_DataLen] = 0;
           }
           break;

        case CMBS_DSMSL_FLD_NAME:
           {
               ST_UTIL_LA_FLD_NAME st_LA_Fld_Name;
               cmbs_util_LA_Fld_NameGet( pu8_FieldId, &st_LA_Fld_Name );
               memcpy(pEntry->sName, st_LA_Fld_Name.pu8_Data, st_LA_Fld_Name.u16_DataLen);
               pEntry->sName[st_LA_Fld_Name.u16_DataLen] = 0;
            }
           break;
            
        case CMBS_DSMSL_FLD_DATETIME: 
            break;
            
        case CMBS_DSMSL_FLD_SMS_ID:
            {
                ST_UTIL_LA_FLD_SMS_ID st_LA_Fld_SMSId;
                cmbs_util_LA_Fld_SMSIdGet( pu8_FieldId, &st_LA_Fld_SMSId );
    	      	pEntry->u32_SMSServiseId = st_LA_Fld_SMSId.u8_Value;            
            }
            break;
            
        case CMBS_DSMSL_FLD_SEND_REQUEST:
            {
                ST_UTIL_SEND_REQUEST st_LA_Fld_SendReq;
                cmbs_util_LA_Fld_SMSSendRequestGet( pu8_FieldId, &st_LA_Fld_SendReq);
                pEntry->b_SendRequest = (st_LA_Fld_SendReq.u8_Attribute & 0x20) ? FALSE : TRUE;            
            }
            break;
            
        case CMBS_DSMSL_FLD_NETWORK_ENC:
            {
                ST_UTIL_NETWORK_SIDE_ENC st_LA_Fld_NetworkEnc;
                cmbs_util_LA_Fld_NetworkSideEncGet(pu8_FieldId, &st_LA_Fld_NetworkEnc);
                pEntry->u32_NetworkSideEncoding = st_LA_Fld_NetworkEnc.u8_UsedNetworkSideEnc;
                pEntry->u32_EncVariant_1 = st_LA_Fld_NetworkEnc.u8_UsedEncVariant_1;
                pEntry->u32_EncVariant_2 = st_LA_Fld_NetworkEnc.u8_UsedEncVariant_2;           
            }
            break;
            
        case CMBS_DSMSL_FLD_SMS_SIZE:
            {
                ST_UTIL_SMS_SIZE st_LA_Fld_SMSSize;
                cmbs_util_LA_Fld_SMSSizeGet(pu8_FieldId, &st_LA_Fld_SMSSize);
                pEntry->u32_SMSSize = st_LA_Fld_SMSSize.u8_Value;
            }
            break;
            
        case CMBS_DSMSL_FLD_SMS_CONTENT:
           {
               ST_UTIL_LA_FLD_SMS_CONTENT st_LA_Fld_SMSContent;
               cmbs_util_LA_Fld_SMSContentGet( pu8_FieldId, &st_LA_Fld_SMSContent );
			   memcpy(pEntry->sSMSContent, st_LA_Fld_SMSContent.pu8_Data, st_LA_Fld_SMSContent.u16_DataLen);
			   pEntry->sSMSContent[st_LA_Fld_SMSContent.u16_DataLen] = 0;
            }
           break;

        default:
            break;
        }

        cmbs_util_la_ShiftIndexToNextFieldID(pu8_FieldId, &u16_Index);
    }
}

static void ParseDTAMSettingsListEntry(IN u16 u16_SessionIdx, OUT u8 pu8_ParsedEntry[], OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    stDTAMSettingsListEntry* pEntry = (stDTAMSettingsListEntry*)pu8_ParsedEntry;
    u16 u16_Index = 0;
    u16 u16_EntryIdentifier;
    u16 u16_EntryLength;
    u8* pu8_FieldId;
    u8* pu8_BufferToParse;

    pu8_BufferToParse = g_pst_LASessions[u16_SessionIdx].pu8_DataBuffer;

    *pu32_FieldsNum = 0;

    /* Get EntryId, Entry Length */
    cmbs_util_LA_EntryLenght_EntryIdentifierGet(pu8_BufferToParse, &u16_EntryIdentifier, &u16_EntryLength, &u16_Index);

    while ( u16_Index < u16_EntryLength )
    {
        pu8_FieldId = pu8_BufferToParse + u16_Index;

        pu32_Fields[(*pu32_FieldsNum)++] = CATiqFieldId_2_AppFieldId(g_pst_LASessions[u16_SessionIdx].u32_ListId, *pu8_FieldId);

        switch ( *pu8_FieldId )
        {
        case CMBS_DSL_FLD_LINE_ID:
            {
                ST_UTIL_LA_FLD_LINE_ID st_LA_Fld_LineId;
                cmbs_util_LA_Fld_LineIdGet( pu8_FieldId, &st_LA_Fld_LineId );

                pEntry->u32_LineId = st_LA_Fld_LineId.u16_LineNumber;
            }
            break;

        case CMBS_DSL_FLD_DTAM_ID:
            {
                ST_UTIL_LA_FLD_DTAM_ID st_LA_Fld_DTAMId;
                cmbs_util_LA_Fld_DTAMIdGet( pu8_FieldId, &st_LA_Fld_DTAMId );
                pEntry->u32_DTAMIdentifier = st_LA_Fld_DTAMId.u8_Value & 0x3F;
                pEntry->b_DTAMLocation     = st_LA_Fld_DTAMId.u8_Value & 0x40;
                pEntry->b_DTAMType         = st_LA_Fld_DTAMId.u8_Attribute & 0x2;
            }
            break;

        case CMBS_DSL_FLD_DTAM_NUMBER:
            {
                ST_UTIL_LA_FLD_NUMBER st_LA_Fld_DTAMNumber;
                cmbs_util_LA_Fld_NumberGet( pu8_FieldId, &st_LA_Fld_DTAMNumber);

                memcpy(pEntry->sDTAMNumber, st_LA_Fld_DTAMNumber.pu8_Data, st_LA_Fld_DTAMNumber.u16_DataLen);
                pEntry->sDTAMNumber[st_LA_Fld_DTAMNumber.u16_DataLen] = 0;
            }
            break;

        case CMBS_DSL_FLD_DTAM_ACTIVATION:
            {
                ST_UTIL_LA_FLD_DTAM_ACTIVATION st_LA_Fld_DTAMActivation;
                cmbs_util_LA_Fld_DTAMActivationGet( pu8_FieldId, &st_LA_Fld_DTAMActivation );

                pEntry->b_DTAMActivation     = st_LA_Fld_DTAMActivation.u8_DTAMActivated == 0x31 ? TRUE : FALSE;
                pEntry->u32_DTAMTimeout      = st_LA_Fld_DTAMActivation.u8_DTAMTimeout & 0x7F;
                pEntry->b_DTAMDefaultTimeout = st_LA_Fld_DTAMActivation.u8_DTAMTimeout & 0x80;
            }
            break;

        case CMBS_DSL_FLD_DTAM_WEBLINK:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_DTAMWeblink;
                cmbs_util_LA_Fld_NameGet( pu8_FieldId, &st_LA_Fld_DTAMWeblink );

                memcpy(pEntry->sDTAMWebLink, st_LA_Fld_DTAMWeblink.pu8_Data, st_LA_Fld_DTAMWeblink.u16_DataLen);
                pEntry->sDTAMWebLink[st_LA_Fld_DTAMWeblink.u16_DataLen] = 0;
            }
            break;
            
        case CMBS_DSL_FLD_DTAM_WELCOME_MSG_PARAMS:
       {
                ST_UTIL_LA_FLD_WELCOME_MSG_PARAMS st_LA_Fld_DTAMWelcomeMsgParams;
                cmbs_util_LA_Fld_WelcomeMsgParamsGet( pu8_FieldId, &st_LA_Fld_DTAMWelcomeMsgParams );

                pEntry->u32_WelcomeMsgParams = st_LA_Fld_DTAMWelcomeMsgParams.u8_Value;
        }
        break;
        
        default:
            break;
        }

        cmbs_util_la_ShiftIndexToNextFieldID(pu8_FieldId, &u16_Index);
    }
}

static void ParseDTAMWelcomeMsgListEntry(IN u16 u16_SessionIdx, OUT u8 pu8_ParsedEntry[], OUT u32 pu32_Fields[], OUT u32* pu32_FieldsNum)
{
    stDTAMWelcomeMessageListEntry* pEntry = (stDTAMWelcomeMessageListEntry*)pu8_ParsedEntry;
    u16 u16_Index = 0;
    u16 u16_EntryIdentifier;
    u16 u16_EntryLength;
    u8* pu8_FieldId;
    u8* pu8_BufferToParse;

    pu8_BufferToParse = g_pst_LASessions[u16_SessionIdx].pu8_DataBuffer;

    *pu32_FieldsNum = 0;

    /* Get EntryId, Entry Length */
    cmbs_util_LA_EntryLenght_EntryIdentifierGet(pu8_BufferToParse, &u16_EntryIdentifier, &u16_EntryLength, &u16_Index);

    while ( u16_Index < u16_EntryLength )
    {
        pu8_FieldId = pu8_BufferToParse + u16_Index;

        pu32_Fields[(*pu32_FieldsNum)++] = CATiqFieldId_2_AppFieldId(g_pst_LASessions[u16_SessionIdx].u32_ListId, *pu8_FieldId);

        switch ( *pu8_FieldId )
        {

        case CMBS_DSL_FLD_DTAM_ID:
            {
                ST_UTIL_LA_FLD_DTAM_ID st_LA_Fld_DTAMId;
                cmbs_util_LA_Fld_DTAMIdGet( pu8_FieldId, &st_LA_Fld_DTAMId );
                pEntry->u32_DTAMIdentifier = st_LA_Fld_DTAMId.u8_Value & 0x3F;
                pEntry->b_DTAMLocation     = st_LA_Fld_DTAMId.u8_Value & 0x40;
                pEntry->b_DTAMType         = st_LA_Fld_DTAMId.u8_Attribute & 0x2;                
            }
            break;

        case FIELD_ID_DTAM_POSITION_INDEX:
            {
                ST_UTIL_LA_FLD_POSITION_IDX st_LA_Fld_DTAMPositionIdx;
                cmbs_util_LA_Fld_DTAMPositionIdxGet( pu8_FieldId, &st_LA_Fld_DTAMPositionIdx);

                pEntry->u32_PositionIdx = st_LA_Fld_DTAMPositionIdx.u8_Value;
            }
            break;

        case FIELD_ID_DTAM_RECORDED_MSG_NAME:
            {
                ST_UTIL_LA_FLD_NAME st_LA_Fld_RecorededMsgName;
                cmbs_util_LA_Fld_NameGet( pu8_FieldId, &st_LA_Fld_RecorededMsgName );
                
                memcpy(pEntry->sRecordedMsgName, st_LA_Fld_RecorededMsgName.pu8_Data, st_LA_Fld_RecorededMsgName.u16_DataLen);
                pEntry->sRecordedMsgName[st_LA_Fld_RecorededMsgName.u16_DataLen] = 0;
               }
            break;

        case FIELD_ID_DTAM_TIME_DURATION_HOURS:
            {
                ST_UTIL_LA_FLD_MSG_DURATION st_LA_Fld_MsgDuration;
                cmbs_util_LA_Fld_DTAMMsgDurationGet( pu8_FieldId, &st_LA_Fld_MsgDuration );

                pEntry->u32_TimeDurationHours = st_LA_Fld_MsgDuration.u8_DurationHours;
                pEntry->u32_TimeDurationMinutes = st_LA_Fld_MsgDuration.u8_DurationMinutes;
                pEntry->u32_TimeDurationSeconds = st_LA_Fld_MsgDuration.u8_DurationSeconds;
            }
            break;
                   
        default:
            break;
        }

        cmbs_util_la_ShiftIndexToNextFieldID(pu8_FieldId, &u16_Index);
    }
}

static void UseDefaultSort(IN u16 u16_SessionIdx, OUT PST_IE_LA_FIELDS pst_SortFields)
{
    u32 u32_ListId = g_pst_LASessions[u16_SessionIdx].u32_ListId;

    pst_SortFields->u16_Length = 1;

    switch ( u32_ListId )
    {
    case LIST_TYPE_CONTACT_LIST:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_CL_FLD_NAME;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_LAST_NAME;
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField2 = pst_SortFields->pu16_FieldId[1] = CMBS_CL_FLD_FIRST_NAME;
        g_pst_LASessions[u16_SessionIdx].u32_SortField2 = FIELD_ID_FIRST_NAME;
        pst_SortFields->u16_Length = 2;
        break;

    case LIST_TYPE_LINE_SETTINGS_LIST:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_LSL_FLD_LINE_ID;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_LINE_ID;
        break;

    case LIST_TYPE_MISSED_CALLS:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_MCL_FLD_DATETIME;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_DATE_AND_TIME;
        break;

    case LIST_TYPE_OUTGOING_CALLS:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_OCL_FLD_DATETIME;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_DATE_AND_TIME;
        break;
    case LIST_TYPE_INCOMING_ACCEPTED_CALLS:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_IACL_FLD_DATETIME;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_DATE_AND_TIME;
        break;
    case LIST_TYPE_ALL_CALLS:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_ACL_FLD_DATETIME;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_DATE_AND_TIME;
        break;
    case LIST_TYPE_ALL_INCOMING_CALLS:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_AICL_FLD_DATETIME;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_DATE_AND_TIME;
        break;

    case LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS:
        g_pst_LASessions[u16_SessionIdx].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0] = CMBS_LADSL_FLD_LINE_ID;
        g_pst_LASessions[u16_SessionIdx].u32_SortField1 = FIELD_ID_LINE_ID;
        break;

    default:
        break;
    }
}

static void SortArray(u16 u16_Array[], u16 u16_Length)
{
    s16 u16_i, u16_j, u16_temp;

    for ( u16_i = u16_Length - 1; u16_i >= 0; --u16_i )
    {
        for ( u16_j = 1; u16_j <= u16_i; ++u16_j )
        {
            if ( u16_Array[u16_j - 1] > u16_Array[u16_j] )
            {
                u16_temp = u16_Array[u16_j - 1];
                u16_Array[u16_j - 1] = u16_Array[u16_j];
                u16_Array[u16_j] = u16_temp;
            }
        }
    }
}

static void InitSession(u32 u32_SessionIdx)
{
    memset(&g_pst_LASessions[u32_SessionIdx], 0, sizeof(g_pst_LASessions[u32_SessionIdx]));
    g_pst_LASessions[u32_SessionIdx].u32_SessionId = LA_SESSION_UNDEFINED_SESSION_ID;
    g_pst_LASessions[u32_SessionIdx].u32_CATiqSortField1 = CMBS_FLD_UNDEFINED;
    g_pst_LASessions[u32_SessionIdx].u32_CATiqSortField2 = CMBS_FLD_UNDEFINED;
    g_pst_LASessions[u32_SessionIdx].u32_SortField1 = FIELD_ID_INVALID;
    g_pst_LASessions[u32_SessionIdx].u32_SortField2 = FIELD_ID_INVALID;
    g_pst_LASessions[u32_SessionIdx].u16_DataBytesMarker = 0;
    g_pst_LASessions[u32_SessionIdx].u16_DataTotalLen = 0;
    g_pst_LASessions[u32_SessionIdx].u32_SaveEntryId = LA_INVALID_ENTRY_ID;
    g_pst_LASessions[u32_SessionIdx].u32_WriteEntryId = LA_INVALID_ENTRY_ID;
    g_pst_LASessions[u32_SessionIdx].st_LineDiagNotifAux.u32_LineId = LIST_LINE_ID_NONE;
}

static void StoreNumOfUnreadMissedCalls(IN u32 u32_SessionIdx, IN bool bBefore)
{
    u32 u32_NumOfLines, u32_Index, u32_NumOfMissedCalls;

    List_GetCount(LIST_TYPE_LINE_SETTINGS_LIST, &u32_NumOfLines);
    List_GetCount(LIST_TYPE_MISSED_CALLS, &u32_NumOfMissedCalls);

    if ( u32_NumOfLines > APPCALL_LINEOBJ_MAX )
    {
        APP_LA_ERROR_PRINT("ERROR: Too many lines!\n");
        return;
    }

    if ( bBefore )
    {
        /* Prepare list of Line IDs */
        for ( u32_Index = 1; u32_Index <= u32_NumOfLines; ++u32_Index )
        {
            u32 u32_FieldId = FIELD_ID_LINE_ID, u32_EntrySize = 1;
            stLineSettingsListEntry st_Entry;

            List_ReadEntries(LIST_TYPE_LINE_SETTINGS_LIST, &u32_Index, TRUE, MARK_LEAVE_UNCHANGED, &u32_FieldId, 1,
                             FIELD_ID_LINE_ID, FIELD_ID_INVALID, &st_Entry, &u32_EntrySize);

            g_pst_LASessions[u32_SessionIdx].pst_MissedCallNotifAux[u32_Index - 1].u32_LineId = st_Entry.u32_LineId;
            g_pst_LASessions[u32_SessionIdx].pst_MissedCallNotifAux[u32_Index - 1].u32_NumOfCallsAfter = 0;
            g_pst_LASessions[u32_SessionIdx].pst_MissedCallNotifAux[u32_Index - 1].u32_NumOfCallsBefore = 0;
        }
    }

    /* Run on Missed call list and count unread messages per line */
    for ( u32_Index = 1; u32_Index <= u32_NumOfMissedCalls; ++u32_Index )
    {
        u32 pu32_FieldsIds[2] = { FIELD_ID_LINE_ID, FIELD_ID_READ_STATUS};
        u32 u32_EntrySize = 1, u32_LineIdx;
        stCallsListEntry st_Entry;

        List_ReadEntries(LIST_TYPE_MISSED_CALLS, &u32_Index, TRUE, MARK_LEAVE_UNCHANGED,  pu32_FieldsIds, 2,
                         FIELD_ID_LINE_ID, FIELD_ID_INVALID, &st_Entry, &u32_EntrySize);

        if ( st_Entry.bRead == FALSE )
        {
            /* Increase ammount of unread entries for the line */
            for ( u32_LineIdx = 0; u32_LineIdx < u32_NumOfLines; ++u32_LineIdx )
            {
                if ( g_pst_LASessions[u32_SessionIdx].pst_MissedCallNotifAux[u32_LineIdx].u32_LineId == st_Entry.u32_LineId )
                {
                    if ( bBefore )
                    {
                        ++(g_pst_LASessions[u32_SessionIdx].pst_MissedCallNotifAux[u32_LineIdx].u32_NumOfCallsBefore);
                    }
                    else
                    {
                        ++(g_pst_LASessions[u32_SessionIdx].pst_MissedCallNotifAux[u32_LineIdx].u32_NumOfCallsAfter);
                    }
                }
            }
        }
    }
}

static void StoreNumOfUnreadSMS(IN u32 u32_SessionIdx, IN bool bBefore)
{
    u32 u32_NumOfSMSId, u32_Index, u32_NumOfIncomingSMS;

    List_GetCount(LIST_TYPE_SMS_SETTINGS_LIST, &u32_NumOfSMSId);
    List_GetCount(LIST_TYPE_INCOMING_SMS, &u32_NumOfIncomingSMS);

    if ( u32_NumOfSMSId > APPCALL_LINEOBJ_MAX )
    {
        APP_LA_ERROR_PRINT("ERROR: Too many SMS Ids!\n");
        return;
    }

    if ( bBefore )
    {
        // Get number of SMS Id's
        for ( u32_Index = 1; u32_Index <= u32_NumOfSMSId; ++u32_Index )
        {
            u32 u32_FieldId = FIELD_ID_LINE_ID, u32_EntrySize = 1;
            stSMSSettingsListEntry st_Entry;

            List_ReadEntries(LIST_TYPE_SMS_SETTINGS_LIST, &u32_Index, TRUE, MARK_LEAVE_UNCHANGED, &u32_FieldId, 1,
                             FIELD_ID_SMS_IDENTIFIER, FIELD_ID_INVALID, &st_Entry, &u32_EntrySize);

            g_pst_LASessions[u32_SessionIdx].pst_SMSNotifAux[u32_Index - 1].u32_SMSId = st_Entry.u32_SMSServiseId;
            g_pst_LASessions[u32_SessionIdx].pst_SMSNotifAux[u32_Index - 1].u32_NumOfSMSAfter = 0;
            g_pst_LASessions[u32_SessionIdx].pst_SMSNotifAux[u32_Index - 1].u32_NumOfSMSBefore = 0;
        }
    }

    /* Run on Incoming SMS list and count unread messages per line */
    for ( u32_Index = 1; u32_Index <= u32_NumOfIncomingSMS; ++u32_Index )
    {
        u32 pu32_FieldsIds[2] = { FIELD_ID_SMS_IDENTIFIER, FIELD_ID_READ_STATUS};
        u32 u32_EntrySize = 1, u32_SMSIdx;
        stSMSListEntry st_Entry;

        List_ReadEntries(LIST_TYPE_INCOMING_SMS, &u32_Index, TRUE, MARK_LEAVE_UNCHANGED,  pu32_FieldsIds, 2,
                         FIELD_ID_SMS_IDENTIFIER, FIELD_ID_INVALID, &st_Entry, &u32_EntrySize);

        if ( st_Entry.b_Read == FALSE )
        {
            /* Increase ammount of unread entries for the line */
            for ( u32_SMSIdx = 0; u32_SMSIdx < u32_NumOfIncomingSMS; ++u32_SMSIdx )
            {
                if ( g_pst_LASessions[u32_SessionIdx].pst_SMSNotifAux[u32_SMSIdx].u32_SMSId == st_Entry.u32_SMSServiseId)
                {
                    if ( bBefore )
                    {
                        ++(g_pst_LASessions[u32_SessionIdx].pst_SMSNotifAux[u32_SMSIdx].u32_NumOfSMSBefore);
                    }
                    else
                    {
                        ++(g_pst_LASessions[u32_SessionIdx].pst_SMSNotifAux[u32_SMSIdx].u32_NumOfSMSAfter);
                    }
                }
            }
        }
    }
}

static void SendLineSettingsListChangeNotification(IN u32 u32_LineID)
{
    u32 u32_HandsetMask;

    if(List_GetAttachedHs(u32_LineID, &u32_HandsetMask) == LIST_RC_OK)
    	ListChangeNotif_ListChanged( u32_LineID, LINE_TYPE_RELATING_TO, u32_HandsetMask, 1, LIST_CHANGE_NOTIF_LIST_TYPE_LINE_SETTINGS_LIST );
        
}

static u32 LineSettingsGetLineIdByEntryId(IN u32 u32_EntryId)
{
    u32 u32_FieldId = FIELD_ID_LINE_ID;
    stLineSettingsListEntry st_Entry;
    List_ReadEntryById(LIST_TYPE_LINE_SETTINGS_LIST, u32_EntryId, &u32_FieldId, 1, &st_Entry);

    return st_Entry.u32_LineId;
}

/*
* HELPER:
* Retrieve entriID from LineSettingsList
*/
u32 LineSettingsGetEntryIdByLineId(int lineID)
{
	u32 pu32_Fields[1], u32_FieldsNum = 1, pu32_StartIdx = 0;
	u32 u32_NumOfReqEntries = 1;
	u8 pu8_Entries[LIST_ENTRY_MAX_SIZE];
	char s_lineID[5] = {0};

	sprintf(s_lineID, "%d", lineID);
	pu32_Fields[0]	= FIELD_ID_ENTRY_ID;
	// search for this entry
	List_SearchEntries(LIST_TYPE_LINE_SETTINGS_LIST, MATCH_EXACT, TRUE, s_lineID, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsNum, 
		FIELD_ID_LINE_ID, FIELD_ID_ENTRY_ID, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);

	if(u32_NumOfReqEntries)
		return ((stContactListEntry*)pu8_Entries)->u32_EntryId;

	return 0;
}

static u32 MissedCallGetLineIdByEntryId(IN u32 u32_EntryId)
{
    u32 u32_FieldId = FIELD_ID_LINE_ID;
    stCallsListEntry st_Entry;
    List_ReadEntryById(LIST_TYPE_MISSED_CALLS, u32_EntryId, &u32_FieldId, 1, &st_Entry);

    return st_Entry.u32_LineId;
}

u32 GetSMSIdByEntryId(IN u32 u32_EntryId, IN LIST_TYPE ListType)
{
    u32 u32_FieldId = FIELD_ID_SMS_IDENTIFIER;
    stSMSListEntry st_Entry;
    List_ReadEntryById(ListType, u32_EntryId, &u32_FieldId, 1, &st_Entry);

    return st_Entry.u32_SMSServiseId;
}

E_CMBS_RC GetLineIdBySMSId(IN u32 u32_SMSId, OUT u32 *pu32_LineId)
{
    char s_SMSID[5] = { 0 };
    u32 pu32_Fields[1], pu32_StartIdx = 0, u32_NumOfReqEntries = 1;
    u8 pu8_Entries[LIST_ENTRY_MAX_SIZE];

    sprintf(s_SMSID, "%d", u32_SMSId);
    pu32_Fields[0] = FIELD_ID_LINE_ID;

    List_SearchEntries(LIST_TYPE_SMS_SETTINGS_LIST, MATCH_EXACT, TRUE, s_SMSID, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, 1,
                       FIELD_ID_SMS_IDENTIFIER, FIELD_ID_LINE_ID, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);

    if ( u32_NumOfReqEntries )
    {
        *pu32_LineId = ((stSMSSettingsListEntry *)pu8_Entries)->u32_LineId;
        return CMBS_RC_OK;
    }

    return CMBS_RC_ERROR_GENERAL;
}


E_CMBS_RC GetSMSIdByLineID(IN u32 u32_LineId,  OUT u32 *pu32_SMSId)
{
    char s_LineID[5] = {0};
    u32 pu32_Fields[1], pu32_StartIdx = 0, u32_NumOfReqEntries=1;
    u8 pu8_Entries[LIST_ENTRY_MAX_SIZE];
    
    sprintf(s_LineID, "%d", u32_LineId);
    pu32_Fields[0] = FIELD_ID_SMS_IDENTIFIER;
    

    List_SearchEntries(LIST_TYPE_SMS_SETTINGS_LIST, MATCH_EXACT, TRUE, s_LineID, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, 1, 
	FIELD_ID_LINE_ID, FIELD_ID_SMS_IDENTIFIER, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);

    if ( u32_NumOfReqEntries )
    {
        *pu32_SMSId = ((stSMSSettingsListEntry *)pu8_Entries)->u32_SMSServiseId;
        return CMBS_RC_OK;
    }

    return CMBS_RC_ERROR_GENERAL;
}

E_CMBS_RC GetContactNameByNumber(IN char * psz_Number,  OUT char * psz_Name)
{
	// find name by caller number
	u32 pu32_Fields[2], u32_FieldsNum = 0, pu32_StartIdx = 0;
	u32 u32_NumOfReqEntries = 1;
	u8 pu8_Entries[LIST_ENTRY_MAX_SIZE];

	pu32_Fields[u32_FieldsNum++] = FIELD_ID_ENTRY_ID;
	pu32_Fields[u32_FieldsNum++] = FIELD_ID_LAST_NAME;


	// search for this entry in the contacts list
	List_SearchEntries(LIST_TYPE_CONTACT_LIST, MATCH_EXACT, TRUE, psz_Number, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsNum, 
		FIELD_ID_CONTACT_NUM_1, FIELD_ID_INVALID, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);

	if(u32_NumOfReqEntries)
	{
		strcpy(psz_Name, ((stContactListEntry*)pu8_Entries)->sLastName);
	}
	else
	{		
		// search for this entry in the contacts list
		List_SearchEntries(LIST_TYPE_CONTACT_LIST, MATCH_EXACT, TRUE, psz_Number, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsNum, 
			FIELD_ID_CONTACT_NUM_2, FIELD_ID_INVALID, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);
		
		if(u32_NumOfReqEntries)
		{
			strcpy(psz_Name, ((stContactListEntry*)pu8_Entries)->sLastName);
		}
		else
		{		
			// search for this entry in the contacts list
			List_SearchEntries(LIST_TYPE_CONTACT_LIST, MATCH_EXACT, TRUE, psz_Number, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsNum, 
				FIELD_ID_CONTACT_NUM_3, FIELD_ID_INVALID, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);
			
			if(u32_NumOfReqEntries)
			{
				strcpy(psz_Name, ((stContactListEntry*)pu8_Entries)->sLastName);
			}
		}
	}

	return (u32_NumOfReqEntries) ? CMBS_RC_OK : CMBS_RC_ERROR_GENERAL;
}

u32 LineDiagnosticGetEntryIdByLineId(int lineID)
{
	u32 pu32_Fields[1], u32_FieldsNum = 1, pu32_StartIdx = 0;
	u32 u32_NumOfReqEntries = 1;
	u8 pu8_Entries[LIST_ENTRY_MAX_SIZE];
	char s_lineID[5] = {0};

	sprintf(s_lineID, "%d", lineID);
	pu32_Fields[0]	= FIELD_ID_ENTRY_ID;
	// search for this entry
	List_SearchEntries(LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS, MATCH_EXACT, TRUE, s_lineID, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsNum, 
		FIELD_ID_LINE_ID, FIELD_ID_ENTRY_ID, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);

	if(u32_NumOfReqEntries)
		return ((stLineAndDiagnosticListEntry*)pu8_Entries)->u32_EntryId;
      else
	return 0xFF;
}
/* ***************** Auxiliary end ***************** */


/*******************************************
Session Mgr API
********************************************/
LA_SESSION_MGR_RC LASessionMgr_Init(void)
{
    LIST_RC list_rc;
    u32 u32_Index;
    u32 u32Count;

    /* Init lists */
    list_rc = List_Init();
    if ( list_rc != LIST_RC_OK )
    {
        return LA_SESSION_MGR_RC_FAIL;
    }

    /* create lists */
    for ( u32_Index = LIST_TYPE_CONTACT_LIST; u32_Index < LIST_TYPE_MAX; ++u32_Index )
    {
        list_rc = List_CreateList(u32_Index);
        if ( list_rc == LIST_RC_UNSUPPORTED_LIST )
        {
            APP_LA_ERROR_PRINT("Error: Unsupported list LA_SESSION_MGR_RC_UNSUPPORTED_LIST\n");
            continue;
        }
        if(u32_Index == LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS)
        {   
            if (LineDiagnosticGetEntryIdByLineId(CMBS_NO_LINE_DEFINED) == CMBS_NO_ENTRY_FOUND)
            List_AddLineAndDiagnosticSystemEntry();
            List_ClearLineAndDiagnosticSystemEntry();
        }
    }

    /* Init sessions */
    for ( u32_Index = 0; u32_Index < LA_SESSION_MGR_MAX_SESSIONS; ++u32_Index )
    {
        InitSession(u32_Index);
    }

    List_GetCount(LIST_TYPE_LINE_SETTINGS_LIST, &u32Count);
    if (u32Count == 0)
    {
		u32 pu32_Fields[13], u32_FieldsNum = 13, u32_EntryId;
		stLineSettingsListEntry  st_LineSettingsListEntry;		
        ST_IE_LINE_SETTINGS_LIST st_LineSettingsList;

        memset(&st_LineSettingsListEntry, 0, sizeof(st_LineSettingsListEntry));
        memset(&st_LineSettingsList, 0, sizeof(st_LineSettingsList));

		st_LineSettingsListEntry.u32_LineId = 0;
		st_LineSettingsListEntry.u32_AttachedHsMask = CMBS_U16_SUBSCRIBED_HS_MASK; //ALL HS
		
        printf("Inserting a default entry to Line Settings List\n");

        pu32_Fields[0] = FIELD_ID_LINE_NAME;
        pu32_Fields[1] = FIELD_ID_LINE_ID;
        pu32_Fields[2] = FIELD_ID_ATTACHED_HANDSETS;
        pu32_Fields[3] = FIELD_ID_DIALING_PREFIX;
        pu32_Fields[4] = FIELD_ID_FP_VOLUME;
        pu32_Fields[5] = FIELD_ID_FP_MELODY;
        pu32_Fields[6] = FIELD_ID_BLOCKED_NUMBER;
        pu32_Fields[7] = FIELD_ID_MULTIPLE_CALLS_MODE;
        pu32_Fields[8] = FIELD_ID_INTRUSION_CALL;
        pu32_Fields[9] = FIELD_ID_PERMANENT_CLIR;
        pu32_Fields[10] = FIELD_ID_CALL_FWD_UNCOND;
        pu32_Fields[11] = FIELD_ID_CALL_FWD_NO_ANSWER;
        pu32_Fields[12] = FIELD_ID_CALL_FWD_BUSY;

		List_InsertEntry(LIST_TYPE_LINE_SETTINGS_LIST, &st_LineSettingsListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

        /* Need to update target with Line Settings */
		st_LineSettingsList.u16_Attached_HS     = (u16)st_LineSettingsListEntry.u32_AttachedHsMask;
        st_LineSettingsList.u8_Call_Intrusion   = st_LineSettingsListEntry.bIntrusionCall;
        st_LineSettingsList.u8_Line_Id          = (u8)st_LineSettingsListEntry.u32_LineId;
        st_LineSettingsList.u8_Multiple_Calls   = st_LineSettingsListEntry.bMultiCalls ? CMBS_MULTICALL_LINE_NUM_OF_CALLS : 0;
        st_LineSettingsList.e_LineType          = st_LineSettingsListEntry.bMultiCalls ? CMBS_LINE_TYPE_VOIP_PARALLEL_CALL : CMBS_LINE_TYPE_VOIP_DOUBLE_CALL;
        app_SrvLineSettingsSet(&st_LineSettingsList, 1);
        printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
		
    }    

    List_GetCount(LIST_TYPE_DTAM_SETTINGS_LIST, &u32Count);
    if (u32Count == 0)
    {
		u32 pu32_Fields[13], u32_FieldsNum = 0, u32_EntryId;
		stDTAMSettingsListEntry  st_DtamSettingsListEntry;		

        memset(&st_DtamSettingsListEntry, 0, sizeof(st_DtamSettingsListEntry));

		st_DtamSettingsListEntry.u32_LineId = 0;
		st_DtamSettingsListEntry.u32_DTAMIdentifier = 0;
		st_DtamSettingsListEntry.b_DTAMType = 1;
		st_DtamSettingsListEntry.b_DTAMLocation = 0;
		st_DtamSettingsListEntry.b_DTAMActivation = 1;
		st_DtamSettingsListEntry.b_DTAMDefaultTimeout = 10;
		st_DtamSettingsListEntry.u32_DTAMTimeout = 0;
		st_DtamSettingsListEntry.u32_WelcomeMsgParams = 1;
		
        printf("Inserting a default entry to DTAM Settings List\n");

        pu32_Fields[u32_FieldsNum++] = FIELD_ID_LINE_ID;
        pu32_Fields[u32_FieldsNum++] = FIELD_ID_DTAM_IDENTIFIER;
        pu32_Fields[u32_FieldsNum++] = FIELD_ID_DTAM_ACTIVATION;
        pu32_Fields[u32_FieldsNum++] = FIELD_ID_DTAM_WELCOME_MSG_PARAMETERS;

		List_InsertEntry(LIST_TYPE_DTAM_SETTINGS_LIST, &st_DtamSettingsListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

        printf("Entry inserted. Entry Id = %d\n", u32_EntryId);
		
    }    

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_GetDataBuffer(IN u16 u16_SessionId, OUT u8** ppu8_DataBuffer, OUT u16** ppu16_Marker, OUT u16** ppu16_Len)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    *ppu8_DataBuffer = g_pst_LASessions[s32_Index].pu8_DataBuffer;
    *ppu16_Marker    = &(g_pst_LASessions[s32_Index].u16_DataBytesMarker);
    *ppu16_Len       = &(g_pst_LASessions[s32_Index].u16_DataTotalLen);

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_StartSession(IN u16 u16_SessionId, IN u16 u16_ListId, IN u16 u16_HsId, IN PST_IE_LA_FIELDS pst_SortFields,
                                            OUT u16* pu16_CurrNumOfEntries)
{
    u32 u32_NumEntries;

    /* find free session object */
    s32 s32_Index = GetIndexBySessionId(LA_SESSION_UNDEFINED_SESSION_ID);
    if ( s32_Index == -1 )
    {
        APP_LA_ERROR_PRINT("LASessionMgr_StartSession: no more free sessions!\n");
        return LA_SESSION_MGR_RC_NO_FREE_SESSIONS;
    }
	
   if (CATiqListId_2_AppListId(u16_ListId) >= LIST_TYPE_MAX)
    {
	 APP_LA_ERROR_PRINT("LASessionMgr_StartSession: Invalid List ID!\n");
	 return LA_SESSION_MGR_RC_UNSUPPORTED_LIST;
    }	
   
    /* Fill in session parameters */
    g_pst_LASessions[s32_Index].u32_SessionId = u16_SessionId;
    g_pst_LASessions[s32_Index].u32_CATiqListId = u16_ListId;
    g_pst_LASessions[s32_Index].u32_ListId = CATiqListId_2_AppListId(u16_ListId);
    g_pst_LASessions[s32_Index].u32_HsId = u16_HsId;

    if ( pst_SortFields->u16_Length > 0 )
    {
        g_pst_LASessions[s32_Index].u32_CATiqSortField1 = pst_SortFields->pu16_FieldId[0];
        g_pst_LASessions[s32_Index].u32_SortField1 = CATiqFieldId_2_AppFieldId(g_pst_LASessions[s32_Index].u32_ListId, pst_SortFields->pu16_FieldId[0]);
    }
    if ( pst_SortFields->u16_Length > 1 )
    {
        g_pst_LASessions[s32_Index].u32_CATiqSortField2 = pst_SortFields->pu16_FieldId[1];
        g_pst_LASessions[s32_Index].u32_SortField2 = CATiqFieldId_2_AppFieldId(g_pst_LASessions[s32_Index].u32_ListId, pst_SortFields->pu16_FieldId[1]);
    }
    if ( pst_SortFields->u16_Length == 0 )
    {
        UseDefaultSort(s32_Index, pst_SortFields);
    }

    List_GetCount(g_pst_LASessions[s32_Index].u32_ListId, &u32_NumEntries);
    *pu16_CurrNumOfEntries = (u16)u32_NumEntries;

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_EndSession(IN u16 u16_SessionId)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    InitSession(s32_Index);

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_GetSupportedFields(IN u16 u16_SessionId,
                                                  OUT PST_IE_LA_FIELDS pst_EditableFields,
                                                  OUT PST_IE_LA_FIELDS pst_NonEditableFields)
{
    u32 pu32_EditableFields[LA_SESSION_MGR_MAX_FIELDS], pu32_NonEditableFields[LA_SESSION_MGR_MAX_FIELDS],
    u32_EditableNum = LA_SESSION_MGR_MAX_FIELDS, u32_NonEditableNum = LA_SESSION_MGR_MAX_FIELDS;

    u32 u32_i, u32_ListId;

    s32 s32_FieldId;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    u32_ListId = g_pst_LASessions[s32_Index].u32_ListId;

    List_GetSupportedFields(u32_ListId, pu32_EditableFields, &u32_EditableNum, pu32_NonEditableFields, &u32_NonEditableNum);

    pst_EditableFields->u16_Length = pst_NonEditableFields->u16_Length = 0;
    for ( u32_i = 0; u32_i < u32_EditableNum; ++u32_i )
    {
        if ( (s32_FieldId = AppFieldId_2_CATiqFieldId(u32_ListId, pu32_EditableFields[u32_i])) != CMBS_FLD_UNDEFINED )
        {
            pst_EditableFields->pu16_FieldId[pst_EditableFields->u16_Length++] = s32_FieldId;
        }
    }
    for ( u32_i = 0; u32_i < u32_NonEditableNum; ++u32_i )
    {
        if ( (s32_FieldId = AppFieldId_2_CATiqFieldId(u32_ListId, pu32_NonEditableFields[u32_i])) != CMBS_FLD_UNDEFINED )
        {
            pst_NonEditableFields->pu16_FieldId[pst_NonEditableFields->u16_Length++] = s32_FieldId;
        }
    }

    SortArray(pst_EditableFields->pu16_FieldId,     pst_EditableFields->u16_Length);
    SortArray(pst_NonEditableFields->pu16_FieldId,  pst_NonEditableFields->u16_Length);

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_ReadEntries(IN u16 u16_SessionId, INOUT u16* pu16_StartIdx, IN bool bForward, IN E_CMBS_MARK_REQUEST eMark,
                                           IN PST_IE_LA_FIELDS pst_RequestedFields, INOUT u16* pu16_NumOfReqEntries,
                                           OUT u8 pu8_Data[], INOUT u16* pu16_DataLen)
{
    u32 u32_ListId, pu32_Fields[LA_SESSION_MGR_MAX_FIELDS], u32_FieldsNum, u32_NumReqEntries = *pu16_NumOfReqEntries, u32_Count, u32_StartIndex;
    u8 pu8_Entries[LA_SESSION_MGR_MAX_ENTRIES_PER_READ * LIST_ENTRY_MAX_SIZE];
    eMarkRequest eReadMark;
    LIST_RC list_rc;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    if ( *pu16_NumOfReqEntries > LA_SESSION_MGR_MAX_ENTRIES_PER_READ )
    {
        //return LA_SESSION_MGR_RC_NOT_ENOUGH_MEMORY;
        u32_NumReqEntries = LA_SESSION_MGR_MAX_ENTRIES_PER_READ;
    }

    u32_ListId = g_pst_LASessions[s32_Index].u32_ListId;

    List_GetCount(u32_ListId, &u32_Count);

    if ( u32_Count == 0 )
    {
        return LA_SESSION_MGR_RC_INVALID_START_INDEX;
    }

    if ( *pu16_StartIdx == 0 )
    {
        /* means last entry */
        *pu16_StartIdx = (u16)u32_Count;
    }
    else
    {
        if ( *pu16_StartIdx < 1 || *pu16_StartIdx > u32_Count )
        {
            return LA_SESSION_MGR_RC_INVALID_START_INDEX;
        }
    }

    PrepareFieldsForRead(u32_ListId, pst_RequestedFields, pu32_Fields, &u32_FieldsNum);

    switch ( eMark )
    {
    case CMBS_MARK_REQUEST_SET_READ_STATUS:     eReadMark = MARK_SET;               break;
    case CMBS_MARK_REQUEST_RESET_READ_STATUS:   eReadMark = MARK_CLEAR;             break;
    default:                                    eReadMark = MARK_LEAVE_UNCHANGED;   break;
    }

    /* Monitor read entries for Missed Call Notification */
    if ( (eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_MISSED_CALLS) )
    {
        StoreNumOfUnreadMissedCalls(s32_Index, TRUE);
    }

    if((eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_INCOMING_SMS))
    {
        StoreNumOfUnreadSMS(s32_Index, TRUE);
    }
    
    u32_StartIndex = *pu16_StartIdx;

	List_SetListChangeOriginatingHandset (u32_ListId, g_pst_LASessions[s32_Index].u32_HsId);

    list_rc = List_ReadEntries(u32_ListId, &u32_StartIndex, bForward, eReadMark, pu32_Fields, u32_FieldsNum,
                               g_pst_LASessions[s32_Index].u32_SortField1, g_pst_LASessions[s32_Index].u32_SortField2,
                               pu8_Entries, &u32_NumReqEntries);

    *pu16_StartIdx = (u16)u32_StartIndex;

    if ( (eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_MISSED_CALLS) )
    {
        StoreNumOfUnreadMissedCalls(s32_Index, FALSE);
    }
    
    if((eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_INCOMING_SMS))
    {
        StoreNumOfUnreadSMS(s32_Index, FALSE);
    }

    if ( list_rc == LIST_RC_FAIL )
    {
        return LA_SESSION_MGR_RC_FAIL;
    }

    ConvertEntriesToCatiqDataBuffer(u32_ListId, pst_RequestedFields, pu8_Entries, u32_NumReqEntries, pu8_Data, pu16_DataLen);

	if ((u32_StartIndex + u32_NumReqEntries - 1) < u32_Count)
	{
		if (u32_NumReqEntries < *pu16_NumOfReqEntries)
		{
			u32_NumReqEntries |= (1 << 7);
		}
	}

    *pu16_NumOfReqEntries = (u16)u32_NumReqEntries;

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_SearchEntries(IN u16 u16_SessionId, IN E_CMBS_LA_SEARCH_MATCHING eMatch, IN bool bCaseSensitive,
                                             IN const char* s_SearchedValue, IN bool bForward, IN E_CMBS_MARK_REQUEST eMark,
                                             IN PST_IE_LA_FIELDS pst_RequestedFields, INOUT u16* pu16_NumOfReqEntries,
                                             OUT u8 pu8_Data[], INOUT u16* pu16_DataLen, OUT u32* pu32_StartIdx)
{
    u32 u32_ListId, pu32_Fields[LA_SESSION_MGR_MAX_FIELDS], u32_FieldsNum, u32_NumOfReqEntries = *pu16_NumOfReqEntries;
    u8 pu8_Entries[LA_SESSION_MGR_MAX_ENTRIES_PER_READ * LIST_ENTRY_MAX_SIZE];
    eMarkRequest eReadMark;
    eMatchOption eSearchMatch;
    LIST_RC list_rc;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    if ( *pu16_NumOfReqEntries > LA_SESSION_MGR_MAX_ENTRIES_PER_READ )
    {
        return LA_SESSION_MGR_RC_NOT_ENOUGH_MEMORY;
    }

    u32_ListId = g_pst_LASessions[s32_Index].u32_ListId;

    PrepareFieldsForRead(u32_ListId, pst_RequestedFields, pu32_Fields, &u32_FieldsNum);

    switch ( eMark )
    {
    case CMBS_MARK_REQUEST_SET_READ_STATUS:     eReadMark = MARK_SET;               break;
    case CMBS_MARK_REQUEST_RESET_READ_STATUS:   eReadMark = MARK_CLEAR;             break;
    default:                                    eReadMark = MARK_LEAVE_UNCHANGED;   break;
    }

    switch ( eMatch )
    {
    case CMBS_LA_SEARCH_MATCHING_EXACT_NEXT:    eSearchMatch = MATCH_RETURN_NEXT_ON_FAIL;   break;
    case CMBS_LA_SEARCH_MATCHING_EXACT_PREV:    eSearchMatch = MATCH_RETURN_PREV_ON_FAIL;   break;
    default:                                    eSearchMatch = MATCH_EXACT;                 break;
    }

    /* Monitor read entries for Missed Call Notification */
    if ( (eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_MISSED_CALLS) )
    {
        StoreNumOfUnreadMissedCalls(s32_Index, TRUE);
    }
    
    if((eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_INCOMING_SMS))
    {
        StoreNumOfUnreadSMS(s32_Index, TRUE);
    }

	List_SetListChangeOriginatingHandset (u32_ListId, g_pst_LASessions[s32_Index].u32_HsId);

    list_rc = List_SearchEntries(u32_ListId, eSearchMatch, bCaseSensitive, s_SearchedValue, bForward, eReadMark, pu32_Fields, u32_FieldsNum, 
                                 g_pst_LASessions[s32_Index].u32_SortField1, g_pst_LASessions[s32_Index].u32_SortField2,
                                 pu8_Entries, &u32_NumOfReqEntries, pu32_StartIdx);

    if ( (eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_MISSED_CALLS) )
    {
        StoreNumOfUnreadMissedCalls(s32_Index, FALSE);
    }
    
    if((eReadMark != MARK_LEAVE_UNCHANGED) && (u32_ListId == LIST_TYPE_INCOMING_SMS))
    {
        StoreNumOfUnreadSMS(s32_Index, FALSE);
    }

    if ( list_rc == LIST_RC_FAIL )
    {
        return LA_SESSION_MGR_RC_FAIL;
    }

    *pu16_NumOfReqEntries = u32_NumOfReqEntries;

    ConvertEntriesToCatiqDataBuffer(u32_ListId, pst_RequestedFields, pu8_Entries, *pu16_NumOfReqEntries, pu8_Data, pu16_DataLen);

    return LA_SESSION_MGR_RC_OK;
}

u32 LASessionMgr_GetWriteEntryID(IN u16 u16_SessionId)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    return g_pst_LASessions[s32_Index].u32_WriteEntryId;
}

LA_SESSION_MGR_RC LASessionMgr_SetWriteEntryID(IN u16 u16_SessionId, IN u32 u32_EntryId)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    g_pst_LASessions[s32_Index].u32_WriteEntryId = u32_EntryId;

    g_pst_LASessions[s32_Index].u16_DataBytesMarker = g_pst_LASessions[s32_Index].u16_DataTotalLen = 0;

    return LA_SESSION_MGR_RC_OK;
}

u32 LASessionMgr_GetSaveEntryID(IN u16 u16_SessionId)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    return g_pst_LASessions[s32_Index].u32_SaveEntryId;
}

LA_SESSION_MGR_RC LASessionMgr_SetSaveEntryID(IN u16 u16_SessionId, IN u32 u32_EntryId)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    g_pst_LASessions[s32_Index].u32_SaveEntryId = u32_EntryId;

    g_pst_LASessions[s32_Index].u16_DataBytesMarker = g_pst_LASessions[s32_Index].u16_DataTotalLen = 0;

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_SaveEntry(IN u16 u16_SessionId, OUT u32* pu32_EntryId, OUT u32* pu32_PositionIdx, OUT u32* pu32_TotalNumEntries)
{
    u8 pu8_Entry[LIST_ENTRY_MAX_SIZE]={0};
    u32 u32_listId, pu32_Fields[LA_SESSION_MGR_MAX_FIELDS]={0}, u32_FieldsNum = 0, u32_index;
    LIST_RC list_rc;
	ST_IE_LINE_SETTINGS_LIST stLineSettingsPrev;
	ST_APP_FACILITY_LCN_SUB_DETAILS st_LcnSubDetails;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    u32_listId = g_pst_LASessions[s32_Index].u32_ListId;

	if (g_pst_LASessions[s32_Index].u32_SaveEntryId != 0xFF)
	{
	    *pu32_EntryId = g_pst_LASessions[s32_Index].u32_SaveEntryId;
	}
	else if (g_pst_LASessions[s32_Index].u32_WriteEntryId != 0xFF)
	{
	    *pu32_EntryId = g_pst_LASessions[s32_Index].u32_WriteEntryId;
	}
	else
	{
		return LA_SESSION_MGR_RC_NOT_ALLOWED;
	}

    if ((u32_listId == LIST_TYPE_LINE_SETTINGS_LIST) && ( *pu32_EntryId != 0 ))
    {
        u32 pu32_FieldIDs[4];
        stLineSettingsListEntry st_Entry;

        pu32_FieldIDs[0] = FIELD_ID_LINE_ID;
        pu32_FieldIDs[1] = FIELD_ID_MULTIPLE_CALLS_MODE;
        pu32_FieldIDs[2] = FIELD_ID_ATTACHED_HANDSETS;
        pu32_FieldIDs[3] = FIELD_ID_INTRUSION_CALL;

        List_ReadEntryById(LIST_TYPE_LINE_SETTINGS_LIST, *pu32_EntryId, pu32_FieldIDs, 4, &st_Entry);

        stLineSettingsPrev.u8_Line_Id           = (u8)st_Entry.u32_LineId;
        stLineSettingsPrev.u8_Multiple_Calls    = st_Entry.bMultiCalls ? CMBS_MULTICALL_LINE_NUM_OF_CALLS : 0;
        stLineSettingsPrev.u16_Attached_HS      = (u16)st_Entry.u32_AttachedHsMask;
        stLineSettingsPrev.u8_Call_Intrusion    = st_Entry.bIntrusionCall;
        stLineSettingsPrev.e_LineType           = st_Entry.bMultiCalls ? CMBS_LINE_TYPE_VOIP_PARALLEL_CALL : CMBS_LINE_TYPE_VOIP_DOUBLE_CALL;

    }

    if (g_pst_LASessions[s32_Index].u16_DataTotalLen > 0)
    {
        switch ( u32_listId )
        {
        case LIST_TYPE_MISSED_CALLS:
        case LIST_TYPE_ALL_INCOMING_CALLS:
            ParseCallListEntry(s32_Index, pu8_Entry, pu32_Fields, &u32_FieldsNum);
            break;
    
        case LIST_TYPE_OUTGOING_CALLS:
        case LIST_TYPE_INCOMING_ACCEPTED_CALLS:
        case LIST_TYPE_ALL_CALLS:
        case LIST_TYPE_LINE_AND_DIAGNOSTIC_STATUS:
            /* No Writeable fields in this list */
            return LA_SESSION_MGR_RC_NOT_ALLOWED;
    
        case LIST_TYPE_LINE_SETTINGS_LIST:
            ParseLineSettingsEntry(s32_Index, pu8_Entry, pu32_Fields, &u32_FieldsNum);
            break;
    
        case LIST_TYPE_CONTACT_LIST:
            ParseContactListEntry(s32_Index, pu8_Entry, pu32_Fields, &u32_FieldsNum);
            break;

        case LIST_TYPE_SMS_SETTINGS_LIST:
            ParseSMSSettingsListEntry(s32_Index, pu8_Entry, pu32_Fields, &u32_FieldsNum);
            break;
            
        case LIST_TYPE_INCOMING_SMS:
        case LIST_TYPE_DRAFT_SMS:
        case LIST_TYPE_OUTGOING_SMS:
        case LIST_TYPE_SENT_SMS:
            ParseSMSListEntry(s32_Index, pu8_Entry, pu32_Fields, &u32_FieldsNum);
            break;
            
        case LIST_TYPE_DTAM_SETTINGS_LIST:
            ParseDTAMSettingsListEntry(s32_Index, pu8_Entry, pu32_Fields, &u32_FieldsNum);
            break;

        case LIST_TYPE_DTAM_WELCOME_MESSAGE:
            ParseDTAMWelcomeMsgListEntry(s32_Index, pu8_Entry, pu32_Fields, &u32_FieldsNum);
            break;
            
        default:
            break;
        }

        if ( *pu32_EntryId == 0 )
        {
            if ( u32_listId == LIST_TYPE_LINE_SETTINGS_LIST || u32_listId == LIST_TYPE_CONTACT_LIST )
            {
				List_SetListChangeOriginatingHandset (u32_listId, g_pst_LASessions[s32_Index].u32_HsId);
				st_LcnSubDetails.u16_Change = APP_FACILITY_CHANGE_ADD;

                /* New Entry */
                list_rc = List_InsertEntry(u32_listId, pu8_Entry, pu32_Fields, u32_FieldsNum, pu32_EntryId);
                if ( list_rc != LIST_RC_OK )
                {
                    return LA_SESSION_MGR_RC_FAIL;
                }
            }
            else
            {
                return LA_SESSION_MGR_RC_NOT_ALLOWED;
            }
        }
        else
        {
            /* Check Non-Editable fields */
            for ( u32_index = 0; u32_index < u32_FieldsNum; ++u32_index )
            {
                if ( AppFieldNonEditable(pu32_Fields[u32_index]) )
                {
                    return LA_SESSION_MGR_RC_NOT_ALLOWED;
                }
            }
    
			List_SetListChangeOriginatingHandset (u32_listId, g_pst_LASessions[s32_Index].u32_HsId);
			st_LcnSubDetails.u16_Change = APP_FACILITY_CHANGE_EDIT;

            list_rc = List_UpdateEntry(u32_listId, pu8_Entry, pu32_Fields, u32_FieldsNum, *pu32_EntryId);

            if ( list_rc != LIST_RC_OK )
            {
                return LA_SESSION_MGR_RC_ENTRY_NOT_AVAILABLE;
            }
        }
    }

    /* Position Index */
    List_GetEntryIdex(u32_listId, g_pst_LASessions[s32_Index].u32_SortField1, g_pst_LASessions[s32_Index].u32_SortField2,
                      *pu32_EntryId, pu32_PositionIdx);

	st_LcnSubDetails.u16_EntryId = *pu32_EntryId;
	st_LcnSubDetails.u16_PosIndex = *pu32_PositionIdx;
	
	List_AddListChangeDetails (u32_listId, &st_LcnSubDetails);

    /* Entry Num */
    List_GetCount(u32_listId, pu32_TotalNumEntries);

    /* For Line Settings List, need to update CMBS target */
    if (u32_listId == LIST_TYPE_LINE_SETTINGS_LIST)
    {
        u32 pu32_FieldIDs[4];
        stLineSettingsListEntry st_Entry;
        ST_IE_LINE_SETTINGS_LIST stLineSettings;

        pu32_FieldIDs[0] = FIELD_ID_LINE_ID;
        pu32_FieldIDs[1] = FIELD_ID_MULTIPLE_CALLS_MODE;
        pu32_FieldIDs[2] = FIELD_ID_ATTACHED_HANDSETS;
        pu32_FieldIDs[3] = FIELD_ID_INTRUSION_CALL;

        List_ReadEntryById(LIST_TYPE_LINE_SETTINGS_LIST, *pu32_EntryId, pu32_FieldIDs, 4, &st_Entry);

        stLineSettings.u8_Line_Id           = (u8)st_Entry.u32_LineId;
        stLineSettings.u8_Multiple_Calls    = st_Entry.bMultiCalls ? CMBS_MULTICALL_LINE_NUM_OF_CALLS : 0;
        stLineSettings.u16_Attached_HS      = (u16)st_Entry.u32_AttachedHsMask;
        stLineSettings.u8_Call_Intrusion    = st_Entry.bIntrusionCall;
        stLineSettings.e_LineType           = st_Entry.bMultiCalls ? CMBS_LINE_TYPE_VOIP_PARALLEL_CALL : CMBS_LINE_TYPE_VOIP_DOUBLE_CALL;

        app_SrvLineSettingsSet(&stLineSettings, 1);

        // In case CFU/CFNA/CFB changed, need to update Line status and Diagnostic list
		for ( u32_index = 0; u32_index < u32_FieldsNum; ++u32_index )
		{
	        if(pu32_Fields[u32_index] == FIELD_ID_CALL_FWD_UNCOND)     
	        {
                LineAndDiagnosticListUpdateCallFWD(stLineSettings.u8_Line_Id, FIELD_ID_CALL_FWD_CFU_STATUS, ((stLineSettingsListEntry *)pu8_Entry)->bCallFwdUncond );
				g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_LineId = stLineSettings.u8_Line_Id;
				g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_AttachedHandsets = stLineSettings.u16_Attached_HS;
	        }
    	    if(pu32_Fields[u32_index] == FIELD_ID_CALL_FWD_NO_ANSWER)
    	    {
                 LineAndDiagnosticListUpdateCallFWD(stLineSettings.u8_Line_Id, FIELD_ID_CALL_FWD_CFNA_STATUS, ((stLineSettingsListEntry *)pu8_Entry)->bCallFwdNoAns);
				 g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_LineId = stLineSettings.u8_Line_Id;
				 g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_AttachedHandsets = stLineSettings.u16_Attached_HS;
    	    }
        	if(pu32_Fields[u32_index] == FIELD_ID_CALL_FWD_BUSY)
        	{
                 LineAndDiagnosticListUpdateCallFWD(stLineSettings.u8_Line_Id, FIELD_ID_CALL_FWD_CFB_STATUS, ((stLineSettingsListEntry *)pu8_Entry)->bCallFwdBusy);
				 g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_LineId = stLineSettings.u8_Line_Id;
				 g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_AttachedHandsets = stLineSettings.u16_Attached_HS;
        	}
		}

		if ( (*pu32_EntryId != 0) && (stLineSettings.u16_Attached_HS != stLineSettingsPrev.u16_Attached_HS) )
		{
			u16 u16_Attached_HS = ~(stLineSettingsPrev.u16_Attached_HS) & stLineSettings.u16_Attached_HS; // only take newly added handsets.
			
			g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_LineId = stLineSettings.u8_Line_Id;
			g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_AttachedHandsets = u16_Attached_HS;
		}
        
    }

    if(u32_listId == LIST_TYPE_DRAFT_SMS)
    {
        // If 'Send request' is set - need to Delete Entry from Draft list and Insert it to Outgoing list
        if(*pu32_Fields == FIELD_ID_SMS_SEND_REQUEST)    
        {
            // Delete entry from Draft SMS list
            list_rc = List_DeleteEntry(LIST_TYPE_DRAFT_SMS, (u32)*pu32_EntryId);            
            if ( list_rc != LIST_RC_OK )
            {
                return LA_SESSION_MGR_RC_FAIL;
            }

            // Insert entry to Outgoing SMS list
            pu32_Fields[0] = FIELD_ID_ENTRY_ID;
            pu32_Fields[1] = FIELD_ID_NUMBER;
            pu32_Fields[2] = FIELD_ID_NAME;
            pu32_Fields[3] = FIELD_ID_DATE_AND_TIME;
            pu32_Fields[4] = FIELD_ID_SMS_IDENTIFIER;
            pu32_Fields[5] = FIELD_ID_SMS_NETWORK_ENCODING;
            pu32_Fields[6] = FIELD_ID_SMS_SIZE;
            pu32_Fields[7] = FIELD_ID_SMS_CONTENT;            
            u32_FieldsNum = 8;

            list_rc = List_InsertEntry(LIST_TYPE_OUTGOING_SMS, pu8_Entry, pu32_Fields, u32_FieldsNum, pu32_EntryId);

            // Send notification to the HS - deletion of entry from "Draft" list
            ListChangeNotif_SMSListChanged( ((stSMSListEntry *)pu8_Entry)->u32_SMSServiseId, LIST_TYPE_DRAFT_SMS, CMBS_ALL_RELEVANT_HS_ID);
            
            if ( list_rc != LIST_RC_OK )
            {
                return LA_SESSION_MGR_RC_FAIL;
            }
        }
    }
    return LA_SESSION_MGR_RC_OK;
}


LA_SESSION_MGR_RC LASessionMgr_EditEntry(IN u16 u16_SessionId, IN u32 u32_EntryId,
                                         IN PST_IE_LA_FIELDS pst_RequestedFields, OUT u8 pu8_Data[], INOUT u16* pu16_DataLen)
{
    u32 u32_ListId, pu32_Fields[LA_SESSION_MGR_MAX_FIELDS], u32_FieldsNum;
    u8 pu8_Entry[LIST_ENTRY_MAX_SIZE];
    LIST_RC list_rc;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    u32_ListId = g_pst_LASessions[s32_Index].u32_ListId;

    PrepareFieldsForRead(u32_ListId, pst_RequestedFields, pu32_Fields, &u32_FieldsNum);

    list_rc = List_ReadEntryById(u32_ListId, u32_EntryId, pu32_Fields, u32_FieldsNum, pu8_Entry);

    if ( list_rc == LIST_RC_FAIL )
    {
        return LA_SESSION_MGR_RC_FAIL;
    }
    else if ( list_rc == LIST_RC_ENTRY_NOT_AVAILABLE )
    {
        return LA_SESSION_MGR_RC_ENTRY_NOT_AVAILABLE;
    }

    ConvertEntriesToCatiqDataBuffer(u32_ListId, pst_RequestedFields, pu8_Entry, 1, pu8_Data, pu16_DataLen);

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_DeleteEntry(IN u16 u16_SessionId, IN u16 u16_EntryId, OUT u16* pu16_NumOfEntries)
{
    u32 u32_ListId, u32_EntryCount, u32_EntryIdx;
    LIST_RC list_rc;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    u32_ListId = g_pst_LASessions[s32_Index].u32_ListId;

    /* For list change notification */
    if ( u32_ListId == LIST_TYPE_MISSED_CALLS || u32_ListId == LIST_TYPE_LINE_SETTINGS_LIST )
    {
        if ( u32_ListId == LIST_TYPE_MISSED_CALLS )
        {
            g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[0] = MissedCallGetLineIdByEntryId(u16_EntryId);
        }
        else
        {
            g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[0] = LineSettingsGetLineIdByEntryId(u16_EntryId);
        }

        List_GetAttachedHs(g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[0],
                           &g_pst_LASessions[s32_Index].pu32_AttachedHsOfDeletedEntries[0]);

        g_pst_LASessions[s32_Index].u32_LineIdsSize = 1;
    }

    /* Check if Entry exists */
    list_rc = List_GetEntryIdex(u32_ListId, g_pst_LASessions[s32_Index].u32_SortField1, g_pst_LASessions[s32_Index].u32_SortField2, 
                                u16_EntryId, &u32_EntryIdx);

    if ( list_rc == LIST_RC_ENTRY_NOT_AVAILABLE )
    {
        return LA_SESSION_MGR_RC_ENTRY_NOT_AVAILABLE;
    }

	List_SetListChangeOriginatingHandset (u32_ListId, g_pst_LASessions[s32_Index].u32_HsId);

    /* Delete Entry */
    list_rc = List_DeleteEntry(u32_ListId, u16_EntryId);

	{
		ST_APP_FACILITY_LCN_SUB_DETAILS st_LcnSubDetails;
		
		st_LcnSubDetails.u16_Change = APP_FACILITY_CHANGE_DELETE;
		st_LcnSubDetails.u16_EntryId = u16_EntryId;
		st_LcnSubDetails.u16_PosIndex = 0;
		
		List_AddListChangeDetails (u32_ListId, &st_LcnSubDetails);
	}

    if ( list_rc == LIST_RC_FAIL )
    {
        return LA_SESSION_MGR_RC_ENTRY_NOT_AVAILABLE;
    }

    List_GetCount(u32_ListId, &u32_EntryCount);
    *pu16_NumOfEntries = (u16)u32_EntryCount;

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_DeleteAllEntries(IN u16 u16_SessionId)
{
    u32 u32_ListId;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    u32_ListId = g_pst_LASessions[s32_Index].u32_ListId;

    if ( u32_ListId == LIST_TYPE_LINE_SETTINGS_LIST )
    {
        return LA_SESSION_MGR_RC_NOT_ALLOWED;
    }

    /* For missed call notification */
    if ( u32_ListId == LIST_TYPE_MISSED_CALLS )
    {
        u32 u32_EntryCount, u32_Index, u32_FieldId = FIELD_ID_LINE_ID, u32_EntrySize = 1, u32_DifferentLineIds = 0;

        List_GetCount(u32_ListId, &u32_EntryCount);

        for ( u32_Index = 1; u32_Index <= u32_EntryCount; ++u32_Index )
        {
            stCallsListEntry st_Entry;

            List_ReadEntries(u32_ListId, &u32_Index, TRUE, MARK_LEAVE_UNCHANGED, &u32_FieldId, 1, FIELD_ID_LINE_ID, FIELD_ID_INVALID,
                             &st_Entry, &u32_EntrySize);

            if ( (u32_DifferentLineIds == 0) || 
                 ((u32_DifferentLineIds > 0) && (g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[u32_DifferentLineIds - 1] != st_Entry.u32_LineId)) )
            {
                g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[u32_DifferentLineIds] = st_Entry.u32_LineId;

                List_GetAttachedHs(st_Entry.u32_LineId, &g_pst_LASessions[s32_Index].pu32_AttachedHsOfDeletedEntries[u32_DifferentLineIds++]);
            }
        }

        g_pst_LASessions[s32_Index].u32_LineIdsSize = u32_DifferentLineIds;
    }

	List_SetListChangeOriginatingHandset (u32_ListId, g_pst_LASessions[s32_Index].u32_HsId);

    List_DeleteAllEntries(u32_ListId);

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_SendMissedCallNotification(IN u16 u16_SessionId, IN E_CMBS_MARK_REQUEST eMark)
{
    u32 u32_NumOfLines, u32_Index;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    if ( g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_MISSED_CALLS &&
         eMark != CMBS_MARK_REQUEST_LEAVE_UNCHANGED )
    {

        List_GetCount(LIST_TYPE_LINE_SETTINGS_LIST, &u32_NumOfLines);

        for ( u32_Index = 0; u32_Index < u32_NumOfLines; ++u32_Index )
        {
            if ( g_pst_LASessions[s32_Index].pst_MissedCallNotifAux[u32_Index].u32_NumOfCallsBefore != 
                 g_pst_LASessions[s32_Index].pst_MissedCallNotifAux[u32_Index].u32_NumOfCallsAfter )
            {
                /* Need to send missed call notification */
                ListChangeNotif_MissedCallListChanged(g_pst_LASessions[s32_Index].pst_MissedCallNotifAux[u32_Index].u32_LineId, FALSE, CMBS_ALL_RELEVANT_HS_ID);
            }
        }
    }

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_SendListChangedNotification(IN u16 u16_SessionId, IN u16 u16_EntryId)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    if ( g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_LINE_SETTINGS_LIST )
    {
        u32 u32_LineId = LineSettingsGetLineIdByEntryId(u16_EntryId);
        SendLineSettingsListChangeNotification(u32_LineId);
		if (g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_LineId != LIST_LINE_ID_NONE)
		{
			app_FacilityDiagnosticIndication(0, g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_LineId, CMBS_LINE_RELATED_CHANGE, (u16)g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_AttachedHandsets);
			g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_LineId = LIST_LINE_ID_NONE;
			g_pst_LASessions[s32_Index].st_LineDiagNotifAux.u32_AttachedHandsets = 0;
		}
    }
    else if ( g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_MISSED_CALLS )
    {
        u32 u32_LineId = MissedCallGetLineIdByEntryId(u16_EntryId);
        ListChangeNotif_MissedCallListChanged(u32_LineId, FALSE, CMBS_ALL_RELEVANT_HS_ID);
    }
    else if ((g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_DRAFT_SMS) || (g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_OUTGOING_SMS)
            || (g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_SENT_SMS) )
    {
        u32 u32_SMSId = GetSMSIdByEntryId(u16_EntryId, g_pst_LASessions[s32_Index].u32_ListId);
        ListChangeNotif_SMSListChanged( u32_SMSId, g_pst_LASessions[s32_Index].u32_ListId, CMBS_ALL_RELEVANT_HS_ID);
    }

    return LA_SESSION_MGR_RC_OK;
}

LA_SESSION_MGR_RC LASessionMgr_SendListChangedNotificationOnDelete(IN u16 u16_SessionId)
{
    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    if ( g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_LINE_SETTINGS_LIST )
    {
        ListChangeNotif_ListChanged( g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[0],
                                     LINE_TYPE_RELATING_TO,
                                     g_pst_LASessions[s32_Index].pu32_AttachedHsOfDeletedEntries[0],
                                     0,
                                     LIST_CHANGE_NOTIF_LIST_TYPE_LINE_SETTINGS_LIST );

    }
    else if ( g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_MISSED_CALLS )
    {
        u32 u32_index;

        for ( u32_index = 0; u32_index < g_pst_LASessions[s32_Index].u32_LineIdsSize; ++u32_index )
        {
            u32 u32_NumOfRead, u32_NumOfUnread;

            List_GetMissedCallsNumOfEntries(g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[u32_index],
                                            &u32_NumOfUnread, &u32_NumOfRead);

            ListChangeNotif_MissedCallListChanged(g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[u32_index], FALSE, CMBS_ALL_RELEVANT_HS_ID);
        }
    }
    else if ((g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_DRAFT_SMS) || (g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_OUTGOING_SMS)
            || (g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_SENT_SMS) )
    {
       u32 u32_index, u32_SMSId;

       for ( u32_index = 0; u32_index < g_pst_LASessions[s32_Index].u32_LineIdsSize; ++u32_index )
       {
           if ( GetSMSIdByLineID(g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[s32_Index], &u32_SMSId) == CMBS_RC_OK )
           {
               ListChangeNotif_SMSListChanged(u32_SMSId, g_pst_LASessions[s32_Index].u32_ListId, CMBS_ALL_RELEVANT_HS_ID);
           }
       }
    }
    else if(g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_INCOMING_SMS)
    {
       u32 u32_index;

       for ( u32_index = 0; u32_index < g_pst_LASessions[s32_Index].u32_LineIdsSize; ++u32_index )
       {
          u32 u32_SMSId = 0;
          if ( GetSMSIdByLineID(g_pst_LASessions[s32_Index].pu32_LineIdsOfDeletedEntries[s32_Index], &u32_SMSId) == CMBS_RC_OK )
          {
              ListChangeNotif_IncomingSMSListChanged(u32_SMSId, CMBS_SMS_OTHER_LIST_CHANGE, CMBS_ALL_RELEVANT_HS_ID);
          }
       }
    }
    return LA_SESSION_MGR_RC_OK;
}


LA_SESSION_MGR_RC LASessionMgr_SendSMSNotification(IN u16 u16_SessionId, IN E_CMBS_MARK_REQUEST eMark)
{
    u32 u32_Entries, u32_Index;

    s32 s32_Index;
    CHECK_SESSION_ID(u16_SessionId);

    if ( g_pst_LASessions[s32_Index].u32_ListId == LIST_TYPE_INCOMING_SMS&&
         eMark != CMBS_MARK_REQUEST_LEAVE_UNCHANGED )
    {

        List_GetCount(LIST_TYPE_INCOMING_SMS, &u32_Entries);

        for ( u32_Index = 0; u32_Index < u32_Entries; ++u32_Index )
        {
            if ( g_pst_LASessions[s32_Index].pst_SMSNotifAux[u32_Index].u32_NumOfSMSBefore != 
                 g_pst_LASessions[s32_Index].pst_SMSNotifAux[u32_Index].u32_NumOfSMSAfter )
            {
                /* Need to send SMS notification */
                ListChangeNotif_IncomingSMSListChanged( g_pst_LASessions[s32_Index].pst_SMSNotifAux[u32_Index].u32_SMSId, CMBS_SMS_OTHER_LIST_CHANGE, CMBS_ALL_RELEVANT_HS_ID );
            }
        }
    }
    return LA_SESSION_MGR_RC_OK;
}

/* End Of File *****************************************************************************************************************************/

