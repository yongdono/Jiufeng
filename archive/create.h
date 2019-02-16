/**
 *  @file create.h
 *
 *  @brief create archive implementation file
 *
 *  @author Min Zhang
 *  
 *  @note
 *
 */

#ifndef ARCHIVE_CREATE_H
#define ARCHIVE_CREATE_H

/* --- standard C lib header files ----------------------------------------- */

/* --- internal header files ----------------------------------------------- */
#include "olbasic.h"
#include "archive.h"

/* --- constant definitions ------------------------------------------------ */

/* --- data structures ----------------------------------------------------- */

/* --- functional routines ------------------------------------------------- */
u32 writeToArchive(
    FILE * fpArchive, const u8 * pu8Fullpath,
    u8 * pu8Buffer, u32 u32BufLen, create_archive_param_t * pParam);

#endif /*ARCHIVE_CREATE_H*/

/*---------------------------------------------------------------------------*/

