#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <err.h>

static void *
xcalloc(size_t nmemb, size_t size)
{
	void *ret;

	ret = calloc(nmemb, size);
	if (ret == NULL)
		errx(1, "calloc: out of memory.");

	return ret;
}

static void *
xrealloc(void *ptr, size_t size)
{
	void *ret;

	ret = realloc(ptr, size);
	if (ret == NULL)
		errx(1, "realloc: out of memory.");

	return ret;
}

static char *
xstrdup(const char *str)
{
	char *ret;
	size_t size;

	if (str == NULL)
		return NULL;

	size = strlen(str) + 1;
	ret = xcalloc(1, size);
	memcpy(ret, str, size);

	return ret;
}

/*
 * Aligned lines are split into a first and rest part by
 * a delimiter. The length of the first part is used to
 * calculate the alignment.
 */
struct line {
	char *first;
	char *rest;
	size_t firstlen;
};

static struct line *
readline(FILE *f, int delimiter)
{
	struct line *ret;
	char *dpos, *line = NULL;
	size_t len = 0, maxlen = 0;
	int ch;

	while ((ch = fgetc(f)) != EOF && ch != '\n') {
		if (len >= maxlen) {
			line = xrealloc(line, maxlen + 32);
			maxlen += 32;
		}

		line[len++] = ch;
	}

	/*
	 * We should be able to read empty lines so
	 * only return NULL if at end of file.
	 */
	if (line == NULL && ch == EOF)
		return NULL;

	line = xrealloc(line, len + 1);
	line[len] = '\0';

	ret = xcalloc(1, sizeof(*ret));

	/*
	 * Split the line into a first and rest part.  If no delimiter is
	 * found first will contain the full line (which may be the empty
	 * string).
	 */
	dpos = strchr(line, delimiter);

	if (dpos != NULL) {
		char *rest = dpos + 1;

		*dpos = '\0';

		if (*rest != '\0')
			ret->rest = xstrdup(rest);
	}

	ret->first = xstrdup(line);
	ret->firstlen = strlen(line);
	free(line);

	return ret;
}

int
main(int argc, char **argv)
{
	struct line *line, **lines = NULL;
	size_t nlines = 0, maxlines = 0;
	size_t maxfirstlen = 0;
	int delimiter = ' ';
	size_t i;

	if (argc == 2)
		delimiter = argv[1][0];

	while ((line = readline(stdin, delimiter)) != NULL) {
		if (nlines >= maxlines) {
			size_t newmax = maxlines + 32;

			lines = xrealloc(lines, sizeof(*lines) * newmax);
			maxlines = newmax;
		}

		/* Only align and get the max length if we have two parts. */
		if (line->rest != NULL && line->firstlen > maxfirstlen)
			maxfirstlen = line->firstlen;

		lines[nlines++] = line;
	}

	lines = xrealloc(lines, sizeof(*lines) * nlines);

	for (i = 0; i < nlines; ++i) {
		struct line *line = lines[i];

		if (line->rest != NULL) {
			printf("%-*s%c%s\n", (int) maxfirstlen, line->first,
			    delimiter, line->rest);
			free(line->rest);
		} else
			printf("%s\n", line->first);

		free(line->first);
		free(line);
	}

	free(lines);

	return 0;
}

