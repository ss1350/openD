/** @file etp.c
 *
 *  @brief Functions for constructing DECT_IE_ETP messages (DECT Information Element Escape To Proprietary).
 *
 *  Created on: Jul 6, 2015
 *      Author: levkov_cha_ext
 *
 */
#include "etp.h"
#include "pie.h"



//------------------------------------------------------------------------------------//
//                               Macros                                               //
//------------------------------------------------------------------------------------//
#define PUT8(p,v)   *(p)++ = (uint8_t)(v) // puts and moves pointer
#define PUT8_(p,v)  *(p)   = (uint8_t)(v) // only puts
#define GETPTR8(p)  p, p++   // gets address and moves
#define GET(p)  *p, p++      // gets value of the pointer and increments the address
#define PUTBUF(p,buf,buf_len) memmove(p, buf, buf_len), p+=buf_len // copies "buf" with "len" into "p", then moves the the pointer p




//------------------------------------------------------------------------------------//
//                         functions constructing ETP messages                        //
//------------------------------------------------------------------------------------//


/** Call this function to start constructing DECT_IE_ETP message
 *
 *  This actually writes the DECT_IE_ETP message header into the supplied message buffer.
 *  After we call pncap_etp_open() we are ready to append PIEs using the function pncap_etp_append()
 *
 *  @param  *msg - context of the DECT_IE_ETP message: \n
 *                 msg->buff - should be set to point to the beginning of the buffer where the message will be assembled.  \n
 *                 msg->buff_size - should be set to the size of the buffer.
 *  @return true - if succeeded. false - in case of an error (user can check the error code by reading "msg->err_code")
 */
bool pncap_etp_open(struct pncap_etp_msg *msg)
{
  if(msg == NULL) return FALSE;


  if(msg->buff == NULL)
  {
    msg->err_code = PNCAP_ETP_NULL_POINTER;
    return FALSE;
  }


  msg->len = 0;
  msg->err_code = PNCAP_ETP_NO_ERROR;
  msg->priv.len_pos  = NULL;


  // Make sure we have enough space to insert the header
  if(msg->buff_size < DECT_IE_ETP_HEADER_LENGTH)
  {
    msg->err_code = PNCAP_ETP_BUFFER_TOO_SMALL;
    return FALSE;
  }


  // Construct DECT_IE_ETP header
  PUT8(msg->buff, DECT_IE_ETP_ID);            //
  msg->buff_size--;
  msg->len++;
  msg->priv.len_pos = GETPTR8(msg->buff);     // remember the position of the Length field which will be written upon calling pncap_close()
  msg->buff_size--;
  msg->len++;
  PUT8(msg->buff, PNCAP_PIE_DISCRIMINATE_EMC);// Equipment Manufacture Code
  msg->buff_size--;
  msg->len++;
  PUT8(msg->buff, PNCAP_PIE_SIEMENS_ID_HIGH); // EMC for Siemens
  msg->buff_size--;
  msg->len++;
  PUT8(msg->buff, PNCAP_PIE_SIEMENS_ID_LOW);  // EMC for Siemens
  msg->buff_size--;
  msg->len++;

  return TRUE;
}


/** Appends proprietary data to a DECT_IE_ETP message.
 *
 *  @note Before being able to append messages we must call pncap_etp_open()
 *
 *  @param  *msg - pointer to the structure which was used when calling pncap_etp_open()
 *  @param  *data - proprietary data to be inserted
 *  @param  data_len - length of the proprietary data in bytes
 *  @return true - if succeeded. false - in case of an error (user can check the error code by reading "msg->err_code")
 */
bool pncap_etp_append(struct pncap_etp_msg *msg, const uint8_t *data, uint8_t data_len)
{
  if(msg == NULL)
  {
    return FALSE;
  }

  if(msg->buff==NULL || data==NULL)
  {
    msg->err_code = PNCAP_ETP_NULL_POINTER;
    return FALSE;
  }

  // Check if "buff" has enough space for the data
  if(msg->buff_size < data_len)
  {
    msg->err_code = PNCAP_ETP_BUFFER_TOO_SMALL;
    return FALSE;
  }

  // Check if the length of DECT_IE_ETP message exceeds the limits
  if(msg->len+data_len > DECT_IE_ETP_MSG_MAX_LENGTH)
  {
    msg->err_code = PNCAP_ETP_DECT_MSG_TOO_LONG;
    return FALSE;
  }

  // Check if pncap_etp_open() was called
  if(msg->priv.len_pos == NULL)
  {
    msg->err_code = PNCAP_ETP_OPEN_MUST_BE_CALLED_BEFORE_APPEND;
    return FALSE;
  }


  // Insert the data
  PUTBUF(msg->buff, data, data_len);
  msg->buff_size -=data_len;
  msg->len = (uint8_t)(msg->len + data_len);

  return TRUE;
}




