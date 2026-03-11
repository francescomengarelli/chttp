#include "chttp/client.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include "chttp/error.h"

#include "internal/io.h"
#include "internal/uri.h"
#include "internal/log.h"

int chttp_post(char *uristr, char *body, char **out_res) {
    chttp_error chttperr = 0;
    struct addrinfo hints, *ai;

    _uri *uri;
    if((chttperr = _uri_create(uristr, &uri))) {
        return chttperr;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int result;
    if((result = getaddrinfo(uri->host, uri->port, &hints, &ai)) < 0) {
        return CHTTP_ERR_GAI(result);
    }
    _log("connecting to %s:%s", uri->host, uri->port);

    _uri_destroy(uri);
    int sockfd = -1;
    for (struct addrinfo *p = ai; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            continue;
        }

        if(connect(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            close(sockfd);
            continue;
        }
        int yes = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
        break;
    }
    if (sockfd < 0) {
        return CHTTP_ERR_SYS(errno);
    }


    if((chttperr = _sendall(sockfd, body, strlen(body) + 1)) != CHTTP_OK) {
        return chttperr;
    }

    char *res;

    if ((chttperr = _recvall(sockfd, &res)) != CHTTP_OK) {
        return chttperr;
    }

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
