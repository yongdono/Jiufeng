/**
 *  @file jf_dynlib.h
 *
 *  @brief Dynamic library header file. Provide some functional routine for dynamic library
 *
 *  @author Min Zhang
 *
 *  @note Routines declared in this file are included in jf_dynlib object
 *  @note Link with jf_jiukun library for memory allocation
 *  @note Link with dl library on Linux platform
 *  
 */

#ifndef JIUTAI_DYNLIB_H
#define JIUTAI_DYNLIB_H

/* --- standard C lib header files -------------------------------------------------------------- */

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_err.h"

/* --- constant definitions --------------------------------------------------------------------- */
typedef void  jf_dynlib_t;

/* --- data structures -------------------------------------------------------------------------- */

/* --- functional routines ---------------------------------------------------------------------- */

u32 jf_dynlib_load(const olchar_t * pstrLibFile, jf_dynlib_t ** ppLib);

u32 jf_dynlib_unload(jf_dynlib_t ** ppLib);

u32 jf_dynlib_getSymbolAddress(
    jf_dynlib_t * pLib, const olchar_t * pstrSymbol, void ** ppAddress);

#endif /*JIUTAI_DYNLIB_H*/

/*------------------------------------------------------------------------------------------------*/


