#include "cmbs_voipline.h"
#include "appsrv.h"
#include <string.h>
#include <stdlib.h>
#include "ListChangeNotif.h"
#include "LASessionMgr.h"

#include "appCallRouter.h"
// A map LineID <-> VoipApplication
ST_EXTVOIPLINE* g_voipLines[APPCALL_LINEOBJ_MAX];

extern ST_APP_FNC_TABLE g_st_AppTable[APPCALL_LINEOBJ_MAX];
extern u32 g_u32_AppCallIDMap[APPCALL_LINEOBJ_MAX];
/*
* Initializes voip applications array
*/
void extvoip_InitVoiplines()
{
	int i=0;
	for(i=0;i<APPCALL_LINEOBJ_MAX;i++)
		g_voipLines[i] = NULL;
}

/*
* Registers VoIP application on desired lineID
* Input: 
* lineID: lineID
* voipline: pointer to ST_EXTVOIPLINE structure
*/
EXTVOIP_RC extvoip_RegisterVoipline(u32 lineID,  char* LineName )
{
	if(lineID < APPCALL_LINEOBJ_MAX)
	{
		//g_voipLines[lineID] = voipline;

		_extvoip_updateLineSettingsList(lineID,LineName);
		
		return EXTVOIP_RC_OK;
	}

	return EXTVOIP_RC_FAIL;
}

/*
* Unregisters VoIP application
* Input: 
* voipline: pointer to ST_EXTVOIPLINE structure
*/
EXTVOIP_RC extvoip_UnRegisterVoipline( ST_EXTVOIPLINE* voipline )
{
	int i=0;
	for(i=0;i<APPCALL_LINEOBJ_MAX;i++)
		if(g_voipLines[i] == voipline)
			g_voipLines[i] = NULL;

	return EXTVOIP_RC_OK;
}

/*
* Retrieves the correspond VoIP application using callID
* Input: 
* cmbs_callID: callID
*/
ST_EXTVOIPLINE* extvoip_GetVoIPAllicationByCallID(int cmbs_callID)
{
	u8 lineID = 0xFF;
	PST_CALL_OBJ   pstLine = _appcall_CallObjGetById(cmbs_callID);
	if(pstLine)
		lineID = pstLine->u8_LineId;

	if(lineID < APPCALL_LINEOBJ_MAX)
		return g_voipLines[lineID];

	return NULL;
}

/*
* Retrieves the correspond VoIP application using lineID
* Input: 
* LineID: LineID
*/
ST_EXTVOIPLINE* extvoip_GetVoIPAllicationByLineID(int LineID)
{
	if(LineID < APPCALL_LINEOBJ_MAX)
		return g_voipLines[LineID];

	return NULL;
}

/*
* Make VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
* data: CLI
* len: CLI length
* mediaChannelID: media channel identifier
*/
EXTVOIP_RC extvoip_MakeCall(u32 cmbs_callID)
{	
	EXTAPP_RC rc = CMBS_RC_OK;
	//ST_EXTVOIPLINE* voipLine = NULL;
	EXTVOIP_CODEC codecs[CMBS_AUDIO_CODEC_MAX] = {0};
	u32 appID;

	// get call object
	PST_CALL_OBJ pstCall = _appcall_CallObjGetById(cmbs_callID);
	if(!pstCall)
		return EXTVOIP_RC_FAIL;

	// make codecs list
	if(pstCall->u8_CodecsLength)
	{
		int i = 0;
		for(i=0;i<pstCall->u8_CodecsLength;i++)
			// TODO: in case voip codecs and cmbs codecs are different - add special function for this transformation
			codecs[i] = (EXTVOIP_CODEC)pstCall->pu8_CodecsList[i];
	}
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	
	// call SetupCall function
	if((g_st_AppTable[appID]).fncSetupCall)
		rc = ((g_st_AppTable[appID]).fncSetupCall)(cmbs_callID, pstCall->ch_CalledID, pstCall->ch_CallerID, pstCall->u8_LineId, codecs, pstCall->u8_CodecsLength);
	UNUSED_PARAMETER(rc);

	// set audio channel
	if(g_st_AppTable[appID].fncSetAudioChannel && pstCall->u32_ChannelID != APPCALL_NO_MEDIA_CHANNEL)

		rc = (g_st_AppTable[appID].fncSetAudioChannel)(cmbs_callID, pstCall->u32_ChannelID);
	return EXTVOIP_RC_OK;
}

