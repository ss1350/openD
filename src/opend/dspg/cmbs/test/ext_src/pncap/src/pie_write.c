/** @file pie_write.c
 *
 *  @brief Functions for constructing Gigaset PNCAP PIEs (Proprietary Information Element).
 *
 */
#include <string.h>
#include "pie.h"

//------------------------------------------------------------------------------------//
//                               Macros                                               //
//------------------------------------------------------------------------------------//
#define PUT8(p,v)         *(p)++ = (uint8_t)(v) // puts and moves pointer
#define PUT8_(p,v)        *(p) = (uint8_t)(v)   // only puts
#define PUTBUF(p,buf,buf_len) memmove(p, buf, buf_len), p+=buf_len // copies "buf" with "len" into "p", then moves the the pointer p
#define GETPTR8(p)        p, p++                      //
#define PUTLEN8_(p,l)     PUT8_((l),(p)-(l)-1)        // writes length without moving pointer
#define GET(p)            *p, p++                     // get value of the pointer and increments the address

#define OPEN(pie, pie_id) uint8_t *length; PUT8(pie, pie_id); length = GETPTR8(pie)              ///< Write the PIE id and store position where the len will be written
#define CLOSE(pie)        PUTLEN8_(pie, length); return (uint8_t)(*length + PNCAP_PIE_HEADER_LEN)///< Write len in PIE header and return total PIE len


//------------------------------------------------------------------------------------//
//                         functions constructing PNCAP messages                      //
//------------------------------------------------------------------------------------//


/** Constructs PNCAP_PIE message to indicate new date and time
 *
 *  @note The PIE <<Escape to System Time>> may be sent by the FP to the PP and vice versa in order to
 *        set a system wide valid system Date and Time. The PIE has to be sent in a CISS message.
 *
 *  @param  *pie   - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @param  *date_time - structure to hold date and time in BCD format
 *  @return total length of the constructed PIE
 */
uint8_t  pncap_pie_set_date_time
(
  uint8_t  *pie,  // destination buffer for PIE
  struct pncap_date_and_time  *date_time  // structure holding date and time values in BCD format
)
{
  // *************************************************************
  // ** PIE  <<Escape to System Time>>
  // *************************************************************
  OPEN(pie, PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME);

  // Date indication
  PUT8(pie, PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_DATE_INDICATION); 
  PUT8(pie, date_time->day);                    
  PUT8(pie, date_time->month);                   
  PUT8(pie, date_time->year);                     
  PUT8(pie, date_time->date_mode);               
  
  // Time indication
  PUT8(pie, PNCAP_PIE_ESCAPE_TO_SYSTEM_TIME_TIME_INDICATION); 
  PUT8(pie, date_time->hours);                   
  PUT8(pie, date_time->minutes);                 
  PUT8(pie, date_time->time_mode);               

  CLOSE(pie);
}



/** Constructs PNCAP_PIE message to indicate new message in voice mail box
 *
 *  The PIE <<INT Number and Name>> is defined in order to allow the PP to display 
 *  its own INT-No. or the name in the idle mode.
 *
 *  @note  The PIE has to be sent in a CISS message and the PP has to acknowledge with the PIE <<Positive Acknowledgement>>.
 *
 *  @param  *pie     - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @param  *intName - name of the handset (null terminated string)
 *  @param  coding   - If 0 Latin1 will be used. Other possible values are: PNCAP_C0_ESC_LATIN1, PNCAP_C0_ESC_CYRILLIC, PNCAP_C0_ESC_LATIN2, PNCAP_C0_ESC_GREEK, PNCAP_C0_ESC_ARABIC, PNCAP_C0_ESC_HEBREW, PNCAP_C0_ESC_TURKISH
 *  @param  intNum   - number of handset 
 *  @return total length of the constructed PIE
 */
