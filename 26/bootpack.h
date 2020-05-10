/*
 * asmhead.asm
 */
struct BOOTINFO {       /* 0x0ff0-0x0fff */
    char cyls;          /* How far the boot sector read the disk */
    char leds;          /* Keyboard status                       */
    char vmode;         /* Video mode (n bit color)              */
    char reserve;       
    short scrnx, scrny; /* Resolutions */
    char *vram;
};
#define ADR_BOOTINFO    0x00000ff0
#define ADR_DISKIMG     0x00100000


/*
 * nasmfunc.asm
 */
void io_hlt(void);
void io_cli(void);  /* CLI: Clear interrupt flag, STI: Set interrupt flag */
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
int io_in16(int port);
int io_in32(int port);
void io_out8(int port, int data);
void io_out16(int port, int data);
void io_out32(int port, int data);
int  io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
int load_cr0();
void load_tr(int tr);
void store_cr0(int cr0);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void asm_hrb_api();
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app();

/*
 * graphic.c
 */
#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int xsize, int ysize);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);


/*
 * dsctbl.c
 */
struct SEGMENT_DESCRIPTOR {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT         0x0026f800
#define LIMIT_IDT       0x000007ff
#define ADR_GDT         0x00270000
#define LIMIT_GDT       0x0000ffff
#define ADR_BOTPAK      0x00280000
#define LIMIT_BOTPAK    0x0007ffff
#define AR_DATA32_RW    0x4092
#define AR_CODE32_ER    0x409a
#define AR_TSS32        0x0089
#define AR_INTGATE32    0x008e

/*
 * mysprintf.c
 */
// void sprintf(char *str, char *fmt, ...);

/*
 * mystrcmp.c
 */
// int strcmp(char *str1, char *str2);
// int strncmp(char *str1, char *str2, int n);

/*
 * int.c
 */
void init_pic(void);
void inthandler27(int *esp);
#define PIC0_ICW1   0x0020
#define PIC0_OCW2   0x0020
#define PIC0_IMR    0x0021
#define PIC0_ICW2   0x0021
#define PIC0_ICW3   0x0021
#define PIC0_ICW4   0x0021
#define PIC1_ICW1   0x00a0
#define PIC1_OCW2   0x00a0
#define PIC1_IMR    0x00a1
#define PIC1_ICW2   0x00a1
#define PIC1_ICW3   0x00a1
#define PIC1_ICW4   0x00a1


/*
 * fifo.c
 */
struct FIFO32 {
    int *buf;
    int putp;   /* Put position   */
    int getp;   /* Get position   */
    int size;   /* Queue size     */
    int free;   /* Available area */
    int flags;  /* Overflow flag  */
    struct TASK *task;
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);


/*
 * keyboard.c
 */
#define PORT_KEYDAT             0x0060
#define PORT_KEYCMD             0x0064
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32 *fifo, int data0);
extern struct FIFO32 *keyfifo;


/*
 * mouse.c
 */
struct MOUSE_DEC {
    unsigned char buf[3], phase;
    int x, y, btn;
};
void inthandler2c(int *esp);
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
extern struct FIFO32 *mousefifo;


/*
 * memory.c
 */
unsigned int memtest_sub(unsigned int start, unsigned int end);
unsigned int memtest(unsigned int start, unsigned int end);

#define MEMMAN_FREES    4090
#define MEMMAN_ADDR     0x003c0000
struct FREEINFO {
    unsigned int addr, size;
};
struct MEMMAN {
    int frees;      /* Number of free information        */
    int maxfrees;   /* Max number of frees               */
    int lostsize;   /* Total size that failed to release */
    int losts;      /* Number of times release failed    */
    struct FREEINFO free[MEMMAN_FREES];
};
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);


/*
 * sheet.c
 */
#define MAX_SHEETS  256
struct SHEET {
    unsigned char *buf;
    int bxsize, bysize;     /* Area is bxsize*bysize */
    int vx0, vy0;           /* Coordinates           */
    int col_inv;            /* Color and invisible   */
    int height;
    int flags;              /* Setting parameters    */
    struct SHTCTL *ctl;
    struct TASK *task;
};
struct SHTCTL {
    unsigned char *vram, *map;
    int xsize, ysize;
    int top;                            /* Height of top sheet */
    struct SHEET *sheets[MAX_SHEETS];   /* Sorted by height    */
    struct SHEET sheets0[MAX_SHEETS];   /* Unsorted            */
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/*
 * timer.c
 */
#define MAX_TIMER   500
struct TIMER {
    unsigned int timeout, flags, flags2;
    struct FIFO32 *fifo;
    int data;
    struct TIMER *next;
};
struct TIMERCTL {
    unsigned int count, next;
    struct TIMER *t0;
    struct TIMER timers0[MAX_TIMER];
};
extern struct TIMERCTL timerctl;
void init_pit();
struct TIMER *timer_alloc();
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);
void timer_adjust();
int timer_cancel(struct TIMER *timer);
void timer_cancelall(struct FIFO32 *fifo);

/*
 * mtask.c
 */
#define MAX_TASKS       1000
#define TASK_GDT0       3       /* From where to assign TSS in GDT */
#define MAX_TASKS_LV    100
#define MAX_TASKLEVELS  10

/* Task status segment */
struct TSS32 {
    int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    int es, cs, ss, ds, fs, gs;
    int ldtr, iomap;
};
struct TASK {
    int sel;    /* Number of GDT */
    int flags;
    int level;
    int priority;
    struct FIFO32 fifo;
    struct TSS32 tss;
    struct CONSOLE *cons;
    int ds_base, cons_stack;
};
struct TASKLEVEL {
    int running;    /* Running task number */
    int now;        /* Current task */
    struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
    int now_lv;
    char lv_change;     /* whether change level or not */
    struct TASKLEVEL level[MAX_TASKLEVELS];
    struct TASK tasks0[MAX_TASKS];
};
extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;
struct TASK *task_now();
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc();
void task_run(struct TASK *task, int level, int priority);
void task_switch();
void task_sleep(struct TASK *task);

/*
 * window.c
 */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);
void change_wtitle8(struct SHEET *sht, char act);

/* 
 * console.c
 */
struct CONSOLE {
    struct SHEET *sht;
    int cur_x, cur_y, cur_c;
    struct TIMER *timer;
};
void console_task(struct SHEET *sheet, unsigned int memtotal);
void cons_putchar(struct CONSOLE *cons, int chr, char move);
void cons_newline(struct CONSOLE *cons);
void cons_putstr0(struct CONSOLE *cons, char *s);
void cons_putstr1(struct CONSOLE *cons, char *s, int l);
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);
void cmd_clear(struct CONSOLE *cons);
void cmd_ls(struct CONSOLE *cons);
void cmd_cat(struct CONSOLE *cons, int *fat, char *cmdline);
void cmd_exit(struct CONSOLE *cons, int *fat);
void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal);
void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);
int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
int *inthandler0d(int *esp);
int *inthandler0c(int *esp);
void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);

/*
 * file.c
 */
struct FILEINFO {
    unsigned char name[8], ext[3], type;
    char reserve[10];
    unsigned short time, date, clustno;
    unsigned int size;
};
void file_readfat(int *fat, unsigned char *img);
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);

/*
 * bootpack.c
 */
struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal);
struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal);
