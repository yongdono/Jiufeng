/**
 *  @file jf_network.h
 *
 *  @brief Header file of network library
 *
 *  @author Min Zhang
 *
 *  @note Routines declared in this file are included in jf_network library
 *  @note timeout is in second if not specified
 *
 */

/*------------------------------------------------------------------------------------------------*/

#ifndef JIUFENG_NETWORK_H
#define JIUFENG_NETWORK_H

/* --- standard C lib header files -------------------------------------------------------------- */
#if defined(LINUX)
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <sys/ioctl.h>
#elif defined(WINDOWS)
    #include <Iphlpapi.h>
#endif

/* --- internal header files -------------------------------------------------------------------- */
#include "jf_basic.h"
#include "jf_err.h"
#include "jf_ifmgmt.h"
#include "jf_mutex.h"

#undef NETWORKAPI
#undef NETWORKCALL
#ifdef WINDOWS
    #include "windows.h"
    #if defined(JIUFENG_NETWORK_DLL)
        #define NETWORKAPI  __declspec(dllexport)
        #define NETWORKCALL
    #else
        #define NETWORKAPI
        #define NETWORKCALL __cdecl
    #endif
#else
    #define NETWORKAPI
    #define NETWORKCALL
#endif

/* --- constant definitions --------------------------------------------------------------------- */

/** Maximum name length for socket and utimer
 */
#define JF_NETWORK_MAX_NAME_LEN     (32)

/* --- data structures -------------------------------------------------------------------------- */
#if defined(LINUX)

#elif defined(WINDOWS)

#endif

/** socket
 */

typedef void  jf_network_socket_t;
typedef void  jf_network_asocket_t;
typedef void  jf_network_assocket_t;
typedef void  jf_network_acsocket_t;
typedef void  jf_network_adgram_t;

/** chain
 */

typedef void  jf_network_chain_t;
typedef void  jf_network_chain_object_t;

/** Callback function before select()
 *
 *  @param pObject [in] chain object
 *  @param readset [in/out] read fd set
 *  @param writeset [in/out] write fd set
 *  @param errorset [in/out] error fd set
 *  @param pu32BlockTime [in/out] timeout in millisecond for select()
 *
 *  @return the error code
 */
typedef u32 (* jf_network_fnPreSelectChainObject_t)(
    jf_network_chain_object_t * pObject, fd_set * readset,
    fd_set * writeset, fd_set * errorset, u32 * pu32BlockTime);

/** Callback function after select()
 *
 *  @param pObject [in] chain object
 *  @param nReady [in] number of ready fd 
 *  @param readset [in] read fd set
 *  @param writeset [in] write fd set
 *  @param errorset [in] error fd set
 *
 *  @return the error code
 */
typedef u32 (* jf_network_fnPostSelectChainObject_t)(
    jf_network_chain_object_t * pObject, olint_t nReady,
    fd_set * readset, fd_set * writeset, fd_set * errorset);

/** Header of chain object, MUST be placed at the beginning of the object
 */
typedef struct
{
    jf_network_fnPreSelectChainObject_t jncoh_fnPreSelect;
    jf_network_fnPostSelectChainObject_t jncoh_fnPostSelect;
} jf_network_chain_object_header_t;

/** utimer
 */

typedef void  jf_network_utimer_t;

typedef u32 (* jf_network_fnCallbackOfUtimerItem_t)(void * pData);
typedef u32 (* jf_network_fnDestroyUtimerItemData_t)(void ** ppData);

/** async server socket
 */

/** The function is to notify upper layer there are incoming data. The pUser is
 *  set by fnAssocketOnConnect_t when a new incoming connection is accepted
 */
typedef u32 (* jf_network_fnAssocketOnData_t)(
    jf_network_assocket_t * pAssocket, jf_network_asocket_t * pAsocket,
    u8 * pu8Buffer, olsize_t * psBeginPointer, olsize_t sEndPointer, void * pUser);

/** The function is to notify upper layer there are new connection
 */
typedef u32 (* jf_network_fnAssocketOnConnect_t)(
    jf_network_assocket_t * pAssocket, jf_network_asocket_t * pAsocket, void ** ppUser);

/** The function is to notify upper layer a connection is closed.
 *
 *  @note The access to the asocket being closed is not allowed.
 *  @note DO NOT use asDisconnect in this callback function as the connection is closed already
 *
 *  @param u32Status [out] the reason why the connection is closed
 */
