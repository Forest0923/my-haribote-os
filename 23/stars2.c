int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
void api_initmalloc(void);
char *api_malloc(int size);
void api_point(int win, int x, int y, int col);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_end(void);

static unsigned int xorshift_x = 123456789;
static unsigned int xorshift_y = 362436069;
static unsigned int xorshift_z = 521288629;
static unsigned int xorshift_w = 88675123;

int rand() {
    unsigned int r;
    r = xorshift_x ^ (xorshift_x<<11);
    xorshift_x = xorshift_y;
    xorshift_y = xorshift_z;
    xorshift_z = xorshift_w;
    xorshift_w ^= r ^ (r>>8) ^ (xorshift_w>>19);
    return ((xorshift_x) / 4294967296 + xorshift_w);
}

void HariMain(void) {
    char *buf;
    int win, i, x, y;
    api_initmalloc();
    buf = api_malloc(150 * 100);
    win = api_openwin(buf, 150, 100, -1, "stars2");
    api_boxfilwin(win + 1,  6, 26, 143, 93, 0 /* 黒 */);
    for (i = 0; i < 50; i++) {
        x = (rand() % 137) +  6;
        y = (rand() %  67) + 26;
        api_point(win + 1, x, y, 3 /* 黄 */);
    }
    api_refreshwin(win,  6, 26, 144, 94);
    api_end();
}
