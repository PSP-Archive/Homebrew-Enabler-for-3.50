	.set noreorder

#include "pspstub.s"

	STUB_START "NandDumper",0x40090000,0x00010005
	STUB_FUNC  0x9EDD6EE1,ReadBlock
	STUB_END
