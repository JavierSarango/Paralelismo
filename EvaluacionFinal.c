#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define RAIZ 0

// Función para enviar valores a los nodos hijos
void enviar_a_hijos(int valor, int nodo_actual, int num_nodos, int num_procesos) {
    int hijo_izquierdo = nodo_actual * 2 + 1;
    int hijo_derecho = nodo_actual * 2 + 2;

    if (hijo_izquierdo < num_nodos && hijo_izquierdo < num_procesos) {
        MPI_Send(&valor, 1, MPI_INT, hijo_izquierdo, 0, MPI_COMM_WORLD);
    }
    if (hijo_derecho < num_nodos && hijo_derecho < num_procesos) {
        MPI_Send(&valor, 1, MPI_INT, hijo_derecho, 0, MPI_COMM_WORLD);
    }
}

// Función para recibir valores de los nodos hijos y sumar los valores
int recibir_y_sumar(int nodo_actual, int num_nodos, int num_procesos) {
    int num_hijos = 2;
    int hijo_izquierdo = nodo_actual * 2 + 1;
    int hijo_derecho = nodo_actual * 2 + 2;
    int suma = 0;
    
    for (int i = 0; i < num_hijos; i++) {
        int hijo = (i == 0) ? hijo_izquierdo : hijo_derecho;
        if (hijo < num_nodos && hijo < num_procesos) {
            int valor_recibido;
            MPI_Recv(&valor_recibido, 1, MPI_INT, hijo, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            suma += valor_recibido;
            printf("Nodo %d recibió valor %d de su hijo %d\n", nodo_actual, valor_recibido, hijo);
        }
    }
    return suma;
}

// Función principal
int main(int argc, char *argv[]) {
    int rango, num_procesos;
    int valor_inicial;
    int num_nodos;
    double tiempo_inicio, tiempo_fin;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos);

    if (argc != 3) {
        if (rango == RAIZ) {
            fprintf(stderr, "Uso: %s <valor_inicial> <num_nodos>\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    valor_inicial = atoi(argv[1]);
    num_nodos = atoi(argv[2]);

    if (rango == RAIZ) {
        printf("Número total de nodos en el árbol: %d\n", num_nodos);
        printf("Número total de procesos: %d\n", num_procesos);
        tiempo_inicio = MPI_Wtime();
    }

    // Calcular el índice del nodo actual para cada proceso
    int nodo_actual = rango;

    // Lógica de envío y recepción de mensajes
    if (nodo_actual < num_nodos) {
        if (nodo_actual == RAIZ) {
            enviar_a_hijos(valor_inicial, nodo_actual, num_nodos, num_procesos);
            int resultado = recibir_y_sumar(nodo_actual, num_nodos, num_procesos) + valor_inicial;
            printf("Resultado final en el nodo 0: %d\n", resultado);
        } else {
            int valor_recibido;
            MPI_Recv(&valor_recibido, 1, MPI_INT, RAIZ, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int valor_suma = valor_recibido;
            printf("Nodo %d recibió valor %d de su padre\n", nodo_actual, valor_recibido);

            if (nodo_actual * 2 + 1 < num_nodos) {
                enviar_a_hijos(valor_suma, nodo_actual, num_nodos, num_procesos);
                int suma_hijos = recibir_y_sumar(nodo_actual, num_nodos, num_procesos);
                valor_suma += suma_hijos;
            }

            MPI_Send(&valor_suma, 1, MPI_INT, RAIZ, 0, MPI_COMM_WORLD);
            printf("Nodo %d envió suma parcial %d al nodo padre\n", nodo_actual, valor_suma);
        }
    }

    if (rango == RAIZ) {
        tiempo_fin = MPI_Wtime();
        printf("Tiempo de ejecución: %f segundos\n", tiempo_fin - tiempo_inicio);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
