#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "chttp/chttp.h"

chttp_server *g_server;

// stop server on CTRL+C
void handler(int sig) {
    (void)sig;
    chttp_server_stop(g_server);
}

int main(void)
{
    // setup server stop
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);

    // create and start server
    chttp_error cherr;
    chttp_server *server;

    cherr = chttp_server_create(NULL, &server);
    if(cherr != CHTTP_OK) {
        fprintf(stderr, "server create: %s\n", chttp_strerror(cherr));
    }

    g_server = server;

    cherr = chttp_server_start(server);
    if(cherr != CHTTP_OK) {
        fprintf(stderr, "server: %s\n", chttp_strerror(cherr));
    }

    printf("server stopped.\n");
    chttp_server_destroy(server);

    server = NULL;
    return EXIT_SUCCESS;
}
