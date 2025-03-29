#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// pms,pls
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



// dms, dls, dmsp, dlsp: Métricas para analizar ventas por fecha
// Estructura auxiliar para contar ventas por fecha
typedef struct {
    char order_date[MAX_STRING];  // Fecha de la venta
    double total_sales;           // Total de dinero recaudado por las ventas en esa fecha
    int total_pizzas;             // Cantidad total de pizzas vendidas en esa fecha
} DateSales;

// Función auxiliar para buscar una fecha en el array de ventas
// Recibe un array de ventas, el tamaño del array y la fecha a buscar
int find_date(DateSales *sales, int size, char *date) {
    // Recorre las ventas registradas para encontrar la fecha
    for (int i = 0; i < size; i++) {
        // Si la fecha ya existe, retorna el índice donde se encuentra
        if (strcmp(sales[i].order_date, date) == 0) {
            return i;
        }
    }
    return -1;  // Si no se encuentra la fecha, retorna -1
}

// Función para calcular las métricas dms, dls, dmsp y dlsp
void calculate_date_metrics(int order_count, Order *orders, char **dms, char **dls, char **dmsp, char **dlsp) {
    // Verificar si no hay órdenes o si las órdenes son NULL
    if (order_count <= 0 || orders == NULL) {
        // Asignar valores de error si no hay ventas
        *dms = strdup("No hay ventas registradas");
        *dls = strdup("No hay ventas registradas");
        *dmsp = strdup("No hay ventas registradas");
        *dlsp = strdup("No hay ventas registradas");
        return;
    }

    // Asignar memoria para almacenar las ventas por fecha
    DateSales *sales = calloc(order_count, sizeof(DateSales));
    if (!sales) {
        // Si no se puede asignar memoria, mostrar error
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        exit(EXIT_FAILURE);
    }

    int sales_count = 0;  // Contador para las ventas registradas

    // Contar las ventas totales y la cantidad de pizzas por fecha
    for (int i = 0; i < order_count; i++) {
        if (strlen(orders[i].order_date) == 0) {
            continue;  // Si la orden no tiene fecha, ignorarla
        }

        // Buscar si la fecha ya existe en el array de ventas
        int index = find_date(sales, sales_count, orders[i].order_date);
        if (index == -1) {  // Nueva fecha, se agrega al arreglo
            strcpy(sales[sales_count].order_date, orders[i].order_date);
            sales[sales_count].total_sales = orders[i].total_price;  // Acumular el dinero de la venta
            sales[sales_count].total_pizzas = orders[i].quantity;    // Acumular la cantidad de pizzas vendidas
            sales_count++;  // Incrementar el contador de fechas
        } else {  // Fecha existente, se suman las ventas y las pizzas
            sales[index].total_sales += orders[i].total_price;  // Sumar el total de dinero
            sales[index].total_pizzas += orders[i].quantity;    // Sumar las pizzas
        }
    }

    // Si no hay ventas válidas después de procesar todas las órdenes
    if (sales_count == 0) {
        // Asignar valores de error a todas las métricas
        *dms = strdup("No hay ventas registradas");
        *dls = strdup("No hay ventas registradas");
        *dmsp = strdup("No hay ventas registradas");
        *dlsp = strdup("No hay ventas registradas");
        free(sales);  // Liberar la memoria asignada
        return;
    }

    // Inicializar los índices para las métricas de ventas más y menos
    int max_sales_index = 0, min_sales_index = 0;
    int max_pizzas_index = 0, min_pizzas_index = 0;

    // Recorrer todas las fechas para encontrar las que tienen más y menos ventas
    for (int i = 1; i < sales_count; i++) {
        // Comparar las ventas totales para encontrar el máximo y mínimo
        if (sales[i].total_sales > sales[max_sales_index].total_sales) {
            max_sales_index = i;  // Actualizar el índice de la fecha con más ventas
        }
        if (sales[i].total_sales < sales[min_sales_index].total_sales) {
            min_sales_index = i;  // Actualizar el índice de la fecha con menos ventas
        }

        // Comparar la cantidad de pizzas vendidas para encontrar el máximo y mínimo
        if (sales[i].total_pizzas > sales[max_pizzas_index].total_pizzas) {
            max_pizzas_index = i;  // Actualizar el índice de la fecha con más pizzas
        }
        if (sales[i].total_pizzas < sales[min_pizzas_index].total_pizzas) {
            min_pizzas_index = i;  // Actualizar el índice de la fecha con menos pizzas
        }
    }

    // Asignar resultados para cada métrica
    *dms = malloc(MAX_STRING * sizeof(char));  // Reservar memoria para el resultado de la métrica dms
    snprintf(*dms, MAX_STRING, "Fecha con más ventas (en dinero): %s, Total: %.2f",
         sales[max_sales_index].order_date, sales[max_sales_index].total_sales);

    *dls = malloc(MAX_STRING * sizeof(char));  // Reservar memoria para el resultado de la métrica dls
    snprintf(*dls, MAX_STRING, "Fecha con menos ventas (en dinero): %s, Total: %.2f",
         sales[min_sales_index].order_date, sales[min_sales_index].total_sales);

    *dmsp = malloc(MAX_STRING * sizeof(char));  // Reservar memoria para el resultado de la métrica dmsp
    snprintf(*dmsp, MAX_STRING, "Fecha con más ventas (en cantidad de pizzas): %s, Total: %d",
         sales[max_pizzas_index].order_date, sales[max_pizzas_index].total_pizzas);

    *dlsp = malloc(MAX_STRING * sizeof(char));  // Reservar memoria para el resultado de la métrica dlsp
    snprintf(*dlsp, MAX_STRING, "Fecha con menos ventas (en cantidad de pizzas): %s, Total: %d",
         sales[min_pizzas_index].order_date, sales[min_pizzas_index].total_pizzas);

    free(sales);  // Liberar la memoria asignada para las ventas por fecha
}



