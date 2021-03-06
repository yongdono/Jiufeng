/**
 *  @file extract.c
 *
 *  @brief Extract archive implementation file
 *
 *  @author Min Zhang
 *
 *  @note
 *  
 */

/* --- standard C lib header files -------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_limit.h"
#include "jf_err.h"
#include "jf_filestream.h"
#include "jf_dir.h"
#include "jf_string.h"
#include "jf_archive.h"

#include "archivecommon.h"

/* --- private data/data structure section ------------------------------------------------------ */

/* --- private routine section ------------------------------------------------------------------ */
/* expect the u32Size, otherwise error */
static u32 _readDataForArchive(
    ar_file_t * pafArchive, u8 * pu8Buffer, olsize_t u32Size)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olsize_t u32Read;

    u32Read = u32Size;
    u32Ret = readArFile(pafArchive, pu8Buffer, &u32Read);
    if (u32Ret == JF_ERR_NO_ERROR)
    {
        if (u32Read != u32Size)
            u32Ret = JF_ERR_ARCHIVE_CORRUPTED;
    }

    return u32Ret;
}

static u32 _getMemberFilename(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    u8 u8DirDepth;
    olchar_t strFullpath[JF_LIMIT_MAX_PATH_LEN];
    olchar_t strFilename[ARCHIVE_BLOCK_SIZE];

    if (pah->ah_u8DirDepth == AH_DIR_DEPTH_NULL)
    {
        ol_bzero(pfh->fh_strFullpath, JF_LIMIT_MAX_PATH_LEN);
    }

    if (pfh->fh_strFullpath[0] == '\0')
    {
        if (pah->ah_u8NameLen == AH_NAME_LEN_LONG)
        {
            u32Ret = _readDataForArchive(
                pfh->fh_pafArchive, (u8 *)strFilename, ARCHIVE_BLOCK_SIZE);
            if (u32Ret == JF_ERR_NO_ERROR)
            {
                ol_strncpy(pfh->fh_strFullpath, strFilename, JF_LIMIT_MAX_PATH_LEN - 1);
            }
        }
        else
        {
            ol_strcpy(pfh->fh_strFullpath, (olchar_t *)pah->ah_u8Name);
        }
    }
    else
    {
        ol_strcpy(strFullpath, pfh->fh_strFullpath);
        u8DirDepth = pah->ah_u8DirDepth;
        while (u8DirDepth > 0)
        {
            jf_file_getDirectoryName(
                strFullpath, JF_LIMIT_MAX_PATH_LEN, pfh->fh_strFullpath);
			strcpy(pfh->fh_strFullpath, strFullpath);
            u8DirDepth --;
        }

        if (pah->ah_u8NameLen == AH_NAME_LEN_LONG)
        {
            u32Ret = _readDataForArchive(
                pfh->fh_pafArchive, (u8 *)strFilename, ARCHIVE_BLOCK_SIZE);
            if (u32Ret == JF_ERR_NO_ERROR)
            {
                ol_snprintf(
                    pfh->fh_strFullpath, JF_LIMIT_MAX_PATH_LEN - 1, "%s%c%s",
                    strFullpath, PATH_SEPARATOR, strFilename);
            }
        }
        else
        {
            ol_snprintf(
                pfh->fh_strFullpath, JF_LIMIT_MAX_PATH_LEN - 1, "%s%c%s",
                strFullpath, PATH_SEPARATOR, pah->ah_u8Name);
        }
    }

    return u32Ret;
}

static u32 _validateHeader(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;


    return u32Ret;
}

static u32 _getFileMode(archive_header_t * pah, jf_file_mode_t * pMode)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    u32Ret = jf_string_getU32FromString(
        (olchar_t *)pah->ah_u8Mode, ol_strlen((olchar_t *)pah->ah_u8Mode), pMode);

    *pMode = *pMode & JF_FILE_MODE_PERM_MASK;

    return u32Ret;
}

static u32 _writeFile(archive_header_t * pah, file_handler_t * pfh, u64 u64Size)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    FILE * fp = NULL;
    olsize_t u32Len;
    u64 u64Len;
    jf_file_mode_t mode = 0;

    _getFileMode(pah, &mode);
    u32Ret = jf_file_create(pfh->fh_strFullpath, mode);
    if (u32Ret == JF_ERR_NO_ERROR)
    {
        u32Ret = jf_filestream_open(pfh->fh_strFullpath, "wb", &fp);
        if (u32Ret == JF_ERR_NO_ERROR)
        {
            u64Len = u64Size;

            while ((u64Len > 0) && (u32Ret == JF_ERR_NO_ERROR))
            {
                u32Len = pfh->fh_sBuf;
                if (u64Len < (u64)u32Len)
                    u32Len = (u32)u64Len;

                u32Ret = _readDataForArchive(
                    pfh->fh_pafArchive, pfh->fh_pu8Buffer, u32Len);
                if (u32Ret == JF_ERR_NO_ERROR)
                {
                    u32Ret = jf_filestream_writen(fp, pfh->fh_pu8Buffer, u32Len);
                    if (u32Ret == JF_ERR_NO_ERROR)
                    {
                        u64Len -= (u64)u32Len;
                    }
                }
                else if (u32Ret == JF_ERR_END_OF_FILE)
                    u32Ret = JF_ERR_ARCHIVE_CORRUPTED;
            }

            jf_filestream_close(&fp);
        }
    }

    return u32Ret;
}

