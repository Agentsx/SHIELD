#include "middle.h"
#include "include/trade_msg.h"
#include "include/trade_type.h"
#include "utils/log.h"
#include "utils/utils.h"
#include <string.h>

map_t *istype_map;
map_t *type_map;

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

    if (map_get(istype_map, (void *)msg_type, (void **)&type))
        return -1;
    else 
	    return *type;
}

static msg_head_t * __resolve_head(const char *msg)
{
	msg_head_t *h = calloc(1, sizeof(msg_head_t));

	__resolve_msg(h, msg, head_template);

	return h;
}


static void *__resolve_body(long long type, const char *body, size_t *len)
{;
    type_mapping_t *type_m = NULL;

    if (map_get(type_map, (void *)&type, (void **)&type_m)) {
		log_error("unkown message type [%lld].", type);
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
    if (head->trade_type > MAX_BIZ_CMD) { // system msg
	    MIDDLE_PUSH_IN(head);
        return 0;
    }

	char *msg = (char *)(head + 1);
	msg_head_t *h = __resolve_head(msg);
	if (h == NULL) {
		log_error("resolve msg head [%s] error.", msg);
		return -1;
	}

	long long trade_type;
	if ((trade_type = __get_type(h->msg_type)) == -1) {
		log_error("message type error [%s].", h->msg_type);
		free(h);	
		return -1;
	}

	size_t len;
	void *b = __resolve_body(trade_type, msg + MSG_HEAD_LEN, &len);
	if (b == NULL) {
		free(h);
		log_error("resolve msg body [%s] error.", msg + MSG_HEAD_LEN);
		return -1;
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
	*len = m->body_len;
	memset(msg, 0x20, m->body_len);
	__package_msg(h + 1, msg, m->templ);

	return msg;
}

int package_msg(shield_head_t *head)
{
	log_notice("middle package msg been called.");

    if (head->trade_type > MAX_BIZ_CMD) {  // system msg
	    MIDDLE_PUSH_OUT(head);
        return 0;
    }

	char *h = __package_head((msg_head_t *)(head + 1));

	size_t len = 0;
	void *b = __package_body(head->trade_type, (msg_head_t *)(head + 1), &len);
	if (b == NULL) {
		free(h);
		log_error("package msg body error.");
		return -1;
	}

	shield_head_t *out = calloc(1, sizeof(shield_head_t) + MSG_HEAD_LEN + len);
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

int middle_init()
{
    istype_map = map_init(STR, INT);
    type_map = map_init(STR, POINTER);

    int i;
    for (i = 0; tm[i].itype != -1; ++i) {
        if (map_put(istype_map, (void *)tm[i].stype, (void *)&tm[i].itype)) {
            log_error("put istype_map err, k[%s] v[%d].", tm[i].stype, tm[i].itype);
            return -1;
        }
        type_mapping_t *m = calloc(1, sizeof(type_mapping_t));
        memcpy(m, &tm[i], sizeof(type_mapping_t));
        if (map_put(type_map, tm[i].stype, m)) {
            log_error("put type_map err, k[%s].", tm[i].stype);
            return -1;
        }
    }

    return 0;
}
