c.h
===

The libc of Chamesle in one header. Programs include c.h instead of a libc and build static, with no libc
under them: mag, rifle and reach are built this way. x86_64 and i386 Linux; an i386 build runs on 32 and 64-bit
x86 alike.

    cc -static -nostdlib -ffreestanding -fno-builtin -fuse-ld=lld -o prog prog.c      prog.c includes "c.h"
    cc ... -m32 -msse2 ...                                                             the i386 build

c.h is made from c.core.h and ext/*.h by ./build.sh, which also builds c.o for programs of several files
(include c.h with C_DECLS_ONLY defined and link c.o). include/ has the usual header names, all pointing at c.h.
