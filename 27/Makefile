APP_ASM		= hello5.asm
APP_ASM_HRB	= $(APP_ASM:%.asm=%.hrb)
APP_C		= hello3.c hello4.c winhelo.c winhelo2.c winhelo3.c star1.c stars2.c lines.c walk.c beepdown.c color.c color2.c noodle.c
APP_C_HRB	= $(APP_C:%.c=%.hrb)

GCC_OPTIONS = -march=i486 -m32 -fno-pic -nostdlib -fno-builtin

.SUFFIXES: .hrb .o

# Default
default:
	make -r os.img

os.img: haribote/ipl.bin haribote/os.sys hello3/hello3.hrb hello4/hello4.hrb hello5/hello5.hrb winhelo3/winhelo3.hrb beepdown/beepdown.hrb star1/star1.hrb stars2/stars2.hrb lines/lines.hrb color/color.hrb color2/color2.hrb walk/walk.hrb noodle/noodle.hrb
	mformat -f 1440 -C -B haribote/ipl.bin -i os.img ::
	mcopy -i os.img haribote/os.sys ::
	mcopy -i os.img haribote/ipl.asm ::
	mcopy -i os.img hello3/hello3.hrb ::
	mcopy -i os.img hello4/hello4.hrb ::
	mcopy -i os.img hello5/hello5.hrb ::
	mcopy -i os.img winhelo3/winhelo3.hrb ::
	mcopy -i os.img beepdown/beepdown.hrb ::
	mcopy -i os.img star1/star1.hrb ::
	mcopy -i os.img stars2/stars2.hrb ::
	mcopy -i os.img lines/lines.hrb ::
	mcopy -i os.img walk/walk.hrb ::
	mcopy -i os.img noodle/noodle.hrb ::
	mcopy -i os.img color/color.hrb ::
	mcopy -i os.img color2/color2.hrb ::

# Commands
full:
	make -C haribote
	make -C apilib
	make -C hello3
	make -C hello4
	make -C hello5
	make -C winhelo3
	make -C beepdown
	make -C star1
	make -C stars2
	make -C lines
	make -C walk
	make -C noodle
	make -C color
	make -C color2
	make os.img

clean:
	make -C haribote	clean
	make -C apilib		clean
	make -C hello3		clean
	make -C hello4		clean
	make -C hello5		clean
	make -C winhelo3	clean
	make -C beepdown	clean
	make -C star1		clean
	make -C stars2		clean
	make -C lines		clean
	make -C walk		clean
	make -C noodle		clean
	make -C color		clean
	make -C color2		clean
	
