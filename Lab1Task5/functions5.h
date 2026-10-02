#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
#define MAX_ITERATIONS 100000     
#define MAX_INTEGRATION_STEPS 24  
#define X 0.5                 

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_NUMBER,
    ERROR_NULL_POINTER,
    ERROR_OUT_OF_DOMAIN,
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

typedef double (*term_next_func)(double prev_term, int n);
 
double next_a(const double prev, const int n) {
    return prev * X / n;
}
double next_b(const double prev, const int n) {
    return prev * (-(X * X)) / ((2.0 * n - 1.0) * (2.0 * n));
}
double next_c(const double prev, const int n) {
    return prev * 27.0 * n * n * n * X * X / ((3.0 * n) * (3.0 * n - 1.0) * (3.0 * n - 2.0));
}
double next_d(const double prev, const int n) {
    return prev * (-(2.0 * n - 1.0) * X * X) / (2.0 * n);
}

status_code sum_series(const term_next_func next_term, const int first_n,
                       const double eps, double *result) {
    if (next_term == NULL || result == NULL) return ERROR_NULL_POINTER;
    if (first_n != 0 && first_n != 1) return ERROR_OUT_OF_DOMAIN;

    double term = 1.0;                 
    double sum = (first_n == 0) ? term : 0.0;          
    for (int n = 1; n <= MAX_ITERATIONS; n++) {
        term = next_term(term, n);
        if (!isfinite(term)) return ERROR_OVERFLOW;
        sum += term;
        if (fabs(term) < eps) {
            *result = sum;
            return SUCCESS;
        }
    }
    return ERROR_NO_CONVERGENCE;
}

typedef double (*real_func)(double x);
 
double f_i1(const double x) { return log1p(x) / x; }    
double f_i2(const double x) { return exp(-x * x / 2.0); }    
double f_i3(const double x) { return -log(1.0 - x); }       
double f_i4(const double x) { return pow(x, x); }      

double midpoint_sum(const real_func f, const int n) {
    const double h = 1.0 / n;
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += f((i + 0.5) * h);
    return sum * h;
}

status_code integrate(const real_func f, const double eps, double *result) {
    if (f == NULL || result == NULL) return ERROR_NULL_POINTER;
 
    int n = 1;
    double prev = midpoint_sum(f, n);
 
    for (int step = 0; step < MAX_INTEGRATION_STEPS; step++) {
        n *= 2;
        const double current = midpoint_sum(f, n);
        if (!isfinite(current)) return ERROR_OVERFLOW;
        if (fabs(current - prev) < eps) {
            *result = current;
            return SUCCESS;
        }
        prev = current;
    }
    return ERROR_NO_CONVERGENCE;
}


 