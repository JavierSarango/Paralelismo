#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Definición de la función a integrar
double f(double x) {
    return x * x; // Ejemplo: f(x) = x^2
}

// Regla trapezoidal para un intervalo dado
double trapezoidal_rule(double local_a, double local_b, int local_n, double h) {
    double integral;
    double x;
    int i;

    integral = (f(local_a) + f(local_b)) / 2.0;
    x = local_a;
    for (i = 1; i <= local_n-1; i++) {
        x += h;
        integral += f(x);
    }
    integral *= h;

    return integral;
}

int main(int argc, char** argv) {
    int my_rank, comm_sz, n, local_n;
    double a, b, h, local_a, local_b;
    double local_integral, total_integral;
    double start_time, end_time;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    start_time = MPI_Wtime();
    // Solo el proceso 0 obtiene los valores de entrada
    if (my_rank == 0) {
        printf("Ingrese a, b, y n\n");
        scanf("%lf %lf %d", &a, &b, &n);
    }

    // Transmitir los valores de a, b, y n a todos los procesos
    MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular h y local_n
    h = (b-a)/n;
    local_n = n/comm_sz;

    // Calcular los intervalos locales
    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;

    // Sincronización y inicio del tiempo
    MPI_Barrier(MPI_COMM_WORLD);
    

    // Calcular la integral local
    local_integral = trapezoidal_rule(local_a, local_b, local_n, h);

    // Reducir todas las integrales locales a la integral total
    MPI_Reduce(&local_integral, &total_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Sincronización y fin del tiempo
    end_time = MPI_Wtime();

    // Proceso 0 imprime el resultado y el tiempo de ejecución
    if (my_rank == 0) {
        printf("Con n = %d trapezoides, nuestra estimación\n", n);
        printf("del integral de %f a %f = %.15e\n", a, b, total_integral);
        printf("El tiempo de ejecución fue %f segundos\n", end_time - start_time);
    }

    // Finalizar MPI
    MPI_Finalize();

    return 0;
}
