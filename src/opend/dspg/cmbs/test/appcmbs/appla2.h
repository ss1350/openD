

#define APP_LA_MAX_NR_OF_SESSIONS            10
#define CMBS_UTIL_LA_MAX_BUFFER 2000 // Max 2K buffer for sending and receiving

typedef struct
{
    u8 u8_EntryFilled;
    u16 u16_SessionId;
    u16 u16_CMBS_LAEntryNr;
    u16 u16_CMBS_LAEndIndex;
    u16 u16_ListId;
    u16 u16_requestedFields;
    u8 u8_finalbuffer[CMBS_UTIL_LA_MAX_BUFFER];
    u16 u16_SizeToSend;
    u16 u16_Index;
    u8 u8_IsInSaveMode; /* Related to SAVE command TRUE/FALSE */
} st_APP_LA_Session, *pst_APP_LA_Session;

st_APP_LA_Session g_st_APP_LA_Session[APP_LA_MAX_NR_OF_SESSIONS];


typedef enum
{
    CMBS_UTIL_LA_FLD_NUMBER,           /*!< List entry field 'Number' */
    CMBS_UTIL_LA_FLD_NAME,             /*!< List entry field 'Name' */
    CMBS_UTIL_LA_FLD_DATETIME,         /*!< List entry field 'Date and time' */
    CMBS_UTIL_LA_FLD_NEW,              /*!< List entry field 'New' */
    CMBS_UTIL_LA_FLD_LINE_NAME,        /*!< List entry field 'Line name' */
    CMBS_UTIL_LA_FLD_LINE_ID,          /*!< List entry field 'Line Id' */
    CMBS_UTIL_LA_FLD_NR_OF_CALLS,      /*!< List entry field 'Number of calls' */
    CMBS_UTIL_LA_FLD_CALL_TYPE,        /*!< List entry field 'Call type' */
    CMBS_UTIL_LA_FLD_FIRST_NAME,       /*!< List entry field 'First name' */
    CMBS_UTIL_LA_FLD_CONTACT_NR,       /*!< List entry field 'Contact number' */
    CMBS_UTIL_LA_FLD_ASSOC_MDY,        /*!< List entry field 'Associated melody' */
    CMBS_UTIL_LA_FLD_FP_VOLUME, 
    CMBS_UTIL_LA_FLD_MULTI_CALL_MODE,
    CMBS_UTIL_LA_FLD_INTRUSION_CALL_MODE,
    CMBS_UTIL_LA_FLD_OK_STATUS,
    CMBS_UTIL_LA_FLD_LINE_USE_STATUS,
    CMBS_UTIL_LA_FLD_HS_USE_STATUS,
    CMBS_UTIL_LA_FLD_CALL_FRWRD_STATUS,
    CMBS_UTIL_LA_FLD_WELCOME_MSG_PARAMETERS,
    CMBS_UTIL_LA_FLD_POSITION_IDX,
    CMBS_UTIL_LA_FLD_DTAM_ID,
    CMBS_UTIL_LA_FLD_SMS_ID,
    CMBS_UTIL_LA_FLD_ENABLE_SMS,
    CMBS_UTIL_LA_FLD_MAX_SMS_SIZE,
    CMBS_UTIL_LA_FLD_SMSC_SERVER,
    CMBS_UTIL_LA_FLD_SMS_DELIVERY_REPORT,
    CMBS_UTIL_LA_FLD_SMS_VALIDITY_PERIOD,
    CMBS_UTIL_LA_FLD_ALLOWED_ENCODING,
    CMBS_UTIL_LA_FLD_SMS_SEND_REQUEST,
    CMBS_UTIL_LA_FLD_SMS_SIZE,
    CMBS_UTIL_LA_FLD_SMS_CONTENT
} E_CMBS_UTIL_LA_TYPE;

typedef enum
{
    CMBS_UTIL_LA_LINE_ID_EXT_CALL = 0,   // 0 0 0 0 0 0 0 Line identifier for external call
    CMBS_UTIL_LA_LINE_ID_SOURCE_INT_CALL, // Source line identifier for internal call 
    CMBS_UTIL_LA_LINE_ID_TARGET_INT_CALL, // Target line identifier for internal call 
    CMBS_UTIL_LA_LINE_ID_RELATED_TO, // "Relating-to" line identifier (see note 2)
    CMBS_UTIL_LA_LINE_ID_ALL_LINES_RELATED_TO, // "All lines" related-to (see note 3)
} E_CMBS_UTIL_LA_LINE_ID_SUBTYPE;

/*! \brief LA field 'New' or 'Call type' */
typedef struct
{
    u8       u8_Attribute;              /*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8       u8_FieldId;                 /*!< Field ID as per catiq definition */
} ST_UTIL_LA_FLD_TYPE_1,    * PST_UTIL_LA_FLD_TYPE_1,
ST_UTIL_LA_FLD_NEW,       * PST_UTIL_LA_FLD_NEW,
ST_UTIL_LA_FLD_CALL_TYPE, * PST_UTIL_LA_FLD_CALL_TYPE,
ST_UTIL_SEND_REQUEST, *PST_UTIL_SEND_REQUEST;

