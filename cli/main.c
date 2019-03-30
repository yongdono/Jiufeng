/**
 *  @file cli/main.c
 *
 *  @brief The main file of cli
 *
 *  @author Min Zhang
 *  
 *  @note
 *
 */

/* --- standard C lib header files ----------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#if defined(WINDOWS)

#elif defined(LINUX)
    #include <fcntl.h>
    #include <termios.h>
    #include <sys/ioctl.h>
#endif

/* --- internal header files ----------------------------------------------- */
#include "olbasic.h"
#include "ollimit.h"
#include "errcode.h"
#include "clieng.h"
#include "clicmd.h"
#include "main.h"
#include "xmalloc.h"

/* --- private data/data structure section --------------------------------- */
jiufeng_cli_master_t * ls_pocmMaster = NULL;

static const olchar_t * ls_pstrProgramName = "olcli";
static const olchar_t * ls_pstrVersion = "1.0.0";
static const olchar_t * ls_pstrBuildData = "2/30/2018";

/* --- private routine section---------------------------------------------- */
static void _printUsage(void)
{
    ol_printf("\
Usage: %s [-h] [logger options] \n\
    -h print the usage.\n\
logger options:\n\
    -T <0|1|2|3|4> the log level. 0: no log, 1: error, 2: info, 3: debug,\n\
       4: data.\n\
    -F <log file> the log file.\n\
    -S <log file size> the size of log file. No limit if not specified.\n",
           ls_pstrProgramName);

    ol_printf("\n");

    exit(0);
}

static u32 _parseCmdLineParam(olint_t argc, olchar_t ** argv, 
    jf_clieng_init_param_t * pjcip, logger_param_t * plp)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olint_t nOpt;
    u32 u32Value;

    while (((nOpt = getopt(argc, argv, "T:F:S:Oh")) != -1) &&
           (u32Ret == OLERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case '?':
        case 'h':
            _printUsage();
            break;
        case 'T':
            if (sscanf(optarg, "%d", &u32Value) == 1)
                plp->lp_u8TraceLevel = (u8)u32Value;
            else
                u32Ret = OLERR_INVALID_PARAM;
            break;
        case 'F':
            plp->lp_bLogToFile = TRUE;
            plp->lp_pstrLogFilePath = optarg;
            break;
        case 'O':
            plp->lp_bLogToStdout = TRUE;
            break;
        case 'S':
            if (sscanf(optarg, "%d", &u32Value) == 1)
                plp->lp_sLogFile = u32Value;
            else
                u32Ret = OLERR_INVALID_PARAM;
            break;
        default:
            u32Ret = OLERR_INVALID_OPTION;
            break;
        }
    }

    return u32Ret;
}

static u32 _printShellGreeting(void * pMaster)
{
    u32 u32Ret = OLERR_NO_ERROR;

    jf_clieng_outputLine(
        "-------------------------------------------------------------");
    jf_clieng_outputLine(
        "Jiufeng Command Line Interface (CLI) Utility");
    jf_clieng_outputLine(
        "Version: %s Build Date: %s", ls_pstrVersion, ls_pstrBuildData);
    jf_clieng_outputLine(
        "-------------------------------------------------------------");

    return u32Ret;
}

/* --- public routine section ---------------------------------------------- */

olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    jf_clieng_init_param_t jcip;
    logger_param_t lpParam;
    cli_param_t cliParam;
//    aether_param_t ap;

    u32Ret = xcalloc((void **)&ls_pocmMaster, sizeof(jiufeng_cli_master_t));
    if (u32Ret == OLERR_NO_ERROR)
    {
        memset(&lpParam, 0, sizeof(logger_param_t));
        lpParam.lp_pstrCallerName = "CLI";

        memset(&jcip, 0, sizeof(jcip));

        jcip.jcip_sMaxCmdLine = JF_CLIENG_MAX_COMMAND_LINE_SIZE;
        jcip.jcip_sCmdHistroyBuf = 20;
        ol_strcpy(jcip.jcip_strCliName, "Jiufeng CLI");
        jcip.jcip_pstrNewLine = "\n";
        jcip.jcip_pMaster = ls_pocmMaster;
        jcip.jcip_fnPrintGreeting = _printShellGreeting;

        u32Ret = _parseCmdLineParam(argc, argv, &jcip, &lpParam);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = jf_clieng_init(&jcip);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = addCmd(ls_pocmMaster, &cliParam);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = jf_clieng_run();
    }

    jf_clieng_fini();

    finiLogger();

    if (ls_pocmMaster != NULL)
        xfree((void **)&ls_pocmMaster);

    return u32Ret;
}

/*---------------------------------------------------------------------------*/

