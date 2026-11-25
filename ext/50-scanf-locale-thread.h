/* ---- scanf, locale and wide char stubs, threads that aren't, dlopen that can't [chamesle] ---- */
static int c_scan(int (*get)(void *), void (*unget)(int, void *), void *src, const char *fmt, va_list ap)
{
    int n = 0, c, count = 0;
    for (; *fmt; fmt++) {
        int sup = 0, width = 0, lng = 0, hh = 0, ok;
        if (isspace((unsigned char)*fmt)) { while (isspace(c = get(src))) ; unget(c, src); continue; }
        if (*fmt != '%') {
            c = get(src);
            if (c != (unsigned char)*fmt) { unget(c, src); return n ? n : (c == EOF ? EOF : 0); }
            count++;
            continue;
        }
        fmt++;
        if (*fmt == '%') { while (isspace(c = get(src))) ; if (c != '%') { unget(c, src); return n; } continue; }
        if (*fmt == '*') { sup = 1; fmt++; }
        while (isdigit((unsigned char)*fmt)) width = width * 10 + *fmt++ - '0';
        for (;; fmt++) {
            if (*fmt == 'l') lng++;
            else if (*fmt == 'h') hh++;
            else if (*fmt == 'z' || *fmt == 'j' || *fmt == 't' || *fmt == 'L' || *fmt == 'q') lng = 2;
            else break;
        }
        if (!width) width = 1 << 30;
        if (*fmt != 'c' && *fmt != '[' && *fmt != 'n') { while (isspace(c = get(src))) ; unget(c, src); }
        if (*fmt == 'n') { if (!sup) *va_arg(ap, int *) = count; continue; }
        {
            char tok[512];
            int len = 0;
            switch (*fmt) {
            case 'd': case 'i': case 'u': case 'x': case 'X': case 'o': case 'p': {
                int base = *fmt == 'd' || *fmt == 'u' ? 10 : *fmt == 'o' ? 8 : *fmt == 'i' ? 0 : 16;
                int neg = 0;
                unsigned long long v = 0;
                int digits = 0;
                c = get(src);
                if (c == '-' || c == '+') { neg = c == '-'; width--; c = get(src); }
                if (width > 0 && c == '0' && (base == 0 || base == 16)) {
                    int c2;
                    count++; width--;
                    c2 = get(src);
                    if ((c2 == 'x' || c2 == 'X') && width > 0) { base = 16; width--; count++; c = get(src); }
                    else { unget(c2, src); if (base == 0) base = 8; digits = 1; c = '0'; if (c2 != 'x') { c = get(src); unget(c, src); c = c2; (void)c; c = get(src); } }
                    if (digits) { c = c2; }
                }
                if (base == 0) base = 10;
                while (width > 0) {
                    int d = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : 99;
                    if (d >= base) break;
                    v = v * base + d; digits = 1; width--; count++;
                    c = get(src);
                }
                unget(c, src);
                if (!digits) return n ? n : (c == EOF ? EOF : 0);
                if (neg) v = -v;
                if (!sup) {
                    if (*fmt == 'd' || *fmt == 'i') {
                        if (lng >= 2) *va_arg(ap, long long *) = v; else if (lng == 1) *va_arg(ap, long *) = v; else if (hh == 1) *va_arg(ap, short *) = v; else if (hh > 1) *va_arg(ap, signed char *) = v; else *va_arg(ap, int *) = v;
                    } else if (*fmt == 'p') *va_arg(ap, void **) = (void *)(unsigned long)v;
                    else if (lng >= 2) *va_arg(ap, unsigned long long *) = v; else if (lng == 1) *va_arg(ap, unsigned long *) = v; else if (hh == 1) *va_arg(ap, unsigned short *) = v; else if (hh > 1) *va_arg(ap, unsigned char *) = v; else *va_arg(ap, unsigned *) = v;
                    n++;
                }
                break;
            }
            case 'f': case 'e': case 'g': case 'E': case 'G': case 'a': {
                char *end;
                double v;
                c = get(src);
                while (len < 500 && width > 0 && (isdigit(c) || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E' || c == 'x' || c == 'X' || c == 'n' || c == 'a' || c == 'i' || c == 'f' || c == 'N' || c == 'A' || c == 'I' || c == 'p' || (c >= 'b' && c <= 'd'))) {
                    tok[len++] = c; width--; c = get(src);
                }
                unget(c, src);
                tok[len] = 0;
                v = strtod(tok, &end);
                if (end == tok) return n ? n : (c == EOF ? EOF : 0);
                count += len;
                if (!sup) { if (lng) *va_arg(ap, double *) = v; else *va_arg(ap, float *) = v; n++; }
                break;
            }
            case 's': {
                char *d = sup ? NULL : va_arg(ap, char *);
                c = get(src);
                while (c != EOF && !isspace(c) && width > 0) { if (d) d[len] = c; len++; width--; c = get(src); }
                unget(c, src);
                if (!len) return n ? n : EOF;
                if (d) { d[len] = 0; n++; }
                count += len;
                break;
            }
            case 'c': {
                char *d = sup ? NULL : va_arg(ap, char *);
                if (width == 1 << 30) width = 1;
                while (width-- > 0) { c = get(src); if (c == EOF) return n ? n : EOF; if (d) d[len] = c; len++; }
                if (d) n++;
                count += len;
                break;
            }
            case '[': {
                char set[256];
                int neg = 0, i;
                char *d = sup ? NULL : va_arg(ap, char *);
                memset(set, 0, sizeof set);
                fmt++;
                if (*fmt == '^') { neg = 1; fmt++; }
                if (*fmt == ']') { set[']'] = 1; fmt++; }
                while (*fmt && *fmt != ']') {
                    if (fmt[1] == '-' && fmt[2] && fmt[2] != ']') { for (i = (unsigned char)fmt[0]; i <= (unsigned char)fmt[2]; i++) set[i] = 1; fmt += 3; }
                    else set[(unsigned char)*fmt++] = 1;
                }
                c = get(src);
                while (c != EOF && width > 0 && set[c] != neg) { if (d) d[len] = c; len++; width--; c = get(src); }
                unget(c, src);
                if (!len) return n ? n : (c == EOF ? EOF : 0);
                if (d) { d[len] = 0; n++; }
                count += len;
                break;
            }
            default: return n;
            }
            (void)ok;
        }
    }
    return n;
}
static int c_sget(void *p) { const char **s = p; int c = (unsigned char)**s; if (!c) return EOF; (*s)++; return c; }
static void c_sunget(int c, void *p) { const char **s = p; if (c != EOF) (*s)--; }
static int c_fget(void *p) { return fgetc(p); }
static void c_funget(int c, void *p) { if (c != EOF) ungetc(c, p); }
int vsscanf(const char *s, const char *fmt, va_list ap) { return c_scan(c_sget, c_sunget, &s, fmt, ap); }
int sscanf(const char *s, const char *fmt, ...) { va_list ap; int n; va_start(ap, fmt); n = vsscanf(s, fmt, ap); va_end(ap); return n; }
int vfscanf(FILE *f, const char *fmt, va_list ap) { return c_scan(c_fget, c_funget, f, fmt, ap); }
int fscanf(FILE *f, const char *fmt, ...) { va_list ap; int n; va_start(ap, fmt); n = vfscanf(f, fmt, ap); va_end(ap); return n; }
int scanf(const char *fmt, ...) { va_list ap; int n; va_start(ap, fmt); n = vfscanf(stdin, fmt, ap); va_end(ap); return n; }
int vscanf(const char *fmt, va_list ap) { return vfscanf(stdin, fmt, ap); }

/* locale: there is only the C one */
#define LC_CTYPE 0
#define LC_NUMERIC 1
#define LC_TIME 2
#define LC_COLLATE 3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL 6
struct lconv { char *decimal_point, *thousands_sep, *grouping, *int_curr_symbol, *currency_symbol, *mon_decimal_point, *mon_thousands_sep, *mon_grouping, *positive_sign, *negative_sign;
    char int_frac_digits, frac_digits, p_cs_precedes, p_sep_by_space, n_cs_precedes, n_sep_by_space, p_sign_posn, n_sign_posn; };
char *setlocale(int cat, const char *name) { (void)cat; if (!name || !*name || !strcmp(name, "C") || !strcmp(name, "POSIX") || !strcmp(name, "C.UTF-8")) return "C"; return NULL; }
struct lconv *localeconv(void)
{
    static struct lconv l = { ".", "", "", "", "", "", "", "", "", "", 127, 127, 127, 127, 127, 127, 127, 127 };
    return &l;
}
typedef void *locale_t;
locale_t newlocale(int mask, const char *name, locale_t base) { (void)mask; (void)name; (void)base; return (locale_t)1; }
void freelocale(locale_t l) { (void)l; }
locale_t uselocale(locale_t l) { (void)l; return (locale_t)1; }
#define LC_GLOBAL_LOCALE ((locale_t)-1)
#define CODESET 14
#define D_T_FMT 0x20028
#define RADIXCHAR 0x10000
char *nl_langinfo(int item) { return item == CODESET ? "ANSI_X3.4-1968" : item == RADIXCHAR ? "." : ""; }
char *gettext(const char *s) { return (char *)s; }
char *dgettext(const char *d, const char *s) { (void)d; return (char *)s; }
char *ngettext(const char *a, const char *b, unsigned long n) { return (char *)(n == 1 ? a : b); }
char *textdomain(const char *d) { return (char *)d; }
char *bindtextdomain(const char *d, const char *dir) { (void)d; return (char *)dir; }
#define _(s) (s)
int isalpha_l(int c, locale_t l) { (void)l; return isalpha(c); }
int strcoll_l(const char *a, const char *b, locale_t l) { (void)l; return strcmp(a, b); }
int strcasecmp_l(const char *a, const char *b, locale_t l) { (void)l; return strcasecmp(a, b); }

typedef int wchar_t;
typedef int wint_t;
typedef int wctype_t;
typedef struct { int c_; } mbstate_t;
#define WEOF (-1)
#define MB_LEN_MAX 4
#define MB_CUR_MAX 1
int mbtowc(wchar_t *w, const char *s, size_t n) { if (!s) return 0; if (!n) return -1; if (w) *w = (unsigned char)*s; return *s ? 1 : 0; }
int wctomb(char *s, wchar_t w) { if (!s) return 0; *s = w; return 1; }
size_t mbrtowc(wchar_t *w, const char *s, size_t n, mbstate_t *st) { (void)st; if (!s) return 0; if (!n) return (size_t)-2; if (w) *w = (unsigned char)*s; return *s ? 1 : 0; }
size_t wcrtomb(char *s, wchar_t w, mbstate_t *st) { (void)st; if (!s) return 1; *s = w; return 1; }
size_t mbstowcs(wchar_t *d, const char *s, size_t n) { size_t i = 0; while (s[i] && (!d || i < n)) { if (d) d[i] = (unsigned char)s[i]; i++; } if (d && i < n) d[i] = 0; return i; }
size_t wcstombs(char *d, const wchar_t *s, size_t n) { size_t i = 0; while (s[i] && (!d || i < n)) { if (d) d[i] = s[i]; i++; } if (d && i < n) d[i] = 0; return i; }
size_t mbrlen(const char *s, size_t n, mbstate_t *st) { return mbrtowc(NULL, s, n, st); }
int mbsinit(const mbstate_t *st) { (void)st; return 1; }
int mblen(const char *s, size_t n) { return mbtowc(NULL, s, n); }
size_t wcslen(const wchar_t *s) { size_t n = 0; while (s[n]) n++; return n; }
int wcscmp(const wchar_t *a, const wchar_t *b) { while (*a && *a == *b) { a++; b++; } return *a - *b; }
wchar_t *wcscpy(wchar_t *d, const wchar_t *s) { wchar_t *r = d; while ((*d++ = *s++)) ; return r; }
wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n) { return memcpy(d, s, n * sizeof *d); }
wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n) { size_t i; for (i = 0; i < n; i++) d[i] = c; return d; }
int iswalpha(wint_t c) { return isalpha(c); }
int iswdigit(wint_t c) { return isdigit(c); }
int iswspace(wint_t c) { return isspace(c); }
int iswupper(wint_t c) { return isupper(c); }
int iswlower(wint_t c) { return islower(c); }
int iswalnum(wint_t c) { return isalnum(c); }
int iswprint(wint_t c) { return c >= 32 && c < 127; }
int iswpunct(wint_t c) { return ispunct(c); }
int iswcntrl(wint_t c) { return iscntrl(c); }
wint_t towlower(wint_t c) { return tolower(c); }
wint_t towupper(wint_t c) { return toupper(c); }
int wcwidth(wchar_t c) { return c == 0 ? 0 : c < 32 || (c >= 127 && c < 160) ? -1 : 1; }
int wcswidth(const wchar_t *s, size_t n) { int w = 0; while (n-- && *s) { int c = wcwidth(*s++); if (c < 0) return -1; w += c; } return w; }
wint_t btowc(int c) { return c; }
int wctob(wint_t c) { return c < 256 ? c : EOF; }

