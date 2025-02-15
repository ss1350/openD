/*!
*	\file			keyb_call.c
*	\brief			
*	\Author		kelbch 
*
*	@(#)	%filespec: keyb_call.c~NBGD53#48 %
*
*******************************************************************************
*	\par	History
*	\n==== History ============================================================
*	date      name     rev     action                           
*	--------------------------------------------------------------------------
* 05-May-2015 tcmc_asa  PR 14071: add RelReason to CALL_PROGRESS DISCONNECTING
* 15-Apr-2015 tcmc_asa  corrected call of app_SrvSubscriptionOpenExt
* 25-Mar-2015 tcmc_asa  changed app_SrvSubscriptionOpen to app_SrvSubscriptionOpenExt
* 19-Mar-2015 tcmc_asa  added parameter to app_SrvSubscriptionOpen
* 01-Dec-2014 tcmc_asa     addedd flexible tone volume for CW
* 07-Mar-2014 tcmc_asa  ---GIT--   added CMBS_INBAND_ITALY
* 12-Jun-2013 tcmc_asa - GIT - merge 2.99.x changes to 3.x branch
* 28-Jan-2013 tcmc_asa 40      added keyb_CallInbandCNIP() PR 3615
* 24-Jan-2013 tcmc_asa 39      added keyb_SetHostCodec(E_APPCALL_SWITCH_RECEIVED_CODEC)
* 24-May-2013 tcmc_asa 48      added Congestion tone (swiss)
* 03-May-2013 tcmc_asa 47      change inband tone handling to new layout
* 30-Apr-2013 tcmc_asa 46      added swiss tones
* 23-Jul-2012 tcmc_asa 42      added further OUTBAND tones, PR 3320
* 24-May-2012 tcmc_asa 38      added CMBS_TONE_CALL_WAITING_OUTBAND, PR 3205
* 22-May-2012 tcmc_asa 37      added CLIP type 's' in keyb_CallEstablish
* 22-May-2012 tcmc_asa 36      added keyb_MediaAutoSwitch()
* 16-Feb-2012 tcmc_asa 28      added CLIP type 't' in keyb_CallEstablish
* 13-Feb-2012 tcmc_asa 27      added further french and polish tones
* 02-Feb-2012 tcmc_asa 26      added CMBS_TONE_CALL_WAITING_FT_FR
* 02-Feb-2012 tcmc_asa 25      added CNIP from Contact list case (only new printf)    
* 12-Dec-2011 tcmc_asa 22      added CLIR changes     
* 18-sep-09		Kelbch		pj1029-479    add quick call demonstration component
*******************************************************************************/

#if	!defined( KEYB_CALL_H )
#define	KEYB_CALL_H


#if defined( __cplusplus )
extern "C"
{
#endif

#if defined( __cplusplus )
}
#endif

#endif	//	KEYB_CALL_H
//*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "cmbs_platf.h"
#include "cmbs_api.h"
#include "cmbs_dbg.h"
#include "cfr_ie.h"
#include "cmbs_util.h"
#include "cfr_mssg.h"
#include "appcmbs.h"
#include "apphan.h"
#include "appsrv.h"
#include "appcall.h"
#include "tcx_keyb.h"
#include <tcx_util.h>
#include "ListChangeNotif.h"
#include "ListsApp.h"

#ifdef WIN32
#define kbhit _kbhit
#else
extern int kbhit(void);
#endif

#define KEYB_CALL_INVALID_CALLID 0xFFFF

/***************************************************************
*	
* external defined function for a quick demonstration
*
****************************************************************/

extern void appcall_InfoCall(int n_Call);

extern u8 g_HoldResumeCfm;
extern u8 g_HoldCfm;

extern u8 g_TransferAutoCfm;
extern u8 g_ConfAutoCfm;

extern u8 g_EarlyMediaAutoCfm;
extern E_APPCALL_PREFERRED_CODEC g_HostCodec;
extern u16 g_u16_DemoCallId;
extern st_AFEConfiguration AFEconfig;
extern PST_CALL_OBJ _appcall_CallObjGetById(u16 u16_CallId);

extern void appmedia_CallObjMediaInternalConnect(int channel, int context, int connect);

extern void keyb_AddPhoneABToContactList(void);

extern void keyb_AddLine0ToLineSettingsList(void);

extern void  keyb_AddDandAContactList(void);

void keyb_CallAnswer(void);
void keyb_TonePlayDTMF(void);

E_CMBS_RC keyb_HostConfigureFxsPort(u8 u8_SrxFxs, u8 u8_FxsChannel, u8 u8_ChannelMode, u32 u32_SlotScheme);

extern E_CMBS_RC GetContactNameByNumber(IN char * psz_Number,  OUT char * psz_Name);

/***************************************************************
*	
* global variable
*
****************************************************************/

u16 g_u16_PhoneACallId = APPCALL_NO_CALL;
u16 g_u16_PhoneBCallId = APPCALL_NO_CALL;
u16 g_u16_PhoneCCallId = APPCALL_NO_CALL;

typedef enum
{
    FXS_EV_UNDEF,
    FXS_EV_OFFHOOK, /*!<  OffHook event       */
    FXS_EV_ONHOOK,  /*!<  OnHook event        */
    FXS_EV_KEYPAD,  /*!<  Keypad event        */
    FXS_EV_OFFHOOK_INT, /* Off hook to make inter com call (call class)*/
    FXS_EV_OFFHOOK_EXT, /* Off hook to make External call (call class)*/
    FXS_EV_PARL_EXT_CALL,
    FXS_EV_PARL_INT_CALL,
    FXS_EV_CW_ACCEPT,
    FXS_EV_CALL_HOLD,
    FXS_EV_CALL_RESUME,
    FXS_EV_CALL_TRFR,
    FXS_EV_CALL_CONF,
    FXS_EV_CALL_RELEASE,
    FXS_EV_CALL_TOGGLE,
    FXS_EV_HOST_CONFIG,
    FXS_EV_REG_FXS,
    FXS_EV_MAX
} FXS_EV;



extern u8 g_u8_ToneType;
u16 g_u16_ToneVolume = 0;

void keyb_ReleaseNotify(u16 u16_CallId)
{
    if ( u16_CallId == g_u16_DemoCallId )
    {
        g_u16_DemoCallId = APPCALL_NO_CALL;
    }
    if ( u16_CallId == g_u16_PhoneACallId )
    {
        g_u16_PhoneACallId = APPCALL_NO_CALL;
    }
    if ( u16_CallId == g_u16_PhoneBCallId )
    {
        g_u16_PhoneBCallId = APPCALL_NO_CALL;
    }
    if ( u16_CallId == g_u16_PhoneCCallId )
    {
        g_u16_PhoneCCallId = APPCALL_NO_CALL;
    }

}

//		========== 	keyb_CallInfo ===========
/*!
        \brief				 print the call information for demonstration line

        \param[in,out]		 <none>

        \return				 <none>

*/

void keyb_CallInfo(void)
{
    if ( g_u16_DemoCallId != APPCALL_NO_CALL )
    {
        appcall_InfoCall(g_u16_DemoCallId);
    }
}

//		========== 	keyb_IncCallWB ===========
/*!
        \brief				 starts an incoming WB call for demonstration line\n
                           in a active call it launch codec change to WB

        \param[in,out]		 <none>

        \return				 <none>
      
      The demonstration line uses the automat of appcall component\n
        CLI            : 1234
      Ringing Pattern: standard
      Handsets       : 12345
      CNAME		   : CMBS WB
*/

void keyb_IncCallWB(void)
{
    ST_APPCALL_PROPERTIES st_Properties[6];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };

    // put codecs priority (WB, NB)
    sprintf(s_codecs, "%d,%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_WB), (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    if ( g_u16_DemoCallId == APPCALL_NO_CALL )
    {

        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "1234\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = "Call WB\0",
           st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "1\0";
        st_Properties[5].e_IE = CMBS_IE_MELODY;
        st_Properties[5].psz_Value = "1\0";

        g_u16_DemoCallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_DemoCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Switch to wide band audio\n");
        appmedia_CallObjMediaOffer(g_u16_DemoCallId, 'w');
    }
}

//		========== 	keyb_IncCallNB ===========
/*!
        \brief				 starts an incoming NB call for demonstration line\n
                           in a active call it launch codec change to NB

        \param[in,out]		 <none>

        \return				 <none>
      
      The demonstration line uses the automat of appcall component\n
        CLI            : 5678
      Ringing Pattern: standard
      Handsets       : 12345
      CNAME		   : CMBS NB
*/

void keyb_IncCallNB(void)
{
    ST_APPCALL_PROPERTIES st_Properties[6];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
    sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    if ( g_u16_DemoCallId == APPCALL_NO_CALL )
    {
        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "p5678\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = "Call NB\0";
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "1\0";
        st_Properties[5].e_IE = CMBS_IE_MELODY;
        st_Properties[5].psz_Value = "1\0";

        g_u16_DemoCallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_DemoCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Switch to wideband audio\n");
        appmedia_CallObjMediaOffer(g_u16_DemoCallId, 'n');
    }

}

// Establish a temp call (similar to NB call) for Splash ring
// We do not use g_u16_DemoCallId as user cannot send events to this call
void keyb_SplashRing(u8 u8_SplashTimer)
{
    ST_APPCALL_PROPERTIES st_Properties[6];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
    u16 TempCallId;

    sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    {
        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "p5678\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = "Call NB\0";
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "1\0";
        st_Properties[5].e_IE = CMBS_IE_MELODY;
        st_Properties[5].psz_Value = "1\0";

        TempCallId = appcall_EstablishSplashRing(st_Properties, 5, u8_SplashTimer);
        // For debug of SplashRing: TempCallId may be stored also in g_u16_DemoCallId
        if ( TempCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
}
//		========== 	keyb_IncCallNB_G711A_OTA ===========
/*!
        \brief      starts an incoming NB call using G.711 A-law OTA (requires support in PP)\n
                    in a active call it launch codec change to NB and G.711 A-law OTA

        \param[in,out]		 <none>

        \return				 <none>
*/
void keyb_IncCallNB_G711A_OTA(void)
{
    ST_APPCALL_PROPERTIES st_Properties[7];
    char s_codecs[2] = { 0 };
    char s_codecsOTA[2] = { 0 };
    sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));
    sprintf(s_codecsOTA, "%d", (CMBS_NB_CODEC_OTA_G711A));

    if ( g_u16_DemoCallId == APPCALL_NO_CALL )
    {
        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "p5678\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = "G.711 OTA\0";
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "0\0";
        st_Properties[5].e_IE = CMBS_IE_NB_CODEC_OTA;
        st_Properties[5].psz_Value = s_codecsOTA;
        st_Properties[6].e_IE = CMBS_IE_MELODY;
        st_Properties[6].psz_Value = "1\0";

        g_u16_DemoCallId = appcall_EstablishCall(st_Properties, 6);
        if ( g_u16_DemoCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Switch to NB Linear PCM16 with G.711 A-law OTA\n");
        appmedia_CallObjMediaOffer(g_u16_DemoCallId, 'g');
    }
}

//		========== 	keyb_IncCallWB_aLAW_mLAW ===========
/*!
        \brief				 starts an incoming WB call for demonstration line\n
                           in a active call it launch codec change to WB

        \param[in,out]		 <none>

        \return				 <none>
      
      The demonstration line uses the automat of appcall component\n
        CLI            : 1234
      Ringing Pattern: standard
      Handsets       : 12345
      CNAME		   : CMBS WB
*/

void keyb_IncCallWB_aLAW_mLAW(void)
{
    ST_APPCALL_PROPERTIES st_Properties[5];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };

    // put codecs priority (WB, NB)
    sprintf(s_codecs, "%d,%d", (CMBS_AUDIO_CODEC_PCMA_WB), (CMBS_AUDIO_CODEC_PCMU_WB));

    if ( g_u16_DemoCallId == APPCALL_NO_CALL )
    {

        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "p1234\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = "Call WB_aLaw_mLaw\0";
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "1\0";

        g_u16_DemoCallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_DemoCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Switch to wide band alaw\\mlaw audio\n");
        appmedia_CallObjMediaOffer(g_u16_DemoCallId, 'a');
    }

}

//		========== 	keyb_IncCallRelease ===========
/*!
        \brief				 release call on demonstration line

        \param[in,out]		 <none>

        \return				 <none>

*/

void keyb_IncCallRelease(void)
{
    ST_APPCALL_PROPERTIES st_Properties;

    if ( g_u16_DemoCallId != APPCALL_NO_CALL )
    {
        st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
        st_Properties.psz_Value = "0\0";

        appcall_ReleaseCall(&st_Properties, 1, g_u16_DemoCallId, NULL);
        g_u16_DemoCallId = APPCALL_NO_CALL;
    }
}


//		========== 	keyb_IncCall_DA_PhA_WB ===========
/*!
        \brief				 starts an incoming WB call for demonstration line\n
                           in a active call it launch codec change to WB

        \param[in,out]		 <none>

        \return				 <none>
      
      The demonstration line uses the automat of appcall component\n
        CLI            : 1234
      Ringing Pattern: standard
      Handsets       : 12345
      CNAME		   : CMBS WB
*/

