#include "bootpack.h"

#define KEYCMD_LED  0xed

int keywin_off(struct SHEET *key_win, struct SHEET *sht_win, int cur_c, int cur_x) {
    change_wtitle8(key_win, 0);
    if (key_win == sht_win) {
        cur_c = -1; /* カーソルを消す */
        boxfill8(sht_win->buf, sht_win->bxsize, COL8_FFFFFF, cur_x, 28, cur_x + 7, 43);
    } else {
        if ((key_win->flags & 0x20) != 0) {
            fifo32_put(&key_win->task->fifo, 3); /* コンソールのカーソルOFF */
        }
    }
    return cur_c;
}

int keywin_on(struct SHEET *key_win, struct SHEET *sht_win, int cur_c) {
    change_wtitle8(key_win, 1);
    if (key_win == sht_win) {
        cur_c = COL8_000000; /* カーソルを出す */
    } else {
        if ((key_win->flags & 0x20) != 0) {
            fifo32_put(&key_win->task->fifo, 2); /* コンソールのカーソルON */
        }
    }
    return cur_c;
}


void HariMain(void) {
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    char s[40];
    struct FIFO32 fifo, keycmd;
    int fifobuf[128], keycmd_buf[32];
    int mx, my, i, cursor_x, cursor_c;
    int key_to = 0, key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
    int key_ctrl = 0;
    struct TIMER *timer;
    unsigned int memtotal;
    unsigned char *buf_back, buf_mouse[256], *buf_win, *buf_cons;
    struct MOUSE_DEC mdec;
    struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
    struct SHTCTL *shtctl;
    struct SHEET *sht_back, *sht_mouse, *sht_win, *sht_cons;
    static char keytable0[0x54] = {
        0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.'
    };
    static char keytable1[0x80] = {
        0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
    };
    struct TASK *task_a, *task_cons;
    struct CONSOLE *cons;
    int j, x, y, mmx = -1, mmy = -1;
    struct SHEET *sht = 0, *key_win;

    init_gdtidt();
    init_pic();
    io_sti();
    fifo32_init(&fifo, 128, fifobuf, 0);
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
    sht_cons = sheet_alloc(shtctl);
    buf_cons = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
    sheet_setbuf(sht_cons, buf_cons, 256, 165, -1); /* 透明色なし */
    make_window8(buf_cons, 256, 165, "console", 0);
    make_textbox8(sht_cons, 8, 28, 240, 128, COL8_000000);
    task_cons = task_alloc();
    task_cons->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
    task_cons->tss.eip = (int) &console_task;
    task_cons->tss.es = 1 * 8;
    task_cons->tss.cs = 2 * 8;
    task_cons->tss.ss = 1 * 8;
    task_cons->tss.ds = 1 * 8;
    task_cons->tss.fs = 1 * 8;
    task_cons->tss.gs = 1 * 8;
    *((int *) (task_cons->tss.esp + 4)) = (int) sht_cons;
    *((int *) (task_cons->tss.esp + 8)) = memtotal;
    task_run(task_cons, 2, 2); /* level=2, priority=2 */

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
    
    sheet_slide(sht_back,  0,  0);
    sheet_slide(sht_cons, 32,  4);
    sheet_slide(sht_win,  64, 56);
    sheet_slide(sht_mouse, mx, my);
    sheet_updown(sht_back,     0);
    sheet_updown(sht_cons,     1);
    sheet_updown(sht_win,      4);
    sheet_updown(sht_mouse,    5);
    key_win = sht_win;
    sht_cons->task = task_cons;
    sht_cons->flags |= 0x20;

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
            task_sleep(task_a);
            io_sti();
        } else {
            i = fifo32_get(&fifo);
            io_sti();
            if (key_win->flags == 0) {
                key_win = shtctl->sheets[shtctl->top - 1];
                cursor_c = keywin_on(key_win, sht_win, cursor_c);
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
                /* Genaral char */
                if (s[0] != 0) {
                    /* To the task_a window */
					if (key_win == sht_win) {
						if (cursor_x < 128) {
							s[1] = 0;
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
							cursor_x += 8;
						}
					}
                    /* To the console */
                    else {
						fifo32_put(&key_win->task->fifo, s[0] + 256);
					}
                }
                
                /* Back space */
                if (i == 256 + 0x0e) {
                    /* To the task_a window */
					if (key_win == sht_win) {
						if (cursor_x > 8) {
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
							cursor_x -= 8;
						}
					}
                    /* To the console */
                    else {
						fifo32_put(&key_win->task->fifo, 8 + 256);
					}
                }
                
                /* Enter */
                if (i == 256 + 0x1c) {
                    /* To the console */
                    if (key_win != sht_win) {
                        fifo32_put(&task_cons->fifo, 10 + 256);
                    }
                }
                
                /* Tab */
                if (i == 256 + 0x0f) {
					cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					cursor_c = keywin_on(key_win, sht_win, cursor_c);
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
                if (i == 256 + 0x2e && key_ctrl != 0 && task_cons->tss.ss0 != 0) {
                    cons = (struct CONSOLE *) *((int *) 0x0fec);
                    cons_putstr0(cons, "\nBreak(key) :\n");
                    io_cli();	/* 強制終了処理中にタスクが変わると困るから */
                    task_cons->tss.eax = (int) &(task_cons->tss.esp0);
                    task_cons->tss.eip = (int) asm_end_app;
                    io_sti();
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

                /* Rewrite cursor */
                if (cursor_c >= 0) {
                    boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                }
                sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
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
                    sheet_slide(sht_mouse, mx, my);
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
                                            cursor_c = keywin_off(key_win, sht_win, cursor_c, cursor_x);
                                            key_win = sht;
                                            cursor_c = keywin_on(key_win, sht_win, cursor_c);
                                        }
                                        if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
                                            mmx = mx;	/* ウィンドウ移動モードへ */
                                            mmy = my;
                                        }
										if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
											/* 「×」ボタンクリック */
											if ((sht->flags & 0x10) != 0) {	/* アプリが作ったウィンドウか？ */
												cons = (struct CONSOLE *) *((int *) 0x0fec);
												cons_putstr0(cons, "\nBreak(mouse) :\n");
												io_cli();	/* 強制終了処理中にタスクが変わると困るから */
												task_cons->tss.eax = (int) &(task_cons->tss.esp0);
												task_cons->tss.eip = (int) asm_end_app;
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
                            sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
                            mmx = mx;	/* 移動後の座標に更新 */
                            mmy = my;
                        }
                    } else {
                        /* 左ボタンを押していない */
                        mmx = -1;	/* 通常モードへ */
                    }
                }
            }
            /* Cursor */
            else if (i <= 1) {
                if (i != 0) {
                    timer_init(timer, &fifo, 0);
                    if (cursor_c >= 0) {
                        cursor_c = COL8_000000;
                    }
                } else {
                    timer_init(timer, &fifo, 1);
                    if (cursor_c >= 0) {
                        cursor_c = COL8_FFFFFF;
                    }
                }
                timer_settime(timer, 50);
                if (cursor_c >= 0) {
                    boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                    sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
                }
            }
        }
    }
}

