.text

.globl _twoComplement

_twoComplement:

pushl %ebp
movl %esp, %ebp

mov 8(%ebp), %eax

xorl $0xff, %eax
incl %eax

leave
ret
