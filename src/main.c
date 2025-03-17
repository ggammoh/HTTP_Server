#include "server.h"

#define PORT "8080"
#define BACKLOG 10

int main() {
    struct server_config config; 
    config.document_root = "public";
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    if (setup_server(&config) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return 1;
    }

    while (1) {
        // Accept client connection using our abstraction
        int client_fd = accept_client(&config, &their_addr, &addr_len);
        if (client_fd == -1) {
            fprintf(stderr, "Failed to accept client\n");
            continue;
        }

        printf("Connected to %s\n", inet_ntoa(((struct sockaddr_in *)&their_addr)->sin_addr));

        // Handle the client connection
        handle_connection(&config, client_fd);
        
        // Close client socket after handling
        close(client_fd);
    }

    // This will never execute in current form, but good practice
    stop_server(&config);
    return 0;
}