/**
 *  @file conffile-test.c
 *
 *  @brief the test file for conffile common object
 *
 *  @author Min Zhang
 *
 *  @note
 *
 */

/* --- standard C lib header files -------------------------------------------------------------- */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_limit.h"
#include "jf_err.h"
#include "jf_conffile.h"
#include "jf_jiukun.h"

/* --- private data/data structure section ------------------------------------------------------ */


/* --- private routine section ------------------------------------------------------------------ */
static void _printConffileTestUsage(void)
{
    ol_printf("\
Usage: conffile-test [-h] conffile\n\
         ");

    ol_printf("\n");
}

static u32 _parseConffileTestCmdLineParam(
    olint_t argc, olchar_t ** argv, jf_logger_init_param_t * pjlip)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olint_t nOpt;

    while (((nOpt = getopt(argc, argv,
        "h?")) != -1) && (u32Ret == JF_ERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case '?':
        case 'h':
            _printConffileTestUsage();
            exit(0);
            break;
        case ':':
            u32Ret = JF_ERR_MISSING_PARAM;
            break;
        default:
            u32Ret = JF_ERR_INVALID_OPTION;
            break;
        }
    }

    return u32Ret;
}

typedef struct
{
    olchar_t * cft_pstrTag;
    olchar_t * cft_pstrDefault;
    olint_t cft_nDefault;
} conf_file_tag_t;

static conf_file_tag_t ls_cftConfFileTag[] =
{
    {"a", "default-a", 0},
    {"b", "default-b", 0},
    {"c", "default-c", 0},
    {"d", "default-d", 0},
    {"e", "default-e", 0},
    {"aa", "default-aa", 0},
    {"bb", NULL, 2342},
    {"bc", NULL, 2312312},
    {"abcd", "default-abcd", 0},
};

static u32 ls_u32NumOfConfFileTag = sizeof(ls_cftConfFileTag) / sizeof(conf_file_tag_t);

static u32 _testConfFile(const olchar_t * pstrFilename)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    jf_conffile_t * pjc = NULL;
    u32 u32Index;
    olchar_t strValue[JF_CONFFILE_MAX_LINE_LEN];
    olint_t nValue;
    jf_conffile_open_param_t jcop;

    ol_printf("conffile: %s\n", pstrFilename);

    ol_memset(&jcop, 0, sizeof(jcop));
    jcop.jcop_pstrFile = (olchar_t *)pstrFilename;

    u32Ret = jf_conffile_open(&jcop, &pjc);
    if (u32Ret == JF_ERR_NO_ERROR)
    {
        for (u32Index = 0; u32Index < ls_u32NumOfConfFileTag; u32Index ++)
        {
            if (ls_cftConfFileTag[u32Index].cft_pstrDefault == NULL)
            {
                u32Ret = jf_conffile_getInt(
                    pjc, ls_cftConfFileTag[u32Index].cft_pstrTag,
                    ls_cftConfFileTag[u32Index].cft_nDefault, &nValue);
                if (u32Ret == JF_ERR_NO_ERROR)
                    ol_printf(
                        "%s=%d(%d)\n", ls_cftConfFileTag[u32Index].cft_pstrTag,
                        nValue, ls_cftConfFileTag[u32Index].cft_nDefault);
                else
                    ol_printf("%s error\n", ls_cftConfFileTag[u32Index].cft_pstrTag);
            }
            else
            {
                u32Ret = jf_conffile_getString(
                    pjc, ls_cftConfFileTag[u32Index].cft_pstrTag,
                    ls_cftConfFileTag[u32Index].cft_pstrDefault, strValue, sizeof(strValue));
                if (u32Ret == JF_ERR_NO_ERROR)
                    ol_printf(
                        "%s=%s(%s)\n", ls_cftConfFileTag[u32Index].cft_pstrTag,
                        strValue, ls_cftConfFileTag[u32Index].cft_pstrDefault);
                else
                    ol_printf("%s error\n", ls_cftConfFileTag[u32Index].cft_pstrTag);
            }
        }

        jf_conffile_close(&pjc);
    }

    return u32Ret;
}

/* --- public routine section ------------------------------------------------------------------- */
olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olchar_t strErrMsg[300];
    jf_logger_init_param_t jlipParam;
    jf_jiukun_init_param_t jjip;

    ol_bzero(&jlipParam, sizeof(jlipParam));
    jlipParam.jlip_pstrCallerName = "ARCHIVE";
    jlipParam.jlip_bLogToStdout = TRUE;
    jlipParam.jlip_u8TraceLevel = JF_LOGGER_TRACE_LEVEL_DEBUG;

    ol_bzero(&jjip, sizeof(jjip));
    jjip.jjip_sPool = JF_JIUKUN_MAX_POOL_SIZE;

    if (argc < 2)
    {
        ol_printf("Missing parameters!!!\n\n");
        _printConffileTestUsage();
        exit(0);
    }

    u32Ret = _parseConffileTestCmdLineParam(argc, argv, &jlipParam);
    if (u32Ret == JF_ERR_NO_ERROR)
    {
        jf_logger_init(&jlipParam);

        u32Ret = jf_jiukun_init(&jjip);
        if (u32Ret == JF_ERR_NO_ERROR)
        {
            u32Ret = _testConfFile(argv[argc - 1]);

            jf_jiukun_fini();
        }

        jf_logger_logErrMsg(u32Ret, "Quit");
        jf_logger_fini();
    }

    if (u32Ret != JF_ERR_NO_ERROR)
    {
        jf_err_getMsg(u32Ret, strErrMsg, 300);
        ol_printf("%s\n", strErrMsg);
    }

    exit(0);
}

/*------------------------------------------------------------------------------------------------*/

