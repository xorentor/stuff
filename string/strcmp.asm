segment .data

segment .text

	global strcmp_sse42
strcmp_sse42:
	push	ebp
	mov	ebp, esp

	mov ecx, [ebp+8]
	mov edx, [ebp+0Ch]
	mov eax, ecx
	sub eax, edx
	sub edx, 10h

.loop:
 	add edx, 10h
    	MovDqU    xmm0, [edx]
    	PcmpIstrI xmm0, [edx + eax], 18h
    	ja .loop

  	jc .diff

  	xor eax, eax
  	ret
.diff:
  	add eax, edx
 	movzx eax, byte[eax + ecx]
 	movzx edx, byte[edx + ecx]
  	sub eax, edx

	mov 	esp, ebp
	pop   	ebp

  	ret
