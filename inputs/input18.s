.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $3, %rax
  pushq %rax
 movq $3, %rax
  pushq %rax
  movq 0(%rsp), %rcx
  imulq 8(%rsp), %rcx
  addq $2, %rcx
  imulq $8, %rcx
  movq %rcx, %rdi
  call malloc@PLT
  popq %rcx
  movq %rcx, 8(%rax)
  popq %rcx
  movq %rcx, 0(%rax)
  movq %rax, -8(%rbp)
 movq $0, %rax
  movq %rax, -16(%rbp)
while_0:
 movq -16(%rbp), %rax
 pushq %rax
 movq $3, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movq $0, %rax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endwhile_0
 movq $0, %rax
  movq %rax, -24(%rbp)
while_1:
 movq -24(%rbp), %rax
 pushq %rax
 movq $3, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movq $0, %rax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endwhile_1
 movq -16(%rbp), %rax
 pushq %rax
 movq $3, %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 pushq %rax
 movq -24(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
  movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
  pushq %rax
  movq -8(%rbp), %rbx
  movq 8(%rbx), %r10
 movq -16(%rbp), %rax
  imulq %r10, %rax
  pushq %rax
 movq -24(%rbp), %rax
  popq %rcx
  addq %rcx, %rax
  addq $2, %rax
  salq $3, %rax
  addq %rbx, %rax
  movq %rax, %rcx
  popq %rax
  movq %rax, (%rcx)
 movq -24(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
  movq %rax, -24(%rbp)
 jmp while_1
endwhile_1:
 movq -16(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
  movq %rax, -16(%rbp)
 jmp while_0
endwhile_0:
  movq -8(%rbp), %rbx
  movq 8(%rbx), %r10
 movq $0, %rax
  imulq %r10, %rax
  pushq %rax
 movq $0, %rax
  popq %rcx
  addq %rcx, %rax
  addq $2, %rax
  salq $3, %rax
  addq %rbx, %rax
  movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movq $0, %rax
 call printf@PLT
  movq -8(%rbp), %rbx
  movq 8(%rbx), %r10
 movq $1, %rax
  imulq %r10, %rax
  pushq %rax
 movq $2, %rax
  popq %rcx
  addq %rcx, %rax
  addq $2, %rax
  salq $3, %rax
  addq %rbx, %rax
  movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movq $0, %rax
 call printf@PLT
  movq -8(%rbp), %rbx
  movq 8(%rbx), %r10
 movq $2, %rax
  imulq %r10, %rax
  pushq %rax
 movq $1, %rax
  popq %rcx
  addq %rcx, %rax
  addq $2, %rax
  salq $3, %rax
  addq %rbx, %rax
  movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movq $0, %rax
 call printf@PLT
 movq $0, %rax
 jmp .end_main
.end_main:
 leave
 ret

.section .note.GNU-stack,"",@progbits
