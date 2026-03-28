extern printf
section .text
global main

main:
push rbp
mov rbp, rsp
sub rsp, 48
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
push 0
pop rax
mov [rbp + -24], rax
push 0
push 0
pop rsi
pop rdi
mov rax, 0
call fun
push 1
pop rax
mov [rbp + -32], rax
FOR_START_1:
mov rax, [rbp + -32]
push rax

push 345

pop rbx
pop rax
cmp rax, rbx
setl al
movzx rax, al
push rax
pop rax
cmp rax, 0
je FOR_END_2
push 5
pop rax
mov [rbp + -40], rax
mov rax, [rbp + -40]
push rax
pop rdi
extern printf
mov rsi, rdi
lea rdi, [format_int]
mov rax, 0
call printf
mov rax, [rbp + -32]
inc rax
mov [rbp + -32], rax
jmp FOR_START_1
FOR_END_2:
main_EXIT_0:
mov rsp, rbp
pop rbp
ret
fun:
push rbp
mov rbp, rsp
sub rsp, 16
mov [rbp + -8], rdi
mov [rbp + -16], rsi
push 1
pop rax
mov [rbp + -8], rax
mov rax, [rbp + -16]
push rax
pop rax
mov [rbp + -16], rax
mov rax, [rbp + -8]
push rax
pop rdi
extern printf
mov rsi, rdi
lea rdi, [format_int]
mov rax, 0
call printf
mov rax, [rbp + -16]
push rax
mov rax, [rbp + -8]
push rax
pop rsi
pop rdi
mov rax, 0
call fun
pop rax
jmp fun_EXIT_3
fun_EXIT_3:
mov rsp, rbp
pop rbp
ret

section .data
format_int: db "%d", 10, 0
format_str: db "%s", 10, 0
format_dbl: db "%f", 10, 0
section .data 
	