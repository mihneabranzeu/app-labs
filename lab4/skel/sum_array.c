#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0

const int chunks_per_proc = 5;
int* create_array (int size) {
    int* arr = (int*) malloc (size * sizeof(int));
    
    for (int i = 0; i < size; ++i) {
        arr[i] = i + 1;
    }

    return arr;
}


int main(int argc, char **argv) {
    int rank, proc, a;
    int *arr, *process_arr;
    int sum, coefficient;

    // Initialize the processes
    MPI_Init(&argc, &argv);

    // Each process has a rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // rank
    // We set the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    // The root process creates the array that it sends to all processes
    if (rank == ROOT) {
        arr = create_array(chunks_per_proc * proc);

        // Read the coefficient
        printf("Enter the coefficient:\n");
        scanf("%d", &coefficient);
    }

    // We allocate memory for the array that each process receives
    process_arr = (int*) malloc (chunks_per_proc * sizeof(int));

    // We send the array from the root process to all processes using MPI_Send
    if (rank == ROOT) {
        for (int i = 0; i < proc; ++i) {
            MPI_Send(arr + i * chunks_per_proc, chunks_per_proc, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    // We receive the array from the root process using MPI_Recv
    MPI_Recv(process_arr, chunks_per_proc, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // We broadcast the coefficient from the root process to all processes
    MPI_Bcast(&coefficient, 1, MPI_INT, ROOT, MPI_COMM_WORLD);


    // We compute the sum of the elements in the array
    sum = 0;
    for (int i = 0; i < chunks_per_proc; ++i) {
        sum += process_arr[i];
    }

    // We multiply the sum by the coefficient
    sum *= coefficient;

    // We print the sum computed by each process
    printf("Process %d computed sum %d\n", rank, sum);
    int final_sum = 0;

    // We gather the sums computed by each process in the root process using MPI_Send and recv
    for (int i = 0; i < proc; ++i) {
        if (rank == i) {
            MPI_Send(&sum, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
        }

        if (rank == ROOT) {
            MPI_Recv(&a, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            final_sum += a;   
        }
        
    }

    printf("The final sum is %d\n", final_sum);

    // We free the memory allocated for the array
    free(process_arr);

    if (rank == ROOT) {
        free(arr);
    }

    MPI_Finalize();
    return 0;
}