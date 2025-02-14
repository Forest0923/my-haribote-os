; nasmfunc.asm
; TAB=4

section .text
    GLOBAL  io_hlt, io_cli, io_sti, io_stihlt
    GLOBAL  io_in8, io_in16, io_in32
    GLOBAL  io_out8, io_out16, io_out32
    GLOBAL  io_load_eflags, io_store_eflags
    ; GLOBAL  write_mem8

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

; write_mem8:     ; void write_mem8(int addr, int data);
;     MOV     ECX,[ESP+4]
;     MOV     AL,[ESP+8]
;     MOV     [ECX],AL
;     RET
