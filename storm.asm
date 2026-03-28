section .data 
	string_0: db `%i\n`, 0
	

extern printf
global main

section .text
add:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov qword [rbp -8], rdi
	mov qword [rbp -16], rsi
	mov rax, qword [rbp -8]
	add rax, qword [rbp -16]
	mov qword [rbp -1000], rax
	mov rax, qword [rbp -1000]
	leave
	ret
run:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov qword [rbp -8], rdi
	mov rax, 0
	mov qword [rbp -16], rax
	mov rax, 0
	mov qword [rbp -24], rax
L0:
	mov rax, qword [rbp -24]
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
	mov qword [rbp -16], rax
	mov rax, qword [rbp -24]
	add rax, 1
	mov qword [rbp -24], rax
	jmp L0
L1:
	mov rax, qword [rbp -16]
	leave
	ret
main:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, 0
	mov qword [rbp -8], rax
L2:
	mov rax, qword [rbp -8]
	cmp rax, 48
	setl al
	movzx rax, al
	mov qword [rbp -1024], rax
	mov rax, qword [rbp -1024]
	cmp rax, 0
	je L3
	mov rdi, 100000
	mov al, 0
	call run
	mov qword [rbp -1032], rax
	mov rdi, string_0
	mov rsi, qword [rbp -1032]
	mov al, 0
	call printf
	mov rax, qword [rbp -8]
	add rax, 1
	mov qword [rbp -8], rax
	mov rdi, 100000
	mov al, 0
	call run
	mov qword [rbp -1040], rax
	mov rdi, string_0
	mov rsi, qword [rbp -1040]
	mov al, 0
	call printf
	mov rax, qword [rbp -8]
	add rax, 1
	mov qword [rbp -8], rax
	mov rdi, 100000
	mov al, 0
	call run
	mov qword [rbp -1048], rax
	mov rdi, string_0
	mov rsi, qword [rbp -1048]
	mov al, 0
	call printf
	mov rax, qword [rbp -8]
	add rax, 1
	mov qword [rbp -8], rax
	mov rdi, 100000
	mov al, 0
	call run
	mov qword [rbp -1056], rax
	mov rdi, string_0
	mov rsi, qword [rbp -1056]
	mov al, 0
	call printf
	mov rax, qword [rbp -8]
	add rax, 1
	mov qword [rbp -8], rax
	jmp L2
L3:
	mov rdi, 100000
	mov al, 0
	call run
	mov qword [rbp -1064], rax
	mov rdi, string_0
	mov rsi, qword [rbp -1064]
	mov al, 0
	call printf
	mov rax, qword [rbp -8]
	add rax, 1
	mov qword [rbp -8], rax
	mov rdi, 100000
	mov al, 0
	call run
	mov qword [rbp -1072], rax
	mov rdi, string_0
	mov rsi, qword [rbp -1072]
	mov al, 0
	call printf
	leave
	ret
