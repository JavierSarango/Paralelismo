#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define numero 4;
int main(int argc, const char **argv)
{

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int matriz_a[numero][numero], matriz_b[numero][numero], matriz_c[numero][numero];
    int fila_a[numero], fila_b[numero], fila_c[numero], fila_bt[numero];

    matriz_a = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
    matriz_b = {{4, 63, 5, 2}, {4, 5, 2, 5}, {5, 2, 6, 87}, {5, 3, 6, 2}};

    if (rank == 0)
    {
        printf("Matriz A:\n");
        for (int i = 0; i < numero; i++)
        {
            for (int j = 0; j < numero; j++)
            {
                printf("%d ", matriz_a[i][j]);
            }
            printf("\n");
        }
        printf("Matriz B:\n");
        for (int i = 0; i < numero; i++)
        {
            for (int j = 0; j < numero; j++)
            {
                printf("%d ", matriz_b[i][j]);
            }
            printf("\n");
        }
    }

    int matriz_bt[numero][numero];

    if (rank == 0)
    {
        for (int i = 0; i < numero; i++)
        {
            for (int j = 0; j < numero; j++)
            {
                matriz_bt[j][i] = matriz_b[i][j];
            }
        }
    }

    if (rank == 0)
    {
        printf("Matriz Traspuesta de B:\n");
        for (int i = 0; i < numero; i++)
        {
            for (int j = 0; j < numero; j++)
            {
                printf("%d ", matriz_bt[i][j]);
            }
            printf("\n");
        }
        
    }
    

    //* Se supone que las matrices a y b están llenas
    MPI_SCatter(matriz_a, numero, MPI_INT, fila_a, numero, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(matriz_bt, numero, MPI_INT, fila_bt, numero, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < numero; i++)
    {
        fila_c[i] = fila_a[i] + fila_bt[i];
    }
    MPI_Gather(fila_c, numero, MPI_INT, matriz_c, numero, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Matriz C:\n");
        for (int i = 0; i < numero; i++)
        {
            for (int j = 0; j < numero; j++)
            {
                printf("%d ", matriz_c[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();

    return 0;
}