#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv)
{
    int N, i;
    int *V, sum = 0;
    double tiempoInicio, tiempoFin;
    printf("Ejecución en Serie\n\n");

    printf("\nLongitud del vector: ");
    scanf("%d", &N); // Multiplo del numero de procesos

    ejecucionSerie(N);
    printf("\nEjecución en MPI\n\n");
    ejecucionMPI(); 

    return 0;
}


void ejecucionSerie(int N)
{
    int i;
    int *V, sum = 0;
    double tiempoInicio, tiempoFin;
    V = (int *)malloc(sizeof(int) * N);
    tiempoInicio = clock() / (double)CLOCKS_PER_SEC;
    for (i = 0; i < N; i++)
        V[i] = rand() % 100 - 50; // inicializacion del vector
    // Procesamiento del vector
    for (i = 0; i < N; i++)
        sum = sum + V[i];
    for (i = 0; i < N; i++)
        V[i] = V[i] * sum;
    tiempoFin = clock() / (double)CLOCKS_PER_SEC;
    // Imprimir unos resultados
    printf("\n sum = %d\n", sum);
    printf("\n Tiempo de ejecucion: %.6f segundos\n", tiempoFin - tiempoInicio);
    printf("\n V[0] = %d, V[N/2] = %d, V[N-1] = %d\n\n\n", V[0], V[N / 2], V[N - 1]);

}

void ejecucionMPI()
{
    int N, i, size, rank;
    int *V, sum = 0;
    double tiempoInicio, tiempoFin;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("\nLongitud del vector: ");
        scanf("%d", &N); // Multiplo del numero de procesos
        V = (int *)malloc(sizeof(int) * N);
        tiempoInicio = MPI_Wtime();
        for (i = 0; i < N; i++)
            V[i] = rand() % 100 - 50; // inicializacion del vector
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    V = (int *)malloc(sizeof(int) * N);

    int chunk = N / size;
    int start = rank * chunk;
    int end = (rank + 1) * chunk;
    if (rank == size - 1)
        end = N;

    for (i = start; i < end; i++)
        sum = sum + V[i];

    MPI_Reduce(&sum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (i = 0; i < N; i++)
            V[i] = V[i] * sum;
        tiempoFin = MPI_Wtime();
        // Imprimir unos resultados
        printf("\n sum = %d\n", sum);
        printf("\n Tiempo de ejecucion: %.6f segundos\n", tiempoFin - tiempoInicio);
        printf("\n V[0] = %d, V[N/2] = %d, V[N-1] = %d\n\n\n", V[0], V[N / 2], V[N - 1]);
    }

    MPI_Finalize();
}
