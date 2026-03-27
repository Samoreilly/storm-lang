extern printf
section .text
global main

main:
push rbp
mov rbp, rsp
sub rsp, 16
push 10
pop rax
mov [rbp + -8], rax
push 20
pop rax
mov [rbp + -16], rax
push 30
pop rax
mov [rbp + -8], rax
mov rax, [rbp + -8]
push rax
pop rdi
extern printf
mov rsi, rdi
lea rdi, [format_int]
mov rax, 0
call printf
main_EXIT_0:
mov rsp, rbp
pop rbp
ret

section .data
format_int: db "%d", 10, 0
format_str: db "%s", 10, 0
format_dbl: db "%f", 10, 0
section .data 
	