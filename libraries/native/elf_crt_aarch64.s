.global _start
.global ___putc
.global _mmap
.global setjmp
.global longjmp
.type _start,%function
.type ___putc,%function
.type _mmap,%function
.type setjmp,%function
.type longjmp,%function

_start:
   mov x29, sp
   ldr x0, [sp]          // argc
   add x1, sp, #8        // argv
   bl _wrap_main
   mov x8, #93           // __NR_exit
   svc #0

___putc:
   sub sp, sp, #16
   strb w0, [sp]
   mov x0, #1            // fd = stdout
   mov x1, sp            // buf
   mov x2, #1            // count
   mov x8, #64           // __NR_write
   svc #0
   add sp, sp, #16
   ret

_mmap:
   mov x0, #0            // addr = NULL
   mov x1, #0x6400000    // len = 100MB
   mov x2, #3            // prot = PROT_READ|PROT_WRITE
   mov x3, #0x22         // flags = MAP_PRIVATE|MAP_ANONYMOUS
   mov x4, #-1           // fd = -1
   mov x5, #0            // offset = 0
   mov x8, #222          // __NR_mmap
   svc #0
   ret

setjmp:
   stp x19, x20, [x0, #0]
   stp x21, x22, [x0, #16]
   stp x23, x24, [x0, #32]
   stp x25, x26, [x0, #48]
   stp x27, x28, [x0, #64]
   stp x29, x30, [x0, #80]
   mov x1, sp
   str x1, [x0, #96]
   mov x0, #0
   ret

longjmp:
   ldp x19, x20, [x0, #0]
   ldp x21, x22, [x0, #16]
   ldp x23, x24, [x0, #32]
   ldp x25, x26, [x0, #48]
   ldp x27, x28, [x0, #64]
   ldp x29, x30, [x0, #80]
   ldr x2, [x0, #96]
   mov sp, x2
   cmp w1, #0
   csinc w0, w1, wzr, ne  // return val ?: 1
   ret
