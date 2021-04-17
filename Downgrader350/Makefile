# Change for your own path to the memstick.
MSROOT=/psph

PSPSDK=$(shell psp-config --pspsdk-path)
PSPDEV=$(shell psp-config --pspdev-path)
INCLUDE=$(PSPSDK)/include
MSDEST=$(MSROOT)/psp/game

release:  decryptprx/decryptprx.prx extractprxs/extprxs.pbp libpsardumper/libpsardumper.prx ta082_driver/ta082driver.prx updateflasher/EBOOT.PBP
	rm -rf release
	mkdir -p release/MS_ROOT/psp/game/UPDATE
	mkdir -p release/SRC
	cp decryptprx/decryptprx.prx release/MS_ROOT/psp/game/UPDATE/
	cp updateflasher/EBOOT.PBP release/MS_ROOT/psp/game/UPDATE/
	cp extractprxs/extprxs.pbp release/MS_ROOT/psp/game/UPDATE/
	cp libpsardumper/libpsardumper.prx release/MS_ROOT/psp/game/UPDATE/
	cp ta082_driver/ta082_driver.prx release/MS_ROOT/psp/game/UPDATE/
	cp docs/* release/
	svn export . release/SRC/Downgrader350

all:    decryptprx/decryptprx.prx extractprxs/extprxs.pbp libpsardumper/libpsardumper.prx ta082_driver/ta082driver.prx updateflasher/EBOOT.PBP
	mkdir -p $(MSDEST)/UPDATE
	cp decryptprx/decryptprx.prx $(MSDEST)/UPDATE/
	cp updateflasher/EBOOT.PBP $(MSDEST)/UPDATE/
	cp extractprxs/extprxs.pbp $(MSDEST)/UPDATE/
	cp libpsardumper/libpsardumper.prx $(MSDEST)/UPDATE/
	cp ta082_driver/ta082_driver.prx $(MSDEST)/UPDATE/

clean:
	make -C decryptprx clean
	make -C extractprxs clean
	make -C libpsardumper clean
	make -C ta082_driver clean
	make -C updateflasher clean
	rm -rf release

decryptprx/decryptprx.prx:  decryptprx/*.h decryptprx/*.c
	make -C decryptprx decryptprx.prx

extractprxs/extprxs.pbp:  extractprxs/*.c  extractprxs/*.h extractprxs/PRXDecrypter.S
	make -C extractprxs extprxs.pbp

libpsardumper/libpsardumper.prx: libpsardumper/*.c libpsardumper/*.h
	make -C libpsardumper libpsardumper.prx

ta082_driver/ta082driver.prx: ta082_driver/*.h ta082_driver/*.c ta082_driver/pspTA082_driver.S
	make -C ta082_driver

updateflasher/EBOOT.PBP: updateflasher/*.h updateflasher/*.c updateflasher/*.S
	make -C updateflasher release

