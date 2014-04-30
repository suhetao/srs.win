/*
The MIT License (MIT)

Copyright (c) 2013 winlin

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <srs_core_socket.hpp>

#include <srs_core_error.hpp>
#ifdef WIN32
#include <errno.h>

#define SRS_SEND_TIMEOUT_US 5000000L
#define SRS_RECV_TIMEOUT_US SRS_SEND_TIMEOUT_US

SrsSocket::SrsSocket(SOCKET client)
{
    sock = client;
	send_timeout = SRS_SEND_TIMEOUT_US;
	recv_timeout = SRS_RECV_TIMEOUT_US;
#else
SrsSocket::SrsSocket(SOCKET client_stfd)
{
	stfd = client_stfd;
	send_timeout = recv_timeout = ST_UTIME_NO_TIMEOUT;
#endif
	recv_bytes = send_bytes = 0;
	start_time_ms = srs_get_system_time_ms();
}

SrsSocket::~SrsSocket()
{
}

void SrsSocket::set_recv_timeout(int64_t timeout_us)
{
	recv_timeout = timeout_us;

}

int64_t SrsSocket::get_recv_timeout()
{
	return recv_timeout;
}

void SrsSocket::set_send_timeout(int64_t timeout_us)
{
	send_timeout = timeout_us;
}

int64_t SrsSocket::get_recv_bytes()
{
	return recv_bytes;
}

int64_t SrsSocket::get_send_bytes()
{
	return send_bytes;
}

int SrsSocket::get_recv_kbps()
{
	int64_t diff_ms = srs_get_system_time_ms() - start_time_ms;
	
	if (diff_ms <= 0) {
		return 0;
	}
	
	return recv_bytes * 8 / diff_ms;
}

int SrsSocket::get_send_kbps()
{
	int64_t diff_ms = srs_get_system_time_ms() - start_time_ms;
	
	if (diff_ms <= 0) {
		return 0;
	}
	
	return send_bytes * 8 / diff_ms;
}


#ifndef WIN32
int SrsSocket::read(const void* buf, size_t size, ssize_t* nread)
{
	int ret = ERROR_SUCCESS;

    *nread = st_read(stfd, (void*)buf, size, recv_timeout);
    
    // On success a non-negative integer indicating the number of bytes actually read is returned 
    // (a value of 0 means the network connection is closed or end of file is reached).
    if (*nread <= 0) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        if (*nread == 0) {
            errno = ECONNRESET;
        }
        
        return ERROR_SOCKET_READ;
    }
#else
int SrsSocket::read(void* buf, size_t size, ssize_t* nread)
{
	int ret = ERROR_SUCCESS;

	fd_set fd;
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = recv_timeout;

	FD_ZERO(&fd);
	FD_SET(sock,&fd);

	if(select(FD_SETSIZE, &fd, NULL, NULL, &tv) == 0)
		return ERROR_SOCKET_TIMEOUT;

	*nread = recv(sock,(char*)buf,size,0);
	if (*nread <= 0) {
		if (*nread == 0) {
			errno = WSAECONNRESET;
		}
		return ERROR_SOCKET_READ;
	}
#endif
    
    recv_bytes += *nread;
        
    return ret;
}


#ifndef WIN32
int SrsSocket::read_fully(const void* buf, size_t size, ssize_t* nread)
{
	int ret = ERROR_SUCCESS;

    *nread = st_read_fully(stfd, (void*)buf, size, recv_timeout);
    
    // On success a non-negative integer indicating the number of bytes actually read is returned 
    // (a value less than nbyte means the network connection is closed or end of file is reached)
    if (*nread != (ssize_t)size) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        if (*nread >= 0) {
            errno = ECONNRESET;
        }
        
        return ERROR_SOCKET_READ_FULLY;
    }
#else
int SrsSocket::read_fully(void* buf, size_t size, ssize_t* nread)
{
	int ret = ERROR_SUCCESS;

	int len ,nsize = 0,left = size;
	char *pbuf = (char*)buf;

	fd_set fd;
	struct timeval tv;

	do
	{
		tv.tv_sec = 0;
		tv.tv_usec = recv_timeout;

		FD_ZERO(&fd);
		FD_SET(sock,&fd);

		if(select(FD_SETSIZE, &fd, NULL, NULL, &tv) == 0)
		{
			ret = WSAETIMEDOUT;
			break;
		}

		len = recv(sock,pbuf,left,0);
		if (len == 0 || len == -1)
			break;
		if (len > 0)
		{
			left -= len;
			nsize += len;
			pbuf += len;
		}
	}while (left > 0);
	if (nsize != size)
	{
		if (WSAETIMEDOUT == ret)
		{
			return ERROR_SOCKET_TIMEOUT;
		}
		if (*nread >= 0) {
			errno = WSAECONNRESET;
		}
		return ERROR_SOCKET_READ_FULLY;
	}
	*nread = nsize;
#endif
    recv_bytes += *nread;
    
    return ret;
}

#ifndef WIN32
int SrsSocket::write(const void* buf, size_t size, ssize_t* nwrite)
{
	int ret = ERROR_SUCCESS;
    *nwrite = st_write(stfd, (void*)buf, size, send_timeout);
    
    if (*nwrite <= 0) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        return ERROR_SOCKET_WRITE;
    }
#else
int SrsSocket::write(void* buf, size_t size, ssize_t* nwrite)
{
	int ret = ERROR_SUCCESS;

	int len, nsize = 0 ,left = size;
	char *pbuf = (char*)buf;
	
	fd_set fd;
	struct timeval tv;

	while(left > 0)
	{
		tv.tv_sec = 0;
		tv.tv_usec = send_timeout;

		FD_ZERO(&fd);
		FD_SET(sock,&fd);

		if(select(FD_SETSIZE, NULL, &fd, NULL, &tv) == 0)
		{
			ret = WSAETIMEDOUT;
			break;
		}

		len = send(sock,(char*)buf,size,0);
		if (len == 0 || len == -1)
			break;
		if (len > 0)
		{
			left -= len;
			nsize += len;
			pbuf += len;
		}
	}
	if (nsize != size)
	{
		if (WSAETIMEDOUT == ret)
		{
			return ERROR_SOCKET_TIMEOUT;
		}
		return ERROR_SOCKET_WRITE;
	}
	*nwrite = nsize;
#endif

    send_bytes += *nwrite;
        
    return ret;
}

#ifndef WIN32
int SrsSocket::writev(const iovec *iov, int iov_size, ssize_t* nwrite)
{
    int ret = ERROR_SUCCESS;
    *nwrite = st_writev(stfd, iov, iov_size, send_timeout);
    
    if (*nwrite <= 0) {
		if (errno == ETIME) {
			return ERROR_SOCKET_TIMEOUT;
		}
		
        return ERROR_SOCKET_WRITE;
    }
#else
int SrsSocket::writev(iovec *iov, int iov_size, ssize_t* nwrite)
{
	int ret = ERROR_SUCCESS;
	int i = 0;
	int len = 0, nsize = 0 , totalsize = 0;
	fd_set fd;
	struct timeval tv;

	while (i < iov_size)
	{
		totalsize += iov[i].iov_len;

		tv.tv_sec = 0;
		tv.tv_usec = send_timeout;

		FD_ZERO(&fd);
		FD_SET(sock,&fd);

		if(select(FD_SETSIZE, NULL, &fd, NULL, &tv) == 0)
		{
			ret = WSAETIMEDOUT;
			break;
		}
		len = send(sock, (char*)iov[i].iov_base, iov[i].iov_len, 0);
		if (0 == len || -1 == len)
			break;
		if (len > 0)
			nsize += len;
		i++;
	}

	if (nsize != totalsize)
	{
		if (WSAETIMEDOUT == ret)
		{
			return ERROR_SOCKET_TIMEOUT;
		}
		return ERROR_SOCKET_WRITE;
	}
	*nwrite = totalsize;
#endif

    send_bytes += *nwrite;
    
    return ret;
}

