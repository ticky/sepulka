TARGET = sepulka
OBJS = main.o libs_lite.o apihook_lite.o


INCDIR =
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

#LIBDIR =
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspmodulemgr_kernel -lpspsysmem_kernel -lpspumd -lpspsystemctrl_kernel -lpspdebug -lpsprtc -lpspge_driver -lpsppower


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
