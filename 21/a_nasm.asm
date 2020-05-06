global api_putchar
global api_end

section .text

api_putchar:
    MOV     EDX,1
    MOV     AL,[ESP+4]
    INT     0x40
    RET

api_end:
    MOV     EDX,4
    INT     0x40

