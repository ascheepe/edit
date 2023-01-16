#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char line[1024];

int main(int argc, char **argv)
{
    int width = 0;

    if (argc > 1) {
        width = atoi(argv[1]);
    }

    if (width < 1) {
        width = 80;
    }

    while (fgets(line, sizeof(line), stdin) != NULL) {
        size_t length;
        size_t offset;

        length = strlen(line);
        offset = (width - length) / 2;
        printf("%*s", (int) (offset + length), line);
    }

    return 0;
}
