import pandas as pd
import matplotlib.pyplot as plt

# Leer datos del archivo CSV
df = pd.read_csv('Sequential/datos.csv')

# Crear figura y subplots
fig, axs = plt.subplots(2)

# Graficar cantidad de solicitudes vs tiempo de ejecución para cada servidor
axs[0].plot(df[df['Servidor'] == 1]['Cantidad de solicitudes'], df[df['Servidor'] == 1]['Tiempo de ejecución'], label='Servidor 1')
axs[0].plot(df[df['Servidor'] == 2]['Cantidad de solicitudes'], df[df['Servidor'] == 2]['Tiempo de ejecución'], label='Servidor 2')
axs[0].plot(df[df['Servidor'] == 3]['Cantidad de solicitudes'], df[df['Servidor'] == 3]['Tiempo de ejecución'], label='Servidor 3')
axs[0].plot(df[df['Servidor'] == 4]['Cantidad de solicitudes'], df[df['Servidor'] == 4]['Tiempo de ejecución'], label='Servidor 4')
axs[0].set_xlabel('Cantidad de solicitudes')
axs[0].set_ylabel('Tiempo de ejecución')
axs[0].legend()

# Graficar cantidad de solicitudes vs consumo de memoria para cada servidor
axs[1].plot(df[df['Servidor'] == 1]['Cantidad de solicitudes'], df[df['Servidor'] == 1]['Consumo de memoria'], label='Servidor 1')
axs[1].plot(df[df['Servidor'] == 2]['Cantidad de solicitudes'], df[df['Servidor'] == 2]['Consumo de memoria'], label='Servidor 2')
axs[1].plot(df[df['Servidor'] == 3]['Cantidad de solicitudes'], df[df['Servidor'] == 3]['Consumo de memoria'], label='Servidor 3')
axs[1].plot(df[df['Servidor'] == 4]['Cantidad de solicitudes'], df[df['Servidor'] == 4]['Consumo de memoria'], label='Servidor 4')
axs[1].set_xlabel('Cantidad de solicitudes')
axs[1].set_ylabel('Consumo de memoria')
axs[1].legend()

# Mostrar gráficas
plt.show()
