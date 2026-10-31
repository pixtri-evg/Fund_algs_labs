#include "functions2.h"

typedef status_code (*calc_func)(double, double *);

typedef struct {
    const char *label;
    calc_func func;
} calc_entry;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    double eps;
    status_code v_status = parse_eps(argv[1], &eps);
    if (v_status != SUCCESS) {
        print_error(v_status);
        return v_status;
    }

    const calc_entry table[] = {
        {"e     (предел)   ", calc_e_lim},
        {"e     (ряд)      ", calc_e_series},
        {"e     (уравнение)", calc_e_eq},
        {"pi    (предел)   ", calc_pi_lim},
        {"pi    (ряд)      ", calc_pi_series},
        {"pi    (уравнение)", calc_pi_eq},
        {"ln2   (предел)   ", calc_ln2_lim},
        {"ln2   (ряд)      ", calc_ln2_series},
        {"ln2   (уравнение)", calc_ln2_eq},
        {"sqrt2 (предел)   ", calc_sqrt2_lim},
        {"sqrt2 (произвед.)", calc_sqrt2_row},
        {"sqrt2 (уравнение)", calc_sqrt2_eq},
        {"gamma (предел)   ", calc_gamma_lim},
        {"gamma (ряд)      ", calc_gamma_row},
        {"gamma (уравнение)", calc_gamma_eq}
    };
    const int table_size = sizeof(table) / sizeof(table[0]);

    printf("eps = %g\n", eps);
    for (int i = 0; i < table_size; i++) {
        double value = 0.0;
        status_code status = table[i].func(eps, &value);
        printf("%s : ", table[i].label);
        if (status == SUCCESS)
            printf("%.15f\n", value);
        else
            print_error(status);
    }

    return SUCCESS;
}