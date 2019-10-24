/*!
*  \file       appsuota.c
*	\brief		handles CAT-iq suota functionality
*	\Author		stein
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
#include "cfr_debug.h"

#ifdef WIN32
//defined CMBS_API to satisfy gmep-us.h
#define CMBS_API
#endif

#include "gmep-us.h"

// CMBS remove
#if ! defined ( WIN32 )
extern int  appRdFrmGmepdFd;
#endif

#ifdef WIN32

#ifdef CMBS_SUOTA_DEBUG
#define SUOTA_INFO APP_SUOTA_INFO_PRINT
#else
#define SUOTA_INFO APP_SUOTA_INFO_PRINT
#endif

#define SUOTA_ERROR APP_SUOTA_ERROR_PRINT


#define FACILITY_PAYLOAD_SIZE   52


typedef enum
{
    CMBS_URL_SENDING_STATE_IDLE,
    CMBS_URL_SENDING_STATE_IN_PROGRESS,
        
    CMBS_URL_SENDING_STATE_MAX
}e_URLSendState;
    

typedef enum
{
    SUOTA_STATE_IDLE,
    SUOTA_STATE_PUSH_MODE_SEND,
    SUOTA_STATE_HS_VER_IND_RX,
    SUOTA_STATE_HS_VER_AVAIL_TX,
    SUOTA_STATE_URL_TX,
    SUOTA_STATE_URL_RX,
    SUOTA_STATE_GMEP_SESSION_OPEN
} t_SuotaState;

typedef struct
{
    t_SuotaState	suotaState;
    u8			noUrl2;
    char inputchar;
} t_st_SuotaSt;

t_st_SuotaSt	g_SuotaInfo;


typedef struct
{
    u16 hsno;
    u8 furl_total;
    u8 furl_sent;
    u8 furl_link[3][40];
    u8 furl_linkLen[3];
}t_st_CplaneUrlInfo;
t_st_CplaneUrlInfo UrlInfo;


typedef struct
{
    t_st_GmepSuotaHsVerAvail hsVerAvail;
    t_st_GmepSuotaUrlInd hsVerUrl;
}t_st_SuotaMgmtConf;
t_st_SuotaMgmtConf gSuotaMgmtConf;

void  Cplane_FillurlInfo(u16 handsetno, int noUrl2);


int suota_ReadConfig(char *pBuffer)
{
    FILE *fp;
    int index = 0, rc = 0, i = 0;

    fp = fopen("suota_mgmt.cfg", "r");

    if ( fp == NULL )
    {
        SUOTA_ERROR(" SUOTA: Error:!!! Can not open /etc/suota_mgmt.cfg ... \n");
        return -1;
    }

    for (index = 0; ((rc = fgetc(fp)) != EOF); ++index)
    {
        if ( rc == '#' )
        { // skip the commented part . i.e. skip full line followed by "#"
            for (i = 0; ((rc = fgetc(fp)) != EOF); ++i)
            {
                if ( rc == '\n' )
                    break;
            }
        }
        if ( rc == '\n' || rc == '\r' )
            --index;
        else
            pBuffer[index] = rc;
    }
    fclose(fp);

    return 0;
}

int app_SoutaParseCfgFile(void)
{
    char buffer[2000];
    char *tempstr;

    memset(&gSuotaMgmtConf, 0, sizeof(t_st_SuotaMgmtConf));

    if ( -1 == suota_ReadConfig(buffer) )
        return -1;

    tempstr = strtok(buffer, ";");
    gSuotaMgmtConf.hsVerAvail.st_Header.u16_delayInMin = atoi(tempstr);

    tempstr = strtok(NULL, ";");
    gSuotaMgmtConf.hsVerAvail.st_Header.u8_URLStoFollow = atoi(tempstr);

    tempstr = strtok(NULL, ";");
    gSuotaMgmtConf.hsVerAvail.st_Header.u8_Spare = atoi(tempstr);

    tempstr = strtok(NULL, ";");
    gSuotaMgmtConf.hsVerAvail.st_Header.u8_UserInteraction = atoi(tempstr);

    tempstr = strtok(NULL, ";");
    strncpy((char *)gSuotaMgmtConf.hsVerAvail.st_SwVer.u8_Buffer, tempstr, sizeof(gSuotaMgmtConf.hsVerAvail.st_SwVer.u8_Buffer) - 1);

    tempstr = strtok(NULL, ";");
    gSuotaMgmtConf.hsVerUrl.u8_URLStoFollow = atoi(tempstr);

    tempstr = strtok(NULL, ";");
    strncpy((char *)gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL, tempstr, sizeof(gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL) - 1);

    SUOTA_INFO("\n SUOTA: CFg Mgmt Configuation: \n");
    SUOTA_INFO(" gSuotaMgmtConf.hsVerAvail.st_Header.u16_delayInMin - %d\n", gSuotaMgmtConf.hsVerAvail.st_Header.u16_delayInMin);
    SUOTA_INFO(" gSuotaMgmtConf.hsVerAvail.st_Header.u8_URLStoFollow- %d\n", gSuotaMgmtConf.hsVerAvail.st_Header.u8_URLStoFollow);
    SUOTA_INFO(" gSuotaMgmtConf.hsVerAvail.st_Header.u8_Spare -%d\n", gSuotaMgmtConf.hsVerAvail.st_Header.u8_Spare);
    SUOTA_INFO(" gSuotaMgmtConf.hsVerAvail.st_Header.u8_UserInteraction - %d\n", gSuotaMgmtConf.hsVerAvail.st_Header.u8_UserInteraction);
    SUOTA_INFO(" gSuotaMgmtConf.hsVerAvail.st_SwVer.u8_Buffer - %s, - %zu\n", gSuotaMgmtConf.hsVerAvail.st_SwVer.u8_Buffer, strlen((char *)gSuotaMgmtConf.hsVerAvail.st_SwVer.u8_Buffer));
    SUOTA_INFO(" gSuotaMgmtConf.hsVerUrl.u8_URLStoFollow - %u\n", gSuotaMgmtConf.hsVerUrl.u8_URLStoFollow);
    SUOTA_INFO(" gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL - %s, %zu\n", gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL, strlen((char *)gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL));

    return 0;
}


u8 getNumOfURLMsgs(char *pch_URL)
{
    u8 u8_URL_Len = strlen((char *)pch_URL);
    u8 u8_NumOfMsgs;
    u8_NumOfMsgs = u8_URL_Len / FACILITY_PAYLOAD_SIZE;
    if ( u8_URL_Len % FACILITY_PAYLOAD_SIZE )
    {
        ++u8_NumOfMsgs; // one more message for the residue
    }
    return u8_NumOfMsgs;
}


//sending HS version available comand to stub.
void cplane_SendHandVerAvail(t_st_GmepSuotaHsVerAvail *hsverAvail1, u16 hsno)
{

    ST_SUOTA_UPGRADE_DETAILS  st_HSVerAvail;
    u16 u16_RequestId;
    ST_VERSION_BUFFER st_SwVersion;

    SUOTA_INFO("SUOTA:cplane_SendHandVerAvail \n");
    g_SuotaInfo.suotaState = SUOTA_STATE_HS_VER_AVAIL_TX;

    st_HSVerAvail.u16_delayInMin = hsverAvail1->st_Header.u16_delayInMin;
    st_HSVerAvail.u8_URLStoFollow = getNumOfURLMsgs((char *)gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL);
    st_HSVerAvail.u8_UserInteraction = hsverAvail1->st_Header.u8_UserInteraction;
    st_HSVerAvail.u8_Spare = hsverAvail1->st_Header.u8_Spare;

    st_SwVersion.u8_VerLen = hsverAvail1->st_SwVer.u8_BuffLen;
    if ( hsverAvail1->st_SwVer.u8_BuffLen > CMBS_MAX_VERSION_LENGTH )
    {
        SUOTA_INFO("SUOTA:cplane_SendHandVerAvail Error - buf length 2 long \n");
    }
    else
        memcpy(st_SwVersion.pu8_VerBuffer, hsverAvail1->st_SwVer.u8_Buffer, hsverAvail1->st_SwVer.u8_BuffLen);

    u16_RequestId = 1;

    st_SwVersion.type = CMBS_SUOTA_SW_VERSION;

    cmbs_dsr_suota_SendHSVersionAvail(NULL, st_HSVerAvail, hsno, &st_SwVersion, u16_RequestId);

}

//sending Url indication command to stub
void cplane_SendUrl(t_st_GmepSuotaUrlInd  *url, u16 handset)
{
    ST_URL_BUFFER pst_Url;
    u16 u16_RequestId;

    SUOTA_INFO("cplane_SendUrl\n");

    pst_Url.u8_UrlLen = url->st_URL.u8_URLLen;
    if ( url->st_URL.u8_URLLen <= CMBS_MAX_URL_SIZE )
    {
        memcpy(pst_Url.pu8_UrlBuffer, url->st_URL.u8_URL, url->st_URL.u8_URLLen);
    }
    else
    {
        pst_Url.u8_UrlLen = 0;
    }

    u16_RequestId = 1;
    cmbs_dsr_suota_SendURL(NULL, handset, url->u8_URLStoFollow, &pst_Url, u16_RequestId);
}


//Receives negative acknowledgement from Gmep test stub
void cplane_processNegack(t_st_GmepSuotaNegAck *pdatanegack, u16 handsetno)
{
    UNUSED_PARAMETER(handsetno);
    SUOTA_INFO("SUOTA:negack_handsetno is %d\n", pdatanegack->hsNo);
    SUOTA_INFO("SUOTA:negack_failurereason %d\n", pdatanegack->rejReason);
}

// Recieves Handset Version Indication Request from Gmep test stub.
//compares HS current sw version with Three versions..
void cplane_ProcessHandVerInd(t_st_GmepSuotaHsVerInd  *hsVerInd1, u16 hsno)
{
    //int noUrl2 = 0;
    char temp1[100], temp2[100];
    g_SuotaInfo.noUrl2 = 0;

    SUOTA_INFO("SUOTA:Handset Version Indication Recieved\n");
    SUOTA_INFO("SUOTA:handsetno - %d \n", hsno);
    SUOTA_INFO("SUOTA:Handset Version Emc is %d\n", hsVerInd1->st_Header.u16_EMC);
    SUOTA_INFO("SUOTA:Handset Version urlstofollow is %d\n", hsVerInd1->st_Header.u8_URLStoFollow);
    SUOTA_INFO("SUOTA:Handset Version filenumber is %d\n", hsVerInd1->st_Header.u8_FileNum);
    SUOTA_INFO("SUOTA:Handset Version spare is %d\n", hsVerInd1->st_Header.u8_Spare);
    SUOTA_INFO("SUOTA:Handset Version flags is %d\n", hsVerInd1->st_Header.u8_Flags);
    SUOTA_INFO("SUOTA:Handset Version reasonis %d\n", hsVerInd1->st_Header.u8_Reason);
    SUOTA_INFO("SUOTA:handsetswverlength - %d \n", hsVerInd1->st_SwVer.u8_BuffLen);
    memcpy(temp1, hsVerInd1->st_SwVer.u8_Buffer, hsVerInd1->st_SwVer.u8_BuffLen);
    temp1[hsVerInd1->st_SwVer.u8_BuffLen] = '\0';
    SUOTA_INFO("SUOTA:suotahandsetswversion is %s \n", temp1);
    SUOTA_INFO("SUOTA:handsethwverlength - %d \n", hsVerInd1->st_HwVer.u8_BuffLen);
    memcpy(temp2, hsVerInd1->st_HwVer.u8_Buffer, hsVerInd1->st_HwVer.u8_BuffLen);
    temp2[hsVerInd1->st_HwVer.u8_BuffLen] = '\0';
    SUOTA_INFO("SUOTA:suotahandsethwversion is %s \n", temp2);

    if ( (!strncmp((char *)hsVerInd1->st_SwVer.u8_Buffer, "1.1", hsVerInd1->st_SwVer.u8_BuffLen))
         || (!strncmp((char *)hsVerInd1->st_SwVer.u8_Buffer, "1.2", hsVerInd1->st_SwVer.u8_BuffLen))
         || (!strncmp((char *)hsVerInd1->st_SwVer.u8_Buffer, "1.3", hsVerInd1->st_SwVer.u8_BuffLen))
         || (!strncmp((char *)hsVerInd1->st_SwVer.u8_Buffer, "1.4", hsVerInd1->st_SwVer.u8_BuffLen)) )
    {
        //if HS is already downloaded with latest version
        if ( !strncmp((char *)hsVerInd1->st_SwVer.u8_Buffer, "1.4", hsVerInd1->st_SwVer.u8_BuffLen) )

        {
            g_SuotaInfo.noUrl2 = 1;
        }
        //if previous file download failed
        if ( (hsVerInd1->st_Header.u8_Reason) != 0 )
        {
            g_SuotaInfo.noUrl2 = 1;
        }
        //Cplane_FillurlInfo(hsVerInd1->hsNo, noUrl2);
    }
// ASA. That's too early
    if(hsVerInd1->st_Header.u8_URLStoFollow == 0)
    {
        Cplane_FillurlInfo(hsno, g_SuotaInfo.noUrl2);
    }

}

void cplane_ProcessHandUrlInd(t_st_GmepSuotaUrlInd  *hsUrlInd1, u16 hsno)
{
    //int noUrl2 = 0;
    char temp1[100];
    g_SuotaInfo.noUrl2 = 0;

    SUOTA_INFO("SUOTA:URL Indication Recieved\n");
    SUOTA_INFO("SUOTA:handsetno - %d \n", hsno);
    SUOTA_INFO("SUOTA:urlstofollow is %d\n", hsUrlInd1->u8_URLStoFollow);
    memcpy(temp1, hsUrlInd1->st_URL.u8_URL, hsUrlInd1->st_URL.u8_URLLen);
    temp1[hsUrlInd1->st_URL.u8_URLLen] = '\0';
    SUOTA_INFO("SUOTA:URL is %s \n", temp1);
    // printf("cplane_ProcessHandUrlInd u8_URLStoFollow = %d  hsno = %d\n", hsUrlInd1->u8_URLStoFollow, hsno);
    if(hsUrlInd1->u8_URLStoFollow == 0)
    {
        // printf("cplane_ProcessHandUrlInd - call Cplane_FillurlInfo\n");
        Cplane_FillurlInfo(hsno, g_SuotaInfo.noUrl2);
    }
}

// Handset version available command is sent with latest version updated.Assuming each software version is associated with 3 files(urls).
void  Cplane_FillurlInfo(u16 handsetno, int noUrl2)
{
    UNUSED_PARAMETER(noUrl2);
    SUOTA_INFO("SUOTA:Cplane_FillurlInfo \n");
    UrlInfo.hsno = handsetno;
    if ( UrlInfo.hsno == handsetno )
    {
        // filling HS version available structure
        t_st_GmepSuotaHsVerAvail hsVerAvail;

        hsVerAvail.st_Header.u16_delayInMin = gSuotaMgmtConf.hsVerAvail.st_Header.u16_delayInMin;
        hsVerAvail.st_Header.u8_URLStoFollow = gSuotaMgmtConf.hsVerAvail.st_Header.u8_URLStoFollow;
        hsVerAvail.st_Header.u8_Spare = gSuotaMgmtConf.hsVerAvail.st_Header.u8_Spare;
        hsVerAvail.st_Header.u8_UserInteraction = gSuotaMgmtConf.hsVerAvail.st_Header.u8_UserInteraction;
        hsVerAvail.st_SwVer.u8_BuffLen = strlen((char *)gSuotaMgmtConf.hsVerAvail.st_SwVer.u8_Buffer);
        strncpy((char *)&hsVerAvail.st_SwVer.u8_Buffer, (char *)gSuotaMgmtConf.hsVerAvail.st_SwVer.u8_Buffer, hsVerAvail.st_SwVer.u8_BuffLen);
        cplane_SendHandVerAvail(&hsVerAvail, UrlInfo.hsno);

    }
}

//filling of url indication command

void cplane_FillUrlMsg(u16 hs)
{

    t_st_GmepSuotaUrlInd hsVerUrl;
    u16 handset = (u16)hs;
    static u8 u8_NumOfMsgs = 0;
    static u8 u8_Index = 0;
    static u8 u8_URL_Len = 0;
    static e_URLSendState e_SendingState = CMBS_URL_SENDING_STATE_IDLE;

    if ( e_SendingState == CMBS_URL_SENDING_STATE_IDLE )
    {
        // First time we enter - calculate the number of messages
        u8_URL_Len = strlen((char *)gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL);

        // split the URL into several FACILITY messages

        // calculate number of FACILITY messages needed
        u8_NumOfMsgs = getNumOfURLMsgs((char *)gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL);


        e_SendingState = CMBS_URL_SENDING_STATE_IN_PROGRESS;
    }

    if ( e_SendingState == CMBS_URL_SENDING_STATE_IN_PROGRESS )
    {
        if ( u8_NumOfMsgs == 0 )
        {
            e_SendingState = CMBS_URL_SENDING_STATE_IDLE;
            u8_NumOfMsgs = 0;
            u8_Index = 0;
            u8_URL_Len = 0;
            return;
        }

        // URL to follow
        hsVerUrl.u8_URLStoFollow = --u8_NumOfMsgs;

        // URL len
        hsVerUrl.st_URL.u8_URLLen = u8_URL_Len > FACILITY_PAYLOAD_SIZE ? FACILITY_PAYLOAD_SIZE : u8_URL_Len;
        u8_URL_Len -= hsVerUrl.st_URL.u8_URLLen;

        // URL data
        strncpy((char *)hsVerUrl.st_URL.u8_URL, (char *)&(gSuotaMgmtConf.hsVerUrl.st_URL.u8_URL[u8_Index]), hsVerUrl.st_URL.u8_URLLen);
        u8_Index += hsVerUrl.st_URL.u8_URLLen;

        // send
        cplane_SendUrl(&hsVerUrl, handset);
    }
}


void cplane_FacilityCB(u16 handset, u8 bFacilitySent)
{
    switch (g_SuotaInfo.suotaState)
    {
#if 0
    case SUOTA_STATE_PUSH_MODE_SEND:
        {
            SUOTA_INFO("\n SUOTA: FacilityCallback : SUOTA_STATE_PUSH_MODE_SEND");
            SUOTA_INFO("\n Handset Recieved Facility %d",handset);
            //prakash; debug purpose
            SUOTA_INFO(" SUOTA:cplane_ProcessHandVerInd returned  $$$$ \n ");
            g_SuotaInfo.suotaState=SUOTA_STATE_HS_VER_AVAIL_TX;
        }
#endif
    case SUOTA_STATE_HS_VER_AVAIL_TX:
        {
            if ( 1 == bFacilitySent )
            {
                SUOTA_INFO("\n SUOTA: cplane_FacilityCB: SUOTA_STATE_HS_VER_AVAIL_TX");
                g_SuotaInfo.suotaState = SUOTA_STATE_URL_TX;
                cplane_FillUrlMsg(handset);
            }
        }
        break;
    case SUOTA_STATE_URL_TX:
        {
            if ( 1 == bFacilitySent )
            {
                g_SuotaInfo.suotaState = SUOTA_STATE_IDLE;
                SUOTA_INFO("\n SUOTA: cplane_FacilityCB: SUOTA_STATE_URL_TX");
            }
        }
        break;
    default:
        SUOTA_INFO("\n SUOTA: cplane_FacilityCB(): Invalid command ");
        break;
    }

}

/*c plane processing  call back function.*/
void  cplane_CmdRecvdCB(u16 Handsetno, t_GmepCmd suotaCommand, void *pData)
{
    void *pdatarecvd;
    u16 hsno;
    hsno = Handsetno;

    switch (suotaCommand)
    {
    case GMEP_US_HS_VER_IND:
        {
            g_SuotaInfo.suotaState = SUOTA_STATE_HS_VER_IND_RX;
            pdatarecvd = (t_st_GmepSuotaHsVerInd *)pData;
            cplane_ProcessHandVerInd(pdatarecvd, hsno);
            // printf(" cplane_CmdRecvdCB - GMEP_US_HS_VER_IND - set HS_Vers_recv\n");

            break;
        }
    case GMEP_US_NACK:
        {
            pdatarecvd = (t_st_GmepSuotaNegAck *)pData;
            //SUOTA_INFO("\n");
            cplane_processNegack(pdatarecvd, hsno);
            break;
        }
    case GMEP_US_URL_IND:
        {

            // printf(" cplane_CmdRecvdCB - GMEP_US_URL_IND \n");
            g_SuotaInfo.suotaState = SUOTA_STATE_URL_RX;
            pdatarecvd = (t_st_GmepSuotaUrlInd *)pData;
            cplane_ProcessHandUrlInd(pdatarecvd, hsno);

            break;
        }
    default:
        SUOTA_INFO(" SUOTA:cplane_CmdRecvdCB: incorrect suota command\n");
        break;
    }
}



