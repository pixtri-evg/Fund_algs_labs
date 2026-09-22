#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_FLAG,
    ERROR_INVALID_NUMBER,
    ERROR_NULL_POINTER,
    ERROR_OUT_OF_DOMAIN,
    ERROR_OVERFLOW
} status_code;


status_code string_to_int(const char *input, int *result) {
    if (input == NULL || result == NULL)
        return ERROR_NULL_POINTER;

    if (*input == '\0')
        return ERROR_INVALID_NUMBER;

    int sign = 1;
    char *ptr = input;

    if (*ptr == '-') {
        sign = -1;
        ptr++;
    }
    else if (*ptr == '+') {
        sign = 1;
        ptr++;
    }

    int value = 0;

    if (*ptr == '\0') 
        return ERROR_INVALID_NUMBER;

    while (*ptr != '\0') {

        if (!isdigit((unsigned char)*ptr))
            return ERROR_INVALID_NUMBER;

        int digit = *ptr - '0';

        if (sign == 1) {
            if (value > INT_MAX / 10 || (value == INT_MAX / 10 && digit > INT_MAX % 10))
                return ERROR_OVERFLOW;

            value = value * 10 + digit;
        }

        else {
            if (value < INT_MIN / 10 || (value == INT_MIN / 10 && -digit < INT_MIN % 10))
                return ERROR_OVERFLOW;

            value = value * 10 - digit;
        }
        ptr++;
    }
    *result = value;
    return SUCCESS;
}

status_code parse_flag(const char *input, char *flag) {
    if (input == NULL || flag == NULL)
        return ERROR_NULL_POINTER;

    if ((input[0] != '-' && input[0] != '/') || input[1] == '\0' || input[2] != '\0') {
        return ERROR_INVALID_FLAG;
    }

    char c = input[1];
    if (c == 'h' || c == 'p' || c == 's' || c == 'e' || c == 'a' || c == 'f') {
        *flag = c;
        return SUCCESS;
    }

    return ERROR_INVALID_FLAG;
}


status_code find_multiplies(const int x, int *out_array, int *count) {
    if (out_array == NULL || count == NULL)
        return ERROR_NULL_POINTER;

    if (x > 100 || x <= 0)
        return ERROR_OUT_OF_DOMAIN;
    
    *count = 0;

    for (int i = x; i <= 100; i += x) {
        out_array[*count] = i;
        (*count)++;
    }

    return SUCCESS;
}


status_code check_prime(const int x, int *is_prime, int *is_comp) {
    if (is_prime == NULL || is_comp == NULL)
        return ERROR_NULL_POINTER;

    *is_prime = 0;
    *is_comp = 0;

    if (x <= 1)
        return SUCCESS;

    for (int i = 2; i <= x / i; i++) {
        if (x % i == 0) {
            *is_comp = 1;
            return SUCCESS;
        }   
    }

    *is_prime = 1;
    return SUCCESS;
    
}

status_code calculate_sum(const int x, long long *sum_answer) {
    if (sum_answer == NULL)
        return ERROR_NULL_POINTER;

    if (x < 1)
        return ERROR_OUT_OF_DOMAIN;
        
    long long n = x;
    *sum_answer = (n * (n + 1)) / 2;

    return SUCCESS;
}

status_code calculate_factorial(const int x, unsigned long long *out_fact) {
    if (out_fact == NULL)
        return ERROR_NULL_POINTER;
    if (x < 0)
        return ERROR_OUT_OF_DOMAIN;

    unsigned long long fact = 1;
    for (int i = 1; i <= x; i++) {
        if (fact > ULLONG_MAX / i)
            return ERROR_OVERFLOW;

        fact *= i;
    }
    *out_fact = fact;
    return SUCCESS;
}

status_code split_into_h(const int x, char *hex_digits, size_t hex_size, int *start_index) {
    if (hex_digits == NULL || start_index == NULL)
        return ERROR_NULL_POINTER;
    
    if (hex_size < 2)
        return ERROR_OUT_OF_DOMAIN;

    unsigned int n = (x > 0) ? (unsigned int)x : -(unsigned int)x;     // чтобы избежать преполнения INT_MIN
    int base = 16;
    

    char *pres = hex_digits + hex_size - 1;
    *pres = '\0';
    pres--;

    if (n == 0) {
        *pres = '0';
        *start_index = pres - hex_digits;
        return SUCCESS;
    }

    while (n > 0) {
        if (pres < hex_digits)
            return ERROR_OVERFLOW;

        int d = n % base;

        if (d > 9)
            *pres-- = d - 10 + 'A';
        else
            *pres-- = d + '0';

        n /= base;
    }
    ++pres;
    *start_index = pres - hex_digits;
    return SUCCESS;

    
}

status_code calculate_powers(const long x, unsigned long long results[][10]) {
    if (results == NULL)
        return ERROR_NULL_POINTER;
    if (x < 1 || x > 10)
        return ERROR_OUT_OF_DOMAIN;

    for (int base = 1; base <= 10; base++) {
        long long temp = 1;

        for (int power = 1; power <= x; power++) {

            temp *= base;
            results[base - 1][power - 1] = temp;
        }
    }
    return SUCCESS;
}

void print_error(status_code code) {
    switch (code) {
        case ERROR_INVALID_ARGS:
            printf("Ошибка: Неверное количество или формат аргументов командной строки.\n");
            break;
        case ERROR_INVALID_FLAG:
            printf("Ошибка: Некорректный флаг. Допустимы: -h, -p, -s, -e, -a, -f (или '/').\n");
            break;
        case ERROR_INVALID_NUMBER:
            printf("Ошибка: Переданная строка не является корректным целым числом.\n");
            break;
        case ERROR_NULL_POINTER:
            printf("Ошибка: Передан нулевой указатель.\n");
            break;
        case ERROR_OUT_OF_DOMAIN:
            printf("Ошибка: Передано значение вне допустимой области определения.\n");
            break;
        case ERROR_OVERFLOW:
            printf("Ошибка: Произошло переполнение типа данных при вычислениях.\n");
            break;
        default:
            printf("Ошибка: Неизвестный сбой приложения.\n");
            break;

    }
}