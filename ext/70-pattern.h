/* ---- fnmatch, glob, ftw, regex, path names [chamesle] ---- */
#define FNM_NOMATCH 1
#define FNM_PATHNAME 1
#define FNM_NOESCAPE 2
#define FNM_PERIOD 4
#define FNM_CASEFOLD 16
static int c_fn_class(const char *name, int c)
{
    if (!strncmp(name, "alpha:", 6)) return isalpha(c);
    if (!strncmp(name, "digit:", 6)) return isdigit(c);
    if (!strncmp(name, "alnum:", 6)) return isalnum(c);
    if (!strncmp(name, "upper:", 6)) return isupper(c);
    if (!strncmp(name, "lower:", 6)) return islower(c);
    if (!strncmp(name, "space:", 6)) return isspace(c);
    if (!strncmp(name, "blank:", 6)) return c == ' ' || c == '\t';
    if (!strncmp(name, "punct:", 6)) return ispunct(c);
    if (!strncmp(name, "print:", 6)) return c >= 32 && c < 127;
    if (!strncmp(name, "graph:", 6)) return c > 32 && c < 127;
    if (!strncmp(name, "cntrl:", 6)) return iscntrl(c);
    if (!strncmp(name, "xdigit:", 7)) return isxdigit(c);
    return 0;
}
int fnmatch(const char *p, const char *s, int flags)
{
    const char *start = s;
    for (; *p; p++, s++) {
        int fold = flags & FNM_CASEFOLD;
        if (*p == '*') {
            while (*p == '*') p++;
            if ((flags & FNM_PERIOD) && *s == '.' && (s == start || ((flags & FNM_PATHNAME) && s[-1] == '/'))) return FNM_NOMATCH;
            if (!*p) return (flags & FNM_PATHNAME) && strchr(s, '/') ? FNM_NOMATCH : 0;
            for (;; s++) {
                if (!fnmatch(p, s, flags & ~FNM_PERIOD)) return 0;
                if (!*s || ((flags & FNM_PATHNAME) && *s == '/')) return FNM_NOMATCH;
            }
        }
        if (!*s) return FNM_NOMATCH;
        if ((flags & FNM_PERIOD) && *s == '.' && *p != '.' && (s == start || ((flags & FNM_PATHNAME) && s[-1] == '/'))) return FNM_NOMATCH;
        if (*p == '?') { if ((flags & FNM_PATHNAME) && *s == '/') return FNM_NOMATCH; continue; }
        if (*p == '[') {
            const char *q = p + 1;
            int neg = 0, ok = 0, c = (unsigned char)*s, first = 1;
            if (*q == '!' || *q == '^') { neg = 1; q++; }
            if ((flags & FNM_PATHNAME) && c == '/') return FNM_NOMATCH;
            while (*q && (*q != ']' || first)) {
                first = 0;
                if (q[0] == '[' && q[1] == ':') { const char *e = strstr(q, ":]"); if (e) { if (c_fn_class(q + 2, c)) ok = 1; q = e + 2; continue; } }
                if (*q == '\\' && !(flags & FNM_NOESCAPE) && q[1]) q++;
                if (q[1] == '-' && q[2] && q[2] != ']') {
                    int lo = (unsigned char)q[0], hi = (unsigned char)q[2];
                    if (fold) { if (tolower(c) >= tolower(lo) && tolower(c) <= tolower(hi)) ok = 1; }
                    if (c >= lo && c <= hi) ok = 1;
                    q += 3;
                } else { if (fold ? tolower(c) == tolower((unsigned char)*q) : c == (unsigned char)*q) ok = 1; q++; }
            }
            if (!*q) { if (*s != '[') return FNM_NOMATCH; continue; }
            if (ok == neg) return FNM_NOMATCH;
            p = q;
            continue;
        }
        if (*p == '\\' && !(flags & FNM_NOESCAPE) && p[1]) p++;
        if (fold ? tolower((unsigned char)*p) != tolower((unsigned char)*s) : *p != *s) return FNM_NOMATCH;
    }
    return *s ? FNM_NOMATCH : 0;
}

