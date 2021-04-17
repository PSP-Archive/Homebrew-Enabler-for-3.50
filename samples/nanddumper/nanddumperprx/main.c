#include <pspsdk.h>
#include <pspkernel.h>
#include <pspnand_driver.h>
#include <string.h>

PSP_MODULE_INFO("NandDumper", 0x1006, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

int nandLocked = 0;

void LockNand() 
{
	if (!nandLocked)
		sceNandLock(0);
	
	nandLocked = 1;
}


void UnlockNand() 
{
	if (nandLocked)
		sceNandUnlock();

	nandLocked = 0;
}

int ReadBlock(u32 page, u8 *buffer)
{
	u32 i, j;
	u32 k1;

	k1 = pspSdkSetK1(0);

	LockNand();

	if (sceNandIsBadBlock(page))
	{
		memset(buffer, 0xFF, 528);
		UnlockNand();
		pspSdkSetK1(k1);		
		return -1;
	}

	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 4; j++)
		{
			sceNandReadPagesRawAll(page, buffer, NULL, 1);
			sceNandReadExtraOnly(page, buffer+512, 1);
		}

		page++;
		buffer += 528;
	}

	UnlockNand();
	pspSdkSetK1(k1);

	return 0;
}


int module_start(SceSize args, void *argp) 
{
	return 0;
}

int module_stop()
{
	return 0;
}
