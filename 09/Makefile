C_OBJ_FILES = bootpack.c graphic.c dsctbl.c int.c fifo.c keyboard.c mouse.c memory.c hankaku.c mysprintf.c nasmfunc.o
GCC_OPTIONS = -march=i486 -m32 -fno-pic -nostdlib -fno-builtin

# Default
default :
	make img

# File gen
ipl.bin : ipl.asm Makefile
	nasm ipl.asm -o ipl.bin

asmhead.bin : asmhead.asm
	nasm asmhead.asm -o asmhead.bin

nasmfunc.o : nasmfunc.asm
	nasm -g -f elf nasmfunc.asm -o nasmfunc.o

makefont.o : makefont.c
	gcc makefont.c -o makefont.o

hankaku.c : makefont.o hankaku.txt
	./makefont.o

bootpack.bin : $(C_OBJ_FILES) har.ld nasmfunc.o
	gcc $(GCC_OPTIONS) -T har.ld $(C_OBJ_FILES) -o bootpack.bin

os.sys : asmhead.bin bootpack.bin
	cat asmhead.bin bootpack.bin > os.sys

os.img : ipl.bin os.sys
	mformat -f 1440 -C -B ipl.bin -i os.img ::
	mcopy -i os.img os.sys ::


# Commands
img : 
	make -r os.img

run : os.img
	make img
	qemu-system-i386 -fda os.img

clean :
	rm -f *.bin *.o *.sys *.img *.lst

