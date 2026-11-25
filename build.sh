#!/bin/sh
# c.core.h and ext/*.h are the libc as written. this joins them, cuts the declarations-only form out of it (c.h), and builds c.o
set -e
cd "$(dirname "$0")"
{ cat c.core.h ext/*.h; printf '\n#endif\n'; } > c.all.h
python3 split.py c.all.h c.h
rm c.all.h
${CC:-clang} -std=gnu11 -O2 -Wall -Wno-deprecated-non-prototype -Wno-empty-body -ffreestanding -fno-builtin -fno-stack-protector \
    -fno-asynchronous-unwind-tables -fno-pie -ffunction-sections -fdata-sections -c c.c -o c.o
