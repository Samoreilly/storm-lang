extern printf
section .text
global main

sum:
push rbp
mov rbp, rsp
sub rsp, 16
mov [rbp + -8], rdi
mov [rbp + -16], rsi
mov rax, [rbp + -8]
push rax

mov rax, [rbp + -16]
push rax

pop rbx
pop rax
add rax, rbx
push rax
pop rax
jmp sum_EXIT_0
sum_EXIT_0:
mov rsp, rbp
pop rbp
ret
fibonaci:
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
je IF_2
mov rax, [rbp + -8]
push rax
pop rax
jmp fibonaci_EXIT_1
IF_2:
mov rax, [rbp + -8]
push rax

push 1

pop rbx
pop rax
sub rax, rbx
push rax
pop rdi
call fibonaci
push rax

mov rax, [rbp + -8]
push rax

push 2

pop rbx
pop rax
sub rax, rbx
push rax
pop rdi
call fibonaci
push rax

pop rbx
pop rax
add rax, rbx
push rax
pop rax
jmp fibonaci_EXIT_1
fibonaci_EXIT_1:
mov rsp, rbp
pop rbp
ret
main:
push rbp
mov rbp, rsp
sub rsp, 16
push 10
pop rdi
call fibonaci
push rax
pop rdi
mov rsi, rdi
extern printf
lea rdi, [format_int]
mov rax, 0
call printf
push rax
push 1
push 2
pop rsi
pop rdi
call sum
push rax
pop rax
mov [rbp + -8], rax
mov rax, [rbp + -8]
push rax
pop rdi
mov rsi, rdi
extern printf
lea rdi, [format_int]
mov rax, 0
call printf
push rax
main_EXIT_3:
mov rsp, rbp
pop rbp
ret

section .data
format_int: db "%d", 10, 0
section .data 
	