segment .data

segment .text

        global nasm_strlen_pcmp
nasm_strlen_pcmp:
	push	ebp
	mov	ebp, esp

	mov 	ecx, [ebp+8]
	mov 	eax, -16
	mov 	edx, ecx
	pxor 	xmm0, xmm0

.loop:
	add 	eax, 16
	PcmpIstrI 	xmm0, [edx + eax], 1000b
	jnz 	.loop

	add 	eax, ecx

	mov 	esp, ebp
	pop   	ebp

	ret
