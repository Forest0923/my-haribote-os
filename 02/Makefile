# Default
default :
	make img


# File gen
ipl.bin : ipl.asm Makefile
	nasm ipl.asm -o ipl.bin -l ipl.lst

helloos.img : ipl.bin Makefile
	cat ipl.bin > helloos.img


# Commands
asm : 
	make -r ipl.bin

img :
	make -r helloos.img

run :
	make img
	qemu-system-i386 helloos.img

clean :
	rm -f ipl.bin
	rm -f ipl.lst

src_only :
	make clean
	rm -f helloos.img

