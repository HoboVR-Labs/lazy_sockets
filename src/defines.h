// SPDX-License-Identifier: GPL-2.0-only

// Copyright (C) 2020-2021 Oleg Vorobiov <oleg.vorobiov@hobovrlabs.org>

#ifndef __LAZY_SOCKETS_DEFS
#define __LAZY_SOCKETS_DEFS

#define __LAZY_SOCKETS_VERSION "0.2.3"
#define __LAZY_SOCKETS_BUILD 1644444000  // build date 2022 02 10


// platform defined types
#ifdef LINUX

#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#include <arpa/inet.h>

namespace lsc {

// pass through for the socket() call
#define lsocket socket

#define LSOCK_ERR -1

#define lerrno errno

typedef struct sockaddr_in lcsockaddr_in;

typedef int lsocket_t;

}  // namespace lsc

#elif defined(WIN)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

namespace lsc {

using socklen_t = int;

// cuz on windows you can't have nice things
#define MSG_DONTWAIT 0
#define LSOCK_ERR INVALID_SOCKET

#define lerrno WSAGetLastError()

typedef struct sockaddr_in lcsockaddr_in;

typedef SOCKET lsocket_t;

static bool __wsa_initialized = false;
static unsigned int  __wsa_initialized_count = 0;

// cuz on windows sockets require wsaStartup, call WSACleanup() yourself later, idc
inline lsocket_t lsocket(int family, int type, int protocol) {
	if (!__wsa_initialized) {
		WSADATA wsaData;
		int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (res != NO_ERROR) {
			// errno = res;
			return INVALID_SOCKET;
		}
		__wsa_initialized = true;
	}
	__wsa_initialized_count++;
	return socket(family, type, protocol);
}

// cuz on windows you can't have accept with flags
inline lsocket_t accept4(lsocket_t soc, struct sockaddr* addr, socklen_t* addrlen, int flags) {
	(void)flags;
	return accept(soc, addr, addrlen);
}

// cuz on windows close() doesn't exist
inline int close(lsocket_t soc) {
	int res = closesocket(soc); // cache close result

	// de init WSA on last socket close
	if (!(--__wsa_initialized_count)) {
		WSACleanup();
		__wsa_initialized = false;
	}

	return res;
}


// cuz windows wants to be special
#define ioctl ioctlsocket


}  // namespace lsc

#else

#error "unsupported platform"

#endif // LINUX/WIN check

#endif // #ifndef __LAZY_SOCKETS_DEFS