void suota_app(t_st_GmepAppMsgCmd *pGmepMsgResp)
{
    int i  = 0;


    SUOTA_INFO("SUOTA: Message type rxed - %d \n", pGmepMsgResp->cmd);
    switch (pGmepMsgResp->cmd)
    {
    case GMEP_US_HS_VER_IND:
        {
            t_st_GmepSuotaHsVerInd hsVerInd;
            u16 hsno;
            
            SUOTA_INFO(" SUOTA: suota_app:GMEP_US_HS_VER_IND\n");
            
            memset(&hsVerInd, 0, sizeof(hsVerInd));
            i = 0;
            memcpy(&hsno/*&hsVerInd.hsNo*/, &pGmepMsgResp->buf[i], 2);
            i = i + 2;
            memcpy(&hsVerInd.st_Header.u16_EMC, &pGmepMsgResp->buf[i], 2);
            i  = i + 2;
            hsVerInd.st_Header.u8_URLStoFollow = pGmepMsgResp->buf[i];
            i = i + 1;
            hsVerInd.st_Header.u8_FileNum = pGmepMsgResp->buf[i];
            i = i  + 1;
            hsVerInd.st_Header.u8_Spare = pGmepMsgResp->buf[i];
            i = i  + 1;
            hsVerInd.st_Header.u8_Flags = pGmepMsgResp->buf[i];
            i = i  + 1;
            hsVerInd.st_Header.u8_Reason = pGmepMsgResp->buf[i];
            i = i  + 1;
            hsVerInd.st_SwVer.u8_BuffLen = pGmepMsgResp->buf[i];
            i = i  + 1;
            hsVerInd.st_SwVer.u8_BuffLen = MIN(MAX_SW_VER_ID_SIZE, hsVerInd.st_SwVer.u8_BuffLen);
            memcpy(&hsVerInd.st_SwVer.u8_Buffer, &pGmepMsgResp->buf[i], hsVerInd.st_SwVer.u8_BuffLen);
            i = i + hsVerInd.st_SwVer.u8_BuffLen;
            hsVerInd.st_HwVer.u8_BuffLen = pGmepMsgResp->buf[i];
            i = i  + 1;
            hsVerInd.st_HwVer.u8_BuffLen = MIN(MAX_SW_VER_ID_SIZE, hsVerInd.st_HwVer.u8_BuffLen);
            memcpy(&hsVerInd.st_HwVer.u8_Buffer, &pGmepMsgResp->buf[i], hsVerInd.st_HwVer.u8_BuffLen);
            i = i + hsVerInd.st_HwVer.u8_BuffLen;

            //pGmepMsgResp->bufLen=i;
            //if(gmepSessionTable.pGmepCPlaneCmdsCb)
            //gmepSessionTable.pGmepCPlaneCmdsCb(SUOTA_HS_VER_IND, &hsVerInd);
            cplane_CmdRecvdCB(hsno, GMEP_US_HS_VER_IND, &hsVerInd);
            break;
        }
    case GMEP_US_URL_IND:
        {

            t_st_GmepSuotaUrlInd hsUrlInd;
            u16 hsno;
            i = 0;
            memcpy(&hsno, &pGmepMsgResp->buf[i], 2);
            i = i + 2;
            hsUrlInd.u8_URLStoFollow = pGmepMsgResp->buf[i];
            i = i + 1;
            hsUrlInd.st_URL.u8_URLLen = pGmepMsgResp->buf[i];
            i  = i  + 1;
            hsUrlInd.st_URL.u8_URLLen = MIN(MAX_URL_SIZE-1, hsUrlInd.st_URL.u8_URLLen);
            memcpy(&hsUrlInd.st_URL.u8_URL, &pGmepMsgResp->buf[i], hsUrlInd.st_URL.u8_URLLen);
            i  = i + hsUrlInd.st_URL.u8_URLLen;



            hsUrlInd.st_URL.u8_URL[hsUrlInd.st_URL.u8_URLLen] = 0; 
            SUOTA_INFO(" SUOTA: suota_app:GMEP_US_URL_IND %s\n", hsUrlInd.st_URL.u8_URL);

            // printf("GMEP_US_URL_IND - call cplane_CmdRecvdCB\n");

            //if(gmepSessionTable.pGmepCPlaneCmdsCb)
            //gmepSessionTable.pGmepCPlaneCmdsCb(SUOTA_URL_IND, &hsUrlInd);
            cplane_CmdRecvdCB(hsno, GMEP_US_URL_IND, &hsUrlInd);
            break;
        }
    case GMEP_US_NACK:
        {
            t_st_GmepSuotaNegAck hsNegAckInd;
            i = 0;
            memcpy(&hsNegAckInd.hsNo, &pGmepMsgResp->buf[i], 2);
            i = i + 2;
            hsNegAckInd.rejReason = pGmepMsgResp->buf[i];
            i = i + sizeof(t_Suota_RejectReason);
            //if(gmepSessionTable.pGmepCPlaneCmdsCb)
            //gmepSessionTable.pGmepCPlaneCmdsCb(SUOTA_NACK, &hsNegAckInd);
            cplane_CmdRecvdCB(hsNegAckInd.hsNo, GMEP_US_NACK, &hsNegAckInd);
            break;
        }
    case GMEP_US_FACILITY_CB:
        {
            u16 hsNo;
            u8 bFacilitySent;
            int i = 0;
            memcpy(&hsNo, &pGmepMsgResp->buf[i], 2);
            i = i + 2;
            memcpy(&bFacilitySent, &pGmepMsgResp->buf[i], 1);
            cplane_FacilityCB(hsNo, bFacilitySent);
            break;
        }


    case GMEP_US_HS_AVAIL_ACK:
        {
            int i = 0;
            u16 hsNo;
            memcpy(&hsNo, &pGmepMsgResp->buf[i], 2);
            SUOTA_INFO("SUOTA: Rcvd From Target GMEP_US_HS_AVAIL_ACK hsNo %d \n", hsNo);
            cplane_FillUrlMsg(hsNo);
        }
        break;

    case GMEP_US_URL_IND_ACK:
        {
            int i = 0;
            u16 hsNo;
            memcpy(&hsNo, &pGmepMsgResp->buf[i], 2);
            SUOTA_INFO("SUOTA:  case GMEP_US_URL_IND_ACK hsNo %d \n", hsNo);
            cplane_FillUrlMsg((u8)hsNo);
        }
        break;

    default:
        {
            break;
        }
    }
}

