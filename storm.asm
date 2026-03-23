extern printf
section .text
global main

main:
push rbp
mov rbp, rsp
sub rsp, 16
lea rax, [STR_0]
push rax
pop rax
mov [rbp + -8], rax
mov rax, [rbp + -8]
push rax
pop rdi
mov rsi, rdi
extern printf
lea rdi, [format_str]
mov rax, 0
call printf
push rax
main_EXIT_0:
mov rsp, rbp
pop rbp
ret

section .data
format_int: db "%d", 10, 0
format_str: db "%s", 10, 0
section .data 
	STR_0: db "hello world", 10, 0
