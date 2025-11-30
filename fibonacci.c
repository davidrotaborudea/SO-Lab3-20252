#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    unsigned long long* arr;
    int n;
} FibArgs;

void* fib_worker(void* arg)
{
    FibArgs* data = (FibArgs*)arg;
    int n = data->n;
    unsigned long long* a = data->arr;

    if (n > 0) a[0] = 0;
    if (n > 1) a[1] = 1;

    for (int i = 2; i < n; ++i) {
        a[i] = a[i - 1] + a[i - 2];
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N debe ser > 0\n");
        return 1;
    }

    unsigned long long* fib_array =
        (unsigned long long*)malloc(N * sizeof(unsigned long long));
    if (fib_array == NULL) {
        perror("malloc");
        return 1;
    }

    FibArgs args;
    args.arr = fib_array;
    args.n   = N;

    pthread_t worker;
    int rc = pthread_create(&worker, NULL, fib_worker, &args);
    if (rc != 0) {
        fprintf(stderr, "Error creando hilo: %d\n", rc);
        free(fib_array);
        return 1;
    }

    pthread_join(worker, NULL);

    printf("Secuencia de Fibonacci (N = %d):\n", N);
    for (int i = 0; i < N; ++i) {
        printf("%llu ", fib_array[i]);
    }
    printf("\n");

    free(fib_array);

    return 0;
}