void keyb_IncCall_DA_PhA_WB(void)
{
    ST_APPCALL_PROPERTIES st_Properties[5];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
	static char sz_CallerName[30] = {0};

    // put codecs priority (WB, NB)
    sprintf(s_codecs, "%d,%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_WB), (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    if ( g_u16_PhoneACallId == APPCALL_NO_CALL )
    {
        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "121\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = "h123\0";
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
		if (GetContactNameByNumber (st_Properties[0].psz_Value, sz_CallerName) == CMBS_RC_OK)
		{
			st_Properties[0].psz_Value = "c121\0";
			st_Properties[3].psz_Value = sz_CallerName;
		}
		else
		{
        	st_Properties[3].psz_Value = "Phone A\0";
		}
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "0\0";

        g_u16_PhoneACallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_PhoneACallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Phone A already in a call!\n");
    }
}


void keyb_IncCall_DA_PhA_NB(void)
{
    ST_APPCALL_PROPERTIES st_Properties[5];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
    sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    if ( g_u16_PhoneACallId == APPCALL_NO_CALL )
    {
        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "121\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = "Phone A\0";
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "0\0";

        g_u16_PhoneACallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_PhoneACallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Phone A already in a call!\n");
    }

}

void keyb_IncCall_DA_PhB_WB(void)
{
    ST_APPCALL_PROPERTIES st_Properties[5];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
    char s_PhoneBCNIP[] = { 0x50, 0x68, 0x6F, 0x6E, 0x65, 0x20, 0x42, 0x20, 0xC3, 0xA9, 0xC3, 0xA0, 0xC3, 0xBC, 0x00 }; //Phone B + (UTF-8 characters)

    // put codecs priority (WB, NB)
    sprintf(s_codecs, "%d,%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_WB), (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    if ( g_u16_PhoneBCallId == APPCALL_NO_CALL )
    {
        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "120\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = s_PhoneBCNIP;
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "0\0";

        g_u16_PhoneBCallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_PhoneBCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Phone A already in a call!\n");
    }
}


void keyb_IncCall_DA_PhB_NB(void)
{
    ST_APPCALL_PROPERTIES st_Properties[5];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
    char s_PhoneBCNIP[] = { 0x50, 0x68, 0x6F, 0x6E, 0x65, 0x20, 0x42, 0x20, 0xC3, 0xA9, 0xC3, 0xA0, 0xC3, 0xBC, 0x00 }; //Phone B + (UTF-8 characters)
    sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    if ( g_u16_PhoneBCallId == APPCALL_NO_CALL )
    {
        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "120\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = s_PhoneBCNIP;
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "1\0";

        g_u16_PhoneBCallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_PhoneBCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Phone A already in a call!\n");
    }
}

void keyb_IncCall_DA_PhC_NB(void)
{
    ST_APPCALL_PROPERTIES st_Properties[5];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
    sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));

    if ( g_u16_PhoneCCallId == APPCALL_NO_CALL )
    {
        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
        st_Properties[0].psz_Value = "1002\0";
        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
        st_Properties[1].psz_Value = ALL_HS_STRING;
        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
        st_Properties[2].psz_Value = s_codecs;
        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[3].psz_Value = "Phone C\0";
        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
        st_Properties[4].psz_Value = "0\0";

        g_u16_PhoneCCallId = appcall_EstablishCall(st_Properties, 5);
        if ( g_u16_PhoneCCallId == APPCALL_NO_CALL )
        {
            printf("Call can not be set-up!\n");
        }
    }
    else
    {
        printf("Phone A already in a call!\n");
    }
}

void keyb_IncCallRelease_PhA(void)
{
    ST_APPCALL_PROPERTIES st_Properties;

    if ( g_u16_PhoneACallId != APPCALL_NO_CALL )
    {
        st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
        st_Properties.psz_Value = "0\0";

        appcall_ReleaseCall(&st_Properties, 1, g_u16_PhoneACallId, NULL);
        g_u16_PhoneACallId = APPCALL_NO_CALL;
    }
}

void keyb_IncCallRelease_PhB(void)
{
    ST_APPCALL_PROPERTIES st_Properties;

    if ( g_u16_PhoneBCallId != APPCALL_NO_CALL )
    {
        st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
        st_Properties.psz_Value = "0\0";

        appcall_ReleaseCall(&st_Properties, 1, g_u16_PhoneBCallId, NULL);
        g_u16_PhoneBCallId = APPCALL_NO_CALL;
    }
}

void keyb_IncCallRelease_PhC(void)
{
    ST_APPCALL_PROPERTIES st_Properties;

    if ( g_u16_PhoneCCallId != APPCALL_NO_CALL )
    {
        st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
        st_Properties.psz_Value = "0\0";

        appcall_ReleaseCall(&st_Properties, 1, g_u16_PhoneCCallId, NULL);
        g_u16_PhoneCCallId = APPCALL_NO_CALL;
    }
}

//      ========== keyb_SetHostCodec===========
/*!
        \brief              set host codec
        \param[in,out]      <prefered codec>
        \return             <none>

*/
void keyb_SetHostCodec(E_APPCALL_PREFERRED_CODEC e_PreferredCodec)
{
    g_HostCodec = e_PreferredCodec;
}



//		========== 	keyb_DandATests ===========
/*!
        \brief				 Helps to Perform D&A TESTS

        \param[in,out]		 <none>

        \return				 <none>

*/

void keyb_DandATests(void)
{
    int n_Keep = TRUE;

    while (n_Keep)
    {
        //      tcx_appClearScreen();

        printf("\n======Dosch & Amand testing menu==========================\n\n");
        printf("Registration Window %s\n\n", g_cmbsappl.RegistrationWindowStatus ? "OPEN" : "CLOSED");
        appcall_InfoPrint();
        printf("\n======Call Control===============================\n\n");
        printf("a => G.722 call from Phone A\n");
        printf("s => G.726 call from Phone A\n");
        printf("b => G.722 call from Phone B\n");
        printf("n => G.726 call from Phone B\n");
        printf("c => G.726 call from Phone C\n");

        printf("A => release call from Phone A\n");
        printf("B => release call from Phone B\n");
        printf("C => release call from Phone C\n");

        printf("e => Call Answer\n");

        printf("r => Open Registration Window\n");

        printf("\n------------------------------------------\n\n");
        printf("i => call/line infos \n");
        printf("I => Insert Phone A, Phone B to contact list \n");
        printf("H => Insert DandA contacts  \n");
        printf("L => Insert Line 0 to line settings list \n");
        printf("------------------------------------------\n");
        printf("Set host preferred CODEC \n");
        printf("   6 => switch received codec \n");
        printf("   7 => use received codec \n");
        printf("   8 => WB \n");
        printf("   9 => NB \n");
        printf("------------------------------------------\n");
        printf("Set CODEC of call Id 0/1\n");
        printf("   1 => change codec to NB, call id 0\n");
        printf("   2 => change codec to WB, call id 0\n");
        printf("   3 => change codec to NB, call id 1\n");
        printf("   4 => change codec to WB, call id 1\n");
        printf("------------------------------------------\n");
        printf("List Change Aggregation\n");
        printf("   x => Clear all list change \n");
        printf("   y => Send all list change\n");
        printf("------------------------------------------\n");
        printf("q => return to interface \n");

        switch (tcx_getch())
        {
        case ' ':
            tcx_appClearScreen();
            break;

        case 'a':
            keyb_IncCall_DA_PhA_WB();
            break;
        case 's':
            keyb_IncCall_DA_PhA_NB();
            break;

        case 'b':
            keyb_IncCall_DA_PhB_WB();
            break;

        case 'n':
            keyb_IncCall_DA_PhB_NB();
            break;

        case 'c':
            keyb_IncCall_DA_PhC_NB();
            break;

        case 'A':
            keyb_IncCallRelease_PhA();
            break;

        case 'B':
            keyb_IncCallRelease_PhB();
            break;

        case 'C':
            keyb_IncCallRelease_PhC();
            break;

        case 'i':
            // call/line info
            tcx_appClearScreen();
            appcall_InfoPrint();
            printf("Press Any Key\n ");
            tcx_getch();
            break;

        case 'I':
            keyb_AddPhoneABToContactList();
            break;

        case 'H':
            keyb_AddDandAContactList();
            break;

        case 'D':
            keyb_AddDandAContactList();
            break;

        case 'L':
            keyb_AddLine0ToLineSettingsList();
            break;

        case 'e':
            keyb_CallAnswer();
            break;

        case 'r':
            // for this purpose it's sufficient to open registration for DECT Handsets (not ULE devices)
            app_SrvSubscriptionOpenExt(120, CMBS_HS_REG_ENABLE_HANDSET);
            break;

        case '6':
            keyb_SetHostCodec(E_APPCALL_SWITCH_RECEIVED_CODEC);
            break;

        case '7':
            keyb_SetHostCodec(E_APPCALL_PREFERRED_CODEC_RECEIVED_CODEC);
            break;

        case '8':
            keyb_SetHostCodec(E_APPCALL_PREFERRED_CODEC_WB);
            break;

        case '9':
            keyb_SetHostCodec(E_APPCALL_PREFERRED_CODEC_NB);
            break;

        case '1':
            appmedia_CallObjMediaOffer(0, 'n');
            break;

        case '2':
            appmedia_CallObjMediaOffer(0, 'w');
            break;

        case '3':
            appmedia_CallObjMediaOffer(0, 'n');
            break;

        case '4':
            appmedia_CallObjMediaOffer(0, 'w');
            break;

        case 'x':
			List_ResetAllListChangeDetails();
			break;

        case 'y':
			ListChangeNotif_SendAllListChangeDetails();
			break;

        case 'q':
            n_Keep = FALSE;
            break;
        }
    }
}



//		========== _keyb_LineIdInput  ===========
/*!
        \brief	input Line ID 

        \param[in]		 <none> 

        \return			<u16> return Line ID in binary form

*/
u16 _keyb_LineIdInput(void)
{
    u32 u32_LineID;

    printf("Enter Line ID - Line ID 0xFF means no line ID selected ");
    tcx_scanf("%u", &u32_LineID);

    return (u16)u32_LineID;
}

//		========== _keyb_CallIdInput  ===========
/*!
        \brief	input Call ID 

        \param[in]		 <none> 

        \return			<u16> return Call ID in binary form

*/
u16 _keyb_CallIdInput(void)
{
    u32 u32_CallId;
    PST_CALL_OBJ pst_Call;

    printf("Enter Call ID ");
    tcx_scanf("%u", &u32_CallId);

    pst_Call = _appcall_CallObjGetById(u32_CallId);
    if ( pst_Call )
        if ( pst_Call->u32_CallInstance )
            return (u16)u32_CallId;

    return KEYB_CALL_INVALID_CALLID;
}

u16 _keyb_CallInstanceInput(void)
{
    u32 u32_CallInstance;

    printf("Enter Call Instance ");
    tcx_scanf("%u", &u32_CallInstance);

    return (u16)u32_CallInstance;
}


//		========== _keyb_SlotInput  ===========
/*!
        \brief	Input IOM Slot Number, 8 bits as one slot

        \param[in]		 <none> 

        \return			<u16> return Slot Number 

*/
u16 _keyb_SlotInput(void)
{
    u16 u16_Slot;
    char InputBuffer[4];

    printf("Enter IOM Slot Number (8Bits as a slot):");

    memset(InputBuffer, 0, sizeof(InputBuffer));
    tcx_gets(InputBuffer, sizeof(InputBuffer));
    u16_Slot = (u16)atoi(InputBuffer);
    return u16_Slot;
}

char ch_caller_party[20];

//		========== keyb_CallEstablish ===========
/*!
        \brief				 establish a call from Host to Target {INCOMING Call from view of CMBS Target}
        \param[in,out]		 <none>
        \return				 <none>

      CMBS_IE_CALLERPARTY, CMBS_CALLEDPARTY, CMBS_IE_MEDIADESCRIPTOR
      
*/
void keyb_CallEstablish(void)
{
    ST_APPCALL_PROPERTIES st_Properties[8];
    int n_Prop = 0;
    char ch_cli[40];
    char ch_cni[40];
    char ch_cld[20];
    char ch_clineid[20];
    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
    char codec;
    char ch_melody[20];
    char ch_call_direction[20];
    char ch_linesubtype[10];

    printf("\nEnter Properties\n");
    printf("CLIP (Presentation: \n");
    printf("        r = Restricted\n");
    printf("        p = Allowed \n");
    printf("         s = Allowed - National number, National Std. plan \n");
    printf("         t = Allowed - National number, Private Plan \n");
    printf("        n = not available\n");
    printf("        c = CNIP from contact list, using CLIP\n");
    printf("   CLIP:");
    tcx_gets(ch_cli, sizeof(ch_cli));

    printf("\nCNIP: (0 = not available):");
    tcx_gets(ch_cni, sizeof(ch_cni));

    printf("\nLineID:");
    tcx_gets(ch_clineid, sizeof(ch_clineid));
	
    printf("\nDestination: h[handsets in hex]d[1st device in dec]d[2nd device in dec]etc... (e.g. for Handsets 1 and 10 and devices 1 and 20 enter h1Ad1d20) ");
    tcx_gets(ch_cld, sizeof(ch_cld));

    printf("\nAudio: [{w/n/8/a/u/A/U}]:");
    codec = tcx_getch();

    // printf( "Melody [{m}{1...8}] (e.g. for alerting pattern 3 (0x43) enter m3) :" );
    printf("\nMelody: [{1...F}] (e.g. for alerting pattern 3 (0x43) enter 3) :");
    tcx_gets(ch_melody, sizeof(ch_melody));
    // ch_melody = tcx_getch();

    printf("\nLine Type: (n -Not available, 0-External, 1-Internal)");
    tcx_gets(ch_linesubtype, sizeof(ch_linesubtype));


    switch (codec)
    {
    case 'w':
        sprintf(s_codecs, "%d,%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_WB), (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));
        break;
    case 'a':
        sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCMA));
        break;
    case 'u':
        sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCMU));
        break;
    case '8':
        sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM8));
        break;
    case 'A':
        sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCMA_WB));
        break;
    case 'U':
        sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCMU_WB));
        break;

    case 'n':
    default:
        sprintf(s_codecs, "%d", (CMBS_AUDIO_CODEC_PCM_LINEAR_NB));
    }

    printf("\nCallDirection: [0 - incoming; 1 - outgoing or recall of outgoing] :");
    tcx_gets(ch_call_direction, sizeof(ch_call_direction));

	memset(&st_Properties, 0x00, sizeof(st_Properties));
    st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
    st_Properties[0].psz_Value = ch_cli;
    st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
    st_Properties[1].psz_Value = ch_cld;
    st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
    st_Properties[2].psz_Value = s_codecs;
    st_Properties[3].e_IE = CMBS_IE_LINE_ID;
    st_Properties[3].psz_Value = ch_clineid;

    if ( strlen(ch_cni) && (ch_cni[0] != '0') )
    {
        st_Properties[4].e_IE = CMBS_IE_CALLERNAME;
        st_Properties[4].psz_Value = ch_cni;
        n_Prop = 5;
    }
    else
    {
        n_Prop = 4;
    }
    st_Properties[n_Prop].e_IE = CMBS_IE_MELODY;
    st_Properties[n_Prop].psz_Value = ch_melody;
    n_Prop++;
    st_Properties[n_Prop].e_IE = CMBS_IE_CALL_DIRECTION;
    st_Properties[n_Prop].psz_Value = ch_call_direction;
    n_Prop++;

    if ( strlen(ch_linesubtype) && (ch_linesubtype[0] != 'n') )
    {
        st_Properties[n_Prop].e_IE = CMBS_IE_LINE_SUBTYPE;
        st_Properties[n_Prop].psz_Value = ch_linesubtype;
        n_Prop++;
    }

    appcall_EstablishCall(st_Properties, n_Prop);
}

void keyb_SplashRing_GetDuration(void)
{
    u8 u8_SplashTimer;
    u32 u32_Value;

    printf("\nEnter Splash ring Duration (0..50 X 100ms units): ");
    tcx_scanf("%d", &u32_Value);

    u8_SplashTimer = (u8)u32_Value;
    printf("Splash Ring Duration is: %d \n", u8_SplashTimer);

    keyb_SplashRing(u8_SplashTimer);
}

