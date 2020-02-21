void io_hlt(void);

void HariMain(void){
fin:
	io_hlt(); /* これでnasmfunc.asmの_io_hltが実行されます */
	goto fin;
}

