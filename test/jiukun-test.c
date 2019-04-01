/**
 *  @file jiukun-test.c
 *
 *  @brief The test file for jiukun library
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
#include "bases.h"
#include "errcode.h"
#include "jiukun.h"
#include "process.h"

/* --- private data/data structure section --------------------------------- */

#define MAX_THREAD_COUNT  2

static boolean_t ls_bToTerminate = FALSE;

boolean_t ls_bMultiThread = FALSE;
boolean_t ls_bStress = FALSE;

/* --- private routine section---------------------------------------------- */

static void _printUsage(void)
{
    ol_printf("\
Usage: jiukun-test [-t] [logger options]\n\
    -t test in multi-threading environment.\n\
    -s stress testing.\n\
logger options:\n\
    -T <0|1|2|3> the log level. 0: no log, 1: error only, 2: info, 3: all.\n\
    -F <log file> the log file.\n\
    -S <trace file size> the size of log file. No limit if not specified.\n");

    ol_printf("\n");
}

static u32 _parseCmdLineParam(olint_t argc, olchar_t ** argv, logger_param_t * plp)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olint_t nOpt;
    u32 u32Value;

    while (((nOpt = getopt(argc, argv,
        "tsOT:F:S:h")) != -1) && (u32Ret == OLERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case '?':
        case 'h':
            _printUsage();
            exit(u32Ret);
            break;
        case 't':
            ls_bMultiThread = TRUE;
            break;
        case 's':
            ls_bStress = TRUE;
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
        case 'S':
            if (sscanf(optarg, "%d", &u32Value) == 1)
                plp->lp_sLogFile = u32Value;
            else
                u32Ret = OLERR_INVALID_PARAM;
            break;
        case 'O':
            plp->lp_bLogToStdout = TRUE;
            break;
        default:
            u32Ret = OLERR_INVALID_OPTION;
            break;
        }
    }

    return u32Ret;
}

#define DEBUG_LOOP_COUNT  100
u32 _testAllocMem(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    u8 * pu8Mem[DEBUG_LOOP_COUNT];
    u32 u32Index = 0, u32Idx, u32Loop, u32Size;
    olchar_t strErrMsg[300];
    olint_t nRand;
    boolean_t bAlloc;

    memset(pu8Mem, 0, sizeof(u8 *) * DEBUG_LOOP_COUNT);
    srand(time(NULL));

    nRand = rand();

    for (u32Loop = 0; u32Loop < DEBUG_LOOP_COUNT; u32Loop ++)
    {
        nRand = rand();

        if ((nRand % 3) != 0)
        {
            bAlloc = TRUE;
            logInfoMsg("Allocate memory");
        }
        else
        {
            bAlloc = FALSE;
            logInfoMsg("free memory");
        }

        nRand = rand();
        u32Index = nRand % DEBUG_LOOP_COUNT;

        if (bAlloc)
        {
            if (pu8Mem[u32Index] == NULL)
            {
                nRand = rand();
                u32Size = nRand % JF_JIUKUN_MAX_MEMORY_SIZE;

                logInfoMsg("!!!! Allocate %u", u32Size);

                u32Ret = jf_jiukun_allocMemory((void **)&(pu8Mem[u32Index]), u32Size, 0);
                if (u32Ret == OLERR_NO_ERROR)
                {
                    logInfoMsg("success, at %u, %p\n", u32Index,
                        pu8Mem[u32Index]);
#if defined(DEBUG_JIUKUN)
                    dumpJiukun();
#endif
                }
                else
                {
                    getErrMsg(u32Ret, strErrMsg, sizeof(strErrMsg));
                    logInfoMsg("failed, %s\n", strErrMsg);
                }
            }
        }
        else
        {
            for (u32Idx = u32Index; u32Idx < DEBUG_LOOP_COUNT; u32Idx ++)
                if (pu8Mem[u32Idx] != NULL)
                {
                    logInfoMsg("!!!! free at %u, %p", u32Idx, pu8Mem[u32Idx]);

                    jf_jiukun_freeMemory((void **)&(pu8Mem[u32Idx]));

                    pu8Mem[u32Idx] = NULL;
#if defined(DEBUG_JIUKUN)
                    jf_jiukun_dump();
#endif
                    break;
                }
        }
    }

    for (u32Loop = 0; u32Loop < DEBUG_LOOP_COUNT; u32Loop ++)
        if (pu8Mem[u32Loop] != NULL)
            jf_jiukun_freeMemory((void **)&(pu8Mem[u32Loop]));

    return u32Ret;
}

#define MAX_ORDER  8