#endif

void app_SuotaDATAReceived(E_CMBS_EVENT_ID e_EventID, void * pv_EventData)
{
	u16                 u16_IE;
	void*               pv_IE;
	t_st_GmepAppMsgCmd 	gmepMsgCmd;
	u16 u16_Handset;
	u16 u16_RequestId;
	ST_IE_DATA			st_Data;
	
	memset(&gmepMsgCmd,0,sizeof(gmepMsgCmd));
	gmepMsgCmd.cmd = e_EventID;
	gmepMsgCmd.bufLen = 0;

	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while ( pv_IE )
	{
		switch ( u16_IE )
		{
		case  CMBS_IE_SUOTA_APP_ID:
			APP_SUOTA_INFO_PRINT("app_SuotaDATAReceived CMBS_IE_SUOTA_APP_ID \n");
			cmbs_api_ie_u32ValueGet(pv_IE, &gmepMsgCmd.appId, CMBS_IE_SUOTA_APP_ID);
			break;

		case  CMBS_IE_SUOTA_SESSION_ID:
			APP_SUOTA_INFO_PRINT("app_SuotaDATAReceived CMBS_IE_SUOTA_SESSION_ID \n");
			cmbs_api_ie_u32ValueGet(pv_IE, &gmepMsgCmd.appSessionId, CMBS_IE_SUOTA_SESSION_ID);
			break;

		case CMBS_IE_DATA:
			APP_SUOTA_INFO_PRINT("app_SuotaDATAReceived CMBS_IE_DATA \n");
			cmbs_api_ie_DataGet(pv_IE, &st_Data);
			gmepMsgCmd.bufLen = st_Data.u16_DataLen;
			memcpy(gmepMsgCmd.buf, st_Data.pu8_Data, gmepMsgCmd.bufLen);
			break;

		case  CMBS_IE_HANDSETS:
			cmbs_api_ie_HandsetsGet( pv_IE, &u16_Handset );
			memcpy(gmepMsgCmd.buf, &u16_Handset, 2);
			gmepMsgCmd.bufLen = 2;
			APP_SUOTA_INFO_PRINT("app_SuotaDATAReceived CMBS_IE_HANDSETS  hs %d \n",u16_Handset);
			break;

		case CMBS_IE_RESPONSE:
			break;
		
		case CMBS_IE_REQUEST_ID:
			cmbs_api_ie_RequestIdGet( pv_IE, &u16_RequestId );
			break;


		default:
			APP_SUOTA_WARNING_PRINT("app_SuotaDATAReceived Unexpected IE:%d\n \n", u16_IE);
		}

		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}
		
	APP_SUOTA_INFO_PRINT("app_SuotaDATAReceived e_EventId %d Send to suota application \n",e_EventID);

#ifdef WIN32
	suota_app (&gmepMsgCmd);
#else
	//msg_fifo_write(appRdFrmGmepdFd, (char *)&gmepMsgCmd, sizeof(gmepMsgCmd));
	write(appRdFrmGmepdFd, (char *)&gmepMsgCmd, sizeof(gmepMsgCmd));
#endif
}

