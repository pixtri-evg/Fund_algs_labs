#include "functions1.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    char flag = '\0';
    status_code status = parse_flag(argv[1], &flag);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    switch (flag) {
        case 'l': {
            if (argc != 3) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }

            size_t length = 0;
            status = get_length(argv[2], &length);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }
            printf("Длина строки: %zu\n", length);
            break;
        }

        case 'r': {
            if (argc != 3) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }

            char *result = NULL;
            status = reverse_string(argv[2], &result);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Результат: %s\n", result);
            free(result);
            break;
        }

        case 'u': {
            if (argc != 3) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }

            char *result = NULL;
            status = upper_odd_string(argv[2], &result);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Результат: %s\n", result);
            free(result);
            break;
        }

        case 'n': {
            if (argc != 3) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }

            char *result = NULL;
            status = sort_chars_string(argv[2], &result);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Результат: %s\n", result);
            free(result);
            break;
        }

        case 'c': {
            if (argc < 5) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }

            unsigned int seed = 0;
            status = parse_seed(argv[3], &seed);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            const size_t count = argc - 3;
            const char **strings = (const char**)malloc(count * sizeof(const char*));
            if (strings == NULL) {
                print_error(ERROR_MEMORY);
                return ERROR_MEMORY;
            }

            strings[0] = argv[2];
            for (int i = 4; i < argc; i++)
                strings[i - 3] = argv[i];

            char *result = NULL;
            status = concat_random(strings, count, seed, &result);
            free(strings);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Результат: %s\n", result);
            free(result);
            break;
        }
        default:
            print_error(ERROR_INVALID_FLAG);
            return ERROR_INVALID_FLAG;
    }
    return SUCCESS;
}