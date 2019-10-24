/** @file pie.h
 *
 *  @brief Include this file to have access to pie_write.h and pie_read.h functions.
 *
 *  Created on: Jul 16, 2015
 *      Author: chavdar
 */

#ifndef PNCAP_PIE_H_
#define PNCAP_PIE_H_

#include "pie_write.h"
#include "pie_read.h"

/** \verbatim
 * The format of the PIE message is the following
 *
 *  no   |    Octet, Coding:                     |  8 7 6 5 4 3 2 1
 * -----------------------------------------------------------------
 *  1    |    Proprietary info element ID (PIEI) |  x x x x x x x x |      --
 *  2    |    Length of PIE contents (L)         |  x x x x x x x x |      __|<--- PIE header (2bytes)
 *  3    |    Content of PIE                     |                  |        |
 *  ...  |    ......                             |                  |        |<--- PIE content
 *  ...  |    .......                            |                  |        |
 *  L+2  |    End of content                     |                  |      --
 * ----------------------------------------------------------
 *
 *  Note: total length of PIE (including header) cannot be more than 56bytes
 *
 * \endverbatim
 */


/** The maximum transmittable DECT message length of 63 Bytes has to be taken into account.
 * 2 bytes are needed for the message header, 5 Bytes for the ETP-Header. That is, e.g. a CC-INFO
 * NWK-message containing only the ETP may contain 63-2-5=56 Bytes proprietary information.
 * Messages containing more Information Elements leave less space for ETPs to be transmitted
 */
#define PNCAP_PIE_MSG_MAX_TOTAL_LEN       56 ///< Maximum total length of a PIE (including its header)
#define PNCAP_PIE_HEADER_LEN              2  ///< Contains PIEI and Length
#define PNCAP_PIE_MSG_MAX_DATA_LEN        (PNCAP_PIE_MSG_MAX_TOTAL_LEN-PNCAP_PIE_HEADER_LEN) ///< Maximum amount of proprietary data within a PIE message



#endif /* PNCAP_PIE_H_ */
////////////////////////////////////////////////////////////////////////////////////////////////////
//  EOF pncap_pie.h                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////
