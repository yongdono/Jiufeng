/**
 *  @file jf_crc.h
 *
 *  @brief CRC header file which define crc algorithm like CRC32C, 32 bit CRC of the polynomial
 *   represented by 0x11EDC6F41.
 *
 *  @author Min Zhang
 *
 *  @note
 *  -# Routines declared in this file are included in jf_crc object.
 *  -# Cyclic redundancy check (CRC) is a type of function that takes as input a data stream of
 *   any length, and produces as output a value of a certain space, commonly a 32-bit integer.
 *
 */

#ifndef JIUTAI_CRC_H
#define JIUTAI_CRC_H

/* --- standard C lib header files -------------------------------------------------------------- */

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"

/* --- constant definitions --------------------------------------------------------------------- */

/** Flag to init the result before doing crc.
 */
#define JF_CRC_CRC32C_FLAG_INIT_RESULT         (0x1)

/** Flag to return the result with network byte order.
 */
#define JF_CRC_CRC32C_FLAG_NETWORK_BYTE_ORDER  (0x2)

/* --- data structures -------------------------------------------------------------------------- */

/** Define the crc32 buffer vector data type.
 */
typedef struct
{
    /**The buffer containing the data.*/
    u8 * jccv_pu8Buffer;
    /**The length of the data.*/
    u32 jccv_u32Len;
} jf_crc_crc32c_vec_t;

/* --- functional routines ---------------------------------------------------------------------- */

/** Calculate the crc32c.
 */
void jf_crc_crc32c(u8 * pu8Data, u32 u32Len, u32 u32Flags, u32 * pu32Result);

/** Calculate the crc32c vector.
 */
void jf_crc_crc32cVec(
    jf_crc_crc32c_vec_t * pjccv, u32 u32Count, u32 u32Flags, u32 * pu32Result);

#endif /*JIUTAI_CRC_H*/

/*------------------------------------------------------------------------------------------------*/


