#include "functions6.h"

int main(void) {
    const double eps1 = 1e-3;   
    const double eps2 = 1e-9;
    int convex;
    double value;
    const char *found[6];
    int found_count;
 
    printf("--- 1. Выпуклость ---\n");
    if (!report(is_convex(&convex, eps2, 4, 0.0, 0.0, 2.0, 0.0, 2.0, 2.0, 0.0, 2.0)))
        printf("Квадрат: %s\n", convex ? "выпуклый" : "невыпуклый");
    if (!report(is_convex(&convex, eps2, 5, 0.0, 0.0, 2.0, 0.0, 1.0, 1.0, 2.0, 2.0, 0.0, 2.0)))
        printf("Стрела: %s\n", convex ? "выпуклый" : "невыпуклый");
 
    printf("\n--- 2. Многочлен ---\n");
    if (!report(poly_value(&value, 2.0, 2, 2.0, -3.0, 1.0)))
        printf("2x^2 - 3x + 1 при x=2: %g\n", value);
 
    printf("\n--- 3. Числа Капрекара (основание 10) ---\n");
    if (!report(find_kaprekar(found, &found_count, 10, 6, "9", "45", "297", "10", "55", "7"))) {
        for (int i = 0; i < found_count; i++) printf("%s ", found[i]);
        printf("\n");
    }
 
    printf("\n--- 4. Среднее геометрическое ---\n");
    if (!report(geom_mean(&value, 4, 1.0, 3.0, 9.0, 27.0))) printf("gm(1, 3, 9, 27) = %g\n", value);
 
    printf("\n--- 5. Быстрое возведение в степень ---\n");
    if (!report(fast_pow(&value, 2.0, 10, eps2))) printf("2^10 = %g\n", value);
    if (!report(fast_pow(&value, 2.0, -3, eps2))) printf("2^-3 = %g\n", value);
 
    printf("\n--- 6. Дихотомия ---\n");
    if (!report(bisection(&value, 0.0, 2.0, eps1, f1))) printf("x^2-2 на [0,2], eps1=1e-3: %.6f\n", value);
    if (!report(bisection(&value, 0.0, 2.0, eps2, f1))) printf("x^2-2 на [0,2], eps2=1e-9: %.9f\n", value);
    if (!report(bisection(&value, 0.0, 1.0, eps2, f2))) printf("cos(x)-x на [0,1]: %.9f\n", value);
 
    /* По одному примеру на каждый код ошибки */
    printf("\n--- Ошибки ---\n");
    report(is_convex(NULL, eps2, 3, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0));            /* NULL_POINTER */
    report(is_convex(&convex, eps2, 2, 0.0, 0.0, 1.0, 1.0));                    /* INVALID_ARGS: 2 вершины */
    report(find_kaprekar(found, &found_count, 10, 1, "29ц8е40цк349"));         /* INVALID_NUMBER */
    report(find_kaprekar(found, &found_count, 10, 1, "294849295899"));         /* OVERFLOW */
    report(geom_mean(&value, 2, 4.0, -1.0));                                   /* DOMAIN */
    report(bisection(&value, 3.0, 4.0, eps2, f1));                             /* NO_ROOT */
    report(bisection(&value, 0.0, 2.0, 1e-30, f1));                            /* ITERATIONS */
 
    return SUCCESS;
}