#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char buf[4096];
    if (!fgets(buf, sizeof(buf), f)) {
        fprintf(stderr, "Error reading file\n");
        fclose(f);
        return 1;
    }
    fclose(f);

    for (int i = 0; buf[i]; i++) {
        if (buf[i] == '\n') {
            buf[i] = '\0';
            break;
        }
    }

    for (int i = 0; buf[i] != '\0'; i++) {
        buf[i] = buf[i] - i;
    }

    printf("%s\n", buf);
    return 0;
}
