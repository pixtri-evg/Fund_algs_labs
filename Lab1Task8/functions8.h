#ifndef FUNCTIONS8_H
#define FUNCTIONS8_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_FILE_OPEN,
    ERROR_IO,
    ERROR_INVALID_TOKEN, 
    ERROR_OVERFLOW         
} status_code;

typedef struct {
    int is_negative;     
    const char *digits;     
    int min_base;        
    long long value;   
} number_info;

void print_error(const status_code code) {
    switch (code) {
        case ERROR_INVALID_ARGS:
            printf("Ошибка: неверное количество или значение аргументов командной строки\n");
            break;
        case ERROR_NULL_POINTER:
            printf("Ошибка: передан нулевой указатель\n");
            break;
        case ERROR_MEMORY:
            printf("Ошибка: не удалось выделить память\n");
            break;
        case ERROR_FILE_OPEN:
            printf("Ошибка: не удалось открыть файл\n");
            break;
        case ERROR_IO:
            printf("Ошибка: сбой чтения или записи файла\n");
            break;
        case ERROR_INVALID_TOKEN:
            printf("Ошибка: во входном файле найдена некорректная запись числа\n");
            break;
        case ERROR_OVERFLOW:
            printf("Ошибка: число слишком велико (переполнение long long)\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
}

int char_to_digit(const int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

int is_separator(const int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

status_code read_token(FILE *in, char **token) {
    if (in == NULL || token == NULL) return ERROR_NULL_POINTER;
    *token = NULL;

    int ch;
    do {
        ch = fgetc(in);
    } while (ch != EOF && is_separator(ch));
    if (ch == EOF) return ferror(in) ? ERROR_IO : SUCCESS;

    size_t capacity = 16;
    size_t len = 0;
    char *buff = (char*)malloc(capacity);
    if (buff == NULL) return ERROR_MEMORY;

    while (ch != EOF && !is_separator(ch)) {

        if (len + 1 >= capacity) {
            size_t new_capacity = capacity * 2;
            char *tmp = (char*)realloc(buff, new_capacity);
            if (tmp == NULL) {
                free(buff);
                return ERROR_MEMORY;
            }
            buff = tmp;
            capacity = new_capacity;
        }
        buff[len++] = (char)ch;
        ch = fgetc(in);
    }

    if (ferror(in)) {
        free(buff);
        return ERROR_IO;
    }

    buff[len] = '\0';
    *token = buff;
    return SUCCESS;
}

status_code parse_token(const char *token, number_info *info) {
    if (token == NULL || info == NULL) return ERROR_NULL_POINTER;

    const char *p = token;
    int is_negative = 0;

    if (*p == '-' || *p == '+') {
        is_negative = (*p == '-');
        p++;
    }
    if (*p == '\0') return ERROR_INVALID_TOKEN;

    int max_digit = 0;
    for (const char *q = p; *q != '\0'; q++) {
        int d = char_to_digit((unsigned char)*q);
        if (d < 0) return ERROR_INVALID_TOKEN;
        if (d > max_digit) max_digit = d;
    }

    int min_base = max_digit + 1;
    if (min_base < 2) min_base = 2;

    while (*p == '0' && *(p + 1) != '\0') p++;

    long long value = 0;
    for (const char *q = p; *q != '\0'; q++) {
        int d = char_to_digit((unsigned char)*q);
        if (value > (LLONG_MAX - d) / min_base) return ERROR_OVERFLOW;
        value = value * min_base + d;
    }

    if (value == 0) is_negative = 0;

    info->is_negative = is_negative;
    info->digits = p;
    info->min_base = min_base;
    info->value = is_negative ? -value : value;
    return SUCCESS;
}

status_code write_number_info(FILE *out, const number_info *info) {
    if (out == NULL || info == NULL) return ERROR_NULL_POINTER;

    if (info->is_negative && fputc('-', out) == EOF) return ERROR_IO;

    for (const char *p = info->digits; *p != '\0'; p++) {
        char c = *p;
        if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
        if (fputc(c, out) == EOF) return ERROR_IO;
    }

    if (fprintf(out, " %d %lld\n", info->min_base, info->value) < 0) return ERROR_IO;
    return SUCCESS;
}

status_code process_file(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return ERROR_NULL_POINTER;

    while (1) {
        char *token = NULL;
        status_code status = read_token(in, &token);
        if (status != SUCCESS) return status;
        if (token == NULL) break;

        number_info info;
        status = parse_token(token, &info);
        if (status == SUCCESS) {
            status = write_number_info(out, &info);
        }

        free(token);              
        if (status != SUCCESS) return status;
    }
    return SUCCESS;
}

#endif

