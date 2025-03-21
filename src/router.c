#include "router.h"
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

const char* get_file_extension(const char* path) {
    if (path == NULL) {
        return "";
    }
    if (strcmp(path, "/") == 0) {
        return "html";
    }
    
    // Skip any trailing slash
    size_t path_len = strlen(path);
    if (path_len > 0 && path[path_len - 1] == '/') {
        return "html";  // Directories are treated as HTML
    }
    
    const char* dot = strrchr(path, '.');
    const char* slash = strrchr(path, '/');
    
    // If there's no dot, or the last dot appears before the last slash,
    // then there's no extension
    if (dot == NULL || (slash != NULL && dot < slash)) {
        return "";
    }
    
    return dot + 1;
}

const char* get_content_type(const char *extension) {
    if (extension == NULL) {
        return "application/octet-stream";
    }
    if (extension[0] == '\0') {
        // If no extension is found, default to HTML
        return "text/html";
    }
    if (strcmp(extension, "html") == 0) {
        return "text/html";
    } else if (strcmp(extension, "css") == 0) {
        return "text/css";
    } else if (strcmp(extension, "js") == 0) {
        return "application/javascript";
    } else if (strcmp(extension, "png") == 0) {
        return "image/png";
    } else if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(extension, "ico") == 0) {
        return "image/x-icon";
    }
    return "application/octet-stream";
}
struct http_response server_file(const char *uri, char *document_root) {
    struct http_response resp = { .status_code = 500, .status_message = NULL, .headers = NULL, .body = NULL };

    char path[100];
    const char* extension = get_file_extension(uri);
    // printf("DEBUG uri: %s", uri);
    // printf("\nDEBUG root: %s", document_root);
    if (strcmp(uri, "/") == 0) {
        snprintf(path, sizeof(path), "%s/index.html", document_root);
    } else {
        if (extension[0] == '\0') {
            // No extension, try to serve as HTML
            snprintf(path, sizeof(path), "%s%s.html", document_root, uri);
        } else {
            // Has extension, use as is
            snprintf(path, sizeof(path), "%s%s", document_root, uri);
        }
    }

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        resp.status_code = 404;
        resp.status_message = strdup("File Not Found");
        resp.headers = strdup("Content-Length: 15");
        resp.body = strdup("File Not Found!");
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

    resp.body = malloc(st.st_size);
    if (!resp.body) {
            resp.status_code = 500;
            resp.status_message = strdup("Internal Server Error");
            resp.headers = strdup("Content-Length: 6");
            resp.body = strdup("Error!");
            close(fd);
            return resp;
    }
    ssize_t bytes_read = read(fd, resp.body, st.st_size);
    if (bytes_read != st.st_size) {
        resp.status_code = 500;
        resp.status_message = strdup("Internal Server Error");
        resp.headers = strdup("Content-Length: 6");
        free(resp.body);
        resp.body = strdup("Error!");
        close(fd);
        return resp;
    }
    close(fd);

    resp.status_code = 200;
    resp.status_message = strdup("OK");

    const char* content_type = get_content_type(extension);
    char headers[100];
    snprintf(headers, sizeof(headers),
             "Content-Type: %s\r\nContent-Length: %zu\r\n", content_type, (size_t)st.st_size);
    resp.headers = strdup(headers);

    return resp;
}