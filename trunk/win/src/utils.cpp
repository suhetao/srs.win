#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <time.h>
#include <errno.h>
#include <srs_auto_headers.hpp>


static ssize_t readv_writev(int fd, struct iovec *iov, unsigned iov_cnt, bool do_write)
{
	unsigned i = 0;
	ssize_t count = 0;
	while (i < iov_cnt) {
		ssize_t written = do_write
			? write(fd, iov[i].iov_base, iov[i].iov_len)
			: read(fd, iov[i].iov_base, iov[i].iov_len);
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

ssize_t readv(int fd, const struct iovec *iov, unsigned int iov_cnt)
{
	return readv_writev(fd, (struct iovec *)iov, iov_cnt, false);
}

ssize_t writev(int fd, const struct iovec *iov, unsigned int iov_cnt)
{
	return readv_writev(fd, (struct iovec *)iov, iov_cnt, true);
}

long sysconf(int name)
{  
	SYSTEM_INFO info;  
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

void initsocket(void){
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(2,2);
	WSAStartup(version, &wsaData);
}