.intel_syntax noprefix
.global bench

bench:
	mov ebp, 0
	mov ebx,[esp+4]
	mov ecx,[esp+8]
	mov edx,[esp+8]
lstart:
	inc ebp
	mov [ebx],ebp
	mov eax,[ecx]
	mov eax,[edx]
	clflush [ecx]
	clflush [edx]
	mfence
	jmp lstart
