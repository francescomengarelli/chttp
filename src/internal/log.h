#ifndef CHTTP_INTERNAL_LOG_H
#define CHTTP_INTERNAL_LOG_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define _clear1up "\r\033[A\033[2K"

#ifdef DEBUG
#define _log(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#define _loge(fmt, ...) fprintf(stderr, "error: " fmt "\n", ##__VA_ARGS__)
#define _logpe(fmt) perror(fmt)
#else
#define _log(fmt, ...) (void)0;
#define _loge(fmt, ...) (void)0;
#define _logpe(fmt) (void)0;
#endif

#endif // !CHTTP_INTERNAL_LOG_H