/** When all the proprietary data has been inserted the user should call this function to complete
 *  the construction of the message.
 *
 *  @note This actually writes the length of the DECT_IE_ETP message into the header
 *
 *  @param *msg - pointer to the structure which was used when calling pncap_etp_open()  \n
 *                msg->buff [OUT] - will point to the first free byte after the newly inserted ETP message.  \n
 *                msg->buff_size [OUT] - remaining free bytes in buff.  \n
 *                msg->len [OUT] - total length of the ETP message (same as the return value)
 *
 *  @return total length of the DECT_IE_ETP message (including header). 0 if error occurred.
 */
uint8_t pncap_etp_close(struct pncap_etp_msg *msg)
{
  if(msg == NULL) return 0;

  if(msg->buff == NULL)
  {
    msg->err_code = PNCAP_ETP_NULL_POINTER;
    return 0;
  }

  if(msg->priv.len_pos == NULL)
  {
    msg->err_code = PNCAP_ETP_OPEN_MUST_BE_CALLED_BEFORE_CLOSE;
    return 0;
  }

  // write the length of the ETP messages inside the header
  *(msg->priv.len_pos) = (unsigned char) ( msg->len - (DECT_IE_ETP_HEADER_LENGTH - DECT_IE_ETP_EMC_LENGTH) );


  // return total length of the ETP message (should not get bigger than DECT_IE_ETP_MSG_MAX_LENGTH)
  return msg->len;
}


/** Moves the supplied pointer from the beginning of the ETP message to the
 *  beginning of the proprietary data.
 *
 *  @note The functions validates the ETP header.
 *
 *  @param  **data [IN]  - should be set to point to the beginning of the ETP message (i.e. DECT_IE_ETP_ID) \n
 *                 [OUT] - will be pointing to the beginning of the proprietary data (i.e. first PIE)
 *
 *  @param *data_size [IN]- the size of the input data  \n
 *                    [OUT] - the length of the proprietary data (as specified in the header)
 *
 *  @return true - if the pointer was moved successfully. false - in case of an error.
 */
bool  pncap_etp_get_proprietary_data(uint8_t **data, size_t *data_size)
{
  uint8_t *p   = *data;
  size_t  size = *data_size;

  uint8_t val;
  uint8_t etp_len;


  // Valid ETP should have at least header
  if(size < DECT_IE_ETP_HEADER_LENGTH)
  {
    return FALSE;
  }

  // ETP_ID
  val = GET(p); // read etp_id and move to length
  size--;       // update remaining size

  if(val!= DECT_IE_ETP_ID) // Make sure we are at the beginning of an ETP message
  {
    return FALSE;
  }

  // LEN
  etp_len = GET(p); // read length and move to EMC_tag
  size--;           // update remaining size
  if(etp_len > size )
  {
    return FALSE;
  }


  // EMC
  val = GET(p);
  size--;           // update remaining size
  if(val !=  PNCAP_PIE_DISCRIMINATE_EMC)// This should be the EMC_tag
  {
    return FALSE;
  }


  val = GET(p);
  size--;           // update remaining size
  if(val !=  PNCAP_PIE_SIEMENS_ID_HIGH)// This should be the Siemens EMC_high
  {
    return FALSE;
  }


  val = GET(p);
  size--;           // update remaining size
  if(val !=  PNCAP_PIE_SIEMENS_ID_LOW)// This should be the Seimens EMC_low
  {
    return FALSE;
  }


  // All is OK - now we can move the supplied pointer to the first byte of the proprietary data
  *data = p;
  *data_size = (size_t) (etp_len-DECT_IE_ETP_EMC_LENGTH);

  return TRUE;
}
