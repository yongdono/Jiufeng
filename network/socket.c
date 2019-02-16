/**
 *  @file socket.c
 *
 *  @brief routines for socket 
 *
 *  @Author Min Zhang
 *
 *  @note
 *
 */

/* --- standard C lib header files ----------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(LINUX)
    #include <sys/param.h>
    #include <sys/socket.h>
    #include <sys/file.h>
    #include <sys/time.h>
    #include <sys/times.h>
    #include <sys/signal.h>

    #include <netinet/in.h>
    #include <netinet/ip.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#elif defined(WINDOWS)
    #include <Winsock2.h>
    #include <Ws2tcpip.h>
#endif

/* --- internal header files ----------------------------------------------- */
#include "olbasic.h"
#include "errcode.h"
#include "network.h"
#include "xmalloc.h"
#include "internalsocket.h"

/* --- private data/data structure section --------------------------------- */


/* --- private routine section---------------------------------------------- */


/* --- public routine section ---------------------------------------------- */

u32 createDgramSocket(ip_addr_t * piaLocal, u16 * pu16Port,
    socket_t ** ppSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;

    assert((piaLocal != NULL) && (pu16Port != NULL));
    assert(ppSocket != NULL);

    u32Ret = createDgramIsocket(piaLocal, pu16Port,
        (internal_socket_t **)ppSocket);

    return u32Ret;
}

u32 createStreamSocket(
    ip_addr_t * piaLocal, u16 * pu16Port, socket_t ** ppSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;

    assert((piaLocal != NULL) && (pu16Port != NULL) && (ppSocket != NULL));

    u32Ret = createStreamIsocket(piaLocal, pu16Port,
        (internal_socket_t **)ppSocket);

    return u32Ret;
}

u32 createTypeStreamSocket(u8 u8AddrType, socket_t ** ppSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;

    assert(ppSocket != NULL);

    if (u8AddrType == IP_ADDR_TYPE_V6)
        u32Ret = createIsocket(
            AF_INET6, SOCK_STREAM, 0, (internal_socket_t **)ppSocket);
    else
        u32Ret = createIsocket(
            AF_INET, SOCK_STREAM, 0, (internal_socket_t **)ppSocket);

    return u32Ret;
}

u32 createSocket(
    olint_t domain, olint_t type, olint_t protocol, socket_t ** ppSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;

    assert(ppSocket != NULL);

    u32Ret = createIsocket(
        domain, type, protocol, (internal_socket_t **)ppSocket);

    return u32Ret;
}

u32 destroySocket(socket_t ** ppSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;

    assert((ppSocket != NULL) && (*ppSocket != NULL));

    destroyIsocket((internal_socket_t **)ppSocket);

    return u32Ret;
}

u32 ioctlSocket(socket_t * pSocket, olint_t req, void * pArg)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = ioctlIsocket(pis, req, pArg);

    return u32Ret;
}

u32 setSocketBlock(socket_t * pSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = setIsocketBlock(pis);

    return u32Ret;
}

u32 setSocketNonblock(socket_t * pSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = setIsocketNonblock(pis);

    return u32Ret;
}

u32 joinMulticastGroup(
    socket_t * pSocket, ip_addr_t * piaAddr, ip_addr_t * piaMulticaseAddr)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);
    assert((piaAddr != NULL) && (piaMulticaseAddr != NULL));

    u32Ret = isJoinMulticastGroup(pis, piaAddr, piaMulticaseAddr);

    return u32Ret;
}

u32 enableBroadcast(socket_t * pSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isEnableBroadcast(pis);

    return u32Ret;
}

/** Try to send all data but only send once, the actual sent size is in psSend
 */
u32 sSend(socket_t * pSocket, void * pBuffer, olsize_t * psSend)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isSend(pis, pBuffer, psSend);

    return u32Ret;
}

/** Try to send all data but only send once, unless timeout the actual sent size
 *  is in psSend
 */
u32 sSendWithTimeout(
    socket_t * pSocket, void * pBuffer, olsize_t * psSend, u32 u32Timeout)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isSendWithTimeout(pis, pBuffer, psSend, u32Timeout);

    return u32Ret;
}

/** Try to send all data with possible several round, until an error occurs, the
 *  actual sent size is in psSend
 */
u32 sSendn(socket_t * pSocket, void * pBuffer, olsize_t * psSend)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isSendn(pis, pBuffer, psSend);

    return u32Ret;
}

/** Try to send all data with possible several round, until an error occurs or
 *  timeout, the actual sent size is in psSend
 */
u32 sSendnWithTimeout(socket_t * pSocket, void * pBuffer, olsize_t * psSend,
    u32 u32Timeout)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isSendnWithTimeout(pis, pBuffer, psSend, u32Timeout);

    return u32Ret;
}

/** Try to recveive all data but only recveive once, the actual received size is
 *  in psRecv
 */
