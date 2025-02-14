#include "bootpack.h"

void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act) {

    boxfill8(buf, xsize, COL8_C6C6C6,         0,         0, xsize - 1,         0);
    boxfill8(buf, xsize, COL8_FFFFFF,         1,         1, xsize - 2,         1);
    boxfill8(buf, xsize, COL8_C6C6C6,         0,         0,         0, ysize - 1);
    boxfill8(buf, xsize, COL8_FFFFFF,         1,         1,         1, ysize - 2);
    boxfill8(buf, xsize, COL8_848484, xsize - 2,         1, xsize - 2, ysize - 2);
    boxfill8(buf, xsize, COL8_000000, xsize - 1,         0, xsize - 1, ysize - 1);
    boxfill8(buf, xsize, COL8_C6C6C6,         2,         2, xsize - 3, ysize - 3);
    boxfill8(buf, xsize, COL8_848484,         1, ysize - 2, xsize - 2, ysize - 2);
    boxfill8(buf, xsize, COL8_000000,         0, ysize - 1, xsize - 1, ysize - 1);
    make_wtitle8(buf, xsize, title, act);
    return;
}

void make_wtitle8(unsigned char *buf, int xsize, char *title, char act) {
    static char closebtn[14][16] = {
        "OOOOOOOOOOOOOOO@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQ@@QQQQ@@QQ$@",
        "OQQQQ@@QQ@@QQQ$@",
        "OQQQQQ@@@@QQQQ$@",
        "OQQQQQQ@@QQQQQ$@",
        "OQQQQQ@@@@QQQQ$@",
        "OQQQQ@@QQ@@QQQ$@",
        "OQQQ@@QQQQ@@QQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@",
        "@@@@@@@@@@@@@@@@"
    };
    int x, y;
    char c, tc, tbc;

    if (act != 0) {
        tc = COL8_FFFFFF;
        tbc = COL8_000084;
    } else {
        tc = COL8_C6C6C6;
        tbc = COL8_848484;
    }
    boxfill8(buf, xsize, tbc, 3, 3, xsize - 4, 20);
    putfonts8_asc(buf, xsize, 24, 4, tc, title);
    for (y=0; y<14; y++) {
        for (x=0; x<16; x++) {
            c = closebtn[y][x];
            if (c == '@') {
                c = COL8_000000;
            }
            else if (c == '$') {
                c = COL8_848484;
            }
            else if (c == 'Q') {
                c = COL8_C6C6C6;
            }
            else {
                c = COL8_FFFFFF;
            }
            buf[(5+y) * xsize + (xsize - 21 + x)] = c;
        }
    }
    return;
}

