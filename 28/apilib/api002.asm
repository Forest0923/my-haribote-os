GLOBAL api_putchar, api_putstr0

section .text

api_putstr0:	; void api_putstr0(char *s);
    PUSH	EBX
    MOV		EDX,2
    MOV		EBX,[ESP+8]		; s
    INT		0x40
    POP		EBX
    RET
