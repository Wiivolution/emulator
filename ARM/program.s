.global _start 

_start:
    mvn r0, #0x7FFFFFFF
    mov r1, #0x12
    str r1, [r0]
    mov r2, #2
    b loop
loop:
    b loop
