; nasmfunc.asm
; TAB=4

EXTERN  inthandler20, inthandler21, inthandler27, inthandler2c
EXTERN  hrb_api

section .text
    GLOBAL  io_hlt, io_cli, io_sti, io_stihlt
    GLOBAL  io_in8, io_in16, io_in32
    GLOBAL  io_out8, io_out16, io_out32
    GLOBAL  io_load_eflags, io_store_eflags
    GLOBAL  load_gdtr, load_idtr
    GLOBAL  asm_inthandler20, asm_inthandler21
    GLOBAL  asm_inthandler27, asm_inthandler2c
    GLOBAL  load_cr0, store_cr0
    GLOBAL  load_tr, farjmp, farcall
    GLOBAL  asm_hrb_api


io_hlt:     ; void io_hlt(void);
    HLT
    RET

io_cli:     ; void io_cli(void);
    CLI
    RET

io_sti:     ; void io_sti(void);
    STI
    RET

io_stihlt:  ; void io_stihlt(void);
    STI
    HLT
    RET

io_in8:     ; int io_in8(int port);
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,0
    IN      AL,DX
    RET

io_in16:    ; int io_in16(int port);
    MOV     EDX,[ESP+4]
    MOV     EAX,0
    IN      AX,DX
    RET

io_in32:    ; int io_in32(int port);
    MOV     EDX,[ESP+4]
    IN      EAX,DX
    RET

io_out8:    ; int io_out8(int port);
    MOV     EDX,[ESP+4]     ; port
    MOV     AL,[ESP+8]      ; data
    OUT     DX,AL
    RET

io_out16:   ; int io_out16(int port);
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,[ESP+8]     ; data
    OUT     DX,AX
    RET

io_out32:   ; int io_out32(int port);
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,[ESP+8]     ; data
    OUT     DX,EAX
    RET

io_load_eflags:     ; int io_load_eflags(void);
    PUSHFD      ; Push flags double-word
    POP     EAX
    RET

io_store_eflags:    ; void io_store_eflags(int eflags);
    MOV     EAX,[ESP+4]
    PUSH    EAX
    POPFD       ; Pop flags double-word
    RET

load_gdtr:  ; void load_gdtr(int limit, int addr);
    MOV     AX,[ESP+4]  ; limit
    MOV     [ESP+6],AX
    LGDT    [ESP+6]
    RET

load_idtr:  ; void load_idtr(int limit, int addr);
    MOV     AX,[ESP+4]  ; limit
    MOV     [ESP+6],AX
    LIDT    [ESP+6]
    RET

asm_inthandler20:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler20
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD

asm_inthandler21:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler21
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD
    
asm_inthandler27:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler27
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD

asm_inthandler2c:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler2c
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD

load_cr0:       ; int load_cr0();
    MOV     EAX,CR0
    RET

store_cr0:      ; void store_cr0(int cr0);
    MOV     EAX,[ESP+4]
    MOV     CR0,EAX
    RET

load_tr:        ; void load_tr(int tr);
    LTR     [ESP+4]
    RET

farjmp:         ; void farjmp(int eip, int cs);
    JMP     FAR [ESP+4]
    RET

farcall:
    CALL    FAR [ESP+4]
    RET

asm_hrb_api:
    STI
    PUSHAD
    PUSHAD
    CALL    hrb_api
    ADD     ESP,32
    POPAD
    IRETD

