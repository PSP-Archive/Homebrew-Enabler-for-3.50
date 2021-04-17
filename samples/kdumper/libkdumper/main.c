#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Define the module info section, note the 0x1000 flag to enable start in kernel mode */
PSP_MODULE_INFO("LibKDumper", 0x1006, 1, 0);

/* Define the thread attribute as 0 so that the main thread does not get converted to user mode */
PSP_MAIN_THREAD_ATTR(0);

/* Dump the cop0 regs, good proof we are in kernel mode */
void dump_cop0_regs(char *str)
{
	u32 regs[32];
	int i;
	u32 k1 = pspSdkSetK1(0);

	asm __volatile__ (
		"mfc0 $3, $0\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $1\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $2\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $3\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $4\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $5\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $6\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $7\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $8\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $9\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $10\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $11\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $12\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $13\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $14\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $15\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $16\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $17\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $18\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $19\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $20\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $21\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $22\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $23\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $24\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $25\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $26\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $27\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $28\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $29\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $30\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		"mfc0 $3, $31\n"
		"sw $3, 0(%0)\n"
		"addi %0, %0, 4\n"
		:
		: "r"(&regs[0])
	);

	sprintf(str, "Cop0 regs\n");
	for(i = 0; i < 32; i+=4)
	{
		sprintf(str, "$%02d: %08X, $%02d: %08X, $%02d: %08X $%02d: %08X\n", 
				i, regs[i], i+1, regs[i+1], i+2, regs[i+2], i+3, regs[i+3]);
		str += strlen(str);
	}
	pspSdkSetK1(k1);
}

/* Well what would you expect ? :) */
void dump_memregion(const char* file, void *addr, int len)
{
	u32 k1;
	int fd;

	k1 = pspSdkSetK1(0);

	fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
	if(fd >= 0)
	{
		printf("Writing %s\n", file);
		sceIoWrite(fd, addr, len);
		sceIoClose(fd);
	}

	pspSdkSetK1(k1);
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}
