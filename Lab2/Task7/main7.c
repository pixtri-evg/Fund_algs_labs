#include "functions7.h"

#define N 3
#define VECTORS_COUNT 4
#define NORMS_COUNT 4

int main(void) {
    const double eps = 1e-9;

    const double values[VECTORS_COUNT][N] = {
        {1.0, 2.0, 2.0},
        {-4.0, 1.0, 0.0},
        {0.0, 4.0, -1.0},
        {3.0, 3.0, 3.0}
    };

    vector_t vecs[VECTORS_COUNT] = {{NULL}};
    status_code status = SUCCESS;
    for (size_t i = 0; i < VECTORS_COUNT; i++) {
        status = create_vector(N, values[i], &vecs[i]);
        if (status != SUCCESS) break;
    }
    if (status != SUCCESS) {
        print_error(status);
        for (size_t i = 0; i < VECTORS_COUNT; i++) free_vector(&vecs[i]);
        return status;
    }

    const double p_one = 1.0;
    const double p_two = 2.0;
    const double p_bad = 0.5;
    const double matrix_a[N * N] = {
        1.0, 0.0, 0.0,
        0.0, 10.0, 0.0,
        0.0, 0.0, 1.0
    };
    const double matrix_bad[N * N] = {
        -1.0, 0.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, 0.0, -1.0
    };
    const void *no_params = NULL;
    const norm_func_t no_norm = NULL;
    const vector_t empty = {NULL};

    vector_t storage[VECTORS_COUNT];
    longest_t results[NORMS_COUNT];

    printf("Все нормы:\n");
    status = find_longest_vectors(N, 4, 4, eps, storage, results,
                                  vecs[0], vecs[1], vecs[2], vecs[3],
                                  norm_inf, no_params,
                                  norm_p, (const void *)&p_one,
                                  norm_p, (const void *)&p_two,
                                  norm_a, (const void *)matrix_a);
    print_longest(status, N, 4, results);
    free_results(results, 4);

    printf("\nОдинаковые векторы:\n");
    status = find_longest_vectors(N, 3, 1, eps, storage, results,
                                  vecs[3], vecs[0], vecs[3],
                                  norm_p, (const void *)&p_two);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nОдин вектор:\n");
    status = find_longest_vectors(N, 1, 1, eps, storage, results,
                                  vecs[0],
                                  norm_inf, no_params);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nНорма p = 0.5:\n");
    status = find_longest_vectors(N, 2, 1, eps, storage, results,
                                  vecs[0], vecs[1],
                                  norm_p, (const void *)&p_bad);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nМатрица A с отрицательными элементами:\n");
    status = find_longest_vectors(N, 2, 1, eps, storage, results,
                                  vecs[0], vecs[1],
                                  norm_a, (const void *)matrix_bad);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nРазмерность 0:\n");
    status = find_longest_vectors(0, 2, 1, eps, storage, results,
                                  vecs[0], vecs[1],
                                  norm_p, (const void *)&p_one);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nНет векторов:\n");
    status = find_longest_vectors(N, 0, 1, eps, storage, results,
                                  norm_p, (const void *)&p_one);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nНет норм:\n");
    status = find_longest_vectors(N, 2, 0, eps, storage, results,
                                  vecs[0], vecs[1]);
    print_longest(status, N, 0, results);
    free_results(results, 0);

    printf("\nЭпсилон < 0:\n");
    status = find_longest_vectors(N, 2, 1, -1.0, storage, results,
                                  vecs[0], vecs[1],
                                  norm_p, (const void *)&p_one);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nВектор с NULL-координатами:\n");
    status = find_longest_vectors(N, 2, 1, eps, storage, results,
                                  vecs[0], empty,
                                  norm_p, (const void *)&p_one);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    printf("\nНорма без функции:\n");
    status = find_longest_vectors(N, 2, 1, eps, storage, results,
                                  vecs[0], vecs[1],
                                  no_norm, no_params);
    print_longest(status, N, 1, results);
    free_results(results, 1);

    for (size_t i = 0; i < VECTORS_COUNT; i++) free_vector(&vecs[i]);
    return SUCCESS;
}

