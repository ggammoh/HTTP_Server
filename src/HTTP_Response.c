#include "HTTP_Response.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

struct http_response process_response(struct http_request request) {
    struct http_response resp = { .status_code = 500, .status_message = NULL, .headers = NULL, .body = NULL };

if (request.method == GET) {
        char path[100];
        snprintf(path, sizeof(path), "public%s", request.URI);
        
        int fd = open(path, O_RDONLY);
        if (fd == -1) {
            resp.status_code = 404;
            resp.status_message = strdup("Not Found");
            resp.headers = strdup("Content-Length: 9");
            resp.body = strdup("Not Found!");
            return resp;
        }

        struct stat st;
        if (fstat(fd, &st) == -1) {
            resp.status_code = 500;
            resp.status_message = strdup("Internal Server Error");
            resp.headers = strdup("Content-Length: 6");
            resp.body = strdup("Error!");
            close(fd);
            return resp;
        }

        resp.body = malloc(st.st_size); // No +1 for binary
        if (!resp.body) {
            resp.status_code = 500;
            resp.status_message = strdup("Internal Server Error");
            resp.headers = strdup("Content-Length: 6");
            resp.body = strdup("Error!");
            close(fd);
            return resp;
        }
        ssize_t bytes_read = read(fd, resp.body, st.st_size);
        close(fd);

        resp.status_code = 200;
        resp.status_message = strdup("OK");
        char headers[100];
        const char *content_type = "text/html"; // Default
        if (strrchr(request.URI, '.') && strcmp(strrchr(request.URI, '.'), ".ico") == 0) {
            content_type = "image/x-icon";
        }
        snprintf(headers, sizeof(headers),
                 "Content-Type: %s\r\nContent-Length: %zu\r\n",
                 content_type, (size_t)st.st_size);
        resp.headers = strdup(headers);

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