entrypoint: add pc, pc, #0x1b
str: .asciz "Hellord"
boot1_aes_key:
    .word 0x9258A752
    .word 0x64960D82
    .word 0x676F9044
    .word 0x56882A73

.text
.global _start 

_start:
    mov r0, #0xd200000
    mov r5, #0xFF000000
    add r5, r5, #0xFF0000
    add r5, r5, #0x1A
    b set_aes_key
    b loop
loop:
    add r1, r1, #1
    b loop

set_aes_key:
    mov r4, #0x3
    mov r1, r5
set_aes_key_loop:
    ldr r3, [r1, #0x4]
    str r1, [r0, #0xc]
    subs r4, r4, #0x1
    bpl set_aes_key_loop
