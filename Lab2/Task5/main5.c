#include "overfprintf.h"

int main(void) {
    char out[256];
    int n;

    overfprintf(stdout, "Стандартные: %d | %5.2f | %s | %c | %05d | %-6u| 100%% %p \n",
                42, 3.14159, "text", 'x', 7, 12u, &n, "oooo");
    overfprintf(stdout, "Длинные: %ld | %lld | %*d\n", 123456789012L, LLONG_MIN, 6, 42);

    overfprintf(stdout, "Ro: %Ro %Ro %Ro\n", 0, 1994, 3999);
    overfprintf(stdout, "Zr: %Zr %Zr %Zr\n", 1u, 4u, 9u);
    overfprintf(stdout, "Cv: %Cv %Cv | CV: %CV\n", 255, 16, -255, 2, 255, 16);
    overfprintf(stdout, "to: %to %to | TO: %TO\n", "ff", 16, "-1010", 2, "FF", 16);
    overfprintf(stdout, "mi: %mi\n", 5);
    overfprintf(stdout, "mu: %mu\n", 15u);
    overfprintf(stdout, "md: %md\n", 1.0);
    overfprintf(stdout, "mf: %mf\n", 1.0f);
    overfprintf(stdout, "%d %Ro %s %Cv\n", 5, 9, "ok", 255, 16);

    FILE *file = fopen("overprintf_output.txt", "w");
    if (file == NULL) {
        fprintf(stderr, "Ошибка открытия файла\n");
        return 1;
    }
    n = overfprintf(file, "В файл: %Ro, %Zr, %Cv\n", 2024, 100u, 255, 16);
    fclose(file);
    printf("Записано в файл байт: %d\n", n);

    n = oversprintf(out, "%Ro | %Zr | %CV | %to", 14, 12u, 3054, 16, "777", 8);
    printf("oversprintf: %s (длина %d)\n", out, n);


    return 0;
}