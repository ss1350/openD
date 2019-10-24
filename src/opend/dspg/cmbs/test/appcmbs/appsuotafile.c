/*!
*  \file       appsuotafile.c
*	\brief		handles suota of local file
*	\Author		Effi
*
*	@(#)	%filespec: appsuota.c~13 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================\n
*	date			name		version	 action                                          \n
*	----------------------------------------------------------------------------\n
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if ! defined ( WIN32 )
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <signal.h>
#endif

#include "cmbs_api.h"
#include "cmbs_ie.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "cfr_debug.h"     // CMBS debug definition
#include "appsuotafile.h"     
#include "tcx_hostlog.h"

// some macro for the logger
#define LOG_SUOTA_FILE_INFO(format, ...) 					tcx_WriteLog(SUOTA_FILE_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define LOG_SUOTA_FILE_TRACE(format, ...) 					tcx_WriteLog(SUOTA_FILE_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define LOG_SUOTA_FILE_WARNING(format, ...) 				tcx_WriteLog(SUOTA_FILE_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define LOG_SUOTA_FILE_ERROR(format, ...) 					tcx_WriteLog(SUOTA_FILE_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define LOG_SUOTA_FILE_RT(format, ...) 						tcx_WriteLog(SUOTA_FILE_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define LOG_SUOTA_FILE_DATA(pu8_Buffer, u16_Length) 		tcx_WriteLogData(SUOTA_FILE_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)


// define it here so that keyb_han.c can be reused in both suota options
#ifdef WIN32
void keyb_SuotaThread(int stab)
{
	return;
}
#endif

// by default we are disabled so in linux the other suota app is working
// when running with han server, this will be enabled
bool g_bool_AppSuotaFileEnable = FALSE;

//Note: this is defined in gmep-us.h which is not needed for this proejct
#define MAX_GMEP_SDU_SIZE 750 

void app_SuotaParseIE(E_CMBS_EVENT_ID e_EventID, void * pv_EventData)
{
	u16                 u16_IE;
	void*               pv_IE;
	u16 u16_Handset;
	u16 u16_RequestId;
	ST_IE_DATA			st_Data;
	u32 appId;
	u32 sessionId;

	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	
	while ( pv_IE )
	{
		switch ( u16_IE )
		{
		case  CMBS_IE_SUOTA_APP_ID:
			cmbs_api_ie_u32ValueGet(pv_IE, &appId, CMBS_IE_SUOTA_APP_ID);
			LOG_SUOTA_FILE_INFO("app_SuotaDATAReceived CMBS_IE_SUOTA_APP_ID =  %d \n", appId);
			break;

		case  CMBS_IE_SUOTA_SESSION_ID:
			cmbs_api_ie_u32ValueGet(pv_IE, &sessionId, CMBS_IE_SUOTA_SESSION_ID);
			LOG_SUOTA_FILE_INFO("app_SuotaDATAReceived CMBS_IE_SUOTA_SESSION_ID =  %d \n", sessionId);
			break;

		case CMBS_IE_DATA:
			cmbs_api_ie_DataGet(pv_IE, &st_Data);
			LOG_SUOTA_FILE_INFO("app_SuotaDATAReceived CMBS_IE_DATA  len %d \n", st_Data.u16_DataLen);
			break;

		case  CMBS_IE_HANDSETS:
			cmbs_api_ie_HandsetsGet( pv_IE, &u16_Handset );
			LOG_SUOTA_FILE_INFO("app_SuotaDATAReceived CMBS_IE_HANDSETS  hs %d \n", u16_Handset);
			break;

		case CMBS_IE_RESPONSE:
			break;
		
		case CMBS_IE_REQUEST_ID:
			cmbs_api_ie_RequestIdGet( pv_IE, &u16_RequestId );
			LOG_SUOTA_FILE_INFO("app_SuotaDATAReceived CMBS_IE_REQUEST_ID = %d \n", u16_RequestId);
			break;


		default:
			LOG_SUOTA_FILE_WARNING("app_SuotaDATAReceived Unexpected IE:%d\n \n", u16_IE);
		}

		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}
		
	
}

void app_GetRange(char* ps8_HttpRequest, int* ps32_startRange, int* ps32_EndRange)
{
	char ps8_HttpRequestBuffer[1000];
	char* ps8_HttpRequestSubString;
	char* ps8_pEnd;
	const char ps8_RangePrefix[] = "Range: bytes=";
	strcpy(ps8_HttpRequestBuffer, ps8_HttpRequest);
	ps8_HttpRequestSubString = strstr(ps8_HttpRequestBuffer, ps8_RangePrefix);
	if (!ps8_HttpRequestSubString)
	{
		*ps32_startRange = 0;
		*ps32_EndRange = 0;
		return;
	}
	ps8_HttpRequestSubString = ps8_HttpRequestSubString + strlen(ps8_RangePrefix);

	//HttpRequestSubString is "low_val-high_val", i.e 0-59
	ps8_pEnd = strchr(ps8_HttpRequestSubString, '-');
	ps8_pEnd[0] = 0;

	//HttpRequestSubString is "low_val"
	*ps32_startRange = atoi(ps8_HttpRequestSubString);
	ps8_HttpRequestSubString = ps8_pEnd + 1;
	
	ps8_pEnd = strchr(ps8_HttpRequestSubString, '\n');
	ps8_pEnd[0] = 0;
	//HttpRequestSubString is "high_val"
	*ps32_EndRange = atoi(ps8_HttpRequestSubString);
}

void app_StrToLowerCase(char ps8_str[]) {
	int s32_index = 0;

	while (ps8_str[s32_index] != '\0') {
		ps8_str[s32_index] = tolower(ps8_str[s32_index]);

		s32_index++;
	}
}

u8 g_u8_GetFullPath = 1;

void app_GetFile(char* ps8_HttpRequest, char* ps8_OutFilename )
{
	char ps8_LowerCaseHttpRequestBuffer[1000];
	char* ps8_Localhost;
	const char *ps8_LocalFilePrefix = "localhost";
	const char *ps8_geturi = "get ";
	const char *ps8_gethost = "host: ";
	char* ps8_uri = NULL;
	char* ps8_host = NULL;
	char* ps8_FileName = NULL;
	u8 u8_UriLen = 0;
	u8 u8_HostLen = 0;
	u8 u8_FileNameLen = 0;
	u8 b_IsLocalFile = 0;
	u8 u8_Index = 0;

	if (ps8_OutFilename)
		*ps8_OutFilename = '\0';

	if ((ps8_HttpRequest == NULL) || (*ps8_HttpRequest == '\0') || (ps8_OutFilename == NULL))
	{
		return;
	}
		
	strcpy(ps8_LowerCaseHttpRequestBuffer, ps8_HttpRequest);
	app_StrToLowerCase(ps8_LowerCaseHttpRequestBuffer);

	/* Extract URI from GET request */
	ps8_uri = strstr(ps8_LowerCaseHttpRequestBuffer, ps8_geturi);
	if(ps8_uri)
	{
		/* goto start of URI */
		ps8_uri += strlen(ps8_geturi);
	}

	/* find length of URI, upto ' ' */
	while (ps8_uri && (ps8_uri[u8_UriLen] != '\0') && (ps8_uri[u8_UriLen] != ' '))
	{
		u8_UriLen++;
	}

	/* replace ' ' with '\0' */
	if (ps8_uri && u8_UriLen)
	{
		ps8_uri[u8_UriLen] = '\0';
	}


	/* Extract HOST from GET request */
	ps8_host = strstr(ps8_uri + u8_UriLen + 1, ps8_gethost);
	if(ps8_host)
	{
		/* goto start of Host */
		ps8_host += strlen(ps8_gethost);
	}

	/* find length of Host, upto ' ' */
	while (ps8_host && (ps8_host[u8_HostLen] != '\r') && (ps8_host[u8_HostLen] != '\n'))
	{
		u8_HostLen++;
	}

	/* replace '\r' with '\0' */
	if (ps8_host && u8_HostLen)
	{
		ps8_host[u8_HostLen] = '\0';
	}


	/* look for a localhost in received host */
	ps8_Localhost = strstr(ps8_host, ps8_LocalFilePrefix);
	if(ps8_Localhost)
	{
		b_IsLocalFile = 1;
	}

	/* if not local file */
	if (!b_IsLocalFile)
	{	
		return;
	}


	if (g_u8_GetFullPath == 0)
	{
		/* Find only file name */
		while (ps8_uri[u8_HostLen] != '/')
		{
			u8_HostLen--;
		}
		ps8_FileName = &ps8_uri[u8_HostLen + 1];
		u8_FileNameLen = strlen(ps8_FileName);
	}
	else
	{
		/* Find file name with full path except host */

		/* uri starts with / relative skip / and give full path */
		if (ps8_uri[0] == '/')
		{
			ps8_FileName = &ps8_uri[1];
			u8_FileNameLen = strlen(ps8_FileName);
		}
		else
		{
			/* Find 'localhost' file name */
			ps8_uri = strstr(ps8_uri, ps8_LocalFilePrefix);

			if (ps8_uri)
			{
				/* Find '/' after 'localhost' and get the fullpath/filename except '/' */
				while (ps8_uri[u8_Index] != '/')
				{
					u8_Index++;
				}
				ps8_FileName = &ps8_uri[u8_Index + 1];
				u8_FileNameLen = strlen(ps8_FileName);
			}
			else
			{
				/* not local file */
				return;
			}
		}
	}

	/* Copy filename from received buffer, ps8_FileName is in lowercase */
	strncpy(ps8_OutFilename, &ps8_HttpRequest[ps8_FileName - ps8_LowerCaseHttpRequestBuffer], u8_FileNameLen);
	ps8_OutFilename[u8_FileNameLen] = '\0';
}