/*! \brief LA field 'Number of calls' or 'Associated melody' or 'Line Id' */
typedef struct
{
    u8       u8_Attribute;              /*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8       u8_FieldId;                 /*!< Field ID as per catiq definition */
    u8       u8_Value;                  /*!< Field value */
} ST_UTIL_LA_FLD_TYPE_2,      * PST_UTIL_LA_FLD_TYPE_2,
ST_UTIL_LA_FLD_NR_OF_CALLS, * PST_UTIL_LA_FLD_NR_OF_CALLS,
ST_UTIL_LA_FLD_ASSOC_MDY,   * PST_UTIL_LA_FLD_ASSOC_MDY,
ST_UTIL_LA_FLD_FP_MELODY, * PST_UTIL_LA_FLD_FP_MELODY,
ST_UTIL_LA_FLD_FP_VOLUME, * PST_UTIL_LA_FLD_FP_VOLUME,
ST_UTIL_LA_FLD_MULTI_CALL_MODE, *PST_UTIL_LA_FLD_MULTI_CALL_MODE,
ST_UTIL_LA_FLD_INTRUSION_CALL, *PST_UTIL_LA_FLD_INTRUSION_CALL,
ST_UTIL_LA_FLD_POSITION_IDX, *PST_UTIL_LA_FLD_POSITION_IDX,
ST_UTIL_LA_SMS_ENABLE, *PST_UTIL_LA_SMS_ENABLE,
ST_UTIL_LA_FLD_MAX_SMS_SIZE, *PST_UTIL_LA_FLD_MAX_SMS_SIZE,
ST_UTIL_LA_FLD_SMS_DELIVERY_REPORT, *PST_UTIL_LA_FLD_SMS_DELIVERY_REPORT,
ST_UTIL_LA_FLD_SMS_VALIDITY_PERIOD, *PST_UTIL_LA_FLD_SMS_VALIDITY_PERIOD;


/*! \brief LA field 'Name', 'First name', 'Line name', 'Number' or 'Contact number' */
typedef struct
{
    u8       u8_Attribute;              /*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8       u8_FieldId;                /*!< Field ID as per catiq definition */
    u16      u16_DataLen;               /*!< Data length. Max 127 bytes */
    u8 *     pu8_Data;                  /*!< Data pointer */
} ST_UTIL_LA_FLD_TYPE_3,     * PST_UTIL_LA_FLD_TYPE_3,
ST_UTIL_LA_FLD_NAME,       * PST_UTIL_LA_FLD_NAME,
ST_UTIL_LA_FLD_FIRST_NAME, * PST_UTIL_LA_FLD_FIRST_NAME,
ST_UTIL_LA_FLD_LINE_NAME,  * PST_UTIL_LA_FLD_LINE_NAME,
ST_UTIL_LA_FLD_NUMBER,     * PST_UTIL_LA_FLD_NUMBER,
ST_UTIL_LA_FLD_CONTACT_NR, * PST_UTIL_LA_FLD_CONTACT_NR,
ST_UTIL_LA_FLD_ATTACHED_HANDSETS, * PST_UTIL_LA_FLD_ATTACHED_HANDSETS,
ST_UTIL_LA_FLD_DIALING_PREFIX, *PST_UTIL_LA_FLD_DIALING_PREFIX,
ST_UTIL_LA_FLD_BLOCKED_NUMBER, *PST_UTIL_LA_FLD_BLOCKED_NUMBER,
ST_UTIL_LA_FLD_HS_USE_STATUS, *PST_UTIL_LA_FLD_HS_USE_STATUS,
ST_UTIL_LA_FLD_SMSC_SERVER, *PST_UTIL_LA_FLD_SMSC_SERVER,
ST_UTIL_LA_FLD_ALLOWED_ENCODING, *PST_UTIL_LA_FLD_ALLOWED_ENCODING,
ST_UTIL_LA_FLD_SMS_CONTENT, *PST_UTIL_LA_FLD_SMS_CONTENT;


typedef struct
{
    u8       u8_Attribute;              /*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8       u8_FieldId;                /*!< Field ID as per catiq definition */
    u8       u8_Value;                  /*!< Field value with MSB set */
} ST_UTIL_LA_FLD_TYPE_4,     * PST_UTIL_LA_FLD_TYPE_4,
ST_UTIL_LA_FLD_OK_STATUS, *PST_UTIL_LA_FLD_OK_STATUS,
ST_UTIL_LA_FLD_LINE_USE_STATUS, *PST_UTIL_LA_FLD_LINE_USE_STATUS,
ST_UTIL_LA_FLD_WELCOME_MSG_PARAMS, *PST_UTIL_LA_FLD_WELCOME_MSG_PARAMS,
ST_UTIL_LA_FLD_DTAM_ID, *PST_UTIL_LA_FLD_DTAM_ID,
ST_UTIL_LA_FLD_SMS_ID, *PST_UTIL_LA_FLD_SMS_ID,
ST_UTIL_SMS_SIZE, *PST_UTIL_SMS_SIZE;

typedef struct
{
    u8       u8_Attribute;              /*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8       u8_FieldId;                 /*!< Field ID as per catiq definition */
    E_CMBS_UTIL_LA_LINE_ID_SUBTYPE Subtype;                  /*!< Field value */
    u16 u16_LineNumber;
} 
ST_UTIL_LA_FLD_LINE_ID,     * PST_UTIL_LA_FLD_LINE_ID;

