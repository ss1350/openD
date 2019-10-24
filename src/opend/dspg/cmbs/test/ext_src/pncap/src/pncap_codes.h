/*
 * pncap_codes.h
 *
 * PNCAP identifiers
 *
 */

#ifndef PNCAP_CODES_H_
  #define PNCAP_CODES_H_



/**************************************************************************/
/********                     ETP-Message-Types                     *******/
/**************************************************************************/

#define PNCAP_PIE_DISCRIMINATE_EMC                0x81
#define PNCAP_PIE_SIEMENS_ID                      0x0002
#define PNCAP_PIE_SIEMENS_ID_HIGH                 0x00
#define PNCAP_PIE_SIEMENS_ID_LOW                  0x02

#define PNCAP_PIE_DISPLAY_REQEST                          0x01
#define PNCAP_PIE_DISPLAY_RELEASE                         0x02
#define PNCAP_PIE_DIAL_CONTROL                            0x03
#define PNCAP_PIE_LOCAL_PRESENTATION                      0x04
#define PNCAP_PIE_APPLICATION_CLASS_REQUEST               0x05
#define PNCAP_PIE_APPLICATION_CLASS_CONFIRM               0x06
#define PNCAP_PIE_ESCAPE_TO_EDIT_FEATURES                 0x07
#define PNCAP_PIE_NEGATIVE_ACKNOWLEDGEMENT                0x08
#define PNCAP_PIE_LOCAL_CONFIGURATION_MANAGEMENT          0x09
#define PNCAP_PIE_LOCAL_FEATURE_RESULT                    0x0A
#define PNCAP_PIE_EXTENDED_DISPLAY_RELEASE                0x0B
#define PNCAP_PIE_LOCAL_CONFIGURATION_REQUEST             0x0C
//#define 0x0D  0x0D
//#define 0x0E  0x0E
#define PNCAP_PIE_ESCAPE_TO_KEYPAD_FEATURES               0x0F

//#define 0x10  0x10
//#define 0x11  0x11
//#define 0x12  0x12
#define PNCAP_PIE_LOCAL_FEATURE_LIST_REQUEST              0x13
#define PNCAP_PIE_LOCAL_FEATURE_LIST_CONFIRM              0x14
#define PNCAP_PIE_LOCAL_FEATURE_ACTIVATION                0x15
#define PNCAP_PIE_LOCAL_FEATURE_RELEASE                   0x16
#define PNCAP_PIE_LOCAL_FEATURE_CANCELLATION              0x17
#define PNCAP_PIE_PT_STATUS                               0x18
#define PNCAP_PIE_ALERTING_SEQUENCE                       0x19
#define PNCAP_PIE_LOCAL_FUNCTION                          0x1a
#define PNCAP_PIE_LOCAL_KEY_EVALUATION                    0x1b
#define PNCAP_PIE_KEY_REQUEST                             0x1c
#define PNCAP_PIE_ONE_BUTTON_DIAL                         0x1d
#define PNCAP_PIE_SERVICE_RELEASE                         0x1e
#define PNCAP_PIE_SERVICE_RELEASE_REQUEST                 0x1f

#define PNCAP_PIE_SERVICE_ESTABLISH                       0x20
#define PNCAP_PIE_CALL_SUSPEND_AND_RESUME                 0x21
#define PNCAP_PIE_SOFTKEY_CAPABILITIES                    0x25
#define PNCAP_PIE_ENHANCED_LOCAL_KEY_EVALUATION           0x26
#define PNCAP_PIE_ESCAPE_TO_DATA_FEATURES                 0x27
#define PNCAP_PIE_PROPRIETARY_CALLING_PARTY_NUMBER        0x28
#define PNCAP_PIE_PT_CAPABILITIES                         0x29
#define PNCAP_PIE_NAME_OF_CALLING_PARTY                   0x2A
#define PNCAP_PIE_REDIAL_BUFFER_STORE                     0x2B
#define PNCAP_PIE_MELODY                                  0x2C
//#define  0x2D
//#define  0x2E
#define PNCAP_PIE_FT_CAPABILITIES                         0x2F

#define PNCAP_PIE_DIAL_END                                0x30
#define PNCAP_PIE_DIRECT_ACCESS                           0x31
#define PNCAP_PIE_TEMPORARY_KEY_REQUEST                   0x32
#define PNCAP_PIE_TEMPORARY_KEY_RELEASE                   0x33
#define PNCAP_PIE_TEXT_GROUP_REQUEST                      0x34
#define PNCAP_PIE_TEXT_GROUP_CONFIRM                      0x35
#define PNCAP_PIE_STOP_REDIAL_BUFFER_STORE                0x36
#define PNCAP_PIE_START_VIRTUAL_CALL                      0x37
#define PNCAP_PIE_AUTOMATIC_CALL_ACCEPTION_REQUEST        0x38
#define PNCAP_PIE_HANDSFREE                               0x39
#define PNCAP_PIE_AUTO_SUBSCRIPTION                       0x3A
#define PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME                   0x3B
    #define PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_DATE_INDICATION 0x03

    #define PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_DATE_MODE_GER   0x00
    #define PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_DATE_MODE_US    0x01

    #define PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_TIME_INDICATION 0x04

    #define PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_TIME_MODE_12    0x00
    #define PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_TIME_MODE_24    0x01

#define PNCAP_PIE_FP_FEATURE_LIST_INDICATION              0x3C
#define PNCAP_PIE_FP_FEATURE_LIST_CONFIRM                 0x3D
#define PNCAP_PIE_FP_FEATURE_ACTIVATION                   0x3E
#define PNCAP_PIE_FP_FEATURE_RELEASE                      0x3F

#define PNCAP_PIE_DISPLAY_SHORTCUT_LABEL                  0x50
#define PNCAP_PIE_LOCAL_EDIT_WINDOW_ACTIVATION            0x51
#define PNCAP_PIE_LOCAL_EDIT_WINDOW_RELEASE               0x52
#define PNCAP_PIE_LOCAL_EDIT_WINDOW_CANCEL                0x53
#define PNCAP_PIE_INT_NUMBER_AND_NAME                     0x54
#define PNCAP_PIE_AUTOMATIC_REDIAL_NUMBER                 0x55
#define PNCAP_PIE_BABYPHON_CALL                           0x56
#define PNCAP_PIE_BABYPHON_MUTE                           0x57
#define PNCAP_PIE_STATUS_ANNOUNCEMENT                     0x58
#define PNCAP_PIE_POSITIVE_ACKNOWLEDGEMENT                0x59
#define PNCAP_PIE_ACOUSTIC_PARAMETERS                     0x5A
#define PNCAP_PIE_TRANSACTION_IDENTIFIER                  0x5B
#define PNCAP_PIE_LOCAL_FEATURE_HOLD                      0x5C
#define PNCAP_PIE_LOCAL_FEATURE_RESUME                    0x5D
#define PNCAP_PIE_AUTOMATIC_CALL_ACCEPTION_CONFIRM        0x5E
#define PNCAP_PIE_PAGING_INDICATION                       0x5F

#define PNCAP_PIE_CONCATENATED_SMS_CAP_REQUEST            0x60
#define PNCAP_PIE_CONCATENATED_SMS_CAP_CONFIRM            0x61
#define PNCAP_PIE_CALL_CLASS                              0x62
#define PNCAP_PIE_CALL_STATE                              0x63
#define PNCAP_PIE_CALL_MODE_REQ                           0x64
#define PNCAP_PIE_KEYPAD                                  0x65
#define PNCAP_PIE_ETP_MODEM_MODE                          0x66
#define PNCAP_PIE_DPS_DATA_REQUEST                        0x67
#define PNCAP_PIE_DPS_DATA_CONFIRM                        0x68
#define PNCAP_PIE_DPS_DATA_REJECT                         0x69

// reserved #define                                       0x6A
// reserved #define                                       0x6B
// reserved #define                                       0x6C
// reserved #define                                       0x6D
// reserved #define                                       0x6E
// reserved #define                                       0x6F

#define PNCAP_PIE_REG_MODE_REQ                            0x70
#define PNCAP_PIE_REG_MODE_CFM                            0x71
#define PNCAP_PIE_REG_COMPLETE                            0x72

#define PNCAP_PIE_TRANSPARENT_CHANNEL_SWITCH              0x80
#define PNCAP_PIE_TRANSPARENT_CHANNEL_CONTENT             0x81
#define PNCAP_PIE_DISPLAY                                 0x82
#define PNCAP_PIE_SIGNAL                                  0x83
#define PNCAP_PIE_TRANSPARENT_CHANNEL_CFM                 0x84
#define PNCAP_PIE_SUBSCRIPTION_LIST_REQ                   0x85
#define PNCAP_PIE_SUBSCRIPTION_LIST_CFM                   0x86
#define PNCAP_PIE_REMOTE_STATE_ANNOUNCEMENT               0x87
#define PNCAP_PIE_REMOTE_STATE_EXPIRATION_TIME            0x88
#define PNCAP_PIE_REMOTE_STATE_CFM                        0x89

// reserved #define                                       0x8A

// reserved #define                                       0x8F

#define PNCAP_PIE_PT_STATUS_REQ                           0x90
#define PNCAP_PIE_INTERN_NAME_IND                         0x91
#define PNCAP_PIE_LOCAL_SETTINGS_REQ                      0x92
#define PNCAP_PIE_LOCAL_SETTINGS                          0x93
#define PNCAP_PIE_REG_INCOMPLETE                          0x94
#define PNCAP_PIE_RING_ON_EVALUATION                      0x95
#define PNCAP_PIE_MSN_ID                                  0x96
// #define
#define PNCAP_PIE_TUBE                                    0x98
#define PNCAP_PIE_WARP                                    0x99
#define PNCAP_PIE_SMS_DATA                                0x9A
#define PNCAP_PIE_INTERNAL_NAME_REQ                       0x9B
#define PNCAP_PIE_INTERNAL_NAME_CFM                       0x9C
#define PNCAP_PIE_SUBSCRIPTION_LIST_CHANGE                0x9D
#define PNCAP_PIE_CALL_INFORMATION                        0x9E
// reserved for further use #define PNCAP_PIE_CALL_BUFFER_STORE                       0x9F
#define PNCAP_PIE_EXTENDED_CAP_REQ                        0xA0
#define PNCAP_PIE_EXTENDED_CAP_CFM                        0xA1
#define PNCAP_PIE_IDN_REQ                                 0xA2
#define PNCAP_PIE_IDN_CFM                                 0xA3
#define PNCAP_PIE_DISTRIBUTED_COM_REQ                     0xA4
#define PNCAP_PIE_DISTRIBUTED_COM_CFM                     0xA5
#define PNCAP_PIE_DISTRIBUTED_COM_REJECT                  0xA6
#define PNCAP_PIE_INT_HOME_APPLICATION_REQ                0xA7         // Integrated Home Application Request
#define PNCAP_PIE_INT_HOME_APPLICATION_CFM                0xA8         // Integrated Home Application Confirm
#define PNCAP_PIE_SETUP_PAR                               0xA9         // Setup Parameter
#define PNCAP_PIE_CALL_CONTROL_REQ                        0xAA
#define PNCAP_PIE_HS_INVENTORY_DATA_REQ                   0xAB         // Handset Inventary Data
#define PNCAP_PIE_HS_INVENTORY_DATA_CFM                   0xAC
#define PNCAP_PIE_BGR_STATUS_REQ                          0xAD
#define PNCAP_PIE_BGR_STATUS_CFM                          0xAE
#define PNCAP_PIE_DISTRIBUTED_COM_CONTROL_DATA            0xAF         // Distributed Communication Call Contol Data
#define PNCAP_PIE_FP_SETTINGS_REQ                         0xB0         // FP-Settings Request
#define PNCAP_PIE_FP_SETTINGS_CFM                         0xB1         // FP-Settings Confirm
#define PNCAP_PIE_INT_HOME_ICON_REQ                       0xB2         // Integrated Home Icon Request
#define PNCAP_PIE_INT_HOME_ICON_CFM                       0xB3         // Integrated Home Icon Request
#define PNCAP_PIE_PP_CFG_REQ                              0xB4         // PP Configuration Request
#define PNCAP_PIE_PP_CFG_CFM                              0xB5         // PP Configuration Confirm
#define PNCAP_PIE_IDLE_SK_PRG_REQ                         0xB6         // Idle Softkey Programming Indication
#define PNCAP_PIE_REPERTORY_KEY_REQ                       0xB7         // Repertory Key Request
#define PNCAP_PIE_REPERTORY_KEY_CFM                       0xB8         // Repertory Key Confirmation
#define PNCAP_PIE_REPERTORY_KEY_IND                       0xB9         // Repertory Key Indication
// #define  reserved 0xB9
#define PNCAP_PIE_PCM_MODE                                0xBB         //
#define PNCAP_PIE_RELEASE_REASON                          0xBC         // Release Reason Indication
#define PNCAP_PIE_DETACH_DATA                             0xBD         // Detach Data
#define PNCAP_PIE_SCENARIO_NAMES_REQ                      0xBE         // Scenario Names Request
#define PNCAP_PIE_SCENARIO_NAMES_CFM                      0xBF         // Scenario Names Confirm
#define PNCAP_PIE_IDLE_SKEY_PRG_REQ                       0xC0         // Idle Softkey Programming Request
#define PNCAP_PIE_IDLE_SKEY_PRG_CFM                       0xC1         // Idle Softkey Programming Confirm
#define PNCAP_PIE_LEW_TRACKING                            0xC2         // Local Edit Window Tracking
#define PNCAP_PIE_LINE_ID                                 0xC3         // Line ID
//#define PNCAP_PIE_COND_REG_MODE_REQ                       0xC4         // reserved
#define PNCAP_PIE_LIST_ACCESS                             0xC5         // List Access / Data Exchange
#define PNCAP_PIE_SUPPORTED_LIST_IND                      0xC6         // Supported List Indication
#define PNCAP_PIE_SUPPORTED_LIST_CFM                      0xC7         // Supported List Confirmation
#define PNCAP_PIE_LOCAL_REPRESENTATION_REQ                0xC8         // Local Representation Request
#define PNCAP_PIE_LOCAL_REPRESENTATION_CFM                0xC9         // Local Representation Confirmation
#define PNCAP_PIE_PROPRIETARY_CPN_PICTURE                 0xCA         // Proprietary CPN and clip picture
#define PNCAP_PIE_NAME_OF_CPN_PICTURE                     0xCB         // Name of Calling Party and CLIP picture
#define PNCAP_PIE_LOCPRES_BASE_PROV_GRAPHIC               0xCC         // Local representation of base provided graphic
#define PNCAP_PIE_LIST_ACCSESS_ATTR                       0xCD         // List access attributes
#define PNCAP_PIE_INFO_DISPLAY                            0xCE         // Info Display
#define PNCAP_PIE_TRANSPARENT_CHANNEL_IND                 0xCF         // Indication of transparent channel information
#define PNCAP_PIE_SUOTA_STATUS_DATA                       0xD0         // SUOTA status data
#define PNCAP_PIE_PT_CAPABILITIES_REDIRECT                0xD1         // Indicate that PT CAPS must be requested in silent call

/**************************************************************************/
/********              Local Feature Function Elements              *******/
/**************************************************************************/
#define PNCAP_LFF_UNDEF                       0x00
#define PNCAP_LFF_PHONE_BOOK                  0x01
#define PNCAP_LFF_RINGER_CTL                  0x02
#define PNCAP_LFF_REC_VOL_CTL                 0x03
#define PNCAP_LFF_SETTINGS                    0x04
#define PNCAP_LFF_KEY_LOCK                    0x05
#define PNCAP_LFF_PHONE_BOOK_LIST_DIAL        0x06
#define PNCAP_LFF_INT_KEY                     0x07
#define PNCAP_LFF_MACRO                       0x0a
#define PNCAP_LFF_PHONE_BOOK_EDITOR           0x0B
#define PNCAP_LFF_REDIAL_NUMBER_LIST          0x0E
#define PNCAP_LFF_NET_LIST                    0x10
#define PNCAP_LFF_NET_LIST_SELECTION          0x11
#define PNCAP_LFF_PTT_SERVICE_LIST            0x12
#define PNCAP_LFF_PTT_SERVICE_LIST_SELECTION  0x13
#define PNCAP_LFF_FULL_SCREEN_EDITOR          0x14

#define PNCAP_LFF_PUSH_SMS                    0x1d
#define PNCAP_LFF_MMS_INBOX                   0x1e
#define PNCAP_LFF_GET_PHONE_BOOK_ENTRY        0x1f
#define PNCAP_LFF_EMAIL_INBOX                 0x20
//#define reserved for DOMO                     0x21
#define PNCAP_LFF_LINE_ATTRIB_ASSIGNM         0x22    // Line Attribute Assignment
#define PNCAP_LFF_MISSED_APPOINTMENTS         0x23
// #define PNCAP_LFF_MUTE                       0x24    // triggers mute-application in PP
#define PNCAP_LFF_HANDSFREE_PROFILE           0x25    // handsfree profile settings for external calls
#define PNCAP_LFF_EARPIECE_PROFILE            0x26    // earpiece profile settings for external calls

/**************************************************************************/
/********              Local Feature Class Elements                 *******/
/**************************************************************************/
#define PNCAP_LFC_TALK_STATE                  0x01
#define PNCAP_LFC_CALL_STATE                  0x02
#define PNCAP_LFC_SERVICE                     0x04
#define PNCAP_LFC_DIALING_STATE               0x08
#define PNCAP_LFC_KOWN_STATE                  0x10

/**************************************************************************/
/********              Call Class Elements                          *******/
/**************************************************************************/

#define PNCAP_CALL_CLASS_SOURCE_AB            0x00  // analoge line
#define PNCAP_CALL_CLASS_SOURCE_GSM           0x01  //
#define PNCAP_CALL_CLASS_SOURCE_ISDN          0x02  //
#define PNCAP_CALL_CLASS_SOURCE_VOIP          0x03  //
#define PNCAP_CALL_CLASS_SOURCE_AM            0x04  //answering machine
#define PNCAP_CALL_CLASS_SOURCE_PP            0x05  //
#define PNCAP_CALL_CLASS_SOURCE_PAGE          0x06  //
#define PNCAP_CALL_CLASS_SOURCE_REMOTE        0x07  // remote controll activated
#define PNCAP_CALL_CLASS_SOURCE_REMOTE_EXT    0x08  // remote controll activated


/**************************************************************************/
/********            Local Configuration Management                  ******/
/**************************************************************************/
// Option identifiers
#define PNCAP_LCM_HANDSET_VOLUME              0x00
#define PNCAP_LCM_SPEAKER_VOLUME              0x01
#define PNCAP_LCM_HEADSET_VOLUME              0x02
#define PNCAP_LCM_L1_RING_VOLUME              0x03
#define PNCAP_LCM_L1_RING_PITCH               0x04
#define PNCAP_LCM_L2_RING_VOLUME              0x05
#define PNCAP_LCM_L2_RING_PITCH               0x06
#define PNCAP_LCM_L3_RING_VOLUME              0x07
#define PNCAP_LCM_L3_RING_PITCH               0x08
#define PNCAP_LCM_L4_RING_VOLUME              0x09
#define PNCAP_LCM_L4_RING_PITCH               0x0A
#define PNCAP_LCM_INT_RING_VOLUME             0x0B
#define PNCAP_LCM_INT_RING_PITCH              0x0C
#define PNCAP_LCM_MASTER_RING_CONTROL         0x0D
#define PNCAP_LCM_LANGUANGE                   0x0E
#define PNCAP_LCM_ROOM_MONITOR_THRESHOLD      0x0F
#define PNCAP_LCM_ROOM_MONITOR_NUMBER         0x10
#define PNCAP_LCM_ROMM_MONITOR_CONTROL        0x11
#define PNCAP_LCM_KEY_CLICK_CONTROL           0x12
#define PNCAP_LCM_BATTERY_WARNING_CONTROL     0x13
#define PNCAP_LCM_FACTORY_DEFAULTS            0x15
#define PNCAP_LCM_RANGE_WARNING               0x16
#define PNCAP_LCM_BACKLIGHT_DURATION          0x17
#define PNCAP_LCM_AUDIO_TIMEOUT               0x18
#define PNCAP_LCM_EDITOR_RINGER_TIMEOUT       0x19
#define PNCAP_LCM_POSITION_OF_CARRIER_LABEL   0x1B
#define PNCAP_LCM_LOCAL_MISSED_CALL_LIST      0x20
#define PNCAP_LCM_MOBILE_ALIAS                0x80
#define PNCAP_LCM_DATE_AND_TIME               0x81
#define PNCAP_LCM_HANDSET_VERSION_NUMBER      0x82
#define PNCAP_LCM_CARRIER_NAME                0x83
#define PNCAP_LCM_END_INDICATOR               0xFF


