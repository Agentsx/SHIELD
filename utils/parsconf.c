#include "parsconf.h"
#include "include/parson.h"

int parse_config(const char *fn, map_t *map)
{
    JSON_Value *val;
    val = json_parse_file_with_comments(fn);
    if (val == NULL) {
        printf("ERROR: [%s][%d] parse file error.\n", __FL__);
        return -1;
    }

    if (json_value_get_type(val) != JSONObject) {
        printf("ERROR: [%s][%d] Json format error.\n", __FL__);
        return -1;
    }

    JSON_Object *obj = json_value_get_object(val);
    size_t count = json_object_get_count(obj);
    int i;
    for (i = 0; i < count; ++i) {
        const char *name = json_object_get_name(obj, i);
        JSON_Value *jv = json_object_get_value(obj, name);
        JSON_Value_Type t = json_value_get_type(jv);
        if (t != JSONString) {
            printf("ERROR: [%s][%d] parse file error.\n", __FL__);
            return -1;
        }
        const char *s = json_value_get_string(jv);
		map_put(map, (void *)name, (void *)s);
    }

	json_value_free(val);

    return 0;  
}
