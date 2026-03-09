#include "chttp/server.h"

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct chttp_server {
    struct addrinfo *ai;
    char *port;
};


int chttp_server_create(char *port, chttp_server **out_server) {
    chttp_server *server = malloc(sizeof(chttp_server));
    if(!server) {
        return -1;
    }

    server->ai = NULL;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    server->port = port == NULL ? "8080" : port;
    int gai_status;
    if((gai_status =
                getaddrinfo(    NULL,
                                server->port,
                                &hints,
                                &server->ai)) != 0) {
        chttp_server_destroy(server);
        return -1;
    }

    *out_server = server;
    return 0;
}

int chttp_server_start(chttp_server *s) {
    struct addrinfo *ai = s->ai;
    int sockfd;
    if((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
        return -1;
    }

    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    if(bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
        return -1;
    }

    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size;
    int clientfd;
    while(1) {
        listen(sockfd, 10);

        clientaddr_size = sizeof clientaddr;
        if((clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &clientaddr_size)) < 0) {
            return -1;
        }

        char req[1024] = { 0 };
        if(recv(clientfd, req, sizeof req, 0) == 0) {
            continue;
        }

        char msg[1024 + strlen(req)];
        snprintf(msg, sizeof msg, "pong\n");

        int len;
        len = strlen(msg);
        send(clientfd, msg, len, 0);

        if (close(clientfd) < 0) {
            return -1;
        }
    }

    if (close(sockfd) < 0) {
        return -1;
    }

    return 0;
}


void chttp_server_destroy(chttp_server *s) {
    if(s->ai) {
        freeaddrinfo(s->ai);
        s->ai = NULL;
    }

    free(s);
}
