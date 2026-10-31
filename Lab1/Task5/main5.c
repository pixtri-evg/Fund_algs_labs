#include "functions5.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }
 
    double eps;
    status_code status = parse_eps(argv[1], &eps);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }
 
    const term_next_func series[] = {next_a, next_b, next_c, next_d};
    const int first_n[]           = {0, 0, 0, 1};
    const real_func integrands[]  = {f_i1, f_i2, f_i3, f_i4};

    printf("x = %g, eps = %g\n\n", X, eps);

    for (int i = 0; i < 4; i++) {
        double value;
        status_code s = sum_series(series[i], first_n[i], eps, &value);
        printf("sum %c      : ", 'a' + i);
        if (s == SUCCESS) printf("%.15f\n", value);
        else print_error(s);
    }
    for (int i = 0; i < 4; i++) {
        double value;
        status_code s = integrate(integrands[i], eps, &value);
        printf("integral %c : ", 'a' + i);
        if (s == SUCCESS) printf("%.15f\n", value);
        else print_error(s);
    }
    return SUCCESS;
}