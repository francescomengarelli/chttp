#include <stdlib.h>

#include "internal/http_parser.h"

int main(void)
{

    char *msg =
        "POST /path/to/resource HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 27\r\n"
        "Connection: close\r\n"
        "\r\n"
        "field1=value1&field2=value2";
    _parse_http(msg);
    return EXIT_SUCCESS;
}
