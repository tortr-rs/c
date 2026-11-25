/* ---- sockets, poll, select, name lookup [chamesle] ---- */
#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_LOCAL 1
#define AF_INET 2
#define AF_INET6 10
#define PF_UNSPEC 0
#define PF_UNIX 1
#define PF_INET 2
#define PF_INET6 10
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_NONBLOCK 04000
#define SOCK_CLOEXEC 02000000
#define SOL_SOCKET 1
#define SO_DEBUG 1
#define SO_REUSEADDR 2
#define SO_TYPE 3
#define SO_ERROR 4
#define SO_BROADCAST 6
#define SO_SNDBUF 7
#define SO_RCVBUF 8
#define SO_KEEPALIVE 9
#define SO_LINGER 13
#define SO_REUSEPORT 15
#define SO_RCVTIMEO 20
#define SO_SNDTIMEO 21
#define SOMAXCONN 4096
#define MSG_PEEK 2
#define MSG_DONTWAIT 0x40
#define MSG_NOSIGNAL 0x4000
#define MSG_WAITALL 0x100
#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define IPPROTO_IPV6 41
#define TCP_NODELAY 1
#define IPV6_V6ONLY 26
#define INADDR_ANY ((in_addr_t)0)
#define INADDR_LOOPBACK ((in_addr_t)0x7f000001)
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
typedef unsigned socklen_t;
typedef unsigned short sa_family_t;
typedef unsigned short in_port_t;
typedef unsigned in_addr_t;
struct sockaddr { sa_family_t sa_family; char sa_data[14]; };
struct sockaddr_storage { sa_family_t ss_family; char pad[126]; unsigned long align; };
struct in_addr { in_addr_t s_addr; };
struct in6_addr { unsigned char s6_addr[16]; };
struct sockaddr_in { sa_family_t sin_family; in_port_t sin_port; struct in_addr sin_addr; unsigned char sin_zero[8]; };
struct sockaddr_in6 { sa_family_t sin6_family; in_port_t sin6_port; unsigned sin6_flowinfo; struct in6_addr sin6_addr; unsigned sin6_scope_id; };
struct sockaddr_un { sa_family_t sun_family; char sun_path[108]; };
struct linger { int l_onoff, l_linger; };
struct msghdr { void *msg_name; socklen_t msg_namelen; struct iovec *msg_iov; size_t msg_iovlen; void *msg_control; size_t msg_controllen; int msg_flags; };
static const struct in6_addr in6addr_any = { { 0 } };
static const struct in6_addr in6addr_loopback = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } };
#define IN6ADDR_ANY_INIT { { 0 } }
unsigned short htons(unsigned short x) { return __builtin_bswap16(x); }
unsigned short ntohs(unsigned short x) { return __builtin_bswap16(x); }
unsigned htonl(unsigned x) { return __builtin_bswap32(x); }
unsigned ntohl(unsigned x) { return __builtin_bswap32(x); }
int socket(int d, int t, int p) { return fail(sys3(SYS_socket, d, t, p)); }
int socketpair(int d, int t, int p, int sv[2]) { return fail(sys4(SYS_socketpair, d, t, p, sv)); }
int bind(int fd, const struct sockaddr *a, socklen_t l) { return fail(sys3(SYS_bind, fd, a, l)); }
int listen(int fd, int n) { return fail(sys2(SYS_listen, fd, n)); }
int accept(int fd, struct sockaddr *a, socklen_t *l) { return fail(sys3(SYS_accept, fd, a, l)); }
int accept4(int fd, struct sockaddr *a, socklen_t *l, int fl) { return fail(sys4(SYS_accept4, fd, a, l, fl)); }
int connect(int fd, const struct sockaddr *a, socklen_t l) { return fail(sys3(SYS_connect, fd, a, l)); }
int shutdown(int fd, int how) { return fail(sys2(SYS_shutdown, fd, how)); }
int getsockname(int fd, struct sockaddr *a, socklen_t *l) { return fail(sys3(SYS_getsockname, fd, a, l)); }
int getpeername(int fd, struct sockaddr *a, socklen_t *l) { return fail(sys3(SYS_getpeername, fd, a, l)); }
int setsockopt(int fd, int lv, int n, const void *v, socklen_t l) { return fail(sys5(SYS_setsockopt, fd, lv, n, v, l)); }
int getsockopt(int fd, int lv, int n, void *v, socklen_t *l) { return fail(sys5(SYS_getsockopt, fd, lv, n, v, l)); }
ssize_t send(int fd, const void *b, size_t n, int fl) { return fail(sys6(SYS_sendto, fd, (long)b, n, fl, 0, 0)); }
ssize_t recv(int fd, void *b, size_t n, int fl) { return fail(sys6(SYS_recvfrom, fd, (long)b, n, fl, 0, 0)); }
ssize_t sendto(int fd, const void *b, size_t n, int fl, const struct sockaddr *a, socklen_t l) { return fail(sys6(SYS_sendto, fd, (long)b, n, fl, (long)a, l)); }
ssize_t recvfrom(int fd, void *b, size_t n, int fl, struct sockaddr *a, socklen_t *l) { return fail(sys6(SYS_recvfrom, fd, (long)b, n, fl, (long)a, (long)l)); }
ssize_t sendmsg(int fd, const struct msghdr *m, int fl) { return fail(sys3(SYS_sendmsg, fd, m, fl)); }
ssize_t recvmsg(int fd, struct msghdr *m, int fl) { return fail(sys3(SYS_recvmsg, fd, m, fl)); }

