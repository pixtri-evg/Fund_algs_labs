#ifndef FUNCTIONS10_H
#define FUNCTIONS10_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef enum {
    SUCCESS = 0,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_IO,
    ERROR_INVALID_BASE,   
    ERROR_INVALID_TOKEN,   
    ERROR_OVERFLOW,       
    ERROR_NO_NUMBERS,    
    ERROR_NO_STOP      
} status_code;

void print_error(const status_code code) {
    switch (code) {
        case ERROR_NULL_POINTER:
            printf("Ошибка: передан нулевой указатель\n");
            break;
        case ERROR_MEMORY:
            printf("Ошибка: не удалось выделить память\n");
            break;
        case ERROR_IO:
            printf("Ошибка: сбой чтения или записи\n");
            break;
        case ERROR_INVALID_BASE:
            printf("Ошибка: основание должно быть целым числом от 2 до 36\n");
            break;
        case ERROR_INVALID_TOKEN:
            printf("Ошибка: некорректная запись числа для заданного основания\n");
            break;
        case ERROR_OVERFLOW:
            printf("Ошибка: переполнение long long\n");
            break;
        case ERROR_NO_NUMBERS:
            printf("Ошибка: не введено ни одного числа\n");
            break;
        case ERROR_NO_STOP:
            printf("Ошибка: ввод закончился без строки Stop\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
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

int input_digit_value(const int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

status_code parse_base(const char *token, int *base) {
    if (token == NULL || base == NULL) return ERROR_NULL_POINTER;
    if (*token == '\0') return ERROR_INVALID_BASE;

    int value = 0;
    for (const char *p = token; *p != '\0'; p++) {
        if (*p < '0' || *p > '9') return ERROR_INVALID_BASE;
        value = value * 10 + (*p - '0');
        if (value > 36) return ERROR_INVALID_BASE;
    }
    if (value < 2) return ERROR_INVALID_BASE;

    *base = value;
    return SUCCESS;
}

status_code parse_number(const char *token, const int base, long long *value) {
    if (token == NULL || value == NULL) return ERROR_NULL_POINTER;

    const char *p = token;
    int is_negative = 0;
    if (*p == '-' || *p == '+') {
        is_negative = (*p == '-');
        p++;
    }
    if (*p == '\0') return ERROR_INVALID_TOKEN;

    long long result = 0;
    for (; *p != '\0'; p++) {
        int d = input_digit_value((unsigned char)*p);
        if (d < 0 || d >= base) return ERROR_INVALID_TOKEN;
        if (result > (LLONG_MAX - d) / base) return ERROR_OVERFLOW;
        result = result * base + d;
    }

    *value = is_negative ? -result : result;
    return SUCCESS;
}

status_code add_checked(const long long a, const long long b, long long *result) {
    if (result == NULL) return ERROR_NULL_POINTER;
    if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b)) {
        return ERROR_OVERFLOW;
    }
    *result = a + b;
    return SUCCESS;
}

status_code read_numbers(FILE *in, const int base, long long *sum, long long *max_abs) {
    if (in == NULL || sum == NULL || max_abs == NULL) return ERROR_NULL_POINTER;

    long long total = 0;
    long long best = 0;
    size_t count = 0;

    while (1) {
        char *token = NULL;
        status_code status = read_token(in, &token);
        if (status != SUCCESS) return status;
        if (token == NULL) return ERROR_NO_STOP;

        if (strcmp(token, "Stop") == 0) {
            free(token);
            break;
        }

        long long value = 0;
        status = parse_number(token, base, &value);
        free(token);  
        if (status != SUCCESS) return status;

        status = add_checked(total, value, &total);
        if (status != SUCCESS) return status;

        long long abs_value = value < 0 ? -value : value;
        long long abs_best = best < 0 ? -best : best;
        if (count == 0 || abs_value > abs_best) best = value;
        count++;
    }

    if (count == 0) return ERROR_NO_NUMBERS;

    *sum = total;
    *max_abs = best;
    return SUCCESS;
}

status_code to_base_string(const long long value, const int base, char *buffer) {
    if (buffer == NULL) return ERROR_NULL_POINTER;
    if (base < 2 || base > 36) return ERROR_INVALID_BASE;

    unsigned long long number = (value < 0) ? -value : value;
    char reversed[70];
    int count = 0;
    do {
        int remainder = number % base;

        if (remainder < 10) reversed[count] = (char)('0' + remainder);
        else reversed[count] = (char)('A' + remainder - 10);

        count++;
        number = number / base;
    } while (number > 0);

    int pos = 0;
    if (value < 0) {
        buffer[pos] = '-';
        pos++;
    }
    for (int i = count - 1; i >= 0; i--) {
        buffer[pos] = reversed[i];
        pos++;
    }
    buffer[pos] = '\0';

    return SUCCESS;
}

status_code print_in_bases(const char *label, const long long value, const int source_base) {
    if (label == NULL) return ERROR_NULL_POINTER;

    const int bases[] = {source_base, 9, 18, 27, 36};
    const size_t bases_count = sizeof(bases) / sizeof(bases[0]);

    printf("%s\n", label);
    for (size_t i = 0; i < bases_count; i++) {
        char text[70];
        status_code status = to_base_string(value, bases[i], text);
        if (status != SUCCESS) return status;

        if (i == 0) printf("  base %d (исходная): %s\n", bases[i], text);
        else        printf("  base %d: %s\n", bases[i], text);
    }
    return SUCCESS;
}

#endif