/* threads: one of them. a mutex always locks, a thread is run to the end when it's made */
typedef unsigned long pthread_t;
typedef struct { int k; } pthread_mutex_t, pthread_mutexattr_t, pthread_cond_t, pthread_condattr_t, pthread_rwlock_t, pthread_attr_t, pthread_rwlockattr_t, pthread_spinlock_t_;
typedef struct { int done; } pthread_once_t;
typedef unsigned pthread_key_t;
#define PTHREAD_MUTEX_INITIALIZER { 0 }
#define PTHREAD_COND_INITIALIZER { 0 }
#define PTHREAD_RWLOCK_INITIALIZER { 0 }
#define PTHREAD_ONCE_INIT { 0 }
#define PTHREAD_MUTEX_RECURSIVE 1
#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_CREATE_DETACHED 1
#define PTHREAD_CREATE_JOINABLE 0
int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a) { (void)m; (void)a; return 0; }
int pthread_mutex_lock(pthread_mutex_t *m) { (void)m; return 0; }
int pthread_mutex_trylock(pthread_mutex_t *m) { (void)m; return 0; }
int pthread_mutex_unlock(pthread_mutex_t *m) { (void)m; return 0; }
int pthread_mutex_destroy(pthread_mutex_t *m) { (void)m; return 0; }
int pthread_mutexattr_init(pthread_mutexattr_t *a) { (void)a; return 0; }
int pthread_mutexattr_settype(pthread_mutexattr_t *a, int t) { (void)a; (void)t; return 0; }
int pthread_mutexattr_destroy(pthread_mutexattr_t *a) { (void)a; return 0; }
int pthread_cond_init(pthread_cond_t *c, const pthread_condattr_t *a) { (void)c; (void)a; return 0; }
int pthread_cond_signal(pthread_cond_t *c) { (void)c; return 0; }
int pthread_cond_broadcast(pthread_cond_t *c) { (void)c; return 0; }
int pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m) { (void)c; (void)m; return 0; }
int pthread_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *t) { (void)c; (void)m; (void)t; return ETIMEDOUT; }
int pthread_cond_destroy(pthread_cond_t *c) { (void)c; return 0; }
int pthread_rwlock_init(pthread_rwlock_t *l, const pthread_rwlockattr_t *a) { (void)l; (void)a; return 0; }
int pthread_rwlock_rdlock(pthread_rwlock_t *l) { (void)l; return 0; }
int pthread_rwlock_wrlock(pthread_rwlock_t *l) { (void)l; return 0; }
int pthread_rwlock_unlock(pthread_rwlock_t *l) { (void)l; return 0; }
int pthread_rwlock_destroy(pthread_rwlock_t *l) { (void)l; return 0; }
int pthread_once(pthread_once_t *o, void (*f)(void)) { if (!o->done) { o->done = 1; f(); } return 0; }
pthread_t pthread_self(void) { return 1; }
int pthread_equal(pthread_t a, pthread_t b) { return a == b; }
static void *c_tls[64];
static int c_tls_n;
int pthread_key_create(pthread_key_t *k, void (*d)(void *)) { (void)d; if (c_tls_n == 64) return EAGAIN; *k = c_tls_n++; return 0; }
int pthread_key_delete(pthread_key_t k) { (void)k; return 0; }
void *pthread_getspecific(pthread_key_t k) { return k < 64 ? c_tls[k] : NULL; }
int pthread_setspecific(pthread_key_t k, const void *v) { if (k >= 64) return EINVAL; c_tls[k] = (void *)v; return 0; }
int pthread_attr_init(pthread_attr_t *a) { (void)a; return 0; }
int pthread_attr_destroy(pthread_attr_t *a) { (void)a; return 0; }
int pthread_attr_setdetachstate(pthread_attr_t *a, int s) { (void)a; (void)s; return 0; }
int pthread_attr_setstacksize(pthread_attr_t *a, size_t s) { (void)a; (void)s; return 0; }
int pthread_create(pthread_t *t, const pthread_attr_t *a, void *(*fn)(void *), void *arg) { (void)a; (void)fn; (void)arg; (void)t; return EAGAIN; }
int pthread_join(pthread_t t, void **r) { (void)t; (void)r; return ESRCH; }
int pthread_detach(pthread_t t) { (void)t; return 0; }
int pthread_atfork(void (*a)(void), void (*b)(void), void (*c)(void)) { (void)a; (void)b; (void)c; return 0; }
int pthread_setcancelstate(int s, int *o) { (void)s; if (o) *o = 0; return 0; }
int pthread_kill(pthread_t t, int sig) { (void)t; return raise(sig); }
int sched_yield(void) { return fail(sys0(SYS_sched_yield)); }

/* dlopen: static binaries have nothing to open */
#define RTLD_LAZY 1
#define RTLD_NOW 2
#define RTLD_GLOBAL 0x100
#define RTLD_LOCAL 0
#define RTLD_DEFAULT ((void *)0)
void *dlopen(const char *f, int flags) { (void)f; (void)flags; return NULL; }
void *dlsym(void *h, const char *s) { (void)h; (void)s; return NULL; }
int dlclose(void *h) { (void)h; return 0; }
char *dlerror(void) { return "dynamic loading is not available"; }

void endpwent(void) { }
void endgrent(void) { }
