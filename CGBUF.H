#ifndef CGBUF_H
#define CGBUF_H

typedef struct cgbuf {
	char *buf;
	size_t alloc;
	size_t pre_len;
	size_t post_len;
} CGBuf;

#define cgbuf_len(b)    (b->pre_len + b->post_len)

void cgbuf_init(CGBuf *cgbuf, const char *text, size_t len);
void cgbuf_fini(CGBuf *cgbuf);
void cgbuf_ensure(CGBuf *cgbuf, size_t extra);
void cgbuf_setcursor(CGBuf *cgbuf, size_t column);
void cgbuf_insertc(CGBuf *cgbuf, int c, size_t n);
void cgbuf_insert(CGBuf *cgbuf, const char *text, size_t len);
void cgbuf_appendc(CGBuf *cgbuf, int c, size_t n);
void cgbuf_append(CGBuf *cgbuf, const char *text, size_t len);
void cgbuf_prependc(CGBuf *cgbuf, int c, size_t n);
void cgbuf_prepend(CGBuf *cgbuf, const char *text, size_t len);
const char *cgbuf_string(CGBuf *cgbuf, char *dest, size_t len);

#endif