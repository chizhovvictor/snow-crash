#include <unistd.h>

// change getuid
uid_t getuid(void) {
    return 4242;
}

// change geteuid
uid_t geteuid(void) {
    return 4242;
}
