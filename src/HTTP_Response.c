#include "HTTP_Response.h"
#include "router.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

struct http_response process_response(struct http_request request) {
    struct http_response resp = { .status_code = 500, .status_message = NULL, .headers = NULL, .body = NULL };

    if (request.method == GET) {
        resp = server_file(request.URI);

    } else if (request.method == POST) {
        resp.status_code = 200;
        resp.status_message = strdup("OK");
        resp.headers = strdup("Content-Type: text/html\r\nContent-Length: 14");
        resp.body = strdup("POST received!");
    } else {
        resp.status_code = 500;
        resp.status_message = strdup("Internal Server Error");
        resp.headers = strdup("Content-Length: 6");
        resp.body = strdup("Error!");
    }

    if (!resp.status_message || !resp.headers || !resp.body) {
        free(resp.status_message);
        free(resp.headers);
        free(resp.body);
        resp.status_code = 500;
        resp.status_message = NULL;
        resp.headers = NULL;
        resp.body = NULL;
    }

    return resp;
}

void free_http_response(struct http_response *resp) {
    if (resp) {
        free(resp->status_message);
        free(resp->headers);
        free(resp->body);
        resp->status_message = NULL;
        resp->headers = NULL;
        resp->body = NULL;
    }
}