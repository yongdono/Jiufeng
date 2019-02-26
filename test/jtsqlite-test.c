/**
 *  @file jtsqlite.c
 *
 *  @brief The test file for jt sqlite common object
 *
 *  @author Min Zhang
 *
 *  @note Create DB with command: sqlite3 env.db
 *  @note Create table with sql statement: CREATE TABLE env(key TEXT PRIMARY
 *   KEY, value TEXT);
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
#include "jtsqlite.h"
#include "xmalloc.h"

/* --- private data/data structure section --------------------------------- */


/* --- private routine section---------------------------------------------- */

static void _printUsage(void)
{
    ol_printf("\
Usage: jtsqlite-test [-h] \n\
    -h show this usage. \n\
    ");

    ol_printf("\n");
}

static u32 _parseCmdLineParam(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olint_t nOpt;

    while (((nOpt = getopt(argc, argv,
        "?h")) != -1) && (u32Ret == OLERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case '?':
        case 'h':
            _printUsage();
            exit(0);
        default:
            u32Ret = OLERR_INVALID_OPTION;
            break;
        }
    }

    return u32Ret;
}

static u32 _setJtSqliteValue(
    jt_sqlite_t * pjs, olchar_t * pKey, olchar_t * pValue)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t * pstrSql = NULL;
    olchar_t strRet[128];
    olsize_t nsize = ol_strlen(pValue) + ol_strlen(pKey) + 256;

    u32Ret = xmalloc((void **)&pstrSql, nsize);
    if (u32Ret == OLERR_NO_ERROR)
    {
        /*update or insert the value into the DB*/
        ol_snprintf(
            pstrSql, nsize,
            "REPLACE INTO env(key, value) VALUES ('%s', '%s');",
            pKey, pValue);
        u32Ret = execJtSqliteSql(pjs, pstrSql, strRet, sizeof(strRet));
    }
    
    if (pstrSql != NULL)
        xfree((void **)&pstrSql);

    return u32Ret;
}

static u32 _getJtSqliteValue(
    jt_sqlite_t * pjs, olchar_t * pKey, olchar_t * pValue, olsize_t sValue)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t strSql[512];

    ol_snprintf(
        strSql, sizeof(strSql), "SELECT value FROM env WHERE key='%s';", pKey);
    u32Ret = execJtSqliteSql(pjs, strSql, pValue, sValue);

    return u32Ret;
}

static u32 _testRwJtSqlite(jt_sqlite_t * pjs)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t * key = "today";
    olchar_t * no_such_key = "no-such-key";
    olchar_t * monday = "Monday";
    olchar_t * tuesday = "tuesday";
    olchar_t value[512];

    ol_printf("Testing jt sqlite common object \n");

    ol_printf("set, %s = %s\n", key, monday);
    u32Ret = _setJtSqliteValue(pjs, key, monday);
    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = _getJtSqliteValue(pjs, key, value, 512);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("get, %s = %s\n", key, value);

        ol_printf("set, %s = %s\n", key, tuesday);
        u32Ret = _setJtSqliteValue(pjs, key, tuesday);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = _getJtSqliteValue(pjs, key, value, 512);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("get, %s = %s\n", key, value);

        u32Ret = _getJtSqliteValue(pjs, no_such_key, value, 512);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("get, %s = %s\n", no_such_key, value);
    }

    return u32Ret;
}

static u32 _testJtSqliteTransaction(jt_sqlite_t * pjs)
{
    u32 u32Ret = OLERR_NO_ERROR;

    ol_printf("Start jt sqlite transaction\n");
    u32Ret = startJtSqliteTransaction(pjs);
    if (u32Ret == OLERR_NO_ERROR)
    {
        _setJtSqliteValue(pjs, "color", "red");
        _setJtSqliteValue(pjs, "name", "min");
        _setJtSqliteValue(pjs, "age", "32");

        ol_printf("Commit jt sqlite transaction\n");
        commitJtSqliteTransaction(pjs);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("Start jt sqlite transaction\n");
        u32Ret = startJtSqliteTransaction(pjs);
        if (u32Ret == OLERR_NO_ERROR)
        {
            _setJtSqliteValue(pjs, "book1", "1");
            _setJtSqliteValue(pjs, "book2", "2");
            _setJtSqliteValue(pjs, "book3", "3");

            ol_printf("Rollback jt sqlite transaction\n");
            rollbackJtSqliteTransaction(pjs);
        }
    }

    return u32Ret;
}

static u32 _testJtSqlite(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    jt_sqlite_t js;
    jt_sqlite_param_t config;

    ol_memset(&config, 0, sizeof(jt_sqlite_param_t));
    config.jsp_pstrDbName = "env.db";

    u32Ret = initJtSqlite(&js, &config);
    if (u32Ret == OLERR_NO_ERROR)
    {
        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = _testRwJtSqlite(&js);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = _testJtSqliteTransaction(&js);

        finiJtSqlite(&js);
    }

    return u32Ret;
}

/* --- public routine section ---------------------------------------------- */

olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    logger_param_t lpParam;

    if (argc < 1)
    {
        _printUsage();
        u32Ret = OLERR_MISSING_PARAM;
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = _parseCmdLineParam(argc, argv);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        memset(&lpParam, 0, sizeof(logger_param_t));
        lpParam.lp_pstrCallerName = "JTSQLITE";
        lpParam.lp_bLogToFile = FALSE;
        lpParam.lp_bLogToStdout = TRUE;
        lpParam.lp_u8TraceLevel = LOGGER_TRACE_DATA;

        initLogger(&lpParam);
    }

    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = _testJtSqlite();
    }

    finiLogger();

    if (u32Ret != OLERR_NO_ERROR)
    {
        ol_printf("Err (0x%X) %s\n", u32Ret, getErrorDescription(u32Ret));
    }

    return u32Ret;
}

/*---------------------------------------------------------------------------*/


