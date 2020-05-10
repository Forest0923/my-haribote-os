#include "bootpack.h"

#define KEYCMD_LED  0xed

void keywin_off(struct SHEET *key_win) {
    change_wtitle8(key_win, 0);
    if ((key_win->flags & 0x20) != 0) {
        fifo32_put(&key_win->task->fifo, 3); /* コンソールのカーソルOFF */
    }
    return;
}

void keywin_on(struct SHEET *key_win) {
    change_wtitle8(key_win, 1);
    if ((key_win->flags & 0x20) != 0) {
        fifo32_put(&key_win->task->fifo, 2); /* コンソールのカーソルON */
    }
    return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal) {
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct TASK *task = task_alloc();
    int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
    task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
    task->tss.esp = task->cons_stack + 64 * 1024 - 12;
    task->tss.eip = (int) &console_task;
    task->tss.es = 1 * 8;
    task->tss.cs = 2 * 8;
    task->tss.ss = 1 * 8;
    task->tss.ds = 1 * 8;
    task->tss.fs = 1 * 8;
    task->tss.gs = 1 * 8;
    *((int *) (task->tss.esp + 4)) = (int) sht;
    *((int *) (task->tss.esp + 8)) = memtotal;
    task_run(task, 2, 2); /* level=2, priority=2 */
    fifo32_init(&task->fifo, 128, cons_fifo, task);
    return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal) {
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct SHEET *sht = sheet_alloc(shtctl);
    unsigned char *buf = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
    sheet_setbuf(sht, buf, 256, 165, -1); /* 透明色なし */
    make_window8(buf, 256, 165, "console", 0);
    make_textbox8(sht, 8, 28, 240, 128, COL8_000000);
    sht->task = open_constask(sht, memtotal);
    sht->flags |= 0x20;	/* カーソルあり */
    return sht;
}

void close_constask(struct TASK *task) {
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    task_sleep(task);
    memman_free_4k(memman, task->cons_stack, 64 * 1024);
    memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
    task->flags = 0; /* task_free(task); の代わり */
    return;
}

void close_console(struct SHEET *sht) {
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct TASK *task = sht->task;
    memman_free_4k(memman, (int) sht->buf, 256 * 165);
    sheet_free(sht);
    close_constask(task);
    return;
}