//		========== keyb_CallRelease ===========
/*!
        \brief				Release a call from Host to Target side
        \param[in,out]		<none>
        \return				<none>

      CMBS_IE_CALLRELEASE_REASON
*/
void keyb_CallRelease(void)
{
    u16 u16_CallId;
    char ch_Reason[2];
    char ch;
    char HsNr[20];
    ST_APPCALL_PROPERTIES st_Properties;

    u16_CallId = _keyb_CallIdInput();
    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {
        printf("\nReason \n");
        printf("0 => Normal\t 1 => Abnormal\n");
        printf("2 => Busy\t 3 => unknown Number\n");

        ch_Reason[0] = tcx_getch();
        ch_Reason[1] = 0;

        st_Properties.e_IE = CMBS_IE_CALLRELEASE_REASON;
        st_Properties.psz_Value = ch_Reason;

        printf("\nRelease Specific Handset (h) / device (d) / Regular release (press Enter) : ");
        ch = tcx_getch();
        if ( ch == 'h' )
        {
            printf("\nEnter Handset No : ");
            tcx_gets(HsNr, sizeof(HsNr));

            st_Properties.e_IE = CMBS_IE_HANDSETS;
            st_Properties.psz_Value = HsNr;
        }
        else if ( ch == 'd' )
        {
            printf("\nEnter Device Id : ");
            tcx_gets(HsNr, sizeof(HsNr));

            st_Properties.e_IE = CMBS_IE_HAN_DEVICE;
            st_Properties.psz_Value = HsNr;
        }

        appcall_ReleaseCall(&st_Properties, 1, u16_CallId, NULL);

        keyb_ReleaseNotify(u16_CallId);
    }
}

//		========== keyb_CallInbandInfo ===========
/*!
        \brief				Host changes Caller ID of an active call (for transfer)
        \param[in,out]		<none>
        \return				<none>

      CMBS_IE_CALLPROGRESS
*/
void keyb_CallInbandInfo(void)
{
    u16 u16_CallId;
    char sCLIP[32];

    u16_CallId = _keyb_CallIdInput();
    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {
        /* for CLIP update demo, we will use constant presentation indicator (will not prompt user) */
        sCLIP[0] = CMBS_ADDR_PROPTYPE_NATIONAL;
        sCLIP[1] = CMBS_ADDR_PRESENT_ALLOW;

        printf("\nNew Caller ID: ");
        tcx_gets(sCLIP + 2, sizeof(sCLIP) - 2);

        appcall_CallInbandInfo(u16_CallId, sCLIP);
    }
}

//		========== keyb_CallInbandCNIP ===========
/*!
        \brief				Host sends CNIP during active call (matching dial number in Contact list)
        \param[in,out]		<none>
        \return				<none>

      CMBS_IE_CALLPROGRESS
*/
void keyb_CallInbandCNIP(void)
{
    u16 u16_CallId;
    char sCalledName[CMBS_CALLED_NAME_MAX_LEN + 1];
    char sCalledFirstName[CMBS_CALLED_NAME_MAX_LEN];
    char ch_cli[40];

    ch_cli[0] = 0;

    u16_CallId = _keyb_CallIdInput();

    printf("\nType (c= Contact List, n = Network provided): ");
    switch (tcx_getch())
    {
    case 'c':
        // Scrrening indicator 'User provided'
        sCalledName[0] = CMBS_ADDR_SCREENING_UTF8_USER;
        printf("Contact List");
        break;

    case 'n':
        // Scrrening indicator 'Network provided'
        // NOTE: In that case CLIP is needed additionally, thus this use case is not fully covered here
        sCalledName[0] = CMBS_ADDR_SCREENING_UTF8_NWK;

        printf("Network provided");
        printf("\nCalled Party Number: ");
        tcx_gets(ch_cli, sizeof(ch_cli));
        break;
    }

    printf("\nCalled Party Sir Name  : ");
    tcx_gets(sCalledName + 1, sizeof(sCalledName) - 1);
    
    printf("Called Party First Name: ");
    tcx_gets(sCalledFirstName, sizeof(sCalledFirstName));

    /* combine the full message with Screening indicator, name and first name */
    if ( sCalledName[0] == CMBS_ADDR_SCREENING_UTF8_NWK )
    {
        appcall_CallInbandInfoCNIP(u16_CallId, sCalledName, sCalledFirstName, ch_cli);
    }
    else
    {
        appcall_CallInbandInfoCNIP(u16_CallId, sCalledName, sCalledFirstName, NULL);
    }
}

extern ST_CALL_OBJ g_call_obj[APPCALL_CALLOBJ_MAX];
void keyb_CallerNameNumber(void)
{
    u8 u8_Num[5] = { '1', '2', '3', '4', '5' };
    u8 u8_Name[6] = { 'A', 'B', 'C', 'D', 'E', 'F', };
    PST_CALL_OBJ pst_Call;
    ST_IE_CALLERPARTY st_CallerParty;
    ST_IE_CALLERNAME st_CallerName;
    u16 u16_CallId;
    void *pv_RefIEList = cmbs_api_ie_GetList();
    u16_CallId = _keyb_CallIdInput();
    pst_Call = g_call_obj + u16_CallId;
    st_CallerParty.u8_AddressProperties = CMBS_ADDR_PROPTYPE_UNKNOWN | CMBS_ADDR_PROPPLAN_UNKNOWN;
    //THis field can be set for Caller ID anonymous feature
    st_CallerParty.u8_AddressPresentation = CMBS_ADDR_PRESENT_ALLOW;
    st_CallerParty.u8_AddressLen = 5;
    st_CallerParty.pu8_Address = (u8 *)u8_Num;

    /*Add Caller Name IE */
    st_CallerName.pu8_Name = (u8 *)u8_Name;
    st_CallerName.u8_DataLen = 6;
    cmbs_api_ie_CallerPartyAdd(pv_RefIEList, &st_CallerParty);
    cmbs_api_ie_CallerNameAdd(pv_RefIEList, &st_CallerName);
    cmbs_api_ie_CallInstanceAdd(pv_RefIEList, pst_Call->u32_CallInstance);
    cmbs_dee_CallInbandInfo(g_cmbsappl.pv_CMBSRef, pv_RefIEList);
}

//		========== keyb_CallProgress ===========
/*!
        \brief				call progress information from Host to Target side
        \param[in,out]		<none>
        \return				<none>

      CMBS_IE_CALLPROGRESS
*/
void keyb_CallProgress(char *psz_Value)
{
    u16 u16_CallId;
    ST_APPCALL_PROPERTIES st_Properties;

    u16_CallId = _keyb_CallIdInput();
    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {
        st_Properties.e_IE = CMBS_IE_CALLPROGRESS;
        st_Properties.psz_Value = psz_Value;
        appcall_ProgressCall(&st_Properties, 1, u16_CallId, NULL);
    }
}
//		========== keyb_CallAnswer ===========
/*!
        \brief				call answer from Host to Target side
        \param[in,out]		<none>
        \return				<none>

*/
void keyb_CallAnswer(void)
{
    u16 u16_CallId;
    u8 u8_WBCodec = 0;
    int i;
    ST_APPCALL_PROPERTIES st_Properties;
    PST_CALL_OBJ pst_Call;

    // get call ID
    u16_CallId = _keyb_CallIdInput();

    // get call object
    pst_Call = _appcall_CallObjGetById(u16_CallId);
    if ( !pst_Call )
    {
        printf("NO SUCH CALL WITH ID %d\n", u16_CallId);
        return;
    }
    // check WB support
    for (i = 0; i < pst_Call->u8_CodecsLength; i++)
    {
        if ( pst_Call->pu8_CodecsList[i] == CMBS_AUDIO_CODEC_PCM_LINEAR_WB )
        {
            u8_WBCodec = 1;
            break;
        }
    }

    // answer call
    memset(&st_Properties, 0, sizeof(st_Properties));
    appcall_AnswerCall(&st_Properties, 0, u16_CallId, NULL);

    // check do we need codec switch
    if ( pst_Call->e_Codec == CMBS_AUDIO_CODEC_PCM_LINEAR_WB && u8_WBCodec == 0 )
    {
        appmedia_CallObjMediaStop(0, u16_CallId, NULL);

        // wait for CMBS_EV_DEM_CHANNEL_STOP_RES
        SleepMs(200);        // wait 0.2 seconds
        appmedia_CallObjMediaOffer(u16_CallId, 'n');

        // wait for CMBS_EV_DEM_CHANNEL_OFFER_RES
#if 0 /* On MEDIA OFFER RES - depending on the codec confirmed by HOST media will be started*/
        SleepMs(200);        // wait 0.2 seconds

        appmedia_CallObjMediaStart(0, u16_CallId, 0xFF, NULL);
#endif
    }
    else if ( pst_Call->e_Codec == CMBS_AUDIO_CODEC_PCM_LINEAR_NB && u8_WBCodec == 1 )
    {
        appmedia_CallObjMediaStop(0, u16_CallId, NULL);

        // wait for CMBS_EV_DEM_CHANNEL_STOP_RES
        SleepMs(200);        // wait 0.2 seconds
        appmedia_CallObjMediaOffer(u16_CallId, 'w');

        // wait for CMBS_EV_DEM_CHANNEL_OFFER_RES
#if 0 /* On MEDIA OFFER RES - depending on the codec confirmed by HOST media will be started*/
        SleepMs(200);        // wait 0.2 seconds

        appmedia_CallObjMediaStart(0, u16_CallId, 0xFF, NULL);
#endif
    }
}

//      ========== keyb_HoldResumeCfm ===========
/*!
        \brief              set automatic accept for hold/resume
        \param[in,out]      <none>
        \return             <none>

*/
void keyb_HoldResumeCfm(void)
{
    u8 u8_Answ;
    printf("\nEnter automatic confirm for hold [Y/N]: ");
    u8_Answ = tcx_getch();
    if ( u8_Answ == 'y' || u8_Answ == 'Y' )
    {
        g_HoldCfm = 1;
    }
    else
    {
        g_HoldCfm = 0;
    }

    printf("\nEnter automatic confirm for resume hold [Y/N]: ");
    u8_Answ = tcx_getch();
    if ( u8_Answ == 'y' || u8_Answ == 'Y' )
    {
        g_HoldResumeCfm = 1;
    }
    else
    {
        g_HoldResumeCfm = 0;
    }
}

//      ========== keyb_EarlyMediaOnOffCfm ===========
/*!
        \brief              enables/disables auto early media
        \param[in,out]      <none>
        \return             <none>

*/
void keyb_EarlyMediaOnOffCfm(void)
{
    u8 u8_Answ;
    printf("\nEnter automatic early media [Y/N]: ");
    u8_Answ = tcx_getch();
    if ( u8_Answ == 'y' || u8_Answ == 'Y' )
    {
        g_EarlyMediaAutoCfm = 1;
    }
    else
    {
        g_EarlyMediaAutoCfm = 0;
    }
}

//      ========== keyb_TransferAutoCfm ===========
/*!
        \brief              set automatic accept for call transfer
        \param[in,out]      <none>
        \return             <none>

*/
void keyb_TransferAutoCfm(void)
{
    u8 u8_Answ;
    printf("\nEnter automatic confirm for transfer request [Y/N]: ");
    u8_Answ = tcx_getch();
    if ( u8_Answ == 'y' || u8_Answ == 'Y' )
    {
        g_TransferAutoCfm = 1;
    }
    else
    {
        g_TransferAutoCfm = 0;
    }
}


static E_CMBS_RC _keybcall_HostConfigureFxsPort(u8 u8_SrxFxs, u8 u8_FxsChannel, u8 u8_ChannelMode, u32 u32_SlotScheme)
{
    /*
    st_Data.pu8_Data[0] = FXS_EV_HOST_CONFIG
    st_Data.pu8_Data[1] = FXs Number
    st_Data.pu8_Data[2] = Channel Number
    st_Data.pu8_Data[3] = Channel Mode
    st_Data.pu8_Data[4] = SlotSchme MSB Byte1
    st_Data.pu8_Data[5] = SlotSchme MSB Byte2
    st_Data.pu8_Data[6] = SlotSchme MSB Byte3
    st_Data.pu8_Data[7] = SlotSchme MSB Byte4
    */
    ST_IE_DATA st_Data;
    u8 pu8_Data[10];
    pu8_Data[0] = FXS_EV_HOST_CONFIG;
    pu8_Data[1] = u8_SrxFxs; //FXS HS Number
    pu8_Data[2] = u8_FxsChannel;
    pu8_Data[3] = u8_ChannelMode;
    pu8_Data[4] = (u32_SlotScheme >> 8); //MSB
    pu8_Data[5] = (u32_SlotScheme & 0x00FF); //LSB
    st_Data.u16_DataLen = 6;
    st_Data.pu8_Data = pu8_Data;

    printf("\npu8_Data[4] = %x , pu8_Data[5] = %x \n", pu8_Data[4], pu8_Data[5]);

    return app_FXSEvent(&st_Data);
}



/*Configures FXS6 & FXS7 with Fixed Data */
void keyb_FixedFxsHostportConfigure(void)
{
    u32 u32_SloScheme;
    u8 HsNo, u8_ChannelNumber, u8_ChannelMode;
    /*Configuration for FXS-6 */
    HsNo = 6;
    u8_ChannelNumber = 0;
    u8_ChannelMode = 2; //E_TAPP_AUD_CHANNEL_MODE_PCM_16_WB;
    u32_SloScheme = 0x000f;
    printf("\n FIXED CHANNEL CONFIG: HsNo = %x,u8_ChannelNumber = %x, u8_ChannelMode = %x, Slot Scheme = %x \n", HsNo, u8_ChannelNumber, u8_ChannelMode, u32_SloScheme);
    _keybcall_HostConfigureFxsPort(HsNo, u8_ChannelNumber, u8_ChannelMode, u32_SloScheme); //API to be used by customers
    /*Configuration for FXS-7 */
    HsNo = 7;
    u8_ChannelNumber = 1;
    u8_ChannelMode = 2; //E_TAPP_AUD_CHANNEL_MODE_PCM_16_WB;
    u32_SloScheme = 0x00f0;
    printf("\n FIXED CHANNEL CONFIG: HsNo = %x,u8_ChannelNumber = %x, u8_ChannelMode = %x, Slot Scheme = %x \n", HsNo, u8_ChannelNumber, u8_ChannelMode, u32_SloScheme);
    _keybcall_HostConfigureFxsPort(HsNo, u8_ChannelNumber, u8_ChannelMode, u32_SloScheme); //API to be used by customers

}


