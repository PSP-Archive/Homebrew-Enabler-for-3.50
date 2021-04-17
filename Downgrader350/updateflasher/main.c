#include <pspsdk.h>
#include <pspkernel.h>
#include <psploadexec.h>
#include <pspvshbridge.h>
#include <pspipl_update.h>
#include <psputils.h>
#include <pspctrl.h>
#include <psppower.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "libpsardumper.h"

/* 0x0800 -> vsh */
PSP_MODULE_INFO("PsarFlasher", 0x0800, 1, 0);

PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);

#define printf pspDebugScreenPrintf

u8 dataPSAR[15100000] __attribute__((aligned(64)));
u8 dataOut[2000000] __attribute__((aligned(64)));
u8 dataOut2[2000000] __attribute__((aligned(64)));

char version[5];
int psarSize;

#define EXTRACT_PRXS	"ms0:/PSP/GAME/UPDATE/extprxs.pbp"

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);

	sceKernelDelayThread(milisecs*1000);
	vshKernelExitVSHVSH(NULL);
}

int LoadStartModule(char *module)
{
	SceUID mod = vshKernelLoadModuleVSH(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0)
	{
		return -1;
	}

	int written = sceIoWrite(fd, buf, size);
	
	if (sceIoClose(fd) < 0)
		return -1;

	return written;
}

int ByPass()
{
	SceCtrlData pad;
	
	sceCtrlReadBufferPositive(&pad, 1);

	if ((pad.Buttons & PSP_CTRL_TRIANGLE) && (pad.Buttons & PSP_CTRL_LTRIGGER))
			return 1;
	
	return 0;
}

void LoadModules()
{
	SceUID mod;

	mod = LoadStartModule("lflash_fatfmt.prx");
	if (mod < 0)
	{
		ErrorExit(6000, "Error %08X loading/starting lflash_fatfmt.prx\n", mod);
	}

	mod = LoadStartModule("ipl_update.prx");
	if (mod < 0)
	{
		ErrorExit(6000, "Error %08X loading/starting ipl_update.prx\n", mod);
	}

	mod = LoadStartModule("libpsardumper.prx");
	if (mod < 0)
	{
		ErrorExit(6000, "Error %08X loading/starting libpsardumper.prx.\n", mod);
	}
}

void GetVersion(char *buf)
{
	strcpy(version, strrchr(buf, ',')+1);
}

void ReadPSAR()
{
	printf("Loading PSAR to RAM...");
	
	SceUID fd = sceIoOpen("UPDATE.PBP", PSP_O_RDONLY, 0777);
	u32 header[10];	
		
	if (fd < 0)
	{
		ErrorExit(6000, "Error opening UPDATE.PBP.\n");
	}

	sceIoRead(fd, header, sizeof(header));

	if (header[0] != 0x50425000)
	{
		ErrorExit(6000, "No a valid PBP file.\n");
	}

	sceIoLseek(fd, header[9], PSP_SEEK_SET);
	psarSize = sceIoRead(fd, dataPSAR, sizeof(dataPSAR));

	if (psarSize <= 0)
	{
		ErrorExit(6000, "Read error or corrupted file.\n");
	}

	if (pspPSARInit(dataPSAR, dataOut, dataOut2) < 0)
	{
		ErrorExit(6000, "Corrupted or unsupported PSAR data\n");
	}

	printf (" done.\n");

	GetVersion((char *)dataOut+0x10);

	printf("Version %s\n", version);

	if (strcmp(version, "2.80") >= 0)
	{
		if (!ByPass())
		{
			ErrorExit(6000, "This program doesn't want to flash 2.80+ upaters.\n");
		}
	}	
}

