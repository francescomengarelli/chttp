
#include <stdio.h>
#include <stdlib.h>

#include "internal/uri.h"

#define try_uri(str)\
{\
_uri *uri = _stouri(str);\
if(!uri) {\
    fprintf(stderr, "failed to generate uri '%s'\n", str);\
    return -1;\
}\
_freeuri(uri);\
}\

int main(void)
{
    try_uri("http://hostname:8080/api/test");
    return EXIT_SUCCESS;
}