struct pollfd { int fd; short events, revents; };
#define POLLIN 1
#define POLLPRI 2
#define POLLOUT 4
#define POLLERR 8
#define POLLHUP 16
#define POLLNVAL 32
typedef unsigned long nfds_t;
int poll(struct pollfd *f, nfds_t n, int ms)
{
    struct timespec t, *tp = NULL;
    if (ms >= 0) { t.tv_sec = ms / 1000; t.tv_nsec = (ms % 1000) * 1000000L; tp = &t; }
    return fail(sys5(SYS_ppoll, f, n, tp, 0, 8));
}
int ppoll(struct pollfd *f, nfds_t n, const struct timespec *t, const sigset_t *m) { return fail(sys5(SYS_ppoll, f, n, t, m, 8)); }
#define FD_SETSIZE 1024
#define C_FDBITS (8 * (int)sizeof(long))
typedef struct { unsigned long fds_bits[FD_SETSIZE / C_FDBITS]; } fd_set;
#define FD_ZERO(s) memset((s), 0, sizeof(fd_set))
#define FD_SET(fd, s) ((s)->fds_bits[(fd) / C_FDBITS] |= 1UL << ((fd) % C_FDBITS))
#define FD_CLR(fd, s) ((s)->fds_bits[(fd) / C_FDBITS] &= ~(1UL << ((fd) % C_FDBITS)))
#define FD_ISSET(fd, s) (((s)->fds_bits[(fd) / C_FDBITS] >> ((fd) % C_FDBITS)) & 1)
int select(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *tv) { return fail(sys5(SYS_select, n, r, w, e, tv)); }
int pselect(int n, fd_set *r, fd_set *w, fd_set *e, const struct timespec *t, const sigset_t *m)
{
    struct timeval tv, *tp = NULL;
    if (t) { tv.tv_sec = t->tv_sec; tv.tv_usec = t->tv_nsec / 1000; tp = &tv; }
    (void)m;
    return select(n, r, w, e, tp);
}

#define EPOLLIN 1
#define EPOLLOUT 4
#define EPOLLERR 8
#define EPOLLHUP 16
#define EPOLLET (1u << 31)
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3
#define EPOLL_CLOEXEC 02000000
struct epoll_event { unsigned events; union { void *ptr; int fd; unsigned u32; unsigned long u64; } data; } __attribute__((packed));
int epoll_create1(int fl) { return fail(sys1(291, fl)); }
int epoll_create(int n) { (void)n; return epoll_create1(0); }
int epoll_ctl(int ep, int op, int fd, struct epoll_event *ev) { return fail(sys4(233, ep, op, fd, ev)); }
int epoll_wait(int ep, struct epoll_event *ev, int max, int ms) { return fail(sys6(232, ep, (long)ev, max, ms, 0, 0)); }