int CreateDirs()
{
	if (sceIoMkdir("flash0:/data", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/dic", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/font", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/kd", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/vsh", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/data/cert", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/kd/resource", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/vsh/etc", 0777) < 0)
		return -1;

	if (sceIoMkdir("flash0:/vsh/module", 0777) < 0)
		return  -1;

	if (sceIoMkdir("flash0:/vsh/resource", 0777) < 0)
		return -1;

	return 0;
}

void SetPercentage(int x, int y, int value, int max, int dv)
{
	if (dv)
	{
		value /= dv;
		max /= dv;
	}

	pspDebugScreenSetXY(x, y);
	printf("%3d%%", ((100 * value) / max));
}

void Update()
{
	char *argv[2];
	int is15X;
	int x, y, i;

	is15X = (strcmp(version, "2.00") < 0);
	
	if (sceIoUnassign("flash0:") < 0)
	{
		ErrorExit(6000, "ERROR: unassigning flash0.\n");		
	}

	if (sceIoUnassign("flash1:") < 0)
	{
		ErrorExit(7000, "ERROR: unassigning flash1.\n");		
	}

	printf("Formating flash0...");
	
	argv[0] = "fatfmt";
	argv[1] = "lflash0:0,0";

	if (vshLflashFatfmtStartFatfmt(2, argv) < 0)
	{
		ErrorExit(6000, "PANIC: Error formating flash0.\n");
	}

	printf(" done.\n");
	printf("Formating flash2...");

	argv[1] = "lflash0:0,2";

	if (vshLflashFatfmtStartFatfmt(2, argv) < 0)
	{
		// This should not be critical
		printf("WARNING: Error formating flash2.\n");
	}

	printf(" done.\n");

	if (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		printf("PANIC: error re-assigning flash0.\n");
	}

	if (sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", IOASSIGN_RDWR, NULL, 0) < 0)
	{
		printf("Error re-assigning flash1.\n");
	}

	printf("Creating directories...");

	if (CreateDirs() < 0)
	{
		printf("PANIC: error creating directories.\n");
	}

	printf(" done.\n");

	if (is15X)
	{
		printf("Writing ipl...");

		if (sceIplUpdateClearIpl() < 0)
		{
			printf("PANIC: error clearing IPL.\n");
		}

		if (sceIplUpdateSetIpl(NULL, 0) < 0)
		{
			printf("PANIC: error writing IPL.\n");
		}

		printf(" done.\n");
	}

	printf("Writing files");
	if (!is15X)
	{
		printf(" and IPL");
	}
	printf("... ");
	x = pspDebugScreenGetX();
	y = pspDebugScreenGetY();
	printf("\n");

	i = 0;
	while (1)
	{
		char name[64];
		int filesize;
		int pos;

		scePowerTick(0);

		int res = pspPSARGetNextFile(dataPSAR, psarSize, dataOut, dataOut2, name, &filesize, &pos);

		if (res < 0)
		{
			ErrorExit(6000, "PANIC: PSAR error.\n");
		}
		else if (res == 0) /* no more files */
		{
			SetPercentage(x, y, 1, 1, 0);
			break;
		}

		if (filesize > 0)
		{
			if (strncmp(name, "ipl", 3) == 0)
			{
				if (sceIplUpdateClearIpl() < 0)
				{
					printf("PANIC: error clearing IPL.\n");
				}

				if (sceIplUpdateSetIpl(dataOut2, filesize) < 0)
				{
					printf("PANIC: error writing IPL.\n");
				}
			}
			else if (strncmp(name, "flash0:", 7) == 0)
			{
				WriteFile(name, dataOut2, filesize);
				sceIoSync("flash0:", 0x10);				
			}
		}

		SetPercentage(x, y, pos, psarSize, 100);		
	}
}

void Agreement()
{
	SceCtrlData pad;

	printf("Update Flasher by Dark_AleX/Noobz.\n");
	printf("PSAR extraction code from psardumper (by PspPet).\n\n");
	
	printf("You are about to flash your PSP.\n");
	printf("Press X to start. By doing it, you accept the risk and ALL\n");
	printf("the responsibility of what happens.\n");
	printf("If you don't agree press R-trigger button.\n");

	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			return;			
		}

		else if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			vshKernelExitVSHVSH(NULL);
		}

		sceKernelDelayThread(10000);
	}
}

void TA082Agreement()
{
	SceCtrlData pad;

	printf("You have a TA082+ PSP, which needs to be patched\n");
	printf("before it can be downgraded.\n\n");
	printf("Press X to start the patch. By doing it, you accept the \n");
	printf("risk and ALL the responsibility of what happens.\n");
	printf("If you don't agree press R-trigger button.\n");

	while (1)
	{
		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			return;			
		}

		else if (pad.Buttons & PSP_CTRL_RTRIGGER)
		{
			vshKernelExitVSHVSH(NULL);
		}

		sceKernelDelayThread(10000);
	}
}

