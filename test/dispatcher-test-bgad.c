/**
 *  @file dispatcher-test-bgad.c
 *
 *  @brief Test file for dispatcher service.
 *
 *  @author Min Zhang
 *
 *  @note
 *
 */

/* --- standard C lib header files -------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_limit.h"
#include "jf_err.h"
#include "jf_messaging.h"
#include "jf_jiukun.h"
#include "jf_option.h"
#include "jf_time.h"
#include "jf_process.h"

#include "dispatcher-test.h"

/* --- private data/data structure section ------------------------------------------------------ */

static boolean_t ls_bTestMessaging = FALSE;

static boolean_t ls_bToTerminateDtb = FALSE;

/* --- private routine section ------------------------------------------------------------------ */

static void _printDispatcherTestBgadUsage(void)
{
    ol_printf("\
Usage: dispatcher-test-bgad.c [-m] [-h] [logger options] \n\
    -m test messaging library.\n\
    -h print the usage.\n\
logger options:\n\
    -T <0|1|2|3|4> the log level. 0: no log, 1: error, 2: info, 3: debug, 4: data.\n\
    -F <log file> the log file.\n\
    -S <log file size> the size of log file. No limit if not specified.\n\
    ");

    ol_printf("\n");
}

static u32 _parseDispatcherTestBgadCmdLineParam(
    olint_t argc, olchar_t ** argv, jf_logger_init_param_t * pjlip)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olint_t nOpt;

    while (((nOpt = getopt(argc, argv, "mT:F:S:h")) != -1) &&
           (u32Ret == JF_ERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case '?':
        case 'h':
            _printDispatcherTestBgadUsage();
            exit(0);
            break;
        case 'm':
            ls_bTestMessaging = TRUE;
            break;
        case 'T':
            u32Ret = jf_option_getU8FromString(optarg, &pjlip->jlip_u8TraceLevel);
            break;
        case 'F':
            pjlip->jlip_bLogToFile = TRUE;
            pjlip->jlip_pstrLogFilePath = optarg;
            break;
        case 'S':
            u32Ret = jf_option_getS32FromString(optarg, &pjlip->jlip_sLogFile);
            break;
        default:
            u32Ret = JF_ERR_INVALID_OPTION;
            break;
        }
    }

    return u32Ret;
}

static u32 _bgadProcessMsg(u8 * pu8Msg, olsize_t sMsg)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
//    jf_messaging_header_t * pHeader = (jf_messaging_header_t *)pu8Msg;
    u32 u32MsgId = jf_messaging_getMsgId(pu8Msg, sMsg);
    bgad_activity_status_msg basm;
    sysctld_system_info_msg * pssim;

    jf_messaging_initMsgHeader(
        (u8 *)&basm, BGAD_MSG_ID_ACTIVITY_STATUS, JF_MESSAGING_PRIO_MID,
        sizeof(basm.basm_basmpPayload));

    switch (u32MsgId)
    {
    case BGAD_MSG_ID_START_ACTIVITY:
        ol_printf("receive message, BGAD_MSG_ID_START_ACTIVITY\n");

        u32Ret = jf_messaging_sendMsg((u8 *)&basm, sizeof(basm));

        break;
    case BGAD_MSG_ID_STOP_ACTIVITY:
        ol_printf("receive message, BGAD_MSG_ID_STOP_ACTIVITY\n");

        u32Ret = jf_messaging_sendMsg((u8 *)&basm, sizeof(basm));

        break;
    case SYSCTLD_MSG_ID_SYSTEM_INFO:
        ol_printf("receive message, SYSCTLD_MSG_ID_SYSTEM_INFO\n");
        pssim = (sysctld_system_info_msg *)pu8Msg;
        ol_printf("system info: %s\n", (olchar_t *)pssim->ssim_ssimpPayload.ssimp_u8Payload);

        break;
    default:
        ol_printf("unsupported subscribed message\n");
        break;
    }

    return u32Ret;
}

static u32 _testMessagingBgad(void)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    jf_messaging_init_param_t jmip;

    ol_bzero(&jmip, sizeof(jmip));
    jmip.jmip_fnProcessMsg = _bgadProcessMsg;
    jmip.jmip_pstrMessagingIn = "bgad_message_in";
    jmip.jmip_pstrMessagingOut = "bgad_message_out";
    jmip.jmip_pstrName = "bgad";
    jmip.jmip_sMaxMsg = 1024;
    jmip.jmip_u32MaxNumMsg = 8;

    u32Ret = jf_messaging_init(&jmip);
    if (u32Ret == JF_ERR_NO_ERROR)
        u32Ret = jf_messaging_start();

    if (u32Ret == JF_ERR_NO_ERROR)
    {
        bgad_activity_info_msg baim;

        jf_messaging_initMsgHeader(
            (u8 *)&baim, BGAD_MSG_ID_ACTIVITY_INFO, JF_MESSAGING_PRIO_MID,
            sizeof(baim.baim_baimpPayload));

        u32Ret = jf_messaging_sendMsg((u8 *)&baim, sizeof(baim));
    }

    if (u32Ret == JF_ERR_NO_ERROR)
    {
        while (! ls_bToTerminateDtb)
        {
            jf_time_sleep(2);
        }
        jf_time_sleep(3);
    }

    jf_messaging_fini();

    return u32Ret;
}

static void _terminateDispatcherTestBgad(olint_t signal)
{
    ol_printf("get signal\n");

    jf_messaging_stop();

    ls_bToTerminateDtb = TRUE;
}


/* --- public routine section ------------------------------------------------------------------- */

olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olchar_t strErrMsg[300];
    jf_logger_init_param_t jlipParam;
    jf_jiukun_init_param_t jjip;

    ol_bzero(&jlipParam, sizeof(jlipParam));
    jlipParam.jlip_pstrCallerName = "DSPT-TEST-BGAD";
//    jlipParam.jlip_bLogToStdout = TRUE;
    jlipParam.jlip_bLogToFile = TRUE;
    jlipParam.jlip_u8TraceLevel = JF_LOGGER_TRACE_LEVEL_DEBUG;

    ol_bzero(&jjip, sizeof(jjip));
    jjip.jjip_sPool = JF_JIUKUN_MAX_POOL_SIZE;

    u32Ret = _parseDispatcherTestBgadCmdLineParam(argc, argv, &jlipParam);
    if (u32Ret == JF_ERR_NO_ERROR)
        u32Ret = jf_process_registerSignalHandlers(_terminateDispatcherTestBgad);

    if (u32Ret == JF_ERR_NO_ERROR)
    {
        jf_logger_init(&jlipParam);

        u32Ret = jf_jiukun_init(&jjip);
        if (u32Ret == JF_ERR_NO_ERROR)
        {
            if (ls_bTestMessaging)
            {
                u32Ret = _testMessagingBgad();
            }
            else 
            {
                ol_printf("No operation is specified !!!!\n\n");
                _printDispatcherTestBgadUsage();
            }

            jf_jiukun_fini();
        }

        jf_logger_fini();
    }

    if (u32Ret != JF_ERR_NO_ERROR)
    {
        jf_err_getMsg(u32Ret, strErrMsg, 300);
        ol_printf("%s\n", strErrMsg);
    }

    return u32Ret;
}

/*------------------------------------------------------------------------------------------------*/

