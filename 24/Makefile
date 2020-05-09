C_OBJ_FILES = bootpack.c graphic.c dsctbl.c int.c fifo.c keyboard.c mouse.c memory.c sheet.c timer.c mtask.c window.c console.c file.c hankaku.c nasmfunc.o
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

a_nasm.o: a_nasm.asm
	nasm -g -f elf a_nasm.asm -o a_nasm.o

stars2.hrb: stars2.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld stars2.c a_nasm.o -o stars2.hrb

lines.hrb: lines.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld lines.c a_nasm.o -o lines.hrb

walk.hrb: walk.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld walk.c a_nasm.o -o walk.hrb

noodle.hrb: noodle.c a_nasm.o noodle.ld
	gcc $(GCC_OPTIONS) -T noodle.ld noodle.c a_nasm.o -o noodle.hrb

bootpack.bin : $(C_OBJ_FILES) har.ld
	gcc $(GCC_OPTIONS) -T har.ld $(C_OBJ_FILES) -o bootpack.bin

os.sys : asmhead.bin bootpack.bin
	cat asmhead.bin bootpack.bin > os.sys

os.img : ipl.bin os.sys stars2.hrb lines.hrb walk.hrb noodle.hrb
	mformat -f 1440 -C -B ipl.bin -i os.img ::
	mcopy -i os.img os.sys ::
	mcopy -i os.img ipl.asm ::
	mcopy -i os.img stars2.hrb ::
	mcopy -i os.img lines.hrb ::
	mcopy -i os.img walk.hrb ::
	mcopy -i os.img noodle.hrb ::

# Commands
img : 
	make -r os.img

run : os.img
	make img
	qemu-system-i386 -fda os.img

clean :
	rm -f *.bin *.o *.sys *.lst *.hrb