uint8_t pncap_pie_set_int_name_number
(
  uint8_t  *pie,   // destination buffer for PIE
  const unsigned char  *name, // name of the handset (null terminated string)
  uint8_t  coding, // If 0 no coding info will be added. Other possible values are: PNCAP_C0_ESC_LATIN1, PNCAP_C0_ESC_CYRILLIC, PNCAP_C0_ESC_LATIN2, PNCAP_C0_ESC_GREEK, PNCAP_C0_ESC_ARABIC, PNCAP_C0_ESC_HEBREW, PNCAP_C0_ESC_TURKISH
  uint8_t  int_num // the INT-number may be in the range 0-126
)
{
  // *************************************************************
  // ** PIE <<Int Name Number Indication>> **
  // *************************************************************
  OPEN(pie, PNCAP_PIE_INT_NUMBER_AND_NAME);
 
  PUT8(pie, int_num);                           // internal number

  if(coding)
  {
    PUT8(pie, PNCAP_C0_ESCAPE);                 // character set
    PUT8(pie, PNCAP_C0_ESC_CHAR_SET);
    PUT8(pie, coding);
  }
  PUTBUF(pie, name, strlen((const char*)name)); // handset name
  
  CLOSE(pie);
}



/** Constructs PNCAP_PIE message to indicate new message in voice mail box
 *
 *  The function creates a PIE <<Status Announcement>> with an information
 *  field <<MWI Status>> inside.
 *
 *  @note The PIE has to be sent in a CISS message and the PP has to acknowledge with the PIE <<Positive Acknowledgement>>.
 *
 *  @param  *pie   - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @param  am_cnt - number of new message; 0 to clear new message indication
 *  @return total length of the constructed PIE
 */
uint8_t pncap_pie_set_mwi_am
(
  uint8_t  *pie,  // destination buffer for PIE
  uint8_t  am_cnt // number of new message; 0 to clear new message indication
)
{
  // *************************************************************
  // ** PIE  <<Status Announcement>> ** --> MWI status
  // *************************************************************
  OPEN(pie, PNCAP_PIE_STATUS_ANNOUNCEMENT);

  PUT8(pie, PNCAP_STAT_ANCMT_MWI_NEW_MSG_T_BOX); // Information field "MWI status"
  PUT8(pie, 2);                                  // Length of the information field
  PUT8(pie, PNCAP_STAT_ANCMT_MWI_ANSWM);         // answering machine list
  PUT8(pie, am_cnt);                             // count
                 
  CLOSE(pie);
}



/** Constructs PNCAP_PIE message which indicates that we are about to send telephone book entries.
 *
 *  @note Once we send this message we need to wait for "data link confirm" with positive response (see
 *        function pncap_pie_find_phonebook_cfm() ).
 *        Once we receive the positive confirmation we are free to send entries using pncap_pie_set_phonebook_insert_entry()
 *  @note Data link request may be submitted either in a <<CC-SETUP>> or in a <<CC-INFO>> message.
 *
 *  @param  *pie    - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @param  int_num - the internal number of the data receiver
 *  @return total length of the constructed PIE
 */
uint8_t pncap_pie_set_phonebook_start_transfer
(
  uint8_t  *pie,   // destination buffer for PIE
  uint8_t  int_num // the internal number of the data receiver
)
{
  // *************************************************************
  // ** PIE  <<Escape to Data Feature>>
  // *************************************************************
  OPEN(pie, PNCAP_PIE_ESCAPE_TO_DATA_FEATURES);

  PUT8(pie, PNCAP_ETDF_LINK_REQ);              // Request a data link in order to send phonebook entries
  PUT8(pie, int_num);                          // The internal number of the data receiver
  PUT8(pie, PNCAP_DF_CT_PHONEBOOK_LIST_WRITE); // Sending of telephone book entries

  CLOSE(pie);
}


/** Constructs PNCAP_PIE message which writes an entry in the handset's local telephone book.
 *
 *  @note Before sending this message we should send:  pncap_pie_set_phonebook_start_transfer().
 *        When done with the transfer of entries we should call pncap_pie_set_phonebook_stop_transfer()

 *  @note This PNCAP_PIE message should be send using <<CC Info>> message in active call state.
 *
 *  @note The total length of the "name" and "number" cannot be more than 50bytes !
 *
 *  @note After each insert request we should be waiting for positive confirmation from the receiver.
 *        For this purpose we can use the function pncap_pie_find_phonebook_cfm()
 *
 *  @param  *pie   - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @param  *name  - Name of the phonebook entry (null terminated string)
 *  @param  *number- Telephone number for the entry (null terminated string) Possible characters are: 1,2,3,4,5,6,7,8,9,0,*,#,R
 *  @return total length of the constructed PIE
 */