void app_SuotaFileProcessDataReceive(E_CMBS_EVENT_ID e_EventID, void * pv_EventData)
{
	u16                 u16_IE;
	void*               pv_IE;
	ST_IE_DATA			st_Data;
	u32 u32_AppId;
	u32 u32_SessionId;
	char ps8_Filename[1000];
	int s32_StartRange, s32_EndRange;
	FILE *pFile = NULL;
	int s32_TotalSize;
	int s32_NumOfBytes = 0;
	char ps8_Payload[2000];
	char ps8_Data[2000];
	int ps8_LenOfHeader;
    size_t  size_t_Res = 0;

   memset(&st_Data, 0, sizeof(ST_IE_DATA));
      
	cmbs_api_ie_GetFirst(pv_EventData, &pv_IE, &u16_IE);
	while (pv_IE)
	{
		switch (u16_IE)
		{
		case  CMBS_IE_SUOTA_APP_ID:
			cmbs_api_ie_u32ValueGet(pv_IE, &u32_AppId, CMBS_IE_SUOTA_APP_ID);
			break;

		case  CMBS_IE_SUOTA_SESSION_ID:
			cmbs_api_ie_u32ValueGet(pv_IE, &u32_SessionId, CMBS_IE_SUOTA_SESSION_ID);
			break;

		case CMBS_IE_DATA:
			cmbs_api_ie_DataGet(pv_IE, &st_Data);
			break;
		}

		cmbs_api_ie_GetNext(pv_EventData, &pv_IE, &u16_IE);
	}

	// process the ps8_Data
	LOG_SUOTA_FILE_INFO("ps8_Data: %s", st_Data.pu8_Data);
	app_GetFile((char*)st_Data.pu8_Data, ps8_Filename);
	if (strlen(ps8_Filename) == 0)
	{
		LOG_SUOTA_FILE_ERROR("app_SuotaFileProcessps8_DataReceive: Not a Local File\n");
		return;
	}
	LOG_SUOTA_FILE_INFO("FIle name = %s\n", ps8_Filename);
	app_GetRange((char*)st_Data.pu8_Data, &s32_StartRange, &s32_EndRange);
	if (s32_StartRange == 0 && s32_EndRange == 0)
	{
		LOG_SUOTA_FILE_ERROR("app_SuotaFileProcessps8_DataReceive: Invalid range\n");
		return;
	}
	LOG_SUOTA_FILE_INFO("app_SuotaFileProcessps8_DataReceive: Range: start=%d end=%d\n", s32_StartRange, s32_EndRange);

	// if during stess test, using stsize_t_Ress file name (itcm_h.1 or itcm_h.2)
	pFile = fopen(ps8_Filename, "rb");
	if (pFile == NULL)
	{
		LOG_SUOTA_FILE_ERROR("app_SuotaFileProcessps8_DataReceive: fail open file, aborting...\n");
		return;
	}

	if (fseek(pFile, 0, SEEK_END) != 0)   // non-portable
		LOG_SUOTA_FILE_ERROR("app_SuotaFileProcessps8_DataReceive: fseek return ERROR\n");

	s32_TotalSize = ftell(pFile);
    if (s32_TotalSize > 0)
    {
		LOG_SUOTA_FILE_INFO("app_SuotaFileProcessps8_DataReceive: Total file size = %d", s32_TotalSize);

		// special case (due to error in the standard)
		/* if (s32_EndRange == 59)
		{
			s32_StartRange = 0;
			s32_EndRange = 60;
			s32_NumOfBytes = 60;
		}
		else */ if (s32_EndRange - s32_StartRange> MAX_GMEP_SDU_SIZE)
		{
			s32_EndRange = s32_StartRange + MAX_GMEP_SDU_SIZE;
		}
		if (s32_EndRange >= s32_TotalSize)
		{
			s32_EndRange = s32_TotalSize-1;
		}
		s32_NumOfBytes = s32_EndRange - s32_StartRange + 1;

		LOG_SUOTA_FILE_INFO("app_SuotaFileProcessps8_DataReceive: Num-of-bytes [%d] start-range [%d] end-range [%d]\n",s32_NumOfBytes,s32_StartRange,s32_EndRange);
		
		if (fseek(pFile, s32_StartRange, SEEK_SET) != 0)
	        LOG_SUOTA_FILE_ERROR("app_SuotaFileProcessps8_DataReceive: fseek return ERROR\n");

		size_t_Res = fread(ps8_Payload, 1, s32_NumOfBytes, pFile);    
	    if ( size_t_Res != s32_NumOfBytes )
	    {
			LOG_SUOTA_FILE_INFO("app_SuotaFileProcessps8_DataReceive: Read [%d] bytes from start range [%d]\n", s32_NumOfBytes, s32_StartRange);
	    }

		//build size_t_Response
		sprintf(ps8_Data, "HTTP/1.1 206 Partial Content\r\nContent-Length: %d\r\nContent-Range: bytes %d-%d/%d\r\nContent-Type: application/octet-stream\r\n\r\n", s32_NumOfBytes, s32_StartRange, s32_EndRange, s32_TotalSize);
		ps8_LenOfHeader=strlen(ps8_Data);
		memcpy(&ps8_Data[ps8_LenOfHeader], ps8_Payload, s32_NumOfBytes);
		
		LOG_SUOTA_FILE_INFO("app_SuotaFileProcessps8_DataReceive: Total size of response [%d]\n",ps8_LenOfHeader + s32_NumOfBytes);
		cmbs_dsr_suota_DataSend(g_cmbsappl.pv_CMBSRef, u32_AppId, u32_SessionId, ps8_Data, ps8_LenOfHeader + s32_NumOfBytes, 1);
    }
    else //s32_TotalSize <= 0
		LOG_SUOTA_FILE_ERROR("app_SuotaFileProcessps8_DataReceive: File is corrupted (file size <= 0)\n");
		 
    fclose(pFile);		 
}

