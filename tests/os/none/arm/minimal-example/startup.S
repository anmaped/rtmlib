.type _start, %function
.word _estack
.word _start
.global _start
_start:
    ldr r0, =_estack
    mov sp, r0
	ldr r1, =main
	bx r1
