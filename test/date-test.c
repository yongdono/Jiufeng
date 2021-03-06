/**
 *  @file date-test.c
 *
 *  @brief test file for jf_date common object
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
#include "jf_date.h"
#include "jf_string.h"

/* --- private data/data structure section ------------------------------------------------------ */

/* --- private routine section ------------------------------------------------------------------ */
/*leap year*/
static void test1(void)
{
    olint_t year[10] = {234, 1600, 1700, 1900, 1950, 2000, 2002, 2004, 2007, 2008};
    olint_t i;

    for (i = 0; i < 10; i ++)
    {
        if (jf_date_isLeapYear(year[i]))
            ol_printf("year %d is leap year\n", year[i]);
        else
            ol_printf("year %d is not leap year\n", year[i]);
    }
    ol_printf("\n");
}

#define NUM_OF_TEST_ENTRY  6

static void test2(void)
{
    olint_t data[NUM_OF_TEST_ENTRY][3] = {{1970, 1, 1}, {1970, 6, 5},
                      {1970, 11, 20}, {1971, 1, 5},
                      {1971, 5, 1}, {1971, 10, 5}};
    olint_t i;

    for (i = 0; i < NUM_OF_TEST_ENTRY; i ++)
    {
        ol_printf("%d-%d-%d is %d days from 1970\n",
               data[i][0], data[i][1], data[i][2],
               jf_date_convertDateToDaysFrom1970(data[i][0], data[i][1], data[i][2]));
    }

    ol_printf("\n");
}

static void test3(void)
{
    olint_t days[] = {1, 156, 324, 370, 486, 643, 12400, 12756, 12759, 13125, 13149, 13150, 13151};
    olint_t numofdays = sizeof(days) / sizeof(olint_t);
    olint_t i;
    olint_t year, mon, day;


    for (i = 0; i < numofdays; i ++)
    {
        jf_date_convertDaysFrom1970ToDate(days[i], &year, &mon, &day);
        ol_printf("%d days from 1700 is %d-%d-%d\n",
               days[i], year, mon, day);

    }

    ol_printf("\n");
}

#define NUM_OF_TEST4_ENTRY  10

static void test4(void)
{
    olint_t data[NUM_OF_TEST4_ENTRY][3] = {{2013, 1, 1}, {2013, 6, 5},
                                       {2013, 11, 20}, {2014, 1, 5},
                                       {2014, 5, 1}, {2014, 10, 5},
                                       {2013, 2, 28}, {2014, 7, 31},
                                       {2014, 9, 30}, {2014, 12, 5}};
    olint_t i;

    for (i = 0; i < NUM_OF_TEST4_ENTRY; i ++)
    {
        ol_printf("%d-%d-%d is %d of week\n",
               data[i][0], data[i][1], data[i][2],
               jf_date_getDayOfWeekFromDate(data[i][0], data[i][1], data[i][2]));
    }

    ol_printf("\n");
}

static u32 _testGetNextTradingDate(const olchar_t * pstrCurr, olchar_t * pstrNext)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olint_t year, month, day;
    olint_t days, dw;

    jf_date_getDate2FromString(pstrCurr, &year, &month, &day);
    days = jf_date_convertDateToDaysFrom1970(year, month, day);
    dw = jf_date_getDayOfWeekFromDate(year, month, day);
    if (dw == 5)
    {
        jf_date_convertDaysFrom1970ToDate(days + 3, &year, &month, &day);
        jf_date_getStringDate2(pstrNext, year, month, day);
    }
    else if (dw == 6)
    {
        jf_date_convertDaysFrom1970ToDate(days + 2, &year, &month, &day);
        jf_date_getStringDate2(pstrNext, year, month, day);
    }
    else
    {
        jf_date_convertDaysFrom1970ToDate(days + 1, &year, &month, &day);
        jf_date_getStringDate2(pstrNext, year, month, day);
    }

    return u32Ret;
}


/* --- public routine section ------------------------------------------------------------------- */

olint_t main(olint_t argc, olchar_t ** argv)
{
    u32 u32Ret = JF_ERR_NO_ERROR;
    olint_t year, mon, day;
    char curdate[16];
    char strdate[16];

    jf_date_getDateToday(&year, &mon, &day);
    ol_printf("Today is %4d-%02d-%02d\n", year, mon, day);

    test1();

    test2();

    test3();

    test4();

    ol_strcpy(curdate, "2004-12-03");
    _testGetNextTradingDate(curdate, strdate);
    ol_printf("curdate: %s, nextdate: %s\n", curdate, strdate);
    return u32Ret;
}

/*------------------------------------------------------------------------------------------------*/

