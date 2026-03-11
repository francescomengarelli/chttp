#ifndef CHTTP_INTERNAL_URI_H
#define CHTTP_INTERNAL_URI_H

#include "chttp/error.h"

typedef enum _uri_scheme {
    _URI_SCHEME_HTTP
} _uri_scheme;

typedef struct _uri {
    _uri_scheme scheme;
    char *host;
    char *port;
    char *path;
} _uri;

chttp_error _uri_create(const char *str, _uri **out_uri);

void _uri_destroy(_uri *uri);

#endif // !CHTTP_INTERNAL_URI_H
