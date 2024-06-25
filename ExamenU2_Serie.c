#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    int N, i;
    int *V, sum = 0;
    double tiempoInicio, tiempoFin;
    printf("===================================\n");
    printf("\tEjecución en Serie\n");
    printf("===================================\n");

    printf("\nLongitud del vector: ");
    scanf("%d", &N); // Multiplo del numero de procesos

    V = (int *)malloc(sizeof(int) * N);
    printf("Con N = %d\n", N);
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
    return 0;
}

