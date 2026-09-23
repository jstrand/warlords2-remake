; W2HOOK.COM -- lets a script drive the original game under DOSBox-X.
;
; Resident, and hooks two interrupts:
;
;   INT 9   F12 takes a screenshot through the DOSBox-X integration device
;           (ports 28h-2Ah, register 00C54010h). F11 plays the next click
;           from the table at the end of this file. F10 does nothing at all,
;           which makes it a pause AUTOTYPE can type. All three are swallowed,
;           so the game never sees them.
;   INT 33h function 3 (buttons and position), which is how the game reads
;           the mouse (1ec0:000e): once a click has been played it reports
;           the click's position from then on, with the button down between
;           3 and 6 timer ticks after the key.
;
; Everything else goes through to the handlers that were there before.
; tools/dosbox/shoot.py builds the click table and runs the game.
;
;   nasm -f bin -o W2HOOK.COM w2hook.asm

        org 100h

start:  jmp install

old9    dd 0
old33   dd 0
active  db 0            ; a click has been played, so the position is ours
fx      dw 0
fy      dw 0
fbtn    dw 0
t0      dw 0
next    dw 0            ; index of the next click in the table

; ------------------------------------------------------------------ INT 9

int9:   push ax
        in al, 60h
        cmp al, 58h             ; F12 make
        je .shot
        cmp al, 57h             ; F11 make
        je .click
        cmp al, 44h             ; F10 make
        je .eat
        cmp al, 0D8h            ; and the three breaks
        je .eat
        cmp al, 0D7h
        je .eat
        cmp al, 0C4h
        je .eat
        pop ax
        jmp far [cs:old9]

.shot:  call shoot
        jmp .eat

.click: push bx
        push si
        push ds
        push cs
        pop ds
        mov si, [next]
        cmp si, [clicks]
        jae .noclk
        inc word [next]
        mov bx, si
        shl si, 1
        add si, bx
        shl si, 1               ; si = index * 6
        mov ax, [table + si]
        mov [fx], ax
        mov ax, [table + si + 2]
        mov [fy], ax
        mov ax, [table + si + 4]
        mov [fbtn], ax
        push es
        mov ax, 40h
        mov es, ax
        mov ax, [es:6Ch]
        pop es
        mov [t0], ax
        mov byte [active], 1
.noclk: pop ds
        pop si
        pop bx

.eat:   in al, 61h              ; acknowledge the keyboard, as BIOS would
        mov ah, al
        or al, 80h
        out 61h, al
        mov al, ah
        out 61h, al
        mov al, 20h
        out 20h, al
        pop ax
        iret

shoot:  push ax
        push dx
        mov dx, 2Ah
        xor al, al              ; reset the byte latches
        out dx, al
        mov dx, 28h             ; index = 00C54010h, a byte at a time
        mov al, 10h
        out dx, al
        mov al, 40h
        out dx, al
        mov al, 0C5h
        out dx, al
        xor al, al
        out dx, al
        inc dx                  ; data = 1, and the fourth byte writes it
        mov al, 1
        out dx, al
        xor al, al
        out dx, al
        out dx, al
        out dx, al
        pop dx
        pop ax
        ret

; ---------------------------------------------------------------- INT 33h

int33:  cmp ax, 3
        jne .pass
        cmp byte [cs:active], 0
        je .pass
        push es
        mov bx, 40h
        mov es, bx
        mov bx, [es:6Ch]
        pop es
        sub bx, [cs:t0]
        cmp bx, 3
        jb .up
        cmp bx, 6
        jae .up
        mov bx, [cs:fbtn]
        jmp .pos
.up:    xor bx, bx
.pos:   mov cx, [cs:fx]
        mov dx, [cs:fy]
        iret
.pass:  jmp far [cs:old33]

; ------------------------------------------------------------ the clicks
; shoot.py patches these: a count, then x, y, buttons for each.

clicks  dw 0
table   times 3 * 256 dw 0
resend:

; ---------------------------------------------------------------- install

install:
        mov ax, 3509h
        int 21h
        mov [old9], bx
        mov [old9 + 2], es
        mov ax, 3533h
        int 21h
        mov [old33], bx
        mov [old33 + 2], es
        mov ax, 2509h
        mov dx, int9
        int 21h
        mov ax, 2533h
        mov dx, int33
        int 21h
        mov dx, resend + 15
        mov cl, 4
        shr dx, cl
        mov ax, 3100h
        int 21h
