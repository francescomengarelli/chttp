#ifndef CHTTP_SERVER_H
#define CHTTP_SERVER_H
// IWYU pragma: private, include "chttp/chttp.h"

typedef struct chttp_server chttp_server;

int chttp_server_create(char *port, chttp_server **out_server);

int chttp_server_start(chttp_server *s);

void chttp_server_destroy(chttp_server *server);

#endif // !CHTTP_SERVER_H
