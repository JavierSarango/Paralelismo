#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define RAIZ 0

// Función para solicitar el número de nodos y el valor inicial
void inicializarArbol(int* numNodos, int* valorInicial) {
    if (RAIZ == 0) {
        printf("Ingrese el número de nodos para el árbol binario: ");
        scanf("%d", numNodos);
        printf("Ingrese el valor que enviará el nodo 0: ");
        scanf("%d", valorInicial);
    }
    MPI_Bcast(numNodos, 1, MPI_INT, RAIZ, MPI_COMM_WORLD);
    MPI_Bcast(valorInicial, 1, MPI_INT, RAIZ, MPI_COMM_WORLD);
}

// Función para encontrar los hijos izquierdo y derecho de un nodo dado
void encontrarHijos(int rango, int numNodos, int* hijoIzquierdo, int* hijoDerecho) {
    *hijoIzquierdo = 2 * rango + 1;
    *hijoDerecho = 2 * rango + 2;
    if (*hijoIzquierdo >= numNodos) {
        *hijoIzquierdo = -1;
    }
    if (*hijoDerecho >= numNodos) {
        *hijoDerecho = -1;
    }
}

// Función para realizar el recorrido del árbol y suma parcial
int recorrerArbol(int rango, int valor, int numNodos) {
    int hijoIzquierdo, hijoDerecho;
    encontrarHijos(rango, numNodos, &hijoIzquierdo, &hijoDerecho);

    int sumaIzquierda = 0, sumaDerecha = 0;

    if (hijoIzquierdo != -1) {
        MPI_Send(&valor, 1, MPI_INT, hijoIzquierdo % numNodos, 0, MPI_COMM_WORLD);
        MPI_Recv(&sumaIzquierda, 1, MPI_INT, hijoIzquierdo % numNodos, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (hijoDerecho != -1) {
        MPI_Send(&valor, 1, MPI_INT, hijoDerecho % numNodos, 0, MPI_COMM_WORLD);
        MPI_Recv(&sumaDerecha, 1, MPI_INT, hijoDerecho % numNodos, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int sumaParcial = valor + sumaIzquierda + sumaDerecha;
    printf("Nodo %d: suma parcial = %d\n", rango, sumaParcial);

    if (rango != RAIZ) {
        MPI_Send(&sumaParcial, 1, MPI_INT, (rango - 1) / 2, 0, MPI_COMM_WORLD);
    } else {
        printf("Resultado final de la suma en el nodo raíz: %d\n", sumaParcial);
    }

    return sumaParcial;
}

int main(int argc, char** argv) {
    int rango, tamanio;
    int numNodos, valorInicial;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanio);

    // Inicializar el número de nodos y el valor inicial
    inicializarArbol(&numNodos, &valorInicial);

    // Ajustar el número de nodos al número de procesos disponibles
    if (numNodos > tamanio) {
        printf("El número de nodos es mayor que el número de procesos disponibles. Algunos procesos manejarán múltiples nodos.\n");
    }

    int sumaFinal = recorrerArbol(rango, valorInicial, numNodos);

    MPI_Finalize();
    return 0;
}