#define GLOB_ERR 1
#define GLOB_MARK 2
#define GLOB_NOSORT 4
#define GLOB_DOOFFS 8
#define GLOB_NOCHECK 16
#define GLOB_APPEND 32
#define GLOB_NOESCAPE 64
#define GLOB_PERIOD 128
#define GLOB_BRACE 1024
#define GLOB_TILDE 4096
#define GLOB_NOSPACE 1
#define GLOB_ABORTED 2
#define GLOB_NOMATCH 3
typedef struct { size_t gl_pathc; char **gl_pathv; size_t gl_offs; } glob_t;
static int c_glob_add(glob_t *g, size_t *cap, const char *path)
{
    if (g->gl_pathc + g->gl_offs + 2 > *cap) {
        char **n;
        *cap = *cap * 2 + 16;
        n = realloc(g->gl_pathv, *cap * sizeof *n);
        if (!n) return -1;
        g->gl_pathv = n;
    }
    g->gl_pathv[g->gl_offs + g->gl_pathc] = strdup(path);
    if (!g->gl_pathv[g->gl_offs + g->gl_pathc]) return -1;
    g->gl_pathv[g->gl_offs + ++g->gl_pathc] = NULL;
    return 0;
}
static int c_glob_walk(const char *dir, const char *pat, int flags, glob_t *g, size_t *cap)
{
    char seg[512];
    const char *rest;
    size_t l;
    int magic;
    DIR *d;
    struct dirent *e;
    char **names = NULL;
    size_t nn = 0, i;
    while (*pat == '/') pat++;
    if (!*pat) return c_glob_add(g, cap, dir);
    l = strcspn(pat, "/");
    if (l >= sizeof seg) return 0;
    memcpy(seg, pat, l); seg[l] = 0;
    rest = pat + l;
    magic = strpbrk(seg, "*?[") != NULL;
    if (!magic) {
        char full[4096];
        struct stat st;
        char *u = seg;
        if (!(flags & GLOB_NOESCAPE)) { char *r = seg, *w = seg; while (*r) { if (*r == '\\' && r[1]) r++; *w++ = *r++; } *w = 0; }
        snprintf(full, sizeof full, "%s%s%s", dir, !*dir || dir[strlen(dir) - 1] == '/' ? "" : "/", u);
        if (lstat(full, &st) < 0) return 0;
        if (*rest && !S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode)) return 0;
        return c_glob_walk(full, rest, flags, g, cap);
    }
    d = opendir(*dir ? dir : ".");
    if (!d) return (flags & GLOB_ERR) ? -2 : 0;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.' && seg[0] != '.' && !(flags & GLOB_PERIOD)) continue;
        if (fnmatch(seg, e->d_name, (flags & GLOB_NOESCAPE ? FNM_NOESCAPE : 0) | FNM_PERIOD * !(flags & GLOB_PERIOD))) continue;
        names = realloc(names, (nn + 1) * sizeof *names);
        names[nn++] = strdup(e->d_name);
    }
    closedir(d);
    for (i = 0; i < nn; i++) {
        char full[4096];
        struct stat st;
        snprintf(full, sizeof full, "%s%s%s", dir, !*dir || dir[strlen(dir) - 1] == '/' ? "" : "/", names[i]);
        if (*rest && (stat(full, &st) < 0 || !S_ISDIR(st.st_mode))) continue;
        if (c_glob_walk(full, rest, flags, g, cap) < 0) return -1;
    }
    for (i = 0; i < nn; i++) free(names[i]);
    free(names);
    return 0;
}
static int c_cmp_str(const void *a, const void *b) { return strcmp(*(char *const *)a, *(char *const *)b); }
int glob(const char *pat, int flags, int (*err)(const char *, int), glob_t *g)
{
    size_t cap = 0, start;
    int r;
    (void)err;
    if (!(flags & GLOB_APPEND)) { g->gl_pathc = 0; g->gl_pathv = NULL; if (!(flags & GLOB_DOOFFS)) g->gl_offs = 0; }
    else cap = g->gl_pathc + g->gl_offs + 2;
    if (!g->gl_pathv) { g->gl_pathv = calloc(g->gl_offs + 16, sizeof(char *)); cap = g->gl_offs + 16; }
    start = g->gl_pathc;
    if (flags & GLOB_BRACE) {
        const char *o = strchr(pat, '{');
        if (o) {
            const char *p = o + 1, *c = NULL;
            int depth = 1;
            for (; *p; p++) { if (*p == '{') depth++; else if (*p == '}' && !--depth) { c = p; break; } }
            if (c) {
                const char *a = o + 1, *q;
                int any = 0;
                depth = 0;
                for (q = a; q <= c; q++) {
                    if (*q == '{') depth++;
                    else if ((*q == ',' && !depth) || q == c) {
                        char *np = malloc(strlen(pat) + (q - a) + 1);
                        memcpy(np, pat, o - pat);
                        memcpy(np + (o - pat), a, q - a);
                        strcpy(np + (o - pat) + (q - a), c + 1);
                        r = glob(np, (flags & ~GLOB_NOCHECK) | GLOB_APPEND, err, g);
                        free(np);
                        if (r && r != GLOB_NOMATCH) return r;
                        if (!r) any = 1;
                        a = q + 1;
                    } else if (*q == '}') depth--;
                }
                if (!any && (flags & GLOB_NOCHECK)) { c_glob_add(g, &cap, pat); return 0; }
                return any ? 0 : GLOB_NOMATCH;
            }
        }
    }
    r = pat[0] == '/' ? c_glob_walk("/", pat, flags, g, &cap) : c_glob_walk("", pat, flags, g, &cap);
    if (r == -2) return GLOB_ABORTED;
    if (r < 0) return GLOB_NOSPACE;
    if (g->gl_pathc == start) {
        if (flags & GLOB_NOCHECK) { c_glob_add(g, &cap, pat); return 0; }
        return GLOB_NOMATCH;
    }
    if (!(flags & GLOB_NOSORT)) qsort(g->gl_pathv + g->gl_offs + start, g->gl_pathc - start, sizeof(char *), c_cmp_str);
    if (flags & GLOB_MARK) {
        size_t i;
        for (i = start; i < g->gl_pathc; i++) {
            struct stat st;
            char *s = g->gl_pathv[g->gl_offs + i];
            if (stat(s, &st) == 0 && S_ISDIR(st.st_mode) && s[strlen(s) - 1] != '/') { char *n = malloc(strlen(s) + 2); strcpy(n, s); strcat(n, "/"); free(s); g->gl_pathv[g->gl_offs + i] = n; }
        }
    }
    return 0;
}
void globfree(glob_t *g) { size_t i; for (i = 0; i < g->gl_pathc; i++) free(g->gl_pathv[g->gl_offs + i]); free(g->gl_pathv); g->gl_pathv = NULL; g->gl_pathc = 0; }

