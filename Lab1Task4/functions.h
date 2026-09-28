#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGS,   
    ERROR_INVALID_FLAG,  
    ERROR_NULL_POINTER,  
    ERROR_MEMORY,         /* не удалось выделить память */
    ERROR_FILE_OPEN,      
    ERROR_IO              /* ошибка чтения/записи */
} status_code;

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
    const int pos = *has_n ? 2 : 1;       /* позиция буквы действия */
    const char c = input[pos];
 
    if (c != 'd' && c != 'i' && c != 's' && c != 'a') return ERROR_INVALID_FLAG;
    if (input[pos + 1] != '\0') return ERROR_INVALID_FLAG;
 
    *action = c;
    return SUCCESS;
}

status_code make_output_path(const char *input_path, char **output_path) {
    if (input_path == NULL || output_path == NULL) return ERROR_NULL_POINTER;
 
    const char *slash = strrchr(input_path, '/');          //находит последнее вхождение символа
 
    const char *name = (slash == NULL) ? input_path : slash + 1;
    const size_t dir_len = (size_t)(name - input_path);   /* длина части с папками */
    const size_t prefix_len = 4;                          /* длина "out_" */
 
    char *path = (char*)malloc(strlen(input_path) + prefix_len + 1);
    if (path == NULL) return ERROR_MEMORY;
 
    memcpy(path, input_path, dir_len);           //копируем первые dir_len симв исх строки
    memcpy(path + dir_len, "out_", prefix_len); //вставляем out в начало строки, сдвинутое на dir_len
    strcpy(path + dir_len + prefix_len, name);  //дописывает имя файла

    //memcpy копирует ровно n байтов
    //strcpy копирует всю строку вместе с '/0'
 
    *output_path = path;
    return SUCCESS;
}

status_code exclude_digits(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return ERROR_NULL_POINTER;
 
    int ch;
    while ((ch = fgetc(in)) != EOF)
        if (!is_arabic_digit(ch) && fputc(ch, out) == EOF) return ERROR_IO;
 
    return ferror(in) ? ERROR_IO : SUCCESS;
}
 
/* -a: цифры копирует, остальные символы заменяет ASCII-кодом в hex.
 * fputc при ошибке возвращает EOF (<0), fprintf - отрицательное число,
 * поэтому обе ветки можно проверить одним условием "written < 0". */
status_code replace_non_digits_hex(FILE *in, FILE *out) {
    if (in == NULL || out == NULL) return ERROR_NULL_POINTER;
 
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        const int written = is_arabic_digit(ch) ? fputc(ch, out) : fprintf(out, "%02X", ch);
        if (written < 0) return ERROR_IO;
    }
    return ferror(in) ? ERROR_IO : SUCCESS;
}

status_code count_matches_per_line(FILE *in, FILE *out, int (*func)(int)) {
    if (in == NULL || out == NULL || func == NULL) return ERROR_NULL_POINTER;
 
    int ch;
    int count = 0;
    int in_line = 0;   /* 1, если после последнего '\n' уже были символы */
 
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
 
    /* последняя строка файла могла не заканчиваться переводом строки */
    if (in_line && fprintf(out, "%d\n", count) < 0) return ERROR_IO;
    return SUCCESS;
}

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