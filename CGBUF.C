#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "cgbuf.h"

#define CGBUF_GAP_INIT_ALLOC (8)
#define CGBUF_GAP_MARKER (-1)
#define CGBUF_BLANK (' ')

#define cgbuf_gs(b)     &b->buf[b->pre_len]
#define cgbuf_ge(b)     &b->buf[b->alloc - b->post_len]
#define cgbuf_glen(b)   (b->alloc - cgbuf_len(b))

#define _markgap(b) do {                                   \
	memset(cgbuf_gs(b), CGBUF_GAP_MARKER, cgbuf_glen(b));  \
} while(0)

static void _alloc(CGBuf *cgbuf, size_t min) {
	size_t alloc = CGBUF_GAP_INIT_ALLOC;

	while (alloc <= min)
		alloc *= 2;

	cgbuf->buf = malloc(alloc);
	cgbuf->alloc = alloc;
	cgbuf->pre_len = cgbuf->post_len = 0;

	_markgap(cgbuf);
}

void cgbuf_init(CGBuf *cgbuf, const char *text, size_t len) {
	size_t i;

	if (len == 0 && text != NULL) len = strlen(text);

	if (len) {
		_alloc(cgbuf, len);
		cgbuf->post_len = len;
		memcpy(cgbuf_ge(cgbuf), text, len);
	}
	else
		memset(cgbuf, 0, sizeof *cgbuf);
}

void cgbuf_fini(CGBuf *cgbuf) {
	if (cgbuf->alloc)
		free(cgbuf->buf);

	memset(cgbuf, 0, sizeof *cgbuf);
}

void cgbuf_ensure(CGBuf *cgbuf, size_t extra) {
	size_t alloc;
	char *buf;

	if (cgbuf->alloc > cgbuf_len(cgbuf) + extra)
		return;

	if (cgbuf->alloc == 0) {
		_alloc(cgbuf, extra);
		return;
	}

	alloc = cgbuf->alloc;
	while (alloc < cgbuf_len(cgbuf) + extra)
		alloc *= 2;

	buf = malloc(alloc);

	if (cgbuf->pre_len)
		memcpy(buf, cgbuf->buf, cgbuf->pre_len);

	if (cgbuf->post_len)
		memcpy(buf + alloc - cgbuf->post_len,
			   cgbuf_ge(cgbuf), cgbuf->post_len);

	free(cgbuf->buf);
	cgbuf->buf = buf;
	cgbuf->alloc = alloc;

	_markgap(cgbuf);
}

void cgbuf_setcursor(CGBuf *cgbuf, size_t column) {
	size_t len;

	if (column > cgbuf_len(cgbuf))
		cgbuf_appendc(cgbuf, CGBUF_BLANK, column - cgbuf_len(cgbuf));

	if (column == cgbuf->pre_len)
		return;

	if (column < cgbuf->pre_len) {
		len = cgbuf->pre_len - column;
		memmove(cgbuf_ge(cgbuf) - len, &cgbuf->buf[column], len);
		cgbuf->pre_len -= len;
		cgbuf->post_len += len;
	}
	else {
		len = column - cgbuf->pre_len;
		memmove(cgbuf_gs(cgbuf), cgbuf_ge(cgbuf), len);
		cgbuf->pre_len += len;
		cgbuf->post_len -= len;
	}

	_markgap(cgbuf);
}

void cgbuf_insertc(CGBuf *cgbuf, int c, size_t n) {
	cgbuf_ensure(cgbuf, n);

	while (n--)
		cgbuf->buf[cgbuf->pre_len++] = c;
}

void cgbuf_insert(CGBuf *cgbuf, const char *text, size_t len) {
	if (len == 0)
		len = strlen(text);

	if (len == 0)
		return;

	cgbuf_ensure(cgbuf, len);

	if (len == 1)
		cgbuf->buf[cgbuf->pre_len++] = *text;
	else {
		memcpy(cgbuf_gs(cgbuf), text, len);
		cgbuf->pre_len += len;
	}
}

void cgbuf_appendc(CGBuf *cgbuf, int c, size_t n) {
	if (n == 0)
		return;

	cgbuf_ensure(cgbuf, n);

	if (cgbuf->post_len)
		memmove(cgbuf_ge(cgbuf) - n, cgbuf_ge(cgbuf), cgbuf->post_len);

	memset(cgbuf_ge(cgbuf), c, n);
	cgbuf->post_len += n;
}

void cgbuf_append(CGBuf *cgbuf, const char *text, size_t len) {
	if (len == 0)
		len = strlen(text);

	if (len == 0)
		return;

	cgbuf_ensure(cgbuf, len);

	if (cgbuf->post_len)
		memmove(cgbuf_ge(cgbuf) - len, cgbuf_ge(cgbuf), cgbuf->post_len);
	memcpy(cgbuf_ge(cgbuf), text, len);
	cgbuf->post_len += len;
}

void cgbuf_prependc(CGBuf *cgbuf, int c, size_t n) {
	if (n == 0)
		return;

	cgbuf_ensure(cgbuf, n);

	if (cgbuf->pre_len)
		memmove(cgbuf->buf + n, cgbuf->buf, cgbuf->pre_len);
	memset(cgbuf->buf, c, n);
	cgbuf->pre_len += n;
}

void cgbuf_prepend(CGBuf *cgbuf, const char *text, size_t len) {
	if (len == 0)
		len = strlen(text);

	if (len == 0)
		return;

	cgbuf_ensure(cgbuf, len);

	if (cgbuf->pre_len)
		memmove(cgbuf->buf + len, cgbuf->buf, cgbuf->pre_len);
	memcpy(cgbuf->buf, text, len);
	cgbuf->pre_len += len;
}

const char *cgbuf_string(CGBuf *cgbuf, char *dest, size_t len) {
	if (len < 2)
		return NULL;

	len = min(len - 1, cgbuf_len(cgbuf));

	strncpy(dest, cgbuf->buf, min(len, cgbuf->pre_len));

	if (len > cgbuf->pre_len)
		strncat(dest, cgbuf_ge(cgbuf),
			min(len - cgbuf->pre_len, cgbuf->post_len));

	dest[len] = '\0';
	return dest;
}