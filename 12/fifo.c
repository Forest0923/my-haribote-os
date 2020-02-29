#include "bootpack.h"

#define FLAGS_OVERRUN   0x0001

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf) {
    fifo->size = size;
    fifo->buf  = buf;
    fifo->free = size;
    fifo->flag = 0;
    fifo->putp = 0; 
    fifo->getp = 0;
    return;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data) {
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

int fifo8_get(struct FIFO8 *fifo) {
    unsigned char data;
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

int fifo8_status(struct FIFO8 *fifo) {
    return fifo->size - fifo->free;
}
