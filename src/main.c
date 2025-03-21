#include "server.h"


int main() {
    struct server_config config; 
    config.document_root = "public";
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    struct fd_queue main_queue;
    main_queue.fds = malloc(sizeof(int) * THREAD_POOL_SIZE);
    main_queue.front = 0;
    main_queue.rear = -1;
    main_queue.count = 0;
    pthread_mutex_init(&main_queue.mutex, NULL);
    pthread_cond_init(&main_queue.not_empty, NULL);
    pthread_cond_init(&main_queue.not_full, NULL);

    if (setup_server(&config) != 0) {
        fprintf(stderr, "Failed to initialize server\n");
        stop_server(&config);
        return 1;
    }

// Create worker threads
    pthread_t threads[THREAD_POOL_SIZE];
    struct worker_args args = { &main_queue, &config };
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        if (pthread_create(&threads[i], NULL, worker_thread, &args) != 0) {
            perror("pthread_create");
            stop_server(&config); // Assuming you have this
            free(main_queue.fds);
            return 1;
        }
    }

    // Main loop: Accept connections and enqueue them
    while (1) {
        int client_fd = accept_client(&config, &their_addr, &addr_len);
        if (client_fd == -1) {
            continue; // Skip on error, keep accepting
        }
        printf("\nConnected to %s\n", inet_ntoa(((struct sockaddr_in *)&their_addr)->sin_addr));
        enqueue(&main_queue, client_fd); // Add to queue for workers
    }

    // Cleanup (unreachable without signal handling, add later if needed)
    stop_server(&config);
    free(main_queue.fds);
    pthread_mutex_destroy(&main_queue.mutex);
    pthread_cond_destroy(&main_queue.not_empty);
    pthread_cond_destroy(&main_queue.not_full);
    return 0;
}