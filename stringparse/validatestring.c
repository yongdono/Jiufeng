/**
 *  @file validatestring.c
 *
 *  @brief routines for validating string
 *
 *  @author Min Zhang
 *  
 *  @note
 *
 */

/* --- standard C lib header files -------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_limit.h"
#include "jf_err.h"
#include "jf_string.h"
#include "jf_option.h"

/* --- private data/data structure section ------------------------------------------------------ */

/* --- private routine section ------------------------------------------------------------------ */

/* --- public routine section ------------------------------------------------------------------- */

/** Validate string for alias. The alias can contain digit, alphabet, space and
 *  underscore, but not started with space.
 */
u32 jf_string_validateStringAlias(const olchar_t * pstrAlias)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olsize_t sIndex = 0, sLength = 0;
    olchar_t cSpace = ' ', cUnderScore = '_';

    if (pstrAlias == NULL)
    {
        u32Ret = JF_ERR_INVALID_ALIAS;
    }
    else if (pstrAlias[0] == cSpace)
    {
        u32Ret = JF_ERR_INVALID_ALIAS;
    }
    else
    {
        sLength = ol_strlen(pstrAlias);
    }

    for (sIndex = 0;
        (sIndex < sLength) && (u32Ret == JF_ERR_NO_ERROR);
        sIndex++)
    {
        if (!isdigit(pstrAlias[sIndex]) &&
            !isalpha(pstrAlias[sIndex]) &&
            (pstrAlias[sIndex] != cSpace) &&
            (pstrAlias[sIndex] != cUnderScore))
        {
            u32Ret = JF_ERR_INVALID_ALIAS;
        }
    }

    return u32Ret;
}

/** Validate string for username. The username can contain digit, alphabet,
 *  and underscore.
 */
u32 jf_string_validateStringUsername(const olchar_t * pstrUserName)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olsize_t sIndex = 0, sLength = 0;
    olchar_t cUnderScore = '_';

    assert(pstrUserName != NULL);

    sLength = ol_strlen(pstrUserName);

    for (sIndex = 0;
        (sIndex < sLength) && (u32Ret == JF_ERR_NO_ERROR);
        sIndex++)
    {
        if (!isdigit(pstrUserName[sIndex]) &&
            !isalpha(pstrUserName[sIndex]) &&
            (pstrUserName[sIndex] != cUnderScore))
        {
            return JF_ERR_INVALID_USER_NAME;
        }
    }

    return u32Ret;
}

/** Validate whether the input string is hex format
 */
u32 jf_string_validateHexString(const olchar_t * pstrHex, const olsize_t sHex)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olsize_t i;

    for (i = 0; i < sHex; i++)
    {
        if ((pstrHex[i] < '0') ||
            ((pstrHex[i] > '9') && (pstrHex[i] < 'A')) ||
            ((pstrHex[i] > 'F') && (pstrHex[i] < 'a')) ||
            (pstrHex[i] > 'f'))
        {
            return JF_ERR_INVALID_STRING;
        }
    }

    return u32Ret;
}

u32 jf_string_validateIntegerString(const olchar_t * pstrInteger, const olsize_t size)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    u32Ret = jf_option_validateIntegerString(pstrInteger, size);

    return u32Ret;
}

u32 jf_string_validateFloatString(const olchar_t * pstrFloat, const olsize_t size)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    u32Ret = jf_option_validateFloatString(pstrFloat, size);

    return u32Ret;
}

/*------------------------------------------------------------------------------------------------*/