int inet_aton(const char *s, struct in_addr *a)
{
    unsigned v = 0;
    int i;
    for (i = 0; i < 4; i++) {
        char *e;
        unsigned long p = strtoul(s, &e, 10);
        if (e == s || p > 255) return 0;
        v = v << 8 | p;
        if (i < 3) { if (*e != '.') return 0; s = e + 1; } else if (*e) return 0;
    }
    a->s_addr = htonl(v);
    return 1;
}
in_addr_t inet_addr(const char *s) { struct in_addr a; return inet_aton(s, &a) ? a.s_addr : (in_addr_t)-1; }
char *inet_ntoa(struct in_addr a) { static char b[16]; unsigned char *p = (unsigned char *)&a; sprintf(b, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]); return b; }
static int c_pton6(const char *s, unsigned char *d)
{
    unsigned short w[8] = { 0 };
    int n = 0, gap = -1;
    if (s[0] == ':' && s[1] == ':') { gap = 0; s += 2; if (!*s) goto done; }
    for (;;) {
        char *e;
        unsigned long v = strtoul(s, &e, 16);
        if (e == s || e - s > 4 || n >= 8) return 0;
        w[n++] = v;
        s = e;
        if (!*s) break;
        if (*s != ':') return 0;
        s++;
        if (*s == ':') { if (gap >= 0) return 0; gap = n; s++; if (!*s) break; }
    }
done:
    if (gap < 0 && n != 8) return 0;
    if (gap >= 0) { int k = 8 - n; memmove(w + gap + k, w + gap, (n - gap) * 2); memset(w + gap, 0, k * 2); }
    for (n = 0; n < 8; n++) { d[2 * n] = w[n] >> 8; d[2 * n + 1] = w[n]; }
    return 1;
}
int inet_pton(int af, const char *s, void *d)
{
    if (af == AF_INET) return inet_aton(s, d) ? 1 : 0;
    if (af == AF_INET6) return c_pton6(s, d);
    errno = EAFNOSUPPORT; return -1;
}
const char *inet_ntop(int af, const void *s, char *d, socklen_t n)
{
    const unsigned char *p = s;
    char buf[48];
    if (af == AF_INET) snprintf(buf, sizeof buf, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    else if (af == AF_INET6) {
        int i, best = -1, bl = 0, cur = -1, cl = 0;
        char *o = buf;
        for (i = 0; i < 8; i++) {
            if (!p[2 * i] && !p[2 * i + 1]) { if (cur < 0) { cur = i; cl = 0; } if (++cl > bl) { bl = cl; best = cur; } } else cur = -1;
        }
        if (bl < 2) best = -1;
        for (i = 0; i < 8; i++) {
            if (i == best) { *o++ = ':'; i += bl - 1; if (i == 7) *o++ = ':'; continue; }
            if (i) *o++ = ':';
            o += sprintf(o, "%x", p[2 * i] << 8 | p[2 * i + 1]);
        }
        *o = 0;
    } else { errno = EAFNOSUPPORT; return NULL; }
    if (strlen(buf) >= n) { errno = ENOSPC; return NULL; }
    strcpy(d, buf);
    return d;
}

/* names: /etc/hosts, then a plain UDP query to the first nameserver in /etc/resolv.conf */
struct hostent { char *h_name, **h_aliases; int h_addrtype, h_length; char **h_addr_list; };
#define h_addr h_addr_list[0]
struct addrinfo { int ai_flags, ai_family, ai_socktype, ai_protocol; socklen_t ai_addrlen; struct sockaddr *ai_addr; char *ai_canonname; struct addrinfo *ai_next; };
struct servent { char *s_name, **s_aliases; int s_port; char *s_proto; };
#define AI_PASSIVE 1
#define AI_CANONNAME 2
#define AI_NUMERICHOST 4
#define AI_NUMERICSERV 0x400
#define AI_ADDRCONFIG 0x20
#define AI_V4MAPPED 8
#define AI_ALL 0x10
#define NI_NUMERICHOST 1
#define NI_NUMERICSERV 2
#define NI_NAMEREQD 8
#define NI_DGRAM 16
#define NI_MAXHOST 1025
#define NI_MAXSERV 32
#define EAI_BADFLAGS (-1)
#define EAI_NONAME (-2)
#define EAI_AGAIN (-3)
#define EAI_FAIL (-4)
#define EAI_FAMILY (-6)
#define EAI_SOCKTYPE (-7)
#define EAI_SERVICE (-8)
#define EAI_MEMORY (-10)
#define EAI_SYSTEM (-11)
#define HOST_NOT_FOUND 1
int h_errno;
static int c_hosts(const char *name, int fam, unsigned char *out, int *outfam)
{
    char line[512];
    FILE *f = fopen("/etc/hosts", "r");
    if (!f) return 0;
    while (fgets(line, sizeof line, f)) {
        char *p = line, *tok, *save;
        unsigned char a[16];
        int af;
        line[strcspn(line, "#\n")] = 0;
        tok = strtok_r(p, " \t", &save);
        if (!tok) continue;
        af = inet_pton(AF_INET, tok, a) == 1 ? AF_INET : inet_pton(AF_INET6, tok, a) == 1 ? AF_INET6 : 0;
        if (!af || (fam != AF_UNSPEC && fam != af)) continue;
        while ((tok = strtok_r(NULL, " \t", &save))) if (!strcasecmp(tok, name)) { memcpy(out, a, 16); *outfam = af; fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}
static int c_dns(const char *name, unsigned char *out)
{
    unsigned char q[300], r[600];
    char line[256], ns[64] = "127.0.0.1";
    struct sockaddr_in sa;
    struct timeval tv = { 3, 0 };
    int fd, n = 0, i, an, pos;
    FILE *f = fopen("/etc/resolv.conf", "r");
    if (f) { while (fgets(line, sizeof line, f)) if (!strncmp(line, "nameserver", 10)) { sscanf(line + 10, "%63s", ns); break; } fclose(f); }
    memset(q, 0, 12);
    q[0] = 0x43; q[1] = 0x68; q[2] = 1; q[5] = 1;
    n = 12;
    while (*name) {
        const char *dot = strchr(name, '.');
        int l = dot ? (int)(dot - name) : (int)strlen(name);
        if (l > 63 || n + l + 6 > (int)sizeof q) return 0;
        q[n++] = l; memcpy(q + n, name, l); n += l;
        name += l; if (*name == '.') name++;
    }
    q[n++] = 0; q[n++] = 0; q[n++] = 1; q[n++] = 0; q[n++] = 1;
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; sa.sin_port = htons(53);
    if (!inet_aton(ns, &sa.sin_addr)) return 0;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
    for (i = 0; i < 2; i++) {
        long len;
        sendto(fd, q, n, 0, (struct sockaddr *)&sa, sizeof sa);
        len = recv(fd, r, sizeof r, 0);
        if (len < 12 || r[0] != q[0] || r[1] != q[1]) continue;
        close(fd);
        if ((r[3] & 15) != 0) return 0;
        an = r[6] << 8 | r[7];
        pos = n;
        while (an-- > 0 && pos < len) {
            int rl, ty;
            while (pos < len && r[pos]) { if ((r[pos] & 0xc0) == 0xc0) { pos++; break; } pos += r[pos] + 1; }
            pos++;
            if (pos + 10 > len) return 0;
            ty = r[pos] << 8 | r[pos + 1]; rl = r[pos + 8] << 8 | r[pos + 9]; pos += 10;
            if (ty == 1 && rl == 4 && pos + 4 <= len) { memcpy(out, r + pos, 4); return 1; }
            pos += rl;
        }
        return 0;
    }
    close(fd);
    return 0;
}
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
    int fam = hints ? hints->ai_family : AF_UNSPEC, socktype = hints ? hints->ai_socktype : 0, flags = hints ? hints->ai_flags : 0;
    unsigned char addr[16];
    int af = 0, port = 0;
    struct addrinfo *ai;
    struct sockaddr_in *s4;
    struct sockaddr_in6 *s6;
    char *e;
    if (!node && !service) return EAI_NONAME;
    if (service) {
        port = strtol(service, &e, 10);
        if (*e) {
            if (flags & AI_NUMERICSERV) return EAI_NONAME;
            port = !strcmp(service, "http") ? 80 : !strcmp(service, "https") ? 443 : !strcmp(service, "ftp") ? 21 : !strcmp(service, "ssh") ? 22 : !strcmp(service, "domain") ? 53 : !strcmp(service, "smtp") ? 25 : !strcmp(service, "git") ? 9418 : 0;
            if (!port) return EAI_SERVICE;
        }
    }
    if (!node) { af = fam == AF_INET6 ? AF_INET6 : AF_INET; memset(addr, 0, 16); if (!(flags & AI_PASSIVE)) { if (af == AF_INET) { addr[0] = 127; addr[3] = 1; } else addr[15] = 1; } }
    else if (inet_pton(AF_INET, node, addr) == 1) af = AF_INET;
    else if (inet_pton(AF_INET6, node, addr) == 1) af = AF_INET6;
    else if (flags & AI_NUMERICHOST) return EAI_NONAME;
    else if (!strcmp(node, "localhost")) { af = fam == AF_INET6 ? AF_INET6 : AF_INET; memset(addr, 0, 16); if (af == AF_INET) { addr[0] = 127; addr[3] = 1; } else addr[15] = 1; }
    else if (c_hosts(node, fam, addr, &af)) ;
    else if (fam != AF_INET6 && c_dns(node, addr)) af = AF_INET;
    else return EAI_NONAME;
    if (fam != AF_UNSPEC && fam != af) return EAI_FAMILY;
    ai = calloc(1, sizeof *ai + sizeof(struct sockaddr_in6) + (node ? strlen(node) + 1 : 1));
    if (!ai) return EAI_MEMORY;
    ai->ai_family = af; ai->ai_socktype = socktype ? socktype : SOCK_STREAM;
    ai->ai_protocol = ai->ai_socktype == SOCK_DGRAM ? IPPROTO_UDP : IPPROTO_TCP;
    ai->ai_addr = (struct sockaddr *)(ai + 1);
    if (af == AF_INET) { s4 = (struct sockaddr_in *)ai->ai_addr; s4->sin_family = AF_INET; s4->sin_port = htons(port); memcpy(&s4->sin_addr, addr, 4); ai->ai_addrlen = sizeof *s4; }
    else { s6 = (struct sockaddr_in6 *)ai->ai_addr; s6->sin6_family = AF_INET6; s6->sin6_port = htons(port); memcpy(&s6->sin6_addr, addr, 16); ai->ai_addrlen = sizeof *s6; }
    if ((flags & AI_CANONNAME) && node) { ai->ai_canonname = (char *)(ai + 1) + sizeof(struct sockaddr_in6); strcpy(ai->ai_canonname, node); }
    *res = ai;
    return 0;
}
void freeaddrinfo(struct addrinfo *ai) { while (ai) { struct addrinfo *n = ai->ai_next; free(ai); ai = n; } }
const char *gai_strerror(int e) { return e == EAI_NONAME ? "Name or service not known" : e == EAI_AGAIN ? "Temporary failure in name resolution" : e == EAI_FAMILY ? "Address family not supported" : e == EAI_SERVICE ? "Service not known" : e == EAI_MEMORY ? "Out of memory" : "Name resolution failed"; }
int getnameinfo(const struct sockaddr *sa, socklen_t l, char *host, socklen_t hl, char *serv, socklen_t sl, int flags)
{
    (void)l; (void)flags;
    if (host && hl) { if (!inet_ntop(sa->sa_family, sa->sa_family == AF_INET ? (void *)&((struct sockaddr_in *)sa)->sin_addr : (void *)&((struct sockaddr_in6 *)sa)->sin6_addr, host, hl)) return EAI_FAMILY; }
    if (serv && sl) snprintf(serv, sl, "%d", ntohs(((struct sockaddr_in *)sa)->sin_port));
    return 0;
}
struct hostent *gethostbyname(const char *name)
{
    static struct hostent h;
    static char *list[2], *al[1], buf[16], nm[256];
    struct addrinfo hints, *r;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    if (getaddrinfo(name, NULL, &hints, &r)) { h_errno = HOST_NOT_FOUND; return NULL; }
    memcpy(buf, &((struct sockaddr_in *)r->ai_addr)->sin_addr, 4);
    freeaddrinfo(r);
    strlcpy(nm, name, sizeof nm);
    list[0] = buf; list[1] = NULL; al[0] = NULL;
    h.h_name = nm; h.h_aliases = al; h.h_addrtype = AF_INET; h.h_length = 4; h.h_addr_list = list;
    return &h;
}
struct servent *getservbyname(const char *n, const char *p) { (void)n; (void)p; return NULL; }
const char *hstrerror(int e) { (void)e; return "Unknown host"; }