u32 _testJiukunPage(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    u8 * pu8Mem[DEBUG_LOOP_COUNT];
    u32 u32Order[DEBUG_LOOP_COUNT];
    u32 u32Index = 0, u32Idx, u32Loop;
    olchar_t strErrMsg[300];
    olint_t nRand;
    boolean_t bAlloc;

    ol_memset(pu8Mem, 0, sizeof(u8 *) * DEBUG_LOOP_COUNT);
    srand(time(NULL));

    nRand = rand();
#if defined(DEBUG_JIUKUN)
    dumpJiukun();
#endif
    for (u32Loop = 0; u32Loop < DEBUG_LOOP_COUNT; u32Loop ++)
    {
        nRand = rand();

        if ((nRand % 3) != 0)
        {
            bAlloc = TRUE;
            logInfoMsg("Allocate page");
        }
        else
        {
            bAlloc = FALSE;
            logInfoMsg("free page");
        }

        nRand = rand();
        u32Index = nRand % DEBUG_LOOP_COUNT;

        if (bAlloc)
        {
            if (pu8Mem[u32Index] == NULL)
            {
                nRand = rand();
                u32Order[u32Index] = nRand % MAX_ORDER;

                u32Ret = jf_jiukun_allocPage(
                    (void **)&(pu8Mem[u32Index]), u32Order[u32Index], 0);
                if (u32Ret == OLERR_NO_ERROR)
                {
#if defined(DEBUG_JIUKUN)
                    jf_jiukun_dump();
#endif
                    logInfoMsg("success, at %u, %p\n", u32Index,
                        pu8Mem[u32Index]);
                }
                else
                {
                    getErrMsg(u32Ret, strErrMsg, sizeof(strErrMsg));
                    logInfoMsg("failed, %s\n", strErrMsg);
                }
            }
        }
        else
        {
            for (u32Idx = u32Index; u32Idx < DEBUG_LOOP_COUNT; u32Idx ++)
                if (pu8Mem[u32Idx] != NULL)
                {
                    logInfoMsg("!!!! free at %u, %p", u32Idx, pu8Mem[u32Idx]);

                    jf_jiukun_freePage((void **)&(pu8Mem[u32Idx]));
#if defined(DEBUG_JIUKUN)
                    jf_jiukun_dump();
#endif
                    break;
                }
        }
    }

    for (u32Loop = 0; u32Loop < DEBUG_LOOP_COUNT; u32Loop ++)
        if (pu8Mem[u32Loop] != NULL)
            jf_jiukun_freePage((void **)&(pu8Mem[u32Loop]));

    return u32Ret;
}

#define TEST_CACHE "test-cache"
static jf_jiukun_cache_t * ls_pacCache = NULL;

u32 _testJiukunCache(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    u8 * pu8Mem[DEBUG_LOOP_COUNT];
    u32 u32Index = 0, u32Idx, u32Loop;
    olchar_t strErrMsg[300];
    olint_t nRand;
    boolean_t bAlloc;

    memset(pu8Mem, 0, sizeof(u8 *) * DEBUG_LOOP_COUNT);
    srand(time(NULL));

    nRand = rand();

    for (u32Loop = 0; u32Loop < DEBUG_LOOP_COUNT; u32Loop ++)
    {
        nRand = rand();

        if ((nRand % 3) != 0)
        {
            bAlloc = TRUE;
            logInfoMsg("Allocate object");
        }
        else
        {
            bAlloc = FALSE;
            logInfoMsg("free object");
        }

        nRand = rand();
        u32Index = nRand % DEBUG_LOOP_COUNT;

        if (bAlloc)
        {
            if (pu8Mem[u32Index] == NULL)
            {
                nRand = rand();

                u32Ret = jf_jiukun_allocObject(
                    ls_pacCache, (void **)&(pu8Mem[u32Index]), 0);
                if (u32Ret == OLERR_NO_ERROR)
                {
                    logInfoMsg("success, at %u, %p\n", u32Index,
                        pu8Mem[u32Index]);
                }
                else
                {
                    getErrMsg(u32Ret, strErrMsg, sizeof(strErrMsg));
                    logInfoMsg("failed, %s\n", strErrMsg);
                }
            }
        }
        else
        {
            for (u32Idx = u32Index; u32Idx < DEBUG_LOOP_COUNT; u32Idx ++)
                if (pu8Mem[u32Idx] != NULL)
                {
                    logInfoMsg("!!!! free at %u, %p", u32Idx, pu8Mem[u32Idx]);

                    jf_jiukun_freeObject(ls_pacCache, (void **)&(pu8Mem[u32Idx]));

                    pu8Mem[u32Idx] = NULL;

                    break;
                }
        }
    }

    for (u32Loop = 0; u32Loop < DEBUG_LOOP_COUNT; u32Loop ++)
        if (pu8Mem[u32Loop] != NULL)
            jf_jiukun_freeObject(ls_pacCache, (void **)&(pu8Mem[u32Loop]));

    return u32Ret;
}

static u32 _stressJiukun(void)
{
    u32 u32Ret = OLERR_NO_ERROR;

    u32Ret = _testJiukunPage();

    u32Ret = _testAllocMem();

    u32Ret = _testJiukunCache();

    return u32Ret;
}

