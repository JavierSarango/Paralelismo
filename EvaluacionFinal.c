#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_NODOS 100

// Función para sumar los valores
int sumar_valores(int *valores, int cantidad) {
    int suma = 0;
    for (int i = 0; i < cantidad; i++) {
        suma += valores[i];
    }
    return suma;
}

int main(int argc, char *argv[]) {
    int rango, tamaño, valor, suma_total;
    int nodos;
    double tiempo_inicio, tiempo_final;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamaño);

    if (rango == 0) {
        // Nodo raíz
        printf("Ingrese la cantidad de nodos (debe ser una potencia de 2): ");
        scanf("%d", &nodos);
        if (nodos <= 0 || nodos > MAX_NODOS) {
            if (rango == 0) {
                printf("Número de nodos inválido. Debe ser entre 1 y %d.\n", MAX_NODOS);
            }
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        
        printf("Ingrese el valor a enviar: ");
        scanf("%d", &valor);

        tiempo_inicio = MPI_Wtime();
        
        // Enviar el valor a todos los procesos
        for (int i = 1; i < tamaño; i++) {
            MPI_Send(&valor, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Otros nodos
        MPI_Recv(&valor, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Determinar el número de hijos de este nodo
    int cantidad_hijos = 2;
    while (cantidad_hijos <= tamaño) {
        cantidad_hijos *= 2;
    }
    cantidad_hijos /= 2;

    int suma_parcial = valor;
    if (rango != 0) {
        int nodo_padre = (rango - 1) / 2;
        MPI_Send(&valor, 1, MPI_INT, nodo_padre, 0, MPI_COMM_WORLD);
    } else {
        int recibidos = 0;
        for (int i = 0; i < cantidad_hijos; i++) {
            int valor_recibido;
            MPI_Recv(&valor_recibido, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            suma_parcial += valor_recibido;
            recibidos++;
        }
        suma_total = suma_parcial;
        tiempo_final = MPI_Wtime();
        printf("Suma total: %d\n", suma_total);
        printf("Tiempo de ejecución: %f segundos\n", tiempo_final - tiempo_inicio);
    }

    MPI_Finalize();
    return 0;
}
