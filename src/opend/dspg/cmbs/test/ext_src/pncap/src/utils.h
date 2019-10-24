/** @file utils.h
 *
 *  @brief Helper macros for creating PNCAP PIE messages and inserting them into DECT_IE_ETP message.
 *
 *  Created on: Jul 6, 2015
 *    Author: levkov_cha_ext
 *
 *
 *
 *  Example usage:                                                                                                                                     <br>
 *  --------------                                                                                                                                     <br>
 *    PNCAP_PIE_HELPER();                           //declare buffer where PIEs will be assembled                                                      <br>
 *    struct pncap_etp_msg  etp;                                                                                                                       <br>
 *    etp.buff = DECT_msg;                                                                                                                             <br>
 *    etp.buff_size = sizeof(DECT_msg);                                                                                                                <br>
 *                                                                                                                                                     <br>
 *    pncap_etp_open(&etp);                         // Write the Header of the ETP message                                                             <br>
 *                                                                                                                                                     <br>
 *    _pncap_pie_mwi_am(5);                         // Create the PIE holding the PNCAP message                                                        <br>
 *    pncap_etp_append(&etp, _pie);                 // Insert PIE into ETP message                                                                     <br>
 *    _pncap_pie_int_name_number(" Int 1", 0, 1);   // Create another PIE                                                                              <br>
 *    pncap_etp_append(&etp, _pie);                 // Insert it                                                                                       <br>
 *                                                                                                                                                     <br>
 *    pncap_etp_close(&etp);                        // Once all the proprietary data is inserted we are ready to write the Length of the ETP message   <br>
 */


#ifndef PNCAP_UTILS_H_
  #define PNCAP_UTILS_H_

#include "pie.h"

//------------------------------------------------------------------------------------//
//                        Helper Macros                                               //
//------------------------------------------------------------------------------------//
#define PNCAP_PIE_HELPER()  unsigned char  pie_buf[PNCAP_PIE_MSG_MAX_TOTAL_LEN]; uint8_t pie_len  ///< define the helper buffer and pie_len that the helper functions from below will use

//
#define _pie  pie_buf, pie_len

//
#define _pncap_pie_set_mwi_am(am_cnt)                pie_len = pncap_pie_set_mwi_am   (pie_buf, am_cnt)

#define _pncap_pie_set_date_time(date_and_time)  pie_len = pncap_pie_set_date_time(pie_buf, date_and_time)

#define _pncap_pie_set_int_name_number(name, coding, number)    pie_len = pncap_pie_set_int_name_number(pie_buf, name, coding, number)

#define _pncap_pie_set_phonebook_start_transfer(int_num)        pie_len = pncap_pie_set_phonebook_start_transfer(pie_buf, int_num)
#define _pncap_pie_set_phonebook_insert_entry(name, telephone)  pie_len = pncap_pie_set_phonebook_insert_entry(pie_buf,  name, telephone)
#define _pncap_pie_set_phonebook_stop_transfer()                pie_len = pncap_pie_set_phonebook_stop_transfer(pie_buf)

#define _pncap_pie_set_phonebook_activate()          pie_len = pncap_pie_set_phonebook_activate(pie_buf)



//------------------- -----------------------------------------------------------------//
//                        Helper Functions                                             //
//-------------------------------------------------------------------------------------//
unsigned char ucBin2Bcd(unsigned char);

#endif //PNCAP_UTILS_H_
////////////////////////////////////////////////////////////////////////////////////////////////////
//  EOF utils.h                                                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////
