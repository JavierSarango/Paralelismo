#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
MPI_Init(&argc, &argv);

int rank, size;
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

double a, b, h;
int N;

if (rank == 0) {
    printf("Ingresa los valores de a, b y N:\n");
    scanf("%lf %lf %d", &a, &b, &N);
}

MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

h = (b - a) / N;

double start_time, end_time;
start_time = MPI_Wtime();

double *x = malloc(N * sizeof(double));
double *f = malloc(N * sizeof(double));

MPI_Scatter(&a, N / size, MPI_DOUBLE, x, N / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

double area_local = 0.0;
int i;
for (i = 0; i < N / size; i++) {
    double x1 = x[i];
    double x2 = x[i] + h;
    double f1 = x1 * x1; // Ejemplo de función: x^2
    double f2 = x2 * x2;
    area_local += (f1 + f2) * h / 2;
}

MPI_Gather(&area_local, 1, MPI_DOUBLE, f, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

MPI_Barrier(MPI_COMM_WORLD);

double area_global = 0.0;
if (rank == 0) {
    for (i = 0; i < size; i++) {
        area_global += f[i];
    }
    printf("Área bajo la curva: %.6f\n", area_global);
    printf("Tiempo de ejecución: %.6f segundos\n", MPI_Wtime() - start_time);
}

free(x);
free(f);

MPI_Finalize();
return 0;
}