/**************************************************************************/
/********              Local Representation Elements                *******/
/**************************************************************************/
#define PNCAP_LREP_LOUDNESS0                  0x00
#define PNCAP_LREP_LOUDNESS1                  0x01
#define PNCAP_LREP_LOUDNESS2                  0x02
#define PNCAP_LREP_LOUDNESS3                  0x03
#define PNCAP_LREP_LOUDNESS4                  0x04
#define PNCAP_LREP_LOUDNESS5                  0x05
#define PNCAP_LREP_LOUDNESS6                  0x06
#define PNCAP_LREP_SOUND0                     0x08
#define PNCAP_LREP_SOUND1                     0x09
#define PNCAP_LREP_SOUND2                     0x0a
#define PNCAP_LREP_SOUND3                     0x0b
#define PNCAP_LREP_SOUND4                     0x0c
#define PNCAP_LREP_SOUND5                     0x0d
#define PNCAP_LREP_SOUND6                     0x0e
#define PNCAP_LREP_ALERTING                   0x0f
#define PNCAP_LREP_CALL_TIMER                 0x10
#define PNCAP_LREP_ONE_LINE_CALL_TIMER        0x11
#define PNCAP_LREP_SOUND7                     0x12
#define PNCAP_LREP_SOUND8                     0x13
#define PNCAP_LREP_SOUND9                     0x14
#define PNCAP_LREP_AM_ALERTING                0x15
#define PNCAP_LREP_LOUDNESS0_SEL              0x16
#define PNCAP_LREP_LOUDNESS1_SEL              0x17
#define PNCAP_LREP_LOUDNESS2_SEL              0x18
#define PNCAP_LREP_LOUDNESS3_SEL              0x19
#define PNCAP_LREP_LOUDNESS4_SEL              0x1A
#define PNCAP_LREP_LOUDNESS5_SEL              0x1B
#define PNCAP_LREP_LOUDNESS6_SEL              0x1C
#define PNCAP_LREP_SOUND0_SEL                 0x1D
#define PNCAP_LREP_SOUND1_SEL                 0x1E
#define PNCAP_LREP_SOUND2_SEL                 0x1F
#define PNCAP_LREP_SOUND3_SEL                 0x20
#define PNCAP_LREP_SOUND4_SEL                 0x21
#define PNCAP_LREP_SOUND5_SEL                 0x22
#define PNCAP_LREP_SOUND6_SEL                 0x23
#define PNCAP_LREP_SOUND7_SEL                 0x24
#define PNCAP_LREP_SOUND8_SEL                 0x25
#define PNCAP_LREP_SOUND9_SEL                 0x26
#define PNCAP_LREP_SPEAKER0                   0x27
#define PNCAP_LREP_SPEAKER1                   0x28
#define PNCAP_LREP_SPEAKER2                   0x29
#define PNCAP_LREP_SPEAKER3                   0x2A
#define PNCAP_LREP_SPEAKER4                   0x2B
#define PNCAP_LREP_SPEAKER5                   0x2C
#define PNCAP_LREP_SPEAKER0_SEL               0x2D
#define PNCAP_LREP_SPEAKER1_SEL               0x2E
#define PNCAP_LREP_SPEAKER2_SEL               0x2F
#define PNCAP_LREP_SPEAKER3_SEL               0x30
#define PNCAP_LREP_SPEAKER4_SEL               0x31
#define PNCAP_LREP_SPEAKER5_SEL               0x32
#define PNCAP_LREP_ALERTING_WITHOUT_CLIP_PIC  0x33
#define PNCAP_LREP_3LINE_ALERTING             0x34
#define PNCAP_LREP_3LINE_AM_ALERTING          0x35
#define PNCAP_LREP_1LINE_IP_ALERTING          0x36
#define PNCAP_LREP_3LINE_IP_ALERTING          0x37
#define PNCAP_LREP_ESC_ENTRY_ID               0x38
#define PNCAP_LREP_CONFIRMATION               0x39
#define PNCAP_LREP_REJECTION                  0x3A
#define PNCAP_LREP_QUESTION                   0x3B
#define PNCAP_LREP_INFORMATION                0x3C
#define PNCAP_LREP_CONNECTION                 0x3D   // Connecting symbolizing the process of connection establishment
#define PNCAP_LREP_CON_ESTABLISHED            0x3E   // Connection Established
#define PNCAP_LREP_CON_REFUSED                0x3F   // Connection Refused
#define PNCAP_LREP_INTERNAL_ALERTING          0x40   // Internal Alerting
#define PNCAP_LREP_BUSY_INDICATOR             0x41   // Busy Indicator (animated sandglass/clock)
#define PNCAP_LREP_INTERNAL_CALL_WAITING      0x42   // Internal Call waiting
#define PNCAP_LREP_CLIP_PICTURE               0x43   // clip picture
#define PNCAP_LREP_MISSED_CALL                0x44   // missed call
#define PNCAP_LREP_ACCEPTED_CALL              0x45   // accepted call
#define PNCAP_LREP_OUTGOING_CALL              0x46   // outgoing call
#define PNCAP_LREP_AM_RECORD_CALL             0x47   // am recorded call
#define PNCAP_LREP_IP_CON_ESTABLISHED         0x48   // IP connection Established
#define PNCAP_LREP_HOME_NUMBER                0x49   // a symbol indicating that a number is the home number
#define PNCAP_LREP_OFFICE_NUMBER              0x4A   // a symbol indicating that a number is the office number
#define PNCAP_LREP_MOBILE_NUMBER              0x4B   // a symbol indicating that a number is the mobile number


/**************************************************************************/
/********            SUOTA Status data elements                     *******/
/********           (PNCAP_PIE_SUOTA_STATUS_DATA)                   *******/
/**************************************************************************/
// first byte of btmask SUOTA status data
#define PNCAP_SUOTA_STATUS_SLOT_STATUS_BYTE_NR   0x00    // Slot Status byte nr
// first byte of btmask DUOTA status data
#define PNCAP_SUOTA_STATUS_SLOT_MSK          0x07    // slotmask
#define PNCAP_SUOTA_STATUS_FULL_SLOT         0x01    // MASK : full slot
#define PNCAP_SUOTA_STATUS_LONG_SLOT         0x02    // MASK : long slot
#define PNCAP_SUOTA_STATUS_DOUBLE_SLOT       0x04    // MASK : double slot

/**************************************************************************/
/********            Application Class Elements                     *******/
/**************************************************************************/
#define PNCAP_AC_KEYP_DISP_NO                 0x00
#define PNCAP_AC_KEYP_DISP_NUM_ICON           0x01
#define PNCAP_AC_KEYP_DISP_GRAPH              0x10
#define PNCAP_AC_DATA                         0x20
#define PNCAP_AC_RNT                          0x40
#define PNCAP_AC_JOE_HS                       0x90
#define PNCAP_AC_SHERLOCK_HS                  0xA0
#define PNCAP_AC_EXTERNAL_LC                  0xB0
#define PNCAP_AC_VALENCIA                     0xC0
#define PNCAP_AC_PLATINUM                     0xC1
#define PNCAP_AC_ACCESSORIES                  0xD0      // Handsfree Device without display and keypad
/**************************************************************************/
/********            Edit Feature Elements                          *******/
/**************************************************************************/
#define PNCAP_EF_KEY_MULTI                    0x00

#define PNCAP_EF_EDIT_START_REQ               0x01
#define PNCAP_EF_EDIT_START_CFM               0x02
#define PNCAP_EF_EDIT_KEY_IND                 0x03
#define PNCAP_EF_EDIT_KEY_RESP                0x04
#define PNCAP_EF_EDIT_END_REQ                 0x05
#define PNCAP_EF_EDIT_END_CFM                 0x06


#define PNCAP_EF_EDIT_ECHO_ON                 0x01
#define PNCAP_EF_EDIT_ECHO_OFF                0x00

/**************************************************************************/
/********                  Keypadtype Elements                      *******/
/**************************************************************************/

#define PNCAP_KEYPAD_TYPE_REQ                 0x00
#define PNCAP_KEYPAD_TYPE_CFM                 0x01
#define PNCAP_KEYPAD_MODE_ADJUST              0x02
#define PNCAP_KEYPAD_MODE_ADJUST_ACCEPT       0x03

#define PNCAP_KP_STANDARD                     0x00
#define PNCAP_KP_MULTI                        0x01

/**************************************************************************/
/********    Mobilteil-Status   Bit 7     : Keypad Access-State************/
/********                       Bit 6     : Dial Status        ************/
/********                       Bit 5 - 0 : Sprache            ************/
/**************************************************************************/

#define PNCAP_PTSTAT_LANGUAGE_MASK            0x3F
#define PNCAP_PTSTAT_KEYPAD_ACCESS            0x80
#define PNCAP_PTSTAT_DIAL_STATUS              0x40

#define PNCAP_PTSTAT_LANG_UNDEF               0x00
#define PNCAP_PTSTAT_LANG_US                  0x01
#define PNCAP_PTSTAT_LANG_GER                 0x02
#define PNCAP_PTSTAT_LANG_INT                 0x03
#define PNCAP_PTSTAT_LANG_SPAIN               0x04
#define PNCAP_PTSTAT_LANG_PORTUG              0x05
#define PNCAP_PTSTAT_LANG_SCAND               0x06
#define PNCAP_PTSTAT_LANG_ITALY               0x07
#define PNCAP_PTSTAT_LANG_GREEC               0x08
#define PNCAP_PTSTAT_LANG_FRENCH              0x09
#define PNCAP_PTSTAT_LANG_NETHER              0x0a

#define PNCAP_PTSTAT_LANG_NORWEGIAN           0x0B
#define PNCAP_PTSTAT_LANG_DANISH              0x0C
#define PNCAP_PTSTAT_LANG_SWEDISH             0x0D
#define PNCAP_PTSTAT_LANG_FINNISH             0x0E
#define PNCAP_PTSTAT_LANG_CZECH               0x0F
#define PNCAP_PTSTAT_LANG_TURKISH             0x10
#define PNCAP_PTSTAT_LANG_POLISH              0x11
// fuer WDCT
#define PNCAP_PTSTAT_LANG_CANAD_FRNCH         0x12
#define PNCAP_PTSTAT_LANG_MEX_SPAIN           0x13
#define PNCAP_PTSTAT_LANG_BRAS_PORTUG         0x14

// G5000
#define PNCAP_PTSTAT_LANG_SLOVAKIAN           0x16
#define PNCAP_PTSTAT_LANG_RUSSIAN             0x17      // with Cyrillic characterset
#define PNCAP_PTSTAT_LANG_GREEK               0x18      // with Greek characterset
#define PNCAP_PTSTAT_LANG_HUNGARIAN           0x19
#define PNCAP_PTSTAT_LANG_CROATIAN            0x1A
#define PNCAP_PTSTAT_LANG_SLOVENIAN           0x1B

#define PNCAP_PTSTAT_LANG_ROMANIAN            0x1C
#define PNCAP_PTSTAT_LANG_SERBIAN             0x1D      // with Cyrillic characterset
#define PNCAP_PTSTAT_LANG_BULGARIAN           0x1E      // with Cyrillic characterset
#define PNCAP_PTSTAT_LANG_HEBREW              0x1F      // with Hebrew charset
#define PNCAP_PTSTAT_LANG_CHINESE             0x20      // with GB 2312 charset
#define PNCAP_PTSTAT_LANG_ARABIC              0x21      // with Arabic charset
#define PNCAP_PTSTAT_LANG_PERSIAN             0x22      // with Arabic charset
#define PNCAP_PTSTAT_LANG_TURKISH_TC          0x23      // with Turkish charset
#define PNCAP_PTSTAT_LANG_CATALAN             0x24
#define PNCAP_PTSTAT_LANG_BOSNIAN             0x25
#define PNCAP_PTSTAT_LANG_UKRAINIAN           0x26      // with Cyrillic characterset
#define PNCAP_PTSTAT_LANG_POLISH_L2           0x27      // with Latin 2 characterset
#define PNCAP_PTSTAT_LANG_HUNGARIAN_L2        0x28      // with Latin 2 characterset
#define PNCAP_PTSTAT_LANG_PERSIAN_FARSI       0x29      // with Farsi characterset
#define PNCAP_PTSTAT_LANG_CZECH_L2            0x2A      // with Latin 2 characterset
#define PNCAP_PTSTAT_LANG_SLOVENIAN_L2        0x2B      // with Latin 2 characterset
#define PNCAP_PTSTAT_LANG_CROATIAN_L2         0x2C      // with Latin 2 characterset
#define PNCAP_PTSTAT_LANG_ROMANIAN_L2         0x2D      // with Latin 2 characterset
#define PNCAP_PTSTAT_LANG_SLOVAKIAN_L2        0x2E      // with Latin 2 characterset
#define PNCAP_PTSTAT_LANG_BOSNIAN_L2          0x2F      // with Latin 2 characterset

// reserved #define 0x4x)

// extended information

// 2nd Byte
// Masks!
#define PNCAP_PTSTAT_EXT1_SIMPLE_MOD          0x00      //  simple mode
#define PNCAP_PTSTAT_EXT1_EXPERT_MOD          0x01      //  expert mode
#define PNCAP_PTSTAT_EXT1_NORMAL_FONT         0x00      //  normal font
#define PNCAP_PTSTAT_EXT1_LARGE_FONT          0x02      //  large font
#define PNCAP_PTSTAT_EXT1_GREGORIAN_CAL       0x00      //  gregorian calendar format (default)
#define PNCAP_PTSTAT_EXT1_FARSI_CAL           0x04      //  farsi calendar format

/**************************************************************************/
/* Display Request Mode */
/**************************************************************************/
#define PNCAP_DSP_REQ_MODE_PROP_FONT 1



/**************************************************************************/
/* Display Release Conditions                                             */
/**************************************************************************/
#define PNCAP_DSP_REL_COND_NO        0
#define PNCAP_DSP_REL_COND_CALL_REL  1
#define PNCAP_DSP_REL_KEEP_AS_SCRSV  0x02



// MSB des Language-byte sagt aus ob Basis DRUF kann oder nicht
#define PNCAP_ECE_DRUF_BIT                    0x80

/**************************************************************************/
/********              Dial-Control-Elements                        *******/
/**************************************************************************/

// Modes

#define PNCAP_DIAL_CTL_SINGLE_DIGIT           0x00
#define PNCAP_DIAL_CTL_MULTI_DIGIT            0x01
#define PNCAP_DIAL_CTL_SINGLE_CTLC            0x02
#define PNCAP_DIAL_CTL_MULTI_CTLC             0x03
#define PNCAP_DIAL_CTL_CONFIG                 0x04

// Mode Masks

#define PNCAP_DIAL_CTL_HN_SINGLE_DIGIT        0x00   // High Nibble Mask
#define PNCAP_DIAL_CTL_HN_MULTI_DIGIT         0x10   // High Nibble Mask
#define PNCAP_DIAL_CTL_HN_SINGLE_CTLC         0x20   // High Nibble Mask
#define PNCAP_DIAL_CTL_HN_MULTI_CTLC          0x30   // High Nibble Mask
#define PNCAP_DIAL_CTL_HN_CONFIG              0x40   // High Nibble Mask

// Control code mode <info elements>
// PNCAP_DIAL_CTL_SINGLE_CTLC  // PNCAP_DIAL_CTL_HN_SINGLE_CTLC
// PNCAP_DIAL_CTL_MULTI_CTLC   // PNCAP_DIAL_CTL_HN_MULTI_CTLC

#define PNCAP_DIAL_CTL_CTLC_RESET             0x00
#define PNCAP_DIAL_CTL_CTLC_BLINK             0x01
#define PNCAP_DIAL_CTL_CTLC_UPDATE_ON         0x02
#define PNCAP_DIAL_CTL_CTLC_UPDATE_OFF        0x03
#define PNCAP_DIAL_CTL_CTLC_BLINK_ALL         0x04
#define PNCAP_DIAL_CTL_CTLC_BLINK_OFF         0x05
#define PNCAP_DIAL_CTL_CTLC_NOBLINK_INC       0x06
#define PNCAP_DIAL_CTL_CTLC_NOBLINK_END       0x07

// Configuration mode <info elements>
// PNCAP_DIAL_CTL_CONFIG  // PNCAP_DIAL_CTL_HN_CONFIG

#define PNCAP_DIAL_CTL_CONFIG_FP_WRITE_2LINE  0x00
#define PNCAP_DIAL_CTL_CONFIG_PP_WRITE        0x01
#define PNCAP_DIAL_CTL_CONFIG_FP_WRITE_1LINE  0x02
#define PNCAP_DIAL_CTL_CONFIG_FP_WRITE_3LINE  0x03

/**************************************************************************/
/********              Local Function Elements                      *******/
/**************************************************************************/

/**************************************************************************/
/* Call Timer                                                             */
/**************************************************************************/
// Coding Local Function Call Timer
#define PNCAP_LFUNC_CALL_TIMER                0x00

// Call Timer (local call duration timer)
#define PNCAP_LFUNC_CALL_TIMER_STOP           0x00
#define PNCAP_LFUNC_CALL_TIMER_START          0x01
#define PNCAP_LFUNC_CALL_TIMER_DISP_OFF       0x02   // Fade out local call duration timer
#define PNCAP_LFUNC_CALL_TIMER_DISP_ON        0x03   // Fade in local call duration timer

/**************************************************************************/
/* LNR                                                                    */
/**************************************************************************/

// Coding Local Function Call LNR
#define PNCAP_LFUNC_CALL_LNR                  0x01

/**************************************************************************/
/* CPN                                                                    */
/**************************************************************************/
// Coding Local Function Call CPN
#define PNCAP_LFUNC_CPN                       0x02

// Parameter 1 fuer CPN
#define PNCAP_LFUNC_CALL_FADE_OUT_CPN         0x00
#define PNCAP_LFUNC_CALL_FADE_IN_CPN          0x01

// Paramter 3 CPN (Presentation-Modes)
#define PNCAP_CPN_MODE_1LINE                  0x00
#define PNCAP_CPN_MODE_2LINE                  0x01
#define PNCAP_CPN_MODE_3LINE                  0x02

// Paramter 4 Frame
#define PNCAP_CPN_MODE_WITHOUT_FRAME          0x00
#define PNCAP_CPN_MODE_WITH_FRAME             0x01


/**************************************************************************/
/* MUTE                                                                   */
/**************************************************************************/

#define PNCAP_LFUNC_MUTE                      0x05

// Parameter
#define PNCAP_LF_MUTE_OFF                     0x00
#define PNCAP_LF_MUTE_ON                      0x01


/**************************************************************************/
/* Call-Mode                                                              */
/**************************************************************************/

#define PNCAP_LFUNC_CALL_MODE                 0x09
// Parameter Call-Mode
#define PNCAP_LFUNC_CM_HANDSET                0x01   // Handset mode  (micro and earpiece)
#define PNCAP_LFUNC_CM_HANDSFREE              0x02   // Handsfree mode (Handsfree micro and speaker)
#define PNCAP_LFUNC_CM_UNFILTERED_HS          0x03   // handset optimized for recording in base (e.g. OGM for AM) (implementation dependent on HW-platform (see PICS))
#define PNCAP_LFUNC_CM_UNFILTERED_HF          0x04   // handsfree optimized for recording in base (implementation dependent on HW-platform (see PICS))
#define PNCAP_LFUNC_CM_IDLE                   0x05   // no call mode
#define PNCAP_LFUNC_CM_HANDSFREE_ALERTING     0x06   // Handsfree mode (Speaker-LED signals <<Alerting>>)

/**************************************************************************/
/* Coding Local Function Call CNIP                                        */
/**************************************************************************/

#define PNCAP_LFUNC_CNIP                      0x0A

// Parameter 1 CPN
#define PNCAP_LFUNC_CALL_FADE_OUT_CNIP        0x00
#define PNCAP_LFUNC_CALL_FADE_IN_CNIP         0x01

// Paramter 3 Presentation mode
#define PNCAP_CNIP_MODE_1LINE                 0x00
#define PNCAP_CNIP_MODE_2LINE                 0x01
#define PNCAP_CNIP_MODE_3LINE                 0x02

// Paramter 4 Frame
#define PNCAP_CNIP_MODE_WITHOUT_FRAME         0x00
#define PNCAP_CNIP_MODE_WITH_FRAME            0x01

/**************************************************************************/
/* Coding Local Function melody volume                                    */
/**************************************************************************/

#define PNCAP_LFUNC_MELODY_VOLUME             0x0B

// Parameter 1
#define PNCAP_LFUNC_MELV_MIN_VOL              0x00      // minimum volume
#define PNCAP_LFUNC_MELV_DEC_VOL              0x01      // decrease volume
#define PNCAP_LFUNC_MELV_INC_VOL              0x02      // increase volume
#define PNCAP_LFUNC_MELV_MAX_VOL              0x03      // increase volume

/**************************************************************************/
/* Coding Local Function call status display                              */
/**************************************************************************/

#define PNCAP_LFUNC_CALL_STATE_DISPLAY        0x0C

#define PNCAP_LFUNC_CALL_STATE_DSP_OFF             0x00     //  off
#define PNCAP_LFUNC_CALL_STATE_DSP_ALERTING        0x01     //  alerting
#define PNCAP_LFUNC_CALL_STATE_DSP_AM_ALERTING     0x02     //  AM alerting
#define PNCAP_LFUNC_CALL_STATE_DSP_IP_ALERTING     0x03     //  IP alerting
#define PNCAP_LFUNC_CALL_STATE_DSP_CLIP_P_ALERTING 0x04     //  CLIP-Picture-Alerting
#define PNCAP_LFUNC_CALL_STATE_DSP_CALL_DELIVERED  0x05     //  call delivered
#define PNCAP_LFUNC_CALL_STATE_DSP_CONNECTED       0x06     //  connected
#define PNCAP_LFUNC_CALL_STATE_DSP_RELEASED        0x07     //  released
#define PNCAP_LFUNC_CALL_STATE_DSP_NEUTRAL         0x08     //  neutral (empty)
#define PNCAP_LFUNC_CALL_STATE_DSP_INT_ALERTING    0x09     //  Internal alerting

/**************************************************************************/
/* Coding Local Function call charges                                     */
/**************************************************************************/

#define PNCAP_LFUNC_CALL_CHARGES              0x0D

/**************************************************************************/
/* Coding Local Function receiver volume                                  */
/**************************************************************************/

#define PNCAP_LFUNC_REC_VOLUME                0x0E

#define PNCAP_LFUNC_REC_VOL_MIN               0x00 // 0: minimum volume
#define PNCAP_LFUNC_REC_VOL_DEC               0x01 // 1: decrease volume
#define PNCAP_LFUNC_REC_VOL_INC               0x02 // 2: increase volume
#define PNCAP_LFUNC_REC_VOL_MAX               0x03 // 3: maximum volume

/**************************************************************************/
/* Key Request Elements                                                   */
/**************************************************************************/
#define PNCAP_KR_SK_ONLY    0x01     /* Anforderung Softkeys                */
#define PNCAP_KR_MK_ONLY    0x02     /* Anforderung Menukey                 */
#define PNCAP_KR_SK_AND_MK  0x03     /* Anforderung alle Soft- und Menukeys */
#define PNCAP_KR_FURTHER_SK 0x05     /* Anforderung weiterer Softkeys       */
#define PNCAP_KR_SK_CAPABILITIES   0x08  /* Anforderung Softkeycapabilities */
#define PNCAP_KR_PHONE_BOOK_KEY    0x09  /* Anforderung Telefonbuchtaste    */
#define PNCAP_KR_NET_LIST_KEY      0x0A  /* Anforderung Net-List-Taste      */
#define PNCAP_KR_TUMBLE_PAD        0x0B  /* Anforderung Taumelscheibe       */
#define PNCAP_KR_MWI_KEY           0x0C  /* Anforderung MWI-Taste           */
#define PNCAP_KR_JOE_HS_SK         0x0D  /* Anforderung Joe-Hs-Sotfkeys     */
#define PNCAP_KR_ON_HOOK_KEY       0x0E  /* Anforderung On-Hook-Taste       */
#define PNCAP_KR_T_SERVICE_KEY     0x0F  /* Anforderung T-Service-Taste     */
#define PNCAP_KR_PUSH_TO_TALK_KEY  0x10  /* Anforderung Push-To-Talk-Taste  */
// #define 0x11  reserved for Home-RF
// #define 0x12  reserved for Home-RF
#define PNCAP_KR_CALL_MODE_KEYS    0x13
#define PNCAP_KR_PLUS_MINUS        0x14
#define PNCAP_KR_TMP_ALL_KEYS      0x15
#define PNCAP_KR_OFF_CHARGER       0x16
#define PNCAP_KR_VOLUME_KEY        0x17
#define PNCAP_KR_CENTER_KEY        0x18
#define PNCAP_KR_OH_SP_KEY         0x19
#define PNCAP_KR_VALENCIA_KEYS     0x1A   // request of Valencia keys
#define PNCAP_KR_NETWORK_DIR_KEY   0x1B   // request of network directory key
#define PNCAP_KR_MUTE_KEY          0x1C   // request of mute key
#define PNCAP_KR_LP_TUMBLE_PAD_L   0x1D   // request for longpress tumble pad left
#define PNCAP_KR_REDIAL_KEY        0x1E   // request for redial key
#define PNCAP_KR_AM_KEYS           0x1F   // request for AM keys (AM key, Play key,Skip Back key long press, Skip Backkey short press)
#define PNCAP_KR_CMB_SEND_HF       0x20   // request for combined Send and Handsfree key
#define PNCAP_KR_REPERTORY_KEYS    0x21   // request for repertory keys 1-4
#define PNCAP_KR_REC_ON_HOOK_KEY   0x22   // request receiver on hook key
#define PNCAP_KR_SEND_CON_KEY      0x23   // request for send connection key
#define PNCAP_KR_LINE_SEL_KEY      0x24   // request for line selection key

