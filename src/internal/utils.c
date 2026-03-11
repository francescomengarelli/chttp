#include "internal/utils.h"

#include <ctype.h>
#include <stddef.h>
#include <string.h>

void _strtolower(const char *str, char **out_str) {

    for (size_t i = 0; i < strlen(str); i++) {
        (*out_str)[i] = tolower(str[i]);
    }
}

size_t _minzu(size_t a, size_t b) {
    return a > b ? a : b;
}
