#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include "defines.h"
#include "include\distormx.h"

#ifdef _WIN64
#pragma comment(lib, "distormx_64.lib")
#else
#pragma comment(lib, "distormx.lib")
#endif

_KiUserApcDispatcher KiUserApcDispatcher_orig = NULL;


void printContext(PCONTEXT ctx)
{
	printf("CONTEXT:\n\t");
	printf("EAX: %X\n", ctx->Eax);
	printf("EBP: %X\n", ctx->Ebp);
	printf("ESP: %X\n", ctx->Esp);
	printf("EIP: %X\n", ctx->Eip);
}

void MyKiUserApcDispatcher(
	PVOID Unused1,
	PVOID Unused2,
	PVOID Unused3,
	PVOID ContextStart,
	PVOID ContextBody)
{
	
	printf("Inside Dispatcher Hook\nPrinting Values\n\t");
	printf("Param1: %p\nParam2: %p\nParam3: %p\n", Unused1, Unused2, Unused3);
	printf("Param4: %X\nParam5: %X\n", ContextStart, ContextBody);
	//PCONTEXT ctx = (PCONTEXT)ContextStart;
	KiUserApcDispatcher_orig(Unused1, Unused2, Unused3, ContextStart, ContextBody);
	
}

VOID MyAPCFunc(LPVOID context, LPVOID cxt1, LPVOID cxt2)
{

	MessageBoxA(NULL, "Hello", "World", MB_OK);
	//printf("Hello World\n");
}

int main(int argc, char** argv)
{

	KiUserApcDispatcher_orig = (_KiUserApcDispatcher)GetProcAddress(GetModuleHandle(TEXT("ntdll")), "KiUserApcDispatcher");

	if (!distormx_hook((void**)&KiUserApcDispatcher_orig, MyKiUserApcDispatcher))
	{
		printf("hook failed!\n");
		getchar();
		return 0;
	}
	_NtQueueApcThread NtQueueApc = (_NtQueueApcThread)GetProcAddress(GetModuleHandle(TEXT("ntdll")), "NtQueueApcThread");
	_NtTestAlert NtTestAlert = (_NtTestAlert)GetProcAddress(GetModuleHandle(TEXT("ntdll")), "NtTestAlert");
	printf("Queuing the APC\n");
	NtQueueApc(GetCurrentThread(), MyAPCFunc, NULL, NULL, NULL);
	//Clear the APC Queue
	printf("Emptying Queue\n");
	NtTestAlert();
	//SetEvent(GetCurrentThread());
	getchar();
	distormx_unhook((void*)&KiUserApcDispatcher_orig);
	distormx_destroy();
	return 0;
}