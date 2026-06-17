.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq $10, %rax
  movq %rax, -8(%rbp)
 movq $20, %rax
  movq %rax, -16(%rbp)
  leaq -8(%rbp), %rax
  movq %rax, -24(%rbp)
  leaq -16(%rbp), %rax
  movq %rax, -32(%rbp)
 movq -24(%rbp), %rax
  movq (%rax), %rax
 pushq %rax
 movq -32(%rbp), %rax
  movq (%rax), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movq $0, %rax
 call printf@PLT
 movq -32(%rbp), %rax
  movq (%rax), %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
  pushq %rax
 movq -24(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  movq %rax, (%rcx)
 movq -8(%rbp), %rax
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
