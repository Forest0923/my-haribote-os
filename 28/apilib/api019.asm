GLOBAL api_freetimer

section .text

api_freetimer:		; void api_freetimer(int timer);
    PUSH	EBX
    MOV		EDX,19
    MOV		EBX,[ESP+ 8]		; timer
    INT		0x40
    POP		EBX
    RET
