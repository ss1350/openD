/*!
*  \file       appmsgparser.c
*  \brief      This file contains implementation of message parser
*
*  \author     podolskiy
*
*  @(#)  %filespec: appmsgparser.c~---GIT-- %
*
*******************************************************************************
*  \par  History
*  \n==== History ============================================================\n
*  date        name     version   action                                          \n
*  ----------------------------------------------------------------------------
* 29-May-2015 tcmc_asa print only prop fields with content
* 05-May-2015 tcmc_asa  PR 14071: added new CMBS_REL_REASON prints
* 21-Mar-2015 tcmc_asa    added CMBS_IE_REG_ENABLE_TYPE and CMBS_IE_TONE_VOLUME
* 17-Oct-14 tcmc_asa  ---GIT-- added CMBS_PARAM_INL_DEL
* 27-Jan-14 tcmc_asa  ---GIT-- solve checksum failure issues 
* 13-Jan-2014 tcmc_asa -- GIT--  take checksum changes from 3.46.x to 3_x main (3.5x)
* 20-Dec-2013 tcmc_asa --GIT-- add checksum changes from 2.99.C
* 28_Nov-2013 tcmc_asa --GIT-- suppress print for CMBS_IE_CHECKSUM
* 19-Nov-2013 tcmc_asa --GIT--  added CMBS_IE_CHECKSUM_ERROR
* 12-Jun-2013 tcmc_asa - GIT - merge 2.99.x and 3.27.4 changes to 3.36
* 03-May-2013 tcmc_asa    23      added CMBS_PARAM_INBAND_COUNTRY
* 23-Jan-2013 tcmc_asa 15.1.7.1.2  added CMBS_PARAM_FP_CUSTOM_FEATURES
* 26-Oct-10    podolskiy    61       Initial creation                             \n
*******************************************************************************/

#if ! defined ( WIN32 )
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h> //we need <sys/select.h>; should be included in <sys/types.h> ???
#include <signal.h>
#include <sys/msg.h>
#include <errno.h>
#endif

#include "cmbs_api.h"
#include "cmbs_ie.h"
#include "cmbs_han.h"
#include "cmbs_han_ie.h"
#include "appcmbs.h"
#include "cmbs_dbg.h"
#include "appmsgparser.h"
#include "tcx_log.h"
#include "hanfun_protocol_defs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

static u8 g_msgparser_enabled = 1;
u16 u16_DeviceId;
u8  u8_HandsetId;
u8  u8_callId;

#define APPCMBS_LOG(...)	\
	{ \
		if((pOutput != NULL)&&(*pu32_Pos < u32_OutputSize))\
		{	\
			*pu32_Pos+=snprintf(pOutput+*pu32_Pos,u32_OutputSize -1  - *pu32_Pos,"    ");\
			if(*pu32_Pos < u32_OutputSize)\
				*pu32_Pos+=snprintf(pOutput+*pu32_Pos,u32_OutputSize -1  - *pu32_Pos,__VA_ARGS__);\
			else\
				printf(__VA_ARGS__);\
		}\
		else\
		{\
			printf(__VA_ARGS__);\
		}\
	}


#define APPCMBS_LOG_HEADER(...)	\
	{ \
		if((pOutput != NULL)&&(*pu32_Pos < u32_OutputSize))\
		{	\
			*pu32_Pos+=snprintf(pOutput+*pu32_Pos,u32_OutputSize -1  - *pu32_Pos,"  ");\
			if(*pu32_Pos < u32_OutputSize)\
				*pu32_Pos+=snprintf(pOutput+*pu32_Pos,u32_OutputSize -1  - *pu32_Pos,__VA_ARGS__);\
			else\
				printf(__VA_ARGS__);\
		}\
		else\
		{\
			printf(__VA_ARGS__);\
		}\
	}

#define CMBS_SAFE_GET_IE(funct,arg) if(funct(pv_IE,arg) != CMBS_RC_OK) return;
#define CMBS_SAFE_GET_IE_2_PARAMS(funct,arg1,arg2) if(funct(pv_IE,arg1,arg2) != CMBS_RC_OK) return;
#define CMBS_SAFE_GET_IE2(funct,arg, type) if(funct(pv_IE,arg, type) != CMBS_RC_OK) return;


#define CMBS_PARSE_DECL(x)	void app_##x##_print(char*pOutput,u32 u32_OutputSize, u32* pu32_Pos, void * pv_IE)
#define CMBS_PARSE_CASE(x)	case x: app_##x##_print(pOutput,u32_OutputSize,&u32_Pos,pv_IE); break;

#define CMBS_PRINT_RELEASE_REASON(x)	case x: {char *temp = #x; APPCMBS_LOG("%s",&temp[16]); break;}

char* app_GetAFEEndpointString(E_CMBS_AFE_CHANNEL e_Channel)
{
    switch (e_Channel)
    {
        caseretstr(CMBS_AFE_CHANNEL_SP_OUT);
        caseretstr(CMBS_AFE_CHANNEL_HS_SP_OUT);
        caseretstr(CMBS_AFE_CHANNEL_LINE0_OUT);
        caseretstr(CMBS_AFE_CHANNEL_MIC_IN);
        caseretstr(CMBS_AFE_CHANNEL_HS_MIC_IN);
        caseretstr(CMBS_AFE_CHANNEL_LINE1_IN);
        caseretstr(CMBS_AFE_CHANNEL_LINE0_IN);
        caseretstr(CMBS_AFE_CHANNEL_IN_SINGIN0);
        caseretstr(CMBS_AFE_CHANNEL_IN_SINGIN1);
        caseretstr(CMBS_AFE_ADC_IN_DIFFIN0);
        caseretstr(CMBS_AFE_ADC_IN_DIFFIN1);
        caseretstr(CMBS_AFE_ADC_IN_SINGIN0_MINUS_SINGIN1);
        caseretstr(CMBS_AFE_ADC_IN_VREF_OFFSET);
        caseretstr(CMBS_AFE_ADC_IN_MUTE_VREF);
        caseretstr(CMBS_AFE_ADC_IN_MUTE_FLOAT);
        caseretstr(CMBS_AFE_ADC_IN_DIGMIC1);
        caseretstr(CMBS_AFE_ADC_IN_DIGMIC2);
        caseretstr(CMBS_AFE_AMPOUT_IN_DAC0);
        caseretstr(CMBS_AFE_AMPOUT_IN_DAC1);
        caseretstr(CMBS_AFE_AMPOUT_IN_DAC0_INV);
        caseretstr(CMBS_AFE_AMPOUT_IN_DAC1_INV);
        caseretstr(CMBS_AFE_AMPOUT_IN_SINGIN);
        caseretstr(CMBS_AFE_AMPOUT_IN_DIFFIN0);
        caseretstr(CMBS_AFE_AMPOUT_IN_DIFFIN1);
        caseretstr(CMBS_AFE_AMPOUT_IN_MUTE);
        caseretstr(CMBS_AFE_AMP_OUT0);
        caseretstr(CMBS_AFE_AMP_OUT1);
        caseretstr(CMBS_AFE_AMP_OUT2);
        caseretstr(CMBS_AFE_AMP_OUT3);
        caseretstr(CMBS_AFE_AMP_SINGIN0);
        caseretstr(CMBS_AFE_AMP_SINGIN1);
        caseretstr(CMBS_AFE_AMP_DIFFIN0);
        caseretstr(CMBS_AFE_AMP_DIFFIN1);
        caseretstr(CMBS_AFE_ADC0);
        caseretstr(CMBS_AFE_ADC1);
        caseretstr(CMBS_AFE_ADC2);
        caseretstr(CMBS_AFE_MIC0);
        caseretstr(CMBS_AFE_MIC1);
    default:
        return (char *)"AFE channel not defined";
        break;
    }
}

char*  app_GetCodecString(E_CMBS_AUDIO_CODEC e_Codec)
{
    switch (e_Codec)
    {
        caseretstr(CMBS_AUDIO_CODEC_UNDEF);
        caseretstr(CMBS_AUDIO_CODEC_PCMU);
        caseretstr(CMBS_AUDIO_CODEC_PCMA);
        caseretstr(CMBS_AUDIO_CODEC_PCMU_WB);
        caseretstr(CMBS_AUDIO_CODEC_PCMA_WB);
        caseretstr(CMBS_AUDIO_CODEC_PCM_LINEAR_WB);
        caseretstr(CMBS_AUDIO_CODEC_PCM_LINEAR_NB);
        caseretstr(CMBS_AUDIO_CODEC_PCM8);
    default:
        return(char *)"Codec undefined";
    }
}

char* app_GetAFEGainString(E_CMBS_AFE_AMP_GAIN e_Gain)
{
    switch (e_Gain)
    {

        caseretstr(CMBS_AFE_IN_GAIN_MINUS_4DB);
        caseretstr(CMBS_AFE_IN_GAIN_MINUS_2DB);
        caseretstr(CMBS_AFE_IN_GAIN_0DB);
        caseretstr(CMBS_AFE_IN_GAIN_11KRIN);
        caseretstr(CMBS_AFE_IN_GAIN_2DB);
        caseretstr(CMBS_AFE_IN_GAIN_4DB);
        caseretstr(CMBS_AFE_IN_GAIN_6DB);
        caseretstr(CMBS_AFE_IN_GAIN_22_5KRIN);
        caseretstr(CMBS_AFE_IN_GAIN_8DB);
        caseretstr(CMBS_AFE_IN_GAIN_10DB);
        caseretstr(CMBS_AFE_IN_GAIN_12B);
        caseretstr(CMBS_AFE_IN_GAIN_45KRIN);
        caseretstr(CMBS_AFE_IN_GAIN_14DB);
        caseretstr(CMBS_AFE_IN_GAIN_16DB);
        caseretstr(CMBS_AFE_IN_GAIN_18DB);
        caseretstr(CMBS_AFE_IN_GAIN_90KRIN);
        caseretstr(CMBS_AFE_IN_GAIN_20DB);
        caseretstr(CMBS_AFE_IN_GAIN_22DB);
        caseretstr(CMBS_AFE_IN_GAIN_24DB);
        caseretstr(CMBS_AFE_IN_GAIN_180KRIN);
        caseretstr(CMBS_AFE_IN_GAIN_26DB);
        caseretstr(CMBS_AFE_IN_GAIN_28DB);
        caseretstr(CMBS_AFE_IN_GAIN_30DB);
        caseretstr(CMBS_AFE_IN_GAIN_360KRIN);
        caseretstr(CMBS_AFE_IN_GAIN_32DB);
        caseretstr(CMBS_AFE_IN_GAIN_34DB);
        caseretstr(CMBS_AFE_IN_GAIN_36DB);
        caseretstr(CMBS_AFE_IN_GAIN_720KRIN);
        caseretstr(CMBS_AFE_IN_GAIN_OLOOP1);
        caseretstr(CMBS_AFE_IN_GAIN_OLOOP2);
        caseretstr(CMBS_AFE_IN_GAIN_OLOOP3);
        caseretstr(CMBS_AFE_IN_GAIN_OLOOP4);
        caseretstr(CMBS_AFE_OUT_SING_GAIN_0DB);
        caseretstr(CMBS_AFE_OUT_SING_GAIN_MINUS_6DB);
        caseretstr(CMBS_AFE_OUT_SING_GAIN_MINUS_12DB);
        caseretstr(CMBS_AFE_OUT_SING_GAIN_MINUS_18DB);
        caseretstr(CMBS_AFE_OUT_SING_GAIN_MINUS_24DB);

    default:
        return(char *)"Gain undefined";
    }
}


char* app_GetAuxMuxInputString(E_CMBS_AUX_INPUT e_MuxInput)
{
    switch (e_MuxInput)
    {
        caseretstr(CMBS_AUX_MUXINPUT_VLED);
        caseretstr(CMBS_AUX_MUXINPUT_VCCA);
        caseretstr(CMBS_AUX_MUXINPUT_VIBAT);
        caseretstr(CMBS_AUX_MUXINPUT_VTEMP0);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN1_AMP);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN2_AMP);
        caseretstr(CMBS_AUX_MUXINPUT_VBGP);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN0);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN02);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN1);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN12);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN2);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN22);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN3);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN32);
        caseretstr(CMBS_AUX_MUXINPUT_VILED_ATT);
        caseretstr(CMBS_AUX_MUXINPUT_VDD);
        caseretstr(CMBS_AUX_MUXINPUT_VIBAT_ATT);
        caseretstr(CMBS_AUX_MUXINPUT_VTEMP0_DIFF);
        caseretstr(CMBS_AUX_MUXINPUT_VBG_12V);
        caseretstr(CMBS_AUX_MUXINPUT_VBG_REG_12V);
        caseretstr(CMBS_AUX_MUXINPUT_VBG_TEST_REGA);
        caseretstr(CMBS_AUX_MUXINPUT_PMU);
        caseretstr(CMBS_AUX_MUXINPUT_VTEMP_2_200_DIFF);
        caseretstr(CMBS_AUX_MUXINPUT_TOUCH_PANEL_INPUT);
        caseretstr(CMBS_AUX_MUXINPUT_VCC5V_5_DCDC);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN1_ATT);
        caseretstr(CMBS_AUX_MUXINPUT_DCIN2_ATT);
        caseretstr(CMBS_AUX_MUXINPUT_VTEMP_200_CONST);
        caseretstr(CMBS_AUX_MUXINPUT_TEST_LED_CTRL);
        caseretstr(CMBS_AUX_MUXINPUT_ISRC_AUX_1U_FROM_DCDC);
        caseretstr(CMBS_AUX_MUXINPUT_TEST_DCDC_CTRL);
        caseretstr(CMBS_AUX_MUXINPUT_1_2V_BANDGAP);
    default:
        return (char *)"Aux Input not defined";
        break;
    }
}

char* app_GetHWChipString(E_CMBS_HW_CHIP e_HWChip)
{
    switch (e_HWChip)
    {
        caseretstr(CMBS_HW_CHIP_VEGAONE);
        caseretstr(CMBS_HW_CHIP_DCX78);
        caseretstr(CMBS_HW_CHIP_DCX79);
        caseretstr(CMBS_HW_CHIP_DCX81);
        caseretstr(CMBS_HW_CHIP_DVF99);
    default:
        return (char *)"Chip type not defined";
        break;
    }
}

char* app_GetHWCOMString(E_CMBS_HW_COM_TYPE e_HWComType)
{
    switch (e_HWComType)
    {
        caseretstr(CMBS_HW_COM_TYPE_UART);
        caseretstr(CMBS_HW_COM_TYPE_USB);
        caseretstr(CMBS_HW_COM_TYPE_SPI0);
        caseretstr(CMBS_HW_COM_TYPE_SPI3);
    default:
        return (char *)"Com type not defined";
        break;
    }
}

char* app_GetHWChipVersionString(E_CMBS_HW_CHIP_VERSION e_HWChipVersion)
{
    switch (e_HWChipVersion)
    {
        caseretstr(CMBS_HW_CHIP_VERSION_B);
        caseretstr(CMBS_HW_CHIP_VERSION_C);
        caseretstr(CMBS_HW_CHIP_VERSION_D);
    default:
        return (char *)"chip version not defined";
        break;
    }
}

char* app_GetLineUseStatus(E_CMBS_LINE_DIAG_STATUS e_LineUseStatus)
{
    switch (e_LineUseStatus)
    {
        caseretstr(CMBS_LINE_DIAG_LINE_IDLE);
        caseretstr(CMBS_LINE_DIAG_LINE_IN_USE);
        caseretstr(CMBS_LINE_DIAG_LINE_SYSTEM_BUSY);
    default:
        return (char *)"Line Use status not defined";
        break;
    }
}

char* app_GetOKStatus(E_CMBS_LINE_DIAG_OK_STATUS e_OKStatus)
{
    switch (e_OKStatus)
    {
        caseretstr(CMBS_LINE_DIAG_OK_STATUS_DOWN);
        caseretstr(CMBS_LINE_DIAG_OK_STATUS_REGISTER);
        caseretstr(CMBS_LINE_DIAG_OK_STATUS_INITIALIZE);
        caseretstr(CMBS_LINE_DIAG_OK_STATUS_UP);
    default:
        return (char *)"OK status not defined";
        break;
    }
}

char* app_GetErrorType(E_CMBS_LINE_DIAG_ERROR_TYPE e_ErrorType)
{
    switch (e_ErrorType)
    {
        caseretstr(CMBS_LINE_DIAG_ERROR_UNKNOWN);
        caseretstr(CMBS_LINE_DIAG_NETWORK_ERROR);
        caseretstr(CMBS_LINE_DIAG_LOCAL_ERROR);
    default:
        return (char *)"Error not defined";
        break;
    }
}

char* app_GetLocalErrorNumber(E_CMBS_LINE_DIAG_LOCAL_ERROR_NUMBER e_LocalErrorNumber)
{
    switch (e_LocalErrorNumber)
    {
        caseretstr(CMBS_LINE_DIAG_LOCAL_ERROR_UNKNOWN);		
        caseretstr(CMBS_LINE_DIAG_LOCAL_LAN_ERROR);			
        caseretstr(CMBS_LINE_DIAG_LOCAL_LAN_2_ERROR);		
        caseretstr(CMBS_LINE_DIAG_LOCAL_LAN_3_ERROR);		
        caseretstr(CMBS_LINE_DIAG_LOCAL_CABLE_ERROR);		
        caseretstr(CMBS_LINE_DIAG_LOCAL_MODEM_HGW_ERROR);
    default:
        return (char *)"Local Error not defined";
        break;
    }
}

char* app_GetMediaString(E_CMBS_MEDIA e_Media)
{
    switch (e_Media)
    {
        caseretstr(E_CMBS_MEDIA_CLOSE);
        caseretstr(E_CMBS_MEDIA_PEND);
        caseretstr(E_CMBS_MEDIA_ACTIVE);

    default:
        return(char *)"Media State undefined";
    }
}

