#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "prefix_tree/prefix_tree.h"

#define NUM_THREADS 4


char * handle_args(int argc, char *argv[]);
char* get_file_name_from_console();
prefix_tree **handle_file_parts_parallel(char *filename, long *file_parts, size_t num_parts);


int main(const int argc, char *argv[]) {
    struct timespec start, end;
    double elapsed;

    char* fileName = "/home/ivan/CLionProjects/untitled/file.txt";
    // char* fileName = get_file_name_from_console();

    long* file_parts = split_file(fileName, NUM_THREADS);

    clock_gettime(CLOCK_MONOTONIC, &start);
    prefix_tree **prefix_trees = handle_file_parts_parallel(fileName, file_parts, NUM_THREADS);
    clock_gettime(CLOCK_MONOTONIC, &end);

    prefix_tree *main_ptree = prefix_tree_init();
    for (int i = 0; i < NUM_THREADS; i++) {
        prefix_tree_insert_tree(main_ptree, prefix_trees[i]);
    }

    elapsed = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time: %f\n", elapsed);
    printf("Found words:\n");
    // prefix_tree_print(main_ptree);

    // free(fileName);
    return 0;
}
// /home/ivan/CLionProjects/untitled/file.txt

prefix_tree **handle_file_parts_parallel(char *filename, long *file_parts, size_t num_parts) {
    pthread_t *threads = malloc(sizeof(pthread_t) * num_parts);
    prefix_tree **prefix_trees = malloc(sizeof(prefix_tree*) * num_parts);
    thread_args *thread_args_massive = malloc(sizeof(thread_args) * num_parts);

    for (int i = 0; i < num_parts; i++) {
        thread_args_massive[i].filename = filename;
        thread_args_massive[i].start = file_parts[i];
        thread_args_massive[i].end = file_parts[i+1];

        if (pthread_create(&threads[i], NULL, get_prefix_tree_by_text, &thread_args_massive[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_parts; i++) {
        if (pthread_join(threads[i], (void**)&prefix_trees[i]) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    free(threads);
    free(thread_args_massive);
    return prefix_trees;
}

char* handle_args(int argc, char *argv[]) {
    return argv[1];
}

char* get_file_name_from_console() {
    char* fileName = malloc(256*sizeof(char));
    printf("Enter file name: ");
    scanf("%s", fileName);
    return fileName;
}