/**************************************************************************/
/* Local Key Evaluation Elements                                          */
/**************************************************************************/
#define PNCAP_LKE_VALCODE_FROMNOW  0x00  // Gueltig von jetzt an
#define PNCAP_LKE_VALCODE_KEYPRESS 0x01  // Gueltig nur fuer einen Tastendruck

/**************************************************************************/
/* Softkey-Capabilities Elements                                          */
/**************************************************************************/
#define PNCAP_SKC_GROUP                 0xF0  // softkey group, constant group
#define PNCAP_SKC_CLASS_SINGLE_SK       0x00  // softkey class: Single standard softkey
#define PNCAP_SKC_CLASS_ROCKER_SK       0x01  // softkey class: combined rocker key (containing several switches)
#define PNCAP_SKC_CLASS_SINGLE_ICON_SK  0x02  // softkey class:single soft key to be labelled via �set icon� control code

/**************************************************************************/
/********              PT-Capabilies-Elements                       *******/
/**************************************************************************/
// Masken!
// Byte 1
#define PNCAP_PTCAP_FLEXIBLE_SKEY_ED_SKEY_CAP_EXCHG 0x01
#define PNCAP_PTCAP_LOCAL_CLIP                      0x02
#define PNCAP_PTCAP_DATA_LIST_EXCHG                 0x04
#define PNCAP_PTCAP_CLIP_EXCHG                      0x08
#define PNCAP_PTCAP_PHONE_BOOK_KEY_REQ              0x10
#define PNCAP_PTCAP_FT_ACCESS_OF_PT_DEF_TEXTS       0x20
#define PNCAP_PTCAP_ONE_LINE_CALL_TIMER             0x40

// Byte 2
#define PNCAP_PTCAP_VIRTUAL_CALL                    0x01
//#define PNCAP_PTCAP_NET_LIST_KEY_REQ                0x02
#define PNCAP_PTCAP_T_SERVICE_KEY_REQ               0x02   // Codings f�r Net-Key und T-Key vertauscht
#define PNCAP_PTCAP_TUMBLE_PAD_KEY_REQ              0x04
#define PNCAP_PTCAP_HANDSFREE                       0x08
#define PNCAP_PTCAP_MWI_KEY_REQ                     0x10
#define PNCAP_PTCAP_ON_HOOK_KEY_REQ                 0x20
#define PNCAP_PTCAP_AUTOMATIC_CALL_ACCEPTION        0x40
#define PNCAP_PTCAP_JOEHS_SOFTKEYS                  0x80

// Byte 3
#define PNCAP_PTCAP_PUSH_TO_TALK_KEY_REQ            0x01
//#define PNCAP_PTCAP_T_SERVICE_KEY_REQ               0x02
#define PNCAP_PTCAP_NET_LIST_KEY_REQ                0x02   // Codings f�r Net-Key und T-Key vertauscht
#define PNCAP_PTCAP_AUTOMATIC_REDIAL                0x04
#define PNCAP_PTCAP_CISS                            0x08
#define PNCAP_PTCAP_EXTENDED_DISPLAY_REQ            0x10
#define PNCAP_PTCAP_DATA_LINK_TOPIC_READ            0x20
#define PNCAP_PTCAP_DATA_LINK_TOPIC_DELETE          0x40
#define PNCAP_PTCAP_SYSTEM_DATE_AND_TIME            0x80

// Byte 4
#define PNCAP_PTCAP_LOCAL_EDIT_WINDOW               0x01
#define PNCAP_PTCAP_DISLAY_SHORT_CUT_LABEL          0x02
#define PNCAP_PTCAP_INT_NUMBER_AND_NAME             0x04
#define PNCAP_PTCAP_STATUS_ANNOUNCEMENT             0x08
#define PNCAP_PTCAP_COMBINED_KEY_REQ                0x10
#define PNCAP_PTCAP_PP_SUPPORTS_FP_FEATURE_ACCESS   0x20
#define PNCAP_PTCAP_PP_SUPPORTS_RELEASE_TIMEOUT     0x40   // lokale Nachlaufzeit bei Ausl�sen implementiert
#define PNCAP_PTCAP_PP_SUPPORTS_FULL_VR_DIR_FUNC    0x80   // korrekte Behandlung des zentralen Telefonbuchs

// Byte 5
#define PNCAP_PTCAP_PP_SUPPORTS_SMS_TI              0x01   // Unterstuetzung Telecom Italia SMS protocol
// reserved
#define PNCAP_PTCAP_PP_SUPPORTS_LF_HOLD_RESUME      0x04   // Unterstuetzung Local Feature hold/resume
#define PNCAP_PTCAP_PP_SUPPORTS_REDUCED_ASCII       0x08   // Unterstuetzung 'Reduced Ascii' keypad character set
// reserved

// Byte 6
#define PNCAP_PTCAP_PP_SUPPORTS_CONCAT_SMS          0x01   // PT supports Concatenated SMS
#define PNCAP_PTCAP_PP_SUPPORTS_ACA_CFM             0x02   // Unterstuetzung Automatic Call Acception CFM
#define PNCAP_PTCAP_PP_SUPPORTS_CF_CHANNEL          0x04   // Unterstuetzung Cf-Channel
#define PNCAP_PTCAP_PP_SUPPORTS_GREEK_CHARSET       0x08   // Unterstuetzung Greek Characterset in address room G1
#define PNCAP_PTCAP_PP_SUPPORTS_CYRILLIC_CHARSET    0x10   // Unterstuetzung Cyrillic Characterset in address room G1
#define PNCAP_PTCAP_PP_SUPPORTS_CALL_MODE_KEY_REQ   0x20   // Unterstuetzung Cyrillic Characterset in address room G1
#define PNCAP_PTCAP_PP_SUPPORTS_SWITCH_ECHOSUP_OFF  0x40   // PT supports switching echo supressor off in FP during handsfree
#define PNCAP_PTCAP_PP_SUPPORTS_DFPAR_CHARSET_ACT   0x80   // Pp supports Characterset Activation in Paragraph

// Byte 7
#define PNCAP_PTCAP_PP_SUPPORTS_DPS                 0x01   // Pp supports DECT Positioning System
#define PNCAP_PTCAP_PP_SUPPORTS_PLMI_KEY_REQ        0x02   // Pp supports +/- Key Req
#define PNCAP_PTCAP_PP_SUPPORTS_EXT_CISS_ACK        0x04   // Pp supports Extended Ciss-Acknowlegement
#define PNCAP_PTCAP_PP_SUPPORTS_PT_STAT_REQ         0x08   // Pp supports Request for PT Status
#define PNCAP_PTCAP_PP_SUPPORTS_ALLKEYS_TMPKEY_REQ  0x10   // Pp supports temporary request for all keys
#define PNCAP_PTCAP_PP_SUPPORTS_PNCAP_T_CHAN        0x20   // Pp supports PNCAP Transparent Channel
#define PNCAP_PTCAP_PP_SUPPORTS_VOL_KEY_REQ         0x40   // Pp supports Request for Volume Key
#define PNCAP_PTCAP_PP_SUPPORTS_OFF_CHARG_EV_REQ    0x80   // Pp supports Request Off Charger Event

// Byte 8
#define PNCAP_PTCAP_PP_SUPPORTS_DATA_INTERW_NONSPEC 0x01   // Pp supports DECT data interworking, no specific interworking (transparent)
#define PNCAP_PTCAP_PP_SUPPORTS_DATA_INTERW_M101V24 0x02   // Pp supports DECT data interworking, M101-V24 interworking
#define PNCAP_PTCAP_PP_SUPPORTS_UPLANE_CONT_PPPHDLC 0x04   // Pp supports DECT U-plane content, PPP-HDLC like framing
#define PNCAP_PTCAP_PP_SUPPORTS_UPLANE_CONT_64KBIT  0x08   // Pp supports DECT U-plane content, 64 kbit/s synchron
#define PNCAP_PTCAP_PP_SUPPORTS_CONNCTL_AT_UPLANE   0x10   // Pp supports External connection control, AT-command via U-plane
#define PNCAP_PTCAP_PP_SUPPORTS_CONNCTL_KEYP_CPLANE 0x20   // Pp supports External connection control, keypad/PNCAP via C-plane
#define PNCAP_PTCAP_PP_SUPPORTS_PERM_ACT_OF_HEBREW  0x40   // Pp supports PT supports permanent activation of Hebrew Characterset in address room G1
#define PNCAP_PTCAP_PP_SUPPORTS_CENTER_KEY_REQ      0x80

// Byte 9
#define PNCAP_PTCAP_PP_SUPPORTS_ALT_OH_SP_KEY_REQ   0x01   // Pp supports request for alternative off-hook and speaker key
#define PNCAP_PTCAP_PP_SUPPORTS_EXC_OF_EXT_CAP      0x02   // Pp supports exchange of extended capabilities
#define PNCAP_PTCAP_PP_SUPPORTS_CHAR_SET_GB2312     0x04   // Pp supports escape to two byte character set GB 2312 (chinese)
#define PNCAP_PTCAP_PP_SUPPORTS_ACCESS_AREA_CODES   0x08   // Pp supports exchange of access code and area code
#define PNCAP_PTCAP_PP_SUPPORTS_ARABIC_CHARSET      0x10   // Pp supports permanent activation of Arabic characterset
#define PNCAP_PTCAP_PP_SUPPORTS_EXCH_INVENTORY_DATA 0x20   // Pp supports exchange of inventory data
#define PNCAP_PTCAP_PP_SUPPORTS_BGR_PROC            0x40   // Pp supports B(erufs)G(enossenschafts)R(ichtlinie) procedures
#define PNCAP_PTCAP_PP_SUPPORTS_DISTRIBUTED_COM     0x80   // Pp supports DECT Distributed Communication

// Byte 10
#define PNCAP_PTCAP_PP_SUPPORTS_DEVIATIONCONTROL    0x01   // Pp supports deviation control (that is switching echo supressor off in FP )
#define PNCAP_PTCAP_PP_SUPPORTS_CC_ALERTING_TONES   0x02   // Pp supports output of tones in call control state alerting
#define PNCAP_PTCAP_PP_SUPPORTS_TIH_SCENARIOS       0x04   // Pp supports TIH scenarios
#define PNCAP_PTCAP_PP_SUPPORTS_TIH_CALL_FWRD_NO    0x08   // Pp supports TIH Call forward number
#define PNCAP_PTCAP_PP_SUPPORTS_TIH_PIN             0x10   // Pp supports TIH PIN
#define PNCAP_PTCAP_PP_SUPPORTS_ICON_LIST_EXCH      0x20   // Pp supports Icon list exchange
#define PNCAP_PTCAP_PP_SUPPORTS_PP_CFG_REQ          0x40   // Pp supports PP configuration request
#define PNCAP_PTCAP_PP_SUPPORTS_CC_COLOUR_CFG       0x80   // Pp supports Online Colour Scheme switch

// Byte 11
#define PNCAP_PTCAP_PP_SUPPORTS_REPERTORY_KEYS      0x01   // Pp supports repertory keys
#define PNCAP_PTCAP_PP_SUPPORTS_VALENCIA_HS_KEYS    0x02   // Pp supports Valencia Hs keys
#define PNCAP_PTCAP_PP_SUPPORTS_NETWORK_DIR_KEY     0x04   // Pp supports Network directory key
#define PNCAP_PTCAP_PP_SUPPORTS_PROP_WIDEBAND       0x08   // Pp supports proprietary Wideband
#define PNCAP_PTCAP_PP_SUPPORTS_TURKISH_CHARSET     0x10   // Pp supports Turkish characterset
#define PNCAP_PTCAP_PP_SUPPORTS_PROLONGED_REGMOD    0x20   // Pp supports prolonged registration mode

/**************************************************************************/
/*        Melody Id                                                       */
/**************************************************************************/
#define PNCAP_MEL_INTEGRATED_HOME                   0xFE
#define PNCAP_MEL_LOCAL                             0xFF

/**************************************************************************/
/* FT-Capabilities Information Elements                                   */
/**************************************************************************/
// First Information Byte
#define PNCAP_FT_CAP_FT_BASED_MACRO_LIST             0x01
#define PNCAP_FT_CAP_CF_CHANNEL_SUPPORT              0x02
#define PNCAP_FT_CAP_GREEK_CHARSET_SUPPORT           0x04
#define PNCAP_FT_CAP_RUSSIAN_CHARSET_SUPPORT         0x08
#define PNCAP_FT_CAP_MMS_INBOX_ACC_SUPPORT           0x10
#define PNCAP_FT_CAP_DATA_MODE_REG_SUPPORT           0x20
#define PNCAP_FT_CAP_REG_REQ_SUPPORT                 0x40
#define PNCAP_FT_CAP_DB_ALERT_OFF_SUPPORT            0x80
#define PNCAP_FT_CAP_SUOTA                           0x10   // FT supports proprietary SUOTA

// Second Information Byte
#define PNCAP_FT_CAP_EXT_CISS_ACK                    0x01
#define PNCAP_FT_CAP_PNCAP_T_CHAN                    0x02
#define PNCAP_FT_CAP_SUBSCR_LIST_EXCH                0x04
#define PNCAP_FT_CAP_MSN_ID                          0x08
#define PNCAP_FT_CAP_DATA_INTERW_NONSPEC             0x10   // FT supports DECT data interworking, no specific interworking (transparent)
#define PNCAP_FT_CAP_DATA_INTERW_M101V24             0x20   // FT supports DECT data interworking, M101-V24 interworking
#define PNCAP_FT_CAP_UPLANE_CONT_PPPHDLC             0x40   // FT supports DECT U-plane content, PPP-HDLC like framing
#define PNCAP_FT_CAP_UPLANE_CONT_64KBIT              0x80   // FT supports DECT U-plane content, 64 kbit/s synchron

// Third Information Byte
#define PNCAP_FT_CAP_CONNCTL_AT_UPLANE               0x01   // FT supports External connection control, AT-command via U-plane
#define PNCAP_FT_CAP_CONNCTL_KEYP_CPLANE             0x02   // FT supports External connection control, keypad/PNCAP via C-pla
#define PNCAP_FT_CAP_PROPFONT_REQ_SUPPPORT           0x04
#define PNCAP_FT_CAP_HEBREW_CHARSET_SUPPORT          0x08
#define PNCAP_FT_CAP_EMAILINBOX_SUPPORT              0x10
#define PNCAP_FT_CAP_AREA_CODES_SUPPORT              0x20
#define PNCAP_FT_CAP_CHAR_SET_GB2312_SUPPORT         0x40
#define PNCAP_FT_CAP_CHAR_SET_ARABIC_SUPPORT         0x80

// Fourth Information Byte
#define PNCAP_FT_CAP_CALL_CONTROL_REQ                0x01   // FT supports call control request
#define PNCAP_FT_CAP_FP_SETTINGS_EXCH                0x02   // FT supports FP settings exchange
#define PNCAP_FT_CAP_REPERTORY_KEYS                  0x04   // FT supports repertory keys
#define PNCAP_FT_CAP_INTEGRATED_HOME_APPL            0x08   // FT supports integrated home applications
#define PNCAP_FT_CAP_INTEGRATED_HOME_SCNENARIO       0x10   // FT supports integrated home scenarions
#define PNCAP_FT_CAP_IDLE_SK_PROGRAMMING             0x20   // FT supports idle softkey programming
#define PNCAP_FT_CAP_PCM_MODE_SWITCHING              0x40   // FT supports switching of PCM-Mode
#define PNCAP_FT_CAP_PROPRIETARY_WIDEBAND            0x80   // FT supports proprietary wideband

// Fifth Information Byte
#define PNCAP_FT_CAP_SCENARIO_NAMES_EXCH             0x01   // FT supports scenario names exchange
#define PNCAP_FT_CAP_VOIP                            0x02   // FT supports VoIP
#define PNCAP_FT_CAP_TURKISH_CHARSET_SUPPORT         0x04   // FT supports Turkish characterset
#define PNCAP_FT_CAP_SCRSVTXT                        0x08   // FT supports Status Announcement Ready to Receive ScreenSaverText
#define PNCAP_FT_CAP_LINE_ID                         0x10   // FT supports line Id
#define PNCAP_FT_CAP_LIVTCTXT                        0x20   // FT supports Status Announcement Ready to Live Ticker Text
#define PNCAP_FT_CAP_LIST_UPDATE_AVAIL_SUPP          0x40   // FT supports List Update Available
#define PNCAP_FT_CAP_MISSED_APPOINT_SUPP             0x80   // FT supports MWI Status Missed Appointment

// Sixth Information Byte
#define PNCAP_FT_CAP_EXT_PT_STATUS_SUPP              0x01   // FT supports extended PT Status information
#define PNCAP_FT_CAP_IP_ISS                          0x02   // FT supports ready to receive IP-Infoservice-Slideshow
#define PNCAP_FT_CAP_CALLED_PARTY_NO_FOR_URIDIAL     0x04   // FT supports the IE <<CALLED PARTY NUMBER>> for URI dialling
#define PNCAP_FT_CAP_EXT_CLIP_NAME                   0x08   // FT supports extended text length in PIE<<Name of calling party>>'
#define PNCAP_FT_CAP_AUTHENTICATION_OF_FT            0x10   // FT supports �authentication of FT�
#define PNCAP_FT_CAP_LATIN2_CHARSET_SUPPORT          0x20   // FT supports Latin2 characterset
#define PNCAP_FT_CAP_PT_STATUS_CISS                  0x40   // FT supports receiving PT Status in a CISS-message
#define PNCAP_FT_CAP_TRANSPARENT_CHANNEL_IND         0x80   // FT supports sending of PIE <<Transparent channel ind>>

// Seventh Information Byte
#define PNCAP_FT_CAP_FARSI_CHARSET_SUPPORT           0x01   // FT supports Farsi characterset
#define PNCAP_FT_CAP_DF_CC_PROC_ALERT                0x02   // FT supports escape to data feature in the cc-states call proceeding and alerting
#define PNCAP_FT_CAP_ANCMT_CALL_STATE_ACTIVE         0x04   // FT supports announcements of the call state �active� via PIE <<Call State>>
#define PNCAP_FT_CAP_MULTIPLE_EC_CFMS                0x08   // FT supports multiple PNCAP_PIE_EXTENDED_CAP_CFMs in response to single PNCAP_PIE_EXTENDED_CAP_REQ
#define PNCAP_FT_CAP_SUPPORTS_PROPR_SUOTA            0x10   // FT supports proprietary SUOTA

/**************************************************************************/
/* Direct-Access-Type Information Elements                                */
/**************************************************************************/
//
#define PNCAP_DIR_ACC_TYPE_FT_BASED_MACRO_LIST       0x01


/**************************************************************************/
/* Text-Group-Identifier                                                  */
/**************************************************************************/
#define PNCAP_TEXT_GROUP_TNET_SHORTCUTS              0x01
#define PNCAP_TEXT_GROUP_SET_PRC                     0x02
#define PNCAP_TEXT_GROUP_BASE_SHORTCUTS              0x03
#define PNCAP_TEXT_GROUP_JOE_HS_MENUS                0x04
#define PNCAP_TEXT_GROUP_SHERLOCK_HS_MENUS           0x05
#define PNCAP_TEXT_GROUP_FP_FEATURE_REL_TEXTS        0x06
#define PNCAP_TEXT_GROUP_SERVICE_DATA                0x07

/**************************************************************************/
/* Text-Group Joe Hs Text Identifier                                      */
/**************************************************************************/
#define PNCAP_TXT_GR_JOE_M_NETFT                0x01
#define PNCAP_TXT_GR_JOE_M_CHARGES              0x02
#define PNCAP_TXT_GR_JOE_M_BS_SETTINGS          0x03
#define PNCAP_TXT_GR_JOE_M_AM_SETTINGS          0x04
#define PNCAP_TXT_GR_JOE_M_MENU_END             0x05

/**************************************************************************/
/* Text-Group-Sherlock Hs Text Identifier                                 */
/**************************************************************************/
#define PNCAP_TXT_GR_SHERLOCK_M_MESSAGES             0x01
#define PNCAP_TXT_GR_SHERLOCK_M_NETFT                0x02
#define PNCAP_TXT_GR_SHERLOCK_M_CHARGES              0x03
#define PNCAP_TXT_GR_SHERLOCK_M_BS_SETTINGS          0x04
#define PNCAP_TXT_GR_SHERLOCK_M_AM_SETTINGS          0x05
#define PNCAP_TXT_GR_SHERLOCK_M_AUTOMATIC_RDL        0x06
#define PNCAP_TXT_GR_SHERLOCK_M_MENU_END             0x07

/**************************************************************************/
/* Text-Group-Service Text Identifier                                     */
/**************************************************************************/
#define PNCAP_TXT_GR_SERVICE_LINE3_BATTERY_DATA            0x01
#define PNCAP_TXT_GR_SERVICE_LINE5_RXINT_RPN_DATA          0x02

/**************************************************************************/
/********              Stop Redial Buffer Store Reason                    */
/**************************************************************************/
#define PNCAP_STOP_LNR_STORE_REASON_UNDEF            0x00
#define PNCAP_STOP_LNR_STORE_REASON_MACRO_START      0x01

/**************************************************************************/
/********              Negativ Acknowledge                          *******/
/**************************************************************************/
#define PNCAP_NEG_ACK_PT_STAT_LANGUAGE               0x01
/**************************************************************************/
/********              Terminal-Portability-Elements                *******/
/**************************************************************************/
#define PNCAP_TP_MSK                                 0xe0

#define PNCAP_TP_SUSPEND_REQ                         0x00
#define PNCAP_TP_SUSPEND                             0x20
#define PNCAP_TP_SUSPEND_OK                          0x40
#define PNCAP_TP_RESUME                              0x60

/**************************************************************************/
/********               Datatransfer -Elements                      *******/
/**************************************************************************/

/**************    Codes for Datatransfer   **************/
#define PNCAP_DF_LINK_REQ                            0x01 // Datenlink - Request
#define PNCAP_DF_LINK_CNF                            0x02 // Datenlink - Confirm
#define PNCAP_DF_DATA_PAC                            0x03 // Datensatz - Paket
#define PNCAP_DF_LINK_REL                            0x04 // Datenlink - Release
#define PNCAP_DF_LINK_PROC                           0x05 // Datenlink - Proceed


