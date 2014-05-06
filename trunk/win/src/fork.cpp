#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include <errno.h>
#include <process.h>

typedef struct _CLIENT_ID {
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _SECTION_IMAGE_INFORMATION {
	PVOID EntryPoint;
	ULONG StackZeroBits;
	ULONG StackReserved;
	ULONG StackCommit;
	ULONG ImageSubsystem;
	WORD SubSystemVersionLow;
	WORD SubSystemVersionHigh;
	ULONG Unknown1;
	ULONG ImageCharacteristics;
	ULONG ImageMachineType;
	ULONG Unknown2[3];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

typedef struct _RTL_USER_PROCESS_INFORMATION {
	ULONG Size;
	HANDLE Process;
	HANDLE Thread;
	CLIENT_ID ClientId;
	SECTION_IMAGE_INFORMATION ImageInformation;
} RTL_USER_PROCESS_INFORMATION, *PRTL_USER_PROCESS_INFORMATION;

#define RTL_CLONE_PROCESS_FLAGS_CREATE_SUSPENDED 0x00000001
#define RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES 0x00000002
#define RTL_CLONE_PROCESS_FLAGS_NO_SYNCHRONIZE 0x00000004

#define RTL_CLONE_PARENT 0
#define RTL_CLONE_CHILD 297

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif

#ifndef pid_t
typedef int pid_t;
#endif

typedef NTSTATUS (_stdcall *RtlCloneUserProcess_f)(__in ULONG ProcessFlags,
						 __in_opt PSECURITY_DESCRIPTOR ProcessSecurityDescriptor, 
						 __in_opt PSECURITY_DESCRIPTOR ThreadSecurityDescriptor, 
						 __in_opt HANDLE DebugPort, 
						 __out PRTL_USER_PROCESS_INFORMATION ProcessInformation ); 

pid_t fork(void)
{
	HMODULE mod;
	RtlCloneUserProcess_f clone_p;
	RTL_USER_PROCESS_INFORMATION process_info;
	NTSTATUS result;

	mod = GetModuleHandle("ntdll.dll");
	if (!mod)
		return -ENOSYS;

	clone_p = (RtlCloneUserProcess_f)GetProcAddress(mod, "RtlCloneUserProcess");
	if (clone_p == NULL)
		return -ENOSYS;

	result = clone_p(RTL_CLONE_PROCESS_FLAGS_CREATE_SUSPENDED | RTL_CLONE_PROCESS_FLAGS_INHERIT_HANDLES, NULL, NULL, NULL, &process_info);

	if (result == RTL_CLONE_PARENT)
	{
		HANDLE me = GetCurrentProcess();
		pid_t child_pid;

		child_pid = GetProcessId(process_info.Process);

		ResumeThread(process_info.Thread);
		CloseHandle(process_info.Process);
		CloseHandle(process_info.Thread);

		return child_pid;
	}
	else if (result == RTL_CLONE_CHILD)
	{
		AllocConsole();
		return 0;
	}
	else
		return -1;
	return -1;
}