char *basename(char *path)
{
    char *s;
    size_t l;
    if (!path || !*path) return ".";
    l = strlen(path);
    while (l > 1 && path[l - 1] == '/') path[--l] = 0;
    if (l == 1 && *path == '/') return path;
    s = strrchr(path, '/');
    return s ? s + 1 : path;
}
char *dirname(char *path)
{
    size_t l;
    if (!path || !*path) return ".";
    l = strlen(path);
    while (l > 1 && path[l - 1] == '/') l--;
    while (l > 0 && path[l - 1] != '/') l--;
    if (!l) return ".";
    while (l > 1 && path[l - 1] == '/') l--;
    path[l] = 0;
    return path;
}

#define FTW_F 0
#define FTW_D 1
#define FTW_DNR 2
#define FTW_NS 3
#define FTW_SL 4
#define FTW_DP 5
#define FTW_SLN 6
#define FTW_PHYS 1
#define FTW_MOUNT 2
#define FTW_CHDIR 4
#define FTW_DEPTH 8
struct FTW { int base, level; };
static int c_nftw_rec(char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int depth, int flags, int level, int maxfd)
{
    struct stat st;
    struct FTW ftw;
    int type, r;
    size_t l = strlen(path);
    char *b = strrchr(path, '/');
    ftw.base = b ? b - path + 1 : 0; ftw.level = level;
    if ((flags & FTW_PHYS ? lstat(path, &st) : stat(path, &st)) < 0) {
        if (!(flags & FTW_PHYS) && lstat(path, &st) == 0) return fn(path, &st, FTW_SLN, &ftw);
        return fn(path, &st, FTW_NS, &ftw);
    }
    type = S_ISDIR(st.st_mode) ? FTW_D : S_ISLNK(st.st_mode) ? FTW_SL : FTW_F;
    if (type != FTW_D) return fn(path, &st, type, &ftw);
    if (!(flags & FTW_DEPTH) && (r = fn(path, &st, FTW_D, &ftw))) return r;
    {
        DIR *d = opendir(path);
        struct dirent *e;
        if (!d) { if (flags & FTW_DEPTH) return fn(path, &st, FTW_DNR, &ftw); return fn(path, &st, FTW_DNR, &ftw); }
        while ((e = readdir(d))) {
            char *child;
            if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
            child = malloc(l + strlen(e->d_name) + 2);
            sprintf(child, "%s%s%s", path, l && path[l - 1] == '/' ? "" : "/", e->d_name);
            r = c_nftw_rec(child, fn, depth, flags, level + 1, maxfd);
            free(child);
            if (r) { closedir(d); return r; }
        }
        closedir(d);
    }
    if (flags & FTW_DEPTH) return fn(path, &st, FTW_DP, &ftw);
    return 0;
}
int nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int fd, int flags)
{
    char *p = strdup(path);
    int r = c_nftw_rec(p, fn, fd, flags, 0, fd);
    free(p);
    return r;
}
static int (*c_ftw_fn)(const char *, const struct stat *, int);
static int c_ftw_adapt(const char *p, const struct stat *s, int t, struct FTW *f) { (void)f; return c_ftw_fn(p, s, t); }
int ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int fd) { c_ftw_fn = fn; return nftw(path, c_ftw_adapt, fd, 0); }

