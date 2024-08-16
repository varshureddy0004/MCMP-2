#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double get_final_temperatures(int N, int maxIter, double radTemp) {
    double **prev_t = (double **)malloc(N * sizeof(double *));
    double **curr_t = (double **)malloc(N * sizeof(double *));
    maxIter = maxIter + 1;
    for (int i = 0; i < N; i++) {
        prev_t[i] = (double *)malloc(N * sizeof(double));
        curr_t[i] = (double *)malloc(N * sizeof(double));
        for (int j = 0; j < N; j++) {
            prev_t[i][j] = 10.0; // Initialize with default room temperature
            curr_t[i][j] = 10.0; // Same as above
        }
    }

    // Set radiator temperatures
    int start = floor((N - 1) * 0.3);
    int end = ceil((N - 1) * 0.7);
    for (int j = start; j <= end; j++) {
        prev_t[j][N - 1] = radTemp;
        curr_t[j][N - 1] = radTemp;
    }

    // Iterative process to update temperatures using OpenMP
    for (int iter = 0; iter < maxIter; iter++) {
        #pragma omp parallel for collapse(2)
        for (int i = 1; i < N - 1; i++) { // Skip boundary rows
            for (int j = 1; j < N - 1; j++) { // Skip boundary columns
                curr_t[i][j] = 0.25 * (prev_t[i - 1][j] + prev_t[i + 1][j] + prev_t[i][j - 1] + prev_t[i][j + 1]);
            }
        }

        #pragma omp barrier // Ensure all updates are finished

        // Swap the arrays
        double **temp = prev_t;
        prev_t = curr_t;
        curr_t = temp;
    }

    // Calculate result from the center of the room
    int pointx = floor((N - 1) * 0.5);
    int pointy = floor((N - 1) * 0.5);
    double result = curr_t[pointx][pointy];

    // Free memory
    for (int i = 0; i < N; i++) {
        free(prev_t[i]);
        free(curr_t[i]);
    }
    free(prev_t);
    free(curr_t);

    return result;
}
