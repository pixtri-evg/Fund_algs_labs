#include "functions2.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    char order = '\0';
    status_code status = parse_flag(argv[2], &order);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    if (strcmp(argv[1], argv[3]) == 0) {
        print_error(ERROR_SAME_FILE);
        return ERROR_SAME_FILE;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        print_error(ERROR_OPEN_FILE);
        return ERROR_OPEN_FILE;
    }

    Employee *employees = NULL;
    size_t count = 0;
    status = read_employees(input, &employees, &count);
    fclose(input);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    if (count > 1) {
        if (order == 'a') qsort(employees, count, sizeof(Employee), compare_asc);
        else              qsort(employees, count, sizeof(Employee), compare_desc);
    }

    FILE *output = fopen(argv[3], "w");
    if (output == NULL) {
        free_employees(employees, count);
        print_error(ERROR_OPEN_FILE);
        return ERROR_OPEN_FILE;
    }

    status = write_employees(output, employees, count);
    if (fclose(output) != 0 && status == SUCCESS) {
        status = ERROR_WRITE;
    }
    free_employees(employees, count);

    if (status != SUCCESS) {
        print_error(status);
        return status;
    }
    return SUCCESS;
}