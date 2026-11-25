/* ---- string, ctype, stdlib, time: what real programs reach for [chamesle] ---- */

/* strings */
char *strsep(char **sp, const char *delim)
{
    char *s = *sp, *e;
    if (!s) return NULL;
    e = s + strcspn(s, delim);
    if (*e) *e++ = 0; else e = NULL;
    *sp = e;
    return s;
}
char *strtok_r(char *s, const char *delim, char **save)
{
    char *e;
    if (!s) s = *save;
    if (!s) return NULL;
    s += strspn(s, delim);
    if (!*s) { *save = NULL; return NULL; }
    e = s + strcspn(s, delim);
    if (*e) *e++ = 0; else e = NULL;
    *save = e;
    return s;
}
char *strtok(char *s, const char *delim) { static char *save; return strtok_r(s, delim, &save); }
size_t strnlen(const char *s, size_t n) { size_t i = 0; while (i < n && s[i]) i++; return i; }
size_t strlcpy(char *d, const char *s, size_t n)
{
    size_t l = strlen(s);
    if (n) { size_t c = l < n - 1 ? l : n - 1; memcpy(d, s, c); d[c] = 0; }
    return l;
}
size_t strlcat(char *d, const char *s, size_t n)
{
    size_t dl = strnlen(d, n), sl = strlen(s);
    if (dl == n) return n + sl;
    strlcpy(d + dl, s, n - dl);
    return dl + sl;
}
char *strncat(char *d, const char *s, size_t n) { char *r = d; d += strlen(d); while (n-- && *s) *d++ = *s++; *d = 0; return r; }
char *stpncpy(char *d, const char *s, size_t n) { size_t l = strnlen(s, n); memcpy(d, s, l); memset(d + l, 0, n - l); return d + l; }
char *strchrnul(const char *s, int c) { while (*s && *s != (char)c) s++; return (char *)s; }
void *mempcpy(void *d, const void *s, size_t n) { return (char *)memcpy(d, s, n) + n; }
void *memccpy(void *d, const void *s, int c, size_t n)
{
    unsigned char *dd = d; const unsigned char *ss = s;
    while (n--) { if ((*dd++ = *ss++) == (unsigned char)c) return dd; }
    return NULL;
}
void *memmem(const void *h, size_t hl, const void *n, size_t nl)
{
    const char *p = h;
    if (!nl) return (void *)h;
    while (hl >= nl) {
        const char *q = memchr(p, *(const char *)n, hl - nl + 1);
        if (!q) return NULL;
        if (!memcmp(q, n, nl)) return (void *)q;
        hl -= q + 1 - p; p = q + 1;
    }
    return NULL;
}
char *strcasestr(const char *h, const char *n)
{
    size_t nl = strlen(n);
    for (; *h; h++) if (!strncasecmp(h, n, nl)) return (char *)h;
    return nl ? NULL : (char *)h;
}
size_t strxfrm(char *d, const char *s, size_t n) { size_t l = strlen(s); if (n) strlcpy(d, s, n); return l; }
int strverscmp(const char *a, const char *b)
{
    while (*a && *b) {
        if (isdigit((unsigned char)*a) && isdigit((unsigned char)*b)) {
            const char *ea = a, *eb = b;
            while (*a == '0') a++;
            while (*b == '0') b++;
            ea = a; eb = b;
            while (isdigit((unsigned char)*ea)) ea++;
            while (isdigit((unsigned char)*eb)) eb++;
            if (ea - a != eb - b) return ea - a < eb - b ? -1 : 1;
            while (a < ea) { if (*a != *b) return (unsigned char)*a - (unsigned char)*b; a++; b++; }
        } else {
            if (*a != *b) return (unsigned char)*a - (unsigned char)*b;
            a++; b++;
        }
    }
    return (unsigned char)*a - (unsigned char)*b;
}
int ffs(int i) { return __builtin_ffs(i); }
int ffsl(long i) { return __builtin_ffsl(i); }
int ffsll(long long i) { return __builtin_ffsll(i); }
void bcopy(const void *s, void *d, size_t n) { memmove(d, s, n); }
void bzero(void *d, size_t n) { memset(d, 0, n); }
int bcmp(const void *a, const void *b, size_t n) { return memcmp(a, b, n); }
char *index(const char *s, int c) { return strchr(s, c); }
char *rindex(const char *s, int c) { return strrchr(s, c); }
void explicit_bzero(void *d, size_t n) { volatile char *p = d; while (n--) *p++ = 0; }
static const char *const c_signames[] = { "Unknown signal 0", "Hangup", "Interrupt", "Quit", "Illegal instruction", "Trace/breakpoint trap", "Aborted",
    "Bus error", "Floating point exception", "Killed", "User defined signal 1", "Segmentation fault", "User defined signal 2", "Broken pipe",
    "Alarm clock", "Terminated", "Stack fault", "Child exited", "Continued", "Stopped (signal)", "Stopped", "Stopped (tty input)",
    "Stopped (tty output)", "Urgent I/O condition", "CPU time limit exceeded", "File size limit exceeded", "Virtual timer expired",
    "Profiling timer expired", "Window changed", "I/O possible", "Power failure", "Bad system call" };
