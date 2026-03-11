#ifndef CHTTP_INTERNAL_HTTPSSN_IMPL_H
#define CHTTP_INTERNAL_HTTPSSN_IMPL_H

#include "httpssn.h"

#include <stddef.h> // IWYU pragma: export
#include <stdbool.h> // IWYU pragma: export


struct _httpssn {
    int sockfd;

    char *headers;
    size_t headers_size;
    size_t headers_used;

    char *body;
    size_t body_size;
    size_t body_used;

    char *transfer_encoding;
    size_t content_len;

    _httpssn_status status;
    bool invalid_req;

};
#endif // !CHTTP_INTERNAL_HTTPSSN_IMPL_H
