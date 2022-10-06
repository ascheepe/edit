#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <err.h>

static void *xcalloc(size_t nmemb, size_t size)
{
    void *result;

    result = calloc(nmemb, size);
    if (result == NULL) {
        errx(1, "calloc: out of memory.");
    }

    return result;
}

static void *xrealloc(void *ptr, size_t size)
{
    void *result;

    result = realloc(ptr, size);
    if (result == NULL) {
        errx(1, "realloc: out of memory.");
    }

    return result;
}

static char *xstrdup(const char *str)
{
    char *result;
    size_t size;

    if (str == NULL) {
        return NULL;
    }

    size = strlen(str) + 1;
    result = xcalloc(1, size);
    memcpy(result, str, size);

    return result;
}

/*
 * Aligned lines are split into a first and rest part by
 * a delimiter. The length of the first part is used to
 * calculate the alignment.
 */
struct line {
    char *first;
    char *rest;
    size_t first_length;
};

static struct line *readline(FILE *f, int delimiter)
{
    struct line *result = NULL;
    char *dot_position = NULL;
    char *line = NULL;
    size_t line_length = 0;
    size_t max_line_length = 0;
    int ch;

    while ((ch = fgetc(f)) != EOF && ch != '\n') {
        if (line_length >= max_line_length) {
            line = xrealloc(line, max_line_length + 32);
            max_line_length += 32;
        }

        line[line_length++] = ch;
    }

    /*
     * We should be able to read empty lines so
     * only return NULL if at end of file.
     */
    if (line == NULL && ch == EOF) {
        return NULL;
    }

    line = xrealloc(line, line_length + 1);
    line[line_length] = '\0';

    result = xcalloc(1, sizeof(*result));

    /*
     * Split the line into a first and rest part.  If no delimiter is
     * found first will contain the full line (which may be the empty
     * string).
     */
    dot_position = strchr(line, delimiter);

    if (dot_position != NULL) {
        char *rest = dot_position + 1;

        *dot_position = '\0';

        if (*rest != '\0') {
            result->rest = xstrdup(rest);
        }
    }

    result->first = xstrdup(line);
    result->first_length = strlen(line);
    free(line);

    return result;
}

int main(int argc, char **argv)
{
    struct line *line = NULL;
    struct line **lines = NULL;
    size_t lines_count = 0;
    size_t max_lines = 0;
    size_t max_first_length = 0;
    int delimiter = ' ';
    size_t i;

    if (argc == 2) {
        delimiter = argv[1][0];
    }

    while ((line = readline(stdin, delimiter)) != NULL) {
        if (lines_count >= max_lines) {
            size_t new_max = max_lines + 32;

            lines = xrealloc(lines, sizeof(*lines) * new_max);
            max_lines = new_max;
        }

        /* Only align and get the max length if we have two parts. */
        if (line->rest != NULL && line->first_length > max_first_length) {
            max_first_length = line->first_length;
        }

        lines[lines_count++] = line;
    }

    lines = xrealloc(lines, sizeof(*lines) * lines_count);

    for (i = 0; i < lines_count; ++i) {
        struct line *line = lines[i];

        if (line->rest != NULL) {
            printf("%-*s%c%s\n", (int) max_first_length, line->first,
                   delimiter, line->rest);
            free(line->rest);
        } else {
            printf("%s\n", line->first);
        }

        free(line->first);
        free(line);
    }

    free(lines);

    return 0;
}
