#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include "Minhook.h"
#include "defines.h"

#ifdef __DISTORMX
#include "include\distormx.h"
#endif


#ifdef _WIN64
	#ifdef _DISTORMX
		#pragma comment(lib, "distormx_64.lib")
	#else
	//#pragma comment(lib, "libMinHook.x64.lib")
	#endif
#else
	#ifdef _DISTORMX
		#pragma comment(lib, "distormx.lib")
	#else
	//#pragma comment(lib, "libMinHook.x86.lib")
	#endif
#endif

_KiUserApcDispatcher KiUserApcDispatcher_orig = NULL;


/*
void printContext(PCONTEXT ctx)
{
	printf("CONTEXT:\n\t");
	printf("EAX: %X\n", ctx->Eax);
	printf("EBP: %X\n", ctx->Ebp);
	printf("ESP: %X\n", ctx->Esp);
	printf("EIP: %X\n", ctx->Eip);
}
*/

__forceinline
void MyKiUserApcDispatcher(
	PVOID Unused1,
	PVOID Unused2,
	PVOID Unused3,
	PVOID ContextStart,
	PVOID ContextBody)
{
	//__asm int 3
#ifndef _M_AMD64
	PVOID savedESP;
	__asm {
		mov [savedESP], esp;
		//pushad
		// pushfd
	}
#endif
	printf("Inside Dispatcher Hook\nPrinting Values\n");
	printf("Param1: %p\nParam2: %p\nParam3: %p\n", Unused1, Unused2, Unused3);
	printf("Param4: %X\nParam5: %X\n", ContextStart, ContextBody);
	//PCONTEXT ctx = (PCONTEXT)ContextStart;
#ifndef _M_AMD64	
	__asm {
		mov esp, [savedESP]
	}
#endif
	KiUserApcDispatcher_orig(Unused1, Unused2, Unused3, ContextStart, ContextBody);
	
}

VOID MyAPCFunc(LPVOID context, LPVOID cxt1, LPVOID cxt2)
{

	MessageBoxA(NULL, "Hello", "World", MB_OK);
	//printf("Hello World\n");
}

int main(int argc, char** argv)
{

	//KiUserApcDispatcher_orig = (_KiUserApcDispatcher)GetProcAddress(GetModuleHandle(TEXT("ntdll")), "KiUserApcDispatcher");
#ifndef __DISTORMX
	if (MH_Initialize() != MH_OK)
	{
		printf("Failed to init hooker\n");
		return 0;
	}
#endif

#ifndef __DISTORMX
	if(MH_CreateHookApiEx(TEXT("ntdll"), "KiUserApcDispatcher", &MyKiUserApcDispatcher, (LPVOID*)&KiUserApcDispatcher_orig, NULL) != MH_OK)
#else
	if (!distormx_hook((void**)&KiUserApcDispatcher_orig, MyKiUserApcDispatcher))
#endif
	{
		printf("hook failed!\n");
		getchar();
		return 0;
	}

#ifndef __DISTORMX
	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		printf("Failed to enable hooks\n");
		return 0;
	}
#endif

	_NtQueueApcThread NtQueueApc = (_NtQueueApcThread)GetProcAddress(GetModuleHandle(TEXT("ntdll")), "NtQueueApcThread");
	_NtTestAlert NtTestAlert = (_NtTestAlert)GetProcAddress(GetModuleHandle(TEXT("ntdll")), "NtTestAlert");
	printf("Queuing the APC\n");
	NtQueueApc(GetCurrentThread(), MyAPCFunc, NULL, NULL, NULL);
	//Clear the APC Queue
	printf("Emptying Queue\n");
	NtTestAlert();
	printf("Aftermath\n");
	//SetEvent(GetCurrentThread());
	getchar();

#ifdef __DISTORMX
	distormx_unhook((void*)&KiUserApcDispatcher_orig);
	distormx_destroy();
#endif
	return 0;
}