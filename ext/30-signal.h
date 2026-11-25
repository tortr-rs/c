/* ---- signals the POSIX way [chamesle] ---- */
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGIOT 6
#define SIGBUS 7
#define SIGFPE 8
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGSTKFLT 16
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGIO 29
#define SIGPOLL 29
#define SIGPWR 30
#define SIGSYS 31
#define NSIG 65
#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO 4
#define SA_ONSTACK 0x08000000
#define SA_RESTART 0x10000000
#define SA_NODEFER 0x40000000
#define SA_RESETHAND 0x80000000
#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2
typedef int sig_atomic_t;
typedef struct { unsigned long __bits[16]; } sigset_t;
typedef struct {
    int si_signo, si_errno, si_code, __pad0;
    union {
        struct { pid_t si_pid; uid_t si_uid; int si_status; };
        void *si_addr;
        char __pad[112];
    };
} siginfo_t;
struct sigaction {
    union { sighandler_t sa_handler; void (*sa_sigaction)(int, siginfo_t *, void *); };
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};
typedef struct { void *ss_sp; int ss_flags; size_t ss_size; } stack_t;
#define SS_ONSTACK 1
#define SS_DISABLE 2
#define MINSIGSTKSZ 2048
#define SIGSTKSZ 8192

int sigemptyset(sigset_t *s) { memset(s, 0, sizeof *s); return 0; }
int sigfillset(sigset_t *s) { memset(s, 0xff, sizeof *s); return 0; }
int sigaddset(sigset_t *s, int n) { if (n < 1 || n > 64) { errno = EINVAL; return -1; } s->__bits[(n - 1) / (8 * sizeof(long))] |= 1UL << (n - 1) % (8 * sizeof(long)); return 0; }
int sigdelset(sigset_t *s, int n) { if (n < 1 || n > 64) { errno = EINVAL; return -1; } s->__bits[(n - 1) / (8 * sizeof(long))] &= ~(1UL << (n - 1) % (8 * sizeof(long))); return 0; }
int sigismember(const sigset_t *s, int n) { if (n < 1 || n > 64) { errno = EINVAL; return -1; } return (s->__bits[(n - 1) / (8 * sizeof(long))] >> (n - 1) % (8 * sizeof(long))) & 1; }
int sigisemptyset(const sigset_t *s) { return !s->__bits[0] && !s->__bits[1]; }
int sigaction(int n, const struct sigaction *act, struct sigaction *old)
{
    struct c_sigaction k, ko;
    long r;
    if (act) { k.handler = act->sa_handler; k.flags = (unsigned long)(unsigned)act->sa_flags | 0x04000000; k.restorer = c_restore_rt; memcpy(&k.mask, act->sa_mask.__bits, 8); }
    r = fail(sys4(SYS_rt_sigaction, n, act ? &k : NULL, old ? &ko : NULL, 8));
    if (r < 0) return -1;
    if (old) { memset(old, 0, sizeof *old); old->sa_handler = ko.handler; old->sa_flags = ko.flags & ~0x04000000UL; memcpy(old->sa_mask.__bits, &ko.mask, 8); }
    return 0;
}
int sigprocmask(int how, const sigset_t *set, sigset_t *old)
{
    unsigned long long o = 0;
    long r = fail(sys4(SYS_rt_sigprocmask, how, set ? set->__bits : NULL, old ? &o : NULL, 8));
    if (r < 0) return -1;
    if (old) { memset(old, 0, sizeof *old); memcpy(old->__bits, &o, 8); }
    return 0;
}
int pthread_sigmask(int how, const sigset_t *set, sigset_t *old) { return sigprocmask(how, set, old) < 0 ? errno : 0; }
int sigpending(sigset_t *s) { unsigned long long o = 0; if (fail(sys2(SYS_rt_sigpending, &o, 8)) < 0) return -1; memset(s, 0, sizeof *s); memcpy(s->__bits, &o, 8); return 0; }
int sigsuspend(const sigset_t *s) { return fail(sys2(SYS_rt_sigsuspend, s->__bits, 8)); }
int sigaltstack(const stack_t *ss, stack_t *old) { return fail(sys2(SYS_sigaltstack, ss, old)); }
int raise(int sig) { return fail(sys3(SYS_tgkill, getpid(), gettid(), sig)); }
int killpg(int pg, int sig) { return kill(-pg, sig); }
int sigignore(int n) { return signal(n, SIG_IGN) == SIG_ERR ? -1 : 0; }
sighandler_t sysv_signal(int n, sighandler_t f) { return signal(n, f); }
void psignal(int sig, const char *s) { fprintf(stderr, "%s%s%s\n", s ? s : "", s && *s ? ": " : "", strsignal(sig)); }
typedef struct { jmp_buf b; } sigjmp_buf[1];
#define sigsetjmp(env, save) setjmp((env)->b)
#define siglongjmp(env, v) longjmp((env)->b, v)
#define _setjmp(b) setjmp(b)
#define _longjmp(b, v) longjmp(b, v)
