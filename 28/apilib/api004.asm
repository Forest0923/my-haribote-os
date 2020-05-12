GLOBAL api_end

section .text

api_end:
    MOV     EDX,4
    INT     0x40
