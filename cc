#!/bin/sh
# clang pointed at c.h: its headers, no system ones, c.o linked in. CHAMESLE_LIBC says where c.h lives
C=${CHAMESLE_LIBC:-$(dirname "$(readlink -f "$0")")}
link=1
for a in "$@"; do case $a in -c|-E|-S|-M|-MM) link=0;; esac; done
if [ $link = 1 ]; then
	exec clang -std=gnu11 -nostdinc -isystem "$C/include" -isystem "$(clang -print-resource-dir)/include" -ffreestanding -fno-builtin -fno-stack-protector \
		-fno-asynchronous-unwind-tables -fno-pie -static -nostdlib -fuse-ld=lld -Wl,--gc-sections -Wno-deprecated-non-prototype "$@" "$C/c.o"
fi
exec clang -std=gnu11 -nostdinc -isystem "$C/include" -isystem "$(clang -print-resource-dir)/include" -ffreestanding -fno-builtin -fno-stack-protector \
	-fno-asynchronous-unwind-tables -fno-pie -Wno-deprecated-non-prototype "$@"
