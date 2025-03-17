#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PORT "8080"
#define BACKLOG 10

struct server_config {
    int socket_fd;
    struct addrinfo *server_info;
    char *document_root;
};

int setup_server(struct server_config *config);
int accept_client(struct server_config *config, struct sockaddr_storage *client_addr, socklen_t *addr_len);
int handle_connection(struct server_config *config, int client_fd);
int stop_server(struct server_config *config);

#endif // SERVER_H