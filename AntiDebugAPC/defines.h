#pragma once
#ifndef _DEFINED_H_
#define _DEFINED_H_

#include <Windows.h>

typedef __success(return >= 0) LONG NTSTATUS;
#define NTAPI __stdcall

typedef VOID(*PKNORMAL_ROUTINE)(PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2
	);

typedef ULONG(WINAPI * _NtQueueApcThread)(HANDLE ThreadHandle,
	PKNORMAL_ROUTINE ApcRoutine,
	PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2
	);

typedef VOID(WINAPI* _NtTestAlert)(VOID);
typedef VOID(NTAPI* _KiUserApcDispatcher)(
	PVOID Unused1,
	PVOID Unused2,
	PVOID Unused3,
	PVOID ContextStart,
	PVOID ContextBody);

#endif