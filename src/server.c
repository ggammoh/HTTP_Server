#include "server.h"
#include "HTTP_Request.h"

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
    // TODO: Implement
    char buf[2048];
    int numbytes;

    numbytes = recv(client_fd, buf, sizeof buf, 0);
    if (numbytes == -1) {
        perror("recv");
        return -1;
    }

    buf[numbytes] = '\0';
    printf("Received request: %s\n", buf);

    // Parse the request
    struct http_request parsed_request = parse_request(buf);

    if (parsed_request.method == INVALID ) {
        const char *bad = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_fd, bad, strlen(bad), 0);
    } else {
        char response[1024];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n<h1>Hello!</h1>");
        int bytes_sent = send(client_fd, response, strlen(response), 0);
        if (bytes_sent == -1) {
            perror("send");
            return -1;
        }
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
