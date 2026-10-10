#include "functions10.h"

int main(void) {
    char *base_token = NULL;
    status_code status = read_token(stdin, &base_token);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }
    if (base_token == NULL) {
        print_error(ERROR_INVALID_BASE);
        return ERROR_INVALID_BASE;
    }

    int base = 0;
    status = parse_base(base_token, &base);
    free(base_token);      
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    long long sum = 0;
    long long max_abs = 0;
    status = read_numbers(stdin, base, &sum, &max_abs);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    status = print_in_bases("Максимальное по модулю:", max_abs, base);
    if (status == SUCCESS) {
        status = print_in_bases("Сумма:", sum, base);
    }
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    return SUCCESS;
}