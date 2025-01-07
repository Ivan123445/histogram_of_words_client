#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>

#include "../prefix_tree/prefix_tree.h"

#define PORT 12346
#define SERVER_IP "127.0.0.1"
#define NUM_PCS 1

#define BROADCAST_PORT 9876
#define NET_BUFFER_SIZE 1024

struct __attribute__((packed)) ptree_word {
    char word[MAX_WORD_LENGTH];
    int col_words;
};


void find_servers(char server_ips[][INET_ADDRSTRLEN], int *server_count);
void get_connection(int *client_socket, const char *ip);
void receive_ptree(prefix_tree *main_tree, int server_socket);

#endif //NET_UTILS_H
