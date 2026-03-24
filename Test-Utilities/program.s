.global _start 

_start:
    mov r0, #9
    mov r1, #0x8
    teq r0, r1
    swi #0
    b loop
loop:
    add r1, r1, #1
    b loop