/**************     Communication-Theme     **************/
#define PNCAP_DF_CT_PHONEBOOK_LIST_WRITE             0x10 // Telefonbuch schreiben
#define PNCAP_DF_CT_NET_PROV_LIST_WRITE              0x11 // Netzanbieter-Liste schreiben
//#define PNCAP_DF_CT_MAKRO_LIST                       0x30 // Makro-Liste allg.
//#define PNCAP_DF_CT_TNET_LIST                        0x31 // TNet - MakroListe
#define PNCAP_DF_CT_PHONEBOOK_LIST_READ              0x40 // Telefonbuch lesen
#define PNCAP_DF_CT_NET_PROV_LIST_READ               0x41 // Netzanbieter-Liste lesen
#define PNCAP_DF_CT_T_KEY_LIST_READ                  0x42 // T-Tasten-Liste lesen
#define PNCAP_DF_CT_PHONEBOOK_LIST_DELETE            0x50 // Telefonbuch loeschen
#define PNCAP_DF_CT_NET_PROV_LIST_DELETE             0x51 // Netzanbieter-Liste loeschen
#define PNCAP_DF_CT_T_KEY_LIST_DELETE                0x52 // T-Tasten-Liste loeschen
#define PNCAP_DF_CT_SMS_WRITE                        0x60 // SMS-schreiben
#define PNCAP_DF_CT_SMS_READ                         0x61 // SMS-lesen
#define PNCAP_DF_CT_MMS_SC_NR_WRITE                  0x70 // MMS-Service-Center Nr write
#define PNCAP_DF_CT_MMS_SC_NR_READ                   0x71 // MMS-Service-Center Nr read
#define PNCAP_DF_CT_OBEX_TRANSFER                    0x80 // OBEX-Session for V-Card
#define PNCAP_DF_CT_SMS_HEADER_READ                  0x90 // exchange of SMS-Header informations
#define PNCAP_DF_CT_BINARY_DATA_WRITE                0xA0 // write binary data

/**************       Confirm - Infos       **************/
#define PNCAP_DF_CNF_UNKNOWN                         0x00 // Fehler unbekannt
#define PNCAP_DF_CNF_MEM_FULL                        0x01 // Fehler wegen "Speicher voll"
#define PNCAP_DF_CNF_PARTNER_ERR                     0x02 // Teilnehmer nicht vorhanden
#define PNCAP_DF_CNF_COMM_THEME_ERR                  0x03 // Komm.Thema nicht unterstuetzt
#define PNCAP_DF_CNF_BASIS_OVERFLOW                  0x04 // Basis ueberlastet
#define PNCAP_DF_CNF_WRONG_PIN                       0x05 // falsche PIN
#define PNCAP_DF_CNF_PARTNER_DISC                    0x06 // Empfaenger loest Verbindung aus

#define PNCAP_DF_CNF_ACCEPT                          0x80 // Accept-Flag 1, alles OK
#define PNCAP_DF_CNF_NOT_ACCEPT                      0x00 // Accept-Flag 0, siehe Info
#define PNCAP_DF_CNF_INFO_MASK                       0x7F // data link confirm: confirm info

#define PNCAP_DF_CNF_SUP_DATA                        0x01 // DataLinkConfirm Par: Rec. supports Date of Birth/VIP
#define PNCAP_DF_CNF_ESC_CHARSET                     0x80 // DataLinkConfirm Par: Rec. supports characterset EscSeq
#define PNCAP_DF_CNF_CHARSET_CYRILLIC                0x01 // DataLinkConfirm Par: Rec. supports Cyrillic characterset
#define PNCAP_DF_CNF_CHARSET_GREEK                   0x02 // DataLinkConfirm Par: Rec. supports Greek characterset
#define PNCAP_DF_CNF_CHARSET_HEBREW                  0x04 // DataLinkConfirm Par: Rec. supports Hebrew characterset
#define PNCAP_DF_CNF_CHARSET_ARABIC                  0x08 // DataLinkConfirm Par: Rec. supports Arabic characterset
#define PNCAP_DF_CNF_CHARSET_CHINESE_GB2312          0x10 // DataLinkConfirm Par: Rec. supports Chinese GB2312 characterset
#define PNCAP_DF_CNF_CHARSET_TURKISH                 0x20 // DataLinkConfirm Par: Rec. supports Turkish characterset
#define PNCAP_DF_CNF_CHARSET_LATIN2                  0x40 // DataLinkConfirm Par: Rec. supports Latin 2 characterset

// Byte 7
#define PNCAP_DF_CNF_EXT_SUP_DATA                    0x01 // DataLinkConfirm Par: Rec. supports extended SupData
#define PNCAP_DF_CNF_EXT_SUP_DATA_VCARD              0x02 // DataLinkConfirm Par: Rec. supp. Ext.SupData Field vCard

// Byte 8 extended characterset support:
#define PNCAP_DF_CNF_CHARSET_FARSI                   0x01 // DataLinkConfirm Par: Rec. supports Farsi characterset

/**************       Binary Data Write Dataformat    **************/
#define PNCAP_DF_BD_JPEG                             0x01 // Data content format jpeg


/* max. Laenge der zu sendenden Datensaetze:                             */
/* Max PIE-Laenge - PIE_ID - PIE_LEN - LEN - ucActPar - ucRestFrag = 52 */
#define PNCAP_DF_MAX_DATA_BYTES               52
#define PNCAP_DF_MAX_DATA_BYTES_G4            48

/**************************************************************************/
/********               Autom.Call-Acception Settings               *******/
/**************************************************************************/
#define PNCAP_ACA_SPEAKER                     0x01  /* Accept-Flag 0, siehe Info    */
#define PNCAP_ACA_RECEIVER                    0x02  /* Accept-Flag 1, siehe Info    */
#define PNCAP_ACA_HANDSFREE                   0x04  /* Accept-Flag 2, siehe Info    */
#define PNCAP_ACA_SIGNAL                      0x08  /* Accept-Flag 3, siehe Info    */
#define PNCAP_ACA_VACID_CALL                  0x10  /* Accept-Flag 4, siehe Info    */

/**************************************************************************/
/********               HandsFree Mode Settings                     *******/
/**************************************************************************/
#define PNCAP_HANDSFREE_MODE_OFF              0x00  /* Accept-Flag 0, siehe Info    */
#define PNCAP_HANDSFREE_MODE_ON               0x01  /* Accept-Flag 0, siehe Info    */

#define PNCAP_DEFAULT_HANDSFREE               0x07  /* Default f�r Handsfree */
#define PNCAP_DEFAULT_HANDSET                 0x03  /* Default f�r Handset   */



/**************************************************************************/
/********               Acoustic Parameters                         *******/
/**************************************************************************/
#define PNCAP_AP_VOICEREC_REQ                 0x01  // Voicerecognition Request

#define PNCAP_AP_VOICEREC_ON                  0x01  // Sprachweg f�r VR aktivieren
#define PNCAP_AP_VOICEREC_OFF                 0x02  // Sprachweg f�r VR deaktivieren

#define PNCAP_AP_VOICEOVERIP_REQ              0x02  // Voice over IP Request

#define PNCAP_AP_VOICEOVERIP_ON               0x01  // Sprachweg Voice over IP
#define PNCAP_AP_VOICEOVERIP_OFF              0x02  //

#define PNCAP_AP_ACF_REQ                      0x03  // Automatic Feedback control

#define PNCAP_AP_AFC_MODE_DIS                 0x00  // AFC Mode disabled
#define PNCAP_AP_SINGLE_MODE                  0x01  // Single Mode
#define PNCAP_AP_MULTIMICROPHONE_MODE         0x02  // Multimicrophone Mode

/**************************************************************************/
/********               FT-Feature Identifier                       *******/
/**************************************************************************/
#define PNCAP_FTF_UNDEFINED                   0x00
#define PNCAP_FTF_NET_FEATURES                0x01  // Zugriff auf Netz-Features
#define PNCAP_FTF_CHARGES                     0x02  // Zugriff auf Einstellung der Kosten
#define PNCAP_FTF_AM_SETTINGS                 0x03  // Zugriff auf das AB-Men�
#define PNCAP_FTF_AM_ACCESS                   0x04  // Zugriff auf AB
#define PNCAP_FTF_VR_DIR_ACCESS               0x05  // Zugriff auf zentrales TB um Sprachwahl auszuf�hren
#define PNCAP_FTF_SMS                         0x06  // Zugriff auf Textmeldung
#define PNCAP_FTF_MWI                         0x07  // Zugriff auf Ereignisliste
#define PNCAP_FTF_DIR_ACCESS                  0x08  // Zugriff auf zentrales TB um Eintrag zu editieren
#define PNCAP_FTF_FP_SETTINGS                 0x09  // Zugriff auf Basis-Einstellungen (an alten Basen wird Service-Setup gemacht)
#define PNCAP_FTF_SUBS_LIST_KMT               0x0A  // Zugriff auf Intern-Namen-Liste
//..
#define PNCAP_FTF_HIDDEN_FP_SETTINGS          0x0B  // Zugriff auf versteckte Basis-Einstellungen
#define PNCAP_FTF_SMS_TI                      0x0C  // Zugriff auf Textmeldung Telecom Italia
#define PNCAP_FTF_SMS_JOE_HS                  0x0D  // Zugriff auf Textmeldung Application-Class Joe-HS
#define PNCAP_FTF_SMS_SERVICES                0x0E  // Zugriff auf Textmeldung Services
#define PNCAP_FTF_VR_DIR_ACCESS_JOE_HS        0x0F  // Zugriff auf zentrales TB, um Sprachwahl auszuf�hren f�r die Application-Class Joe-HS
#define PNCAP_FTF_DIR_ACCESS_JOE_HS           0x10  // Zugriff auf zentrales TB, um Eintrag zu editieren f�r die Application-Class Joe-HS
#define PNCAP_FTF_SMS_TI_JOE_HS               0x11  // Zugriff auf Textmeldung Telecom Italia f�r die Application-Class Joe-HS
#define PNCAP_FTF_SUBS_LIST                   0x12  // Zugriff auf Intern-Namen-Liste
#define PNCAP_FTF_PUSH_SMS                    0x13  // Transmission of SMS ot PP in order to search MMS-notifications
#define PNCAP_FTF_SUB2_LIST2                  0x14  // Subscriber List 2: without entry 'all'
#define PNCAP_FTF_DOWNLOAD                    0x15  // SMS Download Settings
#define PNCAP_FTF_SMS_NOTIFY                  0x16  // SMS Notify
#define PNCAP_FTF_WITHHOLD_CPN                0x17  // Withhold Calling Party Number
#define PNCAP_FTF_AM_MODE                     0x18  // AM-Mode
#define PNCAP_FTF_ISDN_SERVICES               0x19  // ISDN-Services
#define PNCAP_FTF_SMS_RESOURCE                0x1A  // allocates the SMS-Resource of the basestation for SMS-List access
#define PNCAP_FTF_AM_CONTROL                  0x1B  // reserved for base internal use (BaFe, AM-Control (see also additional parameter))
#define PNCAP_FTF_MEMO_REC                    0x1C  // start AM-Recording
#define PNCAP_FTF_MMS_TRANSMISSION            0x1d  // start MMS-Transmission
#define PNCAP_FTF_SMS_OUT                     0x1e  // Sms Outbox
#define PNCAP_FTF_TEXTDATA_MESSAGES           0x1f  // incoming text and data messages
#define PNCAP_FTF_NETWORK_MAILBOX             0x20  // Access to network mailbox
#define PNCAP_FTF_MISSED_CALLS_LIST           0x21  // Access to missed calls list
#define PNCAP_FTF_CALL_DIV                    0x22  // Access to call diversion
#define PNCAP_FTF_SIM_CARD                    0x23  // Access to Sim Card Menu
#define PNCAP_FTF_INT_HOME_APPL               0x24  // Access Integrated Home Applications
#define PNCAP_FTF_INT_HOME_SCENARIOS          0x25  // Access Integrated Home Scenarios
#define PNCAP_FTF_IDLE_SK1_SEL                0x26  // Indicates call establishment due to Idle softkey 1 selection
#define PNCAP_FTF_IDLE_SK2_SEL                0x27  // Indicates call establishment due to Idle softkey 2 selection
#define PNCAP_FTF_IDLE_SK1_LP_SEL             0x28  // Indicates call establishment due to Idle softkey 1 longpress selection
#define PNCAP_FTF_IDLE_SK2_LP_SEL             0x29  // Indicates call establishment due to Idle softkey 2 longpress selection
#define PNCAP_FTF_EMERGENCY_CALL              0x2A  // Indicates call establishment due to emergency call
#define PNCAP_FTF_EMERGENCY_MODE              0x2B  // Access to emergency mode settings
#define PNCAP_FTF_EMERGENCY_CALL_SET          0x2C  // Access to emergency call settings
#define PNCAP_FTF_MESSENGER                   0x2D  // Access to messenger menu
// reserved  0x2D  //
#define PNCAP_FTF_MESSAGING                   0x2f  // Access to messaging menu
#define PNCAP_FTF_IP_TELEPHONY_SET            0x30  // Access to IP-telephony setting
#define PNCAP_FTF_NETWORK_DIR                 0x31  // Access to network directory
#define PNCAP_FTF_FP_CTRL_SK1_PROG            0x32  // Access FP controlled Softkey 1 programming
#define PNCAP_FTF_FP_CTRL_SK2_PROG            0x33  // Access FP controlled Softkey 2 programming
#define PNCAP_FTF_DIAL_VIA_IP_LINE            0x34  // Access Dial via IP Line
#define PNCAP_FTF_DIAL_VIA_PSTN               0x35  // Access Dial via PSTN
#define PNCAP_FTF_EMAIL                       0x36  // Access Email
#define PNCAP_FTF_TRANSPARENT_FP_ID_PROG      0x37  // Access Transparent FP-Feature Id programming
#define PNCAP_FTF_LINE_LIST                   0x38  // Access to Line List
#define PNCAP_FTF_ONL_PHONEBOOKS_BEGIN        0x39  // Access to list of online phonebooks start at the beginning of the list
#define PNCAP_FTF_ONL_PHONEBOOKS_END          0x3A  // Access to list of online phonebooks start at the end of the list
#define PNCAP_FTF_IDLE_SCRSV_SK1_SEL          0x3B  // Indicates call establishment due to Idle screen saver softkey 1 selection
#define PNCAP_FTF_IDLE_SCRSV_SK2_SEL          0x3C  // Indicates call establishment due to Idle screen saver softkey 2 selection
#define PNCAP_FTF_IDLE_SCRSV_SK1_LP_SEL       0x3D  // Indicates call establishment due to Idle screen saver softkey 1 longpress selection
#define PNCAP_FTF_IDLE_SCRSV_SK2_LP_SEL       0x3E  // Indicates call establishment due to Idle screen saver softkey 2 longpress selection
#define PNCAP_FTF_TERMINATE_ACC_RIGHTS        0x3F  // Access to terminate Acceess Rights menu
#define PNCAP_FTF_MENU_LIST                   0x40  // Menu Lists Access Activation
#define PNCAP_FTF_INFO_CENTER                 0x41  // Access to RAP-Services
#define PNCAP_FTF_CONNECTION_ASSISTANT        0x42  // Access to connection assistant
#define PNCAP_FTF_IP_ISS                      0x43  // IP InfoService Download
#define PNCAP_FTF_INFO_DISPLAY_SK1            0x44  // indicates call establishment due to selection of info-display softkey 1
#define PNCAP_FTF_INFO_DISPLAY_SK2            0x45  // indicates call establishment due to selection of info-display softkey 2
#define PNCAP_FTF_ACCESS_RIGHTS_REQ           0x46  // Access Rights Request menu
#define PNCAP_FTF_IDLE_STATUS_TXT_SK1_SEL     0x47  // idle status text softkey 1
#define PNCAP_FTF_IDLE_STATUS_TXT_SK2_SEL     0x48  // idle status text softkey 2
#define PNCAP_FTF_RESET_FP                    0x49  // access to FP reset dialog
#define PNCAP_FTF_REPERTORY_KEY               0x4A  // Repertory Key Activation (parameter following)
#define PNCAP_FTF_REPERTORY_KEYS_PRG          0x4B  // Repertory Keys Programming
#define PNCAP_FTF_SEND_CONNECTION             0x4C  // Seize specific line
#define PNCAP_FTF_LINE_SELECTION              0x4D  // Show list of configured lines
#define PNCAP_FTF_ACCESSORY_KEY1_SP           0x4E  // Archimedes: Accessory key short press
#define PNCAP_FTF_ACCESSORY_KEY1_LP           0x4F  // Archimedes: Accessory key long press


#define PNCAP_FTF_ESCAPE                      0xff  // Access Escape FP-Feature, next octet is relevant

/**************************************************************************/
/********  Parameter Defines for FP-Feature PNCAP_FTF_AM_CONTROL    *******/
/**************************************************************************/

#define PNCAP_FTF_AM_CONTROL_PAR_PLAY_NEW_AND_OLD  0x01  // Starts Message-Playback New and Old (additional Parameter for Mailbox-Index)
#define PNCAP_FTF_AM_CONTROL_PAR_PLAY_NEW_OR_OLD   0x02  // Starts Message-Playback New or Old  (additional Parameter for Mailbox-Index)
#define PNCAP_FTF_AM_CONTROL_PAR_TOGGLE            0x03  // changes the AM-Mode(on/off)
#define PNCAP_FTF_AM_CONTROL_PAR_OGM_PLAYBACK      0x04  // Starts Playback of Outgoing message
#define PNCAP_FTF_AM_CONTROL_PAR_OGM_RECORD        0x05  // Starts Recording of Outgoing message
#define PNCAP_FTF_AM_CONTROL_PAR_DELETE_OLD        0x06  // Delete old messages


/**************************************************************************/
/******** Parameter Defines for FP-Feature Repertory Key Activation *******/
/**************************************************************************/

// Activation Parameter
#define PNCAP_FTF_REPERTORY_KEY_PAR_KEY1            0x01
#define PNCAP_FTF_REPERTORY_KEY_PAR_KEY2            0x02
#define PNCAP_FTF_REPERTORY_KEY_PAR_KEY3            0x03
#define PNCAP_FTF_REPERTORY_KEY_PAR_KEY4            0x04



/**************************************************************************/
/*** FP -Feature Release Results PNCAP_FTF_ONL_PHONEBOOKS_BEGIN/END *******/
/**************************************************************************/

#define PNCAP_FTF_PB_RR_BACK                       0x00  // Online Phonebook release backward
#define PNCAP_FTF_PB_RR_FORWARD                    0x01  // Online Phonebook release forward

/**************************************************************************/
/********  FP-Feature Release Results FP-Feature PNCAP_FTF_RESET_FP *******/
/**************************************************************************/

#define PNCAP_FTF_RB_RR_ESC                         0x00  // escape without reset
#define PNCAP_FTF_RB_RR_USER_DATA_RESET             0x01  // reset of user data done




/**************************************************************************/
/********  Parameter Defines for Local Edit window    *******/
/**************************************************************************/

// Highnibble relevant
#define PNCAP_LEW_ACT_KEYPAD_CHAR_SET                 0x00
#define PNCAP_LEW_ACT_START_LINE                      0x01
#define PNCAP_LEW_ACT_DISP_MODE                       0x02
#define PNCAP_LEW_ACT_EDIT_MODE                       0x03
#define PNCAP_LEW_ACT_CFM_TXT                         0x04
#define PNCAP_LEW_ACT_FIELD_TYPE                      0x05
#define PNCAP_LEW_ACT_EXT_EDIT_MODE                   0x06
#define PNCAP_LEW_ACT_CFG_BITMASK                     0x07       // Configuration Bitmask
#define PNCAP_LEW_ACT_CHAR_TRACKING                   0x08       // Character Tracking
#define PNCAP_LEW_ACT_SEGMENT_INFO                    0x09       // Segmentation Info
#define PNCAP_LEW_ACT_FUNCTIONAL_EDITOR               0x0A       // functional editor

// functional editor purpose:
#define PNCAP_LEW_FE_TIME                             0x01       // Time

// functional editor time mode
#define PNCAP_LEW_FE_TIME_MODE_12                     0x00       // 12 hour Timemode
#define PNCAP_LEW_FE_TIME_MODE_24                     0x01       // 24 hour Timemode


// "Keypad Character Set"
#define PNCAP_LEW_ACT_CHARSET_DIAL                0x01
#define PNCAP_LEW_ACT_CHARSET_NUMERIC             0x02
#define PNCAP_LEW_ACT_CHARSET_ALPHANUMERIC        0x03
#define PNCAP_LEW_ACT_CHARSET_TELEKOM_ITALIA_SMS  0x04
#define PNCAP_LEW_ACT_CHARSET_REDUCED_DIAL        0x05
#define PNCAP_LEW_ACT_CHARSET_CAPITALS            0x06       // alphanumerical capital letters out of G0, G1 and EURO out of G3 excluding small letter


// "Display Mode"
// Masks
#define PNCAP_LEW_ACT_DISPMODE_VISIBLE        0x00       // Default
#define PNCAP_LEW_ACT_DISPMODE_HIDDEN         0x01       //


// "Edit Mode"
#define PNCAP_LEW_ACT_EDITMODE_INSERT       0x00
#define PNCAP_LEW_ACT_EDITMODE_OVERWRITE    0x01

// "Field Type"
#define PNCAP_LEW_ACT_SINGLE_FIELD_EDITOR     0x00
#define PNCAP_LEW_ACT_MULTI_FIELD_EDITOR      0x01

// "Extended Edit Mode"
// Masks
#define PNCAP_LEW_ACT_EXT_EDITMODE_NO_MASK_EDITOR  0x00
#define PNCAP_LEW_ACT_EXT_EDITMODE_MASK_EDITOR     0x01
#define PNCAP_LEW_ACT_EXT_EDITMODE_BIG             0x02       // Big

// "Release Result"
#define PNCAP_LEW_REL_REASON_CANCEL           0x01
#define PNCAP_LEW_REL_REASON_STORE            0x02
#define PNCAP_LEW_REL_REASON_SETUP            0x03
#define PNCAP_LEW_REL_REASON_NEXT_FIELD       0x04
#define PNCAP_LEW_REL_REASON_PREV_FIELD       0x05
#define PNCAP_LEW_REL_REASON_TIMEOUT          0x06
#define PNCAP_LEW_REL_REASON_ALT_SETUP        0x07
#define PNCAP_LEW_REL_REASON_SETUP_HF         0x08
#define PNCAP_LEW_REL_REASON_ALT_SETUP_HF     0x09


// Configuration Bitmask (Lownibble)
#define PNCAP_LEW_CFG_BITMASK_ALTHF_SETUP     0x01  // release result 'handsfree setup', 'alternative handsfree setup' allowed

// Character Tracking
#define PNCAP_LEW_CHARTRACK_OFF               0x00
#define PNCAP_LEW_CHARTRACK_ON                0x01

/**************************************************************************/
/********    Local Feature Receiver Volume Control                  *******/
/**************************************************************************/

// Values Initial Action
#define PNCAP_RECV_INIT_ACTION_NO_MOD         0x00     // no automatic modification of the initial value
#define PNCAP_RECV_INIT_ACTION_INC            0x01     // increment the initial value
#define PNCAP_RECV_INIT_ACTION_DEC            0x02     // decrement the initial value

/**************************************************************************/
/********    Local Feature Fullscreen-Editor                        *******/
/**************************************************************************/

/******** Fullscreen-Editor Settings *******/
                                                    // Feature identifier
