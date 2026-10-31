#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define MAX_ITERATIONS 10000000
#define MAX_M_GAMMA 40

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_NUMBER,
    ERROR_NULL_POINTER,
    ERROR_OUT_OF_DOMAIN,
    ERROR_NO_ROOT,
    ERROR_OVERFLOW,
    ERROR_NO_CONVERGENCE
} status_code;

void print_error(const status_code code) {
    switch (code) {
        case ERROR_INVALID_ARGS:
            printf("Ошибка: неверное количество аргументов командной строки\n");
            break;
        case ERROR_INVALID_NUMBER:
            printf("Ошибка: переданная строка не является корректным числом\n");
            break;
        case ERROR_NULL_POINTER:
            printf("Ошибка: передан нулевой указатель\n");
            break;
        case ERROR_OUT_OF_DOMAIN:
            printf("Ошибка: значение вне допустимой области определения\n");
            break;
        case ERROR_NO_ROOT:
            printf("Ошибка: на интервале нет корня\n");
            break;
        case ERROR_OVERFLOW:
            printf("Ошибка: переполнение при вычислении\n");
            break;
        case ERROR_NO_CONVERGENCE:
            printf("Ошибка: не сошлось за отведённое число шагов\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
}

status_code parse_eps(const char *input, double *result) {
    if (input == NULL || result == NULL) return ERROR_NULL_POINTER;
    if (*input == '\0') return ERROR_INVALID_NUMBER;
 
    char *endptr;
    const double value = strtod(input, &endptr);
 
    if (endptr == input || *endptr != '\0') return ERROR_INVALID_NUMBER;
    if (!(value > 0.0 && value < 1.0)) return ERROR_OUT_OF_DOMAIN;
 
    *result = value;
    return SUCCESS;
}
status_code limit_steps(const double eps, int *n) {
    if (n == NULL) return ERROR_NULL_POINTER;
 
    const double steps = ceil(2.0 / eps);
    if (steps > MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;
 
    *n = (int)steps;
    return SUCCESS;
}

status_code dichotomy(double (*f)(double), double a, double b, const double eps, double *result) {
    if (result == NULL || f == NULL) return ERROR_NULL_POINTER;
    if (a >= b) return ERROR_INVALID_ARGS;

    double fa = f(a);
    double fb = f(b);
    if (fabs(fa) < eps) { *result = a; return SUCCESS; }
    if (fabs(fb) < eps) { *result = b; return SUCCESS; }
    if ((fa < 0) == (fb < 0)) return ERROR_NO_ROOT;

    for (int i = 0; i < MAX_ITERATIONS; i++) {
        const double mid = a + (b - a) / 2;
        const double fm = f(mid);
        if ((b - a) / 2 < eps || fabs(fm) < eps) {
            *result = mid;
            return SUCCESS;
        }
        if ((fa < 0) == (fm < 0)) { a = mid; fa = fm; }
        else b = mid;
    }
    return ERROR_NO_CONVERGENCE;
}

double f_e(double x)     { return log(x) - 1.0; }
double f_pi(double x)    { return -cos(x / 2.0); }
double f_ln2(double x)   { return exp(x) - 2.0; }
double f_sqrt2(double x) { return x * x - 2.0; }

status_code calc_e_eq(const double eps, double *result) {
    return dichotomy(f_e, 2.0, 3.0, eps, result);
}
status_code calc_pi_eq(const double eps, double *result) {
    return dichotomy(f_pi, 3.0, 4.0, eps, result);
}
status_code calc_ln2_eq(const double eps, double *result) {
    return dichotomy(f_ln2, 0.0, 1.0, eps, result);
}
status_code calc_sqrt2_eq(const double eps, double *result) {
    return dichotomy(f_sqrt2, 1.0, 2.0, eps, result);
}

status_code calc_e_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;
 
    int n;
    const status_code status = limit_steps(eps, &n);
    if (status != SUCCESS) return status;
 
    *result = pow(1.0 + 1.0 / n, n);
    return SUCCESS;
}

status_code calc_e_series(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 1;
    double sum = 1.0;
    double term = 1.0;

    while (n < MAX_ITERATIONS && fabs(term) >= eps) {
        term /= n;
        sum += term;
        n++;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = sum;
    return SUCCESS;
}

status_code calc_pi_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n;
    const status_code status = limit_steps(eps, &n);
    if (status != SUCCESS) return status;

    double current = 4.0;
    for (int k = 2; k <= n; k++) {
        const double num = 4.0 * k * (k - 1.0);
        const double den = (2.0 * k - 1.0) * (2.0 * k - 1.0);
        current *= num / den;
    }

    *result = current;
    return SUCCESS;
}

status_code calc_pi_series(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 0;
    double sum = 0.0;
    double term = 1.0;
    int sign = 1;

    while (n < MAX_ITERATIONS && fabs(term) >= eps) {
        n++;
        term = 4.0 * sign / (2.0 * n - 1.0);
        sum += term;
        sign = -sign;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = sum;
    return SUCCESS;
}

/* ===================== ln 2 ===================== */

status_code calc_ln2_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;
 
    int n;
    const status_code status = limit_steps(eps, &n);
    if (status != SUCCESS) return status;
 
    *result = n * expm1(log(2.0) / n);
    return SUCCESS;
}

status_code calc_ln2_series(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 0;
    double sum = 0.0;
    double term = 1.0;
    int sign = 1;

    while (n < MAX_ITERATIONS && fabs(term) >= eps) {
        n++;
        term = (double)sign / n;
        sum += term;
        sign = -sign;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = sum;
    return SUCCESS;
}

status_code calc_sqrt2_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 0;
    double current = -0.5; 
    double prev = -1.5;    

    while (n < MAX_ITERATIONS && fabs(current - prev) > eps) {
        prev = current;
        n++;
        current = prev - (prev * prev) / 2.0 + 1.0;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = current;
    return SUCCESS;
}

status_code calc_sqrt2_row(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int k = 2;
    double prod = 1.0;
    double prev = 0.0;

    while (k < MAX_ITERATIONS && fabs(prod - prev) >= eps) {
        prev = prod;
        prod *= pow(2.0, pow(2.0, -(double)k));
        k++;
    }
    if (k >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = prod;
    return SUCCESS;
}

static double gamma_sum(const int m) {
    double binom = 1.0;    
    double log_fact = 0.0;
    double sum = 0.0;

    for (int k = 1; k <= m; k++) {
        binom *= (double)(m - k + 1) / k;
        log_fact += log((double)k);
        const double sign = (k % 2 == 0) ? 1.0 : -1.0;
        sum += sign * binom / k * log_fact;
    }
    return sum;
}

status_code calc_gamma_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    double prev = gamma_sum(1);
    for (int m = 2; m <= MAX_M_GAMMA; m *= 2) {
        const double current = gamma_sum(m);
        if (fabs(current - prev) < eps) {
            *result = current;
            return SUCCESS;
        }
        prev = current;
    }
    return ERROR_NO_CONVERGENCE;
}

status_code calc_gamma_row(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    const double pi = acos(-1.0);
    double sum = -(pi * pi) / 6.0;

    long long limit = (long long)(1.0 / (eps * eps));
    if (limit > MAX_ITERATIONS) limit = MAX_ITERATIONS;

    for (long long k = 2; k <= limit; k++) {
        const double root = floor(sqrt((double)k));
        const double term = 1.0 / (root * root) - 1.0 / (double)k;
        sum += term;
    }

    *result = sum;
    return SUCCESS;
}

int is_prime(const int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= n / i; i++)
        if (n % i == 0) return 0;
    return 1;
}

status_code calc_gamma_eq(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    double prod = 1.0; 
    double prev = 0.0;
    long long next_check = 10;

    for (long long t = 2; t <= MAX_ITERATIONS; t++) {
        if (is_prime(t)) {
            prod *= ((t - 1.0) / t);
        }

        if (t == next_check) {
            const double current = -log(log((double)t) * prod);

            if (t > 10 && fabs(current - prev) < eps) {
                *result = current;
                return SUCCESS;
            }
            prev = current;
            if (next_check > LLONG_MAX / 10) {
                break;
            }
            next_check *= 10;
        }
    }
    return ERROR_NO_CONVERGENCE;
}

