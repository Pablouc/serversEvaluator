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
  char filename[50];

  if (strcmp(server.tipo_servidor, "secuencial") == 0) {
    sprintf(filename, "../stadistics/secuencial.csv");
  } else if (strcmp(server.tipo_servidor, "hilos") == 0) {
    sprintf(filename, "../stadistics/hilos.csv");
  } else if (strcmp(server.tipo_servidor, "heavy process") == 0) {
    sprintf(filename, "../stadistics/heavy_process.csv");
  } else if (strcmp(server.tipo_servidor, "pre-forked") == 0) {
    sprintf(filename, "../stadistics/pre_forked.csv");
  } else {
    printf("Tipo de servidor desconocido\n");
    return;
  }

  fp = fopen(filename, "a");
  if (fp == NULL) {
    printf("Error al abrir el archivo %s\n", filename);
    return;
  }

  fprintf(fp, "Cantidad de solicitudes,Tiempo de ejecución,Tiempo promedio por solicitud,Consumo de memoria\n");
  fprintf(fp, "%d,%d,%d,%d\n", server.cant_solicitudes, server.tiempo_ejecucion_total, server.tiempo_ejecucion_total / server.cant_solicitudes, server.consumo_memoria_total);
  if (fclose(fp) != 0) {
    printf("Error al cerrar el archivo %s\n", filename);
  } else {
    printf("Archivo %s escrito correctamente\n", filename);
  }
}