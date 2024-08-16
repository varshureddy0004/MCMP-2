#include <time.h>        // Include the time library for measuring runtime
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern double get_final_temperatures(int N, int maxIter, double radTemp); // External function declaration
int read_dims(char *filename);
double *read_array(char *filename, int numOfValues);
void *write_to_output_file(char *filename, double *output, int numOfValues);

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <N> <maxIter> <input file name> <output file name>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int maxIter = atoi(argv[2]);
    char *inputFileName = argv[3];
    char *outputFileName = argv[4];

    // Start measuring time
    clock_t startTime = clock();

    int numOfTemps = read_dims(inputFileName); // Use the custom function to read the number of temperatures
    if (numOfTemps <= 0)
    {
        fprintf(stderr, "Error reading number of temperatures from file.\n");
        return 1;
    }

    double *radiatorTemps = read_array(inputFileName, numOfTemps); // Use the custom function to read temperatures
    if (!radiatorTemps)
    {
        fprintf(stderr, "Error reading temperatures from file.\n");
        return 1;
    }

    double *results = malloc(numOfTemps * sizeof(double));
    if (!results)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        free(radiatorTemps);
        return 1;
    }

    for (int i = 0; i < numOfTemps; i++)
    {
        results[i] = get_final_temperatures(N, maxIter, radiatorTemps[i]);
    }

    write_to_output_file(outputFileName, results, numOfTemps); // Use the custom function to write results to file

    // End measuring time
    clock_t endTime = clock();
    double timeSpent = (double)(endTime - startTime) / CLOCKS_PER_SEC; // Calculate runtime

    printf("Runtime: %f seconds\n", timeSpent); // Print the runtime

    free(radiatorTemps);
    free(results);
    return 0;
}
