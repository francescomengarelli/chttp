#ifndef CHTTP_HTTP_H
#define CHTTP_HTTP_H

#if !defined(CHTTP_SERVER) && !defined(CHTTP_CLIENT)
    #define CHTTP_SERVER
    #define CHTTP_CLIENT
#endif

#ifdef CHTTP_SERVER
#include "server.h" // IWYU pragma: export
#endif // CHTTP_SERVER

#ifdef CHTTP_CLIENT
#include "client.h" // IWYU pragma: export
#endif // CHTTP_CLIENT

#endif // !CHTTP_HTTP_H
