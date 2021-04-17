#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>

PSP_MODULE_INFO("NandDumperMain", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

u8 block[32*32*528];

int ReadBlock(u32 page, u8 *buffer);

#define printf pspDebugScreenPrintf

int main() 
{
	pspDebugScreenInit();
	
	SceUID mod = pspSdkLoadStartModule("nanddumper.prx", PSP_MEMORY_PARTITION_KERNEL);

	if (mod < 0)
	{
		printf("Error 0x%08X loading/starting naddumper.prx.\n", mod);
	}

	else
	{

		SceUID fd = sceIoOpen("ms0:/nanddump.flash", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

		printf("Dumping...\n");

		int i, j;

		for (i = 0; i < (2048*32); )
		{			
			u8 *p = block;
			memset(block, 0xff, sizeof(block));

			for (j = 0; j < 32; j++)
			{

				if (ReadBlock(i, p) < 0)
				{
					printf("bad block at page %d block %d\n", i, i/32);
				}

				i += 32;
				p += (528*32);
			}
			
			sceIoWrite(fd, block, sizeof(block));			
		}

		sceIoClose(fd);

		printf("Done. Exiting in 5 seconds\n");
	}

	sceKernelDelayThread(5*1000*1000);
	sceKernelExitGame();

	return 0;
}



