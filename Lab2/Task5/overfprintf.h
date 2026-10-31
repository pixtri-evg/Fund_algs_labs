#ifndef OVERPRINTF_H
#define OVERPRINTF_H

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SUCCESS = 0,
    ERROR_NULL_POINTER,
    ERROR_INVALID_FORMAT,
    ERROR_INVALID_NUMBER,
    ERROR_OUT_OF_RANGE,
    ERROR_MEMORY_ALLOCATION
} status_code;

typedef struct {
    char *data;    // указатель на выделенную память с текстом
    size_t len;    // сколько символов в буфере сейчас без '0'
    size_t cap;    // сколько байт выделено
} Buffer;

status_code buffer_init(Buffer *buf) {
    if (buf == NULL) return ERROR_NULL_POINTER;
    buf->len = 0;
    buf->cap = 64;
    buf->data = malloc(buf->cap);
    if (buf->data == NULL) {
        buf->cap = 0;
        return ERROR_MEMORY_ALLOCATION;
    }
    buf->data[0] = '\0';
    return SUCCESS;
}

void buffer_free(Buffer *buf) {
    if (buf == NULL) return;
    free(buf->data);
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

status_code buffer_reserve(Buffer *buf, size_t extra) {
    if (buf == NULL || buf->data == NULL) return ERROR_NULL_POINTER;
    if (extra > SIZE_MAX - buf->len - 1) return ERROR_MEMORY_ALLOCATION;

    size_t need = buf->len + extra + 1;
    if (need <= buf->cap) return SUCCESS;

    size_t new_cap = buf->cap;
    while (new_cap < need) {
        if (new_cap > SIZE_MAX / 2) {
            new_cap = need;
            break;
        }
        new_cap *= 2;
    }

    char *tmp = realloc(buf->data, new_cap);
    if (tmp == NULL) return ERROR_MEMORY_ALLOCATION;
    buf->data = tmp;
    buf->cap = new_cap;
    return SUCCESS;
}

status_code buffer_append_n(Buffer *buf, const char *str, size_t n) {
    if (buf == NULL || str == NULL) return ERROR_NULL_POINTER;
    status_code st = buffer_reserve(buf, n);
    if (st != SUCCESS) return st;
    memcpy(buf->data + buf->len, str, n);
    buf->len += n;
    buf->data[buf->len] = '\0';
    return SUCCESS;
}

status_code buffer_append_char(Buffer *buf, char c) {
    return buffer_append_n(buf, &c, 1);
}

status_code append_format(Buffer *buf, const char *spec, ...) {
    if (buf == NULL || spec == NULL) return ERROR_NULL_POINTER;

    // два раза потому что вызываем vsnprintf два раза
    va_list ap, ap_copy;
    va_start(ap, spec);
    va_copy(ap_copy, ap);

    int n = vsnprintf(NULL, 0, spec, ap);   // чтобы узнать длину результата, ничего не пишет
    va_end(ap);

    status_code st = SUCCESS;
    if (n < 0) {
        st = ERROR_INVALID_FORMAT;
    }
    else {
        st = buffer_reserve(buf, (size_t)n);

        if (st == SUCCESS) {
            int written = vsnprintf(buf->data + buf->len, (size_t)n + 1, spec, ap_copy);

            if (written != n) st = ERROR_INVALID_FORMAT;
            else buf->len += (size_t)n;
        }
    }
    va_end(ap_copy);
    return st;
}

int normalize_base(int base) {
    return (base < 2 || base > 36) ? 10 : base;
}

status_code append_roman(Buffer *buf, int value) {
    static const int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    static const char *symbols[] = {"M", "CM", "D", "CD", "C", "XC", "L",
                                    "XL", "X", "IX", "V", "IV", "I"};
    if (buf == NULL) return ERROR_NULL_POINTER;
    if (value < 1 || value > 3999) return ERROR_OUT_OF_RANGE;

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        while (value >= values[i]) {
            status_code st = buffer_append_n(buf, symbols[i], strlen(symbols[i]));
            if (st != SUCCESS) return st;
            value -= values[i];
        }
    }
    return SUCCESS;
}

status_code append_zeckendorf(Buffer *buf, const unsigned int number) {
    if (buf == NULL) return ERROR_NULL_POINTER;

    unsigned long long fib[50];
    unsigned char coef[50] = {0};
    size_t count = 2;
    fib[0] = 1;
    fib[1] = 2;
    while (fib[count - 1] <= number) {
        fib[count] = fib[count - 1] + fib[count - 2];
        count++;
    }

    unsigned long long rest = number;
    size_t top = 0;
    for (int i = (int)count - 1; i >= 0; i--) {
        if (fib[i] <= rest) {
            coef[i] = 1;
            rest -= fib[i];
            if ((size_t)i > top) top = (size_t)i;
        }
    }

    for (size_t i = 0; i <= top; i++) {
        status_code st = buffer_append_char(buf, coef[i] ? '1' : '0');
        if (st != SUCCESS) return st;
    }
    // возвращает статус последнего добавления единицы
    return buffer_append_char(buf, '1');
}

