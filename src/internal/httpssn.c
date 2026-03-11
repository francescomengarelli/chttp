#include "internal/httpssn.h"
#include "internal/httpssn_impl.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "chttp/error.h"

#include "internal/utils.h"
#include "internal/io.h"
#include "internal/log.h"

bool parse_req_info(_httpssn *ssn) {
    bool has_encoding = false, has_length = false;
    char *encodingstr = strstr(ssn->headers, "transfer-encoding:");
    if (encodingstr) {
        char transfer_encoding_value[32];
        has_encoding = sscanf(encodingstr, "transfer-encoding: %31s", transfer_encoding_value) == 1;
    }

    char *content_len = strstr(ssn->headers, "content-length");
    if (content_len) {
        int content_length_value;
        has_length = sscanf(content_len, "content-length: %i", &content_length_value) == 1;

        if (has_length) {
            ssn->content_len = content_length_value;
        }
    }

    (void)has_encoding;
    return has_length;

}

#define MAX_B_PER_RECV 1024
int generate_response(_httpssn *ssn, char **out_res);

int _httpssn_create(int sockfd, _httpssn **out_ssn) {
    _httpssn *ssn = calloc(1, sizeof *ssn);
    if(!ssn) {
        return CHTTP_ERR_SYS(errno);
    }

    ssn->sockfd = sockfd;
    ssn->status = _HTTPSSN_NEED_HEADER;
    ssn->invalid_req = false;

    if (out_ssn != NULL) {
        *out_ssn = ssn;
        return 0;
    }
    else return 0;
}

int _httpssn_recv(_httpssn *ssn) {

    char **buf = ssn->status == _HTTPSSN_NEED_HEADER ? &ssn->headers : &ssn->body;
    size_t *buf_size = ssn->status == _HTTPSSN_NEED_HEADER ? &ssn->headers_size : &ssn->body_size;
    size_t *buf_used = ssn->status == _HTTPSSN_NEED_HEADER ? &ssn->headers_used : &ssn->body_used;
    size_t to_receive = ssn->status == _HTTPSSN_NEED_HEADER ? MAX_B_PER_RECV : _minzu(MAX_B_PER_RECV, ssn->content_len - ssn->body_used);

    if (ssn->status == _HTTPSSN_NEED_HEADER) {
        *buf_size += MAX_B_PER_RECV;
        *buf = realloc(*buf, *buf_size);
    }

    int n = recv(ssn->sockfd, (*buf) + (*buf_used), to_receive, 0);

    if (n == 0) {
        // FIX: should not return ok
        _log("client disconnected");
        return CHTTP_OK;
    }
    if (n < 0) {
        return CHTTP_ERR_SYS(n);
    }

    *buf_used += n;
    _log("received %zu bytes", *buf_used);

    switch (ssn->status) {
        case _HTTPSSN_NEED_HEADER: {

            char *headers_end = strstr(*buf, "\r\n\r\n");
            if(!headers_end) {
                break;
            }
            size_t headers_len = headers_end - ssn->headers + 2;

            // copy extra to body and clear headers buffer
            char *body = headers_end + 4;
            size_t body_len = strlen(body);
            ssn->body = calloc(1, body_len + 1);
            ssn->body_used = body_len + 1;
            ssn->body_size = body_len + 1;
            memcpy(ssn->body, body, body_len);
            memset(*buf + headers_len, 0, *buf_size - headers_len);

            _strtolower(ssn->headers, &ssn->headers);

            if(!parse_req_info(ssn)) {
                ssn->invalid_req = true;
                ssn->status = _HTTPSSN_DONE;
            }

            __attribute__((fallthrough));
        }
        case _HTTPSSN_DONE_HEADER: {
            bool end = ssn->content_len <= ssn->body_used;
            if(!end) {
                break;
            }
            ssn->status = _HTTPSSN_DONE_RECV;
            break;
        }
        default: {
            return CHTTP_ERR_UNKNOWN;
        }
    }

    return CHTTP_OK;
}


int _httpssn_send(_httpssn *ssn) {

    char *res = NULL;

    if(ssn->invalid_req) {
        res = "HTTP/1.1 400 Bad Request\r\n";
    } else {
        generate_response(ssn, &res);
    }
    chttp_error cherr;

    // FIX: this should not be _sendall. should be custom like _httpssn_recv or it might block the server
    cherr = _sendall(ssn->sockfd, res, strlen(res) + 1) ;

    if(!ssn->invalid_req) free(res);
    if(cherr != CHTTP_OK) return cherr;

    ssn->status = _HTTPSSN_DONE;
    return CHTTP_OK;
}

_httpssn_status _httpssn_status_get(_httpssn *ssn) {
    return ssn->status;
}

size_t _httpssn_sizeof(void) {
    return sizeof(_httpssn);
}
void _httpssn_destroy(_httpssn *ssn) {
    close(ssn->sockfd);
    free(ssn->body);
    free(ssn->headers);
    free(ssn);
}

int generate_response(_httpssn *ssn, char **out_res) {
    (void)ssn;
    char *temp =
        "HTTP/1.1 200 OK\r\n";
    char *res = calloc(1, strlen(temp) + 1);
    if(!res) return CHTTP_ERRNO;
    strncpy(res, temp, strlen(temp) + 1);

    *out_res = res;
    return CHTTP_OK;
}
