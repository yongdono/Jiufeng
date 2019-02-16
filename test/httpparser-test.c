/**
 *  @file httpparser-test.c
 *
 *  @brief test file for httpparser library
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
#include "httpparser.h"
#include "xmalloc.h"
#include "hexstr.h"

/* --- private data/data structure section --------------------------------- */

static boolean_t ls_bParseHttp = FALSE;
static boolean_t ls_bParseUri = FALSE;
static boolean_t ls_bGenerateHttpMsg = FALSE;

/* --- private routine section---------------------------------------------- */
static void _printUsage(void)
{
    ol_printf("\
Usage: httpparser-test [-p] [-u] [-g] [-h] [logger options] \n\
    -p parse http header.\n\
    -u parse URI.\n\
    -g generating http message.\n\
    -h print the usage.\n\
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
    u32 u32Value;

    while (((nOpt = getopt(argc, argv, "gupT:F:S:h")) != -1) &&
           (u32Ret == OLERR_NO_ERROR))
    {
        switch (nOpt)
        {
        case '?':
        case 'h':
            _printUsage();
            exit(0);
            break;
        case 'p':
            ls_bParseHttp = TRUE;
            break;
        case 'u':
            ls_bParseUri = TRUE;
            break;
        case 'g':
            ls_bGenerateHttpMsg = TRUE;
            break;
        case 'T':
            if (ol_sscanf(optarg, "%d", &u32Value) == 1)
            {
                plp->lp_u8TraceLevel = (u8)u32Value;
            }
            else
            {
                u32Ret = OLERR_INVALID_PARAM;
            }
            break;
        case 'F':
            plp->lp_bLogToFile = TRUE;
            plp->lp_pstrLogFilePath = optarg;
            break;
        case 'S':
            if (ol_sscanf(optarg, "%d", &u32Value) == 1)
            {
                plp->lp_sLogFile = u32Value;
            }
            else
            {
                u32Ret = OLERR_INVALID_PARAM;
            }
            break;
        default:
            u32Ret = OLERR_INVALID_OPTION;
            break;
        }
    }

    return u32Ret;
}

static u32 _getRawHeader(packet_header_t ** ppHeader)
{
    u32 u32Ret = OLERR_NO_ERROR;
    packet_header_t * pph;
    u8 u8Body[10];
    olchar_t str[100];
    olint_t len;

    u32Ret = createEmptyPacketHeader(&pph);
    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = setDirective(pph, "POST", 4, "/login", 6);
        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "Accept", 6, "*/*", 3, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "Accept-Language", 15, "zh-CN", 5, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "x-flash-version", 15, "10,1,53,64", 10, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "Content-Type", 12,
                                   "application/octet-stream", 24, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
        {
            len = ol_snprintf(str, sizeof(str) - 1, "%u", (u32)sizeof(u8Body));
            u32Ret = addHeaderLine(pph, "Content-Length", 14, str, len, TRUE);
        }

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "Accept-Encoding", 15, "gzip, deflate", 13, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "User-Agent", 10, "greaty", 6, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "Host", 4, "192.168.88.3:8080", 17, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "Connection", 10, "Keep-Alive", 10, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = addHeaderLine(pph, "Cache-Control", 13, "no-cache", 8, TRUE);

        if (u32Ret == OLERR_NO_ERROR)
            u32Ret = setBody(pph, u8Body, sizeof(u8Body), TRUE);

        *ppHeader = pph;
    }

    return u32Ret;
}

static u32 _testHttpMsg(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    packet_header_t * pph = NULL;
    olchar_t * pstr;
    olsize_t size;

    u32Ret = _getRawHeader(&pph);
    if (u32Ret == OLERR_NO_ERROR)
    {
        u32Ret = getRawPacket(pph, &pstr, &size);
        if (u32Ret == OLERR_NO_ERROR)
        {
            ol_printf("HTTP message\n");
            dumpDataInByteHex((u8 *)pstr, size);

            xfree((void **)&pstr);
        }
    }

    if (pph != NULL)
        destroyPacketHeader(&pph);

    return u32Ret;
}

static void _copyHeaderDataString(
    olchar_t * value, olchar_t * data, olsize_t sData)
{
    ol_memcpy(value, data, sData);
    value[sData] = '\0';
}

static void _printHttpPacketHeader(packet_header_t * pph)
{
    olchar_t value[256];
    packet_header_field_t * field;

    _copyHeaderDataString(value, pph->ph_pstrDirective, pph->ph_sDirective);
    ol_printf("Directive: %s(%d)\n", value, pph->ph_sDirective);

    _copyHeaderDataString(
        value, pph->ph_pstrDirectiveObj, pph->ph_sDirectiveObj);
    ol_printf("DirectiveObj: %s(%d)\n", value, pph->ph_sDirectiveObj);

    ol_printf("Status: %d\n", pph->ph_nStatusCode);

    _copyHeaderDataString(value, pph->ph_pstrStatusData, pph->ph_sStatusData);
    ol_printf("StatusData: %s(%d)\n", value,  pph->ph_sStatusData);

    _copyHeaderDataString(value, pph->ph_pstrVersion, pph->ph_sVersion);
    ol_printf("Version: %s(%d)\n", value, pph->ph_sVersion);

    _copyHeaderDataString(value, (char *)pph->ph_pu8Body, pph->ph_sBody);
    ol_printf("Body: %s(%d)\n", value, pph->ph_sBody);

    field = pph->ph_pphfFirst;
    while (field != NULL)
    {
        _copyHeaderDataString(value, field->phf_pstrName, field->phf_sName);
        ol_printf("Header Name: %s(%d)\n", value, field->phf_sName);

        _copyHeaderDataString(value, field->phf_pstrData, field->phf_sData);
        ol_printf("Header Value: %s(%d)\n", value, field->phf_sData);

        field = field->phf_pphfNext;
    }
}

