#ifndef FUNCTIONS10_H
#define FUNCTIONS10_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

typedef enum {
    SUCCESS = 0,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_INVALID_EPSILON,
    ERROR_INVALID_DEGREE
} status_code;

void print_error(const status_code code) {
    switch (code) {
    case ERROR_NULL_POINTER:
        printf("Ошибка: передан нулевой указатель\n");
        break;
    case ERROR_MEMORY:
        printf("Ошибка: не удалось выделить память\n");
        break;
    case ERROR_INVALID_EPSILON:
        printf("Ошибка: эпсилон должен быть положительным числом\n");
        break;
    case ERROR_INVALID_DEGREE:
        printf("Ошибка: степень многочлена должна быть неотрицательной\n");
        break;
    default:
        printf("Неизвестная ошибка\n");
        break;
    }
}

void horner_shift(double *c, const int n, const double a) {
    for (int i = 0; i < n; i++) {
        for (int j = n - 1; j >= i; j--) {
            c[j] += a * c[j + 1];
        }
    }
}

status_code shift_poly(const double eps, const double a, double **result, const int n, ...) {
    if (result == NULL) return ERROR_NULL_POINTER;
    *result = NULL;
    if (eps <= 0) return ERROR_INVALID_EPSILON;
    if (n < 0) return ERROR_INVALID_DEGREE;

    double *c = malloc((n + 1) * sizeof(double));
    if (c == NULL) return ERROR_MEMORY;

    va_list args;
    va_start(args, n);
    for (int i = 0; i <= n; i++) {
        c[i] = va_arg(args, double);
    }
    va_end(args);

    horner_shift(c, n, a);

    for (int i = 0; i <= n; i++) {
        if (fabs(c[i]) < eps) c[i] = 0;
    }

    *result = c;
    return SUCCESS;
}

void print_poly(const status_code status, const double *c, const int n, const double a) {
    if (status != SUCCESS) {
        print_error(status);
        return;
    }

    for (int k = 0; k <= n; k++) {
        printf("%+g", c[k]);
        if (k >= 1) printf("(x%+g)", -a);
        if (k >= 2) printf("^%d", k);
        printf(" ");
    }
    printf("\n");
}

#endif