char *strsignal(int s) { return (char *)(s > 0 && s < 32 ? c_signames[s] : "Unknown signal"); }

/* ctype */
int isblank(int c) { return c == ' ' || c == '\t'; }
int iscntrl(int c) { return (c >= 0 && c < 32) || c == 127; }
int isgraph(int c) { return c > 32 && c < 127; }
int isascii(int c) { return c >= 0 && c < 128; }
int toascii(int c) { return c & 127; }

/* numbers */
typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;
typedef struct { long long quot, rem; } lldiv_t;
div_t div(int a, int b) { div_t r = { a / b, a % b }; return r; }
ldiv_t ldiv(long a, long b) { ldiv_t r = { a / b, a % b }; return r; }
lldiv_t lldiv(long long a, long long b) { lldiv_t r = { a / b, a % b }; return r; }
long long llabs(long long a) { return a < 0 ? -a : a; }
intmax_t imaxabs(intmax_t a) { return a < 0 ? -a : a; }
intmax_t strtoimax(const char *s, char **e, int base) { return strtoll(s, e, base); }
uintmax_t strtoumax(const char *s, char **e, int base) { return strtoull(s, e, base); }
#define RAND_MAX 2147483647
static unsigned long long c_rand_state = 1;
void srandom(unsigned seed) { c_rand_state = seed ? seed : 1; }
long random(void)
{
    c_rand_state = c_rand_state * 6364136223846793005ULL + 1442695040888963407ULL;
    return (long)(c_rand_state >> 33);
}
void srand(unsigned seed) { srandom(seed); }
int rand(void) { return (int)random(); }
int rand_r(unsigned *seed) { unsigned long long s = *seed * 6364136223846793005ULL + 1442695040888963407ULL; *seed = s >> 32; return (int)((s >> 33) & RAND_MAX); }
void srand48(long seed) { srandom(seed); }
long lrand48(void) { return random(); }
long mrand48(void) { return (long)(int)random() * 2; }
double drand48(void) { return (random() >> 7) / 16777216.0 + (random() % 128) / (16777216.0 * 128); }
unsigned arc4random(void) { unsigned v; if (getrandom(&v, sizeof v, 0) != sizeof v) v = random(); return v; }
void arc4random_buf(void *b, size_t n) { if (getrandom(b, n, 0) != (ssize_t)n) { unsigned char *p = b; while (n--) *p++ = random(); } }
unsigned arc4random_uniform(unsigned n)
{
    unsigned lim, v;
    if (n < 2) return 0;
    lim = -n % n;
    do v = arc4random(); while (v < lim);
    return v % n;
}

void *bsearch(const void *key, const void *base, size_t n, size_t size, int (*cmp)(const void *, const void *))
{
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        const char *p = (const char *)base + mid * size;
        int c = cmp(key, p);
        if (!c) return (void *)p;
        if (c < 0) hi = mid; else lo = mid + 1;
    }
    return NULL;
}
static void c_sort_r(char *base, size_t n, size_t size, int (*cmp)(const void *, const void *, void *), void *arg, char *tmp)
{
    size_t mid, i, j, k;
    if (n < 2) return;
    mid = n / 2;
    c_sort_r(base, mid, size, cmp, arg, tmp);
    c_sort_r(base + mid * size, n - mid, size, cmp, arg, tmp);
    i = 0; j = mid; k = 0;
    while (i < mid && j < n) {
        if (cmp(base + j * size, base + i * size, arg) < 0) memcpy(tmp + k++ * size, base + j++ * size, size);
        else memcpy(tmp + k++ * size, base + i++ * size, size);
    }
    while (i < mid) memcpy(tmp + k++ * size, base + i++ * size, size);
    while (j < n) memcpy(tmp + k++ * size, base + j++ * size, size);
    memcpy(base, tmp, n * size);
}
void qsort_r(void *base, size_t n, size_t size, int (*cmp)(const void *, const void *, void *), void *arg)
{
    char *tmp;
    if (n < 2 || !size) return;
    tmp = malloc(n * size);
    if (!tmp) return;
    c_sort_r(base, n, size, cmp, arg, tmp);
    free(tmp);
}

