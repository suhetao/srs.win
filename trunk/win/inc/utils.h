#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define NUM_ELEMENTS(ar) (sizeof(ar) / sizeof(ar[0]))

int internal_errno_from_Win32(unsigned long w32Err);
void* internal_Windows_HANDLE_from_file_handle(int fd);

#ifdef __cplusplus
}
#endif


#endif