status_code append_in_base(Buffer *buf, int value, int base, int upper) {
    if (buf == NULL) return ERROR_NULL_POINTER;
    base = normalize_base(base);

    char letter_base = upper ? 'A' : 'a';
    unsigned int number = (value < 0) ? -(unsigned int)value : (unsigned int)value;

    char reversed[33];
    int count = 0;
    do {
        int remainder = number % base;

        if (remainder < 10) reversed[count] = '0' + remainder;
        else reversed[count] = letter_base + remainder - 10;

        count++;
        number /= base;
    } while (number > 0);

    status_code st;
    if (value < 0) {
        st = buffer_append_char(buf, '-');
        if (st != SUCCESS) return st;
    }
    for (int i = count - 1; i >= 0; i--) {
        st = buffer_append_char(buf, reversed[i]);
        if (st != SUCCESS) return st;
    }
    return SUCCESS;
}

status_code parse_base_string(const char *str, int base, long long *result) {
    if (str == NULL || result == NULL) return ERROR_NULL_POINTER;
    base = normalize_base(base);

    const char *p = str;
    int negative = 0;
    if (*p == '-') {
        negative = 1;
        p++;
    }
    if (*p == '\0') return ERROR_INVALID_NUMBER;

    long long value = 0;
    for (; *p != '\0'; p++) {

        int c = toupper(*p);
        int d;

        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'A' && c <= 'Z') d = c - 'A' + 10;
        else return ERROR_INVALID_NUMBER;

        if (d >= base) return ERROR_INVALID_NUMBER;
        if (value > (LLONG_MAX - d) / base) return ERROR_OUT_OF_RANGE;
        value = value * base + d;
    }

    *result = negative ? -value : value;
    return SUCCESS;
}

status_code append_memory_dump(Buffer *buf, const void *data, size_t size) {
    if (buf == NULL || data == NULL) return ERROR_NULL_POINTER;
    const unsigned char *bytes = (const unsigned char *)data;

    for (size_t i = 0; i < size; i++) {
        status_code st;
        if (i > 0) {
            st = buffer_append_char(buf, ' ');
            if (st != SUCCESS) return st;
        }
        for (int bit = 7; bit >= 0; bit--) {
            st = buffer_append_char(buf, ((bytes[i] >> bit) & 1) ? '1' : '0');
            if (st != SUCCESS) return st;
        }
    }
    return SUCCESS;
}

#define SPEC_MAX 64

static status_code handle_standard(Buffer *buf, const char **pos, va_list *ap) {
    const char *p = *pos;
    char spec[SPEC_MAX] = "%";
    size_t len = 1;

    while (*p != '\0' && strchr("-+ #0123456789.*", *p) != NULL && len < SPEC_MAX - 16) {
        if (*p == '*') {
            int n = va_arg(*ap, int);
            len += (size_t)snprintf(spec + len, SPEC_MAX - len, "%d", n);
        }
        else spec[len++] = *p;
        p++;
    }

    char mod = 0;                     //есть ли буква модификаора
    int two = 0;                      //буква двойная или нет
    if (*p != '\0' && strchr("hlLjzt", *p) != NULL) {
        mod = *p;
        spec[len++] = *p;
        p++;
        if ((mod == 'h' || mod == 'l') && *p == mod) {
            two = 1;
            spec[len++] = *p;
            p++;
        }
    }

    char type = *p;
    if (type == '\0') return ERROR_INVALID_FORMAT;
    spec[len++] = type;
    spec[len] = '\0';
    p++;

    status_code st;
    if (type == 'd' || type == 'i') {
        if (mod == 'l' && two) st = append_format(buf, spec, va_arg(*ap, long long));
        else if (mod == 'l')   st = append_format(buf, spec, va_arg(*ap, long));
        else if (mod == 'j')   st = append_format(buf, spec, va_arg(*ap, intmax_t));
        else if (mod == 'z')   st = append_format(buf, spec, va_arg(*ap, size_t));
        else if (mod == 't')   st = append_format(buf, spec, va_arg(*ap, ptrdiff_t));
        else                   st = append_format(buf, spec, va_arg(*ap, int));
    } else if (type == 'u' || type == 'x' || type == 'X' || type == 'o') {
        if (mod == 'l' && two) st = append_format(buf, spec, va_arg(*ap, unsigned long long));
        else if (mod == 'l')   st = append_format(buf, spec, va_arg(*ap, unsigned long));
        else if (mod == 'j')   st = append_format(buf, spec, va_arg(*ap, uintmax_t));
        else if (mod == 'z')   st = append_format(buf, spec, va_arg(*ap, size_t));
        else if (mod == 't')   st = append_format(buf, spec, va_arg(*ap, ptrdiff_t));
        else                   st = append_format(buf, spec, va_arg(*ap, unsigned int));
    } else if (strchr("fFeEgG", type) != NULL) {
        if (mod == 'L') st = append_format(buf, spec, va_arg(*ap, long double));
        else            st = append_format(buf, spec, va_arg(*ap, double));
    } else if (type == 'c' && mod == 0) {
        st = append_format(buf, spec, va_arg(*ap, int));
    } else if (type == 's' && mod == 0) {
        st = append_format(buf, spec, va_arg(*ap, const char *));
    } else if (type == 'p' && mod == 0) {
        st = append_format(buf, spec, va_arg(*ap, void *));
    } else {
        return ERROR_INVALID_FORMAT;
    }

    if (st == SUCCESS) *pos = p;
    return st;
}

