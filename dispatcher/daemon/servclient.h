/**
 *  @file servclient.h
 *
 *  @brief Header file for message client of dispather service.
 *
 *  @author Min Zhang
 *
 *  @note
 *  
 */

#ifndef JIUFENG_SERVCLIENT_H
#define JIUFENG_SERVCLIENT_H

/* --- standard C lib header files -------------------------------------------------------------- */

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_err.h"
#include "jf_linklist.h"

#include "dispatchercommon.h"

/* --- constant definitions --------------------------------------------------------------------- */


/* --- data structures -------------------------------------------------------------------------- */

/** The parameter for creating dispatcher service client.
 */
typedef struct
{
    /**Max socket connection in async client socket for a service.*/
    u32 cdscp_u32MaxConnInClient;
    /**The directory containing the socket files.*/
    olchar_t * cdscp_pstrSocketDir;
} create_dispatcher_serv_client_param_t;

/* --- functional routines ---------------------------------------------------------------------- */

/** Create dispatcher service client based on service config from file.
 */
u32 createDispatcherServClients(
    jf_linklist_t * pjlServConfig, create_dispatcher_serv_client_param_t * pcdscp);

/** Destroy dispatcher service client.
 */
u32 destroyDispatcherServClients(void);

/** Start dispatcher service client.
 */
u32 startDispatcherServClients(void);

/** Stop dispatcher service client.
 */
u32 stopDispatcherServClients(void);

/** Pause dispatcher service client, the client will stop sending out the message.
 */
u32 pauseDispatcherServClient(pid_t servPid);

/** Resume dispatcher service client, the client will start sending out the message.
 */
u32 resumeDispatcherServClient(pid_t servPid);

/** Dispatch message to service.
 */
u32 dispatchMsgToServClients(dispatcher_msg_t * pdm);

#endif /*JIUFENG_SERVCLIENT_H*/

/*------------------------------------------------------------------------------------------------*/