typedef struct
{
    u8       u8_Attribute;              
    u8       u8_FieldId;               
    u8       u8_Value;     
    u8       u8_ActDataLen;                
    u8 *     pu8_ActData;              
    u8       u8_DesactDataLen;              
    u8 *     pu8_DesactData;                 
} ST_UTIL_LA_FLD_PERMANENT_CLIR,     * PST_UTIL_LA_FLD_PERMANENT_CLIR;



typedef struct
{
    u8       u8_Attribute;              
    u8       u8_FieldId;
    u8       u8_NotSetByUSer;
    u8       u8_Value; 
    u8       u8_Timeout;    
    u8       u8_ActDataLen;                
    u8 *     pu8_ActData;              
    u8       u8_DesactDataLen;              
    u8 *     pu8_DesactData;         
    u8       u8_NumberLen;              
    u8 *     pu8_Number;                
} ST_UTIL_LA_FLD_CALL_FORWARDING,     * PST_UTIL_LA_FLD_CALL_FORWARDING,
ST_UTIL_LA_FLD_CFB, * PST_UTIL_LA_FLD_CFB, ST_UTIL_LA_FLD_CFNA,     * PST_UTIL_LA_FLD_CFNA;

/*! \brief LA field 'Date and Time' */
typedef struct
{
    u8       u8_Attribute;              /*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8       u8_FieldId;                 /*!< Field ID as per catiq definition */
    u8       u8_Year;
    u8       u8_Month;
    u8       u8_Day;
    u8       u8_Hours;
    u8       u8_Mins;
    u8       u8_Secs;
    u8       u8_Coding;
    u8       u8_interpretation;
    u8       u8_TimeZone;
} ST_UTIL_LA_FLD_DATETIME, * PST_UTIL_LA_FLD_DATETIME;


typedef struct
{
    u8      u8_Attribute;         /*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8      u8_FieldId;           /*!< Field ID as per catiq definition */
    u8      u8_ErrorType;         /*!< Diagnostic Error Type */
    u8      u8_ErrorNumber;       /*!< Diagnostic Error Number */
} ST_UTIL_LA_DIAG_ERROR_STATUS,     * PST_UTIL_LA_DIAG_ERROR_STATUS;

typedef struct
{
    u8      u8_Attribute;        	/*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8      u8_FieldId;             /*!< Field ID as per catiq definition */
    u8      u8_CFUStatus;         	/*!< CFU Status */
    u8      u8_CFNAStatus;       	/*!< CFNA Status */
    u8      u8_CFBStatus;       	/*!<CFB Status */
} ST_UTIL_LA_FLD_CALL_FWRD_STATUS, *PST_UTIL_LA_FLD_CALL_FWRD_STATUS;

typedef struct
{
    u8      u8_Attribute;        	/*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8      u8_FieldId;             /*!< Field ID as per catiq definition */
    u8      u8_DTAMActivated;       /*!< DTAM Activated/Deactivated */
    u8      u8_DTAMTimeout;         /*!< number of seconds before DTAM answers an incoming call */
} ST_UTIL_LA_FLD_DTAM_ACTIVATION, *PST_UTIL_LA_FLD_DTAM_ACTIVATION;

typedef struct
{
    u8      u8_Attribute;        	/*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8      u8_FieldId;             /*!< Field ID as per catiq definition */
    u8      u8_DurationHours;       /*!< Duration of the message - Hours */
    u8      u8_DurationMinutes;     /*!< Duration of the message - Minutes */
    u8      u8_DurationSeconds;     /*!< Duration of the message - Seconds */
} ST_UTIL_LA_FLD_MSG_DURATION, *PST_UTIL_LA_FLD_MSG_DURATION;

typedef struct
{
    u8      u8_Attribute;        	/*!< Field attribute bitmask. See CMBS_LA_ATTRIB_xxx */
    u8      u8_FieldId;             /*!< Field ID as per catiq definition */
    u8      u8_UsedNetworkSideEnc; 
    u8      u8_UsedEncVariant_1;     
    u8      u8_UsedEncVariant_2;     
}ST_UTIL_NETWORK_SIDE_ENC, *PST_UTIL_NETWORK_SIDE_ENC;

u8 *         cmbs_util_LA_EntryLenght_EntryIdentifierAdd( u8 *p_entry, u16 u16_index, u16 u16_EntryID);



void cmbs_util_LA_EntryLenght_EntryIdentifierGet(u8 *pu8_Data, u16 *u16_EntryIdentifier, u16 *u16_EntryLength, u16 *u16_Index) ;


