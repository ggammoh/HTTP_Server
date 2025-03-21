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
#include <pthread.h>

#define PORT "8080"
#define BACKLOG 10
#define THREAD_POOL_SIZE 6
#define TIMEOUT_LENGTH 5

struct server_config {
    int socket_fd;
    struct addrinfo *server_info;
    char *document_root;

};

struct fd_queue {
    int* fds;
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty, not_full;
};

struct worker_args {
    struct fd_queue *queue;
    struct server_config *config;
};

int setup_server(struct server_config *config);
int accept_client(struct server_config *config, struct sockaddr_storage *client_addr, socklen_t *addr_len);
int handle_connection(struct server_config *config, int client_fd);

void enqueue(struct fd_queue *queue, int client_fd);
int dequeue(struct fd_queue *queue);

void *worker_thread(void* args);

int stop_server(struct server_config *config);

#endif // SERVER_H