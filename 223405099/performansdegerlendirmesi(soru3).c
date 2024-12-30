#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>

typedef struct {
    int thread_id;
    int start_row;
    int end_row;
    int** image;
    int** result;
    int height;
    int width;
    int r;
} ThreadData;

// Serial mean filter implementation
void meanFilterSerial(int** image, int** result, int height, int width, int r) {
    int filterSize = 2 * r + 1;
    int filterArea = filterSize * filterSize;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i < r || i >= height - r || j < r || j >= width - r) {
                result[i][j] = image[i][j];
            } else {
                int sum = 0;
                for (int k = i - r; k <= i + r; ++k) {
                    for (int l = j - r; l <= j + r; ++l) {
                        sum += image[k][l];
                    }
                }
                result[i][j] = sum / filterArea;
            }
        }
    }
}

// OpenMP parallel mean filter implementation
void meanFilterOpenMP(int** image, int** result, int height, int width, int r) {
    int filterSize = 2 * r + 1;
    int filterArea = filterSize * filterSize;

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i < r || i >= height - r || j < r || j >= width - r) {
                result[i][j] = image[i][j];
            } else {
                int sum = 0;
                for (int k = i - r; k <= i + r; ++k) {
                    for (int l = j - r; l <= j + r; ++l) {
                        sum += image[k][l];
                    }
                }
                result[i][j] = sum / filterArea;
            }
        }
    }
}

// Pthread worker function
void* meanFilterPthread(void* arg) {
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

// Function to create a dynamically allocated matrix
int** createMatrix(int height, int width) {
    int** matrix = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; ++i) {
        matrix[i] = (int*)malloc(width * sizeof(int));
        for (int j = 0; j < width; ++j) {
            matrix[i][j] = rand() % 256; // Random values between 0-255
        }
    }
    return matrix;
}

// Function to free a dynamically allocated matrix
void freeMatrix(int** matrix, int height) {
    for (int i = 0; i < height; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    int sizes[] = {64, 128, 256, 512};
    int filterRadius = 1;
    int num_threads = 4;

    printf("H x W\t\tTseri (s)\tTpthread (s)\tTopenMP (s)\tSpeed-Up (OpenMP)\n");

    for (int s = 0; s < 4; ++s) {
        int size = sizes[s];
        int height = size, width = size;

        // Create matrices for the image and results
        int** image = createMatrix(height, width);
        int** resultSerial = createMatrix(height, width);
        int** resultParallel = createMatrix(height, width);
        int** resultPthread = createMatrix(height, width);

        // Measure serial execution time
        clock_t startSerial = clock();
        meanFilterSerial(image, resultSerial, height, width, filterRadius);
        clock_t endSerial = clock();
        double timeSerial = (double)(endSerial - startSerial) / CLOCKS_PER_SEC;

        // Measure Pthread execution time
        pthread_t threads[num_threads];
        ThreadData threadData[num_threads];
        int rows_per_thread = height / num_threads;

        double startPthread = omp_get_wtime();
        for (int t = 0; t < num_threads; ++t) {
            threadData[t].thread_id = t;
            threadData[t].start_row = t * rows_per_thread;
            threadData[t].end_row = (t == num_threads - 1) ? height : (t + 1) * rows_per_thread;
            threadData[t].image = image;
            threadData[t].result = resultPthread;
            threadData[t].height = height;
            threadData[t].width = width;
            threadData[t].r = filterRadius;
            pthread_create(&threads[t], NULL, meanFilterPthread, &threadData[t]);
        }
        for (int t = 0; t < num_threads; ++t) {
            pthread_join(threads[t], NULL);
        }
        double endPthread = omp_get_wtime();
        double timePthread = endPthread - startPthread;

        // Measure OpenMP parallel execution time
        double startParallel = omp_get_wtime();
        meanFilterOpenMP(image, resultParallel, height, width, filterRadius);
        double endParallel = omp_get_wtime();
        double timeParallel = endParallel - startParallel;

        // Calculate speed-up factors
        double speedUpOpenMP = (timeParallel > 0) ? timeSerial / timeParallel : 0.0;

        // Print results for the current size
        printf("%dx%d\t\t%.6f\t%.6f\t%.6f\t%.2f\n", height, width, timeSerial, timePthread, timeParallel, speedUpOpenMP);

        // Free allocated memory
        freeMatrix(image, height);
        freeMatrix(resultSerial, height);
        freeMatrix(resultParallel, height);
        freeMatrix(resultPthread, height);
    }

    return 0;
}