#define PNCAP_FSE_MODE                      0x01    //  - Mode
#define PNCAP_FSE_MAXLEN                    0x02    //  - Maximal Length
#define PNCAP_FSE_DATA_TRANS_MODE           0x03    //  - Data Transfer Mode
#define PNCAP_FSE_WRITE_CURSOR              0x04    //  - Write Cursor
#define PNCAP_FSE_POSITION_CURSOR           0x05    //  - Position Cursor
#define PNCAP_FSE_REFERENCE_NUMBER          0x06    //  - Referenz-Nummer (optional)
#define PNCAP_FSE_CHARSET                   0x07    //  - Charset (optional)
#define PNCAP_FSE_HEADLINE                  0x08    //  - Headline (Optional)
#define PNCAP_FSE_TEXT_ENTRY_SUPPORT        0x09    //  - Text Entry Support (Eatoni) (optional)
#define PNCAP_FSE_CFG_BITMASK               0x0a    //  - configuration bitmask
#define PNCAP_FSE_SK_LABEL1                 0x0b    //  - Softkey label 1 text
#define PNCAP_FSE_SK_LABEL2                 0x0c    //  - Softkey label 2 text
#define PNCAP_FSE_ESC_TO_SEGMENTED_PAR      0x0d    //  - escape to segmented parameter

                                                    // Parameter Coding 'Mode'
#define PNCAP_FSE_MODE_VIEW                 0x01    //  - View
#define PNCAP_FSE_MODE_EDIT                 0x02    //  - Edit
#define PNCAP_FSE_MODE_SMS_VIEW             0x03    //  - View SMS
#define PNCAP_FSE_MODE_SMS_EDIT             0x04    //  - Edit SMS
#define PNCAP_FSE_MODE_SMS_VIEW_RM          0x05    //  - View SMS Reduced Menu
#define PNCAP_FSE_MODE_SMS_VIEW_ONLY        0x06    //  - Only View SMS
#define PNCAP_FSE_MODE_SMS_VIEW_RM_ONLY     0x07    //  - Only View SMS Reduced Menu
#define PNCAP_FSE_MODE_EMAIL_ADDR_EDIT      0x08    //  - Edit Email Address
#define PNCAP_FSE_MODE_FAX_ADDR_EDIT        0x09    //  - Edit Fax Address
// reserved #define 0x0A
// reserved #define 0x0B
#define PNCAP_FSE_MODE_VIEW_MESSENGER       0x0C    //  - view messenger; results 'reply' and 'delete'
#define PNCAP_FSE_MODE_EDIT_MESSENGER       0x0D    //  - edit messenger; results 'send' and 'delete'
#define PNCAP_FSE_MODE_VIEW_WITH_2SK_LABELS 0x0E    //  - view with 2 softkey labels; results 'sk1' and 'sk2'
#define PNCAP_FSE_MODE_EDIT_WITH_1SK_LABEL  0x0F    //  - edit with 1 softkey  label; result 'sk1'



                                                    // Parameter Coding 'Data Transfer Mode'
#define PNCAP_FSE_DF_MODE_PROTOCOL1         0x01    //  - Protocol 1 Siemens GSM
#define PNCAP_FSE_DF_MODE_GX                0x02    //  - 8 Bit G0,G1,G3 Byte Stream
#define PNCAP_FSE_DF_MODE_PROTOCOL2         0x03    //  - Protocol 2 Telekom Italia

                                                    // Character Set Codings
#define PNCAP_FSE_CHARSET_LATIN1            0x01    //  - Latin 1
#define PNCAP_FSE_CHARSET_CYRILLIC          0x02    //  - Cyrillic
#define PNCAP_FSE_CHARSET_GREEK             0x03    //  - Greek
#define PNCAP_FSE_CHARSET_HEBREW            0x04    //  - Hebrew
#define PNCAP_FSE_CHARSET_CHINESE           0x05    //  - Chinese
#define PNCAP_FSE_CHARSET_ARABIC            0x06    //  - Arabic
#define PNCAP_FSE_CHARSET_TURKISH           0x07    //  - Turkish
#define PNCAP_FSE_CHARSET_LATIN2            0x08    //  - Latin 2
#define PNCAP_FSE_CHARSET_FARSI             0x09    //  - Farsi

/******** Fullscreen-Editor Release Results *******/

#define PNCAP_FSE_RR_SEND                   0x01    //  - send
#define PNCAP_FSE_RR_SEND_CHANGE            0x02    //  - send and change
#define PNCAP_FSE_RR_SAVE                   0x03    //  - save
#define PNCAP_FSE_RR_SAVE_CHANGE            0x04    //  - save and change
#define PNCAP_FSE_RR_REPLY                  0x05    //  - reply
#define PNCAP_FSE_RR_FORWARD                0x06    //  - forward
#define PNCAP_FSE_RR_CANCEL                 0x07    //  - cancel
#define PNCAP_FSE_RR_COPY_NUMBER_TO_DIR     0x08    //  - copy number to phone book editor
#define PNCAP_FSE_RR_MENU                   0x09    //  - menu
#define PNCAP_FSE_RR_ESC                    0x0a    //  - esc
#define PNCAP_FSE_RR_SEND_TO                0x0b    //  - "send_to" with additional receiver number
#define PNCAP_FSE_RR_SETUP                  0x0c    //  - "setup" with additional number
#define PNCAP_FSE_RR_DELETE                 0x0d    //  - delete
#define PNCAP_FSE_RR_SK1                    0x0e    //  - softkey 1
#define PNCAP_FSE_RR_SK2                    0x0f    //  - softkey 2
#define PNCAP_FSE_RR_ALT_SETUP              0x10    //  - alternative "setup" with additional number
#define PNCAP_FSE_RR_SETUP_HF               0x11    //  - "setup" with handsfree key
#define PNCAP_FSE_RR_ALT_SETUP_HF           0x12    //  - alternative "setup" with handsfree key

/******** Fullscreen-Editor parameter configuration bitmask *******/
#define PNCAP_FSE_PARCFG_SUP_RR_SETUP         0x01    // support of release result setup
#define PNCAP_FSE_PARCFG_SUP_RR_DELETE        0x02    // support of release result delete
#define PNCAP_FSE_PARCFG_SUP_RR_REPLY         0x04    // support of release result reply
#define PNCAP_FSE_PARCFG_SUP_RR_SETUP_NO_NUM  0x08    // support of release result 'setup' without number
#define PNCAP_FSE_PARCFG_SUP_RR_ALT_SETUP     0x10    // support of release result 'alernative setup'
#define PNCAP_FSE_PARCFG_SUP_RR_SETUP_HF      0x20    // support of release result 'setup with handsfree key'
#define PNCAP_FSE_PARCFG_SUP_RR_ALT_SETUP_HF  0x40    // support of release result 'alternative setup with handsfree key'

//******* Fullscreen-Editor escape control codes ******************/
#define PNCAP_FSE_ESC_CONTROL_HIGHLIGHT_ON    0x01    // escape control code highlight on (only for FSE)
#define PNCAP_FSE_ESC_CONTROL_HIGHLIGHT_OFF   0x02    // escape control code highlight off (only for FSE)



/**************************************************************************/
/********             local feature push SMS                        *******/
/**************************************************************************/

/******* push SMS Release results *******/

#define PNCAP_PUSH_SMS_RR_DEL               0x01
#define PNCAP_PUSH_SMS_RR_DEL_NOT           0x02
#define PNCAP_PUSH_SMS_RR_CANCEL            0x03

/**************************************************************************/
/********             local feature MMS inbox                       *******/
/**************************************************************************/

/******* MMS inbox Release results *******/

#define PNCAP_MMS_INBOX_RR_ESC              0x01
#define PNCAP_MMS_INBOX_RR_CANCEL           0x02
#define PNCAP_MMS_INBOX_RR_SETUP            0x03


/**************************************************************************/
/********             local feature Line Attribute Assignment       *******/
/**************************************************************************/

/******* Line Attribute Assignment Activation Parameter  *******/

// Information Field
#define PNCAP_LINEID_LINE_NAME              0x01
#define PNCAP_LINEID_FIRST_LINE_IND         0x02             // first line indication
#define PNCAP_LINEID_LAST_LINE_IND          0x03             // last line indication


/******* Line Attribute Assignment Release results *******/

#define PNCAP_LINEID_RR_PREVIOUS_LINE       0x01
#define PNCAP_LINEID_RR_NEXT_LINE           0x02
#define PNCAP_LINEID_RR_ESCAPE              0x03


/**************************************************************************/
/********     Parameter Defines for PIE: SMS data                   *******/
/**************************************************************************/

#define PNCAP_SMS_DATA_READ                 0x01
#define PNCAP_SMS_DATA_SEND                 0x02
#define PNCAP_SMS_DATA_DEL_REQ              0x03
#define PNCAP_SMS_DATA_DEL_CFM              0x04
#define PNCAP_SMS_DATA_STATUS_NEW           0x05
#define PNCAP_SMS_DATA_STATUS_OLD           0x06
#define PNCAP_SMS_DATA_STATUS_CFM           0x07




/**************************************************************************/
/********             Status-Announcement                           *******/
/**************************************************************************/

// Information Fields

// Parameter Status Announcements Types
#define PNCAP_STAT_ANCMT_AM_STATUS   0x01
#define PNCAP_STAT_ANCMT_MWI_STATUS  0x02
#define PNCAP_STAT_ANCMT_STATUS_TEXT 0x03
// reserved                          0x04
// reserved                          0x05
#define PNCAP_STAT_ANCMT_MWI_MMS_TOTAL         0x06
#define PNCAP_STAT_ANCMT_MWI_MMS_NEW           0x07
// reserved for further use #define PNCAP_STAT_ANCMT_CHARGER_STATUS        0x08
#define PNCAP_STAT_ANCMT_NAT_ACC_CODE          0x09
#define PNCAP_STAT_ANCMT_INT_NAT_ACC_CODE      0x0a
#define PNCAP_STAT_ANCMT_NAT_AREA_CODE         0x0b
#define PNCAP_STAT_ANCMT_INT_COUNTRY_CODE      0x0c
#define PNCAP_STAT_ANCMT_INT_HOME_APPL         0x0d
#define PNCAP_STAT_ANCMT_INT_HOME_SCENARIOS    0x0e
#define PNCAP_STAT_ANCMT_INT_HOME_ITEM_ID      0x0f

#define PNCAP_STAT_ANCMT_EMERGENCY_CALL_STAT   0x10
#define PNCAP_STAT_ANCMT_SCREENSAVER_TEXT      0x11
#define PNCAP_STAT_ANCMT_ECO_MODE              0x12
#define PNCAP_STAT_ANCMT_RDY_TO_RCV_SCR_TEXT   0x13   // Ready to receive screensaver text
#define PNCAP_STAT_ANCMT_FP_FEATURE_STATUS     0x14   // Fp-Feature Status
#define PNCAP_STAT_ANCMT_LIVETICKER_TEXT       0x15   // Live Ticker Text
#define PNCAP_STAT_ANCMT_RDY_TO_RCV_LVT_TEXT   0x16   // Ready to receive Live Ticker text
#define PNCAP_STAT_ANCMT_LIST_UPD_AVAIL        0x17   // List Update available
#define PNCAP_STAT_ANCMT_HIGH_PRIO_NET_DIR     0x18   // High priority network directory state
//#define                                      0x19   // reserved
#define PNCAP_STAT_ANCMT_NEMO                  0x1A   // NEMo mode
#define PNCAP_STAT_ANCMT_AM_MEM_STATUS         0x1B   // Answering machine Memory state
#define PNCAP_STAT_ANCMT_HIGHPRIO_STATUS_TEXT  0x1C   // high priority status text
#define PNCAP_STAT_ANCMT_RDY_TO_RCV_IP_ISS     0x1D   // Ready to Receive IP-Infoservice-Slideshow
#define PNCAP_STAT_ANCMT_RDY_FOR_DL_IP_ISS     0x1E   // Ready for Download of IP-Infoservice-Slideshow
#define PNCAP_STAT_ANCMT_RESET_ENTRY_TAGS      0x1F   // Reset Entry Tags
#define PNCAP_STAT_ANCMT_EXT_AM_STATUS         0x20   // Extended answering machine status
#define PNCAP_STAT_ANCMT_AVAILABLE_LINES       0x21   // available lines


// Status Values Answering machine // Extended Answering machine status
#define PNCAP_STAT_ANCMT_AM_AM_UNDEFINED 0x00
#define PNCAP_STAT_ANCMT_AM_STATUS_ON    0x01
#define PNCAP_STAT_ANCMT_AM_STATUS_OFF   0x02
#define PNCAP_STAT_ANCMT_AM_STATUS_BUSY  0x03
#define PNCAP_STAT_ANCMT_AM_NOT_ASSIGNED 0x04


// Status Values Answering machine memory
#define PNCAP_STAT_ANCMT_AM_MEM_STAT_UNDEFINED  0x00   //
#define PNCAP_STAT_ANCMT_AM_MEM_STAT_NOT_FULL   0x01   //
#define PNCAP_STAT_ANCMT_AM_MEM_STAT_FULL       0x02   //

// Parameter MWI-Status Types
#define PNCAP_STAT_ANCMT_MWI_NEW_MSG_ANSWM     0x01
#define PNCAP_STAT_ANCMT_MWI_NEW_MSG_T_BOX     0x02   // PP->PP reserved   FP->PP new message
#define PNCAP_STAT_ANCMT_MWI_NEW_MSG_SMS       0x03   // PP->PP reserved   FP->PP new message
#define PNCAP_STAT_ANCMT_MWI_NEW_MSG_CLL       0x04   // PP->PP reserved   FP->PP new message
#define PNCAP_STAT_ANCMT_MWI_NEW_MSG_MCL       0x05   // PP->PP reserved   FP->PP new message
// reserved                                    0x06
// reserved                                    0x07
#define PNCAP_STAT_ANCMT_MWI_SMS_PUSH_NEW      0x08
#define PNCAP_STAT_ANCMT_MWI_MCL               0x09
#define PNCAP_STAT_ANCMT_MWI_ANSWM             0x0a
#define PNCAP_STAT_ANCMT_MWI_SMS               0x0b
#define PNCAP_STAT_ANCMT_MWI_VOICEDATA         0x0c
#define PNCAP_STAT_ANCMT_MWI_EMAIL_TOTAL       0x0d
#define PNCAP_STAT_ANCMT_MWI_EMAIL_NEW         0x0e
#define PNCAP_STAT_ANCMT_MWI_MISSED_APPOINTM   0x0f   // Missed appointments
#define PNCAP_STAT_ANCMT_MWI_AM_TOTAL          0x10  // answering machine total
#define PNCAP_STAT_ANCMT_MWI_MISS_APP_TOTAL    0x11  // total missed appointments

// Parameter emergency call status
#define PNCAP_STAT_ANCMT_EMC_ON                0x01
#define PNCAP_STAT_ANCMT_EMC_OFF               0x02
#define PNCAP_STAT_ANCMT_EMC_NOT_SET           0x03

// Parameter information field eco mode status
#define PNCAP_STAT_ANCMT_ECO_OFF               0x00
#define PNCAP_STAT_ANCMT_ECO_ON                0x01
#define PNCAP_STAT_ANCMT_ECO_DYN               0x02

// Parameter information field High priority network directory state
#define PNCAP_STAT_ANCMT_HIGH_PRIO_OFF               0x00
#define PNCAP_STAT_ANCMT_HIGH_PRIO_ON                0x01

// Parameter information field NEMo mode status
#define PNCAP_STAT_ANCMT_NEMO_OFF               0x00
#define PNCAP_STAT_ANCMT_NEMO_ON                0x01

// Parameter information field Ready to receive screensaver text
#define PNCAP_STAT_ANCMT_SCRTXT_OFF            0x00
#define PNCAP_STAT_ANCMT_SCRTXT_ON             0x01

// Parameter information field Fp-Feature Status
// Action values
#define PNCAP_STAT_ANCMT_FPF_ACT_DEL_TR_FPID   0x01  // Delete programmed FP-Feature

// Parameter information field Live Ticker Text  (FP->PP)
#define PNCAP_STAT_ANCMT_LVTTXT_FIRST_SEG      0x01  // First segment of Live Ticker Text
#define PNCAP_STAT_ANCMT_LVTTXT_MIDDL_SEG      0x02  // Middle segment of Live Ticker Text
#define PNCAP_STAT_ANCMT_LVTTXT_LAST_SEG       0x03  // Middle segment of Live Ticker Text
#define PNCAP_STAT_ANCMT_LVTTXT_DEL            0x04  // Delete Live Ticker Text
#define PNCAP_STAT_ANCMT_LVTTXT_SUSPEND        0x05  // Suspend Live Ticker Text
#define PNCAP_STAT_ANCMT_LVTTXT_RESUME         0x06  // Resume Live Ticker Text

// Parameter information field Ready to receive Live Ticker Text  (PP->FP)
#define PNCAP_STAT_ANCMT_LVTTXT_OFF            0x00
#define PNCAP_STAT_ANCMT_LVTTXT_ON             0x01  // Ready to receive!
#define PNCAP_STAT_ANCMT_LVTTXT_RESUME_ON      0x02  // Ready to receive or to resume!

// Parameter information field Ready to Receive IP-Infoservice-Slideshow  (PP->FP)
#define PNCAP_STAT_ANCMT_IP_ISS_RDY_OFF        0x00
#define PNCAP_STAT_ANCMT_IP_ISS_RDY_ON         0x01

// Parameter information field Ready for Download of IP-Infoservice-Slideshow (FP->PP)
// see info field entry tag

// Parameter information field Supported line identifier
#define PNCAP_STAT_ANCMT_LINETYPE_NOT_SPECIFIED  0x00   // Mask :not specified
#define PNCAP_STAT_ANCMT_LINETYPE_PSTN           0x01   // Mask :PSTN (POT) Plain old telephony
#define PNCAP_STAT_ANCMT_LINETYPE_ISDN           0x02   // Mask :ISDN
#define PNCAP_STAT_ANCMT_LINETYPE_VOIP           0x04   // Mask :VoIP
#define PNCAP_STAT_ANCMT_LINETYPE_GSM            0x08   // Mask :GSM


/**************************************************************************/
/********             Remote-Status-Announcement                    *******/
/**************************************************************************/

// Parameter Remote status announcement request

// Information field for Remote status announcement request
#define PNCAP_RSAS_REQ_INF_FIELD_MWI_STATUS    0x01
#define PNCAP_RSAS_REQ_INF_FIELD_REFR_MWI_REQ  0x02
#define PNCAP_RSAS_REQ_INF_FIELD_TEXT          0x03

  // Data values for information field MWI status
  #define PNCAP_RSAS_REMOTE_CTRL_OFF              0x00

  // Data values for information field status text
  #define PNCAP_RSAS_REMOTE_STATUS_TEXT_OFF       0x00
  #define PNCAP_RSAS_REMOTE_STATUS_TEXT_HIGH_PRIO 0xFF


// Parameter Remote status announcement confirm
#define PNCAP_RSAS_CFM_CAUSE_ACCEPT            0x00
#define PNCAP_RSAS_CFM_CAUSE_REJ_NO_RES        0x01
#define PNCAP_RSAS_CFM_CAUSE_REJ_PAR_FALSE     0x02
#define PNCAP_RSAS_CFM_CAUSE_REJ_NOT_SUPP      0x03
#define PNCAP_RSAS_CFM_CAUSE_REJ_ALR_USED      0x04

// Parameter Remote status expiration time
#define PNCAP_RSAS_NO_EXPIRATION_TIME          0x00


/**************************************************************************/
/********     Totally Integrated Home applications / scenarios      *******/
/**************************************************************************/

// Parameter integrated home applications and scenarios request

// Information field for "Integrated Home Application Request"
#define PNCAP_TIH_REQ_INF_FIELD_SAVE           0x01
#define PNCAP_TIH_REQ_INF_FIELD_DELETE         0x02


// Parameter Integrated home applications and scenarios confirm
#define PNCAP_TIH_CFM_INF_FIELD_CONFIRM            0x03

// Parameter Integrated home applications confirm
#define PNCAP_TIH_CFM_CAUSE_ACCEPT             0x00
#define PNCAP_TIH_CFM_CAUSE_REJ_NO_RES         0x01  //No resources or text too long
#define PNCAP_TIH_CFM_CAUSE_REJ_LENGTH         0x02  //incorrect text length


/**************************************************************************/
/********             PNCAP_PIE_ESCAPE_TO_DATA_FEATURES             *******/
/**************************************************************************/

// Data link coding
#define PNCAP_ETDF_LINK_REQ                0x01  ///< Used for requesting a data link
#define PNCAP_ETDF_LINK_CFM                0x02  ///< Used to send a positive or negative confirmation on both a <<Data link request>> and <<Data packet>>
#define PNCAP_ETDF_PACKET                  0x03  ///< Used by the transmitter to send the data to the receiver.
#define PNCAP_ETDF_LINK_REL                0x04  ///< Send by the data transmitter to the receiver to release the data link.
#define PNCAP_ETDF_LINK_PROCEEDING         0x05  ///< Used for indicating to the data transmitting PP that the FP supports the data list exchange.


/**************************************************************************/
/********             Registration Mode Confirm                     *******/
/**************************************************************************/

// Result Masks
#define PNCAP_REGMODRES_NOT_ENA                0x00  // not enabled
#define PNCAP_REGMODRES_ENA                    0x01  // enabled


/**************************************************************************/
/********             Transparent Channel Control                   *******/
/**************************************************************************/

// Switch
#define PNCAP_TSC_SWITCH_OFF                   0x00
#define PNCAP_TSC_SWITCH_ON                    0x01

// Internal reference
#define PNCAP_TSC_IREF_NO_INFO_AVAILABLE       0xFE
#define PNCAP_TSC_IREF_TO_ALL                  0xFF

/**************************************************************************/
/********          Subscription place information elements          *******/
/**************************************************************************/

#define PNCAP_SPI_NEW_SUBSCR_DATA_AVAILABLE    0x01
#define PNCAP_SPI_TRANSPARENT_CHANNEL_SUPPORT  0x02
#define PNCAP_SPI_SUBSCRIBER_DATA_FOR_SENDER   0x04
#define PNCAP_SPI_DISTRIBUTED_COM_SUPPORT      0x08

/**************************************************************************/
/********             SMS-Capabilities                              *******/
/**************************************************************************/

#define PNCAP_SMS_CAP_NUMBEROF_SMS_SUPPORTED    0x01

/**************************************************************************/
/********             Call Mode Indication Defines                  *******/
/**************************************************************************/

#define PNCAP_CM_IND_HANDSET              0x01
#define PNCAP_CM_IND_HANDSFREE            0x02

/**************************************************************************/
/********             "Call information" information field codings  *******/
/**************************************************************************/

#define PNCAP_CI_CLIP                0x00
#define PNCAP_CI_CNIP                0x01
#define PNCAP_CI_MSN_NUMBER          0x02
#define PNCAP_CI_MSN_NAME            0x03

/**************************************************************************/
/********             "Extended capabilities" information field codings  **/
/**************************************************************************/