THREAD_RETURN_VALUE _allocFree(void * pArg)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t strErrMsg[300];
    u32 u32Index = (u32)(ulong)pArg;

    logInfoMsg("alloc-free thread %u starts", u32Index);

    while ((! ls_bToTerminate) && (u32Ret == OLERR_NO_ERROR))
    {
        logInfoMsg("alloc-free thread %u starts testing", u32Index);

        u32Ret = _testJiukunCache();
    }


    if (u32Ret == OLERR_NO_ERROR)
        logInfoMsg("alloc-free thread %u quits", u32Index);
    else
    {
        getErrMsg(u32Ret, strErrMsg, 300);
        logInfoMsg("alloc-free thread %u quits, %s", u32Index, strErrMsg);
    }

    THREAD_RETURN(u32Ret);
}

static u32 _testJiukunInThread(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    u32 u32Index;
    jf_jiukun_cache_create_param_t jjccp;

    ol_memset(&jjccp, 0, sizeof(jjccp));
    jjccp.jjccp_pstrName = TEST_CACHE;
    jjccp.jjccp_sObj = 28;

    u32Ret = jf_jiukun_createCache(&ls_pacCache, &jjccp);
    if (u32Ret != OLERR_NO_ERROR)
        return u32Ret;

    if (u32Ret == OLERR_NO_ERROR)
    {
        for (u32Index = 0;
             ((u32Index < MAX_THREAD_COUNT) && (u32Ret == OLERR_NO_ERROR));
             u32Index ++)
        {
            u32Ret = createThread(NULL, NULL, _allocFree, (void *)(ulong)(u32Index + 1));
        }
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("main thread, sleeping for 5 minutes\n");
        sleep(300);
        ol_printf("prepare to exit\n");
    }

    ls_bToTerminate = TRUE;
    sleep(1);

    jf_jiukun_destroyCache(&ls_pacCache);

    return u32Ret;
}

static u32 _baseJiukunFunc(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    u8 * page = NULL;
    olflag_t flags;
    olint_t order = 5;
    jf_jiukun_cache_create_param_t jjccp;
    jf_jiukun_cache_t * cache;
    void * object;

    ol_printf("get jiukun page memory with wait: ");
    u32Ret = jf_jiukun_allocPage((void **)&page, order, 0);
    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("success\n");
        jf_jiukun_freePage((void **)&page);
    }
    else
    {
        ol_printf("fail\n");
        return u32Ret;
    }

    ol_printf("get jiukun page with nowait: ");
    INIT_FLAG(flags);
    SET_FLAG(flags, JF_JIUKUN_PAGE_ALLOC_FLAG_NOWAIT);
    u32Ret = jf_jiukun_allocPage((void **)&page, 10, flags);
    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("success\n");
        jf_jiukun_freePage((void **)&page);
    }
    else
    {
        ol_printf("fail\n");
    }

    ol_printf("create jiukun cache: ");
    ol_memset(&jjccp, 0, sizeof(jjccp));
    jjccp.jjccp_pstrName = "jiukun-test";
    jjccp.jjccp_sObj = 16;
    SET_FLAG(jjccp.jjccp_fCache, JF_JIUKUN_CACHE_CREATE_FLAG_ZERO);

    u32Ret = jf_jiukun_createCache(&cache, &jjccp);
    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = jf_jiukun_allocObject(cache, &object, 0);
        if (u32Ret == OLERR_NO_ERROR)
        {
            ol_printf("success\n");
            jf_jiukun_freeObject(cache, &object);
        }
        else
        {
            ol_printf("fail\n");
        }

        jf_jiukun_destroyCache(&cache);
    }

    return u32Ret;
}

/* --- public routine section ---------------------------------------------- */

olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t strErrMsg[300];
    jf_jiukun_init_param_t jjip;
    logger_param_t lpParam;

    memset(&lpParam, 0, sizeof(logger_param_t));

    lpParam.lp_pstrCallerName = "JIUKUN-TEST";
//    lpParam.lp_pstrLogFilePath = "jiukun-test.log";
    lpParam.lp_bLogToStdout = TRUE;
    lpParam.lp_u8TraceLevel = LOGGER_TRACE_DATA;

    u32Ret = _parseCmdLineParam(argc, argv, &lpParam);
    if (u32Ret == OLERR_NO_ERROR)
    {
        initLogger(&lpParam);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        memset(&jjip, 0, sizeof(jjip));
        jjip.jjip_sPool = JF_JIUKUN_MAX_POOL_SIZE;
        jjip.jjip_bNoGrow = TRUE;

        u32Ret = jf_jiukun_init(&jjip);
        if (u32Ret == OLERR_NO_ERROR)
        {
            if (ls_bMultiThread)
                u32Ret = _testJiukunInThread();
            else if (ls_bStress)
                u32Ret = _stressJiukun();
            else
                u32Ret = _baseJiukunFunc();

            jf_jiukun_fini();
        }
    }

    if (u32Ret != OLERR_NO_ERROR)
    {
        getErrMsg(u32Ret, strErrMsg, sizeof(strErrMsg));
        ol_printf("%s\n", strErrMsg);
    }

    return u32Ret;
}

/*---------------------------------------------------------------------------*/


