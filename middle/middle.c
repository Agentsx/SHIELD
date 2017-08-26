#include "middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "utils/log.h"
#include "utils/utils.h"
#include "utils/md5.h"
#include <string.h>

map_t *istype_map;
map_t *type_map;

int se;

static int __resolve_msg(void *in, const char *msg, template_t *temp)
{
	char tmp[129];

	int mp = 0;
	int sp = 0;
	int i;
	for (i = 0; temp[i].len_in_msg != -1; ++i) {
		memset(tmp, 0, sizeof(tmp));
		STRTRIMNCPY(tmp, msg + mp, temp[i].len_in_msg);
		switch (temp[i].type) {
		case 'L':
			*(long long *)((char *)in + sp) = atol(tmp);
			sp += sizeof(long long);
			log_notice("tmp [%s].", tmp);
			break;
		case 'S':
			strncpy((char *)in + sp, tmp, temp[i].len_in_struct);
			sp += temp[i].len_in_struct;
			log_notice("tmp [%s].", tmp);
			break;
		}
		mp += temp[i].len_in_msg;
	}
	return 0;
}

static int __get_type(const char *msg_type)
{
    int  *type = NULL;

    log_debug("msg_type[%s].", msg_type);
    if (map_get(istype_map, (void *)msg_type, (void **)&type))
        return -1;
    else {
        log_debug("get type from map[%d].", *type);
	    return *type;
    }
}

int __resolve_head(const char *msg, msg_head_t *h)
{
	return __resolve_msg(h, msg, head_template);
}


static void *__resolve_body(int type, const char *body, size_t *len)
{
    type_mapping_t *type_m = NULL;

    if (map_get(type_map, (void *)&type, (void **)&type_m)) {
		log_error("unkown message type [%d].", type);
		return NULL;
    }

	void *new = NULL;
	new = calloc(1, type_m->struct_len);
	*len = type_m->struct_len;
	__resolve_msg((char *)new + sizeof(msg_head_t), body, type_m->templ);

	return new;
}

int resolve_msg(shield_head_t *head)
{
	log_notice("middle resolve msg been called. cmd[%lld]", head->trade_type);

    if (head->trade_type > MAX_BIZ_CMD) { /* system msg */
		shield_head_t *new = calloc(1, sizeof(shield_head_t));
		memcpy(new, head, sizeof(shield_head_t));
	    MIDDLE_PUSH_IN(new);
        return 0;
    }

	char *msg = (char *)(head + 1);
	msg_head_t *h = calloc(1, sizeof(msg_head_t));
    int ret = __resolve_head(msg, h);
	if (ret) {
		log_error("resolve msg head [%s] error.", msg);
		goto ERROR;
	}

    char *sbody = msg + MSG_HEAD_LEN;

    if (se == SZSE) { // md5 check
        char imd5[SIGNATURE_LEN] = {0};
        md5_str(sbody, strlen(sbody), imd5, SIGNATURE_LEN);
        if (strncmp(imd5, h->signature_data, SIGNATURE_LEN)) {
            log_error("md5 check error[%s][%s], discard.", imd5, h->signature_data); 
            goto ERROR;
        }
        log_notice("md5 check ok");
    }

	int trade_type;
	if ((trade_type = __get_type(h->msg_type)) == -1) {
		log_error("message type error [%s].", h->msg_type);
		goto ERROR;
	}

	size_t len;
	void *b = __resolve_body(trade_type, sbody, &len);
	if (b == NULL) {
		log_error("resolve msg body [%s] error.", msg + MSG_HEAD_LEN);
		goto ERROR;
	}

	memcpy(b, h, sizeof(msg_head_t));

	shield_head_t *newh = calloc(1, sizeof(shield_head_t) + len);
	newh->fd = head->fd;
	newh->magic_num = head->magic_num;
	newh->len = len;
	newh->trade_type = trade_type;
	memcpy(newh + 1, b, len);

	free(h);
	free(b);

	MIDDLE_PUSH_IN(newh);
	log_notice("middle resolve msg ok.");
    return 0;

ERROR:
    free(h);
	return -1;
}

static int __package_msg(void *h, char *msg, template_t *temp)
{
	char tmp[129];

	int mp = 0;
	int sp = 0;
	int i;
	char *p;
	size_t len;

	for (i = 0; temp[i].len_in_msg != -1; ++i) {
		memset(tmp, 0, sizeof(tmp));
		switch (temp[i].type) {
		case 'L':
			snprintf(tmp, sizeof(tmp), "%lld", *(long long *)((char *)h + sp));
			memcpy(msg + mp, tmp, strlen(tmp));
			sp += sizeof(long long);
			break;
		case 'S':
			p = (char *)h + sp;
			len = strlen(p) > temp[i].len_in_msg ? temp[i].len_in_msg : strlen(p);
			log_debug("===%s===", p);
			memcpy(msg + mp, p, len);
			sp += temp[i].len_in_struct;
			break;
		}
		mp += temp[i].len_in_msg;
	}
	return 0;
}

