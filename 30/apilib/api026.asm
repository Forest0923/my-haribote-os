GLOBAL  api_cmdline

section .text

api_cmdline:
    PUSH    EBX
    MOV     EDX,26
    MOV     ECX,[ESP+12]
    MOV     EBX,[ESP+8]
    INT     0x40
    POP     EBX
    RET

