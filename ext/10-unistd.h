/* ---- more of unistd, fcntl, sys/...: the thin system calls [chamesle] ---- */

#ifndef O_NONBLOCK
#define O_NONBLOCK 04000
#endif
#ifndef O_NOCTTY
#define O_NOCTTY 0400
#endif
#ifndef O_SYNC
#define O_SYNC 04010000
#endif
#ifndef O_DSYNC
#define O_DSYNC 010000
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0400000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 0200000
#endif
#ifndef O_NOATIME
#define O_NOATIME 01000000
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif
#ifndef O_ACCMODE
#define O_ACCMODE 3
#endif
#define O_NDELAY O_NONBLOCK
#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define F_GETLK 5
#define F_SETLK 6
#define F_SETLKW 7
#define F_DUPFD_CLOEXEC 1030
#define FD_CLOEXEC 1
#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8
#define AT_EACCESS 0x200
#define AT_SYMLINK_FOLLOW 0x400
#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif
#define _SC_ARG_MAX 0
#define _SC_CHILD_MAX 1
#define _SC_CLK_TCK 2
#define _SC_NGROUPS_MAX 3
#define _SC_OPEN_MAX 4
#define _SC_PAGESIZE 30
#define _SC_PAGE_SIZE 30
#define _SC_NPROCESSORS_CONF 83
#define _SC_NPROCESSORS_ONLN 84
#define _SC_PHYS_PAGES 85
#define _SC_GETPW_R_SIZE_MAX 70
#define _SC_GETGR_R_SIZE_MAX 69
#define _SC_HOST_NAME_MAX 180
#define _SC_LINE_MAX 43
#define _POSIX_VERSION 200809L
#define _POSIX_PATH_MAX 256
#define NGROUPS_MAX 65536
#define HOST_NAME_MAX 64
#define LOGIN_NAME_MAX 256
#define NAME_MAX 255
#define PIPE_BUF 4096
#define IOV_MAX 1024
#define SSIZE_MAX LONG_MAX
#define CLOCKS_PER_SEC 1000000

struct iovec { void *iov_base; size_t iov_len; };
ssize_t readv(int fd, const struct iovec *v, int n) { return fail(sys3(SYS_readv, fd, v, n)); }
ssize_t writev(int fd, const struct iovec *v, int n) { return fail(sys3(SYS_writev, fd, v, n)); }

