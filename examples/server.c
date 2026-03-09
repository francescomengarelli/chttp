#include <stdio.h>
#include <stdlib.h>

#include "chttp/chttp.h"

int main(void)
{
    chttp_server *server;
    chttp_server_create(NULL, &server);
    printf("starting server..\n");
    chttp_server_start(server);
    chttp_server_destroy(server);
    server = NULL;
    return EXIT_SUCCESS;
}