#define HTTP_MSG_1 "\
HTTP/1.1 200 OK\r\n\
Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n\
Server: Apache\r\n\
Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n\
ETag: \"34aa387-d-1568eb00\"\r\n\
Accept-Ranges: bytes\r\n\
Content-Length: 10\r\n\
Vary: Accept-Encoding\r\n\
Content-Type: text/plain\r\n\
\r\n\
1234567890\
"

#define HTTP_MSG_2 "\
GET /hello.txt HTTP/1.1\r\n\
User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3\r\n\
Host: www.example.com\r\n\
Accept-Language: en, mi\r\n\
\r\n\
"

typedef struct
{
    olchar_t * pstrHttp;
    u32 u32ErrCode;
} test_http_parser_t;

static u32 _testParseHttp(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t strErrMsg[300];
    test_http_parser_t thp[] = {
        {HTTP_MSG_1, OLERR_NO_ERROR},
        {HTTP_MSG_2, OLERR_NO_ERROR},
    };
    u32 u32NumOfCase = sizeof(thp) / sizeof(test_http_parser_t);
    u32 u32Index;
    packet_header_t * pph = NULL;

    for (u32Index = 0; u32Index < u32NumOfCase; u32Index ++)
    {
        ol_printf("---------------------------------------------------\n");
        ol_printf("Parse http message:\n%s\n\n", thp[u32Index].pstrHttp);

        u32Ret = parsePacketHeader(
            &pph, thp[u32Index].pstrHttp, 0, strlen(thp[u32Index].pstrHttp));
        if (u32Ret == OLERR_NO_ERROR)
        {
            if (u32Ret == OLERR_NO_ERROR)
            {
                ol_printf("Parse result:\n");            
                _printHttpPacketHeader(pph);
            }
            else if (u32Ret != thp[u32Index].u32ErrCode)
            {
                u32Ret = OLERR_PROGRAM_ERROR;
                break;
            }
            else
            {
                ol_printf("Parse result:\n");
                getErrMsg(u32Ret, strErrMsg, 300);
                ol_printf("%s\n", strErrMsg);
            }
            
            destroyPacketHeader(&pph);
        }

        ol_printf("\n");
    }

    return u32Ret;
}

static u32 _testParseUri(void)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t strErrMsg[300];
    olchar_t * pIp, * pPath;
    u16 u16Port;
    test_http_parser_t thp[] = {
        {"http:/192.168.1.1:8080/home/index.html", OLERR_INVALID_HTTP_URI},
        {"http://192.168.1.1:8080/home/index.html", OLERR_NO_ERROR},
        {"http://www.google.com/application/index.html", OLERR_NO_ERROR},
    };
    u32 u32NumOfCase = sizeof(thp) / sizeof(test_http_parser_t);
    u32 u32Index;

    for (u32Index = 0; u32Index < u32NumOfCase; u32Index ++)
    {
        ol_printf("---------------------------------------------------\n");

        u32Ret = parseUri(thp[u32Index].pstrHttp, &pIp, &u16Port, &pPath);
        if (u32Ret == OLERR_NO_ERROR)
        {
            ol_printf("URI: %s\n", thp[u32Index].pstrHttp);
            ol_printf("IP: %s\n", pIp);
            ol_printf("Port: %d\n", u16Port);
            ol_printf("Path: %s\n", pPath);

            xfree((void **)&pIp);
            xfree((void **)&pPath);
        }
        else if (u32Ret != thp[u32Index].u32ErrCode)
        {
            u32Ret = OLERR_PROGRAM_ERROR;
            break;
        }
        else
        {
            getErrMsg(u32Ret, strErrMsg, 300);
            ol_printf("%s\n", strErrMsg);
        }

        ol_printf("\n");
    }
    
    return u32Ret;
}
    
/* --- public routine section ---------------------------------------------- */

olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    olchar_t strErrMsg[300];
    logger_param_t lpParam;

    memset(&lpParam, 0, sizeof(logger_param_t));
    lpParam.lp_pstrCallerName = "HTTPPARSER";
//    lpParam.lp_bLogToStdout = TRUE;
    lpParam.lp_u8TraceLevel = LOGGER_TRACE_DEBUG;

    u32Ret = _parseCmdLineParam(argc, argv, &lpParam);
    if (u32Ret == OLERR_NO_ERROR)
    {
        initLogger(&lpParam);

        if (ls_bParseHttp)
            u32Ret = _testParseHttp();
        else if (ls_bParseUri)
            u32Ret = _testParseUri();
        else if (ls_bGenerateHttpMsg)
            u32Ret = _testHttpMsg();

        finiLogger();
    }

    if (u32Ret != OLERR_NO_ERROR)
    {
        getErrMsg(u32Ret, strErrMsg, 300);
        ol_printf("%s\n", strErrMsg);
    }

    return u32Ret;
}

/*--------------------------------------------------------------------------*/
