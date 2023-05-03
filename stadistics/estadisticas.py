# import pandas as pd
# import matplotlib.pyplot as plt

# def graficar(name):
#     # Leer el archivo CSV con los datos del servidor
#     with open(name, 'r') as f:
#         rows = f.readlines()
#     data = []
#     for row in rows:
#         values = row.split(',')
#         if values[0].isdigit():
#             data.append((int(values[0]), int(values[1]), int(values[3])))

#     data = sorted(data, key=lambda x: x[0])

#     # Graficar cantidad de solicitudes - tiempo de ejecución
#     x = [d[0] for d in data]
#     y1 = [d[1] for d in data]
#     y2 = [d[2] for d in data]

#     plt.scatter(x, y1)
#     plt.xlabel('Cantidad de solicitudes')
#     plt.ylabel('Tiempo de ejecución (microsegundos)')
#     plt.title('Cantidad de solicitudes - Tiempo de ejecución')
#     plt.show()

#     # Graficar cantidad de solicitudes - consumo de memoria
#     plt.scatter(x, y2)
#     plt.xlabel('Cantidad de solicitudes')
#     plt.ylabel('Consumo de memoria (bytes)')
#     plt.title('Cantidad de solicitudes - Consumo de memoria')
#     plt.show()


# graficar('stadistics/secuencial.csv')
# graficar('stadistics/hilos.csv')
# graficar('stadistics/heavy_process.csv')


import pandas as pd
import matplotlib.pyplot as plt

def graficar(name, ax, tipo):
    # Leer el archivo CSV con los datos del servidor
    with open(name, 'r') as f:
        rows = f.readlines()
    data = []
    for row in rows:
        values = row.split(',')
        if values[0].isdigit():
            data.append((int(values[0]), int(values[1]), int(values[3])))

    data = sorted(data, key=lambda x: x[0])

    # Graficar cantidad de solicitudes - tiempo de ejecución
    x = [d[0] for d in data]
    y1 = [d[1] for d in data]
    y2 = [d[2] for d in data]

    ax[0].scatter(x, y1)
    ax[0].set_xlabel('Cantidad de solicitudes')
    ax[0].set_ylabel('Tiempo de ejecución (microsegundos)')
    ax[0].set_title('Cantidad de solicitudes - Tiempo de ejecución - ' + tipo)
    # ax[0].set_ylim([0, 0.000000002])
    ax[0].set_ylim([0, None])

    # Graficar cantidad de solicitudes - consumo de memoria
    ax[1].scatter(x, y2)
    ax[1].set_xlabel('Cantidad de solicitudes')
    ax[1].set_ylabel('Consumo de memoria (bytes)')
    ax[1].set_title('Cantidad de solicitudes - Consumo de memoria - ' + tipo)
    # ax[1].set_ylim([0, ])


fig, axs = plt.subplots(3, 2, figsize=(10, 15))
graficar('stadistics/secuencial.csv', axs[0], 'Sequential')
graficar('stadistics/hilos.csv', axs[1], 'Threads')
graficar('stadistics/heavy_process.csv', axs[2], 'Heavy Process')
plt.tight_layout()
plt.show()