CMBS_PARSE_DECL(CMBS_IE_CALLINSTANCE)
{
    u32            u32_CallInstance = 0;
    CMBS_SAFE_GET_IE(cmbs_api_ie_CallInstanceGet, &u32_CallInstance);

    APPCMBS_LOG("CallInstance: %X", u32_CallInstance);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LINE_ID)
{
    u8             u8_LineId;
    cmbs_api_ie_LineIdGet(pv_IE, &u8_LineId);
    APPCMBS_LOG("LineId: %d", u8_LineId);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_MELODY)
{
    u8             u8_Melody;
    cmbs_api_ie_MelodyGet(pv_IE, &u8_Melody);
    APPCMBS_LOG("Melody: %d", u8_Melody);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLEDPARTY)
{
    ST_IE_CALLEDPARTY  st_CalledParty;
    int i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_CalledPartyGet, &st_CalledParty);

    APPCMBS_LOG("CalledParty[ prop=%d, pres=%d, len=%d, addr=",  st_CalledParty.u8_AddressProperties, st_CalledParty.u8_AddressPresentation, st_CalledParty.u8_AddressLen);
    for (i = 0; i < st_CalledParty.u8_AddressLen; ++i)
    {
        APPCMBS_LOG("%c", st_CalledParty.pu8_Address[i]);
    }
    APPCMBS_LOG(" ]");


}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLPROGRESS)
{
    ST_IE_CALLPROGRESS st_CallProgress;
    CMBS_SAFE_GET_IE(cmbs_api_ie_CallProgressGet, &st_CallProgress);

    APPCMBS_LOG("%s", cmbs_dbg_GetCallProgressName(st_CallProgress.e_Progress));
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLINFO)
{
    ST_IE_CALLINFO st_CallInfo;

    CMBS_SAFE_GET_IE(cmbs_api_ie_CallInfoGet, &st_CallInfo);

    APPCMBS_LOG(" CallInfo Type: ");

    switch (st_CallInfo.e_Type)
    {
    case CMBS_CALL_INFO_TYPE_DISPLAY:
        APPCMBS_LOG("DISPLAY"); break;
    case CMBS_CALL_INFO_TYPE_DIGIT:
        APPCMBS_LOG("DIGITS"); break;
    default:
        APPCMBS_LOG("UNKNOWN"); break;
    }

    if ( st_CallInfo.e_Type == CMBS_CALL_INFO_TYPE_DIGIT )
    {
        u8 i;

        APPCMBS_LOG(": \"");
        for (i = 0; i < st_CallInfo.u8_DataLen; i++)
        {
			if (st_CallInfo.pu8_Info[i] == 0x15)
			{
				APPCMBS_LOG("%c", 'R');
			}
			else
			{
				APPCMBS_LOG("%c", st_CallInfo.pu8_Info[i]);
			}
        }
        APPCMBS_LOG("\" Length:%2d", st_CallInfo.u8_DataLen);
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_MEDIACHANNEL)
{
    ST_IE_MEDIA_CHANNEL  st_MediaChannel;

    CMBS_SAFE_GET_IE(cmbs_api_ie_MediaChannelGet, &st_MediaChannel);

    APPCMBS_LOG(" Media Type: ");
    switch (st_MediaChannel.e_Type)
    {
    case  CMBS_MEDIA_TYPE_AUDIO_IOM:
        APPCMBS_LOG("AUDIO_IOM");  break;
    case  CMBS_MEDIA_TYPE_AUDIO_USB:
        APPCMBS_LOG("AUDIO_USB");  break;
    case  CMBS_MEDIA_TYPE_DATA:
        APPCMBS_LOG("DATA");       break;
    case  CMBS_MEDIA_TYPE_RTP:
        APPCMBS_LOG("RTP");  break;
    case  CMBS_MEDIA_TYPE_AUDIO_NODE:
        APPCMBS_LOG("AUDIO_NODE");  break;
    case  CMBS_MEDIA_TYPE_AUDIO_INTERN:
        APPCMBS_LOG("AUDIO_INTERN");  break;
        case  CMBS_MEDIA_TYPE_AUDIO_SHMEM:
            APPCMBS_LOG("AUDIO_SHMEM");  break;
    default:
        APPCMBS_LOG("UNKNOWN");  break;
    }

    APPCMBS_LOG(" Channel:%d ChannelParam:%x",
                st_MediaChannel.u32_ChannelID, st_MediaChannel.u32_ChannelParameter);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_MEDIADESCRIPTOR)
{
    ST_IE_MEDIA_DESCRIPTOR  st_MediaDesc;

    CMBS_SAFE_GET_IE(cmbs_api_ie_MediaDescGet, &st_MediaDesc);

    APPCMBS_LOG(" Media Descriptor: Codec: ");
    switch (st_MediaDesc.e_Codec)
    {
    case CMBS_AUDIO_CODEC_PCMU:
        APPCMBS_LOG("PCMU");     break;
    case CMBS_AUDIO_CODEC_PCMA:
        APPCMBS_LOG("PCMA");     break;
    case CMBS_AUDIO_CODEC_PCMU_WB:
        APPCMBS_LOG("PCMU_WB");  break;
    case CMBS_AUDIO_CODEC_PCMA_WB:
        APPCMBS_LOG("PCMA_WB");  break;
    case CMBS_AUDIO_CODEC_PCM_LINEAR_WB:
        APPCMBS_LOG("PCM_LINEAR_WB");  break;
    case CMBS_AUDIO_CODEC_PCM_LINEAR_NB:
        APPCMBS_LOG("PCM_LINEAR_NB");  break;
    case CMBS_AUDIO_CODEC_PCM8:
        APPCMBS_LOG("PCM8");     break;
    default:
        APPCMBS_LOG("UNKNOWN");
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLRELEASE_REASON)
{
    ST_IE_RELEASE_REASON st_Reason;

    CMBS_SAFE_GET_IE(cmbs_api_ie_CallReleaseReasonGet, &st_Reason);

    APPCMBS_LOG(" Reason: ");
    switch (st_Reason.e_Reason)
    {
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NORMAL)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_UNEXPECTED_MESSAGE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_UNKNOWN_TRANC_ID)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_MANDATORY_IE_MISSING)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_INVALID_IE_CONTENT)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_INCOMAPTIBLE_SERVICE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_SERVICE_NOT_IMPLEMENTED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NEGOTIATION_NOT_SUPPORTED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_INVALID_IDENTITY)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_AUTHENTICATION_FAILED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_UNKNOWN_IDENTITY)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NEGOTIATION_FAILED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_GENERAL_RESERVED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_TIMER_EXPIRY)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_PARTIAL_RELEASE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_UNKNOWN)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_DETACHED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_NOT_IN_RANGE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_UNKNOWN)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_ALREADY_ACTIVE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_BUSY)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_REJECTION)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_CALL_MODIFY)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_CALL_TRANSFER)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_USER_RESERVED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_EXT_HANDOVER_RESERVED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_EXT_HANDOVER_NOT_SUPPORTED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NETWORK_PARAMETER_MISSING)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_EXT_HANDOVER_RELEASE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NETWORK_NO_RESPONSE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NETWORK_ERROR)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NETWORK_FORBIDDEN)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_TEMP_OVERLOAD_RESERVED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_OVERLOAD)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_INSUFFICIENT_RESOURCES)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_INSUFFICIENT_BEARERS_AVAILABLE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_IWU_CONGESTION)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_SECURITY_ATTACK_ASSUMED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_ENCYPTION_ACTIVATION_FAILED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_RE_KEYING_FAILED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NO_CIPHER_KEY_AVL)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_LINE_IN_USE)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_NUMBER_NOT_AVL)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_SPLASH_RING_TIMEOUT)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_ABNORMAL)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_UNSUPPORTED_MEDIA)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_REDIRECT)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_CTRL_CODE_NOT_SUP)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_CTRL_CODE_FAILED)
    CMBS_PRINT_RELEASE_REASON(CMBS_REL_REASON_MAX)
    default:
        APPCMBS_LOG("UNKNOWN");
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_PARAMETER)
{
    ST_IE_PARAMETER st_Param;
    int i;

    CMBS_SAFE_GET_IE(cmbs_api_ie_ParameterGet, &st_Param);

    switch (st_Param.e_Param)
    {
    case CMBS_PARAM_RFPI:
        APPCMBS_LOG("RFPI");         break;
    case CMBS_PARAM_RVBG:
        APPCMBS_LOG("RVBG");         break;
    case CMBS_PARAM_RVREF:
        APPCMBS_LOG("RVREF");        break;
    case CMBS_PARAM_RXTUN:
        APPCMBS_LOG("RXTUN");        break;
    case CMBS_PARAM_MASTER_PIN:
        APPCMBS_LOG("MASTER_PIN");   break;
    case CMBS_PARAM_AUTH_PIN:
        APPCMBS_LOG("AUTH_PIN");     break;
    case CMBS_PARAM_COUNTRY:
        APPCMBS_LOG("COUNTY");       break;
    case CMBS_PARAM_SIGNALTONE_DEFAULT:
        APPCMBS_LOG("SIGNALTONE_DEFAULT");    break;
    case CMBS_PARAM_TEST_MODE:
        APPCMBS_LOG("TEST_MODE");    break;
    case CMBS_PARAM_ECO_MODE:
        APPCMBS_LOG("ECO_MODE");     break;
    case CMBS_PARAM_AUTO_REGISTER:
        APPCMBS_LOG("AUTO_REGISTER"); break;
    case CMBS_PARAM_NTP:
        APPCMBS_LOG("NTP");          break;
    case CMBS_PARAM_GFSK:
        APPCMBS_LOG("GFSK");         break;
    case CMBS_PARAM_RESET_ALL:
        APPCMBS_LOG("RESET_ALL");    break;
    case CMBS_PARAM_SUBS_DATA:
        APPCMBS_LOG("SUBSCRIPTION DATA"); break;
    case CMBS_PARAM_AUXBGPROG:
        APPCMBS_LOG("AUXBGPROG"); break;
    case CMBS_PARAM_ADC_MEASUREMENT:
        APPCMBS_LOG("ADC_MEASUREMENT"); break;
    case CMBS_PARAM_PMU_MEASUREMENT:
        APPCMBS_LOG("PMU_MEASUREMENT"); break;
    case CMBS_PARAM_RSSI_VALUE:
        APPCMBS_LOG("RSSI_VALUE"); break;
    case CMBS_PARAM_DECT_TYPE:
        APPCMBS_LOG("DECT_TYPE"); break;
    case CMBS_PARAM_MAX_NUM_ACT_CALLS_PT:
        APPCMBS_LOG("MAX_NUM_ACT_CALLS_PT"); break;
    case CMBS_PARAM_ANT_SWITCH_MASK:
        APPCMBS_LOG("ANT_SWITCH_MASK"); break;
    case CMBS_PARAM_PORBGCFG:
        APPCMBS_LOG("PORBGCFG"); break;
    case CMBS_PARAM_AUXBGPROG_DIRECT:
        APPCMBS_LOG("AUXBGPROG_DIRECT"); break;
    case CMBS_PARAM_BERFER_VALUE:
        APPCMBS_LOG("BERFER_VALUE"); break;
    case CMBS_PARAM_INBAND_COUNTRY:
        APPCMBS_LOG("INBAND_TONE_COUNTRY"); break;
    case CMBS_PARAM_FP_CUSTOM_FEATURES:
        APPCMBS_LOG("FP CUSTOM FEATURE");    break;
    case CMBS_PARAM_HAN_DECT_SUB_DB_START:
        APPCMBS_LOG("HAN DECT SUB START");    break;
    case CMBS_PARAM_HAN_DECT_SUB_DB_END:
        APPCMBS_LOG("HAN DECT SUB END");    break;
    case CMBS_PARAM_HAN_ULE_SUB_DB_START:
        APPCMBS_LOG("HAN ULE SUB START");    break;
    case CMBS_PARAM_HAN_ULE_SUB_DB_END:
        APPCMBS_LOG("HAN ULE SUB END");    break;
    case CMBS_PARAM_HAN_FUN_SUB_DB_START:
        APPCMBS_LOG("HAN FUN SUB START");    break;
    case CMBS_PARAM_HAN_FUN_SUB_DB_END:
        APPCMBS_LOG("HAN FUN SUB END");    break;
    case CMBS_PARAM_HAN_ULE_NEXT_TPUI:
        APPCMBS_LOG("HAN ULE NEXT TPUI");    break;
    case CMBS_PARAM_DHSG_ENABLE:
        APPCMBS_LOG("DHSG ENABLE");			break;
    case CMBS_PARAM_PREAM_NORM:
        APPCMBS_LOG("PREAM NORM");    			break;
    case CMBS_PARAM_RF_FULL_POWER:
        APPCMBS_LOG("RF FULL POWER");    		break;
    case CMBS_PARAM_RF_LOW_POWER:
        APPCMBS_LOG("RF LOW POWER");    		break;
    case CMBS_PARAM_RF_LOWEST_POWER:
        APPCMBS_LOG("RF LOWEST POWER");    	break;
    case CMBS_PARAM_RF19APU_MLSE:
        APPCMBS_LOG("RF19APU MLSE");    		break;
    case CMBS_PARAM_RF19APU_KCALOVR:
        APPCMBS_LOG("RF19APU KCALOVR");    	break;
    case CMBS_PARAM_RF19APU_KCALOVR_LINEAR:
        APPCMBS_LOG("RF19APU KCALOVR LINEAR");	break;

    case CMBS_PARAM_RF19APU_SUPPORT_FCC:
        APPCMBS_LOG("RF19APU Support FCC");		break;
    case CMBS_PARAM_RF19APU_DEVIATION:
        APPCMBS_LOG("RF19APU Deviation");			break;
    case CMBS_PARAM_RF19APU_PA2_COMP:
        APPCMBS_LOG("RF19APU PA2 compatibility");	break;
    case CMBS_PARAM_RFIC_SELECTION:
        APPCMBS_LOG("RFIC Selectionr");			break;
    case CMBS_PARAM_MAX_USABLE_RSSI:
        APPCMBS_LOG("MAX usable RSSI");			break;
    case CMBS_PARAM_LOWER_RSSI_LIMIT:
        APPCMBS_LOG("Lower RSSI Limit");			break;
    case CMBS_PARAM_PHS_SCAN_PARAM:
        APPCMBS_LOG("PHS Scan Parameter");			break;
    case CMBS_PARAM_JDECT_LEVEL1_M82:
        APPCMBS_LOG("L1 - minus 82 dBm RSSI threshold for Japan regulation");  break;
    case CMBS_PARAM_JDECT_LEVEL2_M62:
        APPCMBS_LOG("L2 - minus 62 dBm RSSI threshold for Japan regulation");	break;
    case CMBS_PARAM_AUXBGP_DCIN:
        APPCMBS_LOG("AUXILIARY BANDGAP DCIN");  break;
    case CMBS_PARAM_AUXBGP_RESISTOR_FACTOR:
        APPCMBS_LOG("AUXILIARY BANDGAP RESISTOR FACTOR");	break;
    case CMBS_PARAM_INL_DEL:
        APPCMBS_LOG("DELETE HS FROM INL");	break;
    case CMBS_PARAM_SYPO_GPIO:
        APPCMBS_LOG("SYPO GPIO number");	break;
    case CMBS_PARAM_SYPO_WAIT_FOR_SYNC:
        APPCMBS_LOG("SYPO wait for sync before DECT start");	break;
    case CMBS_PARAM_SYPO_MODE:
        APPCMBS_LOG("SYPO mode (Disabled SLAVE or MASTER)");	break;
    case CMBS_PARAM_UART_DELAY_TIMER:
        APPCMBS_LOG("UART delay timer");	break;
    case CMBS_PARAM_MAX_TRANSFER_SIZE:
        APPCMBS_LOG("Max CMBS tarnsfer size");	break;
    case CMBS_PARAM_RING_ON_OFF:
        APPCMBS_LOG("Ringing enable ");	break;
    case CMBS_PARAM_NEMO_MODE:
        APPCMBS_LOG("NEMO Mode ");	break;
    case CMBS_PARAM_HS_CW_DISABLED:
        APPCMBS_LOG("Handset Call Waiting Disable");	break;
       case CMBS_PARAM_INL_ADD:
        APPCMBS_LOG("ADD HS TO INL");	break;
    case CMBS_PARAM_BBD_UPDATE:
        APPCMBS_LOG("BBD Update");	break;
    case CMBS_PARAM_CLOCK_MASTER_EDIT:
        APPCMBS_LOG("Clock Master Editability"); break;
    case CMBS_PARAM_FXS_CALLEE_REGRET_TIME:
        APPCMBS_LOG("FXS Callee Regret Time ");	break;
    case CMBS_PARAM_FXS_FIRST_DIGIT_TIMER:
        APPCMBS_LOG("FXS First Digit Timer ");	break;
    case CMBS_PARAM_FXS_INTER_DIGIT_TIMER:
        APPCMBS_LOG("FXS Inter Digit Timer");	break;
   case CMBS_PARAM_FXS_STAR_HASH_CON_TIMER:
        APPCMBS_LOG("FXS Star Hash Convert Timer");	break;
    case CMBS_PARAM_PREP_QSPI_FOR_HW_RESET:
        APPCMBS_LOG("QSPI SST flash prepare for reset");	break;
    case CMBS_PARAM_FXS_TONE_CONFIG:
        APPCMBS_LOG("FXS Tone Config");	break;
    case CMBS_PARAM_ENC_DISABLE:
        APPCMBS_LOG("ENCRYPTION disable");	break;
    case CMBS_PARAM_SUBS_DATA_EX:
        APPCMBS_LOG("SUBS DATA EX DefCK"); break;
	case CMBS_PARAM_HAN_FUN_GROUP_LIST_START:
		APPCMBS_LOG("HAN FUN GROUP LIST START");	break;
	case CMBS_PARAM_HAN_FUN_GROUP_LIST_END:
		APPCMBS_LOG("HAN FUN GROUP LIST END");	break;
	case CMBS_PARAM_HAN_FUN_GROUP_TABLE_START:
		APPCMBS_LOG("HAN_FUN_GROUP TABLE START");	break;
	case CMBS_PARAM_HAN_FUN_GROUP_TABLE_END:
		APPCMBS_LOG("HAN_FUN_GROUP TABLE END");	break;
	case CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_START:
		APPCMBS_LOG("HAN ULE BROADCAST CONVERSION TABLE START");	break;
	case CMBS_PARAM_HAN_ULE_BROADCAST_CONVERSION_TABLE_END:
		APPCMBS_LOG("HAN ULE BROADCAST CONVERSION TABLE END");	break;
	case CMBS_PARAM_ULE_MULTICAST_ENC_PARAMS:
		APPCMBS_LOG("ULE MULTICAST ENC PARAMS"); break;		
    case CMBS_PARAM_NEMO_CONTROL:
        APPCMBS_LOG("NEMo Control");	break;
    case CMBS_PARAM_REPEATER_TYPE:
        APPCMBS_LOG("Repeater Type");	break;
    case CMBS_PARAM_REPEATER_SUBS_START:
        APPCMBS_LOG("REPEATER SUBS DATA START");	break;
     case CMBS_PARAM_REPEATER_SUBS_END:
        APPCMBS_LOG("REPEATER SUBS DATA END");	break;
   default:
        APPCMBS_LOG("UNKNOWN PARAMETER:%d\n", st_Param.e_Param);
    }

    if ( st_Param.u16_DataLen > 0 && st_Param.u16_DataLen < CMBS_PARAM_MAX_LENGTH )
    {
        APPCMBS_LOG(": ");
        for (i = 0; i < st_Param.u16_DataLen; i++) APPCMBS_LOG("%02X ", st_Param.pu8_Data[i]);
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_PARAMETER_AREA)
{
    ST_IE_PARAMETER_AREA st_ParamArea;
    int i;

    CMBS_SAFE_GET_IE(cmbs_api_ie_ParameterAreaGet, &st_ParamArea);

    APPCMBS_LOG("Param_Area=%d, Offset=%d, Length=%d\n", st_ParamArea.e_AreaType, st_ParamArea.u32_Offset, st_ParamArea.u16_DataLen);

    if ( st_ParamArea.u16_DataLen > 0 )
    {
        APPCMBS_LOG(", Data: ");
		
        for (i = 0; i < st_ParamArea.u16_DataLen; i++) 
		{
			if ((i==15) && (st_ParamArea.u16_DataLen>500))
				{
					APPCMBS_LOG("more...");
					break;
				}
			
			APPCMBS_LOG("%02X ", st_ParamArea.pu8_Data[i]);
        	}
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_FW_VERSION)
{
    ST_IE_FW_VERSION st_FwVersion;

    CMBS_SAFE_GET_IE(cmbs_api_ie_FwVersionGet, &st_FwVersion);

    switch (st_FwVersion.e_SwModule)
    {
    case CMBS_MODULE_CMBS:
        APPCMBS_LOG("CMBS Module ");     break;
    case CMBS_MODULE_DECT:
        APPCMBS_LOG("DECT Module ");     break;
    case CMBS_MODULE_DSP:
        APPCMBS_LOG("DSP Module ");      break;
    case CMBS_MODULE_EEPROM:
        APPCMBS_LOG("EEPROM Module ");   break;
    case CMBS_MODULE_USB:
        APPCMBS_LOG("USB Module ");      break;
    case CMBS_MODULE_BUILD:
        APPCMBS_LOG("Module build ");    break;
    case CMBS_MODULE_BOOTER:
        APPCMBS_LOG("Booter version ");  break;
    default:
        APPCMBS_LOG("UNKNOWN Module ");
    }
    APPCMBS_LOG("\nVER_%04x", st_FwVersion.u16_FwVersion);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_SYS_LOG)
{
    ST_IE_SYS_LOG st_SysLog;
    u8  u8_DataLen;
    u8 *pu8_Data;
    u8  i;
    u8  c;

    CMBS_SAFE_GET_IE(cmbs_api_ie_SysLogGet, &st_SysLog);

    u8_DataLen = st_SysLog.u8_DataLen;
    pu8_Data   = st_SysLog.u8_Data;

    if ( !u8_DataLen )
    {
        char psz_Text[] = "LogBuffer is empty\n";
        APPCMBS_LOG("%s", psz_Text);

        /*if ( appcmbs_get_CMBSAppInstance()->st_ApplSlot.pFnCbLogBuffer.pfn_cmbs_api_log_write_log_buffer_cb != NULL )
        {
            appcmbs_get_CMBSAppInstance()->st_ApplSlot.pFnCbLogBuffer.pfn_cmbs_api_log_write_log_buffer_cb((u8 *)psz_Text, sizeof(psz_Text) - 1);
        }*/
    }
    else
    {
        APPCMBS_LOG("LogBuffer:\n\n");
        for (i = 0; i < u8_DataLen; i++)
        {
            c = pu8_Data[i];
            if ( c >= 0x20 && c <= 0x7e )
            {APPCMBS_LOG("%c", c);		}
            else if ( c == 0x0a )
            {APPCMBS_LOG("\n");		}
            else
            {APPCMBS_LOG("(%02X)", c);	}
        }

        /*if ( appcmbs_get_CMBSAppInstance()->st_ApplSlot.pFnCbLogBuffer.pfn_cmbs_api_log_write_log_buffer_cb != NULL )
        {
            appcmbs_get_CMBSAppInstance()->st_ApplSlot.pFnCbLogBuffer.pfn_cmbs_api_log_write_log_buffer_cb(pu8_Data, (u16)u8_DataLen);
        }*/
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLSTATE)
{
    ST_IE_CALL_STATE st_CallState;
    CMBS_SAFE_GET_IE(cmbs_api_ie_CallStateGet, &st_CallState);

    APPCMBS_LOG("Call state: \n");
    APPCMBS_LOG("CallID = %d \n", st_CallState.u8_ActCallID);
    u8_callId = st_CallState.u8_ActCallID;
    APPCMBS_LOG("LineMask = 0x%02X \n", st_CallState.u8_LinesMask);
    APPCMBS_LOG("HSMask = 0x%04X\n", st_CallState.u16_HandsetsMask);
    APPCMBS_LOG("CallType = %s \n", cmbs_dbg_GetCallTypeName(st_CallState.e_CallType));
    APPCMBS_LOG("CallStatus = %s", cmbs_dbg_GetCallStateName(st_CallState.e_CallStatus));

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_UPDATED_DEVICE_STATE)
{
    ST_IE_UPDATED_DEVICE_STATE st_DeviceState;
    CMBS_SAFE_GET_IE(cmbs_api_ie_UpdatedDeviceStateGet, &st_DeviceState);

    APPCMBS_LOG("DeviceID = 0x%04X\n", st_DeviceState.u16_DeviceID);
    APPCMBS_LOG("CallStatus = %s", cmbs_dbg_GetCallStateName(st_DeviceState.e_CallStatus));

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_DEVICE_LIST_STATUS)
{
    ST_IE_DEVICE_LIST_STATUS st_DeviceListStatus;
    u8 i;
	
    CMBS_SAFE_GET_IE(cmbs_api_ie_DeviceListStatusGet, &st_DeviceListStatus);

    APPCMBS_LOG("Num Device = %d\n", st_DeviceListStatus.u8_NumDevices);
    APPCMBS_LOG("Device Ids = ");
    for (i = 0; i< st_DeviceListStatus.u8_NumDevices; i++)
    {
            APPCMBS_LOG("0x%04X, ", st_DeviceListStatus.u16_DeviceIds[i]);
    }
    APPCMBS_LOG("\n");
    APPCMBS_LOG("CallStatus = %s", cmbs_dbg_GetCallStateName(st_DeviceListStatus.e_CallStatus));

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_RESPONSE)
{
    ST_IE_RESPONSE st_Resp;

    CMBS_SAFE_GET_IE(cmbs_api_ie_ResponseGet, &st_Resp);

    switch (st_Resp.e_Response)
    {
       case CMBS_RESPONSE_OK:
         APPCMBS_LOG("Response: %s", "OK");
         break;
       case CMBS_RESPONSE_ERROR:
         APPCMBS_LOG("Response: %s", "ERROR");
         break;
       case CMBS_RESPONSE_FIFO_FULL:
         APPCMBS_LOG("Response: %s", "FIFO_FULL");
         break;
       case CMBS_RESPONSE_NO_LINK_AVL:
         APPCMBS_LOG("Response: %s", "NO_LINK_AVL");
         break;
	   default:
         APPCMBS_LOG("Response: %s", "Invalid response");
         break;
     }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_SUBSCRIBED_HS_LIST)
{
    ST_IE_SUBSCRIBED_HS_LIST st_SubscribedHsList;

    CMBS_SAFE_GET_IE(cmbs_api_ie_SubscribedHSListGet, &st_SubscribedHsList);

    APPCMBS_LOG("Handset: %d\n", st_SubscribedHsList.u16_HsID);
    APPCMBS_LOG("HS name: %s\n", st_SubscribedHsList.u8_HsName);
    APPCMBS_LOG("\n");
    APPCMBS_LOG("CAT-iq 1.0: %d\n", st_SubscribedHsList.u16_HsCapabilities & 0x2 ? 1 : 0);
    APPCMBS_LOG("CAT-iq 2.0: %d\n", st_SubscribedHsList.u16_HsCapabilities & 0x4 ? 1 : 0);
    APPCMBS_LOG("CAT-iq 2.1: %d\n", st_SubscribedHsList.u16_HsCapabilities & 0x8 ? 1 : 0);    
    APPCMBS_LOG("CAT-iq 3.0: %d\n", st_SubscribedHsList.u16_HsCapabilities & 0x20 ? 1 : 0);
#ifdef PNCAP
    APPCMBS_LOG("PNCAP-CAPB: %d\n\n", st_SubscribedHsList.u16_HsCapabilities & 0x800 ? 1 : 0);
#endif
    APPCMBS_LOG("OG Line Pref: %x\n", st_SubscribedHsList.u8_Pref_OutgoingCall_LineId );
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LINE_SETTINGS_LIST)
{
    ST_IE_LINE_SETTINGS_LIST st_LineSettingsList;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LineSettingsListGet, &st_LineSettingsList);

    APPCMBS_LOG("LineId = %d \n", st_LineSettingsList.u8_Line_Id);
    APPCMBS_LOG("Attached HS mask = 0x%X \n", st_LineSettingsList.u16_Attached_HS);
    APPCMBS_LOG("Call Intrusion = %d Multiple Calls = %d", st_LineSettingsList.u8_Call_Intrusion, st_LineSettingsList.u8_Multiple_Calls);
    APPCMBS_LOG("\nLine Type =");
    switch (st_LineSettingsList.e_LineType)
    {
    case CMBS_LINE_TYPE_PSTN_DOUBLE_CALL:
        APPCMBS_LOG("PSTN_DOUBLE_CALL");
        break;

    case CMBS_LINE_TYPE_PSTN_PARALLEL_CALL:
        APPCMBS_LOG("PSTN_PARALLEL_CALL");
        break;

    case CMBS_LINE_TYPE_VOIP_DOUBLE_CALL:
        APPCMBS_LOG("VOIP_DOUBLE_CALL");
        break;

    case CMBS_LINE_TYPE_VOIP_PARALLEL_CALL:
        APPCMBS_LOG("VOIP_PARALLEL_CALL");
        break;

    default:
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HANDSETINFO)
{
    ST_IE_HANDSETINFO st_HsInfo;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HandsetInfoGet, &st_HsInfo);

    if ( st_HsInfo.u8_State == CMBS_HS_REG_STATE_REGISTERED )
    {
        APPCMBS_LOG("Handset:%d IPUI:%02X%02X%02X%02X%02X Type:%s registered",
                    st_HsInfo.u8_Hs,
                    st_HsInfo.u8_IPEI[0], st_HsInfo.u8_IPEI[1], st_HsInfo.u8_IPEI[2],
                    st_HsInfo.u8_IPEI[3], st_HsInfo.u8_IPEI[4],
                    cmbs_dbg_GetHsTypeName(st_HsInfo.e_Type));
    }
    else if ( st_HsInfo.u8_State == CMBS_HS_REG_STATE_UNREGISTERED )
    {
        APPCMBS_LOG("Handset:%d Unregistered", st_HsInfo.u8_Hs);
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLERPARTY)
{
    ST_IE_CALLERPARTY st_CallerParty;
    int i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_CallerPartyGet, &st_CallerParty);

    APPCMBS_LOG("CALLER_ID address properties:0x%02x\n", st_CallerParty.u8_AddressProperties);
    APPCMBS_LOG("CallerID length:%d:", st_CallerParty.u8_AddressLen);
    for (i = 0; i < st_CallerParty.u8_AddressLen; i++) APPCMBS_LOG(" %c", st_CallerParty.pu8_Address[i]);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLERNAME)
{
    ST_IE_CALLERNAME st_CallerName;
    u8 u8_i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_CallerNameGet, &st_CallerName);

    APPCMBS_LOG("Caller Name: ");
    for (u8_i = 0; u8_i < st_CallerName.u8_DataLen; u8_i++) APPCMBS_LOG("%c", st_CallerName.pu8_Name[u8_i]);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_DISPLAY_STRING)
{
    ST_IE_DISPLAY_STRING st_DisplayString;
    u8 u8_i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_DisplayStringGet, &st_DisplayString);

    APPCMBS_LOG("Display string: ");
    for (u8_i = 0; u8_i < st_DisplayString.u8_DataLen; u8_i++) APPCMBS_LOG("%c", st_DisplayString.pu8_Info[u8_i]);

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_MEDIA_INTERNAL_CONNECT)
{
    ST_IE_MEDIA_INTERNAL_CONNECT st_MediaIC;
    CMBS_SAFE_GET_IE(cmbs_api_ie_MediaICGet, &st_MediaIC);

    APPCMBS_LOG("Media internal connect\n");
    APPCMBS_LOG("ChannelID: %d\n", st_MediaIC.u32_ChannelID);
    APPCMBS_LOG("Node id: %d\n", st_MediaIC.u32_NodeId);
    APPCMBS_LOG("Media Connection type: ")
    switch (st_MediaIC.e_Type)
    {
    case CMBS_MEDIA_IC_DISCONNECT:
        APPCMBS_LOG("CMBS_MEDIA_IC_DISCONNECT");
        break;
    case CMBS_MEDIA_IC_CONNECT:
        APPCMBS_LOG("CMBS_MEDIA_IC_CONNECT");
        break;
    default:
        APPCMBS_LOG("Unknown media type");
        break;
    }

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_TONE_VOLUME)
{
    u16 u16_Tone;
    CMBS_SAFE_GET_IE(cmbs_api_ie_ToneVolumeGet, &u16_Tone);

    APPCMBS_LOG("Tone Volume = %d", u16_Tone);
}

//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_TONE)
{
    ST_IE_TONE st_Tone;
    CMBS_SAFE_GET_IE(cmbs_api_ie_ToneGet, &st_Tone);

    APPCMBS_LOG("Tone Id = %d", st_Tone.e_Tone);
}

//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_REG_ENABLE_TYPE)
{
    /*E_CMBS_HS_REGISTER_ENABLE*/ u8 e_RegEnableType;
            
    CMBS_SAFE_GET_IE(cmbs_api_ie_RegEnableTypeGet, &e_RegEnableType);
    
    switch (e_RegEnableType)
    {
      case CMBS_HS_REG_ENABLE_ALL:
          APPCMBS_LOG("CMBS_HS_REG_ENABLE_ALL");
          break;
      case CMBS_HS_REG_ENABLE_HANDSET:
          APPCMBS_LOG("CMBS_HS_REG_ENABLE_HANDSET");
          break;
      case CMBS_HS_REG_ENABLE_ULE:
          APPCMBS_LOG("CMBS_HS_REG_ENABLE_ULE");
          break;
      default:
          APPCMBS_LOG("Invalid Register Enable Type 0x%x", e_RegEnableType);
          break;
    }
}

//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_PROP_FIELDS_LIST)
{
    ST_IE_PROP_FIELDS_LIST st_PropSettings;
    u8 u8_i, u8_Entry;
    CMBS_SAFE_GET_IE(cmbs_api_ie_PropDectSettingsListGet, &st_PropSettings);

    APPCMBS_LOG("Proprietary Settings:\n");
    
    for (u8_Entry = 0; u8_Entry < CMBS_MAX_NUM_PROP_FIELDS; u8_Entry++)
	{    
        if (st_PropSettings.st_PropField[u8_Entry].u8_Data[0])
        {
            APPCMBS_LOG("\nEvent = %02X\n", st_PropSettings.st_PropField[u8_Entry].u8_Data[0]);
            APPCMBS_LOG("Event data: ");
            for (u8_i = 1; u8_i < st_PropSettings.st_PropField[u8_Entry].u8_Len; u8_i++) APPCMBS_LOG("0x%02X,", st_PropSettings.st_PropField[u8_Entry].u8_Data[u8_i]);
        }
    }
}


//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_TIMEOFDAY)
{
    ST_IE_TIMEOFDAY st_TimeOfDay;
    CMBS_SAFE_GET_IE(cmbs_api_ie_TimeGet, &st_TimeOfDay);

    APPCMBS_LOG("TIME OF DAY(msec since 01.01.1970): %d", st_TimeOfDay.u32_Timestamp);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_STATUS)
{
    ST_IE_SYS_STATUS st_SysStatus;
    CMBS_SAFE_GET_IE(cmbs_api_ie_SysStatusGet, &st_SysStatus);

    APPCMBS_LOG("System status:  ");
    switch (st_SysStatus.e_ModuleStatus)
    {
    case CMBS_MODULE_STATUS_UP:
        APPCMBS_LOG("CMBS_MODULE_STATUS_UP");
        break;
    case CMBS_MODULE_STATUS_DOWN:
        APPCMBS_LOG("CMBS_MODULE_STATUS_DOWN");
        break;
    case CMBS_MODULE_STATUS_REMOVED:
        APPCMBS_LOG("CMBS_MODULE_STATUS_REMOVED");
        break;
    default:
        APPCMBS_LOG("Unknown");
        break;
    }

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_FXS_HOOK_STATUS)
{
    u8 u8_hook_status;
	CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, (u8 *)&u8_hook_status, CMBS_IE_FXS_HOOK_STATUS);

    APPCMBS_LOG("Fxs Channel Status:  ");
    switch (u8_hook_status)
    {
    case CMBS_STATUS_ONHOOK:
        APPCMBS_LOG("CMBS_STATUS_ONHOOK");
        break;
    case CMBS_STATUS_OFFHOOK:
        APPCMBS_LOG("CMBS_STATUS_OFFHOOK");
        break;
    default:
		APPCMBS_LOG("hook_status = %d\n", u8_hook_status);
        APPCMBS_LOG("Unknown");
        break;
    }

}//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALL_DIRECTION)
{
    u8 u8_call_Direction;
	CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, (u8 *)&u8_call_Direction, CMBS_IE_CALL_DIRECTION);

    APPCMBS_LOG("Cmbs Call Direction:  ");
    switch (u8_call_Direction)
    {
    case CMBS_CALL_DIRECTION_INCOMING:
        APPCMBS_LOG("CMBS_CALL_DIRECTION_INCOMING");
        break;
    case CMBS_CALL_DIRECTION_OUTGOING:
        APPCMBS_LOG("CMBS_CALL_DIRECTION_OUTGOING");
        break;
    default:
		APPCMBS_LOG("Call Direction = %d\n", u8_call_Direction);
        APPCMBS_LOG("Unknown");
        break;
    }

}//////////////////////////////////////////////////////////////////////////


