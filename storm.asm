extern printf
section .text
global main

fib:
push rbp
mov rbp, rsp
sub rsp, 16
mov [rbp + -8], rdi
mov rax, [rbp + -8]
push rax

push 1

pop rbx
pop rax
cmp rax, rbx
setle al
movzx rax, al
push rax
pop rax
cmp rax, 0
je IF_1
mov rax, [rbp + -8]
push rax
pop rax
jmp fib_EXIT_0
IF_1:
mov rax, [rbp + -8]
push rax

push 1

pop rbx
pop rax
sub rax, rbx
push rax
pop rdi
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
call fib
push rax

pop rbx
pop rax
add rax, rbx
push rax
pop rax
jmp fib_EXIT_0
fib_EXIT_0:
mov rsp, rbp
pop rbp
ret
main:
push rbp
mov rbp, rsp
sub rsp, 0
push 10
pop rdi
call fib
push rax
pop rdi
mov rsi, rdi
extern printf
lea rdi, [format_int]
mov rax, 0
call printf
push rax
push 1
pop rdi
mov rsi, rdi
extern printf
lea rdi, [format_int]
mov rax, 0
call printf
push rax
main_EXIT_2:
mov rsp, rbp
pop rbp
ret

section .data
format_int: db "%d", 10, 0
section .data 
	