void keyb_HostportConfigure(void)
{
    u32 u32_SloScheme;
    u8 u8_Type, HsNo, u8_ChannelNumber, u8_ChannelMode;
    printf("\n Enter the FXS Hs Number to be configured\n");
    HsNo = tcx_getch();
    HsNo = HsNo - 0x30;
    printf("\n Enter the Channel Number to be used for FXS\n");
    u8_ChannelNumber = tcx_getch();
    u8_ChannelNumber = u8_ChannelNumber - 0x30;
    printf("\n Enter the Channel Mode to be used for FXS\n");
    printf("\n0 - E_TAPP_AUD_CHANNEL_MODE_UNDEF       ");
    printf("\n1 - E_TAPP_AUD_CHANNEL_MODE_PCM_16_NB,   ");
    printf("\n2 - E_TAPP_AUD_CHANNEL_MODE_PCM_16_WB,   ");
    printf("\n3 - E_TAPP_AUD_CHANNEL_MODE_PCM_8_ALAW,  ");
    printf("\n4 - E_TAPP_AUD_CHANNEL_MODE_PCM_8_ULAW,  ");
    printf("\n5 - E_TAPP_AUD_CHANNEL_MODE_PCM_8,       ");
    printf("\n6 - E_TAPP_AUD_CHANNEL_MODE_WB_16_ALAW,  ");
    printf("\n7 - E_TAPP_AUD_CHANNEL_MODE_WB_16_ULAW,	");
    printf("\n8 - E_TAPP_AUD_CHANNEL_MODE_MAX   ");
    u8_ChannelMode = tcx_getch();
    u8_ChannelMode = u8_ChannelMode - 0x30; //get Integer
    printf("\nEnter Slot Scheme NB 0-0x0003  WB 1-0x000f \n NB 2-0x000C WB 3-0x00f0 \n NB 4-0x030 WB 5-0x0f00 \n NB 6-0x00C0 WB 7-0xF000 \n ");
    u8_Type = tcx_getch();
    u32_SloScheme = 0x0000;
    switch (u8_Type)
    {
    case '0':
        {
            u32_SloScheme = 0x0003;
        }
        break;
    case '1':
        {
            u32_SloScheme = 0x000f;
        }
        break;
    case '2':
        {
            u32_SloScheme = 0x000C;
        }
        break;
    case '3':
        {
            u32_SloScheme = 0x00f0;
        }
        break;
    case '4':
        {
            u32_SloScheme = 0x0030;
        }
        break;
    case '5':
        {
            u32_SloScheme = 0x0f00;
        }
        break;
    case '6':
        {
            u32_SloScheme = 0x00C0;
        }
        break;
    case '7':
        {
            u32_SloScheme = 0xF000;
        }
        break;
    }
    printf("\n HsNo = %x,u8_ChannelNumber = %x, u8_ChannelMode = %x, Slot Scheme = %x \n", HsNo, u8_ChannelNumber, u8_ChannelMode, u32_SloScheme);
    keyb_HostConfigureFxsPort(HsNo, u8_ChannelNumber, u8_ChannelMode, u32_SloScheme); //API to be used by customers
}

E_CMBS_RC keyb_FxsCATiqCalls(void)
{
    ST_IE_DATA st_Data;
    u8 HsNo;
    u8 DialDigit;
    u8 u8_Type;
    u8 pu8_Data[10];
    printf("\n Enter the FXS Hs Number from which CATiq key code to be sent\n");
    HsNo = tcx_getch();
    printf("\n Press-1 for Parallel EXT call\n");
    printf("\n Press-2 for Parallel INT call)\n");
    printf("\n Press-3 Call TOGGLE)\n");
    printf("\n Press-4 CW Accept)\n");
    u8_Type = tcx_getch();
    switch (u8_Type)
    {
    case '1':
        pu8_Data[0] = FXS_EV_PARL_EXT_CALL;
        pu8_Data[1] = HsNo - 0x30; //FXS HS Number
        st_Data.u16_DataLen = 2;
        break;
    case '2':
        pu8_Data[0] = FXS_EV_PARL_INT_CALL;
        pu8_Data[1] = HsNo - 0x30; //FXS HS Number
        printf("\n Enter the Destination HsNo for which the call to be made )\n");
        DialDigit = tcx_getch();
        pu8_Data[2] = DialDigit;
        st_Data.u16_DataLen = 3;
        break;
    case '3':
        pu8_Data[0] = FXS_EV_CALL_TOGGLE;
        pu8_Data[1] = HsNo - 0x30; //FXS HS Number
        printf("\n Enter the call Number to which TOGGLE to be made )\n");
        DialDigit = tcx_getch();
        pu8_Data[2] = DialDigit;
        st_Data.u16_DataLen = 3;
        break;
    case '4':
        pu8_Data[0] = FXS_EV_CW_ACCEPT;
        pu8_Data[1] = HsNo - 0x30; //FXS HS Number
        printf("\n Enter the call Number of CW call )\n");
        DialDigit = tcx_getch();
        pu8_Data[2] = DialDigit;
        st_Data.u16_DataLen = 3;
        break;
    }

    st_Data.pu8_Data = pu8_Data;

    return app_FXSEvent(&st_Data);
}

E_CMBS_RC keyb_FxsOffhook(void)
{
    ST_IE_DATA st_Data;
    u8 HsNo;
    u8 DialDigit;
    u8 u8_Type;
    u8 pu8_Data[10];
    printf("\n Enter the FXS Hs Number for which Off-hook to be sent\n");
    HsNo = tcx_getch();
    printf("\n Enter is it ANS(Press-1),INT OG CALL(Press-2) ,EXT OG CALL(Press-3)\n");
    u8_Type = tcx_getch();
    switch (u8_Type)
    {
    case '1':
        pu8_Data[0] = FXS_EV_OFFHOOK;
        pu8_Data[1] = HsNo - 0x30; //FXS HS Number
        st_Data.u16_DataLen = 2;
        break;
    case '2':
        pu8_Data[0] = FXS_EV_OFFHOOK_INT;
        pu8_Data[1] = HsNo - 0x30; //FXS HS Number
        printf("\n Enter the HS number to be dialed\n");
        DialDigit = tcx_getch();
        pu8_Data[2] = DialDigit;
        st_Data.u16_DataLen = 3;
        break;
    case '3':
        pu8_Data[0] = FXS_EV_OFFHOOK_EXT;
        pu8_Data[1] = HsNo - 0x30; //FXS HS Number
        printf("\n Enter the Ext number to be dialed\n");
        DialDigit = tcx_getch();
        pu8_Data[2] = DialDigit;
        //pu8_Data[3] = '5' - 0x30;//just hard coded to check
        pu8_Data[3] = '5'; //just hard coded to check
        st_Data.u16_DataLen = 4;
        break;
    }
    st_Data.pu8_Data = pu8_Data;
    return app_FXSEvent(&st_Data);
}

E_CMBS_RC keyb_FxsOnhook(void)
{
    ST_IE_DATA st_Data;
    u8 HsNo;
    u8 pu8_Data[10];
    printf("\n Enter the FXS Hs Number for which ON-hook to be sent\n");
    HsNo = tcx_getch();
    pu8_Data[0] = FXS_EV_ONHOOK;
    pu8_Data[1] = HsNo - 0x30; //FXS HS Number
    st_Data.pu8_Data = pu8_Data;
    st_Data.u16_DataLen = 2;

    return app_FXSEvent(&st_Data);
}

u8 keyb_Convert_Ascii_To_Hex(u8 u8_Ascii)
{
    u8 u8_Temp = 0;
    if ( (u8_Ascii >= '0') && (u8_Ascii <= '9') )
        u8_Temp = u8_Ascii - '0';
    else if ( (u8_Ascii >= 'a') && (u8_Ascii <= 'f') )
        u8_Temp = u8_Ascii - 'a' + 10;
    else if ( (u8_Ascii >= 'A') && (u8_Ascii <= 'F') )
        u8_Temp = u8_Ascii - 'A' + 10;
    return (u8_Temp);
}


E_CMBS_RC keyb_FxsDial(void)
{
    ST_IE_DATA st_Data;
    u8 u8_Digit, u8_Convert_Digit;
    u8 u8_HsNo, u8_Len, i;
    u8 pu8_Data[10];
    printf("\n Enter the FXS Hs Number for which digits to be sent\n");
    u8_HsNo = tcx_getch();
    printf("\n Enter the Length of  Number of Digits to be dialed\n");
    u8_Len = tcx_getch();
    for (i = 0; i < (u8_Len - 0x30); i++)
    {
        printf("\n Enter DIAL Diagit\n");
        u8_Digit = tcx_getch();
        u8_Convert_Digit = u8_Digit;
#if 0 //A-D need to be allowed for FXS
        if (((u8_Digit >= 'a') && (u8_Digit <= 'f')) ||
            ((u8_Digit >= 'A') && (u8_Digit <= 'F')))
        {
            u8_Convert_Digit = keyb_Convert_Ascii_To_Hex(u8_Digit);
        }
#endif
        pu8_Data[i + 3] = u8_Convert_Digit;
    }
    /*
            st_Data.pu8_Data[0] = FXS_EV_KEYPAD
            st_Data.pu8_Data[1] = FXs Number
            st_Data.pu8_Data[2] = Dialed Digit Length
            st_Data.pu8_Data[3] = here onwards dial Digits starts
    */
    pu8_Data[0] = FXS_EV_KEYPAD;
    pu8_Data[1] = u8_HsNo - 0x30; //FXS HS Number
    pu8_Data[2] = u8_Len - 0x30; //Dial Digit Length
    st_Data.pu8_Data = pu8_Data;
    st_Data.u16_DataLen = 3 + (u8_Len - 0x30); //Total length of IE

    printf("\n Packet Sent from HOST\n");
    for (i = 0; i < (u8_Len - 0x30); i++)
    {
        printf("\n pu8_Data[%x] = %x \n", i, pu8_Data[i]);
    }
    //print packet before sending
    return app_FXSEvent(&st_Data);
}

E_CMBS_RC keyb_FxsRegister(u16 u16_HSNo)
{
    ST_IE_DATA st_Data;
    u8 pu8_Data[10];
    pu8_Data[0] = FXS_EV_REG_FXS;
    pu8_Data[1] = (u8)u16_HSNo; //FXS HS Number
    st_Data.u16_DataLen = 2;
    st_Data.pu8_Data = pu8_Data;

    printf("\n HOST Sending FXS_EV_REG_FXS IE with CMBS_EV_DEE_FXS_EVENT for FXSNo= %x \n", u16_HSNo);

    return app_FXSEvent(&st_Data);
}

//      ========== keyb_ConferenceAutoCfm ===========
/*!
        \brief              set automatic accept for conference creation
        \param[in,out]      <none>
        \return             <none>

*/
void keyb_ConferenceAutoCfm(void)
{
    u8 u8_Answ;
    printf("\nEnter automatic confirm for conference request [Y/N]: ");
    u8_Answ = tcx_getch();
    if ( u8_Answ == 'y' || u8_Answ == 'Y' )
    {
        g_ConfAutoCfm = 1;
    }
    else
    {
        g_ConfAutoCfm = 0;
    }
}


//		========== keyb_MediaSwitch ===========
/*!
        \brief				Media channel start or stop
        \param[in,out]		bo_On    TRUE to start, FALSE to stop
        \return				<none>

*/
void keyb_MediaSwitch(int bo_On)
{
    u16 u16_CallId;
    u16 u16_Slot;

    u16_CallId = _keyb_CallIdInput();
    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {

        if ( bo_On )
        {
            u16_Slot = _keyb_SlotInput();
            appmedia_CallObjMediaStart(0, u16_CallId, u16_Slot, NULL);
        }
        else
        {
            appmedia_CallObjMediaStop(0, u16_CallId, NULL);
        }

    }
}

void keyb_MediaInernalCall(void)
{
    char ch_Channel;
    char ch_Context;
    char ch_Operation;

    printf("\nChannel ID? [0..3]:");
    ch_Channel = tcx_getch();

    if ( (ch_Channel >= '0') && (ch_Channel <= '3') )
    {
        ch_Channel -= '0';
    }
    else
    {
        printf("\nUnavailble Channel ID, Must be 0 to 3, Press any key to continue");
        tcx_getch();
        return;
    }

    printf("\nContext/Node ID? [0..1]:");
    ch_Context = tcx_getch();

    if ( (ch_Context >= '0') && (ch_Context <= '1') )
    {
        ch_Context -= '0';
    }
    else
    {
        printf("\nUnavailble Context/Node ID, Must be 0 or 1, Press any key to continue");
        tcx_getch();
        return;
    }


    printf("\nConnect or Disconnect? [1, Connect; 0, Disconnect]:");
    ch_Operation = tcx_getch();

    if ( (ch_Operation >= '0') && (ch_Operation <= '1') )
    {
        ch_Operation -= '0';
    }
    else
    {
        printf("\nError Command, Must be 0 or 1, Press any key to continue");
        tcx_getch();
        return;
    }

    appmedia_CallObjMediaInternalConnect(ch_Channel, ch_Context, ch_Operation);

}

void keyb_MediaOffer(void)
{
    u16 u16_CallId;
    char ch_Audio;

    u16_CallId = _keyb_CallIdInput();

    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {

        printf("\nSelect codec:\n");
        printf("\tw - WB Linear PCM16  (16 KHz, 16Bit / Sample)\n");
        printf("\tn - NB Linear PCM16  (8 KHz,  16Bit / Sample)\n");
        printf("\ta - NB G.711 A-law   (8 KHz,  8Bit  / Sample)\n");
        printf("\tu - NB G.711 u-law   (8 KHz,  8Bit  / Sample)\n");
        printf("\t8 - NB Linear PCM8   (8 KHz,  8Bit  / Sample)\n");
        printf("\tg - NB Linear PCM16 with G.711 A-law OTA\n");
        ch_Audio = tcx_getch();

        appmedia_CallObjMediaOffer(u16_CallId, ch_Audio);
    }
}

//		========== keyb_MediaAutoSwitch ===========
/*!
        \brief				Switch codec betwen G722 (WB) and G726 (NB) every 4 seconds
        \                   for testing slot switch failures
        \param[in,out]		<none>
        \return				<none>

*/
void keyb_MediaAutoSwitch(void)
{
    u16 u16_CallId;

    u16_CallId = _keyb_CallIdInput();

    printf("\nSwitch codec betwen G722 (WB) and G726 (NB) every 4 seconds until key pressed \n");

    while (!kbhit())
    {
        printf("\nSwitch codec to G726 \n");
        appmedia_CallObjMediaOffer(u16_CallId, 'n');
        SleepMs(4000);        // wait 4 seconds

        if ( !kbhit() )
        {
            printf("\nSwitch codec to G722 \n");
            appmedia_CallObjMediaOffer(u16_CallId, 'w');
            SleepMs(4000);        // wait 4 seconds
        }
    }
}

