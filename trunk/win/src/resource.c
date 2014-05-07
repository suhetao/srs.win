#include <utils.h>
#include <sys/resource.h>
#include <assert.h>
#include <errno.h>
#include <windows.h>

int getrusage(int who,struct  rusage*   r_usage)
{
    assert(NULL != r_usage);

    if( RUSAGE_SELF != who)
    {
        errno = EINVAL;

        return -1;
    }
    else
    {
        ULARGE_INTEGER  kernelTime;
        ULARGE_INTEGER  userTime;
        FILETIME        dummy;

        if(!GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime))
        {
            errno =  internal_errno_from_Win32(GetLastError());
            return -1;
        }
        else
        {
            r_usage->ru_stime.tv_sec    =   (long)(0x7fffffff & (unsigned long)kernelTime.QuadPart / 10000000);
            r_usage->ru_utime.tv_sec    =   (long)(0x7fffffff & (unsigned long)userTime.QuadPart / 10000000);
            r_usage->ru_stime.tv_usec   =   (long)(0x7fffffff & (unsigned long)((kernelTime.QuadPart % 10000000) / 10));
            r_usage->ru_utime.tv_usec   =   (long)(0x7fffffff & (unsigned long)((userTime.QuadPart % 10000000) / 10));

            return 0;
        }
    }
}
