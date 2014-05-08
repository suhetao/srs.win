#ifndef _UNISTD_H
#define _UNISTD_H

#include <direct.h>
#include <io.h>
#include <process.h>

#define getpid _getpid
#define getcwd _getcwd

void usleep(long usec);

#endif /* _UNISTD_H */