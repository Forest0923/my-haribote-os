C_OBJ_FILES = bootpack.c graphic.c dsctbl.c int.c fifo.c keyboard.c mouse.c memory.c sheet.c timer.c mtask.c window.c console.c file.c hankaku.c nasmfunc.o

GCC_OPTIONS = -march=i486 -m32 -fno-pic -nostdlib -fno-builtin

# Default
default :
	make ipl.bin
	make os.sys

# Font
makefont.o : makefont.c
	gcc makefont.c -o makefont.o

hankaku.c : makefont.o hankaku.txt
	./makefont.o


# File gen
ipl.bin : ipl.asm Makefile
	nasm ipl.asm -o ipl.bin

asmhead.bin : asmhead.asm
	nasm asmhead.asm -o asmhead.bin

nasmfunc.o : nasmfunc.asm
	nasm -g -f elf nasmfunc.asm -o nasmfunc.o

bootpack.bin : $(C_OBJ_FILES) har.ld
	gcc $(GCC_OPTIONS) -T har.ld $(C_OBJ_FILES) -o bootpack.bin

os.sys : asmhead.bin bootpack.bin
	cat asmhead.bin bootpack.bin > os.sys

clean :
	rm -f *.bin *.o *.sys

