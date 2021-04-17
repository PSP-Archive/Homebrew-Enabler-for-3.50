PSPSDK=$(shell psp-config --pspsdk-path)
NAND_DRIVER_OBJS = sceNand_driver_0000.o sceNand_driver_0001.o sceNand_driver_0002.o sceNand_driver_0003.o sceNand_driver_0004.o sceNand_driver_0005.o sceNand_driver_0006.o sceNand_driver_0007.o sceNand_driver_0008.o sceNand_driver_0009.o sceNand_driver_0010.o sceNand_driver_0011.o sceNand_driver_0012.o sceNand_driver_0013.o sceNand_driver_0014.o sceNand_driver_0015.o sceNand_driver_0016.o sceNand_driver_0017.o sceNand_driver_0018.o sceNand_driver_0019.o sceNand_driver_0020.o sceNand_driver_0021.o sceNand_driver_0022.o sceNand_driver_0023.o sceNand_driver_0024.o sceNand_driver_0025.o sceNand_driver_0026.o sceNand_driver_0027.o sceNand_driver_0028.o sceNand_driver_0029.o sceNand_driver_0030.o sceNand_driver_0031.o sceNand_driver_0032.o sceNand_driver_0033.o sceNand_driver_0034.o sceNand_driver_0035.o sceNand_driver_0036.o sceNand_driver_0037.o sceNand_driver_0038.o sceNand_driver_0039.o 

%.o: sceNand_driver.S
	psp-gcc -g -O2 -G0 -Wall -I${PSPSDK}/include -DF_$* $< -c -o $@

all: $(NAND_DRIVER_OBJS)
	psp-ar cru libpspnand_driver2.a $(NAND_DRIVER_OBJS)
	psp-ranlib libpspnand_driver2.a

clean:
	rm -f *.o
	rm -f *.a