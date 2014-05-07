#include <windows.h>
#include <sys/uio.h>
#include <utils.h>
#include <errno.h>

ssize_t readv(int fd,struct iovec const*  vector, int count)
{
	int             i;
	size_t          total;
	void*           pv;
	ssize_t  ret;

	/* Determine the total size. */
	for(i = 0, total = 0; i < count; ++i)
	{
		total += vector[i].iov_len;
	}

	pv = HeapAlloc(GetProcessHeap(), 0, total);

	if(NULL == pv)
	{
		errno = internal_errno_from_Win32(GetLastError());

		ret = -1;
	}
	else
	{
		HANDLE  h = (HANDLE)internal_Windows_HANDLE_from_file_handle(fd);
		DWORD   dw;

		if(!ReadFile(h, pv, (DWORD)total, &dw, NULL))
		{
			errno = internal_errno_from_Win32(GetLastError());

			ret = -1;
		}
		else
		{
			for(i = 0, ret = 0; i < count && 0 != dw; ++i)
			{
				size_t n = (dw < vector[i].iov_len) ? dw : vector[i].iov_len;

				(void)memcpy(vector[i].iov_base, (char const*)pv + ret, n);

				ret +=  (ssize_t)n;
				dw  -=  (DWORD)n;
			}
		}

		(void)HeapFree(GetProcessHeap(), 0, pv);
	}

	return ret;
}

ssize_t writev(int fd, struct iovec const*  vector,int count)
{
	int             i;
	size_t          total;
	void*           pv;
	ssize_t  ret;

	/* Determine the total size. */
	for(i = 0, total = 0; i < count; ++i)
	{
		total += vector[i].iov_len;
	}

	pv = HeapAlloc(GetProcessHeap(), 0, total);

	if(NULL == pv)
	{
		errno = internal_errno_from_Win32(GetLastError());

		ret = -1;
	}
	else
	{
		HANDLE  h = (HANDLE)internal_Windows_HANDLE_from_file_handle(fd);
		DWORD   dw;

		for(i = 0, ret = 0; i < count; ++i)
		{
			(void)memcpy((char*)pv + ret, vector[i].iov_base, vector[i].iov_len);

			ret += (ssize_t)vector[i].iov_len;
		}

		if(!WriteFile(h, pv, (DWORD)total, &dw, NULL))
		{
			errno = internal_errno_from_Win32(GetLastError());

			ret = -1;
		}
		else
		{
			ret = (int)dw;
		}

		(void)HeapFree(GetProcessHeap(), 0, pv);
	}

	return ret;
}
