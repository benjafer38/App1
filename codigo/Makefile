# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = main.o csv_reader.o metrics.o
TARGET = app1

# Regla para compilar el ejecutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regla genérica para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -f $(OBJ) $(TARGET)
