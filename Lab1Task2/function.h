#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ITERATIONS 1000000

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGUMENT,
    ERROR_NO_CONVERGENCE,        // превышено число итераций
    ERROR_NULL_POINTER,
    ERROR_OVERFLOW
} status_code;

status_code validate_eps(const char *input, double *result) {
    if (input == NULL || result == NULL)
        return ERROR_NULL_POINTER;

    char *endptr;
    double value = strtod(input, &endptr);

    if (*endptr != '\0' || endptr == input)
        return ERROR_INVALID_ARGUMENT;

    if (!isfinite(value) || value <= 0.0 || value >= 1.0)
        return ERROR_INVALID_ARGUMENT;

    *result = value;
    return SUCCESS;
}

status_code calc_lim(double (*f)(double), const double eps, double *result) {
    if (result == NULL)
        return ERROR_NULL_POINTER;

    int n = 1;
    double prev = f(0);
    double curr = f(1);

    while (n < MAX_ITERATIONS && fabs(curr - prev) >= eps) {
        prev = curr;
        n++;
        curr = f(n);
    }

    if (n >= MAX_ITERATIONS)
        return ERROR_NO_CONVERGENCE;
    
    *result = curr;
    return SUCCESS;
}

status_code calc_series(const double eps, double first_term, double (*f)(int, double), double *result) {
    if (result == NULL)
        return ERROR_NULL_POINTER;

    int n = 1;
    double sum = first_term;
    double term = f(n, first_term);

    while (n < MAX_ITERATIONS && fabs(term) >= eps) {
        sum += term;
        n++;
        term = f(n, term);
    }

    if (n >= MAX_ITERATIONS)
        return ERROR_NO_CONVERGENCE;
    
    *result = sum;
    return SUCCESS;
}

status_code calc_equation(double a, double b, const double eps, double (*f)(double), double *result) {
    if (result == NULL)
        return ERROR_NULL_POINTER;
    
    double mid;
    while ((b - a) > eps) {
        mid = a + (b - a) / 2.0;
        if (f(a) * f(mid) > 0)
            a = mid;
        else
            b = mid;
    }
    *result = a + (b - a) / 2.0;
    return SUCCESS;

}

double e_limit_term(int n) {
    return pow(1.0 + 1.0 / n, n);
}

double e_series_term(int n, double prev_term) {
    return prev_term / n;
} 

double e_equation(double x) {
    return log(x) - 1.0;
}

double pi_limit(int n) {
    double current = current * (4.0 * n * (n - 1)) / ((2.0 * n - 1) * (2.0 * n - 1));
}

double pi_series(int n, )