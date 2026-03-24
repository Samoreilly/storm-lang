extern printf
section .text
global main

solve_factorial:
push rbp
mov rbp, rsp
sub rsp, 32
mov [rbp + -8], rdi
mov [rbp + -16], rsi
push 1
pop rax
mov [rbp + -24], rax
push 1
pop rax
mov [rbp + -32], rax
FOR_START_1:
mov rax, [rbp + -32]
push rax

mov rax, [rbp + -8]
push rax

pop rbx
pop rax
cmp rax, rbx
setle al
movzx rax, al
push rax
pop rax
cmp rax, 0
je FOR_END_2
mov rax, [rbp + -32]
push rax
pop rax
mov [rbp + -24], rax
mov rax, [rbp + -32]
inc rax
mov [rbp + -32], rax
jmp FOR_START_1
FOR_END_2:
mov rax, [rbp + -24]
push rax
pop rax
jmp solve_factorial_EXIT_0
solve_factorial_EXIT_0:
mov rsp, rbp
pop rbp
ret
main:
push rbp
mov rbp, rsp
sub rsp, 64
lea rax, [STR_0]
push rax
pop rax
mov [rbp + -8], rax
push 100000
pop rax
mov [rbp + -16], rax
mov rax, [DBL_0]
push rax
pop rax
movq xmm0, rax
cvttsd2si rax, xmm0
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
push 1
pop rax
mov [rbp + -32], rax
FOR_START_4:
mov rax, [rbp + -32]
push rax

mov rax, [rbp + -16]
push rax

pop rbx
pop rax
cmp rax, rbx
setle al
movzx rax, al
push rax
pop rax
cmp rax, 0
je FOR_END_5
mov rax, [rbp + -32]
push rax
push 0
pop rsi
pop rdi
mov rax, 0
call solve_factorial
push rax
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
push rax

push 5

pop rbx
pop rax
cmp rax, rbx
sete al
movzx rax, al
push rax
pop rax
cmp rax, 0
je IF_6
lea rax, [STR_1]
push rax
pop rax
mov [rbp + -48], rax
IF_6:
mov rax, [rbp + -32]
inc rax
mov [rbp + -32], rax
jmp FOR_START_4
FOR_END_5:
mov rax, [rbp + -24]
push rax
pop rdi
extern printf
mov rsi, rdi
lea rdi, [format_int]
mov rax, 0
call printf
lea rax, [STR_2]
push rax
pop rax
mov [rbp + -56], rax
main_EXIT_3:
mov rsp, rbp
pop rbp
ret

section .data
format_int: db "%d", 10, 0
format_str: db "%s", 10, 0
format_dbl: db "%f", 10, 0
section .data 
	STR_0: db "--- STORM RECURSION TEST ---", 10, 0
DBL_0:  dq 50.0
STR_1: db "MIDPOINT REACHED", 10, 0
STR_2: db "STORM COMPUTATION COMPLETE", 10, 0
