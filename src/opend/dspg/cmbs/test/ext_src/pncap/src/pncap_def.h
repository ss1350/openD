/*
 * pncap_def.h
 *
 * PNCAP Definitions
 *
 */

#ifndef PNCAP_DEF_H
 #define PNCAP_DEF_H


#include "pncap_codes.h"

/***** Makros *************************************************************/

/***** Typedefs ***********************************************************/
// Allgemeiner Header der U-ETP Elemente
typedef struct 
{
  unsigned char ucEtpName;
  unsigned char ucEtpLen;
}
GP_HDR_DEF;

/**************************************************************************/

// base provided graphics

typedef struct
{
  unsigned char ucLength;
  unsigned char ucIDHigh;
  unsigned char ucIDLow;
  unsigned char ucMediaType;
  unsigned char ucValidity;
} UD_PNCAP_BPG_ENTRY_TAG_DEF;

typedef struct
{
  unsigned char ucInformationFieldCoding;
  unsigned char ucData;
} UD_PNCAP_BPG_INFORMATION_FIELD_DEF;

typedef struct
{
  unsigned char ucHeightInLines;
  unsigned char ucWidth;
  UD_PNCAP_BPG_INFORMATION_FIELD_DEF stInformationField;
} UD_PNCAP_BASE_PROVIDED_PICTO_DEF;

typedef struct 
{
  unsigned char ucNumberOfLines;
  UD_PNCAP_BPG_INFORMATION_FIELD_DEF stInformationField;
}
GP_LP_BPG_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char  ucParas;
}
GP_DIAL_CTL_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char  ucCondition;
}
GP_DISPLAY_RELEASE_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char  ucLocPresentId;
}
GP_LOCAL_PRESENTATION_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char  ucFunctionId;
  unsigned char  ucParas;
}
GP_LOCAL_FUNCTION_DEF;

/**************************************************************************/

typedef struct
{
  unsigned char  ucParas;                      
}
GP_KEY_REQ_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucParas;
}
GP_SOFTKEY_CAPABILITIES_DEF;

/**************************************************************************/

typedef struct
{
  unsigned char   ucNoOfPar;
  unsigned char   ucParas;
}
GP_LOC_EDIT_WND_ACT;   // Local Edit Window Activation

/**************************************************************************/

typedef struct
{
  unsigned char   ucInfoField;
}
GP_STAT_ANNOUNCM;   // Local Edit Window Activation

/**************************************************************************/
typedef struct 
{
  unsigned char  ucSkCode;
  unsigned char  ucEvaCode;
  unsigned char  ucValid;
  unsigned char  ucSkText;
}
GP_ENH_LOCAL_KEY_EVALUATION_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char  ucFunctionId;
  unsigned char  ucCtl;
  unsigned char  ucStartLine;
  unsigned char  ucMode;
}
GP_PRESENTATION_OF_CPN_DEF;

/**************************************************************************/
typedef struct 
{ 
  unsigned char ucIntNum;
  unsigned char ucComTheme;
}
GP_LINK_REQ_DEF;

/***********************************/
typedef struct
{ 
  unsigned char ucCnfInfo;
  unsigned char ucSuppDataAcc;
  unsigned char ucCharSetSupp;
  unsigned char ucExtSuppData;
  unsigned char ucExtCharSetSupp;
}
GP_LINK_CNF_DEF;

/***********************************/
typedef struct
{ 
  unsigned char ucRestFrag;
  unsigned char ucDataRec;
}
GP_DATA_PAC_DEF;

/***********************************/
typedef union 
{
  GP_LINK_REQ_DEF  stLinkReq;
  GP_LINK_CNF_DEF  stLinkCnf;
  GP_DATA_PAC_DEF  stDataPac;
}            
GP_DATA_FEAT_DEF;

/***********************************/
typedef struct 
{
  unsigned char  ucActPar;
  GP_DATA_FEAT_DEF u;
}
GP_ESC_TO_DATA_FEAT_DEF;
/***********************************/
typedef struct 
{
  unsigned char  ucMelody;
  unsigned char  ucVolume;
}
GP_MELODY_DEF;
/***********************************/
typedef struct 
{
  unsigned char  ucSettings;
}
GP_AUTOM_CALL_ACCEPT_DEF;

