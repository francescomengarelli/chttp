#include "chttp/client.h"
#include "chttp/chttp.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

int chttp_get(char *req, char *hostname, char *port, char **out_res) {
    port = port == NULL ? "http" : port;
    int sockfd;
    struct addrinfo hints, *ai;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (hostname == NULL || strcmp(hostname, CHTTP_LOCALHOST) == 0) {
        hostname = NULL;
        hints.ai_flags = AI_PASSIVE;
    }

    int result;
    if((result = getaddrinfo(hostname, port, &hints, &ai)) < 0) {
        return -1;
    }

    if((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
        freeaddrinfo(ai);
        return -1;
    }

    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    if(connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
        freeaddrinfo(ai);
        return -1;
    }

    send(sockfd, req, strlen(req), 0);

    char *res = calloc(1, 1024);
    recv(sockfd, res, 1023, 0);

    close(sockfd);

    freeaddrinfo(ai);
    if (out_res == NULL) {
        free(res);
    }
    else {
        *out_res = res;
    }

    return 0;
}
