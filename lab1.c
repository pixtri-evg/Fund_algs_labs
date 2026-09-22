#include "lab1.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        print_error(ERROR_INVALID_ARGS);
        return ERROR_INVALID_ARGS;
    }

    char flag = '\0';
    char *num_str = NULL;

    if (parse_flag(argv[1], &flag) == SUCCESS)
        num_str = argv[2];
    else if (parse_flag(argv[2], &flag) == SUCCESS)
        num_str = argv[1];
    else {
        print_error(ERROR_INVALID_FLAG);
        return ERROR_INVALID_FLAG;
    }

    int x = 0;
    status_code status = string_to_int(num_str, &x);
    if (status != SUCCESS) {
        print_error(status);
        return status;
    }

    switch (flag) {
        case 'h': {
            int multiplies[100];
            int multiplies_count;
            status = find_multiplies(x, multiplies, &multiplies_count);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            if (multiplies_count == 0) {
                printf("Чисел, кратных %d, в диапазоне от 1 до 100 не найдено.\n", x);
            }
            else {
                printf("Числа, кратные %d: ", x);
                for (int i = 0; i < multiplies_count; i++)
                    printf("%d ", multiplies[i]);
                printf("\n");
            }
            break;
        }

        case 'p': {
            int is_prime = 0, is_comp = 0;
            status = check_prime(x, &is_prime, &is_comp);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            if (is_prime)
                printf("Число %d является простым.\n", x);
            else if (is_comp)
                printf("Число %d является составным.\n", x);
            else
                printf("Число %d не является ни простым, ни составным.\n", x);
            break;
        }

        case 's': {
            int start_idx = 0;
            char hex_digits[sizeof(int) * 2 + 1];
            status = split_into_h(x, hex_digits,sizeof(hex_digits), &start_idx);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Шестнадцатеричные цифры числа %d: ", x);
            for (int i = start_idx; hex_digits[i] != '\0'; i++)
                printf("%c ", hex_digits[i]);

            printf("\n");
            break;
        }

        case 'e': {
            unsigned long long powers_table[10][10];
            status = calculate_powers(x, powers_table);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Таблица степеней от 1 до %d для оснований от 1 до 10:\n", x);
            for (int base = 1; base <= 10; base++) {
                for (int power = 1; power <= x; power++)
                    printf("%10llu ", powers_table[base - 1][power - 1]);
            
                printf("\n");
            }

            break;
        }

        case 'a': {
            long long sum_answer = 0;
            status = calculate_sum(x, &sum_answer);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Сумма всех натуральных чисел от 1 до %d равна: %lld\n", x, sum_answer);
            break;
        }
            
        case 'f': {
            unsigned long long fact_answer;
            status = calculate_factorial(x, &fact_answer);

            if (status != SUCCESS) {
                print_error(status);
                return status;
            }

            printf("Факториал числа %d равен: %llu\n", x, fact_answer);
            break;
        }

        default: {
            printf("Неизвестный флаг\n");
            break;
        }

    }
    return SUCCESS;
}