#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "HTTP_Request.h"

struct http_response {
    int status_code;      // e.g., 200
    char *status_message; // e.g., "OK"
    char *headers;        // e.g., "Content-Type: text/html\r\nContent-Length: 13"
    char *body;           // e.g., "Hello, world!"
};

struct http_response process_response(struct http_request request);
void free_http_response(struct http_response *resp);

#endif // HTTP_RESPONSE_H
