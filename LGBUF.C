#include <stdlib.h>

#include "cgbuf.h"

/*** move to header ***/

typedef struct lgbuf {
	CGBuf *buf;
	size_t alloc;
	size_t pre_len;
	size_t post_len;
} LGBuf;

#define lgbuf_len(b)	(b->pre_len + b->post_len)

/*** end header ***/

#define LGBUF_GAP_INIT_ALLOC (8)

#define lgbuf_gs(b)		&b->buf[b->pre_len]
#define lgbuf_ge(b)		&b->buf[b->alloc - b->post_len]
#define lgbuf_glen(b)	(b->alloc - lgbuf_len(b))

static void _alloc(LGBuf *lgbuf, size_t n) {
	size_t alloc = LGBUF_GAP_INIT_ALLOC;

	while (alloc < n)
		alloc *= 2;

	lgbuf->buf = calloc(alloc, sizeof(*lgbuf->buf));
	lgbuf->alloc = alloc;
	lgbuf->pre_len = lgbuf->post_len = 0;
}

void lgbuf_init(LGBuf *lgbuf, const char **lines, size_t n) {
	size_t i;

	if (n) {
		_alloc(lgbuf, n);
		lgbuf->post_len = n;
		for (i = 0; i < n; i++)
			cgbuf_init(lgbuf_ge(lgbuf) + i, lines[i], 0);
	}
	else
		memset(lgbuf, 0, sizeof *lgbuf);
}

void lgbuf_ensure(LGBuf *lgbuf, size_t extra) {
	size_t alloc;
	CGBuf *buf;

	if (lgbuf->alloc > lgbuf_len(lgbuf) + extra)
		return;

	if (lgbuf->alloc == 0) {
		_alloc(lgbuf, extra);
		return;
	}

	alloc = lgbuf->alloc;
	while (alloc < lgbuf_len(lgbuf) + extra)
		alloc *= 2;

	buf = malloc(alloc * sizeof(*buf));

	if (lgbuf->pre_len)
		memcpy(buf, lgbuf->buf, lgbuf->pre_len);

	if (lgbuf->post_len)
		memcpy(buf + alloc - lgbuf->post_len,
			lgbuf_ge(lgbuf), lgbuf->post_len);

	free(lgbuf->buf);
	lgbuf->buf = buf;
	lgbuf->alloc = alloc;
}

void lgbuf_setcursor(LGBuf *lgbuf, size_t line) {
	size_t len;

	if (line > lgbuf_len(lgbuf))
		lgbuf_pad(lgbuf, NULL, line - lgbuf_len(lgbuf));

	if (line == lgbuf->pre_len)
		return;

	if (line < lgbuf->pre_len) {
		len = lgbuf->pre_len = line;
		memmove(lgbuf_ge(lgbuf) - len, &lgbuf->buf[line], len);
		lgbuf->pre_len -= len;
		lgbuf->post_len += len;
	}
	else {
		len = line - lgbuf->pre_len;
		memmove(lgbuf_gs(lgbuf), lgbuf_ge(lgbuf), len);
		lgbuf->pre_len += len;
		lgbuf->post_len -= len;
	}
}

void lgbuf_insertl(LGBuf *lgbuf, const char *text, size_t len) {
	lgbuf_ensure(lgbuf, 1);

	if (text)
		cgbuf_insert(lgbuf_gs(lgbuf), text, len);

	lgbuf->pre_len++;
}