.intel_syntax noprefix
.global loop
	
loop:
	mov rcx,rsi
	mov rdx,rdi
lstart:
	mov rax,[rcx]
	mov rbx,[rdx]
	clflush [rcx]
	clflush [rdx]
	mfence
	jmp lstart