CMBS_PARSE_DECL(CMBS_IE_REQUEST_ID)
{
    u16            u16_RequestId;
    CMBS_SAFE_GET_IE(cmbs_api_ie_RequestIdGet, &u16_RequestId);
    APPCMBS_LOG("Request Id = %d", u16_RequestId);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HANDSETS)
{
    u16 u16_Handsets, i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HandsetsGet, &u16_Handsets);

    for (i = 0; i < 16; i++)
    {
        if ( u16_Handsets & (1 << i) )
            break;
    }
    u8_HandsetId = i+1;
    APPCMBS_LOG("Hs mask = 0x%04X, First HS (1 based)=%d", u16_Handsets, i + 1);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_GEN_EVENT)
{
    ST_IE_GEN_EVENT	st_GenEvent;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GenEventGet, &st_GenEvent);
    APPCMBS_LOG("Generic event: Subtype = %d, Multiplicity = %d, LineId = %d",\
                   st_GenEvent.u8_SubType, st_GenEvent.u16_MultiPlicity, st_GenEvent.u8_LineId);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_PROP_EVENT)
{
    ST_IE_PROP_EVENT st_PropEvent;
    u8 u8_i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_PropEventGet, &st_PropEvent);

    APPCMBS_LOG("Proprietary event = %d\n", st_PropEvent.u16_Reserved);
    APPCMBS_LOG("Event data: ");
    for (u8_i = 0; u8_i < st_PropEvent.u8_DataLen; u8_i++) APPCMBS_LOG("0x%02X,", st_PropEvent.u8_Data[u8_i]);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_DATETIME)
{
    ST_IE_DATETIME st_DateTime;
    CMBS_SAFE_GET_IE(cmbs_api_ie_DateTimeGet, &st_DateTime);
    APPCMBS_LOG("APPSRV-INFO: DateTime: %02d.%02d.%02d %02d:%02d:%02d",
                st_DateTime.u8_Day, st_DateTime.u8_Month, st_DateTime.u8_Year,
                st_DateTime.u8_Hours, st_DateTime.u8_Mins, st_DateTime.u8_Secs);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_DATA)
{
    ST_IE_DATA     st_Data;
    int i;
    memset(&st_Data, 0, sizeof(st_Data));
    CMBS_SAFE_GET_IE(cmbs_api_ie_DataGet, &st_Data);

    if ( st_Data.pu8_Data )
    {
        APPCMBS_LOG("Length:%d\n", st_Data.u16_DataLen);
        for (i = 0; i < st_Data.u16_DataLen; i++)
        {
            APPCMBS_LOG(" %02X", st_Data.pu8_Data[i]);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_DATA_SESSION_ID)
{
    u16	u16_SessionId;
    CMBS_SAFE_GET_IE(cmbs_api_ie_DataSessionIdGet, &u16_SessionId);
    APPCMBS_LOG("SessionnId: %d", u16_SessionId);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_DATA_SESSION_TYPE)
{
    ST_IE_DATA_SESSION_TYPE		st_SessionType;
    CMBS_SAFE_GET_IE(cmbs_api_ie_DataSessionTypeGet, &st_SessionType);
    APPCMBS_LOG("ChannelType:%d ServiceType:%d",
                st_SessionType.e_ChannelType,
                st_SessionType.e_ServiceType);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_SESSION_ID)
{
    u16 u16_SessionId;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LASessionIdGet, &u16_SessionId);
    APPCMBS_LOG("LA Session Id: %d", u16_SessionId);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_LIST_ID)
{
    u16 u16_ListId;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LAListIdGet, &u16_ListId);
    APPCMBS_LOG("LA List Id: %d", u16_ListId);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(PrintLAFields)
{
    ST_IE_LA_FIELDS st_LaFields;
    int i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LAFieldsGet, &st_LaFields);

    APPCMBS_LOG("Length:%d\n", st_LaFields.u16_Length);
    APPCMBS_LOG("Field Ids: ");
    if ( st_LaFields.u16_Length == 0 )
    {
        APPCMBS_LOG(" (none)");
    }
    else
    {
        for (i = 0; i < st_LaFields.u16_Length; i++)
        {
            APPCMBS_LOG(" %2d", st_LaFields.pu16_FieldId[i]);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_FIELDS)
{

    APPCMBS_LOG("LA_FIELDS:\n");
    app_PrintLAFields_print(pOutput, u32_OutputSize, pu32_Pos, pv_IE);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_SORT_FIELDS)
{
    APPCMBS_LOG("LA_SORT_FIELDS:\n");
    app_PrintLAFields_print(pOutput, u32_OutputSize, pu32_Pos, pv_IE);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_EDIT_FIELDS)
{
    APPCMBS_LOG("LA_EDIT_FIELDS:\n");
    app_PrintLAFields_print(pOutput, u32_OutputSize, pu32_Pos, pv_IE);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_CONST_FIELDS)
{
    APPCMBS_LOG("LA_CONST_FIELDS:\n");
    app_PrintLAFields_print(pOutput, u32_OutputSize, pu32_Pos, pv_IE);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_SEARCH_CRITERIA)
{
    ST_IE_LA_SEARCH_CRITERIA st_LASearchCriteria;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LASearchCriteriaGet, &st_LASearchCriteria);
    APPCMBS_LOG("MatchingType:%d\n", st_LASearchCriteria.e_MatchingType);
    APPCMBS_LOG("CaseSensitive:%d\n", st_LASearchCriteria.u8_CaseSensitive);
    APPCMBS_LOG("Direction:%d\n", st_LASearchCriteria.u8_Direction);
    APPCMBS_LOG("MarkEntriesReq:%d\n", st_LASearchCriteria.u8_MarkEntriesReq);
    APPCMBS_LOG("Pattern:%s", st_LASearchCriteria.pu8_Pattern);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_ENTRY_ID)
{
    u16 u16_EntryID;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LAEntryIdGet, &u16_EntryID);
    APPCMBS_LOG("Edit EntryID %d", u16_EntryID);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_ENTRY_INDEX)
{
    u16 u16_EntryIndex;
    cmbs_api_ie_LAEntryIndexGet(pv_IE, &u16_EntryIndex);
    APPCMBS_LOG("u16_EntryIndex=%d", u16_EntryIndex);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_ENTRY_COUNT)
{
    u16 u16_EntryCountRequested;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LAEntryCountGet, &u16_EntryCountRequested);
    APPCMBS_LOG("u16_EntryCountRequested=%d", u16_EntryCountRequested);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_IS_LAST)
{
    u8 u8_IsLast;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LAIsLastGet, &u8_IsLast);
    APPCMBS_LOG("IsLAst:%d\n", u8_IsLast);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_REJECT_REASON)
{
    u8 u8_RejectReason;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, &u8_RejectReason, CMBS_IE_LA_REJECT_REASON);
    APPCMBS_LOG("LA reject reason = %d", u8_RejectReason);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_NR_OF_ENTRIES)
{
    u16 u16_NrOfEntries;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ShortValueGet, &u16_NrOfEntries, CMBS_IE_LA_NR_OF_ENTRIES);
    APPCMBS_LOG("LA Number of records = %d", u16_NrOfEntries);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALLTRANSFERREQ)
{
    ST_IE_CALLTRANSFERREQ st_CallTr;
    CMBS_SAFE_GET_IE(cmbs_api_ie_CallTransferReqGet, &st_CallTr);

    APPCMBS_LOG("Call transfer from #0x%X, to #0x%X, requsted by from %d",\
                   st_CallTr.u32_CallInstanceFrom, st_CallTr.u32_CallInstanceTo, st_CallTr.u8_TermInstance);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HS_NUMBER)
{
    u8 u8_Handset;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HsNumberGet, &u8_Handset);
    APPCMBS_LOG("HS Number =  %d", u8_Handset);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_ATE_SETTINGS)
{
    ST_IE_ATE_SETTINGS st_AteSettings;
    CMBS_SAFE_GET_IE(cmbs_api_ie_ATESettingsGet, &st_AteSettings);
    APPCMBS_LOG("ATE slot type: %d		\n", st_AteSettings.e_ATESlotType);
    APPCMBS_LOG("ATE type: %d			\n", st_AteSettings.e_ATEType);
    APPCMBS_LOG("ATE instance: %d		\n", st_AteSettings.u8_Instance);
    APPCMBS_LOG("ATE slot: %d			\n", st_AteSettings.u8_Slot);
    APPCMBS_LOG("ATE Carrier: %d		\n", st_AteSettings.u8_Carrier);
    APPCMBS_LOG("ATE Ant: %d			\n", st_AteSettings.u8_Ant);
    APPCMBS_LOG("ATE Pattern: %d		\n", st_AteSettings.u8_Pattern);
    APPCMBS_LOG("ATE Normal Preamble: %d\n", st_AteSettings.u8_NormalPreamble);
    APPCMBS_LOG("ATE Power Level: %d	\n", st_AteSettings.u8_PowerLevel);
    APPCMBS_LOG("ATE GPIO: %d			\n", st_AteSettings.u8_Gpio);
    APPCMBS_LOG("ATE BER-FER: %d		\n", st_AteSettings.u8_BERFEREnabled);
    APPCMBS_LOG("ATE BER-FER frames: %d	\n", st_AteSettings.u8_BERFERFrameCount);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_READ_DIRECTION)
{
    ST_IE_READ_DIRECTION st_ReadDirection;
    CMBS_SAFE_GET_IE(cmbs_api_ie_ReadDirectionGet, &st_ReadDirection);
    APPCMBS_LOG("Read Direction=%d", st_ReadDirection.e_ReadDirection);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_MARK_REQUEST)
{
    ST_IE_MARK_REQUEST st_MarkRequest;
    CMBS_SAFE_GET_IE(cmbs_api_ie_MarkRequestGet, &st_MarkRequest);
    APPCMBS_LOG("Mark=%d", st_MarkRequest.e_MarkRequest);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LINE_SUBTYPE)
{
    u8 u8_value;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, &u8_value, CMBS_IE_LINE_SUBTYPE);
    APPCMBS_LOG("Line subtype =%d", u8_value);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_INTEGER_VALUE)
{
    u32 u32_Value;
    CMBS_SAFE_GET_IE(cmbs_api_ie_IntValueGet, &u32_Value);
    APPCMBS_LOG("u32 value =%d", u32_Value);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_SHORT_VALUE)
{
    u16 u16_Value;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ShortValueGet, &u16_Value, CMBS_IE_SHORT_VALUE);
    APPCMBS_LOG("u16 value =%d", u16_Value);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_AVAIL_VERSION_DETAILS)
{
    ST_SUOTA_UPGRADE_DETAILS st_HSVerAvail;
    CMBS_SAFE_GET_IE(cmbs_api_ie_VersionAvailGet, &st_HSVerAvail);
    APPCMBS_LOG("Delay in minutes = %d \n", st_HSVerAvail.u16_delayInMin);
    APPCMBS_LOG("URLs to Follow = %d \n", st_HSVerAvail.u8_URLStoFollow);
    APPCMBS_LOG("Delay in minutes = %d ", st_HSVerAvail.u8_UserInteraction);

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HS_VERSION_BUFFER)
{
    ST_VERSION_BUFFER st_SwVersion;
    CMBS_SAFE_GET_IE(cmbs_api_ie_VersionBufferGet, &st_SwVersion);
    if ( st_SwVersion.type == CMBS_SUOTA_SW_VERSION )
        APPCMBS_LOG("SW version: ")
else
    APPCMBS_LOG("HW version: ");

st_SwVersion.pu8_VerBuffer[st_SwVersion.u8_VerLen] = 0;
APPCMBS_LOG("%s", st_SwVersion.pu8_VerBuffer);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HS_VERSION_DETAILS)
{
    ST_SUOTA_HS_VERSION_IND st_HSVerInd;
    CMBS_SAFE_GET_IE(cmbs_api_ie_VersionIndGet, &st_HSVerInd);
    APPCMBS_LOG("EMC = %d				\n", st_HSVerInd.u16_EMC);
    APPCMBS_LOG("URLs to Follow  = %d	\n", st_HSVerInd.u8_URLStoFollow);
    APPCMBS_LOG("File number = %d		\n", st_HSVerInd.u8_FileNumber);
    APPCMBS_LOG("Flags = %d				\n", st_HSVerInd.u8_Flags);
    APPCMBS_LOG("Reason = %d			 ",	st_HSVerInd.u8_Reason);

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_SU_SUBTYPE)
{
    u8 u8_Value;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, &u8_Value, CMBS_IE_SU_SUBTYPE);
    APPCMBS_LOG("Software update subtype = %d ", u8_Value);

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_URL)
{
    ST_URL_BUFFER st_Url;
    CMBS_SAFE_GET_IE(cmbs_api_ie_UrlGet, &st_Url);
    st_Url.pu8_UrlBuffer[st_Url.u8_UrlLen] = 0;
    APPCMBS_LOG("URL = %s ", st_Url.pu8_UrlBuffer);

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_NUM_OF_URLS)
{
    u8 u8_Value;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, &u8_Value, CMBS_IE_NUM_OF_URLS);
    APPCMBS_LOG("Number of URLs = %d ", u8_Value);

}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_REJECT_REASON)
{
    u8 u8_Value;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, &u8_Value, CMBS_IE_REJECT_REASON);
    APPCMBS_LOG("Reject reason = %d ", u8_Value);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_TARGET_LIST_CHANGE_NOTIF)
{
    ST_IE_TARGET_LIST_CHANGE_NOTIF st_Notif;
    cmbs_api_ie_TargetListChangeNotifGet(pv_IE, &st_Notif);
    APPCMBS_LOG("\nList:\t");
    switch (st_Notif.e_ListId)
    {
    case CMBS_LA_LIST_SUPPORTED_LISTS:
        APPCMBS_LOG("List Of Supported Lists");
        break;

    case CMBS_LA_LIST_INTERNAL_NAMES:
        APPCMBS_LOG("Internal Names List");
        break;

    case CMBS_LA_LIST_DECT_SETTINGS:
        APPCMBS_LOG("DECT System settings list");
        break;

    default:
        APPCMBS_LOG("\n\n Error: Unknown List ID %d %s %d", st_Notif.e_ListId, __FILE__, __LINE__);
        break;
    }

    APPCMBS_LOG("\nNumber of entries in list:\t%d", st_Notif.u32_NumOfEntries);

    APPCMBS_LOG("\nEntry ID:\t%d", st_Notif.u32_EntryId);

    APPCMBS_LOG("\nChange Type:\t");
    switch (st_Notif.e_ChangeType)
    {
    case CMBS_LIST_CHANGE_ENTRY_DELETED:
        APPCMBS_LOG("Entry Deleted");
        break;

    case CMBS_LIST_CHANGE_ENTRY_INSERTED:
        APPCMBS_LOG("Entry Inserted");
        break;

    case CMBS_LIST_CHANGE_ENTRY_UPDATED:
        APPCMBS_LOG("Entry Updated");
        break;

    default:
        APPCMBS_LOG("\n\n Error: Unknown Change type ID %d %s %d", st_Notif.e_ChangeType, __FILE__, __LINE__);
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_RTP_SESSION_INFORMATION)
{
    int i;
    ST_IE_RTP_SESSION_INFORMATION st_RTPSessionInformation;
    if ( cmbs_api_ie_RTPSessionInformationGet(pv_IE, &st_RTPSessionInformation) != CMBS_RC_OK )
    {
        APPCMBS_LOG("\nBad CMBS_IE_RTP_SESSION_INFORMATION");
        return;
    }

    for (i = 0; i < CMBS_MAX_NUM_OF_CODECS; ++i)
    {
        if ( st_RTPSessionInformation.ast_RxCodecList[i].u8_CodecPt == 0 )
        {
            continue;
        }

        st_RTPSessionInformation.ast_RxCodecList[i].sz_CodecStr[CMBS_MAX_DYNAMIC_CODEC_LEN - 1] = '\0';
        APPCMBS_LOG("\nast_RxCodecList[i].sz_CodecStr: \"%s\"", st_RTPSessionInformation.ast_RxCodecList[i].sz_CodecStr);
        APPCMBS_LOG("\nast_RxCodecList[i].sz_CodecPt:  %d",    (int)st_RTPSessionInformation.ast_RxCodecList[i].u8_CodecPt);
    }

    APPCMBS_LOG("\nu8_RxCodecEventPt:              %d",    (int)st_RTPSessionInformation.u8_RxCodecEventPt);
    st_RTPSessionInformation.st_TxCodec.sz_CodecStr[CMBS_MAX_DYNAMIC_CODEC_LEN - 1] = '\0';
    APPCMBS_LOG("\nst_TxCodec.sz_CodecStr:         \"%s\"", st_RTPSessionInformation.st_TxCodec.sz_CodecStr);
    APPCMBS_LOG("\nst_TxCodec.u8_CodecPt:          %d",    (int)st_RTPSessionInformation.st_TxCodec.u8_CodecPt);
    APPCMBS_LOG("\nu8_TxCodecEventPt:              %d",    (int)st_RTPSessionInformation.u8_TxCodecEventPt);
    APPCMBS_LOG("\nu32_Capabilities:               %lu",   (unsigned long)st_RTPSessionInformation.u32_Capabilities);
    st_RTPSessionInformation.sz_SDesName[CMBS_RTCP_MAX_SDES - 1] = '\0';
    APPCMBS_LOG("\nsz_SDesName:                    \"%s\"", st_RTPSessionInformation.sz_SDesName);
    APPCMBS_LOG("\nu16_Duration:                   %d",    (int)st_RTPSessionInformation.u16_Duration);
    APPCMBS_LOG("\nu32_CurrentTime:                %lu",   (unsigned long)st_RTPSessionInformation.u32_CurrentTime);
    APPCMBS_LOG("\nu32_Timestamp:                  %lu",   (unsigned long)st_RTPSessionInformation.u32_Timestamp);
    APPCMBS_LOG("\nu32_SSRC:                       %lu",   (unsigned long)st_RTPSessionInformation.u32_SSRC);
    APPCMBS_LOG("\nu8_JBMinLen:                    %d",    (int)st_RTPSessionInformation.u8_JBMinLen);
    APPCMBS_LOG("\nu8_JBMaxLen:                    %d",    (int)st_RTPSessionInformation.u8_JBMaxLen);
    switch (st_RTPSessionInformation.e_JBMode)
    {
    case CMBS_RTP_JB_MODE_FIXED:
        APPCMBS_LOG("\ne_JBMode:                       CMBS_RTP_JB_MODE_FIXED");      break;
    case CMBS_RTP_JB_MODE_ADAPTIVE:
        APPCMBS_LOG("\ne_JBMode:                       CMBS_RTP_JB_MODE_ADAPTIVE");   break;
    default:
        APPCMBS_LOG("\ne_JBMode:                       Unknown");                     break;
    }
    APPCMBS_LOG("\nu32_DTMFEndPackets:             %lu",   (unsigned long)st_RTPSessionInformation.u32_DTMFEndPackets);
    switch (st_RTPSessionInformation.e_MediaLoopLevel)
    {
    case CMBS_VOICE_LOOP_NONE:
        APPCMBS_LOG("\ne_MediaLoopLevel:               CMBS_VOICE_LOOP_NONE");        break;
    case CMBS_VOICE_LOOP_DSP_LEVEL:
        APPCMBS_LOG("\ne_MediaLoopLevel:               CMBS_VOICE_LOOP_DSP_LEVEL");   break;
    case CMBS_VOICE_LOOP_RTP_LEVEL:
        APPCMBS_LOG("\ne_MediaLoopLevel:               CMBS_VOICE_LOOP_RTP_LEVEL");   break;
    default:
        APPCMBS_LOG("\ne_MediaLoopLevel:               Unknown");                     break;
    }
    APPCMBS_LOG("\nu16_T38LsRedundancy:            %d",    (int)st_RTPSessionInformation.u16_T38LsRedundancy);
    APPCMBS_LOG("\nu16_T38HsRedundancy:            %d",    (int)st_RTPSessionInformation.u16_T38HsRedundancy);
    APPCMBS_LOG("\nu8_T38EcnOn:                    %d",    (int)st_RTPSessionInformation.u8_T38EcnOn);
    switch (st_RTPSessionInformation.e_AudioMode)
    {
    case CMBS_AUDIO_MODE_ACTIVE:
        APPCMBS_LOG("\ne_AudioMode:                    CMBS_AUDIO_MODE_ACTIVE");      break;
    case CMBS_AUDIO_MODE_REC_ONLY:
        APPCMBS_LOG("\ne_AudioMode:                    CMBS_AUDIO_MODE_REC_ONLY");    break;
    case CMBS_AUDIO_MODE_SEND_ONLY:
        APPCMBS_LOG("\ne_AudioMode:                    CMBS_AUDIO_MODE_SEND_ONLY");   break;
    case CMBS_AUDIO_MODE_INACTIVE:
        APPCMBS_LOG("\ne_AudioMode:                    CMBS_AUDIO_MODE_INACTIVE");    break;
    default:
        APPCMBS_LOG("\ne_AudioMode:                    Unknown");                     break;
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_RTCP_INTERVAL)
{
    u32 u32_RTCPInterval;
    if ( cmbs_api_ie_RTCPIntervalGet(pv_IE, &u32_RTCPInterval) != CMBS_RC_OK )
    {
        APPCMBS_LOG("\nBad CMBS_IE_RTCP_INTERVAL");
        return;
    }

    APPCMBS_LOG("\nu32_RTCPInterval:               %lu",   (long unsigned)u32_RTCPInterval);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_RTP_DTMF_EVENT)
{
    ST_IE_RTP_DTMF_EVENT st_RTPDTMFEvent;
    if ( cmbs_api_ie_RTPDTMFEventGet(pv_IE, &st_RTPDTMFEvent) != CMBS_RC_OK )
    {
        APPCMBS_LOG("\nBad CMBS_IE_RTP_DTMF_EVENT");
        return;
    }

    APPCMBS_LOG("\nu8_Event:                       %d",    (int)st_RTPDTMFEvent.u8_Event);
    APPCMBS_LOG("\nu16_Volume:                     %d",    (int)st_RTPDTMFEvent.u16_Volume);
    APPCMBS_LOG("\nu16_Duration:                   %d",    (int)st_RTPDTMFEvent.u16_Duration);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_RTP_DTMF_EVENT_INFO)
{
    ST_IE_RTP_DTMF_EVENT_INFO st_RTPDTMFEventInfo;
    if ( cmbs_api_ie_RTPDTMFEventInfoGet(pv_IE, &st_RTPDTMFEventInfo) != CMBS_RC_OK )
    {
        APPCMBS_LOG("\nBad CMBS_IE_RTP_DTMF_EVENT_INFO");
        return;
    }

    APPCMBS_LOG("\nu16_EventDuration:              %d",    (int)st_RTPDTMFEventInfo.u16_EventDuration);
    APPCMBS_LOG("\nu16_MaxEventDuration:           %d",    (int)st_RTPDTMFEventInfo.u16_MaxEventDuration);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_RTP_FAX_TONE_TYPE)
{
    E_CMBS_FAX_TONE_TYPE e_FaxToneType;
    if ( cmbs_api_ie_RTPFaxToneTypeGet(pv_IE, &e_FaxToneType) != CMBS_RC_OK )
    {
        APPCMBS_LOG("\nBad CMBS_IE_RTP_FAX_TONE_TYPE");
        return;
    }

    switch (e_FaxToneType)
    {
    case CMBS_FAX_TONE_TYPE_CNG:
        APPCMBS_LOG("\ne_FaxToneType:                  CNG");             break;
    case CMBS_FAX_TONE_TYPE_CED:
        APPCMBS_LOG("\ne_FaxToneType:                  CED");             break;
    case CMBS_FAX_TONE_TYPE_V21_PREAMBLE:
        APPCMBS_LOG("\ne_FaxToneType:                  V.21 Preamble");   break;
    case CMBS_FAX_TONE_TYPE_ANS_PHASEREV:
        APPCMBS_LOG("\ne_FaxToneType:                  Ans/");            break;
    default:
        APPCMBS_LOG("\ne_FaxToneType:                  Undefined");       break;
    }
}
/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_INTERNAL_TRANSFER)
{
    ST_IE_INTERNAL_TRANSFER st_IntTrans;
    if ( cmbs_api_ie_InternalCallTransferReqGet(pv_IE, &st_IntTrans) != CMBS_RC_OK )
    {
        APPCMBS_LOG("\nBad ST_IE_INTERNAL_TRANSFER");
        return;
    }

    APPCMBS_LOG("\nInternal Transfer:\n");
    APPCMBS_LOG("HsNum %d\n", st_IntTrans.u16_HsNum);
    APPCMBS_LOG("To Type: %s\n", st_IntTrans.eTransferToType == CMBS_CALL_LEG_TYPE_LINE ? "Line" : "PP");
    APPCMBS_LOG("To ID: %d\n", st_IntTrans.u32_TransferToID);
    APPCMBS_LOG("From Type: %s\n", st_IntTrans.eTransferFromType == CMBS_CALL_LEG_TYPE_LINE ? "Line" : "PP");
    APPCMBS_LOG("From ID: %d", st_IntTrans.u32_TransferFromID);
}
/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_LA_PROP_CMD)
{
    ST_LA_PROP_CMD st_PropLACmd;
    int i;

    if ( cmbs_api_ie_LAPropCmdGet(pv_IE, &st_PropLACmd) != CMBS_RC_OK )
    {
        APPCMBS_LOG("\nBad ST_LA_PROP_CMD");
        return;
    }

    APPCMBS_LOG("\nList Access Proprietary Command:\n");
    APPCMBS_LOG("Length %d\n", st_PropLACmd.u16_Length);
    APPCMBS_LOG("Data: ");

    for (i = 0; i < st_PropLACmd.u16_Length; ++i)
    {
        APPCMBS_LOG("%X ", st_PropLACmd.pu8_data[i]);
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_REG_CLOSE_REASON)
{
    ST_IE_REG_CLOSE_REASON st_RegCloseReason;

    CMBS_SAFE_GET_IE(cmbs_api_ie_RegCloseReasonGet, &st_RegCloseReason);

    switch (st_RegCloseReason.e_Reg_Close_Reason)
    {
    case CMBS_REG_CLOSE_TIMEOUT:
        APPCMBS_LOG("Registration timeout ");     			break;
    case CMBS_REG_CLOSE_HS_REGISTERED:
        APPCMBS_LOG("HS successfully registered ");
        break;
    default:
        APPCMBS_LOG("UNKNOWN Module ");
    }
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_BASE_NAME)
{
    ST_IE_BASE_NAME st_BaseName;

    CMBS_SAFE_GET_IE(cmbs_api_ie_BaseNameGet, &st_BaseName);

    APPCMBS_LOG("Base Name: %s\n", st_BaseName.u8_BaseName);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_EEPROM_VERSION)
{
    ST_IE_EEPROM_VERSION st_EEPROM_ver;

    CMBS_SAFE_GET_IE(cmbs_api_ie_EEPROMVersionGet, &st_EEPROM_ver);
    APPCMBS_LOG("EEPROM version: %X", st_EEPROM_ver.u32_EEPROM_Version);
}

/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CHECKSUM_ERROR)
{
#if defined(CHECKSUM_SUPPORT)
    ST_IE_CHECKSUM_ERROR st_ChecksumError;

    CMBS_SAFE_GET_IE(cmbs_api_ie_ChecksumErrorGet, &st_ChecksumError);

    APPCMBS_LOG("Checksum Error: ");
    switch (st_ChecksumError.e_CheckSumError)
    {
    case CMBS_CHECKSUM_ERROR:
        APPCMBS_LOG("Checksum failure ");     		break;
    case CMBS_CHECKSUM_NOT_FOUND:
        APPCMBS_LOG("Checksum missing ");  	  		break;
    case CMBS_CHECKSUM_NO_EVENT_ID:
        APPCMBS_LOG("No event ID      ");  	  		break;
    default:
        APPCMBS_LOG("Unknown CS fail %d", st_ChecksumError.e_CheckSumError);  break;
    }
#else
    UNUSED_PARAMETER(pOutput);
    UNUSED_PARAMETER(pu32_Pos);
    UNUSED_PARAMETER(pv_IE);
#endif
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_SPLASH_TIMER)
{
	u8 u8_SplashTimer;
    CMBS_SAFE_GET_IE(cmbs_api_ie_SplashTimerGet, &u8_SplashTimer);

	APPCMBS_LOG("CMBS_IE_SPLASH_TIMER IE for duration %d\n",u8_SplashTimer);
}
/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_MSG)
{
    u8	u8_Buffer[CMBS_HAN_MAX_MSG_LEN];
    u32  	u32_Index;	
    ST_IE_HAN_MSG stIe_Msg;
    stIe_Msg.pu8_Data = u8_Buffer;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANMsgGet, &stIe_Msg);

    APPCMBS_LOG("Src: (type = %d, dev_id = %d, un_id = %d)\n", stIe_Msg.u8_DstAddressType, stIe_Msg.u16_SrcDeviceId, stIe_Msg.u8_SrcUnitId);
	if(stIe_Msg.u8_DstAddressType == 0)
	{
    	APPCMBS_LOG("Dst: (type = %d, dev_id = %d, un_id = %d)\n", stIe_Msg.u8_DstAddressType, stIe_Msg.u16_DstDeviceId, stIe_Msg.u8_DstUnitId);
	}
	else
	{
		APPCMBS_LOG("Dst: (type = %d, group_id = %d)\n", stIe_Msg.u8_DstAddressType, stIe_Msg.u16_DstDeviceId);
	}
    APPCMBS_LOG("Transport : %d\n", stIe_Msg.st_MsgTransport.u16_Reserved);
    APPCMBS_LOG("Msg Sequence : %d, Message Type : %d\n", stIe_Msg.u8_MsgSequence, stIe_Msg.e_MsgType);
    APPCMBS_LOG("IF Type : %d, IF Id : 0x%X, IF Member : %d \n", stIe_Msg.u8_InterfaceType, stIe_Msg.u16_InterfaceId, stIe_Msg.u8_InterfaceMember);
    APPCMBS_LOG("Data Len: %d\n", stIe_Msg.u16_DataLen);
    for (u32_Index = 0; u32_Index < stIe_Msg.u16_DataLen; u32_Index++)
    {
        APPCMBS_LOG(" %d", stIe_Msg.pu8_Data[u32_Index]);
    }
}

