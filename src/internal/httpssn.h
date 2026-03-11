#ifndef CHTTP_INTERNAL_HTTPSSN_H
#define CHTTP_INTERNAL_HTTPSSN_H

// HTTP SeSsioN
#include <stddef.h>
#include <stdbool.h>

typedef enum _httpssn_status {
    _HTTPSSN_NEED_HEADER,
    _HTTPSSN_DONE_HEADER,
    _HTTPSSN_DONE_RECV,
    _HTTPSSN_DONE_RES,
    _HTTPSSN_DONE
} _httpssn_status;

typedef struct _httpssn _httpssn;

int _httpssn_create(int sockfd, _httpssn **out_ssn);

int _httpssn_recv(_httpssn *ssn);
int _httpssn_send(_httpssn *ssn);

_httpssn_status _httpssn_status_get(_httpssn *ssn);

size_t _httpssn_sizeof(void);

void _httpssn_destroy(_httpssn *ssn);

#endif // !CHTTP_HTTPSSN_H