/*
* Proceeding VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_ProceedingCall( u32 cmbs_callID )
{
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncProceedingCall)
		g_st_AppTable[appID].fncProceedingCall(cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Alerting VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_AlertingCall( u32 cmbs_callID )
{
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncAlertingCall)
		g_st_AppTable[appID].fncAlertingCall(cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Answer VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_AnswerCall(u32 cmbs_callID)
{	
	u32 appID;
	//ST_EXTVOIPLINE* voipLine = NULL;

	// get call object
	PST_CALL_OBJ pstCall = _appcall_CallObjGetById(cmbs_callID);
	if(!pstCall)
		return EXTVOIP_RC_FAIL;

	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncAnswerCall)
		g_st_AppTable[appID].fncAnswerCall(cmbs_callID, cmbsCodec2VoipCodec(pstCall->e_Codec));

	if(g_st_AppTable[appID].fncSetAudioChannel)
		(g_st_AppTable[appID].fncSetAudioChannel)(cmbs_callID, pstCall->u32_ChannelID);

	return EXTVOIP_RC_OK;
}

/*
* Release VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_DisconnectCall(u32 cmbs_callID)
{	
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	// TODO: disconnect reason
	if (g_st_AppTable[appID].fncDisconnectCall)
		g_st_AppTable[appID].fncDisconnectCall(cmbs_callID, EXTVOIP_REASON_NORMAL);

	return EXTVOIP_RC_OK;
}

/*
* Release Ack VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_DisconnectDoneCall(u32 cmbs_callID)
{	
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];

	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncDisconnectCallDone)
		g_st_AppTable[appID].fncDisconnectCallDone(cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Hold VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_HoldCall(u32 cmbs_callID)
{	
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncHoldCall)
		g_st_AppTable[appID].fncHoldCall(cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Hold ack VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_HoldAckCall(u32 cmbs_callID)
{	
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncHoldCallDone)
		g_st_AppTable[appID].fncHoldCallDone(cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Resume VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_ResumeCall(u32 cmbs_callID)
{	
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncResumeCall)
		g_st_AppTable[appID].fncResumeCall(cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Make conference
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_ConferenceCall( u32 cmbs_callID, u32 cmbs_callID2 )
{
	u32 appID;

	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if ( g_st_AppTable[appID].fncConferenceCall)
		g_st_AppTable[appID].fncConferenceCall(cmbs_callID, cmbs_callID2);

	if ( g_st_AppTable[appID].fncConferenceCall)
		g_st_AppTable[appID].fncConferenceCall(cmbs_callID2, cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Resume Ack VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
*/
EXTVOIP_RC extvoip_ResumeAckCall(u32 cmbs_callID)
{	
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncResumeCallDone)
		g_st_AppTable[appID].fncResumeCallDone(cmbs_callID);

	return EXTVOIP_RC_OK;
}

/*
* Media change for VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
* codec: new codec
*/
EXTVOIP_RC extvoip_MediaChange( u32 cmbs_callID, E_CMBS_AUDIO_CODEC codec )
{
	u32 appID;
	PST_CALL_OBJ pstCall = _appcall_CallObjGetById(cmbs_callID);
	//ST_EXTVOIPLINE* voipLine = extvoip_GetVoIPAllicationByCallID(cmbs_callID);

	(void)codec; // TODO


	// get call object	
	if(!pstCall)
		return EXTVOIP_RC_FAIL;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	// set audio channel
	if(g_st_AppTable[appID].fncSetAudioChannel)
		(g_st_AppTable[appID].fncSetAudioChannel)(cmbs_callID, pstCall->u32_ChannelID);

	return EXTVOIP_RC_OK;

	// voipLine->fncMediaChange(cmbs_callID, cmbsCodec2VoipCodec(codec));
}

