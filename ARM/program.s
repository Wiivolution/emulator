.global _start 

_start:
    mov r0, #0
    mov r1, #1
    mov r2, #2
    b loop
loop:
    b loop
