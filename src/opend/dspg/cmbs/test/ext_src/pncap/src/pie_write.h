/** @file pie_write.h
 *
 *  @brief Functions for constructing Gigaset PNCAP PIEs (Proprietary Information Element).
 *
 *  Created on: Jul 6, 2015
 *      Author: levkov_cha_ext
 *
 */
#ifndef PNCAP_PIE_WRITE_H_
  #define PNCAP_PIE_WRITE_H_

#include <stdint.h>
#include <stddef.h>
#include "pncap_def.h"
#include "cmbs_api.h"
#include "pncap_codes.h"

uint8_t  pncap_pie_set_mwi_am         (uint8_t *pie, uint8_t  am_cnt);
uint8_t  pncap_pie_set_date_time      (uint8_t *pie, struct pncap_date_and_time  *date_time);
uint8_t  pncap_pie_set_int_name_number(uint8_t *pie, const unsigned char *intName, uint8_t coding, uint8_t intNum);

uint8_t  pncap_pie_set_phonebook_start_transfer(uint8_t *pie, uint8_t  int_num);
uint8_t  pncap_pie_set_phonebook_insert_entry(uint8_t *pie, const  unsigned char *name, const  unsigned char *number);
uint8_t  pncap_pie_set_phonebook_stop_transfer(uint8_t *pie);

uint8_t  pncap_pie_set_phonebook_activate(unsigned char *pie);
uint8_t pncap_pie_Send_phonebook_cfm(uint8_t *pie);
uint8_t  pncap_pie_set_phonebook_Proceeding(uint8_t *pie);

#endif //PNCAP_PIE_WRITE_H_
////////////////////////////////////////////////////////////////////////////////////////////////////
//  EOF pie_write.h                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////
