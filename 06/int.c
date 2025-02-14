#include "bootpack.h"

/* Initialize PIC */
void init_pic(void) {
    io_out8(PIC0_IMR,  0xff);   /* Disable all interrupt */
    io_out8(PIC1_IMR,  0xff);   /* Disable all interrupt */

    io_out8(PIC0_ICW1, 0x11);   /* Edge triger mode                 */
    io_out8(PIC0_ICW2, 0x20);   /* IRQ0-7 are accepted at INT20-27  */
    io_out8(PIC0_ICW3, 1 << 2); /* PIC1 is connected to IRQ2        */
    io_out8(PIC0_ICW4, 0x01);   /* non-buffer mode                  */
    
    io_out8(PIC1_ICW1, 0x11);   /* Edge triger mode                 */
    io_out8(PIC1_ICW2, 0x28);   /* IRQ8-15 are accepted at INT28-2f */
    io_out8(PIC1_ICW3, 2);      /* PIC1 is connected to IRQ2        */
    io_out8(PIC1_ICW4, 0x01);   /* non-buffer mode                  */

    io_out8(PIC0_IMR,  0xfb);   /* Prohibit other than PCI1         */
    io_out8(PIC1_IMR,  0xff);   /* Do not accept all interrupt      */

    return;
}

/* Interrupt from keyboard */
void inthandler21(int *esp) {
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32*8-1, 15);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 21 (IRQ-1): PS/2 keyboard");
    for (;;) {
        io_hlt();
    }
}

/* PIC0からの不完全割り込み対策 */
/* Athlon64X2機などではチップセットの都合によりPICの初期化時にこの割り込みが1度だけおこる   */
/* この割り込み処理関数は、その割り込みに対して何もしないでやり過ごす                       */
/* なぜ何もしなくていいの？                                                                 */
/*      →  この割り込みはPIC初期化時の電気的なノイズによって発生したものなので、            */
/*          まじめに何か処理してやる必要がない。                                            */
void inthandler27(int *esp) {
    io_out8(PIC0_OCW2, 0x67);   /* IRQ-07受付完了をPICに通知(7-1参照) */
    return;
}

/* Interrupt from mouse */
void inthandler2c(int *esp) {
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32*8-1, 15);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 2C (IRQ-12): PS/2 mouse");
    for (;;) {
        io_hlt();
    }
}

