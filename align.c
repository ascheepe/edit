#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>

#define TABSIZE (8)

static void *
xmalloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (ptr == NULL)
		errx(1, "malloc: out of memory.");

	return ptr;
}

static void *
xrealloc(void *ptr, size_t size)
{
	void *new_ptr;

	new_ptr = realloc(ptr, size);
	if (new_ptr == NULL)
		errx(1, "realloc: out of memory.");

	return new_ptr;
}

struct string {
	char *txt;
	size_t len;
};

static struct string *
new_string(void)
{
	struct string *str;

	str = xmalloc(sizeof(*str));
	str->txt = NULL;
	str->len = 0;

	return str;
}

static void
free_string(struct string *str)
{
	free(str->txt);
	free(str);
}

static struct string *
read_string(FILE *fp)
{
	struct string *str;
	size_t cap;
	int ch;

	str = new_string();
	cap = 0;

	while ((ch = fgetc(fp)) != EOF && ch != '\n') {
		if (str->len >= cap) {
			str->txt = xrealloc(str->txt, cap + 32);
			cap += 32;
		}
		str->txt[str->len++] = ch;
	}

	if (str->txt == NULL && ch != '\n') {
		free_string(str);
		return NULL;
	}

	str->txt = xrealloc(str->txt, str->len + 1);
	str->txt[str->len] = '\0';

	return str;
}

static void
write_string(const struct string *str, FILE *fp)
{
	fwrite(str->txt, str->len, 1, fp);
	fputc('\n', fp);
}

int
main(int argc, char **argv)
{
	struct string **lines = NULL;
	size_t nlines = 0;
	size_t cap = 0;

	struct string *line;
	size_t maxlen = 0;

	int delimiter = ' ';
	size_t i;
	char *p;

	if (argc == 2)
		delimiter = argv[1][0];

	/* Read all the things! */
	while ((line = read_string(stdin)) != NULL) {
		if (nlines >= cap) {
			lines = xrealloc(lines, sizeof(*lines) * (cap + 32));
			cap += 32;
		}
		lines[nlines++] = line;
	}
	lines = xrealloc(lines, sizeof(*lines) * nlines);

	/* Find the maximum length of a string before the delimiter. */
	for (i = 0; i < nlines; ++i) {
		line = lines[i];
		p = strchr(line->txt, delimiter);
		if (p != NULL) {
			size_t len;

			*p = '\0';
			len = strlen(line->txt);
			if (len > maxlen)
				maxlen = len;
			*p = delimiter;
		}
	}

	/* Align strings which have a delimiter. */
	for (i = 0; i < nlines; ++i) {
		line = lines[i];
		p = strchr(line->txt, delimiter);
		if (p != NULL && p[1] != '\n') {
			*p = '\0';
			printf("%-*s%c%s\n", (int)maxlen,
			    line->txt, delimiter, &p[1]);
			*p = delimiter;
		} else
			write_string(line, stdout);
	}

	for (i = 0; i < nlines; ++i)
		free_string(lines[i]);
	free(lines);

	return 0;
}
