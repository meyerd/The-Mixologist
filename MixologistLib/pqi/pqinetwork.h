/****************************************************************
 *  Copyright 2010, Fair Use, Inc.
 *  Copyright 2004-6, Robert Fernie
 *
 *  This file is part of the Mixologist.
 *
 *  The Mixologist is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  The Mixologist is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Mixologist; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

#ifndef MRK_PQI_NETWORKING_HEADER
#define MRK_PQI_NETWORKING_HEADER


/********************************** WINDOWS/UNIX SPECIFIC PART ******************/
#ifndef WINDOWS_SYS

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/poll.h>

//socket blocking/options.
#include <fcntl.h>
#include <inttypes.h>

#include "util/net.h" /* more generic networking header */

#else

/* This defines the platform to be WinXP or later...
 * and is needed for getaddrinfo.... (not used anymore)
 *
 */

/* This is needed to silence warnings from windef.h in mingw, must be before util/net.h include. */
//#ifdef _WIN32_WINNT
//#undef _WIN32_WINNT
//#endif
#define _WIN32_WINNT 0x0501

#include "util/net.h" /* more generic networking header */

#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
//typedef unsigned long in_addr_t;

// Some Network functions that are missing from windows.

in_addr_t inet_netof(struct in_addr addr);
in_addr_t inet_network(const char *inet_name);
int inet_aton(const char *name, struct in_addr *addr);

extern int errno; /* Define extern errno, to duplicate unix behaviour */

/* define the Unix Error Codes that we use...
 * NB. we should make the same, but not necessary
 */
#define EAGAIN          11
#define EWOULDBLOCK     EAGAIN

#define EUSERS          87
#define ENOTSOCK        88

#define EOPNOTSUPP      95

#define EADDRINUSE      98
#define EADDRNOTAVAIL   99
#define ENETDOWN        100
#define ENETUNREACH     101

#define ECONNRESET      104

#define ETIMEDOUT       110
#define ECONNREFUSED    111
#define EHOSTDOWN       112
#define EHOSTUNREACH    113
#define EALREADY        114
#define EINPROGRESS     115

#endif
/********************************** WINDOWS/UNIX SPECIFIC PART ******************/

#include <iostream>
#include <string>
#include <list>
#include <QString>

// Same def - different functions...

std::ostream &showSocketError(std::ostream &out);

std::string socket_errorType(int err);
int sockaddr_cmp(struct sockaddr_in &addr1, struct sockaddr_in &addr2 );
int inaddr_cmp(struct sockaddr_in addr1, struct sockaddr_in addr2 );
int inaddr_cmp(struct sockaddr_in addr1, unsigned long);

/* Picks and returns the best address out of local interfaces.
   Steps through the interfaces, and returns the first external address we come across.
   If there is no external address, return the first private address we come across.
   If there is no private address, returns loopback address.
   In the case of Windows, if there is an all 0 address, returns it instead of loopback. */
struct in_addr getPreferredInterface();

bool interfaceStillExists(struct in_addr* currentInterface);

/* Returns all local interfaces. */
std::list<std::string> getLocalInterfaces();

/* True if the two addresses are on the same subnet, i.e. (addr1 & 255.255.255.0) == (addr2 & 255.255.255.0) */
bool isSameSubnet(struct in_addr *addr1, struct in_addr *addr2);
/* True if the two addresses are the same, both in IP and port. */
bool isSameAddress(struct sockaddr_in *addr, struct sockaddr_in *addr2);

in_addr_t pqi_inet_netof(struct in_addr addr); // our implementation.

bool LookupDNSAddr(std::string name, struct sockaddr_in *addr);

/* Utility function for usedSockets to convert a sockaddr_in into a string representation of form ###.###.###.###:## */
QString addressToString(const struct sockaddr_in *addr);

/* universal socket interface */

int unix_close(int sockfd);
int unix_socket(int domain, int type, int protocol);
int unix_fcntl_nonblock(int sockfd);
int unix_connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
int unix_getsockopt_error(int sockfd, int *err);

#ifdef WINDOWS_SYS
int     WinToUnixError(int error);
#endif //WINDOWS_SYS

#endif //MRK_PQI_NETWORKING_HEADER