/* atexit: exit runs them backwards, then flushes everything */
static void (*c_atexit_fns[64])(void);
static int c_atexit_n;
int atexit(void (*f)(void)) { if (c_atexit_n == 64) return -1; c_atexit_fns[c_atexit_n++] = f; return 0; }
void c_run_exit(void) { while (c_atexit_n) c_atexit_fns[--c_atexit_n](); fflush(NULL); }
__attribute__((noreturn)) void _Exit(int code) { _exit(code); }
int at_quick_exit(void (*f)(void)) { return atexit(f); }
__attribute__((noreturn)) void quick_exit(int code) { exit(code); }

/* memory */
int posix_memalign(void **out, size_t align, size_t size)
{
    char *p;
    if (align < sizeof(void *) || (align & (align - 1))) return EINVAL;
    p = malloc(size + align + sizeof(void *));
    if (!p) return ENOMEM;
    {
        char *a = (char *)(((unsigned long)p + sizeof(void *) + align - 1) & ~(align - 1));
        ((void **)a)[-1] = p;
        *out = a;
    }
    return 0;
}
void *aligned_alloc(size_t align, size_t size) { void *p; return posix_memalign(&p, align, size) ? NULL : p; }
void *memalign(size_t align, size_t size) { return aligned_alloc(align, size); }
void *valloc(size_t size) { return aligned_alloc(4096, size); }
void *reallocarray(void *p, size_t n, size_t size) { if (size && n > (size_t)-1 / size) { errno = ENOMEM; return NULL; } return realloc(p, n * size); }
#define alloca(n) __builtin_alloca(n)