/////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_MSG_CONTROL)
{
    ST_IE_HAN_MSG_CTL stIe_MsgCtl;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANMsgCtlGet, &stIe_MsgCtl);

    APPCMBS_LOG("Msg Ctrl :\n\tImmediateSend = %d,\n\tIsLast=%d\n", stIe_MsgCtl.ImmediateSend, stIe_MsgCtl.IsLast);
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HS_PROP_EVENT)
{
    ST_IE_HS_PROP_EVENT st_PropEvent;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HsPropEventGet, &st_PropEvent);
    APPCMBS_LOG("Handset number:	%d\n", st_PropEvent.u8_HsNumber);
    APPCMBS_LOG("PropEvent:			%d\n", st_PropEvent.u8_PropEvent);
    APPCMBS_LOG("Handset EMC:		%d %d\n", st_PropEvent.u8_EMC[0], st_PropEvent.u8_EMC[1]);
    APPCMBS_LOG("Line ID:			%d\n", st_PropEvent.u8_LineId);
    APPCMBS_LOG("Line Subtype:		%d\n", st_PropEvent.u8_LineIdSubType);
    APPCMBS_LOG("Data length:		%d", st_PropEvent.u8_DataLen);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_SYPO_SPECIFICATION)
{
    ST_IE_SYPO_SPECIFICATION st_SypoSpecification;
    CMBS_SAFE_GET_IE(cmbs_api_ie_SYPOSpecificationGet, &st_SypoSpecification);
    APPCMBS_LOG("Wait For Sync:	%d\n", st_SypoSpecification.u32_WaitForSync);
    APPCMBS_LOG("GPIO Number:	%X", st_SypoSpecification.u8_GPIO);
}

CMBS_PARSE_DECL(CMBS_IE_AFE_ENDPOINT_CONNECT)
{
    ST_IE_AFE_ENDPOINTS_CONNECT st_EndpointConnect;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_EndpointConnectionGet, &st_EndpointConnect);
    APPCMBS_LOG("Audio Sink Endpoint #1:	%s\n", app_GetAFEEndpointString(st_EndpointConnect.e_AFEEndPointOUT));
    APPCMBS_LOG("Source Endpoint #2:	%s\n", app_GetAFEEndpointString(st_EndpointConnect.e_AFEEndPointIN));
}

CMBS_PARSE_DECL(CMBS_IE_AFE_ENDPOINT)
{
    ST_IE_AFE_ENDPOINT st_Endpoint;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_EndpointGet, &st_Endpoint);
    APPCMBS_LOG("Endpoint :	%s\n", app_GetAFEEndpointString(st_Endpoint.e_AFEChannel));
}

CMBS_PARSE_DECL(CMBS_IE_AFE_ENDPOINT_GAIN)
{
    ST_IE_AFE_ENDPOINT_GAIN st_EndpointGain;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_EndpointGainGet, &st_EndpointGain);
    APPCMBS_LOG("Endpoint:	%s\n", app_GetAFEEndpointString(st_EndpointGain.e_AFEChannel));
    APPCMBS_LOG("Gain:		%s\n", app_GetAFEGainString(st_EndpointGain.s16_NumOfSteps));
}

CMBS_PARSE_DECL(CMBS_IE_AFE_ENDPOINT_GAIN_DB)
{
    ST_IE_AFE_ENDPOINT_GAIN_DB st_EndpointGainDB;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_EndpointGainDBGet, &st_EndpointGainDB);
    APPCMBS_LOG("Endpoint:	%s\n", app_GetAFEEndpointString(st_EndpointGainDB.e_AFEChannel));
    APPCMBS_LOG("Gain:		%d\n", st_EndpointGainDB.s16_GainInDB);
}

CMBS_PARSE_DECL(CMBS_IE_AFE_AUX_MEASUREMENT_SETTINGS)
{
    ST_IE_AFE_AUX_MEASUREMENT_SETTINGS st_AUXMeasurementSettings;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_AUXMeasureSettingsGet, &st_AUXMeasurementSettings);
    APPCMBS_LOG("AUX Input:	0x%X (%s)\n", st_AUXMeasurementSettings.e_AUX_Input, app_GetAuxMuxInputString(st_AUXMeasurementSettings.e_AUX_Input));
    APPCMBS_LOG("BMP:		%d\n", st_AUXMeasurementSettings.b_Bmp);
}

CMBS_PARSE_DECL(CMBS_IE_AFE_AUX_MEASUREMENT_RESULT)
{
    ST_IE_AFE_AUX_MEASUREMENT_RESULT st_AUXMeasurementResults;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_AUXMeasureResultGet, &st_AUXMeasurementResults);
    APPCMBS_LOG("Measurement Result (in HEX):	0x%X", st_AUXMeasurementResults.s16_Measurement_Result);
}
CMBS_PARSE_DECL(CMBS_IE_AFE_RESOURCE_TYPE)
{
    u8 u8_ResourceType;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_ResourceTypeGet, &u8_ResourceType);
    APPCMBS_LOG("Resource Type: %s\n", u8_ResourceType == CMBS_AFE_RESOURCE_DACADC ? "DAC" : "DCLASS");
}
CMBS_PARSE_DECL(CMBS_IE_AFE_INSTANCE_NUM)
{
    u8 u8_InstanceNum;
    CMBS_SAFE_GET_IE(cmbs_api_ie_AFE_InstanceNumGet, &u8_InstanceNum);
    APPCMBS_LOG("Instance Number: %s\n", u8_InstanceNum == CMBS_AFE_INST_DAC0 ? "DAC0" : "DAC1");
}

CMBS_PARSE_DECL(CMBS_IE_DHSG_VALUE)
{
    u8 u8_DHSGValue;
    CMBS_SAFE_GET_IE(cmbs_api_ie_DHSGValueGet, &u8_DHSGValue);
    APPCMBS_LOG("DHSG value: %X\n", u8_DHSGValue);
}

CMBS_PARSE_DECL(CMBS_IE_GPIO_ID)
{
    ST_IE_GPIO_ID st_GPIOId;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GPIOIDGet, &st_GPIOId);
    APPCMBS_LOG("GPIO BANK: %d\n", st_GPIOId.e_GPIOBank);
    APPCMBS_LOG("GPIO Pin:    %d\n", st_GPIOId.u32_GPIO);
}
CMBS_PARSE_DECL(CMBS_IE_GPIO_MODE)
{
    u8 u8_GPIOMode;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GPIOModeGet, &u8_GPIOMode);
    APPCMBS_LOG("GPIO Mode: %s\n", u8_GPIOMode == 0 ? "GPIO_MODE_INPUT" : "GPIO_MODE_OUTPUT");
}
CMBS_PARSE_DECL(CMBS_IE_GPIO_VALUE)
{
    u8 u8_GPIOValue;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GPIOValueGet, &u8_GPIOValue);
    APPCMBS_LOG("GPIO Value: %s\n", u8_GPIOValue == 0 ? "DATA CLR" : "DATA SET");
}
CMBS_PARSE_DECL(CMBS_IE_GPIO_PULL_TYPE)
{
    u8 u8_GPIOPullType;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GPIOPullTypeGet, &u8_GPIOPullType);
    APPCMBS_LOG("GPIO Pull Type: %s\n", u8_GPIOPullType == 0 ? "GPIO_PULL_DN" : "GPIO_PULL_UP");
}
CMBS_PARSE_DECL(CMBS_IE_GPIO_PULL_ENA)
{
    u8 u8_GPIOPullEna;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GPIOPullEnaGet, &u8_GPIOPullEna);
    APPCMBS_LOG("GPIO Pull Enable: %s\n", u8_GPIOPullEna == 0 ? "GPIO_PULL_DIS" : "GPIO_PULL_ENA");
}

CMBS_PARSE_DECL(CMBS_IE_GPIO_ENA)
{
    u8 u8_GPIOEna;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GPIOEnaGet, &u8_GPIOEna);
    APPCMBS_LOG("GPIO Enable : %s\n", u8_GPIOEna == 1 ? "GPIO_ENABLED" : "GPIO_DISABLED");
}

CMBS_PARSE_DECL(CMBS_IE_EXT_INT_CONFIGURATION)
{
    ST_IE_INT_CONFIGURATION st_INTConfig;
    CMBS_SAFE_GET_IE(cmbs_api_ie_ExtIntConfigurationGet, &st_INTConfig);
    APPCMBS_LOG("INTPolarity:    %d\n", st_INTConfig.u8_INTPolarity);
    APPCMBS_LOG("INTType:    %d\n", st_INTConfig.u8_INTType);
}
CMBS_PARSE_DECL(CMBS_IE_EXT_INT_NUM)
{
    u8 u8_INTNumber;
    CMBS_SAFE_GET_IE(cmbs_api_ie_ExtIntNumGet, &u8_INTNumber);
    APPCMBS_LOG("Interrupt number: %d\n", u8_INTNumber);
}

CMBS_PARSE_DECL(CMBS_IE_TERMINAL_CAPABILITIES)
{
    ST_IE_TERMINAL_CAPABILITIES st_TermCapability;

    CMBS_SAFE_GET_IE(cmbs_api_ie_TerminalCapabilitiesGet, &st_TermCapability);

    APPCMBS_LOG("<< TERMINAL-CAPABILITY >> %d\n", st_TermCapability.u8_IE_Type);

    APPCMBS_LOG("Length of Contents : %d\n", st_TermCapability.u8_Length);

    APPCMBS_LOG("tone capabilities = %d\n", st_TermCapability.u8_ToneCapabilities);
    APPCMBS_LOG("display capabilities = %d\n", st_TermCapability.u8_DisplayCapabilities);

    APPCMBS_LOG("A-VOL = %d\n", st_TermCapability.u8_A_VOL);
    APPCMBS_LOG("N-REJ = %d\n", st_TermCapability.u8_N_REJ);
    APPCMBS_LOG("echo parameter = %d\n", st_TermCapability.u8_EchoParameter);

    APPCMBS_LOG("slot type capability = %d\n", st_TermCapability.u8_SlotTypeCapability);

    APPCMBS_LOG("Number of stored display characters (MS) = %d\n", st_TermCapability.u8_StoredDisplayCharactersMS);

    APPCMBS_LOG("Number of stored display characters (LS) = %d\n", st_TermCapability.u8_StoredDisplayCharactersLS);

    APPCMBS_LOG("Number of lines in (physical) display = %d\n", st_TermCapability.u8_NumberOfLinesInDisplay);

    APPCMBS_LOG("Number of characters/line = %d\n", st_TermCapability.u8_NumberOfCharactersInLine);

    APPCMBS_LOG("Scrolling behaviour field = %d\n", st_TermCapability.u8_ScrollingBehaviour);

    APPCMBS_LOG("Profile indicator_1 = %d\n", st_TermCapability.u8_ProfileIndicator_1);

    APPCMBS_LOG("Profile indicator_2 = %d\n", st_TermCapability.u8_ProfileIndicator_2);

    APPCMBS_LOG("Profile indicator_3 = %d\n", st_TermCapability.u8_ProfileIndicator_3);

    APPCMBS_LOG("Profile indicator_4 = %d\n", st_TermCapability.u8_ProfileIndicator_4);

    APPCMBS_LOG("Profile indicator_5 = %d\n", st_TermCapability.u8_ProfileIndicator_5);

    APPCMBS_LOG("Profile indicator_6 = %d\n", st_TermCapability.u8_ProfileIndicator_6);

    APPCMBS_LOG("Profile indicator_7 = %d\n", st_TermCapability.u8_ProfileIndicator_7);

    APPCMBS_LOG("Profile indicator_8 = %d\n", st_TermCapability.u8_ProfileIndicator_8);

    APPCMBS_LOG("Profile indicator_9 = %d\n", st_TermCapability.u8_ProfileIndicator_9);

    APPCMBS_LOG("Profile indicator_10 = %d\n", st_TermCapability.u8_ProfileIndicator_10);

    APPCMBS_LOG("control codes = %d\n", st_TermCapability.u8_ControlCodes);
    APPCMBS_LOG("DSAA2 = %d\n", st_TermCapability.u8_DSAA2);
    APPCMBS_LOG("DSC2 = %d\n", st_TermCapability.u8_DSC2);

    APPCMBS_LOG("escape to 8 bit character = %d\n", st_TermCapability.u8_EscapeTo8BitCharacter);

    APPCMBS_LOG("Blind slot indication = %d\n", st_TermCapability.u8_BlindSlotIndication);
    APPCMBS_LOG("sp0 = %d\n", st_TermCapability.u8_sp0);
    APPCMBS_LOG("sp1 = %d\n", st_TermCapability.u8_sp1);
    APPCMBS_LOG("sp2 = %d\n", st_TermCapability.u8_sp2);
    APPCMBS_LOG("sp3 = %d\n", st_TermCapability.u8_sp3);
    APPCMBS_LOG("sp4 = %d\n", st_TermCapability.u8_sp4);

    APPCMBS_LOG("sp5 = %d\n", st_TermCapability.u8_sp5);
    APPCMBS_LOG("sp6 = %d\n", st_TermCapability.u8_sp6);
    APPCMBS_LOG("sp7 = %d\n", st_TermCapability.u8_sp7);
    APPCMBS_LOG("sp8 = %d\n", st_TermCapability.u8_sp8);
    APPCMBS_LOG("sp9 = %d\n", st_TermCapability.u8_sp9);
    APPCMBS_LOG("sp10 = %d\n", st_TermCapability.u8_sp10);
    APPCMBS_LOG("sp11 = %d\n", st_TermCapability.u8_sp11);
    APPCMBS_LOG("sp12 = %d\n", st_TermCapability.u8_sp11);
}

/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HW_VERSION)
{
    ST_IE_HW_VERSION st_HWVersion;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HwVersionGet, &st_HWVersion);

    switch (st_HWVersion.u8_HwBoard)
    {
    case CMBS_HW_BOARD_MOD:
        APPCMBS_LOG("CMBS Module\n");			break;
    case CMBS_HW_BOARD_DEV:
        APPCMBS_LOG("Development board\n");     break;
    default:
        APPCMBS_LOG("UNKNOWN\n");				break;
    }

    APPCMBS_LOG("HW Chip Type = %s\n", app_GetHWChipString(st_HWVersion.u8_HwChip));
    APPCMBS_LOG("HW Com Type = %s\n", app_GetHWCOMString(st_HWVersion.u8_HwComType));
    APPCMBS_LOG("HW Chip Version = %s\n", app_GetHWChipVersionString(st_HWVersion.u8_HwChipVersion));

}
/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_CALL_HOLD_REASON)
{

    ST_IE_CALL_HOLD_REASON st_Reason;

    CMBS_SAFE_GET_IE(cmbs_api_ie_CallHoldReasonGet, &st_Reason);

    APPCMBS_LOG(" Call Hold Reason: ");
    switch (st_Reason.eReason)
    {
    case E_CMBS_CALL_HOLD_REASON_CALL_WAITING_ACCEPT:
        APPCMBS_LOG("Call Waiting Accept");           break;
    case E_CMBS_CALL_HOLD_REASON_USER_REQUEST:
        APPCMBS_LOG("User Request");                  break;
    case E_CMBS_CALL_HOLD_REASON_CALL_TOGGLE:
        APPCMBS_LOG("Call Toggle");                   break;
    case E_CMBS_CALL_HOLD_REASON_PARALLEL_CALL:
        APPCMBS_LOG("Parallel Call initiation");      break;
    case E_CMBS_CALL_HOLD_REASON_SETTINGS_MISMATCH:
        APPCMBS_LOG("Settings mismatch");             break;
    default:
        APPCMBS_LOG("UNKNOWN");                      break;
    }
}

/////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_CALL_SCREENING)
{

    ST_IE_CALL_SCREENING st_CallScreening;

    CMBS_SAFE_GET_IE(cmbs_api_ie_CallScreeningGet, &st_CallScreening);

    APPCMBS_LOG(" Call Screening: ");
    switch (st_CallScreening.eScreen)
    {
    case E_CMBS_CALL_SCREENING_ACCEPT:
        APPCMBS_LOG("Accept");                  break;
    case E_CMBS_CALL_SCREENING_INTERCEPT:
        APPCMBS_LOG("Intercept");               break;
    default:
        APPCMBS_LOG("UNKNOWN");                 break;
    }
}

/////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_SUOTA_APP_ID)
{
	u32 u32_AppId;

	CMBS_SAFE_GET_IE(cmbs_api_ie_SuotaAppIdGet, &u32_AppId);

    APPCMBS_LOG("SUOTA App Id = 0x%X\n", u32_AppId);
}

CMBS_PARSE_DECL(CMBS_IE_SUOTA_SESSION_ID)
{
	u32 u32_SessionId;

	CMBS_SAFE_GET_IE(cmbs_api_ie_SuotaSessionIdGet, &u32_SessionId);

    APPCMBS_LOG("SUOTA Session Id = 0x%X\n", u32_SessionId);
}

CMBS_PARSE_DECL(CMBS_IE_SUOTA_FILE_INFO)
{
    ST_IE_SUOTA_FILE_INFO st_FileInfo;

    CMBS_SAFE_GET_IE(cmbs_api_ie_SuotaFileInfoGet, &st_FileInfo);

    APPCMBS_LOG(" SUOTA File size (in Bytes) = %d\n", st_FileInfo.u32_FileSize);
    APPCMBS_LOG(" Bytes range: From %d - To %d\n", st_FileInfo.u32_From, st_FileInfo.u32_To);
}

/////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_SLIC_LINE_TEST_RESULTS)
{
    ST_IE_SLIC_LINE_TEST_RES st_LineTestResults;
    LT_GR909_RESULT_t st_FormattedLineTestResults;

    CMBS_SAFE_GET_IE(cmbs_api_ie_LineTestResultsGet, &st_LineTestResults);

#ifdef OLD_OPTION
    APPCMBS_LOG("<< Test Results Mask>> 0x%X\n", st_LineTestResults.valid_mask);
    APPCMBS_LOG("HPT (Hazardous Potential) Test Result : %d\n", st_LineTestResults.hpt);
    APPCMBS_LOG("FEMF (Foreign ElectroMotive Forces) Test Result : %d\n", st_LineTestResults.femf);
    APPCMBS_LOG("RTF (Resistive Faults) Test Result : %d\n", st_LineTestResults.rft);
    APPCMBS_LOG("ROH (Receiver Off-Hook) Test Result : %d\n", st_LineTestResults.roh);
    APPCMBS_LOG("RIT (Ringer Impedance) Test Result : %d\n", st_LineTestResults.rit);
#elif RAW_DATA_OPTION

{
    LT_GR909_RESULT_t *pst_FormattedLineTestResults;
    pst_FormattedLineTestResults = (LT_GR909_RESULT_t *)st_LineTestResults.pu8_data;
    APPCMBS_LOG("\n st_LineTestResults.u8_LineTestResultLen = %d\n", st_LineTestResults.u8_LineTestResultLen);
    APPCMBS_LOG("pst_FormattedLineTestResults->hpt.f_hpt_ac_r2g : %f\n", pst_FormattedLineTestResults->hpt.f_hpt_ac_r2g);
}

#else
    st_FormattedLineTestResults.valid_mask = st_LineTestResults.valid_mask;
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.hpt.b_result, st_LineTestResults.b_hpt_result);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.hpt.f_hpt_ac_r2g, st_LineTestResults.f_hpt_ac_r2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.hpt.f_hpt_ac_t2g, st_LineTestResults.f_hpt_ac_t2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.hpt.f_hpt_ac_t2r, st_LineTestResults.f_hpt_ac_t2r);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.hpt.f_hpt_dc_r2g, st_LineTestResults.f_hpt_dc_r2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.hpt.f_hpt_dc_t2g, st_LineTestResults.f_hpt_dc_t2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.hpt.f_hpt_dc_t2r, st_LineTestResults.f_hpt_dc_t2r);

    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.femf.b_result, st_LineTestResults.b_femf_result);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.femf.f_femf_ac_r2g, st_LineTestResults.f_femf_ac_r2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.femf.f_femf_ac_t2g, st_LineTestResults.f_femf_ac_t2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.femf.f_femf_ac_t2r, st_LineTestResults.f_femf_ac_t2r);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.femf.f_femf_dc_r2g, st_LineTestResults.f_femf_dc_r2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.femf.f_femf_dc_t2g, st_LineTestResults.f_femf_dc_t2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.femf.f_femf_dc_t2r, st_LineTestResults.f_femf_dc_t2r);

    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.rft.b_result, st_LineTestResults.b_rft_result);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.rft.f_rft_r2g, st_LineTestResults.f_rft_r2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.rft.f_rft_t2g, st_LineTestResults.f_rft_t2g);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.rft.f_rft_t2r, st_LineTestResults.f_rft_t2r);

    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.roh.b_result, st_LineTestResults.b_roh_result);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.roh.f_roh_t2r_l, st_LineTestResults.f_roh_t2r_h);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.roh.f_roh_t2r_h, st_LineTestResults.f_roh_t2r_h);
    
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.rit.b_result, st_LineTestResults.b_rit_result);
    COPY_INT_TO_FLOAT(st_FormattedLineTestResults.rit.f_rit_res, st_LineTestResults.f_rit_res);

    
    APPCMBS_LOG("st_FormattedLineTestResults.valid_mask : 0x%X\n", st_FormattedLineTestResults.valid_mask);
    APPCMBS_LOG("st_FormattedLineTestResults.hpt.b_result : %d\n", st_FormattedLineTestResults.hpt.b_result);
    APPCMBS_LOG("st_FormattedLineTestResults.hpt.f_hpt_ac_r2g : %f\n", st_FormattedLineTestResults.hpt.f_hpt_ac_r2g);
    APPCMBS_LOG("st_FormattedLineTestResults.hpt.f_hpt_ac_t2g : %f\n", st_FormattedLineTestResults.hpt.f_hpt_ac_t2g);
    APPCMBS_LOG("st_FormattedLineTestResults.hpt.f_hpt_ac_t2r : %f\n", st_FormattedLineTestResults.hpt.f_hpt_ac_t2r);
    APPCMBS_LOG("st_FormattedLineTestResults.hpt.f_hpt_dc_r2g : %f\n", st_FormattedLineTestResults.hpt.f_hpt_dc_r2g);
    APPCMBS_LOG("st_FormattedLineTestResults.hpt.f_hpt_dc_t2g : %f\n", st_FormattedLineTestResults.hpt.f_hpt_dc_t2g);
    APPCMBS_LOG("st_FormattedLineTestResults.hpt.f_hpt_dc_t2r : %f\n", st_FormattedLineTestResults.hpt.f_hpt_dc_t2r);

    APPCMBS_LOG("st_FormattedLineTestResults.femf.b_result : %d\n", st_FormattedLineTestResults.femf.b_result);
    APPCMBS_LOG("st_FormattedLineTestResults.femf.f_femf_ac_r2g : %f\n", st_FormattedLineTestResults.femf.f_femf_ac_r2g);
    APPCMBS_LOG("st_FormattedLineTestResults.femf.f_femf_ac_t2g : %f\n", st_FormattedLineTestResults.femf.f_femf_ac_t2g);
    APPCMBS_LOG("st_FormattedLineTestResults.femf.f_femf_ac_t2r : %f\n", st_FormattedLineTestResults.femf.f_femf_ac_t2r);
    APPCMBS_LOG("st_FormattedLineTestResults.femf.f_femf_dc_r2g : %f\n", st_FormattedLineTestResults.femf.f_femf_dc_r2g);
    APPCMBS_LOG("st_FormattedLineTestResults.femf.f_femf_dc_t2g : %f\n", st_FormattedLineTestResults.femf.f_femf_dc_t2g);
    APPCMBS_LOG("st_FormattedLineTestResults.femf.f_femf_dc_t2r : %f\n", st_FormattedLineTestResults.femf.f_femf_dc_t2r);

    APPCMBS_LOG("st_FormattedLineTestResults.rft.b_result : %d\n", st_FormattedLineTestResults.rft.b_result);
    APPCMBS_LOG("st_FormattedLineTestResults.rft.f_rft_r2g : %f\n", st_FormattedLineTestResults.rft.f_rft_r2g);
    APPCMBS_LOG("st_FormattedLineTestResults.rft.f_rft_t2g : %f\n", st_FormattedLineTestResults.rft.f_rft_t2g);
    APPCMBS_LOG("st_FormattedLineTestResults.rft.f_rft_t2r : %f\n", st_FormattedLineTestResults.rft.f_rft_t2r);

    APPCMBS_LOG("st_FormattedLineTestResults.roh.b_result : %d\n", st_FormattedLineTestResults.roh.b_result);
    APPCMBS_LOG("st_FormattedLineTestResults.roh.f_roh_t2r_l : %f\n", st_FormattedLineTestResults.roh.f_roh_t2r_l);
    APPCMBS_LOG("st_FormattedLineTestResults.roh.f_roh_t2r_h : %f\n", st_FormattedLineTestResults.roh.f_roh_t2r_h);

    APPCMBS_LOG("st_FormattedLineTestResults.rit.b_result : %d\n", st_FormattedLineTestResults.rit.b_result);
    APPCMBS_LOG("st_FormattedLineTestResults.rit.f_rit_res : %f\n", st_FormattedLineTestResults.rit.f_rit_res);


