/**
 *  @file jf_hex.c
 *
 *  @brief Implementation file for functional routines of hex data.
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
#include "jf_hex.h"
#include "jf_err.h"

/* --- private data/data structure section ------------------------------------------------------ */

/*xxxxxxxxh: xx xx xx xx ...*/
#define BYTE_HEXSTR_HEADER_LENGTH  (11)
#define MIN_BYTE_HEXSTR_LENGTH     (BYTE_HEXSTR_HEADER_LENGTH + 3)
#define MAX_BYTE_PER_HEXSTR_LINE   (16)

/*xxxxxxxxh: xx xx xx xx ...*/
#define WORD_HEXSTR_HEADER_LENGTH  (11)
#define MIN_WORD_HEXSTR_LENGTH     (WORD_HEXSTR_HEADER_LENGTH + 5)
#define MAX_WORD_PER_HEXSTR_LINE   (12)

/*xxxxxxxxh: xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx ; abcdefghijklmnop*/
#define MIN_BYTE_HEXSTR_WITH_ASCII_LENGTH    (63)

/* --- private routine section ------------------------------------------------------------------ */

/* --- public routine section ------------------------------------------------------------------- */

olsize_t jf_hex_convertByteDataToString(
    const u8 * pu8Data, const olsize_t sData, const olsize_t sOffset, 
    olchar_t * pstrHex, olsize_t sStr)
{
    olsize_t sLimited = 0, sLen = 0;
    olchar_t strTemp[8];
    
    assert((pu8Data != NULL) && (sData != 0) &&
           (sOffset < sData) && (pstrHex != NULL));
    assert(sStr >= MIN_BYTE_HEXSTR_LENGTH);

    sLen = sData - sOffset;
    sLimited = (sStr - BYTE_HEXSTR_HEADER_LENGTH) / 3;
    if (sLimited > MAX_BYTE_PER_HEXSTR_LINE)
    {
        sLimited = MAX_BYTE_PER_HEXSTR_LINE;
    }
        
    if (sLimited > sLen)
    {
        sLimited = sLen;
    }
        
    ol_sprintf(pstrHex, "%08xh:", sOffset);

    sLen = 0;
    while (sLen < sLimited)
    {
        ol_sprintf(strTemp, " %02x", pu8Data[sOffset + sLen]);
        ol_strcat(pstrHex, strTemp);
        sLen ++;
    }

    return sLen;
}

olsize_t jf_hex_convertWordDataToString(
    const u16 * pu16Data, const olsize_t sData, const olsize_t sOffset, 
    olchar_t * pstrHex, olsize_t sStr)
{
    u32 sLimited = 0, sLen;
    olchar_t strTemp[8];
    
    assert((pu16Data != NULL) && (sData != 0) &&
           (sOffset < sData) && (pstrHex != NULL));
    assert(sStr >= MIN_WORD_HEXSTR_LENGTH);

    sLen = sData - sOffset;
    sLimited = (sStr - WORD_HEXSTR_HEADER_LENGTH) / 5;
    if (sLimited > MAX_WORD_PER_HEXSTR_LINE)
    {
        sLimited = MAX_WORD_PER_HEXSTR_LINE;
    }
        
    if (sLimited > sLen)
    {
        sLimited = sLen;
    }
        
    ol_sprintf(pstrHex, "%08xh:", sOffset);

    sLen = 0;
    while (sLen < sLimited)
    {
        ol_sprintf(strTemp, " %04x", pu16Data[sOffset + sLen]);
        ol_strcat(pstrHex, strTemp);
        sLen++;
    }
    
    return sLen;
}

olsize_t jf_hex_convertByteDataToStringWithAscii(
    const u8 * pu8Data, const olsize_t sData, const olsize_t sOffset, 
    olchar_t * pstrHex, olsize_t sStr)
{
    olsize_t sLimited = 0, sLen = 0;
    olchar_t strTemp[8];
    
    assert((pu8Data != NULL) && (sData != 0) &&
           (sOffset < sData) && (pstrHex != NULL));
    assert(sStr >= MIN_BYTE_HEXSTR_WITH_ASCII_LENGTH);

    sLen = sData - sOffset;

    sLimited = MAX_BYTE_PER_HEXSTR_LINE;
    if (sLimited > sLen)
    {
        sLimited = sLen;
    }

    ol_sprintf(pstrHex, "%08xh:", sOffset);

    sLen = 0;
    while (sLen < sLimited)
    {
        ol_sprintf(strTemp, " %02x", pu8Data[sOffset + sLen]);
        ol_strcat(pstrHex, strTemp);
        sLen ++;
    }

    sLen = sLimited;
    while (sLen < MAX_BYTE_PER_HEXSTR_LINE)
    {
        ol_strcat(pstrHex, "   ");
        sLen ++;
    }

    ol_strcat(pstrHex, " ; ");

    sLen = 0;
    while (sLen < sLimited)
    {
        if (isprint(pu8Data[sOffset + sLen]))
            ol_sprintf(strTemp, "%c", pu8Data[sOffset + sLen]);
        else
            ol_sprintf(strTemp, "%c", '.');

        ol_strcat(pstrHex, strTemp);
        sLen ++;
    }

    return sLen;
}

void jf_hex_dumpByteDataBuffer(const u8 * pu8Data, const olsize_t sLen)
{
    olsize_t sIndex = 0, sDumped = 0xff;
    olchar_t strLine[80];

    while (sIndex < sLen)
    {
        sDumped = jf_hex_convertByteDataToStringWithAscii(
            pu8Data, sLen, sIndex, strLine, 80);
        if (sDumped > 0)
        {
            sIndex += sDumped;
            ol_printf("%s\n", strLine);
        }
    }

}

olsize_t jf_hex_convertStringToHex(
    const olchar_t * pstr, const olsize_t sStr, u8 * pu8Hex, olsize_t sHexLen)
{
    olsize_t sLen = 0, start;
    u32 u32Hex;
    olchar_t u8Temp[8];
    
    assert((pstr != NULL) && (sStr > 0) && (pu8Hex != NULL) && (sHexLen > 0));

    start = 0;
    ol_bzero(u8Temp, sizeof(u8Temp));
    while ((sLen < sHexLen) && (start + 2 <= sStr))
    {
        ol_strncpy(u8Temp, &(pstr[start]), 2);

        ol_sscanf(u8Temp, "%x", &u32Hex);
        pu8Hex[sLen] = (u8)u32Hex;

        start += 2;

        sLen ++;
    }

    return sLen;
}

olsize_t jf_hex_convertHexToString(
    olchar_t * pstr, olsize_t sStr, const u8 * pu8Hex, const olsize_t sHex)
{
    olsize_t sLen, sIndex;

    sLen = 0;
    sIndex = 0;

    while ((sLen + 2 <= sStr) && (sIndex < sHex))
    {
        ol_sprintf(pstr + sLen, "%02x", pu8Hex[sIndex]);

        sLen += 2;
        sIndex ++;
    }

    return sLen;
}

/*------------------------------------------------------------------------------------------------*/


