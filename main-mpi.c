#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern double get_final_temperatures(int N, int maxIter, double radTemp); // External function declaration
int read_dims(char *filename);
double *read_array(char *filename, int numOfValues);
void *write_to_output_file(char *filename, double *output, int numOfValues);

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 5) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <N> <maxIter> <input file name> <output file name>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);
    int maxIter = atoi(argv[2]);
    char *inputFileName = argv[3];
    char *outputFileName = argv[4];

    double startTime = MPI_Wtime(); // Start the timer

    int numOfTemps = 0;
    double *radiatorTemps = NULL;

    if (rank == 0) {
        numOfTemps = read_dims(inputFileName);
        if (numOfTemps <= 0) {
            fprintf(stderr, "Error reading number of temperatures from file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        radiatorTemps = read_array(inputFileName, numOfTemps);
        if (!radiatorTemps) {
            fprintf(stderr, "Error reading temperatures from file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
    }

    MPI_Bcast(&numOfTemps, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int localNumOfTemps = numOfTemps / size + (rank < numOfTemps % size ? 1 : 0);
    double *localTemps = malloc(localNumOfTemps * sizeof(double));

    int *displs = NULL;
    int *sendcounts = NULL;
    if (rank == 0) {
        displs = malloc(size * sizeof(int));
        sendcounts = malloc(size * sizeof(int));
        int offset = 0;
        for (int i = 0; i < size; i++) {
            sendcounts[i] = numOfTemps / size + (i < numOfTemps % size ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    MPI_Scatterv(radiatorTemps, sendcounts, displs, MPI_DOUBLE, localTemps, localNumOfTemps, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double *localResults = malloc(localNumOfTemps * sizeof(double));
    if (!localResults) {
        fprintf(stderr, "Memory allocation failed.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    for (int i = 0; i < localNumOfTemps; i++) {
        localResults[i] = get_final_temperatures(N, maxIter, localTemps[i]);
    }

    double *results = NULL;
    if (rank == 0) {
        results = malloc(numOfTemps * sizeof(double));
    }

    MPI_Gatherv(localResults, localNumOfTemps, MPI_DOUBLE, results, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double endTime = MPI_Wtime(); // End the timer

    if (rank == 0) {
        write_to_output_file(outputFileName, results, numOfTemps); // Write results to file
        printf("Total runtime: %f seconds.\n", endTime - startTime);
        free(radiatorTemps);
        free(results);
        free(displs);
        free(sendcounts);
    }

    free(localTemps);
    free(localResults);

    MPI_Finalize();
    return 0;
}
