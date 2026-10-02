#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#define MAX_ITER 1000          
#define MAX_KAPREKAR 4294967295ULL    

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_NULL_POINTER,
    ERROR_INVALID_NUMBER,  
    ERROR_OVERFLOW,
    ERROR_DOMAIN,         
    ERROR_NO_ROOT,       
    ERROR_ITERATIONS
} status_code;

void print_error(const status_code code) {
    switch (code) {
        case ERROR_INVALID_ARGS:    printf("Ошибка: неверные аргументы функции\n"); break;
        case ERROR_NULL_POINTER:    printf("Ошибка: нулевой указатель\n"); break;
        case ERROR_INVALID_NUMBER:  printf("Ошибка: некорректная строка с числом\n"); break;
        case ERROR_OVERFLOW:        printf("Ошибка: переполнение\n"); break;
        case ERROR_DOMAIN:          printf("Ошибка: значение вне области определения\n"); break;
        case ERROR_NO_ROOT:         printf("Ошибка: на интервале нет корня (знаки на концах совпадают)\n"); break;
        case ERROR_ITERATIONS:      printf("Ошибка: превышено число итераций\n"); break;
        default:                    printf("Неизвестная ошибка\n"); break;
    }
}

typedef struct { double x, y; } point;

static point read_point(va_list *args) {
    point p;
    p.x = va_arg(*args, double);
    p.y = va_arg(*args, double);
    return p;
}

static void mark_turn(const point a, const point b, const point c, const double eps, int *pos, int *neg) {
    const double cross = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
    if (cross > eps) *pos = 1;
    else if (cross < -eps) *neg = 1;
}

status_code is_convex(int *result, const double eps, const int n, ...) {
    if (result == NULL) return ERROR_NULL_POINTER;
    if (n < 3 || eps < 0) return ERROR_INVALID_ARGS;

    va_list args;
    va_start(args, n);
    const point first = read_point(&args);
    const point second = read_point(&args);
    point a = first, b = second;
    int pos = 0, neg = 0;

    for (int i = 2; i < n; i++) {
        const point c = read_point(&args);
        mark_turn(a, b, c, eps, &pos, &neg);
        a = b;
        b = c;
    }
    va_end(args);

    mark_turn(a, b, first, eps, &pos, &neg);
    mark_turn(b, first, second, eps, &pos, &neg);

    *result = (pos != neg);
    return SUCCESS;
}

status_code poly_value(double *result, const double x, const int n, ...) {
    if (result == NULL) return ERROR_NULL_POINTER;
    if (n < 0) return ERROR_INVALID_ARGS;

    va_list args;
    va_start(args, n);
    double value = 0;
    for (int i = 0; i <= n; i++) value = value * x + va_arg(args, double);
    va_end(args);

    if (!isfinite(value)) return ERROR_OVERFLOW;
    *result = value;
    return SUCCESS;
}

static status_code parse_number(const char *s, const int base, unsigned long long *n) {
    if (s == NULL || n == NULL) return ERROR_NULL_POINTER;
    if (*s == '\0') return ERROR_INVALID_NUMBER;
    *n = 0;
    for (; *s != '\0'; s++) {
        int d;
        if (isdigit((unsigned char)*s)) d = *s - '0';
        else if (isalpha((unsigned char)*s)) d = toupper((unsigned char)*s) - 'A' + 10;
        else return ERROR_INVALID_NUMBER;

        if (d >= base) return ERROR_INVALID_NUMBER;
        *n = *n * base + d;
        if (*n > MAX_KAPREKAR) return ERROR_OVERFLOW;
    }
    return SUCCESS;
}

static int is_kaprekar(const unsigned long long n, const int base) {
    if (n == 1) return 1;
    const unsigned long long sq = n * n;
    unsigned long long pw = base;

    while (pw <= sq) {
        const unsigned long long right = sq % pw, left = sq / pw;
        if (right > 0 && left + right == n) return 1;
        if (pw > sq / base) break;
        pw *= base;
    }
    return 0;
}

status_code find_kaprekar(const char **found, int *found_count, const int base, const int count, ...) {
    if (found == NULL || found_count == NULL) return ERROR_NULL_POINTER;
    if (base < 2 || base > 36 || count < 0) return ERROR_INVALID_ARGS;

    va_list args;
    va_start(args, count);
    *found_count = 0;

    for (int i = 0; i < count; i++) {
        const char *s = va_arg(args, const char *);
        unsigned long long n;
        const status_code status = parse_number(s, base, &n);
        if (status != SUCCESS) {
            va_end(args);
            return status;
        }
        if (is_kaprekar(n, base)) found[(*found_count)++] = s;
    }
    va_end(args);
    return SUCCESS;
}

status_code geom_mean(double *result, const int count, ...) {
    if (result == NULL) return ERROR_NULL_POINTER;
    if (count <= 0) return ERROR_INVALID_ARGS;

    va_list args;
    va_start(args, count);
    double sum_log = 0;   
    for (int i = 0; i < count; i++) {
        const double x = va_arg(args, double);
        if (x < 0) {
            va_end(args);
            return ERROR_DOMAIN;
        }
        sum_log += log(x);
    }
    va_end(args);

    *result = exp(sum_log / count);
    return SUCCESS;
}

static double pow_rec(const double x, const unsigned long long n) {
    if (n == 0) return 1.0;
    const double half = pow_rec(x, n / 2);
    return (n % 2 == 0) ? half * half : half * half * x;
}

status_code fast_pow(double *result, const double x, const int n, const double eps) {
    if (result == NULL) return ERROR_NULL_POINTER;
    if (eps <= 0) return ERROR_INVALID_ARGS;

    if (n >= 0) {
        *result = pow_rec(x, (unsigned long long)n);
    } else {
        if (fabs(x) < eps) return ERROR_DOMAIN;
        *result = 1.0 / pow_rec(x, -(long long)n);
    }
    return SUCCESS;
}

status_code bisection(double *root, double a, double b, const double eps, double (*f)(double)) {
    if (root == NULL || f == NULL) return ERROR_NULL_POINTER;
    if (eps <= 0 || a >= b) return ERROR_INVALID_ARGS;

    double fa = f(a);
    double fb = f(b);
    if (fabs(fa) < eps) { *root = a; return SUCCESS; }
    if (fabs(fb) < eps) { *root = b; return SUCCESS; }
    if ((fa < 0) == (fb < 0)) return ERROR_NO_ROOT;

    for (int i = 0; i < MAX_ITER; i++) {
        const double mid = a + (b - a) / 2;
        const double fm = f(mid);
        if ((b - a) / 2 < eps || fabs(fm) < eps) {
            *root = mid;
            return SUCCESS;
        }
        if ((fa < 0) == (fm < 0)) { a = mid; fa = fm; }
        else b = mid;
    }
    return ERROR_ITERATIONS;
}

double f1(const double x) { return x * x - 2; }
double f2(const double x) { return cos(x) - x; }

int report(const status_code status) {
    if (status == SUCCESS) return 0;
    print_error(status);
    return 1;
}
