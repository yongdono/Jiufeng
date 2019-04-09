/**
 *  @file jf_thread.h
 *
 *  @brief Process header file. Provide some functional routine for
 *   thread manipulation
 *
 *  @author Min Zhang
 *
 *  @note Routines declared in this file are included in jf_thread object
 *  @note For linux, link with stringparse library
 *  @note For Windows, link with psapi.lib
 *  
 */

#ifndef JIUTAI_THREAD_H
#define JIUTAI_THREAD_H

/* --- standard C lib header files ----------------------------------------- */

/* --- internal header files ----------------------------------------------- */
#include "jf_basic.h"

/* --- constant definitions ------------------------------------------------ */
#if defined(LINUX)
    #define jf_thread_getCurrentId()   pthread_self()
#elif defined(WINDOWS)
    #define jf_thread_getCurrentId()   GetCurrentThreadId()
    typedef DWORD pthread_t;
    #define sleep(seconds)  Sleep(seconds * 1000)
#endif

/* --- data structures ----------------------------------------------------- */

typedef struct
{
    boolean_t jta_bDetached;
    u8 jta_u8Reserved[31];
} jf_thread_attr_t;

typedef struct
{
#if defined(LINUX)
    pthread_t jti_ptThreadId;
#elif defined(WINDOWS)
    HANDLE jti_hThread;
#endif
} jf_thread_id_t;

#if defined(LINUX)
    #if defined(JIUFENG_64BIT)
        #define JF_THREAD_RETURN_VALUE       void *
        #define JF_THREAD_RETURN(retval)     pthread_exit((void *)(u64)retval);
        typedef void * (* jf_thread_fnRoutine_t)(void * pArg);
    #else
        #define JF_THREAD_RETURN_VALUE       void *
        #define JF_THREAD_RETURN(retval)     pthread_exit((void *)retval);
        typedef void * (* jf_thread_fnRoutine_t)(void * pArg);
    #endif
#elif defined(WINDOWS)
    #define JF_THREAD_RETURN_VALUE           u32 WINAPI
    #define JF_THREAD_RETURN                 return
    typedef LPTHREAD_START_ROUTINE           jf_thread_fnRoutine_t;
#endif

typedef void (* jf_process_fnSignalHandler_t)(olint_t signal);

/* --- functional routines ------------------------------------------------- */

void jf_thread_initId(jf_thread_id_t * pThreadId);

boolean_t jf_thread_isValidId(jf_thread_id_t * pThreadId);

u32 jf_thread_create(
    jf_thread_id_t * pThreadId, jf_thread_attr_t * pAttr,
    jf_thread_fnRoutine_t fnRoutine, void * pArg);

u32 jf_thread_terminate(jf_thread_id_t * pThreadId);

u32 jf_thread_waitForThreadTermination(jf_thread_id_t threadId, u32 * pu32RetCode);


#endif /*JIUTAI_THREAD_H*/

/*---------------------------------------------------------------------------*/

