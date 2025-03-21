#ifndef ROUTER_H
#define ROUTER_H

#include "HTTP_Request.h"
#include "HTTP_Response.h"

struct http_response server_file(const char *uri, char *document_root);
struct http_response route_request(struct http_request request);

#endif

