#include "net_utils.h"


void find_servers(char server_ips[][INET_ADDRSTRLEN], int *server_count) {
    int sock;
    struct sockaddr_in broadcast_addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds;
    struct timeval timeout;

    *server_count = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));

    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");
    broadcast_addr.sin_port = htons(BROADCAST_PORT);

    if (sendto(sock, NULL, 0, 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        perror("Broadcast message send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Broadcast message sent. Waiting for responses...\n");

    FD_ZERO(&read_fds);
    FD_SET(sock, &read_fds);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while (select(sock + 1, &read_fds, NULL, NULL, &timeout) > 0) {
        struct sockaddr_in server_addr;
        socklen_t addr_len = sizeof(server_addr);
        ssize_t received = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&server_addr, &addr_len);

        if (received < 0) {
            perror("Error receiving response");
            continue;
        }

        buffer[received] = '\0';
        printf("Received response: %s from %s\n", buffer, inet_ntoa(server_addr.sin_addr));

        strncpy(server_ips[*server_count], inet_ntoa(server_addr.sin_addr), INET_ADDRSTRLEN);
        (*server_count)++;
    }

    close(sock);
}

void get_connection(int *client_socket, const char *ip) {
    // Создаем сокет
    if ((*client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Настроим серверный адрес
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        close(*client_socket);
        exit(EXIT_FAILURE);
    }

    // Подключаемся к серверу
    if (connect(*client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(*client_socket);
        exit(EXIT_FAILURE);
    }
}

void receive_ptree(prefix_tree *main_tree, int server_socket) {
    struct ptree_word pword_buffer;
    ssize_t bytes_received;

    while ((bytes_received = recv(server_socket, (char*)&pword_buffer, sizeof(pword_buffer), 0)) > 0) {
        pword_buffer.col_words = ntohl(pword_buffer.col_words);
        prefix_tree_insert_word_with_col_words(main_tree, pword_buffer.word, pword_buffer.col_words);
    }

    if (bytes_received == -1) {
        perror("Receive failed");
    }
}