/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - KDumper sample, demonstrates use of kernel mode.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: main.c 1095 2005-09-27 21:02:16Z jim $
 */

/* Changes for 2.71: splitted into an user and kernel prx's */

#include <pspkernel.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>

/* Define the module info section, note the 0x1000 flag to enable start in kernel mode */
PSP_MODULE_INFO("SDKTEST", 0, 1, 1);

/* Define the thread attribute as 0 so that the main thread does not get converted to user mode */
PSP_MAIN_THREAD_ATTR(0);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	/* Note the use of THREAD_ATTR_USER to ensure the callback thread is in user mode */
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}


void dump_cop0_regs(char *str);
void dump_memregion(const char* file, void *addr, int len);

int LoadModule()
{
	SceUID mod = sceKernelLoadModule("libkdumper.prx", 0, NULL);
	if (mod < 0)
	{
		printf("Error 0x%08X loading module.\n", mod);
		return -1;
	}

	mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
	if (mod < 0)
	{
		printf("Error 0x%08X starting module.\n", mod);
		return -1;
	}

	return 0;
}

char str[1024];

int main(void)
{
	pspDebugScreenInit();
	SetupCallbacks();

	if (LoadModule() >= 0)
	{

		printf("Kernel mem dumper. TyRaNiD 2k5.\n");
		printf("Props to nem, mrbrown, adresd et al\n\n");
		dump_cop0_regs(str);
		printf("%s", str);
		printf("Dumping Boot Mem 0xBFC00000 -> 0xBFD00000\n");
		dump_memregion("ms0:/boot.bin", (void*) 0xBFC00000, 0x100000);
		printf("Dumping Kernel Mem 0x88000000 -> 0x883FFFFF\n");
		dump_memregion("ms0:/kmem.bin", (void*) 0x88000000, 0x400000);
		printf("Done\n");		
	}

	/* Exit the thread, this will allow the exit callback to work */
	sceKernelExitDeleteThread(0);

	return 0;
}
