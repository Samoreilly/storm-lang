extern printf
section .text
global main

main:
push rbp
mov rbp, rsp
sub rsp, 32
push 1

push 5

pop rbx
pop rax
cmp rax, rbx
setl al
movzx rax, al
push rax
pop rax
cmp rax, 0
je IF_1
lea rax, [STR_0]
push rax
pop rdi
extern printf
mov rsi, rdi
lea rdi, [format_str]
mov rax, 0
call printf
IF_1:
push 5
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
push 40
pop rdi
mov rax, 0
call fib
push rax
pop rax
mov [rbp + -16], rax
mov rax, [rbp + -16]
push rax
pop rdi
extern printf
mov rsi, rdi
lea rdi, [format_int]
mov rax, 0
call printf
push 2
pop rdi
mov rax, 0
call fib
push rax
pop rax
mov [rbp + -24], rax
mov rax, [rbp + -24]
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
fib:
push rbp
mov rbp, rsp
sub rsp, 16
mov [rbp + -8], rdi
mov rax, [rbp + -8]
push rax

push 2

pop rbx
pop rax
cmp rax, rbx
setl al
movzx rax, al
push rax
pop rax
cmp rax, 0
je IF_3
mov rax, [rbp + -8]
push rax
pop rax
jmp fib_EXIT_2
IF_3:
mov rax, [rbp + -8]
push rax

push 1

pop rbx
pop rax
sub rax, rbx
push rax
pop rdi
mov rax, 0
call fib
push rax

mov rax, [rbp + -8]
push rax

push 2

pop rbx
pop rax
sub rax, rbx
push rax
pop rdi
mov rax, 0
call fib
push rax

pop rbx
pop rax
add rax, rbx
push rax
pop rax
jmp fib_EXIT_2
fib_EXIT_2:
mov rsp, rbp
pop rbp
ret

section .data
format_int: db "%d", 10, 0
format_str: db "%s", 10, 0
format_dbl: db "%f", 10, 0
section .data 
	STR_0: db "hello", 10, 0
