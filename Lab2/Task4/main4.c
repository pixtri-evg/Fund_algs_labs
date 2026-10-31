#include "functions4.h"

int main(void) {
    FileInfo *files = NULL;
    size_t count = 0;
    long long capacity = 0;

    status_code st = read_input(&files, &count, &capacity);
    if (st != SUCCESS) {
        print_error(st);
        return 1;
    }

    Selection res;
    st = select_files(files, count, capacity, &res);
    if (st == SUCCESS) {
        print_selection(&res);
        free(res.files);
    } else {
        print_error(st);
    }

    free(files);
    return st == SUCCESS ? 0 : 1;
}