//		========== keyb_TonePlay ===========
/*!
        \brief				Play/Stop a tone on Media channel 
        \param[in,out]		psz_Value  enumeration string of CMBS tone
        \param[in,out]		bo_On    TRUE to start, FALSE to stop
        \return				<none>

*/
void keyb_TonePlay(char *psz_Value, int no_On)
{
    u16 u16_CallId;

    u16_CallId = _keyb_CallIdInput();
    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {        appmedia_CallObjTonePlay(psz_Value, g_u16_ToneVolume, no_On, u16_CallId, NULL, 0, 0);

    }
}

//
//		========== keyb_TonePlayDTMF ===========
/*!
		\brief				Play a DTMF tone on Media channel 
		\return				<none>

*/
void     keyb_TonePlayDTMF(void)
{
  u16    u16_CallId;
  int    Tone = 0;
  char ch_Tone[30] = { 0 };
  char ch_ToneDur[40];
  u32 u32_ToneDur =0;
  int input = 0;
  u16 u16_DTMFToneToHSMask=0;

  printf("\n DTMF Tone, select digit 0 .. 9 * # A B C D: ");

  input = tcx_getch();
  
  if((input <= '9' && input >= '0') 
  		|| input == '*' || input == '#' 
  		|| (input <= 'D' && input >= 'A') 
  		|| (input <= 'd' && input >= 'a'))
  {
  	  if(input <= '9' && input >= '0'){
	  		Tone = (input - 0x30) + CMBS_TONE_DTMF_0;
  	  }else if(input <= 'D' && input >= 'A'){
  	  		Tone = (input - 0x41) + CMBS_TONE_DTMF_A;
  	  }else if(input <= 'd' && input >= 'a'){
  	  		Tone = (input - 0x61) + CMBS_TONE_DTMF_A;
  	  }else if(input == '*'){
  	  		Tone = CMBS_TONE_DTMF_STAR;
  	  }else if(input == '#'){
  	  		Tone = CMBS_TONE_DTMF_HASH;
  	  }
  	    
      strncpy(ch_Tone, cmbs_dbg_GetToneName(Tone), (sizeof(ch_Tone) - 1));

      u16_CallId = _keyb_CallIdInput();
      
      printf("\n DTMF tone: %s  Call ID %d \n", ch_Tone, u16_CallId);
	  
      printf("\n Enter DTMF Tone duration in ms (For Default value 80ms press enter)\n");

	  ch_ToneDur[0] = tcx_getch();
	  if(ch_ToneDur[0] != '\r')
		  tcx_gets(ch_ToneDur+1, sizeof(ch_ToneDur)-1);

	  u32_ToneDur = atoi(ch_ToneDur);

      printf("\n Enter Handset Mask (HEX e.g 1F) for which DTMF tone to be played else enter 0\n");

	  tcx_gets(ch_ToneDur, sizeof(ch_ToneDur)-1);

	  u16_DTMFToneToHSMask = (u16)strtol(ch_ToneDur, NULL, 16);

	  appmedia_CallObjTonePlay(ch_Tone, 0, TRUE | g_u8_ToneType, u16_CallId, NULL, (u16)u32_ToneDur, u16_DTMFToneToHSMask);
  }
  else
  {     
      printf("\n wrong input, tone: %d \n", Tone);
  }
}
        



//
//		========== keyb_TonePlaySelect ===========
/*!
        \brief				Play/Stop a tone on Media channel 
        \param[in,out]		psz_Value  enumeration string of CMBS tone
        \param[in,out]		bo_On    TRUE to start, FALSE to stop
        \return				<none>

*/
void keyb_TonePlaySelect(void)
{
    u8 u8_Value;
    char ch_cli[10]; // to be changed, take calling handset, 'i' for internal call CLIP

    printf("\n Tones provided: \n");
    printf("0  : Tone OFF \n");
    printf("1  : Dial tone\n");
    printf("2  : Ring-back tone\n");
    printf("3  : Busy tone\n");
    printf("4  : Call waiting tone\n");
    printf("5  : Message waiting dial tone\n");
    printf("6  : Call Forward dial tone (Swiss only)\n");
    printf("6  : Confirm tone           (italy only)\n");
    printf("7  : Congestion tone        (Swiss only)\n");
    printf("w  : Warning tone           (Korea only)\n");
    printf("\n  Country selection: \n");
    printf("l  : default (0)\n");
    printf("m  : french  (1)\n");
    printf("n  : polish  (2)\n");
    printf("o  : swiss   (3)\n");
    printf("p  : italian (4)\n");
    printf("z  : Korean  (5)\n");
    printf("v  : toggle 'use TOG to audio node' (not IOM) actual 0x%2x \n", g_u8_ToneType);
    printf("k  : set TONE volume, actual 0x%4x \n", g_u16_ToneVolume);
    printf("\n OLD STYLE implementation for french and polish tones:\n");
    printf(" FRENCH tones: \n");
    printf("8  : Dial tone                 - French style \n");
    printf("9  : Message waiting dial tone - French style \n");
    printf("a  : Ring-back tone            - French style \n");
    printf("b  : Busy tone                 - French style \n");
    printf("c  : Call waiting tone         - French style \n");
    printf(" POLISH tones: \n");
    printf("d  : Dial tone                 - Polish style \n");
    printf("e  : Message waiting dial tone - Polish style \n");
    printf("f  : Ring-back tone            - Polish style \n");
    printf("g  : Busy tone                 - Polish style \n");
    printf("h  : Call waiting tone         - Polish style \n");
    printf(" OUTBAND tones (<<SIGNAL>> IE to PP): \n");
    printf("r  : Dial tone                 - outband \n");
    printf("s  : Ring-back tone            - outband \n");
    printf("t  : Busy tone                 - outband \n");
    printf("u  : Call waiting tone         - outband \n");
    printf("x  : tone off                  - outband \n");
    printf("\n  select Tone Style:  \n");

    switch (tcx_getch())
    {
    case '0':
        keyb_TonePlay(NULL, FALSE | g_u8_ToneType);
        break;
    case '1':
        keyb_TonePlay("CMBS_TONE_DIAL\0", TRUE | g_u8_ToneType);
        break;
    case '2':
        keyb_TonePlay("CMBS_TONE_RING_BACK\0", TRUE | g_u8_ToneType);
        break;
    case '3':
        keyb_TonePlay("CMBS_TONE_BUSY\0", TRUE | g_u8_ToneType);
        break;
    case '4':
        keyb_TonePlay("CMBS_TONE_CALL_WAITING\0", TRUE | g_u8_ToneType);
        break;
    case '5':
        keyb_TonePlay("CMBS_TONE_MWI\0", TRUE | g_u8_ToneType);
        break;
    case '6':
        keyb_TonePlay("CMBS_TONE_DIAL_CALL_FORWARD\0", TRUE | g_u8_ToneType);
        break;
    case '7':
        keyb_TonePlay("CMBS_TONE_MWI_OR_CONGESTION\0", TRUE | g_u8_ToneType);
        break;
    case 'w':
        keyb_TonePlay("CMBS_TONE_OFF_HOOK_WARN\0", TRUE | g_u8_ToneType);
        break;
    case 'k':
        // change TOG volume for inband tone
        printf("\n0 - Disable (Don't send\n");
        printf("1 - -7  DB\n");
        printf("2 - -11 DB\n");
        printf("3 - -14 DB\n");
        printf("4 - -17 DB\n");
        printf("5 - -21 DB\n");
        printf("6 - -24 DB\n");
        printf("7 - -27 DB\n");
        printf("8 - -30 DB\n");
        printf("f - flexible setting ");

        switch (tcx_getch())
        {
            /* From audio driver */
#define    DR12_m7_DB    0x1C96
#define    DR12_m9_DB    0x16B5
#define    DR12_m11_DB   0x1209
#define    DR12_m12_DB   0x1000
#define    DR12_m14_DB   0x0CC5
#define    DR12_m15_DB   0x0B61
#define    DR12_m16_DB   0x0A24
#define    DR12_m17_DB   0x090A
#define    DR12_m18_DB   0x0800
#define    DR12_m19_DB   0x07E2
#define    DR12_m21_DB   0x05B4
#define    DR12_m23_DB   0x0488
#define    DR12_m24_DB   0x0400
#define    DR12_m27_DB   0x02DC
#define    DR12_m30_DB   0x0200

        case '0':
            g_u16_ToneVolume = 0;
            break;

        case '1':
            g_u16_ToneVolume = DR12_m7_DB;
            break;

        case '2':
            g_u16_ToneVolume = DR12_m11_DB;
            break;

        case '3':
            g_u16_ToneVolume = DR12_m14_DB;
            break;

        case '4':
            g_u16_ToneVolume = DR12_m17_DB;
            break;

        case '5':
            g_u16_ToneVolume = DR12_m21_DB;
            break;

        case '6':
            g_u16_ToneVolume = DR12_m24_DB;
            break;

        case '7':
            g_u16_ToneVolume = DR12_m27_DB;
            break;

        case '8':
            g_u16_ToneVolume = DR12_m30_DB;
            break;

        case 'f':
            // flexible setting
            printf("\n please type a 4-digit volume (HEX): ");
            tcx_gets(ch_cli, sizeof(ch_cli));
            g_u16_ToneVolume = ((u16)app_ASC2HEX(&ch_cli[0]) << 8) | app_ASC2HEX(&ch_cli[2]);
            break;
        }

        break;
    case 'l':
        u8_Value = CMBS_INBAND_DEFAULT;
        app_SrvParamSet(CMBS_PARAM_INBAND_COUNTRY, &u8_Value, sizeof(u8_Value), 1);
        break;
    case 'm':
        u8_Value = CMBS_INBAND_FRENCH;
        app_SrvParamSet(CMBS_PARAM_INBAND_COUNTRY, &u8_Value, sizeof(u8_Value), 1);
        break;
    case 'n':
        u8_Value = CMBS_INBAND_POLISH;
        app_SrvParamSet(CMBS_PARAM_INBAND_COUNTRY, &u8_Value, sizeof(u8_Value), 1);
        break;
    case 'o':
        u8_Value = CMBS_INBAND_SWISS;
        app_SrvParamSet(CMBS_PARAM_INBAND_COUNTRY, &u8_Value, sizeof(u8_Value), 1);
        break;

    case 'p':
        u8_Value = CMBS_INBAND_ITALY;
        app_SrvParamSet(CMBS_PARAM_INBAND_COUNTRY, &u8_Value, sizeof(u8_Value), 1);
        break;

    case 'z':
        u8_Value = CMBS_INBAND_KOREA;
        app_SrvParamSet(CMBS_PARAM_INBAND_COUNTRY, &u8_Value, sizeof(u8_Value), 1);
        break;

    case 'v':
        g_u8_ToneType ^= 0x80;
        printf(" Tonetype = 0x%2x\n", g_u8_ToneType);
        break;

    case '8':
        keyb_TonePlay("CMBS_TONE_DIAL_FT_FR\0", TRUE);
        break;
    case '9':
        keyb_TonePlay("CMBS_TONE_MWI_FT_FR\0", TRUE);
        break;
    case 'a':
        keyb_TonePlay("CMBS_TONE_RING_BACK_FT_FR\0", TRUE);
        break;
    case 'b':
        keyb_TonePlay("CMBS_TONE_BUSY_FT_FR\0", TRUE);
        break;
    case 'c':
        keyb_TonePlay("CMBS_TONE_CALL_WAITING_FT_FR\0", TRUE);
        break;
    case 'd':
        keyb_TonePlay("CMBS_TONE_DIAL_FT_PL\0", TRUE);
        break;
    case 'e':
        keyb_TonePlay("CMBS_TONE_MWI_FT_PL\0", TRUE);
        break;
    case 'f':
        keyb_TonePlay("CMBS_TONE_RING_BACK_FT_PL\0", TRUE);
        break;
    case 'g':
        keyb_TonePlay("CMBS_TONE_BUSY_FT_PL\0", TRUE);
        break;
    case 'h':
        keyb_TonePlay("CMBS_TONE_CALL_WAITING_FT_PL\0", TRUE);
        break;

    case 'r':
        keyb_TonePlay("CMBS_TONE_DIAL_OUTBAND\0", TRUE);
        break;
    case 's':
        keyb_TonePlay("CMBS_TONE_RING_BACK_OUTBAND\0", TRUE);
        break;
    case 't':
        keyb_TonePlay("CMBS_TONE_BUSY_OUTBAND\0", TRUE);
        break;
    case 'u':
        keyb_TonePlay("CMBS_TONE_CALL_WAITING_OUTBAND\0", TRUE);
        break;
    case 'x':
        keyb_TonePlay("CMBS_TONE_OFF_OUTBAND\0", TRUE);
        break;
    }
}


//  	========== keyb_CallWaiting ===========
/*!
        \brief				start/stop Call waiting from host to target.
        \param[in,out]		<none>
        \return				<none>

      \note             Host is in responsibility of stop call waiting and
                        provide the old CLI to CMBS target.
*/
void keyb_CallWaiting(void)
{
    ST_APPCALL_PROPERTIES st_Properties;

    u16 u16_CallId;
    int n_On = FALSE;
    char ch_cli[30];

    u16_CallId = _keyb_CallIdInput();
    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {

        printf("Call Waiting On {y/n/f = Yes/No/French tone}:\n");
        n_On = tcx_getch();

        printf("Caller Party :");
        tcx_gets(ch_cli, sizeof(ch_cli));
        if ( n_On == 'y' )
        {
            appmedia_CallObjTonePlay("CMBS_TONE_CALL_WAITING\0", g_u16_ToneVolume, TRUE, u16_CallId, NULL, 0, 0);
        }
        else if ( n_On == 'f' )
        {
            appmedia_CallObjTonePlay("CMBS_TONE_CALL_WAITING_FT_FR\0", g_u16_ToneVolume, TRUE, u16_CallId, NULL, 0, 0);
        }
        else
        {
            appmedia_CallObjTonePlay(NULL, g_u16_ToneVolume, FALSE, u16_CallId, NULL, 0, 0);
        }
        st_Properties.e_IE = CMBS_IE_CALLINFO;
        st_Properties.psz_Value = ch_cli;

        appcall_DisplayCall(&st_Properties, 1, u16_CallId, NULL);

    }
}

//  	========== keyb_CallDisplayString ===========
/*!
        \brief				start/stop Call multiple display string from host to target.
        \param[in,out]		<none>
        \return				<none>

      \note             Host is in responsibility of stop call waiting and
                        provide the old CLI to CMBS target.
*/
void keyb_CallDisplayString(void)
{
    ST_APPCALL_PROPERTIES st_Properties;

    u16 u16_CallId;
    char ch_cli[60];

    u16_CallId = _keyb_CallIdInput();
    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {

        printf("\nDisplay string  : ");
        tcx_gets(ch_cli + 1, sizeof(ch_cli) - 1);

        ch_cli[59] = 0;
        st_Properties.e_IE = CMBS_IE_DISPLAY_STRING;
        st_Properties.psz_Value = ch_cli;

        appcall_DisplayString(&st_Properties, 1, u16_CallId, NULL);

    }
}
//		========== keyb_CallHold  ===========
/*!
        \brief			signal to CMBS Target, that the call is hold or resumed.
                     the CMBS Target enable/disable the audio stream.
        \param[in]	   <none>
        \return			<none>
*/

