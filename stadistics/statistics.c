#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>

#include "statistics.h"

/* Declaración de la función getCurrentRSS() */
long int getCurrentRSS(void) {
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  return usage.ru_maxrss;
}

// Función para escribir los datos en un archivo CSV
void writeCSV(struct serverData server) {
  FILE *fp;
  // fp = fopen("datos.csv", "w");
  fp = fopen("../stadistics/datos.csv", "a");
  if (fp == NULL) {
    printf("Error al abrir el archivo datos.csv\n");
    return;
  }
  fprintf(fp, "Cantidad de solicitudes,Tiempo de ejecución,Tiempo promedio por solicitud,Consumo de memoria\n");
  fprintf(fp, "%d,%d,%d,%d\n", server.cant_solicitudes, server.tiempo_ejecucion_total, server.tiempo_ejecucion_total / server.cant_solicitudes, server.consumo_memoria_total);
  if (fclose(fp) != 0) {
    printf("Error al cerrar el archivo datos.csv\n");
  } else {
    printf("Archivo datos.csv escrito correctamente\n");
  }
}