/***********************************/
typedef struct 
{
  unsigned char  ucSettings;
}
GP_HANDSFREE_MODE_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucFeatureId;
}
GP_LOC_FEAT_ACT_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucNameIndex;
  unsigned char ucCpnMsgLen;
  unsigned char ucCpnMsg;
}
GP_CPN_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucCpnCfgLen;
  unsigned char ucParas;
}
GP_CPN_PIC_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucDirAccType;
}
GP_DIR_ACC_TYPE_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucFeatureId;
  unsigned char ucParas;
}
GP_LOC_FEAT_PH_BK_ED_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char  ucMel;
}
GP_MEL_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucFeatureId;
  unsigned char ucParas;
}
GP_EDIT_FEATURE_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucParas;
}
GP_FP_FEATURE_LIST_IND_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucParas;
}
GP_FP_FEATURE_LIST_CFM_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucFeatureId;
}
GP_FP_FEATURE_ACT_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucIntNr;
  unsigned char aucName[16];
}
GP_FP_FEATURE_RELEASE;


/**************************************************************************/
#define REQUEST_DATE    0x01
#define REQUEST_TIME    0x02

typedef struct 
{
  unsigned char ucReqType;
  unsigned char ucReqType2;
}
GP_SYSTEM_TIME_DATE_REQ_DEF;

/**************************************************************************/
#define DATE_INDICATION 0x03

#define DATE_MODE_GER   0x00
#define DATE_MODE_US    0x01

typedef struct 
{
  unsigned char ucDateInd;
  unsigned char ucDay;
  unsigned char ucMonth;
  unsigned char ucYear;
  unsigned char ucMode;
}
GP_SYSTEM_DATE_IND_DEF;

/**************************************************************************/
#define TIME_INDICATION 0x04

#define TIME_MODE_12    0x00
#define TIME_MODE_24    0x01

typedef struct 
{
  unsigned char ucTimeInd;
  unsigned char ucHour;
  unsigned char ucMinute;
  unsigned char ucMode;
}
GP_SYSTEM_TIME_IND_DEF;

/**************************************************************************/

/** all values should be in BCD format */
struct pncap_date_and_time
{ 
  unsigned char   year;       ///< Valid values for the year shall be in the range 0x00 - 0x99
  unsigned char   month;      ///< Valid values for the month shall be in the range 0x01 - 0x12
  unsigned char   day;        ///< Valid values for the day shall be in the range 0x01 - 0x31
  unsigned char   date_mode;  ///< The mode indicates whether the mode day/month/year (0) or the mode month/day/year (1) is active
  unsigned char   hours;      ///< Valid values for the hour shall be in the range 0x00 - 0x23
  unsigned char   minutes;    ///< Valid values for the minute shall be in the range 0x00 - 0x59
  unsigned char   time_mode;  ///< The mode shall indicate whether the 12h mode is active (0) or the 24h mode is active (1)
};

/**************************************************************************/
typedef struct
{
  unsigned char ucTransInd;
}
GP_POS_NEG_ACK;

/**************************************************************************/
typedef struct
{
  unsigned char ucMode;
  unsigned char ucLines;
}
GP_DISPLAY_REQ_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucParas;
}
GP_DISPLAY_SHORTCUT_LABEL_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucTimeStampMSB;
  unsigned char ucTimeStampLSB;
}
GP_AUTO_SUBSCRIPTION_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucFeatureId;
  unsigned char ucParInitAction;
  unsigned char ucParInitVolSpeaker;
  unsigned char ucParInitVolHeadset;
  unsigned char ucParInitVolHandset;

}
GP_LOC_FEAT_RECVOL_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucFeatureId;
  unsigned char ucParasLen;
  unsigned char ucParas;
}
GP_LOC_FEAT_FS_ED_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucIntNo;
  unsigned char ucIntName;
}
GP_INT_NUMBER_NAME;