// Nueva métrica: Promedio de pizzas por orden (apo) y Promedio de pizzas por día (apd)
void calculate_promedio(int order_count, Order *orders, double *apo, double *apd) {
    // Verificamos si el número de órdenes es válido o si las órdenes son NULL
    if (order_count <= 0 || orders == NULL) {
        *apo = -1;  // Si hay un error, asignamos -1 a apo
        *apd = -1;  // Si hay un error, asignamos -1 a apd
        return;     // Salimos de la función
    }

    // Calcular el promedio de pizzas por orden (apo)
    int total_pizzas = 0;  // Variable para acumular el total de pizzas

    // Recorremos todas las órdenes
    for (int i = 0; i < order_count; i++) {
        total_pizzas += orders[i].quantity;  // Sumamos la cantidad de pizzas de cada orden
    }
    
    *apo = (double) total_pizzas / order_count;  // Calculamos el promedio de pizzas por orden

    // Calcular el promedio de pizzas por día (apd)
    // Suponiendo que cada orden tiene una fecha asociada

    // Creamos un arreglo para contar las ventas por fecha
    DateSales *sales = calloc(order_count, sizeof(DateSales));  // Reservamos memoria para el arreglo de ventas
    int sales_count = 0;  // Variable para contar el número de días únicos

    // Recorremos todas las órdenes para contar las ventas por fecha
    for (int i = 0; i < order_count; i++) {
        // Si la fecha de la orden está vacía, la ignoramos
        if (strlen(orders[i].order_date) == 0) continue;

        // Buscamos si la fecha ya está registrada en el arreglo sales
        int index = find_date(sales, sales_count, orders[i].order_date);
        
        // Si la fecha no está registrada, la agregamos al arreglo sales
        if (index == -1) {
            strcpy(sales[sales_count].order_date, orders[i].order_date);  // Guardamos la fecha
            sales[sales_count].total_pizzas = orders[i].quantity;  // Guardamos la cantidad de pizzas para esa fecha
            sales_count++;  // Aumentamos el contador de fechas
        } else {
            // Si la fecha ya está registrada, acumulamos las pizzas vendidas en esa fecha
            sales[index].total_pizzas += orders[i].quantity;
        }
    }

    // Ahora calculamos el total de días y el total de pizzas vendidas en esos días
    int total_days = sales_count;  // El número total de días es igual a las fechas únicas encontradas
    int total_pizzas_day = 0;  // Variable para acumular el total de pizzas por día

    // Sumamos las pizzas vendidas en cada día
    for (int i = 0; i < sales_count; i++) {
        total_pizzas_day += sales[i].total_pizzas;  // Acumulamos el total de pizzas vendidas por día
    }

    // Calculamos el promedio de pizzas vendidas por día
    *apd = (double) total_pizzas_day / total_days;

    // Liberamos la memoria utilizada por el arreglo sales
    free(sales);
}



// Estructura para contar los ingredientes
typedef struct {
    char ingredient[MAX_STRING];
    int count;
} IngredientCount;

// Función para eliminar los espacios al principio y al final de una cadena
void trim_spaces(char *str) {
    int start = 0;
    int end = strlen(str) - 1;

    // Eliminar los espacios al principio
    while (str[start] == ' ' || str[start] == '\t') {
        start++;
    }

    // Eliminar los espacios al final
    while (end >= start && (str[end] == ' ' || str[end] == '\t')) {
        end--;
    }

    // Mover la cadena recortada al inicio
    for (int i = start; i <= end; i++) {
        str[i - start] = str[i];
    }
    str[end - start + 1] = '\0';  // Terminar la cadena
}