/*
* Media change acknowledge for VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
* codec: confirmed codec
*/
EXTVOIP_RC extvoip_MediaAckChange( u32 cmbs_callID, E_CMBS_AUDIO_CODEC codec )
{
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncMediaChangeAck)
		g_st_AppTable[appID].fncMediaChangeAck(cmbs_callID, cmbsCodec2VoipCodec(codec));

	return EXTVOIP_RC_OK;
}

/*
* Send digits for VoIP call
* Input: 
* cmbs_callID: cmbs callID identifier
* digits: digits
*/
EXTVOIP_RC extvoip_SendDigits( u32 cmbs_callID, const char* digits )
{
	u32 appID;
	appID = g_u32_AppCallIDMap[cmbs_callID];
	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;
	if (g_st_AppTable[appID].fncSendDigits)
		g_st_AppTable[appID].fncSendDigits(cmbs_callID, digits);

	return EXTVOIP_RC_OK;
}

/*
 * VoIP Callbacks
 */
extern ST_CALL_OBJ g_call_obj[APPCALL_CALLOBJ_MAX];
// On Incoming call
EXTVOIP_RC extvoip_OnSetupCall(IN u32 appID,IN int lineID, IN const char * calledHandset, IN const char* callerNumber, IN const char* callerName, IN EXTVOIP_CODEC* codecList, IN int codecListLength, OUT int* cmbsCallID)
{
	ST_APPCALL_PROPERTIES st_Properties[5];	
	int  n_Prop = 3;
	u8 i=0;
	static char c_callerName[30] = {0};
	static char c_callerNumber[30] = {0};
	static char c_lineid[3] = {0};
	static char c_hs[7] = {0};
	static char s_codecs[CMBS_AUDIO_CODEC_MAX*2] = {0};

	if ((appID == -1) || (appID >= MAX_NUM_OF_APPS))
		return EXTVOIP_RC_FAIL;

	strncpy(c_callerNumber, callerNumber,(sizeof(c_callerNumber)-1));
	strncpy(c_callerName, callerName,(sizeof(c_callerName)-1));
	sprintf(c_lineid, "%d", lineID);
	if(calledHandset)
		sprintf(c_hs, "%s", calledHandset);
	else
		strcpy(c_hs, "h12345");

	for(i=0;i<5;i++)
		st_Properties[i].psz_Value = 0;

	if(codecListLength)
	{
		for(i=0;i<codecListLength;i++)
		{
			char s_codec[5] = {0};
			if(i)
				sprintf(s_codec, ",%d", voipCodec2CmbsCodec(codecList[i]));
			else
				sprintf(s_codec, "%d", voipCodec2CmbsCodec(codecList[i]));
			strcat(s_codecs, s_codec);
		}
	}
	else
	{
		// take default codecs
		sprintf(s_codecs, "%d,%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_WB), (CMBS_AUDIO_CODEC_PCM_LINEAR_NB)); 
	}

	st_Properties[0].e_IE      = CMBS_IE_CALLERPARTY;
	st_Properties[0].psz_Value = c_callerNumber;
	st_Properties[1].e_IE      = CMBS_IE_CALLEDPARTY;
	st_Properties[1].psz_Value = c_hs;
	st_Properties[2].e_IE      = CMBS_IE_MEDIADESCRIPTOR;
	st_Properties[2].psz_Value = s_codecs; 
	st_Properties[3].e_IE      = CMBS_IE_CALLERNAME;
	st_Properties[3].psz_Value = c_callerName,
	st_Properties[4].e_IE      = CMBS_IE_LINE_ID ;
	st_Properties[4].psz_Value = c_lineid;
	n_Prop=5;

	(*cmbsCallID) = appcall_EstablishCall ( st_Properties, n_Prop );
	g_u32_AppCallIDMap[*cmbsCallID] = appID;
	return EXTVOIP_RC_OK;
}

// On call proceeding
EXTVOIP_RC extvoip_OnProceedingCall( IN int cmbsCallID )
{
	int rc = 0;
	ST_APPCALL_PROPERTIES st_Properties;
	st_Properties.e_IE      = CMBS_IE_CALLPROGRESS;
	st_Properties.psz_Value = "CMBS_CALL_PROGR_PROCEEDING\0";

#ifdef MEDIA5
	//printf("<< extvoip_OnProceedingCall >>\n");
	appmedia_CallObjTonePlay("CMBS_TONE_DIAL\0", 0, TRUE, cmbsCallID, NULL, 0, 0);
#endif

	rc = appcall_ProgressCall ( &st_Properties, 1, cmbsCallID, NULL );

	return rc == TRUE ? EXTVOIP_RC_OK : EXTVOIP_RC_FAIL;
}

// On call alerting
EXTVOIP_RC extvoip_OnAlertingCall( IN int cmbsCallID )
{
	int rc = 0;
	ST_APPCALL_PROPERTIES st_Properties;
	st_Properties.e_IE      = CMBS_IE_CALLPROGRESS;
	st_Properties.psz_Value = "CMBS_CALL_PROGR_RINGING\0";

	rc = appcall_ProgressCall ( &st_Properties, 1, cmbsCallID, NULL );

	return rc == TRUE ? EXTVOIP_RC_OK : EXTVOIP_RC_FAIL;
}

// On call answered
EXTVOIP_RC extvoip_OnAnswerCall(IN int cmbsCallID, IN EXTVOIP_CODEC codec)
{
	ST_APPCALL_PROPERTIES	st_Properties;
	E_CMBS_AUDIO_CODEC		e_Codec;
	int						rc = CMBS_RC_OK;

	memset ( &st_Properties,0, sizeof(st_Properties));

	// convert to cmbs audio codec
	e_Codec = voipCodec2CmbsCodec(codec);

	// answer call
	rc = appcall_AnswerCall ( &st_Properties, 0, cmbsCallID, NULL );

	UNUSED_PARAMETER(e_Codec);
	UNUSED_PARAMETER(rc);

	// TODO: codec switch

	return rc == TRUE ? EXTVOIP_RC_OK : EXTVOIP_RC_FAIL;
}

// On call released
EXTVOIP_RC extvoip_OnDisconnectCall(IN int cmbsCallID, IN EXTVOIP_RELEASE_REASON releaseReason)
{
	ST_APPCALL_PROPERTIES st_Properties;
	int rc = 0;
	static char s_reason[5] = {0};

	sprintf(s_reason, "%d", voipReleaseReason2CmbsReleaseReason(releaseReason));

	// disconnecting call
	st_Properties.e_IE      = CMBS_IE_CALLRELEASE_REASON;
	st_Properties.psz_Value = s_reason;

	rc = appcall_ReleaseCall(  &st_Properties, 1, cmbsCallID, NULL );

	return rc == TRUE ? EXTVOIP_RC_OK : EXTVOIP_RC_FAIL;
}

// On call release done
EXTVOIP_RC extvoip_OnDisconnectCallDone( IN int cmbsCallID )
{
	printf("extvoip_OnDisconnectCallDone call %d\n", cmbsCallID);

	// TODO: To be done

	return EXTVOIP_RC_OK;
}

// On call hold
EXTVOIP_RC extvoip_OnHoldCall(IN int cmbsCallID )
{
	int rc = 0;
	ST_APPCALL_PROPERTIES st_Properties;
	st_Properties.e_IE      = CMBS_IE_CALLPROGRESS;
	st_Properties.psz_Value = "CMBS_CALL_PROGR_HOLD\0";

	rc = appcall_ProgressCall ( &st_Properties, 1, cmbsCallID, NULL );

	return rc == TRUE ? EXTVOIP_RC_OK : EXTVOIP_RC_FAIL;
}

// On call hold ack
EXTVOIP_RC extvoip_OnHoldCallAck(IN int cmbsCallID )
{
	// TODO: To be done
	(void)cmbsCallID;

	return EXTVOIP_RC_OK;
}

// On call resume
EXTVOIP_RC extvoip_OnResumeCall( IN int cmbsCallID )
{
	int rc = 0;
	ST_APPCALL_PROPERTIES st_Properties;
	st_Properties.e_IE      = CMBS_IE_CALLPROGRESS;
	st_Properties.psz_Value = "CMBS_CALL_PROGR_CONNECT\0";

	rc = appcall_ProgressCall ( &st_Properties, 1, cmbsCallID, NULL );

	return rc == TRUE ? EXTVOIP_RC_OK : EXTVOIP_RC_FAIL;
}

// On call resume ack
EXTVOIP_RC extvoip_OnResumeCallAck(IN int cmbsCallID )
{
	// TODO: To be done
	(void)cmbsCallID;

	return EXTVOIP_RC_OK;
}

// On media change
EXTVOIP_RC extvoip_OnMediaChange( IN int cmbsCallID, IN EXTVOIP_CODEC codec )
{
	// TODO: To be done
	(void)cmbsCallID;
	(void)codec;

	return EXTVOIP_RC_OK;
}

// On media change ack
EXTVOIP_RC extvoip_OnMediaChangeAck( IN int cmbsCallID )
{
	// TODO: To be done
	(void)cmbsCallID;

	return EXTVOIP_RC_OK;
}

// On send digits
EXTVOIP_RC extvoip_OnSendDigits( IN int cmbsCallID, IN const char* digits )
{
	// TODO: To be done
	(void)cmbsCallID;
	(void)digits;

	return EXTVOIP_RC_OK;
}


// On Line status
EXTVOIP_RC voipapp_OnLineStatus(IN EXTVOIP_LINE_STATUS status)
{
	cmbsevent_onLineStatus(status);

	return EXTVOIP_RC_OK;
}

// Sync contacts
void extvoip_SyncContacts(ST_EXTVOIPLINE* voipline)
{
	int i=0;
	int added = 0;
	int contactsCount = 0;

	if(!voipline || !voipline->fncContactsCount || !voipline->fncContactsEntry)
		return;

	contactsCount = voipline->fncContactsCount();

	cmbsevent_onSyncContacts(0);

	List_CreateList(LIST_TYPE_CONTACT_LIST);

	for(i=0;i<contactsCount;i++)
	{
			char firstName[255] = {0};
			char lastName[255] = {0};
			char number[255] = {0};
			int isOnline = 0;

			EXTVOIP_RC rc = (voipline->fncContactsEntry)(i, firstName, lastName, number, &isOnline);
			if(rc == EXTVOIP_RC_OK)
			{
				u32 pu32_Fields[1], u32_FieldsNum = 1, u32_EntryId;
				stContactListEntry       st_ContactEntry;
				LIST_RC list_rc;
				u32 u32_NumOfReqEntries = 1;
				u8 pu8_Entries[10 * LIST_ENTRY_MAX_SIZE];
				u32 pu32_StartIdx = 0;

				pu32_Fields[0]  = FIELD_ID_ENTRY_ID;

				memset(&st_ContactEntry,0,sizeof(st_ContactEntry));

				// search for this entry in the contacts list
				list_rc = List_SearchEntries(LIST_TYPE_CONTACT_LIST, MATCH_EXACT, TRUE, firstName, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsNum, 
					FIELD_ID_FIRST_NAME, FIELD_ID_LAST_NAME, pu8_Entries, &u32_NumOfReqEntries, &pu32_StartIdx);
				UNUSED_PARAMETER(list_rc);

				if(!u32_NumOfReqEntries/* && isOnline*/)
				{
					u32 pu32_Fields[5], u32_FieldsNum = 5, u32_EntryId;
					pu32_Fields[0]  = FIELD_ID_LAST_NAME;
					pu32_Fields[1]  = FIELD_ID_FIRST_NAME;
					pu32_Fields[2]  = FIELD_ID_CONTACT_NUM_1;
					pu32_Fields[3]  = FIELD_ID_ASSOCIATED_MELODY;
					pu32_Fields[4]  = FIELD_ID_LINE_ID;

					strcpy(st_ContactEntry.sFirstName, firstName);
					//strcpy(st_ContactEntry.sLastName, lastName);
					if(isOnline)
						strncpy(st_ContactEntry.sLastName, lastName, strlen(lastName) < LIST_NAME_MAX_LEN ? strlen(lastName) : LIST_NAME_MAX_LEN);
					else
#ifdef WIN32
						_snprintf(st_ContactEntry.sLastName, strlen(lastName)-1 < LIST_NAME_MAX_LEN ? strlen(lastName)-1 : LIST_NAME_MAX_LEN, "~%s", lastName);
#else
						snprintf(st_ContactEntry.sLastName, strlen(lastName)-1 < LIST_NAME_MAX_LEN ? strlen(lastName)-1 : LIST_NAME_MAX_LEN, "~%s", lastName);
#endif
					strcpy(st_ContactEntry.sNumber1, number);
					st_ContactEntry.cNumber1Type = 2;
					st_ContactEntry.bNumber1Default = 1;
					st_ContactEntry.bNumber1Own = 1;
					st_ContactEntry.u32_AssociatedMelody = 0;
					st_ContactEntry.u32_LineId = voipline->LineID;

					List_InsertEntry(LIST_TYPE_CONTACT_LIST, &st_ContactEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

					sprintf(st_ContactEntry.sNumber1, "**%d#", u32_EntryId);

					List_UpdateEntry(LIST_TYPE_CONTACT_LIST, &st_ContactEntry, pu32_Fields, u32_FieldsNum, u32_EntryId);
					added++;
				}
				// Update username
				else if(u32_NumOfReqEntries)
				{
					u32 pu32_Fields[1], u32_FieldsNum = 1;
					pu32_Fields[0]  = FIELD_ID_LAST_NAME;

					u32_EntryId = ((stContactListEntry*)pu8_Entries)->u32_EntryId;

					if(isOnline)
						strcpy(st_ContactEntry.sLastName, lastName);
					else
						sprintf(st_ContactEntry.sLastName, "~%s", lastName);

					List_UpdateEntry(LIST_TYPE_CONTACT_LIST, &st_ContactEntry, pu32_Fields, u32_FieldsNum, u32_EntryId);
				}			
			}
			printf("\rSkype contacts sync: %d%%. Added contacts: %d", (((i+1)*100)/contactsCount), added);

			cmbsevent_onSyncContacts((((i+1)*100)/contactsCount));

		}
		printf("\n");

		// send list change notification
		ListChangeNotif_ListChanged( 0, LINE_TYPE_RELATING_TO, 0xFF, contactsCount, LIST_CHANGE_NOTIF_LIST_TYPE_CONTACT_LIST );		

		// dump list
		// extvoip_DumpContacts();
}

void extvoip_DumpContacts()
{
	stContactListEntry u8_Entry;
	u32 u32_Count, u32_Index, pu32_Fields[FIELD_ID_MAX], u32_FieldsSize, u32_SortField, u32_SortField2 = FIELD_ID_INVALID, u32_NumOfEntries = 1;

	printf("--------------------------------------------\n");
	printf("Skype contacts table\n");
	printf("--------------------------------------------\n");

	pu32_Fields[0] = FIELD_ID_ENTRY_ID;
	pu32_Fields[1] = FIELD_ID_LAST_NAME;
	pu32_Fields[2] = FIELD_ID_FIRST_NAME;
	u32_FieldsSize = 3;

	u32_SortField   = FIELD_ID_LAST_NAME;
	u32_SortField2  = FIELD_ID_FIRST_NAME;
	memset(&u8_Entry,0,sizeof(u8_Entry));
	/* Create if not exist */
	List_CreateList(LIST_TYPE_CONTACT_LIST);

	List_GetCount(LIST_TYPE_CONTACT_LIST, &u32_Count);
	printf("Total Num Of Entries = %d\n", u32_Count);

	for ( u32_Index = 0; u32_Index < u32_Count; ++u32_Index )
	{
		u32 u32_StartIdx = u32_Index + 1;
		List_ReadEntries(LIST_TYPE_CONTACT_LIST, &u32_StartIdx, TRUE, MARK_LEAVE_UNCHANGED, pu32_Fields, u32_FieldsSize,
		u32_SortField, u32_SortField2, &u8_Entry, &u32_NumOfEntries);

		printf("**%d#\t%s (%s)\n", u8_Entry.u32_EntryId, u8_Entry.sLastName, u8_Entry.sFirstName);
	}
}

/*
* HELPER:
* Update Line data in LineSettingsList
*/
void _extvoip_updateLineSettingsList(int lineID, char* LineName)
{
	u32 pu32_Fields[5], u32_FieldsNum = 5;
	stLineSettingsListEntry  st_LineSettingsListEntry;		
	u32 u32_EntryId = LineSettingsGetEntryIdByLineId(lineID);
	memset(&st_LineSettingsListEntry,   0, sizeof(st_LineSettingsListEntry));

	/* Create if not exist */
	List_CreateList(LIST_TYPE_LINE_SETTINGS_LIST);
	strcpy(st_LineSettingsListEntry.sLineName, LineName);
	st_LineSettingsListEntry.u32_LineId = lineID;
	st_LineSettingsListEntry.u32_AttachedHsMask = CMBS_U16_SUBSCRIBED_HS_MASK;
	st_LineSettingsListEntry.sDialPrefix[0] = 0;
	st_LineSettingsListEntry.bMultiCalls = 1;

	pu32_Fields[0]  = FIELD_ID_LINE_NAME;
	pu32_Fields[1]  = FIELD_ID_LINE_ID;
	pu32_Fields[2]  = FIELD_ID_ATTACHED_HANDSETS;
	pu32_Fields[3]  = FIELD_ID_DIALING_PREFIX;
	pu32_Fields[4]  = FIELD_ID_MULTIPLE_CALLS_MODE;

	if(u32_EntryId)
		List_UpdateEntry(LIST_TYPE_LINE_SETTINGS_LIST, &st_LineSettingsListEntry, pu32_Fields, u32_FieldsNum, u32_EntryId);
	else
		List_InsertEntry(LIST_TYPE_LINE_SETTINGS_LIST, &st_LineSettingsListEntry, pu32_Fields, u32_FieldsNum, &u32_EntryId);

	/* Need to update target with Line Settings */
	{
		ST_IE_LINE_SETTINGS_LIST st_LineSettingsList;

		st_LineSettingsList.u16_Attached_HS     = (u16)st_LineSettingsListEntry.u32_AttachedHsMask;
		st_LineSettingsList.u8_Call_Intrusion   = st_LineSettingsListEntry.bIntrusionCall;
		st_LineSettingsList.u8_Line_Id          = (u8)st_LineSettingsListEntry.u32_LineId;
		st_LineSettingsList.u8_Multiple_Calls   = st_LineSettingsListEntry.bMultiCalls ? 4 : 0;
        st_LineSettingsList.e_LineType          = CMBS_LINE_TYPE_VOIP_PARALLEL_CALL;

		app_SrvLineSettingsSet(&st_LineSettingsList, 1);
	}
}

/*
* HELPER:
* Convert cmbs audio codec to voip codec
*/
EXTVOIP_CODEC cmbsCodec2VoipCodec(E_CMBS_AUDIO_CODEC codec)
{
	switch(codec)
	{
	case CMBS_AUDIO_CODEC_PCMU:
		return EXTVOIP_CODEC_PCMU;
	case CMBS_AUDIO_CODEC_PCMA:
		return EXTVOIP_CODEC_PCMA;
	case CMBS_AUDIO_CODEC_PCMU_WB:
		return EXTVOIP_CODEC_PCMU_WB;
	case CMBS_AUDIO_CODEC_PCMA_WB:
		return EXTVOIP_CODEC_PCMA_WB;
	case CMBS_AUDIO_CODEC_PCM_LINEAR_WB:
		return EXTVOIP_CODEC_PCM_LINEAR_WB;
	case CMBS_AUDIO_CODEC_PCM_LINEAR_NB:
		return EXTVOIP_CODEC_PCM_LINEAR_NB;
	case CMBS_AUDIO_CODEC_PCM8:
		return EXTVOIP_CODEC_PCM8;
	default:
		break;
	}
	return EXTVOIP_CODEC_UNKNOWN;
}

/*
* HELPER:
* Convert voip audio codec to cmbs codec
*/
E_CMBS_AUDIO_CODEC voipCodec2CmbsCodec(EXTVOIP_CODEC codec)
{
	switch(codec)
	{
	case EXTVOIP_CODEC_PCMU:
		return CMBS_AUDIO_CODEC_PCMU;
	case EXTVOIP_CODEC_PCMA:
		return CMBS_AUDIO_CODEC_PCMA;
	case EXTVOIP_CODEC_PCMU_WB:
		return CMBS_AUDIO_CODEC_PCMU_WB;
	case EXTVOIP_CODEC_PCMA_WB:
		return CMBS_AUDIO_CODEC_PCMA_WB;
	case EXTVOIP_CODEC_PCM_LINEAR_WB:
		return CMBS_AUDIO_CODEC_PCM_LINEAR_WB;
	case EXTVOIP_CODEC_PCM_LINEAR_NB:
		return CMBS_AUDIO_CODEC_PCM_LINEAR_NB;
	case EXTVOIP_CODEC_PCM8:
		return CMBS_AUDIO_CODEC_PCM8;
	default:
		break;
	}
	return CMBS_AUDIO_CODEC_UNDEF;
}

/*
* HELPER:
* Convert voip release reason to cmbs release reason
*/
E_CMBS_REL_REASON voipReleaseReason2CmbsReleaseReason(EXTVOIP_RELEASE_REASON val)
{
	switch(val)
	{
	case EXTVOIP_REASON_NORMAL:
		return CMBS_REL_REASON_NORMAL;
	case EXTVOIP_REASON_ABNORMAL:
		return CMBS_REL_REASON_ABNORMAL;
	case EXTVOIP_REASON_BUSY:
		return CMBS_REL_REASON_USER_BUSY;
	case EXTVOIP_REASON_UNKNOWN_NUMBER:
		return CMBS_REL_REASON_USER_UNKNOWN;
	case EXTVOIP_REASON_FORBIDDEN:
		return CMBS_REL_REASON_NETWORK_FORBIDDEN;
	case EXTVOIP_REASON_UNSUPPORTED_MEDIA:
		return CMBS_REL_REASON_UNSUPPORTED_MEDIA;
	case EXTVOIP_REASON_NO_RESOURCE:
		return CMBS_REL_REASON_INSUFFICIENT_RESOURCES;
	case EXTVOIP_REASON_REDIRECT:
		return CMBS_REL_REASON_REDIRECT;
	default:
		break;
	}
	return CMBS_REL_REASON_NORMAL;
}

