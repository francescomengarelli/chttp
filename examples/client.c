#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chttp/chttp.h"
#include "chttp/error.h"

#define get(req, res)

// Run ex_server in the background, or this won't work!
// run: `./build/bin/release/ex_server &; ./build/bin/release/ex_client`
// then kill the server program by running `jobs` to find the server's PID
// and run `kill %PID`
//
#define BODY\
    "hello world!\r\n"

int main(void)
{
    char *body = "Hello, World!\r\n";
    char headers[512];
    snprintf(headers, sizeof headers, "POST / HTTP/1.1\r\nContent-Length: %lu\r\n", strlen(body));

    char req[1024];

    snprintf(req, sizeof req, "%s\r\n%s", headers, body);

    char *res;
    chttp_error result;
    printf("%s\n",req);
    if((result = chttp_post("localhost:8080", req, &res)) != CHTTP_OK) {
        printf("failed post req: %s\n", chttp_strerror(result));
        return EXIT_FAILURE;
    }
    printf("%s\n", res);
    free(res);

    return EXIT_SUCCESS;
}
