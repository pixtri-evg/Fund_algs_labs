#include "functions3.h"

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 5) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    const char *substring = argv[1];
    const size_t file_count = argc - 2;

    FileResult *results = NULL;
    status_code status = SUCCESS;

    /* Пути лежат в массиве argv, а вызвать функцию с "..." по массиву нельзя,
       поэтому вызываем её отдельно для каждого количества файлов */
    switch (file_count) {
    case 1:
        status = search_in_files(&results, substring, 1, argv[2]);
        break;
    case 2:
        status = search_in_files(&results, substring, 2, argv[2], argv[3]);
        break;
    case 3:
        status = search_in_files(&results, substring, 3, argv[2], argv[3], argv[4]);
        break;
    default:
        status = ERROR_INVALID_ARGS;
        break;
    }

    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    print_results(results, file_count);
    free_results(results, file_count);
    return SUCCESS;
}