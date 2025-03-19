#ifndef CSV_READER_H
#define CSV_READER_H

#define MAX_STRING 100

typedef struct {
    int pizza_id;
    int order_id;
    char pizza_name_id[MAX_STRING];
    int quantity;
    char order_date[MAX_STRING];
    char order_time[MAX_STRING];
    float unit_price;
    float total_price;
    char pizza_size[MAX_STRING];
    char pizza_category[MAX_STRING];
    char pizza_ingredients[MAX_STRING * 2];
    char pizza_name[MAX_STRING];
} Order;

int read_csv(const char *filename, Order **orders);

#endif // CSV_READER_H