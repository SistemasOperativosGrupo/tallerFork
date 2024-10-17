# Proyecto de Procesos y Comunicación entre Procesos

## Descripción

Este programa en C demuestra el uso de procesos concurrentes mediante la llamada al sistema `fork()` y la comunicación entre ellos utilizando `pipe()`. El objetivo del programa es leer arreglos de enteros desde dos archivos, calcular la suma de cada arreglo de manera paralela, y luego sumar ambas sumas parciales, todo ello gestionado a través de procesos hijos y un proceso padre.

## Funcionalidades

- **Lectura de archivos**: Los archivos contienen arreglos de enteros separados por espacios.
- **Creación de procesos**: Se generan tres procesos hijos y un proceso padre:
  - **GranHijo**: Calcula la suma de los elementos del primer archivo.
  - **SegundoHijo**: Calcula la suma de los elementos del segundo archivo.
  - **PrimerHijo**: Suma las sumas parciales de ambos archivos.
  - **Padre**: Coordina y recibe los resultados, imprimiendo la suma total.
- **Comunicación entre procesos**: Se utiliza `pipe()` para enviar los resultados entre los procesos hijos y el proceso padre.
- **Gestión de memoria dinámica**: Los arreglos se almacenan dinámicamente usando `malloc()` y se liberan con `free()` al final del programa.

## Compilación

Para compilar el programa, asegúrate de tener un compilador C instalado y ejecuta el siguiente comando:

```bash
gcc taller_procesos.c -o taller_procesos