typedef u32 (* jf_network_fnAssocketOnDisconnect_t)(
    jf_network_assocket_t * pAssocket, jf_network_asocket_t * pAsocket,
    u32 u32Status, void * pUser);

/** The function is to notify upper layer the data is sent to peer successfully
 */
typedef u32 (* jf_network_fnAssocketOnSendData_t)(
    jf_network_assocket_t * pAssocket, jf_network_asocket_t * pAsocket, u32 u32Status,
    u8 * pu8Buffer, olsize_t sBuf, void * pUser);

typedef struct
{
    /**The initial size of the receive buffer*/
    olsize_t jnacp_sInitialBuf;
    /**The max number of simultaneous connections that will be allowed*/
    u32 jnacp_u32MaxConn;
    u32 jnacp_u32Reserved;
    jf_ipaddr_t jnacp_jiServer;
    /**The port number to bind to. 0 will select a random port*/
    u16 jnacp_u16ServerPort;
    u16 jnacp_u16Reserved[3];
    /**Function that triggers when a connection is established*/
    jf_network_fnAssocketOnConnect_t jnacp_fnOnConnect;
    /**Function that triggers when a connection is closed*/
    jf_network_fnAssocketOnDisconnect_t jnacp_fnOnDisconnect;
    /**Function that triggers when data is coming*/
    jf_network_fnAssocketOnData_t jnacp_fnOnData;
    /**Function that triggers when pending sends are complete*/
    jf_network_fnAssocketOnSendData_t jnacp_fnOnSendData;
    olchar_t * jnacp_pstrName;
} jf_network_assocket_create_param_t;

/** async client socket
 */

/** The function is to notify upper layer there are incoming data
 */
typedef u32 (* jf_network_fnAcsocketOnData_t)(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket,
    u8 * pu8Buffer, olsize_t * psBeginPointer, olsize_t sEndPointer, void * pUser);

/** The function is to notify upper layer there are new connection
 */
typedef u32 (* jf_network_fnAcsocketOnConnect_t)(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket,
    u32 u32Status, void * pUser);

/** The function is to notify upper layer a connection is closed.
 *
 *  @note The access to the asocket being closed is not recommended.
 *  @note DO NOT use disconnectAsocket in this callback function as asocket can
 *   handle it by itself.
 *
 *  @param u32Status [in] the reason why the connection is closed
 */
typedef u32 (* jf_network_fnAcsocketOnDisconnect_t)(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket,
    u32 u32Status, void * pUser);

/** The function is to notify upper layer the data is sent to peer successfully
 */
typedef u32 (* jf_network_fnAcsocketOnSendData_t)(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket,
    u32 u32Status, u8 * pu8Buffer, olsize_t sBuf, void * pUser);


typedef struct
{
    /**The initial size of the receive buffer*/
    olsize_t jnacp_sInitialBuf;
    /**The max number of simultaneous connections that will be allowed*/
    u32 jnacp_u32MaxConn;
    u8 jnacp_u8Reserved[8];
    /**Function that triggers when a connection is established*/
    jf_network_fnAcsocketOnConnect_t jnacp_fnOnConnect;
    /**Function that triggers when a connection is closed*/
    jf_network_fnAcsocketOnDisconnect_t jnacp_fnOnDisconnect;
    /**Function that triggers when data is received*/
    jf_network_fnAcsocketOnData_t jnacp_fnOnData;
    /**Function that triggers when pending sends are complete*/
    jf_network_fnAcsocketOnSendData_t jnacp_fnOnSendData;
    olchar_t * jnacp_pstrName;
} jf_network_acsocket_create_param_t;


/* --- functional routines ---------------------------------------------------------------------- */

/** socket
 */

NETWORKAPI u32 NETWORKCALL jf_network_createSocket(
    olint_t domain, olint_t type, olint_t protocol, jf_network_socket_t ** ppSocket);

NETWORKAPI u32 NETWORKCALL jf_network_destroySocket(jf_network_socket_t ** ppSocket);

/** Allocates a UDP socket for a given interface, choosing a random port number
 *  from 55000 to 65000
 *
 *  @param pjiLocal [in] the interface to bind to 
 *  @param pu16Port [in] the port number
 *  @param ppSocket [out] the created UDP socket 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_createDgramSocket(
    jf_ipaddr_t * pjiLocal, u16 * pu16Port, jf_network_socket_t ** ppSocket);

/** Allocates a TCP socket for a given interface, choosing a random port number
 *  from 50000 to 65000 if port is 0, it will bind the address to socket
 *
 *  @note If the port is 0, we select a random port from the port number range
 *
 *  @param pjiLocal [in] the interface to bind to 
 *  @param pu16Port [in/out] the port number to bind to
 *  @param ppSocket [out] the created UDP socket 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_createStreamSocket(
    jf_ipaddr_t * pjiLocal, u16 * pu16Port, jf_network_socket_t ** ppSocket);

/** Allocate a TCP socket according to address type
 */
