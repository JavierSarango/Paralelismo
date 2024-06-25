#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char **argv)
{
    int N, i;
    int *V;
    int sum = 0;
    double tiempoInicio, tiempoFin;
    int rank, size;
    printf("Ejecución en Paralela\n\n");
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    

    if (rank == 0)
    {
        printf("\nLongitud del vector: ");
        scanf("%d", &N); // Multiplo del numero de procesos
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    V = (int *)malloc(sizeof(int) * N / size);

    tiempoInicio = MPI_Wtime();

    srand(rank);
    for (i = 0; i < N / size; i++)
        V[i] = rand() % 100 - 50; // inicializacion del vector

    MPI_Reduce(&V[0], &V[0], N / size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        sum = V[0];
        for (i = 1; i < N / size; i++)
            V[i] = V[i] * sum;
        tiempoFin = MPI_Wtime();
        // Imprimir unos resultados
        printf("\n sum = %d\n", sum);
        printf("\n Tiempo de ejecucion: %.6f segundos\n", tiempoFin - tiempoInicio);
        printf("\n V[0] = %d, V[N/2] = %d, V[N-1] = %d\n\n\n", V[0], V[N / 2 / size], V[N - 1]);
    }

    MPI_Finalize();

    return 0;
}