int dup3(int a, int b, int flags) { return fail(sys3(SYS_dup3, a, b, flags)); }
int pipe2(int fd[2], int flags) { return fail(sys2(SYS_pipe2, fd, flags)); }
int flock(int fd, int op) { return fail(sys2(SYS_flock, fd, op)); }
pid_t vfork(void) { return fork(); }
pid_t getpgrp(void) { return sys0(SYS_getpgrp); }
pid_t getpgid(pid_t p) { return fail(sys1(SYS_getpgid, p)); }
int setpgid(pid_t p, pid_t g) { return fail(sys2(SYS_setpgid, p, g)); }
int setpgrp(void) { return setpgid(0, 0); }
pid_t getsid(pid_t p) { return fail(sys1(SYS_getsid, p)); }
pid_t setsid(void) { return fail(sys0(SYS_setsid)); }
int setreuid(uid_t r, uid_t e) { return fail(sys2(SYS_setreuid, r, e)); }
int setregid(gid_t r, gid_t e) { return fail(sys2(SYS_setregid, r, e)); }
int setresuid(uid_t r, uid_t e, uid_t s) { return fail(sys3(SYS_setresuid, r, e, s)); }
int setresgid(gid_t r, gid_t e, gid_t s) { return fail(sys3(SYS_setresgid, r, e, s)); }
int seteuid(uid_t e) { return setresuid(-1, e, -1); }
int setegid(gid_t e) { return setresgid(-1, e, -1); }
int setgroups(size_t n, const gid_t *l) { return fail(sys2(SYS_setgroups, n, l)); }
pid_t gettid(void) { return sys0(SYS_gettid); }
unsigned alarm(unsigned s)
{
    struct { long ti[2], tv[2]; } nw = { { 0, 0 }, { s, 0 } }, old = { { 0, 0 }, { 0, 0 } };
    if (fail(sys3(SYS_setitimer, 0, &nw, &old)) < 0) return 0;
    return old.tv[0] + (old.tv[1] ? 1 : 0);
}
int pause(void) { return fail(sys0(SYS_pause)); }
unsigned usleep(unsigned us) { struct timespec t = { us / 1000000, (us % 1000000) * 1000L }; return nanosleep(&t, NULL); }
int clock_getres(int clk, struct timespec *t) { return fail(sys2(SYS_clock_getres, clk, t)); }
int clock_nanosleep(int clk, int flags, const struct timespec *t, struct timespec *rem) { long r = sys4(SYS_clock_nanosleep, clk, flags, t, rem); return r < 0 && r > -4096 ? -r : 0; }
int getpagesize(void) { return 4096; }
int nice(int inc) { int cur = getpriority(0, 0); if (cur < 0) return -1; return setpriority(0, 0, 20 - cur + inc) < 0 ? -1 : 20 - getpriority(0, 0); }
int fchmodat(int dirfd, const char *p, mode_t m, int flags) { (void)flags; return fail(sys3(SYS_fchmodat, dirfd, p, m)); }
int fchownat(int dirfd, const char *p, uid_t u, gid_t g, int flags) { return fail(sys6(SYS_fchownat, dirfd, (long)p, u, g, flags, 0)); }
int faccessat(int dirfd, const char *p, int mode, int flags) { (void)flags; return fail(sys3(SYS_faccessat, dirfd, p, mode)); }
int renameat(int a, const char *p, int b, const char *q) { return fail(sys4(SYS_renameat, a, p, b, q)); }
int linkat(int a, const char *p, int b, const char *q, int flags) { return fail(sys6(SYS_linkat, a, (long)p, b, (long)q, flags, 0)); }
int symlinkat(const char *t, int dirfd, const char *p) { return fail(sys3(SYS_symlinkat, t, dirfd, p)); }
ssize_t readlinkat(int dirfd, const char *p, char *buf, size_t n) { return fail(sys4(SYS_readlinkat, dirfd, p, buf, n)); }
int mknodat(int dirfd, const char *p, mode_t m, dev_t d) { return fail(sys4(SYS_mknodat, dirfd, p, m, d)); }
int mkfifoat(int dirfd, const char *p, mode_t m) { return mknodat(dirfd, p, (m & 07777) | S_IFIFO, 0); }
int fstatfs(int fd, struct statfs *s) { return fail(sys2(SYS_fstatfs, fd, s)); }
int utimes(const char *path, const struct timeval tv[2])
{
    struct timespec t[2];
    if (!tv) return utimensat(AT_FDCWD, path, NULL, 0);
    t[0].tv_sec = tv[0].tv_sec; t[0].tv_nsec = tv[0].tv_usec * 1000;
    t[1].tv_sec = tv[1].tv_sec; t[1].tv_nsec = tv[1].tv_usec * 1000;
    return utimensat(AT_FDCWD, path, t, 0);
}
int futimens(int fd, const struct timespec t[2]) { return fail(sys6(SYS_utimensat, fd, 0, (long)t, 0, 0, 0)); }
struct utimbuf { time_t actime, modtime; };
int utime(const char *path, const struct utimbuf *u)
{
    struct timespec t[2];
    if (!u) return utimensat(AT_FDCWD, path, NULL, 0);
    t[0].tv_sec = u->actime; t[0].tv_nsec = 0; t[1].tv_sec = u->modtime; t[1].tv_nsec = 0;
    return utimensat(AT_FDCWD, path, t, 0);
}
int lockf(int fd, int cmd, off_t len)
{
    (void)len;
    return flock(fd, cmd == 0 ? LOCK_UN : cmd == 1 || cmd == 2 ? LOCK_EX : cmd == 3 ? LOCK_EX | LOCK_NB : LOCK_UN);
}
int posix_fadvise(int fd, off_t o, off_t l, int a) { (void)fd; (void)o; (void)l; (void)a; return 0; }
int posix_fallocate(int fd, off_t o, off_t l) { return fail(sys4(285, fd, 0, o, l)) < 0 ? errno : 0; }
ssize_t sendfile(int out, int in, off_t *off, size_t n) { return fail(sys4(SYS_sendfile, out, in, off, n)); }
ssize_t copy_file_range(int in, off_t *io, int out, off_t *oo, size_t n, unsigned flags) { return fail(sys6(SYS_copy_file_range, in, (long)io, out, (long)oo, n, flags)); }