char *get_current_dir_name(void) { return getcwd(NULL, 0); }
char *canonicalize_file_name(const char *p) { return realpath(p, NULL); }

/* regular expressions: backtracking matcher over a parsed tree. basic and extended, with the usual GNU-ish extras (\w \s \b \< \>) */
#define REG_EXTENDED 1
#define REG_ICASE 2
#define REG_NOSUB 4
#define REG_NEWLINE 8
#define REG_NOTBOL 1
#define REG_NOTEOL 2
#define REG_STARTEND 4
#define REG_NOMATCH 1
#define REG_BADPAT 2
#define REG_ECOLLATE 3
#define REG_ECTYPE 4
#define REG_EESCAPE 5
#define REG_ESUBREG 6
#define REG_EBRACK 7
#define REG_EPAREN 8
#define REG_EBRACE 9
#define REG_BADBR 10
#define REG_ERANGE 11
#define REG_ESPACE 12
#define REG_BADRPT 13
typedef long regoff_t;
typedef struct { regoff_t rm_so, rm_eo; } regmatch_t;
enum { RX_CHAR, RX_ANY, RX_SET, RX_BOL, RX_EOL, RX_WORDB, RX_NWORDB, RX_WORDS, RX_WORDE, RX_GROUP, RX_BACKREF, RX_ALT };
struct rx_node {
    int type, min, max, group, c;
    char *set;
    struct rx_node *sub, *next, *alt;
};
struct rx_inst;
typedef struct { size_t re_nsub; struct rx_inst *prog; int flags; int nregs, len; } regex_t;
struct rx_parse { const char *p; int ext, icase, ngroups, err; };
static struct rx_node *rx_parse_alt(struct rx_parse *ps, int depth);
static struct rx_node *rx_new(int type) { struct rx_node *n = calloc(1, sizeof *n); n->type = type; n->min = n->max = 1; return n; }
static void rx_free(struct rx_node *n)
{
    while (n) { struct rx_node *nx = n->next; rx_free(n->sub); rx_free(n->alt); free(n->set); free(n); n = nx; }
}
static struct rx_node *rx_parse_set(struct rx_parse *ps)
{
    struct rx_node *n = rx_new(RX_SET);
    const char *p = ps->p;
    int neg = 0, first = 1, c;
    n->set = calloc(1, 256);
    if (*p == '^') { neg = 1; p++; }
    while (*p && (*p != ']' || first)) {
        first = 0;
        if (p[0] == '[' && p[1] == ':') {
            const char *e = strstr(p + 2, ":]");
            if (e) {
                char nm[16]; size_t l = e - p - 2;
                if (l > 8) { ps->err = REG_ECTYPE; return n; }
                memcpy(nm, p + 2, l); nm[l] = ':'; nm[l + 1] = 0;
                for (c = 0; c < 256; c++) if (c_fn_class(nm, c)) n->set[c] = 1;
                p = e + 2;
                continue;
            }
        }
        c = (unsigned char)*p++;
        if (*p == '-' && p[1] && p[1] != ']') {
            int hi = (unsigned char)p[1], i;
            p += 2;
            if (hi < c) { ps->err = REG_ERANGE; return n; }
            for (i = c; i <= hi; i++) n->set[i] = 1;
        } else n->set[c] = 1;
    }
    if (*p != ']') { ps->err = REG_EBRACK; return n; }
    p++;
    if (ps->icase) for (c = 0; c < 256; c++) if (n->set[c]) { n->set[tolower(c)] = 1; n->set[toupper(c)] = 1; }
    if (neg) for (c = 0; c < 256; c++) n->set[c] = !n->set[c];
    ps->p = p;
    return n;
}
static struct rx_node *rx_parse_atom(struct rx_parse *ps, int depth, int *bol_ok)
{
    const char *p = ps->p;
    struct rx_node *n;
    int ext = ps->ext;
    (void)bol_ok;
    if (*p == '(' && ext) {
        int g = ++ps->ngroups;
        ps->p = p + 1;
        n = rx_new(RX_GROUP); n->group = g;
        n->sub = rx_parse_alt(ps, depth + 1);
        if (*ps->p != ')') { if (!ps->err) ps->err = REG_EPAREN; return n; }
        ps->p++;
        return n;
    }
    if (*p == '\\' && !ext && p[1] == '(') {
        int g = ++ps->ngroups;
        ps->p = p + 2;
        n = rx_new(RX_GROUP); n->group = g;
        n->sub = rx_parse_alt(ps, depth + 1);
        if (ps->p[0] != '\\' || ps->p[1] != ')') { if (!ps->err) ps->err = REG_EPAREN; return n; }
        ps->p += 2;
        return n;
    }
    if (*p == '.') { ps->p++; return rx_new(RX_ANY); }
    if (*p == '[') { ps->p++; return rx_parse_set(ps); }
    if (*p == '^' && (ext || p == ps->p)) { ps->p++; return rx_new(RX_BOL); }
    if (*p == '$') { ps->p++; return rx_new(RX_EOL); }
    if (*p == '\\') {
        int c = p[1];
        ps->p += 2;
        if (!c) { ps->err = REG_EESCAPE; return rx_new(RX_CHAR); }
        if (c >= '1' && c <= '9') { n = rx_new(RX_BACKREF); n->group = c - '0'; return n; }
        if (c == 'b') return rx_new(RX_WORDB);
        if (c == 'B') return rx_new(RX_NWORDB);
        if (c == '<') return rx_new(RX_WORDS);
        if (c == '>') return rx_new(RX_WORDE);
        if (c == 'w' || c == 'W' || c == 's' || c == 'S' || c == 'd' || c == 'D') {
            int i;
            n = rx_new(RX_SET); n->set = calloc(1, 256);
            for (i = 0; i < 256; i++) {
                int in = (c == 'w' || c == 'W') ? (isalnum(i) || i == '_') : (c == 's' || c == 'S') ? isspace(i) : isdigit(i);
                n->set[i] = isupper(c) ? !in : in;
            }
            return n;
        }
        n = rx_new(RX_CHAR); n->c = c == 'n' ? '\n' : c == 't' ? '\t' : c; return n;
    }
    ps->p++;
    n = rx_new(RX_CHAR); n->c = (unsigned char)*p;
    if (*p == '*' && !ext) { /* a leading star in a basic expression is a plain one */ }
    return n;
}
static struct rx_node *rx_parse_seq(struct rx_parse *ps, int depth)
{
    struct rx_node *head = NULL, **tail = &head;
    while (*ps->p && !ps->err) {
        const char *p = ps->p;
        struct rx_node *a;
        int ext = ps->ext;
        if (ext && (*p == '|' || (*p == ')' && depth > 0))) break;
        if (!ext && p[0] == '\\' && (p[1] == '|' || (p[1] == ')' && depth > 0))) break;
        a = rx_parse_atom(ps, depth, NULL);
        if (ps->err) { rx_free(a); break; }
        for (;;) {
            p = ps->p;
            if (*p == '*' && !(a->type == RX_BOL && !ext)) { a->min = 0; a->max = -1; ps->p++; }
            else if (ext && *p == '+') { a->min = 1; a->max = -1; ps->p++; }
            else if (ext && *p == '?') { a->min = 0; a->max = 1; ps->p++; }
            else if (!ext && p[0] == '\\' && (p[1] == '+' || p[1] == '?')) { a->min = p[1] == '+'; a->max = p[1] == '+' ? -1 : 1; ps->p += 2; }
            else if ((ext && *p == '{') || (!ext && p[0] == '\\' && p[1] == '{')) {
                char *e;
                long lo, hi;
                p += ext ? 1 : 2;
                lo = strtol(p, &e, 10);
                if (e == p) { ps->err = REG_BADBR; break; }
                hi = lo;
                if (*e == ',') { p = e + 1; if (*p == '}' || *p == '\\') { hi = -1; e = (char *)p; } else { hi = strtol(p, &e, 10); } }
                if (ext ? *e != '}' : (e[0] != '\\' || e[1] != '}')) { ps->err = REG_EBRACE; break; }
                ps->p = e + (ext ? 1 : 2);
                if (hi >= 0 && hi < lo) { ps->err = REG_BADBR; break; }
                a->min = lo; a->max = hi;
            } else break;
            if (a->type == RX_BOL || a->type == RX_EOL) break;
            {
                /* a repeat on a repeat: wrap what we have */
                if ((ext && (*ps->p == '*' || *ps->p == '+' || *ps->p == '?'))) {
                    struct rx_node *w = rx_new(RX_GROUP);
                    w->group = 0; w->sub = a; a = w;
                } else break;
            }
        }
        *tail = a; tail = &a->next;
    }
    return head;
}
static struct rx_node *rx_parse_alt(struct rx_parse *ps, int depth)
{
    struct rx_node *left = rx_parse_seq(ps, depth), *n;
    int ext = ps->ext;
    if ((ext && *ps->p == '|') || (!ext && ps->p[0] == '\\' && ps->p[1] == '|')) {
        ps->p += ext ? 1 : 2;
        n = rx_new(RX_ALT);
        n->sub = left;
        n->alt = rx_parse_alt(ps, depth);
        return n;
    }
    return left;
}
enum { I_CHAR, I_ANY, I_SET, I_SPLIT, I_JMP, I_SAVE, I_BOL, I_EOL, I_WORDB, I_NWORDB, I_WORDS, I_WORDE, I_BACKREF, I_MARK, I_CHK, I_MATCH };
struct rx_inst { int op, x, y; char *set; };
struct rx_gen { struct rx_inst *p; int n, cap, nregs, err; int icase; };
static int rx_emit(struct rx_gen *g, int op, int x, int y, char *set)
{
    if (g->n == g->cap) {
        struct rx_inst *np;
        if (g->cap > 200000) { g->err = REG_ESPACE; return 0; }
        g->cap = g->cap * 2 + 32;
        np = realloc(g->p, g->cap * sizeof *np);
        if (!np) { g->err = REG_ESPACE; return 0; }
        g->p = np;
    }
    g->p[g->n].op = op; g->p[g->n].x = x; g->p[g->n].y = y; g->p[g->n].set = set;
    return g->n++;
}
static void rx_gen_list(struct rx_gen *g, struct rx_node *n);
static void rx_gen_one(struct rx_gen *g, struct rx_node *n)
{
    switch (n->type) {
    case RX_CHAR: rx_emit(g, I_CHAR, n->c, 0, NULL); break;
    case RX_ANY: rx_emit(g, I_ANY, 0, 0, NULL); break;
    case RX_SET: rx_emit(g, I_SET, 0, 0, n->set); break;
    case RX_BOL: rx_emit(g, I_BOL, 0, 0, NULL); break;
    case RX_EOL: rx_emit(g, I_EOL, 0, 0, NULL); break;
    case RX_WORDB: rx_emit(g, I_WORDB, 0, 0, NULL); break;
    case RX_NWORDB: rx_emit(g, I_NWORDB, 0, 0, NULL); break;
    case RX_WORDS: rx_emit(g, I_WORDS, 0, 0, NULL); break;
    case RX_WORDE: rx_emit(g, I_WORDE, 0, 0, NULL); break;
    case RX_BACKREF: rx_emit(g, I_BACKREF, n->group, 0, NULL); break;
    case RX_GROUP:
        if (n->group) rx_emit(g, I_SAVE, 2 * n->group, 0, NULL);
        rx_gen_list(g, n->sub);
        if (n->group) rx_emit(g, I_SAVE, 2 * n->group + 1, 0, NULL);
        break;
    case RX_ALT: {
        int sp = rx_emit(g, I_SPLIT, 0, 0, NULL), j;
        if (g->err) return;
        g->p[sp].x = g->n;
        rx_gen_list(g, n->sub);
        j = rx_emit(g, I_JMP, 0, 0, NULL);
        if (g->err) return;
        g->p[sp].y = g->n;
        rx_gen_list(g, n->alt);
        g->p[j].x = g->n;
        break;
    }
    }
}
static void rx_gen_rep(struct rx_gen *g, struct rx_node *n)
{
    int i, ends[256], ne = 0;
    struct rx_node one = *n;
    one.next = NULL;
    if (n->type == RX_ALT) { one.min = one.max = 1; }
    for (i = 0; i < n->min && !g->err; i++) rx_gen_one(g, &one);
    if (n->max < 0) {
        int l1 = g->n, sp = rx_emit(g, I_SPLIT, 0, 0, NULL), reg = g->nregs++;
        if (g->err) return;
        g->p[sp].x = g->n;
        rx_emit(g, I_MARK, reg, 0, NULL);
        rx_gen_one(g, &one);
        rx_emit(g, I_CHK, reg, 0, NULL);
        rx_emit(g, I_JMP, l1, 0, NULL);
        g->p[sp].y = g->n;
    } else {
        for (i = n->min; i < n->max && !g->err; i++) {
            int sp = rx_emit(g, I_SPLIT, 0, 0, NULL);
            if (g->err) return;
            g->p[sp].x = g->n;
            if (ne < 256) ends[ne++] = sp;
            rx_gen_one(g, &one);
        }
        for (i = 0; i < ne; i++) g->p[ends[i]].y = g->n;
    }
}
static void rx_gen_list(struct rx_gen *g, struct rx_node *n)
{
    for (; n && !g->err; n = n->next) {
        if (n->min == 1 && n->max == 1) rx_gen_one(g, n); else rx_gen_rep(g, n);
    }
}
static int rx_nopt(struct rx_node *n) { (void)n; return 0; }
int regcomp(regex_t *re, const char *pat, int flags)
{
    struct rx_parse ps = { pat, flags & REG_EXTENDED, flags & REG_ICASE, 0, 0 };
    struct rx_gen g = { NULL, 0, 0, 0, 0, flags & REG_ICASE };
    struct rx_node *tree;
    (void)rx_nopt;
    tree = rx_parse_alt(&ps, 0);
    if (!ps.err && *ps.p) ps.err = REG_EPAREN;
    if (ps.err) { rx_free(tree); return ps.err; }
    rx_emit(&g, I_SAVE, 0, 0, NULL);
    rx_gen_list(&g, tree);
    rx_emit(&g, I_SAVE, 1, 0, NULL);
    rx_emit(&g, I_MATCH, 0, 0, NULL);
    if (g.err) { rx_free(tree); free(g.p); return g.err; }
    /* sets are owned by the program from here on; give the tree's away without freeing them */
    {
        struct rx_inst *pi;
        for (pi = g.p; pi < g.p + g.n; pi++) if (pi->op == I_SET) { char *c = malloc(256); memcpy(c, pi->set, 256); pi->set = c; }
    }
    rx_free(tree);
    re->prog = g.p; re->len = g.n; re->nregs = g.nregs; re->re_nsub = ps.ngroups; re->flags = flags;
    return 0;
}
struct rx_m { const char *s, *end; const struct rx_inst *p; long *cap, *reg; int ncap, icase, newline, notbol, noteol; long steps; };
static int rx_word(int c) { return isalnum(c) || c == '_'; }
static int rx_exec(struct rx_m *m, int pc, const char *s)
{
    for (;;) {
        const struct rx_inst *i = &m->p[pc];
        if (++m->steps > 20000000) return 0;
        switch (i->op) {
        case I_CHAR: if (s < m->end && (m->icase ? tolower((unsigned char)*s) == tolower(i->x) : (unsigned char)*s == i->x)) { s++; pc++; continue; } return 0;
        case I_ANY: if (s < m->end && !(m->newline && *s == '\n')) { s++; pc++; continue; } return 0;
        case I_SET: if (s < m->end && i->set[(unsigned char)*s]) { s++; pc++; continue; } return 0;
        case I_JMP: pc = i->x; continue;
        case I_SPLIT: if (rx_exec(m, i->x, s)) return 1; pc = i->y; continue;
        case I_SAVE: {
            long old;
            if (i->x >= m->ncap) { pc++; continue; }
            old = m->cap[i->x]; m->cap[i->x] = s - m->s;
            if (rx_exec(m, pc + 1, s)) return 1;
            m->cap[i->x] = old;
            return 0;
        }
        case I_MARK: {
            long old = m->reg[i->x];
            m->reg[i->x] = s - m->s;
            if (rx_exec(m, pc + 1, s)) return 1;
            m->reg[i->x] = old;
            return 0;
        }
        case I_CHK: if (m->reg[i->x] == s - m->s) return 0; pc++; continue;
        case I_BOL: if ((s == m->s && !m->notbol) || (m->newline && s > m->s && s[-1] == '\n')) { pc++; continue; } return 0;
        case I_EOL: if ((s == m->end && !m->noteol) || (m->newline && s < m->end && *s == '\n')) { pc++; continue; } return 0;
        case I_WORDB: case I_NWORDB: case I_WORDS: case I_WORDE: {
            int a = s > m->s && rx_word((unsigned char)s[-1]), b = s < m->end && rx_word((unsigned char)*s), ok;
            ok = i->op == I_WORDB ? a != b : i->op == I_NWORDB ? a == b : i->op == I_WORDS ? !a && b : a && !b;
            if (!ok) return 0;
            pc++; continue;
        }
        case I_BACKREF: {
            long so, eo;
            if (2 * i->x + 1 >= m->ncap) return 0;
            so = m->cap[2 * i->x]; eo = m->cap[2 * i->x + 1];
            if (so < 0 || eo < 0) return 0;
            if (s + (eo - so) > m->end) return 0;
            if (m->icase ? strncasecmp(s, m->s + so, eo - so) : memcmp(s, m->s + so, eo - so)) return 0;
            s += eo - so; pc++; continue;
        }
        case I_MATCH: m->cap[1] = s - m->s; return 1;
        }
        return 0;
    }
}
int regexec(const regex_t *re, const char *str, size_t nmatch, regmatch_t pmatch[], int eflags)
{
    struct rx_m m;
    const char *s = str, *end = str + strlen(str);
    int ncap = 2 * (re->re_nsub + 1), i, found = 0;
    long *cap = malloc((ncap + re->nregs + 1) * sizeof *cap);
    if (!cap) return REG_ESPACE;
    if (eflags & REG_STARTEND) { s = str + pmatch[0].rm_so; end = str + pmatch[0].rm_eo; }
    m.s = str; m.end = end; m.p = re->prog; m.cap = cap; m.reg = cap + ncap; m.ncap = ncap;
    m.icase = re->flags & REG_ICASE; m.newline = re->flags & REG_NEWLINE; m.notbol = eflags & REG_NOTBOL; m.noteol = eflags & REG_NOTEOL; m.steps = 0;
    for (; s <= end; s++) {
        for (i = 0; i < ncap + re->nregs; i++) cap[i] = -1;
        if (rx_exec(&m, 0, s)) { found = 1; break; }
        if (m.steps > 20000000) break;
    }
    if (found && !(re->flags & REG_NOSUB) && pmatch) {
        size_t k;
        for (k = 0; k < nmatch; k++) {
            if ((int)(2 * k + 1) < ncap && cap[2 * k] >= 0 && cap[2 * k + 1] >= 0) { pmatch[k].rm_so = cap[2 * k]; pmatch[k].rm_eo = cap[2 * k + 1]; }
            else { pmatch[k].rm_so = -1; pmatch[k].rm_eo = -1; }
        }
    }
    free(cap);
    return found ? 0 : REG_NOMATCH;
}
size_t regerror(int code, const regex_t *re, char *buf, size_t n)
{
    static const char *const msg[] = { "Success", "No match", "Invalid regular expression", "Invalid collation character", "Invalid character class name", "Trailing backslash", "Invalid back reference", "Unmatched [, [^, [:, [., or [=", "Unmatched ( or \\(", "Unmatched \\{", "Invalid content of \\{\\}", "Invalid range end", "Memory exhausted", "Invalid preceding regular expression" };
    const char *m = code >= 0 && code <= REG_BADRPT ? msg[code] : "Unknown error";
    (void)re;
    if (n) strlcpy(buf, m, n);
    return strlen(m) + 1;
}
void regfree(regex_t *re)
{
    int i;
    if (!re->prog) return;
    for (i = 0; i < re->len; i++) if (re->prog[i].op == I_SET) free(re->prog[i].set);
    free(re->prog);
    re->prog = NULL;
}
