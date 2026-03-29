section .data 
	string_0: db `%i\n`, 0
	

extern printf
global main

section .text
main:
	push rbp
	mov rbp, rsp
	push rbx
	push r15
	push r14
	push r13
	push r12
	sub rsp, 96
	mov rax, 0
	mov qword [rbp -8], rax
	mov rax, 0
	mov qword [rbp -16], rax
L0:
	mov rax, qword [rbp -16]
	cmp rax, 100000
	setl al
	movzx rax, al
	mov r13, rax
	mov rax, r13
	cmp rax, 0
	je L1
	mov rax, 1
	mov r13, rax
	mov rax, 2
	mov r12, rax
	mov rax, 3
	mov rbx, rax
	mov rax, 4
	mov r15, rax
	mov rax, 5
	mov qword [rbp -56], rax
	mov rax, 6
	mov qword [rbp -64], rax
	mov rax, 7
	mov qword [rbp -72], rax
	mov rax, 8
	mov qword [rbp -80], rax
	mov rax, 9
	mov qword [rbp -88], rax
	mov rax, 10
	mov qword [rbp -96], rax
	mov rax, r13
	imul rax, r12
	mov r14, rax
	mov rax, rbx
	imul rax, r15
	mov r12, rax
	mov rax, r14
	add rax, r12
	mov rbx, rax
	mov rax, qword [rbp -56]
	imul rax, qword [rbp -64]
	mov r12, rax
	mov rax, rbx
	add rax, r12
	mov r14, rax
	mov rax, qword [rbp -72]
	imul rax, qword [rbp -80]
	mov r12, rax
	mov rax, r14
	add rax, r12
	mov rbx, rax
	mov rax, qword [rbp -88]
	imul rax, qword [rbp -96]
	mov r12, rax
	mov rax, rbx
	add rax, r12
	mov r14, rax
	mov rax, r14
	mov qword [rbp -8], rax
	mov rax, qword [rbp -16]
	add rax, 1
	mov qword [rbp -16], rax
	jmp L0
L1:
	mov rdi, string_0
	mov rsi, qword [rbp -8]
	mov al, 0
	call printf
	pop r12
	pop r13
	pop r14
	pop r15
	pop rbx
	leave
	ret
