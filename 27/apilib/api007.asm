GLOBAL api_boxfilwin

section .text

api_boxfilwin:
    PUSH    EDI
    PUSH    ESI
    PUSH    EBP
    PUSH    EBX
    MOV     EDX,7
    MOV     EBX,[ESP+20]
    MOV     EAX,[ESP+24]
    MOV     ECX,[ESP+28]
    MOV     ESI,[ESP+32]
    MOV     EDI,[ESP+36]
    MOV     EBP,[ESP+40]
    INT     0x40
    POP     EBX
    POP     EBP
    POP     ESI
    POP     EDI
    RET