int gethostname(char *buf, size_t n)
{
    struct utsname u;
    if (uname(&u) < 0) return -1;
    if (strlen(u.nodename) >= n) { errno = ENAMETOOLONG; return -1; }
    strcpy(buf, u.nodename);
    return 0;
}
int getdomainname(char *buf, size_t n) { struct utsname u; if (uname(&u) < 0) return -1; strncpy(buf, u.domainname, n); return 0; }
char *getlogin(void) { struct passwd *p = getpwuid(getuid()); return p ? p->pw_name : getenv("LOGNAME"); }
int getlogin_r(char *buf, size_t n) { char *l = getlogin(); if (!l) return ENOENT; strncpy(buf, l, n); return 0; }
char *ttyname(int fd)
{
    static char buf[64];
    char p[32];
    ssize_t n;
    snprintf(p, sizeof p, "/proc/self/fd/%d", fd);
    n = readlink(p, buf, sizeof buf - 1);
    if (n < 0) return NULL;
    buf[n] = 0;
    return buf;
}
int ttyname_r(int fd, char *buf, size_t n) { char *t = ttyname(fd); if (!t) return errno; strncpy(buf, t, n); return 0; }
char *ctermid(char *s) { static char d[] = "/dev/tty"; if (!s) return d; strcpy(s, d); return s; }
pid_t tcgetpgrp(int fd) { int p; if (ioctl(fd, 0x540F, &p) < 0) return -1; return p; }
int tcsetpgrp(int fd, pid_t p) { return ioctl(fd, 0x5410, &p); }
pid_t tcgetsid(int fd) { int p; if (ioctl(fd, 0x5429, &p) < 0) return -1; return p; }
long pathconf(const char *p, int name) { (void)p; (void)name; return name == 3 ? 255 : 4096; }
long fpathconf(int fd, int name) { (void)fd; return pathconf("/", name); }
char *getpass(const char *prompt) { static char buf[128]; fputs(prompt, stderr); if (!fgets(buf, sizeof buf, stdin)) return NULL; buf[strcspn(buf, "\n")] = 0; return buf; }
int daemon(int nochdir, int noclose)
{
    pid_t p = fork();
    if (p < 0) return -1;
    if (p) _exit(0);
    if (setsid() < 0) return -1;
    if (!nochdir) chdir("/");
    if (!noclose) { int fd = open("/dev/null", O_RDWR); if (fd >= 0) { dup2(fd, 0); dup2(fd, 1); dup2(fd, 2); if (fd > 2) close(fd); } }
    return 0;
}

/* exec and spawn */
extern char **environ;
int execv(const char *path, char *const argv[]) { return execve(path, argv, environ); }
int execvpe(const char *file, char *const argv[], char *const envp[])
{
    char **save = environ;
    int r;
    environ = (char **)envp;
    r = execvp(file, argv);
    environ = save;
    return r;
}
int execle(const char *path, const char *arg, ...)
{
    char *av[256];
    char **envp;
    int n = 0;
    va_list ap;
    va_start(ap, arg);
    av[n++] = (char *)arg;
    while (n < 255 && (av[n] = va_arg(ap, char *))) n++;
    envp = va_arg(ap, char **);
    va_end(ap);
    return execve(path, av, envp);
}
int execlp(const char *file, const char *arg, ...)
{
    char *av[256];
    int n = 0;
    va_list ap;
    va_start(ap, arg);
    av[n++] = (char *)arg;
    while (n < 255 && (av[n] = va_arg(ap, char *))) n++;
    va_end(ap);
    return execvp(file, av);
}
int system(const char *cmd)
{
    pid_t pid;
    int st;
    if (!cmd) return 1;
    fflush(NULL);
    pid = fork();
    if (pid < 0) return -1;
    if (!pid) {
        char *av[] = { "sh", "-c", (char *)cmd, NULL };
        execve("/bin/sh", av, environ);
        _exit(127);
    }
    while (waitpid(pid, &st, 0) < 0) if (errno != EINTR) return -1;
    return st;
}
FILE *popen(const char *cmd, const char *mode)
{
    int p[2], parent_end, child_end;
    pid_t pid;
    FILE *f;
    int rd = mode[0] == 'r';
    if (pipe(p) < 0) return NULL;
    parent_end = rd ? p[0] : p[1]; child_end = rd ? p[1] : p[0];
    fflush(NULL);
    pid = fork();
    if (pid < 0) { close(p[0]); close(p[1]); return NULL; }
    if (!pid) {
        char *av[] = { "sh", "-c", (char *)cmd, NULL };
        close(parent_end);
        if (child_end != (rd ? 1 : 0)) { dup2(child_end, rd ? 1 : 0); close(child_end); }
        execve("/bin/sh", av, environ);
        _exit(127);
    }
    close(child_end);
    f = fdopen(parent_end, rd ? "r" : "w");
    if (!f) { close(parent_end); return NULL; }
    f->pid = pid;
    return f;
}
int pclose(FILE *f)
{
    int st;
    pid_t pid = f->pid;
    fclose(f);
    while (waitpid(pid, &st, 0) < 0) if (errno != EINTR) return -1;
    return st;
}

