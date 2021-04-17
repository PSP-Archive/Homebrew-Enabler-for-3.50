Homebrew Enabler for 3.50  by Noobz / Dark_AleX
-------------------------

This program enables homebrew and downgraders to be run on 3.50 using the US or EU version of lumines. It makes a patch that remains permanent until hardware reset. Sleep mode won't remove the patch.

This is based on the source for HEN 2.71 provided by Dark_Alex, thanks for the code and the help.

Note: this program writes some small files in the flash, ONLY the first time is executed. It does NOT rewrite any existing file in the system, it writes new files, so the probabilities of bricking are practically null.


Instructions
------------
- Change the language to English, under System Settings -> System Language
- Copy all contents inside MS_ROOT to the root of your memory stick.
- Load Lumines
- When you run the program, you'll see a coloured screen that can be one of two colors:
  * Red: the files couldn't be written to the flash (it happens a few times). Just keep trying.
  * Pale Blue: The files were written succesfully, and HEN is active. After that, the xmb will
    be rebooted. Now you can run the homebrew in format prepared for 2.71. (see details below).
    Note that a pale blue screen will appear again every time the PSP reboots with HEN active.

- As mentioned before, the patch will remain resident in memory, and even entering sleep mode
  won't delete it. Only a hardware reset will remove the program from memory.

- When you do a hardware reset, you'll have to run Lumines again.
  This time there will just be the pale blue screen and xmb reboot, since this time there is
  no need to write to the flash.

Note: Ensure that you leave the PSP/HEN_350 and PSP/SAVEDATA directories that are used for HEN
as some of the files are used each time you enter HEN.
Note2: Once the program is loaded, it is independant of the memory stick contents. So you can 
freely swap memory sticks.


Notes about homebrew running
-----------------------------

- Current homebrews are not supported. Since current homebrews are in static elf format,  they
  won't run (they are rejected by the 3.50 kernel). Homebrews for 3.50 have to be in prx format.
  Programmers see the programming notes.
  In future versions i may patch the kernel to allow it to accept static elf's. 

- You can try using the PatchSFO tool from www.noobz.eu to patch homebrew to HEN format.

- There are some samples in the "GAME" directory, all of them ported from the pspsdk samples,
  except the nanddumper written by Dark_AleX. 

 * polyphonic, pspsdk sample (user mode). 
 * cube, pspsdk sample (user mode). It shows a cube rotating.
 * kdumper, pspsdk sample (kernel mode). It dumps kernel memory to the memory stick.
 * ipldumper, pspsdk sample (kernel mode). It dumps the ipl to the memory stick.
 * systemparam, pspsdk sample  (user mode). It displays info about the system
 * nanddumper, by Dark_AleX. (kernel mode). It dumps the full nand content to the  file
   "nandimage.flash" in the memory stick. The format of the dump is compliant with u.p. chip.
   (It requires 33 MB of free space in the memory stick).



Programming notes for developers
--------------------------------

- As mentioned before, homebrew has to be in prx format, at least atm. Furthermore, the main prx, the one in
  the pbp, has to be an user prx. However this user prx can load unsigned kernel modules from
  the memory stick, allowing kernel mode apps.

- There is no need for noplainmodule check patch or nodevicecheck patch, since those are done
  by the homebrew enabler. 

- Try to use USE_KERNEL_LIBS = 1 in your kernel prx's. In 2.XX, the separation between user
  and kernel mode is higher than in 1.XX. 
  Some functions in "*ForUser" are now totally separated (different address) from their 
  "*ForKernel" equivalents. A good example is ModuleMgr. If you are in a kernel prx and 
  you call the sceKernelLoadModule of ModuleMgrForUser, you will get a "ILLEGA_PERMANENT_CALL" error.

- See the source code of the samples to have a hint on how to program for 3.50.

Credits and thanks.
-------------------
- Noobz team for porting to 3.50
- Noobz team for porting to 3.03
- Noobz team for porting to 2.80

Thanks to:
- Original code for 2.71 HEN supplied by Dark_AleX

- Booster for creating devhook. HEN reboot method is based on devhook one.

- Mathieulh for the help provided to Dark_AleX for the two months that he was without psp.

