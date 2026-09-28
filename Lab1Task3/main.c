#include <functions.h>

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
        case 'q': {
            if (argc != 6) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }
 
            double eps, coeffs[3];
            status = parse_eps(argv[2], &eps);
            for (int i = 0; status == SUCCESS && i < 3; i++)
                status = parse_double(argv[3 + i], &coeffs[i]);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }
 
            double perms[6][3];
            int perm_count = 0;
            status = build_unique_permutations(coeffs, eps, perms, &perm_count);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }
 
            for (int i = 0; i < perm_count; i++) {
                const double a = perms[i][0], b = perms[i][1], c = perms[i][2];
                int root_count = 0;
                double x1 = 0.0, x2 = 0.0;
                solve_quadratic(a, b, c, eps, &root_count, &x1, &x2);
 
                printf("%.6g*x^2 + %.6g*x + %.6g = 0  ->  ", a, b, c);
                if (root_count == -1) printf("Верно при любом x\n");
                else if (root_count == 0) printf("Действительных корней нет\n");
                else if (root_count == 1) printf("x = %.10f\n", x1);
                else printf("x1 = %.10f, x2 = %.10f\n", x1, x2);
            }
            break;
        }
 
        case 'm': {
            if (argc != 4) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }
 
            long a, b;
            status = parse_long(argv[2], &a);
            if (status == SUCCESS)
                status = parse_long(argv[3], &b);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }
 
            int is_multiple;
            status = check_multiplicity(a, b, &is_multiple);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }
 
            if (is_multiple) 
                printf("%ld кратно %ld\n", a, b);
            else 
                printf("%ld не кратно %ld\n", a, b);
            break;
        }
 
        case 't': {
            if (argc != 6) {
                print_error(ERROR_INVALID_ARGS);
                return ERROR_INVALID_ARGS;
            }
 
            double eps, sides[3];
            status = parse_eps(argv[2], &eps);
            for (int i = 0; status == SUCCESS && i < 3; i++)
                status = parse_double(argv[3 + i], &sides[i]);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }
 
            int is_triangle, is_right;
            status = check_right_triangle(sides[0], sides[1], sides[2], eps, &is_triangle, &is_right);
            if (status != SUCCESS) {
                print_error(status);
                return status;
            }
 
            if (is_right)
                printf("Стороны %.6g, %.6g, %.6g образуют прямоугольный треугольник\n",
                       sides[0], sides[1], sides[2]);
            else
                printf("Стороны %.6g, %.6g, %.6g не образуют прямоугольный треугольник\n",
                       sides[0], sides[1], sides[2]);
            break;
        }
 
        default:
            print_error(ERROR_INVALID_FLAG);
            return ERROR_INVALID_FLAG;
    }
 
    return SUCCESS;
}
