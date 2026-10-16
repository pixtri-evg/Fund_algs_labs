#ifndef FUNCTIONS2_H
#define FUNCTIONS2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define EPSILON 1e-9

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_FLAG,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_OPEN_FILE,
    ERROR_INVALID_DATA,
    ERROR_SAME_FILE,
    ERROR_WRITE,
    ERROR_READ
} status_code;

typedef struct {
    unsigned int id;
    char *name;
    char *surname;
    double salary;
} Employee;

void print_error(const status_code code) {
    switch (code) {
    case ERROR_INVALID_ARGS: printf("Ошибка: неверное количество аргументов командной строки\n"); break;
    case ERROR_INVALID_FLAG: printf("Ошибка: некорректный флаг. Допустимы: -a, -d, /a, /d\n"); break;
    case ERROR_NULL_POINTER: printf("Ошибка: передан нулевой указатель\n"); break;
    case ERROR_MEMORY:       printf("Ошибка: не удалось выделить память\n"); break;
    case ERROR_OPEN_FILE:    printf("Ошибка: не удалось открыть файл\n"); break;
    case ERROR_INVALID_DATA: printf("Ошибка: во входном файле найдены некорректные данные\n"); break;
    case ERROR_SAME_FILE:    printf("Ошибка: входной и выходной файлы совпадают\n"); break;
    case ERROR_WRITE:        printf("Ошибка: не удалось записать данные в файл\n"); break;
    case ERROR_READ:         printf("Ошибка: не удалось прочитать данные из файла\n"); break;
    default:                 printf("Неизвестная ошибка\n"); break;
    }
}

status_code parse_flag(const char *str, char *order) {
    if (str == NULL || order == NULL) return ERROR_NULL_POINTER;
    if (str[0] != '-' && str[0] != '/') return ERROR_INVALID_FLAG;
    if (str[1] != 'a' && str[1] != 'd') return ERROR_INVALID_FLAG;
    if (str[2] != '\0') return ERROR_INVALID_FLAG;
    *order = str[1];
    return SUCCESS;
}

static status_code read_line(FILE *file, char **line) {
    if (file == NULL || line == NULL) return ERROR_NULL_POINTER;
    *line = NULL;

    size_t cap = 16, len = 0;
    char *buf = (char*)malloc(cap);
    if (buf == NULL) return ERROR_MEMORY;

    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            char *tmp = (char*)realloc(buf, cap * 2);
            if (tmp == NULL) {
                free(buf);
                return ERROR_MEMORY;
            }
            buf = tmp;
            cap *= 2;
        }
        buf[len++] = (char)c;
    }
    if (c == EOF && ferror(file)) {
        free(buf);
        return ERROR_READ;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return SUCCESS;
    }
    buf[len] = '\0';
    *line = buf;
    return SUCCESS;
}

static status_code split_line(char *line, char *words[], const size_t count) {
    if (line == NULL || words == NULL) return ERROR_NULL_POINTER;

    size_t pos = 0;
    for (size_t i = 0; i < count; i++) {
        while (line[pos] == ' ') pos++;
        if (line[pos] == '\0') return ERROR_INVALID_DATA;

        words[i] = &line[pos];
        while (line[pos] != ' ' && line[pos] != '\0') pos++;

        if (line[pos] == ' ') line[pos++] = '\0';
    }
    while (line[pos] == ' ') pos++;
    return (line[pos] == '\0') ? SUCCESS : ERROR_INVALID_DATA;
}

static status_code parse_id(const char *str, unsigned int *id) {
    if (str == NULL || id == NULL) return ERROR_NULL_POINTER;
    if (str[0] == '\0') return ERROR_INVALID_DATA;

    unsigned int result = 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') return ERROR_INVALID_DATA;
        unsigned int digit = str[i] - '0';
        if (result > (UINT_MAX - digit) / 10) return ERROR_INVALID_DATA;
        result = result * 10 + digit;
    }
    *id = result;
    return SUCCESS;
}

static int is_word(const char *str) {
    if (str == NULL || str[0] == '\0') return 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) return 0;
    }
    return 1;
}

