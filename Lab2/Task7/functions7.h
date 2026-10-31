#ifndef FUNCTIONS7_H
#define FUNCTIONS7_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

typedef enum {
    SUCCESS = 0,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_INVALID_DIMENSION,
    ERROR_INVALID_COUNT,
    ERROR_INVALID_EPSILON,
    ERROR_INVALID_NORM_PARAM
} status_code;

void print_error(const status_code code) {
    switch (code) {
    case ERROR_NULL_POINTER:
        printf("Ошибка: передан нулевой указатель\n");
        break;
    case ERROR_MEMORY:
        printf("Ошибка: не удалось выделить память\n");
        break;
    case ERROR_INVALID_DIMENSION:
        printf("Ошибка: размерность пространства должна быть больше нуля\n");
        break;
    case ERROR_INVALID_COUNT:
        printf("Ошибка: количество векторов и норм должно быть больше нуля\n");
        break;
    case ERROR_INVALID_EPSILON:
        printf("Ошибка: эпсилон должен быть положительным\n");
        break;
    case ERROR_INVALID_NORM_PARAM:
        printf("Ошибка: недопустимый параметр нормы (p < 1 или матрица не положительно определена)\n");
        break;
    default:
        printf("Неизвестная ошибка\n");
        break;
    }
}

typedef struct { double *coords; } vector_t;

typedef struct {
    const vector_t **items;
    size_t count;
} longest_t;

typedef status_code (*norm_func_t)(const vector_t *vec, size_t n, const void *params,
                                   double epsilon, double *result);

status_code create_vector(const size_t n, const double *values, vector_t *vec) {
    if (values == NULL || vec == NULL) return ERROR_NULL_POINTER;
    vec->coords = NULL;
    if (n == 0) return ERROR_INVALID_DIMENSION;

    vec->coords = (double *)malloc(n * sizeof(double));
    if (vec->coords == NULL) return ERROR_MEMORY;

    memcpy(vec->coords, values, n * sizeof(double));
    return SUCCESS;
}

void free_vector(vector_t *vec) {
    if (vec == NULL) return;
    free(vec->coords);
    vec->coords = NULL;
}

status_code norm_inf(const vector_t *vec, const size_t n, const void *params,
                     const double epsilon, double *result) {
    (void)params;
    if (vec == NULL || vec->coords == NULL || result == NULL) return ERROR_NULL_POINTER;
    if (n == 0) return ERROR_INVALID_DIMENSION;

    double max = fabs(vec->coords[0]);
    for (size_t i = 1; i < n; i++) {
        const double value = fabs(vec->coords[i]);
        if (value - max > epsilon) max = value;
    }
    *result = max;
    return SUCCESS;
}

status_code norm_p(const vector_t *vec, const size_t n, const void *params,
                   const double epsilon, double *result) {
    if (vec == NULL || vec->coords == NULL || params == NULL || result == NULL) return ERROR_NULL_POINTER;
    if (n == 0) return ERROR_INVALID_DIMENSION;

    const double p = *(const double *)params;
    if (p < 1.0 - epsilon) return ERROR_INVALID_NORM_PARAM;

    double sum = 0.0;
    for (size_t i = 0; i < n; i++)
        sum += pow(fabs(vec->coords[i]), p);

    *result = pow(sum, 1.0 / p);
    return SUCCESS;
}

status_code norm_a(const vector_t *vec, const size_t n, const void *params,
                   const double epsilon, double *result) {
    if (vec == NULL || vec->coords == NULL || params == NULL || result == NULL) return ERROR_NULL_POINTER;
    if (n == 0) return ERROR_INVALID_DIMENSION;

    const double *a = (const double *)params;
    double sum = 0.0;

    for (size_t i = 0; i < n; i++) {
        double row = 0.0;
        for (size_t j = 0; j < n; j++)
            row += a[i * n + j] * vec->coords[j];
        sum += row * vec->coords[i];
    }

    if (sum < -epsilon) return ERROR_INVALID_NORM_PARAM;
    if (sum < 0.0) sum = 0.0;

    *result = sqrt(sum);
    return SUCCESS;
}

void free_results(longest_t *results, const size_t norms_count) {
    if (results == NULL) return;
    for (size_t i = 0; i < norms_count; i++) {
        free(results[i].items);
        results[i].count = 0;
        results[i].items = NULL;
    }
}

status_code find_longest_vectors(const size_t n, const size_t vectors_count,
                                 const size_t norms_count, const double epsilon,
                                 vector_t *storage, longest_t *results, ...) {

    if (storage == NULL || results == NULL) return ERROR_NULL_POINTER;
    if (n == 0) return ERROR_INVALID_DIMENSION;
    if (vectors_count == 0 || norms_count == 0) return ERROR_INVALID_COUNT;
    if (epsilon <= 0.0) return ERROR_INVALID_EPSILON;

    double *values = (double *)malloc(vectors_count * sizeof(double));
    if (values == NULL) return ERROR_MEMORY;

    va_list args;
    va_start(args, results);
    status_code status = SUCCESS;

    for (size_t i = 0; i < vectors_count; i++) {
        storage[i] = va_arg(args, vector_t);
        if (storage[i].coords == NULL) {
            status = ERROR_NULL_POINTER;
            break;
        }
    }

    for (size_t k = 0; k < norms_count && status == SUCCESS; k++) {
        const norm_func_t norm = va_arg(args, norm_func_t);
        const void *params = va_arg(args, const void *);

        if (norm == NULL) {
            status = ERROR_NULL_POINTER;
            break;
        }
        results[k].items = (const vector_t **)malloc(vectors_count * sizeof(const vector_t *));
        if (results[k].items == NULL) {
            status = ERROR_MEMORY;
            break;
        }
        results[k].count = 0;

        for (size_t i = 0; i < vectors_count; i++) {
            status = norm(&storage[i], n, params, epsilon, &values[i]);
            if (status != SUCCESS) break;
        }
        if (status != SUCCESS) break;

        double max_val = values[0];
        for (size_t i = 1; i < vectors_count; i++)
            if (values[i] - max_val > epsilon) max_val = values[i];

        for (size_t i = 0; i < vectors_count; i++)
            if (fabs(values[i] - max_val) <= epsilon) {
                results[k].items[results[k].count] = &storage[i];
                results[k].count++;
            }
    }
    va_end(args);
    free(values);
    if (status != SUCCESS) free_results(results, norms_count);
    return status;
}

void print_longest(const status_code status, const size_t n,
                   const size_t norms_count, const longest_t *results) {
    if (status != SUCCESS) {
        print_error(status);
        return;
    }
    for (size_t k = 0; k < norms_count; k++) {
        printf("Норма №%zu, самых длинных векторов: %zu\n", k + 1, results[k].count);
        for (size_t j = 0; j < results[k].count; j++) {
            printf("  (");
            for (size_t i = 0; i < n; i++) {
                if (i > 0) printf(", ");
                printf("%g", results[k].items[j]->coords[i]);
            }
            printf(")\n");
        }
    }
}

#endif