/* wait */
#define WNOHANG 1
#define WUNTRACED 2
#define WCONTINUED 8
#define WEXITSTATUS(s) (((s) >> 8) & 0xff)
#define WTERMSIG(s) ((s) & 0x7f)
#define WSTOPSIG(s) WEXITSTATUS(s)
#define WIFEXITED(s) (!WTERMSIG(s))
#define WIFSIGNALED(s) ((signed char)(((s) & 0x7f) + 1) >> 1 > 0)
#define WIFSTOPPED(s) (((s) & 0xff) == 0x7f)
#define WIFCONTINUED(s) ((s) == 0xffff)
#define WCOREDUMP(s) ((s) & 0x80)
struct rusage { struct timeval ru_utime, ru_stime; long ru_maxrss, ru_ixrss, ru_idrss, ru_isrss, ru_minflt, ru_majflt, ru_nswap, ru_inblock, ru_oublock, ru_msgsnd, ru_msgrcv, ru_nsignals, ru_nvcsw, ru_nivcsw; };
#define RUSAGE_SELF 0
#define RUSAGE_CHILDREN (-1)
pid_t wait3(int *st, int options, struct rusage *ru) { return fail(sys4(SYS_wait4, -1, st, options, ru)); }
pid_t wait4(pid_t pid, int *st, int options, struct rusage *ru) { return fail(sys4(SYS_wait4, pid, st, options, ru)); }
int getrusage(int who, struct rusage *ru) { return fail(sys2(SYS_getrusage, who, ru)); }
struct rlimit { unsigned long rlim_cur, rlim_max; };
typedef unsigned long rlim_t;
#define RLIM_INFINITY (~0UL)
#define RLIMIT_CPU 0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA 2
#define RLIMIT_STACK 3
#define RLIMIT_CORE 4
#define RLIMIT_NOFILE 7
#define RLIMIT_AS 9
int getrlimit(int r, struct rlimit *l) { return fail(sys2(SYS_getrlimit, r, l)); }
int setrlimit(int r, const struct rlimit *l) { return fail(sys2(SYS_setrlimit, r, l)); }
struct tms { long tms_utime, tms_stime, tms_cutime, tms_cstime; };
clock_t times(struct tms *t) { return fail(sys1(SYS_times, t)); }
clock_t clock(void) { struct timespec t; if (clock_gettime(2, &t) < 0) return -1; return t.tv_sec * 1000000L + t.tv_nsec / 1000; }
struct sysinfo { long uptime; unsigned long loads[3], totalram, freeram, sharedram, bufferram, totalswap, freeswap; unsigned short procs, pad; unsigned long totalhigh, freehigh; unsigned mem_unit; char f[4]; };
int sysinfo(struct sysinfo *s) { return fail(sys1(SYS_sysinfo, s)); }
int get_nprocs(void) { unsigned long m[16]; long r = sys3(SYS_sched_getaffinity, 0, sizeof m, m); int i, n = 0; if (r < 0) return 1; for (i = 0; i < 16; i++) n += __builtin_popcountl(m[i]); return n ? n : 1; }
int prctl(int op, unsigned long a, unsigned long b, unsigned long c, unsigned long d) { return fail(sys6(SYS_prctl, op, a, b, c, d, 0)); }

