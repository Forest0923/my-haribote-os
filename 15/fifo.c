#include "bootpack.h"

#define FLAGS_OVERRUN   0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf) {
    fifo->size = size;
    fifo->buf  = buf;
    fifo->free = size;
    fifo->flag = 0;
    fifo->putp = 0;
    fifo->getp = 0;
    return;
}

int fifo32_put(struct FIFO32 *fifo, int data) {
    /* Overflow */
    if (fifo->free == 0) {
        fifo->flag |= FLAGS_OVERRUN;
        return -1;
    }
    
    fifo->buf[fifo->putp] = data;
    fifo->putp += 1;
    if (fifo->putp == fifo->size) {
        fifo->putp = 0;
    }
    fifo->free -= 1;
    return 0;
}

int fifo32_get(struct FIFO32 *fifo) {
    int data;
    if (fifo->free == fifo->size) {
        return -1;
    }
    data = fifo->buf[fifo->getp];
    fifo->getp += 1;
    if (fifo->getp == fifo->size) {
        fifo->getp = 0;
    }
    fifo->free += 1;
    return data;
}

int fifo32_status(struct FIFO32 *fifo) {
    return fifo->size - fifo->free;
}
