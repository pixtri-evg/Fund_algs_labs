#include "functions4.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }
 
    char action = '\0';
    int has_n = 0;
    status_code status = parse_flag(argv[1], &action, &has_n);
    if (status != SUCCESS) { 
        print_error(status); 
        return status; 
    }
 
    if (argc != (has_n ? 4 : 3)) { 
        print_error(ERROR_INVALID_ARGS); 
        return ERROR_INVALID_ARGS; 
    }
 
    const char *input_path = argv[2];
    char *generated_path = NULL;
    const char *output_path = NULL;
 
    if (has_n) {
        output_path = argv[3];
        if (strcmp(input_path, output_path) == 0) {
            print_error(ERROR_INVALID_ARGS);
            return ERROR_INVALID_ARGS;
        }
    } else {
        status = make_output_path(input_path, &generated_path);
        if (status != SUCCESS) {
            print_error(status);
            return status;
        }
        output_path = generated_path;
    }
 
    FILE *in = fopen(input_path, "r");
    if (in == NULL) {
        free(generated_path);
        print_error(ERROR_FILE_OPEN);
        return ERROR_FILE_OPEN;
    }
 
    FILE *out = fopen(output_path, "w");
    if (out == NULL) {
        fclose(in);
        free(generated_path);
        print_error(ERROR_FILE_OPEN);
        return ERROR_FILE_OPEN;
    }
 
    switch (action) {
        case 'd': status = exclude_digits(in, out); break;
        case 'i': status = count_matches_per_line(in, out, is_latin_letter); break;
        case 's': status = count_matches_per_line(in, out, is_other_char); break;
        case 'a': status = replace_non_digits_hex(in, out); break;
        default:  status = ERROR_INVALID_FLAG; break;
    }
 
    if (fclose(out) != 0 && status == SUCCESS) status = ERROR_IO;  
    fclose(in);
 
    if (status != SUCCESS) {
        free(generated_path);
        print_error(status);
        return status;
    }
 
    printf("Результат записан в %s\n", output_path);
    free(generated_path);
    return SUCCESS;
}