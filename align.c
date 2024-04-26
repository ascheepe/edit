#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>

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

char *
xstrdup(const char *str)
{
	size_t len;
	char *dup;

	len = strlen(str) + 1;
	dup = xmalloc(len);
	memcpy(dup, str, len);

	return dup;
}

struct string {
	char *txt;
	size_t len;
};

static struct string *
new_string(const char *cstr)
{
	struct string *str;

	str = xmalloc(sizeof(*str));
	if (cstr != NULL) {
		str->txt = xstrdup(cstr);
		str->len = strlen(str->txt);
	} else {
		str->txt = NULL;
		str->len = 0;
	}

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

	str = new_string(NULL);
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

struct line {
	struct string *left;
	struct string *right;
};

struct line *
new_line(void)
{
	struct line *r;

	r = xmalloc(sizeof(*r));
	r->left = NULL;
	r->right = NULL;

	return r;
}

void
free_line(struct line *line)
{
	if (line->left)
		free_string(line->left);
	if (line->right)
		free_string(line->right);
	free(line);
}

int
main(int argc, char **argv)
{
	struct line **lines = NULL;
	size_t nlines = 0;
	size_t cap = 0;

	struct string *str;
	size_t maxlen = 0;

	int delim = ' ';
	size_t i;

	if (argc == 2)
		delim = argv[1][0];

	/*
	 * Read a string from stdin, split it by a delimiter
	 * and keep track of the length of the first part.
	 */
	while ((str = read_string(stdin)) != NULL) {
		struct line *line;
		char *dpos;

		line = new_line();
		dpos = strchr(str->txt, delim);
		if (dpos != NULL && dpos[1] != '\n') {
			*dpos = '\0';
			line->left = new_string(str->txt);
			line->right = new_string(&dpos[1]);
			*dpos = delim;
			if (line->left->len > maxlen)
				maxlen = line->left->len;
		} else
			line->left = new_string(str->txt);

		if (nlines >= cap) {
			lines = xrealloc(lines, sizeof(*lines) * (cap + 32));
			cap += 32;
		}
		lines[nlines++] = line;

		free_string(str);
	}
	lines = xrealloc(lines, sizeof(*lines) * nlines);

	/* Print aligned strings and free them. */
	for (i = 0; i < nlines; ++i) {
		struct line *line = lines[i];

		if (line->right)
			printf("%-*s%c%s\n", (int)maxlen, line->left->txt,
			    delim, line->right->txt);
		else
			write_string(line->left, stdout);

		free_line(lines[i]);
	}

	free(lines);

	return 0;
}
