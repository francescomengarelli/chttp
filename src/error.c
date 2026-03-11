#include "chttp/error.h"
#include <netdb.h>
#include <string.h>

#include "internal/log.h"

const char *chttp_strerror(chttp_error err) {
    if (CHTTP_ERR_IS_GAI(err)) {
        return gai_strerror(CHTTP_ERR_AS_GAI(err));
    } else if (CHTTP_ERR_IS_SYS(err)) {
        return strerror(CHTTP_ERR_AS_SYS(err));
    }
    else if (CHTTP_ERR_IS_LIB(err)) {
        switch (err) {
            case CHTTP_OK:
                return "Ok";
            case CHTTP_ERR_UNKNOWN:
                return "Unknown error";
            case CHTTP_ERR_URI_NOHOSTNAME:
                return "Uri has no hostname";
        }
    }
    return "Invalid error";
}
