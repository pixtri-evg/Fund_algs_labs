#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

void print_error(const status_code code) {
    switch (code) {
        case ERROR_INVALID_ARGS:
            printf("Ошибка: Неверное количество аргументов командной строки\n");
            break;
        case ERROR_INVALID_FLAG:
            printf("Ошибка: Некорректный флаг. Допустимы: -q, -m, -t (или '/')\n");
            break;
        case ERROR_INVALID_NUMBER:
            printf("Ошибка: Переданная строка не является корректным числом\n");
            break;
        case ERROR_NULL_POINTER:
            printf("Ошибка: Передан нулевой указатель\n");
            break;
        case ERROR_OUT_OF_DOMAIN:
            printf("Ошибка: Значение вне допустимой области определения\n");
            break;
        case ERROR_OVERFLOW:
            printf("Ошибка: Переполнение при разборе числа\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
}

static int is_zero(const double x, const double eps) { return fabs(x) < eps; }

static int is_positive(const double x, const double eps) { return x > eps; }
 
status_code parse_flag(const char *input, char *flag) {
    if (input == NULL || flag == NULL) return ERROR_NULL_POINTER;
 
    if ((input[0] != '-' && input[0] != '/') || input[1] == '\0' || input[2] != '\0')
        return ERROR_INVALID_FLAG;
 
    const char c = input[1];
    if (c == 'q' || c == 'm' || c == 't') {
        *flag = c;
        return SUCCESS;
    }
    return ERROR_INVALID_FLAG;
}

status_code parse_double(const char *input, double *result) {
    if (input == NULL || result == NULL) return ERROR_NULL_POINTER;
    if (*input == '\0') return ERROR_INVALID_NUMBER;
 
    char *endptr;
    const double value = strtod(input, &endptr);
 
    if (endptr == input || *endptr != '\0') return ERROR_INVALID_NUMBER;
    if (!isfinite(value)) return ERROR_OVERFLOW;
 
    *result = value;
    return SUCCESS;
}

status_code parse_eps(const char *input, double *result) {
    const status_code status = parse_double(input, result);
    if (status != SUCCESS) return status;
    if (*result <= 0.0 || *result >= 1.0) return ERROR_OUT_OF_DOMAIN;
    return SUCCESS;
}

status_code parse_long(const char *input, long *result) {
    if (input == NULL || result == NULL)
        return ERROR_NULL_POINTER;
 
    int sign = 1;
    if (*input == '+' || *input == '-') {
        if (*input == '-') sign = -1;
        input++;
    }
    if (*input == '\0') 
        return ERROR_INVALID_NUMBER;
 
    long value = 0;
    while (*input != '\0') {
        if (!isdigit((unsigned char)*input)) 
            return ERROR_INVALID_NUMBER;

        const int digit = *input - '0';
        if (value > (LONG_MAX - digit) / 10) 
            return ERROR_OVERFLOW;
        value = value * 10 + digit;
        input++;
    }
    *result = sign * value;
    return SUCCESS;
}

status_code solve_quadratic(const double a, const double b, const double c,
                             const double eps, int *root_count, double *x1, double *x2) {
    if (root_count == NULL || x1 == NULL || x2 == NULL) 
        return ERROR_NULL_POINTER;
 
    if (is_zero(a, eps)) {          
        if (is_zero(b, eps))
            *root_count = is_zero(c, eps) ? -1 : 0;
        else {
            *x1 = -c / b;
            *root_count = 1;
        }
        return SUCCESS;
    }
 
    const double d = b * b - 4.0 * a * c;
    if (fabs(d) ) {
        *x1 = -b / (2.0 * a);
        *root_count = 1;
    } else if (d > 0.0) {            
        const double sq = sqrt(d);
        *x1 = (-b - sq) / (2.0 * a);
        *x2 = (-b + sq) / (2.0 * a);
        *root_count = 2;
    } else {
        *root_count = 0;
    }
    return SUCCESS;
}

status_code build_unique_permutations(const double coeffs[3], const double eps,
                                       double perms[6][3], int *count) {
    if (perms == NULL || count == NULL) 
        return ERROR_NULL_POINTER;
 
    static const int table[6][3] = {
        {0,1,2}, {0,2,1}, {1,0,2}, {1,2,0}, {2,0,1}, {2,1,0}
    };
    *count = 0;
 
    for (int i = 0; i < 6; i++) {
        const double a = coeffs[table[i][0]];
        const double b = coeffs[table[i][1]];
        const double c = coeffs[table[i][2]];
 
        int already_added = 0;
        for (int j = 0; j < *count; j++)
            if (is_zero(a - perms[j][0], eps) &&   /* [5] сравнение с уже добавленной тройкой */
                is_zero(b - perms[j][1], eps) &&
                is_zero(c - perms[j][2], eps)) {
                already_added = 1;
                break;
            }
        if (already_added) continue;
 
        perms[*count][0] = a;
        perms[*count][1] = b;
        perms[*count][2] = c;
        (*count)++;
    }
    return SUCCESS;
}

status_code check_multiplicity(const long a, const long b, int *is_multiple) {
    if (is_multiple == NULL) 
        return ERROR_NULL_POINTER;
    if (a == 0 || b == 0) 
        return ERROR_OUT_OF_DOMAIN; 
    *is_multiple = (a % b == 0);
    return SUCCESS;
}

status_code check_right_triangle(double s1, double s2, double s3, const double eps,
                                  int *is_triangle, int *is_right) {
    if (is_triangle == NULL || is_right == NULL) return ERROR_NULL_POINTER;

    if (s1 > s2) {
        const double t = s1;
        s1 = s2; s2 = t;
    }
    if (s2 > s3) {
        const double t = s2;
        s2 = s3; s3 = t; 
    }
    if (s1 > s2) {
        const double t = s1;
        s1 = s2; s2 = t; 
    }

    if (!is_positive(s1, eps)) {        /* [6] сторона не может быть <= 0 */
        *is_triangle = 0;
        *is_right = 0;
        return SUCCESS;
    }
 
    *is_triangle = is_positive(s1 + s2 - s3, eps);  /* [7] неравенство треугольника */
    if (!*is_triangle) {
        *is_right = 0;
        return SUCCESS;
    }
 
    *is_right = is_zero(s1 * s1 + s2 * s2 - s3 * s3, eps);  /* [8] теорема Пифагора */
    return SUCCESS;
}