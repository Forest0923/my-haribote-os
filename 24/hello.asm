bits 32
    MOV     ECX,msg
putloop:
    MOV     AL,[CS:ECX]
    CMP     AL,0
    JE      fin
    INT     0x40
    ADD     ECX,1
    JMP     putloop
fin:
    MOV     EDX,4
    INT     0x40
msg:
    DB      "hello",0

