/* ---- system call numbers [chamesle] ---- */
#ifndef SYS_read
#define SYS_read 0
#endif
#ifndef SYS_write
#define SYS_write 1
#endif
#ifndef SYS_open
#define SYS_open 2
#endif
#ifndef SYS_close
#define SYS_close 3
#endif
#ifndef SYS_stat
#define SYS_stat 4
#endif
#ifndef SYS_fstat
#define SYS_fstat 5
#endif
#ifndef SYS_lstat
#define SYS_lstat 6
#endif
#ifndef SYS_poll
#define SYS_poll 7
#endif
#ifndef SYS_lseek
#define SYS_lseek 8
#endif
#ifndef SYS_mmap
#define SYS_mmap 9
#endif
#ifndef SYS_mprotect
#define SYS_mprotect 10
#endif
#ifndef SYS_munmap
#define SYS_munmap 11
#endif
#ifndef SYS_brk
#define SYS_brk 12
#endif
#ifndef SYS_rt_sigaction
#define SYS_rt_sigaction 13
#endif
#ifndef SYS_rt_sigprocmask
#define SYS_rt_sigprocmask 14
#endif
#ifndef SYS_rt_sigreturn
#define SYS_rt_sigreturn 15
#endif
#ifndef SYS_ioctl
#define SYS_ioctl 16
#endif
#ifndef SYS_pread64
#define SYS_pread64 17
#endif
#ifndef SYS_pwrite64
#define SYS_pwrite64 18
#endif
#ifndef SYS_readv
#define SYS_readv 19
#endif
#ifndef SYS_writev
#define SYS_writev 20
#endif
#ifndef SYS_access
#define SYS_access 21
#endif
#ifndef SYS_pipe
#define SYS_pipe 22
#endif
#ifndef SYS_select
#define SYS_select 23
#endif
#ifndef SYS_sched_yield
#define SYS_sched_yield 24
#endif
#ifndef SYS_mremap
#define SYS_mremap 25
#endif
#ifndef SYS_msync
#define SYS_msync 26
#endif
#ifndef SYS_madvise
#define SYS_madvise 28
#endif
#ifndef SYS_dup
#define SYS_dup 32
#endif
#ifndef SYS_dup2
#define SYS_dup2 33
#endif
#ifndef SYS_pause
#define SYS_pause 34
#endif
#ifndef SYS_nanosleep
#define SYS_nanosleep 35
#endif
#ifndef SYS_getitimer
#define SYS_getitimer 36
#endif
#ifndef SYS_alarm
#define SYS_alarm 37
#endif
#ifndef SYS_setitimer
#define SYS_setitimer 38
#endif
#ifndef SYS_getpid
#define SYS_getpid 39
#endif
#ifndef SYS_sendfile
#define SYS_sendfile 40
#endif
#ifndef SYS_socket
#define SYS_socket 41
#endif
#ifndef SYS_connect
#define SYS_connect 42
#endif
#ifndef SYS_accept
#define SYS_accept 43
#endif
#ifndef SYS_sendto
#define SYS_sendto 44
#endif
#ifndef SYS_recvfrom
#define SYS_recvfrom 45
#endif
#ifndef SYS_sendmsg
#define SYS_sendmsg 46
#endif
#ifndef SYS_recvmsg
#define SYS_recvmsg 47
#endif
#ifndef SYS_shutdown
#define SYS_shutdown 48
#endif
#ifndef SYS_bind
#define SYS_bind 49
#endif
#ifndef SYS_listen
#define SYS_listen 50
#endif
#ifndef SYS_getsockname
#define SYS_getsockname 51
#endif
#ifndef SYS_getpeername
#define SYS_getpeername 52
#endif
#ifndef SYS_socketpair
#define SYS_socketpair 53
#endif
#ifndef SYS_setsockopt
#define SYS_setsockopt 54
#endif
#ifndef SYS_getsockopt
#define SYS_getsockopt 55
#endif
#ifndef SYS_clone
#define SYS_clone 56
#endif
#ifndef SYS_fork
#define SYS_fork 57
#endif
#ifndef SYS_vfork
#define SYS_vfork 58
#endif
#ifndef SYS_execve
#define SYS_execve 59
#endif
#ifndef SYS_exit
#define SYS_exit 60
#endif
#ifndef SYS_wait4
#define SYS_wait4 61
#endif
#ifndef SYS_kill
#define SYS_kill 62
#endif
#ifndef SYS_uname
#define SYS_uname 63
#endif
#ifndef SYS_fcntl
#define SYS_fcntl 72
#endif
#ifndef SYS_flock
#define SYS_flock 73
#endif
#ifndef SYS_fsync
#define SYS_fsync 74
#endif
#ifndef SYS_fdatasync
#define SYS_fdatasync 75
#endif
#ifndef SYS_truncate
#define SYS_truncate 76
#endif
#ifndef SYS_ftruncate
#define SYS_ftruncate 77
#endif
#ifndef SYS_getdents
#define SYS_getdents 78
#endif
#ifndef SYS_getcwd
#define SYS_getcwd 79
#endif
#ifndef SYS_chdir
#define SYS_chdir 80
#endif
#ifndef SYS_fchdir
#define SYS_fchdir 81
#endif
#ifndef SYS_rename
#define SYS_rename 82
#endif
#ifndef SYS_mkdir
#define SYS_mkdir 83
#endif
#ifndef SYS_rmdir
#define SYS_rmdir 84
#endif
#ifndef SYS_creat
#define SYS_creat 85
#endif
#ifndef SYS_link
#define SYS_link 86
#endif
#ifndef SYS_unlink
#define SYS_unlink 87
#endif
#ifndef SYS_symlink
#define SYS_symlink 88
#endif
#ifndef SYS_readlink
#define SYS_readlink 89
#endif
#ifndef SYS_chmod
#define SYS_chmod 90
#endif
#ifndef SYS_fchmod
#define SYS_fchmod 91
#endif
#ifndef SYS_chown
#define SYS_chown 92
#endif
#ifndef SYS_fchown
#define SYS_fchown 93
#endif
#ifndef SYS_lchown
#define SYS_lchown 94
#endif
#ifndef SYS_umask
#define SYS_umask 95
#endif
#ifndef SYS_gettimeofday
#define SYS_gettimeofday 96
#endif
#ifndef SYS_getrlimit
#define SYS_getrlimit 97
#endif
#ifndef SYS_getrusage
#define SYS_getrusage 98
#endif
#ifndef SYS_sysinfo
#define SYS_sysinfo 99
#endif
#ifndef SYS_times
#define SYS_times 100
#endif
#ifndef SYS_getuid
#define SYS_getuid 102
#endif
#ifndef SYS_getgid
#define SYS_getgid 104
#endif
#ifndef SYS_setuid
#define SYS_setuid 105
#endif
#ifndef SYS_setgid
#define SYS_setgid 106
#endif
#ifndef SYS_geteuid
#define SYS_geteuid 107
#endif
#ifndef SYS_getegid
#define SYS_getegid 108
#endif
#ifndef SYS_setpgid
#define SYS_setpgid 109
#endif
#ifndef SYS_getppid
#define SYS_getppid 110
#endif
#ifndef SYS_getpgrp
#define SYS_getpgrp 111
#endif
#ifndef SYS_setsid
#define SYS_setsid 112
#endif
#ifndef SYS_setreuid
#define SYS_setreuid 113
#endif
#ifndef SYS_setregid
#define SYS_setregid 114
#endif
#ifndef SYS_getgroups
#define SYS_getgroups 115
#endif
#ifndef SYS_setgroups
#define SYS_setgroups 116
#endif
#ifndef SYS_setresuid
#define SYS_setresuid 117
#endif
#ifndef SYS_setresgid
#define SYS_setresgid 119
#endif
#ifndef SYS_getpgid
#define SYS_getpgid 121
#endif
#ifndef SYS_getsid
#define SYS_getsid 124
#endif
#ifndef SYS_rt_sigpending
#define SYS_rt_sigpending 127
#endif
#ifndef SYS_rt_sigtimedwait
#define SYS_rt_sigtimedwait 128
#endif
#ifndef SYS_rt_sigsuspend
#define SYS_rt_sigsuspend 130
#endif
#ifndef SYS_sigaltstack
#define SYS_sigaltstack 131
#endif
#ifndef SYS_utime
#define SYS_utime 132
#endif
#ifndef SYS_mknod
#define SYS_mknod 133
#endif
#ifndef SYS_statfs
#define SYS_statfs 137
#endif
#ifndef SYS_fstatfs
#define SYS_fstatfs 138
#endif
#ifndef SYS_getpriority
#define SYS_getpriority 140
#endif
#ifndef SYS_setpriority
#define SYS_setpriority 141
#endif
#ifndef SYS_prctl
#define SYS_prctl 157
#endif
#ifndef SYS_arch_prctl
#define SYS_arch_prctl 158
#endif
#ifndef SYS_setrlimit
#define SYS_setrlimit 160
#endif
#ifndef SYS_chroot
#define SYS_chroot 161
#endif
#ifndef SYS_sync
#define SYS_sync 162
#endif
#ifndef SYS_mount
#define SYS_mount 165
#endif
#ifndef SYS_umount2
#define SYS_umount2 166
#endif
#ifndef SYS_gettid
#define SYS_gettid 186
#endif
#ifndef SYS_time
#define SYS_time 201
#endif
#ifndef SYS_futex
#define SYS_futex 202
#endif
#ifndef SYS_getdents64
#define SYS_getdents64 217
#endif
#ifndef SYS_set_tid_address
#define SYS_set_tid_address 218
#endif
#ifndef SYS_clock_gettime
#define SYS_clock_gettime 228
#endif
#ifndef SYS_clock_getres
#define SYS_clock_getres 229
#endif
#ifndef SYS_clock_nanosleep
#define SYS_clock_nanosleep 230
#endif
#ifndef SYS_exit_group
#define SYS_exit_group 231
#endif
#ifndef SYS_tgkill
#define SYS_tgkill 234
#endif
#ifndef SYS_utimes
#define SYS_utimes 235
#endif
#ifndef SYS_openat
#define SYS_openat 257
#endif
#ifndef SYS_mkdirat
#define SYS_mkdirat 258
#endif
#ifndef SYS_mknodat
#define SYS_mknodat 259
#endif
#ifndef SYS_fchownat
#define SYS_fchownat 260
#endif
#ifndef SYS_newfstatat
#define SYS_newfstatat 262
#endif
#ifndef SYS_unlinkat
#define SYS_unlinkat 263
#endif
#ifndef SYS_renameat
#define SYS_renameat 264
#endif
#ifndef SYS_linkat
#define SYS_linkat 265
#endif
#ifndef SYS_symlinkat
#define SYS_symlinkat 266
#endif
#ifndef SYS_readlinkat
#define SYS_readlinkat 267
#endif
#ifndef SYS_fchmodat
#define SYS_fchmodat 268
#endif
#ifndef SYS_faccessat
#define SYS_faccessat 269
#endif
#ifndef SYS_ppoll
#define SYS_ppoll 271
#endif
#ifndef SYS_utimensat
#define SYS_utimensat 280
#endif
#ifndef SYS_accept4
#define SYS_accept4 288
#endif
#ifndef SYS_dup3
#define SYS_dup3 292
#endif
#ifndef SYS_pipe2
#define SYS_pipe2 293
#endif
#ifndef SYS_getrandom
#define SYS_getrandom 318
#endif
#ifndef SYS_copy_file_range
#define SYS_copy_file_range 326
#endif
#ifndef SYS_setsid
#define SYS_setsid 112
#endif
#ifndef SYS_fchmodat2
#define SYS_fchmodat2 452
#endif
