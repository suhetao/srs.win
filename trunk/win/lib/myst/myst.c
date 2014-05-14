#include "st.h"
#include "unistd.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <errno.h>
#include "neterr.h"
#include "sys/uio.h"

int poll(struct pollfd *pds, int npds, st_utime_t timeout);

/* _st_GetError xlate winsock errors to unix */
int _st_GetError(int err)
{
	int syserr;

	if(err == 0) syserr=GetLastError();
	SetLastError(0);
	if(syserr < WSABASEERR) return(syserr);
	switch(syserr)
	{
	case WSAEINTR:   syserr=EINTR;
		break;
	case WSAEBADF:   syserr=EBADF;
		break;
	case WSAEACCES:  syserr=EACCES;
		break;
	case WSAEFAULT:  syserr=EFAULT;
		break;
	case WSAEINVAL:  syserr=EINVAL;
		break;
	case WSAEMFILE:  syserr=EMFILE;
		break;
	case WSAEWOULDBLOCK:  syserr=EAGAIN;
		break;
	case WSAEINPROGRESS:  syserr=EINTR;
		break;
	case WSAEALREADY:  syserr=EINTR;
		break;
	case WSAENOTSOCK:  syserr=ENOTSOCK;
		break;
	case WSAEDESTADDRREQ: syserr=EDESTADDRREQ;
		break;
	case WSAEMSGSIZE: syserr=EMSGSIZE;
		break;
	case WSAEPROTOTYPE: syserr=EPROTOTYPE;
		break;
	case WSAENOPROTOOPT: syserr=ENOPROTOOPT;
		break;
	case WSAEOPNOTSUPP: syserr=EOPNOTSUPP;
		break;
	case WSAEADDRINUSE: syserr=EADDRINUSE;
		break;
	case WSAEADDRNOTAVAIL: syserr=EADDRNOTAVAIL;
		break;
	case WSAECONNABORTED: syserr=ECONNABORTED;
		break;
	case WSAECONNRESET: syserr=ECONNRESET;
		break;
	case WSAEISCONN: syserr=EISCONN;
		break;
	case WSAENOTCONN: syserr=ENOTCONN;
		break;
	case WSAETIMEDOUT: syserr=ETIMEDOUT;
		break;
	case WSAECONNREFUSED: syserr=ECONNREFUSED;
		break;
	case WSAEHOSTUNREACH: syserr=EHOSTUNREACH;
		break;
	}
	return(syserr);
}


APIEXPORT int st_connect(_st_netfd_t *fd, const struct sockaddr *addr, int addrlen,
						 st_utime_t timeout)
{
	int n, err = 0;

	while (connect(fd->osfd, addr, addrlen) < 0) {
		if (errno != EINTR) {
			if (errno != EINPROGRESS && (errno != EADDRINUSE || err == 0))
				return -1;
			if (st_netfd_poll(fd, POLLOUT, timeout) < 0)
				return -1;
			n = sizeof(int);
			if (getsockopt(fd->osfd, SOL_SOCKET, SO_ERROR, (char *)&err,(socklen_t *)&n) < 0)
				return -1;
			if (err) {
				errno = err;
				return -1;
			}
			break;
		}
		err = 1;
	}
	return 0;
}



static st_netfd_t *_st_netfd_new(int osfd, int nonblock, int is_socket)
{
	_st_netfd_t *fd;
	int flags = 1;

	fd = calloc(1, sizeof(_st_netfd_t));
	if (!fd)
		return NULL;

	fd->osfd = osfd;

	if(is_socket == FALSE) return(fd);
	if(nonblock) ioctlsocket(fd->osfd, FIONBIO, &flags);

	return fd;
}


APIEXPORT _st_netfd_t *st_accept(_st_netfd_t *fd, struct sockaddr *addr, int *addrlen,
								 st_utime_t timeout)
{
	int osfd;
	_st_netfd_t *newfd;

	while ((osfd = accept(fd->osfd, addr, (socklen_t *)addrlen)) < 0)
	{
		errno=_st_GetError(0);
		if(errno == EINTR) continue;
		if(!_IO_NOT_READY_ERROR) return NULL;
		/* Wait until the socket becomes readable */
		if (st_netfd_poll(fd, POLLIN, timeout) < 0)
			return NULL;
	}
	newfd = _st_netfd_new(osfd, 1, 1);
	if(!newfd)
	{
		closesocket(osfd);
	}
	return newfd;
}



APIEXPORT _st_netfd_t* st_netfd_open_socket(int osfd)
{
	return _st_netfd_new(osfd, 1, 1);
}

int st_set_eventsys(int eventsys)
{
	return 0;
}

int st_usleep(st_utime_t usecs)
{
	usleep(usecs);
	return 0;
}

