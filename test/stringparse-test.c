/**
 *  @file stringparse-test.c
 *
 *  @brief The test file for stringparse library
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
#include "stringparse.h"

/* --- private data/data structure section --------------------------------- */

static boolean_t ls_bScanString = FALSE;
static boolean_t ls_bParseString = FALSE;

/* --- private routine section---------------------------------------------- */

static void _printUsage(void)
{
    ol_printf("\
Usage: stringparse-test [-s] [-p] [logger options] \n\
    -s test scan string.\n\
    -p test parse string.\n\
logger options:\n\
    -T <0|1|2|3|4> the log level. 0: no log, 1: error, 2: info, 3: debug,\n\
       4: data.\n\
    -F <log file> the log file.\n\
    -S <log file size> the size of log file. No limit if not specified.\n\
    ");


    ol_printf("\n");
}

static u32 _parseCmdLineParam(
    olint_t argc, olchar_t ** argv, logger_param_t * plp)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olint_t nOpt;

    while (((nOpt = getopt(argc, argv,
        "sph?")) != -1) && (u32Ret == OLERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case 's':
            ls_bScanString = TRUE;
            break;
        case 'p':
            ls_bParseString = TRUE;
            break;
        case '?':
        case 'h':
            _printUsage();
            exit(u32Ret);
            break;
        default:
            u32Ret = OLERR_INVALID_OPTION;
            break;
        }
    }

    return u32Ret;
}

static void _printParseResult(parse_result_t * result)
{
    parse_result_field_t * field;
    u32 u32Index;
    olchar_t str[64];

    ol_printf("Number of result: %d\n", result->pr_u32NumOfResult);

    field = result->pr_pprfFirst;
    for (u32Index = 0; u32Index < result->pr_u32NumOfResult; u32Index ++)
    {
        ol_printf("%u, size: %d, data: ", u32Index, field->prf_sData);
        if (field->prf_sData != 0)
        {
            strncpy(str, field->prf_pstrData, field->prf_sData);
            str[field->prf_sData] = '\0';
            ol_printf("%s", str);
        }
        ol_printf("\n");
        field = field->prf_pprfNext;
    }

}

static u32 _testParseStringWithDelimiter(
    olchar_t * pstr, olchar_t * pdelimiter)
{
    u32 u32Ret = OLERR_NO_ERROR;
    parse_result_t * xml = NULL;

    u32Ret = parseString(
        &xml, pstr, 0, strlen(pstr), pdelimiter, strlen(pdelimiter));
    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("String to parse: %s\n", pstr);
        ol_printf("Delimiter: %s\n", pdelimiter);

        _printParseResult(xml);

        destroyParseResult(&xml);
        ol_printf("\n");
    }
    
    return u32Ret;
}

static u32 _testParseStringAdvWithDelimiter(
    olchar_t * pstr, olchar_t * pdelimiter)
{
    u32 u32Ret = OLERR_NO_ERROR;
    parse_result_t * xml = NULL;

    u32Ret = parseStringAdv(
        &xml, pstr, 0, strlen(pstr), pdelimiter, strlen(pdelimiter));
    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("String to parse: %s\n", pstr);
        ol_printf("Delimiter: %s\n", pdelimiter);

        _printParseResult(xml);

        destroyParseResult(&xml);
        ol_printf("\n");
    }
    
    return u32Ret;
}

static u32 _testParseString(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t * pstrXml = "<name>";
    olchar_t * pstrAdv = "<My name is\">\" adv>";
    olchar_t * pstrAdv2 = "<My name is\'>\' adv>";
    olchar_t * pstrAdv3 = ">My name is\'>\' adv>";

    ol_printf("----------------------------------------------------\n");
    ol_printf("Test string parse\n");

    u32Ret = _testParseStringWithDelimiter(pstrXml, "<");
    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = _testParseStringWithDelimiter(pstrXml, "=");

    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = _testParseStringWithDelimiter(pstrXml, ">");

    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = _testParseStringWithDelimiter(pstrXml, "m");

    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = _testParseStringWithDelimiter(pstrAdv, ">");

    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = _testParseStringWithDelimiter(pstrAdv3, ">");

    if (u32Ret == OLERR_NO_ERROR)
    {
        ol_printf("----------------------------------------------------\n");        
        ol_printf("Test adv string parse\n");

        u32Ret = _testParseStringAdvWithDelimiter(pstrAdv, ">");
    }
    
    if (u32Ret == OLERR_NO_ERROR)
        u32Ret = _testParseStringAdvWithDelimiter(pstrAdv2, ">");

    if (u32Ret == OLERR_NO_ERROR)
         u32Ret = _testParseStringAdvWithDelimiter(pstrAdv3, ">");

    return u32Ret;
}

static u32 _testScanString(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
	char * sdb = "226303136636.85";
	char * sdb2 = "2298363628138.857";
	char * sdb3 = "230189685431.55";
	oldouble_t db;

	u32Ret = getDoubleFromString(sdb, ol_strlen(sdb), &db);
	if (u32Ret == OLERR_NO_ERROR)
	{
		printf("%s, %.2f\n", sdb, db);
	}

	u32Ret = getDoubleFromString(sdb2, ol_strlen(sdb2), &db);
	if (u32Ret == OLERR_NO_ERROR)
	{
		printf("%s, %.2f\n", sdb2, db);
	}

	u32Ret = getDoubleFromString(sdb3, ol_strlen(sdb3), &db);
	if (u32Ret == OLERR_NO_ERROR)
	{
		printf("%s, %.2f\n", sdb3, db);
	}

    return u32Ret;
}

/* --- public routine section ---------------------------------------------- */

olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    logger_param_t lpParam;
	char strErrMsg[300];

    memset(&lpParam, 0, sizeof(logger_param_t));
    lpParam.lp_pstrCallerName = "XMLPARSER";
//    lpParam.lp_bLogToStdout = TRUE;
    lpParam.lp_u8TraceLevel = LOGGER_TRACE_DEBUG;

    u32Ret = _parseCmdLineParam(argc, argv, &lpParam);
    if (u32Ret == OLERR_NO_ERROR)
    {
        initLogger(&lpParam);

        if (ls_bParseString)
            u32Ret = _testParseString();
        else if (ls_bScanString)
            u32Ret = _testScanString();

        finiLogger();
    }

    if (u32Ret != OLERR_NO_ERROR)
    {
        getErrMsg(u32Ret, strErrMsg, 300);
        ol_printf("%s\n", strErrMsg);
    }

    return u32Ret;
}

/*---------------------------------------------------------------------------*/


