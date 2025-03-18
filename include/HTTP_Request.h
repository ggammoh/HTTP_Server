#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

enum http_method {
    GET = 0,
    POST = 1,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    TRACE,
    CONNECT,
    PATCH,
    INVALID = -1
};

struct http_headers {
    char *name;
    char *value;
};

struct http_request {
    int method;
    char *URI;
    float version;
    char *headers;
    char *body;
};

int interpret_method(const char *method);
struct http_request parse_request(const char *request);
void free_http_request(struct http_request *request);


#endif // HTTP_REQUEST_H