/* temporary files */
static void c_fill_x(char *x)
{
    static const char a[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    unsigned char r[8];
    int i;
    arc4random_buf(r, sizeof r);
    for (i = 0; i < 6; i++) x[i] = a[r[i] % 62];
}
int mkostemps(char *tpl, int suffix, int flags)
{
    size_t l = strlen(tpl);
    int tries;
    if (l < 6 + (size_t)suffix || strncmp(tpl + l - 6 - suffix, "XXXXXX", 6)) { errno = EINVAL; return -1; }
    for (tries = 0; tries < 100; tries++) {
        int fd;
        c_fill_x(tpl + l - 6 - suffix);
        fd = open(tpl, O_RDWR | O_CREAT | O_EXCL | flags, 0600);
        if (fd >= 0 || errno != EEXIST) return fd;
    }
    return -1;
}
int mkstemp(char *tpl) { return mkostemps(tpl, 0, 0); }
int mkstemps(char *tpl, int suffix) { return mkostemps(tpl, suffix, 0); }
int mkostemp(char *tpl, int flags) { return mkostemps(tpl, 0, flags); }
char *mkdtemp(char *tpl)
{
    size_t l = strlen(tpl);
    int tries;
    if (l < 6 || strcmp(tpl + l - 6, "XXXXXX")) { errno = EINVAL; return NULL; }
    for (tries = 0; tries < 100; tries++) {
        c_fill_x(tpl + l - 6);
        if (mkdir(tpl, 0700) == 0) return tpl;
        if (errno != EEXIST) return NULL;
    }
    return NULL;
}
char *mktemp(char *tpl)
{
    size_t l = strlen(tpl);
    struct stat st;
    if (l < 6 || strcmp(tpl + l - 6, "XXXXXX")) { errno = EINVAL; tpl[0] = 0; return tpl; }
    do c_fill_x(tpl + l - 6); while (lstat(tpl, &st) == 0);
    return tpl;
}
char *tmpnam(char *s)
{
    static char buf[L_tmpnam];
    char *d = s ? s : buf;
    strcpy(d, "/tmp/tmpXXXXXX");
    mktemp(d);
    return d;
}
char *tempnam(const char *dir, const char *prefix)
{
    char *d = malloc(strlen(dir ? dir : "/tmp") + (prefix ? strlen(prefix) : 0) + 16);
    if (!d) return NULL;
    sprintf(d, "%s/%sXXXXXX", dir ? dir : "/tmp", prefix ? prefix : "");
    mktemp(d);
    return d;
}
FILE *tmpfile(void)
{
    char name[] = "/tmp/tmpfileXXXXXX";
    int fd = mkstemp(name);
    FILE *f;
    if (fd < 0) return NULL;
    unlink(name);
    f = fdopen(fd, "w+");
    if (!f) close(fd);
    return f;
}

/* printf pieces built on the core ones */
int vasprintf(char **out, const char *fmt, va_list ap)
{
    va_list ap2;
    int n;
    va_copy(ap2, ap);
    n = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);
    if (n < 0) return -1;
    *out = malloc(n + 1);
    if (!*out) return -1;
    return vsnprintf(*out, n + 1, fmt, ap);
}
int asprintf(char **out, const char *fmt, ...) { va_list ap; int n; va_start(ap, fmt); n = vasprintf(out, fmt, ap); va_end(ap); return n; }
int vdprintf(int fd, const char *fmt, va_list ap)
{
    char *s;
    int n = vasprintf(&s, fmt, ap);
    if (n < 0) return -1;
    if (write(fd, s, n) < 0) n = -1;
    free(s);
    return n;
}
int dprintf(int fd, const char *fmt, ...) { va_list ap; int n; va_start(ap, fmt); n = vdprintf(fd, fmt, ap); va_end(ap); return n; }
ssize_t getdelim(char **lp, size_t *cap, int delim, FILE *f)
{
    size_t n = 0;
    int c;
    if (!*lp || !*cap) { *cap = 128; *lp = malloc(*cap); if (!*lp) return -1; }
    while ((c = fgetc(f)) != EOF) {
        if (n + 2 > *cap) { char *p = realloc(*lp, *cap * 2); if (!p) return -1; *lp = p; *cap *= 2; }
        (*lp)[n++] = c;
        if (c == delim) break;
    }
    if (!n) return -1;
    (*lp)[n] = 0;
    return n;
}
int fputs_unlocked(const char *s, FILE *f) { return fputs(s, f); }