NETWORKAPI u32 NETWORKCALL jf_network_createTypeStreamSocket(
    u8 u8AddrType, jf_network_socket_t ** ppSocket);

NETWORKAPI u32 NETWORKCALL jf_network_createTypeDgramSocket(
    u8 u8AddrType, jf_network_socket_t ** ppSocket);

NETWORKAPI u32 NETWORKCALL jf_network_ioctlSocket(
    jf_network_socket_t * pSocket, olint_t req, void * pArg);

NETWORKAPI u32 NETWORKCALL jf_network_setSocketBlock(jf_network_socket_t * pSocket);

NETWORKAPI u32 NETWORKCALL jf_network_setSocketNonblock(jf_network_socket_t * pSocket);

NETWORKAPI u32 NETWORKCALL jf_network_joinMulticastGroup(
    jf_network_socket_t * pSocket, jf_ipaddr_t * pjiAddr, jf_ipaddr_t * pjiMulticaseAddr);

NETWORKAPI u32 NETWORKCALL jf_network_enableBroadcast(jf_network_socket_t * pSocket);

/** Try to send all data but only send once, the actual sent size is in psSend
 */
NETWORKAPI u32 NETWORKCALL jf_network_send(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psSend);

/** Try to send all data but only send once, unless timeout the actual sent size
 *  is in psSend
 */
NETWORKAPI u32 NETWORKCALL jf_network_sendWithTimeout(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psSend,
    u32 u32Timeout);

/** Try to send all data with possible several round, until an error occurs,
 *  the actual sent size is in psSend
 */
NETWORKAPI u32 NETWORKCALL jf_network_sendn(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psSend);

/** Try to send all data with possible several round, until an error occurs
 *  or timeout, the actual sent size is in psSend
 */
NETWORKAPI u32 NETWORKCALL jf_network_sendnWithTimeout(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psSend, u32 u32Timeout);

/** Try to receive all data but only receive once, the actual received size is
 *  in psRecv
 */
NETWORKAPI u32 NETWORKCALL jf_network_recv(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psRecv);

/** Try to receive all data but only receive once, unless timeout the actual
 *  received size is in psRecv
 */
NETWORKAPI u32 NETWORKCALL jf_network_recvWithTimeout(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psRecv, u32 u32Timeout);

/** Try to recveive all data with possible several round, until an error occurs,
 *  the actual recveived size is in psRecv
 */
NETWORKAPI u32 NETWORKCALL jf_network_recvn(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psRecv);

/** Try to recveive all data with possible several round, until an error occurs
 *  or timeout, the actual recveived size is in psRecv
 */
NETWORKAPI u32 NETWORKCALL jf_network_recvnWithTimeout(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psRecv, u32 u32Timeout);

/** Try to receive all data but only receive once, unless timeout the actual
 *  received size is in psRecv
 */
NETWORKAPI u32 NETWORKCALL jf_network_recvfromWithTimeout(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psRecv,
    u32 u32Timeout, jf_ipaddr_t * pjiFrom, u16 * pu16Port);

NETWORKAPI u32 NETWORKCALL jf_network_connect(
    jf_network_socket_t * pSocket, const jf_ipaddr_t * pji, u16 u16Port);

NETWORKAPI u32 NETWORKCALL jf_network_connectWithTimeout(
    jf_network_socket_t * pSocket, const jf_ipaddr_t * pji, u16 u16Port, u32 u32Timeout);

NETWORKAPI u32 NETWORKCALL jf_network_listen(jf_network_socket_t * pSocket, olint_t backlog);

NETWORKAPI u32 NETWORKCALL jf_network_accept(
    jf_network_socket_t * pListen, jf_ipaddr_t * pji, u16 * pu16Port,
    jf_network_socket_t ** ppSocket);

NETWORKAPI u32 NETWORKCALL jf_network_sendto(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psSend, const jf_ipaddr_t * pjiTo,
    u16 u16Port);