static int flag_is(const char *p, const char *flag) {
    return p[0] == flag[0] && p[1] == flag[1];
}

static status_code handle_custom(Buffer *buf, const char *p, va_list *ap, int *handled) {
    *handled = 1;

    if (flag_is(p, "Ro")) {
        int v = va_arg(*ap, int);
        return append_roman(buf, v);
    }
    if (flag_is(p, "Zr")) {
        unsigned int v = va_arg(*ap, unsigned int);
        return append_zeckendorf(buf, v);
    }
    if (flag_is(p, "Cv") || flag_is(p, "CV")) {
        int v = va_arg(*ap, int);
        int base = va_arg(*ap, int);
        return append_in_base(buf, v, base, p[1] == 'V');
    }
    if (flag_is(p, "to") || flag_is(p, "TO")) {
        const char *s = va_arg(*ap, const char *);
        int base = va_arg(*ap, int);
        long long value = 0;
        status_code st = parse_base_string(s, base, &value);
        if (st != SUCCESS) return st;
        return append_format(buf, "%lld", value);
    }
    if (flag_is(p, "mi")) {
        int32_t v = (int32_t)va_arg(*ap, int);
        return append_memory_dump(buf, &v, sizeof(v));
    }
    if (flag_is(p, "mu")) {
        uint32_t v = (uint32_t)va_arg(*ap, unsigned int);
        return append_memory_dump(buf, &v, sizeof(v));
    }
    if (flag_is(p, "md")) {
        double v = va_arg(*ap, double);
        return append_memory_dump(buf, &v, sizeof(v));
    }
    if (flag_is(p, "mf")) {
        float v = (float)va_arg(*ap, double);
        return append_memory_dump(buf, &v, sizeof(v));
    }

    *handled = 0;
    return SUCCESS;
}

static status_code format_to_buffer(Buffer *buf, const char *format, va_list *ap) {
    if (buf == NULL || format == NULL || ap == NULL) return ERROR_NULL_POINTER;

    const char *p = format;
    while (*p != '\0') {
        status_code st;

        if (*p != '%') {
            st = buffer_append_char(buf, *p);
            if (st != SUCCESS) return st;
            p++;
            continue;
        }

        p++;
        if (*p == '\0') return ERROR_INVALID_FORMAT;

        if (*p == '%') {
            st = buffer_append_char(buf, '%');
            if (st != SUCCESS) return st;
            p++;
            continue;
        }

        int handled = 0;
        st = handle_custom(buf, p, ap, &handled);
        if (st != SUCCESS) return st;
        if (handled) {
            p += 2;
            continue;
        }

        st = handle_standard(buf, &p, ap);
        if (st != SUCCESS) return st;
    }
    return SUCCESS;
}

int overfprintf(FILE *stream, const char *format, ...) {
    if (stream == NULL || format == NULL) return -1;

    Buffer buf;
    if (buffer_init(&buf) != SUCCESS) return -1;

    va_list ap;
    va_start(ap, format);
    status_code st = format_to_buffer(&buf, format, &ap);
    va_end(ap);

    if (st != SUCCESS || buf.len > (size_t)INT_MAX) {
        buffer_free(&buf);
        return -1;
    }

    size_t written = fwrite(buf.data, 1, buf.len, stream);
    int result = (written == buf.len) ? (int)buf.len : -1;
    buffer_free(&buf);
    return result;
}

int oversprintf(char *dst, const char *format, ...) {
    if (dst == NULL || format == NULL) return -1;

    Buffer buf;
    if (buffer_init(&buf) != SUCCESS) return -1;

    va_list ap;
    va_start(ap, format);
    status_code st = format_to_buffer(&buf, format, &ap);
    va_end(ap);

    if (st != SUCCESS || buf.len > (size_t)INT_MAX) {
        buffer_free(&buf);
        return -1;
    }

    memcpy(dst, buf.data, buf.len + 1);
    int result = (int)buf.len;
    buffer_free(&buf);
    return result;
}

#endif /* OVERPRINTF_H */