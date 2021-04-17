release: all
	mksfo 'Cube Sample' PARAM.SFO
	pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL cube.prx NULL


TARGET = cube
OBJS = cube.o logo.o ../common/callbacks.o ../common/vram.o

INCDIR =
CFLAGS = -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1

LIBDIR =
LDFLAGS =
LIBS= -lpspgum -lpspgu -lm

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

logo.o: logo.raw
	bin2o -i logo.raw logo.o logo
