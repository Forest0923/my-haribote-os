C_OBJ_FILES = bootpack.c graphic.c dsctbl.c int.c fifo.c keyboard.c mouse.c memory.c sheet.c timer.c mtask.c window.c console.c file.c hankaku.c mysprintf.c mystrcmp.c nasmfunc.o
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

hello.hrb: hello.asm
	nasm hello.asm -o hello.hrb

hello2.hrb: hello2.asm
	nasm hello2.asm -o hello2.hrb

a_nasm.o: a_nasm.asm
	nasm -g -f elf a_nasm.asm -o a_nasm.o

a.hrb: a.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld a.c a_nasm.o -o a.hrb

hello3.hrb: hello3.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld hello3.c a_nasm.o -o hello3.hrb

crack1.hrb: crack1.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld crack1.c a_nasm.o -o crack1.hrb

crack2.hrb: crack2.asm
	nasm -g -f elf crack2.asm -o crack2.hrb

bootpack.bin : $(C_OBJ_FILES) har.ld
	gcc $(GCC_OPTIONS) -T har.ld $(C_OBJ_FILES) -o bootpack.bin

os.sys : asmhead.bin bootpack.bin
	cat asmhead.bin bootpack.bin > os.sys

os.img : ipl.bin os.sys hello.hrb hello2.hrb a.hrb hello3.hrb crack1.hrb crack2.hrb
	mformat -f 1440 -C -B ipl.bin -i os.img ::
	mcopy -i os.img os.sys ::
	mcopy -i os.img ipl.asm ::
	mcopy -i os.img hello.hrb ::
	mcopy -i os.img hello2.hrb ::
	mcopy -i os.img a.hrb ::
	mcopy -i os.img hello3.hrb ::
	mcopy -i os.img crack1.hrb ::
	mcopy -i os.img crack2.hrb ::

# Commands
img : 
	make -r os.img

run : os.img
	make img
	qemu-system-i386 -fda os.img

clean :
	rm -f *.bin *.o *.sys *.lst *.hrb

