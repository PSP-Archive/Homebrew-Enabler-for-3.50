release: all
	pack-pbp EBOOT.PBP PARAM2.SFO ICON0.PNG NULL NULL NULL NULL updateflasher.prx NULL


TARGET = updateflasher
OBJS = main.o pspPSAR.o pspTA082_driver.o netconfbackup.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ./
LDFLAGS =
LIBS = -lpspvshbridge_350 -lpspipl_update -lpsppower_2 -lpsputility

BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
