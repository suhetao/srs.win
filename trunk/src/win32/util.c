#ifdef WIN32

#include <errno.h>
#include <time.h>
#include "util.h"

void InitSockets(void)
{
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(2,2);
	WSAStartup(version, &wsaData);
}

static ssize_t readv_writev(int fd, struct iovec *iov, unsigned iov_cnt, bool do_write)
{
	unsigned i = 0;
	ssize_t count = 0;
	while (i < iov_cnt) {
		ssize_t written = do_write
			//? write(fd, iov[i].iov_base, iov[i].iov_len)
			? send(fd, iov[i].iov_base, iov[i].iov_len, 0)
			//: read(fd, iov[i].iov_base, iov[i].iov_len);
			: recv(fd, iov[i].iov_base, iov[i].iov_len , 0);
		if (written > 0) {
			count += written;
		} else if (!written) {
			break;
		} else if (errno == EINTR) {
			continue;
		} else {
			/* else it is some "other" error,
			* only return if there was no data processed. */
			if (count == 0) {
				count = -1;
			}
			break;
		}
		i++;
	}
	return count;
}

ssize_t readv(int fd, struct iovec *iov, unsigned iov_cnt)
{
	return readv_writev(fd, iov, iov_cnt, false);
}

ssize_t writev(int fd, struct iovec *iov, unsigned iov_cnt)
{
	return readv_writev(fd, iov, iov_cnt, true);
}

int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year     = wtm.wYear - 1900;
	tm.tm_mon     = wtm.wMonth - 1;
	tm.tm_mday     = wtm.wDay;
	tm.tm_hour     = wtm.wHour;
	tm.tm_min     = wtm.wMinute;
	tm.tm_sec     = wtm.wSecond;
	tm. tm_isdst    = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;

	return (0);
}

__int64 hton64(__int64 val ) 
{ 
	long high, low;
	__int64 nRet = 0;

	low = (long)(val & 0x00000000FFFFFFFF);
	val>>=32;
	high = (long)(val & 0x00000000FFFFFFFF);
	low = htonl( low );
	high = htonl( high );

	nRet = low;
	nRet <<=32;
	nRet |= high;

	return nRet; 
} 


__int64 ntoh64(__int64 val )
{
	long high, low;
	__int64 nRet = 0;

	low = (long)(val & 0x00000000FFFFFFFF);
	val>>=32;
	high = (long)(val & 0x00000000FFFFFFFF);
	low = ntohl( low );
	high = ntohl( high );

	nRet = low;
	nRet <<=32;
	nRet |= high;

	return nRet;
}

#endif