void app_SuotaURLReceived(E_CMBS_EVENT_ID e_EventID, void * pv_EventData)
{
	// UNUSED_PARAMETER(e_EventID);
	// UNUSED_PARAMETER(pv_EventData);

	// APP_SUOTA_INFO_PRINT("appsuota.c app_SuotaURLReceived not implemented \n");

	u16                 u16_IE;
	void*               pv_IE;

	t_st_GmepAppMsgCmd 	gmepMsgCmd;
	u16 u16_Handset;
	u16 u16_RequestId;
    u8 u8_URLNum;
        
    ST_URL_BUFFER       st_URL;

	memset(&gmepMsgCmd,0,sizeof(gmepMsgCmd));
	gmepMsgCmd.cmd = e_EventID;
	gmepMsgCmd.bufLen = 0;

	APP_SUOTA_INFO_PRINT("app_SuotaUTLReceived\n");
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while ( pv_IE )
	{
		switch ( u16_IE )
		{
		  case  CMBS_IE_HANDSETS:
			  cmbs_api_ie_HandsetsGet( pv_IE, &u16_Handset );
			  memcpy(gmepMsgCmd.buf, &u16_Handset, 2);
			  gmepMsgCmd.bufLen = 2;
		  break;

		  case  CMBS_IE_NUM_OF_URLS:
			  cmbs_api_ie_NumOfUrlsGet(pv_IE,&u8_URLNum);

			  gmepMsgCmd.buf[gmepMsgCmd.bufLen] = u8_URLNum;
			  gmepMsgCmd.bufLen += 1;

			  break;

		  case  CMBS_IE_URL:
			  cmbs_api_ie_UrlGet(pv_IE,&st_URL);

			  gmepMsgCmd.buf[gmepMsgCmd.bufLen] = st_URL.u8_UrlLen;
			  gmepMsgCmd.bufLen += 1;
			  memcpy(&gmepMsgCmd.buf[gmepMsgCmd.bufLen], st_URL.pu8_UrlBuffer, st_URL.u8_UrlLen);
			  gmepMsgCmd.bufLen += st_URL.u8_UrlLen;
			  break;

		  case CMBS_IE_REQUEST_ID:
			  cmbs_api_ie_RequestIdGet( pv_IE, &u16_RequestId );
			  break;


		  case CMBS_IE_SUOTA_SESSION_ID:
			  cmbs_api_ie_u32ValueGet(pv_IE, &gmepMsgCmd.appSessionId, CMBS_IE_SUOTA_SESSION_ID);
			  break;

		  default:
			  APP_SUOTA_INFO_PRINT("appsuota.c not handled app_SuotaVersionReceived   IE:%d\n \n", u16_IE);
		
		}

		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	APP_SUOTA_INFO_PRINT("appsuota.c app_SuotaURLReceived e_EventId %d Send to suota application \n",e_EventID);

#ifdef WIN32
	suota_app (&gmepMsgCmd);
#else
	write(appRdFrmGmepdFd, (char *)&gmepMsgCmd, sizeof(gmepMsgCmd));
#endif
}

void app_SuotaVersionReceived(E_CMBS_EVENT_ID e_EventID, void * pv_EventData)
{
	u16                 u16_IE;
	void*               pv_IE;
	t_st_GmepAppMsgCmd 	gmepMsgCmd;
	u16 u16_Handset;
	u16 u16_RequestId;
	int first;

	ST_SUOTA_HS_VERSION_IND st_HSVerInd;
    ST_VERSION_BUFFER       st_SwVersion;
    ST_VERSION_BUFFER       st_HwVersion;
	first=1;

	memset(&gmepMsgCmd,0,sizeof(gmepMsgCmd));
	gmepMsgCmd.cmd = e_EventID;
	gmepMsgCmd.bufLen = 0;

	APP_SUOTA_INFO_PRINT("app_SuotaVersionReceived\n");
	cmbs_api_ie_GetFirst( pv_EventData, &pv_IE, &u16_IE );
	while ( pv_IE )
	{
		switch ( u16_IE )
		{
		case  CMBS_IE_HANDSETS:
			cmbs_api_ie_HandsetsGet( pv_IE, &u16_Handset );
			memcpy(gmepMsgCmd.buf, &u16_Handset, 2);
			gmepMsgCmd.bufLen = 2;
		break;

		case  CMBS_IE_HS_VERSION_DETAILS:
			cmbs_api_ie_VersionIndGet(pv_IE,&st_HSVerInd);

			memcpy(&gmepMsgCmd.buf[gmepMsgCmd.bufLen],&st_HSVerInd.u16_EMC, 2);
			gmepMsgCmd.bufLen += 2;

			gmepMsgCmd.buf[gmepMsgCmd.bufLen] = st_HSVerInd.u8_URLStoFollow;
			gmepMsgCmd.bufLen += 1;

			gmepMsgCmd.buf[gmepMsgCmd.bufLen] = st_HSVerInd.u8_FileNumber;
			gmepMsgCmd.bufLen += 1;
			
			gmepMsgCmd.bufLen += 1; // spare

			gmepMsgCmd.buf[gmepMsgCmd.bufLen] = st_HSVerInd.u8_Flags;
			gmepMsgCmd.bufLen += 1; 

			gmepMsgCmd.buf[gmepMsgCmd.bufLen] = st_HSVerInd.u8_Reason;
			gmepMsgCmd.bufLen += 1; 

			break;

		case  CMBS_IE_HS_VERSION_BUFFER:
			if( first )
			{
				cmbs_api_ie_VersionBufferGet( pv_IE, &st_SwVersion);
				gmepMsgCmd.buf[gmepMsgCmd.bufLen] = st_SwVersion.u8_VerLen;
				gmepMsgCmd.bufLen += 1; 
				memcpy(&gmepMsgCmd.buf[gmepMsgCmd.bufLen] , st_SwVersion.pu8_VerBuffer , st_SwVersion.u8_VerLen);
				gmepMsgCmd.bufLen +=  st_SwVersion.u8_VerLen;
				first=0;
			}
			else
			{
				cmbs_api_ie_VersionBufferGet( pv_IE, &st_HwVersion);
				gmepMsgCmd.buf[gmepMsgCmd.bufLen] = st_HwVersion.u8_VerLen;
				gmepMsgCmd.bufLen += 1; 
				memcpy(&gmepMsgCmd.buf[gmepMsgCmd.bufLen] , st_HwVersion.pu8_VerBuffer , st_HwVersion.u8_VerLen);
				gmepMsgCmd.bufLen +=  st_HwVersion.u8_VerLen;
			}
			
			break;

		case CMBS_IE_REQUEST_ID:
			cmbs_api_ie_RequestIdGet( pv_IE, &u16_RequestId );
			break;

	
		case CMBS_IE_SUOTA_SESSION_ID:
			cmbs_api_ie_u32ValueGet(pv_IE, &gmepMsgCmd.appSessionId, CMBS_IE_SUOTA_SESSION_ID);
			break;

		default:
			APP_SUOTA_INFO_PRINT("appsuota.c not handled app_SuotaVersionReceived   IE:%d\n \n", u16_IE);
		
		}

		cmbs_api_ie_GetNext( pv_EventData, &pv_IE, &u16_IE );
	}

	APP_SUOTA_INFO_PRINT("appsuota.c app_SuotaVersionReceived e_EventId %d Send to suota application \n",e_EventID);

#ifdef WIN32
	suota_app (&gmepMsgCmd);
#else
	write(appRdFrmGmepdFd, (char *)&gmepMsgCmd, sizeof(gmepMsgCmd));
#endif
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
int 		   app_SuotaEntity( void * pv_AppRef, E_CMBS_EVENT_ID e_EventID, void * pv_EventData )
{
	t_st_GmepAppMsgCmd 	gmepMsgCmd;
	ST_IE_DATA			st_Data;
	
	UNUSED_PARAMETER( pv_AppRef );
	UNUSED_PARAMETER( pv_EventData );

	memset( &gmepMsgCmd, 0, sizeof(t_st_GmepAppMsgCmd) );
	memset( &st_Data, 0, sizeof(ST_IE_DATA) );

	if ( e_EventID == CMBS_EV_DSR_SUOTA_HS_VERSION_RECEIVED )
	{
		app_SuotaVersionReceived(e_EventID, pv_EventData);
		return TRUE;
	}
	else if ( e_EventID ==  CMBS_EV_DSR_SUOTA_URL_RECEIVED )
	{
		app_SuotaURLReceived(e_EventID, pv_EventData);
		return TRUE;
	}
	else if ( e_EventID == CMBS_EV_DSR_SUOTA_DATA_RECV  ||
			  e_EventID == CMBS_EV_DSR_SUOTA_SESSION_CLOSE_ACK ||
			  e_EventID == CMBS_EV_DSR_SUOTA_DATA_SEND_ACK 	||
			  e_EventID == CMBS_EV_DSR_SUOTA_SESSION_CLOSE ||
			  e_EventID == CMBS_EV_DSR_SUOTA_SEND_VERS_AVAIL_RES || 
			  e_EventID == CMBS_EV_DSR_SUOTA_SEND_URL_RES        || 
			  e_EventID == CMBS_EV_DSR_SUOTA_SESSION_CREATE_ACK     )
	{
		app_SuotaDATAReceived(e_EventID, pv_EventData);
		return TRUE;
	}


	//printf("app_SuotaEntity e_EventId %d Was NOT handled here \n",e_EventID);
	return FALSE;
		
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_SoutaSendHSVersionAvail ( ST_SUOTA_UPGRADE_DETAILS  pHSVerAvail, u16 u16_Handset, ST_VERSION_BUFFER* pst_SwVersion, u16 u16_RequestId)
{
	return cmbs_dsr_suota_SendHSVersionAvail( g_cmbsappl.pv_CMBSRef,pHSVerAvail, u16_Handset, pst_SwVersion, u16_RequestId );
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_SoutaSendNewVersionInd ( u16 u16_Handset, E_SUOTA_SU_SubType enSubType, u16 u16_RequestId )
{
	return cmbs_dsr_suota_SendSWUpdateInd(g_cmbsappl.pv_CMBSRef,u16_Handset, enSubType, u16_RequestId);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_SoutaSendURL ( u16 u16_Handset, u8 u8_URLToFollow, ST_URL_BUFFER* pst_Url, u16 u16_RequestId )
{
	return cmbs_dsr_suota_SendURL(g_cmbsappl.pv_CMBSRef,u16_Handset, u8_URLToFollow, pst_Url, u16_RequestId);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_SoutaSendNack ( u16 u16_Handset, E_SUOTA_RejectReason RejectReason, u16 u16_RequestId )
{
	return cmbs_dsr_suota_SendNack(g_cmbsappl.pv_CMBSRef,u16_Handset,RejectReason, u16_RequestId);
}
//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_SoutaAbort ( u32 u32_SessionId )
{
	return cmbs_dsr_suota_Session_Close(g_cmbsappl.pv_CMBSRef,u32_SessionId);
}

//////////////////////////////////////////////////////////////////////////
E_CMBS_RC app_SoutaUnRegAppCB( )
{
    return cmbs_dsr_suota_Unreg_App_CB();
}

//*/
