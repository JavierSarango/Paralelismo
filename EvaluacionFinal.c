#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define RAIZ 0

// Función para solicitar el número de nodos y el valor inicial
void inicializarArbol(int* numNodos, int* valorInicial, int rango) {
    if (rango == RAIZ) {
        printf("Ingrese el número de nodos para el árbol binario: ");
        fflush(stdout);
        scanf("%d", numNodos);
        printf("Ingrese el valor que enviará el nodo 0: ");
        fflush(stdout);
        scanf("%d", valorInicial);
    }
    MPI_Bcast(numNodos, 1, MPI_INT, RAIZ, MPI_COMM_WORLD);
    MPI_Bcast(valorInicial, 1, MPI_INT, RAIZ, MPI_COMM_WORLD);
}

// Función para encontrar los hijos izquierdo y derecho de un nodo dado
void encontrarHijos(int nodo, int numNodos, int* hijoIzquierdo, int* hijoDerecho) {
    *hijoIzquierdo = 2 * nodo + 1;
    *hijoDerecho = 2 * nodo + 2;
    if (*hijoIzquierdo >= numNodos) {
        *hijoIzquierdo = -1;
    }
    if (*hijoDerecho >= numNodos) {
        *hijoDerecho = -1;
    }
}

// Función para enviar mensajes a los hijos
void enviarMensajes(int nodo, int valor, int numNodos, int tamanio) {
    int hijoIzquierdo, hijoDerecho;
    encontrarHijos(nodo, numNodos, &hijoIzquierdo, &hijoDerecho);

    if (hijoIzquierdo != -1) {
        int rangoHijoIzquierdo = hijoIzquierdo % tamanio;
        MPI_Send(&valor, 1, MPI_INT, rangoHijoIzquierdo, 0, MPI_COMM_WORLD);
    }

    if (hijoDerecho != -1) {
        int rangoHijoDerecho = hijoDerecho % tamanio;
        MPI_Send(&valor, 1, MPI_INT, rangoHijoDerecho, 0, MPI_COMM_WORLD);
    }
}

// Función para recibir mensajes de los hijos y sumar los valores de los hijos
int recibirYSumar(int nodo, int valor, int numNodos, int tamanio) {
    int hijoIzquierdo, hijoDerecho;
    encontrarHijos(nodo, numNodos, &hijoIzquierdo, &hijoDerecho);

    int sumaIzquierda = 0, sumaDerecha = 0;

    // Recibir suma de hijos
    if (hijoIzquierdo != -1) {
        int rangoHijoIzquierdo = hijoIzquierdo % tamanio;
        MPI_Recv(&sumaIzquierda, 1, MPI_INT, rangoHijoIzquierdo, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (hijoDerecho != -1) {
        int rangoHijoDerecho = hijoDerecho % tamanio;
        MPI_Recv(&sumaDerecha, 1, MPI_INT, rangoHijoDerecho, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int sumaParcial = valor + sumaIzquierda + sumaDerecha;
    printf("Nodo %d (Proceso %d): suma parcial = %d\n", nodo, rango, sumaParcial);
    fflush(stdout);

    if (nodo != RAIZ) {
        int rangoPadre = (nodo - 1) / 2 % tamanio;
        MPI_Send(&sumaParcial, 1, MPI_INT, rangoPadre, 0, MPI_COMM_WORLD);
    }

    return sumaParcial;
}

int main(int argc, char** argv) {
    int rango, tamanio;
    int numNodos, valorInicial;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanio);

    double inicio = MPI_Wtime(); // Inicio del tiempo de ejecución

    // Inicializar el número de nodos y el valor inicial
    inicializarArbol(&numNodos, &valorInicial, rango);

    // Depuración: mostrar el número de nodos y el valor inicial
    if (rango == RAIZ) {
        printf("Número de nodos: %d, Valor inicial: %d\n", numNodos, valorInicial);
        fflush(stdout);
    }

    // Enviar mensajes a los hijos primero
    for (int nodo = rango; nodo < numNodos; nodo += tamanio) {
        enviarMensajes(nodo, valorInicial, numNodos, tamanio);
    }

    // Barrera para asegurarse de que todos los procesos hayan enviado los mensajes
    MPI_Barrier(MPI_COMM_WORLD);

    int sumaTotal = 0;
    // Recibir mensajes de los hijos y sumar
    for (int nodo = numNodos - 1; nodo >= rango; nodo -= tamanio) {
        sumaTotal += recibirYSumar(nodo, valorInicial, numNodos, tamanio);
    }

    // Barrera para esperar que todos los procesos terminen
    MPI_Barrier(MPI_COMM_WORLD);

    if (rango == RAIZ) {
        double fin = MPI_Wtime(); // Fin del tiempo de ejecución
        printf("Resultado final de la suma en el nodo raíz: %d\n", sumaTotal);
        printf("Tiempo total de ejecución: %f segundos\n", fin - inicio);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