void Reboot()
{
	SceCtrlData pad;

	if (sceKernelDevkitVersion() < 0x02000010) /* < 2.00 */
	{
		printf("Restart the PSP manually.\n");
		sceKernelSleepThread();		
	}
	else
	{
		printf("Press X to restart the PSP.\n");
	
		while (1)
		{
			sceCtrlReadBufferPositive(&pad, 1);

			if (pad.Buttons & PSP_CTRL_CROSS)
			{
				scePower_0442D852(0);				
			}
		}

		sceKernelDelayThread(10000);
	}
}

int main(int argc, char *argv[])
{
	struct SceKernelLoadExecVSHParam param;
		
	pspDebugScreenInit();
	pspDebugScreenSetTextColor(0x005435D0);

	if (scePowerGetBatteryLifePercent() < 75)
	{
		if (!ByPass())
		{
			ErrorExit(6000, "Battery has to be at least at 75%%.\n");
		}
	}

	SceUID mod = LoadStartModule("ta082_driver.prx");
	if (mod < 0)
	{
		ErrorExit(6000, "Error %08X loading/starting ta082_driver.prx\n", mod);
	}

	if(pspTA082IsTA082())
	{
		if(!pspTA082IsDowngraded())
		{
			TA082Agreement();

			int res = pspTA082ChangeIdStorage();
			if (res < 0)
			{
				ErrorExit(6000, "Error %08X in pspTA082ChangeIdStorage. Probably read-only\n", res);
			}

			printf("Patched OK - continuing in a moment...\n");
			sceKernelDelayThread(3000000);

			// Reload the downgrader now we have sorted out TA082 PSP's
			memset(&param, 0, sizeof(param));
			param.size = sizeof(param);
			param.args = strlen("ms0:/PSP/GAME/UPDATE/EBOOT.PBP")+1;
			param.argp = "ms0:/PSP/GAME/UPDATE/EBOOT.PBP";
			param.key = "updater";

			vshKernelLoadExecVSHMs1("ms0:/PSP/GAME/UPDATE/EBOOT.PBP", &param);

			// Add in just in case above does not reboot the psp
			ErrorExit(6000, "Done - Reload the downgrader to continue downgrade.\n");
		}
	}

	if (argc <= 1)
	{
		printf("Doing preparations...\n");
		memset(&param, 0, sizeof(param));
		param.size = sizeof(param);
		param.args = strlen(EXTRACT_PRXS)+1;
		param.argp = EXTRACT_PRXS;
		param.key = "updater";

		vshKernelLoadExecVSHMs1(EXTRACT_PRXS, &param);
	}

	// Make a backup of the network config
	BackupNetconf("ms0:/netconfig.bak");

	Agreement();	

	LoadModules();

	ReadPSAR();

	scePowerLock(0);
	Update();
	scePowerUnlock(0);

	pspDebugScreenInit();

	printf("\n\n\n\n\n\n\n\n\n");
	printf("   \xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n"); 
	printf("                                                                \n"); 
	printf("               \xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb         \xdb\xdb\xdb\xdb\xdb\xdb     \xdb\xdb\xdb\xdb\xdb\xdb         \n"); 
	printf("             \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb       \xdb\xdb\xdb\xdb\xdb\xdb    \xdb\xdb\xdb\xdb\xdb\xdb          \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb   \xdb\xdb\xdb\xdb\xdb\xdb           \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb  \xdb\xdb\xdb\xdb\xdb\xdb            \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb \xdb\xdb\xdb\xdb\xdb\xdb             \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb              \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb              \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb \xdb\xdb\xdb\xdb\xdb\xdb             \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb  \xdb\xdb\xdb\xdb\xdb\xdb            \n"); 
	printf("            \xdb\xdb\xdb\xdb\xdb\xdb        \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb   \xdb\xdb\xdb\xdb\xdb\xdb           \n"); 
	printf("             \xdb\xdb\xdb\xdb\xdb\xdb      \xdb\xdb\xdb\xdb\xdb\xdb       \xdb\xdb\xdb\xdb\xdb\xdb    \xdb\xdb\xdb\xdb\xdb\xdb          \n"); 
	printf("               \xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb         \xdb\xdb\xdb\xdb\xdb\xdb     \xdb\xdb\xdb\xdb\xdb\xdb         \n"); 
	printf("                                                                \n"); 
	printf("   \xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\xdb\n"); 
	
	printf("\n\n\n\n\n\n\n");
	Reboot();

	return 0;
}
