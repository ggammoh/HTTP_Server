#include "server.h"
#include "HTTP_Request.h"
#include "HTTP_Response.h"

/**
 * Sets up a TCP server according to the provided configuration
 * 
 * This function initializes a socket, binds it to the specified port,
 * and prepares it to accept incoming connections.
 * 
 * @param config Pointer to server configuration structure
 * @return 0 on success, -1 on failure
 */
int setup_server(struct server_config *config) {
    struct addrinfo hints;    // Structure to specify the criteria for selecting socket address structures
    int status;               // Return value for getaddrinfo function

    // Initialize hints structure with zeros to clear any existing data
    memset(&hints, 0, sizeof hints);
    // Set address family to unspecified (IPv4 or IPv6)
    hints.ai_family = AF_UNSPEC;
    // Set socket type to SOCK_STREAM for TCP connection
    hints.ai_socktype = SOCK_STREAM;
    // Set AI_PASSIVE flag to indicate the socket will be used for listening
    hints.ai_flags = AI_PASSIVE;

    // Get address information for the server, using NULL for local address and PORT constant
    if ((status = getaddrinfo(NULL, PORT, &hints, &config->server_info)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }

    // Create a socket using the retrieved address info
    config->socket_fd = socket(config->server_info->ai_family, config->server_info->ai_socktype, config->server_info->ai_protocol);
    if (config->socket_fd == -1) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        // Clean up allocated memory on error
        freeaddrinfo(config->server_info);
        return -1;
    }

    // Bind the socket to the specified address and port
    if (bind(config->socket_fd, config->server_info->ai_addr, config->server_info->ai_addrlen) == -1) {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        // Clean up resources on error
        close(config->socket_fd);
        freeaddrinfo(config->server_info);
        return -1;
    }

    // Start listening for incoming connections
    if (listen(config->socket_fd, BACKLOG) == -1) {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        // Clean up resources on error
        close(config->socket_fd);
        freeaddrinfo(config->server_info);
        return -1;
    }

    // Notify that the server is now listening
    printf("Server listening on port %s\n", PORT);

    // Return success
    return 0;
}

int accept_client(struct server_config *config, struct sockaddr_storage *client_addr, socklen_t *addr_len) {
    int client_fd = accept(config->socket_fd, (struct sockaddr *)client_addr, addr_len);
    if (client_fd == -1) {
        fprintf(stderr, "accept error: %s\n", strerror(errno));
        return -1;
    }
    return client_fd;
}

char *read_request(int client_fd) {
    int max_size = 16384;
    int initial_size = 2048;
    char *buf = malloc(initial_size);
    if (!buf) {
        perror("malloc");
        return NULL;
    }

    int count = 0;

    int numbytes;
    int total_bytes = 0;


    while (1) {
        numbytes = recv(client_fd, buf + total_bytes, initial_size - total_bytes, 0);
        if (numbytes == -1) {
            perror("recv");
            free(buf);
            return NULL;
        }
        if (numbytes == 0) {
            printf("No more data to read\n");
            break;
        }
        total_bytes += numbytes;
        if (total_bytes >= max_size) {
            fprintf(stderr, "Request too large\n");
            free(buf);
            return NULL;
        }
        if (total_bytes >= initial_size) {
            printf("Chunk %d\n", count);
            initial_size *= 2;
            printf("Reallocating buffer to %d bytes\n", initial_size);
            char *new_buf = realloc(buf, initial_size);
            if (!new_buf) {
                perror("realloc");
                free(buf);
                return NULL;
            }
            buf = new_buf;
            printf("Total bytes: %d\n", total_bytes);
        }
        else{
            break;
        }
        
        count++;
    }

    buf[total_bytes] = '\0';
    return buf;
}

/**
 * Handles a client connection by sending a simple HTML response
 * 
 * This function processes the client request and sends back a basic
 * HTML page response.
 * 
 * @param config Pointer to server configuration structure
 * @param client_fd Client socket file descriptor
 * @return 0 on success, -1 on failure
 */
int handle_connection(struct server_config *config, int client_fd) {
    char *request = read_request(client_fd);
    if (!request) {
        perror("read_request");
        return -1;
    }

    // Parse the request
    struct http_request parsed_request = parse_request(request);

    if (parsed_request.method == INVALID ) {
        const char *bad = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_fd, bad, strlen(bad), 0);
    } else{
        struct http_response resp = process_response(parsed_request);
        
        // Send HTTP headers first
        char headers[1024];
        snprintf(headers, sizeof(headers),
                 "HTTP/1.1 %d %s\r\n%s\r\n\r\n",
                 resp.status_code, resp.status_message, resp.headers);
        send(client_fd, headers, strlen(headers), 0);
        
        // Then send the body separately (handles binary data)
        if (resp.body) {
            // Get the file size from the content-length header
            const char *content_length = strstr(resp.headers, "Content-Length: ");
            if (content_length) {
                size_t body_size = (size_t)atoi(content_length + 16); // Skip "Content-Length: "
                ssize_t sent_bytes = send(client_fd, resp.body, body_size, 0);
                if (sent_bytes == -1) {
                    perror("send");
                    return -1;
                }
            }
        }
        
        printf("\nDEBUG: Headers sent: %s\n", headers);
        free_http_response(&resp);
        free_http_request(&parsed_request);
    }
    
    close(client_fd);
    printf("Connection closed\n");
    
    return 0;
}

/**
 * Stops the server by closing the socket and freeing the allocated server info
 * 
 * This function closes the server's socket and frees the memory allocated for
 * the server's address information. It returns 0 on success.
 * 
 * @param config Pointer to server configuration structure
 * @return 0 on success
 */
int stop_server(struct server_config *config) {
    close(config->socket_fd); // Close the socket
    freeaddrinfo(config->server_info); // Free the allocated server info
    return 0;
}
