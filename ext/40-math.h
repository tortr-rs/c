/* ---- libm: sqrt and the roundings by hand, the transcendental ones on the x87 [chamesle] ---- */
#define M_E 2.7182818284590452354
#define M_LOG2E 1.4426950408889634074
#define M_LOG10E 0.43429448190325182765
#define M_LN2 0.69314718055994530942
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.78539816339744830962
#define M_1_PI 0.31830988618379067154
#define M_2_PI 0.63661977236758134308
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT1_2 0.70710678118654752440
#define HUGE_VAL (__builtin_huge_val())
#define HUGE_VALF (__builtin_huge_valf())
#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))
#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4
#define FP_ILOGB0 (-2147483647 - 1)
#define FP_ILOGBNAN (-2147483647 - 1)
#define MATH_ERRNO 1
#define math_errhandling 1
typedef float float_t;
typedef double double_t;
#define isnan(x) __builtin_isnan(x)
#define isinf(x) __builtin_isinf(x)
#define isfinite(x) __builtin_isfinite(x)
#define isnormal(x) __builtin_isnormal(x)
#define signbit(x) __builtin_signbit(x)
#define fpclassify(x) __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)
#define isgreater(a, b) __builtin_isgreater(a, b)
#define isgreaterequal(a, b) __builtin_isgreaterequal(a, b)
#define isless(a, b) __builtin_isless(a, b)
#define islessequal(a, b) __builtin_islessequal(a, b)
#define islessgreater(a, b) __builtin_islessgreater(a, b)
#define isunordered(a, b) __builtin_isunordered(a, b)

static long double c_fyl2x(long double y, long double x) { long double r; __asm__("fyl2x" : "=t"(r) : "0"(x), "u"(y) : "st(1)"); return r; }
static long double c_f2xm1(long double x) { long double r; __asm__("f2xm1" : "=t"(r) : "0"(x)); return r; }
static long double c_fscale(long double x, long double n) { long double r; __asm__("fscale" : "=t"(r) : "0"(x), "u"(n)); return r; }
static long double c_frndint(long double x) { long double r; __asm__("frndint" : "=t"(r) : "0"(x)); return r; }
static long double c_exp2(long double x)
{
    long double n, f;
    if (x > 16400) return __builtin_infl();
    if (x < -16400) return 0;
    n = c_frndint(x); f = x - n;
    return c_fscale(c_f2xm1(f) + 1, n);
}

double fabs(double x) { union { double d; unsigned long long u; } v = { x }; v.u &= ~(1ULL << 63); return v.d; }
float fabsf(float x) { return (float)fabs(x); }
long double fabsl(long double x) { return x < 0 ? -x : x; }
double copysign(double x, double y) { union { double d; unsigned long long u; } a = { x }, b = { y }; a.u = (a.u & ~(1ULL << 63)) | (b.u & (1ULL << 63)); return a.d; }
float copysignf(float x, float y) { return (float)copysign(x, y); }
double sqrt(double x) { double r; __asm__("sqrtsd %1, %0" : "=x"(r) : "x"(x)); return r; }
float sqrtf(float x) { float r; __asm__("sqrtss %1, %0" : "=x"(r) : "x"(x)); return r; }
long double sqrtl(long double x) { long double r; __asm__("fsqrt" : "=t"(r) : "0"(x)); return r; }
double trunc(double x)
{
    union { double d; unsigned long long u; } v = { x };
    int e = (int)((v.u >> 52) & 0x7ff) - 0x3ff;
    if (e >= 52) return x;
    if (e < 0) { v.u &= 1ULL << 63; return v.d; }
    v.u &= ~(0x000fffffffffffffULL >> e);
    return v.d;
}
double floor(double x) { double t = trunc(x); return t > x ? t - 1 : t; }
double ceil(double x) { double t = trunc(x); return t < x ? t + 1 : t; }
double round(double x) { double t = trunc(x); if (fabs(x - t) >= 0.5) t += copysign(1.0, x); return t; }
double rint(double x) { return (double)c_frndint(x); }
double nearbyint(double x) { return rint(x); }
float floorf(float x) { return (float)floor(x); }
float ceilf(float x) { return (float)ceil(x); }
float truncf(float x) { return (float)trunc(x); }
float roundf(float x) { return (float)round(x); }
float rintf(float x) { return (float)rint(x); }
long double floorl(long double x) { long double t = (long double)(long long)x; return t > x ? t - 1 : t; }
long double ceill(long double x) { long double t = (long double)(long long)x; return t < x ? t + 1 : t; }
long double truncl(long double x) { return (long double)(long long)x; }
long lround(double x) { return (long)round(x); }
long long llround(double x) { return (long long)round(x); }
long lrint(double x) { return (long)rint(x); }
long long llrint(double x) { return (long long)rint(x); }
double fmod(double x, double y)
{
    long double r = x, d = y;
    if (y == 0 || isinf(x) || isnan(x) || isnan(y)) return __builtin_nan("");
    if (isinf(y)) return x;
    __asm__ __volatile__("1: fprem\n fnstsw %%ax\n testb $4, %%ah\n jnz 1b" : "+t"(r) : "u"(d) : "ax", "cc");
    return (double)r;
}
float fmodf(float x, float y) { return (float)fmod(x, y); }
double remainder(double x, double y) { double n = rint(x / y); return x - n * y; }
double modf(double x, double *ip) { double t = trunc(x); *ip = t; return isinf(x) ? copysign(0.0, x) : x - t; }
double frexp(double x, int *e)
{
    union { double d; unsigned long long u; } v = { x };
    int ex = (int)((v.u >> 52) & 0x7ff);
    if (!ex) {
        if (x == 0) { *e = 0; return x; }
        x *= 18014398509481984.0;
        v.d = x; ex = (int)((v.u >> 52) & 0x7ff) - 54;
    } else if (ex == 0x7ff) { *e = 0; return x; }
    *e = ex - 0x3fe;
    v.u = (v.u & ~(0x7ffULL << 52)) | (0x3feULL << 52);
    return v.d;
}
double scalbn(double x, int n) { return ldexp(x, n); }
double scalbln(double x, long n) { return ldexp(x, n > 100000 ? 100000 : n < -100000 ? -100000 : (int)n); }
int ilogb(double x) { int e; if (x == 0) return FP_ILOGB0; if (isnan(x)) return FP_ILOGBNAN; if (isinf(x)) return 2147483647; frexp(x, &e); return e - 1; }
double logb(double x) { return (double)ilogb(x); }
double fmin(double a, double b) { return isnan(a) ? b : isnan(b) ? a : a < b ? a : b; }
double fmax(double a, double b) { return isnan(a) ? b : isnan(b) ? a : a > b ? a : b; }
float fminf(float a, float b) { return (float)fmin(a, b); }
float fmaxf(float a, float b) { return (float)fmax(a, b); }
double fdim(double a, double b) { return a > b ? a - b : 0; }
double fma(double a, double b, double c) { return (double)((long double)a * b + c); }
double nan(const char *s) { (void)s; return __builtin_nan(""); }
double nextafter(double x, double y)
{
    union { double d; unsigned long long u; } v = { x };
    if (isnan(x) || isnan(y)) return x + y;
    if (x == y) return y;
    if (x == 0) { v.u = 1; return y > 0 ? v.d : -v.d; }
    if ((x < y) == (x > 0)) v.u++; else v.u--;
    return v.d;
}

