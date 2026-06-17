.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $42, %rax
  movq %rax, -8(%rbp)
  leaq -8(%rbp), %rax
  movq %rax, -16(%rbp)
 movq -16(%rbp), %rax
  movq (%rax), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movq $0, %rax
 call printf@PLT
 movq $100, %rax
  pushq %rax
 movq -16(%rbp), %rax
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