#endif
}
//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_SLIC_NLT_CAP_TEST_RESULTS)
{
    ST_IE_SLIC_NLT_CAP_TEST_RES st_NltCapTestResults;
    NLT_Capacitance_Result_t st_FormattedNltCapTestResults;

    CMBS_SAFE_GET_IE(cmbs_api_ie_NltCapTestResultsGet, &st_NltCapTestResults);

    st_FormattedNltCapTestResults.b_tip2ring_valid = st_NltCapTestResults.b_tip2ring_valid;
    COPY_INT_TO_FLOAT(st_FormattedNltCapTestResults.f_tip2ring_cap, st_NltCapTestResults.f_tip2ring_cap);
    st_FormattedNltCapTestResults.b_line2gnd_valid = st_NltCapTestResults.b_line2gnd_valid;
    COPY_INT_TO_FLOAT(st_FormattedNltCapTestResults.f_tip2gnd_cap, st_NltCapTestResults.f_tip2gnd_cap);
    COPY_INT_TO_FLOAT(st_FormattedNltCapTestResults.f_ring2gnd_cap, st_NltCapTestResults.f_ring2gnd_cap);

	APPCMBS_LOG("st_FormattedNltCapTestResults.b_tip2ring_valid : %d\n", st_FormattedNltCapTestResults.b_tip2ring_valid);
    APPCMBS_LOG("st_FormattedNltCapTestResults.f_tip2ring_cap : %f\n", st_FormattedNltCapTestResults.f_tip2ring_cap);
	APPCMBS_LOG("st_FormattedNltCapTestResults.b_line2gnd_valid : %d\n", st_FormattedNltCapTestResults.b_line2gnd_valid);
    APPCMBS_LOG("st_FormattedNltCapTestResults.f_tip2gnd_cap : %f\n", st_FormattedNltCapTestResults.f_tip2gnd_cap);
    APPCMBS_LOG("st_FormattedNltCapTestResults.f_ring2gnd_cap : %f\n", st_FormattedNltCapTestResults.f_ring2gnd_cap);

}
//////////////////////////////////////////////////////////////////////////





CMBS_PARSE_DECL(CMBS_IE_RSSI_VALUE)
{
    u8 u8_RSSI;

    CMBS_SAFE_GET_IE(cmbs_api_ie_RSSIGet, &u8_RSSI);

    APPCMBS_LOG("RSSI value is %d", u8_RSSI);
}


CMBS_PARSE_DECL(CMBS_IE_STATE_FILTER_TYPE)
{

    u8 u8_FilterType;

    CMBS_SAFE_GET_IE(cmbs_api_ie_CallStateFilterGet, &u8_FilterType);

    switch (u8_FilterType)
    {
	    case CMBS_CALL_STATE_FILTER_SEND_ALL:
	        APPCMBS_LOG("No Filter, sending all call states\n");		break;
	    case CMBS_CALL_STATE_FILTER_SEND_EXT_CALLS_ONLY:
	        APPCMBS_LOG("Sending states for external calls only\n");    break;
	    case CMBS_CALL_STATE_FILTER_SEND_INT_CALLS_ONLY:
	        APPCMBS_LOG("Sending states for internall calls only\n");	break;
	    case CMBS_CALL_STATE_FILTER_SEND_NONE:
	        APPCMBS_LOG("Not sending any call states\n");    			break;        
	    default:
	        APPCMBS_LOG("UNKNOWN\n");									break;
    }
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_JEDEC_ID)
{
    ST_IE_CMBS_JEDEC_ID st_JEDECid;

    CMBS_SAFE_GET_IE(cmbs_api_ie_JEDECIdGet, &st_JEDECid);
    APPCMBS_LOG("ManufacturerID = %X\n", st_JEDECid.u8_ManufacturerID);
    APPCMBS_LOG("DeviceID = %X\n", st_JEDECid.u8_MemoryType);
    APPCMBS_LOG("DeviceID = %X\n", st_JEDECid.u8_MemoryDensity);
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_DTMF_TONE_DUR)
{
    u16 u16_ToneDur;

    CMBS_SAFE_GET_IE(cmbs_api_ie_DTMFToneDurGet, &u16_ToneDur);
    APPCMBS_LOG("DTMF Tone On Duration = %d ms", u16_ToneDur);
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_TDM_SLOTS)
{
    u32 u32_TDMSlots;

    CMBS_SAFE_GET_IE(cmbs_api_ie_TDMSlotsGet, &u32_TDMSlots);

    APPCMBS_LOG("TDM Slots: 0x%X", u32_TDMSlots);
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_DTMF_TONE_TO_HS)
{
    u16 u16_DTMFToneToHSMask;

    CMBS_SAFE_GET_IE(cmbs_api_ie_DTMFToneToHSGet, &u16_DTMFToneToHSMask);

    APPCMBS_LOG("DTMF Tone HS MASK: 0x%X", u16_DTMFToneToHSMask);
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_CRASH_DUMP_ENABLE)
{
    ST_IE_CRASH_DUMP_ENABLE_STATUS st_Enable;

    CMBS_SAFE_GET_IE(cmbs_api_ie_CrashDumpEnableStatusGet, &st_Enable);
	
	if(st_Enable.e_CrashDumpEnble == CRASH_DUMP_DISABLE)
	{
    	APPCMBS_LOG("CRASH_DUMP_DISABLE\n");
	}
	else if(st_Enable.e_CrashDumpEnble == CRASH_DUMP_ENABLE)
	{
    	APPCMBS_LOG("CRASH_DUMP_ENABLE\n");
	}
	else
	{
    	APPCMBS_LOG("CRASH_DUMP_STATUS_INVALID\n");
	}
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_PACKET_NUMBER)
{
    ST_IE_PACKET_NUMBER st_PktNr;

    CMBS_SAFE_GET_IE(cmbs_api_ie_PacketNumberGet, &st_PktNr);
	
   	APPCMBS_LOG("Packet No: %d / %d\n", st_PktNr.u16_PktNr, st_PktNr.u16_TotalPkts);
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_PRODUCT_ID)
{
    ST_IE_PRODUCT_ID st_ProductID;

    CMBS_SAFE_GET_IE(cmbs_api_ie_ProductIDGet, &st_ProductID);
	
   	APPCMBS_LOG("Product ID: %s\n", st_ProductID.u8_ProductID);
}


//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_TABLE_BRIEF_ENTRIES)
{
    ST_HAN_BRIEF_DEVICE_INFO arrSt_Devices[CMBS_HAN_MAX_DEVICES];
    ST_IE_HAN_BRIEF_DEVICE_ENTRIES stIe_Devices;
    u8 u8_Index = 0;
    u8 u8_IndexUnit = 0;
    stIe_Devices.pst_DeviceEntries = arrSt_Devices;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceTableBriefGet, &stIe_Devices);

    APPCMBS_LOG("Number of Han entries: %d, Index of 1st entry: %d", stIe_Devices.u16_NumOfEntries, stIe_Devices.u16_StartEntryIndex);
    for (u8_Index = 0; u8_Index < stIe_Devices.u16_NumOfEntries; u8_Index++)
    {
        ST_HAN_BRIEF_DEVICE_INFO *pst_HanDevice = &arrSt_Devices[u8_Index];
        APPCMBS_LOG("\n");
        APPCMBS_LOG("Device Id   : %d \n", pst_HanDevice->u16_DeviceId);
        APPCMBS_LOG("Reg Status  : %d \n", pst_HanDevice->u8_RegistrationStatus);
		switch (pst_HanDevice->u8_RegistrationStatus)
	    {
	    case CMBS_HAN_DM_DEVICE_REGISTERED_2ND_PHASE_PVC_RESET_COMPLETED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Completed ULE Reg", pst_HanDevice->u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_REGISTERED_3RD_PHASE:
	         APPCMBS_LOG("Reg Status  : %s (%d) \n", "Registered", pst_HanDevice->u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_BLACK_LISTED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Black-listed", pst_HanDevice->u8_RegistrationStatus);
	        break;
	
	    default:
	        APPCMBS_LOG("Unknown Reg Status\n");
	        break;
	    }
				
        APPCMBS_LOG("RequestedPageTime  : %d (0 = non-pagable)\n", pst_HanDevice->u16_RequestedPageTime);
        APPCMBS_LOG("Page Interv : %d \n", pst_HanDevice->u16_PageTime);
        APPCMBS_LOG("# of Units  : %d", pst_HanDevice->u8_NumberOfUnits);
        for (u8_IndexUnit = 0; u8_IndexUnit < pst_HanDevice->u8_NumberOfUnits; u8_IndexUnit++)
        {
            APPCMBS_LOG("\n");
            APPCMBS_LOG("Unit %d Type : 0x%x", pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u8_UnitId, pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_UnitType);
        }
        APPCMBS_LOG("\n\n");
    }
}


/////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_ENTRIES)
{
    ST_HAN_EXTENDED_DEVICE_INFO 		arrSt_Devices[CMBS_HAN_MAX_DEVICES];
    ST_IE_HAN_EXTENDED_DEVICE_ENTRIES 	stIe_Devices;
    u8 u8_Index = 0;
    u8 u8_IndexUnit = 0;
    u8 u8_IndexInterface = 0;

    stIe_Devices.pst_DeviceEntries = arrSt_Devices;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceTableExtendedGet, &stIe_Devices);

    APPCMBS_LOG("Number of Han entries: %d, Index of 1st entry: %d", stIe_Devices.u16_NumOfEntries, stIe_Devices.u16_StartEntryIndex);
    for (u8_Index = 0; u8_Index < stIe_Devices.u16_NumOfEntries; u8_Index++)
    {
        ST_HAN_EXTENDED_DEVICE_INFO *pst_HanDevice = &arrSt_Devices[u8_Index];

        APPCMBS_LOG("\n");
        APPCMBS_LOG("Device Id   : %d \n", pst_HanDevice->u16_DeviceId);
        APPCMBS_LOG("IPUI   	: %02X%02X%02X%02X%02X \n",	pst_HanDevice->u8_IPUI[0],
                    pst_HanDevice->u8_IPUI[1],
                    pst_HanDevice->u8_IPUI[2],
                    pst_HanDevice->u8_IPUI[3],
                    pst_HanDevice->u8_IPUI[4]);
        switch (pst_HanDevice->u8_RegistrationStatus)
	    {
	    case CMBS_HAN_DM_DEVICE_REGISTERED_2ND_PHASE_PVC_RESET_COMPLETED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Completed ULE Reg", pst_HanDevice->u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_REGISTERED_3RD_PHASE:
	         APPCMBS_LOG("Reg Status  : %s (%d) \n", "Registered", pst_HanDevice->u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_BLACK_LISTED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Black-listed", pst_HanDevice->u8_RegistrationStatus);
	        break;
	
	    default:
	        APPCMBS_LOG("Unknown Reg Status\n");
	        break;
	    }
        APPCMBS_LOG("RequestedPageTime  : %d (0 = non-pagable)\n", pst_HanDevice->u16_RequestedPageTime);
        APPCMBS_LOG("Page Interv : %d \n", pst_HanDevice->u16_PageTime);
        APPCMBS_LOG("Device EMC : 0x%02X \n", pst_HanDevice->u16_DeviceEMC);

        APPCMBS_LOG("# of Units  : %d\n", pst_HanDevice->u8_NumberOfUnits);
        for (u8_IndexUnit = 0; u8_IndexUnit < pst_HanDevice->u8_NumberOfUnits; u8_IndexUnit++)
        {
            APPCMBS_LOG("\n");
            APPCMBS_LOG("Unit %d Type : 0x%x\n", pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u8_UnitId, pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_UnitType);
            for (u8_IndexInterface = 0; u8_IndexInterface < pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_NumberOfOptionalInterfaces; u8_IndexInterface++)
            {
				if ((pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_TYPE_MASK) == INTERFACE_TYPE_MASK)
				{
					APPCMBS_LOG("\n\tInterfaces %d ID : 0x%x (Server)", u8_IndexInterface + 1, pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_ID_MASK);
				}
				else
				{
					APPCMBS_LOG("\n\tInterfaces %d ID : 0x%x (Client)", u8_IndexInterface + 1, pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_ID_MASK);
				}
            }
            APPCMBS_LOG("\n\n");

        }
    }
}


CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_PHASE_2_ENTRIES)
{
    ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 arrSt_Devices[CMBS_HAN_MAX_DEVICES];
    ST_IE_HAN_EXTENDED_DEVICE_PHASE_2_ENTRIES stIe_Devices;
    u8 u8_Index = 0;
    u8 u8_IndexUnit = 0;
    u8 u8_IndexInterface = 0;

    stIe_Devices.pst_DeviceEntries = arrSt_Devices;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceTableExtendedPhase2Get, &stIe_Devices);

    APPCMBS_LOG("Number of Han entries: %d, Index of 1st entry: %d", stIe_Devices.u16_NumOfEntries, stIe_Devices.u16_StartEntryIndex);
    for (u8_Index = 0; u8_Index < stIe_Devices.u16_NumOfEntries; u8_Index++)
    {
        ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2* pst_HanDevice = &arrSt_Devices[u8_Index];

        APPCMBS_LOG("\n");
        APPCMBS_LOG("Device Id   : %d \n", pst_HanDevice->u16_DeviceId);
        APPCMBS_LOG("IPUI   	: %02X%02X%02X%02X%02X \n",	pst_HanDevice->u8_IPUI[0],
                    pst_HanDevice->u8_IPUI[1],
                    pst_HanDevice->u8_IPUI[2],
                    pst_HanDevice->u8_IPUI[3],
                    pst_HanDevice->u8_IPUI[4]);
        switch (pst_HanDevice->u8_RegistrationStatus)
	    {
	    case CMBS_HAN_DM_DEVICE_REGISTERED_2ND_PHASE_PVC_RESET_COMPLETED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Completed ULE Reg", pst_HanDevice->u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_REGISTERED_3RD_PHASE:
	         APPCMBS_LOG("Reg Status  : %s (%d) \n", "Registered", pst_HanDevice->u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_BLACK_LISTED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Black-listed", pst_HanDevice->u8_RegistrationStatus);
	        break;
	
	    default:
	        APPCMBS_LOG("Unknown Reg Status\n");
	        break;
	    }
        APPCMBS_LOG("RequestedPageTime  : %d (0 = non-pagable)\n", pst_HanDevice->u16_RequestedPageTime);
        APPCMBS_LOG("Page Interv : %d \n", pst_HanDevice->u16_PageTime);
        APPCMBS_LOG("Device EMC : 0x%02X \n", pst_HanDevice->u16_DeviceEMC);

		switch (pst_HanDevice->u8_ULECapabilities)
	    {
	    case CMBS_HAN_CAPABILITIES_ULE_NOT_SUPPORTED:
	        APPCMBS_LOG("ULE Capabilities         : not supported (%d)\n", pst_HanDevice->u8_ULECapabilities);
	        break;

	    case CMBS_HAN_CAPABILITIES_ULE_PHASE_1:
	        APPCMBS_LOG("ULE capabilities         : Phase 1 support (%d)\n", pst_HanDevice->u8_ULECapabilities);
	        break;

	    case CMBS_HAN_CAPABILITIES_ULE_PHASE_1_2:
	        APPCMBS_LOG("ULE capabilities         : Phase 1.2 support  (%d)\n", pst_HanDevice->u8_ULECapabilities);
	        break;

	    case CMBS_HAN_CAPABILITIES_ULE_PHASE_2:
	        APPCMBS_LOG("ULE capabilities         : Phase 2 support  (%d)\n", pst_HanDevice->u8_ULECapabilities);
			break;

	    case CMBS_HAN_CAPABILITIES_ULE_PHASE_3:
	        APPCMBS_LOG("ULE capabilities         : Phase 3 support (%d)\n", pst_HanDevice->u8_ULECapabilities);
	        break;

			
	    default:
	        APPCMBS_LOG("Unknown Capabilities value\n");
	        break;
	    }

		switch (pst_HanDevice->u8_UleAppProtocolId)
	    {
	    
	    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ID_UNDEFINED:
	        APPCMBS_LOG("ULE app protocol Id      : undefined\n");
	        break;
	    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_1:
	        APPCMBS_LOG("ULE app protocol Id      : ULE HAN-FUN Protocol Version 1\n");
	        break;	
	    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_2:
	        APPCMBS_LOG("ULE app protocol Id      : ULE HAN-FUN Protocol Version 2\n");
	        break;
	    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_PROPRIETARY_MASK:
	        APPCMBS_LOG("ULE app protocol Id      : Proprietary\n");
	        break;
	    default:
	        APPCMBS_LOG("Unknown protocol id\n");
	        break;
	    }

		switch (pst_HanDevice->u8_UleAppProtocolVersion)
	    {
	    
	    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_UNDEFINED:
	        APPCMBS_LOG("ULE app protocol version : undefined\n\n");
	        break;
	    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_0:
	        APPCMBS_LOG("ULE app protocol version : FUN version 1.0\n\n");
	        break;	
	    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_4:
	        APPCMBS_LOG("ULE app protocol version : FUN version 1.4\n\n");
	        break;
	    default:
	        APPCMBS_LOG("ULE app protocol version : Unknown\n\n");
	        break;
	    }

        APPCMBS_LOG("# of Units  : %d\n", pst_HanDevice->u8_NumberOfUnits);
        for (u8_IndexUnit = 0; u8_IndexUnit < pst_HanDevice->u8_NumberOfUnits; u8_IndexUnit++)
        {
            APPCMBS_LOG("\n");
            APPCMBS_LOG("Unit %d Type : 0x%x\n", pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u8_UnitId, pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_UnitType);
            for (u8_IndexInterface = 0; u8_IndexInterface < pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_NumberOfOptionalInterfaces; u8_IndexInterface++)
            {
				if ((pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_TYPE_MASK) == INTERFACE_TYPE_MASK)
				{
					APPCMBS_LOG("\n\tInterfaces %d ID : 0x%x (Server)", u8_IndexInterface + 1, pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_ID_MASK);
				}
				else
				{
					APPCMBS_LOG("\n\tInterfaces %d ID : 0x%x (Client)", u8_IndexInterface + 1, pst_HanDevice->st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_ID_MASK);
				}
            }
            APPCMBS_LOG("\n\n");

        }
    }
}

/////////////////////////////////////////////////////////////////////////


