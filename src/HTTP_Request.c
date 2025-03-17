#include "HTTP_Request.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int interpret_method(const char *method) {
    if (strcmp(method, "GET") == 0) {
        return GET;
    } else if (strcmp(method, "POST") == 0) {
        return POST;
    } else if (strcmp(method, "PUT") == 0) {
        return PUT;
    } else if (strcmp(method, "DELETE") == 0) {
        return DELETE;
    } else if (strcmp(method, "HEAD") == 0) {
        return HEAD;
    } else if (strcmp(method, "OPTIONS") == 0) {
        return OPTIONS;
    } else if (strcmp(method, "TRACE") == 0) {
        return TRACE;
    } else if (strcmp(method, "CONNECT") == 0) {
        return CONNECT;
    } else if (strcmp(method, "PATCH") == 0) {
        return PATCH;
    } else {
        return -1;
    }
    
}

struct http_request parse_request(const char *request) {
    struct http_request parsed_request = { .method = INVALID, .URI = NULL, .version = 0.0, .headers = NULL, .body = NULL };
    
    if (!request) return parsed_request;
    
    // Make a copy of the original request that we can modify
    char *request_copy = strdup(request);
    if (!request_copy) return parsed_request;
    
    // Find headers/body split (CRLF CRLF)
    char *headers_body_split = strstr(request_copy, "\r\n\r\n");
    if (!headers_body_split) {
        free(request_copy);
        return parsed_request; // Malformed: no headers
    }
    
    // Split headers and body
    *headers_body_split = '\0';  // Terminate headers at the split
    char *headers = request_copy;
    char *body = headers_body_split + 4;  // Move past "\r\n\r\n"
    
    // Store a copy of headers and body if needed
    parsed_request.headers = strdup(headers);
    if (strlen(body) > 0) {
        parsed_request.body = strdup(body);
    }
    
    // Find first line of request (request line)
    char *end_of_first_line = strstr(headers, "\r\n");
    if (!end_of_first_line) {
        free(request_copy);
        return parsed_request;
    }
    
    *end_of_first_line = '\0';  // Terminate first line
    char *request_line = headers;
    
    // Parse first line: METHOD URI HTTP/VERSION
    char *method_str = strtok(request_line, " ");
    char *uri_str = strtok(NULL, " ");
    char *version_str = strtok(NULL, " ");
    
    if (!method_str || !uri_str || !version_str) {
        free(request_copy);
        return parsed_request;
    }
    
    // Extract and store method
    parsed_request.method = interpret_method(method_str);
    
    // Store URI (make a copy)
    parsed_request.URI = strdup(uri_str);
    
    // Parse version (skip "HTTP/")
    if (strncmp(version_str, "HTTP/", 5) == 0) {
        parsed_request.version = atof(version_str + 5);
    }
    
    // Free the temporary copy
    free(request_copy);
    
    // Debug output with correct format specifiers
    printf("Method: %d\n", parsed_request.method);
    if (parsed_request.URI) {
        printf("URI: %s\n", parsed_request.URI);
    } else {
        printf("URI: (null)\n");
    }
    printf("Version: %f\n", parsed_request.version);
    printf("Headers: %s\n", parsed_request.headers);
    printf("Body: %s\n", parsed_request.body);
    
    return parsed_request;
}
