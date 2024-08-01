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

// Función para realizar el recorrido del árbol y suma parcial
int recorrerArbol(int nodo, int rango, int valor, int numNodos, int tamanio) {
    int hijoIzquierdo, hijoDerecho;
    encontrarHijos(nodo, numNodos, &hijoIzquierdo, &hijoDerecho);

    int sumaIzquierda = 0, sumaDerecha = 0;

    if (hijoIzquierdo != -1) {
        int rangoHijoIzquierdo = hijoIzquierdo % tamanio;
        printf("Nodo %d enviando a hijo izquierdo %d (proceso %d)\n", nodo, hijoIzquierdo, rangoHijoIzquierdo);
        fflush(stdout);
        MPI_Send(&valor, 1, MPI_INT, rangoHijoIzquierdo, 0, MPI_COMM_WORLD);
        MPI_Recv(&sumaIzquierda, 1, MPI_INT, rangoHijoIzquierdo, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Nodo %d recibió de hijo izquierdo %d (proceso %d): %d\n", nodo, hijoIzquierdo, rangoHijoIzquierdo, sumaIzquierda);
        fflush(stdout);
    }

    if (hijoDerecho != -1) {
        int rangoHijoDerecho = hijoDerecho % tamanio;
        printf("Nodo %d enviando a hijo derecho %d (proceso %d)\n", nodo, hijoDerecho, rangoHijoDerecho);
        fflush(stdout);
        MPI_Send(&valor, 1, MPI_INT, rangoHijoDerecho, 0, MPI_COMM_WORLD);
        MPI_Recv(&sumaDerecha, 1, MPI_INT, rangoHijoDerecho, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Nodo %d recibió de hijo derecho %d (proceso %d): %d\n", nodo, hijoDerecho, rangoHijoDerecho, sumaDerecha);
        fflush(stdout);
    }

    int sumaParcial = valor + sumaIzquierda + sumaDerecha;
    printf("Nodo %d (Proceso %d): suma parcial = %d\n", nodo, rango, sumaParcial);
    fflush(stdout);

    if (nodo != RAIZ) {
        int rangoPadre = (nodo - 1) / 2 % tamanio;
        printf("Nodo %d enviando suma parcial %d a su padre (proceso %d)\n", nodo, sumaParcial, rangoPadre);
        fflush(stdout);
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

    int sumaTotal = 0;
    // Cada proceso maneja múltiples nodos si numNodos > tamanio
    for (int nodo = rango; nodo < numNodos; nodo += tamanio) {
        sumaTotal += recorrerArbol(nodo, rango, valorInicial, numNodos, tamanio);
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
