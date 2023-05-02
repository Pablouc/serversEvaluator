import pandas as pd
import matplotlib.pyplot as plt

# Leer el archivo CSV con los datos del servidor
with open('stadistics/datos.csv', 'r') as f:
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

plt.scatter(x, y1)
plt.xlabel('Cantidad de solicitudes')
plt.ylabel('Tiempo de ejecución (microsegundos)')
plt.title('Cantidad de solicitudes - Tiempo de ejecución')
plt.show()

# Graficar cantidad de solicitudes - consumo de memoria
plt.scatter(x, y2)
plt.xlabel('Cantidad de solicitudes')
plt.ylabel('Consumo de memoria (bytes)')
plt.title('Cantidad de solicitudes - Consumo de memoria')
plt.show()
