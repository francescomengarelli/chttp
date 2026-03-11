#include "internal/uri.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "internal/log.h"

chttp_error _uri_create(const char *str, _uri **out_uri) {

    _uri *uri = malloc(sizeof *uri);
    const char *uri_end = str + strlen(str);

    const char *scheme_start = str;
    const char *scheme_end = strstr(str, "://");
    bool has_scheme = scheme_end != NULL;


    const char *host_start = has_scheme ? scheme_end + 3 : str;
    const char *host_end = strstr(host_start, "/");
    bool has_path = host_end != NULL;
    if(!has_path) host_end = uri_end;
    const char *hostname_end = strstr(host_start, ":");
    bool has_port = hostname_end != NULL;
    if(!has_port) hostname_end = host_end;
    const char *port_start = has_port ? hostname_end + 1 : NULL;

    const char *path_start = has_path ? host_end + 1 : NULL;

    if (has_scheme) {
        (void)scheme_start;
        uri->scheme = _URI_SCHEME_HTTP;
    } else {
        uri->scheme = _URI_SCHEME_HTTP;
    }

    size_t hostname_len = hostname_end - host_start;

    uri->host = calloc(1, hostname_len + 1);
    memcpy(uri->host, host_start, hostname_len);

    for (size_t i = 0; i < hostname_len; i++) {
        uri->host[i] = tolower((unsigned char)uri->host[i]);
    }


    if (has_port) {
        size_t port_len = host_end - port_start;
        uri->port = calloc(1, port_len + 1);
        memcpy(uri->port, port_start, port_len);
    } else {
        char *default_port = "http";
        uri->port = calloc(1, strlen(default_port) + 1);
        memcpy(uri->port, default_port, strlen(default_port));
    }

    if (has_path) {
        size_t path_len = uri_end - path_start;
        if (path_len == 0) {
            path_len = 1;
            path_start = "/";
        }
        uri->path = calloc(1, path_len + 1);
        memcpy(uri->path, path_start, path_len);
    }
    else {
        char *default_path = "/";
        uri->path = calloc(1, strlen(default_path) + 1);
        memcpy(uri->path, default_path, strlen(default_path));
    }

    if (strlen(uri->host) == 0) {
        _uri_destroy(uri);
        return CHTTP_ERR_URI_NOHOSTNAME;
    }

    *out_uri = uri;
    return CHTTP_OK;
}

void _uri_destroy(_uri *uri) {
    free(uri->path);
    free(uri->port);
    free(uri->host);
    free(uri);
}