u32 sRecv(socket_t * pSocket, void * pBuffer, olsize_t * psRecv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isRecv(pis, pBuffer, psRecv);

    return u32Ret;
}

/** Try to recveive all data but only recveive once, unless timeout the actual
 *  received size is in psRecv
 */
u32 sRecvWithTimeout(
    socket_t * pSocket, void * pBuffer, olsize_t * psRecv, u32 u32Timeout)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isRecvWithTimeout(pis, pBuffer, psRecv, u32Timeout);

    return u32Ret;
}

/** Try to recveive all data but only recveive once, unless timeout the actual
 *  received size is in psRecv
 */
u32 sRecvfromWithTimeout(
    socket_t * pSocket, void * pBuffer, olsize_t * psRecv,
    u32 u32Timeout, ip_addr_t * piaFrom, u16 * pu16Port)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isRecvfromWithTimeout(
        pis, pBuffer, psRecv, u32Timeout, piaFrom, pu16Port);

    return u32Ret;
}

/** Try to recveive all data with possible several round, until an error occurs,
 *  the actual recveived size is in psRecv
 */
u32 sRecvn(socket_t * pSocket, void * pBuffer, olsize_t * psRecv)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isRecvn(pis, pBuffer, psRecv);

    return u32Ret;
}

/** Try to recveive all data with possible several round, until an error occurs
 *  or timeout, the actual recveived size is in psRecv
 */
u32 sRecvnWithTimeout(
    socket_t * pSocket, void * pBuffer, olsize_t * psRecv, u32 u32Timeout)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isRecvnWithTimeout(pis, pBuffer, psRecv, u32Timeout);

    return u32Ret;
}

u32 sListen(socket_t * pListen, olint_t backlog)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pListen;

    assert(pListen != NULL);

    u32Ret = isListen(pis, backlog);

    return u32Ret;
}

u32 sConnect(socket_t * pSocket, const ip_addr_t * pia, u16 u16Port)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isConnect(pis, pia, u16Port);

    return u32Ret;
}

u32 sConnectWithTimeout(
    socket_t * pSocket, const ip_addr_t * pia, u16 u16Port, u32 u32Timeout)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isConnectWithTimeout(pis, pia, u16Port, u32Timeout);

    return u32Ret;
}

u32 sAccept(
    socket_t * pListen, ip_addr_t * pia, u16 * pu16Port, socket_t ** ppSocket)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pListen;

    assert((pListen != NULL) && (ppSocket != NULL));

    u32Ret = isAccept(pis, pia, pu16Port, (internal_socket_t **)ppSocket);

    return u32Ret;
}

u32 sSendto(
    socket_t * pSocket, void * pBuffer, olsize_t * psSend,
    const ip_addr_t * piaTo, u16 u16Port)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isSendto(pis, pBuffer, psSend, piaTo, u16Port);

    return u32Ret;
}

u32 sRecvfrom(
    socket_t * pSocket, void * pBuffer, olsize_t * psRecv,
    ip_addr_t * piaFrom, u16 * pu16Port)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = isRecvfrom(pis, pBuffer, psRecv, piaFrom, pu16Port);

    return u32Ret;
}

u32 sSelect(
    fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
    struct timeval * timeout, u32 * pu32Ready)
{
    u32 u32Ret = OLERR_NO_ERROR;

    u32Ret = isSelect(readfds, writefds, exceptfds, timeout, pu32Ready);

    return u32Ret;
}

u32 getSocketName(
    socket_t * pSocket, struct sockaddr * pName, olint_t * pnNameLen)
{
    u32 u32Ret = OLERR_NO_ERROR;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    u32Ret = getIsocketName(pis, pName, pnNameLen);

    return u32Ret;
}

void clearSocketFromFdSet(socket_t * pSocket, fd_set * set)
{
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    clearIsocketFromFdSet(pis, set);
}

boolean_t isSocketSetInFdSet(socket_t * pSocket, fd_set * set)
{
    boolean_t bRet = FALSE;
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    bRet = isIsocketSetInFdSet(pis, set);

    return bRet;
}

void setSocketToFdSet(socket_t * pSocket, fd_set * set)
{
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    setIsocketToFdSet(pis, set);
}

u32 getSocketOpt(
    socket_t * pSocket, olint_t level, olint_t optname,
    void * optval, olsize_t * optlen)
{
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    return isGetSockOpt(pis, level, optname, optval, optlen);
}

u32 setSocketOpt(
    socket_t * pSocket, olint_t level, olint_t optname,
    void * optval, olsize_t optlen)
{
    internal_socket_t * pis = (internal_socket_t *)pSocket;

    assert(pSocket != NULL);

    return isSetSockOpt(pis, level, optname, optval, optlen);
}

/*--------------------------------------------------------------------------*/