#define PNCAP_EC_SUP_C1              0x00   // List of supported C1-codings
#define PNCAP_EC_SUP_ICONS           0x01   // List of supported Icon-codings
#define PNCAP_EC_SUP_LREP            0x02   // List of supported LocalPresentation codings
#define PNCAP_EC_SUP_LFUNC           0x03   // List of supported Local-Function codings
#define PNCAP_EC_SUP_FONTS           0x04   // List of supported fonts
#define PNCAP_EC_SUP_COLSCHEMES      0x05   // Number of supported colour schemes
#define PNCAP_EC_SUP_FSE_PAR         0x06   // List of supported FSE parameters
#define PNCAP_EC_SUP_LNS_STATUS_TXT  0x07   // Display lines for status text
#define PNCAP_EC_SUP_LNS_SCRSAV_TXT  0x08   // Display lines for screen saver text
#define PNCAP_EC_SUP_IE_SIGNAL       0x09   // List of supported tone codings in IE-Signal
#define PNCAP_EC_SUP_LVTTXT          0x0A   // Live Ticker Text Capabilities
#define PNCAP_EC_SUP_KEYPAD_CHAR     0x0B   // Keypad Character Support
#define PNCAP_EC_SUP_DSP_REL         0x0C   // Display Release Codings
#define PNCAP_EC_SUP_STAT_ANCMT      0x0D   // Supported Status Announcements
#define PNCAP_EC_SUP_IP_ISS          0x0E   // Supported IP-infoservice-slideshow capabilities
#define PNCAP_EC_SUP_LEW             0x0F   // Supported local edit window capabilities
#define PNCAP_EC_SUP_LIST_ACC        0x10   // Supported list access capabilities
#define PNCAP_EC_SUP_IDLE_SKP        0x11   // idle softkey programming capabilities
#define PNCAP_EC_SUP_GEN_PT_CAP      0x12   // generic extended PT capabilities
#define PNCAP_EC_SUP_CFG_CPN         0x13   // configuration data in PIE <<Proprietary CPN and CLIP picture>>
#define PNCAP_EC_SUP_ACOUSTIC_PAR    0x14   // Acoustic parameter support


/**************************************************************************/
/********              Extended PT-Capabilies-Elements              *******/
/**************************************************************************/

// Bitmasks!

/********************* Support of C1-Codings ******************************/
// Byte 1
#define PNCAP_EC_C1_STANDARD_GROUP         0x01
#define PNCAP_EC_C1_BIDI                   0x02 // PT supports the following C1 coding: suppress BiDi algorithm
#define PNCAP_EC_C1_ESC_TWO_BYTE_CHARSETS  0x04 // escape to two byte character sets
#define PNCAP_EC_C1_ESC_SINGLE_SHIFT_TWO   0x08 // single shift two
#define PNCAP_EC_C1_CONFIG_DIGIT           0x10 // config digit
#define PNCAP_EC_C1_LINE_ATTRIB_FR_COL_BIG 0x20 // line attributsfor display configuration: frame, colour, big
#define PNCAP_EC_C1_LINE_ALIGN_PAR_X       0x40 // alignment codes for use with control code �define paragraph�: x1H, x2H, x3H, x4H, x5H with x=1,2
#define PNCAP_EC_C1_LINE_ATTRIB_LIST       0x80 // line attribute for display configuration: list

// Byte 2
#define PNCAP_EC_C1_UNDELRLINE             0x01 // Underline (0x95/0x96)
#define PNCAP_EC_C1_LINE_A_VIP_CLIP_IND    0x02 // line attribute VIP-CLIP-index
#define PNCAP_EC_C1_LINE_A_LIST_0100       0x04 // line attribute list with the parameter 0100

/********************* Support of Icon-Codings ****************************/
// Byte 1
#define PNCAP_EC_ICONS_STANDARD_GROUP      0x01
#define PNCAP_EC_ICONS_IP                  0x02
#define PNCAP_EC_ICONS_ALERTING            0x04
#define PNCAP_EC_ICONS_DO_NOT_DIST         0x08

/********************* Support of G3-Codings ****************************/
// Byte 1
#define PNCAP_EC_G3_STANDARD_GROUP         0x01
#define PNCAP_EC_G3_ACT_EURO_MEL_GROUP     0x02
#define PNCAP_EC_G3_BELL_ENVELOPE_GROUP    0x04
#define PNCAP_EC_G3_PICTO_PAWNS            0x08  // escape coding 0xd9 with the pictogram identifier 00, 01 and 02
#define PNCAP_EC_G3_TWO_BYTE_PICTOS        0x10  // escape coding 0xda with the pictogram identifier 00 - 17
#define PNCAP_EC_G3_TWO_BYTE_PICTOS_IP     0x20  // escape coding 0xda with the pictogram identifier 18 - 19 (IP-Pictos)
#define PNCAP_EC_G3_BASE_PROV_GR_25_50     0x40  // escape coding 0xdb with base provided G3 pictograms with number of lines 1-3 and width 25% and 50%
#define PNCAP_EC_G3_INFO_CHARACTER         0x80  // escape coding 0xda with the pictogram identifier 1a
// Byte 2
#define PNCAP_EC_G3_TWO_BYTE_IN_MULTILINE  0x01  // escape coding 0xda (escape to two byte pictograms) in multiline paragraph

/********************* Support of G1-Codings ****************************/
// Byte 1
#define PNCAP_EC_G1_LATIN1                 0x01
#define PNCAP_EC_G1_CYRILLIC               0x02
#define PNCAP_EC_G1_GREEK                  0x04
#define PNCAP_EC_G1_HEBREW                 0x08
#define PNCAP_EC_G1_CHINESE                0x10
#define PNCAP_EC_G1_ARABIC                 0x20
#define PNCAP_EC_G1_TURKISH                0x40
#define PNCAP_EC_G1_LATIN2                 0x80

// Byte 2
#define PNCAP_EC_G1_FARSI                  0x01

/********************* Support of Local-Representation-Codings ***********/
// Byte 1
#define PNCAP_EC_LREP_LOUDNESS_0_6         0x01
#define PNCAP_EC_LREP_SOUND_0_9            0x02
#define PNCAP_EC_LREP_SEL_LOUDNESS_0_6     0x04
#define PNCAP_EC_LREP_SEL_SOUND_0_9        0x08
#define PNCAP_EC_LREP_SPEAKER_VOL          0x10
#define PNCAP_EC_LREP_ALERT_CALLTIMER      0x20
#define PNCAP_EC_LREP_ALERTING_WO_PICT     0x40
#define PNCAP_EC_LREP_3LINE_ALERTING       0x80
// Byte 2
#define PNCAP_EC_LREP_1LINE_IP_ALERTING    0x01
#define PNCAP_EC_LREP_3LINE_IP_ALERTING    0x02
#define PNCAP_EC_LREP_ESC_ENTRY_ID         0x04
#define PNCAP_EC_LREP_REQ                  0x08
#define PNCAP_EC_LREP_CFM_A_CALL           0x10  // confirmation, rejection,question, information,
                                                 // connecting, connection established, connection refused,
                                                 // internal alerting, busy indicator, internal call waiting
#define PNCAP_EC_LREP_CLIP_P               0x20  // clip picture
#define PNCAP_EC_LREP_MISSED_CALL          0x40  // missed call, accepted call, outgoing call
#define PNCAP_EC_LREP_IP_CON               0x80  // PT supports the following local representation coding: Connection established on IP-line

// Byte 3
#define PNCAP_EC_LREP_NUMBER_TYPE          0x01  // home number, mobile number, office number

// #define

/********************* Support of Local-Function-Codings       ***********/
// Byte 1
#define PNCAP_EC_LFUNC_STANDARD_GROUP      0x01       // modify call duration time CPN, Off-Hook, call mode (1-6)
#define PNCAP_EC_LFUNC_LNR                 0x02
#define PNCAP_EC_LFUNC_MUTE                0x04
#define PNCAP_EC_LFUNC_SPEAKERSTATE        0x08
#define PNCAP_EC_LFUNC_TMPKEYCLICK         0x10
#define PNCAP_EC_LFUNC_STANDARDWITHOUTCM   0x20      // Standard without Call mode
#define PNCAP_EC_LFUNC_HSCALLMODES         0x40      // Handset Call modes (without handsfree)
#define PNCAP_EC_LFUNC_CNIP                0x80      // Presentation of CNIP

// Byte 2
#define PNCAP_EC_LFUNC_3LINE_CNIP          0x01      // Presentation of CPN/CNIP with presentation mode three line
#define PNCAP_EC_LFUNC_MELODY_VOL          0x02      // local function coding: melody volume
#define PNCAP_EC_LFUNC_CALL_STATE          0x04      // local function coding: call status display
#define PNCAP_EC_LFUNC_CHARGES             0x08      // local function coding: call charges
#define PNCAP_EC_LFUNC_REC_VOL             0x10      // local function coding: receiver volume


/********************* Support of Font -Codings                ***********/
// Nibble
#define PNCAP_EC_FONT_FONTID               0x00      // Highnibble
#define PNCAP_EC_FONT_LASTFONTID           0x10      // Highnibble
#define PNCAP_EC_FONT_LINES                0x20      // Highnibble
#define PNCAP_EC_FONT_CHAR_PER_LINE_EQFONT 0x30      // Highnibble
#define PNCAP_EC_FONT_PIX_PER_LINE         0x40      // Highnibble
#define PNCAP_EC_FONT_MAX_CHAR_WIDTH_PIX   0x50      // Highnibble
#define PNCAP_EC_FONT_MEAN_CHAR_WIDTH_PIX  0x60      // Highnibble
#define PNCAP_EC_FONT_G3                   0x70      // Highnibble
#define PNCAP_EC_FONT_CHARSET_G1           0x80      // Highnibble
#define PNCAP_EC_FONT_CFGATTR_C1           0x90      // Highnibble

// Font-IDs
#define PNCAP_EC_FONTID_EQUIDISTANT        0x00      // Equidistantfont
#define PNCAP_EC_FONTID_PROPFONT_1         0x01      // Proportionalfont 11
#define PNCAP_EC_FONTID_PROPFONT_2         0x02      // Proportionalfont 14 (formely coded with 0x01)
#define PNCAP_EC_FONTID_BIG_PROPSKFONT     0x03      // Big Proportional Softkey font (17)
#define PNCAP_EC_FONTID_BIG_PROPFONT       0x04      // Big Proportional font (20)
#define PNCAP_EC_FONTID_PROPFONT_5         0x05      // Proportional font 5 (H1 HS, Roman 21 pixel)

// Display Configuration
#define PNCAP_EC_CFGATTRC1_STD             0x01      // Mask
#define PNCAP_EC_CFGATTRC1_SEL             0x02      // Mask
#define PNCAP_EC_CFGATTRC1_HDL             0x04      // Mask
#define PNCAP_EC_CFGATTRC1_TYP             0x08      // Mask
#define PNCAP_EC_CFGATTRC1_BLBORDER        0x10      // Mask

/********************* Support of FSE parameters-Codings       ***********/
// First Byte
#define PNCAP_EC_FSEPAR_SMS_STD            0x01      // Mask : Maximal length,DataTranfer,write / position cursor,
                                                     //        reference number, characterset
#define PNCAP_EC_FSEPAR_HDL_SMS            0x02      // Mask : Headline with mode edit Sms
#define PNCAP_EC_FSEPAR_HDL_VIEW           0x04      // Mask : Headline with mode view
#define PNCAP_EC_FSEPAR_TEXT_ENTRY_SUP     0x08      // Mask : Text entry support
#define PNCAP_EC_FSEPAR_CFG_BITMASK        0x10      // Mask : Configuration bitmask
#define PNCAP_EC_FSEPAR_MODE_STD           0x20      // Mask : Standard modes: edit, view, view sms, edit sms,
                                                     //        view sms reduced menu,
                                                     //        only view sms, only view sms reduced menu, (editor activation not allowed)
                                                     //        email address, fax address
#define PNCAP_EC_FSEPAR_MESSENGER          0x40      // Mask : modes: view messenger, edit messenger
#define PNCAP_EC_FSEPAR_SK_LABEL           0x80      // Mask : modes: view with to softkey labels, edit with softkey label
// Second Byte
#define PNCAP_EC_FSEPAR_SEG_PAR            0x01      // Mask : Parameter field �Escape to Segmented Parameter�

/********************* Support of tone codings in IE-Signal     ***********/

#define PNCAP_EC_IE_SIG_DIAL_RGBACK_BUSY   0x01      // Mask : Supports 0x00 (Dial tone on), 0x01 (Ringback), 0x04 (busy), 0x3F (Tones off)
#define PNCAP_EC_IE_SIG_WRDL_DLRD_CALLW    0x02      // Mask : Supports 0x03 (Wrong Dial tone on), 0x05 (Dial ready), 0x07 (Call waiting)
#define PNCAP_EC_IE_SIG_KNOCKING           0x04      // Mask : Supports 0x44 (Knocking pattern)


/********************* Support Live Ticker Text Capabilities    ***********/
#define PNCAP_EC_LVTTXT_FRST_MID_LAST_SEG  0x01      // Mask : Supports First, Middle and Last Segment
#define PNCAP_EC_LVTTXT_DEL                0x02      // Mask : Supports Delete Live Ticker Text
#define PNCAP_EC_LVTTXT_SUS_RES            0x04      // Mask : Supports Supports Suspend and Resume Live Ticker Text

/********************* Support Keypad Character Capabilities    ***********/
// First Byte
#define PNCAP_EC_KEYCH_MUTE                0x01      // Mask : Supports Mute Hard Key
#define PNCAP_EC_KEYCH_INT_KEY             0x02      // Mask : Supports Int Hard Key (No request necessary Gap Code sent)
#define PNCAP_EC_KEYCH_LP_TUMBLE_PAD_L     0x04      // Mask : Supports longpress tumble pad left for temporary key request
#define PNCAP_EC_KEYCH_REDIAL_KEY          0x08      // Mask : Supports redial key
#define PNCAP_EC_KEYCH_AM_KEYS             0x10      // Mask : Supports AM keys (AM key, Play key, Skip Back key long press, Skip Back key short press)
#define PNCAP_EC_KEYCH_CMB_SEND_HF         0x20      // Mask : Supports combined Send-/HF-Key
#define PNCAP_EC_KEYCH_REC_ON_HOOK         0x40      // Mask : Supports receiver on hook key
#define PNCAP_EC_KEYCH_SEND_CONNECTION     0x80      // Mask : Supports send connection key
// Second Byte
#define PNCAP_EC_KEYCH_LINE_SELECTION      0x01      // Mask : Supports the line selection key
#define PNCAP_EC_KEYCH_MUTE_NAVI_RIGHT     0x02      // Mask : Supports Mute function on right navi key

/********************* Support Display Release Codings          ***********/
#define PNCAP_EC_DSP_REL_IMMEDIATELY        0x01      // Mask : Supports Display Release immediately
#define PNCAP_EC_DSP_REL_CALL_REL           0x02      // Mask : Supports Display Release with call release
#define PNCAP_EC_DSP_REL_KEEP_AS_SCRSV      0x04      // Mask : Supports Display Release keep as screen saver

/********************* Support Status Announcements             ***********/
#define PNCAP_EC_STATANCMT_HIGH_PRIO_NWDIR  0x01      // Mask : Supports High Priority Network Access
#define PNCAP_EC_STATANCMT_NEMO_STATUS      0x02      // Mask : Supports NEMo Status
#define PNCAP_EC_STATANCMT_LIST_UPD         0x04      // Mask : Supports use of �list update available� for caching of menu lists
#define PNCAP_EC_STATANCMT_MWI_MISSED_APP   0x08      // Mask : Supports �mwi status, missed appointments
#define PNCAP_EC_STATANCMT_HP_STATUSTEXT    0x10      // Mask : Supports �high priority status text�
#define PNCAP_EC_STATANCMT_AM_MEM_STATUS    0x20      // Mask : Supports �answering machine memory status�
#define PNCAP_EC_STATANCMT_EXT_AM_STATUS    0x40      // Mask : Supports 'extended answering machine status�
#define PNCAP_EC_STATANCMT_AVAILABLE_LINES  0x80      // Mask : Supports the status announcement �available lines�

/********************* Support IP-infoservice-slideshow         ***********/

/********************* Support local edit window capabilities   ***********/

#define PNCAP_EC_LEW_FCT_TIME               0x01      // Mask : Supports functional local edit window for time
#define PNCAP_EC_LEW_CAPITAL_LETTERS        0x02      // Mask : character set: alphanumerical capital letters out of G0, G1 and EURO out of G3 excluding small letter

/********************* Support list access capabilities         ***********/

#define PNCAP_EC_LIST_ACC_CHARSET_PAR        0x01      // Mask : PT supports characterset switching in list access fields with texts
#define PNCAP_EC_LIST_ACC_CHARSET_DFT        0x02      // Mask : PT supports default characterset switching via PIE <<list default attributes>>

/*********************  idle softkey programming capabilities  ***********/
#define PNCAP_EC_SKP_CHARSETSWITCH_LABEL     0x01      // Mask : PT supports characterset switching in softkey labels
#define PNCAP_EC_SKP_SK12_LP_SK12            0x02      // Mask : PT supports 'idle sk 1/2, idle sk 1/2 longpress'
#define PNCAP_EC_SKP_SCREENSV_SK12_LP_SK12   0x04      // Mask : PT supports 'screensaver sk 1/2, �screensaver sk 1/2 longpress'
#define PNCAP_EC_SKP_STATUS_TXT_SK12         0x08      // Mask : PT supports 'idle status text sk 1/2'

/*********************  generic extended PT capabilities       ***********/
#define PNCAP_EC_GEN_PT_STATUS_CISS_REQ                   0x01      // Mask : PT supports receiving of the PIE <<PT Status Req>> in a CISS message
#define PNCAP_EC_GEN_PT_INFO_DISPLAY                      0x02      // Mask : PT supports the PIE <<Info-Display>>
#define PNCAP_EC_GEN_PT_PRODNAME_AS_INTNAME               0x04      // Mask : PT supports ProductName as IntName ( Archimedes )
#define PNCAP_EC_GEN_PT_EXC_PAUSE_NUM_MEM                 0x08      // Mask : PT supports exclusion of pause key for storage in number memories
#define PNCAP_EC_GEN_PT_DF_CC_PROC_ALERT                  0x10      // Mask : PT supports supports escape to data feature in the cc-states call proceeding and alerting
#define PNCAP_EC_GEN_PT_COMB_CS_AND_SRV_NEG               0x20      // Mask : PT supports supports combined Cs and Service Negotiation
#define PNCAP_EC_GEN_PT_PROPRIETARY_SUOTA                 0x40      // Mask : PT supports proprietary SUOTA
#define PNCAP_EC_GEN_PT_LOCAL_CONFIGURATION_MANAGEMENT    0x80      // Mask : PT supports the PIE <<Local configuration management>>

/*********************  Config Data Proprietary Calling party number ******/

#define PNCAP_EC_CFG_CPN_CLIP_PICTURE         0x01      // Mask : configuration data first byte (c): �request for clip picture availability� supported
#define PNCAP_EC_CFG_CPN_RETURN_NAME          0x02      // Mask : configuration data first byte (r): �return name� supported
#define PNCAP_EC_CFG_CPN_VIP_INDEX            0x04      // Mask : configuration data 2nd byte (v): �VIP-CLIP index� supported
#define PNCAP_EC_CFG_CPN_NUMBER_TYPE          0x08      // Mask : configuration data 2nd byte (n): �number icon identifier� supported
#define PNCAP_EC_CFG_CPN_SURNAME              0x10      // Mask : configuration data 2nd byte (s): �surname distinction� supported

/*********************   Acoustic parameter support                  ******/

#define PNCAP_EC_CFG_AC_PAR_AFC_SINGLE_M        0x01      // Mask : Automatic Feedback control Single Mode
#define PNCAP_EC_CFG_AC_PAR_AFC_MULTIM_M        0x02      // Mask : Automatic Feedback control Multimicrophone Mode


/**************************************************************************/
/********             Call Control Req Type                         *******/
/**************************************************************************/

#define PNCAP_CALL_TYPE_DEFAULT            0x00
#define PNCAP_CALL_TYPE_PSTN               0x01      // Public Switched Telephone Network
#define PNCAP_CALL_TYPE_VOIP               0x02      // Voice over IP
#define PNCAP_CALL_TYPE_INT                0x03      // Internal

/**************************************************************************/
/********             Call Control Req Information Fields             *****/
/**************************************************************************/

#define PNCAP_CALL_REQ_IF_PSTN_NUMBER      0x01

/**************************************************************************/
/********             Setup Parameter Information Fields              *****/
/**************************************************************************/

#define PNCAP_SETUP_PAR_INT_HOME_APP_ID    0x01
#define PNCAP_SETUP_PAR_PSTN_MODE          0x02
#define PNCAP_SETUP_PAR_IDLE_SK1_SEL       0x03
#define PNCAP_SETUP_PAR_IDLE_SK2_SEL       0x04
#define PNCAP_SETUP_PAR_IDLE_SKLP1_SEL     0x05
#define PNCAP_SETUP_PAR_IDLE_SKLP2_SEL     0x06
#define PNCAP_SETUP_PAR_BITMASK            0x07

// Setup Par Bitmask:
#define PNCAP_SETUP_PAR_BM_CALL_SIG        0x00 // default call signalling
#define PNCAP_SETUP_PAR_BM_NO_CALL_SIG     0x01 // no call signalling

/**************************************************************************/
/********             Handset Inventory Information Field Codings   *******/
/**************************************************************************/
#define PNCAP_HSID_IPUI_N                  0x00      //
#define PNCAP_HSID_DEVICE_TYPE             0x01      //
#define PNCAP_HSID_VERSION                 0x02      //
#define PNCAP_HSID_PRODNAMESTR             0x03      // Productname
#define PNCAP_HSID_ARTICLE_CODE            0x04      // Article Code Sachnummer
#define PNCAP_HSID_REVISION                0x05      // Product Revision

/**************************************************************************/
/********             BGR Status Information Field Codings          *******/
/**************************************************************************/

#define PNCAP_BGR_SETTINGS_GENERAL_ALARM   0x01
#define PNCAP_BGR_SERV_NO1                 0x02  //service number PP->FP: startup request
                                                 //service number FP->PP: start function test
#define PNCAP_BGR_SERV_NO2                 0x03  //service number PP->FP: function test ok response
                                                 //service number FP->PP: start monitoring
#define PNCAP_BGR_SERV_NO3                 0x04  //service number PP->FP: function test not ok response
                                                 //reserved
#define PNCAP_BGR_SERV_NO4                 0x05  //service number PP->FP: PNG-S reset request
                                                 //service number FP->PP: reset PNG-S
#define PNCAP_BGR_SERV_NO5                 0x06  //service number PP->FP: switch automatic alarms OFF request
                                                 //service number FP->PP: disable automatic alarms
#define PNCAP_BGR_SERV_NO6                 0x07  //service number PP->FP: switch automatic alarms ON request
                                                 //service number FP->PP: enable automatic alarms
#define PNCAP_BGR_SERV_NO7                 0x08  //service number PP->FP: alarm voice call
                                                 //service number FP->PP: alarm recall (handsfree mode / stop alarm repeat)
