/**
 *  @file main.c
 *
 *  @brief The main file of dongyuan service
 *
 *  @author Min Zhang
 *
 *  @note
 *  
 */

/* --- standard C lib header files ----------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* --- internal header files ----------------------------------------------- */
#include "olbasic.h"
#include "ollimit.h"
#include "errcode.h"
#include "dongyuan.h"
#include "process.h"
#include "files.h"

/* --- private data/data structure section --------------------------------- */
static dongyuan_t * ls_pgDongyuan = NULL;
static boolean_t ls_bForeground = FALSE;

static const olchar_t * ls_pstrProgramName = "oldongyuan";
static const olchar_t * ls_pstrVersion = "1.0.0";

#if defined(LINUX)
    #define SERVICE_RETURN_VALUE  int
    #define SERVICE_RETURN(value) return value
#elif defined(WINDOWS)
    #define SERVICE_RETURN_VALUE  void
    #define SERVICE_RETURN(value) return
#endif

/* --- private routine section---------------------------------------------- */
static void _printDongyuanUsage(void)
{
    ol_printf("\
Usage: %s [-f] [-s setting file] [-V] [logger options]\n\
    -f ruuning in foreground.\n\
    -s setting file: specify the setting file.\n\
    -V show version information.\n\
logger options:\n\
    -T <0|1|2|3> the log level. 0: no log, 1: error only, 2: info, 3: all.\n\
    -F <log file> the log file.\n\
    -S <log file size> the size of log file. No limit if not specified.\n",
           ls_pstrProgramName);

    ol_printf("\n");

    exit(0);
}

static u32 _parseDongyuanCmdLineParam(
    olint_t argc, olchar_t ** argv, 
    dongyuan_param_t * pgp, logger_param_t * plp)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olint_t nOpt;
    u32 u32Value;

    while (((nOpt = getopt(argc, argv, "fs:VT:F:S:Oh")) != -1) &&
           (u32Ret == OLERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case 'f':
            ls_bForeground = TRUE;
            break;
        case 's':
            pgp->gp_pstrSettingFile = optarg;
            break;
        case '?':
        case 'h':
            _printDongyuanUsage();
            break;
        case 'V':
            ol_printf("%s %s\n", ls_pstrProgramName, ls_pstrVersion);
            exit(0);
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

static void _terminate(olint_t signal)
{
    ol_printf("get signal\n");

    if (ls_pgDongyuan != NULL)
        stopDongyuan(ls_pgDongyuan);
}

static u32 _startDongyuan(void)
{
    u32 u32Ret = OLERR_NO_ERROR;

    u32Ret = startDongyuan(ls_pgDongyuan);
    if (u32Ret != OLERR_NO_ERROR)
        logErrMsg(u32Ret, "quit dongyuan");

    if (ls_pgDongyuan != NULL)
        destroyDongyuan(&ls_pgDongyuan);

    finiLogger();

    return u32Ret;
}

static u32 _initDongyuan(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    dongyuan_param_t gp;
    logger_param_t lpParam;
    olchar_t strExecutable[100];

    memset(&lpParam, 0, sizeof(logger_param_t));
    lpParam.lp_pstrCallerName = "DONGYUAN";
    lpParam.lp_u8TraceLevel = 0;

    setDefaultDongyuanParam(&gp);
    gp.gp_pstrCmdLine = argv[0];

    u32Ret = _parseDongyuanCmdLineParam(argc, argv, &gp, &lpParam);
    if (u32Ret == OLERR_NO_ERROR)
    {
        initLogger(&lpParam);

        getFileName(strExecutable, 100, argv[0]);

        if (bAlreadyRunning(strExecutable))
        {
            fprintf(stderr, "another %s is ruuning\n", strExecutable);
            exit(-1);
        }

        if (! ls_bForeground)
            u32Ret = switchToDaemon(strExecutable);
    }

    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = registerSignalHandlers(_terminate);

    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = createDongyuan(&ls_pgDongyuan, &gp);

    return u32Ret;
}

SERVICE_RETURN_VALUE _serviceMain(olint_t argc, char** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;

#if defined(LINUX)
    u32Ret = _initDongyuan(argc, argv);
    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = _startDongyuan();
#endif

    SERVICE_RETURN(u32Ret);
}

/* --- public routine section ---------------------------------------------- */
olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;

#if defined(LINUX)
    u32Ret = _serviceMain(argc, argv);
#elif defined(WINDOWS)
    _serviceMain(argc, argv);
#endif

    return u32Ret;
}


/*---------------------------------------------------------------------------*/
