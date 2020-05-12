GLOBAL api_putstr1

section .text

api_putstr1:
    PUSH	EBX
    MOV		EDX,3
    MOV		EBX,[ESP+ 8]    ; s
    MOV		EBX,[ESP+12]    ; l
    INT		0x40
    POP		EBX
    RET