#define PNCAP_BGR_SERV_NO8                 0x09  //service number PP->FP: reserved
                                                 //service number FP->PP: alarm recall (earpiece mode / stop alarm repeat)
#define PNCAP_BGR_SERV_NO9                 0x0A  //service number PP->FP: reserved
                                                 //service number FP->PP: alarm recall (stop alarm repeat)

#define PNCAP_BGR_SET_DELIBERATE_ALARM     0x10  //settings deliberate emergency alarm
#define PNCAP_BGR_SET_NOMOVEMENT_ALARM     0x11  //settings no movement alarm
#define PNCAP_BGR_SET_MANDOWN_ALARM        0x12  //settings man down alarm
#define PNCAP_BGR_SET_TIME_ALARM           0x13  //settings time alarm
#define PNCAP_BGR_SET_PULLCORD_ALARM       0x14  //settings pull cord alarm
#define PNCAP_BGR_SET_FLIGHT_ALARM         0x15  //settings flight alarm
#define PNCAP_BGR_SET_TECHNICAL_ALARM      0x16  //settings technical alarm

#define PNCAP_BGR_AUDIO_SET_PREALARM_SIG   0x18  //audio settings preliminary alarm signal
#define PNCAP_BGR_AUDIO_SET_TECHALARM_SIG  0x19  //audio settings technical alarm signal
#define PNCAP_BGR_AUDIO_SET_ALARM_SIG      0x1A  //audio settings alarm signal

#define PNCAP_BGR_FCT_TEST_RESULTS         0x1C  //function test results
#define PNCAP_BGR_STATUS_DATA              0x1D  //status data


// SETTINGS_GENERAL_ALARM :
#define PNCAP_BGR_REPEAT_ALARM_CALL        0xff  //repeat alarm count permanent
#define PNCAP_BGR_SINGLE_ALARM_CALL        0x00  //repeat alarm count no repeat, only single alarm cal

// SETTINGS_GENERAL_ALARM Masks
#define PNCAP_BGR_SUSPEND_ALARM_RECALL     0x01  // suspend alarm tone during alarm recall
#define PNCAP_BGR_SUSPEND_ALARM_ALARM_KEY  0x02  // suspend alarm tone by pressing alarm key:
#define PNCAP_BGR_SUSPEND_ALARM_INCHARGER  0x04  // suspend automatic alarms in charger
#define PNCAP_BGR_SUSPEND_ALARM_CTRL_CODE  0x08  // suspend automatic alarms by control call

// Masks
#define PNCAP_BGR_ALARM_ACTIVATED          0x01  // alarm activated
#define PNCAP_BGR_ALARM_SIGNAL_ACTIVATED   0x02  // alarm signal activated
#define PNCAP_BGR_SILENT_ALARM_LP          0x06  // silent alarm when longpress

// Information field �status data�
#define PNCAP_BGR_CHARGING                 0x11  //charging status
#define PNCAP_BGR_NOT_CHARGING             0x10  //charging status

// Highnibble
#define PNCAP_BGR_ACCU                     0x20  // accumulator status
#define PNCAP_BGR_MTBGR_STATUS             0x30  // MT-BGR status value
#define PNCAP_BGR_ALARM_TYPE               0x40  // alarm type

//state
#define PNCAP_BGR_MTBGRST_SHUTDOWN         0x30  // Shut_Down
#define PNCAP_BGR_MTBGRST_CONFIG           0x31  //PNG_S_Configuration
#define PNCAP_BGR_MTBGRST_FCTTEST          0x32  //Function_Test
#define PNCAP_BGR_MTBGRST_MONITOR          0x33  //Monitoring
#define PNCAP_BGR_MTBGRST_MONITOR_ALARM_SP 0x34  //Monitoring (automatic alarms suspended)
#define PNCAP_BGR_MTBGRST_ALARM            0x35  //Alarm

// Alarm
#define PNCAP_BGR_ALARM_NO_ALARM           0x00
#define PNCAP_BGR_ALARM_NO_MOVEMENT_ALARM  0x01
#define PNCAP_BGR_ALARM_MAN_DOWN_ALARM     0x02
#define PNCAP_BGR_ALARM_TIME_ALARM         0x03
#define PNCAP_BGR_ALARM_PULL_CORD_ALARM    0x04
#define PNCAP_BGR_ALARM_FLIGHT_ALARM       0x05
#define PNCAP_BGR_ALARM_EMERGENCY_ALARM    0x06
#define PNCAP_BGR_ALARM_EMERGENCY_ALARM_L  0x07


/**************************************************************************/
/********             FP-Settings Information Field Codings         *******/
/**************************************************************************/

#define PNCAP_FPSET_TIH_CALL_FWRD_NO       0x01      // TIH Call forward number
#define PNCAP_FPSET_TIH_PIN                0x02      // TIH PIN

/**************************************************************************/
/********  PP Configuration Request / Confirm Information Field Codings  **/
/********  PP Settings Exchange Information Field Codings           *******/
/**************************************************************************/

#define PNCAP_PPSET_DEFAULT_COLOUR_SCHEME  0x01
#define PNCAP_PPSET_ONLINE_COLOUR_SCHEME   0x02
#define PNCAP_PPSET_REMOTE_RESET           0x03

// Remote Reset Bitmask (first byte)
#define PNCAP_PPSET_REMOTE_RESET_GENERAL               0x01
#define PNCAP_PPSET_REMOTE_RESET_LOCAL_DIRECTORY       0x02
#define PNCAP_PPSET_REMOTE_RESET_APPOINTMENTS          0x04
#define PNCAP_PPSET_REMOTE_RESET_MISSED_ALARMS         0x08
#define PNCAP_PPSET_REMOTE_RESET_NET_LIST              0x10
#define PNCAP_PPSET_REMOTE_RESET_REDIAL_LIST           0x20
#define PNCAP_PPSET_REMOTE_RESET_KNOWN_DEVICES         0x40
#define PNCAP_PPSET_REMOTE_RESET_PROGRAMMABLE_KEYS     0x80


/**************************************************************************/
/********  Idle Softkey Programming Indication Information Field Codings  */
/**************************************************************************/

#define PNCAP_IDLESK_1_REQ                 0x01  // PNCAP_IDLESK_1_IND
#define PNCAP_IDLESK_2_REQ                 0x02  // PNCAP_IDLESK_2_IND
#define PNCAP_IDLESK_LP1_REQ               0x03  // PNCAP_IDLESK_LP1_IND
#define PNCAP_IDLESK_LP2_REQ               0x04  // PNCAP_IDLESK_LP2_IND
#define PNCAP_IDLE_SCRSV_1_IND             0x05  // Idle screen saver softkey 1 indication
#define PNCAP_IDLE_SCRSV_2_IND             0x06  // Idle screen saver softkey 2 indication
#define PNCAP_IDLE_SCRSV_LP1_IND           0x07  // Idle screen saver softkey Lp1 indication
#define PNCAP_IDLE_SCRSV_LP2_IND           0x08  // Idle screen saver softkey Lp2 indication
#define PNCAP_IDLE_STATUS_TXT_SK1_IND      0x09  // idle status text softkey 1 indication
#define PNCAP_IDLE_STATUS_TXT_SK2_IND      0x0A  // idle status text softkey 2 indication


// Masks
#define PNCAP_IDLESK_ACTION_FREE           0x00  // 'free softkey'
#define PNCAP_IDLESK_ACTION_REQ            0x01  // 'program softkey'

/******************************************************************************/
/******** Idle Softkey Programming Request/Confirm Information Field Codings  */
/******************************************************************************/


#define PNCAP_IDLESK_1_ACTION              0x01
#define PNCAP_IDLESK_2_ACTION              0x02

// Masks
#define PNCAP_IDLESK_RELEASE_REQ           0x01
#define PNCAP_IDLESK_RELEASE_CFM           0x02
#define PNCAP_IDLESK_RELEASE_REJECT        0x04

/*************************************************************************************/
/******** Transparent FP-Feature ID Programming Indication Information Field Codings */
/*************************************************************************************/

#define PNCAP_TRANSPAR_FP_ID               0x01   // FP-Feature Identifier
#define PNCAP_TRANSPAR_FP_LABEL            0x02   // FP-Feature Label
#define PNCAP_TRANSPAR_FP_LINE_ID          0x03   // FP-Feature Line Id (FP-Feature Send Connection)

/**************************************************************************/
/********   Repertory Key Functions Information Field Codings        *******/
/**************************************************************************/

#define PNCAP_RK_NO_FUNCTION               0x01
#define PNCAP_RK_INTERNAL_CALL             0x02
#define PNCAP_RK_EXTERNAL_CALL             0x03

/**************************************************************************/
/********  PCM Mode Information Field Codings                       *******/
/**************************************************************************/

#define PNCAP_PCM_MODE_REQ                 0x01  // PCM Mode request
#define PNCAP_PCM_MODE_CFM                 0x02  // PCM Mode confirm

// values PCM Mode confirm
#define PNCAP_PCM_MCFM_REJECT              0x00
#define PNCAP_PCM_MCFM_4BITADPCM           0x01
#define PNCAP_PCM_MCFM_8BITPCM             0x02

/**************************************************************************/
/********  Release Reason Indication Codings                        *******/
/**************************************************************************/

#define PNCAP_REL_REASON_EMERGENCY_CALL    0x01

/**************************************************************************/
/********  List Access / Data Exchange                              *******/
/**************************************************************************/

// Command codings
#define PNCAP_LA_CMD_ACK                   0x00
#define PNCAP_LA_CMD_DATA_PACKET           0x02
#define PNCAP_LA_CMD_LAST_DATA_PACKET      0x03
#define PNCAP_LA_CMD_READ_ENTRY            0x04

// Cmd Acknowledgement Reasons (PNCAP_LA_CMD_ACK)
#define PNCAP_LA_ACK_POS_CFM               0x00 // Positive acknowledgement
#define PNCAP_LA_ACK_LIST_NOT_AVAILABLE    0x01 // List not available

// Cmd Read List Identifier (PNCAP_LA_CMD_READ_ENTRY)
#define PNCAP_LA_RD_LOC_REPRES_CHARSET     0x80
#define PNCAP_LA_RD_CONTACTS_MENU          0x88
#define PNCAP_LA_RD_TELEPHONY_MENU         0x89
#define PNCAP_LA_RD_SYSTEM_MENU            0x8A
#define PNCAP_LA_RD_AUDIO_SET_MENU         0x8B
#define PNCAP_LA_RD_RESET_MENU             0x8C
#define PNCAP_LA_RD_SETTINGS_MENU          0x8D // settings menua
#define PNCAP_LA_RD_CALL_PROTECTION_MENU   0x8E
#define PNCAP_LA_RD_STATUS_CENTER_MENU     0x8F

// Information Fields / Field identifier
#define PNCAP_LA_FI_ENTRY_ID              0x00 //
#define PNCAP_LA_FI_LIST_ATTRIBUTES       0x01 //
#define PNCAP_LA_FI_LIST_INDEX            0x02 //


#define PNCAP_LA_FI_MENU_TEXT             0x81 // Menu Text
#define PNCAP_LA_FI_FP_FEATURE_ID         0x82 // Fp Feature Identifier
#define PNCAP_LA_FI_MENU_STATUS           0x83 // menu status
#define PNCAP_LA_FI_MENU_STATUS_TXT       0x84 // menu status text
#define PNCAP_LA_FI_2ND_LINE_MENU_TXT     0x85 // second line menu text

#define PNCAP_LA_FI_JUPITER_STDBY_LD_TXT  0x88 // Jupiter Hs Display size, 16 colordepth, standby loading text
#define PNCAP_LA_FI_JUPITER_GR_PART1      0x89 // Jupiter Hs Display size, 16 colordepth, Graphic Part 1
#define PNCAP_LA_FI_JUPITER_GR_PART2      0x8A // Jupiter Hs Display size, 16 colordepth, Graphic Part 2
#define PNCAP_LA_FI_JUPITER_GR_PART3      0x8B // Jupiter Hs Display size, 16 colordepth, Graphic Part 3
#define PNCAP_LA_FI_JUPITER_GR_PART4      0x8C // Jupiter Hs Display size, 16 colordepth, Graphic Part 4
#define PNCAP_LA_FI_JUPITER_GR_PART5      0x8D // Jupiter Hs Display size, 16 colordepth, Graphic Part 5

/**************************************************************************/
/********   Proprietary CPN and clip picture  Configuration Data    *******/
/**************************************************************************/

// First octet masks
#define PNCAP_PCPN_PIC_REQ                0x40  // Picture available ?
#define PNCAP_PCPN_NAME_REQ               0x80  // Name required

// Second octet masks
#define PNCAP_PCPN_VIP_IND_REQ            0x01  // VIP index required
#define PNCAP_PCPN_N_TYPE_REQ             0x02  // number icon identifier (n) (number type)
#define PNCAP_PCPN_SURNAME_FORENAME_REQ   0x04  // surname distinction required (s)

/**************************************************************************/
/********   Name of Calling Party and CLIP picture                  *******/
/**************************************************************************/

// configuration data third byte

// vvvv: No vip-clip index available
#define PNCAP_NACP_VIPIND_NOT_AVAILABLE   0x0F  // vvvv: No vip-clip index available

// nnnn : number icon identifier (nnnn)
#define PNCAP_NACP_NUMBER_TYPE_HOME           0x00     // nnnn: home number icon identifier
#define PNCAP_NACP_NUMBER_TYPE_MOBILE         0x10     // nnnn: mobile number icon identifier
#define PNCAP_NACP_NUMBER_TYPE_OFFICE         0x20     // nnnn: office number icon identifier
#define PNCAP_NACP_NUMBER_TYPE_NOT_AVAILABLE  0xF0     // nnnn: no number icon identifier


/**************************************************************************/
/********             Info Fields List access attributes             *******/
/**************************************************************************/

#define PNCAP_LAA_DFT_CHARSET             0x01  // default characterset

/**************************************************************************/
/********             Info Fields Info Display                      *******/
/**************************************************************************/

#define PNCAP_INFO_D_HEADLINE            0x01  // headline (out of: 0x1b, 0x8f, G0, G1, G3)
#define PNCAP_INFO_D_TEXT                0x02  // text (out of: 0x0a, 0x0c, 0x0d, 0x1b, 0x8f, G0, G1, G3)
#define PNCAP_INFO_D_LOC_PRES            0x03  // local representation coding
#define PNCAP_INFO_D_SK_1                0x04  // softkey 1 label (out of: 0x1b, 0x8f, G0, G1, G3)
#define PNCAP_INFO_D_SK_2                0x05  // softkey 2 label (out of: 0x1b, 0x8f, G0, G1, G3)
#define PNCAP_INFO_D_CONFIG_DISPLAY      0x06  // config info-display

// Info Field config info-display
#define PNCAP_INFO_D_CFG_CLEAR           0x00  // config info-display: clear info-display
#define PNCAP_INFO_D_CFG_HIDE            0x01  // config info-display: hide info-display
#define PNCAP_INFO_D_CFG_SHOW            0x02  // config info-display: show info-display

/**************************************************************************/
/********             Info Field Entry Tag / Last Entry Tag         *******/
/**************************************************************************/

#define PNCAP_IE_ENTRY_TAG        0x01
#define PNCAP_IE_LAST_ENTRY_TAG   0x02

// Values for media type
#define PNCAP_EI_MEDIA_TYPE_DISPLAY_PAGE    0x01           // complete display page (formats and elements)
#define PNCAP_EI_MEDIA_TYPE_GRAPHIC_JPG     0x02           // graphic element (jpeg)

// Values for validity
#define PNCAP_EI_VALIDITY_VALID             0x01           // valid
#define PNCAP_EI_VALIDITY_OBSOLETE          0x02           // obsolete not to be used within local representation of base provided graphic
#define PNCAP_EI_VALIDITY_UPDATE_REQ        0x03           // entry id is valid but update is required

/**************************************************************************/
/********             GigasetSISSP page stream content              *******/
/**************************************************************************/

// Prefix Simple Info Service Slide Show page
// Only works for Ascii characterset!
#define PNCAP_PREFIX_SEQ_GIGASETSISSP   {'G','i','g','a','s','e','t','S','I','S','S','P'}

#define PNCAP_SISSP_MULTIDISPLAY        0x28
#define PNCAP_SISSP_ETP                 0x7B


/**************************************************************************/
/********     Character Format Codings switching within FSE         *******/
/**************************************************************************/

// FSE Character Format
#define PNCAP_FSE_CHAR_FOR_HIGHLIGHT_ON         0x01    // highlight on
#define PNCAP_FSE_CHAR_FOR_HIGHLIGHT_OFF        0x02    // highlight off


/**************************************************************************/
/********             Characterset C0                               *******/
/**************************************************************************/
#define PNCAP_C0_RET_HOME   0x02
#define PNCAP_C0_RET_END    0x03
#define PNCAP_C0_CRS_LEFT   0x08
#define PNCAP_C0_CRS_RIGHT  0x09
#define PNCAP_C0_CRS_DOWN   0x0a
#define PNCAP_C0_CRS_UP     0x0b
#define PNCAP_C0_CLR        0x0c
#define PNCAP_C0_RET        0x0d
#define PNCAP_C0_FLASH_OFF  0x0e
#define PNCAP_C0_FLASH_ON   0x0f
#define PNCAP_C0_COD        0x19
#define PNCAP_C0_COL        0x1a
#define PNCAP_C0_ESCAPE     0x1b


#define PNCAP_C0_ESC_CHAR_SET_CHINESE   0x24
#define PNCAP_C0_ESC_CHAR_SET           0x2D


#define PNCAP_C0_ESC_CHINESE    0x31  // GB 2312: Chinese characters
#define PNCAP_C0_ESC_FARSI      0x37  // PNCAP: ISO 8859-6 + FARSI characters

#define PNCAP_C0_ESC_LATIN1     0x41  // ISO 8859-1
#define PNCAP_C0_ESC_LATIN2     0x42  // ISO 8859-2
#define PNCAP_C0_ESC_GREEK      0x46  // ISO 8859-7
#define PNCAP_C0_ESC_ARABIC     0x47  // ISO 8859-6
#define PNCAP_C0_ESC_HEBREW     0x48  // ISO 8859-8
#define PNCAP_C0_ESC_CYRILLIC   0x4C  // ISO 8859-5
#define PNCAP_C0_ESC_TURKISH    0x4D  // ISO 8859-9





// Escape Sequenz Latin 1
#define PNCAP_ESC_SEQ_LATIN1     {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_LATIN1}

// Escape Sequenz Latin 2
#define PNCAP_ESC_SEQ_LATIN2     {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_LATIN2}

// Escape Sequenz Griechisch
#define PNCAP_ESC_SEQ_GREEK      {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_GREEK}

// Escape Sequenz Arabic
#define PNCAP_ESC_SEQ_ARABIC     {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_ARABIC}

// Escape Sequenz Hebrew
#define PNCAP_ESC_SEQ_HEBREW     {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_HEBREW}

// Escape Sequenz Kyrillisch
#define PNCAP_ESC_SEQ_CYRILLIC   {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_CYRILLIC}

// Escape Sequenz Turkish
#define PNCAP_ESC_SEQ_TURKISH    {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_TURKISH}

// Escape Sequenz Farsi
#define PNCAP_ESC_SEQ_FARSI      {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET,PNCAP_C0_ESC_FARSI}

// Escape Sequenz GB2312 (Chinese)
#define PNCAP_ESC_SEQ_CHINESE    {PNCAP_C0_ESCAPE,PNCAP_C0_ESC_CHAR_SET_CHINESE,PNCAP_C0_ESC_CHINESE}


/**************************************************************************/
/********             Characterset C1                               *******/
/**************************************************************************/

#define PNCAP_C1_GOTOXY               0x81
#define PNCAP_C1_BLINK_ON             0x83
#define PNCAP_C1_BLINK_OFF            0x84
#define PNCAP_C1_SET_PICTO_ATTR       0x88
#define PNCAP_C1_MOVE                 0x89
#define PNCAP_C1_BLINK_LINE_ON        0x8a
#define PNCAP_C1_BLINK_LINE_OFF       0x8b
#define PNCAP_C1_SUP_BDI_ALG          0x8c
#define PNCAP_C1_ESC_TWO_BYTE_CHARSET 0x8d
#define PNCAP_C1_SINGLE_SHIFT_TWO     0x8e
#define PNCAP_C1_SINGLE_SHIFT_THREE   0x8f
#define PNCAP_C1_DEFINE_PARAGRAPH     0x91
#define PNCAP_C1_ESCAPE_TO_EDITOR     0x92
#define PNCAP_C1_CRS_ON               0x93
#define PNCAP_C1_CRS_OFF              0x94
#define PNCAP_C1_UNDERLINE_ON         0x95
#define PNCAP_C1_UNDERLINE_OFF        0x96
#define PNCAP_C1_PP_DEFINED_TEXT      0x99
#define PNCAP_C1_REVERS_ON            0x9a
#define PNCAP_C1_REVERS_OFF           0x9b
#define PNCAP_C1_CONFIG               0x9c
#define PNCAP_C1_FEATURE_NAME         0x9d
#define PNCAP_C1_DISPLAY_DFLT         0x9e

#define PNCAP_KEY_SINGLE_SHIFT_TWO    PNCAP_C1_SINGLE_SHIFT_TWO
#define PNCAP_KEY_SINGLE_SHIFT_THREE  PNCAP_C1_SINGLE_SHIFT_THREE

/**************************************************************************/
/********             PNCAP_C1_ESC_TWO_BYTE_CHARSET                 *******/
/**************************************************************************/

#define PNCAP_C1_ESC_2B_CHARSET_CHINESE_GB2312  0x01



/**************************************************************************/
/********             Icon Character Set                            *******/
/**************************************************************************/

#define PNCAP_C1_ICON_HOOK_OFF       0xA0
#define PNCAP_C1_ICON_INTERN         0xA1
#define PNCAP_C1_ICON_EXTERN         0xA2
#define PNCAP_C1_ICON_KEY_LOCK       0xA8
#define PNCAP_C1_ICON_CSR_UP         0xAF // will always be shown simultaneously...
#define PNCAP_C1_ICON_CSR_DN         0xB0 // ... with this one
#define PNCAP_C1_ICON_OK             0xB1
#define PNCAP_C1_ICON_MWI            0xB2
#define PNCAP_C1_ICON_AM             0xB3
#define PNCAP_C1_ICON_CSR_LFT        0xB4 // will alway be shown simultaneously ...
#define PNCAP_C1_ICON_CSR_RGT        0xB5 // ... with this one
#define PNCAP_C1_ICON_REDIAL         0xB6
#define PNCAP_C1_ICON_BS             0xB7
#define PNCAP_C1_ICON_INT_KEY        0xB8
#define PNCAP_C1_ICON_MENU           0xB9
#define PNCAP_C1_ICON_NO_DIST        0xBA
#define PNCAP_C1_ICON_IP             0xBB
#define PNCAP_C1_ICON_ESC            0xBC // Return
#define PNCAP_C1_ICON_ALERTING       0xBD // Alerting