/**************************************************************************/
typedef struct
{
  unsigned char ucKeyReq;
}
GP_TEMPORARY_KEY_REQ;

/**************************************************************************/
typedef struct
{
  unsigned char ucKeyRel;
}
GP_TEMPORARY_KEY_RELEASE;

/**************************************************************************/
typedef struct
{
  unsigned char ucCallModeId; // von der Basis einzustellender Sprachweg
}
GP_CALL_MODE_IND;


/**************************************************************************/
typedef struct
{
  unsigned char ucParas;
}
GP_FP_CAP_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucTransId;
}
GP_TRANSACTION_ID;

/**************************************************************************/
typedef struct
{
  unsigned char ucExtendedData;
  unsigned char ucHighReqNumber;
  unsigned char ucLowReqNumber;
}
GP_DPS_REQUEST;

/**************************************************************************/
typedef struct
{
  unsigned char ucRejectReason;
  unsigned char ucHighReqNumber;
  unsigned char ucLowReqNumber;
}
GP_DPS_REJECT;

/**************************************************************************/
typedef struct
{
  unsigned char ucExtendedData;
  unsigned char ucRfType;
  unsigned char ucHighReqNumber;
  unsigned char ucLowReqNumber;
  unsigned char aucRssiValues[39];
}
GP_DPS_CFM;

/**************************************************************************/
typedef struct
{
  unsigned char ucRingOnTimerIn10ms;
}
GP_RING_ON_EVAL;

/**************************************************************************/
typedef struct
{
  unsigned char ucMsnId;
}
GP_MSN_ID;


/**************************************************************************/
typedef struct
{
  unsigned char aucMax[60]; // Laenge noch anpassen !!!!!
}
GP_MAXLEN_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucService;
  unsigned char ucEvent;
}
GP_TUBE_DEF;


typedef struct 
{
  unsigned char ucField;
  unsigned char ucFieldLen;
  unsigned char ucFieldParas;
}
GP_ACOUSTIC_PAR_DEF;


typedef struct
{
  unsigned char ucField;
  unsigned char ucFieldLen;
  unsigned char ucFieldAction;
  unsigned char ucSkTextParas;
}
GP_IDLESK_PAR_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucInformationFieldId;
  unsigned char ucInformationLen;
  unsigned char ucInformation;
}
GP_CONFIGURATION_REQ;

/**************************************************************************/
typedef struct 
{
  unsigned char ucReason;
}
GP_RELEASE_REASON;


/**************************************************************************/
typedef struct 
{
  unsigned char ucRegModeResult;
}
GP_REG_MODE_CONFIRM_RESULT;

/**************************************************************************/
typedef struct 
{
  unsigned char ucParas;
}
GP_LA_LIST_IND_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucParas;
}
GP_SETUP_PAR_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucParas;
}
GP_INFO_DISPLAY_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucValue;
}
GP_TRANSPARENT_CH_IND_DEF;

/**************************************************************************/
typedef struct 
{
  unsigned char ucInterRef;
  unsigned char ucStatus;
}
GP_CALL_STATE;
/**************************************************************************/
typedef struct
{
  unsigned char ucParas;
}
GP_SUOTA_STATUS_DEF;

/**************************************************************************/
typedef struct
{
  unsigned char ucOptData;
}
GP_LOCAL_CONF_MANAGEMENT_DEF;

/**************************************************************************/

