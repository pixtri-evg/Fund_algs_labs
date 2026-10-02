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
            printf("Ошибка: некорректный флаг. Допустимы: -d, /d, -i, /i, -s, /s, -a, /a (можно с n: -nd и т.д.)\n");
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

int is_arabic_digit(const int c) { return c >= '0' && c <= '9'; }
 
int is_latin_letter(const int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_other_char(const int c) {
    return !is_latin_letter(c) && !is_arabic_digit(c) && c != ' ';
}

status_code parse_flag(const char *input, char *action, int *has_n) {
    if (input == NULL || action == NULL || has_n == NULL) return ERROR_NULL_POINTER;
    if (input[0] != '-' && input[0] != '/') return ERROR_INVALID_FLAG;
 
    *has_n = (input[1] == 'n');
    const int pos = *has_n ? 2 : 1;     
    const char c = input[pos];
 
    if (c != 'd' && c != 'i' && c != 's' && c != 'a') return ERROR_INVALID_FLAG;
    if (input[pos + 1] != '\0') return ERROR_INVALID_FLAG;
 
    *action = c;
    return SUCCESS;
}

status_code make_output_path(const char *input_path, char **output_path) {
    if (input_path == NULL || output_path == NULL) return ERROR_NULL_POINTER;
 
    const char *slash = strrchr(input_path, '/');
 
    const char *name = (slash == NULL) ? input_path : slash + 1;
    const size_t dir_len = (size_t)(name - input_path);
    const size_t prefix_len = 4;                 
 
    char *path = (char*)malloc(strlen(input_path) + prefix_len + 1);
    if (path == NULL) return ERROR_MEMORY;
 
    memcpy(path, input_path, dir_len);         
    memcpy(path + dir_len, "out_", prefix_len); 
    strcpy(path + dir_len + prefix_len, name);  

    *output_path = path;
    return SUCCESS;
}

status_code exclude_digits(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return ERROR_NULL_POINTER;
 
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        if (!is_arabic_digit(ch)) {
            if (fputc(ch, out) == EOF) return ERROR_IO;
        }
    }
    return ferror(in) ? ERROR_IO : SUCCESS;
}
 
status_code replace_non_digits_hex(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return ERROR_NULL_POINTER;
 
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        if (is_arabic_digit(ch)) {
            if (fputc(ch, out) == EOF) return ERROR_IO;
        }
        else {
            if (fprintf(out, "%02X", ch) < 0) return ERROR_IO;
        }
    }
    return ferror(in) ? ERROR_IO : SUCCESS;
}

status_code count_matches_per_line(FILE *in, FILE *out, int (*func)(int)) {
    if (in == NULL || out == NULL || func == NULL) return ERROR_NULL_POINTER;
 
    int ch;
    int count = 0;
    int in_line = 0;
 
    while ((ch = fgetc(in)) != EOF) {
        if (ch == '\n') {
            if (fprintf(out, "%d\n", count) < 0) return ERROR_IO;
            count = 0;
            in_line = 0;
        } 
        else {
            in_line = 1;
            if (func(ch)) count++;
        }
    }
    if (ferror(in)) return ERROR_IO;
 
    if (in_line && fprintf(out, "%d\n", count) < 0) return ERROR_IO;
    return SUCCESS;
}
