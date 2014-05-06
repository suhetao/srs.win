#ifndef SRS_AUTO_HEADER_HPP
#define SRS_AUTO_HEADER_HPP

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <time.h>
#include <inttypes.h>

#if !defined(snprintf)
#define snprintf  _snprintf
#define PRId8     "hhd"
#define PRId16    "hd"
#define PRId32    "ld"
#define PRId64    "lld"
#endif

#include <io.h>
#define lseek _lseek
#define close _close
#define open _open
#define read _read
#define write _write
#include <process.h>
#define getpid _getpid

#include <direct.h>
#define getcwd _getcwd

typedef struct timeval timeval;
int gettimeofday(struct timeval *tp, void *tzp);

#define	_SC_NPROCESSORS_CONF		57
#define	_SC_NPROCESSORS_ONLN		58

long sysconf(int name);

#ifndef ST_UTIME_NO_TIMEOUT
#define ST_UTIME_NO_TIMEOUT -1
#endif

#ifndef pid_t
typedef int pid_t;
#endif
pid_t fork(void);

typedef void (__cdecl *sig_handler_t)(int);
struct sigaction {
	sig_handler_t sa_handler;
	unsigned sa_flags;
};
#define sigemptyset(x) (void)0
#define SA_RESTART 0

#define S_IFLNK    0120000 /* Symbolic link */
#define S_ISLNK(x) (((x) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(x) 0
#define S_IRGRP 0
#define S_IWGRP 0
#define S_IXGRP 0
#define S_ISGID 0
#define S_IROTH 0
#define S_IXOTH 0

#define SIGHUP 1
#define SIGQUIT 3
#define SIGKILL 9
#define SIGPIPE 13
#define SIGALRM 14
#define SIGCHLD 17

#define SIGUSR2	300

#define F_GETFD 1
#define F_SETFD 2
#define FD_CLOEXEC 0x1

int fcntl(int fildes, int cmd, ...);

typedef struct iovec {
	void *iov_base;
	size_t iov_len;
}iovec;
ssize_t readv(int fd, const struct iovec *iov, unsigned int iov_cnt);
ssize_t writev(int fd, const struct iovec *iov, unsigned int iov_cnt);

void usleep(long usec);
void initsocket(void);

#ifndef WSAETIMEDOUT
#define WSAETIMEDOUT            (WSABASEERR+60)
#endif
#ifndef WSAECONNRESET
#define WSAECONNRESET           (WSABASEERR+54)
#endif
#ifndef ETIME
#define ETIME WSAETIMEDOUT
#endif
#ifndef ECONNRESET
#define ECONNRESET WSAECONNRESET
#endif 

#endif

#define SRS_AUTO_BUILD_DATE "2014-05-04 20:23:56"
#define SRS_AUTO_USER_CONFIGURE
#define SRS_AUTO_CONFIGURE
#define SRS_AUTO_UNAME "modified by hetao.su"
//#define SRS_AUTO_HTTP_SERVER
//#define SRS_AUTO_HTTP_API
//#define SRS_AUTO_HTTP_PARSER
//#define SRS_AUTO_HTTP_CALLBACK

#endif