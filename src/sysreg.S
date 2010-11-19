	.set noreorder

#include "pspstub.s"

	STUB_START	"sceSysreg_driver",0x40090000,0x00020005
	STUB_FUNC	0xAB3185FD, sceSysreg_driver_AB3185FD;
	STUB_FUNC   0x136E8F5A, sceSysreg_driver_136E8F5A;
	STUB_END