/**************************************************************************/
typedef struct 
{
  GP_HDR_DEF    stHdr;
  union GP_DEF
  {
    GP_DIAL_CTL_DEF                     stDialCtl;
    GP_LOCAL_PRESENTATION_DEF           stLocalPresentation;
    GP_LOCAL_FUNCTION_DEF               stLocalFunction;
    GP_KEY_REQ_DEF                      stKeyReq;
    GP_SOFTKEY_CAPABILITIES_DEF         stSoftkeyCapabilities;
    GP_ENH_LOCAL_KEY_EVALUATION_DEF     stLocalKeyEvaluation;
    GP_PRESENTATION_OF_CPN_DEF          stPresentationOfCpn;
    GP_ESC_TO_DATA_FEAT_DEF             stEscToDataFeat;
    GP_AUTOM_CALL_ACCEPT_DEF            stAutomCallAccept;
    GP_HANDSFREE_MODE_DEF               stHandsFreeMode;
    GP_LOC_FEAT_ACT_DEF                 stLocFeatAct;
    GP_LOC_FEAT_PH_BK_ED_DEF            stLocFeatPhBkEd;
    GP_CPN_DEF                          stCpn;
    GP_DIR_ACC_TYPE_DEF                 stDirAccType;
    GP_MEL_DEF                          stMel;
    GP_EDIT_FEATURE_DEF                 stEditFeature;
    GP_FP_FEATURE_LIST_IND_DEF          stFpFeatListInd;
    GP_FP_FEATURE_LIST_CFM_DEF          stFpFeatListCfm;
    GP_FP_FEATURE_RELEASE               stFpFeatRelease;
    GP_FP_FEATURE_ACT_DEF               stFpFeatAct;
    GP_DISPLAY_RELEASE_DEF              stDspRel;
    GP_SYSTEM_TIME_DATE_REQ_DEF         stTimeDateReq;
    GP_SYSTEM_DATE_IND_DEF              stDateInd;
    GP_SYSTEM_TIME_IND_DEF              stTimeInd;
    GP_POS_NEG_ACK                      stPosNegAck;
    GP_DISPLAY_REQ_DEF                  stDspReq;
    GP_DISPLAY_SHORTCUT_LABEL_DEF       stDspShCutLabel;
    GP_AUTO_SUBSCRIPTION_DEF            stAutoSub;
    GP_LOC_FEAT_RECVOL_DEF              stLocFeatRecVol;
    GP_LOC_FEAT_FS_ED_DEF               stLocFeatFsEd;
    GP_LOC_EDIT_WND_ACT                 stLocEditWndAct;
    GP_STAT_ANNOUNCM                    stStatAnnouncm;
    GP_INT_NUMBER_NAME                  stIntNumberName;
    GP_TEMPORARY_KEY_REQ                stTempKeyReq;
    GP_TEMPORARY_KEY_RELEASE            stTempKeyRel;
    GP_CALL_MODE_IND                    stCallModeInd;
    GP_FP_CAP_DEF                       stFpCap;
    GP_TRANSACTION_ID                   stTransactionId;
    GP_DPS_REQUEST                      stDpsRequest;
    GP_DPS_REJECT                       stDpsReject;
    GP_DPS_CFM                          stDpsCfm;
    GP_RING_ON_EVAL                     stRingOnEval;
    GP_MSN_ID                           stMsnId;
    GP_MAXLEN_DEF                       stMaxlen;
    GP_TUBE_DEF                         stTube;
    GP_ACOUSTIC_PAR_DEF                 stAcousticPar;
    GP_IDLESK_PAR_DEF                   stIdleSkPar;
    GP_CONFIGURATION_REQ                stConfigurationReq;
    GP_RELEASE_REASON                   stRelReason;
    GP_REG_MODE_CONFIRM_RESULT          stRegModeResult;
    GP_LA_LIST_IND_DEF                  stLaListInd;
    GP_SETUP_PAR_DEF                    stSetupPar;
    GP_LP_BPG_DEF                       stLpBpg;
    GP_CPN_PIC_DEF                      stCpnPic;
    GP_INFO_DISPLAY_DEF                 stInfoDisplay;
    GP_TRANSPARENT_CH_IND_DEF           stTrChInd;
    GP_CALL_STATE                       stCallState;
    GP_SUOTA_STATUS_DEF                 stSuotaStatus;
    GP_LOCAL_CONF_MANAGEMENT_DEF        stLocalConfManagement;
  } u;
} 
GP_MSG_DEF;

#endif  //PNCAP_DEF_H
////////////////////////////////////////////////////////////////////////////////////////////////////
//  EOF pncap_def_h                                                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////
