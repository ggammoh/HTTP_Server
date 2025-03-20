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
    
    // Store a copy of body if needed
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
    headers = end_of_first_line + 2;
    
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

    // Parse headers
    int init_size = 20;
    parsed_request.headers = malloc(init_size * sizeof(struct http_header));
    char *header_line = strtok(headers, "\r\n");
    while (header_line){
        char *colon = strchr(header_line, ':');
        if (colon) {
            *colon = '\0';
            char *key = strdup(header_line);
            char *value = strdup(colon + 1);
            while (*value == ' ') {
                value++;
            }
            parsed_request.headers[parsed_request.header_count].key = key;
            parsed_request.headers[parsed_request.header_count].value = value;
            parsed_request.header_count++;
        }
        header_line = strtok(NULL, "\r\n");
        if (parsed_request.header_count >= init_size){
            init_size += 10;
            struct http_header *new_headers = realloc(parsed_request.headers, init_size * sizeof(struct http_header));
            parsed_request.headers = new_headers;
        }
    }

    for (int i = 0; i < parsed_request.header_count; i++){
        if (strcmp(parsed_request.headers[i].key, "Connection") == 0 && strcmp(parsed_request.headers[i].value, "keep-alive") == 0){
            parsed_request.keep_alive = 1;
            printf("Found keep-alive connection\n");
            break;
        }
    }
    
    // Free the temporary copy
    free(request_copy);
    
    return parsed_request;
}


void free_http_request(struct http_request *request) {
    if (request) {
        free(request->URI);
        free(request->headers);
        free(request->body);
        request->URI = NULL;
        request->headers = NULL;
        request->body = NULL;
    }
}