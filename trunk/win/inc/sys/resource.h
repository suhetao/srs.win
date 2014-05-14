#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <sys/time.h>
#ifdef __cplusplus
extern "C"
{
#endif

#define RUSAGE_SELF      (0)
#define RUSAGE_CHILDREN  (-1)

struct rusage
{
    struct timeval  ru_utime;       /*!< User time used. */
    struct timeval  ru_stime;       /*!< System time used. */
    char            reserved[56];   /*!< [RESERVED] Reserved for future use. */
};

int getrusage(int who,struct rusage*   r_usage);

#ifdef __cplusplus
}
#endif

#endif

