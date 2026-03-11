#ifndef CHTTP_CLIENT_H
#define CHTTP_CLIENT_H
// IWYU pragma: private, include "chttp/chttp.h"

int chttp_post(char *uri, char *body, char **out_res);

#endif // !CHTTP_CLIENT_H