double exp2(double x) { return (double)c_exp2(x); }
double exp(double x) { return (double)c_exp2((long double)x * 1.4426950408889634073599246810018921L); }
double expm1(double x)
{
    long double t = (long double)x * 1.4426950408889634073599246810018921L;
    if (fabs(x) < 0.3) return (double)(c_f2xm1(t));
    return (double)(c_exp2(t) - 1);
}
double log(double x) { if (x < 0) return __builtin_nan(""); return (double)c_fyl2x(0.693147180559945309417232121458176568L, x); }
double log2(double x) { if (x < 0) return __builtin_nan(""); return (double)c_fyl2x(1, x); }
double log10(double x) { if (x < 0) return __builtin_nan(""); return (double)c_fyl2x(0.301029995663981195213738894724493027L, x); }
double log1p(double x)
{
    long double r;
    if (x < -1) return __builtin_nan("");
    if (fabs(x) < 0.29) { __asm__("fyl2xp1" : "=t"(r) : "0"((long double)x), "u"(0.693147180559945309417232121458176568L) : "st(1)"); return (double)r; }
    return log(1 + x);
}
double pow(double x, double y)
{
    long double r;
    int yint;
    if (y == 0 || x == 1) return 1;
    if (isnan(x) || isnan(y)) return __builtin_nan("");
    yint = fabs(y) < 9007199254740992.0 && y == trunc(y);
    if (x == 0) {
        if (y < 0) return yint && fmod(y, 2) != 0 ? copysign(HUGE_VAL, x) : HUGE_VAL;
        return yint && fmod(y, 2) != 0 ? x : 0;
    }
    if (isinf(y)) { double a = fabs(x); return a == 1 ? 1 : (a > 1) == (y > 0) ? HUGE_VAL : 0; }
    if (isinf(x)) { if (x > 0) return y > 0 ? HUGE_VAL : 0; return y > 0 ? (yint && fmod(y, 2) != 0 ? -HUGE_VAL : HUGE_VAL) : (yint && fmod(y, 2) != 0 ? -0.0 : 0.0); }
    if (x < 0) {
        if (!yint) return __builtin_nan("");
        r = c_exp2((long double)y * c_fyl2x(1, -(long double)x));
        return fmod(y, 2) != 0 ? -(double)r : (double)r;
    }
    return (double)c_exp2((long double)y * c_fyl2x(1, (long double)x));
}
double sin(double x) { long double r = x; if (isinf(x) || isnan(x)) return __builtin_nan(""); if (fabs(x) > 1e18) r = fmod(x, 6.283185307179586476925286766559); __asm__("fsin" : "+t"(r)); return (double)r; }
double cos(double x) { long double r = x; if (isinf(x) || isnan(x)) return __builtin_nan(""); if (fabs(x) > 1e18) r = fmod(x, 6.283185307179586476925286766559); __asm__("fcos" : "+t"(r)); return (double)r; }
double tan(double x) { long double r = x, one; if (isinf(x) || isnan(x)) return __builtin_nan(""); __asm__("fptan" : "+t"(r), "=u"(one)); return (double)one; }
double atan2(double y, double x) { long double r; __asm__("fpatan" : "=t"(r) : "0"((long double)x), "u"((long double)y) : "st(1)"); return (double)r; }
double atan(double x) { return atan2(x, 1); }
double asin(double x) { if (fabs(x) > 1) return __builtin_nan(""); return atan2(x, sqrt((1 - x) * (1 + x))); }
double acos(double x) { if (fabs(x) > 1) return __builtin_nan(""); return atan2(sqrt((1 - x) * (1 + x)), x); }
double sinh(double x) { double e = expm1(fabs(x)); double r = e < 1e300 ? (e + e / (e + 1)) / 2 : exp(fabs(x)) / 2; return copysign(r, x); }
double cosh(double x) { double e = exp(fabs(x)); return (e + 1 / e) / 2; }
double tanh(double x) { double a = fabs(x), e; if (a > 20) return copysign(1.0, x); e = expm1(2 * a); return copysign(e / (e + 2), x); }
double asinh(double x) { double a = fabs(x); return copysign(log(a + sqrt(a * a + 1)), x); }
double acosh(double x) { if (x < 1) return __builtin_nan(""); return log(x + sqrt(x * x - 1)); }
double atanh(double x) { return 0.5 * log1p(2 * x / (1 - x)); }
double hypot(double x, double y)
{
    double a = fabs(x), b = fabs(y), t;
    if (isinf(a) || isinf(b)) return HUGE_VAL;
    if (a < b) { t = a; a = b; b = t; }
    if (a == 0) return 0;
    t = b / a;
    return a * sqrt(1 + t * t);
}
double cbrt(double x)
{
    double r;
    if (x == 0 || isinf(x) || isnan(x)) return x;
    r = copysign(pow(fabs(x), 1.0 / 3.0), x);
    return r - (r * r * r - x) / (3 * r * r);
}
double c_erf_series(double x) { double t = x, s = x; int n; for (n = 1; n < 80; n++) { t *= -x * x / n; s += t / (2 * n + 1); } return 1.1283791670955126 * s; }
double erfc(double x);
double erf(double x) { if (fabs(x) < 2.5) return c_erf_series(x); return x > 0 ? 1 - erfc(x) : erfc(-x) - 1; }
double erfc(double x)
{
    double f = 0;
    int n;
    if (x < 2.5) return 1 - erf(x);
    for (n = 60; n >= 1; n--) f = n / 2.0 / (x + f);
    return exp(-x * x) / (1.7724538509055160 * (x + f));
}
double lgamma(double x)
{
    static const double c[] = { 76.18009172947146, -86.50532032941677, 24.01409824083091, -1.231739572450155, 0.1208650973866179e-2, -0.5395239384953e-5 };
    double y, tmp, ser;
    int j;
    if (x < 0.5) return log(M_PI / fabs(sin(M_PI * x))) - lgamma(1 - x);
    y = x; tmp = x + 5.5; tmp -= (x + 0.5) * log(tmp); ser = 1.000000000190015;
    for (j = 0; j < 6; j++) ser += c[j] / ++y;
    return -tmp + log(2.5066282746310005 * ser / x);
}
double tgamma(double x)
{
    if (x == trunc(x) && x > 0 && x < 24) { double r = 1; int i; for (i = 2; i < (int)x; i++) r *= i; return r; }
    if (x < 0.5) return M_PI / (sin(M_PI * x) * tgamma(1 - x));
    return exp(lgamma(x));
}
#ifdef C_DECLS_ONLY
#define c_float_fn(name) float name##f(float x);
#else
#define c_float_fn(name) float name##f(float x) { return (float)name(x); }
#endif
c_float_fn(sin) c_float_fn(cos) c_float_fn(tan) c_float_fn(asin) c_float_fn(acos) c_float_fn(atan) c_float_fn(exp) c_float_fn(log) c_float_fn(log2)
c_float_fn(log10) c_float_fn(sinh) c_float_fn(cosh) c_float_fn(tanh) c_float_fn(exp2) c_float_fn(expm1) c_float_fn(log1p) c_float_fn(cbrt)
float powf(float x, float y) { return (float)pow(x, y); }
float atan2f(float y, float x) { return (float)atan2(y, x); }
float hypotf(float x, float y) { return (float)hypot(x, y); }
float ldexpf(float x, int n) { return (float)ldexp(x, n); }
float frexpf(float x, int *e) { return (float)frexp(x, e); }
long double expl(long double x) { return c_exp2(x * 1.4426950408889634073599246810018921L); }
long double logl(long double x) { return c_fyl2x(0.693147180559945309417232121458176568L, x); }
long double powl(long double x, long double y) { return (long double)pow((double)x, (double)y); }
long double sinl(long double x) { return sin((double)x); }
long double cosl(long double x) { return cos((double)x); }
