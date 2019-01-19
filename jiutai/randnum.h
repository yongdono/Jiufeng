/**
 *  @file randnum.h
 *
 *  @brief The random number common object header file.
 *
 *  @author Min Zhang
 *  
 *  @note
 *
 */

#ifndef JIUTAI_RANDNUM_H
#define JIUTAI_RANDNUM_H

/* --- standard C lib header files ----------------------------------------- */

/* --- internal header files ----------------------------------------------- */
#include "olbasic.h"

/* --- constant definitions ------------------------------------------------ */

/* --- data structures ----------------------------------------------------- */


/* --- functional routines ------------------------------------------------- */

/** Get random u32 number in range
 *  
 *  @param u32Lower [in] the lower edge of the range
 *  @param u32Upper [in] the upper edge of the range
 *
 *  @return the random u32 number
 */
u32 getRandomU32InRange(u32 u32Lower, u32 u32Upper);


#endif /*JIUTAI_RANDNUM_H*/

/*---------------------------------------------------------------------------*/


