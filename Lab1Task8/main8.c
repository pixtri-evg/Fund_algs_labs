#include "functions8.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];

    if (strcmp(input_path, output_path) == 0) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    FILE *in = fopen(input_path, "r");
    if (in == NULL) {
        print_error(ERROR_FILE_OPEN);
        return ERROR_FILE_OPEN;
    }

    FILE *out = fopen(output_path, "w");
    if (out == NULL) {
        fclose(in);
        print_error(ERROR_FILE_OPEN);
        return ERROR_FILE_OPEN;
    }

    status_code status = process_file(in, out);

    if (fclose(out) != 0 && status == SUCCESS) status = ERROR_IO;
    fclose(in);

    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    printf("Результат записан в %s\n", output_path);
    return SUCCESS;
}