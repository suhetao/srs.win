#ifndef _UIO_H
#define _UIO_H

#include <stddef.h>
#include <WinSock2.h>

#ifdef __cplusplus
extern "C"
{
#endif

int gettimeofday(struct timeval* tv, void*dummy);

#ifdef __cplusplus
}
#endif

#endif