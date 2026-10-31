#include "functions7.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    char action = '\0';
    status_code status = parse_flag(argv[1], &action);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    const int expected_argc = (action == 'r') ? 5 : 4;
    if (argc != expected_argc) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    const char *in1_path = argv[2];
    const char *in2_path = NULL;        
    if (action == 'r') in2_path = argv[3];
    const char *out_path = argv[argc - 1];

    if (strcmp(in1_path, out_path) == 0) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }
    if (in2_path != NULL && strcmp(in2_path, out_path) == 0) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    FILE *in1 = fopen(in1_path, "r");
    if (in1 == NULL) {
        print_error(ERROR_FILE_OPEN);
        return ERROR_FILE_OPEN;
    }

    FILE *in2 = NULL;
    if (action == 'r') {
        in2 = fopen(in2_path, "r");
        if (in2 == NULL) {
            fclose(in1);
            print_error(ERROR_FILE_OPEN);
            return ERROR_FILE_OPEN;
        }
    }

    FILE *out = fopen(out_path, "w");
    if (out == NULL) {
        fclose(in1);
        if (in2 != NULL) fclose(in2);
        print_error(ERROR_FILE_OPEN);
        return ERROR_FILE_OPEN;
    }

    if (action == 'r') status = merge_files(in1, in2, out);
    else status = process_file_a(in1, out);

    if (fclose(out) != 0 && status == SUCCESS) status = ERROR_IO;
    fclose(in1);
    if (in2 != NULL) fclose(in2);

    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    printf("Результат записан в %s\n", out_path);
    return SUCCESS;
}