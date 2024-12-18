#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <time.h>
#include <process.h> // Threadler için

#define PORT 8080
#define BUFFER_SIZE 1024

// Client ile iletiþim fonksiyonu
void handle_client(void *socket_desc) {
    SOCKET client_socket = *(SOCKET *)socket_desc;
    char buffer[BUFFER_SIZE] = {0};

    // Client ile iletiþim
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Veriyi al
        int valread = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
            printf("Client disconnected\n");
            break;
        }
        printf("Received: %s\n", buffer);

        // Komutlarý iþle
        char response[BUFFER_SIZE] = {0};
        if (strcmp(buffer, "apple inventory") == 0) {
            snprintf(response, BUFFER_SIZE, "Apple inventory: 42\n");
        } else if (strcmp(buffer, "date") == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(response, BUFFER_SIZE, "Current date and time: %Y-%m-%d %H:%M:%S\n", t);
        } else {
            snprintf(response, BUFFER_SIZE, "Unknown command\n");
        }

        // Cevabý gönder
        send(client_socket, response, strlen(response), 0);
        printf("Sent: %s\n", response);
    }

    // Client soketini kapat
    closesocket(client_socket);
    _endthread();
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Winsock baþlat
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Soket oluþtur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Sunucu adresini tanýmla
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Soketi baðla
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Dinlemeye baþla
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    printf("Server listening on port %d\n", PORT);

    // Baðlantýlarý kabul et
    while (1) {
        SOCKET new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue;
        }
        printf("Connection established with a new client\n");

        // Yeni bir thread ile client'ý iþle
        _beginthread(handle_client, 0, (void *)&new_socket);
    }

    // Cleanup
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
