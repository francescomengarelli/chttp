#ifndef CHTTP_ERROR_H
#define CHTTP_ERROR_H
// IWYU pragma: private, include "chttp/chttp.h"

typedef int chttp_error;

#define CHTTP_OK                    0
#define CHTTP_ERR_UNKNOWN           1
#define CHTTP_ERR_URI_NOHOSTNAME    2

#define CHTTP_ERR_SYS(e)    (1000 + (e))
#define CHTTP_ERR_GAI(e)    (2000 - (e))

#define CHTTP_ERRNO         CHTTP_ERR_SYS(errno)

#define CHTTP_DEF_RANGE(n, m, M) ((n) > (m) && (n) < (M))
#define CHTTP_ERR_IS_LIB(e) (CHTTP_DEF_RANGE(e, 0, 1000) || (e) == CHTTP_OK)
#define CHTTP_ERR_IS_SYS(e) CHTTP_DEF_RANGE(e, 1000, 2000)
#define CHTTP_ERR_IS_GAI(e) CHTTP_DEF_RANGE(e, 2000, 3000)

#define CHTTP_ERR_AS_SYS(e) ((e) - 1000)
#define CHTTP_ERR_AS_GAI(e) (2000 - (e))

const char *chttp_strerror(chttp_error err);

#endif // !CHTTP_ERROR_H
