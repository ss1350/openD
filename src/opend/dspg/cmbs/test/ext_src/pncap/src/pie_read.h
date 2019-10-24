/** @file pie_get.h
 *
 *  @brief Functions for parsing Gigaset PNCAP PIEs (Proprietary Information Element).
 *
 *  Created on: Jul 16, 2015
 *      Author: chavdar
 */
#ifndef PNCAP_PIE_READ_H_
#define PNCAP_PIE_READ_H_

#include <stdint.h>
#include <stddef.h>
#include "pncap_def.h"
#include "cmbs_api.h"
#include "pncap_codes.h"

uint8_t*  pncap_pie_find(uint8_t *data, size_t size, uint8_t pie_id);
bool      pncap_pie_next(uint8_t **data, size_t *data_size);


bool      pncap_pie_find_phonebook_cfm(uint8_t *data, size_t size, bool *accept_flag);


#endif /* PNCAP_PIE_READ_H_ */
////////////////////////////////////////////////////////////////////////////////////////////////////
//  EOF pie_read.h                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////
