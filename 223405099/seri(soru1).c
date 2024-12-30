#include <stdio.h>

#define HEIGHT 5
#define WIDTH 5

void meanFilterSerial(int image[HEIGHT][WIDTH], int result[HEIGHT][WIDTH], int height, int width, int r) {
    int filterSize = 2 * r + 1; // Size of the filter (3x3 for r = 1)
    int filterArea = filterSize * filterSize; // Total number of elements in the filter

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i < r || i >= height - r || j < r || j >= width - r) {
                // Handle edge cases: copy the original value
                result[i][j] = image[i][j];
            } else {
                // Compute the mean of the surrounding pixels
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
    // Example grayscale image matrix (5x5)
    int image[HEIGHT][WIDTH] = {
        {100, 100, 100, 100, 100},
        {100, 150, 150, 150, 100},
        {100, 150, 200, 150, 100},
        {100, 150, 150, 150, 100},
        {100, 100, 100, 100, 100}
    };

    int result[HEIGHT][WIDTH] = {0}; // Matrix to store the filtered image
    int filterRadius = 1; // Mean filter radius

    // Apply the mean filter
    meanFilterSerial(image, result, HEIGHT, WIDTH, filterRadius);

    // Print the resulting image
    printf("Filtered Image:\n");
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }

    return 0;
}
