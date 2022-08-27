# Crear ejecutable para programa.c
# Compilador
CC = gcc
# Opciones de compilacion
CFLAGS = -Wall
# Nombre del ejecutable
TARGET = server

# Archivos fuente de las librerias
SRCS = consonantCounter.c

# Cabeceras
HEADERS = $(SRCS:.c=.h)

# Objetos
OBJS = $(SRCS:.c=.o)

# Construir el ejecutable
$(TARGET): counter counter2
		$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(OBJS)

# Dependencias
counter: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRCS)

counter2: consonantCounter2.c
	$(CC) consonantCounter2.c -o counter.out

# Eliminar archivos generados
limpiar:
	rm -f $(TARGET) *.o