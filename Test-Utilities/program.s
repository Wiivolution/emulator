.global _start 

_start:
    mov r0, #0xd800000
    add r0, r0, #0xe0
    mov r1, #0xFF
    mov r1, r1, lsl #0x10
    str r1, [r0]
    ldr r2, [r0]
    swi #0
    b loop
loop:
    add r1, r1, #1
    b loop
