/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - Dump the Initial Program Loader (IPL).
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * Sample program to write the contents of the Initial Program Loader (IPL)
 * from flash to a file.  The IPL is written to ms0:/ipl.bin.
 *
 * A table describing the location of the IPL is located at block offset 4
 * (physical page 128) in flash.  The table contains a list of 16-bit values
 * that point to the block offsets of the IPL itself.  The table is terminated
 * with an entry of zero.
 * 
 * $Id: main.c 1212 2005-10-24 06:36:57Z mrbrown $
 */

/* 2.71 changes: splited into an user and a kernel module. */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <pspuser.h>
#include <pspdebug.h>
#include <pspsdk.h>

/* Define the module info section */
PSP_MODULE_INFO("dumpipl", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

int g_iPageSize, g_iPagesPerBlock, g_iBlockSize;
int g_bFlashLocked = 0;

void LockFlash();
void UnlockFlash();
void die(const char *msg);
int ReadIPLBlockTable(void *pBlockBuf);

/* Exit callback */
int exit_callback(void)
{
	//UnlockFlash();
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", (SceKernelCallbackFunction) exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();
	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

int main(int argc, char *argv[])
{
	pspDebugScreenInit();
	SetupCallbacks();
	printf("Dumping ipl...\n");

	SceUID mod = pspSdkLoadStartModule("ipldumper.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (mod < 0 )
	{
		printf("Error %08X loading/starting %s\n", mod);
	}
	

	printf("\nFinished writing IPL to ms0:/ipl.bin.  Use [HOME] to exit.\n");
	sceKernelSleepThread();
	return 0;
}
