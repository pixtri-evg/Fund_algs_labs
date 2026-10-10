#ifndef FUNCTIONS7_H
#define FUNCTIONS7_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,
    ERROR_INVALID_FLAG,
    ERROR_NULL_POINTER,
    ERROR_MEMORY,
    ERROR_FILE_OPEN,
    ERROR_IO
} status_code;

void print_error(const status_code code) {
    switch (code) {
        case ERROR_INVALID_ARGS:
            printf("Ошибка: неверное количество или значение аргументов командной строки\n");
            break;
        case ERROR_INVALID_FLAG:
            printf("Ошибка: некорректный флаг. Допустимы: -r, /r, -a, /a\n");
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
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
}

status_code parse_flag(const char *input, char *action) {
    if (input == NULL || action == NULL) return ERROR_NULL_POINTER;
    if (input[0] != '-' && input[0] != '/') return ERROR_INVALID_FLAG;
    if (input[1] != 'r' && input[1] != 'a') return ERROR_INVALID_FLAG;
    if (input[2] != '\0') return ERROR_INVALID_FLAG;
    *action = input[1];
    return SUCCESS;
}

int is_separator(const int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

status_code read_token(FILE *in, char **token) {
    if (in == NULL || token == NULL) return ERROR_NULL_POINTER;
    *token = NULL;

    int ch = fgetc(in);
    while (ch != EOF && is_separator(ch)) ch = fgetc(in);
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

void to_lower_token(char *token) {
    if (token == NULL) return;
    for (char *p = token; *p != '\0'; p++) {
        if (*p >= 'A' && *p <= 'Z') *p = (char)(*p - 'A' + 'a');
    }
}

status_code char_code_in_base(const unsigned char c, const int base, char *buffer) {
    if (buffer == NULL) return ERROR_NULL_POINTER;
    if (base < 4 || base > 10) return ERROR_INVALID_ARGS;

    char tmp[16];
    int pos = 15;
    tmp[pos] = '\0';

    unsigned int value = c;
    do {
        pos--;
        tmp[pos] = (char)('0' + value % base); 
        value /= base;
    } while (value > 0);

    strcpy(buffer, tmp + pos);
    return SUCCESS;
}

status_code convert_token_to_codes(const char *token, const int base, char **result) {
    if (token == NULL || result == NULL) return ERROR_NULL_POINTER;
    *result = NULL;

    char *buff = (char*)malloc(strlen(token) * 4 + 1);
    if (buff == NULL) return ERROR_MEMORY;

    buff[0] = '\0';
    for (const char *p = token; *p != '\0'; p++) {
        char code[8];
        status_code status = char_code_in_base((unsigned char)*p, base, code);
        if (status != SUCCESS) {
            free(buff);
            return status;
        }
        strcat(buff, code);
    }
    *result = buff;
    return SUCCESS;
}

status_code write_token(FILE *out, const char *token) {
    if (out == NULL || token == NULL) return ERROR_NULL_POINTER;
    if (fputs(token, out) == EOF) return ERROR_IO;
    if (fputc(' ', out) == EOF) return ERROR_IO;
    return SUCCESS;
}

status_code copy_next_token(FILE *in, FILE *out, int *finished) {
    if (in == NULL || out == NULL || finished == NULL) return ERROR_NULL_POINTER;

    char *token = NULL;
    status_code status = read_token(in, &token);
    if (status != SUCCESS) return status;

    if (token == NULL) {
        *finished = 1;
        return SUCCESS;
    }

    status = write_token(out, token);
    free(token);
    return status;
}

status_code merge_files(FILE *in1, FILE *in2, FILE *out) {
    if (in1 == NULL || in2 == NULL || out == NULL) return ERROR_NULL_POINTER;

    int done1 = 0;
    int done2 = 0;

    while (!done1 || !done2) {
        status_code status = SUCCESS;
        if (!done1) status = copy_next_token(in1, out, &done1);
        if (status == SUCCESS && !done2) status = copy_next_token(in2, out, &done2);
        if (status != SUCCESS) return status;
    }

    if (fputc('\n', out) == EOF) return ERROR_IO;
    return SUCCESS;
}

status_code process_file_a(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return ERROR_NULL_POINTER;

    size_t index = 0;                 
    while (1) {
        char *token = NULL;
        status_code status = read_token(in, &token);
        if (status != SUCCESS) return status;
        if (token == NULL) break;  
        index++;

        char *codes = NULL;
        if (index % 10 == 0) {
            to_lower_token(token);
            status = convert_token_to_codes(token, 4, &codes);
        } else if (index % 5 == 0) {
            status = convert_token_to_codes(token, 8, &codes);
        } else if (index % 2 == 0) {
            to_lower_token(token);
        }

        if (status == SUCCESS) {
            const char *text = token;
            if (codes != NULL) text = codes;
            status = write_token(out, text);
        }

        free(codes);
        free(token);
        if (status != SUCCESS) return status;
    }

    if (fputc('\n', out) == EOF) return ERROR_IO;
    return SUCCESS;
}

#endif