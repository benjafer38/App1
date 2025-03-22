#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"

#define MAX_LINE_LENGTH 1024

int read_csv(const char *filename, Order **orders) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    // Contar líneas (excluyendo la cabecera)
    while (fgets(line, sizeof(line), file)) count++;
    rewind(file);

    // Asignar memoria para las órdenes
    *orders = malloc(count * sizeof(Order));
    if (!*orders) {
        fclose(file);
        perror("Memory allocation failed");
        return -1;
    }

    // Leer cabecera y descartarla
    fgets(line, sizeof(line), file);
    printf("Cabecera: %s\n", line);

    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        printf("Leyendo línea %d: %s", index + 1, line);

        char *token, *rest = line;

        // Leer cada campo del CSV
        token = strtok_r(rest, ",", &rest); (*orders)[index].pizza_id = atof(token);
        token = strtok_r(NULL, ",", &rest); (*orders)[index].order_id = atof(token);
        token = strtok_r(NULL, ",", &rest); strcpy((*orders)[index].pizza_name_id, token);
        token = strtok_r(NULL, ",", &rest); (*orders)[index].quantity = atoi(token);
        token = strtok_r(NULL, ",", &rest); strcpy((*orders)[index].order_date, token);
        token = strtok_r(NULL, ",", &rest); strcpy((*orders)[index].order_time, token);
        token = strtok_r(NULL, ",", &rest); (*orders)[index].unit_price = atof(token);
        token = strtok_r(NULL, ",", &rest); (*orders)[index].total_price = atof(token);
        token = strtok_r(NULL, ",", &rest); strcpy((*orders)[index].pizza_size, token);
        token = strtok_r(NULL, ",", &rest); strcpy((*orders)[index].pizza_category, token);

        // 📌 **Manejo especial de los ingredientes encerrados en comillas**
        if (*rest == '"') {  
            rest++; // Saltar la primera comilla
            char *end = strchr(rest, '"');  // Buscar la última comilla
            if (end) {
                *end = '\0';  // Terminar la cadena en la última comilla
                strcpy((*orders)[index].pizza_ingredients, rest);
                rest = end + 2; // Mover el puntero después de la comilla y la coma
            }
        } else {
            token = strtok_r(NULL, ",", &rest);
            if (token) strcpy((*orders)[index].pizza_ingredients, token);
        }

        // Último campo: Nombre de la pizza
        token = strtok_r(NULL, "\n", &rest);
        if (token) strcpy((*orders)[index].pizza_name, token);

        printf("✔ Línea %d leída correctamente: %s\n", index + 1, (*orders)[index].pizza_name);
        index++;
    }

    fclose(file);
    return count;
}
