.data
print_fmt: .string "%ld \n"

.text

.globl area
area:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
 movq %rdi, -8(%rbp)
  movq -8(%rbp), %rax
  movq 0(%rax), %rax
 pushq %rax
  movq -8(%rbp), %rax
  movq 8(%rax), %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 jmp .end_area
.end_area:
 leave
 ret

.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
  movq $16, %rdi
  call malloc@PLT
  movq %rax, -8(%rbp)
 movq $5, %rax
  pushq %rax
  movq -8(%rbp), %rcx
  popq %rax
  movq %rax, 0(%rcx)
 movq $8, %rax
  pushq %rax
  movq -8(%rbp), %rcx
  popq %rax
  movq %rax, 8(%rcx)
 movq -8(%rbp), %rax
 movq %rax, %rdi
 call area
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
