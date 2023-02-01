#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Our internal structure tracking a memory stream
 */
struct memstream
{
	char	*buf;	/* in-memory buffer */
	size_t	rsize;	/* real size of buffer */
	size_t	vsize;	/* virtual size of buffer */
	size_t	curpos;	/* current position in buffer */
	int		flags;	/* see below */
};

/* flags */
#define MS_READ		0x01	/* open for reading */
#define MS_WRITE	0x02	/* open for writing */
#define MS_APPEND	0x04	/* append to stream */
#define MS_TRUNCATE	0x08	/* truncate the stream on open */
#define MS_MYBUF	0x10	/* free buffer on close */

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static int mstream_read(void *, char *, int);
static int mstream_write(void *, const char *, int);
static fpos_t mstream_seek(void *, fpos_t, int);
static int mstream_close(void *);
static int type_to_flags(const char *__restrict type);
static off_t find_end(char *buf, size_t len);

FILE *
fmemopen(void *__restrict buf, size_t size,
    const char *__restrict type)
{
	struct memstream *ms;
	FILE *fp;

	if (size == 0) {
		errno = EINVAL;
		return(NULL);
	}
	if ((ms = malloc(sizeof(struct memstream))) == NULL) {
		errno = ENOMEM;
		return(NULL);
	}
	if ((ms->flags = type_to_flags(type)) == 0) {
		errno = EINVAL;
		free(ms);
		return(NULL);
	}
	if (buf == NULL) {
		if ((ms->flags & (MS_READ|MS_WRITE)) !=
		  (MS_READ|MS_WRITE)) {
			errno = EINVAL;
			free(ms);
			return(NULL);
		}
		if ((ms->buf = malloc(size)) == NULL) {
			errno = ENOMEM;
			free(ms);
			return(NULL);
		}
		ms->rsize = size;
		ms->flags |= MS_MYBUF;
		ms->curpos = 0;
	} else {
		ms->buf = buf;
		ms->rsize = size;
		if (ms->flags & MS_APPEND)
			ms->curpos = find_end(ms->buf, ms->rsize);
		else
			ms->curpos = 0;
	}
	if (ms->flags & MS_APPEND) {			/* "a" mode */
		ms->vsize = ms->curpos;
	} else if (ms->flags & MS_TRUNCATE) {	/* "w" mode */
		ms->vsize = 0;
	} else {								/* "r" mode */
		ms->vsize = size;
	}
	fp = funopen(ms, mstream_read, mstream_write,
	  mstream_seek, mstream_close);
	if (fp == NULL) {
		if (ms->flags & MS_MYBUF)
			free(ms->buf);
		free(ms);
	}
	return(fp);
}

static int
type_to_flags(const char *__restrict type)
{
	const char *cp;
	int flags = 0;

	for (cp = type; *cp != 0; cp++) {
		switch (*cp) {
		case 'r':
			if (flags != 0)
				return(0);	/* error */
			flags |= MS_READ;
			break;

		case 'w':
			if (flags != 0)
				return(0);	/* error */
			flags |= MS_WRITE|MS_TRUNCATE;
			break;

		case 'a':
			if (flags != 0)
				return(0);	/* error */
			flags |= MS_APPEND;
			break;

		case '+':
			if (flags == 0)
				return(0);	/* error */
			flags |= MS_READ|MS_WRITE;
			break;

		case 'b':
			if (flags == 0)
				return(0);	/* error */
			break;

		default:
			return(0);		/* error */
		}
	}
	return(flags);
}

static off_t
find_end(char *buf, size_t len)
{
	off_t off = 0;

	while (off < len) {
		if (buf[off] == 0)
			break;
		off++;
	}
	return(off);
}

static int
mstream_read(void *cookie, char *buf, int len)
{
	int nr;
	struct memstream *ms = cookie;

	if (!(ms->flags & MS_READ)) {
		errno = EBADF;
		return(-1);
	}
	if (ms->curpos >= ms->vsize)
		return(0);

	/* can only read from curpos to vsize */
	nr = MIN(len, ms->vsize - ms->curpos);
	memcpy(buf, ms->buf + ms->curpos, nr);
	ms->curpos += nr;
	return(nr);
}

static int
mstream_write(void *cookie, const char *buf, int len)
{
	int nw, off;
	struct memstream *ms = cookie;

	if (!(ms->flags & (MS_APPEND|MS_WRITE))) {
		errno = EBADF;
		return(-1);
	}
	if (ms->flags & MS_APPEND)
		off = ms->vsize;
	else
		off = ms->curpos;
	nw = MIN(len, ms->rsize - off);
	memcpy(ms->buf + off, buf, nw);
	ms->curpos = off + nw;
	if (ms->curpos > ms->vsize) {
		ms->vsize = ms->curpos;
		if (((ms->flags & (MS_READ|MS_WRITE)) ==
		  (MS_READ|MS_WRITE)) && (ms->vsize < ms->rsize))
			*(ms->buf + ms->vsize) = 0;
	}
	if ((ms->flags & (MS_WRITE|MS_APPEND)) &&
	  !(ms->flags & MS_READ)) {
		if (ms->curpos < ms->rsize)
			*(ms->buf + ms->curpos) = 0;
		else
			*(ms->buf + ms->rsize - 1) = 0;
	}
	return(nw);
}

static fpos_t
mstream_seek(void *cookie, fpos_t pos, int whence)
{
	int off;
	struct memstream *ms = cookie;

	switch (whence) {
	case SEEK_SET:
		off = pos;
		break;
	case SEEK_END:
		off = ms->vsize + pos;
		break;
	case SEEK_CUR:
		off = ms->curpos + pos;
		break;
	}
	if (off < 0 || off > ms->vsize) {
		errno = EINVAL;
		return -1;
	}
	ms->curpos = off;
	return(off);
}

static int
mstream_close(void *cookie)
{
	struct memstream *ms = cookie;

	if (ms->flags & MS_MYBUF)
		free(ms->buf);
	free(ms);
	return(0);
}