/* memory */
#ifndef MAP_PRIVATE
#define MAP_SHARED 1
#define MAP_PRIVATE 2
#endif
#ifndef MAP_ANONYMOUS
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS
#define MAP_NORESERVE 0x4000
#define MAP_POPULATE 0x8000
#endif
#define MAP_FAILED ((void *)-1)
#define PROT_NONE 0
#define MS_ASYNC 1
#define MS_INVALIDATE 2
#define MS_SYNC 4
#define MADV_NORMAL 0
#define MADV_RANDOM 1
#define MADV_SEQUENTIAL 2
#define MADV_WILLNEED 3
#define MADV_DONTNEED 4
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off)
{
#ifdef __i386__
    long r = sys6(SYS_mmap, (long)addr, len, prot, flags, fd, (long)(off >> 12));  /* mmap2 counts in pages */
#else
    long r = sys6(SYS_mmap, (long)addr, len, prot, flags, fd, off);
#endif
    if (r < 0 && r > -4096) { errno = -r; return MAP_FAILED; }
    return (void *)r;
}
int munmap(void *a, size_t n) { return fail(sys2(SYS_munmap, a, n)); }
int msync(void *a, size_t n, int f) { return fail(sys3(SYS_msync, a, n, f)); }
int madvise(void *a, size_t n, int adv) { return fail(sys3(SYS_madvise, a, n, adv)); }
int posix_madvise(void *a, size_t n, int adv) { return madvise(a, n, adv); }
int mlock(const void *a, size_t n) { (void)a; (void)n; return 0; }
int munlock(const void *a, size_t n) { (void)a; (void)n; return 0; }

/* the ioctl / termios corner */
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define FIONREAD 0x541B
#define FIONBIO 0x5421
#define TIOCSCTTY 0x540E
struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };
#define B0 0
#define B9600 015
#define B19200 016
#define B38400 017
#define B57600 010001
#define B115200 010002
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2
#define TCOOFF 0
#define TCOON 1
typedef unsigned int speed_t;
typedef unsigned char cc_t;
typedef unsigned int tcflag_t;
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VTIME 5
#define VMIN 6
#define VSTART 8
#define VSTOP 9
#define VSUSP 10
#define NCCS 32
#define IGNBRK 1
#define BRKINT 2
#define IGNPAR 4
#define PARMRK 8
#define INPCK 16
#define ISTRIP 32
#define INLCR 64
#define IGNCR 128
#define ECHONL 0100
#define IXON 02000
#define IXANY 04000
#define IXOFF 010000
#define CSIZE 060
#define CS5 0
#define CS6 020
#define CS7 040
#define CS8 060
#define CSTOPB 0100
#define CREAD 0200
#define PARENB 0400
#define PARODD 01000
#define HUPCL 02000
#define CLOCAL 04000
#define CBAUD 010017
int tcflush(int fd, int q) { return ioctl(fd, 0x540B, (void *)(long)q); }
int tcdrain(int fd) { return ioctl(fd, 0x5409, (void *)1L); }
int tcflow(int fd, int a) { return ioctl(fd, 0x540A, (void *)(long)a); }
int tcsendbreak(int fd, int d) { (void)d; return ioctl(fd, 0x5409, (void *)0L); }
speed_t cfgetospeed(const struct termios *t) { return t->c_cflag & CBAUD; }
speed_t cfgetispeed(const struct termios *t) { return t->c_cflag & CBAUD; }
int cfsetospeed(struct termios *t, speed_t s) { t->c_cflag = (t->c_cflag & ~CBAUD) | s; return 0; }
int cfsetispeed(struct termios *t, speed_t s) { (void)t; (void)s; return 0; }
int cfsetspeed(struct termios *t, speed_t s) { return cfsetospeed(t, s); }
void cfmakeraw(struct termios *t)
{
    t->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    t->c_oflag &= ~OPOST;
    t->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    t->c_cflag &= ~(CSIZE | PARENB);
    t->c_cflag |= CS8;
    t->c_cc[VMIN] = 1; t->c_cc[VTIME] = 0;
}
int posix_openpt(int flags) { return open("/dev/ptmx", flags); }
int grantpt(int fd) { (void)fd; return 0; }
int unlockpt(int fd) { int z = 0; return ioctl(fd, 0x40045431, &z); }
char *ptsname(int fd)
{
    static char buf[32];
    int n;
    if (ioctl(fd, 0x80045430, &n) < 0) return NULL;
    snprintf(buf, sizeof buf, "/dev/pts/%d", n);
    return buf;
}

