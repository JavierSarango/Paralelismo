#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int N, i, sum = 0, total_sum = 0;
    int *V, *local_V;
    double tiempoInicio, tiempoFin;
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("Ejecución en Paralelo\n\n");
        printf("Longitud del vector: \n");
        scanf("%d", &N); // Multiplo del numero de procesos
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    V = (int *)malloc(sizeof(int) * N);
    local_V = (int *)malloc(sizeof(int) * N / size);

    tiempoInicio = MPI_Wtime();

    if (rank == 0)
    {
        for (i = 0; i < N; i++)
            V[i] = rand() % 100 - 50; // inicializacion del vector
    }

    MPI_Scatter(V, N / size, MPI_INT, local_V, N / size, MPI_INT, 0, MPI_COMM_WORLD);

    for (i = 0; i < N / size; i++)
        sum += local_V[i];

    MPI_Reduce(&sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    for (i = 0; i < N / size; i++)
        local_V[i] = local_V[i] * total_sum;

    MPI_Gather(local_V, N / size, MPI_INT, V, N / size, MPI_INT, 0, MPI_COMM_WORLD);

    tiempoFin = MPI_Wtime();

    if (rank == 0)
    {
        printf("\n sum = %d\n", total_sum);
        printf("\n Tiempo de ejecucion: %.6f segundos\n", tiempoFin - tiempoInicio);
        printf("\n V[0] = %d, V[N/2] = %d, V[N-1] = %d\n\n\n", V[0], V[N / 2], V[N - 1]);
    }

    MPI_Finalize();

    return 0;
}