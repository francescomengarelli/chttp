#include "http_parser.h"

#include "log.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void _parse_http(const char *msg) {
    (void)msg;
    const char *curr = msg;

    while (strstr(curr, "\r\n") == curr) {
        curr += 4;
    }

    const char *start_line_start = curr;
    const char *start_line_end = strstr(curr, "\r\n");

    if(!start_line_end) {
        _log("no start line");
        return;
    }

    curr = start_line_end;

    size_t start_line_len = start_line_end - start_line_start;

    char *start_line = malloc(start_line_len + 1);
    start_line[start_line_len] = '\0';

    strcpy(start_line, start_line_start);

    _log("start line: %s", start_line);
}
