#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "net_utils/net_utils.h"
#include "file_utils/file_utils.h"
#include "prefix_tree/prefix_tree.h"

#define MAX_FILENAME_LEN 256
#define MAX_PCS 10

struct handle_file_args {
    char *filename;
    char *server_ip;
    long file_start;
    long file_end;
};

void *handle_file_part(void *args) {
    prefix_tree *result_tree = prefix_tree_init();
    struct handle_file_args *file_args = args;
    int server_socket;
    get_connection(&server_socket, file_args->server_ip);
    send_file(server_socket, file_args->filename, file_args->file_start, file_args->file_end);
    receive_ptree(result_tree, server_socket);
    close(server_socket);
    return result_tree;
}

prefix_tree *handle_file_parts_parallel(char *filename, long *file_parts, size_t num_pcs, char server_ips[MAX_PCS][INET_ADDRSTRLEN]) {
    pthread_t *threads = malloc(sizeof(pthread_t) * num_pcs);
    prefix_tree **prefix_trees = malloc(sizeof(prefix_tree*) * num_pcs);
    struct handle_file_args *args = malloc(sizeof(struct handle_file_args) * num_pcs);

    for (int i = 0; i < num_pcs; i++) {
        args->filename = filename;
        args->server_ip = server_ips[i];
        args->file_start = file_parts[i];
        args->file_end = file_parts[i+1];

        if (pthread_create(&threads[i], NULL, handle_file_part, &args[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_pcs; i++) {
        if (pthread_join(threads[i], (void**)&prefix_trees[i]) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    prefix_tree *main_ptree = prefix_tree_init();
    for (int i = 0; i < num_pcs; i++) {
        prefix_tree_insert_tree(main_ptree, prefix_trees[i]);
    }

    free(threads);
    free(args);
    free(prefix_trees);
    return main_ptree;
}

void get_filename_from_console(char *filename) {
    printf("Enter filename:");
    scanf("%s", filename);
}

void get_filename_from_arguments(const int argc, char *argv[], char *filename) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: --file <filename>\n");
        exit(0);
    }
    if (strcmp(argv[1], "--file") != 0) {
        printf("Wrong flag\n");
        printf("For more info use --help\n");
        exit(EXIT_FAILURE);
    }
    if (argc != 3) {
        printf("Wrong number of arguments\n");
        printf("For more info use --help\n");
        exit(EXIT_FAILURE);
    }

    strcpy(filename, argv[2]);
}

int main(const int argc, char *argv[]) {
    char fileName[MAX_FILENAME_LEN];
    argc > 1 ? get_filename_from_arguments(argc, argv, fileName) : get_filename_from_console(fileName);

    char server_ips[MAX_PCS][INET_ADDRSTRLEN];
    int server_count;
    find_servers(server_ips, &server_count);

    long *file_parts = split_file(fileName, server_count);

    prefix_tree *result_tree = handle_file_parts_parallel(fileName, file_parts, server_count, server_ips);
    printf("Result tree:\n");
    prefix_tree_print(result_tree);

    return 0;
}
