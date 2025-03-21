#include "server.h"
#include "HTTP_Request.h"
#include "HTTP_Response.h"
#include <sys/select.h>

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

void *worker_thread(void* args){
    struct worker_args *wargs = (struct worker_args *)args; // Cast void* to worker_args
    struct fd_queue *queue = wargs->queue;                // Get queue pointer
    struct server_config *config = wargs->config;         // Get config pointer
    while(1){
        int client_fd = dequeue(queue);
        printf("Thread %ld handling fd %d\n", pthread_self(), client_fd);
        int status = handle_connection(config, client_fd);
        if (status == -1) {
            printf("Thread %ld: Error handling fd %d\n", pthread_self(), client_fd);
        } else {
            printf("Thread %ld: Finished fd %d\n", pthread_self(), client_fd);
        }
    }
    
    pthread_mutex_unlock(&pool->status_mutex);
    return NULL;
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
            if (total_bytes == 0){
                printf("Empty Request\n");
                break;
            }
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
            // printf("DEBUG: Read entire request\n");
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
    int keep_alive = 0;
    do {
        fd_set readfds;
        struct timeval timeout = { .tv_sec = TIMEOUT_LENGTH, .tv_usec = 0 };
        FD_ZERO(&readfds);
        FD_SET(client_fd, &readfds);
        int ready = select(client_fd + 1, &readfds, NULL, NULL, &timeout);

        if (ready == -1) {
            perror("select");
            printf("Select error");
            close(client_fd); // Explicitly close socket on error
            return -1; // Return immediately after cleanup
        }
        if (ready == 0) {
            printf("Timeout: No data received in %d seconds\n", TIMEOUT_LENGTH);
            break; // Timeout, exit loop
        }

        // Read request
        char *request = read_request(client_fd);
        if (!request) {
            perror("read_request");
            printf("failed to read request\n");
            return -1;
        }
        // If the request was empty (Most likely a TCP FIN packet) close the connection.
        if (request[0] == '\0'){
            printf("Empty request\n");
            break;
        }


        // Parse the request
        struct http_request parsed_request = parse_request(request);

        if (parsed_request.method == INVALID ) {
            const char *bad = "HTTP/1.1 400 Bad Request\r\n\r\n";
            free(request);
            send(client_fd, bad, strlen(bad), 0);
        } else{
            struct http_response resp = process_response(parsed_request, config->document_root);
            
            // Send HTTP headers first
            char headers[1024];
            snprintf(headers, sizeof(headers),
                    "HTTP/1.1 %d %s\r\n%s%s\r\n",
                    resp.status_code, resp.status_message, resp.headers, parsed_request.keep_alive ? "Connection: keep-alive\r\n" : "");
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
            
            printf("Sent Response\n");
            // printf("DEBUG: Headers sent: %s\n", headers);
            keep_alive = parsed_request.keep_alive;
            free_http_response(&resp);
            free_http_request(&parsed_request);
            free(request);
        }
    } while (keep_alive != 0);
    
    close(client_fd);
    printf("Connection closed\n\n");
    return 0;
}

void enqueue(struct fd_queue *queue, int client_fd) {
    pthread_mutex_lock(&queue->mutex); // Lock the queue for exclusive access

    // Wait if the queue is full (count == 10)
    while (queue->count >= 10) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }

    // Move rear forward (circularly)
    queue->rear = (queue->rear + 1) % 10;
    // Add the client_fd to the queue
    queue->fds[queue->rear] = client_fd;
    // Increment the count of items
    queue->count++;

    // Signal that the queue is not empty (workers can dequeue)
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex); // Unlock the queue
}

int dequeue(struct fd_queue *queue) {
    pthread_mutex_lock(&queue->mutex); // Lock the queue

    // Wait if the queue is empty (count == 0)
    while (queue->count <= 0) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    // Get the client_fd from the front
    int client_fd = queue->fds[queue->front];
    // Move front forward (circularly)
    queue->front = (queue->front + 1) % 10;
    // Decrement the count of items
    queue->count--;

    // Signal that the queue is not full (main can enqueue)
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex); // Unlock the queue

    return client_fd;
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
