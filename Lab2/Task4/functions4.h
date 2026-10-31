#ifndef FUNCTIONS4_H
#define FUNCTIONS4_H

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 256

typedef enum {
    SUCCESS,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_READ,
    ERROR_INVALID_DATA,
    ERROR_DUPLICATE_NAME
} status_code;

static void print_error(const status_code code) {
    switch (code) {
    case ERROR_NULL_POINTER:
        fprintf(stderr, "Ошибка: передан нулевой указатель\n");
        break;
    case ERROR_MEMORY:
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        break;
    case ERROR_READ:
        fprintf(stderr, "Ошибка: не удалось прочитать данные\n");
        break;
    case ERROR_INVALID_DATA:
        fprintf(stderr, "Ошибка: входные данные не соответствуют формату\n");
        break;
    case ERROR_DUPLICATE_NAME:
        fprintf(stderr, "Ошибка: имена файлов должны быть уникальными\n");
        break;
    default:
        fprintf(stderr, "Неизвестная ошибка\n");
        break;
    }
}

typedef struct {
    char name[NAME_LEN];
    long long size;
    long long importance;
} FileInfo;

typedef struct {
    FileInfo *files;
    size_t count;
    long long total_size;
    long long total_importance;
} Selection;

static status_code parse_number(const char *s, long long *value) {
    if (s == NULL || value == NULL) return ERROR_NULL_POINTER;
    if (s[0] == '\0') return ERROR_INVALID_DATA;

    long long result = 0;
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') return ERROR_INVALID_DATA;
        int digit = s[i] - '0';
        if (result > (LLONG_MAX - digit) / 10) return ERROR_INVALID_DATA;
        result = result * 10 + digit;
    }
    *value = result;
    return SUCCESS;
}

static status_code read_file_info(FileInfo *f) {
    char name[NAME_LEN + 1], size[64], importance[64];

    if (scanf("%256s %63s %63s", name, size, importance) != 3) return ERROR_READ;
    if (strlen(name) >= NAME_LEN) return ERROR_INVALID_DATA;

    status_code st = parse_number(size, &f->size);
    if (st == SUCCESS) st = parse_number(importance, &f->importance);
    if (st == SUCCESS && f->size <= 0) st = ERROR_INVALID_DATA;
    if (st == SUCCESS) strcpy(f->name, name);
    return st;
}

static status_code read_input(FileInfo **files, size_t *count, long long *capacity) {
    if (files == NULL || count == NULL || capacity == NULL) return ERROR_NULL_POINTER;
    *files = NULL;
    *count = 0;

    char n_str[64], cap_str[64];
    long long n = 0;
    if (scanf("%63s %63s", n_str, cap_str) != 2) return ERROR_READ;

    status_code st = parse_number(n_str, &n);
    if (st == SUCCESS) st = parse_number(cap_str, capacity);
    if (st != SUCCESS) return st;
    if (n <= 0) return ERROR_INVALID_DATA;
    if ((unsigned long long)n > SIZE_MAX / sizeof(FileInfo)) return ERROR_MEMORY;

    FileInfo *array = calloc((size_t)n, sizeof(FileInfo));
    if (array == NULL) return ERROR_MEMORY;

    for (size_t i = 0; i < (size_t)n && st == SUCCESS; i++) {
        st = read_file_info(&array[i]);
        for (size_t j = 0; j < i && st == SUCCESS; j++) {
            if (strcmp(array[j].name, array[i].name) == 0) st = ERROR_DUPLICATE_NAME;
        }
    }

    if (st != SUCCESS) {
        free(array);
        return st;
    }
    *files = array;
    *count = (size_t)n;
    return SUCCESS;
}

static status_code select_files(const FileInfo *files, size_t n, long long capacity, Selection *res) {
    if (files == NULL || res == NULL) return ERROR_NULL_POINTER;
    if (n == 0 || capacity < 0) return ERROR_INVALID_DATA;

    res->files = NULL;
    res->count = 0;
    res->total_size = 0;
    res->total_importance = 0;

    long long sum_size = 0, sum_importance = 0;
    for (int i = 0; i < n; i++) {
        if (files[i].size > LLONG_MAX - sum_size ||
            files[i].importance > LLONG_MAX - sum_importance) {
            return ERROR_INVALID_DATA;
        }
        sum_size += files[i].size;
        sum_importance += files[i].importance;
    }

    if (capacity > sum_size) capacity = sum_size;
    if ((unsigned long long)capacity >= SIZE_MAX) return ERROR_MEMORY;
    long long cols = capacity + 1;
    if (n > SIZE_MAX / cols) return ERROR_MEMORY;

    long long *dp =(long long *) calloc(cols, sizeof(long long));
    char *keep = calloc(n * cols, sizeof(char));
    char *chosen = calloc(n, sizeof(char));
    res->files = malloc(n * sizeof(FileInfo));
    if (dp == NULL || keep == NULL || chosen == NULL || res->files == NULL) {
        free(dp);
        free(keep);
        free(chosen);
        free(res->files);
        res->files = NULL;
        return ERROR_MEMORY;
    }

    for (int i = 0; i < n; i++) {
        for (long long w = capacity; w >= files[i].size; w--) {
            long long with = dp[w - files[i].size] + files[i].importance;
            if (with > dp[w]) {
                dp[w] = with;
                keep[i * cols + w] = 1;
            }
        }
    }

    long long w = capacity;
    for (size_t i = n; i > 0; i--) {
        if (keep[(i - 1) * cols + (size_t)w]) {
            chosen[i - 1] = 1;
            w -= files[i - 1].size;
        }
    }

    for (size_t i = 0; i < n; i++) {
        if (chosen[i]) {
            res->files[res->count++] = files[i];
            res->total_size += files[i].size;
            res->total_importance += files[i].importance;
        }
    }

    free(dp);
    free(keep);
    free(chosen);
    return SUCCESS;
}

static void print_selection(const Selection *res) {
    printf("TOTAL_SIZE %lld\n", res->total_size);
    printf("TOTAL_IMPORTANCE %lld\n", res->total_importance);
    printf("FILES %zu\n", res->count);
    for (size_t i = 0; i < res->count; i++) {
        printf("%s\n", res->files[i].name);
    }
}

#endif