#ifndef CHTTP_MESSAGE_H
#define CHTTP_MESSAGE_H

typedef struct chttp_message chttp_message;

#include "chttp/error.h"

chttp_error chttp_message_create(chttp_message **msg);

chttp_error chttp_message_add_header(chttp_message *msg, const char *name, const char *value);
chttp_error chttp_message_get_header(chttp_message *msg, const char *name, char **out_value);

void chttp_message_destroy(chttp_message *msg);

#endif // !CHTTP_MESSAGE_H
