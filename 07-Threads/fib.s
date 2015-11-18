	.syntax unified
	.text
	.align 2
	.thumb
	.thumb_func

	.global fibonacci
	.type fibonacci, function

fibonacci:
	@ ADD/MODIFY CODE BELOW
	@ PROLOG
	@ r0 = t3, r1 = t4, r2 = t0, r3 = t1, r4 = i, r5 = n
	push {r4, r5, lr}
	
	@ Compare r5 wtih 0
	@ if(r0 <= 0) goto .L3 (which returns 0)
	cmp r0, #0
	ble	.L3
	
	mov r2, #1					@ t0 = 1
	mov r3, #1					@ t1 = 1
	mov r4, #1					@ i = 1
	mov r5, r0					@ r5 = n
	mov r0,	#1					@ t3 = 1

.Loop:	
	@ if(n <= i) goto .Exit (which returns R0)
	cmp r5, r4
	ble	.Exit
	
	@ if  n < ( i << 1 ) 
	cmp r5,r4,LSL #1
	blt .else
	
	@ t4 = t1 * t1+  t0 * t0
	MUL r1, r3, r3				
    MLA r1, r2, r2, r1

	@ t3 = t0 * ( 2 * t1 - t0 )
	mov r0, r3,LSL #1
	sub r0, r0, r2
	MUL r0, r2, r0
		   
    mov r2, r0					@ t0 = t3
    mov r3, r1				    @ t1 = t4
    mov r4,r4,LSL #1 			@ i = ( i << 1 )
	bl	.Loop					@ While Loop

.else:	
	mov r2, r0					@ t0 = t3
	mov r0, r1					@ t3 = t4
	add r1, r2, r1				@ t4 = t0 + t4
	add r4, r4, #1				@ i = i + 1
	bl	.Loop					@ While Loop
	
.Exit:
	pop {r4, r5, pc}			@ EPILOG
	@ END CODE MODIFICATION
.L3:
	mov r0, #0					@ R0 = 0
	pop {r4, r5, pc}			@ EPILOG

	.size fibonacci, .-fibonacci
	.end
