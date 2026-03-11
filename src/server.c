#include "chttp/server.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "chttp/error.h"
#include "internal/dynarr.h"
#include "internal/httpssn.h"
#include "internal/log.h"

#define pollfd_arr ((struct pollfd*)_dynarr_get_as_arr(s->fds))

const char *revents_str(short revents, char *buf, size_t buflen);

struct chttp_server {
    struct addrinfo *ai;
    char *port;

    _dynarr *fds;
    _dynarr *ssns;

    bool running;
};


int chttp_server_create(char *port, chttp_server **out_server) {
    chttp_server *server = malloc(sizeof(chttp_server));
    if(!server) {
        return CHTTP_ERRNO;
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
        return CHTTP_ERR_GAI(gai_status);
    }

    int result;
    if((result = _dynarr_create(sizeof(struct pollfd*), 0, 0, &server->fds)) != 0) {
        chttp_server_destroy(server);
        return result;
    }

    if((result = _dynarr_create(sizeof(_httpssn*), 0, 0, &server->ssns)) != 0) {
        chttp_server_destroy(server);
        return result;
    }

    *out_server = server;
    return CHTTP_OK;
}

int chttp_server_start(chttp_server *s) {
    chttp_error cherr;

    struct addrinfo *ai = s->ai;
    struct pollfd server_pollfd;
    server_pollfd.events = POLLIN;
    int sockfd = -1;
    for (struct addrinfo *p = s->ai; p != NULL; p = p->ai_next) {
        if((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
            return CHTTP_ERRNO;
        }

        server_pollfd.fd = sockfd;

        int yes = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

        if(bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
            return CHTTP_ERRNO;
        }
        _dynarr_push(s->fds, &server_pollfd);

        break;
    }
    if (listen(sockfd, 10) < 0) {
        return CHTTP_ERRNO;
    }

    struct sockaddr_storage clientaddr;
    socklen_t clientaddr_size;
    int clientfd;
    _log("starting server");
    s->running = true;
    while(s->running) {
        int event_to_handle = poll(pollfd_arr, _dynarr_count(s->fds), -1);
        for (size_t i = 0; i < _dynarr_count(s->fds); i++) {
            if (pollfd_arr[i].revents != 0) {
                char buf[128]; (void)buf;
                _log("FD %d: %s", pollfd_arr[i].fd, revents_str(pollfd_arr[i].revents, buf, sizeof(buf)));
            }
        }

        bool is_new_connection = pollfd_arr[0].revents & POLLIN;
        if (is_new_connection) {
            clientaddr_size = sizeof clientaddr;
            if((clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &clientaddr_size)) < 0) {
                return CHTTP_ERRNO;
            }

            struct pollfd client_pollfd;
            client_pollfd.fd = clientfd;
            client_pollfd.events = POLLIN;

            _dynarr_push(s->fds, &client_pollfd);
            _httpssn *ssn;
            _httpssn_create(clientfd, &ssn);
            _dynarr_push(s->ssns, &ssn);
            event_to_handle--;
        }

        for (size_t i = 1; i < _dynarr_count(s->fds) && event_to_handle > 0; i++) {
            _httpssn **ssn_ptr;
            _dynarr_at(s->ssns, i - 1, (void**)&ssn_ptr);
            _httpssn *ssn = *ssn_ptr;

            if (pollfd_arr[i].revents & POLLIN) {
                cherr = _httpssn_recv(ssn);

                if (cherr != CHTTP_OK) {
                    _loge("_httpssn_recv: %s", chttp_strerror(cherr));
                    close(pollfd_arr[i].fd);
                    _dynarr_remove(s->fds, i);
                    _dynarr_remove(s->ssns, i - 1);
                    i--;
                } else if (_httpssn_status_get(ssn) == _HTTPSSN_DONE_RECV) {
                    pollfd_arr[i].events &= ~POLLIN;
                    pollfd_arr[i].events |= POLLOUT;
                }
                event_to_handle--;
            }

            if (pollfd_arr[i].revents & POLLOUT) {
                cherr = _httpssn_send(ssn);
                if(cherr != CHTTP_OK) {
                    _loge("_httpssn_send: %s", chttp_strerror(cherr));
                    close(pollfd_arr[i].fd);
                    _dynarr_remove(s->fds, i);
                    _dynarr_remove(s->ssns, i - 1);
                    i--;
                }
                if (_httpssn_status_get(ssn) == _HTTPSSN_DONE) {
                    close(pollfd_arr[i].fd);
                    _dynarr_remove(s->fds, i);
                    _dynarr_remove(s->ssns, i - 1);
                    i--;
                }
                event_to_handle--;
            }

        }
    }

    if (close(sockfd) < 0) {
        return CHTTP_ERRNO;
    }

    return CHTTP_OK;
}

void chttp_server_stop(chttp_server *s) {
    s->running = false;
}

void chttp_server_destroy(chttp_server *s) {
    if(s->ai) {
        freeaddrinfo(s->ai);
        s->ai = NULL;
    }

    _dynarr_destroy(s->fds);
    for (size_t i = 0; i < _dynarr_count(s->ssns); i++) {
        _httpssn *ssn;
        _dynarr_at(s->ssns, i, (void**)&ssn);

        _httpssn_destroy(ssn);
    }
    _dynarr_destroy(s->ssns);

    free(s);
}

const char *revents_str(short revents, char *buf, size_t buflen)
{
    static const struct { short flag; const char *name; } flags[] = {
        { POLLIN,     "POLLIN"     },
        { POLLPRI,    "POLLPRI"    },
        { POLLOUT,    "POLLOUT"    },
        { POLLERR,    "POLLERR"    },
        { POLLHUP,    "POLLHUP"    },
        { POLLNVAL,   "POLLNVAL"   },
        { POLLRDNORM, "POLLRDNORM" },
        { POLLRDBAND, "POLLRDBAND" },
        { POLLWRNORM, "POLLWRNORM" },
        { POLLWRBAND, "POLLWRBAND" },
    };

    buf[0] = '\0';
    int first = 1;
    for (size_t i = 0; i < sizeof(flags)/sizeof(flags[0]); i++) {
        if (revents & flags[i].flag) {
            if (!first)
                strncat(buf, " | ", buflen - strlen(buf) - 1);
            strncat(buf, flags[i].name, buflen - strlen(buf) - 1);
            first = 0;
        }
    }
    return buf;
}
