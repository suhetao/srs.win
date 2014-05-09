
#ifndef __ST_THREAD_H__
#define __ST_THREAD_H__

#include <time.h>
#include <sys/types.h>
#include <stddef.h>
#include <neterr.h>
#include "Thread.h"

#ifndef ETIME
#define ETIME ETIMEDOUT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
typedef int ssize_t;
typedef int mode_t;
typedef int pid_t;
#define APIEXPORT
#else
#define APIEXPORT
#endif

#if EAGAIN != EWOULDBLOCK
#define _IO_NOT_READY_ERROR  ((errno == EAGAIN) || (errno == EWOULDBLOCK))
#else
#define _IO_NOT_READY_ERROR  (errno == EAGAIN)
#endif

#define ST_EVENTSYS_ALT 4

typedef struct _st_netfd {
	int osfd;                   /* Underlying OS file descriptor */
} _st_netfd_t;

typedef unsigned long long  st_utime_t;
typedef ThreadHandle _st_thread_t;
typedef _st_thread_t* st_thread_t;
typedef ThreadCondition *   st_cond_t;
typedef struct _st_netfd *  st_netfd_t;

APIEXPORT int st_init(void);
APIEXPORT int st_set_eventsys(int eventsys);
APIEXPORT st_thread_t st_thread_self(void);
APIEXPORT void st_thread_exit(void *retval);
APIEXPORT int st_thread_join(st_thread_t thread, void **retvalp);
APIEXPORT void st_thread_interrupt(st_thread_t thread);
APIEXPORT st_thread_t st_thread_create(void *(*start)(void *arg), void *arg,
                                    int joinable, int stack_size);
APIEXPORT int st_usleep(st_utime_t usecs);
APIEXPORT st_netfd_t st_netfd_open(int osfd);
APIEXPORT st_netfd_t st_netfd_open_socket(int osfd);
APIEXPORT void st_netfd_free(st_netfd_t fd);
APIEXPORT int st_netfd_close(st_netfd_t fd);
APIEXPORT int st_netfd_fileno(st_netfd_t fd);
APIEXPORT int st_netfd_poll(st_netfd_t fd, int how, st_utime_t timeout);

APIEXPORT int st_poll(struct pollfd *pds, int npds, st_utime_t timeout);
APIEXPORT st_netfd_t st_accept(st_netfd_t fd, struct sockaddr *addr, int *addrlen,
                            st_utime_t timeout);
APIEXPORT int st_connect(st_netfd_t fd, const struct sockaddr *addr, int addrlen,
                      st_utime_t timeout);
APIEXPORT ssize_t st_read(st_netfd_t fd, void *buf, size_t nbyte,
                       st_utime_t timeout);
APIEXPORT ssize_t st_read_fully(st_netfd_t fd, void *buf, size_t nbyte,
                             st_utime_t timeout);
APIEXPORT ssize_t st_write(st_netfd_t fd, const void *buf, size_t nbyte,
                        st_utime_t timeout);
APIEXPORT ssize_t st_writev(st_netfd_t fd, const struct iovec *iov, int iov_size,
                         st_utime_t timeout);

#ifdef __cplusplus
}
#endif

#endif
