#ifndef STATISTICS_H
#define STATISTICS_H

struct sequentialServerData {
  int cant_solicitudes;
  int tiempo_ejecucion_total;
  int consumo_memoria_total;
};

long int getCurrentRSS(void);

void writeCSV(struct sequentialServerData server);

#endif
