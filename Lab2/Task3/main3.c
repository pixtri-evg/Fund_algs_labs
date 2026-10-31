#include "functions3.h"

int main(void) {
    FileResult *results = NULL;
    status_code status;

    status = search_in_files(&results, "ab", 1, "a.txt");
    if (status == SUCCESS) print_results(results, 1);
    else print_error(status);
    free_results(results, 1);
    results = NULL;

    status = search_in_files(&results, "aa", 1, "b.txt");
    if (status == SUCCESS) print_results(results, 1);
    else print_error(status);
    free_results(results, 1);
    results = NULL;

    status = search_in_files(&results, "ab\ncd", 1, "c.txt");
    if (status == SUCCESS) print_results(results, 1);
    else print_error(status);
    free_results(results, 1);
    results = NULL;

    status = search_in_files(&results, "ab\ncd", 1, "c.txt");
    if (status == SUCCESS) print_results(results, 1);
    else print_error(status);
    free_results(results, 1);
    results = NULL;

    status = search_in_files(&results, "one two", 1, "d.txt");
    if (status == SUCCESS) print_results(results, 1);
    else print_error(status);
    free_results(results, 1);
    results = NULL;

    status = search_in_files(&results, "ab", 3, "a.txt", "empty.txt", "b.txt");
    if (status == SUCCESS) print_results(results, 3);
    else print_error(status);
    free_results(results, 3);
    results = NULL;

    status = search_in_files(&results, "ab", 1, "no_such_file.txt");
    if (status == SUCCESS) print_results(results, 1);
    else print_error(status);
    free_results(results, 1);
    results = NULL;

    return SUCCESS;
}