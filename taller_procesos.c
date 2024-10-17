/****************************************************
Fecha: 10-octubre-2024
Tema: fork
descripción: Este programa en C realiza la lectura de dos archivos que contienen arreglos de enteros. Usa procesos creados con fork() para realizar cálculos concurrentes. Se divide en cuatro procesos:

GranHijo: Suma los elementos del primer archivo.
SegundoHijo: Suma los elementos del segundo archivo.
PrimerHijo: Suma los resultados de los dos archivos.
Padre: Recibe la suma total y la imprime.
La comunicación entre procesos se realiza mediante pipes, y cada proceso imprime su PID y el resultado de su cálculo.

*****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Función para leer un archivo y cargar un arreglo dinámico de enteros
int *leerArchivo(const char *nombreArchivo, int cantidad, int *sumaTotal) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    int *arreglo = (int *)malloc(cantidad * sizeof(int));
    if (arreglo == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < cantidad; i++) {
        if (fscanf(archivo, "%d", &arreglo[i]) != 1) {
            perror("Error al leer el archivo");
            exit(EXIT_FAILURE);
        }
        *sumaTotal += arreglo[i];  // Se va sumando el total
    }

    fclose(archivo);
    return arreglo;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s N1 archivo00 N2 archivo01\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int N1 = atoi(argv[1]);
    const char *archivo00 = argv[2];
    int N2 = atoi(argv[3]);
    const char *archivo01 = argv[4];

    int sumaA = 0, sumaB = 0, sumaTotal = 0;
    int pipeA[2], pipeB[2], pipeTotal[2];

    // Crear pipes
    if (pipe(pipeA) == -1 || pipe(pipeB) == -1 || pipe(pipeTotal) == -1) {
        perror("Error al crear pipes");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Error en fork");
        exit(EXIT_FAILURE);
    }

    // Primer hijo
    if (pid1 == 0) {
        pid_t pid2 = fork();
        if (pid2 == -1) {
            perror("Error en fork");
            exit(EXIT_FAILURE);
        }

        // Segundo hijo
        if (pid2 == 0) {
            pid_t pid3 = fork();
            if (pid3 == -1) {
                perror("Error en fork");
                exit(EXIT_FAILURE);
            }

            // Grand hijo (sumaA del archivo00)
            if (pid3 == 0) {
                close(pipeA[0]); // Cerrar lectura del pipe
                int *arregloA = leerArchivo(archivo00, N1, &sumaA);
                write(pipeA[1], &sumaA, sizeof(sumaA));
                free(arregloA);
                close(pipeA[1]); // Cerrar escritura del pipe
                printf("GranHijo: [%d] Sum_File1 = %d\n", getpid(), sumaA);
                exit(EXIT_SUCCESS);
            } 
            // Segundo hijo (sumaB del archivo01)
            else if (pid3 > 0) {
                wait(NULL); // Espera al grand hijo
                close(pipeB[0]); // Cerrar lectura del pipe
                int *arregloB = leerArchivo(archivo01, N2, &sumaB);
                write(pipeB[1], &sumaB, sizeof(sumaB));
                free(arregloB);
                close(pipeB[1]); // Cerrar escritura del pipe
                printf("SegundoHijo: [%d] Sum_File2 = %d\n", getpid(), sumaB);
                exit(EXIT_SUCCESS);
            }
        } 
        // Primer hijo (suma total de ambos arreglos)
        else if (pid2 > 0) {
            wait(NULL); // Esperar al segundo hijo
            close(pipeA[1]);
            close(pipeB[1]);
            close(pipeTotal[0]);

            read(pipeA[0], &sumaA, sizeof(sumaA));
            read(pipeB[0], &sumaB, sizeof(sumaB));
            sumaTotal = sumaA + sumaB;
            write(pipeTotal[1], &sumaTotal, sizeof(sumaTotal));

            close(pipeA[0]);
            close(pipeB[0]);
            close(pipeTotal[1]);
            printf("PrimerHijo: [%d] Suma total = %d\n", getpid(), sumaTotal);
            exit(EXIT_SUCCESS);
        }
    } 
    // Padre
    else if (pid1 > 0) {
        wait(NULL); // Espera al primer hijo
        close(pipeTotal[1]); // Cerrar escritura del pipe
        read(pipeTotal[0], &sumaTotal, sizeof(sumaTotal));
        printf("Padre: [%d] Suma total = %d\n", getpid(), sumaTotal);
        close(pipeTotal[0]); // Cerrar lectura del pipe
    }

    return 0;
}
