#include <stdio.h>
#include <stdlib.h>

#include "chttp/chttp.h"

#define get(req, res)

// Run ex_server in the background, or this won't work!
// run: `./build/bin/release/ex_server &; ./build/bin/release/ex_client`
// then kill the server program by running `jobs` to find the server's PID
// and run `kill %PID`
int main(void)
{
    char *msg = "ping";
    char *res;
    printf("sending: %s\n", msg);
    chttp_get(msg, CHTTP_LOCALHOST, "8080", &res);
    printf("received: %s\n", res);

    free(res);
    return EXIT_SUCCESS;
}
