#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0

// Función para enviar valores a los nodos hijos
void enviar_a_hijos(int valor, int num_procesos) {
    int num_hijos = 2;
    int hijos[num_hijos];
    for (int i = 0; i < num_hijos; i++) {
        hijos[i] = ROOT * num_hijos + i + 1;
    }

    for (int i = 0; i < num_hijos && hijos[i] < num_procesos; i++) {
        MPI_Send(&valor, 1, MPI_INT, hijos[i], 0, MPI_COMM_WORLD);
    }
}

// Función para recibir valores de los nodos hijos y sumar los valores
int recibir_y_sumar(int num_procesos) {
    int num_hijos = 2;
    int suma = 0;
    for (int i = 0; i < num_hijos && ROOT * num_hijos + i + 1 < num_procesos; i++) {
        int valor_recibido;
        MPI_Recv(&valor_recibido, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        suma += valor_recibido;
    }
    return suma;
}

// Función principal
int main(int argc, char *argv[]) {
    int rango, num_procesos;
    int valor_inicial;
    double tiempo_inicio, tiempo_fin;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos);

    if (argc != 2) {
        if (rango == ROOT) {
            fprintf(stderr, "Uso: %s <valor_inicial>\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    valor_inicial = atoi(argv[1]);

    if (rango == ROOT) {
        tiempo_inicio = MPI_Wtime();
    }

    // Lógica de envío y recepción de mensajes
    if (rango == ROOT) {
        enviar_a_hijos(valor_inicial, num_procesos);
        int resultado = recibir_y_sumar(num_procesos) + valor_inicial;
        if (num_procesos > 1) {
            printf("Resultado final en el nodo 0: %d\n", resultado);
        }
    } else {
        int valor_recibido;
        MPI_Recv(&valor_recibido, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int valor_suma = valor_recibido;
        if (rango * 2 + 1 < num_procesos) {
            enviar_a_hijos(valor_suma, num_procesos);
            int suma_hijos = recibir_y_sumar(num_procesos);
            valor_suma += suma_hijos;
        }

        MPI_Send(&valor_suma, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
    }

    if (rango == ROOT) {
        tiempo_fin = MPI_Wtime();
        printf("Tiempo de ejecución: %f segundos\n", tiempo_fin - tiempo_inicio);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
