#include "functions10.h"

int main(void) {
    double *result = NULL;
    status_code status;

    status = shift_poly(1e-9, 1.0, &result, 2, 0.0, 0.0, 1.0);
    print_poly(status, result, 2, 1.0);
    free(result);
    result = NULL;

    status = shift_poly(1e-9, 2.0, &result, 3, 0.0, 0.0, 0.0, 1.0);
    print_poly(status, result, 3, 2.0);
    free(result);
    result = NULL;

    status = shift_poly(1e-9, 0.0, &result, 2, 5.0, 3.0, 2.0);
    print_poly(status, result, 2, 0.0);
    free(result);
    result = NULL;

    status = shift_poly(1e-9, 7.0, &result, 0, 4.0);
    print_poly(status, result, 0, 7.0);
    free(result);
    result = NULL;

    status = shift_poly(1e-9, -1.0, &result, 2, 0.0, 0.0, 1.0);
    print_poly(status, result, 2, -1.0);
    free(result);
    result = NULL;

    status = shift_poly(1e-9, 2.0, &result, 2, 1.5, 0.0, 0.5);
    print_poly(status, result, 2, 2.0);
    free(result);
    result = NULL;

    status = shift_poly(1e-9, 1.0, &result, -1);
    print_poly(status, result, -1, 1.0);
    free(result);
    result = NULL;

    status = shift_poly(0.0, 1.0, &result, 1, 1.0, 1.0);
    print_poly(status, result, 1, 1.0);
    free(result);
    result = NULL;

    return SUCCESS;
}