//*/
//    ==========  cmbs_util_LA_Fld_NumberAdd  ===========
/*!
      \brief
         Add list field 'Number' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_NumberAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Number' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Number' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NumberAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NUMBER * pst_LA_Fld_Number )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_NUMBER structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_LA_Fld_Number  pointer to ST_UTIL_LA_FLD_NUMBER structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NumberAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NUMBER * pst_LA_Fld_Number, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_NumberGet  ===========
/*!
      \brief
         Get list field 'Number' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_NumberGet</h2>

      <h3>Introduction</h3>
         Get list field 'Number' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Number' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NumberGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_NUMBER * pst_LA_Fld_Number )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_Number pointer to ST_UTIL_LA_FLD_NUMBER structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NumberGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_NUMBER * pst_LA_Fld_Number);

//*/
//    ==========  cmbs_util_LA_Fld_NameAdd  ===========
/*!
      \brief
         Add list field 'Name' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_NameAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Name' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Name' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NameAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NAME * pst_LA_Fld_Name )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_NAME structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_LA_Fld_Name   pointer to ST_UTIL_LA_FLD_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NameAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NAME * pst_LA_Fld_Name, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_NameGet  ===========
/*!
      \brief
         Get list field 'Name' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_NameGet</h2>

      <h3>Introduction</h3>
         Get list field 'Name' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Name' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NameGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_NAME * pst_LA_Fld_Name )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_Name   pointer to ST_UTIL_LA_FLD_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NameGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_NAME * pst_LA_Fld_Name);

//*/
//    ==========  cmbs_util_LA_Fld_DateTimeAdd  ===========
/*!
      \brief
         Add list field 'Date and time' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_DateTimeAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Date and time' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Date and time' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_DateTimeAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_DATETIME * pst_LA_Fld_DateTime )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_DATETIME structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_DateTime  pointer to ST_UTIL_LA_FLD_DATETIME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_DateTimeAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_DATETIME * pst_LA_Fld_DateTime, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_DateTimeGet  ===========
/*!
      \brief
         Get list field 'Date and time' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_DateTimeGet</h2>

      <h3>Introduction</h3>
         Get list field 'Date and time' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Date and time' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_DateTimeGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_DATETIME * pst_LA_Fld_DateTime )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_DateTime  pointer to ST_UTIL_LA_FLD_DATETIME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_DateTimeGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_DATETIME * pst_LA_Fld_DateTime );

//*/
//    ==========  cmbs_util_LA_Fld_NewAdd  ===========
/*!
      \brief
         Add list field 'New' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_NewAdd</h2>

      <h3>Introduction</h3>
         Add list field 'New' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'New' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NewAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NEW * pst_LA_Fld_New )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_NEW structure first.

      \param[in]        pv_RefIEList      pointer to IE List
      \param[in]        pst_LA_Fld_New    pointer to ST_UTIL_LA_FLD_NEW structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NewAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NEW * pst_LA_Fld_New, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_NewGet  ===========
/*!
      \brief
         Get list field 'New' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_NewGet</h2>

      <h3>Introduction</h3>
         Get list field 'New' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'New' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NewGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_NEW * pst_LA_Fld_New )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE          pointer to IE list element returned by<br>
                                          cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_New    pointer to ST_UTIL_LA_FLD_NEW structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NewGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_NEW * pst_LA_Fld_New );

//*/
//    ==========  cmbs_util_LA_Fld_LineNameAdd  ===========
/*!
      \brief
         Add list field 'Line name' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_LineNameAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Line name' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Line name' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_LineNameAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_LINE_NAME * pst_LA_Fld_LineName )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_LINE_NAME structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_LineName  pointer to ST_UTIL_LA_FLD_LINE_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_LineNameAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_LINE_NAME * pst_LA_Fld_LineName, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_LineNameGet  ===========
/*!
      \brief
         Get list field 'Line name' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_LineNameGet</h2>

      <h3>Introduction</h3>
         Get list field 'Line name' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Line name' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_LineNameGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_LINE_NAME * pst_LA_Fld_LineName )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_LineName  pointer to ST_UTIL_LA_FLD_LINE_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_LineNameGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_LINE_NAME * pst_LA_Fld_LineName );

//*/
//    ==========  cmbs_util_LA_Fld_LineIdAdd  ===========
/*!
      \brief
         Add list field 'Line Id' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_LineIdAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Line Id' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Line Id' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_LineIdAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_LINE_ID * pst_LA_Fld_LineId )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_LINE_ID structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_LineId    pointer to ST_UTIL_LA_FLD_LINE_ID structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_LineIdAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_LINE_ID * pst_LA_Fld_LineId, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_LineIdGet  ===========
/*!
      \brief
         Get list field 'Line Id' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_LineIdGet</h2>

      <h3>Introduction</h3>
         Get list field 'Line Id' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Line Id' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_LineIdGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_LINE_ID * pst_LA_Fld_LineId )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_LineId    pointer to ST_UTIL_LA_FLD_LINE_ID structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_LineIdGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_LINE_ID * pst_LA_Fld_LineId );

//*/
//    ==========  cmbs_util_LA_Fld_NrOfCallsAdd  ===========
/*!
      \brief
         Add list field 'Number of calls' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_NrOfCallsAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Number of calls' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Number of calls' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NrOfCallsAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NR_OF_CALLS * pst_LA_Fld_NrOfCalls )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_NR_OF_CALLS structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_NrOfCalls pointer to ST_UTIL_LA_FLD_NR_OF_CALLS structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NrOfCallsAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_NR_OF_CALLS * pst_LA_Fld_NrOfCalls, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_NrOfCallsGet  ===========
/*!
      \brief
         Get list field 'Number of calls' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_NrOfCallsGet</h2>

      <h3>Introduction</h3>
         Get list field 'Number of calls' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Number of calls' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_NrOfCallsGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_NR_OF_CALLS * pst_LA_Fld_NrOfCalls )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_NrOfCalls pointer to ST_UTIL_LA_FLD_NR_OF_CALLS structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_NrOfCallsGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_NR_OF_CALLS * pst_LA_Fld_NrOfCalls );

//*/
//    ==========  cmbs_util_LA_Fld_CallTypeAdd  ===========
/*!
      \brief
         Add list field 'Call type' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_CallTypeAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Call type' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Call type' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_CallTypeAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_CALL_TYPE * pst_LA_Fld_CallType )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_CALL_TYPE structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_CallType  pointer to ST_UTIL_LA_FLD_CALL_TYPE structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_CallTypeAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_CALL_TYPE * pst_LA_Fld_CallType, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_CallTypeGet  ===========
/*!
      \brief
         Get list field 'Call type' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_CallTypeGet</h2>

      <h3>Introduction</h3>
         Get list field 'Call type' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Call type' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_CallTypeGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_CALL_TYPE * pst_LA_Fld_CallType )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_CallType  pointer to ST_UTIL_LA_FLD_CALL_TYPE structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_CallTypeGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_CALL_TYPE * pst_LA_Fld_CallType );

//*/
//    ==========  cmbs_util_LA_Fld_FirstNameAdd  ===========
/*!
      \brief
         Add list field 'First name' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_FirstNameAdd</h2>

      <h3>Introduction</h3>
         Add list field 'First name' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'First name' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_FirstNameAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_FIRST_NAME * pst_LA_Fld_FirstName )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_FIRST_NAME structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_FirstName pointer to ST_UTIL_LA_FLD_FIRST_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_FirstNameAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_FIRST_NAME * pst_LA_Fld_FirstName, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_FirstNameGet  ===========
/*!
      \brief
         Get list field 'First name' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_FirstNameGet</h2>

      <h3>Introduction</h3>
         Get list field 'First name' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'First name' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_FirstNameGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_FIRST_NAME * pst_LA_Fld_FirstName )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_FirstName pointer to ST_UTIL_LA_FLD_FIRST_NAME structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_FirstNameGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_FIRST_NAME * pst_LA_Fld_FirstName );

//*/
//    ==========  cmbs_util_LA_Fld_ContactNrAdd  ===========
/*!
      \brief
         Add list field 'Contact number' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_ContactNrAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Contact number' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Contact number' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_ContactNrAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_CONTACT_NR * pst_LA_Fld_ContactNr )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_CONTACT_NR structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_ContactNr pointer to ST_UTIL_LA_FLD_CONTACT_NR structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_ContactNrAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_CONTACT_NR * pst_LA_Fld_ContactNr, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_ContactNrGet  ===========
/*!
      \brief
         Get list field 'Contact number' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_ContactNrGet</h2>

      <h3>Introduction</h3>
         Get list field 'Contact number' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Contact number' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_ContactNrGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_CONTACT_NR * pst_LA_Fld_ContactNr )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_ContactNr pointer to ST_UTIL_LA_FLD_CONTACT_NR structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_ContactNrGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_CONTACT_NR * pst_LA_Fld_ContactNr );

//*/
//    ==========  cmbs_util_LA_Fld_AssocMdyAdd  ===========
/*!
      \brief
         Add list field 'Associated melody' Information Element to IE List.

         <h2>cmbs_util_LA_Fld_AssocMdyAdd</h2>

      <h3>Introduction</h3>
         Add list field 'Associated melody' Information Element to IE List.

      <h3>Use cases</h3>
         Transmitting list field 'Associated melody' IE by means of an Information Elements List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_AssocMdyAdd ( u8 * pv_RefIEList, ST_UTIL_LA_FLD_ASSOC_MDY * pst_LA_Fld_AssocMdy )
         </b><br><br>
         The application has to populate a ST_UTIL_LA_FLD_ASSOC_MDY structure first.

      \param[in]        pv_RefIEList         pointer to IE List
      \param[in]        pst_LA_Fld_AssocMdy  pointer to ST_UTIL_LA_FLD_ASSOC_MDY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_AssocMdyAdd( u8 * pv_RefIEList, ST_UTIL_LA_FLD_ASSOC_MDY * pst_LA_Fld_AssocMdy, u16 *pu16_index );

//*/
//    ==========  cmbs_util_LA_Fld_AssocMdyGet  ===========
/*!
      \brief
         Get list field 'Associated melody' Information Element from IE List element.

         <h2>cmbs_util_LA_Fld_AssocMdyGet</h2>

      <h3>Introduction</h3>
         Get list field 'Associated melody' Information Element from IE List element.

      <h3>Use cases</h3>
         Received list field 'Associated melody' IE from an Information Element List.

      <h3>API Functions description</h3>
         <b>
         E_CMBS_RC cmbs_util_LA_Fld_AssocMdyGet ( u8 * pv_RefIE, ST_UTIL_LA_FLD_ASSOC_MDY * pst_LA_Fld_AssocMdy )
         </b><br><br>
         The function extracts the IE data from the List element.<br>
         NOTE: Data is only valid in context of the callback function.

      \param[in]        pv_RefIE             pointer to IE list element returned by<br>
                                             cmbs_api_ie_GetFirst() or cmbs_api_ie_GetNext()
      \param[in,out]    pst_LA_Fld_AssocMdy  pointer to ST_UTIL_LA_FLD_ASSOC_MDY structure

      \return           Return Code

      \see

      <b>Sample Code:</b><br><br>
      <code></code>
*/
E_CMBS_RC         cmbs_util_LA_Fld_AssocMdyGet( u8 * pv_RefIE, ST_UTIL_LA_FLD_ASSOC_MDY * pst_LA_Fld_AssocMdy );