NETWORKAPI u32 NETWORKCALL jf_network_recvfrom(
    jf_network_socket_t * pSocket, void * pBuffer, olsize_t * psRecv, jf_ipaddr_t * pjiTo,
    u16 * pu16Port);

NETWORKAPI u32 NETWORKCALL jf_network_createSocketPair(
    olint_t domain, olint_t type, jf_network_socket_t * psPair[2]);

NETWORKAPI u32 NETWORKCALL jf_network_destroySocketPair(jf_network_socket_t * sPair[2]);

NETWORKAPI u32 NETWORKCALL jf_network_select(
    fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout,
    u32 * pu32Ready);

NETWORKAPI u32 NETWORKCALL jf_network_getSocketName(
    jf_network_socket_t * pSocket, struct sockaddr * pName, olint_t * pnNameLen);

NETWORKAPI void NETWORKCALL jf_network_clearSocketFromFdSet(
    jf_network_socket_t * pSocket, fd_set * set);

NETWORKAPI boolean_t NETWORKCALL jf_network_isSocketSetInFdSet(
    jf_network_socket_t * pSocket, fd_set * set);

NETWORKAPI void NETWORKCALL jf_network_setSocketToFdSet(
    jf_network_socket_t * pSocket, fd_set * set);

NETWORKAPI void NETWORKCALL jf_network_clearFdSet(fd_set * set);

/** chain
 */

/** Create a chain
 *
 *  @param ppChain [out] the chain to create 
 * 
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_createChain(jf_network_chain_t ** ppChain);

/** Destroy the chain
 *
 *  @param ppChain [in/out] the chain to destory 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_destroyChain(jf_network_chain_t ** ppChain);

/** Add links to the chain
 *
 *  @note All objects added to the chain must extend/implement
 *   jf_network_chain_object_header_t \n
 *  Eg.  \n
 *  struct object    \n
 *  {    \n
 *      jf_network_chain_object_header_t header \n
 *      ...;                               \n
 *  }   \n
 *
 *  @param pChain [in] the chain to add the link to 
 *  @param pObject [in] the link to add to the chain
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_appendToChain(
    jf_network_chain_t * pChain, jf_network_chain_object_t * pObject);

/** Start a Chain
 *
 *  @note This method will use the current thread. All events and processing
 *   will be done on this thread. This method will not return until
 *   stopChain is called.
 *
 *  @param pChain [in] the chain to start 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_startChain(jf_network_chain_t * pChain);

/** Stop a chain, imply the destruction of the chain
 *
 *  @note This will signal the process to shutdown. When the chain cleans itself
 *   up, the thread that is locked on pChain will return.
 *
 *  @param pChain [in] the chain to stop 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_stopChain(jf_network_chain_t * pChain);

/** Wakeup the chain
 *
 *  @note This will wake up a chain from sleeping by close the fake socket.
 *
 *  @param pChain [in] The chain to stop 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_wakeupChain(jf_network_chain_t * pChain);

/** utimer
 */

/** Add a timed callback with second granularity
 *
 *  @param pUtimer [in] the timer
 *  @param pData [in] the data object to associate with the timed callback 
 *  @param u32Seconds [in] number of seconds for the timed callback 
 *  @param fnCallback [in] the callback function pointer 
 *  @param fnDestroy [in] the abort function pointer, which triggers all 
 *   non-triggered timed callbacks, upon shutdown
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_addUtimerItem(
    jf_network_utimer_t * pUtimer, void * pData, u32 u32Seconds,
    jf_network_fnCallbackOfUtimerItem_t fnCallback, jf_network_fnDestroyUtimerItemData_t fnDestroy);

/** Removes timed callback(s) specified by pData from an utimer
 *
 *  @note If there are multiple item pertaining to pData, all of them are
 *   removed
 *  @note Before destroying the utimer item structure, (*fnDestroy)() is called
 *
 *  @param pUtimer [in] the utimer object to remove the callback from 
 *  @param pData [in] the data object to remove 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_removeUtimerItem(jf_network_utimer_t * pUtimer, void * pData);

/** Destroy a timer
 *
 *  @note This method never needs to be explicitly called by the developer. It
 *   is called by the chain as a Destroy.
 *
 *  @param ppUtimer [in/out] the utimer object 
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_destroyUtimer(jf_network_utimer_t ** ppUtimer);

/** Creates an empty utimer 
 *
 *  @note All events are triggered on the thread. Developers must NEVER block
 *   this thread
 *
 *  @param pChain [in] the chain to add the utimer to 
 *  @param ppUtimer [out] the utimer
 *  @param pstrName [in] the name of the utimer object
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_createUtimer(
    jf_network_chain_t * pChain, jf_network_utimer_t ** ppUtimer, const olchar_t * pstrName);

NETWORKAPI void NETWORKCALL jf_network_dumpUtimerItem(jf_network_utimer_t * pUtimer);

/** async server socket
 */

