#include <stdarg.h>
 
//10進数からASCIIコードに変換
int dec2asc (char *str, int dec) {
    int len = 0, len_buf; //桁数
    int buf[10];
    while (1) { //10で割れた回数（つまり桁数）をlenに、各桁をbufに格納
        buf[len++] = dec % 10;
        if (dec < 10) break;
        dec /= 10;
    }
    len_buf = len;
    while (len) {
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}
 
//16進数からASCIIコードに変換
int hex2asc (char *str, int dec) { //10で割れた回数（つまり桁数）をlenに、各桁をbufに格納
    int len = 0, len_buf; //桁数
    int buf[10];
    while (1) {
        buf[len++] = dec % 16;
        if (dec < 16) break;
        dec /= 16;
    }
    len_buf = len;
    while (len) {
        len --;
        *(str++) = (buf[len]<10)?(buf[len] + 0x30):(buf[len] - 9 + 0x60);
    }
    return len_buf;
}
 
void sprintf (char *str, char *fmt, ...) {
    va_list list;
    int i, len;
    va_start (list, fmt);
 
    while (*fmt) {
        if(*fmt=='%') {
            fmt++;
            switch(*fmt){
                case 'd':
                    len = dec2asc(str, va_arg (list, int));
                    break;
                case 'x':
                    len = hex2asc(str, va_arg (list, int));
                    break;
            }
            str += len; fmt++;
        } else {
            *(str++) = *(fmt++);
        }   
    }
    *str = 0x00; //最後にNULLを追加
    va_end (list);
}

int strcmp(char *str1, char *str2) {
    int i;
    for (i = 0; i < 20; i++) {
        if (str1[i] == 0x00 && str2[i] == 0x00) return 0;
        if (str1[i] == 0x00) return 1;
        if (str2[i] == 0x00) return 1;
        if (str1[i] != str2[i]) return 1;
    }
    return -1;
}
int strncmp(char *str1, char *str2, int n) {
    int i;
    for (i = 0; i < n; i++) {
        if (str1[i] == 0x00 && str2[i] == 0x00) return 0;
        if (str1[i] == 0x00) return 1;
        if (str2[i] == 0x00) return 1;
        if (str1[i] != str2[i]) return 1;
    }
    return 0;
}
