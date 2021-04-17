#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspidstorage.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


PSP_MODULE_INFO("pspTA082_Driver", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

u8 leaf[512];

int pspTA082ChangeIdStorage()
{
	int k1 = pspSdkSetK1(0);
	int res = 0;
	
	if (sceIdStorageIsReadOnly())
	{
		res = -1;
	}
	else
	{
		sceIdStorageReadLeaf(0x0005, leaf);
		leaf[0] = 'K';
		sceIdStorageWriteLeaf(0x0005, leaf);
	}

	pspSdkSetK1(k1);
	return res;
}


int pspTA082IsDowngraded()
{
	int key, res = 1;
	int k1 = pspSdkSetK1(0);

	sceIdStorageLookup(0x05, 0, &key, 4);

	if (key == 0x436C6B67) // Clkg
	{
		res = 0;
	}

	pspSdkSetK1(k1);
	return res;
}

int pspTA082IsTA082()
{
	int res = 0;
	int k1 = pspSdkSetK1(0);

	sceIdStorageReadLeaf(0x100, leaf);

	if (leaf[0x3F] == 2)
	{
		sceIdStorageReadLeaf(0x50, leaf);

		if (leaf[0x21] == 2)
		{		
			res = 1;
		}
	}

	pspSdkSetK1(k1);
	return res;
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}
