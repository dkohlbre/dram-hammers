.intel_syntax noprefix
.global loop
	
loop:
	mov ecx,[esp+4]
	mov edx,[esp+8]
lstart:
	mov eax,[ecx]
	mov ebx,[edx]
	clflush [ecx]
	clflush [edx]
	mfence
	jmp lstart