void HariMain(void) {
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    struct SHTCTL *shtctl;
    char s[40];
    struct FIFO32 fifo, keycmd;
    int fifobuf[128], keycmd_buf[32];
    int mx, my, i, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
    unsigned int memtotal;
    struct MOUSE_DEC mdec;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    unsigned char *buf_back, buf_mouse[256];
    struct SHEET *sht_back, *sht_mouse;
    struct TASK *task_a, *task;
    static char keytable0[0x80] = {
        0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0x08, 0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0x0a, 0, 'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
    };
    static char keytable1[0x80] = {
        0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0x08, 0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
    };
    int key_shift = 0, key_ctrl = 0, key_alt = 0;
    int key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
    struct CONSOLE *cons;
    int j, x, y, mmx = -1, mmy = -1, mmx2 = 0;
    struct SHEET *sht = 0, *key_win;

    init_gdtidt();
    init_pic();
    io_sti();
    fifo32_init(&fifo, 128, fifobuf, 0);
    *((int *) 0x0fec) = (int) &fifo;
    init_pit();
    init_keyboard(&fifo, 256);
    enable_mouse(&fifo, 512, &mdec);
    io_out8(PIC0_IMR, 0xf8);    /* Permit PIT, PIC1, keyboard */
    io_out8(PIC1_IMR, 0xef);    /* Permit mouse */
    fifo32_init(&keycmd, 32, keycmd_buf, 0);

    memtotal = memtest(0x00400000, 0xbfffffff);
    memman_init(memman);
    memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
    memman_free(memman, 0x00400000, memtotal - 0x00400000);

    init_palette();
    shtctl    = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
    task_a    = task_init(memman);
    fifo.task = task_a;
    task_run(task_a, 1, 0);
    *((int *) 0x0fe4) = (int) shtctl;

    /* sht_back */
    sht_back  = sheet_alloc(shtctl);
    buf_back  = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
    sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
    init_screen8(buf_back, binfo->scrnx, binfo->scrny);

    /* sht_cons */
    key_win = open_console(shtctl, memtotal);

    /* sht_mouse */
    sht_mouse = sheet_alloc(shtctl);
    sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
    init_mouse_cursor8(buf_mouse, 99);
    mx = (binfo->scrnx - 16) / 2;   /* Make mouse cursor center */
    my = (binfo->scrny - 28 - 16) / 2;
    
    sheet_slide(sht_back,   0,  0);
    sheet_slide(key_win,    32, 4);
    sheet_slide(sht_mouse,  mx, my);
    sheet_updown(sht_back,  0);
    sheet_updown(key_win,   1);
    sheet_updown(sht_mouse, 2);
    keywin_on(key_win);

    fifo32_put(&keycmd, KEYCMD_LED);
    fifo32_put(&keycmd, key_leds);

    for (;;) {
        if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
            keycmd_wait = fifo32_get(&keycmd);
            wait_KBC_sendready();
            io_out8(PORT_KEYDAT, keycmd_wait);
        }
        /* Reset timer each year */
        if (timerctl.count >= 0xbbf81e00 /* 0x1f4 */) {
            timer_adjust();
        }
        io_cli();
        if (fifo32_status(&fifo) == 0) {
            /* FIFOがからっぽになったので、保留している描画があれば実行する */
            if (new_mx >= 0) {
                io_sti();
                sheet_slide(sht_mouse, new_mx, new_my);
                new_mx = -1;
            } else if (new_wx != 0x7fffffff) {
                io_sti();
                sheet_slide(sht, new_wx, new_wy);
                new_wx = 0x7fffffff;
            } else {
                task_sleep(task_a);
                io_sti();
            }
        } else {
            i = fifo32_get(&fifo);
            io_sti();
            if (key_win != 0 && key_win->flags == 0) {	/* ウィンドウが閉じられた */
                if (shtctl->top == 1) {	/* もうマウスと背景しかない */
                    key_win = 0;
                } else {
                    key_win = shtctl->sheets[shtctl->top - 1];
                    keywin_on(key_win);
                }
            }
            /* Keyboard */
            if (256 <= i && i <= 511) {
                if (i < 0x80 + 256) {
                    if (key_shift == 0) {
                        s[0] = keytable0[i - 256];
                    } else {
                        s[0] = keytable1[i - 256];
                    }
                } else {
                    s[0] = 0;
                }

                /* Alphabet */
                if ('A' <= s[0] && s[0] <= 'Z') {
                    if (((key_leds & 4) == 0 && key_shift == 0) || ((key_leds & 4) != 0 && key_shift != 0)) {
                        s[0] += 0x20;
                    }
                }
                /* Genaral char, back space, enter */
                if (s[0] != 0 && key_win != 0 && key_ctrl == 0 && key_alt == 0) {
                    /* To the console */
                    fifo32_put(&key_win->task->fifo, s[0] + 256);
                }
                
                /* Tab */
                if (i == 256 + 0x0f && key_win != 0) {
                    keywin_off(key_win);
                    j = key_win->height - 1;
                    if (j == 0) {
                        j = shtctl->top - 1;
                    }
                    key_win = shtctl->sheets[j];
                    keywin_on(key_win);
                }

                /* L-Shift ON */
                if (i == 256 + 0x2a) {
                    key_shift |= 1;
                }
                /* L-Shift OFF */
                if (i == 256 + 0xaa) {
                    key_shift &= ~1;
                }
                /* R-Shift ON */
                if (i == 256 + 0x36) {
                    key_shift |= 2;
                }
                /* R-Shift OFF */
                if (i == 256 + 0xb6) {
                    key_shift &= ~2;
                }
                /* L-Ctrl ON */
                if (i == 256 + 0x1d) {
                    key_ctrl |= 1;
                }
                /* L-Ctrl OFF */
                if (i == 256 + 0x9d) {
                    key_ctrl &= ~1;
                }
                /* L-Alt ON */
                if (i == 256 + 0x38) {
                    key_alt |= 1;
                }
                /* L-Alt OFF */
                if (i == 256 + 0xb8) {
                    key_alt &= ~1;
                }
                /* CapsLock */
                if (i == 256 + 0x3a) {
                    key_leds ^= 4;
                    fifo32_put(&keycmd, KEYCMD_LED);
                    fifo32_put(&keycmd, key_leds);
                }
                /* NumLock */
                if (i == 256 + 0x45) {
                    key_leds ^= 2;
                    fifo32_put(&keycmd, KEYCMD_LED);
                    fifo32_put(&keycmd, key_leds);
                }
                /* ScrollLock */
                if (i == 256 + 0x46) {
                    key_leds ^= 1;
                    fifo32_put(&keycmd, KEYCMD_LED);
                    fifo32_put(&keycmd, key_leds);
                }
                /* Kill (ctrl + C) */
                if (i == 256 + 0x2e && key_ctrl != 0 && key_win != 0) {
                    task = key_win->task;
                    if (task != 0 && task->tss.ss0 != 0) {
                        cons_putstr0(task->cons, "\nBreak(key) :\n");
                        io_cli();	/* 強制終了処理中にタスクが変わると困るから */
                        task->tss.eax = (int) &(task->tss.esp0);
                        task->tss.eip = (int) asm_end_app;
                        io_sti();
                    }
                }
                /* Open new console (ctrl + alt + T) */
                if (i == 256 + 0x14 && key_ctrl != 0 && key_alt != 0) {
                    if (key_win != 0) {
                        keywin_off(key_win);
                    }
                    key_win = open_console(shtctl, memtotal);
                    sheet_slide(key_win, 32, 4);
                    sheet_updown(key_win, shtctl->top);
                    keywin_on(key_win);
                }
                /* F11 */
                if (i == 256 + 0x57 && shtctl->top > 2) {
                    sheet_updown(shtctl->sheets[1], shtctl->top - 1);
                }
                /* Keyboard received data */
                if (i == 256 + 0xfa) {
                    keycmd_wait = -1;
                }
                /* Keyboard failed to receive data */
                if (i == 256 + 0xfe) {
                    wait_KBC_sendready();
                    io_out8(PORT_KEYDAT, keycmd_wait);
                }
            }
            /* Mouse */
            else if (512 <= i && i <= 767) {
                if (mouse_decode(&mdec, i - 512) != 0) {
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
                    new_mx = mx;
                    new_my = my;
                    if ((mdec.btn & 0x01) != 0) {
                        if (mmx < 0) {
                            /* 通常モードの場合 */
                            /* 上の下じきから順番にマウスが指している下じきを探す */
                            for (j = shtctl->top - 1; j > 0; j--) {
                                sht = shtctl->sheets[j];
                                x = mx - sht->vx0;
                                y = my - sht->vy0;
                                if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
                                    if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {
                                        sheet_updown(sht, shtctl->top - 1);
                                        if (sht != key_win) {
                                            keywin_off(key_win);
                                            key_win = sht;
                                            keywin_on(key_win);
                                        }
                                        if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
                                            mmx = mx;	/* ウィンドウ移動モードへ */
                                            mmy = my;
                                            mmx2 = sht->vx0;
                                            new_wy = sht->vy0;
                                        }
                                        if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
                                            /* 「×」ボタンクリック */
                                            if ((sht->flags & 0x10) != 0) {	/* アプリが作ったウィンドウか？ */
                                                task = sht->task;
                                                cons_putstr0(task->cons, "\nBreak(mouse) :\n");
                                                io_cli();	/* 強制終了処理中にタスクが変わると困るから */
                                                task->tss.eax = (int) &(task->tss.esp0);
                                                task->tss.eip = (int) asm_end_app;
                                                io_sti();
                                            } else {
                                                task = sht->task;
                                                io_cli();
                                                fifo32_put(&task->fifo, 4);
                                                io_sti();
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                        } else {
                            /* ウィンドウ移動モードの場合 */
                            x = mx - mmx;	/* マウスの移動量を計算 */
                            y = my - mmy;
                            new_wx = (mmx2 + x + 2) & ~3;
                            new_wy = new_wy + y;
                            mmy = my;
                        }
                    } else {
                        /* 左ボタンを押していない */
                        mmx = -1;	/* 通常モードへ */
                        if (new_wx != 0x7fffffff) {
                            sheet_slide(sht, new_wx, new_wy);
                            new_wx = 0x7fffffff;
                        }
                    }
                }
            }
            /* Exit console */
            else if (768 <= i && i <= 1023) {
                close_console(shtctl->sheets0 + (i - 768));
            } else if (1024 <= i && i <= 2023) {
                close_constask(taskctl->tasks0 + (i - 1024));
            }
        }
    }
}

