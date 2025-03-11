#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define M 800
#define N 800
#define P 800
#define THREADS 4

double A[M][N], B[N][P], C[M][P], C_seq[M][P];

typedef struct {
    int start;
    int end;
    double timeTaken;
} ThreadData;

void *multiplyPartial(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    for(int i = data->start; i < data->end; i++) {
        for(int j = 0; j < P; j++) {
            C[i][j] = 0.0;
            for(int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
    }
    gettimeofday(&t2, NULL);
    data->timeTaken = (t2.tv_sec - t1.tv_sec) * 1e6 + (t2.tv_usec - t1.tv_usec);
    pthread_exit(NULL);
}

int main() {
    for(int i = 0; i < M; i++)
        for(int j = 0; j < N; j++)
            A[i][j] = (double)(rand()%10);

    for(int i = 0; i < N; i++)
        for(int j = 0; j < P; j++)
            B[i][j] = (double)(rand()%10);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    for(int i = 0; i < M; i++)
        for(int j = 0; j < P; j++) {
            double sum = 0.0;
            for(int k = 0; k < N; k++)
                sum += A[i][k] * B[k][j];
            C_seq[i][j] = sum;
        }
    gettimeofday(&end, NULL);
    double seqTime = (end.tv_sec - start.tv_sec)*1e6 + (end.tv_usec - start.tv_usec);

    pthread_t threads[THREADS];
    ThreadData data[THREADS];
    int rowsPerThread = M / THREADS;
    int remainder = M % THREADS;

    gettimeofday(&start, NULL);
    int currentRow = 0;
    for(int i = 0; i < THREADS; i++) {
        data[i].start = currentRow;
        data[i].end = currentRow + rowsPerThread + (i < remainder ? 1 : 0);
        currentRow = data[i].end;
        pthread_create(&threads[i], NULL, multiplyPartial, &data[i]);
    }
    for(int i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);
    gettimeofday(&end, NULL);
    double parallelTime = (end.tv_sec - start.tv_sec)*1e6 + (end.tv_usec - start.tv_usec);

    for(int i = 0; i < THREADS; i++)
        printf("Thread %d time (us): %.2f\n", i, data[i].timeTaken);

    printf("Sequential time (us): %.2f\n", seqTime);
    printf("Parallel total time (us): %.2f\n", parallelTime);

    return 0;
}
