section .data 
	string_0: db `%i\n`, 0
	

extern printf
global main

section .text
fib:
	push rbp
	mov rbp, rsp
	push rbx
	push r15
	push r14
	push r13
	push r12
	push r11
	sub rsp, 16
	mov r14, rdi
	mov rax, r14
	cmp rax, 0
	setle al
	movzx rax, al
	mov r13, rax
	mov rax, r13
	cmp rax, 0
	je L0
	mov rax, 0
	lea rsp, [rbp - 48]
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	pop rbx
	leave
	ret
L0:
	mov rax, r14
	sub rax, 1
	mov r13, rax
	mov rdi, r13
	mov al, 0
	call fib
	mov r13, rax
	mov rax, r14
	add rax, r13
	mov r12, rax
	mov rax, r12
	lea rsp, [rbp - 48]
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	pop rbx
	leave
	ret
main:
	push rbp
	mov rbp, rsp
	push rbx
	push r15
	push r14
	push r13
	push r12
	push r11
	mov rdi, 10000
	mov al, 0
	call fib
	mov r12, rax
	mov rdi, string_0
	mov rsi, r12
	mov al, 0
	call printf
	mov rax, 0
	lea rsp, [rbp - 48]
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	pop rbx
	leave
	ret
