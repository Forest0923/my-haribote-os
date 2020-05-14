.SUFFIXES: .hrb .o

# Default
default:
	make -r os.img

os.img: haribote/ipl.bin haribote/os.sys hello3/hello3.hrb hello4/hello4.hrb hello5/hello5.hrb winhelo3/winhelo3.hrb beepdown/beepdown.hrb star1/star1.hrb stars2/stars2.hrb lines/lines.hrb color/color.hrb color2/color2.hrb walk/walk.hrb noodle/noodle.hrb sosu/sosu.hrb sosu2/sosu2.hrb typeipl/typeipl.hrb cat/cat.hrb nihongo/nihongo.fnt iroha/iroha.hrb sample_text/sjis.txt sample_text/euc.txt chklang/chklang.hrb notrec/notrec.hrb bball/bball.hrb invader/invader.hrb
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
	mcopy -i os.img sosu/sosu.hrb ::
	mcopy -i os.img sosu2/sosu2.hrb ::
	mcopy -i os.img typeipl/typeipl.hrb ::
	mcopy -i os.img cat/cat.hrb ::
	mcopy -i os.img iroha/iroha.hrb ::
	mcopy -i os.img chklang/chklang.hrb ::
	mcopy -i os.img notrec/notrec.hrb ::
	mcopy -i os.img bball/bball.hrb ::
	mcopy -i os.img invader/invader.hrb ::
	mcopy -i os.img sample_text/sjis.txt ::
	mcopy -i os.img sample_text/euc.txt ::
	mcopy -i os.img nihongo/nihongo.fnt ::

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
	make -C sosu
	make -C sosu2
	make -C typeipl
	make -C cat
	make -C iroha
	make -C chklang
	make -C notrec
	make -C bball
	make -C invader
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
	make -C sosu		clean
	make -C sosu2		clean
	make -C typeipl		clean
	make -C cat			clean
	make -C iroha		clean
	make -C chklang		clean
	make -C notrec		clean
	make -C bball		clean
	make -C invader		clean
