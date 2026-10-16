#ifndef FUNCTIONS1_H
#define FUNCTIONS1_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_FLAG,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_INVALID_NUMBER
} status_code;

void print_error(const status_code code) {
    switch (code) {
        case ERROR_INVALID_ARGS:
            printf("Ошибка: неверное количество или значение аргументов командной строки\n");
            break;
        case ERROR_INVALID_FLAG:
            printf("Ошибка: некорректный флаг. Допустимы: -l, -r, -u, -n, -c\n");
            break;
        case ERROR_NULL_POINTER:
            printf("Ошибка: передан нулевой указатель\n");
            break;
        case ERROR_MEMORY:
            printf("Ошибка: не удалось выделить память\n");
            break;
        case ERROR_INVALID_NUMBER:
            printf("Ошибка: seed должен быть целым неотрицательным числом типа unsigned int\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
}

status_code parse_flag(const char *input, char *action) {
    if (input == NULL || action == NULL) return ERROR_NULL_POINTER;
    if (input[0] != '-') return ERROR_INVALID_FLAG;
    if (input[1] != 'l' && input[1] != 'r' && input[1] != 'u' &&
        input[1] != 'n' && input[1] != 'c') return ERROR_INVALID_FLAG;
    if (input[2] != '\0') return ERROR_INVALID_FLAG;
    *action = input[1];
    return SUCCESS;
}

status_code my_strlen(const char *str, size_t *length) {
    if (str == NULL || length == NULL) return ERROR_NULL_POINTER;
    size_t count = 0;
    while (str[count] != '\0') count++;
    *length = count;
    return SUCCESS;
}

status_code parse_seed(const char *input, unsigned int *seed) {
    if (input == NULL || seed == NULL) return ERROR_NULL_POINTER;
    if (input[0] == '\0') return ERROR_INVALID_NUMBER;

    unsigned int result = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (input[i] < '0' || input[i] > '9') return ERROR_INVALID_NUMBER;
        unsigned int digit = input[i] - '0';
        if (result > (UINT_MAX - digit) / 10) return ERROR_INVALID_NUMBER;
        result = result * 10 + digit;
    }
    *seed = result;
    return SUCCESS;
}

status_code get_length(const char *str, size_t *length) {
    return my_strlen(str, length);
}

status_code reverse_string(const char *str, char **result) {
    if (str == NULL || result == NULL) return ERROR_NULL_POINTER;
    *result = NULL;

    size_t length = 0;
    status_code status = my_strlen(str, &length);
    if (status != SUCCESS) return status;

    char *buff = (char*)malloc(length + 1);
    if (buff == NULL) return ERROR_MEMORY;

    for (size_t i = 0; i < length; i++) {
        buff[i] = str[length - 1 - i];
    }
    buff[length] = '\0';
    *result = buff;
    return SUCCESS;
}

status_code upper_odd_string(const char *str, char **result) {
    if (str == NULL || result == NULL) return ERROR_NULL_POINTER;
    *result = NULL;

    size_t length = 0;
    status_code status = my_strlen(str, &length);
    if (status != SUCCESS) return status;

    char *buff = (char*)malloc(length + 1);
    if (buff == NULL) return ERROR_MEMORY;

    for (size_t i = 0; i < length; i++) {
        char c = str[i];
        if (i % 2 == 1 && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        buff[i] = c;
    }
    buff[length] = '\0';
    *result = buff;
    return SUCCESS;
}

int is_digit_char(const char c) {
    return c >= '0' && c <= '9';
}

int is_letter_char(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

status_code sort_chars_string(const char *str, char **result) {
    if (str == NULL || result == NULL) return ERROR_NULL_POINTER;
    *result = NULL;

    size_t length = 0;
    status_code status = my_strlen(str, &length);
    if (status != SUCCESS) return status;

    char *buff = (char*)malloc(length + 1);
    if (buff == NULL) return ERROR_MEMORY;
    size_t pos = 0; 

    for (size_t i = 0; i < length; i++) {
        if (is_digit_char(str[i])) buff[pos++] = str[i];
    }

    for (size_t i = 0; i < length; i++) {
        if (is_letter_char(str[i])) buff[pos++] = str[i];
    }

    for (size_t i = 0; i < length; i++) {
        if (!is_digit_char(str[i]) && !is_letter_char(str[i])) buff[pos++] = str[i];
    }
    buff[pos] = '\0';
    *result = buff;
    return SUCCESS;
}

status_code concat_random(const char *const *strings, const size_t count,
                          const unsigned int seed, char **result) {
    if (strings == NULL || result == NULL) return ERROR_NULL_POINTER;
    *result = NULL;
    if (count == 0) return ERROR_INVALID_ARGS;

    const char **pool = (const char**)malloc(count * sizeof(const char*));
    if (pool == NULL) return ERROR_MEMORY;

    size_t total = 0;
    for (size_t i = 0; i < count; i++) {
        if (strings[i] == NULL) {
            free(pool);
            return ERROR_NULL_POINTER;
        }
        size_t len = 0;
        status_code status = my_strlen(strings[i], &len);
        if (status != SUCCESS) {
            free(pool);
            return status;
        }
        total += len;
        pool[i] = strings[i];
    }

    char *buff = (char*)malloc(total + 1);
    if (buff == NULL) {
        free(pool);
        return ERROR_MEMORY;
    }

    srand(seed);
    size_t remaining = count; 
    size_t pos = 0;          
    while (remaining > 0) {
        size_t index = rand() % remaining; 
        const char *chosen = pool[index];

        for (size_t j = 0; chosen[j] != '\0'; j++) {
            buff[pos++] = chosen[j];
        }

        pool[index] = pool[remaining - 1];
        remaining--;
    }
    buff[pos] = '\0';
    free(pool);
    *result = buff;
    return SUCCESS;
}
#endif