static status_code parse_salary(const char *str, double *salary) {
    if (str == NULL || salary == NULL) return ERROR_NULL_POINTER;

    int has_digit = 0, dots = 0;
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') has_digit = 1;
        else if (str[i] == '.') dots++;
        else return ERROR_INVALID_DATA;
    }
    if (!has_digit || dots > 1) return ERROR_INVALID_DATA;

    const double value = strtod(str, NULL);
    if (!isfinite(value)) return ERROR_INVALID_DATA;

    *salary = value;
    return SUCCESS;
}

static status_code copy_str(const char *src, char **dest) {
    if (src == NULL || dest == NULL) return ERROR_NULL_POINTER;
    *dest = (char*)malloc(strlen(src) + 1);
    if (*dest == NULL) return ERROR_MEMORY;
    strcpy(*dest, src);
    return SUCCESS;
}

static status_code parse_employee(char *line, Employee *emp) {
    if (line == NULL || emp == NULL) return ERROR_NULL_POINTER;

    char *words[4];
    status_code status = split_line(line, words, 4);
    if (status != SUCCESS) return status;

    unsigned int id;
    double salary;
    status = parse_id(words[0], &id);
    if (status != SUCCESS) return status;
    if (!is_word(words[1]) || !is_word(words[2])) return ERROR_INVALID_DATA;
    status = parse_salary(words[3], &salary);
    if (status != SUCCESS) return status;

    char *name, *surname;
    status = copy_str(words[1], &name);
    if (status != SUCCESS) return status;
    status = copy_str(words[2], &surname);
    if (status != SUCCESS) {
        free(name);
        return status;
    }

    emp->id = id;
    emp->name = name;
    emp->surname = surname;
    emp->salary = salary;
    return SUCCESS;
}

void free_employees(Employee *list, const size_t count) {
    if (list == NULL) return;
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].surname);
    }
    free(list);
}

static int is_blank(const char *line) {
    for (size_t i = 0; line[i] != '\0'; i++) {
        if (line[i] != ' ') return 0;
    }
    return 1;
}

status_code read_employees(FILE *file, Employee **list, size_t *count) {
    if (file == NULL || list == NULL || count == NULL) return ERROR_NULL_POINTER;
    *list = NULL;
    *count = 0;

    Employee *arr = NULL;
    size_t size = 0, cap = 0;

    while (1) {
        char *line;
        status_code status = read_line(file, &line);
        if (status != SUCCESS) {
            free_employees(arr, size);
            return status;
        }
        if (line == NULL) break;
        if (is_blank(line)) {
            free(line);
            continue;
        }
        if (size == cap) {
            size_t new_cap = (cap == 0) ? 4 : cap * 2;
            Employee *tmp = (Employee*)realloc(arr, new_cap * sizeof(Employee));
            if (tmp == NULL) {
                free(line);
                free_employees(arr, size);
                return ERROR_MEMORY;
            }
            arr = tmp;
            cap = new_cap;
        }
        status = parse_employee(line, &arr[size]);
        free(line);
        if (status != SUCCESS) {
            free_employees(arr, size);
            return status;
        }
        size++;
    }
    *list = arr;
    *count = size;
    return SUCCESS;
}

static int compare_doubles(const double a, const double b, const double eps) {
    if (fabs(a - b) < eps) return 0;
    return (a < b) ? -1 : 1;
}

int compare_asc(const void *a, const void *b) {
    const Employee *x = (const Employee *)a;
    const Employee *y = (const Employee *)b;

    int res = compare_doubles(x->salary, y->salary, EPSILON);
    if (res != 0) return res;

    res = strcmp(x->surname, y->surname);
    if (res != 0) return res;

    res = strcmp(x->name, y->name);
    if (res != 0) return res;

    if (x->id < y->id) return -1;
    if (x->id > y->id) return 1;
    return 0;
}

int compare_desc(const void *a, const void *b) {
    return compare_asc(b, a);
}

status_code write_employees(FILE *file, const Employee *list, const size_t count) {
    if (file == NULL) return ERROR_NULL_POINTER;
    if (list == NULL && count > 0) return ERROR_NULL_POINTER;

    for (size_t i = 0; i < count; i++) {
        if (fprintf(file, "%u %s %s %.2f\n", list[i].id, list[i].name,
                    list[i].surname, list[i].salary) < 0)
            return ERROR_WRITE;
    }
    return SUCCESS;
}

#endif