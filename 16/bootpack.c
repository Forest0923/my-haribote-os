#include "bootpack.h"

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l) {
    boxfill8(sht->buf, sht->bxsize, b, x, y, x + l * 8 - 1, y + 15);
    putfonts8_asc(sht->buf, sht->bxsize, x, y, c, s);
    sheet_refresh(sht, x, y, x + l * 8, y + 16);
    return;
}

void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c) {
    int x1 = x0 + sx;
    int y1 = y0 + sy;
    boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
    boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
    boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
    boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
    boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
    boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
    boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
    boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
    boxfill8(sht->buf, sht->bxsize, c,           x0 - 1, y0 - 1, x1 + 0, y1 + 0);
    return;
}


void task_b_main(struct SHEET *sht_win_b) {
    struct FIFO32 fifo;
    struct TIMER *timer_1s;
    int i, fifobuf[128], count = 0, count0 = 0;
    char s[12];

    fifo32_init(&fifo, 128, fifobuf, 0);
    timer_1s = timer_alloc();
    timer_init(timer_1s, &fifo, 100);
    timer_settime(timer_1s, 100);

    for (;;) {
        count++;
        io_cli();
        if (fifo32_status(&fifo) == 0) {
            io_sti();
        } else {
            i = fifo32_get(&fifo);
            io_sti();
            if (i == 100) {
                sprintf(s, "%d", count - count0);
                putfonts8_asc_sht(sht_win_b, 24, 28, COL8_000000, COL8_C6C6C6, s, 11);
                count0 = count;
                timer_settime(timer_1s, 100);
            }
        }
    }
}

