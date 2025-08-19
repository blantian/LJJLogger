

#ifndef CLOGAN_JSON_UTIL_H
#define CLOGAN_JSON_UTIL_H

#include "cJSON.h"

#define CLOGAN_JSON_MAP_STRING 1
#define CLOGAN_JSON_MAP_NUMBER 2
#define CLOGAN_JSON_MAP_BOOL 3

typedef struct json_map {
    char *key;
    const char *valueStr;
    double valueNumber;
    int valueBool;
    int type;
    struct json_map *nextItem;
} Json_map_logan;

Json_map_logan *create_json_map_logan(void);

int is_empty_json_map_clogan(Json_map_logan *item);

void add_item_string_clogan(Json_map_logan *map, const char *key, const char *value);

void add_item_number_clogan(Json_map_logan *map, const char *key, double number);

void add_item_bool_clogan(Json_map_logan *map, const char *key, int boolValue);

void delete_json_map_clogan(Json_map_logan *item);

void inflate_json_by_map_clogan(cJSON *root, Json_map_logan *map);

#endif //CLOGAN_JSON_UTIL_H