void keyb_CallHold(void)
{
    u16 u16_CallId;
    int n_On;

    u16_CallId = _keyb_CallIdInput();

    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {
        printf("\nCall Hold? [y/n] : ");
        n_On = tcx_getch();
        if ( n_On == 'y' )
        {
            appcall_HoldCall(u16_CallId, NULL);
        }
        else
        {
            appcall_ResumeCall(u16_CallId, NULL);
        }
    }
}


//		========== keyb_SendCallStatusUpdateToPP  ===========
/*!
        \brief	        Send an updated call status to the PP
        \param[in]	    <none>
        \return		    <none>
*/
void keyb_SendCallStatusUpdateToPP(void)
{
    u16 u16_CallId;
    ST_APPCALL_PROPERTIES st_Properties[2];

    st_Properties[0].e_IE = CMBS_IE_CALLPROGRESS;
    st_Properties[1].e_IE = 0;  // pre-set with invalid value


    u16_CallId = _keyb_CallIdInput();

    if ( u16_CallId == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Caller ID - Retry with the correct ID \n");
    }
    else
    {

        printf("\n Call Status:\n");
        printf("================\n");
        printf("DISCONNECTING - no reason:\t0\n");
        printf("IDLE:\t1\n");
        printf("HOLD:\t2\n");
        printf("CALL-CONNECTED:\t3\n");
        printf("DISCONNECTING - Reason Line in Use:\t4\n");
        printf("DISCONNECTING - Reason User Busy:\t5\n");
        printf("DISCONNECTING - Reason Number not available:\t6\n");
        printf("UNDEF         - Reason Control code not supported:\t7\n");
        printf("UNDEF         - Reason Control code failed:\t8\n");
        printf("CSCMBS_CALL_PROGR_CHGDIROG        :\t9\n");
        printf("CSCMBS_CALL_PROGR_CHGDIRIC        :\tA\n");
        printf("CMBS_CALL_PROGR_SCREEN_SETUP        :\tB\n");
        printf("CMBS_CALL_PROGR_SCREEN_CONNECT        :\tC\n");
        printf("CMBS_CALL_PROGR_REMOTE_HOLD        :\tD\n");
        printf("CMBS_CALL_PROGR_REMOTE_CONNECT        :\tE\n");

        switch (tcx_getch())
        {
        case '0':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_DISCONNECTING";
            break;

        case '4':
            {
                static char s_reason[5] = { 0 };
                st_Properties[0].psz_Value = "CMBS_CALL_PROGR_DISCONNECTING";

                sprintf(s_reason, "%d", CMBS_REL_REASON_LINE_IN_USE);
                st_Properties[1].psz_Value = s_reason;
                st_Properties[1].e_IE = CMBS_IE_CALLRELEASE_REASON;
                break;
            }

        case '5':
            {
                static char s_reason[5] = { 0 };
                st_Properties[0].psz_Value = "CMBS_CALL_PROGR_DISCONNECTING";

                sprintf(s_reason, "%d", CMBS_REL_REASON_USER_BUSY);
                st_Properties[1].psz_Value = s_reason;
                st_Properties[1].e_IE = CMBS_IE_CALLRELEASE_REASON;
                break;
            }

        case '6':
            {
                static char s_reason[5] = { 0 };
                st_Properties[0].psz_Value = "CMBS_CALL_PROGR_DISCONNECTING";

                sprintf(s_reason, "%d", CMBS_REL_REASON_NUMBER_NOT_AVL);
                st_Properties[1].psz_Value = s_reason;
                st_Properties[1].e_IE = CMBS_IE_CALLRELEASE_REASON;
                break;
            }

        case '7':
            {
                static char s_reason[5] = { 0 };
                st_Properties[0].psz_Value = "CMBS_CALL_PROGR_UNDEF";

                sprintf(s_reason, "%d", CMBS_REL_REASON_CTRL_CODE_NOT_SUP);
                st_Properties[1].psz_Value = s_reason;
                st_Properties[1].e_IE = CMBS_IE_CALLRELEASE_REASON;
                break;
            }

        case '8':
            {
                static char s_reason[5] = { 0 };
                st_Properties[0].psz_Value = "CMBS_CALL_PROGR_UNDEF";

                sprintf(s_reason, "%d", CMBS_REL_REASON_CTRL_CODE_FAILED);
                st_Properties[1].psz_Value = s_reason;
                st_Properties[1].e_IE = CMBS_IE_CALLRELEASE_REASON;
                break;
            }
        case '9':
            {
                st_Properties[0].psz_Value = "CMBS_CALL_PROGR_CHGDIROG";
                break;
            }
        case 'A':
            {
                st_Properties[0].psz_Value = "CMBS_CALL_PROGR_CHGDIRIC";
                break;
            }
        case '1':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_IDLE";
            break;

        case '2':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_HOLD";
            break;

        case '3':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_CONNECT";
            break;

        case 'B':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_SCREEN_SETUP";
            break;

        case 'C':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_SCREEN_CONNECT";
            break;

        case 'D':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_REMOTE_HOLD";
            break;

        case 'E':
            st_Properties[0].psz_Value = "CMBS_CALL_PROGR_REMOTE_CONNECT";
            break;

        default:
            printf("\nERROR, ignoring...\n");
            return;
        }

        if ( st_Properties[1].e_IE )
        {
            appcall_ProgressCall(st_Properties, 2, u16_CallId, NULL);
        }
        else
        {
            appcall_ProgressCall(st_Properties, 1, u16_CallId, NULL);
        }
    }
}

void key_call_test(u8 enableflag)
{
    u8 ucDisableFXSCall = enableflag;    /*!< Enable or Disable route internal * call to host. 0x0: disabled; 0x1: enabled(default) */

    app_ProductionParamSet(CMBS_PARAM_INT_START_CALL_TO_HOST, &ucDisableFXSCall, sizeof(u8), 1);
    printf("Sending CMBS_PARAM_INT_START_CALL_TO_HOST\r\n ");
}


//		========== keyb_CallStressTests  ===========
/*!
        \brief	        Call control stress tests
        \param[in]	    <none>
        \return		    <none>
*/
void keyb_CallStressTests(void)
{
    tcx_appClearScreen();
    printf("Call control stress tests\n");
    printf("==========================\n");
    printf("m\tMissed call stress test\n");
    printf("q\tback to previous menu\n");

    while (1)
    {
        switch (tcx_getch())
        {
        case 'm':
            {
                // loop parameters
                u32 u32_NumOfCalls, u32_DelayBetweenCallsMs, u32_Counter;

                // call ID
                u16 u16_CallId;

                // call parameters
                ST_APPCALL_PROPERTIES st_Properties[6], st_RelProperties;
                int n_Prop = 6;
                char ch_cli[] = "1234567890000";
                char ch_cni[] = "MissedCallsTests";
                char ch_cld[] = ALL_HS_STRING;
                char ch_clineid[] = "0";
                char s_codecs[] = "5,6"; //WB and NB
                char ch_melody[] = "3";

                st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
                st_Properties[0].psz_Value = ch_cli;
                st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
                st_Properties[1].psz_Value = ch_cld;
                st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
                st_Properties[2].psz_Value = s_codecs;
                st_Properties[3].e_IE = CMBS_IE_LINE_ID;
                st_Properties[3].psz_Value = ch_clineid;
                st_Properties[4].e_IE = CMBS_IE_CALLERNAME;
                st_Properties[4].psz_Value = ch_cni;
                st_Properties[5].e_IE = CMBS_IE_MELODY;
                st_Properties[5].psz_Value = ch_melody;

                // release parameters
                st_RelProperties.e_IE = CMBS_IE_CALLRELEASE_REASON;
                st_RelProperties.psz_Value = "0";

                printf("\n\nMissed calls:\nEnter number of calls... ");
                tcx_scanf("%d", &u32_NumOfCalls);
                printf("\nEnter delay between calls (seconds)... ");
                tcx_scanf("%d", &u32_DelayBetweenCallsMs);

                // stress test loop
                for (u32_Counter = 0; u32_Counter < u32_NumOfCalls; ++u32_Counter)
                {
                    tcx_appClearScreen();

                    printf("\n\nPerforming call %d out of %d...\n\n", u32_Counter + 1, u32_NumOfCalls);

                    // perform IC call
                    u16_CallId = appcall_EstablishCall(st_Properties, n_Prop);

                    // wait
                    SleepMs(u32_DelayBetweenCallsMs * 1000);

                    // release the call
                    appcall_ReleaseCall(&st_RelProperties, 1, u16_CallId, NULL);
                }
            }
            break;

        case 'q':
        default:
            return;
        }
    }
}

//		========== keyb_MergeCalls  ===========
/*!
        \brief	        Call control merge calls
        \param[in]	    <none>
        \return		    <none>
*/
void keyb_MergeCalls(void)
{
    u16 u16_CallId1, u16_CallId2;

    printf("\n======Call Objects===============================\n\n");
    appcall_InfoPrint();
    printf("\n======Merge Calls===============================\n");
    printf("\n Provide Call Id1 and Call Id2\n");
    u16_CallId1 = _keyb_CallIdInput();
    if ( u16_CallId1 == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Call ID\n\n");
        return;
    }
    u16_CallId2 = _keyb_CallIdInput();
    if ( u16_CallId2 == KEYB_CALL_INVALID_CALLID )
    {
        printf(" \n Invalid Call ID\n\n");
        return;
    }

    appcall_MergeCalls(u16_CallId1, u16_CallId2, 0);
}

//		========== keyb_MergeCalls  ===========
/*!
        \brief	        Call control merge calls
        \param[in]	    <none>
        \return		    <none>
*/
void keyb_MediaStart(void)
{
    u16 u16_CallId;

    printf("\n======Media Start===============================\n");
    u16_CallId = _keyb_CallIdInput();
    appmedia_CallObjMediaStop(0, u16_CallId, NULL);

    SleepMs(200);        // wait 0.2 seconds

    appmedia_CallObjMediaStart(0, u16_CallId, 0xFF, NULL);
}


//		========== keyb_EmergencyCallRelease  ===========
/*!
        \brief	        Emergency Timer Expired. Call is released on Host side 
        \param[in]	    <none>
        \return		    <none>
*/
void keyb_EmergencyCallRelease(void)
{
    u16 u16_CallId;

    // get call ID
    u16_CallId = _keyb_CallIdInput();

    appcall_EmergencyCallRelease(u16_CallId);

}

//		========== keyb_EmergencyCallReleaseByCallInstance  ===========
/*!
        \brief	        Emergency Timer Expired. Call is released on Host side 
        \param[in]	    <none>
        \return		    <none>
*/
void keyb_EmergencyCallReleaseByCallInstance(void)
{
    u16 u16_CallInstance;

    // get call ID
    u16_CallInstance = _keyb_CallInstanceInput();

    appcall_EmergencyCallReleaseByCallInstance(u16_CallInstance);

}

u16 u16_keyb_han_temp;

