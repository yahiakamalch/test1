#include <stdio.h>
#include <omp.h>

#define HEIGHT 5
#define WIDTH 5

void meanFilterOpenMP(int image[HEIGHT][WIDTH], int result[HEIGHT][WIDTH], int height, int width, int r) {
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

    meanFilterOpenMP(image, result, HEIGHT, WIDTH, filterRadius);

    printf("OpenMP Filtered Image:\n");
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }

    return 0;
}