CMBS_PARSE_DECL(CMBS_IE_HAN_FUN_DEVICE_INFO)
{
	ST_FUN_DEVICE_INFO stIe_FunDevInfo;
    u8 u8_IndexUnit = 0;
    u8 u8_IndexInterface = 0;
	memset(&stIe_FunDevInfo, 0, sizeof(stIe_FunDevInfo));

    CMBS_SAFE_GET_IE(cmbs_api_ie_HanFUNDeviceInfoGet, &stIe_FunDevInfo);

    APPCMBS_LOG("\n");
    APPCMBS_LOG("Device Id   : %d \n", stIe_FunDevInfo.DeviceId);
    APPCMBS_LOG("Device EMC : 0x%02X \n", stIe_FunDevInfo.DeviceEMC);
    APPCMBS_LOG("# of Units  : %d\n", stIe_FunDevInfo.NumOfUnits);
    for (u8_IndexUnit = 0; u8_IndexUnit < stIe_FunDevInfo.NumOfUnits; u8_IndexUnit++)
    {
        APPCMBS_LOG("\n");
        APPCMBS_LOG("Unit %d Type : 0x%x\n", stIe_FunDevInfo.Units[u8_IndexUnit].UnitId, stIe_FunDevInfo.Units[u8_IndexUnit].UnitType);
        for (u8_IndexInterface = 0; u8_IndexInterface < stIe_FunDevInfo.Units[u8_IndexUnit].NumberOfOptionalInterfaces; u8_IndexInterface++)
        {
            APPCMBS_LOG("\n\tInterfaces %d Type : 0x%x", u8_IndexInterface + 1, stIe_FunDevInfo.Units[u8_IndexUnit].OptionalInterfaces[u8_IndexInterface] & 0x7fff);
        }
        APPCMBS_LOG("\n\n");

    }
}

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO)
{
    ST_HAN_EXTENDED_DEVICE_INFO 		stIe_Device;

    u8 u8_IndexUnit = 0;
    u8 u8_IndexInterface = 0;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceSubInfoGet, &stIe_Device);

    APPCMBS_LOG("\n");
    APPCMBS_LOG("Device Id   : %d \n", stIe_Device.u16_DeviceId);
    APPCMBS_LOG("IPUI   	: %02X%02X%02X%02X%02X \n",	stIe_Device.u8_IPUI[0],
                stIe_Device.u8_IPUI[1],
                stIe_Device.u8_IPUI[2],
                stIe_Device.u8_IPUI[3],
                stIe_Device.u8_IPUI[4]);
	
    switch (stIe_Device.u8_RegistrationStatus)
	    {
	    case CMBS_HAN_DM_DEVICE_REGISTERED_2ND_PHASE_PVC_RESET_COMPLETED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Completed ULE Reg", stIe_Device.u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_REGISTERED_3RD_PHASE:
	         APPCMBS_LOG("Reg Status  : %s (%d) \n", "Registered", stIe_Device.u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_BLACK_LISTED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Black-listed", stIe_Device.u8_RegistrationStatus);
	        break;
	
	    default:
	        APPCMBS_LOG("Unknown Reg Status\n");
	        break;
	    }
    APPCMBS_LOG("RequestedPageTime  : %d (0 = non-pagable)\n", stIe_Device.u16_RequestedPageTime);
    APPCMBS_LOG("Page Interv : %d \n", stIe_Device.u16_PageTime);
    APPCMBS_LOG("Device EMC : 0x%02X \n", stIe_Device.u16_DeviceEMC);


		
    APPCMBS_LOG("# of Units  : %d\n", stIe_Device.u8_NumberOfUnits);
    for (u8_IndexUnit = 0; u8_IndexUnit < stIe_Device.u8_NumberOfUnits; u8_IndexUnit++)
    {
        APPCMBS_LOG("\n");
        APPCMBS_LOG("Unit %d Type : 0x%x\n", stIe_Device.st_UnitsInfo[u8_IndexUnit].u8_UnitId, stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_UnitType);
        for (u8_IndexInterface = 0; u8_IndexInterface < stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_NumberOfOptionalInterfaces; u8_IndexInterface++)
        {
			if ((stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_TYPE_MASK) == INTERFACE_TYPE_MASK)
				{
					APPCMBS_LOG("\n\tInterfaces %d Type : 0x%x (Server)", u8_IndexInterface + 1, stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & 0x7fff);
				}
				else
				{
					APPCMBS_LOG("\n\tInterfaces %d Type : 0x%x (Client)", u8_IndexInterface + 1, stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & 0x7fff);
				}
		
        }
        APPCMBS_LOG("\n\n");

    }
}

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_PHASE_2_INFO)
{
    ST_HAN_EXTENDED_DEVICE_INFO_PHASE_2 		stIe_Device;

    u8 u8_IndexUnit = 0;
    u8 u8_IndexInterface = 0;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceSubInfoPhase2Get, &stIe_Device);

    APPCMBS_LOG("\n");
    APPCMBS_LOG("Device Id   : %d \n", stIe_Device.u16_DeviceId);
    APPCMBS_LOG("IPUI   	: %02X%02X%02X%02X%02X \n",	stIe_Device.u8_IPUI[0],
                stIe_Device.u8_IPUI[1],
                stIe_Device.u8_IPUI[2],
                stIe_Device.u8_IPUI[3],
                stIe_Device.u8_IPUI[4]);
    switch (stIe_Device.u8_RegistrationStatus)
	    {
	    case CMBS_HAN_DM_DEVICE_REGISTERED_2ND_PHASE_PVC_RESET_COMPLETED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Completed ULE Reg", stIe_Device.u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_REGISTERED_3RD_PHASE:
	         APPCMBS_LOG("Reg Status  : %s (%d) \n", "Registered", stIe_Device.u8_RegistrationStatus);
	        break;

	    case CMBS_HAN_DM_DEVICE_BLACK_LISTED:
	        APPCMBS_LOG("Reg Status  : %s (%d) \n", "Black-listed", stIe_Device.u8_RegistrationStatus);
	        break;
	
	    default:
	        APPCMBS_LOG("Unknown Reg Status\n");
	        break;
	    }
    APPCMBS_LOG("RequestedPageTime  : %d (0 = non-pagable)\n", stIe_Device.u16_RequestedPageTime);
    APPCMBS_LOG("Page Interv : %d \n", stIe_Device.u16_PageTime);
    APPCMBS_LOG("Device EMC : 0x%02X \n", stIe_Device.u16_DeviceEMC);

	switch (stIe_Device.u8_ULECapabilities)
    {
    case CMBS_HAN_CAPABILITIES_ULE_NOT_SUPPORTED:
        APPCMBS_LOG("ULE Capabilities         : not supported (%d)\n", stIe_Device.u8_ULECapabilities);
        break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_1:
        APPCMBS_LOG("ULE capabilities         : Phase 1 support (%d)\n", stIe_Device.u8_ULECapabilities);
        break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_1_2:
        APPCMBS_LOG("ULE capabilities         : Phase 1.2 support  (%d)\n", stIe_Device.u8_ULECapabilities);
        break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_2:
        APPCMBS_LOG("ULE capabilities         : Phase 2 support  (%d)\n", stIe_Device.u8_ULECapabilities);
		break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_3:
        APPCMBS_LOG("ULE capabilities         : Phase 3 support (%d)\n", stIe_Device.u8_ULECapabilities);
        break;
		
    default:
        APPCMBS_LOG("Unknown Capabilities value\n");
        break;
    }

	switch (stIe_Device.u8_UleAppProtocolId)
    {
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ID_UNDEFINED:
        APPCMBS_LOG("ULE app protocol Id      : undefined\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_1:
        APPCMBS_LOG("ULE app protocol Id      : ULE HAN-FUN Protocol Version 1\n");
        break;	
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_2:
        APPCMBS_LOG("ULE app protocol Id      : ULE HAN-FUN Protocol Version 2\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_PROPRIETARY_MASK:
        APPCMBS_LOG("ULE app protocol Id      : Proprietary\n");
        break;
    default:
        APPCMBS_LOG("Unknown protocol id\n");
        break;
    }

	switch (stIe_Device.u8_UleAppProtocolVersion)
    {
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_UNDEFINED:
        APPCMBS_LOG("ULE app protocol version : undefined\n\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_0:
        APPCMBS_LOG("ULE app protocol version : FUN version 1.0\n\n");
        break;	
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_4:
        APPCMBS_LOG("ULE app protocol version : FUN version 1.4\n\n");
        break;
    default:
        APPCMBS_LOG("ULE app protocol version : Unknown\n\n");
        break;
    }
		
    APPCMBS_LOG("# of Units  : %d\n", stIe_Device.u8_NumberOfUnits);
    for (u8_IndexUnit = 0; u8_IndexUnit < stIe_Device.u8_NumberOfUnits; u8_IndexUnit++)
    {
        APPCMBS_LOG("\n");
        APPCMBS_LOG("Unit %d Type : 0x%x\n", stIe_Device.st_UnitsInfo[u8_IndexUnit].u8_UnitId, stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_UnitType);
        for (u8_IndexInterface = 0; u8_IndexInterface < stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_NumberOfOptionalInterfaces; u8_IndexInterface++)
        {
			if ((stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & INTERFACE_TYPE_MASK) == INTERFACE_TYPE_MASK)
				{
					APPCMBS_LOG("\n\tInterfaces %d Type : 0x%x (Server)", u8_IndexInterface + 1, stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & 0x7fff);
				}
				else
				{
					APPCMBS_LOG("\n\tInterfaces %d Type : 0x%x (Client)", u8_IndexInterface + 1, stIe_Device.st_UnitsInfo[u8_IndexUnit].u16_OptionalInterfaces[u8_IndexInterface] & 0x7fff);
				}
		
        }
        APPCMBS_LOG("\n\n");

    }
}

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_FULL_REGISTRATION_INFO)
{
    ST_HAN_FULL_DEVICE_REGISTRATION_INFO stIe_DeviceRegInfo;
    u8 u8_Index_1=0, u8_Index_2 = 0;
	
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceFullRegistrationInfoGet, &stIe_DeviceRegInfo);

    APPCMBS_LOG("\n");

    APPCMBS_LOG("DECT Information \n");
 APPCMBS_LOG("=================\n"); 
    APPCMBS_LOG("\tIPUI:  %02X %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Ipui[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Ipui[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Ipui[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Ipui[3],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Ipui[4]);	

APPCMBS_LOG("\tTPUI:  %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Tpui[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Tpui[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Tpui[2]);

    APPCMBS_LOG("\tDCK:  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[3],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[4],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[5],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[6],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[7],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[8],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[9],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[10],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[11],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[12],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[13],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[14],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Dck[15]);	

    APPCMBS_LOG("\tUAK:  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[3],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[4],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[5],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[6],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[7],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[8],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[9],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[10],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[11],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[12],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[13],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[14],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Uak[15]);	
	
    APPCMBS_LOG("\tAuthentication Code:  %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_AuthenticationCode[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_AuthenticationCode[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_AuthenticationCode[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_AuthenticationCode[3]);


APPCMBS_LOG("\tUakAuth:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_UakAuth);
APPCMBS_LOG("\tStatus:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_Status);	
APPCMBS_LOG("\tHs Number:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_HsNumber);
APPCMBS_LOG("\tDck Assigned:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_DckAssigned);
APPCMBS_LOG("\tCk Len:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_CkLen);
APPCMBS_LOG("\tFeatures:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_Features);

    APPCMBS_LOG("\tDef CK:  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[3],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[4],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[5],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[6],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[7],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[8],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[9],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[10],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[11],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[12],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[13],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[14],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Defck[15]);	
	
	APPCMBS_LOG("\tDefck Assigned:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_DefckAssigned);

    APPCMBS_LOG("\tCCM CK:  %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[3],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[4],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[5],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[6],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[7],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[8],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[9],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[10],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[11],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[12],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[13],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[14],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_CcmCK[15]);	

    APPCMBS_LOG("\tDLC Operating Mode:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_DlcOperatingMode);

    APPCMBS_LOG("\tHL Features:  %02X\n", stIe_DeviceRegInfo.st_DECT_RegistrationInfo.u8_HlFeatures);


	APPCMBS_LOG("\tReserved:  %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Reserved[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Reserved[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Reserved[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_Reserved[3]);


	APPCMBS_LOG("\tRX Sequence Number:  %02X %02X %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[3],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[4],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextRxSequenceNumber[5]);
	
	APPCMBS_LOG("\tTX Sequence Number:  %02X %02X %02X %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[0],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[1],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[2],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[3],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[4],
				stIe_DeviceRegInfo.st_DECT_RegistrationInfo.pu8_NextTxSequenceNumber[5]);
	
 APPCMBS_LOG("\n");
 APPCMBS_LOG("ULE Information \n");	
 APPCMBS_LOG("=================\n"); 
 APPCMBS_LOG("\tDevice Id:  %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.DeviceId);
 APPCMBS_LOG("\tRegistration Status:  %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.RegistrationStatus);
 APPCMBS_LOG("\tRequested Page Time:  %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.RequestedPageTime);
 APPCMBS_LOG("\tNegotiated Page Time:  %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.NegotiatedPageTime);
 APPCMBS_LOG("\tMax SDU Size:  %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.u16_MaxSduSize);
 APPCMBS_LOG("\tNum App Protocols:  %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.u8_NumAppProtocols);

    APPCMBS_LOG("\n");
    for (u8_Index_1 = 0; u8_Index_1 <CMBS_HAN_ULE_APP_PROTOCOLS; u8_Index_1++)
    	{
		APPCMBS_LOG("\tProtocol %d info:\n", u8_Index_1+1);
		APPCMBS_LOG("\t\tULE App Protocol Id: %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].u8_UleAppProtocolId);
		APPCMBS_LOG("\t\tULE Capabilities: %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].u8_UleCapabilities);
		APPCMBS_LOG("\t\tIs Proprietary Protocol Id: %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].u8_IsProprietaryProtocolId);
		APPCMBS_LOG("\t\tReserved: %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].u8_Reserved);
		APPCMBS_LOG("\t\tULE App Protocol Version: %02X\n", stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].u8_UleAppProtocolVersion);
		APPCMBS_LOG("\t\tEMC or Extended data:  %02X %02X %02X\n",	
				stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].pu8_EmcOrExtendedData[0],
				stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].pu8_EmcOrExtendedData[1],
				stIe_DeviceRegInfo.st_ULE_RegistrationInfo.st_AppProtocols[u8_Index_1].pu8_EmcOrExtendedData[2]);
    	}
 
  APPCMBS_LOG("\n");
  APPCMBS_LOG("FUN Information \n");	
 APPCMBS_LOG("=================\n"); 
  APPCMBS_LOG("\tDevice Id:  %02X\n", stIe_DeviceRegInfo.st_FUN_RegistrationInfo.DeviceId);	
  APPCMBS_LOG("\tDevice EMC: %02X\n", stIe_DeviceRegInfo.st_FUN_RegistrationInfo.DeviceEMC);	
  APPCMBS_LOG("\tNum of units:%02X\n", stIe_DeviceRegInfo.st_FUN_RegistrationInfo.NumOfUnits);	

  for (u8_Index_1 = 0; u8_Index_1 <CMBS_HAN_MAX_UNITS_IN_DEVICE; u8_Index_1++)
  	{
  		 APPCMBS_LOG("\n");
  		APPCMBS_LOG("\t\tUnit Id:  %02X\n", stIe_DeviceRegInfo.st_FUN_RegistrationInfo.Units[u8_Index_1].u8_UnitId);
		APPCMBS_LOG("\t\tUnit Type: %02X\n", stIe_DeviceRegInfo.st_FUN_RegistrationInfo.Units[u8_Index_1].u16_UnitType);
		APPCMBS_LOG("\t\tNumber Of Optional Interfaces:  %02X\n", stIe_DeviceRegInfo.st_FUN_RegistrationInfo.Units[u8_Index_1].u16_NumberOfOptionalInterfaces);

		  for (u8_Index_2 = 0; u8_Index_2 <CMBS_HAN_MAX_OPTIONAL_INTERFACES_IN_UNIT; u8_Index_2++)
		  	{
				APPCMBS_LOG("\t\t\tInterface%d: %02X\n", u8_Index_2+1, stIe_DeviceRegInfo.st_FUN_RegistrationInfo.Units[u8_Index_1].u16_OptionalInterfaces[u8_Index_2]);
		  	}
  	}

}
/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_BIND_TABLE_ENTRIES)
{
    ST_HAN_BIND_ENTRY		arrSt_Binds[CMBS_HAN_MAX_BINDS];
    ST_IE_HAN_BIND_ENTRIES	stIe_Binds;
    u8 u8_Index = 0;
    stIe_Binds.pst_BindEntries = arrSt_Binds;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANBindTableGet, &stIe_Binds);

    APPCMBS_LOG("Number of Han entries: %d, Index of 1st entry: %d", stIe_Binds.u16_NumOfEntries, stIe_Binds.u16_StartEntryIndex);
    for (u8_Index = 0; u8_Index < stIe_Binds.u16_NumOfEntries; u8_Index++)
    {
        ST_HAN_BIND_ENTRY *pst_HanBind = &arrSt_Binds[u8_Index];

	 APPCMBS_LOG("\n");
        APPCMBS_LOG("Src Device Id		:  %d  (0x%02X)\n", pst_HanBind->u16_SrcDeviceID, pst_HanBind->u16_SrcDeviceID);
        APPCMBS_LOG("Src Device Unit Id		:  %d  (0x%02X)\n", pst_HanBind->u8_SrcUnitID, pst_HanBind->u8_SrcUnitID);
        APPCMBS_LOG("Dst Device Id		:  %d  (0x%02X)\n", pst_HanBind->u16_DstDeviceID, pst_HanBind->u16_DstDeviceID);
        APPCMBS_LOG("Dst Device Unit Id		:  %d  (0x%02X)\n", pst_HanBind->u8_DstUnitID, pst_HanBind->u8_DstUnitID);
        APPCMBS_LOG("Interface  Id		:  0x%02X\n", pst_HanBind->u16_DstInterfaceID);
        APPCMBS_LOG("Interface  Type		:  %d    [0-client, 1-server]\n", pst_HanBind->u8_DstInterfaceType);
        APPCMBS_LOG("Address Type		:  %d    [0-individual, 1-group]\n", pst_HanBind->u8_DstAddressType);
    }
}
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_BIND_TABLE_ENTRY)
{
    ST_HAN_BIND_ENTRY		st_HanBind;

	CMBS_SAFE_GET_IE(cmbs_api_ie_HANBindEntryGet, &st_HanBind);

    APPCMBS_LOG("\n");
    APPCMBS_LOG("Src Device Id		:  %d  (0x%02X)\n", st_HanBind.u16_SrcDeviceID, st_HanBind.u16_SrcDeviceID);
    APPCMBS_LOG("Src Device Unit Id		:  %d  (0x%02X)\n", st_HanBind.u8_SrcUnitID, st_HanBind.u8_SrcUnitID);
    APPCMBS_LOG("Dst Device Id		:  %d  (0x%02X)\n", st_HanBind.u16_DstDeviceID, st_HanBind.u16_DstDeviceID);
    APPCMBS_LOG("Dst Device Unit Id		:  %d  (0x%02X)\n", st_HanBind.u8_DstUnitID, st_HanBind.u8_DstUnitID);
    APPCMBS_LOG("Interface  Id		:  0x%02X\n", st_HanBind.u16_DstInterfaceID);
    APPCMBS_LOG("Interface  Type		:  %d    [0-client, 1-server]\n", st_HanBind.u8_DstInterfaceType);
    APPCMBS_LOG("Address Type		:  %d    [0-individual, 1-group]\n", st_HanBind.u8_DstAddressType);

}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


CMBS_PARSE_DECL(CMBS_IE_HAN_GROUP_ID)
{
	u16 u16_GroupId;
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANGroupIdGet, &u16_GroupId);
	APPCMBS_LOG("Group Id = %d ", u16_GroupId);
}

CMBS_PARSE_DECL(CMBS_IE_HAN_UNIT_ID)
{
	u8 u8_UnitId;
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANUnitIdGet, &u8_UnitId);
	APPCMBS_LOG("Unit Id = %d ", u8_UnitId);
}

CMBS_PARSE_DECL(CMBS_IE_HAN_GROUP_COMMAND_FAIL_REASON)
{
    u8	u8_Status;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANGroupCommandFailureReasonGet, &u8_Status);

    APPCMBS_LOG("Group Command Failure: :");

    switch (u8_Status)
    {
    
    case CMBS_HAN_GM_GENERAL_RES_UNKNOWN_FAILURE:
        APPCMBS_LOG(" Unknown Failure");
        break;
    case CMBS_HAN_GM_CREATE_GROUP_RES_NO_FREE_SPACE:
        APPCMBS_LOG(" No free space");
        break;	
    case CMBS_HAN_GM_CREATE_GROUP_RES_BC_CHANNEL_ALLOCATION_FAILURE:
        APPCMBS_LOG(" Broadcast channel allocation failure");
        break;

    case CMBS_HAN_GM_DELETE_GROUP_RES_GROUP_NOT_EXIST:
        APPCMBS_LOG(" Group does not exist");
        break;

	case CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_GROUP_NOT_EXIST:
        APPCMBS_LOG(" Group does not exist");
        break;

	case CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_DEVICE_NOT_REGISTERED:
        APPCMBS_LOG(" Device not registered");
        break;

	case CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_DEVICE_NOT_BROADCASTABLE:
		APPCMBS_LOG(" Device does not support broadcast");
        break;
		
	case CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_NO_FREE_SPACE:		
		APPCMBS_LOG(" No free space");
        break;
		
	case CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_DUPLICATED_RECORD: 
		APPCMBS_LOG(" Device-Unit is already part of group");
        break;

	case CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_GROUP_NOT_EXIST:
        APPCMBS_LOG(" Group does not exist");
        break;

	case CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_DEVICE_NOT_REGISTERED:
        APPCMBS_LOG(" Device not registered");
        break;

	case CMBS_HAN_GM_DELETE_GROUP_RES_GROUP_NOT_EMPTY:
	APPCMBS_LOG(" Group not empty!");
       break;

	case CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_DEVICE_NOT_INCLUDED_IN_GROUP:
		APPCMBS_LOG(" Device is not included in group");
		break;
		
	case CMBS_HAN_GM_SEND_ON_OFF_MSG_TO_GROUP_RES_GROUP_NOT_EXIST:
        APPCMBS_LOG(" Group does not exist");
        break;

	case CMBS_HAN_GM_READ_DEVICE_GROUP_LIST_RES_DEVICE_NOT_REGISTERED:
        APPCMBS_LOG(" Device not registered");
        break;

	case CMBS_HAN_GM_ADD_DEVICE_TO_GROUP_RES_BUSY:
	case CMBS_HAN_GM_REMOVE_DEVICE_FROM_GROUP_RES_BUSY:
        APPCMBS_LOG(" Busy with previous req");
        break;

	case CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_NOT_AUTHORIZED:
		APPCMBS_LOG(" Not Authorized");
        break;

	case CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_INVALID_ARGUMENT:
		APPCMBS_LOG(" Invalid Argument");
        break;

	case CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_NO_RESOURCE:
		APPCMBS_LOG(" No Resource");
        break;

	case CMBS_HAN_GM_COMMAND_RESPONSE_FROM_DEVICE_RES_UNKNOWN_ERR:
		APPCMBS_LOG(" Unknown Error");
        break;

    default:
        APPCMBS_LOG(" ??? \n");
        break;
    }

}

CMBS_PARSE_DECL(CMBS_IE_HAN_GROUP_TABLE_ENTRIES)
{
    ST_HAN_GROUP_TABLE_ENTRY		arrSt_Groups[CMBS_HAN_MAX_GROUPS];
    ST_IE_HAN_GROUP_ENTRIES	stIe_Groups;
    u8 u8_Index = 0;
    stIe_Groups.pst_GroupEntries = arrSt_Groups;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANGroupTableGet, &stIe_Groups);

    APPCMBS_LOG("Number of Group Table entries: %d, Index of 1st entry: %d\n", stIe_Groups.u16_NumOfEntries, stIe_Groups.u16_StartEntryIndex);
    for (u8_Index = 0; u8_Index < stIe_Groups.u16_NumOfEntries; u8_Index++)
    {
        ST_HAN_GROUP_TABLE_ENTRY *pst_HanGroup = &arrSt_Groups[u8_Index];

		APPCMBS_LOG("\n");
        APPCMBS_LOG("Group Id	: %d \n", pst_HanGroup->u16_GroupId);
        APPCMBS_LOG("Device Id	: %d \n", pst_HanGroup->u16_DeviceId);
        APPCMBS_LOG("Unit Id	: %d \n", pst_HanGroup->u8_UnitId);
	 	APPCMBS_LOG("------------------------------------\n");
    }
}

CMBS_PARSE_DECL(CMBS_IE_HAN_GROUP_LIST_ENTRIES)
{
    ST_HAN_GROUP_LIST_ENTRY			arrSt_GroupList[CMBS_HAN_MAX_GROUPS];
    ST_IE_HAN_GROUP_LIST_ENTRIES 	stIe_GroupList;
    u8 u8_Index = 0;
    stIe_GroupList.pst_GroupListEntries = arrSt_GroupList;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANGroupListGet, &stIe_GroupList);

    APPCMBS_LOG("Number of Group List entries: %d, Index of 1st entry: %d\n", stIe_GroupList.u16_NumOfEntries, stIe_GroupList.u16_StartEntryIndex);
	for (u8_Index = 0; u8_Index < stIe_GroupList.u16_NumOfEntries; u8_Index++)
    {
		ST_HAN_GROUP_LIST_ENTRY *pst_HanGroupInfo = &arrSt_GroupList[u8_Index];

		APPCMBS_LOG("\n");
        APPCMBS_LOG("Group Id		: %d \n", pst_HanGroupInfo->u16_GroupId);
        APPCMBS_LOG("# of Device		: %d \n", pst_HanGroupInfo->u16_NumOfDevices);
        APPCMBS_LOG("BC Channel		: %d \n", pst_HanGroupInfo->u8_Channel);
	 	APPCMBS_LOG("------------------------------------\n");
    }
}

CMBS_PARSE_DECL(CMBS_IE_HAN_BROADCAST_CHANNEL_TABLE_ENTRIES)
{

    ST_HAN_BROADCAST_CHANNEL_ENTRY	arrSt_BroadcastChannelTable[CMBS_HAN_MAX_DEVICES];
    ST_IE_HAN_BROADCAST_CHANNEL_ENTRIES	stIe_BroadcastChannelTable;
    u8 u8_Index = 0;

	stIe_BroadcastChannelTable.pst_broadcastChannelEntries = arrSt_BroadcastChannelTable;
	
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANBroadcastChannelTableGet, &stIe_BroadcastChannelTable);

    APPCMBS_LOG("Number of Broadcast Channel Table entries: %d, Index of 1st entry: %d\n", stIe_BroadcastChannelTable.u16_NumOfEntries, stIe_BroadcastChannelTable.u16_StartEntryIndex);

	for (u8_Index = 0; u8_Index < stIe_BroadcastChannelTable.u16_NumOfEntries; u8_Index++)
    {
		ST_HAN_BROADCAST_CHANNEL_ENTRY *pst_BroadcastChannelEntry = &arrSt_BroadcastChannelTable[u8_Index];

		APPCMBS_LOG("\n");
        APPCMBS_LOG("Channel Id		: %d \n", pst_BroadcastChannelEntry->u8_ChannelId);
		APPCMBS_LOG("Channel X Instance	: %d \n", pst_BroadcastChannelEntry->u16_ChannelXInst);
		APPCMBS_LOG("Start MFrame	: %d \n", pst_BroadcastChannelEntry->u8_ChannelStartMFrame);
		APPCMBS_LOG("Start MMFrame	: %d \n", pst_BroadcastChannelEntry->u8_ChannelStartMMFrame);
		APPCMBS_LOG("Periodicity		: %d \n", pst_BroadcastChannelEntry->u8_ChannelPeriodicity);
		

	 	APPCMBS_LOG("------------------------------------\n");
    }
}

/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE)
{

    CMBS_SAFE_GET_IE2(cmbs_api_ie_ShortValueGet, &u16_DeviceId, CMBS_IE_HAN_DEVICE);
    APPCMBS_LOG("Device Id = %d ", u16_DeviceId);
}

/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_CAPABILITIES)
{
    u16 u16_Capabilities;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANUleCapabilitiesGet, &u16_Capabilities);


	switch (u16_Capabilities)
    {
    case CMBS_HAN_CAPABILITIES_ULE_NOT_SUPPORTED:
        APPCMBS_LOG("ULE capabilities not supported (%d)", u16_Capabilities);
        break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_1:
        APPCMBS_LOG("ULE capabilities Phase 1 support (%d)", u16_Capabilities);
        break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_1_2:
        APPCMBS_LOG("ULE capabilities Phase 1.2 support  (%d)", u16_Capabilities);
        break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_2:
        APPCMBS_LOG("ULE capabilities Phase 2 support  (%d)", u16_Capabilities);
        break;

    case CMBS_HAN_CAPABILITIES_ULE_PHASE_3:
        APPCMBS_LOG("ULE capabilities Phase 3 support (%d)", u16_Capabilities);
        break;

		
    default:
        APPCMBS_LOG("Unknown Capabilities value ");
        break;
    }
}

/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_BROADCAST_CHANNEL_ID)
{
    u8 u8_BroadcastChannelId;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANBroadcastChannelIdGet, &u8_BroadcastChannelId);

	switch (u8_BroadcastChannelId)
    {
	    case CMBS_HAN_BROADCAST_SERVICE_CHANNEL_UKNOWN:
	        APPCMBS_LOG("Broadcast channel unknown - use default");
	        break;
			
	    default:
	        APPCMBS_LOG("Broadcast channel id = %d", u8_BroadcastChannelId);
	        break;
    }
}
/////////////////////////////////////////////////////////////////////////



CMBS_PARSE_DECL(CMBS_IE_HAN_LOG_LEVEL)
{
    u8 u8_LogLevel;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, &u8_LogLevel, CMBS_IE_HAN_LOG_LEVEL);

	//CMBS_HAN_LOG_LEVEL_INFO
	if (u8_LogLevel & CMBS_HAN_LOG_LEVEL_INFO)
		{		
    			APPCMBS_LOG("\tLOG INFO Enabled\n");
		}
	else
		{		
    			APPCMBS_LOG("\tLOG INFO Disabled\n");
		}

	//CMBS_HAN_LOG_LEVEL_TRACE
	if (u8_LogLevel & CMBS_HAN_LOG_LEVEL_TRACE)
		{		
    			APPCMBS_LOG("\tLOG TRACE Enabled\n");
		}
	else
		{		
    			APPCMBS_LOG("\tLOG TRACE Disabled\n");
		}

	//CMBS_HAN_LOG_LEVEL_ERROR
	if (u8_LogLevel & CMBS_HAN_LOG_LEVEL_ERROR)
		{		
    			APPCMBS_LOG("\tLOG ERROR Enabled\n");
		}
	else
		{		
    			APPCMBS_LOG("\tLOG ERROR Disabled\n");
		}
}


/////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_CFG)
{
    ST_IE_HAN_CONFIG stIe_HanCfg;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HanCfgGet, &stIe_HanCfg);

    if ( CMBS_HAN_DEVICE_MNGR(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("Device Mngr Ext\n");}
    else
    {APPCMBS_LOG("Device Mngr Int\n");}
    if ( CMBS_HAN_BIND_MNGR(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("Bind Mngr Ext\n");}
    else
    {APPCMBS_LOG("Bind Mngr Int\n");}
    if ( CMBS_HAN_BIND_LOOKUP(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("Bind Lookup Ext\n");}
    else
    {APPCMBS_LOG("Bind Lookup Int\n");}
    if ( CMBS_HAN_GROUP_MNGR(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("Group Mngr Ext\n");}
    else
    {APPCMBS_LOG("Group Mngr Int\n");}
    if ( CMBS_HAN_GROUP_LOOKUP(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("Group Lookup Ext\n");}
    else
    {APPCMBS_LOG("Group Lookup Int\n");}
	if ( CMBS_HAN_RELAY_MODE(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("Relay Mode Enabled\n");}
    else
    {APPCMBS_LOG("Relay Mode Disabled\n");}
	if ( CMBS_HAN_RAW_MODE(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("Raw Mode Enabled\n");}
    else
    {APPCMBS_LOG("Raw Mode Disabled\n");}   
	if ( CMBS_HAN_SSN_HOST_STORE_MODE(stIe_HanCfg.st_HanCfg.u8_HANServiceConfig) )
    {APPCMBS_LOG("HAN SSN Host Store Mode Enabled\n");}
    else
    {APPCMBS_LOG("HAN SSN Host Store Mode Disabled\n");}
	
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_NUM_OF_ENTRIES)
{
    u16 u16_NumOfEntries;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ShortValueGet, &u16_NumOfEntries, CMBS_IE_HAN_NUM_OF_ENTRIES);
    APPCMBS_LOG("Number of Han entries = %d ", u16_NumOfEntries);
}
//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_INDEX_1ST_ENTRY)
{
    u16 u16_IndexOfFirstEntry;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ShortValueGet, &u16_IndexOfFirstEntry, CMBS_IE_HAN_INDEX_1ST_ENTRY);
    APPCMBS_LOG("Index of first Han entry = %d ", u16_IndexOfFirstEntry);
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_TABLE_ENTRY_TYPE)
{
    u8 u8_EntryTableType;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ByteValueGet, &u8_EntryTableType, CMBS_IE_HAN_TABLE_ENTRY_TYPE);

    if ( u8_EntryTableType == CMBS_HAN_DEVICE_TABLE_ENTRY_TYPE_BRIEF )
    {
        APPCMBS_LOG("Table Entry Type = %s ", "BRIEF (0)");
    }
    else if ( u8_EntryTableType == CMBS_HAN_DEVICE_TABLE_ENTRY_TYPE_EXTENDED )
    {
        APPCMBS_LOG("Table Entry Type = %s ", "EXTENDED (1)");
    }
    else
    {
        APPCMBS_LOG("Table Entry Type = %s(%d) ", "UNKNOWN", u8_EntryTableType);
    }
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_GENERAL_STATUS)
{
    ST_HAN_GENERAL_STATUS st_Status;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANGeneralStatusGet, &st_Status);

    if ( st_Status.u16_Status  == CMBS_HAN_GENERAL_STATUS_OK )
    {
        APPCMBS_LOG(" Status = %s ", "SUCCESS (0)");
    }
    else if ( st_Status.u16_Status == CMBS_HAN_GENERAL_STATUS_ERROR )
    {
        APPCMBS_LOG("Status = %s ", "ERROR (1)");
    }
    else
    {
        APPCMBS_LOG("Status = %s(%d) ", "UNKNOWN", st_Status.u16_Status);
    }
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_SEND_FAIL_REASON)
{
    u16 u16_Reason;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANSendErrorReasonGet, &u16_Reason);


    switch (u16_Reason)
    {
    case CMBS_HAN_SEND_MSG_REASON_DATA_TOO_BIG_ERROR:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Data too big", CMBS_HAN_SEND_MSG_REASON_DATA_TOO_BIG_ERROR);
        break;

    case CMBS_HAN_SEND_MSG_REASON_DEVICE_NOT_IN_LINK_ERROR:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Device not in link ", CMBS_HAN_SEND_MSG_REASON_DEVICE_NOT_IN_LINK_ERROR);
        break;

    case CMBS_HAN_SEND_MSG_REASON_TRANSMIT_FAILED_ERROR:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Transmit failed", CMBS_HAN_SEND_MSG_REASON_TRANSMIT_FAILED_ERROR);
        break;

    case CMBS_HAN_SEND_MSG_REASON_DECT_ERROR:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Dect Failure", CMBS_HAN_SEND_MSG_REASON_DECT_ERROR);
        break;

    case CMBS_HAN_SEND_MSG_REASON_BUSY_WITH_PREVIOUS_MESSAGES:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Busy with prev msg", CMBS_HAN_SEND_MSG_REASON_BUSY_WITH_PREVIOUS_MESSAGES);
        break;

    case CMBS_HAN_SEND_MSG_REASON_INVALID_DST_DEVICE_LIST:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Invalid dst device list", CMBS_HAN_SEND_MSG_REASON_INVALID_DST_DEVICE_LIST);
        break;

    case CMBS_HAN_SEND_MSG_REASON_NO_TX_REQUEST:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "No Tx request exists", CMBS_HAN_SEND_MSG_REASON_NO_TX_REQUEST);
        break;

    case CMBS_HAN_SEND_MSG_REASON_UNKNOWN_ERROR:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Unknown Failure", CMBS_HAN_SEND_MSG_REASON_UNKNOWN_ERROR);
        break;

    case CMBS_HAN_SEND_MSG_REASON_NO_FREE_SPACE:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "No free space", CMBS_HAN_SEND_MSG_REASON_NO_FREE_SPACE);
        break;

	case CMBS_HAN_SEND_MSG_REASON_GROUP_EMPTY:
		APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Group is empty", CMBS_HAN_SEND_MSG_REASON_GROUP_EMPTY);
        break;

	case CMBS_HAN_SEND_MSG_REASON_GROUP_NOT_EXIST:
		APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Group doesn't exist", CMBS_HAN_SEND_MSG_REASON_GROUP_NOT_EXIST);
        break;

	case CMBS_HAN_SEND_MSG_REASON_ULE_SDU_TOO_BIG:
		APPCMBS_LOG("Send Failed Reason = %s(%d) ", "SDU size too big", CMBS_HAN_SEND_MSG_REASON_ULE_SDU_TOO_BIG);
        break;

	case CMBS_HAN_SEND_MSG_REASON_BUSY_WITH_PREV_MSG:
		APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Busy with previous request", CMBS_HAN_SEND_MSG_REASON_BUSY_WITH_PREV_MSG);
        break;
	
	case CMBS_HAN_SEND_MSG_REASON_ZERO_DATA_LEN:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Data length is zero", CMBS_HAN_SEND_MSG_REASON_ZERO_DATA_LEN);
        break;
		
    default:
        APPCMBS_LOG("Send Failed Reason = %s(%d) ", "Unknown Failure ", u16_Reason);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_REG_ERROR_REASON)
{
    u16 u16_Reason;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANRegErrorReasonGet, &u16_Reason);

    switch (u16_Reason)
    {
    case CMBS_HAN_REG_FAILED_REASON_DECT_REG_FAILED:
        APPCMBS_LOG("Reg Failure Reason : Did not proceed to Service Call\n");
        break;
    case CMBS_HAN_REG_FAILED_REASON_PARAMS_NEGOTIATION_FAILURE:
        APPCMBS_LOG("Reg Failure Reason : App Params Negotiation Failure \n");
        break;
    case CMBS_HAN_REG_FAILED_REASON_COULD_NOT_FINISH_PVC_RESET:
        APPCMBS_LOG("Reg Failure Reason : Could not finish PVC Reset\n");
        break;
    case CMBS_HAN_REG_FAILED_REASON_DID_NOT_RECEIVE_FUN_MSG:
        APPCMBS_LOG("Reg Failure Reason : Did not receive Fun Registration Message \n");
        break;
    case CMBS_HAN_REG_FAILED_REASON_PROBLEM_IN_FUN_MSG:
        APPCMBS_LOG("Reg Failure Reason : Received Bad Fun Registration Message \n");
        break;
    case CMBS_HAN_REG_FAILED_REASON_COULD_NOT_ACK_FUN_MSG:
        APPCMBS_LOG("Reg Failure Reason : Could not receive OK and Fun Registraiton Responce\n");
        break;
    case CMBS_HAN_REG_FAILED_REASON_INTERNAL_AFTER_3RD_STAGE:
        APPCMBS_LOG("Reg Failure Reason : 3rd Finished but an Internal Error occured\n");
        break;
    case CMBS_HAN_REG_FAILED_REASON_FUN_DEV_ID_ALLOCATION_FAILURE:
	 APPCMBS_LOG("Reg Failure Reason : Could not allocate FUN device id\n");
        break;
    default:
    case CMBS_HAN_REG_FAILED_REASON_UNEXPECTED:
        APPCMBS_LOG("Reg Failure Reason : Unknown problem \n");
        break;


    }

}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_FORCEFUL_DELETE_ERROR_STATUS_REASON)
{
    u16 u16_Reason;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANForcefulDeRegErrorReasonGet, &u16_Reason);

    switch (u16_Reason)
    {
    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_INVALID_ID:
        APPCMBS_LOG("DeReg Failure Reason : Ivalid or Unregistered Device ID\n");
        break;
    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_DEV_IN_MIDDLE_OF_REG:
        APPCMBS_LOG("DeReg Failure Reason : Device Id is valid but the device is in the middle of registration\n");
        break;
    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_BUSY:
        APPCMBS_LOG("DeReg Failure Reason : Engine is busy with previous task. Try soon\n");
        break;
    default:
    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_UNEXPECTED:
        APPCMBS_LOG("DeReg Failure Reason : Unknown Error\n");
        break;
    }

}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_TX_ENDED_REASON)
{
    u16 u16_Reason;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANTxEndedReasonGet, &u16_Reason);

    switch (u16_Reason)
    {
    case CMBS_HAN_TX_ENDED_REASON_LINK_FAILED_ERROR:
        APPCMBS_LOG("Tx Ended with Reason = %s(%d) ", "Device lost link", CMBS_HAN_TX_ENDED_REASON_LINK_FAILED_ERROR);
        break;
    case CMBS_HAN_TX_ENDED_REASON_LINK_DROPPED_ERROR:
        APPCMBS_LOG("Tx Ended with Reason = %s(%d) ", "Link Dropped due to inactivity", CMBS_HAN_TX_ENDED_REASON_LINK_DROPPED_ERROR);
        break;
    default:
        APPCMBS_LOG("Tx Ended with Reason = %s(%d) ", "Unknown Failure ", u16_Reason);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_PVC_RESET_REQ_FAIL_REASON)
{
    u8 u8_Reason;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANPvcResetReqFailureReasonGet, &u8_Reason);

    switch (u8_Reason)
    {
    case CMBS_HAN_PVC_RESET_REQ_DEVICE_NOT_REGISTERED:
        APPCMBS_LOG("PVC reset req faulire reason = %s(%d) ", "Device not registered", CMBS_HAN_PVC_RESET_REQ_DEVICE_NOT_REGISTERED);
        break;
    case CMBS_HAN_PVC_RESET_REQ_UNKNOWN_FAILURE:
        APPCMBS_LOG("PVC reset req faulire reason = %s(%d) ", "Unknown Failure ", CMBS_HAN_PVC_RESET_REQ_UNKNOWN_FAILURE);
        break;
    default:
        APPCMBS_LOG("PVC reset req faulire reason = %s(%d) ", "Unknown Failure ", u8_Reason);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_PVC_RESET_REQ_STATE_FAIL_REASON)
{
    u8 u8_Reason;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANPvcResetReqStateFailureReasonGet, &u8_Reason);

    switch (u8_Reason)
    {
    case CMBS_HAN_PVC_RESET_REQ_DEVICE_NOT_REGISTERED:
        APPCMBS_LOG("PVC reset req state faulire reason = %s(%d) ", "Device not registered", CMBS_HAN_PVC_RESET_REQ_DEVICE_NOT_REGISTERED);
        break;
    case CMBS_HAN_PVC_RESET_REQ_UNKNOWN_FAILURE:
        APPCMBS_LOG("PVC reset req state faulire reason = %s(%d) ", "Unknown Failure ", CMBS_HAN_PVC_RESET_REQ_UNKNOWN_FAILURE);
        break;
    default:
        APPCMBS_LOG("PVC reset req state failure reason = %s(%d) ", "Unknown Failure ", u8_Reason);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_PVC_RESET_REQ_STATE)
{
    u8 u8_State;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANPvcResetReqStateGet, &u8_State);

    switch (u8_State)
    {
    case CMBS_HAN_PVC_RESET_REQ_STATE_PENDING:
        APPCMBS_LOG("PVC reset req state = %s(%d) ", "request pending", CMBS_HAN_PVC_RESET_REQ_STATE_PENDING);
        break;
    case CMBS_HAN_PVC_RESET_REQ_STATE_NO_REQ:
        APPCMBS_LOG("PVC reset req state = %s(%d) ", "no request", CMBS_HAN_PVC_RESET_REQ_STATE_NO_REQ);
        break;
    default:
        APPCMBS_LOG("PVC reset req state = %s(%d) ", "no request", u8_State);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_MSG_REG_INFO)
{
    ST_HAN_MSG_REG_INFO st_HanMsgRegInfo;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HanMsgRegInfoGet, &st_HanMsgRegInfo);
    APPCMBS_LOG("Unit id: %d,  Inteface Id: %d ", st_HanMsgRegInfo.u8_UnitId, st_HanMsgRegInfo.u16_InterfaceId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HAN_TABLE_UPDATE_INFO)
{
    ST_IE_HAN_TABLE_UPDATE_INFO st_HanTableUpdateInfo;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANTableUpdateInfoGet, &st_HanTableUpdateInfo);
    APPCMBS_LOG("Table: %d ", st_HanTableUpdateInfo.e_Table);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_UNKNOWN_DEVICE_CONTACTED_PARAMS)
{
    ST_HAN_UNKNOWN_DEVICE_CONTACT_PARAMS	st_Status;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANUnknownDeviceContactedParamsGet, &st_Status);

    APPCMBS_LOG("IPUE[3]: {%d,%d,%d} \n", st_Status.u8_IPUI[0], st_Status.u8_IPUI[1], st_Status.u8_IPUI[2]);
    APPCMBS_LOG("SetupType: %d\n", st_Status.SetupType);
    APPCMBS_LOG("NodeResponce: %d", st_Status.NodeResponse);
}

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_CONNECTION_STATUS)
{
    u16	st_Status;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANConnectionStatusGet, &st_Status);

    APPCMBS_LOG("Device is :");

    switch (st_Status)
    {
    case CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_NOT_IN_LINK_AND_NOT_REQUESTED:
        APPCMBS_LOG(" Not in link and Tx Request not sent\n");
        break;
    case CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_NOT_IN_LINK_BUT_REQUESTED:
        APPCMBS_LOG(" Not in link but Tx Request was sent. Awaiting \n");
        break;
    case CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_IN_LINK:
        APPCMBS_LOG(" In Link\n");
        break;
    case CMBS_HAN_ULE_DEVICE_CONNECTIONS_STATUS_NOT_REGISTERED:
        APPCMBS_LOG(" NOT REGISTERED \n");
        break;
    default:
        APPCMBS_LOG(" ??? \n");
        break;
    }

}

CMBS_PARSE_DECL(CMBS_IE_HAN_BASE_INFO)
{
    ST_IE_HAN_BASE_INFO st_HanBaseInfo;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HanUleBaseInfoGet, &st_HanBaseInfo);

	switch (st_HanBaseInfo.u8_UleAppProtocolId)
    {
    
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ID_UNDEFINED:
        APPCMBS_LOG("ULE app protocol Id undefined\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_1:
        APPCMBS_LOG("ULE app protocol Id: ULE HAN-FUN Protocol Version 1\n");
        break;	
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_2:
        APPCMBS_LOG("ULE app protocol Id: ULE HAN-FUN Protocol Version 2\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_PROPRIETARY_MASK:
        APPCMBS_LOG("ULE app protocol Id: Proprietary\n");
        break;
    default:
        APPCMBS_LOG("Unknown protocol id\n");
        break;
    }

	switch (st_HanBaseInfo.u8_UleAppProtocolVersion)
    {
    
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_UNDEFINED:
        APPCMBS_LOG("ULE app protocol version undefined");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_0:
        APPCMBS_LOG("ULE app protocol version: FUN version 1.0");
        break;	
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_4:
        APPCMBS_LOG("ULE app protocol version: FUN version 1.4");
        break;
    default:
        APPCMBS_LOG("Unknown protocol version");
        break;
    }

}

/* Use as global variable for access. */
ST_HAN_REG_STAGE_1_STATUS	st_Status;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_REG_STAGE1_OK_STATUS_PARAMS)
{
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANRegStage1OKResParamsGet, &st_Status);

    APPCMBS_LOG("IPUE[5]: {%d,%d,%d,%d,%d}", st_Status.u8_IPUI[0], st_Status.u8_IPUI[1], st_Status.u8_IPUI[2], st_Status.u8_IPUI[3], st_Status.u8_IPUI[4]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_REG_STAGE2_OK_STATUS_PARAMS)
{
    ST_HAN_REG_STAGE_2_STATUS	st_Status;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANRegStage2OKResParamsGet, &st_Status);

	switch (st_Status.u16_UleAppProtocolId)
    {
    
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ID_UNDEFINED:
        APPCMBS_LOG("Protocol Id undefined\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_1:
        APPCMBS_LOG("Ptorocol Id: ULE HAN-FUN Protocol Version 1\n");
        break;	
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_ULE_PROTOCOL_2:
        APPCMBS_LOG("Ptorocol Id: ULE HAN-FUN Protocol Version 2\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_PROPRIETARY_MASK:
        APPCMBS_LOG("Ptorocol Id: Proprietary\n");
        break;
    default:
        APPCMBS_LOG("Unknown protocol id\n");
        break;
    }

	switch (st_Status.u16_UleAppProtocolVersion)
    {
    
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_UNDEFINED:
        APPCMBS_LOG("Protocol version undefined\n");
        break;
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_0:
        APPCMBS_LOG("ULE app protocol version: FUN version 1.0\n");
        break;	
    case CMBS_HAN_ULE_APPLICATION_PROTOCOL_VERSION_1_4:
        APPCMBS_LOG("ULE app protocol version: FUN version 1.4\n");
        break;
    default:
        APPCMBS_LOG("Unknown protocol id\n");
        break;
    }
		
    APPCMBS_LOG("Requested Page Interval (Ms): %d \n", st_Status.u32_OriginalDevicePagingInterval);
    APPCMBS_LOG("Actual Page Interval (Ms): %d", st_Status.u32_ActualDevicePagingInterval);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_EV_DSR_HAN_DEVICE_FORCEFUL_DELETE_RES)
{
    UNUSED_PARAMETER(pv_IE);
    APPCMBS_LOG("ULE Device Delete Status");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_EV_DSR_HAN_DEVICE_GET_CONNECTION_STATUS_RES)
{
    UNUSED_PARAMETER(pv_IE);
    APPCMBS_LOG("ULE Device Connection Status ( In Link or not ? )");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_EV_DSR_HAN_DEVICE_REG_DELETED)
{
    u16 u16_DeviceId;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceGet, &u16_DeviceId);

    APPCMBS_LOG(" Device with partial registration deleted ( id = %d )", u16_DeviceId);
}



CMBS_PARSE_DECL(CMBS_IE_HAN_TX_REQ_RES_REASON)
{
    u16	st_Status;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANTxReqResReasonGet, &st_Status);

    APPCMBS_LOG("Device is :");

    switch (st_Status)
    {
    
    case CMBS_HAN_ULE_TX_REQ_RES_INVALID_DEVICE_ID:
        APPCMBS_LOG(" Invalid Device ID");
        break;
    case CMBS_HAN_ULE_TX_REQ_RES_PREVIOUS_TX_REQ_PENDING:
        APPCMBS_LOG(" Not in link but Tx Request was sent. Awaiting");
        break;	
    case CMBS_HAN_ULE_TX_REQ_RES_NO_FREE_SPACE:
        APPCMBS_LOG(" TMH DB no free memory");
        break;
    case CMBS_HAN_ULE_TX_REQ_RES_DEVICE_NOT_REGISTERED:
        APPCMBS_LOG(" NOT REGISTERED ");
        break;
    case CMBS_HAN_ULE_TX_REQ_RES_DEVICE_ALREADY_IN_LINK:
        APPCMBS_LOG(" Device already in Link ");
        break;
    default:
        APPCMBS_LOG(" ??? \n");
        break;
    }

}


