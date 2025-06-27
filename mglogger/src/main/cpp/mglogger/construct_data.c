

#include <string.h>
#include "construct_data.h"
#include "cJSON.h"
#include <stdlib.h>
#include "json_util.h"
#include "console_util.h"

static const char *log_key = "c";
static const char *flag_key = "f";
static const char *localtime_key = "l";
static const char *threadname_key = "n";
static const char *threadid_key = "i";
static const char *ismain_key = "m";

Construct_Data_cLogan *
construct_json_data_clogan(char *log, int flag, long long local_time, char *thread_name,
                           long long thread_id, int is_main) {
    Construct_Data_cLogan *construct_data = NULL;
    cJSON *root = NULL;
    Json_map_logan *map = NULL;
    root = cJSON_CreateObject();
    map = create_json_map_logan();
    if (NULL != root) {
        if (NULL != map) {
            add_item_string_clogan(map, log_key, log);
            add_item_number_clogan(map, flag_key, (double) flag);
            add_item_number_clogan(map, localtime_key, (double) local_time);
            add_item_string_clogan(map, threadname_key, thread_name);
            add_item_number_clogan(map, threadid_key, (double) thread_id);
            add_item_bool_clogan(map, ismain_key, is_main);
            inflate_json_by_map_clogan(root, map);
            char *back_data = cJSON_PrintUnformatted(root);
            construct_data = (Construct_Data_cLogan *) malloc(sizeof(Construct_Data_cLogan));

            if (NULL != construct_data) {
                memset(construct_data, 0, sizeof(Construct_Data_cLogan));
                size_t str_len = strlen(back_data);
                size_t length = str_len + 1;
                unsigned char *temp_data = (unsigned char *) malloc(length);
                if (NULL != temp_data) {
                    unsigned char *temp_point = temp_data;
                    memset(temp_point, 0, length);
                    memcpy(temp_point, back_data, str_len);
                    temp_point += str_len;
                    char return_data[] = {'\n'};
                    memcpy(temp_point, return_data, 1); //添加\n字符
                    construct_data->data = (char *) temp_data; //赋值
                    construct_data->data_len = (int) length;
                } else {
                    free(construct_data); //创建数据
                    construct_data = NULL;
                    printf_clogan(
                            "construct_json_data_clogan > malloc memory fail for temp_data\n");
                }
            }
            free(back_data);
        }
        cJSON_Delete(root);
    }
    if (NULL != map) {
        delete_json_map_clogan(map);
    }
    return construct_data;
}

void construct_data_delete_clogan(Construct_Data_cLogan *item) {
    if (NULL != item) {
        if (NULL != item->data) {
            free(item->data);
        }
        free(item);
    }
}
