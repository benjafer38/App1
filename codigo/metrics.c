#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura auxiliar para contar las pizzas vendidas
typedef struct {
    char pizza_name[MAX_STRING];
    int quantity;
} PizzaCount;

// Función auxiliar para buscar una pizza en el array de conteo
int find_pizza(PizzaCount *pizzas, int size, char *name) {
    for (int i = 0; i < size; i++) {
        if (strcmp(pizzas[i].pizza_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Función para encontrar la pizza más y menos vendida
void calculate_pms_pls(int order_count, Order *orders, char **pms, char **pls) {
    if (order_count <= 0 || orders == NULL) {
        *pms = strdup("No hay pizzas registradas");
        *pls = strdup("No hay pizzas registradas");
        return;
    }

    PizzaCount *pizzas = calloc(order_count, sizeof(PizzaCount));
    if (!pizzas) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        exit(EXIT_FAILURE);
    }

    int pizza_count = 0;

    // Contar las ventas de cada tipo de pizza
    for (int i = 0; i < order_count; i++) {
        if (strlen(orders[i].pizza_name) == 0) {
            printf("Orden ignorada: Nombre vacío.\n");
            continue;  // Ignora pedidos con nombre vacío
        }

        int index = find_pizza(pizzas, pizza_count, orders[i].pizza_name);
        if (index == -1) { // Nueva pizza
            strcpy(pizzas[pizza_count].pizza_name, orders[i].pizza_name);
            pizzas[pizza_count].quantity = orders[i].quantity; // Registra aunque sea 0
            pizza_count++;
        } else { // Pizza existente, sumamos la cantidad
            pizzas[index].quantity += orders[i].quantity;
        }
    }

    // Si después del filtrado no hay pizzas válidas
    if (pizza_count == 0) {
        *pms = strdup("No hay pizzas registradas");
        *pls = strdup("No hay pizzas registradas");
        free(pizzas);
        return;
    }

    // Buscar la pizza más y menos vendida
    int max_index = 0, min_index = 0;
    for (int i = 1; i < pizza_count; i++) {
        if (pizzas[i].quantity > pizzas[max_index].quantity) {
            max_index = i;
        }
        if (pizzas[i].quantity < pizzas[min_index].quantity) {
            min_index = i;
        }
    }

    // Asignar resultados
    *pms = strdup(pizzas[max_index].pizza_name);
    *pls = strdup(pizzas[min_index].pizza_name);

    free(pizzas);
}

// Función para ejecutar métricas solicitadas
char* execute_metric(const char *metric, int order_count, Order *orders) {
    static char *pms_result = NULL;
    static char *pls_result = NULL;

    // Solo calcular si aún no se ha hecho
    if (!pms_result || !pls_result) {
        calculate_pms_pls(order_count, orders, &pms_result, &pls_result);
    }

    char *result = malloc(MAX_STRING * sizeof(char));
    if (!result) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(metric, "pms") == 0) {
        snprintf(result, MAX_STRING, "Pizza más vendida: %s", pms_result);
    } else if (strcmp(metric, "pls") == 0) {
        snprintf(result, MAX_STRING, "Pizza menos vendida: %s", pls_result);
    } else {
        snprintf(result, MAX_STRING, "Métrica no implementada: %s", metric);
    }

    return result;
}