CMBS_PARSE_DECL(CMBS_IE_HAN_READ_DEVICE_FULL_REGISTRATION_INFO_RES_REASON)
{
    u8	st_Status;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANReadDeviceFullRegInfoResReasonGet, &st_Status);

    APPCMBS_LOG("Failure status :");

    switch (st_Status)
    {
    
    case CMBS_HAN_READ_REGISTRATION_INFO_NO_FAILURE:
        APPCMBS_LOG(" No failure");
        break;
    case CMBS_HAN_READ_REGISTRATION_INFO_DEVICE_NOT_REGISTERED:
        APPCMBS_LOG(" Device NOT registered");
        break;	
    case CMBS_HAN_READ_REGISTRATION_INFO_INVALID_DEVICE_ID:
        APPCMBS_LOG(" Invalid device Id, exceeding maximal device id vaule");
        break;
    default:
        APPCMBS_LOG(" ??? \n");
        break;
    }

}


CMBS_PARSE_DECL(CMBS_IE_HAN_CERTIFICATION_OPERATION)
{
	u8 u8_CertificationOperation;

	CMBS_SAFE_GET_IE(cmbs_api_ie_HANCertificationOperationGet, &u8_CertificationOperation);

	APPCMBS_LOG("Certification Operation: ");

	switch (u8_CertificationOperation)
	    {   
			case CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_MARK_CCM_KEY_AS_USED:
		       	APPCMBS_LOG("Mark Ccm Key As Used");
		        	break;

			case CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_MIC_CORRUPTION_NUMBER:
		       	APPCMBS_LOG("Set Tx Mic Corruption Number");
		        	break;

			case CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_SEQUENCE_NUMBER:
		       	APPCMBS_LOG("Set Tx Sequence Number");
		        	break;

			case CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_GET_TX_SEQUENCE_NUMBER:
		       	APPCMBS_LOG("Get Tx Sequence Number");
		        	break;

			case CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_GET_RX_SEQUENCE_NUMBER:
		       	APPCMBS_LOG("Get Rx Sequence Number");
		        	break;

			case CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_TX_ACK_DROP_NUMBER:
		        	APPCMBS_LOG("Set Tx Ack Drop Number");
		        	break;

			case CMBS_HAN_ULE_DEVICE_CERTIFICATION_OPERATION_SET_PVC_STATE:
		       	APPCMBS_LOG("Set Pvc State");
		        	break;
				
		    default:
		        APPCMBS_LOG(" Undefined \n");
		        break;
	    }
}


CMBS_PARSE_DECL(CMBS_IE_HAN_TX_SEQUENCE_NUMBER)
{

	u16 u16_TxSequenceNumber;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANTxSequenceNumberGet, &u16_TxSequenceNumber);

	APPCMBS_LOG(" TX Sequence Number: 0x%x", u16_TxSequenceNumber);
}


CMBS_PARSE_DECL(CMBS_IE_HAN_RX_SEQUENCE_NUMBER)
{
	u16 u16_RxSequenceNumber;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANRxSequenceNumberGet, &u16_RxSequenceNumber);

	APPCMBS_LOG(" RX Sequence Number: 0x%x", u16_RxSequenceNumber);
}

CMBS_PARSE_DECL(CMBS_IE_HAN_CCM_KEY_USED)
{
	u8 u8_CcmKeyUsed;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANCcmKeyAsUsedGet, &u8_CcmKeyUsed);

	if (u8_CcmKeyUsed)
		{
			APPCMBS_LOG(" CCM Key is Used");
		}
	else
		{
			APPCMBS_LOG(" CCM Key is NOT Used");
		}
	
}

CMBS_PARSE_DECL(CMBS_IE_HAN_TX_ACK_DROP_NUMBER)
{
	u16 u16_TxAckDropNumber;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANTxAckDropNumberGet, &u16_TxAckDropNumber);

	APPCMBS_LOG(" Drop Acks from 0x%x", u16_TxAckDropNumber);
}

CMBS_PARSE_DECL(CMBS_IE_HAN_PVC_STATE)
{
	u8 u8_PVCState;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANPvcStateGet, &u8_PVCState);

	APPCMBS_LOG(" PVC State:  ");

	switch (u8_PVCState)
		{
			case 0:
				APPCMBS_LOG("Suspended <%d>", u8_PVCState);
				break;

			case 1:
				APPCMBS_LOG("Resumed <%d>", u8_PVCState);
				break;
				
			case 2:
				APPCMBS_LOG("Suspended Waiting Resume <%d>", u8_PVCState);
				break;

			case 3:
				APPCMBS_LOG("Resumed Waiting Suspend <%d>", u8_PVCState);
				break;

			default:
				APPCMBS_LOG(" Invalid <%d>", u8_PVCState);
				break;
		}
}	

CMBS_PARSE_DECL(CMBS_IE_HAN_RX_GAIN)
{
	u8 u8_RxGain;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANRxGainGet, &u8_RxGain);

	APPCMBS_LOG(" System Rx Gain:  ");

	switch (u8_RxGain)
		{
			case 0:
				APPCMBS_LOG("Normal <0x%X>", u8_RxGain);
				break;

			case 0x0F:
				APPCMBS_LOG("Minus 4 dB <0x%X>", u8_RxGain);
				break;
				
			case 0xCF:
				APPCMBS_LOG("Minus 12 dB <0x%X>", u8_RxGain);
				break;

			case 0xEF:
				APPCMBS_LOG("Minus 25 dB <0x%X>", u8_RxGain);
				break;

			default:
				APPCMBS_LOG(" Invalid <0x%X>", u8_RxGain);
				break;
		}
}	
	

CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_TBR6_CARRIER)
{
	u8 u8_TBR6Carrier;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANTBR6CarrierGet, &u8_TBR6Carrier);

	APPCMBS_LOG(" ULE TB6 Carrier is : 0x%x", u8_TBR6Carrier);

}	

CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_CARRIER)
{
	u8 u8_Carrier;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANCarrierGet, &u8_Carrier);

	APPCMBS_LOG(" ULE Carrier is : 0x%x", u8_Carrier);

}	

CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_SLOT)
{
	u8 u8_Slot;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANSlotGet, &u8_Slot);

	APPCMBS_LOG(" ULE Slot is : 0x%x", u8_Slot);

}	

CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_TBR6_PERIOD)
{
	u32 u32_TBR6Period;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANTBR6PeriodGet, &u32_TBR6Period);

	APPCMBS_LOG(" ULE TB6 period is : 0x%x", u32_TBR6Period);

}



CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_DEVICE_IPUI)
{
    ST_IE_HAN_ULE_DEVICE_IPUI	st_UleDeviceIpui;

    CMBS_SAFE_GET_IE(cmbs_api_ie_HANULEDeviceIPUIGet, &st_UleDeviceIpui);

    APPCMBS_LOG("ULE Device TBR 6 IPUE[5]: {%X,%X,%X,%X,%X}", st_UleDeviceIpui.u8_IPUI[0], st_UleDeviceIpui.u8_IPUI[1], st_UleDeviceIpui.u8_IPUI[2], st_UleDeviceIpui.u8_IPUI[3], st_UleDeviceIpui.u8_IPUI[4]);

}

CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_TBR6_STATISTICS)
{
	ST_IE_HAN_ULE_TBR6_STATISTICS	st_ie_tbr6_stat;

	CMBS_SAFE_GET_IE(cmbs_api_ie_HANULETBR6StatisticsGet, &st_ie_tbr6_stat);

	APPCMBS_LOG("\n");

	APPCMBS_LOG("TBR 6 Statistics\n");
	APPCMBS_LOG("=================\n");  

	APPCMBS_LOG("\tStatus: %s \n\n ", (st_ie_tbr6_stat.u32_Status==2)?"OK":(st_ie_tbr6_stat.u32_Status==1)?"INCONCLUSIVE":"FAIL");
	APPCMBS_LOG("\tCounter: 0x%02X\n ", st_ie_tbr6_stat.u32_Counter);
	
	APPCMBS_LOG("\tBer:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u32_ber);
	APPCMBS_LOG("\tFer:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u32_fer);
	APPCMBS_LOG("\tBCRC Wrong: 0x%02X\n ", st_ie_tbr6_stat.st_ule_tbr6_statistics.u32_bcrc_wrong);
	APPCMBS_LOG("\tCount:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u32_count);	
	APPCMBS_LOG("\tFirst RX Frame:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u32_first_rx_frame);	
	APPCMBS_LOG("\tLast RX Frame:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u32_last_rx_frame);	
	APPCMBS_LOG("\tPeriod :  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u16_period);
	APPCMBS_LOG("\tPeriod Error:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u8_period_error);	
	APPCMBS_LOG("\tPeriod Min:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u16_period_min);	
	APPCMBS_LOG("\tPeriod Max:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u16_period_max);	

	APPCMBS_LOG("\tRSSI Local:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u8_rssi_local);
	APPCMBS_LOG("\tRSSI Local Min:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u8_rssi_local_min);
	APPCMBS_LOG("\tRSSI Local Max:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u8_rssi_local_max);

	APPCMBS_LOG("\tRSSI:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u8_rssi);
	APPCMBS_LOG("\tRSSI Min:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u8_rssi_min);
	APPCMBS_LOG("\tRSSI Max:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u8_rssi_max);
	
	APPCMBS_LOG("\tMissed Acks:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u16_missed_acks);	
			
	APPCMBS_LOG("\tSync Attempts:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u16_sync_attempts);	
	APPCMBS_LOG("\tSync Frames:  0x%02X\n", st_ie_tbr6_stat.st_ule_tbr6_statistics.u16_sync_frame);	

}


CMBS_PARSE_DECL(CMBS_IE_HAN_REQ_FAILURE_REASON)
{
	u8 u8_Reason;		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HanReqFailureReasonGet, &u8_Reason);

	switch(u8_Reason)
	{
		case CMBS_HAN_NOT_INITIALIZED:
			APPCMBS_LOG("Han was not initialized");
			break;
		default:
			APPCMBS_LOG("Unknown Error");
	}
}

CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_PAGING_INTERVAL)
{
	u32 u32_ULEPagingInterval;		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HanULEPagingIntervalGet, &u32_ULEPagingInterval);

	APPCMBS_LOG("Paging Interval: %d",u32_ULEPagingInterval);
}


CMBS_PARSE_DECL(CMBS_IE_HAN_ULE_PAGING_INTERVAL_MODIFICATION_FAILURE_REASON)
{
	u8 u8_Reason;		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HanULEPagingIntervalModificationFailureReasonGet, &u8_Reason);

	switch(u8_Reason)
	{
		case CMBS_HAN_ULE_PAGING_INTERVAL_BELOW_MIN:
			APPCMBS_LOG("Paging interval below the minimum value");
			break;
		case CMBS_HAN_ULE_PAGING_INTERVAL_ABOVE_MAX:
			APPCMBS_LOG("Paging interval above the maximum value");
			break;
		case CMBS_HAN_ULE_PAGING_INTERVAL_DEVICE_NOT_REGISTERED:
			APPCMBS_LOG("Device is : NOT REGISTERED");
			break;	
		case CMBS_HAN_ULE_PAGING_INTERVAL_DEVICE_NOT_PAGEABLE:
			APPCMBS_LOG("Device is : NOT PAGEABLE");
			break;	
		case CMBS_HAN_ULE_PAGING_INTERVAL_MODIFICATION_UNKNOWN_FAILED:
			APPCMBS_LOG("Paging interval modification unknown failure");
			break;	
		default:
			APPCMBS_LOG("Unknown Error");
	}	
}

CMBS_PARSE_DECL(CMBS_IE_HAN_NODE_SETUP_ATTEMPTS_ENABLE)
{
	u8 u8_Enable;		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HanNodeSetupAttemptsEnableGet, &u8_Enable);

	switch(u8_Enable)
	{
		case CMBS_HAN_NODE_SETUP_ATTEMPTS_ENABLE:
			APPCMBS_LOG("Node Setup Enable");
			break;
		case CMBS_HAN_NODE_SETUP_ATTEMPTS_DISABLE:
			APPCMBS_LOG("Node Setup Disable");
			break;
		default:
			APPCMBS_LOG("Unknown Error");
	}	
}


CMBS_PARSE_DECL(CMBS_IE_HAN_LINK_RELEASE_REASON)
{
	u8 u8_ReleaseReason;

	CMBS_SAFE_GET_IE(cmbs_api_ie_HanDeviceLinkReleaseReasonGet, &u8_ReleaseReason);

	switch(u8_ReleaseReason)
	{
		case CMBS_HAN_CP_BEARER_RELEASE:
			APPCMBS_LOG("bearer release");
			break;

		case CMBS_HAN_CP_BASE_STATION_BUSY:
			APPCMBS_LOG("base station is busy");
			break;

		case CMBS_HAN_CP_UNACCEPTABLE_PMID:
			APPCMBS_LOG("unaccpentable PMID");
			break;

		case CMBS_HAN_CP_STAY_IN_LCE_PAGING_DETECTION:
			APPCMBS_LOG("stay in LCE paging detection");
			break;

		case CMBS_HAN_CP_SWITCH_TO_CM:
			APPCMBS_LOG("swith to CM");
			break;
			
		case CMBS_HAN_CP_STAY_IN_HIGHER_PAGING_DETECTION:
			APPCMBS_LOG("stay in higher paging detection");
			break;		

		case CMBS_HAN_CP_SETUP_AGAIN:
			APPCMBS_LOG("CP setup again");
			break;		

		case CMBS_HAN_CP_NO_CONNECTION_OR_VC:
			APPCMBS_LOG("no connection or virtual connection");
			break;

		case CMBS_HAN_CP_LU13_LU14_ERROR:
			APPCMBS_LOG("LU13 LU14 Error");
			break;

		default:
			APPCMBS_LOG("Unknown Error");
	}	
}


CMBS_PARSE_DECL(CMBS_IE_HAN_MAX_NUM_OF_DEVICES)
{
	u16 u16_MaxNumOfDevices;
	
	CMBS_SAFE_GET_IE(cmbs_api_ie_HanMaxNumOfDevicesGet, &u16_MaxNumOfDevices);
	APPCMBS_LOG("Number of maximal devices = %d ", u16_MaxNumOfDevices);
}

CMBS_PARSE_DECL(CMBS_IE_CALL_LIST)
{
	ST_IE_CALL_LIST st_CallList;
	int i = 0;

    CMBS_SAFE_GET_IE(cmbs_api_ie_CallListGet, &st_CallList);

	APPCMBS_LOG("Call Type: %s\n", cmbs_dbg_GetCallTypeName(st_CallList.u8_CallType));
    APPCMBS_LOG("Call Instance: 0x%08x\n", st_CallList.u32_CallInstanse);
	APPCMBS_LOG("CMBS Call Id: %d\n", st_CallList.u8_CallId);
    APPCMBS_LOG("Line ID: %d\n", st_CallList.u8_LineId);
	APPCMBS_LOG("Caller Number: %s\n", st_CallList.u8_CallerParty);
	APPCMBS_LOG("Called Number: %s\n", st_CallList.u8_CalledParty);
	APPCMBS_LOG("Handset Mask: 0x%04x\n", st_CallList.u16_HSMask);
	APPCMBS_LOG("Channel ID: %d\n", st_CallList.u32_ChannelID);
	APPCMBS_LOG("Channel Param: 0x%04x\n", st_CallList.u32_ChannelParameter);
	APPCMBS_LOG("Codec: %s\n", app_GetCodecString(st_CallList.u8_Codec));
	APPCMBS_LOG("Codec List: \n");
	for(i = 0; i < st_CallList.u8_CodecsLength; i++)
	{
		APPCMBS_LOG("Codec %d: %s\n", i, app_GetCodecString(st_CallList.pu8_CodecsList[i]));
	}
	
	//APPCMBS_LOG("Call State: %d\n", st_CallList.u8_CallStatus);
	APPCMBS_LOG("Call State: %s\n", cmbs_dbg_GetCallStateName(st_CallList.u8_Callstatus));
	APPCMBS_LOG("Media State: %s\n", app_GetMediaString(st_CallList.u8_Media));
	
}

//////////////////////////////////////////////////////////////////////////
CMBS_PARSE_DECL(CMBS_IE_HSRELEASE_REASON)
{
  ST_IE_HS_RELEASE_REASON st_HsReleaseReason;
  CMBS_SAFE_GET_IE(cmbs_api_ie_HSReleaseReasonGet, &st_HsReleaseReason);
  APPCMBS_LOG("HS Number =  %d \n", st_HsReleaseReason.u8_HsHr);
  APPCMBS_LOG("HS Release Reason: ");
  switch (st_HsReleaseReason.e_Reason)
  {
    case CMBS_REL_REASON_NORMAL:
        APPCMBS_LOG("NORMAL");          break;
    case CMBS_REL_REASON_USER_REJECTION:
        APPCMBS_LOG("CALL REJECTED");    break;
    case CMBS_REL_REASON_USER_NOT_IN_RANGE:
        APPCMBS_LOG("NOT_IN_RANGE");     break;
    default:
        APPCMBS_LOG("UNKNOWN");
  }
}
//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_LA_SELECTION_TYPE)
{
    u16 u16_SelectionType;
    CMBS_SAFE_GET_IE2(cmbs_api_ie_ShortValueGet, &u16_SelectionType, CMBS_IE_LA_SELECTION_TYPE);
    APPCMBS_LOG("LA Selection Type = %d", u16_SelectionType);
}
//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_LA_ENTRY_RANGE)
{
    ST_LA_ENTRY_RANGE st_EntryRange;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LAEntryRangeGet, &st_EntryRange);
    APPCMBS_LOG("Entry Id: %d,  Field Id: %d, Lower Bound: %d,  Upper Bound: %d ", st_EntryRange.u16_EntryId, st_EntryRange.u16_FieldId, st_EntryRange.u16_LowerBound, st_EntryRange.u16_UpperBound);
}
//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(PrintLAEntryIds)
{
    ST_LA_ENTRY_IDS st_EntryIds;
    int i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_LAEntryIdsGet, &st_EntryIds);

    APPCMBS_LOG("Length:%d\n", st_EntryIds.u16_Length);
    APPCMBS_LOG("Entry Ids: ");
    if ( st_EntryIds.u16_Length == 0 )
    {
        APPCMBS_LOG(" (none)");
    }
    else
    {
        for (i = 0; i < st_EntryIds.u16_Length; i++)
        {
            APPCMBS_LOG(" %2d", st_EntryIds.pu16_EntryId[i]);
        }
    }
}
//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_LA_ENTRY_IDS)
{

    APPCMBS_LOG("LA_Entry Ids:\n");
    app_PrintLAEntryIds_print(pOutput, u32_OutputSize, pu32_Pos, pv_IE);
}
//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_GEN_EVENT_DETAILS)
{
	ST_IE_GEN_EVENT_DETAILS st_GenEventDetails;
	int i;
    CMBS_SAFE_GET_IE(cmbs_api_ie_GenEventDetailsGet, &st_GenEventDetails);
    APPCMBS_LOG("Generic event details: Count = %d, Origination HS = %d",\
                   st_GenEventDetails.u8_Count, st_GenEventDetails.u16_OrgHs);
	for (i = 0; i < st_GenEventDetails.u8_Count; i++)
	{
		APPCMBS_LOG("\nChange = ");
		switch (st_GenEventDetails.st_SubDetails[i].u16_Change)
		{
		  case CMBS_CHANGE_EDIT:
			  APPCMBS_LOG("Modification, ");		  break;
		  case CMBS_CHANGE_ADD:
			  APPCMBS_LOG("Addition, ");    break;
		  case CMBS_CHANGE_DELETE:
			  APPCMBS_LOG("Deletion, ");	   break;
		  default:
			  APPCMBS_LOG("Unknown, ");
		}
		APPCMBS_LOG("Entry Id = %d, Pos Index = %d",\
					   st_GenEventDetails.st_SubDetails[i].u16_EntryId, st_GenEventDetails.st_SubDetails[i].u16_PosIndex);
	}
}
//////////////////////////////////////////////////////////////////////////

CMBS_PARSE_DECL(CMBS_IE_HAN_MSG_RSSI)
{
	u8 u8_RSSI;
		
	CMBS_SAFE_GET_IE(cmbs_api_ie_HANMsgRSSIGet, &u8_RSSI);

	APPCMBS_LOG("RSSI: 0x%02x", u8_RSSI);

}

//////////////////////////////////////////////////////////////////////////


CMBS_PARSE_DECL(CMBS_IE_HAN_DEVICE_BLACK_LIST_DELETE_ERROR_STATUS_REASON)
{
    u16 u16_Reason;
    CMBS_SAFE_GET_IE(cmbs_api_ie_HANDeviceBlackListDeleteErrorReasonGet, &u16_Reason);

    switch (u16_Reason)
    {
	    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_INVALID_ID:
	        APPCMBS_LOG("DeReg Failure Reason : Ivalid or Unregistered Device ID\n");
	        break;
	    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_DEV_IN_MIDDLE_OF_REG:
	        APPCMBS_LOG("DeReg Failure Reason : Device Id is valid but the device is in the middle of registration\n");
	        break;
	    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_BUSY:
	        APPCMBS_LOG("DeReg Failure Reason : Engine is busy with previous task. Try soon\n");
	        break;
	    default:
	    case CMBS_HAN_DEVICE_DEREGISTRATION_ERROR_UNEXPECTED:
	        APPCMBS_LOG("DeReg Failure Reason : Unknown Error\n");
	        break;
    }

}


//////////////////////////////////////////////////////////////////////////


u32 app_PrintIe2Log(char *pOutput, u32 u32_OutputSize, void *pv_IE, u16 u16_IE)
{
    u32 u32_Pos = 0;
    u32 *pu32_Pos = &u32_Pos;

#ifdef CHECKSUM_SUPPORT
    if ( u16_IE == CMBS_IE_CHECKSUM )
    {
        // checksum should not be printed.
        return u32_Pos;
    }
#endif

    APPCMBS_LOG_HEADER("<%s(%d)>:\n", cmbs_dbg_GetIEName(u16_IE), u16_IE);

    // adjust output
    switch (u16_IE)
    {
        CMBS_PARSE_CASE(CMBS_IE_CALLINSTANCE)
        CMBS_PARSE_CASE(CMBS_IE_CALLERPARTY)
        CMBS_PARSE_CASE(CMBS_IE_CALLERNAME)
        CMBS_PARSE_CASE(CMBS_IE_CALLEDPARTY)
        CMBS_PARSE_CASE(CMBS_IE_CALLPROGRESS)
        CMBS_PARSE_CASE(CMBS_IE_CALLINFO)
        CMBS_PARSE_CASE(CMBS_IE_DISPLAY_STRING)
        CMBS_PARSE_CASE(CMBS_IE_CALLRELEASE_REASON)
        CMBS_PARSE_CASE(CMBS_IE_CALLSTATE)
        CMBS_PARSE_CASE(CMBS_IE_MEDIACHANNEL)
        CMBS_PARSE_CASE(CMBS_IE_MEDIA_INTERNAL_CONNECT)
        CMBS_PARSE_CASE(CMBS_IE_MEDIADESCRIPTOR)
        CMBS_PARSE_CASE(CMBS_IE_TONE)
        CMBS_PARSE_CASE(CMBS_IE_TIMEOFDAY)
        CMBS_PARSE_CASE(CMBS_IE_HANDSETINFO)
        CMBS_PARSE_CASE(CMBS_IE_PARAMETER)
        CMBS_PARSE_CASE(CMBS_IE_SUBSCRIBED_HS_LIST)
        CMBS_PARSE_CASE(CMBS_IE_LINE_SETTINGS_LIST)
        //CMBS_PARSE_CASE(CMBS_IE_LINE_SETTINGS_TYPE)(not used, kept for compatibility)
        CMBS_PARSE_CASE(CMBS_IE_FW_VERSION)
        CMBS_PARSE_CASE(CMBS_IE_SYS_LOG)
        CMBS_PARSE_CASE(CMBS_IE_RESPONSE)
        CMBS_PARSE_CASE(CMBS_IE_STATUS)
        CMBS_PARSE_CASE(CMBS_IE_INTEGER_VALUE)
        CMBS_PARSE_CASE(CMBS_IE_LINE_ID)
        CMBS_PARSE_CASE(CMBS_IE_PARAMETER_AREA)
        CMBS_PARSE_CASE(CMBS_IE_REQUEST_ID)
        CMBS_PARSE_CASE(CMBS_IE_HANDSETS)
        CMBS_PARSE_CASE(CMBS_IE_GEN_EVENT)
        CMBS_PARSE_CASE(CMBS_IE_PROP_EVENT)
        CMBS_PARSE_CASE(CMBS_IE_DATETIME)
        CMBS_PARSE_CASE(CMBS_IE_DATA)
        CMBS_PARSE_CASE(CMBS_IE_DATA_SESSION_ID)
        CMBS_PARSE_CASE(CMBS_IE_DATA_SESSION_TYPE)
        CMBS_PARSE_CASE(CMBS_IE_LA_SESSION_ID)
        CMBS_PARSE_CASE(CMBS_IE_LA_LIST_ID)
        CMBS_PARSE_CASE(CMBS_IE_LA_FIELDS)
        CMBS_PARSE_CASE(CMBS_IE_LA_SORT_FIELDS)
        CMBS_PARSE_CASE(CMBS_IE_LA_EDIT_FIELDS)
        CMBS_PARSE_CASE(CMBS_IE_LA_CONST_FIELDS)
        CMBS_PARSE_CASE(CMBS_IE_LA_SEARCH_CRITERIA)
        CMBS_PARSE_CASE(CMBS_IE_LA_ENTRY_ID)
        CMBS_PARSE_CASE(CMBS_IE_LA_ENTRY_INDEX)
        CMBS_PARSE_CASE(CMBS_IE_LA_ENTRY_COUNT)
        CMBS_PARSE_CASE(CMBS_IE_LA_IS_LAST)
        CMBS_PARSE_CASE(CMBS_IE_LA_REJECT_REASON)
        CMBS_PARSE_CASE(CMBS_IE_LA_NR_OF_ENTRIES)
        CMBS_PARSE_CASE(CMBS_IE_CALLTRANSFERREQ)
        CMBS_PARSE_CASE(CMBS_IE_HS_NUMBER)
        CMBS_PARSE_CASE(CMBS_IE_SHORT_VALUE)
        CMBS_PARSE_CASE(CMBS_IE_ATE_SETTINGS)
        CMBS_PARSE_CASE(CMBS_IE_LA_READ_DIRECTION)
        CMBS_PARSE_CASE(CMBS_IE_LA_MARK_REQUEST)
        CMBS_PARSE_CASE(CMBS_IE_LINE_SUBTYPE)

        CMBS_PARSE_CASE(CMBS_IE_AVAIL_VERSION_DETAILS)
        CMBS_PARSE_CASE(CMBS_IE_HS_VERSION_BUFFER)
        CMBS_PARSE_CASE(CMBS_IE_HS_VERSION_DETAILS)
        CMBS_PARSE_CASE(CMBS_IE_SU_SUBTYPE)
        CMBS_PARSE_CASE(CMBS_IE_URL)
        CMBS_PARSE_CASE(CMBS_IE_NUM_OF_URLS)
        CMBS_PARSE_CASE(CMBS_IE_REJECT_REASON)
        CMBS_PARSE_CASE(CMBS_IE_TARGET_LIST_CHANGE_NOTIF)
        CMBS_PARSE_CASE(CMBS_IE_HW_VERSION)

        CMBS_PARSE_CASE(CMBS_IE_RTP_SESSION_INFORMATION)
        CMBS_PARSE_CASE(CMBS_IE_RTCP_INTERVAL)
        CMBS_PARSE_CASE(CMBS_IE_RTP_DTMF_EVENT)
        CMBS_PARSE_CASE(CMBS_IE_RTP_DTMF_EVENT_INFO)
        CMBS_PARSE_CASE(CMBS_IE_RTP_FAX_TONE_TYPE)

        CMBS_PARSE_CASE(CMBS_IE_INTERNAL_TRANSFER)
        CMBS_PARSE_CASE(CMBS_IE_LA_PROP_CMD)
        CMBS_PARSE_CASE(CMBS_IE_MELODY)
        CMBS_PARSE_CASE(CMBS_IE_REG_CLOSE_REASON)
        CMBS_PARSE_CASE(CMBS_IE_BASE_NAME)
        CMBS_PARSE_CASE(CMBS_IE_EEPROM_VERSION)
        CMBS_PARSE_CASE(CMBS_IE_HS_PROP_EVENT)
        CMBS_PARSE_CASE(CMBS_IE_SYPO_SPECIFICATION)

        CMBS_PARSE_CASE(CMBS_IE_AFE_ENDPOINT_CONNECT)
        CMBS_PARSE_CASE(CMBS_IE_AFE_ENDPOINT)
        CMBS_PARSE_CASE(CMBS_IE_AFE_ENDPOINT_GAIN)
        CMBS_PARSE_CASE(CMBS_IE_AFE_ENDPOINT_GAIN_DB)
        CMBS_PARSE_CASE(CMBS_IE_AFE_AUX_MEASUREMENT_SETTINGS)
        CMBS_PARSE_CASE(CMBS_IE_AFE_AUX_MEASUREMENT_RESULT)
        CMBS_PARSE_CASE(CMBS_IE_AFE_RESOURCE_TYPE)
        CMBS_PARSE_CASE(CMBS_IE_AFE_INSTANCE_NUM)
        CMBS_PARSE_CASE(CMBS_IE_DHSG_VALUE)
        CMBS_PARSE_CASE(CMBS_IE_GPIO_ID)
        CMBS_PARSE_CASE(CMBS_IE_GPIO_MODE)
        CMBS_PARSE_CASE(CMBS_IE_GPIO_VALUE)
        CMBS_PARSE_CASE(CMBS_IE_GPIO_PULL_TYPE)
        CMBS_PARSE_CASE(CMBS_IE_GPIO_PULL_ENA)
        CMBS_PARSE_CASE(CMBS_IE_GPIO_ENA)
        CMBS_PARSE_CASE(CMBS_IE_EXT_INT_CONFIGURATION)
        CMBS_PARSE_CASE(CMBS_IE_EXT_INT_NUM)
        CMBS_PARSE_CASE(CMBS_IE_TERMINAL_CAPABILITIES)
        CMBS_PARSE_CASE(CMBS_IE_CALL_HOLD_REASON)
        CMBS_PARSE_CASE(CMBS_IE_CHECKSUM_ERROR)
        CMBS_PARSE_CASE(CMBS_IE_SUOTA_FILE_INFO)
        CMBS_PARSE_CASE(CMBS_IE_SPLASH_TIMER)
        CMBS_PARSE_CASE(CMBS_IE_TONE_VOLUME)                
        CMBS_PARSE_CASE(CMBS_IE_REG_ENABLE_TYPE)       
        CMBS_PARSE_CASE(CMBS_IE_PROP_FIELDS_LIST)
        CMBS_PARSE_CASE(CMBS_IE_RSSI_VALUE)
        CMBS_PARSE_CASE(CMBS_IE_STATE_FILTER_TYPE)
        CMBS_PARSE_CASE(CMBS_IE_SLIC_LINE_TEST_RESULTS)             
        CMBS_PARSE_CASE(CMBS_IE_FXS_HOOK_STATUS)
        CMBS_PARSE_CASE(CMBS_IE_CALL_DIRECTION)
        CMBS_PARSE_CASE(CMBS_IE_JEDEC_ID)
        CMBS_PARSE_CASE(CMBS_IE_SUOTA_APP_ID)
        CMBS_PARSE_CASE(CMBS_IE_SUOTA_SESSION_ID)

        CMBS_PARSE_CASE(CMBS_IE_UPDATED_DEVICE_STATE)
        CMBS_PARSE_CASE(CMBS_IE_DTMF_TONE_DUR)
        CMBS_PARSE_CASE(CMBS_IE_CALL_LIST)
        CMBS_PARSE_CASE(CMBS_IE_TDM_SLOTS)
        CMBS_PARSE_CASE(CMBS_IE_SLIC_NLT_CAP_TEST_RESULTS)             
        CMBS_PARSE_CASE(CMBS_IE_DTMF_TONE_TO_HS)
        CMBS_PARSE_CASE(CMBS_IE_DEVICE_LIST_STATUS)
        CMBS_PARSE_CASE(CMBS_IE_HSRELEASE_REASON)
        CMBS_PARSE_CASE(CMBS_IE_CRASH_DUMP_ENABLE)
        CMBS_PARSE_CASE(CMBS_IE_PACKET_NUMBER)
        CMBS_PARSE_CASE(CMBS_IE_CALL_SCREENING)
        CMBS_PARSE_CASE(CMBS_IE_LA_SELECTION_TYPE)
        CMBS_PARSE_CASE(CMBS_IE_LA_ENTRY_RANGE)
        CMBS_PARSE_CASE(CMBS_IE_LA_ENTRY_IDS)
        CMBS_PARSE_CASE(CMBS_IE_GEN_EVENT_DETAILS)
        CMBS_PARSE_CASE(CMBS_IE_PRODUCT_ID)

        CMBS_PARSE_CASE(CMBS_IE_HAN_MSG)
        CMBS_PARSE_CASE(CMBS_IE_HAN_MSG_CONTROL)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_TABLE_BRIEF_ENTRIES)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_ENTRIES)
        CMBS_PARSE_CASE(CMBS_IE_HAN_BIND_TABLE_ENTRIES)
        CMBS_PARSE_CASE(CMBS_IE_HAN_GROUP_TABLE_ENTRIES)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE)
        CMBS_PARSE_CASE(CMBS_IE_HAN_CFG)
        CMBS_PARSE_CASE(CMBS_IE_HAN_NUM_OF_ENTRIES)
        CMBS_PARSE_CASE(CMBS_IE_HAN_INDEX_1ST_ENTRY)
        CMBS_PARSE_CASE(CMBS_IE_HAN_TABLE_ENTRY_TYPE)
        CMBS_PARSE_CASE(CMBS_IE_HAN_GENERAL_STATUS)
        CMBS_PARSE_CASE(CMBS_IE_HAN_SEND_FAIL_REASON)
        CMBS_PARSE_CASE(CMBS_IE_HAN_TX_ENDED_REASON)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_FORCEFUL_DELETE_ERROR_STATUS_REASON)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_REG_ERROR_REASON)
        CMBS_PARSE_CASE(CMBS_IE_HAN_MSG_REG_INFO)
        CMBS_PARSE_CASE(CMBS_IE_HAN_FUN_DEVICE_INFO)
        CMBS_PARSE_CASE(CMBS_IE_HAN_TABLE_UPDATE_INFO)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_REG_STAGE1_OK_STATUS_PARAMS)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_REG_STAGE2_OK_STATUS_PARAMS)
        CMBS_PARSE_CASE(CMBS_IE_HAN_UNKNOWN_DEVICE_CONTACTED_PARAMS)
        CMBS_PARSE_CASE(CMBS_IE_HAN_BASE_INFO)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_CONNECTION_STATUS)
        CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_INFO)
        CMBS_PARSE_CASE(CMBS_IE_HAN_BIND_TABLE_ENTRY)
        CMBS_PARSE_CASE(CMBS_IE_HAN_TX_REQ_RES_REASON)
        CMBS_PARSE_CASE(CMBS_IE_HAN_LOG_LEVEL)
		
	CMBS_PARSE_CASE( CMBS_IE_HAN_CERTIFICATION_OPERATION)
	CMBS_PARSE_CASE( CMBS_IE_HAN_CCM_KEY_USED)
	CMBS_PARSE_CASE( CMBS_IE_HAN_TX_SEQUENCE_NUMBER)
	CMBS_PARSE_CASE( CMBS_IE_HAN_RX_SEQUENCE_NUMBER)
	CMBS_PARSE_CASE( CMBS_IE_HAN_TX_ACK_DROP_NUMBER)
	CMBS_PARSE_CASE( CMBS_IE_HAN_PVC_STATE)
	CMBS_PARSE_CASE( CMBS_IE_HAN_RX_GAIN)

	CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_FULL_REGISTRATION_INFO)
	CMBS_PARSE_CASE(CMBS_IE_HAN_READ_DEVICE_FULL_REGISTRATION_INFO_RES_REASON)

	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_TBR6_CARRIER)
	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_TBR6_PERIOD)
	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_DEVICE_IPUI)
	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_TBR6_STATISTICS)

	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_CARRIER)
	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_SLOT)
	CMBS_PARSE_CASE(CMBS_IE_HAN_REQ_FAILURE_REASON)
	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_PAGING_INTERVAL)
	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_PAGING_INTERVAL_MODIFICATION_FAILURE_REASON)
	CMBS_PARSE_CASE(CMBS_IE_HAN_NODE_SETUP_ATTEMPTS_ENABLE)
	CMBS_PARSE_CASE(CMBS_IE_HAN_LINK_RELEASE_REASON)
	CMBS_PARSE_CASE(CMBS_IE_HAN_MAX_NUM_OF_DEVICES)

	CMBS_PARSE_CASE(CMBS_IE_HAN_GROUP_ID)
	CMBS_PARSE_CASE(CMBS_IE_HAN_GROUP_COMMAND_FAIL_REASON)
	CMBS_PARSE_CASE(CMBS_IE_HAN_UNIT_ID)
	CMBS_PARSE_CASE(CMBS_IE_HAN_GROUP_LIST_ENTRIES)

	CMBS_PARSE_CASE(CMBS_IE_HAN_MSG_RSSI)

	CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_BLACK_LIST_DELETE_ERROR_STATUS_REASON)

	CMBS_PARSE_CASE(CMBS_IE_HAN_ULE_CAPABILITIES)	

	CMBS_PARSE_CASE(CMBS_IE_HAN_BROADCAST_CHANNEL_ID)	

	CMBS_PARSE_CASE(CMBS_IE_HAN_BROADCAST_CHANNEL_TABLE_ENTRIES)	

	CMBS_PARSE_CASE( CMBS_IE_HAN_PVC_RESET_REQ_FAIL_REASON)
	CMBS_PARSE_CASE( CMBS_IE_HAN_PVC_RESET_REQ_STATE_FAIL_REASON)
	CMBS_PARSE_CASE( CMBS_IE_HAN_PVC_RESET_REQ_STATE)

	CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_TABLE_EXTENDED_PHASE_2_ENTRIES)

	CMBS_PARSE_CASE(CMBS_IE_HAN_DEVICE_MANGER_READ_DEVICE_SUB_PHASE_2_INFO)
				
	case CMBS_IE_USER_DEFINED_START:
    case (CMBS_IE_USER_DEFINED_START + 1):
    case (CMBS_IE_USER_DEFINED_START + 2):
    case (CMBS_IE_USER_DEFINED_START + 3):
    case (CMBS_IE_USER_DEFINED_START + 4):
    case (CMBS_IE_USER_DEFINED_START + 5):
    case (CMBS_IE_USER_DEFINED_START + 6):
    case (CMBS_IE_USER_DEFINED_START + 7):
    case (CMBS_IE_USER_DEFINED_START + 8):
    case (CMBS_IE_USER_DEFINED_END):
        APPCMBS_LOG("CMBS_IE_USER_DEFINED\n");
        break;

    default:
        APPCMBS_LOG("app_PrintIe2Log: IE:%d not implemented", u16_IE);
    }
    APPCMBS_LOG("\n");
    return u32_Pos;
}
//////////////////////////////////////////////////////////////////////////
static 	char IEToString_buffer[CMBS_MAX_IE_PRINT_SIZE];

void    app_IEToString(void *pv_IE, u16 u16_IE)
{
    u16 length;
    if ( !g_msgparser_enabled )
        return;

    length = app_PrintIe2Log(IEToString_buffer, sizeof(IEToString_buffer), pv_IE, u16_IE);
    if ( length >= CMBS_MAX_IE_PRINT_SIZE )
    {
        IEToString_buffer[CMBS_MAX_IE_PRINT_SIZE - 1] = 0;
        IEToString_buffer[CMBS_MAX_IE_PRINT_SIZE - 2] = '.';
        IEToString_buffer[CMBS_MAX_IE_PRINT_SIZE - 3] = '.';
        IEToString_buffer[CMBS_MAX_IE_PRINT_SIZE - 4] = '.';
    }

    tcx_PushToPrintQueue(IEToString_buffer, length);
    tcx_ForcePrintIe();
}

void app_set_msgparserEnabled(u8 value)
{
    g_msgparser_enabled = value;
}

u8 app_get_msgparserEnabled(void)
{
    return g_msgparser_enabled;
}
