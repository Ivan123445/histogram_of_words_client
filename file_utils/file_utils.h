#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#define MAX_FILENAME_LEN 256
#define FILENAME_PREFIX "/tmp/file_for_histogram"


void send_file(int client_socket, const char *filename, long start, long end);
void write_file_from_console(char *filename);
void write_file(char *input_filename, char *output_filename, long start, long end);
void print_file_parts(char *filename, long *file_parts, int num_threads);

#endif //FILE_UTILS_H