/** Create async server socket.
 *
 *  @param pChain [in] the chain to add this assocket to
 *  @param ppAssocket [out] the async server socket
 *  @param pjnacp [in] the parameters for creating assocket
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_createAssocket(
    jf_network_chain_t * pChain, jf_network_assocket_t ** ppAssocket,
    jf_network_assocket_create_param_t * pjnacp);

/** Destroy async server socket.
 *
 */
NETWORKAPI u32 NETWORKCALL jf_network_destroyAssocket(jf_network_assocket_t ** ppAssocket);

/** Returns the port number the server is bound to
 *
 *  @param pAssocket [in] the assocket to query
 *
 *  @return the listening port number
 */
NETWORKAPI u16 NETWORKCALL jf_network_getPortNumberOfAssocket(jf_network_assocket_t * pAssocket);

/** Returns the user's Tag associated with the assocket
 *
 *  @param pAssocket [in] the assocket to query
 *
 *  @return the user tag
 */
NETWORKAPI void * NETWORKCALL jf_network_getTagOfAssocket(jf_network_assocket_t * pAssocket);

/** Sets the user's tag associated with the assocket
 *
 *  @param pAssocket [in] the assocket to save the tag to
 *  @param pTag [in] the tag
 */
NETWORKAPI void NETWORKCALL jf_network_setTagOfAssocket(
    jf_network_assocket_t * pAssocket, void * pTag);

NETWORKAPI u32 NETWORKCALL jf_network_disconnectAssocket(
    jf_network_assocket_t * pAssocket, jf_network_asocket_t * pAsocket);

NETWORKAPI u32 NETWORKCALL jf_network_sendAssocketData(
    jf_network_assocket_t * pAssocket, jf_network_asocket_t * pAsocket, u8 * pu8Buffer,
    olsize_t sBuf);

/** async client socket
 */

NETWORKAPI u32 NETWORKCALL jf_network_createAcsocket(
    jf_network_chain_t * pChain, jf_network_acsocket_t ** ppAcsocket,
    jf_network_acsocket_create_param_t * pjnacp);

/** Destroy async client socket
 *
 *  @param ppAcsocket [in/out] the async client socket
 *
 *  @return the error code
 */
NETWORKAPI u32 NETWORKCALL jf_network_destroyAcsocket(jf_network_acsocket_t ** ppAcsocket);

/** Returns the user's tag associated with the acsocket
 *
 *  @param pAcsocket [in] the acsocket to query
 *
 *  @return the user Tag
 */
NETWORKAPI void * NETWORKCALL jf_network_getTagOfAcsocket(jf_network_acsocket_t * pAcsocket);

/** Sets the user's tag associated with the acsocket
 *
 *  @param pAcsocket [in] the acsocket to save the tag to
 *  @param pTag [in] the user's tag
 */
NETWORKAPI void NETWORKCALL jf_network_setTagOfAcsocket(
    jf_network_acsocket_t * pAcsocket, void * pTag);

NETWORKAPI boolean_t NETWORKCALL jf_network_isAcsocketFree(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket);

NETWORKAPI u32 NETWORKCALL jf_network_connectAcsocketTo(
    jf_network_acsocket_t * pAcsocket, jf_ipaddr_t * pjiRemote, u16 u16RemotePort, void * pUser);

NETWORKAPI u32 NETWORKCALL jf_network_disconnectAcsocket(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket);

NETWORKAPI u32 NETWORKCALL jf_network_sendAcsocketData(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket,
    u8 * pu8Buffer, olsize_t sBuf);

NETWORKAPI void NETWORKCALL jf_network_getLocalInterfaceOfAcsocket(
    jf_network_acsocket_t * pAcsocket, jf_network_asocket_t * pAsocket, jf_ipaddr_t * pjiAddr);

/** resolve
 */

NETWORKAPI u32 NETWORKCALL jf_network_getHostByName(
    const olchar_t * pstrName, struct hostent ** ppHostent);

#endif /*JIUFENG_NETWORK_H */

/*------------------------------------------------------------------------------------------------*/