E_CMBS_RC               cmbs_util_LA_Fld_AttachedHandsetsAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_ATTACHED_HANDSETS *st_LA_Fld_AttachedHandsets,  u16 *pindex);
E_CMBS_RC               cmbs_util_LA_Fld_DialPrefixeAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_DIALING_PREFIX *st_LA_Fld_DialingPrefixe,  u16 *pindex);
E_CMBS_RC               cmbs_util_LA_Fld_FPvolumeAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_FP_VOLUME *st_LA_Fld_FPvolume,  u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_BlockedNumberAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_BLOCKED_NUMBER *st_LA_Fld_BlockedNumber,  u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_MultiCallModeAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_MULTI_CALL_MODE *st_LA_Fld_MultiCallMode,  u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_IntrusionCallAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_INTRUSION_CALL *st_LA_Fld_IntrusionCall,  u16 *pindex);
E_CMBS_RC               cmbs_util_LA_Fld_PermanentCLIRAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_PERMANENT_CLIR *st_LA_Fld_PermanentCLIR, u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_CallForwardingAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_CALL_FORWARDING *st_LA_Fld_CallForwarding, u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_CFNAAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_CFNA *st_LA_Fld_CFNA,  u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_CFBAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_CFB *st_LA_Fld_CFB, u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_OKStatusAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_OK_STATUS *st_LA_Fld_OKStatus, u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_LineUseStatusAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_LINE_USE_STATUS*st_LA_Fld_LineUseStatus, u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_HSUseStatusAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_HS_USE_STATUS *st_LA_Fld_HSUseStatus, u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_CallForwardingStatusAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_CALL_FWRD_STATUS*st_LA_Fld_CallForwarding, u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_DiagnosticErrorAdd(  u8 * pv_buff, ST_UTIL_LA_DIAG_ERROR_STATUS *st_LA_fld_DiagErrorStatus, u16 *pindex );
E_CMBS_RC   			cmbs_util_LA_Fld_DTAMIdAdd(u8 * pv_buff, ST_UTIL_LA_FLD_DTAM_ID *st_LA_fld_DTAMId, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_DTAMActivationAdd(u8 * pv_buff, ST_UTIL_LA_FLD_DTAM_ACTIVATION *st_LA_fld_DTAMActivation, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_WelcomeMsgParamsAdd(u8 * pv_buff, ST_UTIL_LA_FLD_WELCOME_MSG_PARAMS *st_LA_fld_DTAMWelcomeMsgParams, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_DTAMPositionIdxAdd(u8 * pv_buff, ST_UTIL_LA_FLD_POSITION_IDX *st_LA_fld_DTAMPositionIdx, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_DTAMMsgDurationAdd(u8 * pv_buff, ST_UTIL_LA_FLD_MSG_DURATION *st_LA_fld_DTAMMsgDuration, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSIdAdd(u8 * pv_buff, ST_UTIL_LA_FLD_SMS_ID *st_LA_fld_SMSId, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSMaxSizeAdd(u8 * pv_buff, ST_UTIL_LA_FLD_MAX_SMS_SIZE *st_LA_fld_SMSMaxSize, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSCServerAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_SMSC_SERVER *st_LA_Fld_BlockedNumber,  u16 *pindex );
E_CMBS_RC 				cmbs_util_LA_Fld_SMSDeliveryReportAdd(u8 * pv_buff, ST_UTIL_LA_FLD_SMS_DELIVERY_REPORT *st_LA_fld_SMSDeliveryReport, u16 *pindex);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSAllowedEncodingAdd(  u8 * pv_buff, ST_UTIL_LA_FLD_ALLOWED_ENCODING *st_LA_Fld_SMSAllowedEncoding,  u16 *pindex );
E_CMBS_RC 				cmbs_util_LA_Fld_SMSSendRequestAdd( u8 * pv_buff, ST_UTIL_SEND_REQUEST * st_LA_Fld_SendReq , u16 *pindex );
E_CMBS_RC 				cmbs_util_LA_Fld_NetworkSideEncAdd( u8 * pv_buff, ST_UTIL_NETWORK_SIDE_ENC * st_LA_Fld_SendReq , u16 *pindex );
E_CMBS_RC 				cmbs_util_LA_Fld_SMSSizeAdd( u8 * pv_buff, ST_UTIL_SMS_SIZE * st_LA_Fld_SMSSize , u16 *pindex );
E_CMBS_RC 				cmbs_util_LA_Fld_SMSContentAdd( u8 * pv_buff, ST_UTIL_LA_FLD_SMS_CONTENT * st_LA_Fld_SMSContent , u16 *pindex );
E_CMBS_RC               cmbs_util_LA_Fld_SMSValidityPeriodAdd(u8 * pv_buff, ST_UTIL_LA_FLD_SMS_VALIDITY_PERIOD*st_LA_fld_SMSValidityPeriod, u16 *pindex);
E_CMBS_RC               cmbs_util_LA_Fld_SMSEnableAdd(u8 * pv_buff, ST_UTIL_LA_SMS_ENABLE *st_LA_fld_SMSEnable, u16 *pindex);

E_CMBS_RC               cmbs_util_LA_Fld_AttachedHandsetsGet(  u8 * pv_buff, ST_UTIL_LA_FLD_ATTACHED_HANDSETS *st_LA_Fld_AttachedHandsets);
E_CMBS_RC               cmbs_util_LA_Fld_DialPrefixeGet(  u8 * pv_buff, ST_UTIL_LA_FLD_DIALING_PREFIX *st_LA_Fld_DialingPrefixe);
E_CMBS_RC               cmbs_util_LA_Fld_FPvolumeGet(  u8 * pv_buff, ST_UTIL_LA_FLD_FP_VOLUME *st_LA_Fld_FPvolume );
E_CMBS_RC               cmbs_util_LA_Fld_BlockedNumberGet(  u8 * pv_buff, ST_UTIL_LA_FLD_BLOCKED_NUMBER *st_LA_Fld_BlockedNumber);
E_CMBS_RC               cmbs_util_LA_Fld_MultiCallModeGet(  u8 * pv_buff, ST_UTIL_LA_FLD_MULTI_CALL_MODE *st_LA_Fld_MultiCallMode );
E_CMBS_RC               cmbs_util_LA_Fld_IntrusionCallGet(  u8 * pv_buff, ST_UTIL_LA_FLD_INTRUSION_CALL *st_LA_Fld_IntrusionCall);
E_CMBS_RC               cmbs_util_LA_Fld_PermanentCLIRGet(  u8 * pv_buff, ST_UTIL_LA_FLD_PERMANENT_CLIR *st_LA_Fld_PermanentCLIR);
E_CMBS_RC               cmbs_util_LA_Fld_CallForwardingGet(  u8 * pv_buff, ST_UTIL_LA_FLD_CALL_FORWARDING *st_LA_Fld_CallForwarding);
E_CMBS_RC               cmbs_util_LA_Fld_CFNAGet(  u8 * pv_buff, ST_UTIL_LA_FLD_CFNA *st_LA_Fld_CFNA);
E_CMBS_RC               cmbs_util_LA_Fld_CFBGet(  u8 * pv_buff, ST_UTIL_LA_FLD_CFB *st_LA_Fld_CFB );
E_CMBS_RC               cmbs_util_LA_Fld_OKStatusGet( u8 * pv_buff, ST_UTIL_LA_FLD_OK_STATUS *st_LA_Fld_OKStatus );
E_CMBS_RC               cmbs_util_LA_Fld_LineUseStatusGet(  u8 * pv_buff, ST_UTIL_LA_FLD_LINE_USE_STATUS *st_LA_Fld_LineUseStatus );
E_CMBS_RC               cmbs_util_LA_Fld_HSUseStatusGet( u8 * pv_buff, ST_UTIL_LA_FLD_HS_USE_STATUS *st_LA_Fld_HSUseStatus  );
E_CMBS_RC               cmbs_util_LA_Fld_CallForwardingStatusGet( u8 * pv_buff, ST_UTIL_LA_FLD_CALL_FWRD_STATUS *st_LA_Fld_CallForwarding );
E_CMBS_RC               cmbs_util_LA_Fld_DiagnosticErrorGet( u8 * pv_buff, ST_UTIL_LA_DIAG_ERROR_STATUS *st_LA_fld_DiagErrorStatus  );
E_CMBS_RC   			cmbs_util_LA_Fld_DTAMIdGet(u8 * pv_buff, ST_UTIL_LA_FLD_DTAM_ID *st_LA_fld_DTAMId);
E_CMBS_RC 				cmbs_util_LA_Fld_DTAMActivationGet(u8 * pv_buff, ST_UTIL_LA_FLD_DTAM_ACTIVATION *st_LA_fld_DTAMActivation);
E_CMBS_RC 				cmbs_util_LA_Fld_WelcomeMsgParamsGet(u8 * pv_buff, ST_UTIL_LA_FLD_WELCOME_MSG_PARAMS *st_LA_fld_DTAMWelcomeMsgParams);
E_CMBS_RC 				cmbs_util_LA_Fld_DTAMPositionIdxGet(u8 * pv_buff, ST_UTIL_LA_FLD_POSITION_IDX *st_LA_fld_DTAMPositionIdx);
E_CMBS_RC 				cmbs_util_LA_Fld_DTAMMsgDurationGet(u8 * pv_buff, ST_UTIL_LA_FLD_MSG_DURATION *st_LA_fld_DTAMMsgDuration);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSIdGet(u8 * pv_buff, ST_UTIL_LA_FLD_SMS_ID *st_LA_fld_SMSId);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSEnableGet(u8 * pv_buff, ST_UTIL_LA_SMS_ENABLE *st_LA_fld_SMSEnable);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSMaxSizeGet(u8 * pv_buff, ST_UTIL_LA_FLD_MAX_SMS_SIZE *st_LA_fld_SMSMaxSize);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSCServerGet(  u8 * pv_buff, ST_UTIL_LA_FLD_SMSC_SERVER *st_LA_Fld_BlockedNumber);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSDeliveryReportGet(u8 * pv_buff, ST_UTIL_LA_FLD_SMS_DELIVERY_REPORT *st_LA_fld_SMSDeliveryReport);
E_CMBS_RC               cmbs_util_LA_Fld_SMSValidityPeriodGet(u8 * pv_buff, ST_UTIL_LA_FLD_SMS_VALIDITY_PERIOD *st_LA_fld_SMSValidityPeriod);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSAllowedEncodingGet(  u8 * pv_buff, ST_UTIL_LA_FLD_ALLOWED_ENCODING *st_LA_Fld_SMSAllowedEncoding);
E_CMBS_RC 				cmbs_util_LA_Fld_SMSSendRequestGet( u8 * pv_buff, ST_UTIL_SEND_REQUEST * st_LA_Fld_SendReq );
E_CMBS_RC 				cmbs_util_LA_Fld_NetworkSideEncGet( u8 * pv_buff, ST_UTIL_NETWORK_SIDE_ENC * st_LA_Fld_SendReq );
E_CMBS_RC 				cmbs_util_LA_Fld_SMSSizeGet( u8 * pv_buff, ST_UTIL_SMS_SIZE * st_LA_Fld_SMSSize );
E_CMBS_RC 				cmbs_util_LA_Fld_SMSContentGet( u8 * pv_buff, ST_UTIL_LA_FLD_SMS_CONTENT * st_LA_Fld_SMSContent );

/*****************************************************************************
 * List Access fields
 *****************************************************************************/

E_CMBS_RC         cmbs_util_LA_Fld_Type1_Add( u8 * pv_RefIEList,
                                              ST_UTIL_LA_FLD_TYPE_1 * pst_LA_Fld_Type1,
                                              E_CMBS_UTIL_LA_TYPE e_IETYPE,
                                              u16 *pindex  );

E_CMBS_RC         cmbs_util_LA_Fld_Type1_Get( u8 * pv_RefIE, ST_UTIL_LA_FLD_TYPE_1 * pst_LA_Fld_Type1, E_CMBS_UTIL_LA_TYPE type );

E_CMBS_RC         cmbs_util_LA_Fld_Type2_Add( u8 * pv_RefIEList,
                                              ST_UTIL_LA_FLD_TYPE_2 * pst_LA_Fld_Type2,
                                              E_CMBS_UTIL_LA_TYPE e_IETYPE,
                                              u16 *pindex  );

E_CMBS_RC         cmbs_util_LA_Fld_Type2_Get( u8 * pv_RefIE, ST_UTIL_LA_FLD_TYPE_2 * pst_LA_Fld_Type2, E_CMBS_UTIL_LA_TYPE type  );

E_CMBS_RC         cmbs_util_LA_Fld_Type3_Add( u8 * pv_RefIEList,
                                              ST_UTIL_LA_FLD_TYPE_3 * pst_LA_Fld_Type3,
                                              E_CMBS_UTIL_LA_TYPE e_IETYPE,
                                              u16 *pindex  );

E_CMBS_RC         cmbs_util_LA_Fld_Type3_Get( u8 * pv_RefIE, ST_UTIL_LA_FLD_TYPE_3 * pst_LA_Fld_Type3, E_CMBS_UTIL_LA_TYPE type  );
E_CMBS_RC         cmbs_util_LA_Fld_Type4_Get( u8 * pv_buff, ST_UTIL_LA_FLD_TYPE_4 * pst_LA_Fld_Type4, E_CMBS_UTIL_LA_TYPE type  );
E_CMBS_RC         cmbs_util_LA_Fld_Type4_Add( u8 * pv_buff,
                                              ST_UTIL_LA_FLD_TYPE_4 * pst_LA_Fld_Type4,
                                              E_CMBS_UTIL_LA_TYPE e_LATYPE,
                                              u16 *pindex  );

void cmbs_util_LA_GetLengthAndDataShiftIndex(u8 *pu8_Buffer, 
                                             u8* pu8_length,
                                             u8 ** ppu8_data,
                                             u16 *pu16_index);

void        cmbs_util_la_ShiftIndexToNextFieldID(u8 *pu8_FieldId, u16 *u16_Index);


void cmbs_util_LA_GetInfo_ShiftIndex(u8 *pu8_buf, u16 *pu16_data_to_fill, u16 *pu16_Index);
void cmbs_util_add_u16_and_shift(u8 *pu8_buf, u16 u16_value_to_write, u16 *pu16_Index);

u8 cmbs_util_LA_extendlen(u8 byte);

E_CMBS_RC cmbs_util_LA_AddEntryToFinalBuffer( void * pv_AppRef,
                                              u16 u16_SessionId,
                                              u8 * pu8_EntryBuff,
                                              u16 u16_sizeofbuff );

void    cmbs_util_LA_SendFinalBuffer( void * pv_AppRef,
                                      u16 u16_SessionId);


int app_LaGetSessionIndex ( u16 u16_SessionId );

#include "tcx_hostlog.h"
#define APP_LA_INFO_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_INFO, format,  ##__VA_ARGS__ )
#define APP_LA_TRACE_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_TRACE, format,  ##__VA_ARGS__ )
#define APP_LA_WARNING_PRINT(format, ...) 					tcx_WriteLog(CALL_MODULE, LOG_LEVEL_WARNING, format,  ##__VA_ARGS__ )
#define APP_LA_ERROR_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_ERROR, format,  ##__VA_ARGS__ )
#define APP_LA_RT_PRINT(format, ...) 						tcx_WriteLog(CALL_MODULE, LOG_LEVEL_REAL_TIME, format,  ##__VA_ARGS__ )
#define APP_LA_PRINT_DATA(pu8_Buffer, u16_Length) 			tcx_WriteLogData(CALL_MODULE, LOG_LEVEL_INFO, pu8_Buffer, u16_Length)

