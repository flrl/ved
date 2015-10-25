#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*** move to header ***/

typedef struct cgbuf {
	char *buf;
	char *gs;
	char *ge;
	size_t alloc;
	size_t len;
	size_t displen;
} CGBuf;

/*** end header ***/

#define CGBUF_GAP_INIT_ALLOC (8)
#define CGBUF_GAP_MARKER (-1)
#define CGBUF_BLANK (' ')

void cgbuf_init(CGBuf *cgbuf, const char *text, size_t len) {
	size_t alloc = CGBUF_GAP_INIT_ALLOC;
	size_t i;

	if (len == 0 && text != NULL) len = strlen(text);
	while (alloc <= len)
		alloc *= 2;

	cgbuf->gs = cgbuf->buf = malloc(alloc);
	cgbuf->alloc = alloc;
	memset(cgbuf->buf, CGBUF_GAP_MARKER, alloc);

	cgbuf->ge = &cgbuf->buf[alloc - len];
	memcpy(cgbuf->ge, text, len);

	cgbuf->len = len;
	cgbuf->displen = 0;
	for (i = 0; i < len; i++)
		cgbuf->displen += (2 - isprint(cgbuf->ge[i]));
}

void cgbuf_ensure(CGBuf *cgbuf, size_t extra) {
	size_t alloc, pre_len, post_len;
	char *buf, *gs, *ge;

	if (cgbuf->alloc > cgbuf->len + extra)
		return;

	alloc = cgbuf->alloc;
	while (alloc < cgbuf->len + extra)
		alloc *= 2;

	buf = malloc(alloc);

	pre_len = cgbuf->gs - cgbuf->buf;
	memcpy(buf, cgbuf->buf, pre_len);
	gs = &buf[pre_len];

	post_len = cgbuf->alloc - (cgbuf->ge - cgbuf->buf);
	ge = &buf[alloc - post_len];
	memcpy(ge, cgbuf->ge, post_len);

	memset(gs, CGBUF_GAP_MARKER, ge - gs);

	free(cgbuf->buf);
	cgbuf->buf = buf;
	cgbuf->gs = gs;
	cgbuf->ge = ge;
	cgbuf->alloc = alloc;
}

void cgbuf_pad(CGBuf *cgbuf, int c, size_t n) {
	if (n == 0)
		return;

	cgbuf_ensure(cgbuf, n);

	memmove(cgbuf->ge - n, cgbuf->ge, n);
	memset(cgbuf->ge, c, n);
	cgbuf->ge -= n;
}

void cgbuf_setcursor(CGBuf *cgbuf, size_t column) {
	char *point = &cgbuf->buf[column];
	size_t len;

	if (column > cgbuf->len)
		cgbuf_pad(cgbuf, CGBUF_BLANK, column - cgbuf->len);

	if (point == cgbuf->gs)
		return;

	if (point < cgbuf->gs) {
		len = cgbuf->gs - point;
		cgbuf->ge -= len;
		memmove(cgbuf->ge, point, len);
		cgbuf->gs = point;
	}
	else {
		len = point - cgbuf->gs;
		memmove(cgbuf->gs, cgbuf->ge, len);
		cgbuf->gs = point;
		cgbuf->ge += len;
	}

	memset(cgbuf->gs, CGBUF_GAP_MARKER, cgbuf->ge - cgbuf->gs);
}

void cgbuf_insert(CGBuf *cgbuf, const char *text, size_t len) {
	if (len == 0)
		len = strlen(text);

	if (len == 0)
		return;

	cgbuf_ensure(cgbuf, len);

	if (len == 1)
		*(cgbuf->gs++) = *text;
	else {
		memcpy(cgbuf->gs, text, len);
		cgbuf->gs += len;
	}

	cgbuf->len += len;
}

void cgbuf_append(CGBuf *cgbuf, const char *text, size_t len) {
	if (len == 0)
		len = strlen(text);

	if (len == 0)
		return;

	cgbuf_ensure(cgbuf, len);

	memmove(cgbuf->ge - len, cgbuf->ge, len);
	memcpy(cgbuf->ge, text, len);
	cgbuf->ge -= len;

	cgbuf->len += len;
}