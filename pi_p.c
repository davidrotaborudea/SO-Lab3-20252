/*
 *  OpenMP lecture exercises (adaptado a Pthreads)
 *  Copyright (C) 2011 by Christian Terboven <terboven@rz.rwth-aachen.de>
 *
 *  Este programa ha sido modificado para paralelizar el cálculo de pi
 *  usando hilos POSIX (Pthreads).
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

double f(double a);
double CalcPi(int n, int num_threads);
double GetTime(void);
void* thread_func(void* arg);

typedef struct {
    int start;
    int end;
    double h;
} ThreadArgs;

double GetTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

void* thread_func(void* arg)
{
    ThreadArgs* data = (ThreadArgs*)arg;
    int start = data->start;
    int end   = data->end;
    double h  = data->h;

    double local_sum = 0.0;
    double fX;

    for (int i = start; i < end; ++i) {
        fX = h * ((double)i + 0.5);
        local_sum += f(fX);
    }

    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) {
        perror("malloc");
        pthread_exit(NULL);
    }
    *result = local_sum;
    pthread_exit(result);
}

int main(int argc, char **argv)
{
    int n = 2000000000;
    const double fPi25DT = 3.141592653589793238462643;
    double fPi;
    double fTimeStart, fTimeEnd;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <num_hilos>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "El número de hilos debe ser > 0\n");
        return 1;
    }
    

    if (n <= 0 || n > 2147483647 ) 
    {
        printf("\ngiven value has to be between 0 and 2147483647\n");
        return 1;
    }
    
    fTimeStart = GetTime();

    fPi = CalcPi(n, num_threads);

    fTimeEnd = GetTime();
    
    printf("\npi is approximately = %.20f \nError               = %.20f\n",
           fPi, fabs(fPi - fPi25DT));
    
    printf("Tiempo CalcPi paralelo con %d hilos: %f segundos\n",
           num_threads, fTimeEnd - fTimeStart);

    return 0;
}

double f(double a)
{
    return (4.0 / (1.0 + a*a));
}

double CalcPi(int n, int num_threads)
{
    const double fH = 1.0 / (double)n;

    pthread_t* threads   = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadArgs* args     = (ThreadArgs*)malloc(num_threads * sizeof(ThreadArgs));

    if (threads == NULL || args == NULL) {
        fprintf(stderr, "Error reservando memoria para hilos\n");
        exit(1);
    }

    int base = n / num_threads;
    int rem  = n % num_threads;

    int current_start = 0;

    for (int t = 0; t < num_threads; ++t) {
        int chunk = base + (t < rem ? 1 : 0);
        int start = current_start;
        int end   = start + chunk;

        args[t].start = start;
        args[t].end   = end;
        args[t].h     = fH;

        current_start = end;

        int rc = pthread_create(&threads[t], NULL, thread_func, &args[t]);
        if (rc != 0) {
            fprintf(stderr, "Error en pthread_create (%d)\n", rc);
            exit(1);
        }
    }

    double total_sum = 0.0;

    for (int t = 0; t < num_threads; ++t) {
        void* ret;
        int rc = pthread_join(threads[t], &ret);
        if (rc != 0) {
            fprintf(stderr, "Error en pthread_join (%d)\n", rc);
            exit(1);
        }

        double* partial = (double*)ret;
        if (partial != NULL) {
            total_sum += *partial;
            free(partial);
        }
    }

    free(threads);
    free(args);

    return fH * total_sum;
}
