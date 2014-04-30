#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef WIN32

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "stdbool.h"

#include <io.h>
#define lseek _lseek
#define close _close
//#define open _open

typedef intptr_t ssize_t;
typedef unsigned int u_int32_t;
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;

#if !defined(snprintf)
#define snprintf  _snprintf
#define PRId8     "hhd"
#define PRId16    "hd"
#define PRId32    "ld"
#define PRId64    "lld"
#endif

#define	CleanupSockets() WSACleanup()


void InitSockets(void);

typedef struct iovec {
	void *iov_base;
	size_t iov_len;
}iovec;

#define IOV_MAX 1024
#ifndef ST_UTIME_NO_TIMEOUT
#define ST_UTIME_NO_TIMEOUT ((long long)-1ULL)
#endif

int gettimeofday(struct timeval *tp, void *tzp);
static ssize_t readv_writev(int fd, struct iovec *iov, unsigned iov_cnt, bool do_write);
ssize_t readv(int fd, struct iovec *iov, unsigned iov_cnt);
ssize_t writev(int fd, struct iovec *iov, unsigned iov_cnt);

__int64 ntoh64(__int64 val );
__int64 hton64(__int64 val );

#endif
#ifdef __cplusplus
}
#endif
#endif