static u32 _extractRegularFile(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    u64 u64Size, u64AlignedSize;
    olsize_t sLen;

    sLen = ol_strlen((olchar_t *)pah->ah_u8Size);
    u32Ret = jf_string_getU64FromString((olchar_t *)pah->ah_u8Size, sLen, &u64Size);
    if (u32Ret == JF_ERR_NO_ERROR)
    {
        jf_logger_logInfoMsg("file size: %llu", u64Size);
        u64AlignedSize = ARCHIVE_ALIGH_SIZE(u64Size);
        if (! pfh->fh_bListArchive)
        {
            u32Ret = _writeFile(pah, pfh, u64Size);
            if (u32Ret == JF_ERR_NO_ERROR)
            {
                sLen = (u32)(u64AlignedSize - u64Size);
                if (sLen != 0)
                {
                    u32Ret = _readDataForArchive(pfh->fh_pafArchive, pfh->fh_pu8Buffer, sLen);
                }
            }
        }
        else
        {
            if (u64AlignedSize != 0)
            {
                u32Ret = seekArFile(pfh->fh_pafArchive, u64AlignedSize, SEEK_CUR);
                if (u32Ret == JF_ERR_END_OF_FILE)
                    u32Ret = JF_ERR_ARCHIVE_CORRUPTED;
            }
        }
    }

    return u32Ret;
}

static u32 _extractDir(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    mode_t mode = 0;

    if (! pfh->fh_bListArchive)
    {
        _getFileMode(pah, &mode);
        u32Ret = jf_dir_create(pfh->fh_strFullpath, mode);
    }

    return u32Ret;
}

static u32 _extractDeviceFile(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    if (! pfh->fh_bListArchive)
    {

    }

    return u32Ret;
}

static u32 _extractHardLinkFile(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    if (! pfh->fh_bListArchive)
    {

    }

    return u32Ret;
}

static u32 _extractSymLinkFile(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    if (! pfh->fh_bListArchive)
    {

    }

    return u32Ret;
}

static u32 _extractFifoFile(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    if (! pfh->fh_bListArchive)
    {

    }

    return u32Ret;
}

static u32 _extractMemberFile(archive_header_t * pah, file_handler_t * pfh)
{
    u32 u32Ret = JF_ERR_NO_ERROR;

    if (pfh->fh_bVerbose)
        ol_printf("extract %s\n", pfh->fh_strFullpath);
    jf_logger_logInfoMsg("extract %s", pfh->fh_strFullpath);

    switch (pah->ah_u8Type)
    {
    case AH_TYPE_REGULAR:
        u32Ret = _extractRegularFile(pah, pfh);
        break;
    case AH_TYPE_HARD_LINK:
        u32Ret = _extractHardLinkFile(pah, pfh);
        break;
    case AH_TYPE_SYM_LINK:
        u32Ret = _extractSymLinkFile(pah, pfh);
        break;
    case AH_TYPE_CHAR:
    case AH_TYPE_BLOCK:
        u32Ret = _extractDeviceFile(pah, pfh);
        break;
    case AH_TYPE_DIR:
        u32Ret = _extractDir(pah, pfh);
        break;
    case AH_TYPE_FIFO:
        u32Ret = _extractFifoFile(pah, pfh);
        break;
    default:
        u32Ret = JF_ERR_INVALID_FILE_TYPE;
        break;
    }

    return u32Ret;
}

/* --- public routine section ------------------------------------------------------------------- */
u32 extractFromArchive(
    ar_file_t * pafArchive, u8 * pu8Buffer, olsize_t sBuf, jf_archive_extract_param_t * pParam)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    file_handler_t fh;
    archive_block_t ab;
    archive_header_t * pah = &(ab.ab_ahHeader);

    ol_bzero(&fh, sizeof(file_handler_t));
    fh.fh_pafArchive = pafArchive;
    fh.fh_pu8Buffer = pu8Buffer;
    fh.fh_sBuf = sBuf;
    fh.fh_bListArchive = pParam->jaep_bListArchive;

    while ((u32Ret == JF_ERR_NO_ERROR) && (! isEndOfArFile(pafArchive)))
    {
        ol_bzero(pah, ARCHIVE_BLOCK_SIZE);
        u32Ret = _readDataForArchive(pafArchive, (u8 *)pah, ARCHIVE_BLOCK_SIZE);
        if (u32Ret == JF_ERR_NO_ERROR)
        {
            u32Ret = _validateHeader(pah, &fh);
            if (u32Ret == JF_ERR_NO_ERROR)
            {
                u32Ret = _getMemberFilename(pah, &fh);
            }

            if (u32Ret == JF_ERR_NO_ERROR)
            {
                u32Ret = _extractMemberFile(pah, &fh);
            }
        }
        else if (u32Ret == JF_ERR_END_OF_FILE)
        {
            u32Ret = JF_ERR_NO_ERROR;
        }
    }

    return u32Ret;
}

/*------------------------------------------------------------------------------------------------*/


