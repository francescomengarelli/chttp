#include <stdlib.h>

#include "chttp/chttp.h"
#include "chttp/client.h"

int main(void)
{
    chttp_client_hello();
    chttp_server_hello();
    return EXIT_SUCCESS;
}
