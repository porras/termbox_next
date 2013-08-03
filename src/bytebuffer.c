#include "bytebuffer.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void bytebuffer_reserve(struct bytebuffer *b, int cap) {
	if (b->cap >= cap) {
		return;
	}

	// prefer doubling capacity
	if (b->cap * 2 >= cap) {
		cap = b->cap * 2;
	}

	char *newbuf = malloc(cap);
	if (b->len > 0) {
		// copy what was there, b->len > 0 assumes b->buf != null
		memcpy(newbuf, b->buf, b->len);
	}
	if (b->buf) {
		// in case there was an allocated buffer, free it
		free(b->buf);
	}
	b->buf = newbuf;
	b->cap = cap;
}

void init_bytebuffer(struct bytebuffer *b, int cap) {
	b->cap = 0;
	b->len = 0;
	b->buf = 0;

	if (cap > 0) {
		b->cap = cap;
		b->buf = malloc(cap); // just assume malloc works always
	}
}

void free_bytebuffer(struct bytebuffer *b) {
	if (b->buf)
		free(b->buf);
}

void clear_bytebuffer(struct bytebuffer *b) {
	b->len = 0;
}

void bytebuffer_puts(struct bytebuffer *b, const char *str) {
	bytebuffer_append(b, str, strlen(str));
}

void bytebuffer_append(struct bytebuffer *b, const char *data, int len) {
	bytebuffer_reserve(b, b->len + len);
	memcpy(b->buf + b->len, data, len);
	b->len += len;
}

void resize_bytebuffer(struct bytebuffer *b, int len) {
	bytebuffer_reserve(b, len);
	b->len = len;
}

void flush_bytebuffer(struct bytebuffer *b, int fd) {
	write(fd, b->buf, b->len);
	clear_bytebuffer(b);
}

void bytebuffer_truncate(struct bytebuffer *b, int n) {
	if (n <= 0)
		return;
	if (n > b->len)
		n = b->len;
	const int nmove = b->len - n;
	memmove(b->buf, b->buf+n, nmove);
	b->len -= n;
}
