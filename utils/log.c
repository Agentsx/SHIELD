#include "log.h"

get_cat_func get_category = NULL;

log_category_t *log_get_category(const char *name)
{
    return zlog_get_category(name);
}

void log_set_callback(get_cat_func f)
{
    get_category = f;
}

int log_init(const char *fn)
{
	return zlog_init(fn);
}

/*
void log_debug(log_category_t *c, const char *msg)
{
    zlog_info(c, msg);
}

static void __log_info(log_category_t *c, const char *info)
{
    zlog_info(c, info);
}

void log_info(const char *info)
{
    log_category_t *c = get_category();
    if (c == NULL)
        printf("FALTAL: [%s][%d] log get category error.", __FILE__, __LINE__); 

    __log_info(c, info);
}

static void __log_error(log_category_t *c, const char *error)
{
    zlog_error(c, error);
}

void log_error(const char *error)
{
    log_category_t *c = get_category();
    if (c == NULL)
        printf("FALTAL: [%s][%d] log get category error.", __FILE__, __LINE__); 

    __log_error(c, error);
}
*/

void log_fini()
{
    zlog_fini();
}