void HariMain(void) {
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    struct FIFO32 fifo;
    char s[40];
    int fifobuf[128];
    struct TIMER *timer;
    int mx, my, i, cursor_x, cursor_c, task_b_esp;
    unsigned int memtotal;
    unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_win_b;
    struct MOUSE_DEC mdec;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct SHTCTL *shtctl;
    struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_win_b[3];
    static char keytable[0x54] = {
        0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.'
    };
    struct TASK *task_a, *task_b[3];

    init_gdtidt();
    init_pic();
    io_sti();
    fifo32_init(&fifo, 128, fifobuf, 0);
    init_pit();
    init_keyboard(&fifo, 256);
    enable_mouse(&fifo, 512, &mdec);
    io_out8(PIC0_IMR, 0xf8);    /* Permit PIT, PIC1, keyboard */
    io_out8(PIC1_IMR, 0xef);    /* Permit mouse */

    memtotal = memtest(0x00400000, 0xbfffffff);
    memman_init(memman);
    memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
    memman_free(memman, 0x00400000, memtotal - 0x00400000);

    init_palette();
    shtctl    = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
    task_a    = task_init(memman);
    fifo.task = task_a;
    task_run(task_a, 1, 0);

    /* sht_back */
    sht_back  = sheet_alloc(shtctl);
    buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
    sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
    init_screen8(buf_back, binfo->scrnx, binfo->scrny);

    /* sht_win_b */
    for (i=0; i<3; i++) {
        sht_win_b[i] = sheet_alloc(shtctl);
        buf_win_b = (unsigned char *) memman_alloc_4k(memman, 144 * 52);
        sheet_setbuf(sht_win_b[i], buf_win_b, 144, 52, -1);
        sprintf(s, "task%d", i);
        make_window8(buf_win_b, 144, 52, s, 0);
        task_b[i] = task_alloc();
        task_b[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
        task_b[i]->tss.eip = (int) &task_b_main;
        task_b[i]->tss.es = 1 * 8;
        task_b[i]->tss.cs = 2 * 8;
        task_b[i]->tss.ss = 1 * 8;
        task_b[i]->tss.ds = 1 * 8;
        task_b[i]->tss.fs = 1 * 8;
        task_b[i]->tss.gs = 1 * 8;
        *((int *) (task_b[i]->tss.esp + 4)) = (int) sht_win_b[i];
        task_run(task_b[i], 2, i+1);
    }

    /* sht_win */
    sht_win   = sheet_alloc(shtctl);
    buf_win   = (unsigned char *) memman_alloc_4k(memman, 160 * 52);
    sheet_setbuf(sht_win, buf_win, 160, 52, -1);
    make_window8(buf_win, 160, 52, "Window", 1);
    make_textbox8(sht_win, 8, 28, 144, 16, COL8_FFFFFF);
    cursor_x = 8;
    cursor_c = COL8_FFFFFF;
    
    timer = timer_alloc();
    timer_init(timer, &fifo, 1);
    timer_settime(timer, 50);
    
    /* sht_mouse */
    sht_mouse = sheet_alloc(shtctl);
    sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
    init_mouse_cursor8(buf_mouse, 99);
    mx = (binfo->scrnx - 16) / 2;   /* Make mouse cursor center */
    my = (binfo->scrny - 28 - 16) / 2;
    
    sheet_slide(sht_back, 0, 0);
    sheet_slide(sht_win_b[0], 168,  56);
    sheet_slide(sht_win_b[1],   8, 116);
    sheet_slide(sht_win_b[2], 168, 116);
    sheet_slide(sht_win, 80, 72);
    sheet_slide(sht_mouse, mx, my);
    sheet_updown(sht_back,     0);
    sheet_updown(sht_win_b[0], 1);
    sheet_updown(sht_win_b[1], 2);
    sheet_updown(sht_win_b[2], 3);
    sheet_updown(sht_win,      4);
    sheet_updown(sht_mouse,    5);
    sprintf(s, "(%d, %d)", mx, my);
    putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
    sprintf(s, "memory: %dMB  free: %dKB", memtotal / (1024*1024), memman_total(memman) / 1024);
    putfonts8_asc_sht(sht_back, 0, 32, COL8_FFFFFF, COL8_008484, s, 40);

    for (;;) {
        /* Reset timer each year */
        if (timerctl.count >= 0xbbf81e00 /* 0x1f4 */) {
            timer_adjust();
        }
        io_cli();
        if (fifo32_status(&fifo) == 0) {
            task_sleep(task_a);
            io_sti();
        } else {
            i = fifo32_get(&fifo);
            io_sti();
            /* Keyboard */
            if (256 <= i && i <= 511) {
                sprintf(s, "%x", i - 256);
                putfonts8_asc_sht(sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
                if (i < 0x54 + 256) {
                    if (keytable[i-256] != 0 && cursor_x < 144) {
                        s[0] = keytable[i-256];
                        s[1] = 0;
                        putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
                        cursor_x += 8;
                    }
                }
                /* Back space */
                if (i == 256 + 0x0e && cursor_x > 8) {
                    putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
                    cursor_x -= 8;
                }
                boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
            }
            /* Mouse */
            else if (512 <= i && i <= 767) {
                if (mouse_decode(&mdec, i - 512) != 0) {
                    sprintf(s, "[lcr %d %d]", mdec.x, mdec.y);
                    if ((mdec.btn & 0x01) != 0) {
                        s[1] = 'L';
                    }
                    if ((mdec.btn & 0x02) != 0) {
                        s[3] = 'R';
                    }
                    if ((mdec.btn & 0x04) != 0) {
                        s[2] = 'C';
                    }
                    putfonts8_asc_sht(sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);

                    /* Mouse cursor */
                    mx += mdec.x;
                    my += mdec.y;
                    if (mx < 0) {
                        mx = 0;
                    }
                    if (my < 0) {
                        my = 0;
                    }
                    if (mx > binfo->scrnx - 1) {
                        mx = binfo->scrnx - 1;
                    }
                    if (my > binfo->scrny - 1) {
                        my = binfo->scrny - 1;
                    }
                    sprintf(s, "(%d, %d)", mx, my);
                    putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
                    sheet_slide(sht_mouse, mx, my);
                    if ((mdec.btn & 0x01) != 0) {
                        sheet_slide(sht_win, mx - 80, my - 8);
                    }
                }
            }
            /* Cursor */
            else if (i <= 1) {
                if (i != 0) {
                    timer_init(timer, &fifo, 0);
                    cursor_c = COL8_000000;
                }
                else {
                    timer_init(timer, &fifo, 1);
                    cursor_c = COL8_FFFFFF;
                }
                timer_settime(timer, 50);
                boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
            }
        }
    }
}

