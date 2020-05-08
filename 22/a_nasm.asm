GLOBAL api_putchar, api_putstr0
GLOBAL api_end
GLOBAL api_openwin
GLOBAL api_putstrwin, api_boxfilwin

section .text

api_putchar:
    MOV     EDX,1
    MOV     AL,[ESP+4]
    INT     0x40
    RET

api_putstr0:	; void api_putstr0(char *s);
    PUSH	EBX
    MOV		EDX,2
    MOV		EBX,[ESP+8]		; s
    INT		0x40
    POP		EBX
    RET

api_end:
    MOV     EDX,4
    INT     0x40

api_openwin:
    PUSH    EDI
    PUSH    ESI
    PUSH    EBX
    MOV     EDX,5
    MOV     EBX,[ESP+16]
    MOV     ESI,[ESP+20]
    MOV     EDI,[ESP+24]
    MOV     EAX,[ESP+28]
    MOV     ECX,[ESP+32]
    INT     0x40
    POP     EBX
    POP     ESI
    POP     EDI
    RET

api_putstrwin:
    PUSH    EDI
    PUSH    ESI
    PUSH    EBP
    PUSH    EBX
    MOV     EDX,6
    MOV     EBX,[ESP+20]
    MOV     ESI,[ESP+24]
    MOV     EDI,[ESP+28]
    MOV     EAX,[ESP+32]
    MOV     ECX,[ESP+36]
    MOV     EBP,[ESP+40]
    INT     0x40
    POP     EBX
    POP     EBP
    POP     ESI
    POP     EDI
    RET

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
     