// Función para calcular el ingrediente más vendido
void calculate_ims(int order_count, Order *orders, char **ims_result) {
    IngredientCount ingredients[100];
    int ing_count = 0;
    
    // Recorrer todas las órdenes
    for (int i = 0; i < order_count; i++) {
        // Usar coma como delimitador para separar los ingredientes
        char *token = strtok(orders[i].pizza_ingredients, ",");
        while (token) {
            // Eliminar espacios extra al principio y al final de cada ingrediente
            trim_spaces(token);

            int found = 0;
            // Buscar si el ingrediente ya está en la lista
            for (int j = 0; j < ing_count; j++) {
                if (strcmp(ingredients[j].ingredient, token) == 0) {
                    ingredients[j].count += orders[i].quantity;
                    found = 1;
                    break;
                }
            }
            // Si el ingrediente no se encontró, agregarlo a la lista
            if (!found) {
                strcpy(ingredients[ing_count].ingredient, token);
                ingredients[ing_count].count = orders[i].quantity;
                ing_count++;
            }

            // Obtener el siguiente ingrediente
            token = strtok(NULL, ",");
        }
    }
    
    // Encontrar el ingrediente más vendido
    int max_count = 0;
    char most_sold[MAX_STRING];
    for (int i = 0; i < ing_count; i++) {
        if (ingredients[i].count > max_count) {
            max_count = ingredients[i].count;
            strcpy(most_sold, ingredients[i].ingredient);
        }
    }
    
    // Asignar el resultado al puntero de salida
    *ims_result = malloc(MAX_STRING);
    snprintf(*ims_result, MAX_STRING, "Ingrediente más vendido: %s", most_sold);
}

// Función para calcular cantidad de pizzas por categoría
void calculate_hp(int order_count, Order *orders, char **hp_result) {
    typedef struct {
        char category[MAX_STRING];
        int count;
    } CategoryCount;
    
    CategoryCount categories[20];
    int cat_count = 0;
    
    for (int i = 0; i < order_count; i++) {
        int found = 0;
        for (int j = 0; j < cat_count; j++) {
            if (strcmp(categories[j].category, orders[i].pizza_category) == 0) {
                categories[j].count += orders[i].quantity;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(categories[cat_count].category, orders[i].pizza_category);
            categories[cat_count].count = orders[i].quantity;
            cat_count++;
        }
    }
    
    *hp_result = malloc(MAX_STRING);
    strcpy(*hp_result, "Categorías de pizzas vendidas:\n");
    for (int i = 0; i < cat_count; i++) {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%s: %d\n", categories[i].category, categories[i].count);
        strcat(*hp_result, buffer);
    }
}


// Función para ejecutar métricas solicitadas.
char* execute_metric(const char *metric, int order_count, Order *orders) {
    static char *pms_result = NULL;
    static char *pls_result = NULL;
    static char *dms_result = NULL;
    static char *dls_result = NULL;
    static char *dmsp_result = NULL;
    static char *dlsp_result = NULL;
    static double apo_result = -1;
    static double apd_result = -1;
    static char *ims_result = NULL;
    static char *hp_result = NULL;


    // Solo calcular si aún no se ha hecho
    if (!pms_result || !pls_result) {
        calculate_pms_pls(order_count, orders, &pms_result, &pls_result);
    }
    if (!dms_result || !dls_result || !dmsp_result || !dlsp_result) {
        calculate_date_metrics(order_count, orders, &dms_result, &dls_result, &dmsp_result, &dlsp_result);
    }
    if (apo_result == -1 || apd_result == -1) {
        calculate_promedio(order_count, orders, &apo_result, &apd_result);
    }
    if (!ims_result) {
        calculate_ims(order_count, orders, &ims_result);
    }
    if (!hp_result) {
        calculate_hp(order_count, orders, &hp_result);
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
    } else if (strcmp(metric, "dms") == 0) {
        snprintf(result, MAX_STRING, "Fecha con más ventas: %s", dms_result);
    } else if (strcmp(metric, "dls") == 0) {
        snprintf(result, MAX_STRING, "%s", dls_result);
    } else if (strcmp(metric, "dmsp") == 0) {
        snprintf(result, MAX_STRING, "%s", dmsp_result);
    } else if (strcmp(metric, "dlsp") == 0) {
        snprintf(result, MAX_STRING, "%s", dlsp_result);
    } else if (strcmp(metric, "apo") == 0) {
        snprintf(result, MAX_STRING, "Promedio de pizzas por orden: %.2f", apo_result);
    } else if (strcmp(metric, "apd") == 0) {
        snprintf(result, MAX_STRING, "Promedio de pizzas por día: %.2f", apd_result);
    } else if (strcmp(metric, "ims") == 0) {
        snprintf(result, MAX_STRING, "%s", ims_result);
    } else if (strcmp(metric, "hp") == 0) {
        snprintf(result, MAX_STRING, "%s", hp_result);
    } else {
        snprintf(result, MAX_STRING, "Métrica no implementada: %s", metric);
    }
    return result;
}
