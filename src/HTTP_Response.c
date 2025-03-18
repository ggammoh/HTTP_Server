#include "HTTP_Response.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct http_response process_response(struct http_request request) {
    struct http_response resp = { .status_code = 500, .status_message = NULL, .headers = NULL, .body = NULL };

    if (request.method == GET) {
        // Simple GET response for now
        resp.status_code = 200;
        resp.status_message = strdup("OK");
        resp.headers = strdup("Content-Type: text/html\r\nContent-Length: 48\r\n");
        resp.body = strdup("<html><body><h1>Hello, world!</h1></body></html>");
    } else if (request.method == POST) {
        // Simple POST response (placeholder)
        resp.status_code = 200;
        resp.status_message = strdup("OK");
        resp.headers = strdup("Content-Type: text/html\r\nContent-Length: 14");
        resp.body = strdup("POST received!");
    } else {
        // Error case
        resp.status_code = 500;
        resp.status_message = strdup("Internal Server Error");
        resp.headers = strdup("Content-Length: 6");
        resp.body = strdup("Error!");
    }

    // Check allocations (simplified for clarity)
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