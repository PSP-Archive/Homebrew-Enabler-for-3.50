	.set noreorder

#include "pspstub.s"

	STUB_START "KernelDumper",0x40090000,0x00020005
	STUB_FUNC  0xCFBB7A8D,dump_cop0_regs
	STUB_FUNC  0x7EBFCB19,dump_memregion
	STUB_END