/* getopt */
char *optarg;
int optind = 1, opterr = 1, optopt;
static int c_optpos;
int getopt(int argc, char *const argv[], const char *shorts)
{
    const char *p;
    char *a;
    int c;
    if (optind == 0) { optind = 1; c_optpos = 0; }
    if (!c_optpos) {
        if (optind >= argc) return -1;
        a = argv[optind];
        if (a[0] != '-' || !a[1]) return -1;
        if (a[1] == '-' && !a[2]) { optind++; return -1; }
        c_optpos = 1;
    }
    a = argv[optind];
    c = a[c_optpos++];
    p = strchr(shorts[0] == '+' || shorts[0] == ':' ? shorts + 1 : shorts, c);
    if (!a[c_optpos]) { optind++; c_optpos = 0; }
    if (!p || c == ':') {
        optopt = c;
        if (opterr && shorts[0] != ':') fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], c);
        return '?';
    }
    if (p[1] == ':') {
        if (c_optpos) { optarg = argv[optind] + c_optpos; optind++; c_optpos = 0; }
        else if (p[2] == ':') optarg = NULL;
        else if (optind >= argc) {
            optopt = c;
            if (opterr && shorts[0] != ':') fprintf(stderr, "%s: option requires an argument -- '%c'\n", argv[0], c);
            return shorts[0] == ':' ? ':' : '?';
        } else optarg = argv[optind++];
    } else optarg = NULL;
    return c;
}
struct option { const char *name; int has_arg; int *flag; int val; };
#define no_argument 0
#define required_argument 1
#define optional_argument 2
int getopt_long(int argc, char *const argv[], const char *shorts, const struct option *longs, int *idx)
{
    char *a;
    const struct option *o, *hit = NULL;
    size_t l;
    char *eq;
    if (optind == 0) { optind = 1; c_optpos = 0; }
    if (c_optpos || optind >= argc) return getopt(argc, argv, shorts);
    a = argv[optind];
    if (a[0] != '-' || a[1] != '-') return getopt(argc, argv, shorts);
    if (!a[2]) { optind++; return -1; }
    eq = strchr(a + 2, '=');
    l = eq ? (size_t)(eq - a - 2) : strlen(a + 2);
    for (o = longs; o->name; o++) {
        if (!strncmp(o->name, a + 2, l)) {
            if (strlen(o->name) == l) { hit = o; break; }
            if (!hit) hit = o;
            else if (hit->has_arg != o->has_arg || hit->flag != o->flag || hit->val != o->val) { if (opterr) fprintf(stderr, "%s: option '%s' is ambiguous\n", argv[0], a); optind++; return '?'; }
        }
    }
    if (!hit) { if (opterr) fprintf(stderr, "%s: unrecognized option '%s'\n", argv[0], a); optind++; return '?'; }
    optind++;
    if (idx) *idx = hit - longs;
    if (hit->has_arg == no_argument) {
        if (eq) { if (opterr) fprintf(stderr, "%s: option '--%s' doesn't allow an argument\n", argv[0], hit->name); return '?'; }
        optarg = NULL;
    } else if (eq) optarg = eq + 1;
    else if (hit->has_arg == required_argument) {
        if (optind >= argc) { if (opterr) fprintf(stderr, "%s: option '--%s' requires an argument\n", argv[0], hit->name); return shorts[0] == ':' ? ':' : '?'; }
        optarg = argv[optind++];
    } else optarg = NULL;
    if (hit->flag) { *hit->flag = hit->val; return 0; }
    return hit->val;
}
int getopt_long_only(int argc, char *const argv[], const char *shorts, const struct option *longs, int *idx) { return getopt_long(argc, argv, shorts, longs, idx); }
