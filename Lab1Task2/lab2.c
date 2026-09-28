#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ITERATIONS 100000

/* ===================== СТАТУС-КОДЫ ===================== */
typedef enum {
    SUCCESS = 0,
    ERROR_INVALID_ARGUMENT,
    ERROR_NO_CONVERGENCE,   /* превышено число итераций */
    ERROR_NULL_POINTER,
    ERROR_OVERFLOW          /* получили inf/nan при вычислении */
} status_code;

/* ===================== ВАЛИДАЦИЯ ВВОДА ===================== */

/* Проверяет строку из argv и превращает её в eps: конечное число из (0;1) */
status_code validate_eps(const char *input, double *result) {
    if (input == NULL || result == NULL)
        return ERROR_NULL_POINTER;

    char *endptr;
    double value = strtod(input, &endptr);

    if (endptr == input || *endptr != '\0')
        return ERROR_INVALID_ARGUMENT;

    if (!isfinite(value) || value <= 0.0 || value >= 1.0)
        return ERROR_INVALID_ARGUMENT;

    *result = value;
    return SUCCESS;
}

status_code dichotomy(double (*f)(double), double a, double b,
                       const double eps, double *result) {
    if (f == NULL || result == NULL) return ERROR_NULL_POINTER;

    double mid;
    int n = 0;
    while (n < MAX_ITERATIONS && (b - a) > eps) {
        mid = a + (b - a) / 2.0;
        if (f(mid) < 0.0) a = mid; else b = mid;
        n++;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = a + (b - a) / 2.0;
    return SUCCESS;
}

double f_e(double x)     { return log(x) - 1.0; }
double f_pi(double x)    { return -sin(x); }
double f_ln2(double x)   { return exp(x) - 2.0; }
double f_sqrt2(double x) { return x * x - 2.0; }

status_code calc_e_eq(const double eps, double *result) {
    return dichotomy(f_e, 2.0, 3.0, eps, result);
}
status_code calc_pi_eq(const double eps, double *result) {
    return dichotomy(f_pi, 3.0, 4.0, eps, result);
}
status_code calc_ln2_eq(const double eps, double *result) {
    return dichotomy(f_ln2, 0.0, 1.0, eps, result);
}
status_code calc_sqrt2_eq(const double eps, double *result) {
    return dichotomy(f_sqrt2, 1.0, 2.0, eps, result);
}

/* =====================================================================
   Общий приём для всех функций "предел"/"ряд": считаем текущий член
   последовательности, сравниваем с предыдущим (или с суммой) по модулю
   с точностью eps, и обязательно ограничиваем число шагов MAX_ITERATIONS.
   Общий приём для всех "уравнений": метод дихотомии на отрезке, где
   функция меняет знак, тоже с ограничением по числу итераций.
   ===================================================================== */

/* ===================== e ===================== */

/* e = lim (1 + 1/n)^n, n -> inf */
status_code calc_e_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 1;
    double prev = 0.0;
    double current = 2.0; /* значение при n = 1 */

    while (n < MAX_ITERATIONS && fabs(current - prev) > eps) {
        prev = current;
        n++;
        current = pow(1.0 + 1.0 / n, n);
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = current;
    return SUCCESS;
}

/* e = сумма 1/n! */
status_code calc_e_series(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 1;
    double sum = 1.0; /* 1/0! */
    double term = 1.0;

    while (n < MAX_ITERATIONS && fabs(term) >= eps) {
        term /= n; /* term = 1/n! */
        sum += term;
        n++;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = sum;
    return SUCCESS;
}

/* ===================== pi ===================== */

/* pi = lim (2^n * n!)^4 / (n * ((2n)!)^2).
   n! и (2n)! растут слишком быстро для double, поэтому считаем не саму
   формулу напрямую, а отношение current(n) / current(n-1) — оно
   упрощается до простой дроби без факториалов. */
status_code calc_pi_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 1;
    double current = 4.0; /* значение при n = 1 */
    double prev = 0.0;

    while (n < MAX_ITERATIONS && fabs(current - prev) > eps) {
        prev = current;
        n++;
        double ratio = 4.0 * n * (n - 1) / ((2.0 * n - 1) * (2.0 * n - 1));
        current = prev * ratio;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = current;
    return SUCCESS;
}

/* pi = 4 * сумма (-1)^(n-1) / (2n-1)  — ряд Лейбница.
   Сходится очень медленно: при маленьком eps скорее всего вернёт
   ERROR_NO_CONVERGENCE, это ожидаемо для данного ряда. */
status_code calc_pi_series(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 0;
    double sum = 0.0;
    double term = 1.0;
    int sign = 1;

    while (n < MAX_ITERATIONS && fabs(term) >= eps) {
        n++;
        term = 4.0 * sign / (2.0 * n - 1.0);
        sum += term;
        sign = -sign;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = sum;
    return SUCCESS;
}

/* ===================== ln 2 ===================== */

/* ln2 = lim n * (2^(1/n) - 1), n -> inf.
   ВАЖНО: n начинается с 1, а не с 0 — при n=0 формула не определена
   (деление на 0 в показателе степени). current сразу хранит реальное
   значение при n=1, а не "выдуманное" число — иначе на первом же шаге
   сравнение может случайно совпасть и цикл остановится слишком рано. */
status_code calc_ln2_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 1;
    double current = 1.0; /* значение при n = 1: 1*(2^1 - 1) = 1 */
    double prev = 0.0;

    while (n < MAX_ITERATIONS && fabs(current - prev) > eps) {
        prev = current;
        n++;
        current = n * (pow(2.0, 1.0 / n) - 1.0);
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = current;
    return SUCCESS;
}

/* ln2 = сумма (-1)^(n-1) / n — тоже сходится очень медленно,
   при маленьком eps ожидаемо вернёт ERROR_NO_CONVERGENCE. */
status_code calc_ln2_series(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 0;
    double sum = 0.0;
    double term = 1.0;
    int sign = 1;

    while (n < MAX_ITERATIONS && fabs(term) >= eps) {
        n++;
        term = (double)sign / n;
        sum += term;
        sign = -sign;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = sum;
    return SUCCESS;
}

/* ===================== sqrt(2) ===================== */

/* x(n+1) = x(n) - x(n)^2/2 + 1, x(0) = -0.5  ->  предел sqrt(2) */
status_code calc_sqrt2_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int n = 0;
    double current = -0.5;  /* x0, реальный первый член */
    double prev = -1.5;     /* заведомо другое число, просто чтобы войти в цикл */

    while (n < MAX_ITERATIONS && fabs(current - prev) > eps) {
        prev = current;
        n++;
        current = prev - (prev * prev) / 2.0 + 1.0;
    }
    if (n >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = current;
    return SUCCESS;
}

/* sqrt(2) = произведение по k=2,3,4,... от 2^(2^-k) */
status_code calc_sqrt2_row(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int k = 2;
    double prod = 1.0; /* "пустое" произведение — законное начальное значение */
    double prev = 0.0;

    while (k < MAX_ITERATIONS && fabs(prod - prev) >= eps) {
        prev = prod;
        prod *= pow(2.0, pow(2.0, -(double)k));
        k++;
    }
    if (k >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = prod;
    return SUCCESS;
}

/* ===================== gamma (постоянная Эйлера-Маскерони) ===================== */

/* gamma = lim(m->inf) сумма(k=1..m) C(m,k) * (-1)^k / k * ln(k!).
   Формула математически верна, но численно НЕустойчива: C(m,k) и ln(k!)
   быстро растут, а сумма получается за счёт взаимного вычитания больших
   чисел. Поэтому при увеличении m легко словить inf/nan — в этом случае
   возвращаем ERROR_OVERFLOW. Если же после MAX_ITERATIONS шагов не
   достигли нужной точности — ERROR_NO_CONVERGENCE. Оба исхода нормальны
   для этой формулы. */
status_code calc_gamma_lim(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    double prev;
    double current = 0.0;
    int m = 1;

    do {
        prev = current;
        m++;
        current = 0.0;

        double binom = 1.0;    /* C(m, k), пересчитываем по рекурренте */
        double log_fact = 0.0; /* ln(k!) = сумма ln(i) по i от 1 до k */

        for (int k = 1; k <= m; k++) {
            binom *= (double)(m - k + 1) / k; /* C(m,k) из C(m,k-1) */
            log_fact += log((double)k);
            double sign = (k % 2 == 0) ? 1.0 : -1.0;
            current += sign * binom / k * log_fact;
        }

        if (!isfinite(current))
            return ERROR_OVERFLOW;

        /* при m = 1 первый член формулы случайно равен 0 и совпадает
           с начальным prev = 0 — это не признак сходимости, поэтому
           требуем минимум 2 шага перед проверкой eps */
    } while (m < MAX_ITERATIONS && fabs(current - prev) > eps);

    if (m >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = current;
    return SUCCESS;
}

/* gamma = -pi^2/6 + сумма(k=2..inf) (1/floor(sqrt(k))^2 - 1/k) */
status_code calc_gamma_row(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    const double pi = acos(-1.0);
    double sum = -(pi * pi) / 6.0;
    int k = 1;

    while (k < MAX_ITERATIONS) {
        k++;
        double root = floor(sqrt((double)k));
        double term = 1.0 / (root * root) - 1.0 / k;
        sum += term;

        if (fabs(term) < eps)
            break;
    }
    if (k >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = sum;
    return SUCCESS;
}

/* проверка простоты — нужна для calc_gamma_eq */
int is_prime(const int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= n / i; i++)
        if (n % i == 0) return 0;
    return 1;
}

/* gamma — из "уравнения" e^(-x) = lim(t->inf) [ ln(t) * произведение по
   простым p<=t от (p-1)/p ]. Здесь правая часть сама является пределом,
   поэтому наращиваем t, обновляем произведение только когда t простое,
   и на каждом шаге решаем тривиальное уравнение относительно x:
   x = -ln( ln(t) * произведение ). */
status_code calc_gamma_eq(const double eps, double *result) {
    if (result == NULL) return ERROR_NULL_POINTER;

    int t = 2;
    double prod = 1.0;
    double current = 0.0, prev = -1.0; /* просто чтобы войти в цикл */

    while (t < MAX_ITERATIONS && fabs(current - prev) > eps) {
        prev = current;
        if (is_prime(t))
            prod *= (double)(t - 1) / t;
        current = -log(log((double)t) * prod);
        t++;
    }
    if (t >= MAX_ITERATIONS) return ERROR_NO_CONVERGENCE;

    *result = current;
    return SUCCESS;
}

/* ===================== ВЫВОД (отдельно от вычислений) ===================== */

typedef status_code (*calc_func)(double, double *);

typedef struct {
    const char *label;
    calc_func func;
} calc_entry;

void print_error(const status_code status) {
    switch (status) {
        case ERROR_INVALID_ARGUMENT:
            printf("Ошибка: Некорректный аргумент\n");
            break;
        case ERROR_NO_CONVERGENCE:
            printf("Ошибка: Не сошлось за %d итераций\n", MAX_ITERATIONS);
            break;
        case ERROR_NULL_POINTER:
            printf("Ошибка: Нулевой указатель\n");
            break;
        case ERROR_OVERFLOW:
            printf("Ошибка: Переполнение при вычислении\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_error(ERROR_INVALID_ARGUMENT);
        return ERROR_INVALID_ARGUMENT;
    }

    double eps;
    status_code v_status = validate_eps(argv[1], &eps);
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