void keyb_SendVoiceCallReqtoDevice ( void )
{
	u16 u16_DeviceId = 0;
	u8 u8_UnitId = 0;
	u8 u8_TempBuffer[100];
	ST_HAN_VOICE_CALL_REQ_PARAMS st_CallReqParams;
	st_CallReqParams.u32_FieldMask = 0;
	
		
	printf("\nEnter Device Id (dec):");
	tcx_scanf("%hu",&u16_DeviceId);

	printf("\nEnter Unit Id (dec):");
	tcx_scanf("%hhu",&u16_keyb_han_temp);
	u8_UnitId = (u8) u16_keyb_han_temp;

	printf("\nEnter codec:\n");
	printf("-------------\n");
	printf("n => NB Basic\n");
	printf("w => WB G722\n");
	printf("d => default\n");
	printf("e => empty\n");
	
	switch (tcx_getch())
		{
			case 'N':
			case 'n':
			case 'D':
			case 'd':
				{
					st_CallReqParams.u8_PreferredCodec = CMBS_HAN_ULE_CALL_CODEC_NB_BASIC;

					st_CallReqParams.u32_FieldMask |= CMBS_HAN_PREFFERED_CODEC_MASK;
				}
				break;
				
			case 'W':
			case 'w':
				{
					st_CallReqParams.u8_PreferredCodec = CMBS_HAN_ULE_CALL_CODEC_WB_G722;

					st_CallReqParams.u32_FieldMask |= CMBS_HAN_PREFFERED_CODEC_MASK;
				}
				break;

			default:
				break;	
		}

	printf("\nEnter other party type:\n");
	printf("----------------------\n");
	printf("h => HS\n");
	printf("d => Device\n");
	printf("n => external number\n");
	printf("e => empty\n");
	
		switch (tcx_getch())
		{
			case 'H':
			case 'h':
				{
					st_CallReqParams.u8_OtherPartyType = (u8) CMBS_HAN_OTHER_PARTY_TYPE_HS;

					st_CallReqParams.u32_FieldMask |= CMBS_HAN_OTHER_PARTY_TYPE_MASK;
				}
				break;
			case 'D':
			case 'd':
				{
					st_CallReqParams.u8_OtherPartyType = (u8) CMBS_HAN_OTHER_PARTY_TYPE_DEVICE;

					st_CallReqParams.u32_FieldMask |= CMBS_HAN_OTHER_PARTY_TYPE_MASK;
				}
				break;
				
			case 'N':
			case 'n':
				{
					st_CallReqParams.u8_OtherPartyType = (u8) CMBS_HAN_OTHER_PARTY_TYPE_NUMBER;

					st_CallReqParams.u32_FieldMask |= CMBS_HAN_OTHER_PARTY_TYPE_MASK;
				}
				break;

			default:
				break;	
		}

	memset(&st_CallReqParams.pu8_Digits[0], 0, sizeof(st_CallReqParams.pu8_Digits));
	
	printf("\nEnter digits? (Y/N/D for default)\n");
	switch (tcx_getch())
	{
		case 'D':
		case 'd':
			{
				st_CallReqParams.pu8_Digits[0] = 10;		//length
				st_CallReqParams.pu8_Digits[1] = '0';
				st_CallReqParams.pu8_Digits[2] = '5';
				st_CallReqParams.pu8_Digits[3] = '4';
				st_CallReqParams.pu8_Digits[4] = '5';
				st_CallReqParams.pu8_Digits[5] = '8';
				st_CallReqParams.pu8_Digits[6] = '4';
				st_CallReqParams.pu8_Digits[7] = '7';
				st_CallReqParams.pu8_Digits[8] = '0';
				st_CallReqParams.pu8_Digits[9] = '6';
				st_CallReqParams.pu8_Digits[10] = '0';

				st_CallReqParams.u32_FieldMask |= CMBS_HAN_DIGITS_MASK;
			}
			break;

		case 'Y':
		case 'y':
			{
				printf(">> ");
				tcx_gets((char *)u8_TempBuffer, sizeof(u8_TempBuffer));

				u8_TempBuffer[sizeof(st_CallReqParams.pu8_Digits)-1] = 0;
				
				st_CallReqParams.pu8_Digits[0] = strlen((char *)u8_TempBuffer);
				strcat((char *)st_CallReqParams.pu8_Digits, (char *)u8_TempBuffer);
				st_CallReqParams.u32_FieldMask |= CMBS_HAN_DIGITS_MASK;
			}
			break;
			
		default:
			break;
	}

	memset(&st_CallReqParams.pu8_OtherPartyName[0], 0, sizeof(st_CallReqParams.pu8_OtherPartyName));
	
	printf("\nEnter other party name? (Y/N/D for default)\n");
	switch (tcx_getch())
	{
		case 'D':
		case 'd':
			{
				st_CallReqParams.pu8_OtherPartyName[0] = 5;		//length
				st_CallReqParams.pu8_OtherPartyName[1] = 'M';
				st_CallReqParams.pu8_OtherPartyName[2] = 'o';
				st_CallReqParams.pu8_OtherPartyName[3] = 'r';
				st_CallReqParams.pu8_OtherPartyName[4] = 'i';
				st_CallReqParams.pu8_OtherPartyName[5] = 'a';

				st_CallReqParams.u32_FieldMask |= CMBS_HAN_OTHER_PARTY_NAME_MASK;
			}
			break;

		case 'Y':
		case 'y':
			{
				printf(">> ");
				tcx_gets((char *)u8_TempBuffer, sizeof(u8_TempBuffer));
				u8_TempBuffer[sizeof(st_CallReqParams.pu8_OtherPartyName)-1] = 0;
								
				st_CallReqParams.pu8_OtherPartyName[0] = strlen((char *)u8_TempBuffer);
				strcat((char *)st_CallReqParams.pu8_OtherPartyName, (char *)u8_TempBuffer);
				st_CallReqParams.u32_FieldMask |= CMBS_HAN_OTHER_PARTY_NAME_MASK;
			}
			break;
			
		default:
			break;
	}

	memset(&st_CallReqParams.pu8_OtherPartyId[0], 0, sizeof(st_CallReqParams.pu8_OtherPartyId));
		
	printf("\nEnter other party id? (Y/N/D for default)\n");
	switch (tcx_getch())
	{
		case 'D':
		case 'd':
			{
				st_CallReqParams.pu8_OtherPartyId[0] = 2;		//length
				st_CallReqParams.pu8_OtherPartyId[1] = '2';
				st_CallReqParams.pu8_OtherPartyId[2] = '8';

				st_CallReqParams.u32_FieldMask |= CMBS_HAN_OTHER_PARTY_ID_MASK;
			}
			break;

		case 'Y':
		case 'y':
			{
				printf(">> ");
				tcx_gets((char *)u8_TempBuffer, sizeof(u8_TempBuffer));
				u8_TempBuffer[sizeof(st_CallReqParams.pu8_OtherPartyId)-1] = 0;
								
				st_CallReqParams.pu8_OtherPartyId[0] = strlen((char *)u8_TempBuffer);
				strcat((char *)st_CallReqParams.pu8_OtherPartyId, (char *)u8_TempBuffer);
				st_CallReqParams.u32_FieldMask |= CMBS_HAN_OTHER_PARTY_ID_MASK;
			}
			break;
			
		default:
			break;
	}
	
	app_DsrHanVoiceCallReq(u16_DeviceId, u8_UnitId, &st_CallReqParams);
}

//		========== keyb_CallRefresh  ===========
/*!
        \brief			signal to CMBS Target, that read call info table from target .
        \param[in]	   <none>
        \return			<none>
*/

void keyb_CallRefresh(void)
{
	appcall_RefreshCallInfo();
}

void keyb_GetHsCallStatus()
{
	u32 u32_HsNo1,u8_HsNum;
	ST_IE_CALL_STATE Lst_HsCallState[2];
	memset(&Lst_HsCallState[0], 0, sizeof(ST_IE_CALL_STATE));
	memset(&Lst_HsCallState[1], 0, sizeof(ST_IE_CALL_STATE));
	printf ("\nEnter the HS Number 1..5 for which Call status to be checked\n");
	tcx_scanf("%d", &u32_HsNo1);
    u8_HsNum = (u8)u32_HsNo1;
	appcall_GetHsStatus(u8_HsNum, Lst_HsCallState);
     		printf("\n\n<<<<<<<<<$$$ Printing  DATA-0 For HsNo = %x $$$$>>>\n\n", u8_HsNum);
    		printf("\nLst_HsCallState[u8_HsNum].e_CallStatus = %x ", Lst_HsCallState[0].e_CallStatus);
			printf("\nLst_HsCallState[u8_HsNum].e_CallType = %x ",Lst_HsCallState[0].e_CallType);
			printf("\nLst_HsCallState[u8_HsNum].u16_HandsetsMask = %x ",Lst_HsCallState[0].u16_HandsetsMask);
			printf("\nLst_HsCallState[u8_HsNum].u8_ActCallID = %x ",Lst_HsCallState[0].u8_ActCallID);
			printf("\nLst_HsCallState[u8_HsNum].u8_LinesMask = %x ",Lst_HsCallState[0].u8_LinesMask);
			
			printf("\n\n<<<<<<<<<$$$ Printing DATA-1 For  HsNo = %x $$$$>>>\n\n", u8_HsNum);
			printf("\nLst_HsCallState[u8_HsNum].e_CallStatus = %x ", Lst_HsCallState[1].e_CallStatus);
			printf("\nLst_HsCallState[u8_HsNum].e_CallType = %x ", Lst_HsCallState[1].e_CallType);
			printf("\nLst_HsCallState[u8_HsNum].u16_HandsetsMask = %x ", Lst_HsCallState[1].u16_HandsetsMask);
			printf("\nLst_HsCallState[u8_HsNum].u8_ActCallID = %x ", Lst_HsCallState[1].u8_ActCallID);
			printf("\nLst_HsCallState[u8_HsNum].u8_LinesMask = %x ", Lst_HsCallState[1].u8_LinesMask);
}
//		========== keyb_ConfigTDMSlots  ===========
/*!
        \brief	        TDM Slot configuration
        \param[in]	    <none>
        \return		    <none>
*/
void keyb_ConfigTDMSlots(void)
{
	u32 u32_TDMSlots;

	printf ("\n======TDM Slot configuration========================\n");
	printf ("\nEnter the slot scheme in HEX format e.g. f0 for 0xf0\n");

	tcx_scanf("%x", &u32_TDMSlots);

	app_SrvSendTDMSlotConfiguration(u32_TDMSlots);
}


//		========== keyb_CallLoop ===========
/*!
        \brief				call management loop to control the call activities
        \param[in,out]		<none>
        \return				<none>

*/

void keyb_CallLoop(void)
{
    int n_Keep = TRUE;


    static u8 call2hostenable = 0;

    while (n_Keep)
    {
        //      tcx_appClearScreen();

        printf("\n======Call Objects===============================\n\n");
        appcall_InfoPrint();
        printf("\n======Call Control===============================\n\n");
        printf("l => automat on             L => automat off\n");
        printf("e => establish call         r => release call\n");
        printf("c => change codec           h => hold/resume\n");
        printf("w => call waiting           k => ringing\n");
        printf("a => answer call            f => early connect\n");
        printf("g => setup-ack              t => automatic confirm for hold/resume...\n");
        printf("G => call-proceeding        C => Change caller ID of an active call\n");
        printf("u => transfer auto cfm...   o => conference auto cfm...\n");
        printf("x => automatic codec switch G726/G722 every 10 seconds \n");
        printf("X => Move to FAX call 		z => Splash ring \n");
        printf("< => Move to FAX call and direction as TX \n");
        printf("> => Move to FAX call and direction as RX \n");
        printf("b => Send Call Status update to PP...\n");
        printf("v => Send called party name (CNIP) during a call.\n");
        printf("V => Send calling party name & Number of IC call during a call.\n");
        printf("s => Stress tests ...\n");
        printf("S => send display string...\n");
        printf("y => Merge calls ...\n");
        printf("M => Start Media for ext call\n");
        printf("j => call-proceeding-emergency-call 	\n");
        printf("N => Emergency Call Release \n");
		printf("# => Send Make a Call FUN req to Device \n");
		printf("O => Get all the active calls from Target \n");
        printf("\n======Media===============================\n\n");
        printf("1 => Dial Tone      2 => Ring Tone\n");
        printf("3 => Busy Tone      0 => Off Tone \n");
        printf("6 => Other tones by further selection \n");
        printf("m => Media on       n => Media off\n");
        printf("D => DTMF tone select\n");       
        printf("p => Media Loopback\n");
        printf("d => early media on/off\n");
        printf("T => TDM Slot Configuration\n");
        printf("4 => FXS Bypass->Normal  5 => FXS Normal->Bypass \n");
        printf("------------------------------------------\n");
        printf("Set host preferred CODEC \n");
        printf("7 => use received codec \n");
        printf("8 => WB \n");
        printf("9 => NB \n");
        printf("------------------------------------------\n\n");
        printf("i => call/line infos \n");
        printf("q => return to interface \n");
		printf("---------Getting Current HS Status----------\n\n");
		printf("R => Get HS Call Status \n");

        switch (tcx_getch())
        {
        case ' ':
            tcx_appClearScreen();
            break;

        case 'l':
            appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);
            break;
        case 'L':
            appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_OFF);
            break;

        case 'e':
            // establish
            keyb_CallEstablish();
            break;

        case 'z':
            // Splash ring Call
            keyb_SplashRing_GetDuration();
            break;

        case 'r':
            // release call
            keyb_CallRelease();
            break;
        case 'c':
            // change codec
            keyb_MediaOffer();
            break;
        case 'x':
            // change codec between G726 and G722 every 10 seconds
            keyb_MediaAutoSwitch();
            break;
        case 'h':
            // Call Hold/Resume
            keyb_CallHold();
            break;
        case 'w':
            // call waiting
            keyb_CallWaiting();
            break;
        case 'k':
            // ringback-tone
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_RINGING\0");
            break;
        case 'a':
            // answer call
            keyb_CallAnswer();
            break;
        case 'f':
            // early media
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_INBAND\0");
            break;
        case 'g':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_SETUP_ACK\0");
            break;

        case 'G':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_PROCEEDING\0");
            break;

        case 'j':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_PROCEED_EMERGENCY\0");
            break;

        case 'X':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_FAX\0");
            break;
        case '1':
            keyb_TonePlay("CMBS_TONE_DIAL\0", TRUE);
            break;
        case '2':
            keyb_TonePlay("CMBS_TONE_RING_BACK\0", TRUE);
            break;
        case '3':
            keyb_TonePlay("CMBS_TONE_BUSY\0", TRUE);
            break;
        case '4':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_FXS_BYPASS_TO_NORMAL\0");
            break;
        case '5':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_FXS_NORMAL_TO_BYPASS\0");
            break;
        case '<':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_FAX_TX\0");
            break;
        case '>':
            keyb_CallProgress((char *)"CMBS_CALL_PROGR_FAX_RX\0");
            break;
        case '6':
            keyb_TonePlaySelect();
            break;
        case 'm':
            keyb_MediaSwitch(TRUE);
            break;
        case 'n':
            keyb_MediaSwitch(FALSE);
            break;
        case 'p':
            keyb_MediaInernalCall();
            break;
        case '0':
            keyb_TonePlay(NULL, FALSE | g_u8_ToneType);
            break;
        case 'i':
            // call/line info
            //tcx_appClearScreen();
            appcall_InfoPrint();
            printf("Press Any Key\n ");
            tcx_getch();
            break;
            
		case 'D':
            keyb_TonePlayDTMF();
			break;

        case 't':
            keyb_HoldResumeCfm();
            break;

        case 'd':
            keyb_EarlyMediaOnOffCfm();
            break;

        case '7':
            keyb_SetHostCodec(E_APPCALL_PREFERRED_CODEC_RECEIVED_CODEC);
            break;

        case '8':
            keyb_SetHostCodec(E_APPCALL_PREFERRED_CODEC_WB);
            break;

        case '9':
            keyb_SetHostCodec(E_APPCALL_PREFERRED_CODEC_NB);
            break;

        case 'u':
            keyb_TransferAutoCfm();
            break;

        case 'v':
            keyb_CallInbandCNIP();
            break;

        case 'V':
            keyb_CallerNameNumber();
            break;

        case 'o':
            keyb_ConferenceAutoCfm();
            break;

        case 'C':
            keyb_CallInbandInfo();
            break;

        case 'b':
            keyb_SendCallStatusUpdateToPP();
            break;

        case 's':
            keyb_CallStressTests();
            break;

        case 'S':
            keyb_CallDisplayString();
            break;


        case 'y':
            keyb_MergeCalls();
            break;

        case 'M':
            keyb_MediaStart();
            break;

        case 'N':
            keyb_EmergencyCallReleaseByCallInstance();
            break;

		case '#':
			keyb_SendVoiceCallReqtoDevice();
			break;

		case 'O':
			keyb_CallRefresh();
			break;			
			
        case 'q':
            n_Keep = FALSE;
            break;
		case 'R':
     		keyb_GetHsCallStatus();
            break;

        case 'Z':
            //  keyb_MediaStart();
            call2hostenable = !call2hostenable;
            key_call_test(call2hostenable);

            break;

        case 'T':
            keyb_ConfigTDMSlots();
            break;


        default:
            break;
        }
    }
}




void keyb_FxsEventSimulation(void)
{
    int n_Keep = TRUE;

    while (n_Keep)
    {
        //      tcx_appClearScreen();



        //puts("\n 1) Current FXS State \n");
        //puts("\n 2) Set FXS HS \n");
        printf("\n##############################################################\n");
        printf("\n 1) Add FXS1 (HS6) to INL \n");
        printf("\n 2) Add FXS2 (HS7) to INL \n");
        printf("\n 3) Go OnHook \n");
        printf("\n 4) Go OffHook(ANS,INTER OG call,EXT OG CALL \n");
        printf("\n 5) Flash Key \n");
        printf("\n 6) CATiq Features \n");
        printf("\n 7) Host Configure\n");
        printf("\n 8) Dial Digit \n");
        printf("\n 9) Fixed FXS Slot Configuration \n");
        printf("\n 0) Exit\n");
        printf("\n##############################################################\n");
        printf("\n	Select Option:");


#if 0
        printf ( "-----------------------------\n" );
        printf ( "1 => Open data session\n");
        printf ( "2 => Send 128 bytes through IWU-INFO channel\n");
        printf ( "3 => Close data session\n");
        printf ( "- - - - - - - - - - - - - - - \n");
        printf ( "q => Return to Interface Menu\n" );
#endif
        switch (tcx_getch())
        {

        case '1':
            break;

        case '2':
            break;

        case '3':
            keyb_FxsOnhook();
            break;

        case '4':
            keyb_FxsOffhook();
            break;

        case '5':
            break;

        case '6':
            keyb_FxsCATiqCalls();
            break;

        case '7':
            keyb_HostportConfigure();
            break;

        case '8':
            keyb_FxsDial();
            break;

        case '9':
            keyb_FixedFxsHostportConfigure();
            break;

        case '0':
            n_Keep = FALSE;
            break;
        }

    }
}

