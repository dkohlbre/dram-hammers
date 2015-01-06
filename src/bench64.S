.intel_syntax noprefix
.global bench

bench:
	mov rbp, 0
	mov rbx,rsi
	mov rcx,rdx

lstart:
	inc rbp
	mov [rdi],ebp
	mov rax,[rcx]
	mov rax,[rdx]
	clflush [rcx]
	clflush [rdx]
	mfence
	jmp lstart
