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

a_nasm.o: a_nasm.asm
	nasm -g -f elf a_nasm.asm -o a_nasm.o

a.hrb: a.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld a.c a_nasm.o -o a.hrb

hello3.hrb: hello3.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld hello3.c a_nasm.o -o hello3.hrb

hello4.hrb: hello4.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld hello4.c a_nasm.o -o hello4.hrb

hello5.o: hello5.asm
	nasm -g -f elf hello5.asm -o hello5.o

hello5.hrb: hello5.o a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld hello5.o a_nasm.o -o hello5.hrb
	# ld -m elf_i386 -e HariMain -Tapi.ld hello5.o a_nasm.o -o hello5.hrb

winhelo.hrb: winhelo.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld winhelo.c a_nasm.o -o winhelo.hrb

winhelo2.hrb: winhelo2.c a_nasm.o api.ld
	gcc $(GCC_OPTIONS) -T api.ld winhelo2.c a_nasm.o -o winhelo2.hrb

bootpack.bin : $(C_OBJ_FILES) har.ld
	gcc $(GCC_OPTIONS) -T har.ld $(C_OBJ_FILES) -o bootpack.bin

os.sys : asmhead.bin bootpack.bin
	cat asmhead.bin bootpack.bin > os.sys

os.img : ipl.bin os.sys a.hrb hello3.hrb hello4.hrb hello5.hrb winhelo.hrb winhelo2.hrb
	mformat -f 1440 -C -B ipl.bin -i os.img ::
	mcopy -i os.img os.sys ::
	mcopy -i os.img ipl.asm ::
	mcopy -i os.img a.hrb ::
	mcopy -i os.img hello3.hrb ::
	mcopy -i os.img hello4.hrb ::
	mcopy -i os.img hello5.hrb ::
	mcopy -i os.img winhelo.hrb ::
	mcopy -i os.img winhelo2.hrb ::

# Commands
img : 
	make -r os.img

run : os.img
	make img
	qemu-system-i386 -fda os.img

clean :
	rm -f *.bin *.o *.sys *.lst *.hrb