char *__package_head(msg_head_t *head)
{
	char *h = calloc(1, MSG_HEAD_LEN + 1);
	
	memset(h, 0x20, MSG_HEAD_LEN);
	__package_msg(head, h, head_template);

	return h;
}

char *__package_body(long long type, msg_head_t *h, size_t *len)
{

    type_mapping_t *m = NULL;
    if (map_get(type_map, (void *)&type, (void **)&m)) {
		log_error("package msg error trade type [%lld].", type);
        return NULL;
    }

	char *msg = NULL;
	msg = calloc(1, m->body_len + 1);
	*len = m->body_len + 1;
	memset(msg, 0x20, m->body_len);
	__package_msg(h + 1, msg, m->templ);

	return msg;
}

int package_msg(shield_head_t *head)
{
	log_notice("middle package msg been called. cmd[%lld]", head->trade_type);

    if (head->trade_type > MAX_BIZ_CMD) {  // system msg
		shield_head_t *new = calloc(1, sizeof(shield_head_t));  /* avoid double free */
		memcpy(new, head, sizeof(shield_head_t));
	    MIDDLE_PUSH_OUT(new);
        return 0;
    }

	size_t len = 0;
    msg_head_t *mh = (msg_head_t *)(head + 1);
	void *b = __package_body(head->trade_type, mh, &len);
	if (b == NULL) {
		log_error("package msg body error.");
		return -1;
	}

    if (se == SZSE) {// md5 check
        md5_str((const char *)b, len - 1, mh->signature_data, SIGNATURE_LEN);
    }

	char *h = __package_head((msg_head_t *)(head + 1));


	shield_head_t *out = calloc(1, sizeof(shield_head_t) + MSG_HEAD_LEN + len + 1);
	out->magic_num = head->magic_num;
	out->fd = head->fd;
	out->trade_type = head->trade_type;
	out->len = MSG_HEAD_LEN + len;
	memcpy(out + 1, h, MSG_HEAD_LEN);
	memcpy((char *)(out + 1) + MSG_HEAD_LEN, b, len);

	free(h);
	free(b);

	log_notice("middle package msg[%s] ok.", (char *)(out + 1));
	log_notice("out[%p].", out);

	MIDDLE_PUSH_OUT(out);

	return 0;
}

static int __type_map_init()
{
    istype_map = map_init(STR, INT);
    type_map = map_init(INT, POINTER);

    int i;
    for (i = 0; tm[i].itype != -1; ++i) {
        log_debug("stype[%s], itype[%d]", tm[i].stype, tm[i].itype);
        if (map_put(istype_map, (void *)tm[i].stype, (void *)&tm[i].itype)) {
            log_error("put istype_map err, k[%s] v[%d].", tm[i].stype, tm[i].itype);
            goto ERROR;
        }
        type_mapping_t *m = calloc(1, sizeof(type_mapping_t));
        memcpy(m, &tm[i], sizeof(type_mapping_t));
        if (map_put(type_map, (void *)&tm[i].itype, m)) {
            log_error("put type_map err, k[%s].", tm[i].stype);
            goto ERROR;
        }
    }

    /* debug */
    void **keys = NULL;
    size_t size = 0;
    keys = map_keys(type_map, &size);
    if (keys == NULL)
        log_error("keys null");

    type_mapping_t *h;
    for (i = 0; i < size; ++i) {
        if (map_get(type_map, keys[i], (void **)&h)) {
            log_debug("get val error of key[%d].", *(int *)keys[i]);
            goto ERROR;
        } else {
            log_debug("%s %d %d %d", h->stype, h->itype, h->struct_len, h->body_len); 
        }
    }
	map_destroy_keys(keys);

    return 0;

ERROR:
    map_destroy(istype_map);
    map_destroy(type_map);
    return -1;
}

int middle_init(map_t *cfg)
{
    log_notice("------middle init begin------");
    if (__type_map_init()) {
        log_error("type map init error."); 
        return -1;
    }

    char *tmp = NULL;
    if (map_get(cfg, "se", (void **)&tmp)) {
        log_error("get se from cfg error."); 
        return -1;
    }

    se = atoi(tmp);

    return 0;
}