extern u32 g_u32_UsedSlots;

void keyb_AudioTestCall(E_CMBS_AUDIO_CODEC codec)
{
    int n_Keep = TRUE;

    while (n_Keep)
    {
        char value = 0;
        int i = 0, startSlot = 0, slotMask = 0xF, item = 0;
        int slotSize = codec == CMBS_AUDIO_CODEC_PCM_LINEAR_WB ? 4 : 2;
        int slotShift = 2;
        int audioSlaveMenuIndex = 1;
        char audioSlaveMenuIndexName[] = "W16";

        switch (codec)
        {
        case CMBS_AUDIO_CODEC_PCM_LINEAR_WB:
            slotSize = 4;
            slotShift = 2;
            slotMask = 0xF;
            audioSlaveMenuIndex = 1;
            strcpy(audioSlaveMenuIndexName, "W16");
            break;
        case CMBS_AUDIO_CODEC_PCM_LINEAR_NB:
            slotSize = 2;
            slotShift = 2;
            slotMask = 0x3;
            audioSlaveMenuIndex = 2;
            strcpy(audioSlaveMenuIndexName, "N16");
            break;
        case CMBS_AUDIO_CODEC_PCMA:
            slotSize = 1;
            slotShift = 1;
            slotMask = 0x1;
            audioSlaveMenuIndex = 3;
            strcpy(audioSlaveMenuIndexName, "N8a");
            break;
        case CMBS_AUDIO_CODEC_PCMU:
            slotSize = 1;
            slotShift = 1;
            slotMask = 0x1;
            audioSlaveMenuIndex = 4;
            strcpy(audioSlaveMenuIndexName, "N8u");
            break;
        case CMBS_AUDIO_CODEC_PCMA_WB:
            slotSize = 2;
            slotShift = 1;
            slotMask = 0x3;
            audioSlaveMenuIndex = 5;
            strcpy(audioSlaveMenuIndexName, "W8a");
            break;
        case CMBS_AUDIO_CODEC_PCMU_WB:
            slotSize = 2;
            slotShift = 1;
            slotMask = 0x3;
            audioSlaveMenuIndex = 6;
            strcpy(audioSlaveMenuIndexName, "W8u");
            break;

        default:
            slotSize = 1;
            slotShift = 1;
            slotMask = 0x1;
        }

        //      tcx_appClearScreen();
        printf("\nHint: Caller number contains digits sequence for TDM Master!");
        printf("\n----------------------------  \n");

        // print slot selection menu
        while (startSlot + slotSize <= 12 && item < 9)
        {
            printf("%d => Use slots 0x%.4X (slots %d-%d) \n", ++item, slotMask, startSlot, startSlot + slotSize - 1);
            slotMask = slotMask << slotShift;
            startSlot += slotShift;
        }

        printf("- - - - - - - - - - - - - - -  \n");
        keyb_CallInfo();
        printf("r => Release test call  \n");
        printf("q => Return to Interface Menu  \n");

        value = tcx_getch();
        switch (value)
        {
        case 'r':
            keyb_IncCallRelease();
            break;
        case 'q':
            n_Keep = FALSE;
            break;
        default:
            if ( value >= '1' && value <= '9' )
            {
                int index = value - '1';
                startSlot = index * slotShift;

                printf("\n------------------------------  \n");
                printf("-- TDM MASTER INSTRUCTIONS --  \n");
                printf("------------------------------  \n");
                printf("- PRESS '0' : to mute all channels \n");
                printf("- PRESS '8' : to go into settings \n");
                printf("- PRESS '%d' : to choose %s \n", audioSlaveMenuIndex, audioSlaveMenuIndexName);
                printf("- PRESS '%s%d' : to choose %d start slot \n", startSlot < 10 ? "0" : "", startSlot, startSlot);
                printf("------------------------------  \n");

                // mark other slots as busy
                g_u32_UsedSlots = 0;
                for (i = 0; i < startSlot; i++) g_u32_UsedSlots |= 0x1 << i;

                {
                    ST_APPCALL_PROPERTIES st_Properties[6];
                    char s_codecs[CMBS_AUDIO_CODEC_MAX * 2] = { 0 };
                    char s_callername[CMBS_CALLER_NAME_MAX_LEN] = { 0 };
                    char s_callerparty[CMBS_CALLER_NAME_MAX_LEN] = { 0 };

                    sprintf(s_codecs, "%d", (codec));
                    sprintf(s_callername, "%s, Slots %d-%d", audioSlaveMenuIndexName, startSlot, startSlot + slotSize - 1);
                    sprintf(s_callerparty, "08%d%s%d", audioSlaveMenuIndex, startSlot < 10 ? "0" : "", startSlot);

                    if ( g_u16_DemoCallId == APPCALL_NO_CALL )
                    {
                        appcall_AutomatMode(E_APPCALL_AUTOMAT_MODE_ON);

                        st_Properties[0].e_IE = CMBS_IE_CALLERPARTY;
                        st_Properties[0].psz_Value = s_callerparty;
                        st_Properties[1].e_IE = CMBS_IE_CALLEDPARTY;
                        st_Properties[1].psz_Value = ALL_HS_STRING;
                        st_Properties[2].e_IE = CMBS_IE_MEDIADESCRIPTOR;
                        st_Properties[2].psz_Value = s_codecs;
                        st_Properties[3].e_IE = CMBS_IE_CALLERNAME;
                        st_Properties[3].psz_Value = s_callername;
                        st_Properties[4].e_IE = CMBS_IE_LINE_ID;
                        st_Properties[4].psz_Value = "1\0";
                        st_Properties[5].e_IE = CMBS_IE_MELODY;
                        st_Properties[5].psz_Value = "1\0";

                        g_u16_DemoCallId = appcall_EstablishCall(st_Properties, 5);
                        if ( g_u16_DemoCallId == APPCALL_NO_CALL )
                        {
                            printf("Call can not be set-up!\n");
                        }
                    }
                    else
                    {
                        printf("Please, release existing test call first\n");
                    }
                }
                n_Keep = FALSE;
            }
        }
    }
}

static void keyb_StartAFEAudioLoop(void)
{
  ST_IE_AFE_ENDPOINT st_Endpoint;

  AFEconfig.st_AFEEndpoints.e_AFEEndPointIN = CMBS_AFE_CHANNEL_MIC_IN;
  AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT = CMBS_AFE_CHANNEL_HS_SP_OUT;

  st_Endpoint.e_AFEChannel = AFEconfig.st_AFEEndpoints.e_AFEEndPointIN;
  app_AFEEnableDisableEndpoint(&st_Endpoint,1);

  st_Endpoint.e_AFEChannel = AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT;
  app_AFEEnableDisableEndpoint(&st_Endpoint,1);

  st_Endpoint.e_AFEChannel = CMBS_AFE_CHANNEL_LINE0_OUT;
  app_AFEEnableDisableEndpoint(&st_Endpoint,1);

  app_AFEConnectEndpoints(); //MIC -> HSSPOUT; MIC -> LOUT

  printf("\n AFE audio loop MIC -> HSSPOUT/LOUT Enabled \n");
}


static void keyb_StopAFEAudioLoop(void)
{
  ST_IE_AFE_ENDPOINT st_Endpoint;

  app_AFEConnectEndpoints();

  st_Endpoint.e_AFEChannel = AFEconfig.st_AFEEndpoints.e_AFEEndPointIN;
  app_AFEEnableDisableEndpoint(&st_Endpoint,0);

  st_Endpoint.e_AFEChannel = AFEconfig.st_AFEEndpoints.e_AFEEndPointOUT;
  app_AFEEnableDisableEndpoint(&st_Endpoint,0);

  st_Endpoint.e_AFEChannel = CMBS_AFE_CHANNEL_LINE0_OUT;
  app_AFEEnableDisableEndpoint(&st_Endpoint,0);

  printf("\n AFE audio loop MIC -> HSSPOUT/LOUT Disabled \n");
}


void keyb_AudioTest(void)
{
    int n_Keep = TRUE;
	u8 u8_loopback=1;
	int samples = 0;
	
    tcx_appClearScreen();
    while (n_Keep)
    {
        printf("-----------------------------  \n");
        printf("1 => Incoming PCM_LINEAR_WB 16 bit 16kHz call \n");
        printf("2 => Incoming PCM_LINEAR_NB 16 bit  8kHz call \n");
        printf("3 => Incoming PCMA_WB        8 bit 16kHz call \n");
        printf("4 => Incoming PCMU_WB        8 bit 16kHz call \n");
        printf("5 => Incoming PCMA           8 bit  8kHz call \n");
        printf("6 => Incoming PCMU           8 bit  8kHz call \n");
        printf("7 => Incoming PCM8           8 bit  8kHz call \n");
        printf(". => Start TDM loopback mode \n");
        printf(", => Stop TDM loopback mode \n\n");
		
        printf("- - - - - - - - - - - - - - -  \n");
        keyb_CallInfo();
        printf("r => Release test call  \n\n");
		
        printf("- - - - - - - - - - - - - - -  \n");
        printf("******* AFE TEST MENU *******  \n\n");
        printf("I => Enable  AFE Audio Loop Mode: MIC -> HSSPOUT & LOUT \n");
	    printf("J => Disable AFE Audio Loop Mode: MIC -> HSSPOUT & LOUT \n");
        printf("- - - - - - - - - - - - - - -  \n\n");

        printf("******* TDM HW TEST MENU *******  \n\n");
        printf("a => Start TDM HW LOOP Back  \n");
		printf("d => Send Fixed data to all the slots in DCX_TX direction\n\n");
        printf("s => Stop Audio  \n");
        printf("- - - - - - - - - - - - - - -  \n");		
        printf("q => Return to Interface Menu  \n");


        switch (tcx_getch())
        {
        case '1':
            keyb_AudioTestCall(CMBS_AUDIO_CODEC_PCM_LINEAR_WB);
            break;
        case '2':
            keyb_AudioTestCall(CMBS_AUDIO_CODEC_PCM_LINEAR_NB);
            break;
        case '3':
            keyb_AudioTestCall(CMBS_AUDIO_CODEC_PCMA_WB);
            break;
        case '4':
            keyb_AudioTestCall(CMBS_AUDIO_CODEC_PCMU_WB);
            break;
        case '5':
            keyb_AudioTestCall(CMBS_AUDIO_CODEC_PCMA);
            break;
        case '6':
            keyb_AudioTestCall(CMBS_AUDIO_CODEC_PCMU);
            break;
        case '7':
            keyb_AudioTestCall(CMBS_AUDIO_CODEC_PCM8);
            break;
        case 'r':
            keyb_IncCallRelease();
            break;

        case 'i':
        case 'I':
            keyb_StartAFEAudioLoop();
            break;

        case 'j':
        case 'J':
            keyb_StopAFEAudioLoop();
            break;
			
        case 'q':
            n_Keep = FALSE;
            break;
        case '.':
            {
                void *pv_RefIEList = cmbs_api_ie_GetList();
                cmbs_dem_TDMLoopbackStart(0, pv_RefIEList);
            }
            break;
        case ',':
            {
                void *pv_RefIEList = cmbs_api_ie_GetList();
                cmbs_dem_TDMLoopbackStop(0, pv_RefIEList);
            }
            break;
	
        case 'a':
			u8_loopback = 1;
			app_SrvSendTDMSlotConfiguration(0xFFFF);
            app_ProductionParamSet(CMBS_PARAM_IOM_TEST_MODE, &u8_loopback, sizeof(u8_loopback), 1);
            break;

        case 'd':

			printf("\n\n Enter number of samples to be repeated - Maximum 15 sample repeation is allowed currently\n");
			printf(" For e.g. If n=1, then same sample will be transmitted in every frame\n");
			printf(" For e.g. If n=2 (x,y), then 1st frame = x, 2nd frame = y, 3rd frame = x again and so on ..\n\n");

			tcx_scanf("%d", &samples);
			
			if (samples != 0)
				u8_loopback = 0x80 + samples;
			else
				u8_loopback = 0x81;

			if(u8_loopback >= 0x90)
				u8_loopback = 0x8F;
			app_SrvSendTDMSlotConfiguration(0xFFFF);	
            app_ProductionParamSet(CMBS_PARAM_IOM_TEST_MODE, &u8_loopback, sizeof(u8_loopback), 1);
            break;

        case 's':
			u8_loopback = 0;
            app_ProductionParamSet(CMBS_PARAM_IOM_TEST_MODE, &u8_loopback, sizeof(u8_loopback), 1);
            break;


        default:
            break;
        }
    }
}


E_CMBS_RC keyb_HostConfigureFxsPort(u8 u8_SrxFxs, u8 u8_FxsChannel, u8 u8_ChannelMode, u32 u32_SlotScheme)
{
    /*
    st_Data.pu8_Data[0] = FXS_EV_HOST_CONFIG
    st_Data.pu8_Data[1] = FXs Number
    st_Data.pu8_Data[2] = Channel Number
    st_Data.pu8_Data[3] = Channel Mode
    st_Data.pu8_Data[4] = SlotSchme MSB Byte1
    st_Data.pu8_Data[5] = SlotSchme MSB Byte2
    st_Data.pu8_Data[6] = SlotSchme MSB Byte3
    st_Data.pu8_Data[7] = SlotSchme MSB Byte4
    */
    ST_IE_DATA st_Data;
    u8 pu8_Data[10];
    pu8_Data[0] = FXS_EV_HOST_CONFIG;
    pu8_Data[1] = u8_SrxFxs; //FXS HS Number
    pu8_Data[2] = u8_FxsChannel;
    pu8_Data[3] = u8_ChannelMode;
    pu8_Data[4] = (u32_SlotScheme >> 8); //MSB
    pu8_Data[5] = (u32_SlotScheme & 0x00FF); //LSB
    st_Data.u16_DataLen = 6;
    st_Data.pu8_Data = pu8_Data;

    printf("\npu8_Data[4] = %x, pu8_Data[5] = %x \n", pu8_Data[4], pu8_Data[5]);

    return app_FXSEvent(&st_Data);
}



