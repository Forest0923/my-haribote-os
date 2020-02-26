#include "bootpack.h"

unsigned int memtest_sub(unsigned int start, unsigned int end) {
    unsigned int i, *p, old;
    unsigned int pat0 = 0xaa55aa55;
    unsigned int pat1 = 0x55aa55aa;
    for (i=start; i<=end; i+=0x1000) {
        p   = (unsigned int *) (i + 0xffc);
        old = *p;
        *p  = pat0;         /* Change value */
        *p ^= 0xffffffff;   /* XOR */
        if (*p != pat1) {
not_memory:
            *p = old;
            break;
        }
        *p ^= 0xffffffff;
        if (*p != pat0) {
            goto not_memory;
        }
        *p = old;
    }
    return i;
}

unsigned int memtest(unsigned int start, unsigned int end) {
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    /* Check CPU version */
    /* If the CPU is earlier than 486, the 18th bit is always 0 */
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;  /* AC-bit -> 1 */
    io_store_eflags(eflg);
    eflg = io_load_eflags();

    /* If the 18th bit is still "1" => 486 or latter*/
    if ((eflg & EFLAGS_AC_BIT) != 0) {
        flg486 = 1;
    }
    eflg &= ~EFLAGS_AC_BIT; /* AC-bit -> 0 */
    io_store_eflags(eflg);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }

    return i;
}

void memman_init(struct MEMMAN *man) {
    man->frees    = 0;
    man->maxfrees = 0;
    man->lostsize = 0;
    man->losts    = 0;
    return;
}

/* Return total size of free area */
unsigned int memman_total(struct MEMMAN *man) {
    unsigned int i, t = 0;
    for (i=0; i < man->frees; i++) {
        t += man->free[i].size;
    }
    return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size) {
    unsigned int i, a;
    for (i=0; i < man->frees; i++) {
        if (man->free[i].size >= size) {
            a = man->free[i].addr;
            man->free[i].addr += size;
            man->free[i].size -= size;
            if (man->free[i].size == 0) {
                man->frees -= 1;
                for (; i < man->frees; i++) {
                    man->free[i] = man->free[i+1];
                }
            }
            return a;
        }
    }
    return 0;
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size) {
    int i, j;
    for (i=0; i < man->frees; i++) {
        if (man->free[i].addr > addr) {
            break;
        }
    }
    /* free[i-1].addr < addr < free[i].addr */
    /* If free[i-1] exist */
    if (i > 0) {
        /* If free[i-1] and target is continuous */
        if (man->free[i-1].addr + man->free[i-1].size == addr) {
            man->free[i-1].size += size;
            /* If free[i] exist */
            if (i < man->frees) {
                /* If free[i] and target is continuous */
                if (addr + size == man->free[i].addr) {
                    man->free[i-1].size += man->free[i].size;
                    man->frees -= 1;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i+1];
                    }
                }
            }
            return 0;
        }
    }
    /* If free[i-1] and targetis not continuous */
    if (i < man->frees) {
        if (addr + size == man->free[i].addr) {
            man->free[i].addr = addr;
            man->free[i].size += size;
            return 0;
        }
    }
    /* Target has nothing continuous */
    if (man->frees < MEMMAN_FREES) {
        for (j=man->frees; j > i; j--) {
            man->free[j] = man->free[j-1];
        }
        man->frees += 1;
        if (man->maxfrees < man->frees) {
            man->maxfrees = man->frees;
        }
        man->free[i].addr = addr;
        man->free[i].size = size;
        return 0;
    }
    man->losts++;
    man->lostsize += size;
    return -1;
}

