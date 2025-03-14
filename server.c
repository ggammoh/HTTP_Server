#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 8080

int main() {
    WSADATA wsa;
    int server_fd;

    // Start Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        printf("Socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("Socket created successfully!\n");

    // Cleanup for now (we’ll add more later)
    closesocket(server_fd);
    WSACleanup();
    return 0;
}