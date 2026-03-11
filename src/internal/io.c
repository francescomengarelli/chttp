#include "internal/io.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>

#include "internal/log.h"
#include "chttp/error.h"

int _sendall(int sockfd, char *buf, size_t buf_size) {
    size_t len = buf_size;
    size_t total_sent = 0, total_len = len;
    const char *ptr = buf;
    _log();
    while (len > 0) {
        ssize_t sent = send(sockfd, ptr, len, 0);
        if (sent < 0) {
            return CHTTP_ERR_SYS(errno);
        }
        total_sent += sent;
        ptr += sent;
        len -= sent;

        (void)total_len; (void)total_sent;
        _log(_clear1up "Sending %zu/%zub", total_sent, total_len);
    }
    _log(_clear1up "Sent %zu/%zub", total_sent, total_len);
    return 0;
}

int _recvall(int sockfd, char **out_buf) {
    char *buf = NULL;

    size_t received = 0;
    _log();
    while (1) {
        buf = realloc(buf, received + 1024);
        if(!buf) {
            return CHTTP_ERR_SYS(errno);
        }

        int n = recv(sockfd, buf + received, 1023, 0);
        received += n;

        if (n == 0) {
            break;
        }

        if (n < 0) {
            free(buf);
            return CHTTP_ERR_SYS(errno);
        }
        _log(_clear1up "Receiving %zub", received);
    }
    _log(_clear1up "Received %zub", received);

    if(out_buf == NULL) {
        free(buf);
        return 0;
    }

    *out_buf = buf;
    return 0;
}
