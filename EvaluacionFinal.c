#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// Definición del nodo del árbol binario
typedef struct NodoArbol {
    int valor;
    struct NodoArbol *izquierdo;
    struct NodoArbol *derecho;
} NodoArbol;

// Función para crear un nuevo nodo del árbol
NodoArbol* crear_nodo(int valor) {
    NodoArbol *nodo = (NodoArbol*) malloc(sizeof(NodoArbol));
    nodo->valor = valor;
    nodo->izquierdo = NULL;
    nodo->derecho = NULL;
    return nodo;
}

// Función para insertar nodos en el árbol binario
NodoArbol* insertar(NodoArbol *raiz, int valor) {
    if (raiz == NULL) {
        return crear_nodo(valor);
    }
    if (valor < raiz->valor) {
        raiz->izquierdo = insertar(raiz->izquierdo, valor);
    } else {
        raiz->derecho = insertar(raiz->derecho, valor);
    }
    return raiz;
}

// Función para crear un árbol binario con 'n' nodos
NodoArbol* crear_arbol(int n) {
    NodoArbol *raiz = NULL;
    for (int i = 0; i < n; i++) {
        raiz = insertar(raiz, rand() % 100); // Insertar un valor aleatorio entre 0 y 99
    }
    return raiz;
}

// Función para calcular la suma de los valores de los nodos del árbol
int sumar_arbol(NodoArbol *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    int suma_izquierda = sumar_arbol(raiz->izquierdo);
    int suma_derecha = sumar_arbol(raiz->derecho);
    return raiz->valor + suma_izquierda + suma_derecha;
}

// Función para liberar la memoria del árbol binario
void liberar_arbol(NodoArbol *raiz) {
    if (raiz != NULL) {
        liberar_arbol(raiz->izquierdo);
        liberar_arbol(raiz->derecho);
        free(raiz);
    }
}

// Función para asignar tareas a los nodos de MPI
void asignar_tareas(NodoArbol *raiz, int rango, int tamaño) {
    if (raiz == NULL) {
        return;
    }
    if (2 * rango + 1 < tamaño) {
        MPI_Send(&raiz->valor, 1, MPI_INT, 2 * rango + 1, 0, MPI_COMM_WORLD);
        asignar_tareas(raiz->izquierdo, 2 * rango + 1, tamaño);
    }
    if (2 * rango + 2 < tamaño) {
        MPI_Send(&raiz->valor, 1, MPI_INT, 2 * rango + 2, 0, MPI_COMM_WORLD);
        asignar_tareas(raiz->derecho, 2 * rango + 2, tamaño);
    }
}

// Función para recibir tareas en los nodos de MPI
int recibir_tareas(int rango) {
    int valor;
    MPI_Recv(&valor, 1, MPI_INT, (rango - 1) / 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return valor;
}

int main(int argc, char **argv) {
    int num_nodos, valor_a_sumar;
    int rango, tamaño;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamaño);

    // Proceso 0 lee el número de nodos y el valor a sumar desde la consola
    if (rango == 0) {
        printf("Ingrese el número de nodos del árbol: ");
        scanf("%d", &num_nodos);
        printf("Ingrese el número a sumar: ");
        scanf("%d", &valor_a_sumar);
    }

    // Distribuir el número de nodos y el valor a sumar a todos los procesos
    MPI_Bcast(&num_nodos, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&valor_a_sumar, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Crear el árbol en el proceso 0
    NodoArbol *raiz = NULL;
    if (rango == 0) {
        raiz = crear_arbol(num_nodos);
    }

    // Iniciar la medición del tiempo
    double tiempo_inicio = MPI_Wtime();

    // Enviar el valor a sumar desde el nodo raíz a los hijos
    if (rango == 0) {
        asignar_tareas(raiz, rango, tamaño);
    } else {
        valor_a_sumar = recibir_tareas(rango);
    }

    // Calcular la suma de los valores de los nodos del árbol en cada proceso
    int suma_local = sumar_arbol(raiz) + valor_a_sumar;
    int suma_total = 0;

    // Enviar la suma local al nodo padre y recibir las sumas parciales
    if (rango != 0) {
        MPI_Send(&suma_local, 1, MPI_INT, (rango - 1) / 2, 0, MPI_COMM_WORLD);
    } else {
        for (int i = 1; i < tamaño; i++) {
            int suma_parcial;
            MPI_Recv(&suma_parcial, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            suma_local += suma_parcial;
            printf("Proceso %d, suma parcial: %d\n", i, suma_parcial);
        }
        suma_total = suma_local;
    }

    // Detener la medición del tiempo
    double tiempo_fin = MPI_Wtime();

    // Proceso 0 imprime el resultado y el tiempo de ejecución
    if (rango == 0) {
        printf("La suma total de los nodos del árbol es: %d\n", suma_total);
        printf("Tiempo de ejecución: %f segundos\n", tiempo_fin - tiempo_inicio);
    }

    // Mostrar sumas parciales en cada proceso
    if (rango != 0) {
        printf("Proceso %d, suma parcial: %d\n", rango, suma_local);
    }

    // Liberar memoria
    if (raiz != NULL) {
        liberar_arbol(raiz);
    }

    MPI_Finalize();
    return 0;
}
