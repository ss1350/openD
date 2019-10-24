/*!
*  \file       appfacility.h
*	\brief
*	\Author		stein
*
*	@(#)	%filespec: appfacility.h~6 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
*******************************************************************************/

#if	!defined( APPFACILITY_H )
#define	APPFACILITY_H


#if defined( __cplusplus )
extern "C"
{
#endif

typedef enum
{
    APP_FACILITY_CHANGE_EDIT = 0,			 		 	/*!< List change due to edit/modification */
	APP_FACILITY_CHANGE_ADD,							/*!< List change due to addition */
	APP_FACILITY_CHANGE_DELETE						 	/*!< List change due to deletion */
} E_APP_FACILITY_CHANGE_REASON;


#define APP_FACILITY_MAX_SUB_DETAILS 5

typedef struct
{
    u16       u16_Change;				//E_CMBS_LIST_CHANGE_REASON
    u16       u16_EntryId;
    u16       u16_PosIndex;
} ST_APP_FACILITY_LCN_SUB_DETAILS;

typedef struct
{
    u8        u8_Count;
    u16       u16_OrgHs;
    ST_APP_FACILITY_LCN_SUB_DETAILS        st_SubDetails[APP_FACILITY_MAX_SUB_DETAILS];				//E_CMBS_LIST_CHANGE_REASON
} ST_APP_FACILITY_LCN_DETAILS;


E_CMBS_RC         app_FacilityInit(void);
E_CMBS_RC         app_FacilityMWI( u16 u16_RequestId, u8 u8_LineId, u16 u16_Messages, u16 u16_HSMask, E_CMBS_MWI_TYPE eType );
E_CMBS_RC         app_FacilityMissedCalls( u16 u16_RequestId, u8 u8_LineId, u16 u16_NewMissedCalls, u16 u16_HSMask, bool bNewMissedCall, u16 u16_TotalMissedCalls );
E_CMBS_RC         app_FacilityListChanged( u16 u16_RequestId, u8 u8_ListId, u8 u8_ListEntries, u16 u16_HSMask, u8 u8_LineId, u8 u8_LineSubtype );
E_CMBS_RC         app_FacilityWebContent( u16 u16_RequestId, u8 u8_NumOfWebCont, u16 u16_HSMask );

E_CMBS_RC         app_FacilityPropEvent( u16 u16_RequestId, u16 u16_Reserved, u8 * pu8_Data, u8 u8_DataLen, u16 u16_HSMask );
E_CMBS_RC         app_FacilityDateTime( u16 u16_RequestId, ST_DATE_TIME * pst_DateTime, u16 u16_Hs);
E_CMBS_RC         app_FacilityLineUseIndication(u16 u16_RequestId, u8 u8_LineId, u32 u32_LineUseStatus, u16 u16_Hs);
E_CMBS_RC         app_FacilityHSUseIndication( u16 u16_RequestId, u8 u8_LineId, u16 u16_Hs);
E_CMBS_RC         app_FacilityDiagnosticIndication( u16 u16_RequestId, u8 u8_LineId, u8 u8_Type, u16 u16_Hs);
E_CMBS_RC         app_FacilitySMSMessageNotification(u8 u8_NewSMS, u16 u16_TotalNumOfMsgs, u16 u16_NumOfUnreadMsgs, u16 u16_HSMask);
E_CMBS_RC         app_FacilityPropEventNotification(u16 u16_RequestId, u16 u16_PropEvent, u8 * pu8_Data, u8 u8_DataLen, char * psz_Handsets);

E_CMBS_RC         app_FacilityListChangedEx(u16 u16_RequestId, u8 u8_ListId, u8 u8_ListEntries, u16 u16_HSMask, u8 u8_LineId, u8 u8_LineSubtype, 
												ST_APP_FACILITY_LCN_DETAILS *pst_Details);

#define MAX_FACILITY_PROP_EVENT_LEN CMBS_PROP_EVENT_LENGTH
#define FACILITY_PROP_EVENT_HEADER_LEN 5
#define FACILITY_DISCRIMINATOR_TYPE 0x81
#define CUSTOMER_DESCRIMINATOR 0x0505

#include "tcx_hostlog.h"
#define APP_FACILITY_INFO_PRINT(format, ...) 						tcx_WriteLog(FACILITY_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define APP_FACILITY_TRACE_PRINT(format, ...) 						tcx_WriteLog(FACILITY_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define APP_FACILITY_WARNING_PRINT(format, ...) 					tcx_WriteLog(FACILITY_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define APP_FACILITY_ERROR_PRINT(format, ...) 						tcx_WriteLog(FACILITY_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define APP_FACILITY_RT_PRINT(format, ...) 						tcx_WriteLog(FACILITY_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define APP_FACILITY_PRINT_DATA(pu8_Buffer, u16_Length) 			tcx_WriteLogData(FACILITY_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)

#if defined( __cplusplus )
}
#endif

#endif	//APPFACILITY_H
//*/
