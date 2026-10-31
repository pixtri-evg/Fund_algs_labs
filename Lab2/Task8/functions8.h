#ifndef FUNCTIONS8_H
#define FUNCTIONS8_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
    SUCCESS = 0,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_INVALID_BASE,
    ERROR_INVALID_DATA
} status_code;

void print_error(const status_code code) {
    switch (code) {
    case ERROR_NULL_POINTER:
        printf("Ошибка: передан нулевой указатель\n");
        break;
    case ERROR_MEMORY:
        printf("Ошибка: не удалось выделить память\n");
        break;
    case ERROR_INVALID_BASE:
        printf("Ошибка: основание системы счисления должно быть в диапазоне [2..36]\n");
        break;
    case ERROR_INVALID_DATA:
        printf("Ошибка: число пустое или содержит недопустимые символы\n");
        break;
    default:
        printf("Неизвестная ошибка\n");
        break;
    }
}

int char_to_digit(const char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    return -1;
}

char digit_to_char(const int d) {
    if (d < 10) return '0' + d;
    return 'A' + d - 10;
}

int is_valid_number(const char *str, const int base) {
    if (str == NULL || str[0] == '\0') return 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        const int digit = char_to_digit(str[i]);
        if (digit < 0 || digit >= base) return 0;
    }
    return 1;
}

void reverse_string(char *str, const size_t length) {
    for (size_t i = 0; i < length / 2; i++) {
        char tmp = str[i];
        str[i] = str[length - 1 - i];
        str[length - 1 - i] = tmp;
    }
}

status_code add_in_column(const char *a, const char *b, const int base, char **result) {
    if (a == NULL || b == NULL || result == NULL) return ERROR_NULL_POINTER;
    *result = NULL;
    if (!is_valid_number(a, base) || !is_valid_number(b, base)) return ERROR_INVALID_DATA;
    if (base < 2 || base > 36) return ERROR_INVALID_BASE;

    int a_left = strlen(a);
    int b_left = strlen(b);
    const int max_len = a_left > b_left ? a_left : b_left;

    char *buffer = (char *)malloc(max_len + 2);
    if (buffer == NULL) return ERROR_MEMORY;

    int len = 0;
    int carry = 0;

    while (a_left > 0 || b_left > 0 || carry != 0) {
        int sum = carry;

        if (a_left > 0) {
            sum += char_to_digit(a[a_left - 1]);
            a_left--;
        }
        if (b_left > 0) {
         sum += char_to_digit(b[b_left - 1]);
            b_left--;
        }
        buffer[len++] = digit_to_char(sum % base);
        carry = sum / base;
    }



    while (len > 1 && buffer[len - 1] == '0') len--;
    buffer[len] = '\0';

    reverse_string(buffer, len);
    *result = buffer;
    return SUCCESS;

}

status_code sum_in_base(char **result, const int base, const int count, ...) {
    if (result == NULL) return ERROR_NULL_POINTER;
    if (count <= 0)
    {
        *result = strdup("0");
        return (*result ==NULL) ? ERROR_MEMORY : SUCCESS;
    }
    *result = NULL;
    if (base < 2 || base > 36) return ERROR_INVALID_BASE;

    char *sum = NULL;

    va_list args;
    va_start(args, count);

    status_code status = SUCCESS;

    for (int i = 0; i < count; i++) {
        const char *number = va_arg(args, const char*);
        char *new_sum = NULL;

        if (i == 0) new_sum = strdup(number);
        else {
            status = add_in_column(sum, number, base, &new_sum);
            free(sum);
        }
        sum = new_sum;

    }
    va_end(args);
    if (status != SUCCESS) {
        free(sum);
        return status;
    }
    *result = sum;
    return SUCCESS;
}

void print_sum(const status_code status, const char *result) {
    if (status != SUCCESS) {
        print_error(status);
        return;
    }
    printf("Сумма: %s\n", result);
}

#endif
