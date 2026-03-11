#ifndef CHTTP_INTERNAL_IO_H
#define CHTTP_INTERNAL_IO_H

#include <stddef.h>

int _sendall(int sockfd, char *buf, size_t buf_size);

int _recvall(int sockfd, char **buf);

#endif // !CHTTP_INTERNAL_IO_H
