/** @file etp.h
 *
 *  @brief Functions for constructing DECT_IE_ETP messages (DECT Information Element Escape To Proprietary)
 *
 *  Example usage:                                                                                    <br>
 *  --------------                                                                                    <br>
 *    struct pncap_etp_msg  etp;                                                                      <br>
 *    etp.buff = DECT_msg;                // supply a buffer where the ETP message will be written    <br>
 *    etp.buff_size = sizeof(DECT_msg);   // specify the size of the buffer                           <br>
 *                                                                                                    <br>
 *    pncap_etp_open(&etp);                                       // open a etp msg (this actually writes the header)   <br>
 *    pncap_etp_append(&etp, <proprietary_data>, <size_of_data>); // insert data                                        <br>
 *    pncap_etp_close(&etp);                                      // close the etp msg (this writes the length of the message inside the header)  <br>
 *
 */

#ifndef PNCAP_ETP_H_
 #define PNCAP_ETP_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cmbs_api.h"

/** \verbatim
 * The format of the DECT_IE_ETP message containing some PIEs is the following:
 *
 *  no   |    Octet, Coding:              |  8 7 6 5 4 3 2 1
 * ----------------------------------------------------------
 *  1    |    ESCAPE-TO-PROPRIETARY (0x7b)|  0 1 1 1 1 0 1 1 |      --
 *  2    |    Length of contents (L)      |  x x x x x x x x |        |
 *  3    |    Equipment Manufacturer Code |  1 0 0 0 0 0 0 1 |        | <--- DECT_IE_ETP header (5bytes)
 *  4    |    EMC (Siemens ID)            |  0 0 0 0 0 0 0 0 |        |
 *  5    |    EMC (Siemens ID)            |  0 0 0 0 0 0 1 0 |      --
 *  6    |    PIE1 ID                     |  ...             |      --
 *  7    |    Length of PIE1 (L1)         |                  |        |
 *  8    |    Contents of PIE1            |                  |        |
 *  ...  |    ...                         |                  |        |
 *  7+L1 |    End of contents of PIE1     |                  |        |
 *  ...  |    PIE2 ID                     |                  |        | <--- Proprietary data - PIE(s)
 *       |    Length of PIE2 (L2)         |                  |        |
 *       |    Contents of PIE2            |                  |        |
 *  ...  |    ...                         |                  |        |
 *  L+2  |    End of contents of PIE2     |                  |      --
 * ----------------------------------------------------------
 *
 *  Note: Each DECT_IE_ETP message can hold one or more PIEs (PNCAP messages)
 *
 * \endverbatim
 */


#define DECT_IE_ETP_ID          0x7B ///< Identifier for DECT Information Element - Escape To Proprietary


/** The maximum transmittable DECT message length of 63 Bytes has to be taken into account.
 * 2 bytes are needed for the message header, 5 Bytes for the ETP-Header. That is, e.g. a CC-INFO
 * NWK-message containing only the ETP may contain 63-2-5=56 Bytes proprietary information.
 * Messages containing more Information Elements leave less space for ETPs to be transmitted
 */
#define DECT_IE_ETP_MSG_MAX_LENGTH          61 ///< Maximum total length of a DECT_IE_ETP message (including its header)
#define DECT_IE_ETP_HEADER_LENGTH            5 ///< Length of the header of an ETP message (
#define DECT_IE_ETP_EMC_LENGTH               3 ///< Length of the EMC_ID plus 2 bytes for the Siemens ID
#define DECT_IE_ETP_PROP_DATA_MAX_LENGTH    (DECT_IE_ETP_MSG_MAX_LENGTH-DECT_IE_ETP_HEADER_LENGTH) ///< Max length of proprietary data that we can keep inside an ETP message



/**
 *  Possible errors that can occur while constructing the DECT_IE_ETP message
 */
enum pncap_error_codes
{
  // errors while constructing a message
  PNCAP_ETP_NO_ERROR,                         ///< All is fine
  PNCAP_ETP_NULL_POINTER,                     ///< Null pointer was passed as parameter
  PNCAP_ETP_BUFFER_TOO_SMALL,                 ///< If the supplied buffer can't fit anymore data
  PNCAP_ETP_OPEN_MUST_BE_CALLED_BEFORE_APPEND,///< If we are trying to append data to the message before opening one
  PNCAP_ETP_OPEN_MUST_BE_CALLED_BEFORE_CLOSE, ///< If we are trying to close a message before opening one
  PNCAP_ETP_DECT_MSG_TOO_LONG,                ///< If we are trying to construct a DECT message which is too long \see DECT_IE_ETP_MSG_MAX_LENGTH
};


/**
 * This structure is used for holding the context of the DECT_IE_ETP message
 */
struct pncap_etp_msg
{
  uint8_t   *buff;     ///< [IN] The buffer where the DECT_IE_ETP message will be constructed.
  size_t    buff_size; ///< [IN] Size of "buff" in bytes.
  uint8_t   len;       ///< [OUT] Total length of the ETP message (including its header)

  enum  pncap_error_codes err_code;  ///< [OUT] Error code for the last operation (user is responsible for clearing it in case of an error)

  /** Keeping private data - not interesting for the user */
  struct
  {
    uint8_t *len_pos;   ///< Address where the total length of the DECT_IE_ETP must be written (this is done upon calling close())
  }priv;
};


//------------------------------------------------------------------------------------//
//                         functions constructing ETP message                         //
//------------------------------------------------------------------------------------//
bool pncap_etp_open(struct pncap_etp_msg *msg);
bool pncap_etp_append(struct pncap_etp_msg *msg, const uint8_t *data, uint8_t data_len);
uint8_t  pncap_etp_close(struct pncap_etp_msg *msg);


//------------------------------------------------------------------------------------//
//                         functions parsing ETP message                              //
//------------------------------------------------------------------------------------//
bool  pncap_etp_get_proprietary_data(uint8_t **data, size_t *data_size);


#endif //PNCAP_ETP_H_
////////////////////////////////////////////////////////////////////////////////////////////////////
//  EOF etp.h                                                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////
