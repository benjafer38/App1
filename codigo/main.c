#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"
#include "metrics.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <ventas_csv> <metrica1> <metrica2> ...\n", argv[0]);
        return 1;
    }
    
    Order *orders = NULL;
    int total_orders = read_csv(argv[1], &orders);
    if (total_orders == -1) {
        printf("Error al leer el archivo CSV.\n");
        return 1;
    }
    
    for (int i = 2; i < argc; i++) {
        char *result = execute_metric(argv[i], total_orders, orders);
        if (result) {
            printf("%s\n", result);
            free(result);
        } else {
            printf("Métrica desconocida: %s\n", argv[i]);
        }
    }
    
    free(orders);
    return 0;
}