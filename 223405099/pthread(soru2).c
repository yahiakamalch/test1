#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define HEIGHT 5
#define WIDTH 5
#define NUM_THREADS 2

typedef struct {
    int thread_id;
    int start_row;
    int end_row;
    int (*image)[WIDTH];
    int (*result)[WIDTH];
    int height;
    int width;
    int r;
} ThreadData;

void* meanFilterThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int filterSize = 2 * data->r + 1;
    int filterArea = filterSize * filterSize;

    for (int i = data->start_row; i < data->end_row; ++i) {
        for (int j = 0; j < data->width; ++j) {
            if (i < data->r || i >= data->height - data->r || j < data->r || j >= data->width - data->r) {
                data->result[i][j] = data->image[i][j];
            } else {
                int sum = 0;
                for (int k = i - data->r; k <= i + data->r; ++k) {
                    for (int l = j - data->r; l <= j + data->r; ++l) {
                        sum += data->image[k][l];
                    }
                }
                data->result[i][j] = sum / filterArea;
            }
        }
    }

    return NULL;
}

int main() {
    int image[HEIGHT][WIDTH] = {
        {100, 100, 100, 100, 100},
        {100, 150, 150, 150, 100},
        {100, 150, 200, 150, 100},
        {100, 150, 150, 150, 100},
        {100, 100, 100, 100, 100}
    };

    int result[HEIGHT][WIDTH] = {0};
    int filterRadius = 1;

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    int rows_per_thread = HEIGHT / NUM_THREADS;

    // Initialize and create threads
    for (int t = 0; t < NUM_THREADS; ++t) {
        threadData[t].thread_id = t;
        threadData[t].start_row = t * rows_per_thread;
        threadData[t].end_row = (t == NUM_THREADS - 1) ? HEIGHT : (t + 1) * rows_per_thread;
        threadData[t].image = image;
        threadData[t].result = result;
        threadData[t].height = HEIGHT;
        threadData[t].width = WIDTH;
        threadData[t].r = filterRadius;

        pthread_create(&threads[t], NULL, meanFilterThread, &threadData[t]);
    }

    // Join threads
    for (int t = 0; t < NUM_THREADS; ++t) {
        pthread_join(threads[t], NULL);
    }

    // Print the filtered image
    printf("Pthread Filtered Image:\n");
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }

    return 0;
}
