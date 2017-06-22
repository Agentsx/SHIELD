#ifndef __LOG_H__
#define __LOG_H__

#include "include/zlog.h"

typedef zlog_category_t log_category_t;
typedef log_category_t *(*get_cat_func)();

void log_set_callback(get_cat_func f);
int log_init(const char *fn);
log_category_t *log_get_category(const char *name);
void log_fini();

extern get_cat_func get_category;

#define log_debug(fmt, ...) do { \
	log_category_t *c = get_category(); \
	if (c == NULL) \
    	printf("FALTAL: [%s][%d] log get category error.", __FILE__, __LINE__); \
	zlog_debug(c, fmt, ##__VA_ARGS__); \
} while (0)

#define log_info(fmt, ...) do { \
	log_category_t *c = get_category(); \
	if (c == NULL) \
    	printf("FALTAL: [%s][%d] log get category error.", __FILE__, __LINE__); \
	zlog_info(c, fmt, ##__VA_ARGS__); \
} while (0)

#define log_error(fmt, ...) do { \
	log_category_t *c = get_category(); \
	if (c == NULL) \
    	printf("FALTAL: [%s][%d] log get category error.", __FILE__, __LINE__); \
	zlog_error(c, fmt, ##__VA_ARGS__); \
} while (0)

#define log_fatal(fmt, ...) do { \
	log_category_t *c = get_category(); \
	if (c == NULL) \
    	printf("FALTAL: [%s][%d] log get category error.", __FILE__, __LINE__); \
	zlog_fatal(c, fmt, ##__VA_ARGS__); \
} while (0)

#define log_warn(fmt, ...) do { \
	log_category_t *c = get_category(); \
	if (c == NULL) \
    	printf("FALTAL: [%s][%d] log get category error.", __FILE__, __LINE__); \
	zlog_warn(c, fmt, ##__VA_ARGS__); \
} while (0)

#endif