/* time */
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
#define CLOCK_MONOTONIC_RAW 4
#define CLOCK_REALTIME_COARSE 5
#define CLOCK_MONOTONIC_COARSE 6
#define CLOCK_BOOTTIME 7
#define TIMER_ABSTIME 1
long timezone;
int daylight;
char *tzname[2] = { "UTC", "UTC" };
void tzset(void) { }
struct tm *gmtime_r(const time_t *t, struct tm *out) { *out = *gmtime(t); return out; }
struct tm *localtime_r(const time_t *t, struct tm *out) { *out = *localtime(t); return out; }
double difftime(time_t a, time_t b) { return (double)(a - b); }
char *asctime_r(const struct tm *t, char *buf)
{
    static const char wd[] = "SunMonTueWedThuFriSat", mo[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sprintf(buf, "%.3s %.3s %2d %02d:%02d:%02d %d\n", wd + 3 * (t->tm_wday % 7), mo + 3 * (t->tm_mon % 12), t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, t->tm_year + 1900);
    return buf;
}
char *asctime(const struct tm *t) { static char buf[32]; return asctime_r(t, buf); }
char *ctime_r(const time_t *t, char *buf) { return asctime_r(localtime(t), buf); }
char *ctime(const time_t *t) { static char buf[32]; return ctime_r(t, buf); }
int clock_settime(int clk, const struct timespec *t) { return fail(sys2(227, clk, t)); }
int settimeofday(const struct timeval *tv, const void *tz) { return fail(sys2(164, tv, tz)); }
int timespec_get(struct timespec *t, int base) { if (base != 1) return 0; return clock_gettime(CLOCK_REALTIME, t) < 0 ? 0 : base; }
struct itimerval { struct timeval it_interval, it_value; };
#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2
int setitimer(int which, const struct itimerval *nw, struct itimerval *old) { return fail(sys3(SYS_setitimer, which, nw, old)); }
int getitimer(int which, struct itimerval *cur) { return fail(sys2(SYS_getitimer, which, cur)); }
#define timeradd(a, b, r) do { (r)->tv_sec = (a)->tv_sec + (b)->tv_sec; (r)->tv_usec = (a)->tv_usec + (b)->tv_usec; if ((r)->tv_usec >= 1000000) { (r)->tv_sec++; (r)->tv_usec -= 1000000; } } while (0)
#define timersub(a, b, r) do { (r)->tv_sec = (a)->tv_sec - (b)->tv_sec; (r)->tv_usec = (a)->tv_usec - (b)->tv_usec; if ((r)->tv_usec < 0) { (r)->tv_sec--; (r)->tv_usec += 1000000; } } while (0)
#define timercmp(a, b, op) (((a)->tv_sec == (b)->tv_sec) ? ((a)->tv_usec op (b)->tv_usec) : ((a)->tv_sec op (b)->tv_sec))
#define timerclear(t) ((t)->tv_sec = (t)->tv_usec = 0)
#define timerisset(t) ((t)->tv_sec || (t)->tv_usec)

char *strptime(const char *s, const char *fmt, struct tm *tm)
{
    static const char *const mon[] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
    static const char *const day[] = { "sun", "mon", "tue", "wed", "thu", "fri", "sat" };
    for (; *fmt; fmt++) {
        char *e;
        long v;
        int i;
        if (isspace((unsigned char)*fmt)) { while (isspace((unsigned char)*s)) s++; continue; }
        if (*fmt != '%') { if (*s++ != *fmt) return NULL; continue; }
        fmt++;
        if (*fmt == 'E' || *fmt == 'O') fmt++;
        switch (*fmt) {
        case 'Y': v = strtol(s, &e, 10); if (e == s) return NULL; tm->tm_year = v - 1900; s = e; break;
        case 'y': v = strtol(s, &e, 10); if (e == s) return NULL; tm->tm_year = v < 69 ? v + 100 : v; s = e; break;
        case 'm': v = strtol(s, &e, 10); if (e == s || v < 1 || v > 12) return NULL; tm->tm_mon = v - 1; s = e; break;
        case 'd': case 'e': while (*s == ' ') s++; v = strtol(s, &e, 10); if (e == s || v < 1 || v > 31) return NULL; tm->tm_mday = v; s = e; break;
        case 'H': v = strtol(s, &e, 10); if (e == s || v > 23) return NULL; tm->tm_hour = v; s = e; break;
        case 'M': v = strtol(s, &e, 10); if (e == s || v > 59) return NULL; tm->tm_min = v; s = e; break;
        case 'S': v = strtol(s, &e, 10); if (e == s || v > 61) return NULL; tm->tm_sec = v; s = e; break;
        case 'j': v = strtol(s, &e, 10); if (e == s) return NULL; tm->tm_yday = v - 1; s = e; break;
        case 'b': case 'B': case 'h':
            for (i = 0; i < 12; i++) if (!strncasecmp(s, mon[i], 3)) break;
            if (i == 12) return NULL;
            tm->tm_mon = i; s += 3;
            while (isalpha((unsigned char)*s)) s++;
            break;
        case 'a': case 'A':
            for (i = 0; i < 7; i++) if (!strncasecmp(s, day[i], 3)) break;
            if (i == 7) return NULL;
            tm->tm_wday = i; s += 3;
            while (isalpha((unsigned char)*s)) s++;
            break;
        case 'T': s = strptime(s, "%H:%M:%S", tm); if (!s) return NULL; break;
        case 'R': s = strptime(s, "%H:%M", tm); if (!s) return NULL; break;
        case 'F': s = strptime(s, "%Y-%m-%d", tm); if (!s) return NULL; break;
        case 'D': s = strptime(s, "%m/%d/%y", tm); if (!s) return NULL; break;
        case 'n': case 't': while (isspace((unsigned char)*s)) s++; break;
        case 'z': if (*s == 'Z') { s++; break; } if (*s != '+' && *s != '-') return NULL; s++; while (isdigit((unsigned char)*s) || *s == ':') s++; break;
        case 'Z': while (isalpha((unsigned char)*s)) s++; break;
        case '%': if (*s++ != '%') return NULL; break;
        default: return NULL;
        }
    }
    return (char *)s;
}
