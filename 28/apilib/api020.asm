GLOBAL api_beep

section .text

api_beep:
    MOV     EDX,20
    MOV     EAX,[ESP+4]
    INT     0x40
    RET

