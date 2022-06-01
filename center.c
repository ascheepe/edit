#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
static char line[BUFFER_SIZE];

int main(int argc, char **argv) {
    int width = 0;

    if (argc > 1) {
        width = atoi(argv[1]);
    }

    if (width < 1) {
        width = 80;
    }

    while (fgets(line, sizeof(line), stdin) != NULL) {
        size_t line_length;
        size_t length;

        line_length = strlen(line);
        length = (width - line_length) / 2;

        printf("%*s", (int) (length + line_length), line);
    }

    return 0;
}