int st_netfd_fileno(_st_netfd_t *fd)
{
	return (fd->osfd);
}

APIEXPORT ssize_t st_read(_st_netfd_t* fd, void *buf, size_t nbyte, st_utime_t timeout)
{
	ssize_t n;

	while((n = recv(fd->osfd, buf, nbyte,0)) < 0)
	{
		errno=_st_GetError(0);
		if(errno == EINTR) continue;
		if(!_IO_NOT_READY_ERROR) return(-1);
		/* Wait until the socket becomes readable */
		if(st_netfd_poll(fd, POLLIN, timeout) < 0) return(-1);
	}
	return n;
}

APIEXPORT _st_netfd_t *st_netfd_open(int osfd)
{
	return _st_netfd_new(osfd, 1, 0);
}

int _st_active_count = 0;

APIEXPORT int st_init(void)
{
	static WSADATA wsadata;
	if (_st_active_count) {
		return 0;
	}

	Thread_Init();
	WSAStartup(2,&wsadata);

	return 0;
}

APIEXPORT ssize_t st_read_fully(_st_netfd_t *fd, void *buf, size_t nbyte,
								st_utime_t timeout)
{
	ssize_t n;
	size_t nleft = nbyte;

	while (nleft > 0) {
		if ((n = recv(fd->osfd, buf, nleft,0)) < 0) {
			errno=_st_GetError(0);
			if (errno == EINTR)
				continue;
			if (!_IO_NOT_READY_ERROR)
				return -1;
		} else {
			nleft -= n;
			if (nleft == 0 || n == 0)
				break;
			buf = (void *)((char *)buf + n);
		}
		/* Wait until the socket becomes readable */
		if (st_netfd_poll(fd, POLLIN, timeout) < 0)
			return -1;
	}

	return (ssize_t)(nbyte - nleft);
}



APIEXPORT ssize_t st_write(_st_netfd_t *fd, const void *buf, size_t nbyte,
						   st_utime_t timeout)
{
	ssize_t n;
	ssize_t nleft = nbyte;

	while (nleft > 0) {
		if ((n = send(fd->osfd, buf, nleft,0)) < 0) {
			errno=_st_GetError(0);
			if (errno == EINTR)
				continue;
			if (!_IO_NOT_READY_ERROR)
				return -1;
		} else {
			if (n == nleft)
				break;
			nleft -= n;
			buf = (const void *)((const char *)buf + n);
		}
		/* Wait until the socket becomes writable */
		if (st_netfd_poll(fd, POLLOUT, timeout) < 0)
			return -1;
	}

	return (ssize_t)nbyte;
}

#define _LOCAL_MAXIOV  16

static ssize_t _st_writev(int fd, struct iovec *iov, unsigned int iov_cnt)
{
	int             i;
	size_t          total;
	void*           pv = NULL;
	ssize_t  ret;

	for(i = 0, total = 0; i < iov_cnt; ++i)
	{
		total += iov[i].iov_len;
	}
	pv = calloc(1, total);
	if(NULL == pv){
		errno = Errno_From_Win32(GetLastError());
		ret = -1;
	}
	else
	{
		for(i = 0, ret = 0; i < iov_cnt; ++i){
			(void)memcpy((char*)pv + ret, iov[i].iov_base, iov[i].iov_len);
			ret += (ssize_t)iov[i].iov_len;
		}

		if(!send(fd, pv,total,0)){
			errno = Errno_From_Win32(GetLastError());
			ret = -1;
		}
		else{
			ret = total;
		}
		(void)free(pv);
	}

	return ret;
}

