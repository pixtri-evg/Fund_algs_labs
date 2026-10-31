#include "functions9.h"

int main(void) {
    int *mas = NULL;
    statements status;

    // Основание 2
    status = check_representation(&mas, 2, 3, 0.5, 0.25, 0.3);
    if (status == correct) {
        printf("Base 2: %d %d %d\n", mas[0], mas[1], mas[2]);
        free(mas);
    }

    // Основание 3
    status = check_representation(&mas, 3, 3, 0.5, 0.25, 1.0 / 3.0);
    if (status == correct) {
        printf("Base 3: %d %d %d\n", mas[0], mas[1], mas[2]);
        free(mas);
    }

    // Основание 6
    status = check_representation(&mas, 6, 3, 0.5, 0.25, 0.3);
    if (status == correct) {
        printf("Base 6: %d %d %d\n", mas[0], mas[1], mas[2]);
        free(mas);
    }

    // Основание 10
    status = check_representation(&mas, 10, 3, 0.5, 0.125, 0.3);
    if (status == correct) {
        printf("Base 10: %d %d %d\n", mas[0], mas[1], mas[2]);
        free(mas);
    }

    // Основание 16
    status = check_representation(&mas, 16, 3, 0.5, 0.125, 0.1);
    if (status == correct) {
        printf("Base 16: %d %d %d\n", mas[0], mas[1], mas[2]);
        free(mas);
    }

    return 0;
}