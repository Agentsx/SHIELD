#ifndef __MD5_H__
#define __MD5_H__

extern char *md5_str(const char *message, size_t len, char *dest, size_t maxlen);
extern int  md5(const char *message, size_t len, char *output, size_t maxlen);

#endif
