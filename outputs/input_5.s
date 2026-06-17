.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
 movq $1, %rax
  pushq %rax
 movq $2, %rax
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
 movq $2, %rax
  pushq %rax
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
  movq %rax, %rcx
  popq %rax
  movq %rax, (%rcx)
 movq $3, %rax
  pushq %rax
  movq -8(%rbp), %rbx
  movq 8(%rbx), %r10
 movq $0, %rax
  imulq %r10, %rax
  pushq %rax
 movq $1, %rax
  popq %rcx
  addq %rcx, %rax
  addq $2, %rax
  salq $3, %rax
  addq %rbx, %rax
  movq %rax, %rcx
  popq %rax
  movq %rax, (%rcx)
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
 movq $0, %rax
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