uint8_t pncap_pie_set_phonebook_insert_entry
(
  uint8_t  *pie,   // destination buffer for PIE. Size should be at least PNCAP_PIE_MSG_MAX_TOTAL_LEN!
  const unsigned char *name, // name of the entry (if the name starts with space char (0x40) the entry will appear on top of the list)
  const unsigned char *num   // telephone number
)
{
  // *************************************************************
  // ** PIE  <<Escape to Data Feature>>
  // *************************************************************
  OPEN(pie, PNCAP_PIE_ESCAPE_TO_DATA_FEATURES);

  PUT8(pie, PNCAP_ETDF_PACKET);                   // Data packet
  PUT8(pie, 0);                                   // Number of data fragments to follow
  PUT8(pie, strlen((const char*)name));           // Name length
  PUTBUF(pie, name, strlen((const char*)name));   // Name
  PUT8(pie, strlen((const char*)num));            // Number length
  PUTBUF(pie, num, strlen((const char*)num));     // Number

  CLOSE(pie);
}



/** Constructs PNCAP_PIE message which indicates that we have finished sending telephone book entries
 *
 *  @note This PNCAP_PIE message should be send using <<CC Info>> message in active call state.
 *
 *  @param  *pie   - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @return total length of the constructed PIE
 */
uint8_t pncap_pie_set_phonebook_stop_transfer
(
    uint8_t  *pie // destination buffer for PIE
)
{
  // *************************************************************
  // ** PIE  <<Escape to Data Feature>>
  // *************************************************************
  OPEN(pie, PNCAP_PIE_ESCAPE_TO_DATA_FEATURES);

  PUT8(pie, PNCAP_ETDF_LINK_REL);

  CLOSE(pie);
}



uint8_t  pncap_pie_set_phonebook_Proceeding(uint8_t *pie)
{
	// *************************************************************
	// ** PIE  <<Escape to Data Feature>>
	// *************************************************************
	OPEN(pie, PNCAP_PIE_ESCAPE_TO_DATA_FEATURES);

	PUT8(pie, PNCAP_ETDF_LINK_PROCEEDING);

	CLOSE(pie);
}

uint8_t pncap_pie_Send_phonebook_cfm(uint8_t *pie)
{
	// *************************************************************
	// ** PIE  <<Escape to Data Feature>>
	// *************************************************************
	OPEN(pie, PNCAP_PIE_ESCAPE_TO_DATA_FEATURES);

	PUT8(pie, PNCAP_ETDF_LINK_CFM);

	CLOSE(pie);
}





/**  Constructs PNCAP_PIE message which tells the receiver to open the phone book
 *
 *  @note This message should be send using <<CC Info>> message in active call state.
 *
 *  @param  *pie   - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @return total length of the constructed PIE
 */
uint8_t  pncap_pie_set_phonebook_activate
(
    uint8_t *pie // destination buffer for PIE
)
{
  // *************************************************************
  // ** PIE  << Local Feature Activation >>
  // *************************************************************
  OPEN(pie, PNCAP_PIE_LOCAL_FEATURE_ACTIVATION);  //

  PUT8(pie, 0x06);                                // choosing numbers from the phone book

  CLOSE(pie);                                     // write length in header and return total PIE length
}




// Template for a PNCAP PIE set function
/** Constructs PNCAP_PIE message to ...
 *
 *  @param  *pie   - destination buffer for PIE. Size should be at least \ref PNCAP_PIE_MSG_MAX_TOTAL_LEN !
 *  @param  param1
 *  @return total length of the constructed PIE
 */
//uint8_t pncap_pie_set.........
//(
//  uint8_t  *pie,     // destination buffer for PIE
//  unsigned char  param1    // ..............
//)
//{
//  // *************************************************************
//  // ** PIE  <<              >>
//  // *************************************************************
//  OPEN(pie, PNCAP_PIE_ESCAPE_TO_DATA_FEATURES);  // write header
//
//  PUT8(pie, PNCAP_ETDF_LINK_REL);
//  
//  CLOSE(pie);                                    // write length in header and return total PIE length
//}
