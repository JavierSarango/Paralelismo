#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// Definición de la función que se va a integrar
// f(x) = (x^3) + (x^2) + (x + 1)
double f(double x) {
    return exp(2 * pow(x, 2));
}

// Regla trapezoidal para un intervalo dado
double regla_Trapezoidal(double local_a, double local_b, int local_n, double h, int rank) {
    // double resultado;
    // double x;
    // int i;

    // resultado = (f(local_a) + f(local_b)) / 2.0;
    // x = local_a;
    // for (i = 1; i <= local_n-1; i++) {
    //     x += h;
    //     resultado += f(x);
    // }
    // resultado *= h;

    // return resultado;

     double resultado;
    double x;
    int i;

    resultado = (f(local_a) + f(local_b)) / 2.0;
    x = local_a;
    printf("Proceso %d: x = %f, f(x) = %f\n", rank, x, f(x));  // Mostrar el primer valor
    for (i = 1; i <= local_n-1; i++) {
        x += h;
        resultado += f(x);
        printf("Proceso %d: x = %f, f(x) = %f\n", rank, x, f(x));  // Mostrar los valores intermedios
    }
    resultado *= h;
    printf("Proceso %d: x = %f, f(x) = %f\n", rank, local_b, f(local_b));  // Mostrar el último valor

    return resultado;
}

int main(int argc, char** argv) {
    int rank, size, n, local_n;
    double a, b, h, local_a, local_b;
    double areaCalculada, areaMostrar;
    double tiempoInicio, tiempoFin;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
     if (rank == 0) {
        printf("Ingrese a, b, y n\n");
        scanf("%lf %lf %d", &a, &b, &n);
    }

    MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular h y local_n
    h = (b-a)/n;
    local_n = n/size;

    // Calculo de los intervalos de manera local, en donde:
    /*
    local_a es el extremo inferior de la regla trapezoidal y
    local_b es el extremo superior
    */
    local_a = a + rank*local_n*h;
    local_b = local_a + local_n*h;

    // Sincronización y inicio de toma de tiempo
    MPI_Barrier(MPI_COMM_WORLD);
    tiempoInicio = MPI_Wtime();

    // Calcular el area con la formula de la regla trapezoidal
    areaCalculada = regla_Trapezoidal(local_a, local_b, local_n, h);
    
    // El reduce obtiene el area de todos los procesos
    MPI_Reduce(&areaCalculada, &areaMostrar, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Capturar el tiempo de ejecución de finalizacion
    tiempoFin = MPI_Wtime();
    printf("Proceso %d: Intervalo [%f, %f], Area calculada = %.15e\n", rank, local_a, local_b, areaCalculada);
    // Mostrar resultado por parte del proceso 0
    if (rank == 0) {
        printf("Con n = %d trapezoides\n", n);
        printf("El area bajo la curva de %f a %f = %.15e\n", a, b, areaMostrar);
        printf("El tiempo de ejecución fue %f segundos\n", tiempoFin - tiempoInicio);
    }

    MPI_Finalize();

    return 0;
}