/**
 * 
 * 
 * @brief
 *  Handles CMBS SUOTA messages -- translates them to the
 *  original format handled by the 823 - for "same" handling
 *  further the road.
 * @param pv_AppRef 
 * @param e_EventID 
 * @param pv_EventData 
 * 
 * @return int 
 */
int 	app_SuotaFileEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData )
{
	UNUSED_PARAMETER( pv_AppRef );
	UNUSED_PARAMETER( pv_EventData );

	// if we are not enable do nothing
	if (g_bool_AppSuotaFileEnable  == FALSE)
	{
		LOG_SUOTA_FILE_WARNING("app_SuotaFileEntity: Suota File is disable, do nothing!!!\n");
		return FALSE;
	}

	switch (e_EventID)
	{
		case CMBS_EV_DSR_SUOTA_SESSION_CREATE_ACK:
		{
			LOG_SUOTA_FILE_INFO("app_SuotaFileEntity: got CMBS_EV_DSR_SUOTA_SESSION_CREATE_ACK\n");
			app_SuotaParseIE(e_EventID, pv_EventData);
			return TRUE;
		}
		case CMBS_EV_DSR_SUOTA_SESSION_CLOSE_ACK:
		{
			LOG_SUOTA_FILE_INFO("app_SuotaFileEntity: got CMBS_EV_DSR_SUOTA_SESSION_CLOSE_ACK\n");
			app_SuotaParseIE(e_EventID, pv_EventData);
			return TRUE;
		}
		case CMBS_EV_DSR_SUOTA_DATA_RECV:
		{
			LOG_SUOTA_FILE_INFO("app_SuotaFileEntity: got CMBS_EV_DSR_SUOTA_DATA_RECV\n");
			app_SuotaParseIE(e_EventID, pv_EventData);
			app_SuotaFileProcessDataReceive(e_EventID, pv_EventData);
			return TRUE;
		}
		case CMBS_EV_DSR_SUOTA_DATA_SEND_ACK:
		{
			LOG_SUOTA_FILE_INFO("app_SuotaFileEntity: got CMBS_EV_DSR_SUOTA_DATA_SEND_ACK\n");
			app_SuotaParseIE(e_EventID, pv_EventData);
			return TRUE;
		}
		case CMBS_EV_DSR_SUOTA_SESSION_CLOSE:
		{
			LOG_SUOTA_FILE_INFO("app_SuotaFileEntity: got CMBS_EV_DSR_SUOTA_SESSION_CLOSE\n");
			app_SuotaParseIE(e_EventID, pv_EventData);
			return TRUE;
		}
    default:
        return FALSE;
	}
}


//////////////////////////////////////////////////////////////////////////
void app_SuotaFileEnable(bool b_Enable)
{
	g_bool_AppSuotaFileEnable  = b_Enable;
	LOG_SUOTA_FILE_INFO("app_SuotaFileEnable: enable =%d\n",g_bool_AppSuotaFileEnable);
}


