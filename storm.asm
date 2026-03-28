section .data 
	string_0: db `%i\n`, 0
	

extern printf
global main

section .text
main:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, 1
	mov qword [rbp -8], rax
	mov rax, 2
	mov qword [rbp -16], rax
	mov rdi, string_0
	mov rsi, qword [rbp -16]
	mov al, 0
	call printf
	mov rax, 5
	mov qword [rbp -16], rax
	mov rdi, string_0
	mov rsi, qword [rbp -16]
	mov al, 0
	call printf
	mov rdi, 500000000
	mov al, 0
	call fib
	mov qword [rbp -16], rax
	mov rdi, string_0
	mov rsi, qword [rbp -16]
	mov al, 0
	call printf
	leave
	ret
fib:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	mov qword [rbp -8], rdi
	mov rax, 0
	mov qword [rbp -16], rax
	mov rax, 1
	mov qword [rbp -24], rax
	mov rax, 0
	mov qword [rbp -32], rax
L0:
	mov rax, qword [rbp -32]
	cmp rax, qword [rbp -8]
	setl al
	movzx rax, al
	mov qword [rbp -1008], rax
	mov rax, qword [rbp -1008]
	cmp rax, 0
	je L1
	mov rax, qword [rbp -16]
	add rax, qword [rbp -24]
	mov qword [rbp -1016], rax
	mov rax, qword [rbp -1016]
	mov qword [rbp -40], rax
	mov rax, qword [rbp -24]
	mov qword [rbp -16], rax
	mov rax, qword [rbp -40]
	mov qword [rbp -24], rax
	mov rax, qword [rbp -32]
	add rax, 1
	mov qword [rbp -32], rax
	jmp L0
L1:
	mov rax, qword [rbp -16]
	leave
	ret