APIEXPORT ssize_t st_writev(st_netfd_t fd, const struct iovec *iov, int iov_size,
							st_utime_t timeout)
{
	ssize_t n, rv;
	size_t nleft, nbyte;
	int index, iov_cnt;
	struct iovec *tmp_iov;
	struct iovec local_iov[_LOCAL_MAXIOV];

	/* Calculate the total number of bytes to be sent */
	nbyte = 0;
	for (index = 0; index < iov_size; index++)
		nbyte += iov[index].iov_len;

	rv = (ssize_t)nbyte;
	nleft = nbyte;
	tmp_iov = (struct iovec *) iov;	/* we promise not to modify iov */
	iov_cnt = iov_size;

	while (nleft > 0) {
		if (iov_cnt == 1) {
			if (st_write(fd, tmp_iov[0].iov_base, nleft, timeout) != (ssize_t) nleft)
				rv = -1;
			break;
		}
		if ((n = _st_writev(fd->osfd, tmp_iov, iov_cnt)) < 0) {
			if (errno == EINTR)
				continue;
			if (!_IO_NOT_READY_ERROR) {
				rv = -1;
				break;
			}
		} else {
			if ((size_t) n == nleft)
				break;
			nleft -= n;
			/* Find the next unwritten vector */
			n = (ssize_t)(nbyte - nleft);
			for (index = 0; (size_t) n >= iov[index].iov_len; index++)
				n -= iov[index].iov_len;

			if (tmp_iov == iov) {
				/* Must copy iov's around */
				if (iov_size - index <= _LOCAL_MAXIOV) {
					tmp_iov = local_iov;
				} else {
					tmp_iov = calloc(1, (iov_size - index) * sizeof(struct iovec));
					if (tmp_iov == NULL)
						return -1;
				}
			}

			/* Fill in the first partial read */
			tmp_iov[0].iov_base = &(((char *)iov[index].iov_base)[n]);
			tmp_iov[0].iov_len = iov[index].iov_len - n;
			index++;
			/* Copy the remaining vectors */
			for (iov_cnt = 1; index < iov_size; iov_cnt++, index++) {
				tmp_iov[iov_cnt].iov_base = iov[index].iov_base;
				tmp_iov[iov_cnt].iov_len = iov[index].iov_len;
			}
		}
		/* Wait until the socket becomes writable */
		if (st_netfd_poll(fd, POLLOUT, timeout) < 0) {
			rv = -1;
			break;
		}
	}

	if (tmp_iov != iov && tmp_iov != local_iov)
		free(tmp_iov);

	return rv;
}

APIEXPORT st_thread_t st_thread_create(void *(*start)(void *arg), void *arg,
									   int joinable, int stk_size)
{
	st_thread_t thread;
	thread = calloc(1, sizeof(st_thread_t));
	if (NULL == thread){
		return NULL;
	}
	Thread_Create(thread, NULL, start,arg);
	return thread;
}

st_thread_t st_thread_self(void)
{
	return GetCurrentThread();
}



APIEXPORT void st_netfd_free(_st_netfd_t *fd)
{

}

APIEXPORT int st_netfd_close(_st_netfd_t *fd)
{
	st_netfd_free(fd);
	closesocket(fd->osfd);
	fd->osfd = INVALID_SOCKET;
	free(fd);
	errno=_st_GetError(0);
	return(errno);
}


APIEXPORT int st_thread_join(st_thread_t thread, void **retvalp)
{
	Thread_Join(*thread,retvalp);
	if (retvalp)
		*retvalp = thread->ppRet;
	return 0;
}



APIEXPORT void st_thread_interrupt(st_thread_t thread)
{
	//can't implemente yet,because there isn't any thread loop event below win32 system
	//so only use TerminateThread function instead
	TerminateThread(thread->hHandle,0);
}

APIEXPORT void st_thread_exit(void *retval)
{
	TerminateThread(GetCurrentThread(),0);
}

int poll(struct pollfd *pds, int npds, st_utime_t timeout)
{	
	struct pollfd *pd;
	struct pollfd *epd = pds + npds;

	fd_set readfds, writefds, errorfds;
	struct timeval *tp;
	struct timeval timeout_t;
	int maxfd = 0;
	int ret = 0;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&errorfds);

	for (pd = pds; pd < epd; pd++) {
		if (pd->events & POLLIN) {
			FD_SET(pd->fd, &readfds);
		}
		if (pd->events & POLLOUT) {
			FD_SET(pd->fd, &writefds);
		}
		if (pd->events & POLLPRI) {
			FD_SET(pd->fd, &errorfds);
		}

	}
	timeout_t.tv_sec = timeout / 1000;
	timeout_t.tv_usec= (timeout % 1000) * 1000;
	tp= &timeout_t;
	if (timeout == -1)
	{
		tp = NULL;
	}
	ret = select(maxfd + 1, &readfds, &writefds, &errorfds, tp);
	if (ret > 0)
	{
		for (pd = pds; pd < epd; pd++) {
			if(FD_ISSET(pd->fd, &readfds)){
				pd->revents |= POLLIN;
			}
			else if(FD_ISSET(pd->fd, &readfds)){
				pd->revents |= POLLOUT;
			}
			else if(FD_ISSET(pd->fd, &errorfds)){
				pd->revents |= POLLPRI;
			}
		}
	}
	return ret;
}

APIEXPORT int st_netfd_poll(_st_netfd_t *fd, int how, st_utime_t timeout)
{
	struct pollfd pd;
	int n;

	pd.fd = fd->osfd;
	pd.events = (short) how;
	pd.revents = 0;

	if ((n = poll(&pd, 1, timeout)) < 0)
		return -1;
	if (n == 0) {
		/* Timed out */
		errno = ETIME;
		return -1;
	}

	if(pd.revents == 0) {
		errno = EBADF;
		return -1;
	}

	return 0;
}
