#ifndef APPCALLROUTER
#define APPCALLROUTER

#include "cmbs_voip_api.h"
#include "ListsApp.h"

#define 	MAX_NUM_OF_APPS 	 1


typedef enum
{
	EXTAPP_RC_OK,
	EXTAPP_RC_FAIL
} EXTAPP_RC;


// Make a call
typedef EXTAPP_RC (*app_SetupCallFunc)(IN u32 cmbsCallID, IN const char* calledID, IN const char* callerHandset, IN u32 lineID, IN EXTVOIP_CODEC* codecList, IN u32 codecListLength);

// Proceeding
typedef EXTAPP_RC (*app_ProceedingCallFunc)(IN u32 cmbsCallID);

// Alerting
typedef EXTAPP_RC (*app_AlertingCallFunc)(IN u32 cmbsCallID);

// Answer call
typedef EXTAPP_RC (*app_AnswerCallFunc)(IN u32 cmbsCallID, IN EXTVOIP_CODEC codec);

// Release active call
typedef EXTAPP_RC (*app_DisconnectCallFunc)(IN u32 cmbsCallID, IN EXTVOIP_RELEASE_REASON disconnectReason);

// Release active call ack
typedef EXTAPP_RC (*app_DisconnectCallDoneFunc)(IN u32 cmbsCallID);

// Hold call
typedef EXTAPP_RC (*app_HoldCallFunc)(IN u32 cmbsCallID);

// Hold call ack
typedef EXTAPP_RC (*app_HoldCallAckFunc)(IN u32 cmbsCallID);

// Resume call
typedef EXTAPP_RC (*app_ResumeCallFunc)(IN u32 cmbsCallID);

// Resume call ack
typedef EXTAPP_RC (*app_ResumeCallAckFunc)(IN u32 cmbsCallID);

// Conference call
typedef EXTAPP_RC (*app_ConferenceCallFunc)(IN u32 cmbsCallID, IN u32 cmbsCallID2);

// Change codec
typedef EXTAPP_RC (*app_MediaChangeFunc)(IN u32 cmbsCallID, IN EXTVOIP_CODEC codec);

// Change codec ack
typedef EXTAPP_RC (*app_MediaChangeAckFunc)(IN u32 cmbsCallID, IN EXTVOIP_CODEC codec);

// Send digits
typedef EXTAPP_RC (*app_SendDigitsCallFunc)(IN u32 cmbsCallID, IN const char* digits);

/*
 * Helpers
 */

// Retrieve call caller id
typedef EXTAPP_RC (*app_GetCallerIDFunc)(IN u32 cmbsCallID, OUT char* callerID);

// Retrieve call called id
typedef EXTAPP_RC (*app_GetCalledIDFunc)(IN u32 cmbsCallID, OUT char* calledID);


/*
 * Audio config
 */

// Set audio channel
typedef EXTAPP_RC (*app_SetAudioChannelFunc)(IN u32 cmbsCallID, IN u32 mediaChannelID);

/*
 * Contacts sync
 */

// Retrieve contacts size
typedef u32 (*app_GetContactsCountFunc)();

// Retrieve contact entry
typedef EXTAPP_RC (*app_GetContactEntryFunc)(IN u32 itemid, OUT char* firstName, OUT char* lastName, OUT char* number, OUT u32* isOnline);


typedef struct app_FnTable_s
{	
	// line ID
	//u32 LineID;

	char LineName[LIST_NAME_MAX_LEN];
	
	// function for call setup
	app_SetupCallFunc			fncSetupCall;
	// function for call proceeding
	app_ProceedingCallFunc		fncProceedingCall;
	// function for call proceeding
	app_AlertingCallFunc		fncAlertingCall;
	// function for call answer
	app_AnswerCallFunc			fncAnswerCall;
	// function for call disconnect
	app_DisconnectCallFunc		fncDisconnectCall;
	// function for call disconnect done
	app_DisconnectCallDoneFunc	fncDisconnectCallDone;
	// function for call hold
	app_HoldCallFunc			fncHoldCall;
	// function for call hold acknowledge
	app_HoldCallAckFunc			fncHoldCallDone;
	// function for call resume
	app_ResumeCallFunc			fncResumeCall;
	// function for call conference
	app_ConferenceCallFunc		fncConferenceCall;
	// function for call resume acknowledge
	app_ResumeCallAckFunc		fncResumeCallDone;
	// function for call codec change	
	app_MediaChangeFunc			fncMediaChange;
	// function for call codec change acknowledge
	app_MediaChangeAckFunc		fncMediaChangeAck;
	// function to send digits
	app_SendDigitsCallFunc		fncSendDigits;
	// function to retrieve caller ID
	app_GetCallerIDFunc			fncGetCallerID;
	// function to retrieve called ID
	app_GetCalledIDFunc			fncGetCalledID;
	// function to set audio channel
	app_SetAudioChannelFunc		fncSetAudioChannel;
	// function to retrieve cotacts size
	app_GetContactsCountFunc	fncContactsCount;
	// function to retrieve contact entry
	app_GetContactEntryFunc		fncContactsEntry;
} ST_APP_FNC_TABLE;


typedef struct
{
	u32 u32_AppId;
	u32 u32_StartLineId;
	u32 u32_MaxLineId;
	const char *psz_HandsetNum;
	const char *psz_DeviceId;
	const char *psz_Digits;
	//void (*app_callback)(u32 u32_AppId, u32 u32_StartLineId, u32 u32_MaxLineId, ST_APP_FNC_TABLE *pfn_AppTable);
}ST_CMBS_ROUTE_TABLE;


void AppCallRouter_Init (void);
EXTAPP_RC AppCallRouter_AppSelection(IN u32 u32_CallID, IN const char* psz_CalledId, IN const char* psz_CallerHandset, IN u32 u32_LineId);

u8 AppCallRouter_AppInit (u32 u32_AppId, ST_APP_FNC_TABLE *pst_AppTable);

EXTVOIP_RC AppCallRouter_MakeCall(u32 u32_CallID, u32 u32_AppId);



#endif
