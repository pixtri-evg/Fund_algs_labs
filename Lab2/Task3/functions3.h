#ifndef FUNCTIONS3_H
#define FUNCTIONS3_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_OPEN_FILE,
    ERROR_READ
} status_code;

typedef struct {
    size_t line;
    size_t column;
} Match;

typedef struct {
    const char *path;
    Match *matches;     /* динамический массив вхождений */
    size_t count;       /* сколько вхождений найдено */
} FileResult;

void print_error(const status_code code) {
    switch (code) {
    case ERROR_INVALID_ARGS:
        printf("Ошибка: неверные аргументы командной строки\n");
        break;
    case ERROR_NULL_POINTER:
        printf("Ошибка: передан нулевой указатель\n");
        break;
    case ERROR_MEMORY:
        printf("Ошибка: не удалось выделить память\n");
        break;
    case ERROR_OPEN_FILE:
        printf("Ошибка: не удалось открыть файл\n");
        break;
    case ERROR_READ:
        printf("Ошибка: не удалось прочитать данные из файла\n");
        break;
    default:
        printf("Неизвестная ошибка\n");
        break;
    }
}

status_code read_file_content(const char *path, char **text, size_t *length) {
    if (path == NULL || text == NULL || length == NULL) return ERROR_NULL_POINTER;
    *text = NULL;
    *length = 0;

    FILE *file = fopen(path, "r");
    if (file == NULL) return ERROR_OPEN_FILE;

    size_t capacity = 256;
    size_t size = 0;
    char *buffer = (char*)malloc(capacity);
    if (buffer == NULL) {
        fclose(file);
        return ERROR_MEMORY;
    }

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (size == capacity) {
            size_t new_capacity = capacity * 2;
            char *tmp = (char*)realloc(buffer, new_capacity);
            if (tmp == NULL) {
                free(buffer);
                fclose(file);
                return ERROR_MEMORY;
            }
            buffer = tmp;
            capacity = new_capacity;
        }
        buffer[size] = c;
        size++;
    }
    if (ferror(file)) {
        free(buffer);
        fclose(file);
        return ERROR_READ;
    }
    fclose(file);
    *text = buffer;
    *length = size;
    return SUCCESS;
}

int is_match_at(const char *content, const size_t length, const size_t pos,
                const char *sub, const size_t sub_len) {
    if (pos + sub_len > length) return 0;

    for (size_t i = 0; i < sub_len; i++) {
        if (content[pos + i] != sub[i]) return 0;
    }
    return 1;
}

status_code find_matches(const char *content, const size_t length, const char *sub,
                         Match **matches, size_t *count) {
    if (content == NULL || sub == NULL || matches == NULL || count == NULL) {
        return ERROR_NULL_POINTER;
    }
    *matches = NULL;
    *count = 0;

    const size_t sub_len = strlen(sub);
    if (sub_len == 0) return SUCCESS;

    Match *array = NULL;
    size_t size = 0;
    size_t capacity = 0;

    size_t line = 1;
    size_t column = 1;

    for (size_t pos = 0; pos < length; pos++) {
        if (is_match_at(content, length, pos, sub, sub_len)) {
            if (size == capacity) {
                size_t new_capacity = (capacity == 0) ? 4 : capacity * 2;
                Match *tmp = (Match*)realloc(array, new_capacity * sizeof(Match));
                if (tmp == NULL) {
                    free(array);
                    return ERROR_MEMORY;
                }
                array = tmp;
                capacity = new_capacity;
            }
            array[size].line = line;
            array[size].column = column;
            size++;
        }

        if (content[pos] == '\n') {
            line++;
            column = 1;
        }
        else column++;
    }
    *matches = array;
    *count = size;
    return SUCCESS;
}

void free_results(FileResult *results, const size_t count) {
    if (results == NULL) return;
    for (size_t i = 0; i < count; i++) {
        free(results[i].matches);
    }
    free(results);
}

/* Главная функция задания: ищет подстроку в нескольких файлах.
   results   - сюда запишем динамический массив результатов (по одному на файл),
   substring - искомая подстрока,
   file_count - сколько путей к файлам передано,
   ...       - сами пути к файлам (const char *), ровно file_count штук.
   Ничего не печатает, только возвращает результат через параметры. */
status_code search_in_files(FileResult **results, const char *substring,
                            const size_t file_count, ...) {
    if (results == NULL || substring == NULL) return ERROR_NULL_POINTER;
    *results = NULL;
    if (file_count == 0) return ERROR_INVALID_ARGS;

    FileResult *array = (FileResult*)calloc(file_count, sizeof(FileResult));
    if (array == NULL) return ERROR_MEMORY;

    va_list args;
    va_start(args, file_count);

    status_code status = SUCCESS;
    for (size_t i = 0; i < file_count; i++) {
        const char *path = va_arg(args, const char *);
        if (path == NULL) {
            status = ERROR_NULL_POINTER;
            break;
        }
        array[i].path = path;

        char *content = NULL;
        size_t length = 0;
        status = read_file_content(path, &content, &length);
        if (status != SUCCESS) break;

        status = find_matches(content, length, substring,
                              &array[i].matches, &array[i].count);
        free(content);
        if (status != SUCCESS) break;
    }
    va_end(args);

    if (status != SUCCESS) {
        free_results(array, file_count);
        return status;
    }
    *results = array;
    return SUCCESS;
}

void print_results(const FileResult *results, const size_t count) {
    if (results == NULL) return;
    for (size_t i = 0; i < count; i++) {
        printf("Файл: %s\n", results[i].path);
        if (results[i].count == 0) {
            printf("  вхождений нет\n");
            continue;
        }
        for (size_t j = 0; j < results[i].count; j++) {
            printf("  строка %zu, символ %zu\n",
                   results[i].matches[j].line, results[i].matches[j].column);
        }
    }
}

#endif