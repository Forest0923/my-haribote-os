GLOBAL  api_getlang

section .text

api_getlang:
    MOV     EDX,27
    INT     0x40
    RET