/**************************************************************************/
/********             Icon Attributes                               *******/
/**************************************************************************/
#define PNCAP_C1_ICON_ATTR_OFF       0x00
#define PNCAP_C1_ICON_ATTR_ON        0x01
#define PNCAP_C1_ICON_ATTR_BLINK     0x02
#define PNCAP_C1_ICON_ATTR_LOCAL     0x03


/**************************************************************************/
/********             Display Configuration  PNCAP_C1_CONFIG        *******/
/**************************************************************************/

#define PNCAP_C1_CFG_ATTR_STD        0x00
#define PNCAP_C1_CFG_ATTR_SEL        0x01
#define PNCAP_C1_CFG_ATTR_HDL        0x02
#define PNCAP_C1_CFG_ATTR_TYP        0x03
#define PNCAP_C1_CFG_ATTR_BLBORDER   0x04
#define PNCAP_C1_CFG_ATTR_FRAME      0x05
#define PNCAP_C1_CFG_ATTR_COLOUR     0x06
#define PNCAP_C1_CFG_ATTR_FONT_SIZE  0x07
#define PNCAP_C1_CFG_ATTR_LIST       0x08
#define PNCAP_C1_CFG_ATTR_BG_COL     0x09  // background colour
#define PNCAP_C1_CFG_ATTR_VIP_IND    0x0A  // VIP-CLIP index

// Type
#define PNCAP_C1_CFG_ATTR_TYP_NEUTRAL    0x03
#define PNCAP_C1_CFG_ATTR_TYP_OLD        0x13
#define PNCAP_C1_CFG_ATTR_TYP_NEW        0x23

// Block
#define PNCAP_C1_CFG_ATTR_BLBORDER_DEL           0x04
#define PNCAP_C1_CFG_ATTR_BLBORDER_BEGIN         0x14
#define PNCAP_C1_CFG_ATTR_BLBORDER_END           0x24
#define PNCAP_C1_CFG_ATTR_BLBORDER_BEGIN_LIST    0x34
#define PNCAP_C1_CFG_ATTR_BLBORDER_MIDDLE        0x44

// Frame
#define PNCAP_C1_CFG_ATTR_FRAME_DEL       0x05
#define PNCAP_C1_CFG_ATTR_FRAME_BEGIN     0x15
#define PNCAP_C1_CFG_ATTR_FRAME_END       0x25
#define PNCAP_C1_CFG_ATTR_FRAME_ONE_LINE  0x35
#define PNCAP_C1_CFG_ATTR_FRAME_MAX_SIZE  0x45

// Colour
#define PNCAP_C1_CFG_ATTR_COLOUR_DFLT     0x06  // Default Colour
#define PNCAP_C1_CFG_ATTR_COLOUR_SEL      0x16  // Selected Colour

// Font Size
#define PNCAP_C1_CFG_ATTR_FONT_SIZE_BIG    0x07
#define PNCAP_C1_CFG_ATTR_FONT_SIZE_SMALL  0x17
#define PNCAP_C1_CFG_ATTR_FONT_SIZE_XXL    0x27

// List
#define PNCAP_C1_CFG_ATTR_LIST_BEGIN                        0x08
#define PNCAP_C1_CFG_ATTR_LIST_END                          0x18
#define PNCAP_C1_CFG_ATTR_LIST_BEGIN_PRECEDING_ELEMENTS     0x28
#define PNCAP_C1_CFG_ATTR_LIST_END_FOLLOWING_ELEMENTS       0x38
#define PNCAP_C1_CFG_ATTR_LIST_FOL_AND_PREC_ELEMENTS        0x48   // Item with following and preceding elements

// background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_BLACK       0x09  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_MAROON      0x19  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_GREEN       0x29  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_OLIVE       0x39  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_NAVY        0x49  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_PURPLE      0x59  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_CYAN        0x69  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_GRAY        0x79  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_SILVER      0x89  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_RED         0x99  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_LIME        0xA9  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_YELLOW      0xB9  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_BLUE        0xC9  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_MAGENTA     0xD9  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_AQUA        0xE9  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_WHITE       0xF9  // background colour
#define PNCAP_C1_CFG_ATTR_BACK_COLOUR_MASK        0xF0  // background colour


/**************************************************************************/
/********             Alignment                                     *******/
/**************************************************************************/

#define PNCAP_C1_ALIGN_MASK                       0x0F  // alignment mask

#define PNCAP_C1_ALIGN_LEFT                       0x01  // aligned left in x+1 rows and cut right in last row
#define PNCAP_C1_ALIGN_CENTERED                   0x02  // aligned centered in x+1 rows
#define PNCAP_C1_ALIGN_RIGHT                      0x03  // aligned right in x+1 rows and cut left in first row
#define PNCAP_C1_ALIGN_LEFT_CUT_LEFT              0x04  // aligned left in x+1 rows and cut left in first row
#define PNCAP_C1_ALIGN_RIGHT_CUT_RIGHT            0x05  // aligned right in x+1 rows and cut right in last row

/**************************************************************************/
/********             Display Character Set  G3                     *******/
/**************************************************************************/

#define PNCAP_G3_DSP_KEY_PAD_LOCKED1              0xA0  // Key pad locked
#define PNCAP_G3_DSP_KEY_PAD_LOCKED2              0xB0  // Key pad locked
#define PNCAP_G3_DSP_ACT_IND                      0xC0  // Activation indication
#define PNCAP_G3_DSP_ABBR                         0xD0  // Abbrevation
// reserved #define PNCAP_G3_DSP_                    0xE0
#define PNCAP_G3_DSP_EURO                         0xF0  // Euro
#define PNCAP_G3_DSP_BACKSPACE1                   0xA1  // Backspace
#define PNCAP_G3_DSP_BACKSPACE2                   0xB1  // Backspace
#define PNCAP_G3_DSP_THREE_DOTS                   0xC1  // Three Dots
#define PNCAP_G3_DSP_MOUTH_IND                    0xD1  // mouth indication
// reserved #define PNCAP_G3_DSP_    0xE1
// reserved #define PNCAP_G3_DSP_    0xF1
#define PNCAP_G3_DSP_AKZ                          0xA2   // AKZ
#define PNCAP_G3_DSP_HKZ                          0xB2   // HKZ
// reserved #define PNCAP_G3_DSP_    0xC2
// reserved #define PNCAP_G3_DSP_    0xD2
// reserved #define PNCAP_G3_DSP_    0xE2
// reserved #define PNCAP_G3_DSP_    0xF2
#define PNCAP_G3_DSP_EMPTY_MEMORY                 0xA3  // Empty Memory
#define PNCAP_G3_DSP_R                            0xB3  // R
// reserved #define PNCAP_G3_DSP_    0xC3
// reserved #define PNCAP_G3_DSP_    0xD3
// reserved #define PNCAP_G3_DSP_    0xE3
// reserved #define PNCAP_G3_DSP_    0xF3
#define PNCAP_G3_DSP_U                            0xA4  //
#define PNCAP_G3_DSP_P                            0xB4  //
// reserved #define PNCAP_G3_DSP_    0xC4
// reserved #define PNCAP_G3_DSP_    0xD4
// reserved #define PNCAP_G3_DSP_    0xE4
// reserved #define PNCAP_G3_DSP_    0xF4
#define PNCAP_G3_DSP_MENU_OPT1                    0xA5  // Menu Option, Context menu, F+
#define PNCAP_G3_DSP_MENU_OPT2                    0xB5  // Menu Option, Context menu, F+
#define PNCAP_G3_DSP_MENU_OPT3                    0xC5  // Menu Option, Context menu, F+
// reserved #define PNCAP_G3_DSP_    0xD5
// reserved #define PNCAP_G3_DSP_    0xE5
// reserved #define PNCAP_G3_DSP_    0xF5
// reserved #define PNCAP_G3_DSP_    0xA6
// reserved #define PNCAP_G3_DSP_    0xB6
#define PNCAP_G3_DSP_TD                           0xC6  // DEUTSCHE TELEKOM DOT
// reserved #define PNCAP_G3_DSP_    0xD6
// reserved #define PNCAP_G3_DSP_    0xE6
// reserved #define PNCAP_G3_DSP_    0xF6
#define PNCAP_G3_DSP_PHONE_BOOK1                  0xA7  //Telephone Book
#define PNCAP_G3_DSP_PHONE_BOOK2                  0xB7
#define PNCAP_G3_DSP_PHONE_BOOK3                  0xC7
// reserved #define PNCAP_G3_DSP_A_DOWN                         0xD7  // Arrow down
// reserved #define PNCAP_G3_DSP_    0xE7
// reserved #define PNCAP_G3_DSP_    0xF7
#define PNCAP_G3_DSP_MWI1                         0xA8
#define PNCAP_G3_DSP_MWI2                         0xB8
#define PNCAP_G3_DSP_MWI3                         0xC8
#define PNCAP_G3_DSP_OFF_IND                      0xD8  // Off indication
// reserved #define PNCAP_G3_DSP_    0xE8
// reserved #define PNCAP_G3_DSP_    0xF8
#define PNCAP_G3_DSP_LNR1                         0xA9  // Last Number Redial
#define PNCAP_G3_DSP_LNR2                         0xB9
#define PNCAP_G3_DSP_LNR3                         0xC9
#define PNCAP_G3_DSP_ESC_BUDDY_ICONS              0xD9  // escape to buddy pictograms
// reserved #define PNCAP_G3_DSP_    0xE9
// reserved #define PNCAP_G3_DSP_    0xF9
#define PNCAP_G3_DSP_PAUSE1                       0xAA  // Pause
#define PNCAP_G3_DSP_PAUSE2                       0xBA
#define PNCAP_G3_DSP_PAUSE3                       0xCA
#define PNCAP_G3_DSP_ESC_2BYTE_ICONS              0xDA  // escape to two byte pictograms
// reserved #define PNCAP_G3_DSP_    0xEA
// reserved #define PNCAP_G3_DSP_    0xFA
#define PNCAP_G3_DSP_UP                           0xAB  // Scroll up
#define PNCAP_G3_DSP_DOWN                         0xBB  // Scroll down
// reserved #define PNCAP_G3_DSP_    0xCB
#define PNCAP_G3_DSP_ESC_BASE_PROV_PICTO          0xDB  // Escape to base provided pictograms
// reserved #define PNCAP_G3_DSP_    0xEB
// reserved #define PNCAP_G3_DSP_    0xFB
#define PNCAP_G3_DSP_CRS_LEFT                     0xAC  // Cursor left
#define PNCAP_G3_DSP_CRS_RIGHT                    0xBC  // Cursor right
// reserved #define PNCAP_G3_DSP_    0xCC
// reserved #define PNCAP_G3_DSP_    0xDC
// reserved #define PNCAP_G3_DSP_    0xEC
// reserved #define PNCAP_G3_DSP_    0xFC
#define PNCAP_G3_DSP_ESCAPE1                      0xAD  // Escape
#define PNCAP_G3_DSP_ESCAPE2                      0xBD
#define PNCAP_G3_DSP_ESCAPE3                      0xCD
#define PNCAP_G3_DSP_MELODY1                      0xDD  // Melody
#define PNCAP_G3_DSP_MELODY2                      0xED
// reserved #define PNCAP_G3_DSP_    0xFD
#define PNCAP_G3_DSP_AM_MENU1                     0xAE  // Answering machine menu
#define PNCAP_G3_DSP_AM_MENU2                     0xBE  // Answering machine menu
#define PNCAP_G3_DSP_AM_MENU3                     0xCE  // Answering machine menu
// reserved #define PNCAP_G3_DSP_    0xDE
// reserved #define PNCAP_G3_DSP_    0xEE
// reserved #define PNCAP_G3_DSP_    0xFE
#define PNCAP_G3_DSP_CARRIER_SEL1                 0xAF  // Carrier selection
#define PNCAP_G3_DSP_CARRIER_SEL2                 0xBF  // Carrier selection
#define PNCAP_G3_DSP_CARRIER_SEL3                 0xCF  // Carrier selection
// reserved #define PNCAP_G3_DSP_    0xDF
// reserved #define PNCAP_G3_DSP_    0xEF
// reserved #define PNCAP_G3_DSP_    0xFF


/**************************************************************************/
/********             Display Character Set G3 Buddy pictograms     *******/
/**************************************************************************/

#define PNCAP_G3_DSP_D9_BUDDY_UNKNOWN              0x00  // Yellow
#define PNCAP_G3_DSP_D9_BUDDY_OFFLINE              0x01  // Red
#define PNCAP_G3_DSP_D9_BUDDY_ONLINE               0x02  // Green

/**************************************************************************/
/********             Display Character Set G3 Two-byte pictograms  *******/
/**************************************************************************/

// #define  0x00  // reserved
#define PNCAP_G3_DSP_DA_INTERNAL_CALL              0x01
#define PNCAP_G3_DSP_DA_INCOMING_CALL              0x02
#define PNCAP_G3_DSP_DA_CALL                       0x03     // call
#define PNCAP_G3_DSP_DA_ANSWERED_CALL              0x04     // answered call
#define PNCAP_G3_DSP_DA_UNANSWERED_CALL            0x05     // unanswered call
#define PNCAP_G3_DSP_DA_DIALED_NUMBER              0x06     // dialed number
#define PNCAP_G3_DSP_DA_ANSWERING_MACHINE          0x07     // answering machine
#define PNCAP_G3_DSP_DA_HOME_NUMBER                0x08     // home number
#define PNCAP_G3_DSP_DA_MOBILE_NUMBER              0x09     // mobile number
#define PNCAP_G3_DSP_DA_OFFICE_NUMBER              0x0A     // office number
#define PNCAP_G3_DSP_DA_RADIOBUTTON_SEL            0x0B     // Radiobutton (selected)
#define PNCAP_G3_DSP_DA_RADIOBUTTON_NOT_SEL        0x0C     // Radiobutton (not selected)
#define PNCAP_G3_DSP_DA_CHECKMARK_WITHOUT_BOX      0x0D     // check mark (activation indication without checkbox)
#define PNCAP_G3_DSP_DA_INT_CALL_IND               0x0E     // Internal Caller Indication
#define PNCAP_G3_DSP_DA_RTONE_SINUS                0x0F     // ring tone (sinus 1)
#define PNCAP_G3_DSP_DA_RTONE_MONO                 0x10     // ring tone (monophone)
#define PNCAP_G3_DSP_DA_RTONE_POLY                 0x11     // ring tone (polyphone)
#define PNCAP_G3_DSP_DA_RTONE_AUDIO_FILE           0x12     // ring tone (imported audio file)
#define PNCAP_G3_DSP_DA_RTONE_LOCKED               0x13     // ring tone (locked)
#define PNCAP_G3_DSP_DA_HOME_CALLSTATE             0x14     // home (in call states)
#define PNCAP_G3_DSP_DA_BELL_CALL_WAITING          0x15     // bell (in call states / call waiting)
#define PNCAP_G3_DSP_DA_HANDSET_CALLSTATE          0x16     // handset (in call states / call swap / conference)
#define PNCAP_G3_DSP_DA_CALL_LIST_SPACE            0x17     // Space
#define PNCAP_G3_DSP_DA_SECURE_CALL                0x18     // Secure Call
#define PNCAP_G3_DSP_DA_HDSP                       0x19     // High Definition Sound Performance
#define PNCAP_G3_DSP_DA_INFO_CHARACTER             0x1A     // Info Character

/**************************************************************************/
/********             Keypad Character Set  G3                      *******/
/**************************************************************************/
#define PNCAP_G3_KEY_AM              0xA0
#define PNCAP_G3_KEY_PLAY            0xA1
#define PNCAP_G3_KEY_SKIP_BACK_L     0xA2   // Skip Back key long press
#define PNCAP_G3_KEY_SKIP_BACK       0xA3   // Skip Back key short press
#define PNCAP_G3_KEY_REPERTORY_1     0xA4   // Repertory Key 1
#define PNCAP_G3_KEY_REPERTORY_2     0xA5   // Repertory Key 2
#define PNCAP_G3_KEY_REPERTORY_3     0xA6   // Repertory Key 3
#define PNCAP_G3_KEY_REPERTORY_4     0xA7   // Repertory Key 4
#define PNCAP_G3_KEY_REC_ON_HOOK     0xA8   // Receiver on hook (receiver may also be headset)
#define PNCAP_G3_KEY_SEND_CON        0xA9   // Send Connection
#define PNCAP_G3_KEY_LINE_SEL        0xAA   // Line Selection
#define PNCAP_G3_KEY_PLAY_L          0xAB   // Play Key Longpress

#define PNCAP_G3_KEY_SK1             0xB0
#define PNCAP_G3_KEY_SK2             0xB1
#define PNCAP_G3_KEY_MENU_KEY        0xB2
#define PNCAP_G3_KEY_SERVICE_RR      0xB3
#define PNCAP_G3_KEY_LNR             0xB4
#define PNCAP_G3_KEY_SK3             0xB5
#define PNCAP_G3_KEY_SK4             0xB6
#define PNCAP_G3_KEY_TUMBLEPADUP     0xB7
#define PNCAP_G3_KEY_TUMBLEPADDOWN   0xB8
#define PNCAP_G3_KEY_TUMBLEPADLEFT   0xB9
#define PNCAP_G3_KEY_TUMBLEPADRIGHT  0xBA

#define PNCAP_G3_KEY_OFF_HOOK_KEY    0xC0
#define PNCAP_G3_KEY_PHONE_BOOK_KEY  0xC1
#define PNCAP_G3_KEY_NET_LIST_KEY    0xC3
#define PNCAP_G3_KEY_MWI_KEY         0xC4
#define PNCAP_G3_KEY_ON_HOOK_KEY     0xC5
#define PNCAP_G3_KEY_PUSHTOTALK_KEY  0xC6
//#define PNCAP_G3_KEY_T_SERVICE_KEY   0xC7
#define PNCAP_G3_KEY_PLUS            0xC7
#define PNCAP_G3_KEY_MINUS           0xC8
#define PNCAP_G3_KEY_VOLUME          0xC9
#define PNCAP_G3_KEY_CENTER          0xCA
#define PNCAP_G3_KEY_ALT_OFF_HOOK    0xCB
#define PNCAP_G3_KEY_ALT_SPEAKER     0xCC
#define PNCAP_G3_KEY_NETWORK_DIR     0xCD
#define PNCAP_G3_KEY_MUTE_KEY        0xCE
#define PNCAP_G3_KEY_CMB_SEND_HF     0xCF


#define PNCAP_G3_KEY_OFF_CHARGER_KEY 0xD4
#define PNCAP_G3_KEY_ON_CHARGER_KEY  0xD5
//#define PNCAP_G3_KEY_              0xD6
//#define PNCAP_G3_KEY_              0xD7
//#define PNCAP_G3_KEY_              0xD8
//#define PNCAP_G3_KEY_              0xD9
#define PNCAP_G3_KEY_SPEAKER_KEY     0xDA
//#define PNCAP_G3_KEY_CALL_LOG_     0xDB
//#define PNCAP_G3_KEY_AUTO_LINE_SEL   0xDC

#define PNCAP_G3_KEY_TUMBLEPADLEFT_L 0xDD   // longpress tumble pad left
#define PNCAP_G3_KEY_REDIAL_KEY      0xDE


#define PNCAP_G3_KEY_EURO            0xF0



/**************************************************************************/
/********             Keypad Character Set GAP                      *******/
/**************************************************************************/

#define GAP_KEY_BELEG      0xa0
#define GAP_KEY_INTERN     0x17
#define GAP_KEY_RR         0x15
#define GAP_KEY_STERN      0x2a
#define GAP_KEY_RAUTE      0x23
#define GAP_KEY_PAUSE      0x05
#define GAP_KEY_SERVICE    0x18
#define GAP_KEY_1          0x31
#define GAP_KEY_2          0x32
#define GAP_KEY_3          0x33
#define GAP_KEY_4          0x34
#define GAP_KEY_5          0x35
#define GAP_KEY_6          0x36
#define GAP_KEY_7          0x37
#define GAP_KEY_8          0x38
#define GAP_KEY_9          0x39
#define GAP_KEY_0          0x30

#define GAP_KEY_GOTO_PULSE  0x12
#define GAP_KEY_GOTO_DTMF   0x14


/**************************************************************************/
/********             IE - SIGNAL                                   *******/
/**************************************************************************/

#define GAP_IE_SIGNAL_DIAL_TONE_ON              0x00  //
#define GAP_IE_SIGNAL_RINGBACK_TONE_ON          0x01  //
#define GAP_IE_SIGNAL_WRONG_DIALLING_TONE_ON    0x03  //  Network congestion
#define GAP_IE_SIGNAL_BUSYTONE_ON               0x04  //
#define GAP_IE_SIGNAL_DIAL_READY_TONE_ON        0x05  //  Confirm
#define GAP_IE_SIGNAL_CALL_WAITING_TONE_ON      0x07  //
#define GAP_IE_SIGNAL_TONES_OFF                 0x3F  //


/**************************************************************************/
/********            CALL STATE                                      ******/
/**************************************************************************/

#define PNCAP_CALL_STATE_NULL                               0x00  // null,                        call phase: IDLE
#define PNCAP_CALL_STATE_CALL_INITIATED                     0x01  // call initiated,              call phase: OUTGOING CALL
#define PNCAP_CALL_STATE_OVERLAP_SENDING                    0x02  // overlap sending,             call phase: OUTGOING CALL
#define PNCAP_CALL_STATE_OUTGOING_CALL_PROCEEDING           0x03  // outgoing call proceeding,    call phase: OUTGOING CALL
#define PNCAP_CALL_STATE_CALL_DELIVERED                     0x04  // call delivered,              call phase: OUTGOING CALL
#define PNCAP_CALL_STATE_CALL_PRESENT                       0x05  // call present,                call phase: INCOMING CALL
#define PNCAP_CALL_STATE_OVERLAP_RECEIVING                  0x06  // overlap receiving,           call phase: INCOMING CALL
#define PNCAP_CALL_STATE_INCOMING_CALL_PROCEEDING           0x07  // incoming call proceeding,    call phase: INCOMING CALL
#define PNCAP_CALL_STATE_CALL_RECEIVED                      0x08  // call received,               call phase: INCOMING CALL
#define PNCAP_CALL_STATE_CONNECT_REQUEST                    0x09  // connect request,             call phase: INCOMING CALL
#define PNCAP_CALL_STATE_ACTIVE                             0x0A  // active,                      call phase: ACTIVE
#define PNCAP_CALL_STATE_DISCONNECT_REQUEST                 0x0B  // disconnect request,          call phase: CALL CLEARING
#define PNCAP_CALL_STATE_DISCONNECT_INDICATION              0x0C  // disconnect indication,       call phase: CALL CLEARING
#define PNCAP_CALL_STATE_RELEASE_REQUEST                    0x0D  // release request,             call phase: CALL CLEARING
#define PNCAP_CALL_STATE_RESUME_REQUEST                     0x0E  // resume request,              call phase: RESTART PROCEDURE



#endif /* PNCAP_CODES_H_ */
////////////////////////////////////////////////////////////////////////////////////////////////////
//  EOF PNCAP_CODES_H                                                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////
